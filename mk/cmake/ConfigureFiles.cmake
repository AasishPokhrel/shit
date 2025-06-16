# Copy files from source directory to destination directory, substituting any
# variables.  Create destination directory if it does not exist.

macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${srcDir} ${destDir})
endmacro(configure_files)
