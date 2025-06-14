#!/bin/sh

# POOP

LIMINE_VERSION="9.x"
LIMINE_RELEASE_URL="https://github.com/limine-bootloader/limine.git"
DOWNLOAD_DIR_TEMP="limine_download_temp"
TARGET_DIR_FROM_ROOT="limine_files"

NEEDED_FILES=(
    "limine-bios.sys"
    "limine-bios-cd.bin"
)

echon() {
    echo "---- $1 ----"
}

echosub() {
    echo "    --> $1"
}

echoerr() {
    echo "ERROR: $1" >&2
}

echinfo() {
    echo "INFO: $1"
}

echon "Starting Limine Bootloader Setup"
echosub "Target Limine version: v$LIMINE_VERSION"

if [ ! -d "$TARGET_DIR_FROM_ROOT"]; then
    echosub "Creating target directory: $TARGET_DIR_FROM_ROOT"
    mkdir -p "$TARGET_DIR_FROM_ROOT"
fi

echosub "Downloading Limine binaries from: $LIMINE_RELEASE_URL"

if git clone https://github.com/limine-bootloader/limine.git --branch=v9.x-binary --depth=1 $DOWNLOAD_DIR_TEMP; then
    echosub "Download successful: $DOWNLOAD_DIR_TEMP"
else
    echoerr "Failed to download Limine."
    rm -rf $DOWNLOAD_DIR_TEMP
    exit 1
fi

for file_to_move in "${NEEDED_FILES[@]}"; do
    cp "$DOWNLOAD_DIR_TEMP/$file_to_move" "$TARGET_DIR_FROM_ROOT/"
done

echosub "Cleaning up temporary download files..."
rm -rf $DOWNLOAD_DIR_TEMP
