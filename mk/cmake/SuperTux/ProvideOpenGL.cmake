option(ENABLE_OPENGL "Enable OpenGL support" ON)
if(EMSCRIPTEN)
  option(ENABLE_OPENGLES2 "Enable OpenGLES2 support" ON)
else()
  option(ENABLE_OPENGLES2 "Enable OpenGLES2 support" OFF)
endif()
option(GLBINDING_ENABLED "Use glbinding instead of GLEW" OFF)
option(GLBINDING_DEBUG_OUTPUT "Enable glbinding debug output for each called OpenGL function" OFF)

if(ENABLE_OPENGL)
  if(ENABLE_OPENGLES2)
    message(STATUS "Checking for OpenGLES2")

    set(HAVE_OPENGL TRUE)
    list(APPEND OPENGL_COMPILE_DEFINITIONS "USE_OPENGLES2")

    if(EMSCRIPTEN)
      set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -sFULL_ES2=1")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} -sFULL_ES2=1")
    else()
      pkg_check_modules(GLESV2 REQUIRED glesv2)
      list(APPEND OPENGL_INCLUDE_DIRECTORIES "${GLESV2_INCLUDE_DIRS}")
      list(APPEND OPENGL_LINK_LIBRARIES "${GLESV2_LIBRARIES}")
    endif()
  else()
    message(STATUS "Checking for OpenGL")

    set(OpenGL_GL_PREFERENCE "LEGACY")
    find_package(OpenGL)

    if(OPENGL_FOUND)
      set(HAVE_OPENGL TRUE)

      list(APPEND OPENGL_LINK_LIBRARIES "${OPENGL_LIBRARIES}")
      list(APPEND OPENGL_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}")

      if(GLBINDING_ENABLED)
        list(APPEND OPENGL_COMPILE_DEFINITIONS "USE_GLBINDING")
        if(GLBINDING_DEBUG_OUTPUT)
          list(APPEND OPENGL_COMPILE_DEFINITIONS "USE_GLBINDING_DEBUG_OUTPUT")
        endif()

        if(VCPKG_BUILD)
          find_package(glbinding CONFIG REQUIRED)
          list(APPEND OPENGL_LINK_LIBRARIES glbinding::glbinding glbinding::glbinding-aux)
        else()
          find_package(GLBINDING REQUIRED)
          list(APPEND OPENGL_LINK_LIBRARIES ${GLBINDING_LIBRARIES})
          list(APPEND OPENGL_INCLUDE_DIRECTORIES ${GLBINDING_INCLUDES})
        endif()
      else()
        find_package(GLEW REQUIRED)
        list(APPEND OPENGL_LINK_LIBRARIES GLEW::GLEW)
        list(APPEND OPENGL_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}")
      endif()
    endif()
  endif()

  if(NOT HAVE_OPENGL)
    message(STATUS "  OpenGL not found")
  else()
    add_library(LibOpenGL INTERFACE)
    set_target_properties(LibOpenGL PROPERTIES
      INTERFACE_LINK_LIBRARIES "${OPENGL_LINK_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIRECTORIES}"
      INTERFACE_COMPILE_DEFINITIONS "${OPENGL_COMPILE_DEFINITIONS}"
      )

    message(STATUS "  OPENGL_LINK_LIBRARIES: ${OPENGL_LINK_LIBRARIES}")
    message(STATUS "  OPENGL_INCLUDE_DIRECTORIES: ${OPENGL_INCLUDE_DIRECTORIES}")
    message(STATUS "  OPENGL_COMPILE_DEFINITIONS: ${OPENGL_COMPILE_DEFINITIONS}")
  endif()
endif()

mark_as_advanced(
  GLEW_INCLUDE_DIR
  GLEW_LIBRARY
  )

# EOF #
