#pragma once

class XZIPCrypt
{
private:
	char Passwordchar[32];
	//char PasswordByteFinal[32];
	//char SecurityWord[100];//= "suminfobizexpress2008date20070326telephone83631066other83627788";
	int InitPasswordArray();       //��ʼ�����ܽ����ֽ�����

public:
	XZIPCrypt(void);
	~XZIPCrypt(void);

	//Private Const SecurityWord = "suminfobizexpress2008date20070326telephone83631066other83627788"
	int GetCompress (unsigned char*unCompress ,const int lngUnCompressLength,unsigned char*Compress,int *lngCompressLength );  //��ȡ����ѹ������ֽ�����
	int GetUnCompress (unsigned char*unCompress , int *lngUnCompressLength,unsigned char*Compress,const int lngCompressLength );  //��ȡ������ѹ������ַ���
	
	int GetPassword(unsigned char*Password);  //���ܵ������ַ���,����MD5����

	int EnCrypt(unsigned char*PlainData,int lngLength);  //���ֽ�������м���
	int DeCrypt(unsigned char*CryptByte,int lngLength);	//���ֽ�������н���


	int ZipByteArray  (unsigned char*Source ,const int lngSourceLength,unsigned char*Dest,int *lngDestLength );  //��ָ���ֽ��������ѹ��������,
	int UnZipByteArray  (unsigned char*Source ,const int lngSourceLength,unsigned char*Dest,int *lngDestLength );  //��ָ���ֽ�������н��ܲ���ѹ,

};
