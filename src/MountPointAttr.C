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
 *        May 27 2011 DHA: Added config.h support
 *        May 20 2011 DHA: Cleaned up interfaces: Added URI concept and 
 *                         use FileUriInfo to describe both remote and local
 *                         resources. Removed convLocal2Remote and 
 *                         getGloballyUniqFileId. Users of this model 
 *                         will have to use the unified getUri method 
 *                         of an FileUriInfo object.   
 *        May 13 2011 DHA: Added debug print support
 *        Jan 10 2011 DHA: Added rootfs support.
 *        Jan 07 2011 DHA: Logic to handle union mount file system,
 *                         specifically AUFS.
 *        Jan 04 2011 DHA: File created.
 *
 */

#ifndef HAVE_MOUNTPOINTATTR_H
# include "config.h"
# define HAVE_MOUNTPOINTATTR_H 1
#endif

#include "MountPointAttr.h"

extern "C" {
#include <limits.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <netdb.h>
}

#include <iostream>
#include <sstream>
#include <stdexcept>


using namespace FastGlobalFileStatus;

using namespace FastGlobalFileStatus::MountPointAttribute;

///////////////////////////////////////////////////////////////////
//
//  Global Variables:    namespace FastGlobalFileStatus
//
//
int FastGlobalFileStatus::MountPointAttribute::verboseLevel = 0;


///////////////////////////////////////////////////////////////////
//
//  Static Variables:    namespace FastGlobalFileStatus
//
//
static FILE *debugOut = stdout;
static bool nNameCached = false;
static char localNodeName[PATH_MAX];

//
// TODO: This should be later changed as mount point-specific configuration
//
static const FileSystemTypeInfo ftinfo[] = {
    /* 0 */ {fs_nfs, BASE_FS_SPEED, BASE_FS_SCALABILITY, "nfs"}, 
    /* 1 */ {fs_nfs4, BASE_FS_SPEED, BASE_FS_SCALABILITY, "nfs4"}, 
    /* 2 */ {fs_lustre, BASE_FS_SPEED, 6*BASE_FS_SCALABILITY, "lustre"}, 
    /* 3 */ {fs_gpfs, BASE_FS_SPEED, 6*BASE_FS_SCALABILITY, "gpfs"}, 
    /* 4 */ {fs_panfs, BASE_FS_SPEED, 6*BASE_FS_SCALABILITY, "panfs"}, 
    /* 5 */ {fs_plfs, BASE_FS_SPEED, BASE_FS_SCALABILITY, "plfs"}, 
    /* 6 */ {fs_cifs, BASE_FS_SPEED, BASE_FS_SCALABILITY, "cifs"}, 
    /* 7 */ {fs_smbfs, BASE_FS_SPEED, BASE_FS_SCALABILITY, "smbfs"}, 
    /* 8 */ {fs_dvs, BASE_FS_SPEED, 2*BASE_FS_SCALABILITY, "dvs"}, 
    /* 9 */ {fs_ext, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "ext"}, 
    /* 10 */ {fs_ext2, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "ext2"}, 
    /* 11 */ {fs_ext3, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "ext3"}, 
    /* 12 */ {fs_ext4, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "ext4"}, 
    /* 13 */ {fs_jfs, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "jfs"}, 
    /* 14 */ {fs_xfs, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "xfs"}, 
    /* 15 */ {fs_reiserfs, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "reiserfs"}, 
    /* 16 */ {fs_hpfs, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "hpfs"}, 
    /* 17 */ {fs_iso9660, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "iso9660"}, 
    /* 18 */ {fs_aufs, INDIRECTION, INDIRECTION, "aufs"}, 
    /* 19 */ {fs_ramfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "ramfs"}, 
    /* 20 */ {fs_tmpfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "tmpfs"}, 
    /* 21 */ {fs_rootfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "rootfs"}, 
    /* 22 */ {fs_proc, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "proc"}, 
    /* 23 */ {fs_fusectl, 2*BASE_FS_SPEED, BASE_FS_SCALABILITY, "fusectl"}, 
    /* 24 */ {fs_sysfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "sysfs"}, 
    /* 25 */ {fs_usbfs, 5*BASE_FS_SPEED, BASE_FS_SCALABILITY, "usbfs"}, 
    /* 26 */ {fs_debugfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "debugfs"}, 
    /* 27 */ {fs_devpts, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "devpts"}, 
    /* 28 */ {fs_securityfs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "securityfs"}, 
    /* 29 */ {fs_binfmt_misc, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "binfmt_misc"}, 
    /* 30 */ {fs_cpuset, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "cpuset"}, 
    /* 31 */ {fs_rpc_pipefs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "rpc_pipefs"}, 
    /* 32 */ {fs_autofs, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "autofs"}, 
    /* 33 */ {fs_selinux, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "selinux"}, 
    /* 34 */ {fs_nfsd, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "nfsd"}, 
    /* 35 */ {fs_cgroup, 10*BASE_FS_SPEED, BASE_FS_SCALABILITY, "cgroup"}, 
    /* 36 */ {fs_unknown, BASE_FS_SPEED, BASE_FS_SCALABILITY, "unknown"} 
};
    

