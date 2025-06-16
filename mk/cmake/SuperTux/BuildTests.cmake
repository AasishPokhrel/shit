option(BUILD_TESTS "Build test cases" OFF)

if(BUILD_TESTS)
  find_package(Threads REQUIRED)
  find_package(GTest REQUIRED)

  # build SuperTux tests
  file(GLOB_RECURSE TEST_SUPERTUX_SOURCES tests/*.cpp)
  add_executable(test_supertux2 ${TEST_SUPERTUX_SOURCES})
  target_compile_options(test_supertux2 PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_supertux2
    GTest::GTest GTest::Main
    supertux2_lib
    ${CMAKE_THREAD_LIBS_INIT})

  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
  add_test(NAME test_supertux2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND test_supertux2)

  # Prepare coverage with gcov/lcov
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(supertux2_lib PUBLIC -fprofile-arcs -ftest-coverage)
      target_link_options(supertux2_lib PUBLIC -lgcov --coverage)

      add_custom_target(coverage_clear_unix
        COMMAND find . -name "*.gcda" | xargs rm || true # FIXME: Make this platform-neutral
        COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/coverage.info
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/coverage)

      add_custom_target(coverage_unix
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target coverage_clear_unix
        COMMAND ${CMAKE_CTEST_COMMAND}
        COMMAND lcov --capture --directory ${CMAKE_CURRENT_BINARY_DIR} --exclude '/usr/*' --exclude '*/tests/*' --exclude '*/external/*' --output-file ${CMAKE_CURRENT_BINARY_DIR}/coverage.info
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/coverage
        COMMAND genhtml ${CMAKE_CURRENT_BINARY_DIR}/coverage.info -o ${CMAKE_CURRENT_BINARY_DIR}/coverage -t "${SUPERTUX_VERSION_STRING}")
    else()
      message(WARNING "Coverage for non-GNU compilers has yet to be implemented")
    endif()
  else()
    message(WARNING "Building tests in a non-Debug build will NOT make files for coverage! Change CMAKE_BUILD_TYPE to Debug to prepare for coverage analysis.")
  endif()
endif()

# EOF #
