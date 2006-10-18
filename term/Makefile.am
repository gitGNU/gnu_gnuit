# $Id: Makefile.am,v 1.2 2005-12-07 13:56:05 ianb Exp $
EXTRA_DIST = .gitrc.aixterm.in .gitrc.ansi.in .gitrc.common.in\
	     .gitrc.generic.in .gitrc.hft.in .gitrc.hpterm.in .gitrc.linux.in\
	     .gitrc.mach.in .gitrc.minix.in .gitrc.thix.in .gitrc.vt100.in\
	     .gitrc.vt102.in .gitrc.xterm.in .gitrc.xterm-color.in .gitrc.linux-m.in
pkgdata_DATA = .gitrc.aixterm .gitrc.ansi .gitrc.common\
	       .gitrc.generic .gitrc.hft .gitrc.hpterm .gitrc.linux\
	       .gitrc.mach .gitrc.minix .gitrc.thix .gitrc.vt100\
	       .gitrc.vt102 .gitrc.xterm .gitrc.xterm-color .gitrc.linux-m

GIT_GENERICS	 = .gitrc.sun .gitrc.sun-cmd .gitrc.eterm .gitrc.pc3
GIT_XTERMS	 = .gitrc.xterms
GIT_ANSIS	 = .gitrc.iris-ansi .gitrc.iris-ansi-net .gitrc.cygwin
GIT_COLOR_XTERMS = .gitrc.dtterm .gitrc.rxvt .gitrc.xterm-debian .gitrc.Eterm .gitrc.rxvt-unicode
GIT_LINUXS	 = .gitrc.console
GIT_HPTERMS	 = .gitrc.hp
GIT_VT100S	 = .gitrc.vt125 .gitrc.vt200 .gitrc.vt201 .gitrc.vt220\
		   .gitrc.vt240 .gitrc.vt300 .gitrc.vt320 .gitrc.vt400\
		   .gitrc.vt420 .gitrc.screen

install-data-hook:
	for i in $(GIT_XTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.xterm $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_COLOR_XTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.xterm-color $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_HPTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.hpterm $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_GENERICS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.generic $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_VT100S);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.vt100 $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_ANSIS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.ansi $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_LINUXS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	    ln -s $(datadir)/git/.gitrc.linux $(DESTDIR)$(datadir)/git/$$i;\
	done

uninstall-local:
	for i in $(GIT_XTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_COLOR_XTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_HPTERMS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_GENERICS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_VT100S);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_ANSIS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
	\
	for i in $(GIT_LINUXS);\
	do\
	    rm -f $(DESTDIR)$(datadir)/git/$$i;\
	done
