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
 *        May 27 2011 DHA: Added MyMntEnt::operator== and operator!=
 *        May 23 2011 DHA: Added doxygen doxygen.
 *        May 23 2011 DHA: Moved internal data structure 
 *                         to MountPointAttrInternal.h.
 *        May 13 2011 DHA: Added debug print support. 
 *        Jan 10 2011 DHA: Added Doxygen documentation support.
 *        Jan 07 2011 DHA: Logic to handle union mount file system;
 *                         specifically AUFS.
 *        Jan 04 2011 DHA: File created.
 *
 */

#ifndef MOUNT_POINT_ATTR_H
#define MOUNT_POINT_ATTR_H 1

extern "C" {
#include <mntent.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
}

#include <string>
#include <map>
#include <vector>
#include "FgfsCommon.h"
#include "MountPointAttrUri.h"

namespace FastGlobalFileStatus {

    /**
     @mainpage Mount Point Attributes Module for Fast Global File Status (FGFS)
     @author Dong H. Ahn, Development Environment Group, Livermore Computing (LC) Division, LLNL

     @section intro Introduction

     Large-scale systems typically mount many different file systems with
     distinct performance characteristics and capacity. High performance computing 
     applications must efficiently use this storage in order to realize their full performance
     potential. Users must take into account potential file replication throughout
     the storage hierarchy as well as contention in lower levels of the 
     I/O system, and must consider communicating the results of file I/O 
     between application processes to reduce file system accesses.
     Addressing these issues and optimizing file accesses requires detailed 
     run-time knowledge of file system performance characteristics and 
     the location(s) of files on them.
     
     We developed Fast Global File Status (FGFS) to provide a scalable 
     mechanism to retrieve such information of a file, including its degree of 
     distribution or replication and consistency. FGFS uses a novel node-local 
     technique that turns expensive, non-scalable file system calls into 
     simple string comparison operations. FGFS raises the namespace of a 
     locally-defined file path to a global namespace with little or no file 
     system calls to obtain global file properties efficiently. Our evaluation 
     on a large multi-physics application showed that most FGFS file status queries on 
     its executable and 848 shared library files complete in 272 milliseconds
     or faster at 32,768 MPI processes. Even the most expensive operation, 
     which checks global file consistency, completes in under 7 seconds at this 
     scale, an improvement of several orders of magnitude 
     over the traditional checksum technique.

     The main abstractions that enable raising the namespace of local file names
     are packaged up into the \c MountPointAttributes module.

     @section fnresol File Name Resolution Engine 
     The core technique of \c MountPointAttributes is a scalable mechanism to 
     raise the local namespace of a file to a global namespace. 
     The global namespace enables fast comparisons of local 
     file properties across distributed machines with little or
     no access requirement on the underlying file systems. 
     More specifically, the file name resolution engine of \c MountPointAttributes 
     turns a local file path into a Uniform Resource Identifier (URI), 
     a globally unique identifier of the file. This resolution 
     process is merely a memory operation, as our technique 
     builds an URI through the file system mount point table, 
     which is available in system memory. Thus,
     this core logic requires no communication and can scale well.

     The \c MountPointInfo class is the main data type 
     associated with the file name resolution process. For example,
     the following code snippet stores \c filepath's URI into 
     \c uriString like "nfs://dip-nfs.llnl.gov:/vol/g0/joe/readme"
     through a FileUriInfo object.

     @verbatim
         #include "MountPointAttr.h"
         using namespace FastGlobalFileStatus;
         ...

         MountPointInfo mpInfo(true);
         if (!IS_YES(mpInfo.isParsed())) 
            return;
            
         FileUriInfo uriInfo;
         std::string uriString;
         const char *filepath = "/g/g0/joe/readme";
         mpInfo.getFileUriInfo(filepath, uriInfo);

         // getUri resolves filepath into a URI string like
         // nfs://dip-nfs.llnl.gov:/vol/g0/joe/readme
         uriInfo.getUri(uriString);

         ...
     @endverbatim

     In addition, \c MountPointInfo allows developers to retrieve mount-points
     information directly through a \c MyMntEnt object and offers 
     higher-level abstractions to query relevant properties: 
     e.g., is the source of a file remote or local.

     @verbatim
         #include "MountPointAttr.h"
         using namespace FastGlobalFileStatus;
         ...

         MyMntEnt anEntry;
        
         //
         // Returns the mount point info on filepath
         //
         mpInfo.getMntPntInfo(filepath, anEntry)   

         //
         // Determines whether the filepath is remotely served or not
         // 
         if (IS_YES(mpInfo.isRemoteFileSystem(filepath, anEntry)) {
             // filepath is remotely served
         }
     @endverbatim
    */

