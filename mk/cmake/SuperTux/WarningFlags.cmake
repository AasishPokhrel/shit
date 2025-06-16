option(WERROR "Stop on first compiler warning" OFF)
option(WARNINGS "Enable long list of warnings for compiler to check" OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Debug flags")
  set(CMAKE_CXX_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  set(CMAKE_C_FLAGS_PROFILE "-pg" CACHE STRING "Profile flags")
  set(CMAKE_LD_FLAGS_PROFILE "-lgmon" CACHE STRING "Profile flags")

  if(WERROR)
    string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS "-Werror ")
  endif()

  if(WARNINGS)
    # temporarily disabled:
    #   -Wsign-conversion -Wfloat-equal -Wundef -Wswitch-default
    #   -Wswitch-enum -Wsign-promo -Wcast-qual -Woverloaded-virtual -Wmissing-format-attribute
    #   -Wpadded -Wabi -Winline -Wunsafe-loop-optimizations -Wstrict-overflow=5
    # fails on MinGW:
    #   -ansi
    if(CMAKE_COMPILER_IS_GNUCXX)
      string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
        "-Wall "
        "-Wextra "
        "-fdiagnostics-show-option "
        "-pedantic "
        "-Wno-long-long "
        "-Wcast-align "
        "-Wdisabled-optimization "
        "-Winit-self -Winvalid-pch "
        "-Wmissing-include-dirs "
        "-Wmissing-noreturn "
        "-Wpacked -Wredundant-decls "
        "-Wformat=2 "
        "-Weffc++ "
        "-Wctor-dtor-privacy "
        "-Wno-unused-parameter "
        "-Wshadow "
        "-Wnon-virtual-dtor "
        "-Wcast-qual "
        "-Wold-style-cast "
        "-Wzero-as-null-pointer-constant "
        "-Wconversion "
        "-Wlogical-op "
        "-Wstrict-null-sentinel "
        "-Wsuggest-override "
        )
      if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
        string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
          "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
          "-Wint-in-bool-context "
          )
      endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
        "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
        "-Weverything "

        # flags that we deliberately ignore
        "-Wno-unused-parameter "
        "-Wno-unused-template "
        "-Wno-c++98-compat "
        "-Wno-c++98-compat-pedantic "
        "-Wno-float-equal "
        "-Wno-padded "
        "-Wno-weak-vtables "
        "-Wno-disabled-macro-expansion "
        "-Wno-documentation "
        "-Wno-reserved-id-macro "
        "-Wno-sign-conversion "
        "-Wno-reserved-identifier "
        "-Wno-unknown-warning-option "

        # warnings that should probably be fixed in code
        "-Wno-documentation-unknown-command "
        "-Wno-inconsistent-missing-destructor-override "
        "-Wno-deprecated-dynamic-exception-spec "
        "-Wno-deprecated "
        "-Wno-switch-enum "
        "-Wno-covered-switch-default "
        "-Wno-exit-time-destructors "
        "-Wno-global-constructors "
        "-Wno-duplicate-enum "
        "-Wno-unreachable-code "
      )

      # This snipped adapted from AOMediaCodec cmakefile
      # The detection of cross compilation by -Wpoison-system-directories has false positives on macOS because
      # --sysroot is implicitly added. Turn the warning off.
      if(NOT DEFINED HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
          check_cxx_compiler_flag(-Wpoison-system-directories HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
      endif()
      if(HAVE_POISON_SYSTEM_DIRECTORIES_WARNING)
        string(CONCAT SUPERTUX2_EXTRA_WARNING_FLAGS
          "${SUPERTUX2_EXTRA_WARNING_FLAGS} "
          "-Wno-poison-system-directories ")
      endif()
    endif()
  endif()
endif()

# EOF #
