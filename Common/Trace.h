///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2007, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：日志打印类，可以将日志打印到指定窗口或文件中
// 生成日期：2007-4-4
// 作者：刘兆兵
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_)
#define AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlstr.h>
#include "Lock.h"
#include "ZBase64.h"

//日志级别，有如下几种：
//
//                             在正式版和调式版中都打印  只在调试版打印
//同时向窗口（如果窗口句柄不为
//NULL）、文件和Debug窗口输出  LOGL_TOP                  LOGL_MID
//
//只向文件和Debug窗口输出      LOGL_LOW                  LOGL_LOW
//
//只从Debug窗口输出                                      LOGL_DBG
enum
{
    LOGL_TOP = 0,
    LOGL_HIG = 1,
    LOGL_MID = 2,
    LOGL_LOW = 3,
    LOGL_DBG = 4,
};

//日志类型
enum
{
    LOGT_ERROR = 0, //严重错误，程序已经或可能无法继续运行
    LOGT_WARNING = 1, //告警，一般错误，程序可继续运行
    LOGT_PROMPT = 2, //提示，用于在关键处输出一些信息
	LOGT_COMPRESS = 3,//压缩，用于日志太长无法全面打印，需压缩才能全部打印出来；
	LOGT_DEBUG = 4, //调试，用于多次答应重复的数据，但实际没多大用处
	
};

//日志颜色
#define LOGRGB(r,g,b) \
    ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
enum
{
    LOGC_ERROR = LOGRGB(180, 0, 0),
    LOGC_WARNING = LOGRGB(160, 0, 120),
    LOGC_PROMPT = LOGRGB(0, 0, 0),
	LOGC_DEBUG = LOGRGB(0,0,180),
};

//最大备份日志文件个数
#ifdef _UDPSERVER
#define MAX_LOGFILEBAK_COUNT          9
#else
#define MAX_LOGFILEBAK_COUNT          2
#endif

#define MAX_LEN_OF_LOG_LINE        1024 * 10 //一行Log的最长字符数
#define MAX_SIZE_OF_LOGFILE        (2 * 1024 * 1024) //一个Log文件最大字符数
#define MAX_LINE_OF_LOGWND         (2 * 1024) //Log窗口最大行数
#define LEN_SHORT_SOURCEFILE_NAME  25 //显示的源文件名长度
#define MAX_CHK_SIZE_PER_TIMES     50 //检查文件大小的间隔次数

#define TRACE_WND_MSG              (WM_USER + 101) //通知Log窗口打印的自定义消息
#define TRACE_PARAM_CLEAR          10 //清空Log窗口的消息参数

#define LOG_ENCRYPT_KEY "suminfo4gerlog"

#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define __TFILE__ __WFILE__
#else
#define __TFILE__ __FILE__
#endif
#ifdef _DEBUG
    //#define cjhdebug
   // #define taskdebug
#endif
//日志打印类，可以将日志打印到指定文件中
class CLogTrace
{
private:
	int m_nOutLogInfo;   //是否需要输出；
	int m_nEncryptLog ; //是否需要加密；
	int m_nOutLogLevel;  //输出日志类型等级；
	int m_nSaveAllLog;  //保存所有日志；
public:
	//下在三个函数用于创建动态日志文件
	CLogTrace(); 
	bool IsLogFileOpened();    //是否创建了日志文件
	
	void OpenLogFile(const TCHAR * pszTraceFileName, HWND hWnd = NULL);   //创建日志文件
	///////////////////////////////////

    //在构造函数中打开日志文件，并传入窗口句柄 
    CLogTrace(const TCHAR * pszTraceFileName, HWND hWnd);
    
    //在析构函数中关闭日志文件
    virtual ~CLogTrace();
    
    //标准日志打印
    void Trace(BYTE cLevel, BYTE cType, const TCHAR * pszSourceFile,
        DWORD dwLine, LPCTSTR lpszFormat, ...);
    
    //简单日志打印，不含日期、时间、类型、文件名、代码行信息
    void TraceSim(BYTE cLevel, LPCTSTR lpszFormat, ...);

    //返回日志字串，用于日志窗口打印时取用
    const TCHAR * GetLogBuf();

    //设置日志窗口句柄
    void SetLogWnd(HWND hLogWnd);

	bool IsLogFileEnable(); //是否要输出日志文件

	//有关加密日志信息 宋亚非 
	bool IsEncryptLog() ;  //是否加密输出日志
	bool SetEncryptFlag() ;  //文件首部写入加密标志
	bool GetEncryptFlag(int &nFlag) ; //从文件首部读取加密标志
	bool CreatLogFile() ;//创建log文件 如果存在则打开
private:

    //带颜色的简单日志打印
    void TraceColor(BYTE cLevel, DWORD dwColor);
	void ZipEecodeStdString( const CString &strSource, CString &strZipped);   //压缩
	void GetLogType(CString &strType, DWORD &dwColor,BYTE cType);  //获取日志类型文本及颜色；
private:

    CLock m_lock; //锁
	CZBase64 m_base64Encrypt ;
    
    TCHAR  m_szFullLogFileName[MAX_PATH]; //日志文件全名
    HANDLE m_hLogFile; //日志文件句柄
    HWND   m_hLogWnd; //日志窗口句柄
    bool   m_bFirstCreate; //是否首次创建日志文件

    TCHAR   m_szBuffer[2][MAX_LEN_OF_LOG_LINE]; //两行日志的缓存
};

#define TOPLOG(x) g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__, x)
	
#endif // !defined(AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_)
