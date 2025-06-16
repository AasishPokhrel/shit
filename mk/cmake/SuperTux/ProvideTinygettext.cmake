option(USE_SYSTEM_TINYGETTEXT "Use preinstalled tinygettext if available" ON)
if(USE_SYSTEM_TINYGETTEXT)
  find_package(tinygettext QUIET)
endif()

if(TARGET tinygettext)
  message(STATUS "Found preinstalled tinygettext")

  add_library(LibTinygettext ALIAS tinygettext)
else()
  if(USE_SYSTEM_TINYGETTEXT)
    message(STATUS "Could NOT find tinygettext, using external/tinygettext fallback")
  endif()

  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/tinygettext/CMakeLists.txt)
    message(FATAL_ERROR "tinygettext submodule is not checked out or ${CMAKE_CURRENT_SOURCE_DIR}/external/tinygettext/CMakeLists.txt is missing")
  endif()

  # Include altivec wrapper on ppc
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^ppc.*")
    set(TINYGETTEXT_CXX_FLAGS "-isystem ${CMAKE_CURRENT_SOURCE_DIR}/src/ppc ${CMAKE_CXX_FLAGS}")
  else()
    set(TINYGETTEXT_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  endif()

  # CMake lists are just strings with ';' separators, thus passing lists
  # of lists is impossible. ExternalProject_Add provides LIST_SEPARATOR
  # to work around this limitations by using a different separator.
  string(REPLACE ";" "|" TINYGETTEXT_SDL2_INCLUDE_DIRS "${SDL2_INCLUDE_DIRS}")

  set(TINYGETTEXT_PREFIX ${CMAKE_BINARY_DIR}/tinygettext)
  ExternalProject_Add(tinygettext_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/tinygettext/"
    BUILD_BYPRODUCTS "${TINYGETTEXT_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}tinygettext${CMAKE_STATIC_LIBRARY_SUFFIX}"
    LIST_SEPARATOR "|"
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${TINYGETTEXT_CXX_FLAGS}
    -DBUILD_SHARED_LIBS=OFF
    -DHAVE_SDL=ON
    -DCMAKE_INSTALL_PREFIX=${TINYGETTEXT_PREFIX}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DSDL2_LIBRARIES=${SDL2_LIBRARIES}
    -DSDL2_INCLUDE_DIRS=${TINYGETTEXT_SDL2_INCLUDE_DIRS}
    -DLIB_SUFFIX=${LIB_SUFFIX})

  # Pre-create directory so that cmake doesn't complain about its non-existance
  file(MAKE_DIRECTORY ${TINYGETTEXT_PREFIX}/include)

  add_library(LibTinygettext STATIC IMPORTED)
  set_target_properties(LibTinygettext PROPERTIES
    IMPORTED_LOCATION "${TINYGETTEXT_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}tinygettext${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${TINYGETTEXT_PREFIX}/include")

  add_dependencies(LibTinygettext tinygettext_project)
endif()

# EOF #
