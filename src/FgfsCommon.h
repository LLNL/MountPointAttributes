/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * All rights reserved.
 *
 * Update Log:
 *
 *        Jan 19 2011 DHA: File created.
 *
 */

#ifndef FGFS_COMMON_H
#define FGFS_COMMON_H 1

namespace FastGlobalFileStat {

    /**
     *   Customizes a boolean type.
     */
    enum FGFSInfoAnswer {
        ans_yes,
        ans_no,
        ans_error
    };

    inline bool IS_YES(FGFSInfoAnswer answer) {
        return (answer == ans_yes)? true : false;
    }
    inline bool IS_NO(FGFSInfoAnswer answer) {
        return (answer == ans_no)? true : false;
    }
    inline bool IS_ERROR(FGFSInfoAnswer answer) {
        return (answer == ans_error)? true : false;
    }
    inline FGFSInfoAnswer NOT(FGFSInfoAnswer answer) {
        FGFSInfoAnswer r;
        if (answer == ans_yes) {
            r = ans_no;
        }
        else if (answer == ans_no) {
            r = ans_yes;
        }
        else {
            r = answer;
        }
        return r;
    }
}
#endif //FGFS_COMMON_H

