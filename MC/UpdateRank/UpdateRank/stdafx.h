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
#include <afxdisp.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vector>
#include "stringutils.h"
#include "IServerSocket.h"
#include "trace.h"
#include "FileReadAndSave.h"
#include "StdString.h"
#include "FtpFile.h"
#include "InternetHttp.h"

#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) 


/**************************************
*  һЩ���ݶ���
**************************************/

#define BADIDU_INDEX     0
#define S360_INDEX       1
#define SOGOU_INDEX      2
#define BING_INDEX       3
#define YOUDAO_INDEX     4
#define PHONEBAIDU_INDEX 5
#define PHONESOGOU_INDEX 6

// ������������
#define SEARCHFLAGCOUNT  7

// �������
enum
{
	SEARCH_BAIDU      = 0x01,               // �ٶ�        
	SEARCH_360        = 0x02,               // 360����
	SEARCH_SOGOU      = 0x04,               // �ѹ�
	SEARCH_BING       = 0x08,               // ��Ӧ
	SEARCH_YOUDAO     = 0x10,               // �е�
	SEARCH_PHONEBAIDU = 0x20,               // �ֻ��ٶ�
	SEARCH_PHONESOGOU = 0x40                // �ֻ��ѹ�
};


#define OPEN_PAGE_NORMAL           -2
#define OPEN_PAGE_FAILED           -1
#define NOT_FOUND_COMPANY          0
#define FOUND_COMPANY              1
#define PAGE_NOT_FOUND             2
#define PAIMING_EXIST              3
#define NO_PAIMING                 4
#define ENGINE_APPEAR_VERIFYCODE   5


// �ؼ���������Ϣ
typedef struct _Tag_KeyWordDataInfo
{
	int                   iFlag;            // ������־
	int                   iCurPage;         // ��ǰ�ڼ�ҳ
	CString               strKey;           // �ؼ���Key
	CString               strKeyWordName;   // �����ؼ�����
	CString               strKeyHex;        // �����ؼ���16������
	CString               strUrl;           // Ŀ����ַ
	CStringA              strWebList;       // �۴�ʦ��������վ�б�
	CString               strWebFlag;       // ��վ���0����ʾ������վ�б�ֻץȡ�۴�ʦ��¼��������վ
	                                        //         1����ʾץȡ��վ�б������۴�ʦ��¼����վ
	                                        //         2����ʾ��ץȡ��վ�б���ָ������վ
	CString               strComany;        // ��˾��
	std::vector<CString>  vCompanys;        // ��˾��(�����������)
	std::vector<CStdString>  vAllCompanys;		//������˾��ƺ�ȫ�ƣ�

	_Tag_KeyWordDataInfo()
	{
		iCurPage = 0;
		iFlag = 0;
		vCompanys.clear();
		vAllCompanys.clear();
	}

	~_Tag_KeyWordDataInfo()
	{
		vCompanys.clear();
		vAllCompanys.clear();
	}

}KeyWordDataInfo, *pKeyWordDataInfo;


// ������������
typedef struct _Tag_BackDataInfo
{
	int         iRank;                      // �ؼ�������(1~30)
	int         iFlag;                      // ���������־
	CString     strKeyWordName;             // �ؼ�������
	CString     strKey;                     // �ؼ���Key
	CString     strPagePath;                // ����·��
	CString     strCompanyName;            // ��˾��

	_Tag_BackDataInfo()
	{
		iRank = 0;
		iFlag = 0;
		strPagePath = _T("");
		strKey = _T("");
		strKeyWordName = _T("");
		strCompanyName = _T("");
	}

}BackDataInfo, *pBackDataInfo;

// ������������iFlag��ӳ��
extern std::map<CString, int> g_mapSearch;

// �洢���йؼ���
extern std::vector<pKeyWordDataInfo> g_vAllKeyWords;

// ���治ͬ�û��Ĺ�˾��
extern std::vector<CString> g_vDiffCompany;

// ��������(�Ƿ�����)
extern volatile BOOL bSearchFlag[];

// ���ش�������socket�ӿ������������ݵ�����
extern IServerSocket *g_Socket;

// ץȡ������־
extern CLogTrace g_pageLog;

// ������֤�����ҳ�쳣��־
extern CLogTrace g_pLog;

// �ٽ������
extern CRITICAL_SECTION critSection;
extern CRITICAL_SECTION critSendMsg;
extern CRITICAL_SECTION critSearchFlag;
extern CRITICAL_SECTION critCount;

// ��¼ץȡ�ؼ��ʸ���
extern int g_iBaidu;
extern int g_iSogou;
extern int g_iBing;
extern int g_i360;
extern int g_iYouDao;

// ץȡ��վ�б�
extern CStringA  g_strWeb;

// �ؼ���֮�����ʱ
extern int g_iDelayForKeyWord;

// ó��վ֮�����ʱ
extern int g_iDalayForSite;

extern int g_iThreadCount;

//����Ҫץȡ��URL
extern CString g_strDebugUrl;
extern BOOL g_bIsDebug;

const int HtmlCharNum = 18;
const int CHAR_LEN = 2;
extern TCHAR tSpecialChar[HtmlCharNum][CHAR_LEN];

/**************************************
* ���ݶ������
**************************************/


char* CStringToMutilChar(CString& str, int& chLength, WORD wPage);

// ��ʼ��map
void InitMap();


/*
@brief �����ϲ㴫���������ݣ����ֽ�����
@param strWordҪ�������ַ���
*/
void ParseAllKeyWordData(const CStdString &strWord);

/*
@brief ��keywordĿ¼�´�����˾��Ŀ¼�������
@param strDirName Ŀ¼����
*/
void CreateCompanyDir(CString strDirName);

/*
@brief �Ѳ�ͬ�û��Ĺ�˾��װ��vector��
@param strComName ��˾����
*/
void SaveCompanyName(CString strComName);

/*
@brief ����ÿ���ؼ�������
@param strKeyWordҪ�����Ĺؼ��������ַ���
*/
void ParseKeyWordData(const CStdString &strKeyWord);

/*
@brief  ȡ����
@return ������Ϣָ��
*/
pKeyWordDataInfo GetSearKeyWordInfo();

/*
@brief  ����SiteList.txt�ļ����ݵ�strWeb
@return
*/
void GetDefaultWeb();

/*
@brief  ���ַ�ת���ɶ��ֽ�
@param  pChar  ���ַ�
@return ���ض��ֽ�
*/
char* WideToChar(const wchar_t *pWide, DWORD dwCode = CP_ACP);

//����ٶȡ����ѡ�360���������滺��
void DeleteSearchCache();

//URLEncode---UTF-8����
BYTE toHex(const BYTE &x);
CString UrlEncode(CString sIn);

//URLDecode---UTF-8����
CString Utf8ToStringT(LPSTR str);
CString UrlDecode(LPCTSTR url);

// ��ȡCPU������
int GetCPUCoreNums();

//�滻��ҳ�����ַ�
void ReplaceHtmlChar(CString &strData);