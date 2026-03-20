#!/bin/sh
# This script is executed by the swupdate daemon after files are moved.

PHASE="$1"

case "$PHASE" in
    preinst)
        echo "Phase: preinst. Ensuring daemon is stopped..."
        if [ -f "/etc/init.d/S99door_security_daemon" ]; then
            /etc/init.d/S99door_security_daemon stop
        fi
        sleep 1
        ;;
    postinst)
        echo "Phase: postinst. Installation successful, starting service..."
        # Restart the daemon so the new binary actually starts running
        if [ -f "/etc/init.d/S99door_security_daemon" ]; then
            /etc/init.d/S99door_security_daemon restart
        fi
        ;;
    postfailure)
        echo "Phase: postfailure. Update failed, ensuring daemon is running..."
        if [ -f "/etc/init.d/S99door_security_daemon" ]; then
            /etc/init.d/S99door_security_daemon start
        fi
        ;;
esac

exit 0
