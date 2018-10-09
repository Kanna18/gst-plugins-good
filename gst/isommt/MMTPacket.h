/*******************************************************************************************
 *  License:This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: MMTPacket.h
 *  Created on: 12-Feb-2018
 *  Author: Trimurthulu Amaradhi
 *******************************************************************************************/

#include<stdio.h>
#include "mmtdemux.h"
#ifndef MMTPACKET_H_
#define MMTPACKET_H_

//Payload type 6 bits indicates below if V=0
#define MPU            0x00
#define Generic_Object 0x01
#define Signalling_Msg 0x02
#define Repair_symbol  0x03
#define INIT_DATA 0
#define MOOF_DATA 1
#define MFU_DATA 2
enum FEC_types
{
	MMTP_NO_FEC_SRC_PAYLOAD_ID=0,
	MMTP_YES_FEC_SRC_PAYLOAD_ID,
	MMTP_FEC_REPAIR_PAYLOAD_MODE_0,
	MMTP_FEC_REPAIR_PAYLOAD_MODE_1
};


int MMT_package_id;
int short MP_table_descriptors;
int number_of_assets;

struct MPUBOX
{
	unsigned short is_complete;          //1 bit True or False
	unsigned short is_adc_present;       //1 bit True or False
	unsigned short reserved;             // 6 bits
	unsigned int   mpu_sequence_number;  //4 bytes
	unsigned int   asset_id_scheme;
	unsigned int   asset_id_length;
	unsigned char  asset_id_value[];     //This array size is asset_id_length
};

struct multilayer_info
{
   unsigned char multilayerflag;
   unsigned char reserved;
   unsigned char dependency_id;
   unsigned char depth_flag;
   unsigned char reserved1;
   unsigned char temporarl_id;
   unsigned char reserved2;
   unsigned char quality_id;
   unsigned char prority_id;
   unsigned short view_id;
   unsigned char layer_id;
   unsigned char temporal_id_els;
   unsigned char reserved3;
};


struct MMTHSample
{
	unsigned int sequence_number;
	char trackrefindex;
	unsigned int movie_fragment_sequence_number;
	unsigned int SampleNumber;
	unsigned int priority;
	unsigned int dependency_counter;
	unsigned int offset;
	unsigned int length;
	unsigned short int priority_dep_counter;
	struct multilayer_info muli;
	unsigned short int item_id;
};


struct signalling_message
{
	unsigned short message_id;
	unsigned char version;
	unsigned int  lenght;
};

struct PA_Message_Tables
{
	unsigned char table_id;
	unsigned char table_version;
	unsigned short int table_length;
};

struct PA_message
{
	struct signalling_message PAmsg;
	unsigned char number_of_tables;
	struct PA_Message_Tables PATabs;
};

struct PA_Tables
{
	unsigned char table_id;
	unsigned char version;
	unsigned short int length;
	unsigned char number_of_tables;
	unsigned char signalling_information_table_id;
	unsigned char signalling_information_table_version;
	unsigned char reserved;
	unsigned char alternative_loacation_flag;  //1 bit

};

struct MP_Tables
{
	unsigned char table_id;
	unsigned char version;
	unsigned short int lenght;
	unsigned char reserved;  //111111  ...only 6 bits.
	unsigned char Mp_table_mode;  // only 2 bits.
	unsigned char MMT_package_id_length;
	unsigned char MMT_package_id_byte;

	//descriptors
	unsigned short int MP_table_descriptors_length;
	unsigned char MP_table_descriptors_byte;

};

struct MPT_message
{
	unsigned short int  message_id;
	unsigned char 		version;
	unsigned short int 	length;

};

struct MPI_message
{
	unsigned short int  message_id;
	unsigned char 		version;
	unsigned short int 	length;
};

void StartMMTPSession();
unsigned char* MMTPacketStart(unsigned char* Bitstream);
int  ParseMMTPV1Pack(GstMMTDemux * mmtdemux, unsigned char*  Bitstream, GstBuffer *gbuf);
void ParseMMTPV0Pack(unsigned char*  Bitstream );
void ParseMMTPacket(unsigned char* Bitstream);
unsigned int ParseMMTPHDR(unsigned char* Bitstream, GstMMTDemux *, unsigned int *);

struct MMTHSample MMTHSampleFun(struct MMTHSample);
void ParseSignalling_Msg(unsigned char* Bitstream);
void ParsePAsignal(unsigned char* Bitstream );
void ParseGenSignal(unsigned char* Bitstream );
void ParseMP_Table(unsigned char* Bitstream);
void ParseMPTTable(unsigned char* Bitstream);
void ParseMMTPackage(unsigned char* Bitstream);
void ParsePayloadHDR(unsigned char* Bitstream);
void ParsePayloadIsombmffHDR(unsigned char* Bitstream);
void ParseVideoAsset(unsigned char* Bitstream);
void ParseAudioAsset(unsigned char* Bitstream);
void ParseMPU(unsigned char* Bitstream);
void ParseMFU(unsigned char*);
void ParseAU(unsigned char*);
void ParseMMTPAsignal(unsigned char*);
void ParseMMTMPTsig(unsigned char*);
void ParseMMTPayloadHdr(unsigned char*);
void ParseALFEC(unsigned char*);
int GainMMTdemux(GstMMTDemux * mmtdemux, unsigned char *buf,int size,int DataType, int, GstBuffer *gbuf);

#endif /* MMTPACKET_H_ */
