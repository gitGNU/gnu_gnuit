dnl #####################################################################
dnl #          Autoconf input file for GNU Interactive Tools            #
dnl #      Copyright (C) 1993-2003 Free Software Foundation, Inc.       #
dnl #####################################################################


dnl Initialize the autoconf & automake stuff.
AC_REVISION($Revision: 1.7 $)
AC_INIT(git, 4.3.21, tudor@hulubei.net)
AC_CONFIG_SRCDIR([config.h.in])
AM_INIT_AUTOMAKE
AC_PROG_MAKE_SET
AM_SANITY_CHECK
AM_CONFIG_HEADER([config.h])
AC_PREREQ(2.54)
AC_CANONICAL_HOST
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_REQUIRE_CPP
HOST=$host

AH_TOP(
    [
/* Include the header file containing the version of the package.  */
#include <version.h>

/* Define to get the GNU functionality in the C header files.  */
#define _GNU_SOURCE
    ])

AH_BOTTOM(
    [
/* Define to get the Unix 64bit I/O API (Linux/glibc only).  */
#ifdef HAVE_64BIT_IO
#define _LARGEFILE64_SOURCE
#endif

/* See the file PROBLEMS for details on this.  */
#ifdef HAVE_LINUX
#undef HAVE_LSTAT
#define HAVE_LSTAT 1
#endif

/* Support for O_BINARY, required for CYGWIN.  */
#ifndef O_BINARY
#define O_BINARY 0
#endif
    ])

dnl Check the command line arguments.
AC_ARG_WITH(
    terminfo,
    [  --with-terminfo         use terminfo insted of termcap],
    TERMINFO="$withval",
    TERMINFO=no)

dnl Check for SMP compilation.
AC_ARG_ENABLE(
    smp,
    [  --enable-smp            enable parallel compilation (Linux only)],
    SMP="$enableval",
    SMP=no)

dnl Check the system type.  We are especially looking for Linux.
MAKE="make"
if test "$host_os" = "linux-gnu"; then
    AC_DEFINE([HAVE_LINUX], 1, [Define if your system Linux.])
    if test "$SMP" = "yes"; then
        AC_CACHE_CHECK(
	    [for the number of processors],
            [ac_cv_processors],
            [ac_cv_processors="1"
             if test -f /proc/cpuinfo; then
                 ac_cv_processors=`cat /proc/cpuinfo |\
                     grep processor | wc -l | sed 's/ //g'`
             fi])
        if test "$ac_cv_processors" -ge 2; then
            MAKE="make -j`expr $ac_cv_processors + 1`"
        fi
    fi
fi

AC_ARG_ENABLE(
        gcc-warnings,
        [  --enable-gcc-warnings   more gcc warnings for debugging purposes],
        GCC_WARNINGS="$enableval",
        GCC_WARNINGS=no)

AC_ARG_ENABLE(
        efence,
        [  --enable-efence         link against the Electric Fence library],
        EFENCE="$enableval",
        EFENCE=no)

dnl Increase the warnings level if using GNU C.
dnl We don't pass -ansi here, since it disables 64bit I/O.
if test "$CC" = "gcc"; then
    AC_DEFINE([HAVE_GCC], 1, [Define if the compiler used is GNU CC.])
    if test "$GCC_WARNINGS" = "yes"; then
        CFLAGS="$CFLAGS -W -Wall \
-Waggregate-return -Wbad-function-cast -fstrict-aliasing \
-Wtraditional -Winline -Wpointer-arith -Wcast-qual"
    else
        AC_DEFINE(
	    [NDEBUG], 1,
	    [Define if you want to disable various consistency checkings.])
    fi
else
    AC_DEFINE(
	[NDEBUG], 1,
	[Define if you want to disable various consistency checkings.])
fi

dnl Use the Electric Fence library for debugging purposes.
if test "$EFENCE" = "yes"; then
    LIBS="$LIBS -lefence"
fi

dnl Check for some UNIX variants.  These ones need special treatement
dnl due to exceptional oddities in their header files or libraries.
AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_CHECK_LIB(sun, getpwnam)


dnl Check for standard C as well as system dependent header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(
    unistd.h stdlib.h string.h memory.h values.h\
    sys/param.h sys/mount.h sys/vfs.h sys/filsys.h fcntl.h\
    sys/statfs.h sys/dustat.h sys/statvfs.h sys/sioctl.h stddef.h)

