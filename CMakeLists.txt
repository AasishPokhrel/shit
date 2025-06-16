#
# SuperTux - root build script
# Copyright (C) 2006 Christoph Sommer <christoph.sommer@2006.expires.deltadevelopment.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#


#
# INSTRUCTIONS:
# -------------
#
# Create a directory build/ and change to it. Run
#
#   cmake ..
#
# This creates a set of Makefiles to build the project. Run
#
#   make
#

cmake_minimum_required(VERSION 3.1)

## Project name to use as command prefix.

project(SUPERTUX)


### CMake configuration

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0008 NEW)
  cmake_policy(SET CMP0023 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This should be set only with windows Visual Studio generator builds
string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" PLATFORM)
if(${PLATFORM} MATCHES "arm64")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_ARM64_ /DMY_CPU_LE")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D_ARM64_ /DMY_CPU_LE")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake)
include(ConfigureFiles)
include(ExternalProject)
include(CheckCXXCompilerFlag)
include(CheckSymbolExists)

## For autopackage
set(APPDATADIR "${CMAKE_INSTALL_PREFIX}/share/games/supertux2")

set(BUILD_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(BUILD_CONFIG_DATA_DIR "${CMAKE_BINARY_DIR}/data")

## Check endianess
if(NOT EMSCRIPTEN)
  # FIXME: Any reason why we need this?
  include(TestBigEndian)
  test_big_endian(WORDS_BIGENDIAN)
endif()

## Add definitions
if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  add_definitions(-DRELEASE)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
endif()

# Options for install
if(WIN32 AND NOT UNIX)
  set(INSTALL_SUBDIR_BIN "bin" CACHE STRING "Installation subdir for binaries")
  set(INSTALL_SUBDIR_SHARE "data" CACHE STRING "Installation subdir for data")
  set(INSTALL_SUBDIR_DOC "doc" CACHE STRING "Installation subdir for docs")
else()
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DISABLE_CPACK_BUNDLING)
    set(INSTALL_SUBDIR_BIN "SuperTux.app/Contents/MacOS" CACHE STRING "Installation subdir for binaries")
    set(INSTALL_SUBDIR_SHARE "SuperTux.app/Contents/Resources/data" CACHE STRING "Installation subdir for data")
    set(INSTALL_SUBDIR_DOC "SuperTux.app/Contents/Resources" CACHE STRING "Installation subdir for docs")
  else()
    set(INSTALL_SUBDIR_BIN "games" CACHE STRING "Installation subdir for binaries")
    set(INSTALL_SUBDIR_SHARE "share/games/supertux2" CACHE STRING "Installation subdir for data")
    set(INSTALL_SUBDIR_DOC "share/doc/supertux2" CACHE STRING "Installation subdir for docs")
  endif()
endif()

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX .html)
  set(IS_EMSCRIPTEN_BUILD ON)

  set(EM_USE_FLAGS "-sDISABLE_EXCEPTION_CATCHING=0")
  set(EM_LINK_FLAGS " -sINITIAL_MEMORY=134217728 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=536870912 -sERROR_ON_UNDEFINED_SYMBOLS=0 --preload-file ${BUILD_CONFIG_DATA_DIR} --use-preload-plugins -lidbfs.js")
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(EM_USE_FLAGS "${EM_USE_FLAGS} -fsanitize=undefined")
    set(EM_LINK_FLAGS "${EM_LINK_FLAGS} -fsanitize=undefined -sSAFE_HEAP=1 -sASSERTIONS=1")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EM_USE_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EM_USE_FLAGS}")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${EM_USE_FLAGS} ${EM_LINK_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${EM_USE_FLAGS} ${EM_LINK_FLAGS}")
endif()

# TODO: Add " OR ANDROID OR IOS" to this
if(EMSCRIPTEN OR UBUNTU_TOUCH OR ANDROID)
  option(REMOVE_QUIT_BUTTON "Remove the option to quit the game (useful on mobile devices)" ON)
