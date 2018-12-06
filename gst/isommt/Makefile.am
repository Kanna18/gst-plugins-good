
plugin_LTLIBRARIES = libgstisommt.la

libgstisommt_la_CFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_BASE_CFLAGS) $(GST_CFLAGS)
libgstisommt_la_LIBADD = \
    $(GST_PLUGINS_BASE_LIBS) \
    -lgstriff-@GST_API_VERSION@ \
    -lgstaudio-@GST_API_VERSION@ \
    -lgstvideo-@GST_API_VERSION@ \
    -lgstrtp-@GST_API_VERSION@ \
    -lgsttag-@GST_API_VERSION@ \
    -lgstpbutils-@GST_API_VERSION@ \
    $(GST_BASE_LIBS) $(GST_LIBS) $(ZLIB_LIBS)
libgstisommt_la_LDFLAGS = ${GST_PLUGIN_LDFLAGS}
libgstisommt_la_SOURCES = isommt-plugin.c gstrtpxqtdepay.c \
	mmtdemux.c mmtdemux_types.c mmtdemux_dump.c mmtdemux_lang.c \
	gstqtmux.c gstqtmoovrecover.c atoms.c MMTBitstream.c atomsrecovery.c descriptors.c \
	properties.c gstqtmuxmap.c gstisoff.c MMTPacket.c mmtsignal.c process_mfu.c

presetdir = $(datadir)/gstreamer-$(GST_API_VERSION)/presets
preset_DATA = GstMMTMux.prs

noinst_HEADERS = \
	qtatomparser.h   \
	mmtdemux.h        \
	mmtdemux_types.h  \
	mmtdemux_debug.h  \
	mmtdemux_dump.h   \
	mmtdemux_lang.h   \
	qtpalette.h      \
	gstrtpxqtdepay.h \
	gstqtmux.h       \
	gstqtmoovrecover.h \
	atoms.h          \
	atomsrecovery.h  \
	descriptors.h    \
	properties.h     \
	fourcc.h         \
	gstisoff.h       \
	gstqtmuxmap.h   \ 
        MMTTables.h      \
        MMTPacket.h     \
        MMTBitstream.h   
			atoms.h \
			mmtsignal.h \
			parse.h

EXTRA_DIST = \
	gstqtmux-doc.c \
	gstqtmux-doc.h \
	$(preset_DATA)
