option(USE_CLANG_TIDY "Run clang-tidy" OFF)

if(CMAKE_VERSION VERSION_GREATER 3.6)
  if(USE_CLANG_TIDY)
    find_program(
      CLANG_TIDY_EXE
      NAMES "clang-tidy"
      DOC "Path to clang-tidy executable"
      )

    if(NOT CLANG_TIDY_EXE)
      message(FATAL_ERROR "clang-tidy not found.")
    else()
      message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
  endif()
endif()

# EOF #
