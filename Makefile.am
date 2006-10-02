# $Id: Makefile.am,v 1.5 2005-12-07 13:56:05 ianb Exp $

AUTOMAKE_OPTIONS = 1.9

EXTRA_DIST = PLATFORMS PROBLEMS LSM.in version version.h.in\
	     git.spec.in mkvcs prepare 
SUBDIRS = lib src term doc 

ACLOCAL_AMFLAGS = -I m4

html:
	cd doc && $(MAKE) html

dist-hook:
	cp LSM git.spec $(distdir) 