else()
  option(REMOVE_QUIT_BUTTON "Remove the option to quit the game (useful on mobile devices)" OFF)
endif()

option(STEAM_BUILD "Prepare build for Steam" OFF)
option(IS_SUPERTUX_RELEASE "Build as official SuperTux release" OFF)

set(SUPERTUX_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(VCPKG_BUILD OFF CACHE BOOL "Use dependencies installed via vcpkg (not dependency package)")
set(VCPKG_APPLOCAL_DEPS ${VCPKG_BUILD} BOOL)

# Detect mobile builds
option(UBUNTU_TOUCH "Compile the project for an Ubuntu Touch target" OFF)

# Mobile builds
if(UBUNTU_TOUCH OR ANDROID)
  option(HIDE_NONMOBILE_OPTIONS "Hide options that are impractical on mobile devices (e. g. changing screen resolution)" ON)
else()
  option(HIDE_NONMOBILE_OPTIONS "Hide options that are impractical on mobile devices (e. g. changing screen resolution)" OFF)
endif()

# Configure main menu logo
if(("${SUPERTUX_VERSION_STRING}" MATCHES "^v[0-9]+\\.[0-9]+\\.[0-9]+$") OR IS_SUPERTUX_RELEASE OR STEAM_BUILD)
  set(LOGO_FILE "logo.png")
else()
  set(LOGO_FILE "logo_dev.png")
endif()

if(WIN32)
  include(SuperTux/Win32)
endif()

## Check platform-dependent build options
include(ConfigureChecks)

## Some additional compiler switches
include(SuperTux/ClangTidy)
include(SuperTux/WarningFlags)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib")
endif()

## Add lots of dependencies to compiler switches
include(SuperTux/ProvideGlm)
include(SuperTux/ProvideFmt)
include(SuperTux/ProvideSDL2)
include(SuperTux/ProvideOpenAL)
include(SuperTux/ProvideOggVorbis)
include(SuperTux/ProvidePhysfs)
include(SuperTux/ProvideCurl)
include(SuperTux/ProvideSimpleSquirrel)
include(SuperTux/ProvideTinygettext)
include(SuperTux/ProvideSDL2_ttf)
include(SuperTux/ProvideDiscord)
include(SuperTux/ProvideSexpcpp)
include(SuperTux/ProvideSavePNG)
include(SuperTux/ProvidePartioZip)
include(SuperTux/ProvideOpenGL)

## Build stuff
include(SuperTux/BuildVersion)
include(SuperTux/BuildDocumentation)
include(SuperTux/BuildMessagePot)

## Build list of sources for supertux binary
file(GLOB SUPERTUX_SOURCES_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} external/obstack/*.c external/findlocale/findlocale.c)

file(GLOB SUPERTUX_SOURCES_CXX RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*/*.cpp src/supertux/menu/*.cpp src/video/sdl/*.cpp src/video/null/*.cpp)
file(GLOB SUPERTUX_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/tmp/*.rc")

if(HAVE_OPENGL)
  file(GLOB SUPERTUX_OPENGL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/video/gl/*.cpp)
  set(SUPERTUX_SOURCES_CXX ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_OPENGL_SOURCES})
endif()

## Sort source lists to have deterministic linking order
list(SORT SUPERTUX_SOURCES_C)
list(SORT SUPERTUX_SOURCES_CXX)
list(SORT SUPERTUX_RESOURCES)

## On Windows, add an icon
if(WIN32)
  if(MINGW)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o
      COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons -i${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc -o ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
    set(SUPERTUX_SOURCES_C ${SUPERTUX_SOURCES_C} ${CMAKE_CURRENT_BINARY_DIR}/supertux_rc.o)
  endif()
endif()

include(SuperTux/CompileAmalgation)

## Generate supertux executable in the right place

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

## Add target for supertux binary

add_library(supertux2_c OBJECT ${SUPERTUX_SOURCES_C})
add_library(supertux2_lib STATIC ${CMAKE_BINARY_DIR}/version.h ${SUPERTUX_SOURCES_CXX} ${SUPERTUX_RESOURCES} $<TARGET_OBJECTS:supertux2_c>)
target_include_directories(supertux2_lib PUBLIC ${CMAKE_BINARY_DIR} src/)

if(WIN32)
  add_executable(supertux2 WIN32 src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.rc)
  target_link_libraries(supertux2 LibSDL2main)
else()
  add_executable(supertux2 src/main.cpp)
endif()
target_link_libraries(supertux2 supertux2_lib)

set_target_properties(supertux2_lib PROPERTIES OUTPUT_NAME supertux2_lib)
set_target_properties(supertux2_lib PROPERTIES COMPILE_FLAGS "${SUPERTUX2_EXTRA_WARNING_FLAGS}")
if(EMSCRIPTEN)
  target_link_options(supertux2 PUBLIC -sEXPORTED_FUNCTIONS=['_main','_set_resolution','_save_config','_onDownloadProgress','_onDownloadFinished','_onDownloadError','_onDownloadAborted','_getExceptionMessage'] PUBLIC -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap'])
endif()

if(WIN32 AND NOT VCPKG_BUILD)
if(NOT MINGW)
  ## Copy dlls on windows
  add_custom_command(TARGET supertux2_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${DEPENDENCY_FOLDER}/dll"
    $<TARGET_FILE_DIR:supertux2_lib>)
endif()
endif()

## Some additional include paths
target_include_directories(supertux2_lib SYSTEM PUBLIC
  external/findlocale/
  external/obstack/
  )

# Include altivec wrapper on ppc
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^ppc.*")
  target_include_directories(supertux2_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ppc)
endif()

## Link supertux binary with squirrel and other libraries

target_link_libraries(supertux2_lib PUBLIC LibSimpleSquirrel)
target_link_libraries(supertux2_lib PUBLIC LibTinygettext)
target_link_libraries(supertux2_lib PUBLIC LibSexp)
target_link_libraries(supertux2_lib PUBLIC LibSavePNG)
target_link_libraries(supertux2_lib PUBLIC LibPartioZip)
target_link_libraries(supertux2_lib PUBLIC LibOpenAL)
target_link_libraries(supertux2_lib PUBLIC LibGlm)
target_link_libraries(supertux2_lib PUBLIC LibFmt)
target_link_libraries(supertux2_lib PUBLIC LibPhysfs)

if(NOT EMSCRIPTEN)
  target_link_libraries(supertux2_lib PUBLIC LibSDL2_ttf)
  target_link_libraries(supertux2_lib PUBLIC LibSDL2 LibSDL2_image)
  target_link_libraries(supertux2_lib PUBLIC LibOggVorbis)
  target_link_libraries(supertux2_lib PUBLIC LibCurl)
endif()

if(HAVE_OPENGL)
  target_link_libraries(supertux2_lib PUBLIC LibOpenGL)
endif()

if(ENABLE_DISCORD)
  target_link_libraries(supertux2_lib PUBLIC LibDiscord)
endif()

## Install stuff
include(SuperTux/BuildInstall)

## Create config.h now that INSTALL_SUBDIR_* have been set.
configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

## Build tests
include(SuperTux/BuildTests)

## CPack/Installation-specific stuff
include(SuperTux/BuildCPack)

# move some config clutter to the advanced section
mark_as_advanced(
  INSTALL_SUBDIR_BIN
  INSTALL_SUBDIR_SHARE
  INSTALL_SUBDIR_DOC
  )

mark_as_advanced(
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_BUILD_TYPE
  CMAKE_INSTALL_PREFIX
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  CMAKE_OSX_ARCHITECTURES
  CMAKE_OSX_SYSROOT
  )

mark_as_advanced(
  APPDATADIR
  )

# EOF #
