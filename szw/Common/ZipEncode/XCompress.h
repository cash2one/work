#pragma once

class XCompress
{
public:
	XCompress(void);
	~XCompress(void);

	int Compress1(unsigned char *unCompress ,const int lngUnCompressLength,unsigned char *Compress,int *lngCompressLength);
	int unCompress1(unsigned char *unCompress ,int *lngUnCompressLength,unsigned char *Compress,const int lngCompressLength);
};
