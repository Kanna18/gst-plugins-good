/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2009> STEricsson <benjamin.gaignard@stericsson.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_QTDEMUX_DUMP_H__
#define __GST_QTDEMUX_DUMP_H__

#include <gst/gst.h>
#include <mmtdemux.h>

G_BEGIN_DECLS
    gboolean mmtdemux_dump_mvhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_tkhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_elst (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_mdhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_hdlr (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_vmhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_dref (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stsd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stts (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stss (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stps (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stsc (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stsz (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_stco (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_co64 (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_dcom (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_cmvd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_ctts (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_cslg (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_mfro (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_mfhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_tfra (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_tfhd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_trun (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_trex (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_mehd (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_sdtp (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_tfdt (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_unknown (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_svmi (GstMMTDemux *mmtdemux, GstByteReader *data,
    int depth);
gboolean mmtdemux_dump_dfLa (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_fLaC (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);
gboolean mmtdemux_dump_gmin (GstMMTDemux * mmtdemux, GstByteReader * data,
    int depth);

gboolean mmtdemux_node_dump (GstMMTDemux * mmtdemux, GNode * node);

G_END_DECLS
#endif /* __GST_QTDEMUX_DUMP_H__ */
