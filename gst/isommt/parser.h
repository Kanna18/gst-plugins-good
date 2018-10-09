/*
 * ISOBMFF PARSER
 * MOHAMMED SAFWAN SADA
 *
 * */
#ifndef __MMT
#define __MMT
#include <stdlib.h>
#include "mmtdemux.h"

//int GainMMTAudio(unsigned char *buf,int size,int DataType);
//int GainMMTVideo(unsigned char *buf,int size,int DataType);


#define FTYP "ftyp"
#define MMPU "mmpu"
#define MOOV "moov"
#define MOOF "moof"


#define SOUN 1
#define VIDE 2
#define HINT 3




typedef struct mmultilayer_info{
	char multilayer_flag;
	char reserved0;
	char dependency_id;
	char depth_flag;
	char reserved1;
	char temporal_id;
	char reserved2;
	char quality_id;
	char priority_id;
	char view_id;
	char layer_id;
}multiLayerInfo;

typedef struct mfu_Fullbox{
   unsigned int sequence_number; //32bit
   signed int trackrefindex; //8 bit
   unsigned int movie_fragment_sequence_number; //32 bit
   unsigned int samplenumber; //32 bit
   unsigned int priority; //8 bit
   unsigned int dependency_counter; // 8 bit
   unsigned int offset; // 32 bit
   unsigned int length; // 32 bit
   multiLayerInfo mul_linfo;
 }mfu_box;
mfu_box mfu_b;



int pullatom(unsigned char *data, char *buf);
void parse_mfu(unsigned char *data, int length);
int process_init_data(GstMMTDemux * mmtdemux, unsigned char *buf,int size);
void process_mfus(GstMMTDemux * mmtdemux, unsigned char* data, int size, int mediaType, GstBuffer *gbuf);



#endif
