AC_PREREQ([2.59])
AC_INIT([Keetchi], [0.1], [adu@comnets.uni-bremen.de])
# Remove foreign to create a gnu compliant package (including some more files)
AM_INIT_AUTOMAKE(foreign -Wall -Werror)
AC_CONFIG_MACRO_DIR([m4])

#Doxygen configuration
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$(PACKAGE_NAME)], [Doxyfile], [doxygen-doc])

AM_PROG_AR
# C++ app
AC_PROG_CXX
# We need some C
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
# Some project specific data to be used in your app
AC_CONFIG_HEADERS([config.h])
# Files handles and processes by autotools
AC_CONFIG_FILES([Makefile src/testapp/Makefile lib/Makefile Doxyfile])

AC_FUNC_MALLOC
AC_OUTPUT
