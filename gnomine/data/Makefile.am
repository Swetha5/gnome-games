SUBDIRS = icons

gsettings_in_file = org.gnome.gnomine.gschema.xml.in
gsettings_SCHEMAS = $(gsettings_in_file:.xml.in=.xml)
@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

man_MANS = gnomine.6

pixmapdir   = $(datadir)/gnomine
pixmap_DATA = face-cool.svg face-sad.svg face-smile.svg face-win.svg \
	face-worried.svg \
	flag.svg flag-question.svg mine.svg bang.svg warning.svg

desktopdir = $(datadir)/applications
desktop_in_files = gnomine.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
             $(gsettings_in_file) \
             $(desktop_in_files) \
             $(man_MANS) \
             $(pixmap_DATA)

CLEANFILES = $(desktop_DATA) $(gsettings_SCHEMAS)
DISTCLEANFILES = $(desktop_DATA) $(gsettings_SCHEMAS)

-include $(top_srcdir)/git.mk
