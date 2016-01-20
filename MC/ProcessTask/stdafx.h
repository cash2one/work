// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "stdstring.h"
#include "IServerSocket.h"
#include "IGlobalDataBlock.h"
#include "IConfigInfo.h"
#include "Trace.h"
#include "structdef.h"



extern CLogTrace g_log;
extern IGlobalDataBlock* g_GlobalDataBlock;
extern IConfigInfo *g_Config;
extern IServerSocket *g_ServerSocket;


// LPSTR WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD *dwSize);
// LPWSTR MByteToWChar( LPSTR lpcszStr, LPWSTR lpwszStr, DWORD *dwSize);


//add 20150601
struct CommonParam
{
	DWORD dwLoopPdtCnt;			//��Ʒ����
	DWORD dwCurPdtCnt;			//��ǰ��Ʒ����
	DWORD dwTaskType;			//��������

	DWORD dwEmbedID;			//�������ͣ�126����sina);
	
	int	  iFunCount;		//�ܲ�����
	CStdString strPreCompName;  //��һ����˾����
	CStdString strSendDataBak;  //���ݷ��͸��ں˵����ݣ����������ȡ��Ʒ�滻
	std::vector<CStdString> vPdtLists;  //һ�������ȡ�Ĳ�Ʒ���⣻

	BOOL bIsObject;  //�Ƿ�Object ��վ
	CommonParam()
	{
		dwLoopPdtCnt = 1;
		dwCurPdtCnt = 0;
		dwTaskType = 0;
		iFunCount = 0;
		dwEmbedID = 0;
		strPreCompName = _T("");
		strSendDataBak = _T("");
		vPdtLists.clear();
		bIsObject = FALSE;
	};
};
