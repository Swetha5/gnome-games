bin_PROGRAMS = swell-foop

swell_foop_SOURCES = \
	game-view.vala \
	game.vala \
	history.vala \
	config.vapi \
	swell-foop.vala

swell_foop_VALAFLAGS = \
	--pkg posix \
	--pkg gmodule-2.0 \
	--pkg clutter-gtk-1.0

swell_foop_CFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
	-DDATADIR=\""$(datadir)/swell-foop"\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	$(GHTREAD_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(GTK_CFLAGS) \
	$(CLUTTER_GTK_CFLAGS)

swell_foop_LDADD =	\
	$(GTHREAD_LIBS)	\
	$(GMODULE_LIBS) \
	$(GTK_LIBS)	\
	$(CLUTTER_GTK_LIBS)	\
	$(INTLLIBS)

-include $(top_srcdir)/git.mk
