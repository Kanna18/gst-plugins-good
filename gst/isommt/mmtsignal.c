
/*******************************************************************************************
 *  License: This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: mmtsignal.c
 *  Created on: 04-MAY-2018
 *  Author: Madhu Nadendla
 *******************************************************************************************/

#include <string.h>
#include <stdio.h>
#include "MMTBitstream.h"
#include "mmtsignal.h"
#include "mmtdemux.h"

unsigned int MmtpHDRCount;
unsigned int sig_length;
unsigned short g_packet_id;

unsigned int mime_type (unsigned char* Bitstream) 
{
	unsigned int i=0,count=0;
	unsigned short int mime_type_length;	//N1 8 uimsbf
	unsigned char mime_type_byte[8];	//8 uimsbf

	mime_type_length = Read8(Bitstream); 
	Bitstream = Bitstream + 1;
	count++;
	GST_DEBUG_OBJECT(NULL,"\n mime_type_length = %hd\n",mime_type_length);

	GST_DEBUG_OBJECT(NULL,"mime_type_byte: ");
	for(i=0;i<mime_type_length;i++) 
	{
		memset(mime_type_byte,'\0',sizeof(mime_type_byte));
		memcpy(mime_type_byte,Bitstream ,1);
		Bitstream = Bitstream+1;
		count++;
		GST_DEBUG_OBJECT(NULL," %s ",mime_type_byte);
	}
	GST_DEBUG_OBJECT(NULL,"\n");
	return count;
}

unsigned int asset_id(unsigned char* Bitstream)
{
	unsigned int asset_id_scheme;  	// 32 uimsbf
	unsigned int asset_id_length;  	// N1 32 uimsbf
	unsigned char asset_id_byte;	// 8 uimsbf
	unsigned int i=0,count=0;

	asset_id_scheme = Read32(Bitstream);
	Bitstream = Bitstream +4;
	asset_id_length = Read32(Bitstream);
	Bitstream = Bitstream +4;
	
	count = count + 8;

	GST_DEBUG_OBJECT(NULL,"\n asset_id_scheme = %d asset_id_length=%d \n ",asset_id_scheme,asset_id_length);
	GST_DEBUG_OBJECT(NULL,"asset_id_byte: ");
	for (i=0; i<asset_id_length; i++) 
	{  
		asset_id_byte = Read8(Bitstream); 
		Bitstream = Bitstream +1;
		count++;
		GST_DEBUG_OBJECT(NULL," %02x ",asset_id_byte);
	}
	GST_DEBUG_OBJECT(NULL,"\n");

	return count;
}

unsigned int Identifier_mapping (unsigned char* Bitstream)
{
	unsigned int i=0,j=0,nbytes=0,count=0;
	unsigned char identifier_type;			//8 uimsbf
	unsigned short int URL_count=16;			//N1 16 uimsbf
	unsigned short int URL_length[32];	//N2 16 uimsbf
	//unsigned short int URL_byte[10];		//8  uimsbf

	unsigned short int regex_length ; 		//N3 16 uimsbf
	//unsigned char regex_byte; 			//   8  uimsbf
	unsigned short int representation_id_length;  	//N4 16 uimsbf
	//unsigned short int representation_id_byte; 	//   8  uimsbf

	unsigned short int private_length ;		//N5 16 uimsbf
	//unsigned char private_byte; 			//   8 uimsbf

	identifier_type = Read8(Bitstream);
	Bitstream = Bitstream+1;
	count++;
	GST_DEBUG_OBJECT(NULL,"\nidentifier_type = %02x \n",identifier_type);

	if (identifier_type == 0x00) {

		nbytes = asset_id(Bitstream);
		Bitstream = Bitstream + nbytes;
		count = count + nbytes;

	} else if (identifier_type == 0x01) {
		URL_count = Read16(Bitstream); 	
		Bitstream = Bitstream + 2;
		for(i=0;i<URL_count;i++) {
			URL_length[i]= Read16(Bitstream);
			for(j=0;j<URL_length[i];j++){
				//URL_byte[j] = Read8(Bitstream);
				Bitstream = Bitstream + 1;
			}
		} 

	} else if (identifier_type == 0x02) {

		regex_length = Read16(Bitstream);           
		Bitstream = Bitstream + 2;
		for(i=0;i<regex_length;i++){
			//regex_byte = Read8(Bitstream);     
			Bitstream = Bitstream + 1;
		}

	} else if (identifier_type == 0x03) {

		representation_id_length = Read16(Bitstream);    
		Bitstream = Bitstream+2;
		for (i=0;i<representation_id_length;i++) {
			//representation_id_byte = Read8(Bitstream); 
			Bitstream = Bitstream +1;
		}

	} else {

		private_length = Read16(Bitstream);
		Bitstream = Bitstream+2;
		for(i=0;i<private_length;i++){
			//private_byte = Read8(Bitstream); 
			Bitstream = Bitstream +1;
		}
	}

	return count;
}
				

unsigned int MMT_general_location_info(unsigned char* Bitstream) 
{
	unsigned int i,nbytes=0;
	unsigned char onebyte;
	unsigned short int location_type; 		//8 uimsbf
	unsigned char ipv4_src_addr[32]={'\0'};
	unsigned char ipv4_dst_addr[32]={'\0'};
	unsigned char ipv6_src_addr[128]={'\0'};	//128 uimsbf
	unsigned char ipv6_dst_addr[128]={'\0'}; 	//128 uimsbf

	unsigned short packet_id;
	unsigned short int dst_port;
	unsigned short int message_id;
	unsigned short int network_id = 0;	
	unsigned short int MPEG_2_transport_stream_id;
	unsigned short int MPEG_2_PID;			// 13 uimsbf
	unsigned char URL_length;
	//unsigned char URL_byte;
	unsigned short int length;
	//unsigned short int byte;

	location_type = Read8(Bitstream);
	Bitstream = Bitstream+1;
	nbytes = nbytes+1;
	GST_DEBUG_OBJECT(NULL,"location_type = %02x\n",location_type);

	if (location_type == 0x00) {
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream +2;
		nbytes = nbytes+2;
		GST_DEBUG_OBJECT(NULL,"packet_id = %hd\n",packet_id);
      g_packet_id = packet_id;
	} else if (location_type == 0x01) {

		memcpy(ipv4_src_addr ,Bitstream,4);
		Bitstream = Bitstream+4;
		memcpy(ipv4_dst_addr,Bitstream,4);
		Bitstream = Bitstream+4;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+12;
		GST_DEBUG_OBJECT(NULL,"ipv4_src_addr=%s\tipv4_dst_addr=%s\tdst_port=%hd\tpacket_id=%hd\n",ipv4_src_addr,ipv4_dst_addr,dst_port,packet_id);
	} else if (location_type == 0x02) {

		memcpy( ipv6_src_addr,Bitstream,16);
		Bitstream = Bitstream +16;
		memcpy(	ipv6_dst_addr,Bitstream ,16);
		Bitstream = Bitstream +16;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+36;
		GST_DEBUG_OBJECT(NULL,"ipv6_src_addr=%s\tipv6_dst_addr=%s\tdst_port=%hd\tpacket_id=%hd\n",ipv6_src_addr,ipv6_dst_addr,dst_port,packet_id);

	} else if (location_type == 0x03) { 

		network_id=Read16(Bitstream);	
		Bitstream = Bitstream+2;
		MPEG_2_transport_stream_id=Read16(Bitstream);
		Bitstream = Bitstream+2;
		onebyte = Read16(Bitstream);
		//reserved	“111”  3 bslbf
		MPEG_2_PID = onebyte & 0x1FFF;      // 	13 uimsbf
		Bitstream = Bitstream+2;
		nbytes = nbytes+6;
		GST_DEBUG_OBJECT(NULL,"network_id=%hd\tMPEG_2_transport_stream_id=%hd\tMPEG_2_PID=%hd\n",network_id,MPEG_2_transport_stream_id,MPEG_2_PID); 
	} else if (location_type == 0x04) {

		memcpy(ipv6_src_addr,Bitstream,16);
		Bitstream = Bitstream +16;
		memcpy(	ipv6_dst_addr,Bitstream ,16);
		Bitstream = Bitstream +16;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		onebyte = Read16(Bitstream);
		//reserved	“111”  3 bslbf
		MPEG_2_PID = onebyte & 0x1FFF;
		Bitstream = Bitstream+2;
		nbytes = nbytes+36;
		GST_DEBUG_OBJECT(NULL,"ipv6_src_addr=%s\tipv6_dst_addr=%s\tdst_port=%hd\tMPEG_2_PID=%hd\n",ipv6_src_addr,ipv6_dst_addr,dst_port,MPEG_2_PID);

	} else if (location_type == 0x05) {
		URL_length=Read8(Bitstream);
		Bitstream = Bitstream+1;
		nbytes++;
		for (i=0; i<URL_length; i++) {
			//URL_byte=Read8(Bitstream);	
			Bitstream = Bitstream+1;
			nbytes++;
		}
		
	} else if (location_type == 0x06) {
		length=Read16(Bitstream);	
		Bitstream = Bitstream+2;
		nbytes = nbytes+2;
		for (i=0;i<length;i++) {
			//byte = Read8(Bitstream);
			Bitstream = Bitstream+1;
			nbytes++;
		}
	} else if (location_type == 0x07) {

	} else if (location_type == 0x08) {

		message_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+2;
		GST_DEBUG_OBJECT(NULL,"message_id=%hd\n",message_id);

	} else if (location_type == 0x09) {

		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		message_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+4;
		GST_DEBUG_OBJECT(NULL,"network_id=%hd\tmessage_id=%hd\n",network_id,message_id);

	} else if (location_type == 0x0A) {

		memcpy(ipv4_src_addr ,Bitstream,4);
		Bitstream = Bitstream+4;
		memcpy(ipv4_dst_addr,Bitstream,4);
		Bitstream = Bitstream+4;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		message_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+14;
		GST_DEBUG_OBJECT(NULL,"ipv4_src_addr=%s\tipv4_dst_addr=%s\tdst_port=%hd\tpacket_id=%hd\tmessage_id=%hd\n",ipv4_src_addr,ipv4_dst_addr,dst_port,packet_id,message_id);

	} else if (location_type == 0x0B) {

		memcpy(ipv6_src_addr,Bitstream,16);
		Bitstream = Bitstream +16;
		memcpy(ipv6_dst_addr,Bitstream ,16);
		Bitstream = Bitstream +16;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		message_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		nbytes = nbytes+38;
		GST_DEBUG_OBJECT(NULL,"ipv6_src_addr=%s\tipv6_dst_addr=%s\tdst_port=%hd\tpacket_id=%hd,message_id=%hd\n",ipv6_src_addr,ipv6_dst_addr,dst_port,packet_id,message_id);

	} else if(location_type == 0x0C) {

		memcpy(ipv4_src_addr ,Bitstream,4);
		Bitstream = Bitstream+4;
		memcpy(ipv4_dst_addr,Bitstream,4);
		Bitstream = Bitstream+4;
		dst_port = Read16(Bitstream);
		Bitstream = Bitstream+2;
		onebyte = Read16(Bitstream);
		//	reserved	“111”  3 bslbf
		MPEG_2_PID = onebyte & 0x1FFF; 
		Bitstream = Bitstream+2;
		nbytes = nbytes+12;
		GST_DEBUG_OBJECT(NULL,"ipv4_src_addr=%s\tipv4_dst_addr=%s\tdst_port=%hd\tMPEG_2_PID=%hd\n",ipv4_src_addr,ipv4_dst_addr,dst_port,MPEG_2_PID);
	}

	return nbytes;
}


 /************************* Descriptor *************************************/

