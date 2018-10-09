
/*******************************************************************************************
 *  License: This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: mmtsignal.c
 *  Created on: 04-MAY-2018
 *  Author: Madhu Nadendla
 *******************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "mmtdemux.h"


typedef struct
{
        unsigned int message_id;
        unsigned char version;
        unsigned short int length;

}Signalling_Message;
Signalling_Message sig_msg;

typedef struct
{
        unsigned short int table_id;
        unsigned char version;
        unsigned short int length;
}Table_Info;
Table_Info table_info;


int ParseSigPayldHDR(unsigned char* Bitstream);
int ParseSignal(unsigned char* Bitstream, GstMMTDemux *);
unsigned int mime_type (unsigned char* Bitstream);
unsigned int asset_id(unsigned char* Bitstream);
unsigned int Identifier_mapping (unsigned char* Bitstream);
unsigned int MMT_general_location_info(unsigned char* Bitstream);

void PA_table(unsigned char* Bitstream);
void MPI_table(unsigned char* Bitstream);
void MP_table(unsigned char* Bitstream, GstMMTDemux *);
void DCI_table(unsigned char* Bitstream);
void CRI_table (unsigned char* Bitstream);
void SI_table(unsigned char* Bitstream);

void PA_Message(unsigned char* Bitstream, GstMMTDemux *);
void MPI_Message(unsigned char* Bitstream, GstMMTDemux *);
void MPT_Message(unsigned char* Bitstream, GstMMTDemux *);
void CRI_Message (unsigned char* Bitstream);
void DCI_Message (unsigned char* Bitstream);
void SSWR_Message (unsigned char* Bitstream);
void AL_FEC_Message(unsigned char*Bitstream);
void HRBM_Message (unsigned char *Bitstream);
void MC_Message(unsigned char* Bitstream);
void AC_Message(unsigned char* Bitstream);
void AF_Message(unsigned char* Bitstream);
void RQF_Message (unsigned char* Bitstream );
void ADC_Message (unsigned char* Bitstream);
void HRBM_Removal_Message(unsigned char* Bitstream );
void LS_Message (unsigned char* Bitstream);
void LR_Message (unsigned char* Bitstream);
void NAMF_Message(unsigned char* Bitstream);
void LDC_Message (unsigned char* Bitstream );

void CRI_descriptor(unsigned char* Bitstream);
void MPU_timestamp_descriptor (unsigned char* Bitstream);
void Dependency_descriptor(unsigned char* Bitstream);
void GFD_descriptor (unsigned char* Bitstream);
void SI_descriptor(unsigned char* Bitstream);




