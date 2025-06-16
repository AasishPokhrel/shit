if(EMSCRIPTEN)
  option(USE_SYSTEM_GLM "Use the system-provided GLM instead of the external module" OFF)
else()
  option(USE_SYSTEM_GLM "Use the system-provided GLM instead of the external module" ON)
endif()

if(USE_SYSTEM_GLM)
  find_package(glm QUIET)

  if(NOT glm_FOUND)
    # fallback for old glm version in UBPorts
    find_path(GLM_INCLUDE_DIR
      NAMES glm/glm.hpp
      PATHS ${GLM_ROOT_DIR}/include)
    if(NOT GLM_INCLUDE_DIR)
      message(STATUS "Could NOT find glm, using external/glm fallback")
      set(USE_SYSTEM_GLM OFF)
    else()
      message(STATUS "Found glm: ${GLM_INCLUDE_DIR}")
      add_library(LibGlm INTERFACE IMPORTED)
      set_target_properties(LibGlm PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "GLM_ENABLE_EXPERIMENTAL")
    endif()
  endif()
endif()

if(NOT USE_SYSTEM_GLM)
  if (NOT EXISTS "${CMAKE_SOURCE_DIR}/external/glm/CMakeLists.txt")
    message(FATAL_ERROR "")
  endif()

  set(GLM_PREFIX ${CMAKE_BINARY_DIR}/glm/ex)
  ExternalProject_Add(glm_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/glm/"
    BUILD_BYPRODUCTS
    "${GLM_PREFIX}/include"
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX=${GLM_PREFIX}
    -DBUILD_TESTING=OFF
    -DINSTALL_INC_DIR=include)

  add_library(LibGlm INTERFACE IMPORTED)

  file(MAKE_DIRECTORY ${GLM_PREFIX}/include)

  set_target_properties(LibGlm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${GLM_PREFIX}/include"
    INTERFACE_COMPILE_DEFINITIONS "GLM_ENABLE_EXPERIMENTAL")

  add_dependencies(LibGlm glm_project)
elseif(glm_FOUND)
  if(TARGET glm::glm)
    message(STATUS "Found glm")

    add_library(LibGlm INTERFACE IMPORTED)
    set_target_properties(LibGlm PROPERTIES
      INTERFACE_LINK_LIBRARIES "glm::glm"
      INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "$<TARGET_PROPERTY:glm::glm,INTERFACE_INCLUDE_DIRECTORIES>"
      INTERFACE_COMPILE_DEFINITIONS "GLM_ENABLE_EXPERIMENTAL")
  else()
    # fallback for old glm version in EmuELEC and Nix
    message(STATUS "Found glm: ${GLM_INCLUDE_DIR}")

    add_library(LibGlm INTERFACE IMPORTED)
    set_target_properties(LibGlm PROPERTIES
      INTERFACE_LINK_LIBRARIES "glm"
      INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "$<TARGET_PROPERTY:glm,INTERFACE_INCLUDE_DIRECTORIES>"
      INTERFACE_COMPILE_DEFINITIONS "GLM_ENABLE_EXPERIMENTAL")
  endif()
endif()

# EOF #
