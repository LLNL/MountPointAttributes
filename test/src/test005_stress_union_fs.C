/*
 * -------------------------------------------------------------------------------- 
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>. 
 * LLNL-CODE-490173. All rights reserved.
 * 
 * This file is part of MountPointAttributes. For details, 
 * see https://computing.llnl.gov/?set=resources&page=os_projects
 * 
 * Please also read LICENSE - Our Notice and GNU Lesser General Public License.
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License (as published by the Free Software
 * Foundation) version 2.1 dated February 1999.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 * --------------------------------------------------------------------------------
 *
 * Update Log:
 *        May 27 2011 DHA: File created.
 *
 */

#ifndef HAVE_MOUNTPOINTATTR_H
# include "config.h"
# define HAVE_MOUNTPOINTATTR_H 1
#endif

extern "C" {
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
}

#include <map>
#include <string>
#include "MountPointAttr.h"

using namespace FastGlobalFileStat;
using namespace FastGlobalFileStat::MountPointAttribute;

int 
recursiveCheckForAufs(MountPointInfo &mpInfo, MyMntEnt &aufsEntry, const char * dirname, bool remote) 
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
 
    MyMntEnt result;
    const char *errStr 
        = mpInfo.getMntPntInfo((const char *) dirname, result); 

    if (errStr || (aufsEntry != result)) {
        if (ChkVerbose(1)) {
            MPA_sayMessage("Unit Test", 
                false, 
                "%s is not on the target aufs mountpoint.", 
                dirname);
        } 
        return 0;
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
                return 0;
            }
            if (S_ISLNK(stFileInfo.st_mode)) {
                return 0;
            }

            if(S_ISDIR(stFileInfo.st_mode)) {
                if(strcmp(ep->d_name, ".") 
                   && strcmp(ep->d_name, "..")) {
                    rc += recursiveCheckForAufs(mpInfo, aufsEntry, abs_filename, remote);
                }
            } 
            else {
                  MyMntEnt anEntry;
                  std::string myPath;
                  if (remote) {
                      //
                      // All the files that are characterized as remote should
                      // exist remotely in the readonly branch.
                      //
                      if (IS_YES(mpInfo.isRemoteFileSystem(abs_filename, anEntry))) {
                          FileUriInfo myUri;
                          mpInfo.getFileUriInfo(abs_filename, myUri);
                          myPath = anEntry.dir_branch 
                                       + std::string("/")
                                       + myUri.pathFromExportDir; 
                                         
                          if (access(myPath.c_str(), F_OK) < 0) {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "%s doesn't exist", myPath.c_str());
                              rc = -1;
                          } 
                      }
                  }
                  else {
                      //
                      // This test would probably be more important.
                      // all the files that are characterized as local should
                      // exist locally in the readwrite branch.
                      //
                      if (IS_YES(mpInfo.isLocalDevice(abs_filename, anEntry))) {
                          FileUriInfo myUri;
                          mpInfo.getFileUriInfo(abs_filename, myUri);
                          myPath = anEntry.dir_branch 
                                       + std::string("/")
                                       + myUri.pathFromExportDir; 
                          if (access(myPath.c_str(), F_OK) < 0) {
                              MPA_sayMessage("Unit Test", 
                                             false, 
                                             "%s doesn't exist", myPath.c_str());
                              rc = -1;
                          } 
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
        rc = 0; 
    }

    return rc;
}



int 
main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Usage: test \n");
        exit(1);
    }

    if (getenv("MPA_TEST_ENABLE_VERBOSE")) {
        MPA_registerMsgFd(stdout, 2);
    }

    MountPointInfo mpInfo;
    const char *errStr = mpInfo.parse();
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "parse method returns an error %s.", errStr);
        exit(1);
    }

    const std::map<std::string, MyMntEnt> mpInfoMap 
        = mpInfo.getMntPntMap();

    std::map<std::string, MyMntEnt>::const_iterator iter;  
    bool found = false;
    MyMntEnt aufsEntry; 
    for (iter = mpInfoMap.begin(); iter != mpInfoMap.end(); iter++) {
        if (mpInfo.determineFSType(iter->second.type) == fs_aufs) {
            aufsEntry = iter->second;
            found = true;
            break;
        }
    }

    if (!found) {
        MPA_sayMessage("Unit Test", 
            false, 
            "SKIP: no union fs mount exists on the system.");
        exit(1);

    }

    int rc = recursiveCheckForAufs(mpInfo, 
                                   aufsEntry, 
                                   aufsEntry.dir_master.c_str(), 
                                   true);
    if (rc < 0) {
        MPA_sayMessage("Unit Test", 
            true, 
            "union fs walk for remote failed .");
        exit(1);
    }  

    rc = recursiveCheckForAufs(mpInfo, 
                               aufsEntry, 
                               aufsEntry.dir_master.c_str(), 
                               false);
    if (rc < 0) {
        MPA_sayMessage("Unit Test", 
            true, 
            "union fs walk for local failed .");
        exit(1);
    }  

    MPA_sayMessage("Unit Test", false, "PASS");

    return EXIT_SUCCESS;
}

