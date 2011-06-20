/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * All rights reserved.
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
#include <limits.h>
#include <string.h>
#include <errno.h>
}
#include "MountPointAttr.h"

using namespace FastGlobalFileStat;

const char testTextName[] = "test.txt"; 
const char testTextLinkName[] = "test.txt.hardlink"; 

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

    //
    // Test with a valid hardlink
    //
    char cwdPath[PATH_MAX];
    char testTextPath[PATH_MAX];
    char testTextLinkPath[PATH_MAX];
    if (!(getcwd(cwdPath, PATH_MAX))) {
        MPA_sayMessage("Unit Test", 
            true, 
            "getcwd failed %s.", strerror(errno));
        exit(1);
    }
    snprintf(testTextLinkPath, PATH_MAX, 
             "%s/%s", cwdPath, testTextLinkName);
    snprintf(testTextPath, PATH_MAX, 
             "%s/%s", cwdPath, testTextName);

    remove(testTextLinkName);
    errno = 0;
    
    if ( link(testTextPath, testTextLinkName) < 0 ) {
        MPA_sayMessage("Unit Test", 
            true, 
            "link can't create a hardlink: %s.", strerror(errno));
        exit(1);
    }

    MyMntEnt entry1, entry2;
    errStr = mpInfo.getMntPntInfo(testTextPath, entry1);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getMntPntInfo returns an error: %s.", errStr);
        exit(1);
    }
    errStr = mpInfo.getMntPntInfo(testTextLinkPath, entry2);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getMntPntInfo returns an error: %s.", errStr);
        exit(1);
    }
    if ( entry1 != entry2) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: the same file through a hardlink returns different mount point info!");
        exit(1);
    }

    if (remove(testTextLinkName) < 0 ) {
        MPA_sayMessage("Unit Test", 
            true, 
            "can't remove the tested hardlink %s", strerror(errno));
        exit(1);
    }

    //
    // Ensure the system disallows a valid hardlink across different mount points
    //
    char *tmpDir = getenv("TMP");
    if (tmpDir) {
        MyMntEnt entry3;
        errStr = mpInfo.getMntPntInfo(tmpDir, entry3);
        if (errStr) {
            MPA_sayMessage("Unit Test", 
                true, 
                "FAILURE: getMntPntInfo returns an error: %s.", errStr);
            exit(1);
        }

        if (entry3 != entry1) {
            char cmd[PATH_MAX];
            char newTestTextPath[PATH_MAX];
            snprintf(newTestTextPath, PATH_MAX, "%s/%s", tmpDir, testTextName);
            remove(newTestTextPath);
            errno = 0;
            snprintf(cmd, PATH_MAX, "cp %s %s", testTextName, newTestTextPath);
            system(cmd);
            if ( link(newTestTextPath, testTextLinkName) == 0 ) {
                MPA_sayMessage("Unit Test", 
                   true, 
                   "link allows a hardlink between different mount points!");
                   exit(1);
            }
            else {
                if (ChkVerbose(1)) {
                    MPA_sayMessage("Unit Test",   
                        false,
                        "link disallows a hardlink between different mount points: %s", 
                        strerror(errno));
                }
            } 
        }
        else {
            MPA_sayMessage("Unit Test", 
                false, 
                "$TMP is in the same mount point as $CWD; skip testing a hardlink on multiple mountpoints");
        }
    }
    else {
        MPA_sayMessage("Unit Test", 
            false, 
            "TMP envVar isn't set; skip testing a hardlink on multiple mountpoints");
    }

    MPA_sayMessage("Unit Test", false, "PASS");

    return EXIT_SUCCESS;
}

