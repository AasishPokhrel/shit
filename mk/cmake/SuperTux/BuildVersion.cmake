if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  ## Find revision of WC
  mark_as_advanced(GIT_EXECUTABLE)
  find_program(GIT_EXECUTABLE git)
  if(NOT GIT_EXECUTABLE EQUAL "GIT_EXECUTABLE-NOTFOUND")
    include(GetGitRevisionDescription)
    git_describe(VERSION_STRING_GIT "--tags" "--match" "?[0-9]*.[0-9]*.[0-9]*")
    string(REPLACE "v" "" VERSION_LIST ${VERSION_STRING_GIT})
    string(REGEX REPLACE "[^\\-]+\\-([0-9]+)\\-.*" "\\1" VERSION_NUMBER_GIT "${VERSION_LIST}")
    string(REGEX REPLACE "(-|_|\\.)" ";" VERSION_LIST ";${VERSION_LIST}")
  endif()
endif()

get_filename_component(BASEDIR ${CMAKE_SOURCE_DIR} NAME)
if("${VERSION_LIST}" STREQUAL "")
  if(${BASEDIR} MATCHES "supertux2-[0-9\\.]*")
    string(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
  endif()
endif()

file(GLOB ORIG_TGZ ../*.orig.tar.gz)
if("${VERSION_LIST}" STREQUAL "" AND (NOT "${ORIG_TGZ}" STREQUAL ""))
  get_filename_component(BASEDIR ${ORIG_TGZ} NAME)
  string(REGEX REPLACE "(\\.|_|-)" ";" VERSION_LIST ${BASEDIR})
endif()

list(LENGTH VERSION_LIST VERSION_LIST_SIZE)

if(${VERSION_LIST_SIZE} GREATER 0)
  list(GET VERSION_LIST 1 MAJOR_VERSION_GIT)
  list(GET VERSION_LIST 2 MINOR_VERSION_GIT)
  list(GET VERSION_LIST 3 PATCH_VERSION_GIT)

  if("${VERSION_STRING_GIT}" STREQUAL "")
    set(VERSION_STRING_GIT "${MAJOR_VERSION_GIT}.${MINOR_VERSION_GIT}.${PATCH_VERSION_GIT}")
  endif()

  configure_file("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/version.cmake")
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/version.cmake")
  message( SEND_ERROR "Could not find GIT or valid version.cmake. Version information will be invalid." )
endif()
include("${CMAKE_SOURCE_DIR}/version.cmake")

if(FORCE_VERSION_STRING)
  set(SUPERTUX_VERSION_STRING "${FORCE_VERSION_STRING}")
endif()
set(SUPERTUX_VERSION ${SUPERTUX_VERSION_STRING})

configure_file(version.h.in ${CMAKE_BINARY_DIR}/version.h )

set_source_files_properties(${CMAKE_BINARY_DIR}/version.h
  PROPERTIES GENERATED true)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/main.cpp
  PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/supertux/title_screen.cpp
  PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/addon/addon_manager.cpp
  PROPERTIES OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/version.h")

# EOF #
