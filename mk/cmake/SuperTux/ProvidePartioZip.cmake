if(EMSCRIPTEN)
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -sUSE_ZLIB=1")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_ZLIB=1")
else()
  find_package(ZLIB REQUIRED)
endif()

add_library(LibPartioZip ${CMAKE_CURRENT_SOURCE_DIR}/external/partio_zip/zip_manager.cpp)
target_link_libraries(LibPartioZip PUBLIC ${ZLIB_LIBRARIES})
target_include_directories(LibPartioZip SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_include_directories(LibPartioZip SYSTEM PUBLIC external/partio_zip)

# EOF #
