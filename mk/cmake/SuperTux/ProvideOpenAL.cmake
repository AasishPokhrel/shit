if(NOT EMSCRIPTEN)
  if(VCPKG_BUILD)
    find_package(OpenAL CONFIG REQUIRED)
    add_library(LibOpenAL ALIAS OpenAL::OpenAL)
  else()
    find_package(OpenAL REQUIRED)

    add_library(LibOpenAL INTERFACE IMPORTED)
    set_target_properties(LibOpenAL PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENAL_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
      )
  endif()
else()
  add_library(LibOpenAL INTERFACE IMPORTED)
  set_target_properties(LibOpenAL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/mk/emscripten/AL"
    INTERFACE_LINK_LIBRARIES "-lopenal"
    )
endif()

mark_as_advanced(
  OPENAL_INCLUDE_DIR
  OPENAL_LIBRARY
  )

# EOF #