  namespace MountPointAttribute {

    /**
     *   FGFS_MOUNTS_FILE
     *   Defines a file that has most accurate mount point info.
     *   On Linux, the macro points to /proc/mounts that is on
     *   the procfs. This file contains the most precious current
     *   mount point information. Plus, procfs is served off of
     *   memory, so it is most scalable way of getting the mount 
     *   point information of a node.
     */
     const char FGFS_MOUNTS_FILE[] = "/proc/mounts";


    /**  FGFS_ALT_MOUNTS_FILE
     *   Defines an alternative file that contains the mount point
     *   info. On Linux, if /proc/mounts is not available for some
     *   reason, this macro will allow to fall back to /etc/mtab.
     *   The mount point information in this configuration file
     *   should be mostly accurate. But the file could be served remotely
     *   on diskless nodes.
     */
     const char FGFS_ALT_MOUNTS_FILE[] = "/etc/mtab";


    /** FGFS_STR_SIZE
     *   Defines the max string size
     */
    const int FGFS_STR_SIZE = 4096;


    /**
     *   Enumerates various file system types.
     *
     *   This type should be extended this enum as you extend your support 
     *   to other file system types.
     *
     *   TODO file systems: coda, coherent (Coherent Remote File System)
     *                      cramfs, efs, hfs, minix, msdos,
     *                      ncpfs, ntfs, romfs, sysv, udf,
     *                      ufs, umsdos, vfat, xenix, xiafs 
     */
    enum FileSystemType {
        fs_nfs     = 0, /*!< network file system version 3, remote */
        fs_nfs4    = 1, /*!< network file system version 4, remote */
        fs_lustre  = 2, /*!< parallel file system, remote */
        fs_gpfs    = 3, /*!< parallel file system, remote */
        fs_panfs   = 4, /*!< PanFS file system, remote */
        fs_plfs    = 5, /*!< PLFS checkpoint file system, remote */
        fs_cifs    = 6, /*!< common internet file system, remote */  
        fs_smbfs   = 7, /*!< server message block protocol, remote */  
        fs_dvs     = 8, /*!< Cray Compute Node's DVS, remote */
        fs_ext     = 9, /*!< local disk: sextended file system */
        fs_ext2    = 10, /*!< local disk: second extended file system */
        fs_ext3    = 11, /*!< local disk: third extended file system */
        fs_ext4    = 12, /*!< local disk: fourth extended file system */
        fs_jfs     = 13, /*!< local disk: IBM journaled file system */
        fs_xfs     = 14, /*!< local disk: high performance journaling fs */
        fs_reiserfs= 15, /*!< local disk: journaled file system */
        fs_hpfs    = 16, /*!< local disk: high performance file system */ 
        fs_iso9660 = 17, /*!< local compact disk file system */
        fs_aufs    = 18, /*!< AUFS "another union file system" (care is needed) */
        fs_ramfs   = 19, /*!< ramfs served off of memory */
        fs_tmpfs   = 20, /*!< a ramfs kind */
        fs_rootfs  = 21, /*!< a ramfs kind but care is needed since other FS can be overlaid */
        fs_proc    = 22, /*!< procfs, memory */
        fs_fusectl = 23, /*!< fuse file system, memory */
        fs_sysfs   = 24, /*!< sysfs, memory */
        fs_usbfs   = 25, /*!< usb file system, USB memory */
        fs_debugfs = 26, /*!< debugfs memory */
        fs_devpts  = 27, /*!< /dev/pts, memory */
        fs_securityfs = 28, /*!< memory */
        fs_binfmt_misc = 29,
        fs_cpuset  = 30,
        fs_rpc_pipefs = 31,
        fs_autofs  = 32, /*!< root directory of automount, memory */
        fs_selinux = 33, /*!< /selinux, pseudo file system, memory */
        fs_nfsd    = 34, /*!< /proc/fs/nfsd pseudo file system, memory */ 
        fs_cgroup  = 35, /*!< /proc/fs/cgroup pseudo file system, memory */ 
        fs_unknown = 36  /*!< catch all */
    };


    const int BASE_FS_SPEED = 1;
    const int BASE_FS_SCALABILITY = 1;
    const unsigned short INDIRECTION = USHRT_MAX;


    /**
     *   Info for file system types. This supports
     *   upper layers that classifies storage. 
     */
    struct FileSystemTypeInfo {
        FileSystemType t;
        unsigned short speed;
        unsigned short scalability;
        const char *fs_name;
    };


