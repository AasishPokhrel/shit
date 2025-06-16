Install instructions for SuperTux - <https://supertux.org/>
====================================================================
Last update: August 28, 2024

Quick links:
- [Binaries](#binaries)
  - [Releases](#releases)
  - [Nightlies](#nightlies)
- [Compiling](#compiling)
  - [Requirements](#requirements)
  - [Unix and Unix-like (Linux/MacOS/\*BSD)](#linuxunix-using-cmake)
  - [Windows](#windows-using-cmake-and-visual-studio)
  - [Android](#android-using-sdl2)
  - [Browser (WASM)](#wasm-using-emscripten)
  - [Ubuntu Touch](#ubuntu-touch-using-clickable)

Binaries
--------

### Releases

We try to provide precompiled binaries of SuperTux for a number of
platforms. You should check <https://supertux.org/download.html>
for the packages and instructions on how to install them. If there are
no prebuilt binaries for your platform, then you might still be able
to compile the source code yourself. In this case read the next
sections.

### Nightlies

We also provide binaries automatically generated as code is added to the
repository. These are built using GitHub Actions and are generally used to
validate code quality, but as they produce binaries, those can be downloaded
and installed like any release. You may find the nightlies at
<https://download.supertux.org/>.

Alternatively, if you have a GitHub account, you may find nightlies directly on
GitHub either
[here](https://github.com/SuperTux/supertux/actions/workflows/main.yml?query=branch%3Amaster)
for common platforms (Windows, Mac, Linux) or
[here](https://github.com/SuperTux/supertux/actions/workflows/other.yml?query=branch%3Amaster)
for the rest (FreeBSD, WASM, Ubuntu Touch); click on any entry in the list on
the right (topmost = most recent), then scroll down to the Artifacts section.
Note that this only works if you are logged in to GitHub.

Compiling
---------

<sub>Tip: You may take inspiration from the [workflow files](.github/workflows).</sub>

### Requirements

To build SuperTux from source, you need to have a number of tools and
libraries installed. Note that most of these things should already be
available prepackaged and optimized for your distribution, it is
recommended that you check your distribution first before downloading
from the websites. You can also check
<https://github.com/SuperTux/wiki/blob/master/Building-SuperTux.md> for up-to-date
build instructions for a variety of different platforms and
distributions.

* General development tools:
  - C++ compiler (choose one of the two options below):
    + [gcc compiler suite](http://gcc.gnu.org) version 3.2 or newer (including g++)
    + [LLVM compiler](http://llvm.org/) (you probably want the clang frontend too)
  - [GNU Binutils](http://www.gnu.org/software/binutils) (or the BSD/OS X equivalent)
  - a shell and common POSIX command line tools
  - **Note:** To get these tools, you can install `build-essential` on Debian-based distros,
    `base-devel` on Arch-based distros and the Xcode Command Line tools on OS X.
* [CMake](http://www.cmake.org/) 3.1 or later: most package managers ship this as `cmake`
* OpenGL headers and libraries: OpenGL libraries and headers are
  specific to your graphics card. Make sure that you have hardware
  accelerated OpenGL drivers installed. Software renderers like Mesa
  will make SuperTux unplayable slow.
* [SDL2](http://www.libsdl.org) (2.0.1 or later)
* [SDL2_image](http://www.libsdl.org/projects/SDL_image) (2.0.0 or later)
* [OpenAL](http://www.openal.org): (1.0 or later)
* C++ OpenGL library (choose one of the two options below):
  - [GLEW](http://glew.sourceforge.net/) or
  - [glbinding](https://github.com/hpicgs/glbinding)
* [cURL](http://curl.haxx.se/libcurl/): for Add-on downloads
* [libogg and libvorbis](https://www.xiph.org/)
* [FreeType](https://www.freetype.org/)
* [GLM](https://github.com/g-truc/glm)
* [ZLib](https://www.zlib.net/)
* [libraqm](https://github.com/HOST-Oman/libraqm): optional, but needed
  to display Arabic

**Note I:** for any of the above listed libraries (OpenGL, SDL2, SDL2_image,
OpenAL, GLEW/glbinding, cURL, libogg and libvorbis), you should
also have development headers installed. Debian-based distributions have `-devel`
packages containing the mentioned headers, on Arch Linux these should be included
in the library package.

**Note II:** We tried to write our code clean, portable and platform neutral,
so it should be possible to compile it on a wide range of platforms and also
with other compilers than gcc or clang. We use GitHub Actions
to test commits and pull requests in our repository, but unfortunately it's not
always possible to test the code in very exotic setups. However, feel free to
report issues to our bug tracker on GitHub.

**Note III (regarding glbinding):** To use glbinding instead of GLEW, call `cmake`
with the flag -DGLBINDING_ENABLED=ON

For ease of use, here are some installation lines for some Linux distributions:

- Ubuntu 18.04/20.04:
  ```
  sudo apt-get update && sudo apt-get install -y cmake build-essential libogg-dev libvorbis-dev libopenal-dev libsdl2-dev libsdl2-image-dev libfreetype6-dev libraqm-dev libcurl4-openssl-dev libglew-dev libharfbuzz-dev libfribidi-dev libglm-dev zlib1g-dev
  ```

- ArchLinux (using sudo, as of August 28th 2024)
  ```
  sudo pacman -S cmake base-devel libogg libvorbis openal sdl2 sdl2_image freetype2 libraqm curl openssl glew harfbuzz fribidi glm zlib
  ```

### Linux/UNIX using CMake

SuperTux uses CMake to generate a set of Makefiles for the build
process. To generate these Makefiles and build SuperTux, perform the
following steps:

1. `cd` to the directory where you unpacked the SuperTux source
   archive, i.e. to the directory containing `src` and `data`.

2. If you cloned this Supertux repo using git run `git submodule
   update --init --recursive` to fetch/update squirrel, tinygettext,
   physfs, and some other modules.
   (If you got this version of Supertux from a tarball (.tar), squirrel
   and tinygettext are already in the tarball.)

3. Create and change to a new, empty build directory by running `mkdir
   build`, `cd build`.

4. Run `cmake ..` to create the Makefiles needed to build SuperTux
   with standard options. If you are missing any libraries needed to
   build SuperTux, install those first, then try running CMake again.
   See below for instructions on how to change to standard options.

5. Type `make` to start the build process.

6. Type `make install` to install the programs and any data files and
   documentation. (You should be a root user on Linux systems. You can
   become a root user with the `su` command or by using `sudo make
   install`) Note that there is no uninstall target, so you might wish
   to create a package or other system-specific installation instead.

7. The game should work now and you can remove the source directory.

You can customize the build process by setting additional options for
CMake. The easiest way to do this is to use run `ccmake ..` instead of
`cmake ..` to bring up the curses-based user interface of CMake.
Select an option using the arrow keys, change the selected option by
pressing the Enter key, then hit the `c` (repeatedly, if necessary) to
apply your changes and bring up new options resulting from your newly
set ones. When you are done, press the `g` key to generate a new set
of Makefiles and exit.

Alternatively, you can pass options to `cmake ..` via the command
line. Some common command line switches are:

- `-DCMAKE_VERBOSE_MAKEFILE=ON`
  : Generates Makefiles that print all commands prior to executing them.

- `-Dxxx_LIBRARY=/path/to/library.so -Dxxx_INCLUDE_DIR=/path/to/headerfiles`
  : Manually specify the installation directory of a library.

- `-DCMAKE_BUILD_TYPE=DEBUG`
  : Enables debug mode and compiles extra debug symbols into the SuperTux
  executable. This is useful when sending in bug reports to the
  developers.

- `-DCMAKE_BUILD_TYPE=RELEASE`
  : Enables release mode and compiles some sanity checks out of the build.

- `-DCMAKE_C_COMPILER=cc`, `-DCMAKE_CXX_COMPILER=cpp`
  : Changes which compiler to use. Options are also available for the linker,
    but using this option is less common.

- `-DENABLE_OPENGL=ON`, `-DENABLE_OPENGLES2=ON`
  : The former controls whether to compile in OpenGL support (else the game
  will use the SDL renderer), the latter controls supporting OpenGL ES 2.

- `-DBUILD_TESTS=ON`
  : Enables compiling the test suite. To run the test suite, run
  "test_supertux2" after compiling SuperTux. Requires installing
  [GoogleTest](https://github.com/google/googletest) as an additional
  dependency.

- `-DBUILD_DOCUMENTATION=ON`
  : Enables building documentation. Requires downloading and installing
  [Doxygen](https://www.doxygen.nl/index.html) as an additional dependency.
  You may build documentation after building SuperTux by running
  "doxygen docs/Doxyfile" from the build directory, and by opening
  "docs/doxygen/html/index.html".

- `-DENABLE_DISCORD=ON`
  : Enables compiling the Discord integration in SuperTux. You may re-disable
  the integration later by replacing "ON" with "OFF".

- `-DVCPKG_BUILD=ON`
  : Used if using vcpkg for dependencies. If you are using this, don't forget
  `-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake` and
  `-DVCPKG_TARGET_TRIPLET=your-triplet` if necessary.

- `-DWARNINGS=ON`
  : Enable all warnings. The CI enables this option, so it might be useful to
  keep this option activated during development.

- `-DWERROR=ON`
  : Treat all warnings as errors. The CI enables this option, so it might be
  useful to enable this option and to recompile before opening a pull request.


#### Notes for GIT users

SuperTux does not need to be installed on the system;
you can run it from its own directory.

If `ccache` is installed and working,
it can help to significantly reduce the (re)compilation time,
e.g. when doing a git bisect with many checkouts.
Furthermore, it is possible to reduce the optimization level to O1,
which may reduce compilation times:
```
cmake .. -DWARNINGS=ON -DCMAKE_CXX_FLAGS="-O1"
```

A general way to reduce compilation time is running `make` with multiple
threads:
```
make -j $(nproc || sysctl -n hw.ncpu || echo 2)
```


### Windows using CMake and Visual Studio

**Note: SuperTux is currently not compilable with MinGW.**

To build SuperTux on Windows with Visual Studio, you will need:
- An IDE of your choice (Optional, but Git will need you to select one when installing)
- Visual Studio Community (or any edition). SuperTux is tested with Visual Studio 16 (2019).
- [CMake](https://cmake.org/download/)
- [vcpkg](https://github.com/Microsoft/vcpkg)
- [Git](https://git-scm.com/)

Once all of these are installed; you may install dependencies with vcpkg. In any CLI, from the vcpkg folder, run:
```
./bootstrap-vcpkg.bat -disableMetrics
./vcpkg integrate install
./vcpkg install --triplet=x64-windows gtest curl freetype glew libogg libraqm libvorbis openal-soft sdl2 sdl2-image[libjpeg-turbo] glm zlib
```

**Note:** If you wish to produce 32-bit builds, replace `--triplet=x64-windows` with `--triplet=x86-windows`.

Once dependencies are installed:

1. Unpack the SuperTux source pack or get the source with git (`git clone --recursive https://github.com/SuperTux/supertux.git`).

2. Create a new, empty `build` folder.

3. Open a console window and navigate to the `build` directory.

5. Run `cmake .. -DCMAKE_BUILD_TYPE=Release -DVCPKG_BUILD=ON -DCMAKE_TOOLCHAIN_FILE=C:/PATH/TO/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows`.

    **Make sure to change the path to vcpkg to the actual path to vcpkg on your system!**

    Options:
    - `-DCMAKE_BUILD_TYPE=Release` creates a Release build, which does not contain debug information and runs faster. You may also produce `Debug` builds, but these run significantly slower. Other options may be available, such as `RelWithDebInfo` and `MinSizeRel`, but they are not tested. [More info](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)
    - `-DVCPKG_BUILD=ON` tells SuperTux to use vcpkg to find dependencies.
    - `-DCMAKE_TOOLCHAIN_FILE=C:/PATH/TO/vcpkg/scripts/buildsystems/vcpkg.cmake` tells CMake to use the vcpkg toolchain to load dependencies. **Make sure to change the path to the actual path to vcpkg on your system!**
    - `-DVCPKG_TARGET_TRIPLET=x64-windows` tells vcpkg to use 64-bit dependencies for Windows. If you are compiling 32-bit executables, replace "x64" with "x86".
    - Optionally, you may add `-G "Visual Studio 16 2019"` to force a certain version of Visual Studio if multiple are installed on your system.

    For more CMake options, look at end of the Linux/UNIX build section.

5. You may now build SuperTux either by opening the new Visual Studio solution "SUPERTUX.sln" in the "build" directory, or by running `cmake --build . --config Release` instead. (If you prefer building a debug build, change "Release" to "Debug". Make sure you use the same option as when calling CMake!)

6. Move the file "run_supertux.bat" from the "build" folder to the "build/Release" (or "build/Debug") folder.

7. Now you can run SuperTux using the "run_supertux.bat" file. If you run this file from the command line, note that you need to `cd` in the Release/Debug folder beforehand.

### Android using SDL2

This port exists thanks to **[Pelya](https://github.com/pelya)**!

You will need to install Android Studio with the command-line tools and with
NDK **version 23** (other versions won't work). You will also need git.

It is recommended to run these commands on Ubuntu 20.04.

Note that the commands which require a certain wordking directory will have the
assumed initial directory at the top of hte code block; if you follow these
instructions step by step, you should have the assumed Current Working
Directory at each step.

1. Unpack the source archive or clone the repository recursively using git.

```
git clone --recursive https://github.com/supertux/supertux
```

2. Inside the repository, unpack
[Pelya's cross-compilation suite](https://github.com/pelya/commandergenius) on
commit `532acc9192`. Rename the folder "build.android" and place it at the root
of the SuperTux repository.

```
# Assuming CWD = root of the SuperTux source folder
# Use `cd supertux` if you ran the commands from the preceeding step verbatim

git clone --depth=100 https://github.com/pelya/commandergenius.git build.android
git -C build.android checkout 532acc9192

# You can also download it from GitHub, but make sure you download it from
# commit 532acc9192!
```

3. Clone the submodules that SuperTux needs: Iconv, SDL2, SDL2_image,
SDL2_mixer and SDL2_ttf.

```
# Assuming CWD = root of the SuperTux source folder
cd build.android

git submodule update --init --recursive --depth=1 \
          project/jni/iconv/src                   \
          project/jni/sdl2 project/jni/sdl2_image \
          project/jni/sdl2_mixer project/jni/sdl2_ttf
```

4. Symlink the project inside the build directory.

```
# Assuming CWD = build.android
cd ..

rm -rf build.android/project/jni/application/supertux/supertux
ln -s `pwd` build.android/project/jni/application/supertux/supertux
ln -s supertux build.android/project/jni/application/src
```

5. Optionally, limit the build to a certain set of architectures.

```
# Assuming CWD = root of the SuperTux source folder

# Possible options: armeabi-v7a arm64-v8a x86 x86_64
# You may specify multiple; if so, separate each architecture with a space.
sed -i "s/MultiABI=.*/MultiABI='armeabi-v7a arm64-v8a x86 x86_64'/g" \
          build.android/project/jni/application/supertux/AndroidAppSettings.cfg

# You can also edit that file manually using any text editor.
```

6. Add your $ANDROID_NDK_HOME to your PATH, then configure SuperTux.
(Tip: Your NDK home is probably `$HOME/Android/Sdk/ndk/[version 23]`, if it is
not already set.)

```
# Assuming CWD = root of the SuperTux source folder

export PATH=$ANDROID_NDK_LATEST_HOME:$PATH
cd build.android
./changeAppSettings.sh
```

7. Accept the Android Studio Licenses. You may either open the
`build.android/project` folder in Android Studio, although it might get laggy;
you can also accept licenses from the CLI.

```
# It is recommended to read the licenses, but if you've already read them, you
# can execute `sudo -v` to enable sudo priviledges and add `yes | ` in front of
# the next line.
sudo $ANDROID_SDK_ROOT/cmdline-tools/latest/bin/sdkmanager --licenses
```

8. Assemble a release build. <sub>TODO: Check for debug builds</sub>

```
# Assuming CWD = build.android
cd project

./gradlew assembleRelease
```

9. Accept the licenses again, but this time by specifying the project as root.

```
# Similarly as step #7, you can prepend `yes | ` to accept all licenses
sudo $ANDROID_SDK_ROOT/cmdline-tools/latest/bin/sdkmanager --licenses \
          --sdk_root=`pwd`
```

10. Generate a debug key, if you don't have one already.

```
mkdir -p ~/.android
keytool -genkey -v -keystore ~/.android/debug.keystore -storepass android \
          -alias androiddebugkey -keypass android -keyalg RSA -keysize 2048 \
          -validity 10000 \
          -dname "CN=Debug, OU=Debug, O=Debug, L=Debug, ST=Debug, C=Debug"
```

11. Setup the project properties. <sub>TODO: What do these do and what value can
they take?</sub>

```
# Assuming CWD = build.android/project

echo "sdk.dir=$ANDROID_SDK_ROOT" > local.properties
echo "proguard.config=proguard.cfg;proguard-local.cfg" >> local.properties
```

12. Build SuperTux by running the `build.sh` script in the build.android folder.

```
# Assuming CWD = build.android/project
cd ..

export PATH=$ANDROID_NDK_HOME:$ANDROID_SDK_ROOT/build-tools/31.0.0:$PATH
./build.sh
```

13. To generate a .aab, run `./gradlew bundleReleaseWithDebugInfo` from the
project folder. The file will be located, from `build.android/project`, at
`app/build/outputs/bundle/releaseWithDebugInfo/app-releaseWithDebugInfo.aab`.

```
# Assuming CWD = build.android

cd project
./gradlew bundleReleaseWithDebugInfo
```

14. To generate a .apk, run `./create-apk-with-data.sh` from the
`build.android/project/jni/application/supertux` folder. The file will be
located at `build.android/SuperTux-with-data.apk`.

```
# Assuming CWD = build.android/project

cd jni/application/supertux
./create-apk-with-data.sh
```

### WASM using Emscripten

To compile SuperTux to host on the web, you will need [the Emscripten
SDK](https://github.com/emscripten-core/emsdk) and
[vcpkg](https://github.com/Microsoft/vcpkg), in addition to the
[general development tools](#requirements). Note that you only need the
*General development tools*; you do not need any library, as those are managed
with Vcpkg.

Note that you must install and activate the version **1.40.1** of the Emscripten
toolchain; newer versions are known not to work properly.

0. Make sure you have all the submodules if you are using Git:
```
git submodule update --init --recursive
```

1. Install [Emscripten](emscripten.org):
```
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

2. Run CMake using Emscripten's wrapper:
```
# Make sure you ran `source /path/to/emsdk/emsdk_env.sh` if you opened a new terminal since last step!
emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
```

3. Copy data files to the build folder, as Emscripten will package them to make
them usable from WASM:
```
rsync -aP ../data/ data/
```

4. Build SuperTux:
```
emmake make -j$(nproc || sysctl -n hw.ncpu || echo 2)
```

5. Replace the Emscripten HTML template with SuperTux's custom container:
```
cp template.html supertux2.html
```
You may skip the step above you intend to directly open the `template.html` file;
note that SuperTux won't work if it is not located in the custom template, as it
requires some custom JavaScript functions to work properly.

6. Run the Emscripten webserver:
```
emrun supertux2.html
```

You can now play SuperTux by opening `http://localhost:6931/supertux2.html` in
your browser.

Note that if you intend to run SuperTux on a public web server, you must set
[two headers](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer#security_requirements)
as a web security requirement.

### Ubuntu Touch using Clickable

To compile for Ubuntu Touch phones, you will need an Ubuntu desktop, as well as
[Clickable](https://clickable-ut.dev/en/latest/install.html). You will not
need any other development library, as those are already managed by Clickable.

First, make sure you have all the submodules:
```
git submodule update --init --recursive
```

Then, enable developer mode on your phone, if it isn't enabled already.

Then:
- To install SuperTux on your phone, plug your phone to your computer and run:
  ```
  clickable
  ```
  This will by default build with the SDL renderer only; if you wish to use the
  OpenGL renderer, you may specify a different build file with `--config` to
  build with one of the files located at `mk/clickable/build-with-*.json`:
  ```
  clickable --config mk/clickable/build-with-glew.json
  ```

- To run SuperTux directly on your computer:
  ```
  clickable desktop --config mk/clickable/clickable.json
  ```

- To build SuperTux without running it:
  ```
  clickable build --config mk/clickable/clickable.json
  ```
  You may specify an architecture using the `--arch ARCH` flag. Clickable
  currently supports `amd64`, `arm64` and `armhf`.
