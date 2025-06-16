#!/bin/sh

ST_VERSION=$(git describe --tags --always | sed 's/^v//')
DISTDIR="supertux-$ST_VERSION"

if test -e "$DISTDIR"
then
	echo "$DISTDIR already exists."
	exit 1
fi

echo "Creating directory $DISTDIR"
mkdir "$DISTDIR" || exit 1

cp "CMakeLists.txt" "LICENSE.txt" "INSTALL.md" "README.md" "NEWS.md" "config.h.cmake" "makedist.sh" "makepot.sh" "org.supertuxproject.SuperTux.metainfo.xml" "supertux2.desktop" "version.cmake" "version.cmake.in" "version.h.in" $DISTDIR
cp --parents mk/cmake/*.cmake $DISTDIR
cp --parents mk/msvc/* $DISTDIR

echo "Copying files:"
for DIR in contrib data docs man src tools external tests
do
	echo -n "  $DIR ... "
	find "$DIR" -type f -exec "cp" "--parents" "{}" "$DISTDIR" ";" -o -name .svn -prune
	echo "done"
done

echo -n "Creating $DISTDIR.tar.gz ... "
tar czf $DISTDIR.tar.gz $DISTDIR
echo "done"

echo -n "Creating $DISTDIR.tar.bz2 ... "
tar cjf $DISTDIR.tar.bz2 $DISTDIR
echo "done"

echo -n "Removing $DISTDIR ... "
rm -rf $DISTDIR
echo "done"