dnl Unisys has a broken sys/filsys.h.  We don't want to include it,
dnl considering the fact that other methods are available.
AC_CACHE_CHECK(
    [for broken sys/filsys.h],
    [ac_cv_broken_sys_filsys_h],
    [AC_TRY_LINK(
	[
#include <sys/types.h>
#ifdef HAVE_SYS_FILSYS_H
#include <sys/filsys.h>
#endif
	],
        [{}],
        ac_cv_broken_sys_filsys_h=no,
        ac_cv_broken_sys_filsys_h=yes)])
if test $ac_cv_broken_sys_filsys_h = yes; then
    AC_DEFINE(
	[BROKEN_SYS_FILSYS_H], 1,
        [Define if you have a broken sys/filesys.h header file.])
fi

dnl Check for system specific directory functions declarations.
AC_HEADER_DIRENT

dnl Check whether the C compiler do support the 'const' keyword.
AC_C_CONST

dnl Check for predefined C types.  Fix them if necessary...
AC_TYPE_SIGNAL
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

dnl Check how to include time.h and sys/time.h.
AC_CHECK_HEADERS(sys/time.h)
AC_HEADER_TIME

dnl Check whether the system supports long file names.  GIT will issue
dnl a warning at run time if long file names are not supported.  Really!?
AC_SYS_LONG_FILE_NAMES

dnl Check the sys/stat.h macros.
AC_HEADER_STAT

dnl Check for a POSIX.1 compatible sys/wait.h.
AC_HEADER_SYS_WAIT

dnl Check for the existence of various functions.
AC_CHECK_FUNCS([\
    lstat sigaction strerror strcasecmp strncasecmp\
    strstr memmove putenv setenv getcwd rename readlink])

dnl This configure.in code has been stolen from GNU fileutils-3.12.
dnl Its purpose is detecting a method to get file system information.
space=no

# Here we'll compromise a little (and perform only the link test)
# since it seems there are no variants of the statvfs function.
if test $space = no; then
    # SVR4
    AC_CHECK_FUNCS(statvfs)
    if test $ac_cv_func_statvfs = yes; then
        space=yes
        AC_DEFINE(
	    [STAT_STATVFS], 1,
            [Define if you have the statvfs system call.])
    fi
fi

if test $space = no; then
    # DEC Alpha running OSF/1
    AC_CACHE_CHECK(
	[for 3-argument statfs function (DEC OSF/1)],
        [fu_cv_sys_stat_statfs3_osf1],
        [AC_TRY_RUN(
	    [
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
main()
{
    struct statfs fsd;
    fsd.f_fsize = 0;
    exit(statfs(".", &fsd, sizeof(struct statfs)));
}
	    ],
	    fu_cv_sys_stat_statfs3_osf1=yes,
	    fu_cv_sys_stat_statfs3_osf1=no,
	    fu_cv_sys_stat_statfs3_osf1=no)])
    if test $fu_cv_sys_stat_statfs3_osf1 = yes; then
        space=yes
        AC_DEFINE(
	    [STAT_STATFS3_OSF1], 1,
            [Define if you have 3-argument statfs function (DEC OSF/1).])
    fi
fi

if test $space = no; then
    # AIX
    AC_CACHE_CHECK(
	[for two-argument statfs with statfs.bsize member (AIX, 4.3BSD)],
        [fu_cv_sys_stat_statfs2_bsize],
        [AC_TRY_RUN(
	    [
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif
main()
{
    struct statfs fsd;
    fsd.f_bsize = 0;
    exit(statfs(".", &fsd));
}
	    ],
	    fu_cv_sys_stat_statfs2_bsize=yes,
	    fu_cv_sys_stat_statfs2_bsize=no,
	    fu_cv_sys_stat_statfs2_bsize=no)])
    if test $fu_cv_sys_stat_statfs2_bsize = yes; then
	space=yes
	AC_DEFINE(
	    [STAT_STATFS2_BSIZE], 1,
            [Define if you have two-argument statfs with statfs.bsize member (AIX, 4.3BSD).])
    fi
fi