///////////////////////////////////////////////////////////////////
//
//  PUBLIC INTERFACE:   namespace FastGlobalFileStatus::MountPointAttribute
//
//
void 
FastGlobalFileStatus::MountPointAttribute::MPA_registerMsgFd(
    FILE *fptr, int lvl)
{
    if (fptr) 
    {
        debugOut = fptr; 
    }

    if (lvl >= 0) {
       verboseLevel = lvl;
    }
}


void 
FastGlobalFileStatus::MountPointAttribute::MPA_sayMessage(
    const char* m, bool b, const char* output, ...)
{
    va_list ap;
    char log[PATH_MAX];
    const char *ei_str = (b) ? "ERROR" : "INFO";
	
    snprintf(log, PATH_MAX, "<%s> (%s): %s\n", m, ei_str, output);
	
    va_start(ap, output);
    vfprintf(debugOut, log, ap);
    va_end(ap);
}


int 
FastGlobalFileStatus::MountPointAttribute::MPA_getLocalNodeName(
    char *name, size_t len)
{
    int rc = 0;
    int l = 0;

    if (!nNameCached) {
        return -1;
    }

    l = (len < PATH_MAX)? len : PATH_MAX;
    strncpy(name, localNodeName, l);

    return rc;
}


///////////////////////////////////////////////////////////////////
//
//  class NfsUriScheme 
//
//
void
NfsUriScheme::getUri(const std::string &hostAddr,
                     const std::string &exportDir,
                     const std::string &pathFromExportDir,
                     const std::string &mountPoint,
                     std::string &uri) const
{
    //
    // Make sure exportDir start with '/'
    // and pathFromExportDir is relative
    uri = std::string("nfs://") 
          + hostAddr
          + exportDir + std::string("/")
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class LustreUriScheme
//
//
void
LustreUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    //
    // use ah_ prefix since URI scheme hasn't standardized for lustre 
    //
    uri = std::string("ah_lustre://") 
          + hostAddr
          + exportDir + std::string("/")
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class GpfsUriScheme
//
//
void
GpfsUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    //
    // use ah_ prefix since URI scheme hasn't standardized for lustre 
    //
    uri = std::string("ah_gpfs://") 
          + hostAddr
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class PanfsUriScheme
//
//
void
PanfsUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    //
    // use ah_ prefix since URI scheme hasn't standardized for lustre 
    //
    uri = hostAddr + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class PlfsUriScheme
//
//
void
PlfsUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    //
    // use ah_ prefix since URI scheme hasn't standardized for lustre 
    //
    uri = std::string("ah_plfs://") + hostAddr + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class DvsUriScheme
//
//
void
DvsUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    //
    // use ah_ prefix since URI scheme hasn't standardized for lustre 
    //
    uri = std::string("ah_dvs://") + hostAddr + pathFromExportDir; 
}

