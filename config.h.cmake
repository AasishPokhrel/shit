#ifndef CONFIG_H
#define CONFIG_H

#define PACKAGE_NAME "supertux2"

#cmakedefine ENABLE_SQDBG

#define INSTALL_SUBDIR_BIN "${INSTALL_SUBDIR_BIN}"
#define INSTALL_SUBDIR_SHARE "${INSTALL_SUBDIR_SHARE}"

#cmakedefine HAVE_ICONV_CONST
#ifdef HAVE_ICONV_CONST
#define ICONV_CONST const
#else
#define ICONV_CONST
#endif

#cmakedefine WORDS_BIGENDIAN

#cmakedefine HAVE_OPENGL

#cmakedefine HAVE_LIBCURL

#define BUILD_DATA_DIR "${BUILD_DATA_DIR}"

#define BUILD_CONFIG_DATA_DIR "${BUILD_CONFIG_DATA_DIR}"

#cmakedefine ENABLE_DISCORD
#cmakedefine STEAM_BUILD

#cmakedefine UBUNTU_TOUCH
#cmakedefine HIDE_NONMOBILE_OPTIONS

#cmakedefine REMOVE_QUIT_BUTTON

#endif /*CONFIG_H*/
