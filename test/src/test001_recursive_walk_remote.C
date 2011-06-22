/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * All rights reserved.
 *
 * Update Log:
 *        May 27 2011 DHA: Added config.h support
 *        May 23 2011 DHA: File created.
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

using namespace FastGlobalFileStat;
using namespace FastGlobalFileStat::MountPointAttribute;

extern int 
recursiveCheck(MountPointInfo &mpInfo, const char * dirname, bool remote); 


int 
main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr, "Usage: test root_dir_served_by_remote_server\n");
        exit(1);
    }
  
    if (getenv("MPA_TEST_ENABLE_VERBOSE")) {
        MPA_registerMsgFd(stdout, 2);
    }

    MountPointInfo mpInfo(true);
    if (!IS_YES(mpInfo.isParsed())) {
        MPA_sayMessage("Unit Test", 
                       true, 
                       "MountPointInfo failed to parse");
        exit(1);
    }

    if (recursiveCheck(mpInfo, argv[1], true) == 0) {
        MPA_sayMessage("Unit Test", 
                       false, 
                       "PASS");
    }
    else {
        MPA_sayMessage("Unit Test", 
                       false, 
                       "FAILURE");
    }

    return EXIT_SUCCESS;
}

