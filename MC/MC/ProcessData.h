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
		SPLIT_CHARS_LENGTH = 4,   //分隔符测长度
		
	}; // 枚举常量


	DWORD CovertBufDataWeb(char *Orgbuf,DWORD dwTotalLen);

	DWORD DoFromWebTaskByMainTask(const CStdString &strData);        //发布主任务任务数据解析
	DWORD DoFromWebTaskByRegistry(const CStdString &strData);       //注册任务数据解析
	DWORD DoFromWebTaskByRefreshAndDelete(DWORD dwType,const CStdString &strData);       //刷新与删除数据解析
	DWORD DoFromWebTaskByLogin(const CStdString &strData);         //自动登录

	DWORD DoFromWebTaskByDomainPost(const CStdString &strData);         //域名推广

	DWORD DoFromWebTaskByWeiboPost(const CStdString &strData);         //微博发布

	DWORD DoFromWebTaskByUserInfoMigrate(const CStdString &strData);        //一键搬家

	DWORD DoFromWebTaskByInfoModify(const CStdString &strData);			  //修改资料

	DWORD DoFromWebTaskByRefreshShopTraffic(const CStdString &strData);			  //刷新商铺流量

	void CreateResultPage(const CStdString &strData);			//创建快照目录

	void GetSearchPhoto(const CStdString &strData);         //抓取各搜索引擎快照

	void SearchKeyData(const CStdString &strData);			//查找关键词数据

	DWORD WebServerMintustoMcontrol(const DWORD dwServerMintus); //将服务器的时间传入到主控的全局数据区
	/*
	@brief   取得本地图片路径
	@param  [out] strPdtPic  产品图片路径 
	@param  [out] strTradePic 营业执照路径
	@param  [out] strCompPic  公司照片路径
	@return  
	*/
    DWORD GetImagePath(CStdString strPicName, CStdString& strLocalPic);
	/*
	@brief 图片转换
	@param strImagePath; 要转换的图片地址；
	@param dwType;    转换类型 (1.jpg,2.png,3.bmp,4.gif)
	*/
	BOOL ImageConvert(CStdString &strSrcPath, DWORD dwType);

public:
	CServerSocket *m_pServerSocket;
	BOOL bAlreadySend;  //已经发送过数据，后期再有数据就是错误
};

#endif // !defined(AFX_PROCESSDATA_H__2311A88F_9C29_4EE7_BFEB_7C8A656E99C4__INCLUDED_)
