#!/bin/bash

########################################################################
# Package the binaries built in CI as an AppImage
# By Simon Peter 2016
# For more information, see http://appimage.org/
########################################################################


if [ $ARCH = '64' ]; then
  export ARCH='x86_64';
elif [ $ARCH = '32' ]; then
  export ARCH='i386';
fi

APP=SuperTux
LOWERAPP=supertux2

GIT_REV=$(git rev-parse --short HEAD)
echo $GIT_REV

RELEASE_VERSION=$(git describe --tags)

make install DESTDIR=$HOME/$APP/$APP.AppDir

cd $HOME/$APP/

wget -q https://github.com/probonopd/AppImages/raw/master/functions.sh -O ./functions.sh
#Remove line that should not be in functions.sh
sed -i -r 's/set -x//' functions.sh
#Silence wget
sed -i 's/wget/wget -q/' functions.sh
. ./functions.sh

cd $APP.AppDir

########################################################################
# Copy desktop and icon file to AppDir for AppRun to pick them up
########################################################################

get_apprun
get_desktop

# SVG icons are not supported by get_icon, copy it over manually.
cp ./usr/share/icons/hicolor/scalable/apps/$LOWERAPP.svg . || true
ls -lh $LOWERAPP.svg || true

########################################################################
# Copy in the dependencies that cannot be assumed to be available
# on all target systems
########################################################################

copy_deps

if [ -d "./usr/lib/x86_64-linux-gnu/gstreamer-1.0/" ] ; then
  mv -v ./usr/lib/x86_64-linux-gnu/gstreamer-1.0/* ./usr/lib/x86_64-linux-gnu/
  rm -vr ./usr/lib/x86_64-linux-gnu/gstreamer-1.0
fi

if [ -d "./usr/lib/x86_64-linux-gnu/pulseaudio/" ] ; then
  mv -v ./usr/lib/x86_64-linux-gnu/pulseaudio/* ./usr/lib/x86_64-linux-gnu/
  rm -vr ./usr/lib/x86_64-linux-gnu/pulseaudio
fi

########################################################################
# Delete stuff that should not go into the AppImage
########################################################################

# Delete dangerous libraries; see
# https://github.com/probonopd/AppImages/blob/master/excludelist
#delete_blacklisted # We'll need to specify our own blacklist, see below.

# Fix the function ourselves for now
# Delete blacklisted files
delete_blacklisted_patched()
{
  BLACKLISTED_FILES=$( cat_file_from_url https://github.com/probonopd/AppImages/raw/master/excludelist | sed '/^\s*$/d' | sed '/^#.*$/d' | sed '/libkrb5.so.26/d' | sed '/libkrb5.so.3/d' | sed '/libhcrypto.so.4/d' | sed '/libhx509.so.5/d' | sed '/libroken.so.18/d' | sed '/libwind.so.0/d')
  echo $BLACKLISTED_FILES
  for FILE in $BLACKLISTED_FILES ; do
    FOUND=$(find . -xtype f -name "${FILE}" 2>/dev/null)
    if [ ! -z "$FOUND" ] ; then
      echo "Deleting blacklisted ${FOUND}"
      rm -f "${FOUND}"
    fi
  done

  # Do not bundle developer stuff
  rm -rf usr/include || true
  rm -rf usr/lib/cmake || true
  rm -rf usr/lib/pkgconfig || true
  find . -name '*.la' | xargs -i rm {}
}

delete_blacklisted_patched

########################################################################
# desktopintegration asks the user on first run to install a menu item
########################################################################

get_desktopintegration $LOWERAPP

########################################################################
# Determine the version of the app; also include needed glibc version
########################################################################

VERSION=${RELEASE_VERSION}
export VERSION

########################################################################
# Patch away absolute paths
########################################################################

patch_usr

########################################################################
# AppDir complete
# Now packaging it as an AppImage
########################################################################

cd .. # Go out of AppImage

mkdir -p ../out/
generate_type2_appimage

