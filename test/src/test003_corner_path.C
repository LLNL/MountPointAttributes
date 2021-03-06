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
 *        May 27 2011 DHA: Added config.h support
 *        May 25 2011 DHA: File created.
 *
 */

#ifndef HAVE_MOUNTPOINTATTR_H
# include "config.h"
# define HAVE_MOUNTPOINTATTR_H 1
#endif

extern "C" {
#include <stdio.h>
#include <stdlib.h>
}
#include "MountPointAttr.h"

using namespace FastGlobalFileStatus;
using namespace FastGlobalFileStatus::MountPointAttribute;

const char nonexistence[] = "/nonexistence/nonexistence/nonexistence"; 
const char invalidpath[] = "./invalid";

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
    // Test with invalid path 
    //
    MyMntEnt anEntry;
    if (!IS_ERROR(mpInfo.isRemoteFileSystem(invalidpath, anEntry))) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isRemoteFileSystem method doesn't return an error on a relative path.");
        exit(1);
    }
    if (!IS_ERROR(mpInfo.isLocalDevice(invalidpath, anEntry))) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isLocalDevice method doesn't return an error on an invalid path.");
        exit(1);
    }

    FileUriInfo uri; 
    errStr = mpInfo.getFileUriInfo(invalidpath, uri);
    if (!errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getFileUriInfo method doesn't return an error on an invalid path.");
        exit(1);
    }
    if (ChkVerbose(1)) {
        MPA_sayMessage("Unit Test", false, "%s", errStr);
    }

    errStr = mpInfo.getMntPntInfo(invalidpath, anEntry);
    if (!errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getMntPntInfo method doesn't return an error on a nonexistent path.");
        exit(1);
    }

    //
    // Test with nonexistent path 
    //
    FGFSInfoAnswer ans = mpInfo.isRemoteFileSystem(nonexistence, anEntry);
    if (IS_ERROR(ans)) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isRemoteFileSystem method returns an error on an absolute path.");
        exit(1);
    }
    if (anEntry.dir_master != std::string("/")) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isRemoteFileSystem method returns nonroot as the resulting mount point.");
        exit(1);
    }

    errStr = mpInfo.getFileUriInfo(nonexistence, uri);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getFileUriInfo method returns an error on an absolute path.");
        exit(1);
    }
    if (ChkVerbose(1)) {
        std::string uriStr;
        uri.getUri(uriStr);
        MPA_sayMessage("Unit Test", false, "%s", uriStr.c_str());
    }

    errStr = mpInfo.getMntPntInfo(nonexistence, anEntry);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getMntPntInfo method returns an error on an absolute path.");
        exit(1);
    }

    //
    // Test with an existence path after some failures
    //
    char *homepath = getenv("HOME");
    if (!getenv("HOME")) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: HOME envVar not set.");
        exit(1);
    }
    if (IS_ERROR(mpInfo.isRemoteFileSystem(homepath, anEntry))) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isRemoteFileSystem method return ERROR on the home dir.");
        exit(1);
    }
    if (IS_ERROR(mpInfo.isLocalDevice(homepath, anEntry))) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: isLocalDevice method returns ERROR on the home dir.");
        exit(1);
    }
    errStr = mpInfo.getMntPntInfo(homepath, anEntry);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getMntPntInfo method returns an error on the home dir.");
        exit(1);
    }
        
    errStr = mpInfo.getFileUriInfo(homepath, uri);
    if (errStr) {
        MPA_sayMessage("Unit Test", 
            true, 
            "FAILURE: getFileUriInfo method returns an error on the home dir.");
        exit(1);
    }
    if (ChkVerbose(1)) {
        std::string uriStr;
        uri.getUri(uriStr);
        MPA_sayMessage("Unit Test", false, "%s => %s", homepath, uriStr.c_str());
    }

    MPA_sayMessage("Unit Test", false, "PASS");

    return EXIT_SUCCESS;
}

