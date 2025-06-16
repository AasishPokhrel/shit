find_package(PhysFS)

if(PHYSFS_LIBRARY)
  set(CMAKE_REQUIRED_LIBRARIES ${PHYSFS_LIBRARY})
  check_symbol_exists("PHYSFS_getPrefDir" "${PHYSFS_INCLUDE_DIR}/physfs.h" HAVE_PHYSFS_GETPREFDIR)
endif()

if(HAVE_PHYSFS_GETPREFDIR)
  set(USE_SYSTEM_PHYSFS ON CACHE BOOL "Use preinstalled physfs (must support getPrefDir)")
else()
  set(USE_SYSTEM_PHYSFS OFF CACHE BOOL "Use preinstalled physfs (must support getPrefDir)")
endif()

if(USE_SYSTEM_PHYSFS)
  add_library(LibPhysfs INTERFACE)
  target_link_libraries(LibPhysfs INTERFACE ${PHYSFS_LIBRARY})
  set_target_properties(LibPhysfs PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${PHYSFS_INCLUDE_DIR}")
else()
  if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/external/physfs/CMakeLists.txt)
    message(FATAL_ERROR "physfs submodule is not checked out or ${CMAKE_CURRENT_SOURCE_DIR}/external/physfs/CMakeLists.txt is missing")
  endif()

  if(WIN32)
    set(PHYSFS_BUILD_SHARED TRUE)
    set(PHYSFS_BUILD_STATIC FALSE)
  else()
    set(PHYSFS_BUILD_SHARED FALSE)
    set(PHYSFS_BUILD_STATIC TRUE)
  endif()

  set(PHYSFS_PREFIX ${CMAKE_BINARY_DIR}/physfs)
  ExternalProject_Add(physfs_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/physfs/"
    BUILD_BYPRODUCTS
    "${PHYSFS_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}physfs${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${PHYSFS_PREFIX}/lib${LIB_SUFFIX}/physfs${CMAKE_LINK_LIBRARY_SUFFIX}"
    "${PHYSFS_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}physfs${CMAKE_STATIC_LIBRARY_SUFFIX}"
    CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_INSTALL_PREFIX=${PHYSFS_PREFIX}
    -DLIB_SUFFIX=${LIB_SUFFIX}
    -DPHYSFS_BUILD_SHARED=${PHYSFS_BUILD_SHARED}
    -DPHYSFS_BUILD_STATIC=${PHYSFS_BUILD_STATIC}
    -DPHYSFS_BUILD_TEST=FALSE)

  # Pre-create directory so that cmake doesn't complain about its non-existance
  file(MAKE_DIRECTORY "${PHYSFS_PREFIX}/include/")

  if(WIN32)
    add_library(LibPhysfs SHARED IMPORTED)
    set_target_properties(LibPhysfs PROPERTIES
      IMPORTED_LOCATION "${PHYSFS_PREFIX}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}physfs${CMAKE_SHARED_LIBRARY_SUFFIX}"
      IMPORTED_IMPLIB "${PHYSFS_PREFIX}/lib${LIB_SUFFIX}/physfs${CMAKE_LINK_LIBRARY_SUFFIX}"
      INTERFACE_INCLUDE_DIRECTORIES "${PHYSFS_PREFIX}/include/")
  else()
    add_library(LibPhysfs STATIC IMPORTED)
    set_target_properties(LibPhysfs PROPERTIES
      IMPORTED_LOCATION "${PHYSFS_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}physfs${CMAKE_STATIC_LIBRARY_SUFFIX}"
      INTERFACE_INCLUDE_DIRECTORIES "${PHYSFS_PREFIX}/include/")
  endif()

  if(APPLE)
    set_target_properties(LibPhysfs PROPERTIES
      INTERFACE_LINK_LIBRARIES "-framework CoreFoundation;-framework Foundation;-framework IOKit")
  endif()

  add_dependencies(LibPhysfs physfs_project)

  if(WIN32)
    get_property(PHYSFS_LIB_PATH TARGET LibPhysfs PROPERTY IMPORTED_LOCATION)
    install(FILES ${PHYSFS_LIB_PATH} DESTINATION "${INSTALL_SUBDIR_BIN}")
  endif()
endif()

mark_as_advanced(
  PHYSFS_INCLUDE_DIR
  PHYSFS_LIBRARY
  )

# EOF #
