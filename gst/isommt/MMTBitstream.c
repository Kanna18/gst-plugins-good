/*******************************************************************************************
 *  License:This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: MMTBitstream.c
 *******************************************************************************************/
#include "MMTBitstream.h"

unsigned char Read8(unsigned char* Bitstream )
{
	unsigned char val = *(unsigned char*)Bitstream;
	return val;

}

unsigned short int Read16(unsigned char* Bitstream)
{

	unsigned short int val = Read8(Bitstream) << 8;
	val |= Read8(Bitstream + 1);
	return val;

}
unsigned int Read32(unsigned char* Bitstream)
{
	unsigned int val = Read16(Bitstream) << 16;
	val |= Read16(Bitstream + 2);
	return val;
}

unsigned  int Read24(unsigned char* Bitstream)
{
	 unsigned int val = Read16(Bitstream) << 8;
	 val |= Read8(Bitstream + 2);
	 return val;
}

unsigned long  Read64(unsigned char *Bitstream)
{
	unsigned long long  val;
	val = Read32(Bitstream); 
	val <<= 32; 
	val |= Read32(Bitstream + 4);
	return val;
}
unsigned  long Read48(unsigned char* Bitstream)
{
	 unsigned long long val = Read32(Bitstream) << 16;
	 val |= Read16(Bitstream + 4);
	 return val;
}