if test $space = no; then
    # SVR3
    AC_CACHE_CHECK(
	[for four-argument statfs (AIX-3.2.5, SVR3)],
        [fu_cv_sys_stat_statfs4],
        [AC_TRY_RUN(
	    [
#include <sys/types.h>
#ifdef HAVE_SYS_STATFS_H
#include <sys/statfs.h>
#endif
main()
{
    struct statfs fsd;
    exit(statfs(".", &fsd, sizeof fsd, 0));
}
	    ],
	    fu_cv_sys_stat_statfs4=yes,
	    fu_cv_sys_stat_statfs4=no,
	    fu_cv_sys_stat_statfs4=no)])
    if test $fu_cv_sys_stat_statfs4 = yes; then
	space=yes
	AC_DEFINE(
	    [STAT_STATFS4], 1,
            [Define if you have four-argument statfs (AIX-3.2.5, SVR3).])
    fi
fi

if test $space = no; then
    # 4.4BSD and NetBSD
    AC_CACHE_CHECK(
	[for two-argument statfs with statfs.fsize member (4.4BSD and NetBSD)]
        [fu_cv_sys_stat_statfs2_fsize],
        [AC_TRY_RUN(
	    [
#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
main()
{
    struct statfs fsd;
    fsd.f_fsize = 0;
    exit(statfs(".", &fsd));
}
	    ],
	fu_cv_sys_stat_statfs2_fsize=yes,
	fu_cv_sys_stat_statfs2_fsize=no,
	fu_cv_sys_stat_statfs2_fsize=no)])
    if test $fu_cv_sys_stat_statfs2_fsize = yes; then
        space=yes
        AC_DEFINE(
	    [STAT_STATFS2_FSIZE], 1,
            [Define if you have two-argument statfs with statfs.fsize member (4.4BSD and NetBSD).])
    fi
fi

if test $space = no; then
    # Ultrix
    AC_CACHE_CHECK(
        [for two-argument statfs with struct fs_data (Ultrix)],
        [fu_cv_sys_stat_fs_data],
        [AC_TRY_RUN(
	    [
#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_FS_TYPES_H
#include <sys/fs_types.h>
#endif
main()
{
    struct fs_data fsd;
    /* Ultrix's statfs returns 1 for success,
       0 for not mounted, -1 for failure.  */
    exit(statfs(".", &fsd) != 1);
}
	    ],
	    fu_cv_sys_stat_fs_data=yes,
	    fu_cv_sys_stat_fs_data=no,
	    fu_cv_sys_stat_fs_data=no)])
    if test $fu_cv_sys_stat_fs_data = yes; then
	space=yes
	AC_DEFINE(
	    [STAT_STATFS2_FS_DATA], 1,
            [Define if you have two-argument statfs with struct fs_data (Ultrix).])
    fi
fi


dnl If the GNU readline library is installed on the system, we will use
dnl its tilde_expand() and history functions.  Otherwise, we have to use
dnl the versions of tilde.c and history.c included in the package.  These
dnl files belong to the GNU readline library version 2.0.1.
#AC_CHECK_LIB(readline, tilde_expand)
#AC_CHECK_FUNC(tilde_expand,, AC_LIBOBJ([tilde]))
#AC_CHECK_FUNC(using_history,, AC_LIBOBJ([history]))

dnl Check for compilers that do not support constructs combinations
dnl of ANSI C like prototypes & K&R definitions.  B.O.S. systems.
AC_CACHE_CHECK(
    [for dumb C compiler],
    [git_cv_dumb_cc],
    [AC_TRY_LINK(
        [void tst(char); void tst(i) char i; {}],
        [{ tst('A'); } ],
        git_cv_dumb_cc=no,
        git_cv_dumb_cc=yes)])
if test "$git_cv_dumb_cc" = "yes"; then
    AC_DEFINE(
	[HAVE_DUMB_CC], 1,
        [Define if you have a dumb C compiler (e.g. B.O.S.).])
fi

