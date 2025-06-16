if(WIN32 AND NOT UNIX)
  install(DIRECTORY $<TARGET_FILE_DIR:supertux2_lib>/
          DESTINATION ${INSTALL_SUBDIR_BIN}
          FILES_MATCHING PATTERN "*.dll")

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png
                ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.ico
          DESTINATION ".")

  # Install PDB files for use with the error handler.
  install(FILES $<TARGET_PDB_FILE:supertux2>
          DESTINATION ${INSTALL_SUBDIR_BIN}
          OPTIONAL)

  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/run_supertux.bat
                ${CMAKE_CURRENT_SOURCE_DIR}/mk/msvc/run_supertux_portable.bat
          DESTINATION ".")

  option(PACKAGE_VCREDIST "Package the VCREDIST libraries with the program" OFF)

  if(PACKAGE_VCREDIST)
    set(CMAKE_INSTALL_UCRT_LIBRARIES true)
    include(InstallRequiredSystemLibraries)
  endif()

else()
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND DISABLE_CPACK_BUNDLING)

    set(INFOPLIST_CFBUNDLEEXECUTABLE "supertux2")

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/info.plist.in ${CMAKE_BINARY_DIR}/tools/darwin/info.plist)
    install(FILES ${CMAKE_BINARY_DIR}/tools/darwin/info.plist DESTINATION "SuperTux.app/Contents/")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tools/darwin/receipt DESTINATION "SuperTux.app/Contents/_MASReceipt/")

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.icns DESTINATION "SuperTux.app/Contents/Resources/")

  else()

    if(UBUNTU_TOUCH)
      set(LINUX_DESKTOP_ICON "assets/supertux-256x256.png")
      # FIXME: The "install" folder is a folder managed by Clickable and shouldn't be hardcoded here
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/supertux2.desktop.in "install/supertux2.desktop")
      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mk/clickable/supertux2.apparmor DESTINATION ".")
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/clickable/manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/install/manifest.json)
      set(APPS "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/supertux2")
      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux-256x256.png DESTINATION "assets")
    else()
      set(LINUX_DESKTOP_ICON "supertux2")
      configure_file(${CMAKE_CURRENT_SOURCE_DIR}/supertux2.desktop.in "supertux2.desktop")
      install(FILES ${CMAKE_BINARY_DIR}/supertux2.desktop DESTINATION "share/applications")
      set(APPS "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${INSTALL_SUBDIR_BIN}/supertux2")
      install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.png ${CMAKE_CURRENT_SOURCE_DIR}/data/images/engine/icons/supertux.xpm DESTINATION "share/pixmaps/")
    endif()

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/supertux2.svg DESTINATION "share/icons/hicolor/scalable/apps")

  endif()
endif()

if(UBUNTU_TOUCH)
  install(TARGETS supertux2 DESTINATION ".")
else()
  install(TARGETS supertux2 DESTINATION ${INSTALL_SUBDIR_BIN})
endif()

if(EMSCRIPTEN)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/emscripten/template.html.in ${CMAKE_CURRENT_BINARY_DIR}/template.html)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/emscripten/supertux2.png ${CMAKE_CURRENT_BINARY_DIR}/supertux2.png COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/emscripten/supertux2_bkg.png ${CMAKE_CURRENT_BINARY_DIR}/supertux2_bkg.png COPYONLY)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/INSTALL.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt ${CMAKE_CURRENT_SOURCE_DIR}/NEWS.md DESTINATION ${INSTALL_SUBDIR_DOC})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/credits.stxt DESTINATION ${INSTALL_SUBDIR_SHARE})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.supertuxproject.SuperTux.metainfo.xml DESTINATION "share/metainfo" )

install(DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR}/data/images
  ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts
  ${CMAKE_CURRENT_SOURCE_DIR}/data/music
  ${CMAKE_CURRENT_SOURCE_DIR}/data/particles
  ${CMAKE_CURRENT_SOURCE_DIR}/data/scripts
  ${CMAKE_CURRENT_SOURCE_DIR}/data/shader
  ${CMAKE_CURRENT_SOURCE_DIR}/data/speech
  ${CMAKE_CURRENT_SOURCE_DIR}/data/sounds
  ${CMAKE_CURRENT_SOURCE_DIR}/data/locale
  DESTINATION ${INSTALL_SUBDIR_SHARE})

if(CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
    DESTINATION ${INSTALL_SUBDIR_SHARE}
    PATTERN "data/levels/test" EXCLUDE
    PATTERN "data/levels/test_old" EXCLUDE
    PATTERN "data/levels/incubator" EXCLUDE)
else()
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/levels
    DESTINATION ${INSTALL_SUBDIR_SHARE})
endif()

# move some config clutter to the advanced section
mark_as_advanced(
  INSTALL_SUBDIR_BIN
  INSTALL_SUBDIR_SHARE
  INSTALL_SUBDIR_DOC
  )

# EOF #
