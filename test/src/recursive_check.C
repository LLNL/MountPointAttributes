/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * All rights reserved.
 *
 * Update Log:
 *        May 27 2011 DHA: Added config.h support
 *        May 24 2011 DHA: File created.
 *
 */

#ifndef HAVE_MOUNTPOINTATTR_H
# include "config.h"
# define HAVE_MOUNTPOINTATTR_H 1
#endif

extern "C" {
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
}

#include <string>
#include "MountPointAttr.h"

using namespace FastGlobalFileStat;

int 
recursiveCheck(MountPointInfo &mpInfo, const char * dirname, bool remote) 
{
    DIR *dp;
    struct dirent *ep;
    int rc = 0;

    if (!dirname || dirname[0] != '/') {
        MPA_sayMessage("Unit Test", 
                       true, 
                       "Not an absolute path");
        return -1;
    } 

    char abs_filename[FILENAME_MAX];
    dp = opendir (dirname);
    if (dp != NULL) {
        while (ep = readdir (dp)) {
            struct stat stFileInfo;

            size_t len = strlen(dirname);
            if ((len < PATH_MAX) && dirname[len-1] != '/') {
                snprintf(abs_filename, FILENAME_MAX, "%s/%s", dirname, ep->d_name);
            }
            else {
                snprintf(abs_filename, FILENAME_MAX, "%s%s", dirname, ep->d_name);
            }

            if (lstat(abs_filename, &stFileInfo) < 0) {
                MPA_sayMessage("Unit Test", 
                               true, 
                               "lstat returns negative: %s", abs_filename);
                continue;
            }

            if (S_ISLNK(stFileInfo.st_mode)) {
                continue;
            }

            if(S_ISDIR(stFileInfo.st_mode)) {
                if(strcmp(ep->d_name, ".") 
                   && strcmp(ep->d_name, "..")) {
                    rc += recursiveCheck(mpInfo, abs_filename, remote);
                }
            } 
            else {
                  MyMntEnt anEntry;
                  if (remote) {
                      if (!IS_YES(mpInfo.isRemoteFileSystem(abs_filename, anEntry))) {
                          if (ChkVerbose(1)) {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "isRemoteFileSystem returned No or Error: %s", 
                                             abs_filename);
                          } 
                          rc += -1;
                      }
                  }
                  else {
                      if (!IS_YES(mpInfo.isLocalDevice(abs_filename, anEntry))) {
                          if (ChkVerbose(1)) {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "isLocalDevice returned No or Error: %s", 
                                             abs_filename);
                          } 
                          rc += -1;
                      }
                  }
                  if (ChkVerbose(1)) {
                      FileUriInfo uri; 
                      const char *errStr;
                      errStr = mpInfo.getFileUriInfo(abs_filename, uri);
                      if (!errStr) {
                          std::string uriStr;
                          if (uri.getUri(uriStr)) {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "--------------------------------------");
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "%s => %s", abs_filename, uriStr.c_str());
                          }
                          else {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "getUri returned false");
                          }
                      }
                      else {
                          MPA_sayMessage("Unit Test", 
                                         false, 
                                         "getFileUriInfo returned an error string");
                      }
                  }
            }
        }
        closedir (dp);
    }
    else {
        MPA_sayMessage("Unit Test", 
                       false, 
                       "Couldn't open %s", dirname);
        MPA_sayMessage("Unit Test", 
                       false, 
                       "Continue...");
    }

    return rc;
}