void CRI_descriptor(unsigned char* Bitstream) 
{
	unsigned short int descriptor_tag;		//16 uimsbf
	unsigned short int descriptor_length;		//16 uimsbf
	unsigned short int clock_relation_id;		//8 uimsbf

	//unsigned long long STC_sample; 			//42 uimsbf
	unsigned long long NTP_timestamp_sample;	//64 uimsbf

	descriptor_tag	= Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	descriptor_length = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	clock_relation_id = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;

 //TODO read 6bytes
      //reserved		“11 1111” 6 uimsbf
      //STC_sample		42 uimsbf

	NTP_timestamp_sample = Read64(Bitstream);
	Bitstream = Bitstream +8;
	sig_length = sig_length +8;
	
	GST_DEBUG_OBJECT(NULL,"CRI descriptor_tag = %hd\n descriptor_length =%hd\n clock_relation_id=%hd\n NTP_timestamp_sample = %llu\n ",descriptor_tag,descriptor_length,clock_relation_id,NTP_timestamp_sample);
  
}

void MPU_timestamp_descriptor (unsigned char* Bitstream) 
{
	unsigned int i=0;
	unsigned short int descriptor_tag;		//16 uimsbf
	unsigned short int descriptor_length;		//N*12 8 uimsbf
	unsigned int mpu_sequence_number;		//32 uimsbf
	unsigned long long mpu_presentation_time;	//64 uimsbf
	
	descriptor_tag = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	descriptor_length = Read8(Bitstream);    //N*12   8 uimsbf
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;
	GST_DEBUG_OBJECT(NULL,"MPU_timestamp_descriptor_tag = %hd descriptor_length = %hd\n",descriptor_tag,descriptor_length);

	for (i=0; i<descriptor_length/12 ; i++) 
	{//TODO
		mpu_sequence_number = Read32(Bitstream);
		Bitstream = Bitstream +4;
		sig_length = sig_length +4;

		mpu_presentation_time = Read64(Bitstream);
		Bitstream = Bitstream +8;
		sig_length = sig_length +8;

		GST_DEBUG_OBJECT(NULL,"mpu_sequence_number = %u mpu_presentation_time = %llu \n",mpu_sequence_number,mpu_presentation_time);	
	}

}

void Dependency_descriptor(unsigned char* Bitstream) 
{ 
	unsigned int i=0/*,count=0*/;
	unsigned short descriptor_length;	//16 uimsbf       
	unsigned short descriptor_tag;		//16 uimsbf
	unsigned short num_dependencies;	//N1 8 uimsbf
	
	descriptor_length = Read16(Bitstream);     
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	descriptor_tag = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	num_dependencies = Read8(Bitstream);	
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;

	GST_DEBUG_OBJECT(NULL,"Dependency_descriptor descriptor_tag = %hd descriptor_length = %hd num_dependencies = %hd \n",descriptor_tag,descriptor_length,num_dependencies);

	for ( i = 0 ; i < num_dependencies ; i++ ) {
		//count = asset_id(Bitstream);
	   //GST_DEBUG_OBJECT(NULL,"\t %u");
		
	}
  GST_DEBUG_OBJECT(NULL,"\n");

}
  
void GFD_descriptor (unsigned char* Bitstream) 
{
	unsigned int i=0,j=0;
	unsigned char onebyte;
	//unsigned short	descriptor_tag;		//16 uimsbf
	//unsigned int	descriptor_length;	//32 uimsbf
	unsigned short 	number_of_CodePoints;	//N1 8 uimsbf

	//unsigned short value;			 //8  uimsbf
	//unsigned long long maximumTransferLength;//48 uimsbf

	//unsigned short 	fileDeliveryMode;		//2 bslbf
	//unsigned short constantTransferLength_flag;	//1 bslbf	
	//unsigned short outOfOrderSending_flag;		//1 bslbf
	unsigned short FileTemplate_flag;		//1 bslbf
	unsigned short startTOI_flag;			//1 bslbf
	unsigned short endTOI_flag;			//1 bslbf
	unsigned short EntityHeader_flag;		//1 bslbf

	unsigned short File_length;		//N3 16 uimsbf
	//unsigned short File_byte;		//8 uimsbf

	unsigned short EntityHeader_byte;	//N4 16 uimsbf
	//unsigned short EntityHeader_length;	//8 uimsbf
	unsigned short FileTemplate_length;	//N2 8 uimsbf
	//unsigned short FileTemplate_byte;	//8 uimsbf


	//descriptor_tag = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;

	//descriptor_length = Read32(Bitstream);	
	Bitstream = Bitstream +4;
	sig_length = sig_length +4;

 	number_of_CodePoints = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;

	for(i=0; i<number_of_CodePoints; i++) 
	{ 
  		//value = Read8(Bitstream);
		Bitstream = Bitstream +1;
		sig_length = sig_length +1;
		
		onebyte = Read8(Bitstream);
		Bitstream = Bitstream +1;
		sig_length = sig_length +1;

		//fileDeliveryMode 	    = (onebyte >> 6); 
		//constantTransferLength_flag = (onebyte >>5)&0x01 ;
		//outOfOrderSending_flag 	    = (onebyte >> 4)&0x01;
		FileTemplate_flag 	    = (onebyte >> 3)&0x01;
		startTOI_flag 		    = (onebyte >>2)&0x01;
		endTOI_flag 		    = (onebyte>>1)&0x01;
		EntityHeader_flag 	    = onebyte & 0x01;

//TODO read 6bytes
		//maximumTransferLength		48 uimsbf

		if(FileTemplate_flag == 1) {
 			FileTemplate_length = Read8(Bitstream);	
			Bitstream = Bitstream +1;
			sig_length = sig_length +1;
			for(j = 0; j < FileTemplate_length; j++){
				//FileTemplate_byte = Read8(Bitstream);
				Bitstream = Bitstream +1;
				sig_length = sig_length +1;
			}
		} else {
			File_length = Read16(Bitstream);
			Bitstream = Bitstream +2;
			sig_length = sig_length +2;

			for(j = 0; j < File_length; j++)
			{
				//File_byte = Read8(Bitstream); 
				Bitstream = Bitstream +1;
				sig_length = sig_length +1;
			}
		}

		if(startTOI_flag == 1);
//TODO			startTOI
		if(endTOI_flag == 1);
//TODO		 	endTOI
		if(EntityHeader_flag == 1) 
		{
			EntityHeader_byte = Read16(Bitstream);	
			Bitstream = Bitstream +2;
			sig_length = sig_length +2;
			for(j = 0; j < EntityHeader_byte; j++)
			{
				//EntityHeader_length = Read8(Bitstream);	
				Bitstream = Bitstream +1;
				sig_length = sig_length +1;
			
			}
		}
  	}

}        


void SI_descriptor(unsigned char* Bitstream) 
{
	unsigned int i=0,j=0;
	unsigned char onebyte;

	//unsigned short descriptor_tag;			//16 uimsbf
	//unsigned short descriptor_length;		//16 uimsbf
	unsigned short security_system_count;		//N1 8 uimsbf
	unsigned short system_provider_url_flag	;	//1 bslbf

	unsigned short	system_provider_url_length;	//N2 8 uimsbf
	//unsigned short 	system_provider_url_byte;	//8 uimsbf

	unsigned char system_id[128]={'\0'};		//16*8 uimsbf
	unsigned short kid_count;			//N3  16 uimsbf
	unsigned char KID[128]={'\0'};
	unsigned int data_size;				//N4 32 uimsbf
	//unsigned short data;				//8 uimsbf

	//descriptor_tag = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;
	
	//descriptor_length = Read16(Bitstream);	
	Bitstream = Bitstream +2;
	sig_length = sig_length +2;
	
	security_system_count = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;
	
	onebyte = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length +1;
//	reserved				“000 0000” 7 uimsbf
	system_provider_url_flag = (onebyte>>7) ;

	if (system_provider_url_flag) 
	{
		system_provider_url_length = Read8(Bitstream);	
		Bitstream = Bitstream +1;
		sig_length = sig_length +1;
		for (i=0; i<system_provider_url_length; i++) 
		{
			//system_provider_url_byte = Read8(Bitstream);
			Bitstream = Bitstream +1;
			sig_length = sig_length +1;
		}
	}
    
  	for (i=0;i<security_system_count;i++) 
	{
		memcpy(system_id,Bitstream,128);
		Bitstream = Bitstream +16;
		sig_length = sig_length +16;

		kid_count = Read16(Bitstream);	
		Bitstream = Bitstream +2;
		sig_length = sig_length +2;

		for(j=0;j<kid_count;j++) 
		{
			memcpy(KID ,Bitstream,128);	
			Bitstream = Bitstream +16;
			sig_length = sig_length +16;
		}

		data_size = Read32(Bitstream);
		Bitstream = Bitstream +4;
		sig_length = sig_length +4;

		for(j=0;j<data_size;j++) 
		{
			//data = Read8(Bitstream);
			Bitstream = Bitstream +1;
			sig_length = sig_length +1;
		}

	}

}
 
/************************************************************************************/

void PA_table(unsigned char*Bitstream)
{
	unsigned short int table_id;      //8 uimsbf    0x00
	unsigned short int version;       //8 uimsbf
	unsigned short int length;        //16 uimsbf
	unsigned char onebyte;
	unsigned int i=0,nbytes;
	unsigned short int number_of_tables;				//N1 8 uimsbf
	//unsigned short int signalling_information_table_id;		//8 uimsbf
	//unsigned short int signalling_information_table_version;	//8 uimsbf
	unsigned short int alternative_location_flag;			//1 bslbf
	unsigned short int private_extension_flag; 			//1 bslbf

	table_id = Read8(Bitstream);
	Bitstream = Bitstream + 1;
	version = Read8(Bitstream);
	Bitstream = Bitstream + 1;
	length = Read16(Bitstream);
	Bitstream = Bitstream + 2;

	GST_DEBUG_OBJECT(NULL,"PA_table_id=%hd,version=%hd,length=%hd\n",table_id,version,length);

	/**************	information_table_info **************************/
	number_of_tables = Read8(Bitstream);
	Bitstream = Bitstream+1;

	for (i=0; i<number_of_tables; i++) 
	{
		//signalling_information_table_id = Read8(Bitstream);
		Bitstream = Bitstream + 1;
		//signalling_information_table_version = Read8(Bitstream); 
		Bitstream = Bitstream + 1;

		/**************	location *******************/
		nbytes = MMT_general_location_info(Bitstream);
		Bitstream = Bitstream+nbytes;

		onebyte = Read8(Bitstream);
		//reserved         "1111 111"   7 bslbf
		alternative_location_flag = onebyte&0x01; 
		Bitstream = Bitstream + 1;
		if (alternative_location_flag == 1) 
		{
			//location_type from 0x07 to 0x0B shell be used 
			nbytes = MMT_general_location_info(Bitstream);
			Bitstream = Bitstream+nbytes;
		}

	}

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream + 1;
	// reserved           “1111 111”   7 bslbf
	private_extension_flag = onebyte & 0x01;
	if (private_extension_flag == 1){
		//private_extension {
		//}
	}

}


