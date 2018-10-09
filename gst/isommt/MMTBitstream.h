/*******************************************************************************************
 *  License:This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: MMTBitstream.h
 *  Created on: 12-Feb-2018
 *  Author: Trimurthulu Amaradhi
 *******************************************************************************************/


#ifndef MMTBITSTREAM_H_
#define MMTBITSTREAM_H_

struct ByteReader
{
	unsigned char *data; //Pointer to data
	unsigned int DataSize; //Size of bytes to read and process.
	unsigned char *BytePosition; // Latest position of bit-stream pointer.
};

unsigned char Read8(unsigned char*);
unsigned short int Read16(unsigned char* Bitstream);
unsigned int  Read32(unsigned char* Bitstream);
unsigned char SkippBytest(unsigned char*);
unsigned char RdBigEndianValue(unsigned char*);
unsigned  int Read24(unsigned char* Bitstream);
unsigned long Read64(unsigned char *Bitstream);
unsigned  long Read48(unsigned char* Bitstream);

#endif /* MMTBITSTREAM_H_ */
