/*
 * --------------------------------------------------------------------------------
 * Copyright (c) 2011, Lawrence Livermore National Security, LLC. Produced at
 * the Lawrence Livermore National Laboratory. Written by Dong H. Ahn <ahn1@llnl.gov>.
 * All rights reserved.
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
}

#include <string>
#include <map>
#include <vector>
#include "FgfsCommon.h"
#include "MountPointAttrUri.h"

namespace FastGlobalFileStat {

    /**
     @mainpage Fast Global File Stat (FGFS)
     @author Dong H. Ahn, Development Environment Group, Livermore Computing (LC) Division, LLNL

     @section intro Introduction

     Efficient use of storage is essential for high performance 
     computing applications. HPC environments must employ best 
     practices in accessing all levels of the storage hierarchy, 
     from registers to caches and memory, further to various 
     file systems so as to realize the full potential of the system.
     However, trends towards millions of processing cores continue to impose
     increasingly larger pressure to lower levels in the hierarchy.  
     In particular, the exponential growth in core counts of high-end systems 
     quickly outpace the scalability of any file system technologies. 
     Often, a large-scale application's single uncoordinated, unbalanced access to a shared 
     file system becomes a catastrophe. Whenever a new system is
     brought up, HPC centers observe that uncoorindated file system
     access by a newly ported application drastically overwhelms 
     a shared file system, a problem surfacing as what appears 
     to be a denial of service attack on the shared resource. 
     This problem is pandemic from scientific applications 
     to runtime tools as they always build on 
     some software components that are built for serial processing
     with no notion of coordination needed for parallel processing. 

     Many tools and applications have implemented ad-hoc 
     solutions to address this well-recognized challenge. But that 
     approach have proven to be costly. Supporting the task of porting 
     and maintaining \a m solutions to \a n environments becomes \a m x \a n 
     efforts. We rather desire a common middleware solution that provides
     applications and runtime tools with abstractions and 
     mechanisms for efficient, scalable use of file systems. 
     We developed Fast Global File Stat (FGFS) with the goal to provide 
     such a solution. FGFS offers abstractions and scalable mechanisms 
     that determine common distributed properties of a file 
     such as its uniqueness, consistency and the degree of distribution. 
     Using these abstractions, developers can easily judge 
     the I/O performance expection on the target file at a scale, 
     the information that allows them to devise an effective storage 
     access strategy involving various trade-offs.
  
     @section fnresol File Name Resolution Engine 
     The core building block of FGFS is the technique that 
     raises the local namespace of a file to a global namespace. 
     The global namespace enables fast comparisons of local 
     file properties across distributed machines with little or
     no access requirement on the underlying file systems. 
     More specifically, our file name resolution engine 
     turns a local file path into a Uniform Resource Identifier (URI), 
     a globally unique identifier of the file. This resolution 
     process is merely a memory operation as our technique 
     builds an URI through file system mount point table 
     that is available in system memory. Thus,
     this core logic requires no communication and is extremely 
     scalable by definition. 

     The MountPointInfo class is the main data type 
     associated with the file name resolution process. For example,
     the following code snippet stores \c filepath's URI into 
     \c uriString like "nfs://dip-nfs.llnl.gov:/vol/g0/joe/readme"
     through a FileUriInfo object.

     @verbatim
         #include "MountPointAttr.h"
         using namespace FastGlobalFileStat;
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

     In addition, MountPointInfo allows developers to get mount point
     information directly through a MyMntEnt object and provides 
     higher-level abstractions to determine other properties 
     like whether a file is served remotely or locally.

     @verbatim
         #include "MountPointAttr.h"
         using namespace FastGlobalFileStat;
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

     @section distabsr Parallel Abstractions and Mechanisms
     Across \a N different computer nodes, the file name resolution engine 
     can turn a same local file name into \a N many different URIs
     or a unique URI or somewhere in between. 
     The unique URI resolution would imply that the application's I/O
     on that file would potentially suffer performance degradation 
     once the level of concurrent access on the file starts overwhelming 
     the underlying file server's capability.  
     On the other hand, with many different URIs, a problem related 
     to consistency of those distributed files may arise. 
     FGFS provides various parallel abstractions and algorithms 
     that efficiently answer distributed properties related to such 
     common issues.

     TBD
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
        fs_nfs,         /*!< network file system version 3, remote */
        fs_nfs4,        /*!< network file system version 4, remote */
        fs_lustre,      /*!< parallel file system, remote */
        fs_gpfs,        /*!< parallel file system, remote */
        fs_panfs,       /*!< PanFS file system, remote */
        fs_plfs,        /*!< PLFS checkpoint file system, remote */
        fs_cifs,        /*!< common internet file system, remote */  
        fs_smbfs,       /*!< server message block protocol, remote */  
        fs_dvs,         /*!< Cray Compute Node's DVS, remote */
        fs_ext,         /*!< local disk: sextended file system */
        fs_ext2,        /*!< local disk: second extended file system */
        fs_ext3,        /*!< local disk: third extended file system */
        fs_ext4,        /*!< local disk: fourth extended file system */
        fs_jfs,         /*!< local disk: IBM journaled file system */
        fs_xfs,         /*!< local disk: high performance journaling fs */
        fs_reiserfs,    /*!< local disk: journaled file system */
        fs_hpfs,        /*!< local disk: high performance file system */ 
        fs_iso9660,     /*!< local compact disk file system */
        fs_aufs,        /*!< AUFS "another union file system" (care is needed) */
        fs_ramfs,       /*!< ramfs served off of memory */
        fs_tmpfs,       /*!< a ramfs kind */
        fs_rootfs,      /*!< a ramfs kind but care is needed since other FS can be overlaid */
        fs_proc,        /*!< procfs, memory */
        fs_fusectl,     /*!< fuse file system, memory */
        fs_sysfs,       /*!< sysfs, memory */
        fs_usbfs,       /*!< usb file system, USB memory */
        fs_debugfs,     /*!< debugfs memory */
        fs_devpts,      /*!< /dev/pts, memory */
        fs_securityfs,  /*!< memory */
        fs_binfmt_misc,
        fs_cpuset,
        fs_rpc_pipefs,
        fs_autofs,      /*!< root directory of automount, memory */
        fs_selinux,     /*!< /selinux, pseudo file system, memory */
        fs_nfsd,        /*!< /proc/fs/nfsd pseudo file system, memory */ 
        fs_unknown      /*!< catch all */
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

            std::string fsname;     /*!< Device or server for filesystem. */
            std::string dir_master; /*!< Directory mounted on. */
            std::string dir_branch; /*!< Directory mounted on. */
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
             *   Returns a mount point entry corresponding to an absolute path.
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

} // FastGlobalFileStat namespace

#endif // MOUNT_POINT_ATTR_H

