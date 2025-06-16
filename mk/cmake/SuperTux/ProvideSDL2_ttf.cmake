option(USE_SYSTEM_SDL2_TTF "Use preinstalled SDL2_ttf if available" OFF)
if(USE_SYSTEM_SDL2_TTF)
  find_package(SDL2_ttf QUIET)
endif()

if (EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL_TTF=2")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sUSE_SDL_TTF=2")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -sUSE_SDL_TTF=2")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_SDL_TTF=2")
elseif(TARGET SDL2_ttf)
  message(STATUS "Found preinstalled SDL2_ttf")

  add_library(LibSDL2_ttf ALIAS SDL2_ttf)
else()
  message(STATUS "Could NOT find SDL2_ttf, using external/SDL_ttf fallback")

  # Won't be used in practice - preserved for knowledge
  if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_FREETYPE=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sUSE_FREETYPE=1")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -sUSE_FREETYPE=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_FREETYPE=1")
  else()
    ## external/SDL_ttf with patches
    if(VCPKG_BUILD)
      find_package(freetype CONFIG REQUIRED)
    else()
      find_package(Freetype REQUIRED)
    endif()
  endif()

  find_package(RAQM)

  set(SDL2_TTF_PREFIX ${CMAKE_BINARY_DIR}/SDL2_ttf)
  ExternalProject_Add(SDL2_ttf_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/SDL_ttf/"
    BUILD_BYPRODUCTS "${SDL2_TTF_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2_ttf${CMAKE_STATIC_LIBRARY_SUFFIX}"
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX=${SDL2_TTF_PREFIX}
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DWITH_RAQM=${RAQM_FOUND}
    -DRAQM_LIBRARIES=${RAQM_LIBRARY}
    -DRAQM_INCLUDE_DIR=${RAQM_INCLUDE_DIR}
    -DBUILD_SHARED_LIBS=OFF
    -DEMSCRIPTEN=${EMSCRIPTEN})

  # Pre-create directory so that cmake doesn't complain about its non-existance
  file(MAKE_DIRECTORY ${SDL2_TTF_PREFIX}/include/SDL2)

  if(VCPKG_BUILD)
    list(APPEND SDL2_TTF_LINK_LIBRARIES freetype)
  else()
    list(APPEND SDL2_TTF_LINK_LIBRARIES ${FREETYPE_LIBRARIES})
  endif()

  if(RAQM_FOUND)
    find_package(FriBidi REQUIRED)
    find_package(HarfBuzz REQUIRED)
    list(APPEND SDL2_TTF_LINK_LIBRARIES
      ${HARFBUZZ_LIBRARY}
      ${FRIBIDI_LIBRARY}
      ${RAQM_LIBRARY})
  endif()

  add_library(LibSDL2_ttf STATIC IMPORTED)
  target_link_libraries(LibSDL2_ttf INTERFACE "${SDL2_TTF_LINK_LIBRARIES}")
  set_target_properties(LibSDL2_ttf PROPERTIES
    IMPORTED_LOCATION "${SDL2_TTF_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2_ttf${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_PREFIX}/include/SDL2")

  add_dependencies(LibSDL2_ttf SDL2_ttf_project)
endif()

# EOF #
