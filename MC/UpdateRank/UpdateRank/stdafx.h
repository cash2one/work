// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
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
*  一些数据定义
**************************************/

#define BADIDU_INDEX     0
#define S360_INDEX       1
#define SOGOU_INDEX      2
#define BING_INDEX       3
#define YOUDAO_INDEX     4
#define PHONEBAIDU_INDEX 5
#define PHONESOGOU_INDEX 6

// 搜索引擎数量
#define SEARCHFLAGCOUNT  7

// 搜索标记
enum
{
	SEARCH_BAIDU      = 0x01,               // 百度        
	SEARCH_360        = 0x02,               // 360搜索
	SEARCH_SOGOU      = 0x04,               // 搜狗
	SEARCH_BING       = 0x08,               // 必应
	SEARCH_YOUDAO     = 0x10,               // 有道
	SEARCH_PHONEBAIDU = 0x20,               // 手机百度
	SEARCH_PHONESOGOU = 0x40                // 手机搜狗
};


#define OPEN_PAGE_NORMAL           -2
#define OPEN_PAGE_FAILED           -1
#define NOT_FOUND_COMPANY          0
#define FOUND_COMPANY              1
#define PAGE_NOT_FOUND             2
#define PAIMING_EXIST              3
#define NO_PAIMING                 4
#define ENGINE_APPEAR_VERIFYCODE   5


// 关键词数据信息
typedef struct _Tag_KeyWordDataInfo
{
	int                   iFlag;            // 搜索标志
	int                   iCurPage;         // 当前第几页
	CString               strKey;           // 关键词Key
	CString               strKeyWordName;   // 搜索关键词名
	CString               strKeyHex;        // 搜索关键词16进制码
	CString               strUrl;           // 目标网址
	CStringA              strWebList;       // 舟大师发布的网站列表
	CString               strWebFlag;       // 网站标记0：表示忽略网站列表，只抓取舟大师收录的其它网站
	                                        //         1：表示抓取网站列表及所有舟大师收录的网站
	                                        //         2：表示仅抓取网站列表中指定的网站
	CString               strComany;        // 公司名
	std::vector<CString>  vCompanys;        // 公司名(包含发布标记)
	std::vector<CStdString>  vAllCompanys;		//包括公司简称和全称；

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


// 返回主控数据
typedef struct _Tag_BackDataInfo
{
	int         iRank;                      // 关键词排名(1~30)
	int         iFlag;                      // 搜索引擎标志
	CString     strKeyWordName;             // 关键词名称
	CString     strKey;                     // 关键词Key
	CString     strPagePath;                // 快照路径
	CString     strCompanyName;            // 公司名

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

// 搜索引擎名到iFlag的映射
extern std::map<CString, int> g_mapSearch;

// 存储所有关键词
extern std::vector<pKeyWordDataInfo> g_vAllKeyWords;

// 保存不同用户的公司名
extern std::vector<CString> g_vDiffCompany;

// 搜索开关(是否搜索)
extern volatile BOOL bSearchFlag[];

// 主控传过来的socket接口用来发送数据到主控
extern IServerSocket *g_Socket;

// 抓取快照日志
extern CLogTrace g_pageLog;

// 出现验证码或网页异常日志
extern CLogTrace g_pLog;

// 临界区相关
extern CRITICAL_SECTION critSection;
extern CRITICAL_SECTION critSendMsg;
extern CRITICAL_SECTION critSearchFlag;
extern CRITICAL_SECTION critCount;

// 记录抓取关键词个数
extern int g_iBaidu;
extern int g_iSogou;
extern int g_iBing;
extern int g_i360;
extern int g_iYouDao;

// 抓取网站列表
extern CStringA  g_strWeb;

// 关键词之间的延时
extern int g_iDelayForKeyWord;

// 贸网站之间的延时
extern int g_iDalayForSite;

extern int g_iThreadCount;

//调试要抓取的URL
extern CString g_strDebugUrl;
extern BOOL g_bIsDebug;

const int HtmlCharNum = 18;
const int CHAR_LEN = 2;
extern TCHAR tSpecialChar[HtmlCharNum][CHAR_LEN];

/**************************************
* 数据定义结束
**************************************/


char* CStringToMutilChar(CString& str, int& chLength, WORD wPage);

// 初始化map
void InitMap();


/*
@brief 解析上层传过来的数据，并分解任务
@param strWord要解析的字符串
*/
void ParseAllKeyWordData(const CStdString &strWord);

/*
@brief 在keyword目录下创建公司名目录保存快照
@param strDirName 目录名称
*/
void CreateCompanyDir(CString strDirName);

/*
@brief 把不同用户的公司名装进vector中
@param strComName 公司名称
*/
void SaveCompanyName(CString strComName);

/*
@brief 解析每条关键词数据
@param strKeyWord要解析的关键词数据字符串
*/
void ParseKeyWordData(const CStdString &strKeyWord);

/*
@brief  取任务
@return 任务信息指针
*/
pKeyWordDataInfo GetSearKeyWordInfo();

/*
@brief  加载SiteList.txt文件内容到strWeb
@return
*/
void GetDefaultWeb();

/*
@brief  宽字符转换成多字节
@param  pChar  宽字符
@return 返回多字节
*/
char* WideToChar(const wchar_t *pWide, DWORD dwCode = CP_ACP);

//清理百度、搜搜、360等搜索引擎缓存
void DeleteSearchCache();

//URLEncode---UTF-8编码
BYTE toHex(const BYTE &x);
CString UrlEncode(CString sIn);

//URLDecode---UTF-8解码
CString Utf8ToStringT(LPSTR str);
CString UrlDecode(LPCTSTR url);

// 获取CPU核心数
int GetCPUCoreNums();

//替换网页特殊字符
void ReplaceHtmlChar(CString &strData);