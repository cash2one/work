
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN           
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS     

#define _AFX_ALL_WARNINGS

#include <afxwin.h>        
#include <afxext.h>        


#include <afxdisp.h>       



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>    

#include <afxhtml.h>                


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#ifndef _MY_DEFINES
#define _MY_DEFINES

#include "CommFunc.h"
#include "Trace.h"
#include "enumdef.h"
#include "structdef.h"
#include "StdString.h"
#include "StdStrUtils.h"
#include "MyMessageBox.h"
#include "inifile.h"
#include "CommonDef.h"
#include "OcrInput.h"
#include "ManmadeCode.h"
#include "PublicFunc.h"
#include "GetCpuUseage.h"
#include "IXMLRW.h"
#include "InternetHttp.h"

#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include<stdlib.h>
//#include<crtdbg.h>
//#include <vld.h> 
#endif 

extern CLogTrace g_runlog;     
extern CLogTrace g_CheckInputLog;
extern CLogTrace g_CheckCpuLog;
extern CMessageShow g_msgbox;
extern IXMLRW g_configInfo;
extern COcrInput g_ocrInput;
extern CManmadeCode g_orcManmade;//人工打码
extern HANDLE g_hStepCtrlEvent;


#define WM_MAINWND_UPDATE_ADDRESS_URL   WM_USER + 1
#define WM_MAINWND_SPLITE_WINDOW        WM_USER + 2
#define WM_VIEW_USER_CMD_ENTER          WM_USER + 3
#define WM_VIEW_STOP_TASK               WM_USER + 4
#define WM_VIEW_SET_JUMP_URL            WM_USER + 5
#define WM_VIEW_EVENT_ACTIVE_ELEMENT    WM_USER + 6
#define WM_VIEW_TASK_DATA               WM_USER + 7
#define WM_FREE_VIEW_RESOURCE           WM_USER + 8
#define WM_VIEW_ACTIVE_VIEW             WM_USER + 9

#define WM_VIEW_GO_BACK                 WM_USER + 10
#define WM_VIEW_GO_FOWARD               WM_USER + 11
#define WM_VIEW_REFRESH                 WM_USER + 12
#define WM_VIEW_STOP                    WM_USER + 13
#define WM_VIEW_INIT_SETTING            WM_USER + 14
#define WM_VIEW_SUB_VIEW_FINISHED       WM_USER + 15  
#define WM_VIEW_GEN_QUICKLOOK_PIC       WM_USER + 16  
#define WM_VIEW_GET_QUICKLOOK_TEXT      WM_USER + 17  
#define WM_VIEW_NAVIGATE_URL            WM_USER + 18 


#define WM_MAINWND_NEW_FILE             WM_USER + 50
#define WM_MAINWND_CANCEL_VAGUE_LOC     WM_USER + 51
#define WM_MAINWND_FAILED_PAUSE         WM_USER + 52
#define WM_MAINWND_NAVIGATE_URL			WM_USER + 53

//#define WM_CHILDWND_DELETE				WM_USER + 54

#define TIMER_STEP_EXEC                  8801
#define TIMER_STEP_FINISH              8802
#define TIMER_PAGE_STATE_TIMEOUT_CHK          8803      
//#define TIMER_SHOW_IMG_CODE             8804
#define TIMER_PAGE_STATE_CLICK_CHK            8805       
#define TIMER_PAGE_STATE_TIMEOUT_CHK2         8806     
#define  TIMER_GET_CODE_RESULT 8807  
#define TIMER_FRAMEWIN 8808				 //定义主框架的timer
#define TIMER_CHECK_POP 8809
#define TIMER_CHECK_DRAGRESULT 8810      //获取拖动/点击型验证码结果
#define TIMER_CHECK_SHOW_TIMEOUT 8811    //内核显示拖动/点击验证码超时
#define TIMER_CHECK_ENGINE_ACTIVE 8812   //检测内核是否卡死

#define STEP_DELAY_TIME                 _T("100")      

#define WM_POPUP_MENU_BASE              40000
#define DELIMITER_DATA                  _T(",")
#define DELIMITER_STEP_DESCRIPTION      _T("--")
#define DELIMITER_STEP_ERR_AND_UIMSG    _T("<@@@>")
#define DELIMITER_POPUPINFO_FOR_ONE_STEP             _T("<@@>")
#define DELIMITER_POPUPINFO_BETWEEN_STEPS    _T("<###>")
#define DELIMITER_STEP_DESC_AND_STEP_ERRINFO _T("<-->")

#define CUR_STEP_RETRY_TIME_LIMITE 1

//???????
#define INVALIDATE_FLAG  0  
#define BMP_FLAG         1   
#define JPG_FLAG         2  
#define GIF_FLAG         3
#define PNG_FLAG         4 
#define XBM_FLAG         5

#define _NEW(p, type) \
	(p) = new type;\
	if (!(p))\
{\
	printf("new operator fail!!");\
}

#define _NEWA(p, type, count) \
	(p) = new type[count];\
	if (!(p))\
{\
	printf("new operator fail!!");\
}

#define _DELETE(p) \
	if (p)\
{\
	delete (p);\
	(p) = NULL;\
}

#define _DELETEA(p) \
	if (p)\
{\
	delete[] (p);\
	(p) = NULL;\
}


#define STEP_FINISH(eRes) \
{\
StepFinished(eRes);\
return;\
}

#define STEP_FINISH_WITH_RETURN(eRes, ret) \
{\
	StepFinished(eRes);\
	return ret;\
}

