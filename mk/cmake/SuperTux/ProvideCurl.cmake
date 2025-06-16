## The curl headers are in the standard include path in dependencies
## so it's not required to add them here

if(NOT EMSCRIPTEN)
  find_package(CURL REQUIRED)

  add_library(LibCurl INTERFACE IMPORTED)
  set_target_properties(LibCurl PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}")
  if(WIN32)
    target_link_libraries(LibCurl INTERFACE ${CURL_LIBRARY})
  else()
    set_target_properties(LibCurl PROPERTIES
      INTERFACE_LINK_LIBRARIES "${CURL_LIBRARY}")
  endif()

  set(HAVE_LIBCURL TRUE)
endif()

# EOF #
