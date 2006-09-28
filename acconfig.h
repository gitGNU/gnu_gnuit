/* $Id: acconfig.h,v 1.3 1999/01/16 22:37:15 tudor Exp $ */

/* Define if you want to disable various consistency checkings.  */
#undef NDEBUG

/* Define if you have GCC.  */
#undef HAVE_GCC

/* Define if you have a termcap library.  */
#undef HAVE_LIBTERMCAP

/* Define if you have a terminfo library.  */
#undef HAVE_LIBTERMINFO

/* Define if your system is Linux.  */
#undef HAVE_LINUX

/* Define if you have a dumb C compiler, like the one running on B.O.S.  */
#undef HAVE_DUMB_CC

/* Define if you have POSIX compatible terminal interface.  */
#undef HAVE_POSIX_TTY

/* Define if you have System V compatible terminal interface.  */
#undef HAVE_SYSTEMV_TTY

/* Define if you have BSD compatible terminal interface.  */
#undef HAVE_BSD_TTY

/* Define if you have the utsname system call.  */
#undef HAVE_UTSNAME

/* Define if you have the TIOCGWINSZ ioctl system call.  */
#undef HAVE_WINSZ

/* Define if you have two-argument statfs with statfs.bsize member
   (AIX, 4.3BSD).  */
#undef STAT_STATFS2_BSIZE

/* Define if you have two-argument statfs with statfs.fsize member
   (4.4BSD and NetBSD).  */
#undef STAT_STATFS2_FSIZE

/* Define if you have two-argument statfs with struct fs_data (Ultrix).  */
#undef STAT_STATFS2_FS_DATA

/* Define if you have 3-argument statfs function (DEC OSF/1).  */
#undef STAT_STATFS3_OSF1

/* Define if you have four-argument statfs (AIX-3.2.5, SVR3).  */
#undef STAT_STATFS4

/* Define if you have the statvfs system call.  */
#undef STAT_STATVFS

/* Define if you have a broken sys/filesys.h header file.  */
#undef BROKEN_SYS_FILSYS_H

/* Define this to the name of the package.  */
#undef PACKAGE

/* Define this to the version of the package.  */
#undef VERSION


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */
