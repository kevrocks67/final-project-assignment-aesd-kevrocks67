#!/bin/sh
# This script is executed by the swupdate daemon after files are moved.

# Update the App-specific version file
echo "$SWUPDATE_VERSION" > /data/app-version

# Restart the daemon so the new binary actually starts running
if [ -f "/etc/init.d/S99door_security_daemon" ]; then
    /etc/init.d/S99door_security_daemon restart
fi

exit 0
