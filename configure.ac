# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(vsi, 0.1, https://bitbucket.org/matrim/vsi/issues)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/training/src/main.cpp])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h syslog.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday localtime_r memset mkdir strerror strstr])

#AC_OUTPUT
AC_OUTPUT(Makefile \
		src/Makefile \
		src/training/src/Makefile )