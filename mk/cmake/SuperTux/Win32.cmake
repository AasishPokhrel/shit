## These variables enable MSVC to find libraries located in "dependencies{32|64}"

if(NOT VCPKG_BUILD)
  ## Store path of dependecy folder
  set(DEPENDENCY_FOLDER "${PROJECT_SOURCE_DIR}/dependencies")
endif()

## To test if the host (not the build) is x64:
## "$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "AMD64"
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  set(WIN64 ON)
  if(NOT VCPKG_BUILD)
    set(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}64")
  endif()
else()
  set(WIN64 OFF)
  if(NOT VCPKG_BUILD)
    set(DEPENDENCY_FOLDER "${DEPENDENCY_FOLDER}32")
  endif()
endif()

if(WIN64)
  set(SUPERTUX_SYSTEM_NAME win64)
else()
  set(SUPERTUX_SYSTEM_NAME win32)
endif()

if(NOT VCPKG_BUILD)
  set(ENV{PATH} "$ENV{PATH};${DEPENDENCY_FOLDER}/include")
  set(ENV{LIB} "${DEPENDENCY_FOLDER}/lib")
  set(ENV{OPENALDIR} "${DEPENDENCY_FOLDER}")
endif()

## Enable multi-processor compilation (faster)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

## And shut up about unsafe stuff
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
## Add an icon
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/icon_rc.template" "${PROJECT_BINARY_DIR}/tmp/icon.rc")

## Find all used libraries
if(NOT VCPKG_BUILD)
  file(GLOB DLLS "${DEPENDENCY_FOLDER}/dll/*")
endif()

add_definitions(-D_USE_MATH_DEFINES -DNOMINMAX)
add_definitions(-DWIN32)

# EOF #
