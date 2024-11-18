#!/bin/bash
# TS100 Flasher for Linux by Alex Wigen (https://github.com/awigen)
# Jan 2021 - Update by Ysard (https://github.com/ysard)

DIR_TMP="/tmp/ts100"
HEX_FIRMWARE="$DIR_TMP/ts100.hex"

usage() {
    echo
    echo "#################"
    echo "# TS100 Flasher #"
    echo "#################"
    echo
    echo " Usage: $0 <HEXFILE>"
    echo
    echo "This script has been tested to work on Fedora."
    echo "If you experience any issues please open a ticket at:"
    echo "https://github.com/Ralim/IronOS/issues/new"
    echo
}

GAUTOMOUNT=0
disable_gautomount() {
    if ! GSETTINGS=$(which gsettings); then
        return 1
    fi
    if ! gsettings get org.gnome.desktop.media-handling automount | grep true > /dev/null; then
        GAUTOMOUNT=1
        gsettings set org.gnome.desktop.media-handling automount false
    fi
}

enable_gautomount() {
    if [ "$GAUTOMOUNT" -ne 0 ]; then
        gsettings set org.gnome.desktop.media-handling automount true
    fi
}

is_attached() {
    if ! output=$(lsblk -b --raw --output NAME,MODEL | grep 'DFU.*Disk'); then
      return 1
    fi
    DEVICE=$(echo "$output" | awk '{print "/dev/"$1}')
}

instructions="not printed"
wait_for_ts100() {
    while ! is_attached; do
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
    done
}

mount_ts100() {
    mkdir -p "$DIR_TMP"
    user="${UID:-$(id -u)}"
    if ! sudo mount -t msdos -o uid=$user "$DEVICE" "$DIR_TMP"; then
        echo "Failed to mount $DEVICE on $DIR_TMP"
        exit 1
    fi
}

umount_ts100() {
    if ! (mountpoint "$DIR_TMP" > /dev/null && sudo umount "$DIR_TMP"); then
        echo "Failed to unmount $DIR_TMP"
        exit 1
    fi
    sudo rmdir "$DIR_TMP"
}

check_flash() {
    RDY_FIRMWARE="${HEX_FIRMWARE%.*}.rdy"
    ERR_FIRMWARE="${HEX_FIRMWARE%.*}.err"
    if [ -f "$RDY_FIRMWARE" ]; then
        echo -e "\e[92mFlash is done\e[0m"
        echo "Disconnect the USB and power up the iron. You're good to go."
    elif [ -f "$ERR_FIRMWARE" ]; then
        echo -e "\e[91mFlash error; Please retry!\e[0m"
    else
        echo -e "\e[91mUNKNOWN error\e[0m"
        echo "Flash result: "
        ls "$DIR_TMP"/ts100*
    fi
}

cleanup() {
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

if [ "$(head -c1 "$1")" != ":" ] || [ "$(tail -n1 "$1" | head -c1)" != ":" ]; then
    echo "'$1' doesn't look like a valid HEX file. Please provide a HEX file to flash"
    usage
    exit 1
fi

disable_gautomount

wait_for_ts100
echo "Found TS100 config disk device on $DEVICE"

mount_ts100
echo "Mounted config disk drive, flashing..."
dd if="$1" of="$HEX_FIRMWARE" oflag=direct
umount_ts100

echo "Waiting for TS100 to flash"
sleep 5

echo "Remounting config disk drive"
wait_for_ts100
mount_ts100
check_flash

