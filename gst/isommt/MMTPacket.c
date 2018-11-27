/*******************************************************************************************
 *  License:This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: MMTPacket.c
 *  Created on: 12-Feb-2018
 *  Author: Trimurthulu Amaradhi
 *******************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include"MMTPacket.h"
#include "mmtsignal.h"
#include "MMTBitstream.h"
#include "mmtdemux.h"
//#include "parser.h"

int SigFlag=0;
unsigned short int av_packet;// 0:video, 1:audio, -1:NULL
struct MMTHSample mmthsample;
//FILE *fw;

struct PA_message PA_Msg;  //PA_Message
struct MPI_message MPI_Msg; //MPI_Message

void ParseAudioAsset(unsigned char* Bitstream)
{

}

void ParseMPU(unsigned char* Bitstream)
{
	struct MPUBOX Mpu;
	unsigned char Byte=Read8(Bitstream);
	unsigned char localByte=Byte;
	Mpu.is_complete=localByte&0x01;   //LSB considered first bit
	Mpu.is_adc_present=localByte&0x02;
	Mpu.reserved=localByte&0xFC;      //6 bits reserved.
	Mpu.mpu_sequence_number =Read32(Bitstream+1);
	GST_DEBUG_OBJECT(NULL,"  MPU Sequence Number %d \n ",Mpu.mpu_sequence_number);
	Mpu.asset_id_scheme=Read32(Bitstream+1+4);
	GST_DEBUG_OBJECT(NULL,"  MPU Sequence Number %d \n ",Mpu.asset_id_scheme);
	Mpu.asset_id_length=Read32(Bitstream+1+4+4);
	GST_DEBUG_OBJECT(NULL,"  Mpu.asset_id_length %d \n",Mpu.asset_id_length);
	Mpu.asset_id_value[Mpu.asset_id_length]=Read32(Bitstream+1+4+4+4);

}

unsigned char* MMTPacketStart(unsigned char* Bitstream)
{


	if((Bitstream[0]==0x40) && ((Bitstream[1]==0x00)  || (Bitstream[1]==0x02))  )
	{
		if(Bitstream[1]==0x02)
		{
			SigFlag=1;
			return Bitstream;

		}
	}
		return Bitstream;

}

void ParsePAsignal(unsigned char* Bitstream)
{
	struct PA_message PAMsg;
	PAMsg.PAmsg.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	PAMsg.PAmsg.version=Read8(Bitstream);
	Bitstream++;
	PAMsg.PAmsg.lenght=Read32(Bitstream);
	Bitstream=Bitstream+4;
	PAMsg.number_of_tables=Read8(Bitstream);
	if(PAMsg.number_of_tables>0)
		for(int i=0;i<PAMsg.number_of_tables;i++)
		{
			PAMsg.PATabs.table_id=Read8(Bitstream+i+1);
			PAMsg.PATabs.table_version=Read8(Bitstream+i+2);
			PAMsg.PATabs.table_length=Read16(Bitstream+i+3);
			//table()  // Need to understand better and add the table instace of the PA
		}
}

void ParseMPTTable(unsigned char* Bitstream )
{
	struct MPT_message MPTmsg;
	MPTmsg.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	MPTmsg.version=Read8(Bitstream);
	Bitstream++;
	MPTmsg.length=Read16(Bitstream);
	ParseMP_Table(Bitstream);
	GST_DEBUG_OBJECT(NULL," messageid: %u, version %u, length: %u\n", MPTmsg.message_id, MPTmsg.version, MPTmsg.length);

}

void ParseMP_Table(unsigned char* Bitstream)
{
	struct MP_Tables MP_Tab;
	unsigned char TempResv;
	int N1, N2, i;
	MP_Tab.table_id=Read8(Bitstream);
	Bitstream++;
	MP_Tab.version=Read8(Bitstream);
	Bitstream++;
	MP_Tab.lenght=Read16(Bitstream);
	Bitstream=Bitstream+2;
	TempResv=Read8(Bitstream);
	MP_Tab.reserved=(TempResv)&0xF8;
	MP_Tab.Mp_table_mode=(TempResv)&0x03;
	if(MP_Tab.table_id==0x20 || MP_Tab.table_id==0x11)
	{
		Bitstream++;
		N1=MMT_package_id;
		MP_Tab.MMT_package_id_length=Read8(Bitstream);
		for(i=0;i<N1;i++)
		{   Bitstream++;
			MP_Tab.MMT_package_id_byte=Read8(Bitstream);

		}
	}
	//MP Table Descriptios.
	Bitstream++;
	N2=MP_table_descriptors;
	MP_Tab.MP_table_descriptors_length=Read16(Bitstream);
	Bitstream=Bitstream+2;
	for(i=0;i<N2;i++)
	{
		MP_Tab.MP_table_descriptors_byte=Read8(Bitstream);
		Bitstream++;
	}

}
void ParseMMTPacket(unsigned char* Bitstream)
{
#if 0
	if(Bitstream[0] & 0x40)
		ParseMMTPV1Pack(Bitstream);
	else
		ParseMMTPV0Pack(Bitstream);
#endif

}


void ParseMMTPackage(unsigned char* Bitstream)
{



}

unsigned int ParseMMTPHDR(unsigned char* Bitstream, GstMMTDemux *mmtdemux, unsigned int *h_count)
{
	unsigned char FirstByte;
	int PackCountFlag=0;
	unsigned char FECType;
	unsigned char SecByte;
	unsigned short int PacketID;
	unsigned int timestamp;
	unsigned int Seq_Number;
   unsigned int header_count;
	unsigned char PakCountBit;
	unsigned short int r_TB_DS_TP_flow_label, DS, Tp, Flow_label, Header_extension, generic_flag, signal_flag, payld_length;
	unsigned char TB;
	int HeaderExtFlag = Header_extension = generic_flag = signal_flag = 0;
	av_packet  = -1;
	header_count=0;
	GST_DEBUG_OBJECT(NULL," IIIIIIIIIIII first 4 byte: %x %x %x %x\n", Bitstream[0], Bitstream[1], Bitstream[2], Bitstream[3]);
	FirstByte=Read8(Bitstream);
	PakCountBit=(FirstByte & 0x20)>>5;
	if(PakCountBit)
	{

		GST_DEBUG_OBJECT(NULL,"  packet counter bit is enabled ");
		PackCountFlag=1;

	}
	else
	{
		GST_DEBUG_OBJECT(NULL," No packet counter bit \n ");
	}
	FECType=(FirstByte & 0x18);
	FECType=FECType>>3;
	//switch(FirstByte & 0x18)
	switch(FECType)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet without Source_FEC_Field_ID \n");
			//return 3;
			break;
		case 1:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  Source_FEC_Pyaload_Field_ID");
			break;
		case 2:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  for repair symbols for FEC Payload Mode 0");
			break;
		case 3:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  for repair symbols for FEC Payload Mode 1");
			break;
		default:
			GST_DEBUG_OBJECT(NULL," None of the above");
			break;

	}
	//Parse X bit if it is 1  header extension is there

	if(FirstByte & 0x04)
	{
		GST_DEBUG_OBJECT(NULL,"  Header is extension bit is enabled.");
		HeaderExtFlag=1;
	}
	//Parse R bit .. This indicates Random Access type (RAP) is there in the stream.
	if(FirstByte & 0x02)
	{
		GST_DEBUG_OBJECT(NULL,"  RAP  bit is enabled \n");
	}
	//TODO:// SAFWANN.. NEED TO HANDLE THIS CASE TOO
	//Q bit for the future.
	//unsigned char QBit=Bitstream & 0x01;
	if(FirstByte & 0x01)
	{
		GST_DEBUG_OBJECT(NULL,"  Q bit is enabled \n");
		//return 3;
	}

	Bitstream=Bitstream+1;
	header_count++;
	SecByte=Read8(Bitstream);

	if(SecByte&0x80)
	{
		GST_DEBUG_OBJECT(NULL," Flow bit is enabled \n");
	}

	if(SecByte&0x40)
	{
		GST_DEBUG_OBJECT(NULL," E bit  is enabled \n");
	}
	if(SecByte&0x20)
	{
		GST_DEBUG_OBJECT(NULL," B bit is enabled\n");
	}

	if(SecByte&0x10)
	{
		GST_DEBUG_OBJECT(NULL," I bit is enabled \n ");
	}

	//switch(SecByte&0x08) //Payload type
	switch(SecByte&0x0F)
	{
		case 0x0:   //MPU
			GST_DEBUG_OBJECT(NULL,"  PayLoad type MPU is  enabled \n");
			break;

		case 0x1:  //Generic object
			GST_DEBUG_OBJECT(NULL,"  PayLoad type Generic object is  enabled  return -5\n");
			generic_flag = 1;
			return 3;
			break;
		case 0x2:  //signalling message
			GST_DEBUG_OBJECT(NULL,"  PayLoad type signalling message is  enabled \n");
			signal_flag = 1;
			break;
		case 0x3:  //repair symbol
			GST_DEBUG_OBJECT(NULL,"  PayLoad type repair symbol message is  enabled \n");
			return 0x04;
			break;
		case 0x4 ... 0x9: //0x4 to 0x9  reserved for ISO
			GST_DEBUG_OBJECT(NULL,"  PayLoad type 0x4 to 0x9  reserved for ISO is  enabled \n");
			return 0x04;
			break;
		case 0xA ... 0xF:
			GST_DEBUG_OBJECT(NULL,"  PayLoad type 0xA to 0xF  private use \n");
			return 0x04;
			break;  //0xA to 0xF  private use
		default:
			GST_DEBUG_OBJECT(NULL,"  PayLoad type none of the above use \n");
			break;  //none of the above.
	}
	Bitstream=Bitstream+1;
	header_count++;
	PacketID=Read16(Bitstream);
	Bitstream=Bitstream+2;
	header_count +=2;
	GST_DEBUG_OBJECT(NULL, "packet_id from signalling message video:%hd, audio:%hd", mmtdemux->video_packet_id, mmtdemux->audio_packet_id);
	if(PacketID==mmtdemux->audio_packet_id)
		av_packet = 1;
	else if(PacketID == mmtdemux->video_packet_id)
		av_packet = 0;
	else
		av_packet = -1;

	timestamp=Read32(Bitstream);                
	Bitstream=Bitstream+4;
	header_count +=4;
	Seq_Number=Read32(Bitstream);
	GST_DEBUG_OBJECT(NULL,"Seq_Number %x , Time stamp: %x",Seq_Number, timestamp);

	if(PackCountFlag)
	{
		Bitstream=Bitstream+4;
		header_count +=4;
		//PacketCounter=Read32(Bitstream);
	}
	//fclose(Param5);


	//Read the two bytes to be read
	//r | TB| DS| TP | flow_label
	Bitstream=Bitstream+4;
	header_count +=4;
	r_TB_DS_TP_flow_label=Read16(Bitstream);

	if(r_TB_DS_TP_flow_label & 0x8000)
	{
		//reliability_flag=1;
	}
	TB=(r_TB_DS_TP_flow_label&0x6000)>>13;
	switch(TB)
	{
		case 00:
			GST_DEBUG_OBJECT(NULL,"  \n Constant bit Rate \n");
			break;
		case 01:
			GST_DEBUG_OBJECT(NULL,"  \n Variable Bit rate \n");
			break;
		case 10:
		case 11:
			GST_DEBUG_OBJECT(NULL,"  \n Reserved for the future use \n");
			break;
		default:
			break;
	}

	DS=r_TB_DS_TP_flow_label&0x1C00;
	DS=DS>>10;
	switch(DS) //DS: Delay_Sensitivity
	{
		case 7://111:
			GST_DEBUG_OBJECT(NULL," \n Conversational service \n");
			break;
		case 6://110:
			GST_DEBUG_OBJECT(NULL," \n Live streaming service \n");
			break;
		case 5://101:
			GST_DEBUG_OBJECT(NULL," \n Delay Sensitive inter-acitve service \n");
			break;
		case 4://100:
			GST_DEBUG_OBJECT(NULL," \n interactive service \n");
			break;
		case 3://011:
			GST_DEBUG_OBJECT(NULL," \n Streaming service \n");
			break;
		case 2://010:
			GST_DEBUG_OBJECT(NULL," \n non real time service \n");
			break;
		case 1://001:
		case 0://000:
			GST_DEBUG_OBJECT(NULL," \n reserved \n");
			break;

	}

	Tp =(r_TB_DS_TP_flow_label & 0x0380);  //Transmission_Priority
	GST_DEBUG_OBJECT(NULL," \n Tp Priority %d \n",Tp);

	Flow_label=r_TB_DS_TP_flow_label & 0x007F;
	//if header extension flag is set to 1.. i.e X=1
	Bitstream=Bitstream+2;
	header_count +=2;
	if(HeaderExtFlag)
	{
		//TODO: SAFWAN NEED CHANGES HERE
		GST_DEBUG_OBJECT(NULL," Flow_label: %u, length: %u\n", Flow_label, Read16(Bitstream));
		Header_extension=Read16(Bitstream);
		header_count +=2;
	}
	GST_DEBUG_OBJECT(NULL," Flow_label: %u, Header_extension: %u\n", Flow_label, Header_extension);


	if(generic_flag)
	{
		Bitstream = Bitstream+6;
		GST_DEBUG_OBJECT(NULL,"  GGGGGGGGGGGGGGNNNNNNNNNNNN: start offset: %06lx byte_count: %d", Read48(Bitstream), header_count);
	}
   if(signal_flag)
	{
		*h_count = header_count;
		return 2;
	}
   *h_count = header_count;

	payld_length=Read16(Bitstream);   //Pay load length...starts the next MMTP Packet.
	return payld_length;
}

int ParseMMTPV1Pack(GstMMTDemux * mmtdemux, unsigned char*  Bitstream, GstBuffer *gbuf)
{
	unsigned int ref_offset = 0;
#if 1
	unsigned int lmoof_offset, incr;
#endif
	unsigned short int DU_HDR_ItemID;
	unsigned short int FT_T_f_i_A_Frag_Cnter, FT, f_i;
	unsigned char Fragemnt_counter;
	unsigned int Mpu_Sequence_Number;
	//unsigned char FirstByte=Read8(Bitstream);
	unsigned int AggrFlag;
	unsigned int MPU_Timed_data=0;
	//unsigned int A=0;
	unsigned short int payld_length=Read16(Bitstream);   //Pay load length...starts the next MMTP Packet.
	unsigned char * PayLoadBuf;
	if(av_packet == -1)
		goto skip;
	if(payld_length <= 1)
		goto skip;
	GST_DEBUG_OBJECT( NULL, "SSSS==================== payld_length: %d %03x  ::::: \n", payld_length, payld_length);

	//unsigned short int PayHDRsize=20;//24;

	//copy the payload into the dynamic buffer.
	PayLoadBuf=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
	if(PayLoadBuf == NULL)
		GST_DEBUG_OBJECT(NULL, "PAYYYYYYYYLOAD, memory not allocated");
	//memset(PayLoadBuf,0,(payld_length-6));
	//unsigned char *PayLoadBuf=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
	//unsigned int PayloadStartAddress=Bitstream+14+2;
	//TODO: why crash here?
	memset(PayLoadBuf,'\0',payld_length-6);


#if 1

	//Bitstream=Bitstream+2+6;
	memcpy(PayLoadBuf,(Bitstream+2+6),payld_length-6);
	ref_offset += 8;

	//memcpy(PayLoadBuf,(Bitstream+8),payld_length);

#else
	memcpy(PayLoadBuf,(Bitstream+2+22),payld_length-22);
#endif
	Bitstream=Bitstream+2;
	ref_offset += 2;
	FT_T_f_i_A_Frag_Cnter=Read16(Bitstream);
	FT=FT_T_f_i_A_Frag_Cnter &  0xF000;
	GST_DEBUG_OBJECT(NULL,"FT_T_f_i_A_Frag_Cnter %x \n ",FT_T_f_i_A_Frag_Cnter);
	GST_DEBUG_OBJECT(NULL,"FT: %x \n ",FT);
	FT=FT>>12;

	switch(FT)
	{
		case INIT_DATA:  //No additional DU headers
			GST_DEBUG_OBJECT(NULL,"MPU: MPU Meta data ftyp,mmpu,mov,metaboxes etc \n ");
			break;
		case MOOF_DATA://No additional DU headers
#if 1
			lmoof_offset = 0;
			for(incr = 0; incr < payld_length; incr++)
			{
				if(!memcmp(Bitstream+incr, "moof", 4))
				{
					lmoof_offset = incr+2;
					break;
				}
			}
			mmtdemux->moof_offset = mmtdemux->packet_offset+lmoof_offset;
			GST_DEBUG_OBJECT(NULL,"Movie Fragment Meta data ....Mooof box and mdat box moof_offset: %x, incr: %d", (int)mmtdemux->moof_offset, (int)incr);
#endif
			break;
		case MFU_DATA:
			GST_DEBUG_OBJECT(NULL,"MFU: contains a sample or subsample of timed media data or an item of non-timed data \n");
			break;
		case 03 ...15:
			GST_DEBUG_OBJECT(NULL,"Reserved for private use \n");
			break;
		default:
			break;
	}

	if(FT_T_f_i_A_Frag_Cnter & 0x0800)
	{
		MPU_Timed_data=1;  // for non-timed data it is 0
	}
	f_i=((FT_T_f_i_A_Frag_Cnter & 0x0600)>>13);
	switch(f_i)
	{
		case 00:
			GST_DEBUG_OBJECT(NULL,"Payload contains one or more complete data units. \n");
			break;
		case 01:
			GST_DEBUG_OBJECT(NULL,"Payload contains the first fragment of data unit \n");
			break;
		case 02:
			GST_DEBUG_OBJECT(NULL,"Payload contains the fragment of data unit that is neither first nor the last data unit \n");
			break;
		case 03:
			GST_DEBUG_OBJECT(NULL,"Payload contains last fragment of the data unit \n");
			break;

	}
	AggrFlag=FT_T_f_i_A_Frag_Cnter & 0x0100;
	if((AggrFlag)){
		GST_DEBUG_OBJECT(NULL,"1: More than one data unit is present in the payload  \n");
		//A=1;
	}
	if(!AggrFlag)
	{

	}

	Fragemnt_counter =FT_T_f_i_A_Frag_Cnter & 0x00FF;
	GST_DEBUG_OBJECT(NULL,"1: Fragemnt_counter %d \n",Fragemnt_counter);
	//Fragemnt_counter : No of Fragments of the same data MFUs.

	Bitstream=Bitstream+2;
	ref_offset +=2;
	Mpu_Sequence_Number=Read32(Bitstream);
	GST_DEBUG_OBJECT(NULL,"1: Mpu_Sequence_Number %d %04x %04x\n",Mpu_Sequence_Number, Mpu_Sequence_Number, Read32(Bitstream+4));
	//DU_Length
	Bitstream=Bitstream+4;
	ref_offset += 4;
	if(AggrFlag)
		Bitstream += 2;

	GST_DEBUG_OBJECT(NULL," CHECKKK MPU_Timed_data: %d......... av_packet: %s\n", MPU_Timed_data, av_packet==0?"video":av_packet==1?"audio":"NULL PACKET");
	if(MPU_Timed_data)
	{
		mmthsample.movie_fragment_sequence_number=Read32(Bitstream);
		Bitstream=Bitstream+4;
		ref_offset += 4;
		mmthsample.SampleNumber=Read32(Bitstream);
		Bitstream=Bitstream+4;
		ref_offset += 4;
		mmthsample.offset=Read32(Bitstream);

		Bitstream=Bitstream+4;
		ref_offset +=4;
		mmthsample.priority_dep_counter=Read16(Bitstream);
		ref_offset +=2;
		mmthsample.priority=mmthsample.priority_dep_counter & 0xFF00;
		mmthsample.dependency_counter=mmthsample.priority_dep_counter & 0x00FF;
		GST_DEBUG_OBJECT(NULL, "movie_fragment_sequence_number: %u, sample_number: %u, offset: %u", mmthsample.movie_fragment_sequence_number, mmthsample.SampleNumber, mmthsample.offset); 
	}
	else// Non-timed DU Header
	{
		Bitstream=Bitstream+2;
		ref_offset +=2;
		DU_HDR_ItemID=Read32(Bitstream);
		ref_offset +=4;
		GST_DEBUG_OBJECT(NULL," DU_HDR_ItemID: %u\n", DU_HDR_ItemID);

	}
	mmtdemux->ref_offset += 8;
	GST_DEBUG_OBJECT (NULL, "HPHPHPHPHPHP offset:%x, ref: %u", (int)mmtdemux->ref_offset, ref_offset);

	GainMMTdemux(mmtdemux, PayLoadBuf,payld_length-6,FT,av_packet, gbuf);
   free(PayLoadBuf);
	return 0;
skip:
	 if(gbuf)
		  gst_buffer_unref (gbuf);
	 return -1;
}

void ParseMMTPV0Pack(unsigned char*  Bitstream )
{


}


void ParseGenSignal(unsigned char* Bitstream )
{

}

void ParseSignalling_Msg(unsigned char* Bitstream)
{
	struct signalling_message Signal;
	Bitstream=Bitstream+2;
	Signal.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	Signal.version=Read8(Bitstream);
	Bitstream=Bitstream+1;
	if((Signal.message_id !=PA_Msg.PAmsg.message_id) && (Signal.message_id !=MPI_Msg.message_id) )
	{

		Signal.lenght=Read16(Bitstream);
	}
	else
	{
		Signal.lenght=Read32(Bitstream);
	}


}
