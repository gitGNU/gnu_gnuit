# $Id: Makefile.am,v 1.5 2005-12-07 13:56:05 ianb Exp $
info_TEXINFOS = git.texinfo
man_MANS = gitfm.1 gitaction.1 gitkeys.1 gitmount.1 gitrgrep.1 gitunpack.1 \
           git.transition.1 gitps.1 gitinternal.1 gitwipe.1 gitview.1
infodir = $(prefix)/share/info
htmldir = $(prefix)/share/doc/git-$(VERSION)
BUILT_SOURCES = git.html
html_DATA = git.html
EXTRA_DIST = gitfm.1 gitaction.1 gitkeys.1 gitmount.1 gitrgrep.1 gitunpack.1\
	     git.html git.transition.1 gitps.1 gitview.1 gitwipe.1 gitinternal.1

GIT_INTERNAL_MANS=gitwhich gitmkdirs gitxgrep
GIT_GREPS=gitregrep gitrfgrep

#AM_MAKEINFOHTMLFLAGS = --no-split
#dist-hook: html

git.html:
	-make html
html:
if HTMLDOCS_TEXI2HTML
	texi2html -expandinfo $(srcdir)/git.texinfo
else
if HTMLDOCS_MAKEINFO
	makeinfo --html --no-split -o git.html  $(srcdir)/git.texinfo
else
	echo Not building html documentation: cannot find texi2html or makeinfo
endif
endif

dist-hook: html

install-data-hook:
	for i in $(GIT_GREPS);\
	do\
	    rm -f $(DESTDIR)$(mandir)/man1/$${i}.1;\
	    ln -s $(mandir)/man1/gitrgrep.1 $(DESTDIR)$(mandir)/man1/$${i}.1;\
	done
	\
	for i in $(GIT_INTERNAL_MANS);\
	do\
	    rm -f $(DESTDIR)$(mandir)/man1/$${i}.1;\
	    ln -s $(mandir)/man1/gitinternal.1 $(DESTDIR)$(mandir)/man1/$${i}.1;\
	done


uninstall-local:
	-rmdir $(DESTDIR)$(htmldir)
	for i in $(GIT_GREPS);\
	do\
	    rm -f $(DESTDIR)$(mandir)/man1/$${i}.1;\
	done
	\
	for i in $(GIT_INTERNAL_MANS);\
	do\
	    rm -f $(DESTDIR)$(mandir)/man1/$${i}.1;\
	done
