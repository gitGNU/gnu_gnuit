# $Id: Makefile.am,v 1.4 1999/04/26 00:28:00 tudor Exp $
EXTRA_DIST = .gitrc.aixterm .gitrc.ansi .gitrc.common.in\
	     .gitrc.generic .gitrc.hft .gitrc.hpterm .gitrc.linux\
	     .gitrc.mach .gitrc.minix .gitrc.thix .gitrc.vt100\
	     .gitrc.vt102 .gitrc.xterm .gitrc.xterm-color
pkgdata_DATA = .gitrc.aixterm .gitrc.ansi .gitrc.common\
	       .gitrc.generic .gitrc.hft .gitrc.hpterm .gitrc.linux\
	       .gitrc.mach .gitrc.minix .gitrc.thix .gitrc.vt100\
	       .gitrc.vt102 .gitrc.xterm .gitrc.xterm-color

GIT_GENERICS	 = .gitrc.sun .gitrc.sun-cmd .gitrc.eterm .gitrc.pc3
GIT_XTERMS	 = .gitrc.xterms
GIT_ANSIS	 = .gitrc.iris-ansi .gitrc.iris-ansi-net
GIT_COLOR_XTERMS = .gitrc.dtterm .gitrc.rxvt .gitrc.xterm-debian
GIT_LINUXS	 = .gitrc.console
GIT_HPTERMS	 = .gitrc.hp
GIT_VT100S	 = .gitrc.vt125 .gitrc.vt200 .gitrc.vt201 .gitrc.vt220\
		   .gitrc.vt240 .gitrc.vt300 .gitrc.vt320 .gitrc.vt400\
		   .gitrc.vt420 .gitrc.screen

install-data-hook:
	for i in $(GIT_XTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.xterm $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_COLOR_XTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.xterm-color $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_HPTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.hpterm $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_GENERICS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.generic $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_VT100S);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.vt100 $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_ANSIS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.ansi $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_LINUXS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	    ln $(datadir)/git/.gitrc.linux $(datadir)/git/$$i;\
	done

uninstall-local:
	for i in $(GIT_XTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_COLOR_XTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_HPTERMS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_GENERICS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_VT100S);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_ANSIS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_LINUXS);\
	do\
	    rm -f $(datadir)/git/$$i;\
	done
