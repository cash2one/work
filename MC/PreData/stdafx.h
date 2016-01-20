// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once
#pragma  comment(lib,"ZipEnCode.lib")

#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
//#define _AFXDLL
// Windows ͷ�ļ�:
#include <afxwin.h>         // MFC ��������ͱ�׼���
//#include <windows.h>
//MFC֧��
//#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxext.h>         // MFC ��չ
#include <afxdisp.h>        // MFC �Զ�����

#include <winbase.h>


#include <atlbase.h>
#include <tchar.h>
#include <list>
#include <deque>
#include <map>
#include <vector>

#include "StdString.h"
#include "stringutils.h"
#include "trace.h"
#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "SiteInfo.h"
#include "IGlobalDataBlock.h"
#include "IServerSocket.h"
#include  "IConfigInfo.h"


extern IGlobalDataBlock* g_GlobalDataBlock;
extern IConfigInfo *g_Config;
extern CLogTrace g_sysLog;

extern CStdString strCity ;		//���б���
extern CStdString strProv ;     //ʡ�ݱ���



enum E_PPD_ERR_TYPE
{
	PPD_SUCCESS=0,
	PPD_NETCODELESSZERO_ERR,
	PPD_NOWEBDATA,
};

struct TParamItem
{
	CStdString Key;
	CStdString Value;
	TParamItem(CStdString& k, CStdString& v):Key(k), Value(v)
	{};
	TParamItem(TCHAR* k, TCHAR* v):Key(k), Value(v)
	{};

	TParamItem()
	{
	};

	
};
//end add 20080922


//add 20080923
struct TRapalceDataMsg
{
	std::vector<TParamItem> tParamItemTable;
	//δ�滻ͨ����Ĳ���
	std::vector<CStdString> vNoRepleaceParam;

	DWORD dNameSize;
	DWORD dPsdSize;
	UINT64 nNetItemCode;
	CStdString strInfoID;
	CStdString strNetCode;
	E_Account_Style eUserType;
	E_Password_Style ePsdType;
	E_PPD_ERR_TYPE eErrType;//���󷵻������ж�����
	E_TASK_TYPE eTaskType;

	DWORD dwNetLanType;
	CStdString strWebName;
	CStdString strWebPass;
	CStdString strDefaultName;
	CStdString strDefaultPass;


	TRapalceDataMsg()
	{
		dNameSize = 8;  //Ĭ���ʺŴ�С
		dPsdSize = 8;  //Ĭ�������С
		nNetItemCode = 0;
		strInfoID = _T("");

		eUserType = asNOT_INCLUDE_CHINESE;
		ePsdType = psNO_INCLUDE_CHINESE;
		eErrType = PPD_SUCCESS;//���󷵻������ж�����

		dwNetLanType = 0;
		eTaskType = ETTypeNone;
	};
};

enum E_PPD_TRIM_TYPE
{
	PPD_BBodyC_TYPE=0,
	PPD_BBodyE_TYPE,
	PPD_BBodyLen_TYPE,
	PPD_WWebsH_TYPE,
	PPD_BBodyCI_TYPE,
	PPD_BBodyP_TYPE,
};

/* ����:
strInStr: ������ַ���
strOutStr: ������ַ���
eType: ���Ƶ�����,Ŀǰֻ������(E_LimtedLen(���Ƴ���), E_CutAimStr(ȥ��Ŀ���ִ�), E_BothType(ǰ����ͬʱ��Ҫ����))
strCutStr: ��Ҫȥ�����ִ�
nLimtLen: ���Ƶĳ���

����ֵ: 0�ɹ�,����ʧ��
*/
DWORD LimitedStr(const CStdString &strInStr, CStdString &strOutStr, 
                 E_PPD_TRIM_TYPE eType, CStdString strCutStr, int nLimtLen = 0);