dnl Check if struct utsname is available.
AC_CACHE_CHECK(
    [for struct utsname],
    [git_cv_struct_utsname],
    [AC_TRY_LINK(
	[#include <sys/utsname.h>],
        [{
	    char *ptr;
	    struct utsname u;
	    uname(&u);
	    ptr = u.nodename;
	    ptr = u.sysname;
	    ptr = u.machine;
	}],
        git_cv_struct_utsname=yes,
        git_cv_struct_utsname=no)])
if test "$git_cv_struct_utsname" = "yes"; then
    AC_DEFINE(
	[HAVE_UTSNAME], 1,
        [Define if you have the utsname system call.])
fi

dnl Check for POSIX compatible terminal interface.
AC_CACHE_CHECK(
    [for POSIX compatible terminal interface],
    [git_cv_sys_posix_tty],
    [AC_TRY_LINK(
	[#include <termios.h>],
        [{
	    struct termios term;
	    tcgetattr(0, &term);
	}],
        git_cv_sys_posix_tty=yes,
        git_cv_sys_posix_tty=no)])
if test "$git_cv_sys_posix_tty" = "yes"; then
    AC_DEFINE(
	[HAVE_POSIX_TTY], 1,
        [Define if you have POSIX compatible terminal interface.])
fi

dnl Check for System V compatible terminal interface.
AC_CACHE_CHECK(
    [for System V compatible terminal interface],
    [git_cv_sys_systemv_tty],
    [AC_TRY_LINK(
	[#include <termio.h>],
        [{
	    struct termio term;
	    ioctl(0, TCGETA, &term);
	}],
        git_cv_sys_systemv_tty=yes,
        git_cv_sys_systemv_tty=no)])
if test "$git_cv_sys_systemv_tty" = "yes"; then
    AC_DEFINE(
	[HAVE_SYSTEMV_TTY], 1,
        [Define if you have System V compatible terminal interface.])
fi

dnl Check for BSD compatible terminal interface.
AC_CACHE_CHECK(
    [for BSD compatible terminal interface],
    [git_cv_sys_bsd_tty],
    [AC_TRY_LINK(
	[#include <sgtty.h>],
        [{
            struct sgttyb arg;
            struct tchars targ;
            struct ltchars ltarg;

            ioctl(0, TIOCGETP,&arg);
            ioctl(0, TIOCGETC,&targ);
            ioctl(0, TIOCGLTC,&ltarg);
        }],
        git_cv_sys_bsd_tty=yes,
        git_cv_sys_bsd_tty=no)]
)
if test "$git_cv_sys_bsd_tty" = "yes"; then
    AC_DEFINE(
	[HAVE_BSD_TTY], 1,
        [Define if you have BSD compatible terminal interface.])
fi

dnl Check if ioctl(1, TIOCGWINSZ, &winsz) works.
AC_CACHE_CHECK(
    [for struct winsz],
    [git_cv_struct_winsz],
    [AC_TRY_LINK(
	[
#include <sys/types.h>
#include <sys/ioctl.h>
#ifdef HAVE_POSIX_TTY
#include <termios.h>
#else
#ifdef HAVE_SYSTEMV_TTY
#include <termio.h>
#else
#include <sgtty.h>
#endif /* HAVE_SYSTEMV_TTY */
#endif /* HAVE_POSIX_TTY */
#if defined TIOCGSIZE && !defined TIOCGWINSZ
#define TIOCGWINSZ TIOCGSIZE
#endif
	],
        [{
            struct winsize winsz;
            ioctl(1, TIOCGWINSZ, &winsz);
            winsz.ws_col = winsz.ws_row;
        }],
        git_cv_struct_winsz=yes,
        git_cv_struct_winsz=no)]
)
if test "$git_cv_struct_winsz" = "yes"; then
    AC_DEFINE(
	[HAVE_WINSZ], 1,
        [Define if you have the TIOCGWINSZ ioctl system call.])
fi

dnl Check the terminal capabilities library to use.
define(
    git_TERMINFO_LIBRARY,
    [
	AC_CHECK_LIB(
	    ncurses, tgetent,
	    AC_DEFINE(
		[HAVE_LIBTERMINFO], 1,
		[Define if you have a terminfo library.])
            LIBS="$LIBS -lncurses"
            TERMLIB_FOUND=yes,
            AC_CHECK_LIB(
		termlib, tgetent,
		AC_DEFINE(
		    [HAVE_LIBTERMINFO], 1,
		    [Define if you have a terminfo library.])
                LIBS="$LIBS -ltermlib"
                TERMLIB_FOUND=yes,
                AC_CHECK_LIB(
		    curses, tgetent,
		    AC_DEFINE(
			[HAVE_LIBTERMINFO], 1,
			[Define if you have a terminfo library.])
                    LIBS="$LIBS -lcurses"
                    TERMLIB_FOUND=yes,
                    AC_CHECK_LIB(
			tinfo, tgetent,
			AC_DEFINE(
			    [HAVE_LIBTERMINFO], 1,
			    [Define if you have a terminfo library.])
                        LIBS="$LIBS -ltinfo"
                        TERMLIB_FOUND=yes,
                        AC_CHECK_LIB(
			    terminfo, tgetent,
			    AC_DEFINE(
				[HAVE_LIBTERMINFO], 1,
				[Define if you have a terminfo library.])
                            LIBS="$LIBS -lterminfo"
                            TERMLIB_FOUND=yes,
                            TERMLIB_FOUND=no
                        )
                    )
                )
            )
        )
    ]
)

if test "$TERMINFO" = "yes"; then
    git_TERMINFO_LIBRARY
    if test "$TERMLIB_FOUND" = "no"; then
        AC_CHECK_LIB(
	    termcap, tgetent,
            AC_DEFINE(
		[HAVE_LIBTERMCAP], 1,
		[Define if you have  a termcap library.])
            LIBS="$LIBS -ltermcap"
            TERMLIB_FOUND="yes")
    fi
else
    AC_CHECK_LIB(
	termcap, tgetent,
        AC_DEFINE(
	    [HAVE_LIBTERMCAP], 1,
	    [Define if you have a termcap library.])
        LIBS="$LIBS -ltermcap"
        TERMLIB_FOUND="yes",
        git_TERMINFO_LIBRARY)
fi

if test "$TERMLIB_FOUND" = "no"; then
    AC_MSG_ERROR([no library for handling terminal capabilities])
fi

dnl Check for 64bit I/O.
AC_CACHE_CHECK(
    [for 64bit I/O support],
    [git_cv_64bit_io],
    [AC_TRY_LINK(
	[
#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
	],
        [{
            off64_t x;
            fopen64("x", "r");
        }],
        git_cv_64bit_io=yes,
        git_cv_64bit_io=no)]
)
if test "$git_cv_64bit_io" = "yes"; then
    AC_DEFINE(
        [HAVE_64BIT_IO], 1,
        [Define if you have the Unix 64bit I/O API.])
fi

dnl Check for the presence of the makeinfo utility.  If we can't find
dnl it, we can't build the info documentation.
AC_PROGRAM_CHECK(MAKEINFO, makeinfo, makeinfo, false)
if test "$MAKEINFO" = "false"; then
    AC_MSG_WARN(***** makeinfo utility not found *****)
fi

dnl Check for the presence of the texi2dvi utility.  If we can't find
dnl it, we can't build the dvi documentation.
AC_PROGRAM_CHECK(TEXI2DVI, texi2dvi, texi2dvi, false)
if test "$TEXI2DVI" = "false"; then
    AC_MSG_WARN(***** texi2dvi utility not found *****)
fi

dnl Check for the presence of the texi2html utility.  If we can't find
dnl it, we can't build the html documentation.
AC_PROGRAM_CHECK(TEXI2HTML, texi2html, texi2html, false)
if test "$TEXI2HTML" = "false"; then
    AC_MSG_WARN(***** texi2html utility not found *****)
fi

dnl According to Joachim Baumann, for Solaris 2.3 & Solaris 2.4 (at least)
dnl -lucb should be included after -ltermcap.  This hasn't been tested!
if test "$TERMINFO" = "no"; then
    if test "$host_os" = "solaris2.3" || test "$host_os" = "solaris2.4"; then
        AC_CHECK_LIB(ucb, tgetent, , , $LIBS)
    fi
fi

CLEANFILES="core tutu gogu *.out t tt mwatch.log mtrace.log autoscan.log autom4te.cache/*"

dnl Make final variable substitutions.
AC_SUBST(PRODUCT, "GNU Interactive Tools")
AC_SUBST(HOST)
AC_SUBST(MAKE)
AC_SUBST(INSTALL)
AC_SUBST(INSTALL_PROGRAM)
AC_SUBST(INSTALL_DATA)
AC_SUBST(CFLAGS)
AC_SUBST(CPP)
AC_SUBST(DEFS)
AC_SUBST(LIBS)
AC_SUBST(LDFLAGS)
AC_SUBST(INCLUDES)
AC_SUBST(CLEANFILES)

dnl Generate the Makefiles.
AC_OUTPUT([
    version.h
    Makefile
    src/Makefile
    term/Makefile
    term/.gitrc.common
    doc/Makefile
    git.spec
    LSM])