    /**
     *   Defines a data type to store file source information.
     *   It stores a URI scheme for the source as well. The tuple
     *   uniquely defines the orgin of the file, providing a 
     *   reference space in which objects of this tuple 
     *   are globally comparable. 
     */
    class FileUriInfo {
        public:
            FileUriInfo();
            ~FileUriInfo(); 

            std::string hostAddr;  /*!< host address that uniquely defines the remote file server */
            std::string exportDir; /*!< FS export directory within that server */
            std::string pathFromExportDir;  /*!< where the the file physically resides */
            std::string mountPoint;  /*!< local mount point corresponding to exportDir */

            /**
             *   Returns a globally unique id in the form of a URI string
             *   @param[out] uri buffer to store the URI string
             *
             *   @return true on success.
             */
            bool getUri(std::string &uri) const;

        private:
            FileUriInfo(const FileUriInfo &i);
            FileUriInfo & operator=(const FileUriInfo &rhs); 
            UriScheme *uscheme;
            friend class MountPointInfo; 
    };


    /**
     *
     *   Defines a data type to store getmntent_t information
     */
    class MyMntEnt {
        public:
            MyMntEnt();
            MyMntEnt(const MyMntEnt &o);
            explicit MyMntEnt(struct mntent &m);
            ~MyMntEnt();
            MyMntEnt & operator=(const MyMntEnt &rhs);
            bool operator==(const MyMntEnt &rhs);
            bool operator!=(const MyMntEnt &rhs);
            const std::string & getRealMountPointDir() const;

            std::string fsname;     /*!< Device or server for filesystem. */
            std::string dir_master; /*!< Directory mounted on. */
            std::string dir_branch; /*!< Directory mounted on. It points to the RW dir in the case of unionfs*/
            std::string type;       /*!< Type of filesystem: ufs, nfs, etc. */
            std::string opts;       /*!< Comma-separated options for fs. */
            int freq;               /*!< Dump frequency (in days). */
            int passno;             /*!< Pass number for `fsck'. */
    };


    /**
     *   Defines a data type that relates an arbiturary local file path to
     *   the mount point database. The local mount point DB has 
     *   sufficient information to resolve the file path to a globally
     *   unique identifier in the form of URI. This abstraction strictly 
     *   is limited to local, sequential processing.
     */
    class MountPointInfo {
        public:
            /**
             *   Default ctor. 
             */
            MountPointInfo();

            /**
             *   ctor that provides a knob to parse the mount point
             *   information. 
             *
             *   @param[in] prs if true, this ctor calls the parse method.
             *                  because ctor cannot return an error code.
             *                  the caller should call the isParse method 
             *                  to check the existence of an error condition.
             */
            MountPointInfo(bool prs);

            /**
             *   copy ctor.
             *   
             */
            MountPointInfo(const MountPointInfo &o);

            /**
             *   dtor.
             *   
             */
            ~MountPointInfo();

            /**
             *   assignment operator
             *   
             */
            MountPointInfo & operator=(const MountPointInfo &rhs);

            /**
             *   Checks if the mount point file has been parsed.
             *   @return a FGFSInfoAnswer.
             */
            FGFSInfoAnswer isParsed() const;

            /**
             *   Parses a mount point file and stores the info 
             *   in a database. This must be called after the
             *   default ctor is called and before other methods
             *   are used.
             *
             *   @return a C string if an error is encountered; otherwise NULL.
             */
            const char * parse();

            /**
             *   Returns a mount point entry corresponding to the given absolute path.
             *
             *   Note that the method bases its operation solely on the name: 
             *   any given absolute path will be resolved even if it is nonexistent 
             *   or you don't proper access to the path. Starting from the longest path 
             *   of the given path, which is the path itself, this method will compare 
             *   progressively shorter sub-paths of the path with the mount point database
             *   until it finds a match. For high scalability, we must avoid flooding 
             *   file servers even for metadata operations. This trade-off was made 
             *   because of that reason. Users should ensure the path is valid 
             *   before calling this method. 
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[out] result the mount point entry of that path of MyMntEnt type.
             *   @return a C string if an error is encountered; otherwise NULL.
             */
            const char * getMntPntInfo(const char *path,
                                       MyMntEnt &result) const;

            /**
             *   Returns a mount point entry corresponding to the given absolute path.
             * 
             *   This call is identical as getMntPntInfo except that it resolve
             *   the level of indirection if the file system supports it. For example,
             *   for union fs system, it returns the mount point for the 
             *   underlying file system wherein the actual file resides.
             *   TODO: this call should resolve dvs as well, which isn't yet supported.
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[out] result the mount point entry of that path of MyMntEnt type.
             *   @return a C string if an error is encountered; otherwise NULL.
             */
            const char * getMntPntInfo2(const char *path,
                                       MyMntEnt &result) const;

