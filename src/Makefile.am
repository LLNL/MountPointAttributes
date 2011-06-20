## $Header: $
##
## Makefile.am -- Process this file with automake to produce Makefile.in 
##
##
##--------------------------------------------------------------------------------
## Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
## the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
## LLNL-CODE-xxxxxx. All rights reserved.
##--------------------------------------------------------------------------------
##
##  Update Log:
##        May 23 2011 DHA: File created.
##

lib_LTLIBRARIES              = libmpattr.la 

INCLUDES                     = -I$(top_srcdir)

include_HEADERS              = MountPointAttrUri.h \
			       MountPointAttr.h \
  			       FgfsCommon.h

libmpattr_la_SOURCES         = MountPointAttr.C 

libmpattr_la_CFLAGS          = $(AM_CFLAGS)
libmpattr_la_CXXFLAGS        = $(AM_CXXFLAGS) 

libmpattr_la_LDFLAGS         = $(AM_LDFLAGS) \
			       -version-info @MPA_CURRENT@:@MPA_REVISION@:@MPA_AGE@

