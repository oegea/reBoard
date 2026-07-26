#include "PtySession.h"

#include <cstdlib>

#include <pty.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace reboard::terminal {

PtySession::PtySession(QObject* parent) : QObject(parent) {}

PtySession::~PtySession() {
    if (childPid_ > 0) {
        ::kill(childPid_, SIGHUP);
        ::waitpid(childPid_, nullptr, WNOHANG);
    }
    if (masterFd_ >= 0) {
        ::close(masterFd_);
    }
}

bool PtySession::start(int columns, int rows) {
    winsize size{};
    size.ws_col = static_cast<unsigned short>(columns);
    size.ws_row = static_cast<unsigned short>(rows);

    const pid_t pid = ::forkpty(&masterFd_, nullptr, nullptr, &size);
    if (pid < 0) {
        return false;
    }
    if (pid == 0) {
        ::setenv("TERM", "xterm", 1);
        const char* shell = ::getenv("REBOARD_TERMINAL_SHELL");
        if (shell == nullptr || shell[0] == '\0') {
            shell = "/bin/bash";
        }
        ::execlp(shell, shell, "-l", static_cast<char*>(nullptr));
        ::execlp("/bin/sh", "/bin/sh", "-l", static_cast<char*>(nullptr));
        ::_exit(127);
    }

    childPid_ = pid;
    notifier_ = new QSocketNotifier(masterFd_, QSocketNotifier::Read, this);
    connect(notifier_, &QSocketNotifier::activated, this, [this] { onReadable(); });
    return true;
}

void PtySession::onReadable() {
    char buffer[4096];
    const ssize_t bytesRead = ::read(masterFd_, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        notifier_->setEnabled(false);
        ::waitpid(childPid_, nullptr, WNOHANG);
        childPid_ = -1;
        emit finished();
        return;
    }
    emit outputReceived(QByteArray(buffer, static_cast<int>(bytesRead)));
}

void PtySession::write(const QByteArray& bytes) {
    if (masterFd_ >= 0 && !bytes.isEmpty()) {
        ::write(masterFd_, bytes.constData(), static_cast<std::size_t>(bytes.size()));
    }
}

void PtySession::resize(int columns, int rows) {
    if (masterFd_ < 0) {
        return;
    }
    winsize size{};
    size.ws_col = static_cast<unsigned short>(columns);
    size.ws_row = static_cast<unsigned short>(rows);
    ::ioctl(masterFd_, TIOCSWINSZ, &size);
}

}  // namespace reboard::terminal
