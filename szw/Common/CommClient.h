//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
// 版本：
// 文件说明：SOCKET客户端类 通过回调函数处理收到的数据，用于向主控发送ANSI字符数据
// 生成日期：
// 作者：何培田
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//

#pragma once
#include "Lock.h"

typedef DWORD (*pfnDataHandler)(char *Orgbuf, DWORD dwTotalLen);

class CCommClient
{
public:
	CCommClient(void);
	~CCommClient(void);

	int Init(int iPort, pfnDataHandler fn);
	int SendData(TCHAR *pData,int nLen); //发送数据到服务进程
	int SendData(CStdString strData);

	void Stop(void);   //停止数据接收线程
	bool StopIsSet(void);  //检测是否设置了停止标记

private:
	SOCKET m_socket;
	HANDLE m_hThread;
	bool m_bStop;

	pfnDataHandler m_pfnDataHandler;        //收到数据包之后的回调函数
// 	pfnDataHandler *m_pfnDataHandler;        //收到数据包之后的回调函数
	CLock m_lockSend;                     //发送数据的互斥锁

private:
	int Connect(int iPort);
	DWORD DisConnect(void);
	DWORD CreateRecvThread(void);
	int SetDataHandler(pfnDataHandler fn);

	static DWORD WINAPI StaticThreadFunc(LPVOID lpParam);
};
