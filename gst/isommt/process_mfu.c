/*********************************************************************************************
 *  License: This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: process_mfu.c
 *  Created on: 04-MAY-2018
 *  Author: Mohammed Safwan Sada 
 *******************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "pthread.h"
#include "MMTPacket.h"
#include "atoms.h"
#include "mmtdemux.h"
#include "parser.h"
#include "MMTBitstream.h"

#define ASSET_LENGTH 16
#define MAX_SIZE 4096
#define NUM_OF_STREAM 2

void process_mfus(GstMMTDemux * mmtdemux, unsigned char* data, int size, int mediaType, GstBuffer *gbuf)
{
	int i, offset, mfu_offset, is_muli;
	static int need_data[NUM_OF_STREAM], sample_size[NUM_OF_STREAM], count[NUM_OF_STREAM], sample_number[NUM_OF_STREAM];
	static unsigned int rem_size[NUM_OF_STREAM];
	static GstBuffer *databuffer;
	GstClockTime pts[2], dts[2];
	GstBuffer *mfu_buf = NULL;
	is_muli = offset = mfu_offset = 0;
	mmtdemux->mfu_offset = 0;
	for(i = 0; i < size; ++i)
	{
		if(memcmp(data+i,"muli",4) == 0)
		{
			count[mediaType] = 0;
			is_muli = 1;
			mfu_offset = i+Read32(data+(i-4))-4;
			mmtdemux->mfu_offset = mfu_offset;
			rem_size[mediaType] = size - mfu_offset;
			offset = i- 27;
			parse_mfu(data+offset, rem_size[mediaType]);
			mmtdemux->mfu_size = mfu_b.length;
			if(rem_size[mediaType] >= mfu_b.length) 
				need_data[mediaType] = 0;
			else
			{
				sample_size[mediaType] = mfu_b.length;
				sample_number[mediaType] = mfu_b.samplenumber;
				need_data[mediaType] = 1;
			}
			break;
		}
	}

	if(is_muli)
	{
		mmtdemux->ref_offset +=mfu_offset;
		if(gbuf)
		{
		   pts[mediaType] = gbuf->pts;
		   dts[mediaType] = gbuf->dts;
		  //skip 22 bytes of header from sample
			gst_buffer_resize(gbuf, mfu_offset+22, rem_size[mediaType]);
		}
		else
		{
			gst_mmtdemux_pull_atom (mmtdemux, mmtdemux->ref_offset, rem_size[mediaType], &mfu_buf);
		}
	}
	else
	{
		count[mediaType]++;
		if(databuffer && (rem_size[mediaType]+size > sample_size[mediaType]))
		{
			GST_DEBUG_OBJECT(mmtdemux," GGGSIZEE %d :: mfu_b.length: %d..................... count:%d", rem_size[mediaType]+size-14, sample_size[mediaType], count[mediaType]);
		}
		if(gbuf)
		{
		  //skip 14byte extra in samole packet & 22 bytes of header
			gst_buffer_resize(gbuf, 14+22, size-14);
		   //if(!mediaType && gbuf)
			//	fprintf(stderr, "LLLLL================ gbuf pts: %"G_GUINT64_FORMAT" count:%d \n", gbuf->pts, count[mediaType]);
		}
		else
		{
			GST_DEBUG_OBJECT(mmtdemux, "ELEMENTARY STREAM START: %x, size: %d", (int)mmtdemux->ref_offset+14, size-14);
			gst_mmtdemux_pull_atom (mmtdemux, mmtdemux->ref_offset+14, size-14, &mfu_buf);
		}
	}
	if(need_data[mediaType])
	{
		if(is_muli)
		{
			if(databuffer)
			{
				gst_mmtdemux_state_movie(mmtdemux, NULL, 0, mediaType, 0);
				gst_buffer_unref (databuffer);
				databuffer = NULL;
			}
		}
		if(databuffer == NULL)
		{
			databuffer = (gbuf != NULL?gbuf:mfu_buf);
		}
		else
		{
			databuffer = gst_buffer_append(databuffer, gbuf != NULL?gbuf:mfu_buf);
		}

		if(!is_muli)
			rem_size[mediaType] +=(size-14);	 

		GST_DEBUG_OBJECT(mmtdemux," SIZEEEE %d :: mfu_b.length: %d count:%d", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
		//fprintf(stderr," SIZEEEE %d :: mfu_b.length: %d count:%d\n", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
		if(rem_size[mediaType] == sample_size[mediaType])
		{
			GST_DEBUG_OBJECT(mmtdemux," UUUUSIZE %d :: mfu_b.length: %d..................... count:%d sample_number: %d", rem_size[mediaType], sample_size[mediaType], count[mediaType], sample_number[mediaType]);

			//databuffer->dts = dts[mediaType];
	//		if(gbuf)
	//			fprintf(stderr, "MMMMMMM================ gbuf pts: %"G_GUINT64_FORMAT" pts:%"G_GUINT64_FORMAT" dts: %"G_GUINT64_FORMAT"\n", gbuf->pts, databuffer->pts, databuffer->dts);
		   //databuffer->pts=pts[mediaType];
		   if(gbuf)
			{
				fprintf(stderr, "LLLLL================ gbuf pts: %"G_GUINT64_FORMAT" count:%d \n", gbuf->pts, count[mediaType]);
				databuffer->pts=pts[mediaType];
				databuffer->dts=dts[mediaType];
			}
			gst_mmtdemux_state_movie(mmtdemux, databuffer, 0, mediaType, sample_number[mediaType]);
			need_data[mediaType] = 0;
			databuffer = NULL;
		}
		else if(rem_size[mediaType] > sample_size[mediaType])
		{
			GST_DEBUG_OBJECT(mmtdemux," UNKNOWN PACKET..... UUUUSIZE %d :: mfu_b.length: %d..................... count:%d", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
			gst_mmtdemux_state_movie(mmtdemux, NULL, 0, mediaType, 0);
			gst_buffer_unref (databuffer);
			databuffer = NULL;
		}
	}
	else if (is_muli && (rem_size[mediaType] == mfu_b.length))
	{
		//fprintf (stderr,"STATE_MOVIE rendering... \n");
		   if(!mediaType && gbuf)
				fprintf(stderr, "MMMMMM================ gbuf pts: %"G_GUINT64_FORMAT" count:%d \n", gbuf->pts, count[mediaType]);
		gst_mmtdemux_state_movie(mmtdemux, gbuf != NULL?gbuf:mfu_buf, (guint64 )mfu_b.offset, mediaType, mfu_b.samplenumber);
	}
	else
	{
		GST_DEBUG_OBJECT (mmtdemux,"no mfu header found, skipping rendering... ");
		fprintf (stderr,"no mfu header found, skipping rendering...\n");
		if(gbuf) 
			gst_buffer_unref (gbuf);
		if(mfu_buf)
			gst_buffer_unref (mfu_buf);
		if(databuffer) 
		{
			gst_buffer_unref (databuffer);
			databuffer = NULL;
		}
	}
}


void parse_mfu(unsigned char *data, int length)
{
	unsigned char byte8;
	unsigned short byte16;
	unsigned int byte32;
	//mfu_box mfu_b;
	mfu_b.sequence_number = Read32(data);
	mfu_b.trackrefindex = Read8(data+4);
	mfu_b.movie_fragment_sequence_number = Read32(data+5);
	mfu_b.samplenumber = Read32(data+9);
	mfu_b.priority = Read8(data+13);
	mfu_b.dependency_counter = Read8(data+14);
	mfu_b.offset = Read32(data+15);
	mfu_b.length = Read32(data+19);

	GST_DEBUG_OBJECT (NULL," KKK sequence_number: %u, trackrefindex: %u, movie_fragment_sequence_number: %u, samplenumber: %u, priority: %u, dependency_counter: %u, offset: %u, length: %u\n\n", mfu_b.sequence_number, mfu_b.trackrefindex, mfu_b.movie_fragment_sequence_number, mfu_b.samplenumber, mfu_b.priority, mfu_b.dependency_counter, mfu_b.offset, mfu_b.length);

	byte8 = Read8(data+23);
	mfu_b.mul_linfo.multilayer_flag = byte8 & 0x01;
	if(mfu_b.mul_linfo.multilayer_flag)
	{
	   byte32 = Read32(data+24);
		mfu_b.mul_linfo.dependency_id = byte32 & 0x07;
		mfu_b.mul_linfo.depth_flag = byte32 & 0x08;
		mfu_b.mul_linfo.temporal_id = byte32 & 0x0700;
		mfu_b.mul_linfo.quality_id = byte32 & 0xF000;
		byte16 = Read16(data+28);
		mfu_b.mul_linfo.priority_id =  byte16 & 0x3F;
		mfu_b.mul_linfo.view_id = byte16 & 0xFFC;
	}
   else
	{
		byte16 = Read16(data+24);
		mfu_b.mul_linfo.layer_id = byte16 & 0x3F;
		mfu_b.mul_linfo.temporal_id = byte16 & 0x1E0;
	}
}


int pullatom(unsigned char *data, char *buf)
{
    unsigned int length = 0;
	 int offset = 0;
    length = Read32(data);
    //if(!length || (length > 4096))
    if(!length && (length > 4096))
	 {
		  GST_DEBUG_OBJECT (NULL,"INVALIDDDDDDDDDDD length: %d\n", length);
        return 0;
	 }
    offset += 4;
    memcpy(buf, data+offset, 4);
    GST_DEBUG_OBJECT (NULL,"\t length: %d, box: %s\n", length, buf);
    return length;
}

int process_moof_data(GstMMTDemux * mmtdemux, unsigned char *buf,int size)
{
   int length;
   static unsigned char Data[MAX_SIZE];
   static int tempsize;
   static int offset = 0;
   char boxType[4] = {"0"};
   GST_DEBUG_OBJECT (mmtdemux, "KKKKKKKKKKK %s size: %d \n", __func__, size);
   memcpy(Data+offset,buf,size);
   size += tempsize;
   tempsize = size;
#if 0
   while(localOffset<size)
   {
      length = pullatom(Data+localOffset, boxType);
      if(length > (size-localOffset) )
         goto DataInSufficient;  // 0  for insufficient data
      else if(length <= 0)
         goto reset_buf;  // 0  for insufficient data
      if(!strncmp(boxType,"moof",4))
      {
			  mmtdemux_parse_moof (mmtdemux, Data+localOffset, length, mmtdemux->moof_offset, NULL);
      }
      localOffset+=length;
   }
#else
      length = pullatom(Data, boxType);
      if(length > size)
         goto DataInSufficient;  // 0  for insufficient data
      else if(length <= 0)
         goto reset_buf;  // 0  for insufficient data
		 mmtdemux_parse_moof (mmtdemux, Data, length, mmtdemux->moof_offset, NULL);
#endif

reset_buf:
   tempsize = 0;
   offset = 0;
   memset(Data, '\0', sizeof(Data));
   return 1;
DataInSufficient:
   GST_DEBUG_OBJECT(mmtdemux," MOSS: Insufficient data, required length: %d, available: %d\n", length, size);
   if(length > 4096)
	{
		tempsize = 0;
		offset = 0;
	   return 0;
	}
   offset+=size;
   return 0;
}

int process_init_data(GstMMTDemux * mmtdemux, unsigned char *buf,int size)
{
   int length;
   static unsigned char Data[MAX_SIZE];
   static int tempsize;
   static int offset = 0;
   char boxType[4] = {"0"};
   int localOffset;
   GST_DEBUG_OBJECT (mmtdemux, "HHHHHHHHHH %s size: %d \n", __func__, size);
   memcpy(Data+offset,buf,size);
   size += tempsize;
   tempsize = size;
   localOffset=0;
   while(localOffset<size)
   {
      length = pullatom(Data+localOffset, boxType);
      if(length > (size-localOffset) )
         goto DataInSufficient;  // 0  for insufficient data
      else if(length <= 0)
         goto reset_buf;  // 0  for insufficient data
      if(!strncmp(boxType,FTYP,4))
      {
	        mmtdemux_parse_ftyp (mmtdemux, Data+localOffset, length);
      }
      else if(!strncmp(boxType,MMPU,4))
      {
		        mmtdemux_parse_mmpu (mmtdemux, Data+localOffset, length);

      }
      else if(!strncmp(boxType,MOOV,4))
      {
			  mmtdemux_parse_moov (mmtdemux, Data+localOffset, length);
			  mmtdemux_parse_tree(mmtdemux);
      }
      localOffset+=length;
   }
reset_buf:
   tempsize = 0;
   offset = 0;
   memset(Data, '\0', sizeof(Data));
   return 1;
DataInSufficient:
   GST_DEBUG_OBJECT(mmtdemux," MOSS: Insufficient data, required length: %d, available: %d\n", length, size);
   if(length > 4096)
	{
		tempsize = 0;
		offset = 0;
	   return 0;
	}
   offset+=size;
   return 0;
}

int GainMMTdemux(GstMMTDemux * mmtdemux, unsigned char *buf,int size,int DataType, int mediaType, GstBuffer *gbuf)
{
	static short init_videoSeg, moof_videoSeg;
	GST_DEBUG_OBJECT (mmtdemux, "FCHECK %s size: %d DataType: %d, mediaType:%s total stream:%d\n", __func__, size, DataType, mediaType==1?"audio":"video", mmtdemux->n_streams);
	if(!init_videoSeg && !mediaType && !DataType)
		init_videoSeg = !init_videoSeg;
	else if(!moof_videoSeg && !mediaType && (DataType == MOOF_DATA))
		moof_videoSeg = !moof_videoSeg;

	if(!init_videoSeg)
		goto exit_on_error;

	mmtdemux->mediaType=mediaType;
	switch(DataType)
	{
		case INIT_DATA:
			{
				if(mmtdemux->n_streams < 2)
					process_init_data(mmtdemux, buf, size);
				goto exit_on_success;
			}
		case MOOF_DATA:
			{
				if(!moof_videoSeg && !mediaType)
					moof_videoSeg = !moof_videoSeg;
				if(!moof_videoSeg)
					goto exit_on_error;
				process_moof_data(mmtdemux, buf, size);
				if((mmtdemux->exposed == FALSE) && (mediaType == 1) && (mmtdemux->n_streams == 2))
				{
					mmtdemux_prepare_streams (mmtdemux);
					mmtdemux_expose_streams (mmtdemux);
				}
				goto exit_on_success;
			}
		case MFU_DATA:
			{
				if(mmtdemux->exposed == TRUE)
					process_mfus(mmtdemux, buf, size, mediaType, gbuf);
				else
					goto exit_on_success;
				break;
			}
		default:
			goto exit_on_error;
	}
	return 1;
exit_on_success:
	if(gbuf)
		gst_buffer_unref (gbuf);
	return 1;
exit_on_error:
	if(gbuf)
		gst_buffer_unref (gbuf);
	return -1;
}
