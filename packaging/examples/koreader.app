# Example reBoard application manifest.
#
# Copy files like this one (with the .app extension) to one of:
#   /etc/reboard/apps
#   /opt/etc/reboard/apps
#   ~/.config/reboard/apps
#
# The file name (without the extension) becomes the application id.

# Human-readable name shown on the board (required).
name=KOReader

# Command line to execute (use `unit=<systemd unit>` instead for services).
# Exactly one of `exec` or `unit` is required. Quote arguments with spaces.
exec=/opt/koreader/koreader.sh

# Optional icon shown on the board.
#icon=/opt/etc/reboard/icons/koreader.png

# Optional: pin this application to the fixed dock at the bottom (default: false).
dock=false
