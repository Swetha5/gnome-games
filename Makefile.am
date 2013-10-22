ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = $(gamelist) po

DIST_SUBDIRS = po $(allgames) $(staginggames)

DISTCHECK_CONFIGURE_FLAGS = \
	--with-platform=gnome \
	--disable-schemas-install \
	--disable-scrollkeeper \
	--disable-silent-rules \
	--disable-introspection

EXTRA_DIST = \
	autogen.sh \
	COPYING-DOCS \
	MAINTAINERS	\
	HACKING		 \
	ChangeLog

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

MAINTAINERCLEANFILES = \
	ChangeLog \
	$(srcdir)/INSTALL \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/autoscan.log \
	$(srcdir)/compile \
	$(srcdir)/py-compile \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/configure \
	$(srcdir)/configure.scan \
	$(srcdir)/depcomp \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs \
	$(srcdir)/po/Makefile.in.in \
	`find "$(srcdir)" -type f -name Makefile.in -print`

# Ignore score files, these *should* be left behind.
distuninstallcheck_listfiles = find . -type f -print | grep -v '.scores' | grep-v scrollkeeper

# Build ChangeLog from git log
ChangeLog:
	$(AM_V_GEN) if test -f $(top_srcdir)/.git/HEAD; then \
		git log --stat --no-color --since=2008-06-21 > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

-include $(top_srcdir)/git.mk