void MPI_table(unsigned char*Bitstream)
{
	//unsigned char onebyte;
	unsigned int i=0,j=0;
	//unsigned short int PI_mode;
	unsigned short int MPIT_descriptors_length;     //  N2      16 uimsbf
	//unsigned short int MPIT_descriptors_byte;       //  8 uimsbf
	unsigned short int PI_content_count;		// N3  8 uimsbf
	unsigned short int PI_content_type_length; 	//N4 8 uimsbf
	//unsigned short int PI_content_type_length_byte; //8 uimsbf
	unsigned short int PI_content_name_length;      // N5 8 uimsbf
	//unsigned short int PI_content_name_byte; 	//    8 uimsbf
	unsigned short int PI_content_descriptors_length;  //   N6 16 uimsbf
	//unsigned short int PI_contnent_descriptors_byte;   //     8 uimsbf
	unsigned short int PI_content_length;		// N6 16 uimsbf
	//unsigned short int PI_content_byte;		//    8 uimsbf

	//    reserved        “1111” 4 bslbf
	//    PI_mode         2 uimsbf
	//    reserved        “11” 2 bslbf

	//onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	//PI_mode = (onebyte&0xc0)>>2 ;

	GST_DEBUG_OBJECT(NULL,"MPI_table_id=%hd,version=%hd,length=%hd\n",table_info.table_id,table_info.version,table_info.length);

	/*********************** MPIT_descriptors *********************/

	MPIT_descriptors_length = Read16(Bitstream);
	Bitstream = Bitstream +2;
	for (i=0; i<MPIT_descriptors_length; i++) {
		//MPIT_descriptors_byte = Read8(Bitstream);
		Bitstream = Bitstream +1;
	}

	PI_content_count = Read8(Bitstream);
	Bitstream = Bitstream +1;

	for (i =0; i<PI_content_count; i++) 
	{ 
		/************************** PI_content_type **********************/
		PI_content_type_length = Read8(Bitstream); 
		Bitstream = Bitstream +1;
		for (j=0; j<PI_content_type_length; j++) {
			//PI_content_type_length_byte = Read8(Bitstream); 
			Bitstream = Bitstream +1;
		}

		PI_content_name_length = Read8(Bitstream); 
		Bitstream = Bitstream +1;

		for (j=0; j<PI_content_name_length; j++) 
		{
			//PI_content_name_byte = Read8(Bitstream);
			Bitstream = Bitstream +1;
		}

		/************************** PI_content_descriptors *******************/

		PI_content_descriptors_length = Read16(Bitstream);
		Bitstream = Bitstream +2;

		for (j=0; j<PI_content_descriptors_length; j++) 
		{
			//PI_contnent_descriptors_byte = Read8(Bitstream);
			Bitstream = Bitstream +1;
		}	

		PI_content_length=Read16(Bitstream);
		Bitstream = Bitstream+2;

		for (j=0; j<PI_content_length; j++) 
		{
			//PI_content_byte=Read8(Bitstream);
			Bitstream = Bitstream +1;
		}

	}

}

void MP_table(unsigned char *Bitstream, GstMMTDemux *mmtdemux)
{
	unsigned int i=0,j=0,nbytes=0;
	//unsigned short reserved ;	  		// “11 1111” 6 bslbf
	unsigned short	MP_table_mode; 			// 2 bslbf
	unsigned char onebyte ;

	unsigned short int MMT_package_id_length;         //N1 8 uimsbf
	unsigned short int MMT_package_id_byte;	 	  //8 uimsbf
	unsigned short int MP_table_descriptors_length;   //N2 16 uimsbf
	unsigned short int MP_table_descriptors_byte;  	  //8 uimsbf

	unsigned short int number_of_assets ;		// N3 8 uimsbf
	unsigned char asset_type[36] ={'\0'}; 		// 32 char
	//unsigned char  reserved;       		// “1111 11”   6 bslbf
	unsigned short int default_asset_flag;		// 1 bslbf
	unsigned short int asset_clock_relation_flag;	// 1 bslbf

	//unsigned char asset_clock_relation_id;     	// 8 uimsbf
	//unsigned short int reserved;     	   	// “1111 111”   7  bslbf
	unsigned short int asset_time_scale_flag;   	// 1 bslbf
	//unsigned int asset_timescale ;      		// 32 uimsbf

	unsigned short int location_count;	    	// N4 8 uimsbf
	unsigned short int asset_descriptors_length;  	// N5 16 uimsbf
	unsigned short int asset_descriptors_byte;    	// 8 uimsbf


	table_info.table_id = Read8(Bitstream);
	Bitstream = Bitstream+1;
	table_info.version = Read8(Bitstream);
	Bitstream = Bitstream+1;
	table_info.length = Read16(Bitstream);
	Bitstream = Bitstream+2;
	sig_length = sig_length +4;
	GST_DEBUG_OBJECT(NULL,"MP_table_id=%hd,version=%hd,length=%hd\n",table_info.table_id,table_info.version,table_info.length);

	onebyte = Read8(Bitstream);	
	Bitstream = Bitstream + 1;
	sig_length = sig_length+1;
	MP_table_mode = (onebyte & 0xc0)>>6 ;
	switch(MP_table_mode)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL,"sequential_order_processing_mode\n");
			break;
		case 1:
			GST_DEBUG_OBJECT(NULL,"order_irrelevant_processing_mode\n");
			break;
		case 2:
			GST_DEBUG_OBJECT(NULL,"independent_processing_mode\n");
			break;
		case 3:	
			GST_DEBUG_OBJECT(NULL,"Reserved\n");
			break;
	}

	if ((table_info.table_id == 0x20) || (table_info.table_id == 0x11))
	{
		/**************** MMT_package_id ***********************/
		MMT_package_id_length = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		GST_DEBUG_OBJECT(NULL,"MMT_package_id_length = %hd\n",MMT_package_id_length);
		GST_DEBUG_OBJECT(NULL,"MMT_package_id_byte : ");
		for (i=0; i<MMT_package_id_length; i++) 
		{
			MMT_package_id_byte = Read8(Bitstream);
			Bitstream= Bitstream+1;
			sig_length = sig_length+1;
			GST_DEBUG_OBJECT(NULL," %02x ",MMT_package_id_byte);
		}
		GST_DEBUG_OBJECT(NULL,"\n");

		/************  MP_table_descriptors ***********************/
		MP_table_descriptors_length = Read16(Bitstream);
		Bitstream = Bitstream+2;
		sig_length = sig_length+2;
		GST_DEBUG_OBJECT(NULL,"MP_table_descriptors_length=%hd\n",MP_table_descriptors_length);
		GST_DEBUG_OBJECT(NULL,"MP_table_descriptors_byte: ");
		for (i=0; i<MP_table_descriptors_length; i++) 
		{
			MP_table_descriptors_byte=Read8(Bitstream);
			Bitstream = Bitstream +1;
			sig_length = sig_length+1;
			GST_DEBUG_OBJECT(NULL,"%02x",MP_table_descriptors_byte);
		}
		GST_DEBUG_OBJECT(NULL,"\n");
	}

	number_of_assets = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length+1;
	GST_DEBUG_OBJECT(NULL,"number_of_assets = %hd\n",number_of_assets);
	for (i=0; i<number_of_assets; i++) 
	{
		nbytes = Identifier_mapping(Bitstream);
		Bitstream = Bitstream + nbytes;
		sig_length = sig_length + nbytes;
	
		memcpy(asset_type,Bitstream,4);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;
		GST_DEBUG_OBJECT(NULL,"asset_type = %s\n",asset_type);
		
		onebyte = Read8(Bitstream);
		Bitstream = Bitstream +1;
		sig_length = sig_length+1;
		//reserved "1111 11"  6 bslbf
		default_asset_flag= (onebyte&0x02)>>1 ;  
		asset_clock_relation_flag = onebyte&0x01;

		GST_DEBUG_OBJECT(NULL,"reserved =%02x default_asset_flag= %hd asset_clock_relation_flag=%hd\n",onebyte,default_asset_flag,asset_clock_relation_flag);

		if (asset_clock_relation_flag == 1) 
		{
			//asset_clock_relation_id = Read8(Bitstream);
			Bitstream = Bitstream+1;
			sig_length = sig_length+1;
			onebyte = Read8(Bitstream);
			Bitstream = Bitstream+1;
			sig_length = sig_length+1;
			//	reserved "1111 111" 7 bslbf
			asset_time_scale_flag = onebyte&0x01;

			if (asset_time_scale_flag == 1) 
			{
				//asset_timescale = Read32(Bitstream);
				Bitstream=Bitstream+4; 
				sig_length = sig_length+4;
			}

		}

		/******** asset_location *************************/
		location_count= Read8(Bitstream);
		Bitstream=Bitstream+1;
		sig_length = sig_length+1;
		GST_DEBUG_OBJECT(NULL,"location_count=%hd\n",location_count);
		for (j=0; j<location_count; j++) 
		{
			nbytes = MMT_general_location_info(Bitstream);
			Bitstream = Bitstream+nbytes;
			sig_length = sig_length+nbytes;
		}
#if 1
	   if(!memcmp(asset_type, "hev1", 4))
		  mmtdemux->video_packet_id=g_packet_id;		
	   else if(!memcmp(asset_type, "mp4a", 4))
		  mmtdemux->audio_packet_id=g_packet_id;		
#endif

		/******	asset_descriptors ***********************/
		asset_descriptors_length = Read16(Bitstream);
		Bitstream = Bitstream+2;
		sig_length = sig_length+2;
		GST_DEBUG_OBJECT(NULL,"asset_descriptors_length = %hd\n",asset_descriptors_length);
		GST_DEBUG_OBJECT(NULL,"asset_descriptors_byte : ");
		for (j=0; j<asset_descriptors_length; j++) 
		{
			asset_descriptors_byte = Read8(Bitstream);
			Bitstream=Bitstream+1;          
			sig_length = sig_length+1;
			GST_DEBUG_OBJECT(NULL,"%02x ",asset_descriptors_byte);
		}
		GST_DEBUG_OBJECT(NULL,"\n\n");
	}

}

void CRI_table (unsigned char *Bitstream) 
{
	unsigned int i=0;
//	unsigned short int table_id;
//	unsigned char version;
//	unsigned short int length;
	unsigned short int number_of_CRI_descriptor; //N1 8 uimsbf


	table_info.table_id = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.version = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.length = Read16(Bitstream);
	Bitstream=Bitstream+2;
	sig_length = sig_length +4;
	GST_DEBUG_OBJECT(NULL,"CRI_table_id=%hd,version=%hd,length=%hd\n",table_info.table_id,table_info.version,table_info.length);

	number_of_CRI_descriptor = Read8(Bitstream); 
	Bitstream = Bitstream+1;
	GST_DEBUG_OBJECT(NULL,"number_of_CRI_descriptor =%hd\n",number_of_CRI_descriptor);

	for (i=0; i<number_of_CRI_descriptor; i++) 
	{

		//	CRI_descriptor()		152 uimsbf

	}

}

