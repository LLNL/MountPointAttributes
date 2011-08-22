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

