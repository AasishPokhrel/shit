option(USE_SYSTEM_FMT "Use preinstalled fmt if available, must be 8.0.0 or newer" ON)
if(USE_SYSTEM_FMT)
  find_package(fmt 8.0.0 QUIET)
endif()

if(TARGET fmt::fmt)
  message(STATUS "Found fmt")

  add_library(LibFmt ALIAS fmt::fmt)

else()
  message(STATUS "Could NOT find fmt, using external/fmt fallback")

  add_subdirectory(external/fmt EXCLUDE_FROM_ALL)

  add_library(LibFmt INTERFACE IMPORTED)
  set_target_properties(LibFmt PROPERTIES
    INTERFACE_LINK_LIBRARIES "fmt::fmt"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "$<TARGET_PROPERTY:fmt::fmt,INTERFACE_INCLUDE_DIRECTORIES>")

endif()

# EOF #
