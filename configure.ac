dnl $Header: $
dnl
dnl configure.ac
dnl
dnl -------------------------------------------------------------------------------- 
dnl Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
dnl the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>. 
dnl LLNL-CODE-490173. All rights reserved.
dnl 
dnl This file is part of MountPointAttributes. For details, 
dnl see https://computing.llnl.gov/?set=resources&page=os_projects
dnl 
dnl Please also read LICENSE - Our Notice and GNU Lesser General Public License.
dnl 
dnl This program is free software; you can redistribute it and/or modify it under 
dnl the terms of the GNU General Public License (as published by the Free Software
dnl Foundation) version 2.1 dated February 1999.
dnl 
dnl This program is distributed in the hope that it will be useful, but WITHOUT ANY
dnl WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU
dnl General Public License for more details.
dnl 
dnl You should have received a copy of the GNU Lesser General Public License along
dnl with this program; if not, write to the Free Software Foundation, Inc., 59 Temple
dnl Place, Suite 330, Boston, MA 02111-1307 USA
dnl -------------------------------------------------------------------------------- 
dnl
dnl   Update Log:
dnl         May 23 2011 DHA: File created.
dnl                          

AC_PREREQ(2.59)
AC_INIT(MountPointAttr, 1.1, ahn1@llnl.gov)
dnl Version numbers, remember to change them just *before* a release. 
dnl (Interfaces removed: CURRENT++, AGE=0, REVISION=0)
dnl (Interfaces added: CURRENT++, AGE++, REVISION=0)
dnl (No interfaces changed: REVISION++) 
MPA_CURRENT=2
MPA_REVISION=0
MPA_AGE=1
AC_SUBST(MPA_CURRENT)
AC_SUBST(MPA_REVISION)
AC_SUBST(MPA_AGE)
AC_CONFIG_AUX_DIR([config]) 
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(MountPointAttr, 1.1)
AM_MAINTAINER_MODE([enable])
AC_CANONICAL_HOST


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