void DCI_table(unsigned char *Bitstream) 
{
	unsigned int i,nbytes=0;
	unsigned char onebyte;
	unsigned short int number_of_assets;
	unsigned short int codec_complexity_flag;

	//unsigned short int video_average_bitrate; 	//16 uimsbf
	//unsigned short int video_maximum_bitrate; 	//16 uimsbf
	//unsigned short int horizontal_resolution;	//16 uimsbf
	//unsigned short int vertical_resolution;		//16 uimsbf
	//unsigned short int temporal_resolution;		//8 uimsbf
	//unsigned short int video_minimum_buffer_size;	//16 uimsbf

	//unsigned short int audio_average_bitrate;	//16 uimsbf
	//unsigned short int audio_maximum_bitrate;	//16 uimsbf
	//unsigned short int audio_minimum_buffer_size;	//16 uimsbf	
	//unsigned int required_storage;  		// 32 uimsbf


	table_info.table_id = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.version = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.length = Read16(Bitstream);
	Bitstream=Bitstream+2;
	sig_length = sig_length +4;
	GST_DEBUG_OBJECT(NULL,"DCI table_id=%hd,version=%hd,length=%hd\n",table_info.table_id,table_info.version,table_info.length);

	number_of_assets = Read8(Bitstream); // N1 8 uimsbf	
	Bitstream = Bitstream+1;
	sig_length = sig_length+1;

	GST_DEBUG_OBJECT(NULL,"number_of_assets = %hd\n",number_of_assets);	
	for (i=0; i<number_of_assets; i++) 
	{
		nbytes = asset_id(Bitstream);	
		Bitstream = Bitstream+nbytes;
		
		nbytes = mime_type(Bitstream);
		Bitstream = Bitstream+nbytes;

		onebyte = Read8(Bitstream);
		Bitstream = Bitstream+1;

		//reserved	“111 1111”   7 bslbf
		codec_complexity_flag= onebyte&0x01; //	1 bslbf
		GST_DEBUG_OBJECT(NULL,"codec_complexity_flag = %hd\n",codec_complexity_flag);

		if (codec_complexity_flag == 1) 
		{
#if 0
			if ( top_level_mime_type == video )
			{
				/****** video_codec_complexity ****************/
				video_average_bitrate =Read16(Bitstream);	
				Bitstream = Bitstream+2;
				video_maximum_bitrate= Read16(Bitstream);	
				Bitstream = Bitstream+2;
				horizontal_resolution=Read16(Bitstream);	
				Bitstream = Bitstream+2;
				vertical_resolution=Read16(Bitstream);	
				Bitstream = Bitstream+2;
				temporal_resolution=Read8(Bitstream);	
				Bitstream = Bitstream+1;
				video_minimum_buffer_size=Read16(Bitstream);	
				Bitstream = Bitstream+2;

				GST_DEBUG_OBJECT(NULL,"video_average_bitrate=%hd video_maximum_bitrate=%hd horizontal_resolution=%hd vertical_resolution=%hd temporal_resolution=%hd video_minimum_buffer_size =%hd ",video_average_bitrate,video_maximum_bitrate,horizontal_resolution,vertical_resolution,temporal_resolution,video_minimum_buffer_size);


			} else if (top_level_mime_type == audio){
				/********* audio_codec_complexity *****************/
				audio_average_bitrate=Read16(Bitstream);		
				Bitstream = Bitstream+2;
				audio_maximum_bitrate=Read16(Bitstream);	
				Bitstream = Bitstream+2;
				audio_minimum_buffer_size=Read16(Bitstream);
				Bitstream = Bitstream+2;
				GST_DEBUG_OBJECT(NULL,"audio_average_bitrate=%hd audio_maximum_bitrate=%hd audio_minimum_buffer_size=%hd\n",audio_average_bitrate,audio_maximum_bitrate,audio_minimum_buffer_size);

			}
			else
			{
				/*************download_capability******************/ 
				required_storage=Read32(Bitstream);	
				Bitstream = Bitstream+4;
			}
#endif
		}
	}

}


void SI_table(unsigned char*Bitstream) 
{
	unsigned int i=0,j=0;
	unsigned char onebyte;
	unsigned short system_id_length;		//N2 8 uimsbf
	//unsigned short system_id_byte; 			//8 uimsbf  
	unsigned short	systemProvider_URL_length;	//N3 8 uimsbf
	//unsigned short	URL_byte;			//8 uimsbf
	//unsigned short 	length_byte;			//8 uimsbf
	unsigned short encryption_flag;			//1  bslbf

	unsigned short encAlgorithm_length;		//N4 8 uimsbf
	//unsigned short encAlgorithm_byte;		//8   uimsbf
	unsigned short keySize_length;			//N5 8 uimsbf
	//unsigned short keySize_byte;			//8 uimsbf
	unsigned short key_URL_length;			//N6 8 uimsbf
	unsigned short initializationVector_length;	//N7 8 uimsbf
	unsigned short iv_URL_length;			//N8 8uimsbf
	unsigned short keyUrlTemplate_length;		//N9 8   uimsbf
	unsigned short ivUrlTemplate_length;  		//N10 8 uimsbf
	unsigned short authentication_flag;    		//1 bslbf

	unsigned short digest_URL_length;		//N11  8 uimsbf
	unsigned short digestUrlTemplate_length;	//N12 8 uimsbf

	unsigned short  signature_URL_length;		//N13  8 uimsbf
	unsigned short 	signatureUrlTemplate_length;	//N14 8 uimsbf

	unsigned short	signature_length;		//N15 8 uimsbf
	unsigned short 	signatureKey_URL_length	;	//N16 8 uimsbf

	unsigned short	number_of_License ; 		//N17 8 uimsbf
	unsigned short licenseUrl_length; 		//N18 8 uimsbf
	unsigned short lisenceUrlTemplate_length; 	//N19 8 uimsbf

	unsigned short number_of_Security_System; 


	table_info.table_id = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.version = Read8(Bitstream);
	Bitstream=Bitstream+1;
	table_info.length = Read16(Bitstream);
	Bitstream=Bitstream+2;
	sig_length = sig_length +4;
	GST_DEBUG_OBJECT(NULL,"SI_table_id=%hd,version=%hd,length=%hd\n",table_info.table_id,table_info.version,table_info.length);

	number_of_Security_System = Read8(Bitstream); 
	for (i=0; i<number_of_Security_System; i++) 
	{
		/******** system_id **********/
		system_id_length = Read8(Bitstream); 	
		Bitstream = Bitstream +1;
		for (j=0; j<system_id_length; j++) {
			//system_id_byte  = Read8(Bitstream); 
			Bitstream = Bitstream +1;
		}

		/********* systemProvider ****/
		systemProvider_URL_length = Read8(Bitstream);	
		Bitstream = Bitstream +1;
		for (i=0; i<systemProvider_URL_length; i++) {	
			//URL_byte = Read8(Bitstream);
			Bitstream = Bitstream +1;
		}

		//reserved		“1111 111”   7 bslbf
		onebyte = Read8(Bitstream);
		Bitstream = Bitstream +1;
		encryption_flag	= onebyte&0x01;		
		if (encryption_flag == 1) 
		{
			/******* encAlgorithm *******/

			encAlgorithm_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<encAlgorithm_length; i++) {
				//encAlgorithm_byte = Read8(Bitstream);
				Bitstream = Bitstream +1;
			}

			/*************** keySize ************/
			keySize_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<keySize_length; i++) {
				//keySize_byte = Read8(Bitstream);
				Bitstream = Bitstream +1;
			}

			/*********** keyUrl ***************/
			key_URL_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<key_URL_length; i++) {
				//URL_byte = Read8(Bitstream);
				Bitstream = Bitstream +1;
			}

			/******* initializationVector ****/
			initializationVector_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<initializationVector_length; i++) {
				//length_byte = Read8(Bitstream);
				Bitstream = Bitstream+1;
			}

			/********** ivUrl *****************/ 
			iv_URL_length = Read8(Bitstream);	
			Bitstream = Bitstream +1;
			for (i=0; i<iv_URL_length; i++) {
				//URL_byte = Read8(Bitstream);
				Bitstream = Bitstream +1;
			}

			/****** keyUrlTemplate  **********/
			keyUrlTemplate_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<keyUrlTemplate_length; i++) {
				//length_byte = Read8(Bitstream);
				Bitstream = Bitstream+1;
			}

			/********* ivUrlTemplate ************/
			ivUrlTemplate_length = Read8(Bitstream);
			Bitstream = Bitstream +1;
			for (i=0; i<ivUrlTemplate_length; i++) {
				//length_byte = Read8(Bitstream);
				Bitstream = Bitstream+1;
			}

			onebyte = Read8(Bitstream);
			Bitstream = Bitstream +1;
			//reserved  “1111 111” 	   	7  bslbf
			authentication_flag = onebyte &0x01;
			if (authentication_flag == 1) 
			{

				/************** digestUrl ******************/
				digest_URL_length = Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<digest_URL_length; i++) {
					//URL_byte = Read8(Bitstream);
					Bitstream = Bitstream +1;
				}

				/****************** digestURLTemplate *********/
				digestUrlTemplate_length = Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<digestUrlTemplate_length; i++) {
					//length_byte = Read8(Bitstream);
					Bitstream = Bitstream+1;
				}

				/********* signatureUrl ****************/
				signature_URL_length = Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<signature_URL_length; i++) {
					//URL_byte = Read8(Bitstream);
					Bitstream = Bitstream +1;
				}

				/******** signatureURLTemplate ***************/
				signatureUrlTemplate_length = Read8(Bitstream);	
				Bitstream = Bitstream +1;
				for (i=0; i<signatureUrlTemplate_length; i++) {
					//length_byte = Read8(Bitstream);
					Bitstream = Bitstream+1;
				}

				/***************** signatureLength *****************/
				signature_length = Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<signature_length; i++) {
					//length_byte = Read8(Bitstream);
					Bitstream = Bitstream+1;
				}

				/************** signatureKeyUrl **********************/
				signatureKey_URL_length	= Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<signatureKey_URL_length; i++) {
					//URL_byte = Read8(Bitstream);
					Bitstream = Bitstream +1;
				}

				number_of_License = Read8(Bitstream);
				Bitstream = Bitstream +1;
				for (i=0; i<number_of_License; i++) 
				{

					/******* license_Url *********/
					licenseUrl_length = Read8(Bitstream);
					Bitstream = Bitstream +1;
					for (j=0; j<licenseUrl_length; j++) {
						//URL_byte = Read8(Bitstream);
						Bitstream = Bitstream +1;
					}	
					/********* licenseURLTemplate ************************/
					lisenceUrlTemplate_length = Read8(Bitstream);
					Bitstream = Bitstream +1;
					for (i=0; i<lisenceUrlTemplate_length; i++) {
						//length_byte = Read8(Bitstream);
						Bitstream = Bitstream+1;
					}
				}

			}
		}
	}

}


/**************************** MESSAGES ********************************************/

