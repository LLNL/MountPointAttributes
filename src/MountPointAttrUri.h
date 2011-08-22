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
 *        May 23 2011 DHA: Moved internal data structure from 
 *                         MountPointAttr.h.
 *
 */

#ifndef MOUNT_POINT_ATTR_URI_H
#define MOUNT_POINT_ATTR_URI_H 1

#include <string>

namespace FastGlobalFileStat {

  namespace MountPointAttribute {

    /**
     *   Defines a simple date type to support Union mount.
     *
     *   A union mount overlays multiple file systems to provide a coherent
     *   file system. A typical technique would involve checking attributes
     *   such as readonly vs. read/write of each and all of the branches 
     *   that are stacked up together through the union file system to make
     *   an intelligent decision.
     */
    struct UnionMountBranch {
        std::string um_branch; /*!< a branch path of the union FS */
        std::string um_perm;   /*!< permission of this branch */
    };


    /**
     *   Defines a base data type from which various URI schemes 
     *   derives. Using a class hierarchy to ease extending the 
     *   system for a new file system. 
     */
    class UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const = 0;
    };


    /**
     *   Defines a derived UriScheme data type for NFS
     *   From RFC2224 (NFS URL Scheme)
     */
    class NfsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;

    };


    /**
     *   Defines a derived UriScheme data type for Lustre 
     *   This hasn't standardized. Until then, we use an ad-hoc scheme.
     */
    class LustreUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for GPFS
     *   This hasn't standardized. Until then, we use an ad-hoc scheme.
     */
    class GpfsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for Panfs
     */
    class PanfsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for Plfs
     */
    class PlfsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for Dvs
     */
    class DvsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for Cifs (Not tested yet)
     */
    class CifsUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for Lustre (Not tested yet)
     */
    class SmbUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;
    };


    /**
     *   Defines a derived UriScheme data type for local files
     *   From RFC1738 (Uniform Resource Locators (URL))
     */
    class LocalUriScheme : public UriScheme {
        public:
            virtual void getUri(const std::string &hostAddr,
                                const std::string &exportDir,
                                const std::string &pathFromExportDir,
                                const std::string &mountPoint,
                                std::string &uri) const;

    };


    /**
     *   Compare the given level to the current verbose level. The function 
     *   should be internal to the Fast Global File Stat project.
     */
    extern int verboseLevel;
    inline bool ChkVerbose(int level)
    {
        return (level <= verboseLevel)? true: false;
    }


    /**
     *   Prints a error/info message to a stream. The function should be
     *   internal to the Fast Global File Stat project.
     */
    void MPA_sayMessage(const char *m, bool b, const char *output, ...);


    /**
     *   Wraps around gethostname to deal with some odd systems. 
     */
    int MPA_getLocalNodeName(char *name, size_t len);

  } // MountPointAttribute namespace

} // FastGlobalFileStat namespace

#endif // MOUNT_POINT_ATTR_URI_H
