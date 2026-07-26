#include "infrastructure/repositories/FileApplicationRepository.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace reboard::infrastructure {

namespace fs = std::filesystem;

namespace {

bool caseInsensitiveLess(const std::string& left, const std::string& right) {
    return std::lexicographical_compare(
        left.begin(), left.end(), right.begin(), right.end(), [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) <
                   std::tolower(static_cast<unsigned char>(b));
        });
}

}  // namespace

FileApplicationRepository::FileApplicationRepository(std::vector<std::string> manifestDirectories)
    : manifestDirectories_(std::move(manifestDirectories)) {}

std::vector<domain::Application> FileApplicationRepository::findAll() const {
    std::vector<domain::Application> applications;

    for (const auto& directory : manifestDirectories_) {
        std::error_code errorCode;
        if (!fs::is_directory(directory, errorCode)) {
            continue;
        }
        for (const auto& entry : fs::directory_iterator(directory, errorCode)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".app") {
                continue;
            }
            std::ifstream file(entry.path());
            if (!file) {
                std::cerr << "reboard: cannot read manifest " << entry.path() << std::endl;
                continue;
            }
            std::ostringstream content;
            content << file.rdbuf();

            std::string error;
            const auto application =
                parser_.parse(entry.path().stem().string(), content.str(), &error);
            if (!application) {
                std::cerr << "reboard: skipping invalid manifest " << entry.path() << ": " << error
                          << std::endl;
                continue;
            }
            applications.push_back(*application);
        }
    }

    std::sort(applications.begin(), applications.end(),
              [](const domain::Application& left, const domain::Application& right) {
                  return caseInsensitiveLess(left.name().value(), right.name().value());
              });
    return applications;
}

}  // namespace reboard::infrastructure
