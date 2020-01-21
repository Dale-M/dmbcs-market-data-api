#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dmbcs-market-data-api], [1.0], [https://rdmp.org/dmbcs/contact])
AM_INIT_AUTOMAKE([silent-rules subdir-objects])
AM_SILENT_RULES([yes])
LT_INIT
AC_CONFIG_SRCDIR([src/dmbcs-market-data-api.cc])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES([third_party], [curlpp openssl])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD

AC_CONFIG_FILES([makefile
                 src/makefile
                 dmbcs-market-data-api.pc])
AC_OUTPUT
