find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_RAQM raqm)
endif()

find_path(RAQM_INCLUDE_DIR NAMES raqm.h HINTS ${PC_RAQM_INCLUDE_DIRS}
  ${PC_RAQM_INCLUDEDIR})
find_library(RAQM_LIBRARY NAMES raqm HINTS ${PC_RAQM_LIBRARY_DIRS}
  ${PC_RAQM_LIBDIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RAQM DEFAULT_MSG RAQM_INCLUDE_DIR RAQM_LIBRARY)
