mp4_sources = [
  'isommt-plugin.c',
  'gstrtpxqtdepay.c',
  'mmtdemux.c',
  'mmtdemux_types.c',
  'mmtdemux_dump.c',
  'mmtdemux_lang.c',
  'gstisoff.c',
  'gstqtmoovrecover.c',
  'atoms.c',
  'atomsrecovery.c',
  'descriptors.c',
  'properties.c',
  'MMTPacket.c',
  'process_mfu.c',
  'mmtsignal.c',
  'gstqtmuxmap.c'
]

gstismmt = library('gstisommt',
  mp4_sources,
  c_args : gst_plugins_good_args,
  link_args : noseh_link_args,
  include_directories : [configinc, libsinc],
  dependencies : [gst_dep, gstriff_dep, gstaudio_dep, gstvideo_dep,
                  gstrtp_dep, gsttag_dep, gstpbutils_dep, zlib_dep],
  install : true,
  install_dir : plugins_install_dir,
)

install_data(sources: 'GstMMTMux.prs', install_dir: presetdir)