void PA_Message(unsigned char* Bitstream, GstMMTDemux *mmtdemux)
{
	unsigned short int number_of_tables ;
	unsigned int i =0;
	Table_Info N1[1024];

	GST_DEBUG_OBJECT(NULL,"PA_signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	number_of_tables = Read8(Bitstream);
	Bitstream=Bitstream+1;
	GST_DEBUG_OBJECT(NULL,"number_of_tables=%u\n",number_of_tables);

	for(i=0;i<number_of_tables;i++)
	{
		N1[i].table_id = Read8(Bitstream);
		Bitstream=Bitstream+1;
		N1[i].version = Read8(Bitstream);
		Bitstream=Bitstream+1;
		N1[i].length = Read16(Bitstream);
		Bitstream=Bitstream+2;

	}	

	for(i=0;i<number_of_tables;i++)
	{
		switch(N1[i].table_id)
		{

			case 0x00:
				PA_table(Bitstream);
				break;
			case 0x01 ... 0x10:
				MPI_table(Bitstream);
				break;
			case 0x11 ... 0x20:
				MP_table(Bitstream, mmtdemux);
				break;
			case 0x21:
				CRI_table(Bitstream);
				break;
			case 0x22:
				DCI_table(Bitstream);
				break;
			case 0x23:
				SI_table(Bitstream);
				break;
			case 0x24 ... 0x7F :
				GST_DEBUG_OBJECT(NULL,"Rerseved for ISO use (16 bit length table)\n");
				break;
			case 0x80 ... 0xFF :
				GST_DEBUG_OBJECT(NULL,"Reserved for Private use\n");
				break;
			default:
				break;
		}

	}


}

void MPI_Message(unsigned char *Bitstream, GstMMTDemux *mmtdemux)
{
	unsigned char extension;
	unsigned short int associated_MP_table_flag ; 

	GST_DEBUG_OBJECT(NULL,"MPI_signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	extension = Read8(Bitstream);
	sig_length = sig_length +1;

	associated_MP_table_flag = extension >> 7 ;
	GST_DEBUG_OBJECT(NULL,"MPI associated_MP_table_flag=%hd\n",associated_MP_table_flag);

	MPI_table(Bitstream);
	if(associated_MP_table_flag)
		MP_table(Bitstream, mmtdemux);

}
void MPT_Message(unsigned char *Bitstream, GstMMTDemux *mmtdemux)
{

	GST_DEBUG_OBJECT(NULL,"MPT_signal message id=%u,version=%hd,length=%hd \n",sig_msg.message_id,sig_msg.version,sig_msg.length);
	MP_table(Bitstream, mmtdemux);

}	
void CRI_Message (unsigned char* Bitstream)
{

	GST_DEBUG_OBJECT(NULL,"CRI_signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);
	CRI_table(Bitstream);

}

void DCI_Message (unsigned char* Bitstream)
{

	GST_DEBUG_OBJECT(NULL,"DCI_signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);
	DCI_table(Bitstream);

}

void SSWR_Message (unsigned char* Bitstream) 
{
	unsigned int i=0,j=0;
	unsigned short token_ID_URI_length;	//N1 8 uimsbf
	//unsigned short token_ID_URI_byte;	// 8 bslbf

	unsigned short Number_of_deviceID;	//N2 8 uimsbf
	unsigned short deviceID_length;		//N3 8 uimsbf
	//unsigned short deviceID_byte;  		//8 uimsbf

	unsigned short tokenIssuer_ID_URI_length;	//N4 8 uimsbf
	//unsigned short tokenIssuer_URI_bytes;		//8 uimsbf

	//unsigned long  tokenIssueTime;			//64 uimsbf
	//unsigned long  tokenExpireTime;			//64 uimsbf

	unsigned short number_of_tables;		//N5 8 uimsbf
	//unsigned short MMT_signaling_table_id;		//8  uimsbf
	//unsigned short MMT_signaling_table_version;	//8 uimsbf

	GST_DEBUG_OBJECT(NULL,"SWR_signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	/******************* token_ID **************************/
	token_ID_URI_length = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length =  sig_length + 1;

	for (i=0; i<token_ID_URI_length; i++) {
		//token_ID_URI_byte=Read8(Bitstream);	
		Bitstream = Bitstream+1;
		sig_length = sig_length +1;		
	}

	Number_of_deviceID=Read8(Bitstream);	
	Bitstream = Bitstream+1;	
	sig_length = sig_length +1;		

	for (i=0; i<Number_of_deviceID; i++) 
	{
		/************** device_ID *************************/
		deviceID_length	= Read8(Bitstream);
		Bitstream = Bitstream+1;		
		sig_length = sig_length +1;		
		for (j=0; j<deviceID_length; j++) {
			//deviceID_byte = Read8(Bitstream);
			Bitstream=Bitstream+1;		
			sig_length = sig_length +1;		
		}
	}

	/***************** tokenIssure_ID ***************************/
	tokenIssuer_ID_URI_length = Read8(Bitstream);
	Bitstream=Bitstream+1;		
	sig_length = sig_length +1;		
	for (i=0; i<tokenIssuer_ID_URI_length; i++) {
		//tokenIssuer_URI_bytes = Read8(Bitstream);
		Bitstream=Bitstream+1;		
		sig_length = sig_length +1;		
	}

	//tokenIssueTime = Read64(Bitstream);
	Bitstream=Bitstream+8;
	sig_length = sig_length +8;		
	//tokenExpireTime = Read64(Bitstream);
	Bitstream=Bitstream+8;
	sig_length = sig_length +8;		

	/***************** information_table_info ********************/
	number_of_tables=Read8(Bitstream);
	Bitstream=Bitstream+1;		
	sig_length = sig_length +1;		
	for (i=0; i<number_of_tables; i++) {
		//MMT_signaling_table_id= Read8(Bitstream);	
		Bitstream=Bitstream+1;		
		sig_length = sig_length +1;		
		//MMT_signaling_table_version=Read8(Bitstream);
		Bitstream=Bitstream+1;		
		sig_length = sig_length +1;		
	}

}


void AL_FEC_Message( unsigned char* Bitstream )
{
	unsigned int i,j,k,l;
	unsigned char onebyte;
	unsigned short fec_flag ; 			//1 bslbf
	unsigned short private_fec_flag;		//1 bslbf
	//unsigned short length_of_fec_flow_descriptor; 	//16 uimsbf
	unsigned short number_of_fec_flows;		//N1 8 uimbsf

	//unsigned short fec_flow_id;			//8 uimbsf
	//unsigned short source_flow_id;			//8 uimbsf
	unsigned short number_of_assets;		//N2 8 uimbsf
	//unsigned short packet_id;			//16 uimbsf

	unsigned short fec_coding_structure;		//4 uimbsf
	unsigned short ssbg_mode;			//2 uimbsf
	//unsigned short ffsrpts_flag;			//1 bslbf
	//reserved  “1”		1  bslbf
	//unsigned short 	length_of_repair_symbol;			//16 uimbsf
	//unsigned short	num_of_repair_symbol_per_packet;		//16 uimbsf
	//unsigned short 	num_of_symbol_element_per_source_symbol;	//16 uimbsf

	//unsigned short repair_flow_id;			//8 uimbsf
	//unsigned short fec_code_id_for_repair_flow;	//8 uimbsf
	//unsigned short private_flag;			//1 bslbf
	//unsigned short private_field_length;		//N3 7 bslbf
	//unsigned short private_field;			//N3*8 uimbsf
	//unsigned int maximum_k_for_repair_flow;		//24 uimbsf
	//unsigned int maximum_p_for_repair_flow;		//24 uimbsf
	//unsigned int protection_window_time;		//32 uimbsf
	//unsigned int protection_window_size; 		//32 uimbsf

	//unsigned short num_of_sub_block_per_source_block; 	//8 uimbsf
	unsigned short number_of_parity_flows;			//N5 8 uimbsf
	unsigned short num_of_layer_for_LAFEC;			//8 uimbsf


	GST_DEBUG_OBJECT(NULL,"AL_FEC signal message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length+1;

	fec_flag = onebyte&0x01;
	private_fec_flag = onebyte&0x02;
	//reserved “1111 11” 6 bslbf
	GST_DEBUG_OBJECT(NULL,"fec_flag =%hd\t private_fec_flag = %hd \n",fec_flag,private_fec_flag);
	if (fec_flag==1) 
	{
		//length_of_fec_flow_descriptor = Read16(Bitstream);
		Bitstream = Bitstream +2;
		sig_length = sig_length+2;
		/******** fec_flow_descriptor() *******/
		number_of_fec_flows = Read8(Bitstream);
		Bitstream = Bitstream +1;
		sig_length = sig_length+1;
		for ( i=0; i<number_of_fec_flows ; i++) 
		{
			//fec_flow_id = Read8(Bitstream);		
			Bitstream = Bitstream +1;
			sig_length = sig_length+1;

			//source_flow_id	= Read8(Bitstream);	
			Bitstream = Bitstream +1;
			sig_length = sig_length+1;

			number_of_assets = Read8(Bitstream); 
			Bitstream = Bitstream +1;
			sig_length = sig_length+1;

			for ( j=0; j<number_of_assets ;j++) 
			{
				//packet_id = Read16(Bitstream);
				Bitstream = Bitstream +2;
				sig_length = sig_length+2;
			}

			onebyte = Read8(Bitstream);
			Bitstream = Bitstream +1;
			sig_length = sig_length+1;

			fec_coding_structure = (onebyte&0xF0)>>4;
			ssbg_mode = (onebyte&0x0c)>>2;
			//ffsrpts_flag = (onebyte&0x02)>>1;
			//reserved  “1”		1  bslbf

			//length_of_repair_symbol = Read16(Bitstream);
			Bitstream = Bitstream +2;
			sig_length = sig_length+2;
			if (ssbg_mode == 2 /*10 */ ) 
			{
				//num_of_repair_symbol_per_packet = Read16(Bitstream);
				Bitstream = Bitstream +2;
				sig_length = sig_length+2;
				//num_of_symbol_element_per_source_symbol = Read16(Bitstream);	
				Bitstream = Bitstream +2;
				sig_length = sig_length+2;
			}
			if (fec_coding_structure == 1) 
			{
				//repair_flow_id	= Read8(Bitstream);
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				//fec_code_id_for_repair_flow = Read8(Bitstream);
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				if (private_fec_flag == 1) 
				{
					onebyte = Read8(Bitstream);
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
				//TODO 
					//private_flag = onebyte&0x01;  // 1 bslbf
					//private_field_length = (onebyte&0xFE)>>1; //N3 7 bslbf
					//private_field = N3*8 uimbsf
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
				}
				//maximum_k_for_repair_flow = Read24(Bitstream);  
				Bitstream = Bitstream +3;
				sig_length = sig_length+3;
				//maximum_p_for_repair_flow = Read24(Bitstream); 
				Bitstream = Bitstream +3;
				sig_length = sig_length+3;
				//protection_window_time	= Read32(Bitstream);
				Bitstream = Bitstream +4;
				sig_length = sig_length+4;
				//protection_window_size = Read32(Bitstream);
				Bitstream = Bitstream +4;
				sig_length = sig_length+4;
			}

			if (fec_coding_structure == 2) 
			{

				//num_of_sub_block_per_source_block = Read8(Bitstream);
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				number_of_parity_flows = Read8(Bitstream);	//N5 8 uimbsf
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				for ( k=0; k<number_of_parity_flows ;k++)
				{
					//repair_flow_id = Read8(Bitstream);
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
					//fec_code_id_for_repair_flow = Read8(Bitstream);	
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
					if (private_fec_flag == 1) 
					{
						onebyte = Read8(Bitstream);
						Bitstream = Bitstream +1;
						sig_length = sig_length+1;
					//TODO
						//private_flag = onebyte&0x01;  // 1 bslbf
						//private_field_length = (onebyte&0xFE)>>1; //N6 7 bslbf
						//	private_flag			N6*8 uimbsf
						Bitstream = Bitstream +1;
						sig_length = sig_length+1;
					}
					//maximum_k_for_repair_flow = Read24(Bitstream);  
					Bitstream = Bitstream +3;
					sig_length = sig_length+3;
					//maximum_p_for_repair_flow = Read24(Bitstream); 
					Bitstream = Bitstream +3;
					sig_length = sig_length+3;
					//protection_window_time	= Read32(Bitstream);
					Bitstream = Bitstream +4;
					sig_length = sig_length+4;
					//protection_window_size = Read32(Bitstream);
					Bitstream = Bitstream +4;
					sig_length = sig_length+4;
				}

			}
			if (fec_coding_structure == 3) 
			{
				num_of_layer_for_LAFEC = Read8(Bitstream); 	//N4 8 uimbsf
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				//fec_code_id_for_repair_flow = Read8(Bitstream); 
				Bitstream = Bitstream +1;
				sig_length = sig_length+1;
				if (private_fec_flag == 1) 
				{
					onebyte = Read8(Bitstream);
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
				//TODO 
					//private_flag = onebyte&0x01;  // 1 bslbf
					//private_field_length = (onebyte&0xFE)>>1; //N7 7 bslbf
					//pivate_flag			N7*8 uimbsf
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
				}
				for ( l=0;l<num_of_layer_for_LAFEC ;l++)
				{
					//repair_flow_id = Read8(Bitstream); 
					Bitstream = Bitstream +1;
					sig_length = sig_length+1;
					//maximum_k_for_repair_flow = Read24(Bitstream); 
					Bitstream = Bitstream +3;
					sig_length = sig_length+3;
					//maximum_p_for_repair_flow = Read24(Bitstream); 
					Bitstream = Bitstream +3;
					sig_length = sig_length+3;
					//protection_window_time	= Read32(Bitstream);
					Bitstream = Bitstream +4;
					sig_length = sig_length+4;
					//protection_window_size = Read32(Bitstream);
					Bitstream = Bitstream +4;
					sig_length = sig_length+4;
				}

			}
		}
	}

}


void HRBM_Message (unsigned char *Bitstream )
{
	unsigned int fixed_end_to_end_delay;	// 32 uimsbf
	unsigned int max_transmission_delay;	// 32 uimsbf
	unsigned int max_buffer_size;  		// 32 uimsbf

	GST_DEBUG_OBJECT(NULL,"HRBM_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	fixed_end_to_end_delay = Read32(Bitstream);
	Bitstream=Bitstream+4;
	sig_length = sig_length+4;
	max_transmission_delay=Read32(Bitstream);	
	Bitstream=Bitstream+4;
	sig_length = sig_length+4;
	max_buffer_size  = Read32(Bitstream); 
	Bitstream=Bitstream+4;
	sig_length = sig_length+4;

	GST_DEBUG_OBJECT(NULL,"fixed_end_to_end_delay = %u milliseconds\nmax_transmission_delay = %u milliseconds\nmax_buffer_size = %u bytes of MMT Assets\n",fixed_end_to_end_delay,max_transmission_delay,max_buffer_size);

}

void MC_Message(unsigned char*Bitstream) 
{
	unsigned char onebyte;
	unsigned short measurement_mode;	//2 bslbf
	unsigned int measurement_start_time = 0;	//32 uimsbf
	unsigned int measurement_stop_time;	//32  uimsbf
	unsigned int measurement_period;	//32  uimsbf
	unsigned short report_type;		//8 bslbf
	unsigned int nbytes=0;

	GST_DEBUG_OBJECT(NULL,"MC_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	onebyte=Read8(Bitstream);
	Bitstream=Bitstream+1;
	sig_length = sig_length +1;		

	//reserved		“1111 11”   6 bslbf
	measurement_mode = onebyte&0x03;
	GST_DEBUG_OBJECT(NULL,"measurement_mode=%hd\n",measurement_mode);
	if(measurement_mode != 3) 
	{
		if(measurement_mode == 1){
			measurement_start_time=Read32(Bitstream);
			Bitstream=Bitstream+4;
			sig_length = sig_length +4;		
		}else if(measurement_mode == 2) {
			//measurement_start_condition();
		}

		measurement_stop_time = Read32(Bitstream);
		Bitstream=Bitstream+4;
		sig_length = sig_length +4;		
		measurement_period=Read32(Bitstream);
		Bitstream=Bitstream+4;
		sig_length = sig_length +4;		

		/******** measurement report  and server_address ********/
		nbytes = MMT_general_location_info(Bitstream);
		Bitstream = Bitstream+nbytes;
		sig_length = sig_length +nbytes;		

		report_type = Read8(Bitstream);
		Bitstream=Bitstream+1;
		sig_length = sig_length +1;		

		GST_DEBUG_OBJECT(NULL,"measurement_start_time=%u\tmeasurement_stop_time%u\tmeasurement_period=%u\treport_type=%hd\n",measurement_start_time,measurement_stop_time,measurement_period,report_type);

	}

}

void AC_Message(unsigned char *Bitstream) 
{
	unsigned int i=0,nbytes=0;
	unsigned char onebyte;
	unsigned short packet_id;			//16 uimsbf
	unsigned int rtx_window_timeout;		//32 uimsbf

	//unsigned short delay_constrained_ARQ_flag;	//1 bslbf
	unsigned short number_of_packet_id;		//N1 7 uimsbf
	//unsigned short fb_flow_label; 			// 7 uimsbf
	unsigned short flow_label_flag;			//1 bslbf

	GST_DEBUG_OBJECT(NULL,"AC_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;		

	flow_label_flag = onebyte&0x01;
	if(flow_label_flag == 1) {
		//fb_flow_label = (onebyte&0xFE)>>1;
	} else {
		//reserved	“1111 111” 7 bslbf
	}

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;		

	//delay_constrained_ARQ_flag = onebyte&0x01 ;
	number_of_packet_id = onebyte&0xFE ;	
	GST_DEBUG_OBJECT(NULL,"number_of_packet_id = %hd and ",number_of_packet_id);
	for(i=0; i<number_of_packet_id; i++) 
	{
		packet_id = Read16(Bitstream);
		Bitstream = Bitstream+2;
		sig_length = sig_length +2;		
		rtx_window_timeout = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length +4;		
		GST_DEBUG_OBJECT(NULL,"%hd,%u ",packet_id,rtx_window_timeout);

	}
	GST_DEBUG_OBJECT(NULL,"\n");

	/******** arq_server_address ***********/
	nbytes = MMT_general_location_info(Bitstream); //loaction type restricted to 0x01 0x02 and 0x05 for simplicity
	Bitstream = Bitstream+nbytes;
	sig_length = sig_length +nbytes;		
	
}

void AF_Message(unsigned char *Bitstream) 
{
	unsigned int i=0,j=0;
	unsigned char onebyte;
	unsigned short argument_type ;  	        //1 bslbf
	unsigned short delay_constrained_ARQ_mode ;	//2 uimsbf

	unsigned int ARQ_feedback_timestamp=0;		//32 uimsbf
	unsigned int propagation_delay=0;			//32 uimsbf
	//unsigned int packet_counter;			//32 uimsbf
	unsigned short masklength;			//N1 8 uimsbf
	//unsigned short arrival_deadline	;		//16 uimsbf
	//unsigned short mask_byte; 			//8 uimsbf

	unsigned short number_of_packet_id ;   		// N2 7 uimsbf
	unsigned short packet_id;			//16 uimsbf
	//unsigned int packet_sequence_number; 		//32 uimsbf

	GST_DEBUG_OBJECT(NULL,"AF_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;		

	//reserved	‘1111 1’  5 uimsbf
	argument_type = (onebyte&0x08)>>2;	
	delay_constrained_ARQ_mode = (onebyte&0x03); 
	GST_DEBUG_OBJECT(NULL,"argument_type = %hd\tdelay_constrained_ARQ_mode=%hd\n",argument_type,delay_constrained_ARQ_mode);

	if(argument_type == 0) 
	{
		if (delay_constrained_ARQ_mode == 1){
			ARQ_feedback_timestamp = Read32(Bitstream);
			Bitstream = Bitstream + 4;
			sig_length = sig_length +4;		

		}
		if (delay_constrained_ARQ_mode == 2){
			propagation_delay = Read32(Bitstream);	
			Bitstream = Bitstream + 4;
			sig_length = sig_length +4;		

		}

		//packet_counter = Read32(Bitstream);
		Bitstream = Bitstream + 4;
		sig_length = sig_length +4;		
		masklength = Read8(Bitstream);
		Bitstream = Bitstream +1;
		sig_length = sig_length +1;		


		if (delay_constrained_ARQ_mode == 1){
			//arrival_deadline = Read16(Bitstream);
			Bitstream = Bitstream +2;
			sig_length = sig_length +2;		

		}
		for(i=0; i<masklength; i++){
			//mask_byte = Read8(Bitstream);
			Bitstream = Bitstream +1;
			sig_length = sig_length +1;		

		}
	}

	if(argument_type == 1) 
	{
		onebyte = Read8(Bitstream);
		sig_length = sig_length +1;		

		//reserved  ‘1’  1 bslbf
		number_of_packet_id = onebyte&0x7F;

		if (delay_constrained_ARQ_mode == 1){
			ARQ_feedback_timestamp = Read32(Bitstream);
			Bitstream = Bitstream + 4;
			sig_length = sig_length +4;		

		}

		if (delay_constrained_ARQ_mode == 2){
			propagation_delay = Read32(Bitstream);
			Bitstream = Bitstream + 4;
			sig_length = sig_length +4;		

		}
		GST_DEBUG_OBJECT(NULL,"number_of_packet_id = %hd are \n",number_of_packet_id);
		for(i=0; i<number_of_packet_id; i++)
		{
			packet_id = Read16(Bitstream);
			Bitstream = Bitstream + 2;
			sig_length = sig_length +2;		

			//packet_sequence_number = Read32(Bitstream);
			Bitstream = Bitstream + 4;
			sig_length = sig_length +4;		

			masklength = Read8(Bitstream);		
			Bitstream = Bitstream + 1;
			sig_length = sig_length +1;		

			if (delay_constrained_ARQ_mode == 1){
				//arrival_deadline = Read16(Bitstream);	
				Bitstream = Bitstream + 2;
				sig_length = sig_length +2;		

			}

			for(j=0; j<masklength; j++){
				//mask_byte = Read8(Bitstream);
				Bitstream = Bitstream + 1;
				sig_length = sig_length +1;		

			}
			GST_DEBUG_OBJECT(NULL,"%hd, ",packet_id);
		}
		GST_DEBUG_OBJECT(NULL,"\n\n");

	}

	GST_DEBUG_OBJECT(NULL,"ARQ_feedback_timestamp=%u propagation_delay=%u\n",ARQ_feedback_timestamp,propagation_delay);
}


void RQF_Message (unsigned char*Bitstream ) 
{
	unsigned short packet_loss_ratio;	//8 unsigned char	
	unsigned int inter_arrival_jitter;	//32 unsigned integer
	unsigned int max_transmission_delay;	//32 unsigned integer
	unsigned int min_transmission_delay;	//32 unsigned integer

	unsigned int propagation_delay;		// 32 unsigned integer
	unsigned int feedback_timestamp;	// 32 unsigned integer

	GST_DEBUG_OBJECT(NULL,"RQF_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	packet_loss_ratio      = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;
	inter_arrival_jitter   = Read32(Bitstream);
	Bitstream = Bitstream+4;
	sig_length = sig_length +4;
	max_transmission_delay = Read32(Bitstream);
	Bitstream = Bitstream+4;
	sig_length = sig_length +4;
	min_transmission_delay = Read32(Bitstream);
	Bitstream = Bitstream+4;
	sig_length = sig_length +4;
	/********** RTT_parameter( ) **************/
	propagation_delay  = Read32(Bitstream);	
	Bitstream = Bitstream+4;
	sig_length = sig_length +4;
	feedback_timestamp = Read32(Bitstream);	
	Bitstream = Bitstream+4;
	sig_length = sig_length +4;


	GST_DEBUG_OBJECT(NULL,"packet_loss_ratio=%hd\tinter_arrival_jitter=%u\tmax_transmission_delay=%u\tmin_transmission_delay=%u",packet_loss_ratio,inter_arrival_jitter,max_transmission_delay,min_transmission_delay);

	GST_DEBUG_OBJECT(NULL,"propagation_delay=%u\t feedback_timestamp=%u\n",propagation_delay,feedback_timestamp);

}


void ADC_Message (unsigned char *Bitstream) 
{
	unsigned int i=0;
	unsigned char onebyte;
	//unsigned int validity_start_time;	//32 uimsbf
	//unsigned int validity_duration;		//32 uimsbf
	unsigned short 	ADC_level_flag;		//1 boolean
	unsigned short	flow_label_flag;	//1 boolean
	unsigned int	MPU_sequence_number=0; 	//32 uimsbf
	//unsigned short 	packet_id;		//16 uimsbf

	//unsigned short	loss_tolerance;		//8 bslbf
	//unsigned short	jitter_sensitivity;	//8 bslbf
	unsigned short	class_of_service;	//1 boolean
	//unsigned short	bidirection_indicator;	//1 boolean

	//unsigned short spatial_quality;		//16 uimsbf
	//unsigned short temporal_quality;	//16 uimsbf
	//unsigned int   aggregate_rate;		//32 uimsbf
	unsigned short n_samples;		// 16 uimsbf
	//unsigned short sample_group_index;	//16 uimsbf

	//unsigned short 	sustainable_rate;	//16 uimsbf
	//unsigned short 	buffer_size;		//16 uimsbf
	//unsigned short 	peak_rate;		//16 uimsbf
	//unsigned short 	max_MFU_size;		//8 uimsbf
	//unsigned short 	mfu_period;		//8 uimsbf

	GST_DEBUG_OBJECT(NULL,"ADC_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	//validity_start_time = Read32(Bitstream);
	Bitstream = Bitstream+4;
	sig_length = sig_length + 4;
	//validity_duration  = Read32(Bitstream);	
	Bitstream = Bitstream+4;	
	sig_length = sig_length + 4;
	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;	
	sig_length = sig_length +1; 

	ADC_level_flag = onebyte&0x01;	
	flow_label_flag = onebyte&0x02;	
	//reserved		6 uimsbf

	if (ADC_level_flag == 1){
		MPU_sequence_number = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length + 4;
	}
	 GST_DEBUG_OBJECT(NULL,"MPU_sequence_number: %u\n", MPU_sequence_number);

	//packet_id = Read16(Bitstream);	
	Bitstream = Bitstream+2;
	sig_length = sig_length + 2; 

	/************** qos_descriptor **************/
	//loss_tolerance = Read8(Bitstream);
	Bitstream = Bitstream+1;	
	sig_length = sig_length +1; 
	//jitter_sensitivity = Read8(Bitstream);	
	Bitstream = Bitstream+1;	
	sig_length = sig_length +1; 
	onebyte=Read8(Bitstream);
	Bitstream = Bitstream+1;	
	sig_length = sig_length +1; 
	class_of_service = onebyte&0x01;
	//bidirection_indicator = onebyte&0x02;
	//reserved		6 bslbf

	/*************** qoe_descriptor ************/
	n_samples = Read16(Bitstream);
	Bitstream = Bitstream+2;
	sig_length = sig_length +2; 
	for (i=0;i<n_samples; i++) {
		//sample_group_index = Read16(Bitstream);	
		Bitstream=Bitstream+2;
		sig_length = sig_length +2; 
	}
	//spatial_quality = Read16(Bitstream);
	Bitstream = Bitstream+2;
	sig_length = sig_length +2; 
	//temporal_quality = Read16(Bitstream);
	Bitstream = Bitstream+2;
	sig_length = sig_length +2; 
	//aggregate_rate = Read16(Bitstream);	
	Bitstream = Bitstream+2;
	sig_length = sig_length +2; 
	if (class_of_service == 1)
	{
		/************* bitstream_descriptor_vbr ***************/
		//sustainable_rate = Read16(Bitstream);
		Bitstream=Bitstream+2;
		sig_length = sig_length +2; 
		//buffer_size	= Read16(Bitstream);
		Bitstream=Bitstream+2;
		sig_length = sig_length +2; 
		//peak_rate = Read16(Bitstream);	
		Bitstream = Bitstream+2;
		sig_length = sig_length +2; 
		//max_MFU_size = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length +1; 
		//mfu_period = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length +1; 
	}else{
		/*************** bitstream_descriptor_cbr ***********/
		//peak_rate = Read16(Bitstream);	
		Bitstream = Bitstream+2;
		sig_length = sig_length +2; 
		//max_MFU_size = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length +1; 
		//mfu_period = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length +1; 
		if(flow_label_flag == 1) 
		{
			onebyte = Read8(Bitstream);
			Bitstream = Bitstream+1;
			sig_length = sig_length +1; 
			//flow_label		7 uimsbf
			//reserved		1 uimsbf
		}
	}

}

void HRBM_Removal_Message(unsigned char* Bitstream )
{
	unsigned int i=0;
	unsigned short number_of_operation_modes;	//8 uimsbf
	unsigned short data_removal_type;		//8 uimsbf
	unsigned int   max_decapsulation_buffer_size;   //32 uimsbf
	unsigned short buffer_management_valid;		//1 bslbf
	unsigned char onebyte;

	GST_DEBUG_OBJECT(NULL,"HRBMR_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	number_of_operation_modes = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;
	GST_DEBUG_OBJECT(NULL,"number_of_operation_modes =%hd\n",number_of_operation_modes);
	for(i=0; i<number_of_operation_modes; i++)
	{
		data_removal_type = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length +1;

		max_decapsulation_buffer_size = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length +4;
		GST_DEBUG_OBJECT(NULL,"data_removal_type =%hd\t max_decapsulation_buffer_size =%u\n",data_removal_type,max_decapsulation_buffer_size);
	}

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length +1;
	buffer_management_valid = onebyte&0x01;
	//reserved		7 uimsbf
	GST_DEBUG_OBJECT(NULL,"buffer_management_valid = %hd\n",buffer_management_valid);
}

void LS_Message (unsigned char* Bitstream) 
{
	unsigned int i=0;
	unsigned char system_id[128]={'\0'};			//16*8 uimsbf
	unsigned int number_of_licenses; 			//N1   32 uimsbf

	unsigned short license_message_hash_length;		//N2 8 uimsbf
	unsigned char license_message_hash[512]={'\0'};		//8*N2 uimsbf
	unsigned short encrypted_license_data_length;		//N3 16 uimsbf
	unsigned char encrypted_license_data[512]={'\0'};	//8*N3 uimsbf

	GST_DEBUG_OBJECT(NULL,"LS_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	memcpy(system_id ,Bitstream,128);
	Bitstream = Bitstream + 16;
	sig_length = sig_length + 16;		
	number_of_licenses = Read32(Bitstream);
	Bitstream = Bitstream + 4;
	sig_length = sig_length + 4;		
	for(i=0;i<number_of_licenses;i++) 
	{
		license_message_hash_length=Read8(Bitstream);  
		Bitstream=Bitstream+1;
		sig_length = sig_length + 1;		
		memcpy(license_message_hash,Bitstream,8*license_message_hash_length);
		Bitstream = Bitstream + 8*license_message_hash_length;
		sig_length = sig_length + 8*license_message_hash_length;		
		encrypted_license_data_length = Read16(Bitstream);
		Bitstream = Bitstream + 2;
		sig_length = sig_length + 2;		
		memcpy(encrypted_license_data,Bitstream,8*encrypted_license_data_length);
		Bitstream = Bitstream + 8*encrypted_license_data_length;
		sig_length = sig_length + 8*encrypted_license_data_length;		
	}

}


void LR_Message (unsigned char * Bitstream) 
{
	unsigned int i=0;
	unsigned char system_id[128]={'\0'};			//16*8 uimsbf
	unsigned int number_of_licenses; 			//N1   32 uimsbf

	unsigned short encrypted_license_challenge_length;	//N2   16 uimsbf
	unsigned char encrypted_license_challenge[512]={'\0'};	//8*N2 uimsbf

	GST_DEBUG_OBJECT(NULL,"LR_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	memcpy(system_id ,Bitstream,128);
	Bitstream = Bitstream + 16;
	sig_length = sig_length +16;		
	number_of_licenses = Read32(Bitstream);
	Bitstream = Bitstream + 4;
	sig_length = sig_length+4;

	for(i=0;i<number_of_licenses;i++) {
		encrypted_license_challenge_length = Read16(Bitstream);
		Bitstream = Bitstream + 2;
		sig_length = sig_length +2;
		memcpy(encrypted_license_challenge,Bitstream,8*encrypted_license_challenge_length);
		sig_length = sig_length + 8*encrypted_license_challenge_length;
	}

}


void NAMF_Message(unsigned char*Bitstream)
{
	unsigned char onebyte;
	unsigned short NAM_flag;			//1 unsigned integer
	unsigned short CLI_id;				//8 unsigned integer
	unsigned char relative_available_bitrate;	//8 float
	unsigned char relative_buffer_fullness;		//8 float
	unsigned char relative_peak_bitrate;		//8 float
	unsigned short average_bitrate_period;		//16 unsigned integer
	float current_delay;		//32 float
	float generation_time;		//32 float
	float BER;			//32 float

	float available_bitrate;	//32 float
	float buffer_fullness;		//32 float
	float peak_bitrate;		//32 float
	unsigned int SDU_size;		//32 unsigned integer
	unsigned short SDU_loss_ratio;  //8 unsigned integer

	GST_DEBUG_OBJECT(NULL,"NAMF_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	onebyte = Read8(Bitstream);
	Bitstream = Bitstream+1;
	sig_length = sig_length+1;

	//extension {
	NAM_flag = onebyte&0x01;
	//reserved	‘111 1111’ 7 unsigned integer
	//}

	if(NAM_flag == 0) {
		CLI_id = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		relative_available_bitrate = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		relative_buffer_fullness = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		relative_peak_bitrate = Read8(Bitstream);	
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		average_bitrate_period = Read16(Bitstream);
		Bitstream = Bitstream+2;
		sig_length = sig_length+2;

		current_delay = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		generation_time	= Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		BER = Read32(Bitstream);		
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;
		
		GST_DEBUG_OBJECT(NULL,"CLI_id = %d\n relative_available_bitrate = %c\n relative_buffer_fullness = %f\n relative_peak_bitrate = %f\n average_bitrate_period = %hd\n",CLI_id,relative_available_bitrate,(float)relative_buffer_fullness,(float)relative_peak_bitrate,average_bitrate_period);

		GST_DEBUG_OBJECT(NULL," current_delay = %f\n generation_time = %f\n BER = %f\n",current_delay,generation_time,BER);

	}else if(NAM_flag == 1) {
		CLI_id = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		available_bitrate = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		buffer_fullness = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		peak_bitrate = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		current_delay = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		average_bitrate_period = Read16(Bitstream);
		Bitstream = Bitstream+2;
		sig_length = sig_length+2;

		SDU_size = Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		SDU_loss_ratio = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;

		generation_time	= Read32(Bitstream);
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		BER = Read32(Bitstream);		
		Bitstream = Bitstream+4;
		sig_length = sig_length+4;

		GST_DEBUG_OBJECT(NULL,"CLI_id = %d\n available_bitrate = %f\n buffer_fullness = %f\n peak_bitrate = %f\n",CLI_id,available_bitrate,buffer_fullness,peak_bitrate);

		GST_DEBUG_OBJECT(NULL," current_delay = %f\n average_bitrate_period = %hd\n SDU_size = %u\n SDU_loss_ratio = %hd\n generation_time = %f\n BER = %f\n",current_delay,average_bitrate_period,SDU_size,SDU_loss_ratio,generation_time,BER);
	}

}


void LDC_Message (unsigned char* Bitstream )
{
	unsigned int temp,i;
	unsigned int base_presentation_time_offset; 		//31 uimsbf
	unsigned short coding_dependency_structure_flag ;	//1 bslbf
	unsigned short period_of_intra_coded_sample;		//N1  8 uimsbf
	unsigned short sample_composition_time_offset_sign;	//1 bslbf
	unsigned int sample_composition_time_offset_value;	//31 uimsbf

	GST_DEBUG_OBJECT(NULL,"LDC_message id=%u,version=%hd,length=%u \n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	temp = Read32(Bitstream);
	Bitstream = Bitstream+4;
	sig_length = sig_length+4;

	base_presentation_time_offset = temp&0xFFFE; 
	coding_dependency_structure_flag = temp&0x0001; 
	
	GST_DEBUG_OBJECT(NULL,"base_presentation_time_offset = %u\n coding_dependency_structure_flag = %hd\n",base_presentation_time_offset,coding_dependency_structure_flag);

	if (coding_dependency_structure_flag == 1)
	{
		period_of_intra_coded_sample = Read8(Bitstream);
		Bitstream = Bitstream+1;
		sig_length = sig_length+1;
		GST_DEBUG_OBJECT(NULL,"period_of_intra_coded_sample = %hd \n",period_of_intra_coded_sample);
		for (i=0 ; i<period_of_intra_coded_sample;i++)
		{
			temp = Read32(Bitstream);
			Bitstream = Bitstream+4;
			sig_length = sig_length+4;
			sample_composition_time_offset_sign = temp&0x8000;
			sample_composition_time_offset_value = temp&0x7FFF;
			
			GST_DEBUG_OBJECT(NULL,"sample_composition_time_offset_sign = %hd sample_composition_time_offset_value = %u \n",sample_composition_time_offset_sign,sample_composition_time_offset_value);
		}

	}
	
	GST_DEBUG_OBJECT(NULL,"\n\n");
}

/*******************************END MESSAGES ***************************************************/

int ParseSignal(unsigned char* Bitstream, GstMMTDemux *mmtdemux)
{
   int hdr_len;
	sig_length = 0;
   hdr_len = ParseSigPayldHDR(Bitstream);
   GST_DEBUG_OBJECT(NULL, ">>>>>>>>>>>. header length: %d", hdr_len);
   Bitstream += hdr_len;
	sig_length = sig_length+hdr_len;
	sig_msg.message_id = Read16(Bitstream);
	Bitstream = Bitstream +2;
	sig_msg.version = Read8(Bitstream);
	Bitstream = Bitstream +1;
	sig_length = sig_length+3;
   

	if(sig_msg.message_id == 0x0000 && (sig_msg.message_id > 0x0001 && sig_msg.message_id <= 0x0010) && sig_msg.message_id != 0x0209 && sig_msg.message_id != 0x020B && sig_msg.message_id != 0x020C )
	{
		/* PA,MPI,ADC,LS,LR messages length is 32 bits */
		sig_msg.length = Read32(Bitstream);
		Bitstream = Bitstream + 4;
		sig_length = sig_length + 4;
	}
	else if(sig_msg.message_id > 32767)
	{
		// ReservedPrivate 
		sig_msg.length = Read32(Bitstream);
		Bitstream = Bitstream + 4;
		sig_length = sig_length + 4;
	}
	else
	{ 
		sig_msg.length = Read16(Bitstream);
		Bitstream = Bitstream +2;
		sig_length = sig_length+2;
	}

	GST_DEBUG_OBJECT(NULL,"signal message id=%u,version=%hd,length=%hd\n",sig_msg.message_id,sig_msg.version,sig_msg.length);

	switch(sig_msg.message_id)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL,"PA_Mssage\n");
			PA_Message(Bitstream, mmtdemux);
			break;
		case 1 ... 16:
			GST_DEBUG_OBJECT(NULL,"MPI_Message\n");
			MPI_Message(Bitstream, mmtdemux);
			break;
		case 17 ... 32:
			GST_DEBUG_OBJECT(NULL,"MPT_Message\n");
			MPT_Message(Bitstream, mmtdemux);	
			break;
		case 33 ... 511:
			GST_DEBUG_OBJECT(NULL,"Reserved \n");
			return (sig_msg.length + sig_length);
			break;
		case 512:
			GST_DEBUG_OBJECT(NULL,"CRI_Message\n");
			CRI_Message(Bitstream);
			break;
		case 513:
			GST_DEBUG_OBJECT(NULL,"DCI_Message\n");
			DCI_Message(Bitstream);
			break;
		case 514:
			GST_DEBUG_OBJECT(NULL,"SSWR_Message\n");
			SSWR_Message(Bitstream);
			break;
		case 515:
			GST_DEBUG_OBJECT(NULL,"AL_FEC_Message\n");
			AL_FEC_Message(Bitstream);
			break;
		case 516:
			GST_DEBUG_OBJECT(NULL,"HRBM_Message \n");
			HRBM_Message(Bitstream);
			break;
		case 517:
			GST_DEBUG_OBJECT(NULL,"MC_Message\n");
			MC_Message(Bitstream);
			break;
		case 518:
			GST_DEBUG_OBJECT(NULL,"AC_Message\n");
			AC_Message(Bitstream);
			break;
		case 519:
			GST_DEBUG_OBJECT(NULL,"AF_Message\n");
			AF_Message(Bitstream);
			break;
		case 520:
			GST_DEBUG_OBJECT(NULL,"RQF_Message\n");
			RQF_Message(Bitstream);
			break;
		case 521:
			GST_DEBUG_OBJECT(NULL,"ADC_Message\n");
			ADC_Message(Bitstream);
			break;
		case 522:
			GST_DEBUG_OBJECT(NULL,"HRBM_Removal_Message\n");
			HRBM_Removal_Message(Bitstream);
			break;
		case 523:
			GST_DEBUG_OBJECT(NULL,"LS_Message\n");
			LS_Message(Bitstream);
			break;
		case 524:
			GST_DEBUG_OBJECT(NULL,"LR_Message\n");
			LR_Message(Bitstream);
			break;
		case 525:
			GST_DEBUG_OBJECT(NULL,"NAMF_Message\n");
			NAMF_Message(Bitstream);
			break;
		case 526:
			GST_DEBUG_OBJECT(NULL,"LDC_Message\n");
			LDC_Message(Bitstream);
			break;
		case 527 ... 28671:
			GST_DEBUG_OBJECT(NULL," ReservedForISO_16\n");
			return (sig_msg.length + sig_length);
			break;
		case 28672 ... 32767:
			GST_DEBUG_OBJECT(NULL,"ReservedForISO_32\n");
			return (sig_msg.length + sig_length);
			break;
		default:
			GST_DEBUG_OBJECT(NULL,"ReservedPrivate\n");
			return (sig_msg.length + sig_length);
			break;

	}
	return sig_length;
}

int ParseSigPayldHDR(unsigned char* Bitstream)
{
	unsigned char FirstByte/*,SecByte*/ ; 
	unsigned int PacketCounter,PackCountFlag=0,HeaderExtFlag=0;
	//unsigned short int Header_extension=0;
	unsigned short int PacketID=0/*, r_TB_DS_TP_flow_label,Flow_label*/;
	unsigned int timestamp,Seq_Number;

	int SigMsgLen=0,fragment_counter=0;
	unsigned int Aggregation_flag ,H_flag,res;
	int f_i;

	MmtpHDRCount=0;

	FirstByte = Read8(Bitstream);
	Bitstream=Bitstream+1;
	MmtpHDRCount++;

	if(FirstByte & 0x20)
	{
		GST_DEBUG_OBJECT(NULL,"packet counter bit is enabled \n");
		PackCountFlag=1;
	}

	if(FirstByte & 0x04)
	{
		GST_DEBUG_OBJECT(NULL,"Header is extension bit is enabled.\n");
		HeaderExtFlag=1;
	}


	//SecByte=Read8(Bitstream);
	Bitstream=Bitstream+1;
	MmtpHDRCount++;

	PacketID=Read16(Bitstream);
	Bitstream=Bitstream+2;
	MmtpHDRCount=MmtpHDRCount+2;

	timestamp=Read32(Bitstream);
	Bitstream=Bitstream+4;
	MmtpHDRCount=MmtpHDRCount+4;

	Seq_Number=Read32(Bitstream);
	Bitstream=Bitstream+4;
	MmtpHDRCount=MmtpHDRCount+4;

	GST_DEBUG_OBJECT(NULL,"MMTP Signal PacketID =%u timestamp=%u Sequence Number=%u \n",PacketID,timestamp,Seq_Number);

	if(PackCountFlag)
	{
		Bitstream=Bitstream+4;
		MmtpHDRCount=MmtpHDRCount+4;
		PacketCounter=Read32(Bitstream);
		GST_DEBUG_OBJECT(NULL,"PacketCounter = %u\n",PacketCounter);	
	}

	//r_TB_DS_TP_flow_label = Read16(Bitstream);
	Bitstream=Bitstream+2;
	MmtpHDRCount=MmtpHDRCount+2;
	//Flow_label = r_TB_DS_TP_flow_label & 0x007F;

	if(HeaderExtFlag)
	{
		Bitstream=Bitstream+2;
		MmtpHDRCount=MmtpHDRCount+2;
		//Header_extension=Read16(Bitstream);
	}

	/*********** Parsing Signalling Message Header **************************/

	FirstByte = Read8(Bitstream);
	Bitstream = Bitstream +1;
	MmtpHDRCount = MmtpHDRCount+1;

	Aggregation_flag = FirstByte&0x01 ;
	H_flag  = (FirstByte&0x02)>>1 ;
	res = (FirstByte&0x3c) >> 2 ;
	f_i = (FirstByte&0xc0)>>6;

	GST_DEBUG_OBJECT(NULL,"Aggregation_flag = %u & res=%u\n",Aggregation_flag, res);
	switch(f_i)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL,"Payload contains one or more complete signalling messages.\n");
			break;
		case 1:
			GST_DEBUG_OBJECT(NULL,"Payload contains the last fragment of a signalling message.\n");
			break;
		case 2:
			GST_DEBUG_OBJECT(NULL,"Payload contains the first fragment of a signalling message.\n");
			break;
		case 3:
			GST_DEBUG_OBJECT(NULL,"Payload contains a fragment of a signalling message that is neither the first nor the last fragment.\n");
			break;
		default:
			GST_DEBUG_OBJECT(NULL,"None of the above\n");
	}

	fragment_counter = Read8(Bitstream); // fragment_counter is 0 if aggregation_flag is set 1
	Bitstream = Bitstream +1;
	MmtpHDRCount = MmtpHDRCount+1;
	 GST_DEBUG_OBJECT(NULL,"fragment_counter: %d\n", fragment_counter);

	/* NOTE: when Aggregation_flag is set to "0" the length field will not present */
	if( Aggregation_flag )
	{
		if(H_flag){
			SigMsgLen = Read32(Bitstream);
			Bitstream = Bitstream +4;
			MmtpHDRCount = MmtpHDRCount+4;
		}else{
			SigMsgLen  = Read16(Bitstream);
			Bitstream = Bitstream +2;
			MmtpHDRCount = MmtpHDRCount+2;
		}
		  GST_DEBUG_OBJECT(NULL,"SigMsgLen: %d\n", SigMsgLen);

	}

	return MmtpHDRCount;
}