            /**
             *   Returns remote file server origin information that corresponds to a path.
             *
             *   Note that the method bases its operation solely on the name: 
             *   any given absolute path will be resolved even if it is nonexistent 
             *   or you don't proper access to the path. Starting from the longest path 
             *   of the given path, which is the path itself, this method will compare 
             *   progressively shorter sub-paths of the path with the mount point database
             *   until it finds a match. For high scalability, we must avoid flooding 
             *   file servers even for metadata operations. This trade-off was made 
             *   because of that reason. Users should ensure the path is valid 
             *   before calling this method. 
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[out] fui path's source information of FileUriInfo type.
             *   @return a C string if an error is encountered; otherwise NULL.
             */
            const char * getFileUriInfo(const char *path, 
                                        FileUriInfo &fui);

            /**
             *   Determines if a path is remotely served or not.
             *
             *   Note that the method bases its operation solely on the name: 
             *   any given absolute path will be resolved even if it is nonexistent 
             *   or you don't proper access to the path. Starting from the longest path 
             *   of the given path, which is the path itself, this method will compare 
             *   progressively shorter sub-paths of the path with the mount point database
             *   until it finds a match. For high scalability, we must avoid flooding 
             *   file servers even for metadata operations. This trade-off was made 
             *   because of that reason. Users should ensure the path is valid 
             *   before calling this method. 
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[out] result the mount point entry of that path of MyMntEnt type.
             *   @return an answer of MNPInfoAnswer type.
             */
            FGFSInfoAnswer isRemoteFileSystem(const char *path, 
                                              MyMntEnt &result) const;

            /**
             *   Determines if a path is locally served or not.
             *
             *   Note that the method bases its operation solely on the name: 
             *   any given absolute path will be resolved even if it is nonexistent 
             *   or you don't proper access to the path. Starting from the longest path 
             *   of the given path, which is the path itself, this method will compare 
             *   progressively shorter sub-paths of the path with the mount point database
             *   until it finds a match. For high scalability, we must avoid flooding 
             *   file servers even for metadata operations. This trade-off was made 
             *   because of that reason. Users should ensure the path is valid 
             *   before calling this method. 
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[out] result the mount point entry of that path of MyMntEnt type.
             *   @return an answer of MNPInfoAnswer type.
             */
            FGFSInfoAnswer isLocalDevice(const char *path,
                                         MyMntEnt &result) const;


            /**
             *   Resolves a file system type string to a FileSystemType enumerator.
             *
             *   @param[in] fsType file system type string.
             *   @return a FileSystemType object.
             */
            FileSystemType determineFSType(const std::string &fsType) const;


            /**
             *   Return File System's speed estimate as an multiple of 
             *   BASE_FS_SPEED 
             *
             *   @param[in] t of FileSystemType.
             *   @return speed. 0 if an error is encountered.
             */
            const int getSpeed(FileSystemType t) const; 


            /**
             *   Return File System's scalability estimate as an multiple of 
             *   BASE_FS_SCALABILITY 
             *
             *   @param[in] t of FileSystemType.
             *   @return scalability. 0 if an error is encountered.
             */
            const int getScalability(FileSystemType t) const; 


            /**
             *   Return File System's string name
             *
             *   @param[in] t of FileSystemType.
             *   @return File System's name. NULL if an error is encountered.
             */
            const char *getFSName(FileSystemType t) const;    


           /**
             *   Returns the MntPntMap as immutable object
             *
             *   @return a map object
             */
            const std::map<std::string, MyMntEnt> &getMntPntMap() const;

        private:

            /**
             *   Determines if a path is remotely served or not for AUFS union file system.
             *
             *   @param[in] path an absolute path that contains no links.
             *   @param[in] myEntry a MyMntEnt object describing mount point of the aufs path.
             *   @param[out] result the mount point entry of that path of MyMntEnt type.
             *   @return an answer of MNPInfoAnswer type.
             */
            FGFSInfoAnswer isAufsRemote(const char *path, 
                                        MyMntEnt &myEntry,
                                        MyMntEnt &result) const;

            UriScheme * createUriSchemeInstance(FileSystemType fst);

            std::map<std::string, MyMntEnt> mMntPntMap;
            bool parsed;
    };


    /**
     *   Registered an open FILE pointer to which a verbose message 
     *   is printed.
     *
     *   @param[in] fd open FILE pointer.
     *   @param[in] lvl 0=error_only 1=verbosity_1 2=verbosity_2
     *   @return none.
     */
    void MPA_registerMsgFd(FILE *fd, int lvl);

  } // MountPointAttribute namespace

} // FastGlobalFileStatus namespace

#endif // MOUNT_POINT_ATTR_H

