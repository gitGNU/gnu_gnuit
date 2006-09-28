# $Id: Makefile.am,v 1.4 2000/03/12 14:03:10 tudor Exp $
EXTRA_DIST = PLATFORMS PROBLEMS LSM.in version version.h.in\
	     git.spec.in mkvcs prepare
SUBDIRS = src term doc

html:
	cd doc && $(MAKE) html

dist-hook:
	rm -f $(distdir)/TODO
	cp LSM git.spec $(distdir) 
