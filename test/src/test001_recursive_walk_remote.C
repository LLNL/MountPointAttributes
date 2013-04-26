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
 *        May 26 2013 DHA: Checked memory correctness 
 *                         (Both Valgrind and TotalView memScape.)
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

using namespace FastGlobalFileStatus;
using namespace FastGlobalFileStatus::MountPointAttribute;

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

