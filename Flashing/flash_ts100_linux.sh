#!/bin/bash
# TS100 Flasher for Linux by Alex Wigen (https://github.com/awigen)

DIR_TMP="/tmp/ts100"

function usage() {
    echo
    echo "#################"
    echo "# TS100 Flasher #"
    echo "#################"
    echo
    echo " Usage: $0 <HEXFILE>"
    echo
    echo "This script has been tested to work on Fedora."
    echo "If you experience any issues please open a ticket at:"
    echo "https://github.com/Ralim/ts100/issues/new"
    echo
}

GAUTOMOUNT=0
function disable_gautomount {
    GSETTINGS=`which gsettings`
    if [ $? -ne 0 ]; then
        return 1
    fi
    gsettings get org.gnome.desktop.media-handling automount | grep true > /dev/null
    if [ $? -eq 0 ]; then
        GAUTOMOUNT=1
        gsettings set org.gnome.desktop.media-handling automount false
    fi
}

function enable_gautomount {
    if [ "$GAUTOMOUNT" -ne 0 ]; then
        gsettings set org.gnome.desktop.media-handling automount true
    fi
}

function is_attached {
    output=`lsblk -b --raw --output NAME,MODEL | grep 'DFU.*Disk'`
    if [ $? -ne 0 ]; then
      return 1
    fi
    DEVICE=`echo $output | awk '{print "/dev/"$1}'`
}

instructions="not printed"
function wait_for_ts100 {
    is_attached
    while [ $? -ne 0 ]; do
        if [ "$instructions" = "not printed" ]; then
            echo
            echo "#####################################################"
            echo "#   Waiting for TS100 config disk device to appear  #"
            echo "#                                                   #"
            echo "# Connect the soldering iron with a USB cable while #"
            echo "# holding the button closest to the tip pressed     #"
            echo "#####################################################"
            echo
            instructions="printed"
        fi
        sleep 0.1
        is_attached
    done
}

function mount_ts100 {
    mkdir -p "$DIR_TMP"
    sudo mount -t msdos -o uid=$UID "$DEVICE" "$DIR_TMP"
    if [ $? -ne 0 ]; then
        echo "Failed to mount $DEVICE on $DIR_TMP"
        exit 1
    fi
}

function umount_ts100 {
    mountpoint "$DIR_TMP" > /dev/null && sudo umount "$DIR_TMP"
    if [ $? -ne 0 ]; then
        echo "Failed to unmount $DIR_TMP"
        exit 1
    fi
    rmdir "$DIR_TMP"
}

function cleanup {
    enable_gautomount
    if [ -d "$DIR_TMP" ]; then
        umount_ts100
    fi
}
trap cleanup EXIT

if [ "$#" -ne 1 ]; then
    echo "Please provide a HEX file to flash"
    usage
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "'$1' is not a regular file, please provide a HEX file to flash"
    usage
    exit 1
fi

if [ `head -c1 "$1"` != ":" ] || [ `tail -n1 "$1" | head -c1` != ":" ]; then
    echo "'$1' doesn't look like a valid HEX file. Please provide a HEX file to flash"
    usage
    exit 1
fi

disable_gautomount

wait_for_ts100
echo "Found TS100 config disk device on $DEVICE"

mount_ts100
echo "Mounted config disk drive, flashing..."
cp -v "$1" "$DIR_TMP/ts100.hex"
sync

echo "Waiting for TS100 to flash"
sleep 5

echo "Remounting config disk drive"
umount_ts100
wait_for_ts100
mount_ts100

echo "Flash result: "
ls "$DIR_TMP"/ts100*
