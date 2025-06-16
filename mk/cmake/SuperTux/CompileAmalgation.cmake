## Compile everything at once (roughly equivalent to cat *.cpp | gcc)
option(COMPILE_AMALGATION "Compile all the files together at once (experimental)" OFF)

if(COMPILE_AMALGATION)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "// Includes all source files of the project\n")
  foreach(ST_FILE ${SUPERTUX_SOURCES_CXX})
    file(RELATIVE_PATH CXX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src "${CMAKE_CURRENT_SOURCE_DIR}/${ST_FILE}")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp "#include \"${CXX_FILE}\"\n")
  endforeach(ST_FILE)
  ## Build instead of CXX sources
  set(SUPERTUX_SOURCES_CXX ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
else()
  if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/amalgation.cpp)
  endif()
endif()

# EOF #
