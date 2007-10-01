AUTOMAKE_OPTIONS = 1.9

EXTRA_DIST = PLATFORMS PROBLEMS LSM.in version version.h.in\
	     git.spec.in mkvcs prepare \
	     lib/tilde.c lib/tilde.h lib/history.c lib/history.h \
	     m4/gnulib-tool.m4 m4/gnulib-cache.m4 \
	     m4/absolute-header.m4 m4/gnulib-common.m4

SUBDIRS = lib src term doc

ACLOCAL_AMFLAGS = -I m4

html:
	cd doc && $(MAKE) html

dist-hook:
	cp LSM git.spec $(distdir)
