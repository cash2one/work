#pragma once

class XZIPCrypt
{
private:
	char Passwordchar[32];
	//char PasswordByteFinal[32];
	//char SecurityWord[100];//= "suminfobizexpress2008date20070326telephone83631066other83627788";
	int InitPasswordArray();       //初始化加密解密字节数组

public:
	XZIPCrypt(void);
	~XZIPCrypt(void);

	//Private Const SecurityWord = "suminfobizexpress2008date20070326telephone83631066other83627788"
	int GetCompress (unsigned char*unCompress ,const int lngUnCompressLength,unsigned char*Compress,int *lngCompressLength );  //获取经过压缩后的字节数组
	int GetUnCompress (unsigned char*unCompress , int *lngUnCompressLength,unsigned char*Compress,const int lngCompressLength );  //获取经过解压缩后的字符串
	
	int GetPassword(unsigned char*Password);  //加密的密码字符串,用来MD5加密

	int EnCrypt(unsigned char*PlainData,int lngLength);  //对字节数组进行加密
	int DeCrypt(unsigned char*CryptByte,int lngLength);	//对字节数组进行解密


	int ZipByteArray  (unsigned char*Source ,const int lngSourceLength,unsigned char*Dest,int *lngDestLength );  //对指定字节数组进行压缩并加密,
	int UnZipByteArray  (unsigned char*Source ,const int lngSourceLength,unsigned char*Dest,int *lngDestLength );  //对指定字节数组进行解密并解压,

};
