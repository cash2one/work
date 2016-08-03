//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
// 版本：
// 文件说明：SOCKET服务端类 通过回调函数处理收到的数据
// 生成日期：
// 作者：何培田
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//

#ifndef _CSERVERSOCKET__
#define _CSERVERSOCKET__

#include "IServerSocket.h"
#include "StdString.h"
#include "Trace.h"
#include "Lock.h"

#pragma comment(lib, "wsock32.lib")
 
typedef DWORD (*pfnDataHandler)(TCHAR *Orgbuf, DWORD dwTotalLen);

class CCommServer : public IServerSocket
{
public:
	CCommServer(void);
	~CCommServer(void);

	int Init(int nPort, pfnDataHandler fn);	//初始化
	int UnInit();	//解除初始化
	int CloseClient() ;  //关闭ClientSocket
	int SendData(DWORD dwLen,DWORD Flag,char *buff);
	int SendData(TCHAR *buff, int dwLen);
	int SendData(CStdString strData);
	bool  IsClientConnected() const;

	friend DWORD WINAPI ThreadRecv(PVOID param);

private:
	HANDLE m_hThread;
	BOOL m_bRun;
	SOCKET m_socket;
	SOCKET m_ClientSocket;
	pfnDataHandler m_pfnDataHandler;        //收到数据包之后的回调函数
	CLock m_lockSend;                     //发送数据的互斥锁

	int WaitClient(); //等待客户端连接
	int WaitRecData(BOOL IsThread);	//接收数据
};
#endif