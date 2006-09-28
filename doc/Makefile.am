# $Id: Makefile.am,v 1.7 2000/03/09 23:16:54 tudor Exp $
info_TEXINFOS = git.texinfo
man_MANS = git.1 gitaction.1 gitkeys.1 gitmount.1 gitrgrep.1 gitunpack.1
htmldir = $(prefix)/doc/git-$(VERSION)
html_DATA = git.html
EXTRA_DIST = git.1 gitaction.1 gitkeys.1 gitmount.1 gitrgrep.1 gitunpack.1\
	     git.html

html:
	cd $(srcdir) &&\
	texi2html -expandinfo -glossary -monolithic $(srcdir)/git.texinfo

dist-hook: html
