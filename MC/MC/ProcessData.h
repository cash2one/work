// ProcessData.h: interface for the CProcessData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSDATA_H__2311A88F_9C29_4EE7_BFEB_7C8A656E99C4__INCLUDED_)
#define AFX_PROCESSDATA_H__2311A88F_9C29_4EE7_BFEB_7C8A656E99C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ServerSocket.h"

class CProcessData  
{

	friend class CServerSocket;
public:  
	CProcessData();
	virtual ~CProcessData();
public:
	DWORD CovertBufData(char *Orgbuf,DWORD dwTotalLen);
	
	DWORD SetPtrOfServer(CServerSocket *pServerSocket);
private:

	enum { 

		PREFIX_TASKDATA_LENGTH = 12,  //infopost(;0)
		SPLIT_CHARS_LENGTH = 4,   //�ָ����ⳤ��
		
	}; // ö�ٳ���


	DWORD CovertBufDataWeb(char *Orgbuf,DWORD dwTotalLen);

	DWORD DoFromWebTaskByMainTask(const CStdString &strData);        //�����������������ݽ���
	DWORD DoFromWebTaskByRegistry(const CStdString &strData);       //ע���������ݽ���
	DWORD DoFromWebTaskByRefreshAndDelete(DWORD dwType,const CStdString &strData);       //ˢ����ɾ�����ݽ���
	DWORD DoFromWebTaskByLogin(const CStdString &strData);         //�Զ���¼

	DWORD DoFromWebTaskByDomainPost(const CStdString &strData);         //�����ƹ�

	DWORD DoFromWebTaskByWeiboPost(const CStdString &strData);         //΢������

	DWORD DoFromWebTaskByUserInfoMigrate(const CStdString &strData);        //һ�����

	DWORD DoFromWebTaskByInfoModify(const CStdString &strData);			  //�޸�����

	DWORD DoFromWebTaskByRefreshShopTraffic(const CStdString &strData);			  //ˢ����������

	void CreateResultPage(const CStdString &strData);			//��������Ŀ¼

	void GetSearchPhoto(const CStdString &strData);         //ץȡ�������������

	void SearchKeyData(const CStdString &strData);			//���ҹؼ�������

	DWORD WebServerMintustoMcontrol(const DWORD dwServerMintus); //����������ʱ�䴫�뵽���ص�ȫ��������
	/*
	@brief   ȡ�ñ���ͼƬ·��
	@param  [out] strPdtPic  ��ƷͼƬ·�� 
	@param  [out] strTradePic Ӫҵִ��·��
	@param  [out] strCompPic  ��˾��Ƭ·��
	@return  
	*/
    DWORD GetImagePath(CStdString strPicName, CStdString& strLocalPic);
	/*
	@brief ͼƬת��
	@param strImagePath; Ҫת����ͼƬ��ַ��
	@param dwType;    ת������ (1.jpg,2.png,3.bmp,4.gif)
	*/
	BOOL ImageConvert(CStdString &strSrcPath, DWORD dwType);

public:
	CServerSocket *m_pServerSocket;
	BOOL bAlreadySend;  //�Ѿ����͹����ݣ������������ݾ��Ǵ���
};

#endif // !defined(AFX_PROCESSDATA_H__2311A88F_9C29_4EE7_BFEB_7C8A656E99C4__INCLUDED_)
