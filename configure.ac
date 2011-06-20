dnl $Header: $
dnl
dnl configure.ac
dnl
dnl --------------------------------------------------------------------------------
dnl Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
dnl the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
dnl LLNL-CODE-xxxxxx. All rights reserved. 
dnl
dnl   Update Log:
dnl         May 23 2011 DHA: File created.
dnl                          

AC_PREREQ(2.59)
AC_INIT(MountPointAttr, 1.0, ahn1@llnl.gov)
dnl Version numbers, remember to change them just *before* a release. 
dnl (Interfaces removed: CURRENT++, AGE=0, REVISION=0)
dnl (Interfaces added: CURRENT++, AGE++, REVISION=0)
dnl (No interfaces changed: REVISION++) 
MPA_CURRENT=2
MPA_REVISION=1
MPA_AGE=1
AC_SUBST(MPA_CURRENT)
AC_SUBST(MPA_REVISION)
AC_SUBST(MPA_AGE)
AC_CANONICAL_SYSTEM
AC_CONFIG_AUX_DIR([config]) 
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(MountPointAttr, 1.0)


dnl -----------------------------------------------
dnl enable debug support
dnl -----------------------------------------------
X_AC_ENABLE_DEBUG


dnl -----------------------------------------------
dnl Checks for programs.
dnl -----------------------------------------------
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CC_C_O


dnl -----------------------------------------------
dnl Checks for libraries. 
dnl -----------------------------------------------
dnl here


dnl -----------------------------------------------
dnl Checks for header files. 
dnl -----------------------------------------------
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([map iostream string sstream stdexcept])
AC_LANG_POP([C++])
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h mntent.h string.h libgen.h dirent.h netdb.h])


dnl -----------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics.
dnl -----------------------------------------------
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T


dnl -----------------------------------------------
dnl Checks for library functions.
dnl -----------------------------------------------
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([access gethostname strdup opendir readdir lstat gethostbyname gethostname])


dnl -----------------------------------------------
dnl configure's OUTPUTS 
dnl -----------------------------------------------
AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile
                 test/Makefile
                 test/src/Makefile])

AC_OUTPUT