#define ToString(x) #x 


enum E_TOOLBAR_CMD_INDEX
{
	eBtnGoBack = 0,
	eBtnGoForward,
	eBtnStop,
	eBtnRefresh,
	//eBtnViewAll,
	eBtnStepCtrl,         
	eBtnViewSrc,
	eBtnVagueLoc,

	eBtnNormalMaxEnd,     

	eBtnStepCtrlPause = eBtnStepCtrl,
	eBtnStepCtrlContinue = eBtnNormalMaxEnd,
};


enum E_IMAGE_CODE_INPUT_STATUS
{
	eNULL = 0,       
	eInput,           
	eRefresh,         
	eFinish,          
};


enum E_STEP_PHASE
{
	eStepStart,     
	eStepRunning,    
	eStepEnd,        
};

struct T_MENU_SELECTED
{
	CPoint  pt;                     
	E_ActionMethod amMethod;      
	CStdString strMenuString;       
	CStdString strSymbol;         

	T_MENU_SELECTED()
	{
		pt.x = 0;
		pt.y = 0;
		amMethod = amNone;
		strMenuString = _T("");
		strSymbol = _T("");
	}
};

struct T_STEP_DATA_TO_TASKVIEW
{
	HANDLE hSignalEvent;          
	CStdString strStepData;      
	int iObjIndexForVagueLoc;  
	CStdString strStepID;        
	int iCurVLIndexValue;         //block第几次重试
	int nRetryVertifyTimes;       //由于验证码成功标记查找失败，而使block重试的次数
	CStdString strOCRParam;		  //验证码参数

	T_STEP_DATA_TO_TASKVIEW()
	{
		hSignalEvent = NULL;
		strStepData = _T("");
		iObjIndexForVagueLoc = -1;
		strStepID = _T("");
		iCurVLIndexValue = 0;
		nRetryVertifyTimes = 0;
		strOCRParam = _T("");
	}
};


struct T_CONFIG_PARAM
{
	int nPort;                  
	int nMode;                   
	int nTimeoutRetry;         
	int nFailRetry;            
	int nOcrMaxRetry;            
	int nMaxTimesOfRestartFrom1thStep;

	int nPageTimeout;          
	int nPageTimeoutRetry;       
	int nClickTimeout;           
	int nClickTimeoutRetry;
	int nMaxCpuUseage;
};

class CSumengine2View;


struct T_UPLOAD_FILE_PARAM
{
	CSumengine2View *pView;   
	HWND hMainFrameWnd;       
	HANDLE hUploadFinishSignal;           //同步对象
	CStdString strFile;       
	CStdString strWebName;
	CStdString strErrorMsg;				//错误信息

	T_UPLOAD_FILE_PARAM &operator = (const T_UPLOAD_FILE_PARAM &other)
	{
		if (this == &other)
			return *this;

		hUploadFinishSignal = other.hUploadFinishSignal;
		pView = other.pView;
		hMainFrameWnd = other.hMainFrameWnd;
		strFile = other.strFile;
		strWebName = other.strWebName;
		strErrorMsg =  other.strErrorMsg;

		return *this;
	}
};

struct T_UPLOAD_FILE_RESULT
{
	bool bRes;
	CStdString strErrorMsg;//错误信息

	T_UPLOAD_FILE_RESULT()
	{
		bRes = false;
		strErrorMsg = _T("");
	}
};

struct T_DOWNLOAD_OCR_PIC_PARAM
{
	CSumengine2View *pView;   
	CStdString strURL;       
	CStdString strImgFile;  

	T_DOWNLOAD_OCR_PIC_PARAM &operator = (const T_DOWNLOAD_OCR_PIC_PARAM &other)
	{
		if (this == &other)
			return *this;

		pView = other.pView;
		strURL = other.strURL;
		strImgFile = other.strImgFile;

		return *this;
	}
};

//打码服务用的结构体
struct T_INDETIFY_ORC_PARAM
{
	LONG       nRecognizeResult;  //识别验证码返回的结果值
	CStdString strImgPath;        //图片路径
	LPTSTR     strCode;           //识别出来的验证码
};

//调用内核时的命令行参数
typedef struct tdosparam
{
	int m_iCommpPort;		       //通信端口号
	int m_iRunMode;			       //内核运行模式
	CString m_strUrl;			   //需要内核打开的URL地址(暂时只用在企业版定时任务)
	BOOL  m_bIsRemoteCode;		   //是否远程打码
	BOOL  m_bIsYunTask;			   //是否为云推广任务

	tdosparam()
	{
		m_iCommpPort = 0;
		m_iRunMode = 0;
		m_strUrl = _T("");
		m_bIsRemoteCode = FALSE;
		m_bIsYunTask = FALSE;
	}
}_TDOSPARAM, *PTDOSPARAM;

struct XYPoint
{
	int x;
	int y;
};

extern T_CONFIG_PARAM g_cfgParam;
extern vector<CString> g_vecWords;
extern map<CStdString, XYPoint> g_mapObject;
extern vector<CStdString> g_vecSilentWeb;
extern map<CStdString, CStdString> g_mapWebIdToUrl;
//记录远程打码时需要将打码结果转小写的网站ID
extern CString g_strMakeLowerWebId;
extern LONGLONG g_lCurrentTickCount;
//限制同一进程的抢占焦点步骤同时进入
extern BOOL g_bSyncStepEnter;		
#endif