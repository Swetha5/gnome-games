bin_PROGRAMS = lightsoff

lightsoff_SOURCES = \
	board-view.vala \
	config.vapi \
	fixes.vapi \
	lightsoff.vala \
	led-array.vala \
	puzzle-generator.vala \
	game-view.vala

lightsoff_VALAFLAGS = \
	--pkg posix \
	--pkg gmodule-2.0 \
	--pkg clutter-gtk-1.0

lightsoff_CFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
	-DDATADIR=\""$(datadir)/lightsoff"\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	$(GHTREAD_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(GTK_CFLAGS) \
	$(CLUTTER_GTK_CFLAGS)

lightsoff_LDADD =	\
	$(GMODULE_LIBS) \
	$(GTK_LIBS)	\
	$(CLUTTER_GTK_LIBS)	\
	$(INTLLIBS)

CLEANFILES = \
	$(patsubst %.vala,%.c,$(filter %.vala, $(SOURCES))) \
	*_vala.stamp

-include $(top_srcdir)/git.mk
