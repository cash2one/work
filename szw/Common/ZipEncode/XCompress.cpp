
#include <stdio.h>
// TODO: reference additional headers your program requires here
#include <malloc.h>

#include "XCompress.h"
#include "deflate.h"
#include "zlib.h"

XCompress::XCompress(void)
{
}

XCompress::~XCompress(void)
{
}
int XCompress::Compress1(unsigned char *unCompress ,const int lngUnCompressLength,unsigned char *Compress,int *lngCompressLength)
{
	//
	uLongf			outlen = 0;
	int lngResult =0;
	outlen = lngUnCompressLength;
	lngResult=compress( Compress,&outlen,(Byte *) unCompress,lngUnCompressLength);

	*lngCompressLength = outlen;

	return lngResult;

}

int XCompress::unCompress1(unsigned char *unCompress ,int *lngUnCompressLength,unsigned char *Compress,const int lngCompressLength)
{
	//
	uLongf			outlen = 0;

	int lngResult =0;
	outlen = *lngUnCompressLength;
	lngResult = uncompress((Byte *) unCompress,&outlen, Compress,lngCompressLength);
	
	*lngUnCompressLength = outlen;

	return lngResult;

}