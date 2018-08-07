## $Header: $
##
## Makefile.am -- Process this file with automake to produce Makefile.in 
##
## -------------------------------------------------------------------------------- 
## Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
## the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>. 
## LLNL-CODE-490173. All rights reserved.
## 
## This file is part of MountPointAttributes. 
## For details, see https://computing.llnl.gov/?set=resources&page=os_projects
## 
## Please also read LICENSE - Our Notice and GNU Lesser General Public License.
## 
## This program is free software; you can redistribute it and/or modify it under 
## the terms of the GNU General Public License (as published by the Free Software
## Foundation) version 2.1 dated February 1999.
## 
## This program is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU Lesser General Public License along
## with this program; if not, write to the Free Software Foundation, Inc., 59 Temple
## Place, Suite 330, Boston, MA 02111-1307 USA
## -------------------------------------------------------------------------------- 
##
##  Update Log:
##        May 27 2011 DHA: Added test004_hardlink_path and test005_stress_union_fs
##        May 25 2011 DHA: Added test003_corner_path
##        May 24 2011 DHA: Added test002_recursive_walk_local
##        May 23 2011 DHA: Added test001_recursive_walk_remote
##        May 23 2011 DHA: File created.
##

AM_CPPFLAGS                              = -I$(top_srcdir)/src

testdir                                  = ${pkgdatadir}/tests
test_PROGRAMS                            = test001_recursive_walk_remote \
					   test002_recursive_walk_local \
					   test003_corner_path \
					   test004_hardlink_path \
					   test005_stress_union_fs

test_SCRIPTS                             = test.txt

##
## TEST001
##
test001_recursive_walk_remote_SOURCES    = test001_recursive_walk_remote.C \
				           recursive_check.C
test001_recursive_walk_remote_CFLAGS     = $(AM_CFLAGS) 
test001_recursive_walk_remote_CXXFLAGS   = $(AM_CXXFLAGS) 
test001_recursive_walk_remote_LDFLAGS    = -L../../src
test001_recursive_walk_remote_LDADD      = -lmpattr


#
# TEST002 
#
test002_recursive_walk_local_SOURCES     = test002_recursive_walk_local.C \
				           recursive_check.C
test002_recursive_walk_local_CFLAGS      = $(AM_CFLAGS) 
test002_recursive_walk_local_CXXFLAGS    = $(AM_CXXFLAGS) 
test002_recursive_walk_local_LDFLAGS     = -L../../src
test002_recursive_walk_local_LDADD       = -lmpattr


#
# TEST003 
#
test003_corner_path_SOURCES              = test003_corner_path.C 
test003_corner_path_CFLAGS               = $(AM_CFLAGS) 
test003_corner_path_CXXFLAGS             = $(AM_CXXFLAGS) 
test003_corner_path_LDFLAGS              = -L../../src
test003_corner_path_LDADD                = -lmpattr


#
# TEST004 
#
test004_hardlink_path_SOURCES            = test004_hardlink_path.C 
test004_hardlink_path_CFLAGS             = $(AM_CFLAGS) 
test004_hardlink_path_CXXFLAGS           = $(AM_CXXFLAGS) 
test004_hardlink_path_LDFLAGS            = -L../../src
test004_hardlink_path_LDADD              = -lmpattr


#
# TEST005 
#
test005_stress_union_fs_SOURCES            = test005_stress_union_fs.C 
test005_stress_union_fs_CFLAGS             = $(AM_CFLAGS) 
test005_stress_union_fs_CXXFLAGS           = $(AM_CXXFLAGS) 
test005_stress_union_fs_LDFLAGS            = -L../../src
test005_stress_union_fs_LDADD              = -lmpattr

EXTRA_DIST                                = test.txt