///////////////////////////////////////////////////////////////////
//
//  class CifsUriScheme
//  http://davenport.sourceforge.net/draft-crhertel-smb-url-07.txt
//
void
CifsUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    uri = std::string("cifs://") 
          + hostAddr
          + exportDir + std::string("/")
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class SmbUriScheme
//  http://davenport.sourceforge.net/draft-crhertel-smb-url-07.txt
//
void
SmbUriScheme::getUri(const std::string &hostAddr,
                        const std::string &exportDir,
                        const std::string &pathFromExportDir,
                        const std::string &mountPoint,
                        std::string &uri) const
{
    uri = std::string("smb://") 
          + hostAddr
          + exportDir + std::string("/")
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class LocalUriScheme
//
//
void
LocalUriScheme::getUri(const std::string &hostAddr,
                       const std::string &exportDir,
                       const std::string &pathFromExportDir,
                       const std::string &mountPoint,
                       std::string &uri) const
{
    //
    // For local case, exportDir should be empty 
    // and pathFromExportDir starts with '/' 
    // Note taht hostAddr must be FQDN of the local host
    uri = std::string("file://") 
          + hostAddr
          + pathFromExportDir; 
}


///////////////////////////////////////////////////////////////////
//
//  class FileUriInfo
//
//
FileUriInfo::FileUriInfo() : uscheme(NULL)
{

}


FileUriInfo::~FileUriInfo()
{
    if (uscheme) {
        delete uscheme;
        uscheme = NULL;
    }
}


bool 
FileUriInfo::getUri(std::string &uri) const
{
    bool rc = false;

    if (uscheme) {
       uscheme->getUri(hostAddr, 
                       exportDir, 
                       pathFromExportDir,
                       mountPoint,
                       uri);
       rc = true;
    } 

    return rc;
}


///////////////////////////////////////////////////////////////////
//
//  class MyMntEnt
//
//
MyMntEnt::MyMntEnt()
{

}


MyMntEnt::MyMntEnt(const MyMntEnt &o)
{
    fsname = o.fsname;
    dir_master = o.dir_master;
    dir_branch = o.dir_branch;
    type = o.type;
    opts = o.opts;
    freq = o.freq;
    passno = o.passno;
}


MyMntEnt::MyMntEnt(struct mntent &m)
{
    fsname = m.mnt_fsname;
    dir_master = m.mnt_dir;
    dir_branch = m.mnt_dir;
    type = m.mnt_type;
    opts = m.mnt_opts;
    freq = m.mnt_freq;
    passno = m.mnt_passno;
}


MyMntEnt::~MyMntEnt()
{

}


MyMntEnt &
MyMntEnt::operator=(const MyMntEnt &rhs)
{
    fsname = rhs.fsname;
    dir_master = rhs.dir_master;
    dir_branch = rhs.dir_branch;
    type = rhs.type;
    opts = rhs.opts;
    freq = rhs.freq;
    passno = rhs.passno;

    return *this;
}


bool
MyMntEnt::operator==(const MyMntEnt &rhs)
{
    return ( (fsname == rhs.fsname)
             && (dir_master == rhs.dir_master)
             && (dir_branch == rhs.dir_branch)
             && (type == rhs.type)
             && (opts == rhs.opts)
             && (freq == rhs.freq)
             && (passno == rhs.passno) );
}


bool
MyMntEnt::operator!=(const MyMntEnt &rhs)
{
    return ( !((fsname == rhs.fsname)
             && (dir_master == rhs.dir_master)
             && (dir_branch == rhs.dir_branch)
             && (type == rhs.type)
             && (opts == rhs.opts)
             && (freq == rhs.freq)
             && (passno == rhs.passno)) );
}


const std::string & 
MyMntEnt::getRealMountPointDir() const
{
    //
    // In most cases, dir_master is identical as dir_branch.
    // But in the case of stacked file system like unionfs, dir_branch
    // points to the real RW mount point (dir_branch) hidden below
    // RO mount point (dir_master). This method return dir_branch.
    //
    return dir_branch;
}


///////////////////////////////////////////////////////////////////
//
//  class MountPointInfo
//
//
MountPointInfo::MountPointInfo() : parsed(false)
{

}


MountPointInfo::MountPointInfo(bool prs) : parsed(false)
{
    if (prs) {
        parse();
    } 
}


MountPointInfo::MountPointInfo(const MountPointInfo &o)
{
    mMntPntMap = o.mMntPntMap;
    parsed = o.parsed;
}


MountPointInfo::~MountPointInfo()
{
    if (!mMntPntMap.empty()) {
        mMntPntMap.clear();
    }
    parsed = false;
}


MountPointInfo & 
MountPointInfo::operator=(const MountPointInfo &rhs)
{
    mMntPntMap = rhs.mMntPntMap;
    parsed = rhs.parsed;

    return *this;
}


FGFSInfoAnswer 
MountPointInfo::isParsed() const
{
    return (parsed)? ans_yes : ans_no;
}


const char *
MountPointInfo::parse()
{
    struct mntent mntbuf;
    FILE *mpfptr = NULL;
    char strbuf[FGFS_STR_SIZE];
    struct hostent *hent;
    const char *errStr = NULL;
    std::stringstream ss;
    char hname[PATH_MAX];

    if (gethostname(hname, PATH_MAX) < 0) {
        ss << "gethostname returned neg";
        errStr = strdup(ss.str().c_str());
        goto l_has_err;
    }

    if ( (hent = gethostbyname(hname)) ) {
        strncpy(localNodeName, hent->h_name, PATH_MAX);
    }
    else {
        strncpy(localNodeName, hname, PATH_MAX);
    }
    nNameCached = true;

    mpfptr = setmntent(FGFS_MOUNTS_FILE, "r");
    if ( !mpfptr ) {
        ss << "Error opening "
           << FGFS_MOUNTS_FILE
           << "; trying an alternative file. "
           << FGFS_ALT_MOUNTS_FILE;
        errStr = strdup(ss.str().c_str());

        if (ChkVerbose(1)) {
            MPA_sayMessage("MountPointAttr", false, errStr);
        }

        mpfptr = setmntent(FGFS_ALT_MOUNTS_FILE, "r");
        if (!mpfptr ) {
            ss << "Error opening "
               << FGFS_ALT_MOUNTS_FILE
               << "as well.";
            errStr = strdup(ss.str().c_str());

            if (ChkVerbose(0)) {
                MPA_sayMessage("MountPointAttr", true, errStr);
            }
            goto l_has_err;
        }
    }

    while (getmntent_r(mpfptr, &mntbuf, strbuf, FGFS_STR_SIZE) 
           != NULL) {

        MyMntEnt anEntry(mntbuf);
        std::map<std::string, MyMntEnt>::iterator miter;
        miter = mMntPntMap.find(mntbuf.mnt_dir);
        if ( (miter != mMntPntMap.end()) ) {
            if (miter->second.type != "rootfs") {

                //
                // The same mount point appeared more than twice. 
                // It is reasonable for rootfs (because of union file 
                // systems) and probably some odd
                // FSs that have not been tested as yet. Currently
                // we simply throw away redundant entries.
                //
                // Exception is rootfs. The system typically mounts 
                // multiple devices at the root and designates rootfs
                // as the root of that; we want to overwrite
                // the rootfs entry with what appears next.
                // NOTE: with this logic, rootfs mnt point better  
                // appears early. 
                //
                if (ChkVerbose(1)) {
                    char logbuf[PATH_MAX];
                    snprintf(logbuf, PATH_MAX, "%s: %s: %s: %s", 
                         localNodeName,
                         "Double mount point entries, ignoring",
                         FGFS_MOUNTS_FILE, mntbuf.mnt_dir);

                    MPA_sayMessage("MountPointAttr", false, logbuf);
                }

                continue;
            } 
            else {
                //
                // An entry found but it's rootfs
                //
                if (ChkVerbose(1)) {
                     MPA_sayMessage("MountPointAttr", 
                         false, 
                         "an entry rootfs is about to be replaced with %s", 
                         anEntry.type.c_str());
                }
            }
        }

        //
        // Taking a copy of anEntry and put it into the map
        // key off of the mount point. 
        //
        mMntPntMap[std::string(mntbuf.mnt_dir)] = anEntry;
    }

    parsed = true;
    return NULL;

l_has_err:
    return errStr;
}


const char *
MountPointInfo::getMntPntInfo(const char *path, 
                              MyMntEnt &result) const
{
    std::stringstream ss;

    if (!path) {
        ss << "The given path is null.";
        if (ChkVerbose(1)) {
            MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
        }
        return (const char *) strdup(ss.str().c_str());
    }

    if (path[0] != '/') {
        ss << "The given path is not absolute.";
        if (ChkVerbose(1)) {
            MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
        }
        return (const char *) strdup(ss.str().c_str());
    }

    char *dname = strdup(path);
    char *dnameCp = dname;

    while (!(dname[0] == '/' && dname[1] == '\0')) {
        if (mMntPntMap.find(dname) != mMntPntMap.end()) {
            break;
        }
        dname = dirname(dname);
    }

    std::map<std::string, MyMntEnt>::const_iterator iter
        = mMntPntMap.find(dname);

    free(dnameCp);

    if (iter != mMntPntMap.end()) {
        result = iter->second;
    }
    else {
        ss << "Not found.";
        if (ChkVerbose(1)) {
            MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
        }
        return (const char *) strdup(ss.str().c_str());
    }

    return NULL;
}


const char *
MountPointInfo::getMntPntInfo2(const char *path, 
                              MyMntEnt &result) const
{
    std::stringstream ss;
    FGFSInfoAnswer rc = isRemoteFileSystem(path, result); 
    if (IS_ERROR(rc)) {
        ss << "Error encountered in isRemoteFileSystem";
        return (strdup(ss.str().c_str()));
    }

    return NULL;
}


const char *
MountPointInfo::getFileUriInfo(const char *path, 
                               FileUriInfo &fui) 
{
    std::stringstream ss;
    MyMntEnt myEntry;
    const char *errStr = NULL; 

    FGFSInfoAnswer rc = isRemoteFileSystem(path, myEntry); 

    if (rc == ans_yes) {
        FileSystemType fsType = determineFSType(myEntry.type); 
        fui.uscheme = createUriSchemeInstance(fsType); 

        size_t found = myEntry.fsname.find_first_of(":");
        if (found != std::string::npos) {
            fui.hostAddr = myEntry.fsname.substr(0, found);
            found = myEntry.fsname.find_first_not_of(":", found);
            if (found != std::string::npos) {
                if ( (myEntry.fsname.substr(found)[0] == '/')
                     && (myEntry.fsname.substr(found)[1] == '/')) {
                    //
                    // URI is used for fsname. panfs://ipaddress being an example
                    //
                    fui.hostAddr = myEntry.fsname;
                    fui.exportDir = "";
                    fui.mountPoint = myEntry.dir_master;
                    fui.pathFromExportDir = std::string(path);

                    if (ChkVerbose(1)) {
                        ss << "Remote file server source string is URI format";
                        MPA_sayMessage("MountPointAttr", false, ss.str().c_str());
                    }
                }
                else {
                    fui.exportDir = myEntry.fsname.substr(found);
                    std::string pathStr = path;
                    size_t po = pathStr.find(myEntry.dir_master);
                    fui.mountPoint = myEntry.dir_master;
                    if (po != std::string::npos) {
                        if (fui.mountPoint[fui.mountPoint.length()-1] != '/' ) {
                            if (fui.mountPoint.size() < pathStr.size()) { 
                                fui.pathFromExportDir 
                                    = pathStr.substr(fui.mountPoint.size()+1);
                            }
                        } 
                        else {
                            if (fui.mountPoint.size() < pathStr.size()) { 
                                fui.pathFromExportDir 
                                    = pathStr.substr(fui.mountPoint.size());
                            }
                        }
                    }
                    else {
                        fui.pathFromExportDir = "";
                        ss << "Mounted directory does not match the given path prefix.";

                        if (ChkVerbose(1)) {
                            MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
                        }

                        errStr = strdup(ss.str().c_str()); 
                        goto l_has_err_or_notfound;
                    }
                }
            }
            else {
                //
                // ":" is at the end of fsname! error
                //
                fui.hostAddr = "";
                ss << "Ill-formed remote file server source string.";

                if (ChkVerbose(1)) {
                    MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
                }

                errStr = strdup(ss.str().c_str());
                goto l_has_err_or_notfound;
            }
        }
        else {
                //
                // If you don't have a colon, you want to use the whole fsname
                // as the identity. This would be the case for a file system
                // like GPFS.
                //
                fui.hostAddr = myEntry.fsname;
                fui.exportDir = "";
                fui.mountPoint = myEntry.dir_master;
                fui.pathFromExportDir = std::string(path);

                if (ChkVerbose(1)) {
                    ss << "Remote file server source string isn't \"ip:/exportDir\" format. ";
                    ss << "Using the whole fsname as the identity.";
                    MPA_sayMessage("MountPointAttr", false, ss.str().c_str());
                }
        }
    }
    else if (rc == ans_no) {
        size_t po;

        fui.uscheme = new LocalUriScheme(); 
        if (!nNameCached) {
            ss << "Cached local node name is not available";

            if (ChkVerbose(1)) {
                MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
            }

            errStr = strdup(ss.str().c_str());
            goto l_has_err_or_notfound;
        }

        po = std::string(path).find(myEntry.dir_master);
        if (po != std::string::npos) {
           fui.hostAddr = localNodeName;
           fui.mountPoint = myEntry.dir_master;
           fui.exportDir = std::string("");
           fui.pathFromExportDir = path;
        }
        else {
            ss << "mismatch in the path and the mount point into ";
            if (ChkVerbose(1)) {
                MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
            }
            errStr = strdup(ss.str().c_str());
            goto l_has_err_or_notfound;
        }
    }
    else {
        ss << "isRemoteFileSystem() returned an error";

        if (ChkVerbose(1)) {
            MPA_sayMessage("MountPointAttr", true, ss.str().c_str());
        }

        errStr = strdup(ss.str().c_str());
        goto l_has_err_or_notfound;
    }

    return NULL;

l_has_err_or_notfound:
    return errStr;
}


FGFSInfoAnswer
MountPointInfo::isRemoteFileSystem(const char *path,
                                   MyMntEnt &result) const
{
    FGFSInfoAnswer answer;
    MyMntEnt entry;

    const char *msg = getMntPntInfo(path, entry);
    if (msg) {
        answer = ans_error;
    }
    else {
       //
       // Note: Following needs to be extended to support 
       // any new file system type.
       //
        switch (determineFSType(entry.type)) {
            case fs_nfs:
            case fs_nfs4:
            case fs_lustre:
            case fs_gpfs:
            case fs_panfs:
            case fs_plfs:
            case fs_dvs:
            case fs_cifs:
            case fs_smbfs:
                {
                    answer = ans_yes;
                    result = entry;
                    break;
                }
            case fs_aufs:
                {
                    answer = isAufsRemote(path, entry, result);
                    break;
                }
            default:
                {
                    answer = ans_no;
                    result = entry;
                    break;
                }
        }
    }

    return answer;
}


FGFSInfoAnswer
MountPointInfo::isLocalDevice(const char *path, MyMntEnt &result) const
{
    return NOT(isRemoteFileSystem(path, result));
}


const int 
MountPointInfo::getSpeed(FileSystemType t) const
{
    if (t >= 0 && t <= fs_unknown && ftinfo[t].t == t) {
        return (ftinfo[t].speed);
    }

    return 0;
}


const int 
MountPointInfo::getScalability(FileSystemType t) const
{
    if (t >= 0 && t <= fs_unknown && ftinfo[t].t == t) {
        return (ftinfo[t].scalability);
    }

    return 0;
}


const char *
MountPointInfo::getFSName(FileSystemType t) const
{
    if (t >= 0 && t <= fs_unknown && ftinfo[t].t == t) {
        return (ftinfo[t].fs_name);
    }

    return NULL;
}


const std::map<std::string, MyMntEnt> &
MountPointInfo::getMntPntMap() const
{
    return mMntPntMap;
}


FileSystemType
MountPointInfo::determineFSType(const std::string &fsType) const
{
    FileSystemType t;

    //
    // Note: Following needs to be extended to support 
    // any new file system type.
    //
    if (fsType == "nfs") {
        t = fs_nfs;
    }
    else if (fsType == "nfs4") {
        t = fs_nfs4;
    }
    else if (fsType == "lustre") {
        t = fs_lustre;
    }
    else if (fsType == "gpfs") {
        t = fs_gpfs;
    }
    else if (fsType == "panfs") {
        t = fs_panfs;
    }
    else if (fsType == "fuse.plfs") {
        t = fs_plfs;
    }
    else if (fsType == "dvs") {
        t = fs_dvs;
    }
    else if (fsType == "cifs") {
        // NOTE: June/13/2011 URI hasn't yet been tested 
        t = fs_cifs;
    }
    else if (fsType == "smbfs") {
        // NOTE: June/13/2011 URI hasn't yet been tested 
        t = fs_smbfs;
    }
    else if (fsType == "rootfs") {
        t = fs_rootfs;
    }
    else if (fsType == "ext") {
        t = fs_ext;
    }
    else if (fsType == "ext2") {
        t = fs_ext2;
    }
    else if (fsType == "ext3") {
        t = fs_ext3;
    }
    else if (fsType == "ext4") {
        t = fs_ext4;
    }
    else if (fsType == "jfs") {
        // NOTE: June/13/2011 hasn't yet been tested 
        t = fs_jfs;
    }
    else if (fsType == "xfs") {
        // NOTE: June/13/2011 hasn't yet been tested 
        t = fs_xfs;
    }
    else if (fsType == "reiserfs") {
        // NOTE: June/13/2011 hasn't yet been tested 
        t = fs_reiserfs;
    }
    else if (fsType == "hpfs") {
        // NOTE: June/13/2011 hasn't yet been tested 
        t = fs_hpfs;
    }
    else if (fsType == "iso9660") {
        t = fs_iso9660;
    }
    else if (fsType == "tmpfs") {
        t = fs_tmpfs;
    }
    else if (fsType == "proc") {
        t = fs_proc;
    }
    else if (fsType == "sysfs") {
        t = fs_sysfs;
    }
    else if (fsType == "usbfs") {
        t = fs_usbfs;
    }
    else if (fsType == "devpts") {
        t = fs_devpts;
    }
    else if (fsType == "securityfs") {
        t = fs_securityfs;
    }
    else if (fsType == "binfmt_misc") {
        t = fs_binfmt_misc;
    }
    else if (fsType == "cpuset") {
        t = fs_cpuset;
    }
    else if (fsType == "rpc_pipefs") {
        t = fs_rpc_pipefs;
    }
    else if (fsType == "aufs") {
        t = fs_aufs;
    }
    else if (fsType == "selinux") {
        t = fs_selinux;
    }
    else if (fsType == "nfsd") {
        t = fs_nfsd;
    }
    else {
        t = fs_unknown;
    }

    return t;
}


///////////////////////////////////////////////////////////////////
//
//  PRIVATE METHODS:   MountPointAttributes
//
//

//
// Make Copy ctor a private so that it can't be copied
//
FileUriInfo::FileUriInfo(const FileUriInfo &i) 
{
    hostAddr = i.hostAddr;
    exportDir = i.exportDir;
    pathFromExportDir = i.pathFromExportDir;
    mountPoint = i.mountPoint;
    uscheme = i.uscheme;
}


//
// Make Copy ctor a private so that it can't be assigned
//
FileUriInfo & 
FileUriInfo::operator=(const FileUriInfo &rhs)
{
    hostAddr = rhs.hostAddr;
    exportDir = rhs.exportDir;
    pathFromExportDir = rhs.pathFromExportDir;
    mountPoint = rhs.mountPoint;
    //uscheme = rhs.uscheme;

    return *this;
}


FGFSInfoAnswer
MountPointInfo::isAufsRemote(const char *path,
                             MyMntEnt &myEntry, 
                             MyMntEnt &result) const
{
    if (!path || myEntry.type != "aufs") {
        return ans_error;
    }

    // Find the field describing branch information
    size_t found = myEntry.opts.find_first_of("br:");
    if (found == std::string::npos) {
        // No branch information 
        return ans_error;
    }

    try {
        std::string brString = myEntry.opts.substr(found);
        size_t blankFound = brString.find_first_of("\t\n ");
        brString = brString.substr(0, blankFound);

        // brString contains only br:... field
        size_t pos1=0;
        size_t pos2=0;
        std::vector<size_t> colons;

        while ( (pos1 = brString.find_first_of(":", pos1))
                != std::string::npos) {
            colons.push_back(pos1);
            pos1++;
        }

        std::vector<UnionMountBranch> branches;
        std::string aBranchString;
        size_t eqSignFound;

        if (!colons.empty()) {
            std::vector<size_t>::const_iterator i;
            i = colons.begin();
            pos1  = (*i);
            ++i;
            while ( i != colons.end() ) {
               pos2 = (*i);
               aBranchString = brString.substr(pos1+1, pos2-pos1-1);
               eqSignFound = aBranchString.find_first_of("=");
               UnionMountBranch umb;
               umb.um_branch = aBranchString.substr(0, eqSignFound);
               umb.um_perm = aBranchString.substr(eqSignFound+1);
               branches.push_back(umb);
               pos1 = pos2;
               ++i;
            }

            aBranchString = brString.substr(pos1+1);
            eqSignFound = aBranchString.find_first_of("=");
            UnionMountBranch umb;
            umb.um_branch = aBranchString.substr(0, eqSignFound);
            umb.um_perm = aBranchString.substr(eqSignFound+1);
            branches.push_back(umb);
        }

        // We want to locate the ro or rr branch and characterize its type
        // if at least one of ro or rr mounts is not remote, we decide 
        // FS isn's remote
        std::vector<UnionMountBranch>::const_iterator bIter;

        std::string pathStr = std::string(path);
        size_t po = pathStr.find(myEntry.dir_master);
        std::string pathSuffix;
        if ( po != std::string::npos) {
            pathSuffix = pathStr.substr(po+1);
        }
        else {
            pathSuffix = pathStr; 
        }

        //
        // We simplify this by only checking if the path exists 
        // in a branch that is read-write for two reasons. 
        // 1) the read-write branch is typically local and thus
        // doing access check is cheap; 2) I never seen a configuration
        // where multiple readonly file systems are unioned with
        // a read-write branch so it seems to safe just to assume 
        // that the file exists in the other one and only one 
        // read-only branch. But for the case that this assumption is
        // wrong, I'am adding a debug statement.    
        //
        if (branches.size() != 2) {
            MPA_sayMessage("MountPointAttr", 
                false, 
                "AUFS unions more than two branches!");
            MPA_sayMessage("MountPointAttr", 
                false, 
                "Remaining logic will work incorrectly.");
        }
       
        bool foundInRw = false;
        FGFSInfoAnswer isRwRemote, isRoRemote;
        MyMntEnt rwEntry, roEntry;
        for (bIter = branches.begin(); bIter != branches.end(); ++bIter) {

            if ((*bIter).um_perm == "rw") {
                std::string testPath;
                // check if the file exists in the branch 
                if ((*bIter).um_branch[(*bIter).um_branch.length()-1] != '/') {
                    testPath = (*bIter).um_branch + std::string("/") + pathSuffix;
                } 
                else {
                    testPath = (*bIter).um_branch + pathSuffix;
                }
               
                //
                // This access call shouldn't have scalability problem 
                //
                if (access(testPath.c_str(), F_OK) == 0) {
                    foundInRw = true;
                    isRwRemote = isRemoteFileSystem((*bIter).um_branch.c_str(), 
                                                    rwEntry);
                }
            }
            else if (((*bIter).um_perm == "ro") || ((*bIter).um_perm == "rr")) {
                isRoRemote = isRemoteFileSystem((*bIter).um_branch.c_str(),
                                                roEntry);     
            }
            else {
                MPA_sayMessage("MountPointAttr", 
                    true, 
                    "Unknown branch permission.");
            }
        }

        FGFSInfoAnswer retAns;
        if (foundInRw) {
            retAns = isRwRemote;
            rwEntry.dir_master = myEntry.dir_master;
            result = rwEntry;
        }
        else {
            retAns = isRoRemote;
            roEntry.dir_master = myEntry.dir_master;
            result = roEntry;
        }

        return retAns;
    }
    catch (std::out_of_range &e) {
        return ans_error;
    }

}


UriScheme * 
MountPointInfo::createUriSchemeInstance(FileSystemType fst)
{
    UriScheme *rObj;
    //
    // Note: Following needs to be extended to support 
    // any new file system type.
    //
    switch (fst) {
    case fs_nfs:
    case fs_nfs4:
    case fs_aufs:
        {
            //
            // fs_aufs belongs here because this method should 
            // only be called after path is determined to be
            // served remotely.
            // 
            rObj = new NfsUriScheme();  
            break; 
        }
    case fs_lustre:
        {
            rObj = new LustreUriScheme(); 
            break;
        }
    case fs_gpfs:
        {
            rObj = new GpfsUriScheme(); 
            break;
        }
    case fs_panfs:
        {
            rObj = new PanfsUriScheme(); 
            break;
        }
    case fs_plfs:
        {
            rObj = new PlfsUriScheme(); 
            break;
        }
    case fs_dvs:
        {
            rObj = new DvsUriScheme(); 
            break;
        }
    case fs_cifs:
        {
            rObj = new CifsUriScheme(); 
            break;
        }
    case fs_smbfs:
        {
            rObj = new SmbUriScheme(); 
            break;
        }
    default:
        {
            if (ChkVerbose(0)) {
                MPA_sayMessage("MountPointAttr", true, 
                    "localUriScheme part must not be exercised!");
            }
            rObj = new LocalUriScheme();
            break;
        }
    }
 
    return rObj;
}

