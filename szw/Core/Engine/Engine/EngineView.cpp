
// Sumengine2View.cpp : CSumengine2View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Engine.h"
#endif

#include "EngineDoc.h"
#include "EngineView.h"
#include "ChildFrm.h"
// #include "DlgJumpURL.h"
#include "HtmElementLocate.h"
#include "MainFrm.h"
#include "SimulateInputString.h"
#include "PublicFunc.h"
#include "CommonDef.h"
#include "ImageCode.h"
#include "SelectFile.h"
#include <MsHtmdid.h>
#import <msxml6.dll>
#include <atlbase.h>
#include <Mshtml.h>
#import <mshtml.tlb>
#include <UrlMon.h>
#include "KeyboardInput.h"
#include "LevelDlg.h"
#include "ClipboardBackup.h"
#pragma comment(lib, "urlmon.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMyPopupMenu CSumengine2View::m_sPopupMenu;
CLock CSumengine2View::m_sLockFrameWnd;

// CSumengine2View

IMPLEMENT_DYNCREATE(CSumengine2View, CLhpHtmlView)

BEGIN_MESSAGE_MAP(CSumengine2View, CLhpHtmlView)
	ON_MESSAGE(WM_VIEW_USER_CMD_ENTER, &CSumengine2View::OnUserCmdEnter)
	ON_MESSAGE(WM_VIEW_STOP_TASK, &CSumengine2View::OnStopTask)
	ON_MESSAGE(WM_VIEW_SET_JUMP_URL, &CSumengine2View::OnSetJumpURL)
	ON_MESSAGE(WM_VIEW_EVENT_ACTIVE_ELEMENT, &CSumengine2View::OnEventActiveElement)
	ON_MESSAGE(WM_VIEW_GO_BACK, &CSumengine2View::OnMsgGoBack)
	ON_MESSAGE(WM_VIEW_GO_FOWARD, &CSumengine2View::OnMsgGoForward)
	ON_MESSAGE(WM_VIEW_REFRESH, &CSumengine2View::OnMsgRefresh)
	ON_MESSAGE(WM_VIEW_STOP, &CSumengine2View::OnMsgStop)
	ON_MESSAGE(WM_VIEW_TASK_DATA, &CSumengine2View::OnRecieveStepData)

	ON_MESSAGE(WM_VIEW_INIT_SETTING, &CSumengine2View::OnDocViewInit)	
	ON_MESSAGE(WM_VIEW_ACTIVE_VIEW, &CSumengine2View::OnViewActive)
	ON_MESSAGE(WM_VIEW_SUB_VIEW_FINISHED, &CSumengine2View::OnSubViewFinishedLoading)

	ON_MESSAGE(WM_FREE_VIEW_RESOURCE, &CSumengine2View::OnFreeViewResource)
	ON_MESSAGE(WM_VIEW_GET_QUICKLOOK_TEXT, &CSumengine2View::OnMsgQuickLookText)
	ON_MESSAGE(WM_VIEW_GEN_QUICKLOOK_PIC, &CSumengine2View::OnMsgQuickLookPic)
	ON_MESSAGE(WM_VIEW_NAVIGATE_URL, &CSumengine2View::OnShowURL)
//	ON_WM_MOUSEHOVER()
//	ON_WM_MOUSEMOVE()
//ON_WM_MOUSEMOVE()
ON_WM_TIMER()
ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

// CSumengine2View 构造/析构

CSumengine2View::CSumengine2View()
{
	m_pLastHoverElem = NULL;
	m_strLastHoverElemHTML = _T("");
	m_bPageFinishLoad = false;
	m_bNewPageOpen = false;
	m_nCount = 0;

	m_eVerifyCodeType = evtNULL;
	m_strUIMsg = _T("");
	m_pRelatedParentView = NULL;
	m_pMapIIDKs = NULL;
	m_mapStepChkHistory.clear();
	m_dwAmbientProperty = 0;
	m_bOwnInputFocus = false;
	m_bOwnSystemInput = false;

	m_bIsTempView = false ;
	m_hNavigateEvent = NULL ;

	m_pBrforeStep = NULL;
	m_pDelItemStep = NULL;
	m_pSaveSuccStep = NULL;
	m_pCurrentStep = NULL;
	m_pBits = NULL;

	m_bIsTakeMoney = false;
	SetShowWindowFlag(false);

	m_bAutoReg = false;				//当前验证码识别步骤是否执行过自动识别
	m_bLongRange = false;			//当前验证码识别步骤是否执行过远程打码
	m_bManmade = false;				//当前验证码识别步骤是否执行过人工打码
	m_bIsGetSyncEnterStep = false;  //默认当前view不拥有步骤同步标记
	m_strLastStepID = _T("");
	m_bIsStepRetry = false;
	m_bIsSameStepShowMessage = false;
	m_strApiPhotoFileName = _T("");
}

CSumengine2View::~CSumengine2View()
{
	m_bStop = true;
	if(m_pBits)
	{
		delete m_pBits;
		m_pBits = NULL;
	}
#ifdef xiaoqiang
	if(NULL != m_pLastHoverElem)
	{
		m_pLastHoverElem->Release();
	}
#endif

	//如果此标记不激活，设置为激活
	ReleaseEnterFlag();

	//页面关闭时释放同步焦点互斥信号
	ReleaseInputFocusMutex();
}

BOOL CSumengine2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CLhpHtmlView::PreCreateWindow(cs);
}

void CSumengine2View::OnInitialUpdate()
{
	CLhpHtmlView::OnInitialUpdate();
 
	CFrameWnd *pFrameWnd = GetParentFrame();
	if(pFrameWnd)
	{
		pFrameWnd->ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("pFrameWnd 为空！不能最大化"));
	}
	
	if (theApp.GetCurrentRunMode() == rmNormal)
	{	
		GetDocument()->SetTitle(_T(""));
	
	}
	else
	{
		GetDocument()->SetTitle(_T("about:blank"));
	}
}

HRESULT CSumengine2View::OnShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT * plResult)
{
	//非正常模式/显示模式时允许弹框，否则屏蔽弹框
 	if (rmNormal != theApp.GetCurrentRunMode() && rmShow != theApp.GetCurrentRunMode())
 	{
 		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到弹框提示,未屏蔽，标题：%s 内容：%s"), CString(lpstrCaption), CString(lpstrText));
	    *plResult = MessageBox(CString(lpstrText), CString(lpstrCaption), dwType);
	}
 	else
 	{
		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("捕获到弹框提示,已屏蔽，标题：%s 内容：%s"), CString(lpstrCaption), CString(lpstrText));
 		*plResult = 1;
 	}

	CString strText = lpstrText;

	//此处做截取处理(处理类似与浙江民营企业网一直弹框的问题)
	if (!m_bIsSameStepShowMessage)
	{
		//分隔不同步骤之间的弹框信息
		if (m_strUIMsg != _T(""))
			m_strUIMsg += DELIMITER_POPUPINFO_BETWEEN_STEPS;
		
		m_strUIMsg += strText;
	}
	else
	{
		//步骤X连续弹出对话框的处理

		CStdString strStdTemp = m_strUIMsg;

		//1.找到本步骤的弹框信息位置
		int iPos = strStdTemp.ReverseFind(DELIMITER_POPUPINFO_BETWEEN_STEPS) + _tclen(DELIMITER_POPUPINFO_BETWEEN_STEPS);

		//2.得到本步骤的弹框信息
		CString strTemp = strStdTemp.Mid(iPos);

		//3.若新弹出的内容未包含在本步骤的弹框信息中则添加
		if (strTemp.Find(strText) == -1)
		{
			m_strUIMsg += DELIMITER_POPUPINFO_FOR_ONE_STEP;
			m_strUIMsg += strText;
		}
	}
	
	m_bIsSameStepShowMessage = true;
	return S_OK;
}

// CSumengine2View 诊断

#ifdef _DEBUG
void CSumengine2View::AssertValid() const
{
// 	return;       //暂时屏蔽，不启用断言，避免切换到验证码显示窗口时异常

	CLhpHtmlView::AssertValid();
}

void CSumengine2View::Dump(CDumpContext& dc) const
{
	CLhpHtmlView::Dump(dc);
}

CSumengine2Doc* CSumengine2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSumengine2Doc)));
	return (CSumengine2Doc*)m_pDocument;
}
#endif //_DEBUG

// CSumengine2View 消息处理程序
LRESULT CSumengine2View::OnUserCmdEnter(WPARAM wParam, LPARAM lParam)
{
	CStdString strTaskID = CStdString((LPCTSTR)wParam);
	
	SetTaskID(strTaskID);   

	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		GetDocument()->SetTitle(_T(""));
	}
	else
	{
		GetDocument()->SetTitle(strTaskID);
	}

	return 0;
}

LRESULT CSumengine2View::OnStopTask(WPARAM wParam, LPARAM lParam)
{
	//停止任务时，关闭框架窗口
	CFrameWnd *pFrameWnd = GetParentFrame();
	if(pFrameWnd)
	{
		GetParentFrame()->SendMessage(WM_CLOSE);
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("pFrameWnd 为空！WM_CLOSE"));
	}
	
	return 0;
}

LRESULT CSumengine2View::OnSetJumpURL(WPARAM wParam, LPARAM lParam)
{
	CStdString strURL = CStdString((LPCTSTR)wParam);

	//准备数据发送给采编工具
	//....

	//更新地址栏
    AfxGetMainWnd()->SendMessage(WM_MAINWND_UPDATE_ADDRESS_URL, (WPARAM)(LPCTSTR)strURL);

	//Navigate2(strURL);
	Navigate2(strURL, navTrustedForActiveX | navNoReadFromCache | navNoWriteToCache);

	return 0;
}

LRESULT CSumengine2View::OnShowURL(WPARAM wParam, LPARAM lParam)
{
	CStdString strURL = CStdString((LPCTSTR)wParam);

	Navigate2(strURL, navTrustedForActiveX | navNoReadFromCache | navNoWriteToCache);

	return 0;
}

LRESULT CSumengine2View::OnEventActiveElement(WPARAM wParam, LPARAM lParam)
{
	CStdString strData = CStdString((LPCTSTR)wParam);

	//显示到源码窗口
	CFrameWnd *pFrameWnd = GetParentFrame();
	if(pFrameWnd)
	{
		CWnd *pWnd = ((CChildFrame*)GetParentFrame())->GetSourceWnd();
		if(pWnd)
		{
			pWnd->SetWindowText((LPCTSTR)strData);
		}
		else
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("pWnd 为空！显示到源码窗口失败"));
		}
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("pFrameWnd 为空！GetSourceWnd失败"));
	}

	return 0;
}

LRESULT CSumengine2View::OnMsgGoBack(WPARAM wParam, LPARAM lParam)
{
	GoBack();
	return 0;
}

LRESULT CSumengine2View::OnMsgGoForward(WPARAM wParam, LPARAM lParam)
{
	GoForward();
	return 0;
}

LRESULT CSumengine2View::OnMsgRefresh(WPARAM wParam, LPARAM lParam)
{
	Refresh();
	return 0;
}

LRESULT CSumengine2View::OnMsgStop(WPARAM wParam, LPARAM lParam)
{
	Stop();
	return 0;
}

LRESULT CSumengine2View::OnDocViewInit(WPARAM wParam, LPARAM lParam)
{
	//设置文档标题
	if (theApp.GetCurrentRunMode() == rmNormal)
	{
		GetDocument()->SetTitle(_T(""));
	}
	else
	{
		GetDocument()->SetTitle(m_strNsName);
	}

	if(rmNormal == theApp.GetCurrentRunMode())
	{
		vector<CStdString>::iterator iter;
		CStdString strTmp;
		for(iter = g_vecSilentWeb.begin(); iter != g_vecSilentWeb.end(); ++iter)
		{
			strTmp = *iter;
			if(-1 != strTmp.Find(m_strTaskID))
			{
				SetSilent(TRUE);
			}
		}
	}
	

	//设置视图是否加载图片等标记
	CComQIPtr<IOleControl> QiOleControl(m_pBrowserApp);
	if (QiOleControl)
	{
		LRESULT hr = QiOleControl->OnAmbientPropertyChange(DISPID_AMBIENT_DLCONTROL);
		if(FAILED(hr))
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("QiOleControl->OnAmbientPropertyChange失败"));
		}
	}
	
	return 0;
}

LRESULT CSumengine2View::OnFreeViewResource(WPARAM wParam, LPARAM lParam)
{
	m_bStop = true;

	//释放步骤同步标记
	ReleaseEnterFlag();

	//此处应该也要释放互斥量
	ReleaseInputFocusMutex();

	CFrameWnd *frameWndTemp = GetParentFrame();
	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("+++++++++++++++++++++GetParentFrame = %d"), frameWndTemp);
	if(frameWndTemp)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("+++++++++++++++++++++View发送数据到父窗口"));
		frameWndTemp->PostMessage(WM_CLOSE);
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("OnFreeViewResource失败"));
	}

	return 0;
}

void CSumengine2View::GetViewURL(_bstr_t &bstr)
{
	//_bstr_t bstr;
	CComQIPtr<IHTMLDocument2> spDoc;
	spDoc.Attach((IHTMLDocument2*)GetHtmlDocument());                       
	if (spDoc)
	{
		spDoc->get_URL(&bstr.GetBSTR());
	}
}

//响应获取页面文本的消息
LRESULT CSumengine2View::OnMsgQuickLookText(WPARAM wParam, LPARAM lParam)
{
	CHtmElementLocate locate;
	CComQIPtr<IHTMLDocument2> pTempDoc1;// = (IHTMLDocument2 *)GetHtmlDocument();
	pTempDoc1.Attach((IHTMLDocument2 *)GetHtmlDocument());
	if(NULL == pTempDoc1)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("OnMsgQuickLookText失败"));
		return -1;
	}
	locate.GetBodyText((IHTMLDocument2 *)pTempDoc1, m_strQuickLookText);
	//locate.GetBodyText((IHTMLDocument2 *)GetHtmlDocument(), m_strQuickLookText);

	return 0;
}

//获取当前页面的高度
int CSumengine2View::GetPageRect(RECTL &rect)
{
	CComQIPtr<IHTMLDocument2> pDoc;// = (IHTMLDocument2 *)GetHtmlDocument();
	pDoc.Attach((IHTMLDocument2 *)GetHtmlDocument());
	if (NULL == pDoc)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetPageRect失败"));
		return -1;
	}

	rect.left = 0;
	rect.top  = 0;

	//取Body元素的高度
	long lHeight = -1;
	CComQIPtr<IHTMLElement> qiBody;
	pDoc->get_body(&qiBody);
	if (qiBody)
	{
		CComQIPtr<IHTMLElement2> qiBody2;
		qiBody.QueryInterface(&qiBody2);
		if (qiBody2)
		{
			qiBody2->get_scrollHeight(&rect.bottom);
			qiBody2->get_scrollWidth(&rect.right);
			lHeight = rect.bottom;
			//qiBody2->getAttribute( L"scrollHeight", 0, &vtAttrib );
		}
		else
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("qiBody2获取失败"));
			return -1;
		}

		rect.bottom += 20;
		rect.right += 20;
	}

	return lHeight;
}
//检查Object元素是否被包含在Frame中
HRESULT CSumengine2View::CheckObjInFrame(CComQIPtr<IHTMLElement> qiObjElem, bool &bEmbedInFrame, CComQIPtr<IHTMLElement>& pFrame)
{
	//检查Object元素是否被包含在Frame中
	CComQIPtr<IDispatch> qiWbDisp;
	HRESULT    hr = NULL;
	bEmbedInFrame = false;
	CHtmElementLocate locate;

	T_ITEM *pItem = m_objStepData.GetItem();

	IHTMLDocument2 *pCurDoc = (IHTMLDocument2*)GetHtmlDocument();
	
	locate.SetHtmlDocument(pCurDoc);
	if (locate.IsElementInFrame(pCurDoc, *pItem, pFrame))
	{
		bEmbedInFrame = true;
		return S_OK;
	}

	return S_FALSE;
}

//响应生成页面快照的消息
LRESULT CSumengine2View::OnMsgQuickLookPic(WPARAM wParam, LPARAM lParam)
{
	int iRet = 0;
	CRect rectView;
	RECTL rectTemp;

	//Step1.获取View的原始大小
	GetClientRect(&rectView);      

	//Step2.将View调整为刚好装下整个页面
	int tmpRet = GetPageRect(rectTemp);
	if(-1 == tmpRet)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("OnMsgQuickLookPic获取PageRect失败"));
		return 0;
	}
	MoveWindow((LPCRECT)&rectTemp);

	CComQIPtr<IViewObject> qiViewObj;
	CComQIPtr<IHTMLDocument2> pDoc;// = (IHTMLDocument2 *)GetHtmlDocument();
	pDoc.Attach((IHTMLDocument2 *)GetHtmlDocument());
	if (NULL == pDoc)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetHtmlDocument失败"));
		goto end;
	}

	//Step3.截图
	pDoc.QueryInterface(&qiViewObj);
	if (qiViewObj)
	{
		HRESULT hRes = 0;
		//VARIANT vtAttrib;

		CDC *pDc = GetDC();

		CDC MemDC;
		CBitmap *pBitmap, *pOldBitmap;
		CBitmap bitmap;

		MemDC.CreateCompatibleDC( pDc );
		bitmap.CreateCompatibleBitmap( pDc, rectTemp.right - rectTemp.left, rectTemp.bottom - rectTemp.top);
		pDc->ReleaseOutputDC();

		pOldBitmap = (CBitmap *)MemDC.SelectObject( &bitmap );

		hRes = qiViewObj->Draw(  DVASPECT_CONTENT, //Aspect to draw - we always want content 
			-1,					//Always -1 
			NULL,				//We don't use pvAspect 
			NULL,				//We don't need to specify a target device 
			NULL,				//We don't need a target HIC 
			MemDC.m_hDC,		//The DC to draw on 
			&rectTemp,				//The bounding box to draw in 
			NULL,				//Only for metafiles 
			NULL,				//Not using a callback 
			0 );				//Not using a callback 

		if (FAILED(hRes))
		{
			//bitmap.DeleteObject();
			MemDC.DeleteDC();
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("qiViewObj->Draw失败"));
			goto end;
		}

		pBitmap = (CBitmap *)MemDC.SelectObject( pOldBitmap );

		//保存为.jpg图片
		CImage imgTemp;
		TCHAR *pFileName = (TCHAR *)wParam;
		imgTemp.Attach(pBitmap->operator HBITMAP());
		HRESULT hr = imgTemp.Save(pFileName, Gdiplus::ImageFormatJPEG);
		if(FAILED(hr))
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("保存图片文件失败"));
		}

		//回收
		bitmap.DeleteObject();
		MemDC.DeleteDC();
		iRet = 1;
	}	

end:
	//Step4.还原View大小，显示滚动条 一定要做，否则影响页面的正常浏览
	MoveWindow((LPCRECT)&rectView);
	ShowWindow(SW_MAXIMIZE);

	return iRet;
}

LRESULT CSumengine2View::OnViewActive(WPARAM wParam, LPARAM lParam)
{
    //激活视图所属的框架
	CFrameWnd *pFrame = GetParentFrame();
	if(pFrame)
	{
		//pFrame->ActivateFrame(SW_MAXIMIZE);
		ForceFrameWndToCurrent(pFrame);
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("OnViewActive失败"));
	}
	//GetParentFrame()->ActivateFrame(SW_MAXIMIZE);

	//非正常模式下显示活动框架的URL
	if (theApp.GetCurrentRunMode() != rmNormal)
	{
		((CMainFrame*)AfxGetMainWnd())->SendMessage(WM_MAINWND_UPDATE_ADDRESS_URL, (WPARAM)GetLocationURL().GetBuffer());
	}

	return 0;
}

//“点击”操作引起的新页面打开完成
LRESULT CSumengine2View::OnSubViewFinishedLoading(WPARAM wParam, LPARAM lParam)
{
	KillTimer(TIMER_PAGE_STATE_CLICK_CHK);         //停止本页的状态检查Timer
	m_bNewPageOpen = false;

	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("新页面打开完成！"));
    STEP_FINISH_WITH_RETURN(srFinishedOK, 0);
}

bool CSumengine2View::Init(CStdString strTaskID, CStdString strNsName, DWORD dwAmbientProperty)
{
	SetTaskID(strTaskID);
	SetNsName(strNsName);
	m_dwCreationTime = GetTickCount();
	m_bStop = false;
	m_dwAmbientProperty |= dwAmbientProperty;
	m_nTimeoutRetryedTimes = 0;
	m_nClickStatCheckedTimes = 0;

	CStdString strLogName = _T("");
	strLogName.Format(_T("Core\\%s.log"), strTaskID.GetBuffer());
	if (!m_log.IsLogFileOpened())
	{
		m_log.OpenLogFile(strLogName.GetBuffer(),NULL,TRUE);
	}

	m_strOldImgPath.Format(_T("%s\\image\\code\\old-%s.tmp"), GetInstallPath(), strTaskID);

	//初始化设置，如设置文档标题为网站名称、是否屏图等
	SendMessage(WM_VIEW_INIT_SETTING);

	return true;
}

CStdString CSumengine2View::GetTaskID()
{
	return m_strTaskID;
}

void CSumengine2View::SetTaskID(CStdString strTaskID)
{
	m_strTaskID = strTaskID;
}

void CSumengine2View::SetNsName(CStdString strNsName)
{
	m_strNsName = strNsName;
}

void CSumengine2View::SetIIDKs(std::map<CStdString, CStdString> *pMapIIDKs)
{
	if (!pMapIIDKs)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("错误：动参表指针为空！"));
	}

	m_pMapIIDKs = pMapIIDKs;
}

//设置产生本视图的视图指针
void CSumengine2View::SetParentView(CSumengine2View *pView)
{
	m_pRelatedParentView = pView;
}

CStdString CSumengine2View::GetNsName()
{
	return m_strNsName;
}

//步骤执行结束，设置最终结果
void CSumengine2View::StepFinished(E_RESULT eRes/*, CStdString strErrDesc*/)
{
	//容错处理
	if ((srFinishedFail == eRes || srUploadFileError == eRes) && m_objStepData.GetFaultTolerant())
	{
		SetStepResult(srFinishedOKTolerantFail);
	}
	else
	{
		SetStepResult(eRes);
	}

	//若步骤失败且当前内核窗口为显示状态则隐藏
	if (eRes != srFinishedOK && GetShowWindowFlag())
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("步骤：%s 执行失败且当前窗口为显示状态!"), CString(m_objStepData.GetDescrition()));
		if (theApp.GetCurrentRunMode() == rmNormal)
		{
			//::ShowWindow(AfxGetMainWnd()->m_hWnd, SW_HIDE);
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, NULL, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_HIDEWINDOW);
			SetShowWindowFlag(false);
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("正常模式，步骤失败后隐藏窗口!"));
		}
	}
}

void CSumengine2View::SetStepResult(E_RESULT eRes)
{
	m_resVal = eRes;
}

void CSumengine2View::SetStepErrDesc(CStdString strDesc)
{	
	if (m_pCurrentStep == NULL)
	{
		m_strErrDescription = m_objStepData.GetDescrition() + DELIMITER_STEP_DESC_AND_STEP_ERRINFO + strDesc;
	}
	else
	{
		m_strErrDescription = m_objCurrentStep.GetDescrition() + DELIMITER_STEP_DESC_AND_STEP_ERRINFO + strDesc;
	}
}

void CSumengine2View::SetSetpOutData(CStdString strAttr, CStdString strValue)
{
	//多个值之间以(;,)分隔
	CStdString strOneItem = _T("");
	strOneItem.Format(_T("%s=%s(;,)"), strAttr, strValue);

	m_strOutData += strOneItem;
}

void CSumengine2View::SetQuickLook()
{
	CString strPage = _T("");
	GetSource(strPage);

	m_strQuickLookText = strPage;
}

CStdString CSumengine2View::GetViewTitle()
{
	return GetDocument()->GetTitle();
}

CStdString CSumengine2View::GetViewRelatedURL()
{
	return GetLocationURL();
}

//不带对象定位信息的步骤数据（如人工输入）
void CSumengine2View::GetStepData(T_MENU_SELECTED &param)
{
	m_objStepData.Clear();

	m_objStepData.SetSurWait(STEP_DELAY_TIME);
	m_objStepData.SetAction(param.amMethod);
	m_objStepData.SetDescription(param.strMenuString);
	m_objStepData.SetSymbol(param.strSymbol);
}

void CSumengine2View::FillStepDataFromItem(T_ITEM *pItem, CComQIPtr<IHTMLElement> QiElem, T_MENU_SELECTED &param, CHtmElementLocate &elemLoc, bool bVagueLoc, int nLevel)
{
	CStdString strItemDescription = _T("");

	//项目描述 先使用outext，为空则使用前文
	strItemDescription = pItem->GetAttribute(_T("outtext"));
	if (_T("") == strItemDescription)
		strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("value"));
	if (_T("") == strItemDescription)
		strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("pertext"));
	if (_T("") == strItemDescription)
		strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("surtext"));

	if (strItemDescription.GetLength() > 10)
		strItemDescription = strItemDescription.Left(10) + _T("...");
	else if (strItemDescription.GetLength() == 0)
		strItemDescription = _T("???");

	//将对象的HTML源码加入属性列表，以便于采编执行如提取数据之类的操作
	pItem->mapAttributes[_T("html")] = elemLoc.GetOuterHTML(QiElem);
	//如果为成功标记做了简化，则检查是否去掉了outtext属性(百度注册需用outtext作为成功标记)---zhumingxing 20160225(其他成功标记也要outtext属性)
	if ((amSuccFeature == param.amMethod || amVerifyCodeSuccFeature == param.amMethod || amSaveSuccFeature == param.amMethod ) && !bVagueLoc)
	{
		CStdString strAttr = pItem->GetAttribute(_T("outtext"));
		CStdString strValue = elemLoc.GetOuterText(QiElem);

		//若有效的outtext被简化掉则将其加入属性列表
		if (strAttr.IsEmpty() && !strValue.IsEmpty())
		{
			pItem->mapAttributes[_T("outtext")] = strValue;
		}
	}
	//为成功标记和提取数据准备比对或提取属性
	std::vector<T_STEP_RETURN> vAttr;
	if (amSuccFeature == param.amMethod || amVerifyCodeSuccFeature == param.amMethod || amSaveSuccFeature == param.amMethod || amGetAttribute == param.amMethod || amGetIIDK == param.amMethod)
	{
		GetElemAllAttributes(elemLoc, QiElem, vAttr);
	}
	m_objStepData.Clear();
	m_objStepData.SetSurWait(STEP_DELAY_TIME);
	m_objStepData.SetAction(param.amMethod);
	m_objStepData.SetDescription(param.strMenuString + DELIMITER_STEP_DESCRIPTION + strItemDescription);
	m_objStepData.SetLevel(nLevel);
	m_objStepData.SetItem(pItem);
	m_objStepData.SetStepReturn(vAttr);
	m_objStepData.SetSymbol(param.strSymbol);
	m_objStepData.SetValuelyLocate(bVagueLoc);
	if (amShow == param.amMethod)
	{
		m_objStepData.SetSymbol(_T("2,(width,height)"));    //真正显示标记,(窗口左上角在屏幕宽度上的百分比,窗口左上角在屏幕高度上的百分比)
	}
	FreeItem(pItem);
}

//返回包含对象定位信息的步骤数据
bool CSumengine2View::GetStepDataWithElemInfo(T_MENU_SELECTED &param, int &iTotalFound)
{
	CStdString strItemDescription = _T("");
	CHtmElementLocate elemLoc;
	CComQIPtr<IHTMLElement> QiElem;
	int nLevel = 0;//获取层级

	//采编数据时，每次“模糊定位”标记仅一次有效
	bool bVagueLoc = ((CMainFrame*)AfxGetMainWnd())->GetVagueLocState();
	if (bVagueLoc)
	{
		((CMainFrame*)AfxGetMainWnd())->SendMessage(WM_MAINWND_CANCEL_VAGUE_LOC);
		//只有模糊查找的时候才会弹出层级对话框
		CLevelDlg levelDlg;
		if ( IDOK == levelDlg.DoModal() )
		{
			nLevel = levelDlg.m_nLevel;
		}
	}
	elemLoc.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	QiElem = elemLoc.GetElement(param.pt);
	if (NULL == QiElem)
	{
		g_msgbox.Show(_T("错误"), _T("获取元素失败！"));
		return false;
	}

	if (nLevel >= 1)//建树操作,必然是模糊查找
	{
		//查找最上层级的父节点，只能有一个,找出最上层的父节点QiElemParent和实际层数nLevel
		CComQIPtr<IHTMLElement> QiElemTemp = QiElem;
		CComQIPtr<IHTMLElement> QiElemParent = QiElemTemp;
		int nCount = nLevel;
		while(nCount > 0)
		{
			if(QiElemTemp)
			{
				QiElemTemp->get_parentElement(&QiElemParent);
				if(QiElemParent == NULL)//如果最多为当前层，假如为2，但是nCount是5层的情况
				{
					QiElemParent = QiElemTemp;
					nLevel = nLevel - nCount;//实际上的层数
					break;
				}
				else
				{
					QiElemTemp = QiElemParent;
					nCount--;
				}
			}
		}

		//从父节点搜索，找出这个父节点的所有子节点，层层递进
		T_ITEM *pItem = elemLoc.GetItemTreeFromElement(QiElemParent, QiElem/*, nLevel*/);
		FillStepDataFromItem(pItem, QiElem, param, elemLoc, bVagueLoc, nLevel);
	}
	else if (0 == nLevel)
	{
		T_ITEM *pItem = elemLoc.GetLocateInfo(QiElem, bVagueLoc, iTotalFound);
		if (NULL == pItem)
		{
			g_msgbox.Show(_T("错误"), _T("NULL == pItem！"));
			return false;
		}
		//项目描述 先使用outext，为空则使用前文
		strItemDescription = pItem->GetAttribute(_T("outtext"));
		if (_T("") == strItemDescription)
			strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("value"));
		if (_T("") == strItemDescription)
			strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("pertext"));
		if (_T("") == strItemDescription)
			strItemDescription = elemLoc.GetAttributeValue(QiElem, _T("surtext"));

		if (strItemDescription.GetLength() > 10)
			strItemDescription = strItemDescription.Left(10) + _T("...");
		else if (strItemDescription.GetLength() == 0)
			strItemDescription = _T("???");

		//将对象的HTML源码加入属性列表，以便于采编执行如提取数据之类的操作
		pItem->mapAttributes[_T("html")] = elemLoc.GetOuterHTML(QiElem);
		//如果为成功标记做了简化，则检查是否去掉了outtext属性(百度注册需用outtext作为成功标记)
		if (amSuccFeature == param.amMethod && !bVagueLoc)
		{
			CStdString strAttr = pItem->GetAttribute(_T("outtext"));
			CStdString strValue = elemLoc.GetOuterText(QiElem);

			//若有效的outtext被简化掉则将其加入属性列表
			if (strAttr.IsEmpty() && !strValue.IsEmpty())
			{
				pItem->mapAttributes[_T("outtext")] = strValue;
			}
		}
		//为成功标记和提取数据准备比对或提取属性
		std::vector<T_STEP_RETURN> vAttr;
		if (amSuccFeature == param.amMethod || 
			amVerifyCodeSuccFeature == param.amMethod ||
			amSaveSuccFeature == param.amMethod ||
			amGetAttribute == param.amMethod || 
			amGetIIDK == param.amMethod)
		{
			GetElemAllAttributes(elemLoc, QiElem, vAttr);
		}
		m_objStepData.Clear();
		m_objStepData.SetSurWait(STEP_DELAY_TIME);
		m_objStepData.SetAction(param.amMethod);
		m_objStepData.SetDescription(param.strMenuString + DELIMITER_STEP_DESCRIPTION + strItemDescription);
		m_objStepData.SetLevel(nLevel);
		m_objStepData.SetItem(pItem);
		m_objStepData.SetStepReturn(vAttr);
		m_objStepData.SetSymbol(param.strSymbol);
		m_objStepData.SetValuelyLocate(bVagueLoc);
		if (amShow == param.amMethod)
		{
			m_objStepData.SetSymbol(_T("2,(width,height)"));    //真正显示标记,(窗口左上角在屏幕宽度上的百分比,窗口左上角在屏幕高度上的百分比)
		}
		FreeItem(pItem);
		return true;
	}
	else
	{
		//错误
		g_msgbox.Show(_T("错误"), _T("建树操作错误！"));
		return false;
	}
}

void CSumengine2View::ShowHTMLbyCursorPos(CPoint pt)
{
	CHtmElementLocate htmLocOjb;
	IHTMLElement *pCurElem = NULL;

	IHTMLDocument2 *pDoc2 = (IHTMLDocument2*)GetHtmlDocument();
	if (NULL != pDoc2)
	{
		//add html release
		//((IHTMLDocument2*)pDoc2)->Release();

		ScreenToClient(&pt);
		htmLocOjb.SetHtmlDocument(pDoc2);
		pCurElem = htmLocOjb.GetElement(pt);
 		if (NULL != pCurElem && pCurElem != m_pLastHoverElem)
		{
			//释放先前的
			if (NULL != m_pLastHoverElem) 
			{
				m_pLastHoverElem->Release();
			}

			m_pLastHoverElem = pCurElem;

			CStdString strData = htmLocOjb.GetOuterHTML(m_pLastHoverElem);

			if (m_strLastHoverElemHTML != strData)
			{
				//显示到源码窗口
				CFrameWnd *pFrame = GetParentFrame();
				if(pFrame)
				{
					CWnd *pWnd = ((CChildFrame*)GetParentFrame())->GetSourceWnd();
					if(pWnd)
					{
						pWnd->SetWindowText((LPCTSTR)strData);
					}
					else
					{
						m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("错误：pWnd为空！"));
						return;
					}
				}
				else
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("错误：pFrame为空！"));
					return;
				}

				m_strLastHoverElemHTML = strData;
			}
		}	
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("ShowHTMLbyCursorPos失败！"));
	}
}


BOOL CSumengine2View::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	static bool bHoldOnMouse = false;

	switch (pMsg->message)
	{
	case WM_RBUTTONDOWN:
		m_sPopupMenu.TrackPopupMenu(TPM_LEFTALIGN, pMsg->pt.x, pMsg->pt.y, this);
		break;

	case WM_MOUSEMOVE:
		//仅当设置了显示源码标记时才响应鼠标移动消息并输出源码信息
		if (CChildFrame::m_sbViewSrcFlag && !bHoldOnMouse)
		{
			ShowHTMLbyCursorPos(pMsg->pt);
		}
		break;

	case WM_KEYDOWN:
		//F2暂停/继续为新的元素显示网页源码
		if (pMsg->wParam == VK_F2)
		{
			bHoldOnMouse = !bHoldOnMouse;
			break;
		}
	default:
		break;
	}

	return CLhpHtmlView::PreTranslateMessage(pMsg);
}


BOOL CSumengine2View::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	T_MENU_SELECTED menuItem;
	CStdString strTmp = _T("R0");
	//处理自定义的右键菜单消息
	if (m_sPopupMenu.GetMenuItemSelected((UINT)wParam, menuItem))
	{
		ScreenToClient(&menuItem.pt);   //转换成客户区坐标以便定位元素用

		CStepData objStepData;
		CString strText = _T("");
		bool bOK = true;
		E_VERIFY_TYPE eVerifyCodeType = evtNULL;
		CStdString strParam = _T("");
		CStdString strURL = _T("");
		CStdString strInputPrompt = _T("");
		CStdString strShowCodeHeight = _T("");
		int iTotalFound = 0;

		switch (menuItem.amMethod)
		{
		case amNone:
			break;
		case amClick:
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		case amFill:
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		case amIdentify:
			GetIdentifyParam(menuItem.strSymbol, eVerifyCodeType, strParam, strURL, strInputPrompt, strTmp, strShowCodeHeight);
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		case amJumpURL:
		case amSetDataArea://step容器,空信息
			GetStepData(menuItem);
			break;
		case amKeyPress:
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		case amUploadFile:
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		case amUserForm:
			//OnMenuUserForm(menuItem);
			//待处理........
			break;
		case amUserInput:
			GetStepData(menuItem);
			break;
		case amShow:
		case amHide:
		case amGetAttribute:
		case amGetIIDK:
		case amSuccFeature:
		case amFireEvent:
		case amNextPage:
		case amVerifyCodeSuccFeature:
		case amMarkObject:
		case amSaveItem:
		case amDownLoadFile:
		case amDeleteNode:
		case amSaveDelNode:
		case amSaveSuccFeature:
			bOK = GetStepDataWithElemInfo(menuItem, iTotalFound);
			break;
		}

		//定位元素失败或其它异常情况
		if (!bOK)
		{
			m_strErrDescription.Format(_T("找到%d个对象，请尝试使用模糊定位！"), iTotalFound);
			MessageBox(m_strErrDescription, _T("定位失败"), MB_OK);
			return TRUE;
		}

		//准备数据发送给采编工具
		CStdString strStepData = m_objStepData.GetSource();

		T_DATA_OUT dataOut;
		dataOut.strResponseType = _T("GET");
		dataOut.strOutDataset = strStepData;
		dataOut.strTaskID = m_strTaskID;
		dataOut.strCutImg = m_strCutImg;

		CStdString strEncodeData = dataOut.GetSource();
		CStdString strResText = _T("");
		strResText.Format(_T("%s,%s"), m_strTaskID, strEncodeData);

		//MessageBox(strResText);
		theApp.m_sCommClient.SendData(strResText);
		
		return TRUE;
	}

	return CLhpHtmlView::OnCommand(wParam, lParam);
}

//重载此方法，将所链接都在内核中打开
void CSumengine2View::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel)
{
	CWinApp *pApp = AfxGetApp();

	// Get the correct document template.
	POSITION pos = pApp->GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemplete = pApp->GetNextDocTemplate(pos);

	// Create a new frame.
	CFrameWnd *pFrame = pDocTemplete->CreateNewFrame(GetDocument(), (CFrameWnd*)AfxGetMainWnd());

	// Activate the frame.
	pFrame->InitialUpdateFrame(GetDocument(), TRUE);
	CSumengine2View *pView = (CSumengine2View *)((CChildFrame *)pFrame)->GetPageWnd();

	pView->Init(m_strTaskID, m_strNsName, m_dwAmbientProperty);

	//用于检测新页面加载完成
	pView->SetParentView(this);	
	pView->SetTimer(TIMER_PAGE_STATE_TIMEOUT_CHK2, g_cfgParam.nPageTimeout, NULL);  //启动新页面的超时检查Timer
	m_bNewPageOpen = true;
	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("OnNewWindow2即将打开新页面!"));
	//////////////////////////

	//新页面打开后仍将当前框架作为活动框架显示，在视图切换时会激活需要的框架
 	if (theApp.GetCurrentRunMode() == rmCaibian)
	{
		if(!GetParentFrame())
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("OnNewWindow2失败！"));
		}
 		//GetParentFrame()->ActivateFrame(SW_MAXIMIZE);
		ForceFrameWndToCurrent(GetParentFrame());
	}
	else if (theApp.GetCurrentRunMode() == rmBrowser)
	{
		//pFrame->ActivateFrame(SW_MAXIMIZE);
		ForceFrameWndToCurrent(pFrame);
	}

	// Pass pointer of WebBrowser object.
	pView->SetRegisterAsBrowser( TRUE );
	*ppDisp = pView->GetApplication();   

	//CLhpHtmlView::OnNewWindow2(ppDisp, Cancel);
}

void CSumengine2View::ForceFrameWndToCurrent(CFrameWnd *pFrameWnd)
{
	CLocalLock lk(&m_sLockFrameWnd);

	if (!pFrameWnd)
	{  
		pFrameWnd->ActivateFrame(SW_MAXIMIZE);
	}
}

void CSumengine2View::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	LPDISPATCH lpWBDisp = NULL;
	HRESULT    hr = NULL;
	bool bFinishLoad = false;

	hr = m_pBrowserApp->QueryInterface(IID_IDispatch, (void**)&lpWBDisp);
	if(FAILED(hr))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("DocumentComplete失败！"));
		return;
	}

	//屏蔽节点
	if (m_pDelItemStep)
	{
		CHtmElementLocate locate;
		T_ITEM *pItem = m_pDelItemStep->GetItem();
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		locate.DelTargetAllNodes(pItem);
	}

	if (pDisp == lpWBDisp)
	{
		bFinishLoad = true;   //主页面加载完成
	}
	lpWBDisp->Release();

	if (bFinishLoad)
	{
		if (amJumpURL == m_objStepData.GetAction())
		{
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("跳转页面完成，停止超时检查！"));

			KillTimer(TIMER_PAGE_STATE_TIMEOUT_CHK);  //停止超时检查Timer
			STEP_FINISH(srFinishedOK);
		}
		else if (NULL != m_pRelatedParentView)
		{
			//由“点击”等操作引起的新页面加载完成后通知父视图
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("新页面加载完成，通知父视图步骤返回！"));
			m_pRelatedParentView->PostMessage(WM_VIEW_SUB_VIEW_FINISHED);
			KillTimer(TIMER_PAGE_STATE_TIMEOUT_CHK2);  //停止超链接超时检查Timer

			m_pRelatedParentView = NULL;   //通知过父视图后解除与父视图的关系，后续页面加载的消息由本视图自己处理
		}
		
		m_bPageFinishLoad = true;
	}
}

void CSumengine2View::FreeItem(T_ITEM *&pItem)
{
	if (NULL == pItem)
		return;

	FreeItem(pItem->pParent);
	if (NULL != pItem->pParent)
	{
		delete pItem->pParent;
		pItem->pParent = NULL;
	}

	FreeItem(pItem->pBrother);
	if (NULL != pItem->pBrother)
	{
		delete pItem->pBrother;
		pItem->pBrother = NULL;
	}

	pItem->mapAttributes.clear();

	delete pItem;
	pItem = NULL;
}

E_RESULT CSumengine2View::GetStepResult()
{
	return m_resVal;
}

CStdString CSumengine2View::GetStepErrDesc()
{
	return m_strErrDescription;
}

//返回网页的弹框提示信息
CStdString CSumengine2View::GetUiMessage()
{
	return m_strUIMsg;
}

//返回页面的文字内容，便于主控查找任务失败特征
CStdString CSumengine2View::GetQuickLookText()
{
	SendMessage(WM_VIEW_GET_QUICKLOOK_TEXT);

	return m_strQuickLookText;
}

//生成页面快照图片并返回图片路径
CStdString CSumengine2View::GenQuickLookPic(BOOL bFlag)                
{	
	IXMLRW xml;
	TCHAR tszBuf[MAX_PATH] = {0};
	CTime time;
	time = CTime::GetCurrentTime();
	TCHAR tszTarDir[MAX_PATH] = {0};

	TCHAR tszTmp[MAX_PATH] = {0};
	CString tszQuickLookDir = _T("");
	_stprintf(tszTmp, _T("%s\\data2\\mc.dat"), GetInstallPath());
	xml.init(tszTmp);
	xml.ReadString(_T("MC"),_T("RESULTPATH"), _T("savepath"),tszQuickLookDir);


#ifdef _DEBUG
	_stprintf(tszQuickLookDir, _T("%s\\image\\result\\DebugTemp"), GetInstallPath());
	if(!PathFileExists(tszQuickLookDir))
	{
		::CreateDirectory(tszQuickLookDir, NULL);
	}
	TCHAR tszSuccQuickLookDir[MAX_PATH] = {0};
	_stprintf(tszSuccQuickLookDir, _T("%s\\succ"), tszQuickLookDir);
	if(!PathFileExists(tszSuccQuickLookDir))
	{
		::CreateDirectory(tszSuccQuickLookDir, NULL);
	}

	if (bFlag)
	{
		_sntprintf(tszBuf, _TRUNCATE, _T("%s\\%s_%02d%02d%02d.jpg"), \
			tszSuccQuickLookDir,m_strTaskID.GetBuffer(), time.GetHour(), time.GetMinute(), time.GetSecond());
	}
	else
	{
		_sntprintf(tszBuf, _TRUNCATE, _T("%s\\%s_%02d%02d%02d.jpg"), \
			tszQuickLookDir,m_strTaskID.GetBuffer(), time.GetHour(), time.GetMinute(), time.GetSecond());
	}

	if (0 == SendMessage(WM_VIEW_GEN_QUICKLOOK_PIC, (WPARAM)tszBuf))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, _T("取快照图片失败！"));
		return _T("");
	}

	return tszBuf;
#else
	if( (PathFileExists(tszQuickLookDir) == FALSE) || (_tcslen(tszQuickLookDir) == 0) )
	{
		return tszBuf;
	}

	TCHAR tszSuccQuickLookDir[MAX_PATH] = {0};
	_stprintf(tszSuccQuickLookDir, _T("%s\\succ"), tszQuickLookDir);
	if(!PathFileExists(tszSuccQuickLookDir))
	{
		::CreateDirectory(tszSuccQuickLookDir, NULL);
	}

	if (bFlag)
	{
		_sntprintf(tszBuf, _TRUNCATE, _T("%s\\%s_%02d%02d%02d.jpg"), \
			tszSuccQuickLookDir,m_strTaskID.GetBuffer(), time.GetHour(), time.GetMinute(), time.GetSecond());
	}
	else
	{
		_sntprintf(tszBuf, _TRUNCATE, _T("%s\\%s_%02d%02d%02d.jpg"), \
		tszQuickLookDir,m_strTaskID.GetBuffer(), time.GetHour(), time.GetMinute(), time.GetSecond());
	}

	if (0 == SendMessage(WM_VIEW_GEN_QUICKLOOK_PIC, (WPARAM)tszBuf))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, _T("取快照图片失败！"));
		return _T("");
	}

	return tszBuf;
#endif
}

CStdString CSumengine2View::GetStepOutData()
{
	return m_strOutData;
}

int CSumengine2View::GetVagueObjFound()
{
	return m_nVagueObjFound;
}

//获取操作对象的源码 用于循环结束条件的比较
CStdString CSumengine2View::GetStepObjHTML(void)
{
	return m_strStepHTML;
}

DWORD CSumengine2View::GetCreationTime(void)//View的创建时间
{
	return m_dwCreationTime;
}

//清除上步操作的相关数据
void CSumengine2View::ClearLastStepResult(void)
{
	m_resVal = srNoneResult;
	m_strErrDescription = _T("");
	m_strOutData = _T("");
	m_strQuickLookText = _T("");
	m_strStepHTML = _T("");
	m_nVagueObjFound = 0;
	m_nClickStatCheckedTimes = 0;
	m_nTimeoutRetryedTimes = 0;
	m_stepPhase = eStepStart;
	m_bOwnInputFocus = false;
	m_bOwnSystemInput = false;

	//步骤默认执行1次
	m_nStepLoopStartIndex = 1;
	m_nStepLoopEndIndex = 1;

}

//获取对象本身的描述（不含操作描述）
CStdString CSumengine2View::GetPureObjectDescription()
{
	CStdString strText = m_objStepData.GetDescrition();
	int iPos = strText.find(DELIMITER_STEP_DESCRIPTION);
	return strText.Mid(iPos + _tcslen(DELIMITER_STEP_DESCRIPTION));
}

BYTE CSumengine2View::GetVKeyCode(CStdString strVKey)
{
	CString strIniPath = _T("");
	strIniPath.Format(_T("%s\\data2\\key.dat"), GetInstallPath());

	IXMLRW keyIni;
	keyIni.init(strIniPath);

	int nValue = 0;
	keyIni.ReadInt(_T("key"),_T("VKeys"), CString(strVKey), nValue);

	if (nValue == 0)
	{
		CStdString strMsg = _T("");
		strMsg.Format(_T("未定义的虚拟键值: %s！"), strVKey);
		g_msgbox.Show(_T("错误"), strMsg);
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,CString(strMsg));
	}

	return (BYTE)nValue;
}

//发送键盘消息
bool CSumengine2View::SendKeys(CStdString strKeys)
{
	int nSendSucc = 0;
	bool bHandled = false;
	
	if (strKeys.Left(1) == _T("{") && strKeys.Right(1) == _T("}"))
	{
		//需要特殊处理的字符用"｛VK_x｝"括起来，如tab键表示为{VK_TAB}
		CStdString strPreKey = strKeys.Mid(1, strKeys.GetLength() - 2);    
		BYTE byKey = GetVKeyCode(strPreKey);
		if (byKey > 0)
		{
			bHandled = true;
				CKeyboardInput kInput(byKey, true, 100);
				CKeyboardInput kInput2(byKey, false, 100);
				nSendSucc = kInput.Send();
				nSendSucc = kInput2.Send();
		}
	}

	// 发送消息
	if (!bHandled)
	{
		//直接发送字符串
		SendString send(strKeys);
		nSendSucc = send.Send();
	}

	if (nSendSucc > 0)
	{
		return true;
	}
	else
	{
		CStdString strMsg = _T("");
		strMsg.Format(_T("发送键盘消息失败:%s err:%d"), strKeys, GetLastError());
		g_msgbox.Show(_T("错误"), strMsg);
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,CString(strMsg));
		return false;
	}
}

void CSumengine2View::DispUIMessage(void)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))               
	{       
		if (msg.message == WM_QUIT)        
			break;   

		TranslateMessage (&msg);       
		DispatchMessage (&msg);       
	}
}

//延时且不阻塞UI消息
void CSumengine2View::WaitWithUIMsgDisp(int nWait)
{
	DWORD dwStartTime = GetTickCount();

	do 
	{
		DispUIMessage();
		Sleep(100);
	} while (GetTickCount() - dwStartTime < nWait && !m_bStop);
}

void CSumengine2View::ActionSaveItem(bool bDelete)
{
	if (bDelete)
	{
		m_objDelItemStep = m_objStepData;
		m_pDelItemStep   = &m_objDelItemStep;
	}
	else
	{
		m_objBeforeStep = m_objStepData;
		m_pBrforeStep = &m_objBeforeStep;
	}
	StepFinished(srFinishedOK);
	SetEvent(m_hNotifySignal);   
	return ;
}

void CSumengine2View::ActionSavePreSuccFlag()
{
	m_objSaveSuccStep = m_objStepData;
	m_pSaveSuccStep = &m_objSaveSuccStep;

	StepFinished(srFinishedOK);
	SetEvent(m_hNotifySignal);
	return;
}

LRESULT CSumengine2View::OnRecieveStepData(WPARAM wParam, LPARAM lParam)
{	
	T_STEP_DATA_TO_TASKVIEW *pData = (T_STEP_DATA_TO_TASKVIEW *)wParam;

	//动参替换
	CStdString strStepData = pData->strStepData;
	ReplaceIIDKs(strStepData);

	//步骤数据解码
	m_hNotifySignal = pData->hSignalEvent;
	m_objStepData.SetSource(strStepData);

	m_iObjIndexForVagueLoc = pData->iObjIndexForVagueLoc;     
	m_strCurStepID = pData->strStepID;
	m_nCurRetryTime = pData->nRetryVertifyTimes;//当前步骤重试的次数

	if (m_strLastStepID != m_strCurStepID)
	{
		m_strLastStepID = m_strCurStepID;
		m_bIsStepRetry = false;
	}
	else
	{
		m_bIsStepRetry = true;
	}

	//判断步骤是否有变化
	m_bIsSameStepShowMessage = false;

	if(m_objStepData.GetAction() == amIdentify)//在这里保存特定元素,用在下一步的验证码识别的时候点击
	{
		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("当前识别验证码的重试次数为：%d"), m_nCurRetryTime);
	}

	ClearLastStepResult();

	if(m_objStepData.GetAction() == amSaveItem)//在这里保存特定元素,用在下一步的验证码识别的时候点击
	{
		ActionSaveItem(false);
		return 1;
	}
	else if (m_objStepData.GetAction() == amSaveDelNode)
	{
		ActionSaveItem(true);
		return 1;
	}
	//保留预成功标记------20160225
	else if (m_objStepData.GetAction() == amSaveSuccFeature)
	{
		ActionSavePreSuccFlag();
		return 1;
	}
	//解析步骤需要执行的次数
	GetStepLoopParams(m_nStepLoopStartIndex, m_nStepLoopEndIndex, pData->iCurVLIndexValue);
	
	//启动Timer，开始步骤的执行
	SetTimer(TIMER_STEP_EXEC, 15, NULL);
	
	return 1;
}

//通过与Timer（TIMER_STEP_EXEC）配合的方式实现循环
//避免一直在UI线程中do...while循环导致其它任务无法执行的问题
void CSumengine2View::DoAction(void)
{
	try
	{
		switch(m_stepPhase)
		{
		case eStepStart:
			m_stepPhase = eStepRunning;
			m_resVal = srNoneResult;

			//指定本次操作的对象的序号
			m_iObjIndexForVagueLoc = m_nStepLoopStartIndex;

			switch (m_objStepData.GetAction())
			{
			case amNone:
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("无效步骤操作！"));
				g_msgbox.Show(_T("错误"), _T("无效步骤操作！"));
				break;
			case amClick:
			case amNextPage:
				ActionClick();
				break;
			case amFill:
				ActionFill();
				break;
			case amIdentify:
				if(NULL != m_pBrforeStep)//需要点击的验证码
				{
					ActionIdentify(true, _T(""), true);//要注意这个_T("")是不是等价于Empty
				}
				else//不需要点击的验证码
				{
					ActionIdentify(true, _T(""), false);
				}
				break;
			case amJumpURL:
				ActionJumpURL(m_objStepData.GetSymbol());
				break;
			case amKeyPress:
				ActionKeyPress(m_objStepData.GetSymbol());
				break;
			case amUploadFile:
				ActionUploadFile(m_objStepData.GetSymbol());
				break;
			case amShow:
				ActionShowWindow(true);
				break;
			case amHide:
				ActionShowWindow(false);
				break;
			case amGetAttribute:
				ActionGetAttribute();
				break;
			case amGetIIDK:
				ActionGetIIDKs();
				break;
			case amSuccFeature:
			case amVerifyCodeSuccFeature:
				ActionSuccFeature();
				break;
			case amFireEvent:
				ActionFireEvent();
				break;
			case amMarkObject:
				ActionMarkObject();
				break;
			case amDownLoadFile:
				ActionGetOnceKeyMove();
				break;
			case amDeleteNode:
				ActionDelNode();
				break;
			//自定义表单
			case amUserForm:
				ActionPost();
				break;
			default:
				break;
			}

			break;
		case eStepRunning:
			if (srNoneResult != m_resVal)
			{	
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("步骤已返回有效的结果:Result:%d"), m_resVal);
				m_stepPhase = eStepEnd;
			}
			break;
		case eStepEnd:
			m_nStepLoopStartIndex++;

			//如果此标记不激活，设置为激活
			ReleaseEnterFlag();
			//步骤结束释放同步焦点互斥信号
			ReleaseInputFocusMutex();

			//步骤(循环)错误或达到最大循环次数时结束步骤
			if ((srFinishedOK != m_resVal && srFinishedOKTolerantFail != m_resVal && srFinishPost != m_resVal) || m_nStepLoopStartIndex > m_nStepLoopEndIndex)
			{
				//步骤操作完成，通知执行管理模块处理
				KillTimer(TIMER_STEP_EXEC);
				SetEvent(m_hNotifySignal);   
				break;
			}
			else
			{
				//执行下一次循环
				m_stepPhase = eStepStart;

				//有些步骤执行后会引起页面刷新，导致获取的元素集失效
				//所以，保险起见，在一个操作结束后再获取一次元素集
				int iTotalElems = 0;
				GetTargetElemCollection(iTotalElems);
			}
		}
	}
	catch (...)
	{
		g_msgbox.Show(_T("出现异常"), _T("DoAction操作异常!"));

		//如果此标记不激活，设置为激活
		ReleaseEnterFlag();

		//异常后释放同步焦点互斥信号
		ReleaseInputFocusMutex();

		SetStepErrDesc(_T("网站异常!"));
		SetStepResult(srFinishedFail);

		//异常后通知执行管理模块处理
		KillTimer(TIMER_STEP_EXEC);
		SetEvent(m_hNotifySignal);   
	}
}

//释放步骤拥有的输入焦点互斥信号，允许验证码窗口弹出
void CSumengine2View::ReleaseInputFocusMutex()
{	
	//此标记需要保留，避免不同进程同时进入到抢占焦点步骤
	if (m_bOwnInputFocus)
	{	
		m_bOwnInputFocus = false;
		g_ocrInput.ReleaseInputFocus();
	}
	if (m_bOwnSystemInput)
	{	
		//此处不能用WaitWithUIMsgDisp,不然定时器会一直进入导致问题
		//延时必须要加，否则客户端无法检测到是不是内核触发的鼠标键盘事件
		Sleep(300);
		m_bOwnSystemInput = false;
		g_ocrInput.ReleaseSystemInputMutex();
	}
}

//取得用于控制步骤循环的参数
void CSumengine2View::GetStepLoopParams(int &iStart, int &iEnd, int curIndex)
{
	int iTotalElems = 0;
	if (GetTargetElemCollection(iTotalElems) && m_objStepData.GetValuelyLocate() && iTotalElems > 0)
	{
		//解析模糊定位参数
		CStdString strVlSymbol = m_objStepData.GetVLSymbol();
		//if (!strVlSymbol.CompareNoCase(_T("%VLFirst%")))
		if (!strVlSymbol.CompareNoCase(_T("#VLFirst#")))
		{
			//模糊定位第一个
			iStart = 1;
			iEnd = 1;
		}
		//else if (!strVlSymbol.CompareNoCase(_T("%VLLast%")))
		else if (!strVlSymbol.CompareNoCase(_T("#VLLast#")))
		{
			//模糊定位最后一个
			iStart = iTotalElems;
			iEnd = iTotalElems;
		}
		//else if (!strVlSymbol.CompareNoCase(_T("%VLRandom%")))
		else if (!strVlSymbol.CompareNoCase(_T("#VLRandom#")))
		{
			//模糊定位找到对象中的任意一个
			iStart = 1 + rand() % iTotalElems;
			iEnd = iStart;
		}
		//else if (!strVlSymbol.CompareNoCase(_T("%VLAll%")))
		else if (!strVlSymbol.CompareNoCase(_T("#VLAll#")))
		{
			//模糊定位所有找到的对象，循环多次
			iStart = 1;
			iEnd = iTotalElems;
		}
		else if (!strVlSymbol.CompareNoCase(_T("#VLIndex#")))
		{
			if (curIndex > 0)
			{
				iEnd = iStart = curIndex;
			}
			else
			{
				iEnd = iStart = 1;
			}
		}
		else
		{
			//兼容%VLIndex%,主控在执行时会将其替换成数字
			int iIndex = m_iObjIndexForVagueLoc;
			if (-1 == iIndex)
			{
				iIndex = _ttoi(strVlSymbol.GetBuffer());
			}

			iStart = iIndex;
			iEnd = iIndex;
		}
	}
}

//获取目标操作对象的集合
bool CSumengine2View::GetTargetElemCollection(int &iTotalElems)
{	
	CHtmElementLocate locate;
	T_ITEM *pItem = m_objStepData.GetItem();
#ifdef xiaoqiang
	//add by caozhiqiang
	m_log.Trace(LOGL_TOP,LOGT_DEBUG,__TFILE__, __LINE__,_T("增加m_objQiElemCollection指针的清空操作，析构函数中没删除"));
	QI_ELEMENT_COLLECTION::iterator iter = m_objQiElemCollection.begin();	
	for(; iter != m_objQiElemCollection.end(); ++iter)
	{
		if((*iter) != NULL)
		{
			((IHTMLElement *)(*iter))->Release();
		}
	}
#endif

	m_objQiElemCollection.clear() ;
	m_sFindCanDelete.clear();

	//不需定位则返回true
	if (NULL == pItem || pItem->mapAttributes.size() == 0)
		return true;
	
	//此处增加大小写的转换操作
	pItem->MaKeAttributeLower();

	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	locate.SetLevel(m_objStepData.GetLevel());
	//if(vecItem.size() == 0)//没有多个删除的情况
	//{
		if (locate.FromLocateInfo(pItem, m_objQiElemCollection, iTotalElems))
		{
			locate.GetMatchedLocValue(m_sFindCanDelete);
			return true;
		}

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetTargetElemCollection失败！"));
	return false;
}


//获取当前要操作的目标对象
E_RESULT CSumengine2View::GetTargetElement(CComQIPtr<IHTMLElement> &pElem)
{
	CHtmElementLocate locate;
	int iTotalElems = m_objQiElemCollection.size();
	T_ITEM *pItem = m_objStepData.GetItem();
	CStdString strMsg = _T("");

	//不需定位则返回true
	if (NULL == pItem || pItem->mapAttributes.size() == 0)
	{
		return srFinishedOK;
	}
	
	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

	//模糊定位
	if (m_objStepData.GetValuelyLocate())
	{

		if (m_iObjIndexForVagueLoc > iTotalElems)
		{
			m_nVagueObjFound = iTotalElems;    //保存找到的对象个数，控制模块可能会用
		
			strMsg.Format(_T("模糊定位：指定的对象序号(%d)大于实际的对象个数(%d)！"), m_iObjIndexForVagueLoc, iTotalElems);
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,CString(strMsg));
			
			SetStepErrDesc(strMsg);

			return srIndexOutOfVagueLocRange;
		}

		int iIndex = m_iObjIndexForVagueLoc - 1;
		if (iIndex >= 0)
		{
			if(iTotalElems == 0)
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("m_objQiElemCollection.size() == 0,引用了m_objQiElemCollection[%d]"), iIndex);
			}
			pElem = m_objQiElemCollection[iIndex];
			m_strStepHTML = locate.GetOuterHTML(pElem);
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模糊定位：选择的对象序号为第%d个！"), m_iObjIndexForVagueLoc);
			return srFinishedOK;
		}
		else
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("模糊定位：iIndex < 0;m_iObjIndexForVagueLoc = %d"), m_iObjIndexForVagueLoc);
		}

		strMsg = _T("模糊定位");
	}
	else
	{//精确定位

		if (iTotalElems == 1)
		{
			pElem = m_objQiElemCollection[0];
			m_strStepHTML = locate.GetOuterHTML(pElem);
			return srFinishedOK;
		}
		else
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("模糊定位：m_iObjIndexForVagueLoc = %d,iTotalElems = %d"), m_iObjIndexForVagueLoc,iTotalElems);
		}

		strMsg = _T("精确定位");

	}

	strMsg.Format(_T("%s：定位对象[%s]失败，找到%d个对象！"), CStdString(strMsg), m_objStepData.GetDescrition(), iTotalElems);
	SetStepErrDesc(strMsg);

	//////////记录定位失败的属性信息//////////////////////////////////
	CStdString strAttrVal = _T("定位属性：");
	std::map<CStdString, CStdString>::iterator itAttr = pItem->mapAttributes.begin();
	for (; itAttr != pItem->mapAttributes.end(); ++itAttr)
	{
		CStdString strTmp = itAttr->second;
		ReplaceIIDKs(strTmp);
		strAttrVal += itAttr->first + _T("=") + strTmp + _T("  ");
	}

	strMsg += strAttrVal;

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,CString(strMsg));
	////////////////////////////////////////////////////////////////////

	return srFinishedFail;
}

//提取元素所有的可用属性供成功标记进行比对选择或作为提取数据的依据
void CSumengine2View::GetElemAllAttributes(CHtmElementLocate &locate, IHTMLElement *pElem, std::vector<T_STEP_RETURN> &vAttr)
{
	T_ITEM *pItem = NULL;
	if(NULL == pElem)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetElemAllAttributes元素为空"));
		return;
	}

	locate.GetAllUsedAttributes(pElem, pItem);

	if (NULL != pItem)
	{
		std::map<CStdString, CStdString>::iterator itAttr = pItem->mapAttributes.begin();
		for (; itAttr != pItem->mapAttributes.end(); ++itAttr)
		{
			T_STEP_RETURN step;
			step.strAttrib = itAttr->first;
			step.strValue = itAttr->second;

			vAttr.push_back(step);
		}

		FreeItem(pItem);
	}
}

//点击操作
void CSumengine2View::ActionClick(void)
{	
	//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("ActionClick！"));
	
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);

	//模拟鼠标点击，内核窗口需显示
	if (m_objStepData.GetClickType() == ClickMouse)
	{	
		if (srFinishedOK == eRes && NULL != QiElem)
		{	
			ClickElement(QiElem);

			WaitWithUIMsgDisp(200);
			//避免出现采编忘记采集隐藏步骤
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			}

			STEP_FINISH(srFinishedOK);        
		}
	}
	//点击弹出久久信息网的框
	else if (m_objStepData.GetClickType() == ClickDlg)
	{
		if (srFinishedOK == eRes && NULL != QiElem)
		{
			SetTimer(TIMER_CHECK_POP, 1000, NULL);
			ElemClick(QiElem);

			STEP_FINISH(srFinishedOK);
		}

	} 
	//模拟鼠标双击事件
	else if (m_objStepData.GetClickType() == ClickDBMouse)
	{
		if (srFinishedOK == eRes && NULL != QiElem)
		{
			ClickElement(QiElem,TRUE);

			WaitWithUIMsgDisp(200);
			//避免出现采编忘记采集隐藏步骤
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			}
			STEP_FINISH(srFinishedOK);
		}
	}
	else
	{
		if (srFinishedOK == eRes && NULL != QiElem)
		{	
			ElemClick(QiElem);

			//“点击”成功后不立即返回，而是交由Timer检查是否有新页面打开，
			//如果有则像跳转网页一样等待页面打开完成
			SetTimer(TIMER_PAGE_STATE_CLICK_CHK, g_cfgParam.nClickTimeout, NULL);
			return;
		}
	}

	STEP_FINISH(eRes);
}

//删除节点
void CSumengine2View::ActionDelNode()
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		//ElemClick(QiElem);
		//return;
		IHTMLDOMNode *pNode;

		HRESULT hr = QiElem->QueryInterface(&pNode);
		if (SUCCEEDED(hr))
		{
			pNode->removeNode(VARIANT_TRUE, &pNode);
		}
	}
	STEP_FINISH(eRes);
}

//赋值操作
void CSumengine2View::ActionFill(void)
{	
	CString strFillData = m_objStepData.GetSymbol().Mid(0, 3);
	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("Fill data:%s*"), strFillData);

	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CComQIPtr<IHTMLElement2> QiElem2 = QiElem;	
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		CStdString strTagName = locate.GetTagName(QiElem);

		//如果body标签复制操作，经测试使用ElemFocus会导致抢占焦点的问题出现，下面需要做一个特殊处理
		if (strTagName.CompareNoCase(_T("body")))
		{
			ElemClick(QiElem);
			ElemFocus(QiElem2);   //获得焦点
		}
		if (!FillElement(QiElem, m_objStepData.GetSymbol(), m_objStepData.GetFillType()))
		{			
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("赋值失败，元素描述:%s，值:%s！"), CString(m_objStepData.GetDescrition()), CString(m_objStepData.GetSymbol()));
			SetStepErrDesc(_T("填写") + GetPureObjectDescription() + _T("失败"));

			ElemBlur(QiElem2);     //失去焦点
			STEP_FINISH(srFinishedFail);
		}
		//如果不是包含body的填充方式，按原来的方式
		if(strTagName.CompareNoCase(_T("body")))
		{
			ElemBlur(QiElem2);     //失去焦点
			//为确保触发失去焦点事件成功，再来一次
			//ElemFocus(QiElem2);    //获得焦点
			//ElemBlur(QiElem2);     //失去焦点
		}
		else
		{
			CComQIPtr<IHTMLElement2> QiElemTmp = locate.GetFirstTagElement(_T("a"), true);
			if (QiElemTmp == NULL)
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("随机查找元素失败"));
				STEP_FINISH(srFinishedOK);//这里可以这么返回应该没问题
			}
			//ElemFocus(QiElemTmp);    //获得焦点
			//ElemFocus(QiElem2);    //获得焦点
			//ElemFocus(QiElemTmp);    //获得焦点
			ElemBlur(QiElemTmp);     //失去焦点
		} 

		STEP_FINISH(srFinishedOK);
	}

	STEP_FINISH(eRes);
}

//add by zhumingxing 20160808处理API方式发布相关产品信息
void CSumengine2View::ActionPost(void)
{	
	//AfxMessageBox(_T("xxx"));
	//获取步骤数据
	CString strData = m_objStepData.GetSymbol();
	
	int ipos = strData.Find(_T("<@@@>"));
	if (ipos == -1)
	{	
		SetStepErrDesc(_T("Post参数填写有误，请检测!"));
		STEP_FINISH(srFinishedFail);
	}
	else
	{	
		CString strResponse = _T("");
		CString strPostUrl = strData.Mid(0, ipos);
		CString strPostData = strData.Mid(ipos+5);

		//替换发布的图片地址
		strPostData.Replace(_T("#Photo#"), m_strApiPhotoFileName);


		CInternetHttp http;

		//strPostData = _T("Bdetails=原装&Status=1&DiyClass=0&ValidTime=9999&CompanyId=14350350&Currency=1&Name=FAS系类精密电子平&SubName=FAS系类精密电子平1&ImageFlag=&AgentPassword=350D8E6A51FC47D582479EDCBB7DBA85&ImageUrl3=&ImageUrl2=&MainDetails=我公司始终坚持以有超重报警功能，用户可根据自己的要求设置重量的上下限,我公司生产的天平称具有精度高、反应速度快、不锈钢秤盘、砝码外部校准功能、超大LCD、计数百分比功能等特点，同时还具有超重报警功能，用户可根据自己的要求设置重量的上下限.&Region=1027900715&Price=100&Tags=精密天平&ProSize=120*70&Id=0&Class=76&ProNum=1000000&ProUnit=台");
		//strPostData = _T("AgentPassword=350D8E6A51FC47D582479EDCBB7DBA85&CompanyId=14350350&Id=0&Name=FAS系类精密电子天平哦UI几句就急急急&ProSize=120*70&Bdetails=原装&Status=0&ProNum=100&ProUnit=台&Price=5500&ValidTime=9999&ImageUrl2=img.qy6.com/new17/dixie1956327/1430304015_s.jpg&ImageUrl3=&Tags=精密天平,FA精密天平&SubName=FAS系类精密电子天平2333&DiyClass=0&Class=1&Region=1&MainDetails=我公司始终坚持以有超重报警功能，用户可根据自己的要求设置重量的上下限。大大打算大大阿打算大大阿打算的&ImageFlag=&Currency=0");
		int ret = http.HttpPost(strPostUrl, strPostData, strResponse);
		
		if (ret != 0)
		{
			SetStepErrDesc(_T("httpPost失败!"));
			STEP_FINISH(srFinishedFail);
		}

		//代表是完善相册，此时提取图片文件名保存到m_strApiPhotoFileName
		if (strPostUrl.Find(_T("Act=Photo")) != -1)
		{
			int ipos = strResponse.Find(_T("\"Photo\":\""));

			if (ipos != -1)
			{
				int ipos1 = strResponse.Find(_T(".jpg\""));

				if (ipos1 != -1)
				{
					m_strApiPhotoFileName = strResponse.Mid(ipos);

					m_strApiPhotoFileName.Replace(_T("\"Photo\":\""), _T(""));
					m_strApiPhotoFileName.Replace(_T("\""), _T(""));
					m_strApiPhotoFileName.Replace(_T("\r\n"), _T(""));
					m_strApiPhotoFileName.Replace(_T("}"), _T(""));
					m_strApiPhotoFileName.Replace(_T(" "), _T(""));
				}
			}
			else
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("完善相册API调用失败！URL:%s"), strPostUrl.GetBuffer());
			}

		}

		SetStepErrDesc(strResponse.GetBuffer());
		STEP_FINISH(srFinishPost);
	}
}

void CSumengine2View::ActionFireEvent(void)
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CComQIPtr<IHTMLElement2> QiElem2 = QiElem;
		ElemFocus(QiElem2);     //获得焦点

		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

		// 	locate.FireEvent(QiElem, _T("onkeyup"));
		HRESULT hr = locate.FireEvent(QiElem, m_objStepData.GetSymbol().GetBuffer());
		if (SUCCEEDED(hr))
		{
			STEP_FINISH(srFinishedOK);
		}

		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("触发事件失败，类型：%s, hr=0x%08x！"), CString(m_objStepData.GetSymbol()), hr);
		SetStepErrDesc(_T("触发事件失败！") + m_objStepData.GetSymbol());
		STEP_FINISH(srFinishedError);//找到对象但是检查条件失败
		return;
	}
	
	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("触发事件失败！"), CString(m_objStepData.GetSymbol()));
	STEP_FINISH(eRes);
}

CStdString CSumengine2View::GetCutImg()
{
	return m_strCutImg;
}

void CSumengine2View::ActionMarkObject()
{
	m_strCutImg.clear();
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		
		//在view上画框框住选中的内容。有可能多个
		long left;
		long top;
		long width;
		long height;
		CDC *pDc = GetDC();
		CComQIPtr<IHTMLRect> pRect; 
		CPen pen(PS_SOLID, 4, RGB(255, 0, 0));
		CBrush *pBrush    = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CPen *pOldPen     = pDc->SelectObject(&pen);//
		CBrush *pOldBrush = pDc->SelectObject(pBrush);

		//用for语句也不用担心是模糊查找还是唯一查找，因为只要过了GetTargetElement函数
		//模糊查找m_objQiElemCollection就可能有多个元素，而精确查找却只有一个
		QI_ELEMENT_COLLECTION::iterator iter = m_objQiElemCollection.begin();
		for(; iter != m_objQiElemCollection.end(); ++iter)
		{
			CComQIPtr<IHTMLElement2> QiElem2 = *iter;
			
			QiElem2->getBoundingClientRect(&pRect);
			pRect->get_left(&left);
			pRect->get_top(&top);
			pRect->get_right(&width);
			pRect->get_bottom(&height);
			pDc->Rectangle(left, top, width, height);
		}
		pDc->SelectObject(pOldPen);
		pDc->SelectObject(pOldBrush);

		//截图出来
		CRect rectView;
		GetClientRect(&rectView);      
		CDC MemDC;
		CBitmap *pBitmap, *pOldBitmap;
		CBitmap bitmap;
		MemDC.CreateCompatibleDC( pDc );
		bitmap.CreateCompatibleBitmap( pDc, rectView.right - rectView.left, rectView.bottom - rectView.top);
		pOldBitmap = (CBitmap *)MemDC.SelectObject( &bitmap );
		MemDC.BitBlt(0, 0, rectView.right - rectView.left, rectView.bottom - rectView.top, pDc, 0, 0, SRCCOPY);
		pBitmap = (CBitmap *)MemDC.SelectObject( pOldBitmap );
		pDc->ReleaseOutputDC();

		//保存为.jpg图片
		TCHAR tszDir[MAX_PATH] = {0};
		TCHAR tszFileName[MAX_PATH] = {0};
		_stprintf(tszDir, _T("%s\\image\\result"), GetInstallPath());
		if(!PathFileExists(tszDir))
		{
			::CreateDirectory(tszDir, NULL);
		}
		CTime time;
		CImage imgTemp;
		time = CTime::GetCurrentTime();
		_stprintf(tszFileName, _T("%s\\%s_%02d%02d%02d.jpg"), tszDir, m_strTaskID.GetBuffer(), time.GetHour(), time.GetMinute(), time.GetSecond());
		imgTemp.Attach(pBitmap->operator HBITMAP());
		imgTemp.Save(tszFileName, Gdiplus::ImageFormatJPEG);
		m_strCutImg = tszFileName;

		//回收
		bitmap.DeleteObject();
		MemDC.DeleteDC();

		//重绘
		MoveWindow((LPCRECT)&rectView);
		ShowWindow(SW_MAXIMIZE);
	}

	STEP_FINISH(eRes);
}

bool CSumengine2View::VerifyCodeHandleQuestion(CStdString &strParam, bool bFirst, bool bCopy)
{
	if(bFirst)
	{
		GetVerifyCodeOuttext(strParam);
		if(_T("") == strParam)
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取文本验证码为空"));
			return false;
		}
	}
	else 
	{
		if(bCopy)
		{
			if (!ClickSaveElement())//点击验证码,如果失败
			{
				return false;
			}
			WaitWithUIMsgDisp(500);
		}
		CComQIPtr<IHTMLElement> QiElem;
		E_RESULT eRes = GetTargetElement(QiElem);
		if (srFinishedOK == eRes && NULL != QiElem)
		{
			CHtmElementLocate locate;
			locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
			strParam = _T("");
			strParam = locate.GetAttributeValue(QiElem, _T("outtext"));
			if( (strParam != m_strOldVerifyCode) && (strParam != _T("")))
			{
				m_strOldVerifyCode = strParam;
			}

			if(strParam == _T(""))
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取文本验证码为空"));
				return false;
			}
		}
	}
	return true;
}

bool CSumengine2View::VerifyCodeHandlePicture(CStdString strImgPath, bool bCopy, bool bFirst)
{
	if(bCopy)//需要点击的图片验证码，包括第一次第二次......直到最后一次的验证码
	{
		int nRetry = 0;
		int nTime = 0;

	  RETRY://确保下载到验证码，下载不到就返回
		nTime = 0;
		while(nTime < 2)
		{
			WaitWithUIMsgDisp(300);
			if (!GetOriginVerifyCodeFromPage(strImgPath))
			{
				nTime++;
				if(nTime >= 2)
				{
					nRetry++;
					if(nRetry < 2)//重复点击4次一定可以有验证码
					{
						if (!ClickSaveElement())//点击验证码,如果失败
						{
							m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("点击验证码失败"));
							SetStepErrDesc(_T("下载验证码失败！"));
							//STEP_FINISH(srFinishedFail);
							return false;//break;
						}
						goto RETRY;
					}
					else
					{
						m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("重试2次完毕，验证码还是下载不了"));
						SetStepErrDesc(_T("下载验证码失败！"));
						//STEP_FINISH(srFinishedFail);
						return false;//break;
					}
				}
			}
			else//下载验证码tmp文件成功，tmp是bmp格式的验证码图片,需要判断是不是跟上一次的tmp一样，因为有可能搞的跟上次的tmp一样的问题
			{
				if(bFirst)//第一次下载成功验证码不需要通过这里
				{
					//把strImgPath复制到m_strOldImgPath文件中
					//以只读|二进制的方式打开第一个文件
					CFile hFile;
					int len;
					hFile.Open(strImgPath.GetBuffer(),CFile::modeRead|CFile::typeBinary,0);
					len = hFile.GetLength();
					//分配缓冲区
					char *buf;
					buf = new char[len+1];
					hFile.Read(buf,len); //读文件
					hFile.Close();   
					//--------------------------------------------
					//只写|二进制|创建的方式打开第二个文件
					hFile.Open(m_strOldImgPath.GetBuf(),CFile::modeWrite|CFile::typeBinary|CFile::modeCreate,0);
					hFile.Write(buf,len); //写文件
					hFile.Close(); 
					delete[] buf; //释放缓冲区

					break;
				}
				else
				{
					CFile fileNew(strImgPath.GetBuf(),CFile::modeRead); 
					CFile fileOld(m_strOldImgPath.GetBuf(),CFile::modeRead); 
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("strImgPath      = %s"), strImgPath.GetBuf());
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("m_strOldImgPath = %s"), m_strOldImgPath.GetBuf());
					char *pBufNew,*pBufOld;
					DWORD dwFileLenNew,dwFileLenOld;

					dwFileLenNew=fileNew.GetLength();
					dwFileLenOld=fileOld.GetLength();
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("dwFileLenNew = %d, dwFileLenOld = %d"), dwFileLenNew, dwFileLenOld);
					pBufNew=new char[dwFileLenNew+1];
					pBufNew[dwFileLenNew]=0;
					fileNew.Read((void *)pBufNew,dwFileLenNew);
					pBufOld=new char[dwFileLenOld+1];
					pBufOld[dwFileLenOld]=0;
					fileOld.Read((void *)pBufOld,dwFileLenOld);
					fileNew.Close();
					fileOld.Close();

					DWORD dwCount = dwFileLenNew < dwFileLenOld ? dwFileLenNew : dwFileLenOld;
					if(0 == memcmp((void *)pBufOld, (void *)pBufNew, dwCount))//2次验证码相同，需要再取
					{
						m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("+++++++++++++++获取到相同验证+++++++++++++++"));
						delete pBufNew;
						delete pBufOld;

						nRetry = 0;
						nTime++;
						continue;
					}
					else
					{
						//把strImgPath复制到m_strOldImgPath文件中
						//以只读|二进制的方式打开第一个文件
						CFile hFile;
						int len;
						hFile.Open(strImgPath.GetBuffer(),CFile::modeRead|CFile::typeBinary,0);
						len = hFile.GetLength();
						//分配缓冲区
						char *buf;
						buf = new char[len+1];
						hFile.Read(buf,len); //读文件
						hFile.Close();   
						//--------------------------------------------
						//只写|二进制|创建的方式打开第二个文件
						hFile.Open(m_strOldImgPath.GetBuf(),CFile::modeWrite|CFile::typeBinary|CFile::modeCreate,0);
						hFile.Write(buf,len); //写文件
						hFile.Close(); 
						delete[] buf; //释放缓冲区

						delete pBufNew;
						delete pBufOld;
						break;
					}
				}
			}
		}

		if (ChkImageValid(strImgPath) == false)//如果图片不是有效图片(如果下载到的验证码不是有效图片)
		{
			if(nRetry < 2)
			{
				if (!ClickSaveElement())//点击验证码,如果失败
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("点击验证码失败"));
					SetStepErrDesc(_T("下载验证码失败！"));
					//STEP_FINISH(srFinishedFail);
					return false;//break;
				}
				goto RETRY;
			}
			else
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码无效图片"));
				return false;//break;
			}
		}
		
		//m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码下载成功"));
		return true;
	}
	else//验证码是不需要点击的验证码
	{
		int nTime = 0;
		while(nTime < 2)
		{
			WaitWithUIMsgDisp(100);
			if (!GetOriginVerifyCodeFromPage(strImgPath))
			{
				nTime++;
				if(nTime >= 2)
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("重试2次完毕，验证码还是下载不了"));
					return false;
					//break;
				}
				else
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("读取页面上的验证码失败，需要下载新的验证码！"));
					continue;
				}
			}

			if (ChkImageValid(strImgPath) == false)//如果图片不是有效图片
			{
				nTime++;
				if(nTime >= 2)
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("重试2次完毕，验证码还是下载不了"));
					return false;
					//break;
				}
				else
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("下载的验证码不是图片格式，需要下载新的验证码！"));
					continue;
				}
			}

			//m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码下载成功"));
			return true;
		}
	}
}

CStdString CSumengine2View::GetParamFromRP(CStdString &strRP)
{
	int nCur = 0;
	strRP.ToLower();
	if (strRP[0] == 'r')
	{
		nCur = strRP[1] - 48;
		if (m_vecOcrRegister.size() > nCur && nCur >= 0)
		{
			return m_vecOcrRegister[nCur];
		}
		else
		{
			return _T("");
		}
	}
	else if (strRP[0] == 'p')
	{
		nCur = strRP[1] - 48;
		if (m_vecOcrPublish.size() > nCur && nCur >= 0)
		{
			return m_vecOcrPublish[nCur];
		}
		else
		{
			return _T("");
		}
	}
	else
	{
		return _T("");
	}
}

void CSumengine2View::ActionIdentify(bool bFirst, CString strPath, bool bCopy)//bCopy针对保存节点，刷新验证码的时候不能正确获取验证码的，就需要截图处理
{
	CStdString strParam			= _T("");					   //验证码图片的路径，从这个路径下载
	CStdString strIdentifyURL	= _T("");					   //针对flash类型验证码，GetAbsoluteURL
	CStdString strSymbol		= m_objStepData.GetSymbol();   //识别的symbol格式：验证码类型<@@>识别参数<@@>验证码地址<@@>验证码输入提示<@@>验证码所使用的R0R1P0P1 （参数可能包含动参）<@@>拖动/点击类型验证码显示高度设置
	CStdString strImgPath		= GetFileNameForVerifyCode();  //生成验证码的tmp文件，其实是一个bmp文件
	CStdString strAutoOcrResult = _T("");                      //验证码结果
	CStdString strInputPrompt	= _T("");                      //_T("验证码输入提示")
	CString    strUniqueID		= GetUniqueIdBaseOnTask();     //返回一个以任务为ID为前缀的唯一ID字符串（用作验证码文件名等）
	CStdString strRP;										   //自动识别验证码识别参数
	CStdString strShowCodeHeight = _T("");					   //显示高度，目前只使用于极验验证码浮动式

	//strSymbol中的内容分别赋值到相应的作用字符串中 
	GetIdentifyParam(strSymbol, m_eVerifyCodeType, strParam, strIdentifyURL, strInputPrompt, strRP, strShowCodeHeight);
	m_strCurRegOrP = GetParamFromRP(strRP);

	//0表示由弹出框取默认值
	m_ocrData.lgTimeWait = 0;        
	bool bFirstSucc = false;

	switch (m_eVerifyCodeType)
	{
	case evtPicture:
	case evtWeiXin:
		if (!HandlePictureCode(bFirst, bCopy, strImgPath, strParam, strUniqueID, strPath, strInputPrompt))
		{
			STEP_FINISH(srFinishedFail);
		}
		break;

	case evtQuestion:
		if (!HandleQuestionCode(bFirst, bCopy, strImgPath, strParam, strUniqueID, strPath, strInputPrompt))
		{
			STEP_FINISH(srFinishedFail);
		}
		break;

	case evtFlash:
		if (!HandleFlashCode(CString(GetTaskID()), strPath))
		{	
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("拼接flashURL失败!"));
			SetStepErrDesc(_T("显示flash验证码失败！"));
			STEP_FINISH(srFinishedFail);
		}
		break;

	case evtTelephone:
	case evtTelephoneCode:
	case evtEMail:
	case evtEMailCode:
		HandleUserInputCode(strParam, strAutoOcrResult, strUniqueID, strPath, strInputPrompt);
		break;

	case evtDragCode:
	{
		if (HandleDragCode(strInputPrompt, strShowCodeHeight))
		{
			return;
		}
		else
		{
			STEP_FINISH(srFinishedError);
		}
	}
	break;

	default:
		break;
	}
	
	//显示验证码
	if (evtNULL != m_eVerifyCodeType && !strPath.IsEmpty())
	{
		m_strImgCodePath = strPath;
		m_ocrData.strAutoResult = strAutoOcrResult;
		m_ocrData.strNsCode = m_strTaskID;
		m_ocrData.strNsName = m_strNsName;
		m_ocrData.strPicPath = m_strImgCodePath;
		m_ocrData.nCodeType = m_eVerifyCodeType;

		E_ORC_Indentify_Type tp = (E_ORC_Indentify_Type)m_objStepData.GetIndentifyType();// oitAuto;
		//对开通打码服务和不开通打码服务的处理
		if (m_eVerifyCodeType == evtPicture || m_eVerifyCodeType == evtQuestion)//图片验证码的处理方式-------------------------------------------------------------------------------------------------------
		{
			if (tp == oitAuto && m_nCurRetryTime < CUR_STEP_RETRY_TIME_LIMITE)//自动识别
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("自动打码，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), m_nCurRetryTime, (int)m_eVerifyCodeType);
				HandleAutoVertify(strImgPath);
			}
			else if (tp == oitLongRange || (tp == oitAuto && m_nCurRetryTime >= CUR_STEP_RETRY_TIME_LIMITE))//远程打码
			{
				if (g_orcManmade.CodeType())//开通打码服务的
				{
					CStdString strQuestionPath;
					if (m_eVerifyCodeType == evtQuestion)
					{
						CString strText = (CString)strParam;
						CString strBmpPath = (CString)strImgPath;
						if (!TextToBmp(strBmpPath, strText))//问题验证码转换成图片
						{	
							m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("问题验证码转换成图片失败，返回失败"));
							STEP_FINISH(srFinishedFail);
						}
						strQuestionPath = strBmpPath.GetBuffer();
					}
					else
					{
						strQuestionPath = strImgPath;
					}
					if (tp == oitAuto)
					{
						m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("自动打码次数用完，转为远程打码，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					}
					else
					{
						m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("远程打码，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					}
					HandleLongRangeVertify(strQuestionPath);
				}
				else//没开通打码服务的就人工打码
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("没有开通打码服务，人工打码，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					HandleManMadeVertify();
				}
			}
			else//人工打码
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("只设置了人工打码，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
				HandleManMadeVertify();
			}
		}
		else//其他类型验证码，比如flash，邮件验证等，除图片和问题以外的-------------------------------------------------------------------------------------------------------
		{
			if(g_orcManmade.CodeType())//开通打码服务的==============================
			{
				if (tp == oitAuto || tp == oitLongRange/* && m_nCurRetryTime < CUR_STEP_RETRY_TIME_LIMITE*/)//自动识别
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("其他类型验证码，开通打码服务，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					HandleAutoVertify(strImgPath);
				}
			}
			else//没有开通打码服务的==============================
			{
				if(tp == oitAuto && m_nCurRetryTime < CUR_STEP_RETRY_TIME_LIMITE)//自动识别
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("其他类型验证码，没开通打码服务，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					HandleAutoVertify(strImgPath);
				}
				else
				{
					m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("人工打码，没有开打码服务，识别类型 = %d，m_nCurRetryTime = %d，m_eVerifyCodeType = %d"), (int)tp, m_nCurRetryTime, (int)m_eVerifyCodeType);
					HandleManMadeVertify();
				}
			}
		}

		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("压入验证码到显示队列..."));
		//压入显示队列
		g_ocrInput.PushOcrData(m_ocrData);

		//定时获取返回结果
		SetTimer(TIMER_GET_CODE_RESULT, 50, NULL) ;
	}
	else
	{	
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("识别验证码步骤失败：type:%d, path:%s！"), m_eVerifyCodeType, CString(strPath));
		SetStepErrDesc(_T("识别验证码失败！"));
		STEP_FINISH(srFinishedFail);
	}
}

void CSumengine2View::HandleAutoVertify(const CStdString &strImgPath)//自动打码
{
	m_bAutoReg = true;
	m_eCodeOIT = oitAuto;
	autoIndentifyORC(m_ocrData, strImgPath);
	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("自动识别模块------->>只在识别为空的时候显示，识别结果：%s,识别次数：%d"), m_ocrData.strAutoResult, m_nCurRetryTime);
	if (m_ocrData.strAutoResult == _T(""))//如果识别为空，则这次返回失败
	{
		m_ocrData.nCodeType = evtAutoRegonize;
		m_ocrData.strAutoResult = _T("XYZT");
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("自动打码模块识别为空，设置为evtAutoRegonize，XYZT，让他进入打码模块但是不显示出来"));
		//STEP_FINISH(srFinishedFail);
	}
}

void CSumengine2View::HandleLongRangeVertify(const CStdString &strImgPath)//远程打码
{
	m_bLongRange = true;
	m_eCodeOIT = oitLongRange;
	m_ocrData.nCodeType = evtAutoRegonize;
	TCHAR strCode[20] = { 0 };
		
	HANDLE hThread = NULL;	
	DWORD dwExitCode = 0xFFFF;
	T_INDETIFY_ORC_PARAM param;
	param.strImgPath = strImgPath;
	param.strCode = strCode;
	hThread = CreateThread(NULL, 0, &CSumengine2View::ThreadPicResult, (LPVOID)&param, 0, NULL);
	if (!hThread)
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("创建下载图片线程失败，%d!"), ::GetLastError());
	}
	while (GetExitCodeThread(hThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
	{
		WaitWithUIMsgDisp(400);
	}
	CloseHandle(hThread);

	m_RecognizeResult = param.nRecognizeResult;

	//修改远程打码结果返回方式，默认转换为小写，需要转换为大写时才大写
	if (g_strMakeLowerWebId.Find(m_strTaskID)  != -1)
	{
		CString strTmp = param.strCode;
		m_ocrData.strAutoResult = strTmp.MakeUpper();
	}
	else
	{	
		CString strTmp = param.strCode;
		m_ocrData.strAutoResult = strTmp.MakeLower();
	}

	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("远程打码:%s  识别结果：%s, 返回值：%ld"), CString(strImgPath), m_ocrData.strAutoResult, m_RecognizeResult);
	if (m_ocrData.strAutoResult == _T(""))//如果识别为空，则这次返回失败
	{
		m_ocrData.nCodeType = evtAutoRegonize;
		m_ocrData.strAutoResult = _T("XYZT");
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("远程打码模块识别为空，设置为evtAutoRegonize，XYZT，让他进入打码模块但是不显示出来"));
		//STEP_FINISH(srFinishedFail);
	}
	m_bIsTakeMoney = true;
	//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("远程打码模块------->>只在识别为空的时候显示，图片验证码扣钱操作,验证码自动识别为:%s,m_RecognizeResult= %ld"), m_ocrData.strAutoResult,m_RecognizeResult);
}

void CSumengine2View::HandleManMadeVertify()//人工打码
{
	m_bManmade = true;
	m_eCodeOIT = oitManmade;
	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("人工打码模块------->>需要显示验证码对话框"));
}

bool CSumengine2View::TextToBmp(const CString &strBmpPath, const CString &strText)
{	
	CFont font;
	LOGFONT   lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfCharSet = 1;
	lf.lfHeight = 20;
	lf.lfWeight = 100;
	_tcscpy(lf.lfFaceName, _T("微软雅黑"));
	font.CreateFontIndirect(&lf);
	CRect rect(0, 0, 0, 0);
	CPaintDC dc(this);

	CDC bmpdc;
	bmpdc.CreateCompatibleDC(&dc);
	bmpdc.SaveDC();
	bmpdc.SelectObject(font);

	bmpdc.DrawText(strText, rect, DT_CENTER | DT_VCENTER | DT_CALCRECT);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	bmpdc.SelectObject(bmp);
	bmpdc.FillSolidRect(&rect, RGB(255, 255, 255));
	bmpdc.DrawText(strText, rect, DT_CENTER | DT_VCENTER);

	saveBmpFile((HBITMAP)bmp, strBmpPath);

	bmpdc.RestoreDC(-1);
	bmpdc.DeleteDC();

	return true;
}

void CSumengine2View::saveBmpFile(HBITMAP bmp, const CString &strBmpPath)
{
	//把位图的信息保存到bmpinfo;  
	BITMAP bmpinfo;
	GetObject(bmp, sizeof(BITMAP), &bmpinfo);
	DWORD dwBmBitsSize = ((bmpinfo.bmWidth * 32 + 31) / 32) * 4 * bmpinfo.bmHeight;
	//位图文件头 14字节  
	BITMAPFILEHEADER bf;
	bf.bfType = 0x4D42;                  //BM  
	bf.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwBmBitsSize;
	bf.bfReserved1 = 0;
	bf.bfReserved2 = 0;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	//位图信息头  
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpinfo.bmWidth;
	bi.biHeight = bmpinfo.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 8;
	bi.biClrImportant = 0;
	//位图数据;  
	char* context = new char[dwBmBitsSize];
	HDC dc = ::GetDC(NULL);
	GetDIBits(dc, bmp, 0, bi.biHeight, context, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	
	//写bmp文件
	CFile file(strBmpPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

	file.Write((char*)&bf, sizeof(BITMAPFILEHEADER));
	file.Write((char*)&bi, sizeof(BITMAPINFOHEADER));
	file.Write(context, dwBmBitsSize);
	file.Close();
	delete context;
	::ReleaseDC(m_hWnd, dc);
}

void CSumengine2View::autoIndentifyORC(T_CodeData_IN &orcData, const CStdString &strImgPath)
{
	DWORD      nRecognizeResult;  //识别验证码返回的结果值
	CString    strResult;         //识别出来的验证码

	m_ocrData.nCodeType = evtAutoRegonize; 
	//nRecognizeResult = g_orcManmade.GetWeRecognize(m_strOCRParam, strImgPath, strResult);
	nRecognizeResult = g_orcManmade.GetWeRecognize(m_strCurRegOrP, strImgPath, strResult);
	m_ocrData.strAutoResult = strResult;
	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("自动识别:%s  识别结果：%s, 返回值：%ld"), CString(strImgPath), m_ocrData.strAutoResult, nRecognizeResult);
}

DWORD WINAPI CSumengine2View::ThreadPicResult(LPVOID lpParameter)
{
	T_INDETIFY_ORC_PARAM* lparam = (T_INDETIFY_ORC_PARAM*)lpParameter;
	lparam->nRecognizeResult = g_orcManmade.GetRecognize(lparam->strImgPath, lparam->strCode);

	return 1;
}


void CSumengine2View::ActionGetOnceKeyMove()
{
	CStdString strImgURL   = _T("");
	CStdString strFileName = _T("");
	CStdString strPath     = _T("");
	CComQIPtr<IHTMLElement> QiElem;

	CStdString strSymbol;
	std::map<CStdString, CStdString> vMapAttrVar;
	FetchMapOfIIDKs(m_objStepData.GetSymbol(), vMapAttrVar);
	map<CStdString, CStdString>::iterator iter = vMapAttrVar.begin();
	if(iter != vMapAttrVar.end())
	{
		strSymbol = iter->first;
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("没有通配符"));
		//SetSetpOutData(strSymbol, _T("没用找到需要的元素"));      //保存提取结果，注意是写到与属性关联的目标变量中
		STEP_FINISH(srFinishedFail);
		return;
	}

	//下载的URL
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		strImgURL = locate.GetAttributeValue(QiElem, _T("src"));
		if(_T("") == strImgURL)
		{
			strImgURL = locate.GetAttributeValue(QiElem, _T("herf"));
		}
		if(_T("") == strImgURL)
		{
			m_log.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("提起目标的路径为空"));
			SetSetpOutData(strSymbol, _T("提起目标的路径为空"));      //保存提取结果，注意是写到与属性关联的目标变量中
			STEP_FINISH(srFinishedFail);
			return;
		}
	}
	else
	{
		m_log.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("没用找到需要的元素"));
		SetSetpOutData(strSymbol, _T("没用找到需要的元素"));      //保存提取结果，注意是写到与属性关联的目标变量中
		STEP_FINISH(srFinishedFail);
		return;
	}
	//下载的目的路径
	int nPos = strImgURL.find_last_of('/');
	if(-1 == nPos)
	{
		strFileName = strImgURL;
	}
	else
	{
		strFileName = strImgURL.Mid(nPos + 1);
	}

	nPos = strFileName.find_first_of('?');//去掉文件名中后面带有？号的
	if (-1 != nPos)
	{
		strFileName = strFileName.Left(nPos);
	}
	TCHAR strTempPath[MAX_PATH];//%temp%的路径
	::GetTempPathW(MAX_PATH, strTempPath);
	strPath.Format(_T("%s%s_%s"), strTempPath, GetUniqueIdBaseOnTask(), strFileName);
	//下载
	DownloadOcrPic(strImgURL, strPath);

	
	SetSetpOutData(strSymbol, strPath);      //保存提取结果，注意是写到与属性关联的目标变量中
	STEP_FINISH(eRes);
}

bool CSumengine2View::ClickSaveElement()//对保存的元素进行点击，这样可以更换网页上需要点击的验证码
{
	bool bIsSucc = false;
	//在这里增加根据m_objItem找到点击的对象点击
	T_ITEM* pItem = m_pBrforeStep->GetItem();
	CHtmElementLocate locate;
	QI_ELEMENT_COLLECTION elemCollection;
	int iTotalElems;

	if(pItem->mapAttributes.size() == 0)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("ClickSaveElement------mapAttributes.size() == 0！"));
		return false;
	}
	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	if (!locate.FromLocateInfo(pItem, elemCollection, iTotalElems))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("ClickSaveElement------FromLocateInfo失败！"));
		return false;
	}
	if (elemCollection.size() == 0)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("elemCollection.size() == 0"));
		return false;
	}
	m_targetElement = elemCollection[0];
	ElemClick(m_targetElement);
	return true;
}

void CSumengine2View::InitCodeStyleFlag()//block执行前，初始化上面3个变量为false
{
	m_bAutoReg = false;     //当前验证码识别步骤是否执行过自动识别
	m_bLongRange = false;   //当前验证码识别步骤是否执行过远程打码
	m_bManmade = false;     //当前验证码识别步骤是否执行过人工打码
}

bool CSumengine2View::GetAutoReg()   //设置自动识别标志true
{
	return m_bAutoReg; 
}

bool CSumengine2View::GetLongRange() //设置远程打码标志true
{
	return m_bLongRange;
}

bool CSumengine2View::GetManmade()   //设置人工输入标志true
{
	return m_bManmade;
}

E_ORC_Indentify_Type CSumengine2View::GetCodeOIT()//获取当前步骤使用的打码方式
{
	return m_eCodeOIT;
}

E_VERIFY_TYPE CSumengine2View::GetVerifyCodeType()//获取验证码类型
{
	return m_eVerifyCodeType;
}

void CSumengine2View::SetOcrParamFillRP(CStdString strParamOcr)//主控传过来的验证码参数填充m_vecOcrRegister，m_vecOcrPublish，每个View创建之初就设置，整个推广完成都不会改变
{
	if (strParamOcr == _T(""))
	{
		return;
	}

	//格式：验证码类型<@@>识别参数<@@>验证码地址<@@>验证码输入提示  （参数可能包含动参）
	std::vector<CStdString> vResData;
	CStdStrUtils StringUtil;
	DWORD dwCount = StringUtil.SplitString(strParamOcr, _T("#"), vResData, true);

	if (dwCount > 1 || dwCount < 1)
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("strParamOcr = %s"), (CString)strParamOcr);
	}

	if (dwCount >= 1)
	{
		m_vecOcrRegister.clear();
		int nReg = StringUtil.SplitString(vResData[0], _T("@"), m_vecOcrRegister, true);

		m_vecOcrPublish.clear();
		StringUtil.SplitString(vResData[1], _T("@"), m_vecOcrPublish, true);
	}
}

//主动获取验证码弹框返回的确认结果
void CSumengine2View::GetCodeResult()
{
	T_CodeData_Back tCodeData;
	bool bGet = g_ocrInput.GetResult((CString)m_strTaskID, tCodeData);

	if (!bGet)
	{
		return;
	}

	if (tCodeData.strNsCode != m_strTaskID)
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("收到非请求网站的验证码！"));
		return;
	}

	KillTimer(TIMER_GET_CODE_RESULT);

	if (tCodeData.bIsRefresh)
	{	
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("刷新验证码！路径：%s"), CString(tCodeData.strPicPath));
		//刷新验证码
		if (NULL != m_pBrforeStep)//如果指针有值，说明有要点击的验证码，刷新验证码需要点击的情况
		{
			if (ClickSaveElement())
			{
				ActionIdentify(false, tCodeData.strPicPath, true);
			}
			else
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("点击验证码来刷新的步骤失败！---会导致验证码对话框时间计时器不变化!"));
			}
			return;
		}
		//刷新验证码不需要点击的情况
		ActionIdentify(false, tCodeData.strPicPath, false);
	}
	else
	{
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("获取到验证码识别结果Result:%s"), tCodeData.strResult);

		m_strRetImgCode = tCodeData.strResult;

		if (tCodeData.strResult.IsEmpty())
		{	
			SetStepErrDesc(_T("验证码识别结果为空!"));
			STEP_FINISH(srFinishedFail);
		}
		else
		{
			STEP_FINISH(srFinishedOK);;
		}
	}

}

bool CSumengine2View::FillElement(CComQIPtr<IHTMLElement> pElem, CStdString strSymbol, int iFillMethod)
{
	HRESULT hr = 0;
	long lngCnt = 0;
	long lngSelIndex = 0;
	long itemLength = 0;
	bool bHaveValue = false;

	CHtmElementLocate locate;
	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	CStdString strTagName = locate.GetTagName(pElem);

	//替换识别结果
	if (strSymbol == _T("#imgcap#"))
	{
		strSymbol = m_strRetImgCode;
	}

	if (!strTagName.CompareNoCase(_T("input")))
	{
		CComQIPtr<IHTMLInputElement> QiElem = pElem;
		if (NULL == QiElem)
		{
			g_msgbox.Show(_T("错误"), _T("IHTMLInputElement指针无效，赋值失败！"));
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLInputElement指针无效，赋值失败！"));
			return false;
		}
		if (!strSymbol.CompareNoCase(_T("#random#")))
		{
			strSymbol.Format(_T("%d"), rand()); 
		}

		CStdString strType = locate.GetInputElementType(QiElem);

		QiElem->select();      //与IHTMLElement的Click配合获取输入焦点

		if (!strType.CompareNoCase(_T("text")) || 
			!strType.CompareNoCase(_T("password")))
		{
			int iRetry = 5;
			CStdString strFilledVal = _T("");
 			do 
 			{	
				if (iFillMethod == 0 || strSymbol.IsEmpty())
				{
					QiElem->put_value(strSymbol.GetBuffer());
				}
				else
				{
					CString strContent = _T("");
					
					for (int iindex = 0; iindex < strSymbol.GetLength(); ++iindex)
					{		
						strContent += strSymbol[iindex];

						FireEvent(pElem, _T("OnKeyDown"));
						QiElem->put_value(strContent.GetBuffer());
						FireEvent(pElem, _T("OnKeyUp"));
					}
				}

				
				//QiElem->put_alt(strSymbol.GetBuffer());      //设置替换文本
				//检查是否成功，不成功则重试
 				_bstr_t bstrVal;
 				QiElem->get_value(&bstrVal.GetBSTR());
 				if (!CompareBSTR(bstrVal, strSymbol.GetBuffer()))
 					break;

				WaitWithUIMsgDisp(500);
 
 			} while (iRetry-- > 0);
		}

		else if (!strType.CompareNoCase(_T("radio")) || 
			!strType.CompareNoCase(_T("checkbox")))
		{
			QiElem->put_checked(true);
			pElem->click();  //通过IHTMLElement触发Click事件
		}
		else if (!strType.CompareNoCase(_T("file")))
		{	
			if (iFillMethod == 0 || strSymbol.IsEmpty())
			{
				QiElem->put_value(strSymbol.GetBuffer());
			}
			else
			{
				CString strContent = _T("");

				for (int iindex = 0; iindex < strSymbol.GetLength(); ++iindex)
				{
					strContent += strSymbol[iindex];

					FireEvent(pElem, _T("OnKeyDown"));
					QiElem->put_value(strContent.GetBuffer());
					FireEvent(pElem, _T("OnKeyUp"));
				}
			}
		}
		else
		{
			if (iFillMethod == 0 || strSymbol.IsEmpty())
			{
				QiElem->put_value(strSymbol.GetBuffer());
			}
			else
			{
				CString strContent = _T("");

				for (int iindex = 0; iindex < strSymbol.GetLength(); ++iindex)
				{
					strContent += strSymbol[iindex];

					FireEvent(pElem, _T("OnKeyDown"));
					QiElem->put_value(strContent.GetBuffer());
					FireEvent(pElem, _T("OnKeyUp"));
				}
			}
		}

		return true;
	}
	else if (!strTagName.CompareNoCase(_T("textarea")) ||
		!strTagName.CompareNoCase(_T("body")) ||
		!strTagName.CompareNoCase(_T("div")) ||
		!strTagName.CompareNoCase(_T("span")) ||
		!strTagName.CompareNoCase(_T("table")) ||
		!strTagName.CompareNoCase(_T("td")) ||
		!strTagName.CompareNoCase(_T("tr")) )
	{
		pElem->put_innerText(strSymbol.GetBuffer());
		return true;
	}
	else if ( !strTagName.CompareNoCase(_T("p")) )
	{
		pElem->put_outerText(strSymbol.GetBuffer()) ;
		return true ;
	}
	else if (!strTagName.CompareNoCase(_T("html")))
	{
		//什么情况下会用？...

	}
	else if (!strTagName.CompareNoCase(_T("select")))
	{
		if (strSymbol == _T(""))
		{
			//strSymbol = _T("%random%");
			strSymbol = _T("#random#");
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("下拉选项值传入为空!默认赋为%random%！"));
		}
		
		FireEvent(pElem, _T("OnClick"));

		CComQIPtr<IHTMLSelectElement> QiSelectElem = pElem;
		if (NULL == QiSelectElem)
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取IHTMLSelectElement接口失败！"));
			return false;
		}

		itemLength = locate.GetSelectElementLen(QiSelectElem);
		if (itemLength == 0 || (itemLength == 1 && GetSelectObjVal(QiSelectElem, 0).Find(_T("选择")) != -1))
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("检测到0个列表项，选择失败！"));
			return false;
		}

		//if (!strSymbol.CompareNoCase(_T("%random%")))
		if (!strSymbol.CompareNoCase(_T("#random#")))
		{
			if (itemLength == 1)
				lngSelIndex = 0;
			else if (itemLength == 2)
				lngSelIndex = 1;
			else if (itemLength > 2)
			{
				//避开项为空的情况发生
				do 
				{
					lngSelIndex = rand() % itemLength + 1;

					//避免越界
					if (lngSelIndex == itemLength)
						lngSelIndex--;
					CStdString strVal = GetSelectObjVal(QiSelectElem, lngSelIndex);
					if (_T("") != strVal)
					{
						m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("为select随机选取到%s，成功避开空项！"), CString(strVal));
						break;
					}
					else
						m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("为select随机选取时发现空项！"), CString(strVal));

					if (lngCnt++ > 10)
						break;

				} while (true);
			}

			//避开不让选第一项
			if (itemLength > 1 && lngSelIndex == 0)
			{
				QiSelectElem->put_selectedIndex(1);
			}
			else
			{
				QiSelectElem->put_selectedIndex(lngSelIndex);
			}

			bHaveValue = true;
		}
		else
		{
			//1.先试下用Value属性来相等匹配下拉项
			for (int i = 0; i < itemLength; i++)
			{
				if (GetSelectObjVal(QiSelectElem, i) == strSymbol)
				{
					QiSelectElem->put_selectedIndex(i);
					bHaveValue = true;
					break;
				}
			}

			//2.试下用Text属性来相等匹配下拉项
			if (!bHaveValue)
			{
				for (int i = 0; i < itemLength; i++)
				{
					if (-1 != GetSelectObjText(QiSelectElem, i).Find(strSymbol))
					{
						QiSelectElem->put_selectedIndex(i);
						bHaveValue = true;
						break;
					}
				}
			}

			//5.还是找不到用随机选
			if (!bHaveValue)
			{
				if (FillElement(pElem, _T("#random#")), m_objStepData.GetFillType())
				{
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("为select随机选取时找不到%s,转为随机选取！"), CString(strSymbol));
					return true;
				}
			}
		}

		if (bHaveValue)
		{
			//触发Onchnage事件
			FireEvent(pElem, _T("OnChange"));
			FireEvent(pElem, _T("OnBlur"));
			return true;
		}
	}
	else if (!strTagName.CompareNoCase(_T("option")) ||
		!strTagName.CompareNoCase(_T("form")))
	{

	}
	else if (!strTagName.CompareNoCase(_T("a")) ||
		!strTagName.CompareNoCase(_T("area")) ||
		!strTagName.CompareNoCase(_T("base")) ||
		!strTagName.CompareNoCase(_T("button")) ||
		!strTagName.CompareNoCase(_T("canvas")) ||
		!strTagName.CompareNoCase(_T("frame")))
	{

	}
	else if (!strTagName.CompareNoCase(_T("iframe")) ||
		!strTagName.CompareNoCase(_T("img")) ||
		!strTagName.CompareNoCase(_T("link")) ||
		!strTagName.CompareNoCase(_T("meta")) ||
		!strTagName.CompareNoCase(_T("object")))
	{

	}
}

//获取select元素的值
CStdString CSumengine2View::GetSelectObjVal(CComQIPtr<IHTMLSelectElement> pSelect, int nIndex)
{
	_variant_t vIndex = nIndex;
	CComDispatchDriver disp;
	CHtmElementLocate locate;

	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

	HRESULT hr = pSelect->item(vIndex, vIndex, &disp);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IHTMLOptionElement> QiOptElem;
		if (SUCCEEDED(disp.QueryInterface(&QiOptElem)))
		{
			return locate.GetOptionElementVal(QiOptElem);
		}
	}

	return _T("");
}

//获取select元素的Outtext
CStdString CSumengine2View::GetSelectObjText(CComQIPtr<IHTMLSelectElement> pSelect, int nIndex)
{
	_variant_t vIndex = nIndex;
	CComDispatchDriver disp;
	CHtmElementLocate locate;

	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

	HRESULT hr = pSelect->item(vIndex, vIndex, &disp);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IHTMLOptionElement> QiOptElem;
		if (SUCCEEDED(disp.QueryInterface(&QiOptElem)))
		{
			return locate.GetOptionElementText(QiOptElem);
		}
	}

	return _T("");
}

//从字符串中分离出动参及其对应的属性
//strSymbol格式：%目标变量1%:属性,%目标变量2%:属性,....%目标变量n%:属性
void CSumengine2View::FetchMapOfIIDKs(CStdString strSymbol, std::map<CStdString, CStdString>&mapIIDK)
{
	mapIIDK.clear();

	if (strSymbol.IsEmpty())
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("动参字符串为空，提取失败！"));
		return;
	}

	std::vector<CStdString> vResData;
	CStdStrUtils StringUtil;
	StringUtil.SplitStringEx(strSymbol, _T(","), vResData, true);
	for (int i = 0; i < vResData.size(); i++)
	{
		CStdString strTmp = vResData[i];
		int iPos = strTmp.Find(_T(":"));
		if (-1 != iPos && iPos < strTmp.GetLength() - 1)
		{
			CStdString strKey = strTmp.Left(iPos);
			CStdString strValue = strTmp.Right(strTmp.GetLength() - iPos - 1);
			mapIIDK[strKey] = strValue;
		}
	}
}

void CSumengine2View::GetIdentifyParam(CStdString strSymbol, E_VERIFY_TYPE &vtype, CStdString &strOCRParam, 
										CStdString &strURL, CStdString &strPromptInfo, CStdString &strRegOrP, CStdString& strShowCodeHeight)
{
	//格式：验证码类型<@@>识别参数<@@>验证码地址<@@>验证码输入提示  （参数可能包含动参）
	std::vector<CStdString> vResData;
	CStdStrUtils StringUtil;
	DWORD dwCount = StringUtil.SplitString(strSymbol, _T("<@@>"), vResData, true);

	vtype = evtNULL;
	strOCRParam = _T("");

	if (vResData.size() >= 2)
	{
		vtype = (E_VERIFY_TYPE)_ttoi(vResData[0].GetBuffer());
		strOCRParam = vResData[1];
		ReplaceIIDKs(strOCRParam);   //动参替换
	}

	if (vResData.size() >= 3)
	{
		strURL = vResData[2];   //识别码地址
		ReplaceIIDKs(strURL);   //动参替换
	}

	//验证码输入提示
	if (vResData.size() >= 4)
	{
		strPromptInfo = vResData[3];
	}

	//自动识别参数
	if (vResData.size() >= 5)
	{
		strRegOrP = vResData[4];
	}

	//点击/拖动验证码显示高度
	if (vResData.size() >= 6)
	{
		strShowCodeHeight = vResData[5];
	}
}

//获取验证码的URL
bool CSumengine2View::GetVerifyCodeImgSrc(CStdString &strImgURL)
{
	strImgURL = _T("");

	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		strImgURL = locate.GetAttributeValue(QiElem, _T("src"));
		return true;
	}

	return false;
}

//获取文字类型验证码的Outtext
bool CSumengine2View::GetVerifyCodeOuttext(CStdString &strText)
{
	strText = _T("");
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		strText = locate.GetAttributeValue(QiElem, _T("outtext"));
		m_strOldVerifyCode = strText;
		return true;
	}

	return false;
}

//获取页面上的验证码图片并保存到指定文件
bool CSumengine2View::GetOriginVerifyCodeFromPage(CStdString strPath/*, bool bCopy*/)
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	HRESULT hr = S_FALSE;
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		CComQIPtr<IHTMLElement> QiElemBody;
		CComQIPtr<IHTMLElement2> QiElemBody2;
		CComQIPtr<IHTMLDocument2> QiDoc2;// = (IHTMLDocument2*)GetHtmlDocument();
		QiDoc2.Attach((IHTMLDocument2*)GetHtmlDocument());
		if(!QiDoc2)
		{
			return false;
		}

		//得到Body的 IHTMLElement2接口
		QiDoc2->get_body(&QiElemBody);
		hr = QiElemBody.QueryInterface(&QiElemBody2);

		if (SUCCEEDED(hr) && QiElemBody2)
		{
			//为非文本对象建立控制排版集合
			CComDispatchDriver disp;
			hr = QiElemBody2->createControlRange(&disp);
			if (SUCCEEDED(hr) && disp)
			{
				//得到IHTMLControlRange接口
				CComQIPtr<IHTMLControlRange> QiCtrlRange;
				hr = disp->QueryInterface(&QiCtrlRange);

				if (SUCCEEDED(hr) && QiCtrlRange)
				{
					//得到 IHTMLControlElement接口
					CComQIPtr<IHTMLControlElement> QiCtrlElem;
					hr = QiElem.QueryInterface(&QiCtrlElem);

					if (SUCCEEDED(hr) && QiCtrlElem)
					{
						//将验证码图片copy到剪帖板
						_bstr_t bstrCommand = _T("Copy");
						VARIANT_BOOL vbReturn;
						VARIANT vEmpty;
						VariantInit(&vEmpty);

						//恢复原有用户剪贴板数据
						CClipboardBackup UserDataObj;

						hr = QiCtrlRange->add(QiCtrlElem);
						hr = QiCtrlRange->execCommand(bstrCommand, VARIANT_FALSE, vEmpty, &vbReturn);

						if (SUCCEEDED(hr) && SaveClipboardPicture(strPath))
						{	
							return true;
						}
					}
				}
			}
		}	
	}
	
	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, _T("保存页面验证码失败！hr=%d"), hr);

	return false;
}

//从剪贴板获取图片并保存到指定文件
bool CSumengine2View::SaveClipboardPicture(CStdString strPath)
{
	if(OpenClipboard())
	{
		//获得剪贴板数据
		HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);
		CBitmap *pBitmap = CBitmap::FromHandle(handle);

		if (pBitmap && pBitmap->m_hObject)
		{
			//比较验证码是否是相同的验证码，如果相同就再取，如果不同就刷新
			DWORD numTmp = pBitmap->GetBitmapBits(0, NULL);
			unsigned char *pBits = new unsigned char[numTmp];
			pBitmap->GetBitmapBits(numTmp, (LPVOID)pBits);
			if(m_pBits == NULL)
			{
				m_pBits = pBits;
			}
			else
			{
				delete m_pBits;
				m_pBits = pBits;
			}

			CImage imgTemp;
			imgTemp.Attach(pBitmap->operator HBITMAP());
			imgTemp.Save(strPath.GetBuffer(), Gdiplus::ImageFormatJPEG);
			EmptyClipboard();//每次清空，避免下次检查的时候碰到旧的验证码
			CloseClipboard();

			//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__, _T("保存页面验证码成功！path:%s"), CString(strPath));
			return true;
		}

		EmptyClipboard();
		CloseClipboard();
	}
	else
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("打开粘贴板失败!ErrorCode:%d"), CString(strPath), GetLastError());
	}

	return false;
}

//对元素操作的再次封装（记录或提示操作失败信息）
bool CSumengine2View::ElemClick(CComQIPtr<IHTMLElement> &qiElem)
{
	HRESULT hr = S_FALSE;
	try
	{
		hr = qiElem->click();
		
	}
	catch (...)
	{
		g_msgbox.Show(_T("捕获异常"), _T("qiElem->click()异常"));
		//ExitProcess(0);
	}

	if (FAILED(hr))
	{
		CStdString strMsg = _T("");		
		strMsg.Format(_T("执行click[%s]失败！hr=%08x"), CString(m_objStepData.GetDescrition()), hr);
		g_msgbox.Show(_T("错误"), strMsg);
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, CString(strMsg));
		return false;
	}

	return true;
}

bool CSumengine2View::ElemFocus(CComQIPtr<IHTMLElement2> &qiElem)
{
	HRESULT hr = qiElem->focus();
	if (FAILED(hr))
	{
		CStdString strMsg = _T("");		
		strMsg.Format(_T("执行focus[%s]失败！hr=%08x"), CString(m_objStepData.GetDescrition()), hr);
		//g_msgbox.Show(_T("错误"), strMsg);//对于鼠标放在上面的操作，最好不要弹出这个提示，在日志里面有就可以了
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, CString(strMsg));
		return false;
	}

	return true;
}

bool CSumengine2View::ElemBlur(CComQIPtr<IHTMLElement2> &qiElem)
{
	HRESULT hr = qiElem->blur();
	if (FAILED(hr))
	{
		CStdString strMsg = _T("");		
		strMsg.Format(_T("执行blur[%s]失败！hr=%08x"), CString(m_objStepData.GetDescrition()), hr);
		g_msgbox.Show(_T("错误"), strMsg);
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, CString(strMsg));
		return false;
	}

	return true;
}

bool CSumengine2View::FireEvent(CComQIPtr<IHTMLElement> &qiElem, _bstr_t bstrEventName)
{
	CHtmElementLocate locate;
	locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	
	HRESULT hr = locate.FireEvent(qiElem, bstrEventName);
	if (FAILED(hr))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, _T("触发事件失败：%s"), CString(bstrEventName.GetBSTR()));
		return false;
	}

	return true;
}

//从动参-属性列表中取出与指定属性对应的动参名称
bool CSumengine2View::GetIIDK(std::map<CStdString, CStdString> &mapIIDK, CStdString strAttr, CStdString &strIIDK)
{
	std::map<CStdString, CStdString>::iterator itIIDK = mapIIDK.begin();
	for (; itIIDK != mapIIDK.end(); ++itIIDK)
	{
		if (itIIDK->second == strAttr)
		{
			strIIDK = itIIDK->first;
			return true;
		}
	}

	return false;
}

//获取来自当前网页的所有弹框消息
void CSumengine2View::GetPopupText(std::vector<CStdString> &vText)
{
	vText.clear();

	CStdString strTotalPopInfo = m_strUIMsg;

	std::vector<CStdString> vInfoBetweenSteps;
	CStdStrUtils StringUtil;
	StringUtil.SplitStringEx(strTotalPopInfo, DELIMITER_POPUPINFO_BETWEEN_STEPS, vInfoBetweenSteps, true);

	for (size_t i = 0; i < vInfoBetweenSteps.size(); i++)
	{
		std::vector<CStdString> vInfoOfOneStep;
		StringUtil.SplitStringEx(vInfoBetweenSteps[i], DELIMITER_POPUPINFO_FOR_ONE_STEP, vInfoOfOneStep, true);

		for (size_t j = 0; j < vInfoOfOneStep.size(); j++)
		{
			if (!vInfoOfOneStep[j].IsEmpty())
			{
				vText.push_back(vInfoOfOneStep[j]);
			}	
		}
	}
}

void CSumengine2View::ClearPopupUIMsg()
{
	m_strUIMsg.Empty();
	m_mapStepChkHistory.clear();
	m_bIsStepRetry = false;
}

void CSumengine2View::ReplaceIIDKs(CStdString &strText)
{
	if (NULL != m_pMapIIDKs)
	{
		std::map<CStdString, CStdString>::iterator itIIDK = m_pMapIIDKs->begin();
		for(; itIIDK != m_pMapIIDKs->end(); ++itIIDK)
		{
			strText.Replace(itIIDK->first, itIIDK->second);

			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("动参: %s=%s!"), CString(itIIDK->first), CString(itIIDK->second));
		}
	}
}

//执行数据/动参提取，根据传入的参数不同可提取对象属性、网页弹框等信息
bool CSumengine2View::DoFetchIIDK(CComQIPtr<IHTMLElement> QiElem, CStdString strAttr, CStdString strRule, std::vector<CStdString> &vRes)
{
	bool bGeted = false;
	CHtmElementLocate locate;
	CMyRegEx regEx;
	CStdString strText = _T("");

	if (NULL != QiElem)
	{
		//提取元素属性值
		strText = locate.GetAttributeValue(QiElem, strAttr);   
		if(strText.IsEmpty())
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("strText为空,严重错误，strAttr = %s"), CString(strAttr));
			return false;
		}
		bGeted = regEx.Match(strText, strRule, vRes);
		
		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("InnerHTML:%s, 属性值:%s=%s，提取规则：%s, 提取结果：%s!"), CString(locate.GetOuterHTML(QiElem)), CString(strAttr), CString(strText), CString(strRule), bGeted ? _T("成功") : _T("失败"));
	}
	else if (!strAttr.CompareNoCase(_T("#GetPopupInfo#")))
	//else if (!strAttr.CompareNoCase(_T("%GetPopupInfo%")))
	{
		//提取网页弹框消息
		std::vector<CStdString> vPopupInfo;
		GetPopupText(vPopupInfo);

		for (int j = 0; j < vPopupInfo.size(); j++)
		{
			strText = vPopupInfo[j];
			if(strText.IsEmpty())
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("strText为空,严重错误，strAttr = %s"), CString(strAttr));
				return false;
			}
			bGeted = regEx.Match(strText, strRule, vRes);
			if (bGeted)
				break;
		}				
	}
	else if (!strAttr.Left(7).CompareNoCase(_T("file://")))   //自定义属性需特殊处理
	{
		//下载文件到本地，并返回其文件目录
		//...
	}
	else if (!strAttr.CompareNoCase(_T("_defCurURL")))
	{
		strText = GetLocationURL();
		if(strText.IsEmpty())
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("strText为空,严重错误，strAttr = %s"), CString(strAttr));
			return false;
		}
		bGeted = regEx.Match(strText, strRule, vRes);;
	}

	if (!bGeted)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取数据/动参时匹配错误，input:%s, reg:%s!"), CString(strText), CString(strRule));
		return false;
	}

	return true;
}

//返回URL相对于当前网站的绝对路径
CStdString CSumengine2View::GetAbsoluteURL(CStdString strRelativeURL)
{
	CStdString strHost = GetLocationURL();
	return ComBineUrl(strHost, strRelativeURL);
}

bool CSumengine2View::GetImageCode(CStdString strURL, /*CStdString strIndex, */CStdString strPath)
{
	CStdString strSerImage = GetAbsoluteURL(strURL);

	CImageCode imgCode;
	if (!imgCode.GetImage(strSerImage, strPath))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("下载验证码失败:%s！"), CString(strSerImage));
		return false;
	}

  	if (!imgCode.GetImageType(strPath.GetBuffer()))
  	{
  		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码不是有效图片,URL:%s, filepath:%s！"), CString(strSerImage), CString(strPath));
  		return false;
  	}

	return true;
}

bool CSumengine2View::ChkImageValid(const CStdString &strImage)
{
	CImageCode imgCode;

	if (!imgCode.GetImageType(strImage))
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码不是有效图片,filepath:%s！"), CString(strImage));
		return false;
	}

	//m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码是有效图片"));
	//对图片进行缩放
	StretchImage(strImage, 230, 120);
	//m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("验证码拉伸"));
	return true;
}

bool CSumengine2View::StretchImage(const CStdString &strImage, int iMaxWidth, int iMaxHeight)
{
	CImage  imgOld;
	CImage  imgNew;

	if (SUCCEEDED(imgOld.Load(strImage.c_str())))
	{
		int iHeight = imgOld.GetHeight();
		int iWidth = imgOld.GetWidth();
		
		if (iHeight > iMaxHeight)
			iHeight = iMaxHeight;

		if (iWidth > iMaxWidth)
			iWidth = iMaxWidth;

		imgNew.Create(iWidth, iHeight, 32, 0);
		RECT rc;
		rc.top = 0;
		rc.left = 0;
		rc.right = iWidth;
		rc.bottom = iHeight;

		imgOld.StretchBlt(imgNew.GetDC(), rc);
		imgNew.ReleaseDC();

		if (SUCCEEDED(imgNew.Save(strImage.c_str(), Gdiplus::ImageFormatJPEG)))
		{
			//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("已将图片[%s]大小调整为宽：%d, 长：%d！"), strImage, iWidth, iHeight);
			return true;
		}
	}

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("调整图片大小失败[%s]！"), CString(strImage));
	return false;
}

bool CSumengine2View::StretchWeinXinImage(const CStdString &strImage)
{
	CImage  imgOld;
	CImage  imgNew;

	if (SUCCEEDED(imgOld.Load(strImage.c_str())))
	{	
		//为了适应验证码对话框，固定为130*130
		int iHeight = 130;
		int iWidth = 130;

		imgNew.Create(iWidth, iHeight, 32, 0);
		RECT rc;
		rc.top = 0;
		rc.left = 0;
		rc.right = iWidth;
		rc.bottom = iHeight;

		imgOld.StretchBlt(imgNew.GetDC(), rc);
		imgNew.ReleaseDC();

		if (SUCCEEDED(imgNew.Save(strImage.c_str(), Gdiplus::ImageFormatJPEG)))
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("已将图片[%s]大小调整为宽：%d, 长：%d！"), strImage, iWidth, iHeight);
			return true;
		}
	}

	m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("调整微信图片大小失败[%s]！"), CString(strImage));
	return false;
}
//返回一个以任务为ID为前缀的唯一ID字符串（用作验证码文件名等）
CStdString CSumengine2View::GetUniqueIdBaseOnTask()
{
	CStdString strRes = _T("");
	//取系统日期、时间
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); /*取得当地时间*/
	strRes.Format(_T("%s_%02d%02d%02d"),m_strTaskID ,p->tm_hour,p->tm_min,p->tm_sec);

	//strRes.Format(_T("%s_%d"), m_strTaskID, GetGlobalIndexNum());

	return strRes;
}

//返回一个唯一的验证码文件名路径
CStdString CSumengine2View::GetFileNameForVerifyCode()
{
	CStdString strCodePath = _T("") ;
	strCodePath.Format(_T("%s\\image\\code\\"), GetInstallPath()) ;
	if ( !PathFileExists(strCodePath) )
	{
		SHCreateDirectory(NULL, strCodePath) ;
	}

	CStdString strPath = _T("");
	strPath.Format(_T("%s\\image\\code\\%s.jpg"), GetInstallPath(), GetUniqueIdBaseOnTask());

	return strPath;
}

void CSumengine2View::SetInternetFeature(void)
{
	LRESULT lr = 0;
	INTERNETFEATURELIST featureToEnable = FEATURE_RESTRICT_FILEDOWNLOAD ;

	//禁止下载文件
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_RESTRICT_FILEDOWNLOAD失败,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_RESTRICT_FILEDOWNLOAD失败,err:%d"), GetLastError());
	}

	//禁止网页导航及提交操作时发出声音
	featureToEnable = FEATURE_DISABLE_NAVIGATION_SOUNDS;
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_DISABLE_NAVIGATION_SOUNDS失败,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_DISABLE_NAVIGATION_SOUNDS失败,err:%d"), GetLastError());
	}

}

void CSumengine2View::ActionJumpURL(CStdString strTarURL)
{	
	//正常模式时禁用网页的一些行为
 	if (theApp.GetCurrentRunMode() == rmNormal)
 	{
		SetInternetFeature();
	}
	//非正常模式下显示活动框架的URL
	if (theApp.GetCurrentRunMode() != rmNormal)
	{
		((CMainFrame*)AfxGetMainWnd())->SendMessage(WM_MAINWND_UPDATE_ADDRESS_URL, (WPARAM)strTarURL.GetBuffer());
	}
	//保留原有网址,用来进行强制刷新处理
	m_strOldURL = GetLocationURL();
	
	//新页面打开不论是不是旧的页面不论是否需要刷新
	if (JumpNewPage == m_objStepData.GetJumpType())
	{
		Navigate2(strTarURL.GetBuffer(), navOpenInNewWindow);
		//启动超时检查Timer
		SetTimer(TIMER_PAGE_STATE_TIMEOUT_CHK, g_cfgParam.nPageTimeout, NULL);
	}
	else
	{
		//已是目标页面时，强制刷新
		if (!m_strOldURL.CompareNoCase(strTarURL) || strTarURL == _T(""))
		{
			if (m_objStepData.GetJumpType() == JumpNofrushPage)//打开时不刷新
			{
				STEP_FINISH(srFinishedOK);
			}
			else if (m_objStepData.GetJumpType() == JumpFrushPage)//打开时刷新
			{
				Refresh2(REFRESH_COMPLETELY);
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("已是目标页面%s，刷新！URL:%s"), CString(strTarURL), CString(m_strOldURL));
				SetTimer(TIMER_PAGE_STATE_TIMEOUT_CHK, g_cfgParam.nPageTimeout, NULL);
			}
			else//新页面打开
			{
				Navigate2(strTarURL.GetBuffer(), navOpenInNewWindow);
				//启动超时检查Timer
				SetTimer(TIMER_PAGE_STATE_TIMEOUT_CHK, g_cfgParam.nPageTimeout, NULL);
			}
		}
		else
		{
			Navigate2(strTarURL.GetBuffer());//在原有页面上更改
			//启动超时检查Timer
			SetTimer(TIMER_PAGE_STATE_TIMEOUT_CHK, g_cfgParam.nPageTimeout, NULL);
		}
	}
}

void CSumengine2View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strURL = _T("");
	BOOL bBusy = FALSE;
	READYSTATE rs = READYSTATE_UNINITIALIZED;

	switch (nIDEvent)
	{
	case TIMER_STEP_EXEC:
		{
			DoAction();
		}
		return;
	case TIMER_PAGE_STATE_TIMEOUT_CHK:
		//页面跳转检查超时
		if (ChkPageLoadState(bBusy, rs))
		{
			KillTimer(TIMER_PAGE_STATE_TIMEOUT_CHK);
			strURL = GetLocationURL();
			//用跳转前后的URL是否相同作为跳转成功的判断
			if (strURL != m_strOldURL || !bBusy)
			{
				STEP_FINISH(srFinishedOK);
			}
			//页面跳转超时
			SetStepErrDesc(_T("网页加载超时!"));
			STEP_FINISH(srTimeOut);
		}
		return;
	case TIMER_PAGE_STATE_TIMEOUT_CHK2:
		//超链接超时检查
		if (ChkPageLoadState(bBusy, rs))
		{
			KillTimer(TIMER_PAGE_STATE_TIMEOUT_CHK2);

			if (m_pRelatedParentView)
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("子视图加载超时，通知父视图步骤返回！"));
				m_pRelatedParentView->PostMessage(WM_VIEW_SUB_VIEW_FINISHED);
				m_pRelatedParentView = NULL;
			}
		}
		return;
	case TIMER_CHECK_POP:
	{
		SelectPicFromWeb();
	}
	return;

	case TIMER_CHECK_DRAGRESULT:
	{	
		if (HandleDragResult())
		{
			KillTimer(TIMER_CHECK_DRAGRESULT);
			KillTimer(TIMER_CHECK_SHOW_TIMEOUT);
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::ShowWindow(AfxGetMainWnd()->m_hWnd, SW_HIDE);
			}
			else
			{
				//最大化内核窗口
				::ShowWindow(AfxGetMainWnd()->m_hWnd, SW_MAXIMIZE);
			}
			STEP_FINISH(srFinishedOK);
		}	
	}
	return;

	//验证码显示超时检测，超时当失败处理不在重试
	case TIMER_CHECK_SHOW_TIMEOUT:
	{
		--m_iShowDragCodeTime;
		//已经到了超时时间
		if (m_iShowDragCodeTime == 0)
		{	
			KillTimer(TIMER_CHECK_SHOW_TIMEOUT);
			KillTimer(TIMER_CHECK_DRAGRESULT);
			
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::ShowWindow(AfxGetMainWnd()->m_hWnd, SW_HIDE);
			}
			else
			{
				//最大化内核窗口
				::ShowWindow(AfxGetMainWnd()->m_hWnd, SW_MAXIMIZE);
			}
			SetStepErrDesc(_T("点击/拖动验证码显示超时!"));
			STEP_FINISH(srFinishedError);
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("拖动/点击型验证码显示超时超时!"));
		}
		else
		{	
			CString strShowTime = _T("");
			strShowTime.Format(_T("%s:剩余%d秒"), m_strShowTitle, m_iShowDragCodeTime);
			((CMainFrame*)AfxGetMainWnd())->SetWindowText(strShowTime);
		}
	}
	return;

	case TIMER_PAGE_STATE_CLICK_CHK:
		//检查“点击”操作引发的页面状态变化：
		//1.在本页提交，需监视本页的状态为READYSTATE_COMPLETE
		//2.新打开了页面，此时停止本Timer（说明：新页面有自身的超时检查并会通过OnNewPageFinished函数返回步骤）
		
		//没有引起新页面的情况
		if (!m_bNewPageOpen)
		{
			if (ChkClickPageState(bBusy, rs))
			{
				KillTimer(TIMER_PAGE_STATE_CLICK_CHK);
				STEP_FINISH(srFinishedOK);
			}
		}
		else
		{
			//新页面有自身的超时检查并会通过OnNewPageFinished函数返回步骤
			KillTimer(TIMER_PAGE_STATE_CLICK_CHK);
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("点击引发了页面跳转，等待其返回!"));
		}
		return;
	case TIMER_GET_CODE_RESULT:
		{
			GetCodeResult() ;	
		}
		break;
	default:
		break;
	}

	CLhpHtmlView::OnTimer(nIDEvent);
}

//检查页面状态，超时次数小于预设且页面状态为BUSY时返回false,否则返回true
bool CSumengine2View::ChkPageLoadState(BOOL &bIsBusy, READYSTATE &rState)
{
	try
	{
		bIsBusy = GetBusy();
	}
	catch (...)
	{
		g_msgbox.Show(_T("出现异常"), _T("GetBusy操作异常!"));
	}

	try
	{
		rState = GetReadyState();
	}
	catch (...)
	{
		g_msgbox.Show(_T("出现异常"), _T("GetReadyState操作异常!"));
	}

	//CString strSource = _T("");
	//GetSource(strSource);
	//页面跳转时
	if (((++m_nTimeoutRetryedTimes) <= g_cfgParam.nPageTimeoutRetry) && bIsBusy /*|| READYSTATE_COMPLETE != rState*/)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("页面跳转%d秒超时%d次！busy:%d, readstate:%d"), g_cfgParam.nPageTimeout, m_nTimeoutRetryedTimes, bIsBusy, rState);
		return false;
	}

	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("页面跳转的页面状态满足成功条件! 检查次数：%d, busy:%d, readstate:%d"), m_nTimeoutRetryedTimes, bIsBusy, rState);
	//m_log.Trace(LOGL_TOP,LOGC_ERROR,__TFILE__, __LINE__,_T("source:\r\n%s"), strSource);

	return true;
}

//检查点击操作后页面状态，超时次数小于预设且页面状态为BUSY时返回false,否则返回true
bool CSumengine2View::ChkClickPageState(BOOL &bIsBusy, READYSTATE &rState)
{
	bIsBusy = GetBusy();
	rState = GetReadyState();
	//CString strSource = _T("");
	//GetSource(strSource);

	if (++m_nClickStatCheckedTimes <= g_cfgParam.nClickTimeoutRetry && bIsBusy/* || READYSTATE_COMPLETE != rState*/)     //重试45次 等待时间为90秒
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("点击后等待页面状态，超时%d次! busy:%d, readstate:%d"), m_nClickStatCheckedTimes, bIsBusy, rState);
		return false;
	}

// 	if (strSource.IsEmpty())
// 		MessageBox(_T("source is empty"));

	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("点击操作的页面状态满足成功条件! 检查次数：%d, busy:%d, readstate:%d"), m_nClickStatCheckedTimes, bIsBusy, rState);
	//m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("source:\r\n%s"), strSource);

	return true;
}

void CSumengine2View::ActionKeyPress(CStdString strKeys)
{	
	g_CheckInputLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到影响用户输入焦点网站：%s,模拟键盘步骤！"), CString(m_strNsName));

	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK != eRes)
		STEP_FINISH(eRes);

	while (!WaitEnterFlag())
	{
		WaitWithUIMsgDisp(100);
	}
	//多进程同步互斥
	while (!g_ocrInput.SyncInputFocusWithOcrDlg())
	{
		WaitWithUIMsgDisp(100);
	}
	m_bOwnInputFocus = true;

	
	//切换输入焦点到内核进行，以便进行键盘模拟输入
	HWND hForeWnd = ::GetForegroundWindow();
	ChangeFocusToEngine(hForeWnd);

	//GetParentFrame()->ActivateFrame(SW_MAXIMIZE);
	
	if (QiElem)
	{
		CComQIPtr<IHTMLElement2> QiElem2 = QiElem;
		QiElem2->focus();
	}

	//替换识别结果
	if (strKeys == _T("#imgcap#"))
	//if (strKeys == _T("%imgcap%"))
	{
		strKeys = m_strRetImgCode;
	}

	g_ocrInput.GetSystemInputMutex();
	m_bOwnSystemInput = true;

  	if (!SendKeys(strKeys))
  	{
  		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("发送键盘消息失败!"));
		::SetForegroundWindow(hForeWnd);
		SetStepErrDesc(_T("模拟键盘失败!"));
  		STEP_FINISH(srFinishedFail);
  	}
	WaitWithUIMsgDisp(100);

	::SetForegroundWindow(hForeWnd);

	STEP_FINISH(srFinishedOK);
}

void CSumengine2View::SetShowWindowFlag(bool bVal)
{
	m_flagObjectUpload = bVal;
}

bool CSumengine2View::GetShowWindowFlag(void)
{
	return m_flagObjectUpload;
}

void CSumengine2View::ActionShowWindow(bool bShow)
{
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	int nWidth = rt.right;
	int nHight = rt.bottom;

	if (bShow)
	{
		g_CheckInputLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到可能影响用户输入焦点网站：%s,显示窗口步骤！"), CString(m_strNsName));

		SetShowWindowFlag(true);

		//symbol的两种格式:
		//1.按百分比显示：_T("1,(x%,y%)") 
		//2.按指定宽高在右下角显示：_T("2,(width,height)")
		//是否真正显示窗口,(窗口左上角在屏幕宽度上的百分比,窗口左上角在屏幕高度上的百分比)
		CStdString strSymbol = m_objStepData.GetSymbol();

		//计算窗口左上角的显示位置
		int iLastCommaPos = strSymbol.rfind(_T(","));
		int iWidthX = _ttoi(strSymbol.Mid(3, iLastCommaPos).GetBuf());
		int iHeightY = _ttoi(strSymbol.Mid(iLastCommaPos + 1).GetBuf());

		if (strSymbol.Left(1) == _T("1"))
		{//按百分比显示窗口

			int iLeft = (nWidth * iWidthX) / 100;
			int iTop = (nHight * iHeightY) / 100;
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("显示窗口位置(x:%d%%, y:%d%%),width:%d,height:%d!"), iWidthX, iHeightY, nWidth - iLeft, nHight - iTop);

			if (rmNormal == theApp.GetCurrentRunMode())
			{
				setWndTransparent(AfxGetMainWnd()->m_hWnd,1);
			}

			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, iLeft, iTop, nWidth - iLeft, nHight - iTop, SWP_SHOWWINDOW);
		}
		else if (strSymbol.Left(1) == _T("2"))
		{
			//按高宽在右下角显示窗口
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("在右下角显示窗口大小：width:%d, height:%d!"), iWidthX, iHeightY);

			int iLeft = (nWidth - iWidthX)/2;
			int iTop = (nHight - iHeightY)/2;
			
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				setWndTransparent(AfxGetMainWnd()->m_hWnd,1);
			}

			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, iLeft, iTop, iWidthX, iHeightY, SWP_SHOWWINDOW);
			
			WaitWithUIMsgDisp(500);
		}
		else
		{
			//仅将窗口移到屏幕区域
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				setWndTransparent(AfxGetMainWnd()->m_hWnd,1);
			}
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, (nWidth * iWidthX) / 100, (nHight * iHeightY) / 100, nWidth, nHight, SWP_HIDEWINDOW | SWP_NOACTIVATE);
		}

		//GetAndShowCurrentElement();
	}
	else
	{	
		//采编和测试模式不要隐藏要提示
		if (rmNormal == theApp.GetCurrentRunMode())
		{		
			setWndTransparent(AfxGetMainWnd()->m_hWnd, 1);
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, nWidth, nHight, nWidth, nHight, SWP_HIDEWINDOW | SWP_NOACTIVATE);
			SetShowWindowFlag(false);
		}
		else
		{
			//去掉最顶端显示
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			::MessageBoxW(NULL, _T("采编模式，测试模式内核不会隐藏，便于测试，注意在采编完成以后提交要有隐藏步骤"), _T("温馨提示"), 0);
		}
	}

	

	STEP_FINISH(srFinishedOK);
}

 //将当前step object显示出来，便于做点击操作
bool CSumengine2View::GetAndShowCurrentElement()
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK != eRes)
		return false;

	if (QiElem == NULL)
	{
		if (rmCaibian == theApp.GetCurrentRunMode())
		{
			MessageBox(_T("未指定定位元素！"), _T("提示"));
		}
		return false;
	}

	VARIANT va;
	va.vt = VT_BOOL;
	va.boolVal = FALSE;
	if (SUCCEEDED(QiElem->scrollIntoView(va)))
		return true;

	return false;
}


void CSumengine2View::ClickElement(CComQIPtr<IHTMLElement> qiElem,BOOL bIsDoubleClick)
{	
	g_CheckInputLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到影响用户输入焦点网站：%s,模拟鼠标点击步骤！"), CString(m_strNsName));

	while (!WaitEnterFlag())
	{
		WaitWithUIMsgDisp(100);
	}
	//多进程同步互斥
	while (!g_ocrInput.SyncInputFocusWithOcrDlg())
	{
		WaitWithUIMsgDisp(100);
	}
	m_bOwnInputFocus = true;

	CComQIPtr<IHTMLRect> pRect; 
	CComQIPtr<IHTMLElement2> QiElem2 = qiElem;
	
	//保留输入焦点点击完成之后还原换到之前焦点窗口
	HWND hForeWnd = ::GetForegroundWindow();
	ChangeFocusToEngine(hForeWnd);

	VARIANT va;
	va.vt = VT_BOOL;
	va.boolVal = FALSE;
	qiElem->scrollIntoView(va);

	//记录当前鼠标位置
	GetOrSetCursorPos(true);   
	
	QiElem2->getBoundingClientRect(&pRect);
	RECT rt = {0, 0, 0, 0};
	pRect->get_left(&rt.left);
	pRect->get_right(&rt.right);
	pRect->get_top(&rt.top);
	pRect->get_bottom(&rt.bottom);

	POINT pix;
	if(g_mapObject.find(m_strTaskID) != g_mapObject.end())
	{
		pix.x = g_mapObject[m_strTaskID].x;
		pix.y = g_mapObject[m_strTaskID].y;
	}
	else
	{
		pix.x = rt.left + (rt.right - rt.left) / 4;
		pix.y = rt.top + (rt.bottom - rt.top) / 2;
	}

	//将Object对象坐标转换为屏幕坐标
	ClientToScreen(&pix); 

	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模拟单击准备！"));

	RECT rtUploadWnd;
	::GetWindowRect(AfxGetMainWnd()->m_hWnd, &rtUploadWnd);
	
	if (rmNormal == theApp.GetCurrentRunMode())
	{
		setWndTransparent(AfxGetMainWnd()->m_hWnd, 1);
	}
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, rtUploadWnd.left,rtUploadWnd.top,rtUploadWnd.right-rtUploadWnd.left,rtUploadWnd.bottom-rtUploadWnd.top, SWP_SHOWWINDOW);

	while(::IsWindowVisible(AfxGetMainWnd()->m_hWnd) != TRUE)
	{
		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("Showwindow但窗口未显示出来，循环等待！"));
	}

	g_ocrInput.GetSystemInputMutex();
	m_bOwnSystemInput = true;

	SetCursorPos(pix.x, pix.y);

	if (bIsDoubleClick)
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("模拟双击完成！"));
	}
	else
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);

		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("模拟单击完成！"));
	}

	//非采编模式时恢复鼠标等位置
	if (((CSumengine2App*)AfxGetApp())->GetCurrentRunMode() != rmCaibian)
	{	
		//还原鼠标位置
		GetOrSetCursorPos(false);   
	}

	::SetForegroundWindow(hForeWnd);	
}

void CSumengine2View::ActionUploadFile(CStdString strFile)
{	
	CComQIPtr<IHTMLElement> QiElem;
	BOOL bTmp = PathFileExists(strFile);

	if (!bTmp)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("文件不存在, 网站：%s，path:%s！"), CString(m_strNsName), CString(strFile));
		g_msgbox.Show(_T("上传文件错误"), _T("文件不存在！"));
		SetStepErrDesc(_T("要上传的文件不存在!"));
		//不需要重试
		STEP_FINISH(srUploadFileError);
	}

	BOOL bLastShow = FALSE;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes)
	{
		DWORD dwExitCode = 0xFFFF;

		while (!WaitEnterFlag())
		{
			WaitWithUIMsgDisp(100);
		}
		//多进程同步互斥
		while(!g_ocrInput.SyncInputFocusWithOcrDlg()) 
		{
			WaitWithUIMsgDisp(100);
		}
		m_bOwnInputFocus = true;

		m_tUploadFileParam.pView = this;
		m_tUploadFileParam.hMainFrameWnd = AfxGetMainWnd()->m_hWnd;
		m_tUploadFileParam.strFile = strFile;
		m_tUploadFileParam.strWebName = m_strNsName;	
		m_tUploadFileParam.hUploadFinishSignal = CreateEvent(NULL, FALSE, FALSE, NULL);
		

		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("找到浏览按钮，准备上传图片!, 网站：%s，hMainFrameWnd = %d, viewWnd = %d"), CString(m_strNsName),m_tUploadFileParam.hMainFrameWnd, this->m_hWnd);
		((CSumengine2App*)AfxGetApp())->UploadFile(m_tUploadFileParam);

		if (NULL != QiElem)
		{
			if (OpenObjectUploadCtrl(QiElem))
			{
				m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("打开Object类型图片控件!"));
			}
			else
			{	
				//此处点击普通元素可能会异常，异常直接失败
				if (!ElemClick(QiElem))
				{	
					SetStepErrDesc(_T("点击上传图片元素失败!"));
					STEP_FINISH(srUploadFileError);
				}
			}
		}

		m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("等待上传图片结束!"));
		while (WAIT_OBJECT_0 != WaitForSingleObject(m_tUploadFileParam.hUploadFinishSignal, 0)) 
		{
			WaitWithUIMsgDisp(10);
		}
		
		CloseHandle(m_tUploadFileParam.hUploadFinishSignal);

		//判断上传是否成功
		T_UPLOAD_FILE_RESULT tRes;
		((CSumengine2App*)AfxGetApp())->GetUploadResult(m_strNsName, tRes);
		if(!tRes.bRes)
		{
			m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("上传文件失败!"));

			SetStepErrDesc(_T("上传文件失败:")+ m_tUploadFileParam.strErrorMsg) ;

			//为了避免采编人员忘记采集隐藏步骤，所以此处上传无论成功还是失败，都隐藏掉
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			}
			
			STEP_FINISH(srUploadFileError);
		}
		else
		{
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("上传文件成功!"));
			SetStepErrDesc(_T("上传文件成功！")) ;

			//为了避免采编人员忘记采集隐藏步骤，所以此处上传无论成功还是失败，都隐藏掉
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			}
			STEP_FINISH(srFinishedOK);
		}
	}
	else
	{
		if (rmNormal == theApp.GetCurrentRunMode())
		{
			//在失败之前是显示了内核的，在推广模式下要先隐藏内核
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_BOTTOM, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
		}
		
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("找不到浏览按钮，无法打开文件上传窗口!, 网站：%s"), CString(m_strNsName));

		SetStepErrDesc(_T("找不到浏览按钮，无法打开文件上传窗口!"));
		STEP_FINISH(srUploadFileError);
	}
}

void CSumengine2View::GetOrSetCursorPos(bool bGet)
{
	static POINT ptCursor;

	if (bGet)
	{
		GetCursorPos(&ptCursor);
	}
	else
	{
		SetCursorPos(ptCursor.x, ptCursor.y);
	}
}

bool CSumengine2View::OpenObjectUploadCtrl(CComQIPtr<IHTMLElement> qiElem)
{		
	bool bRes = FALSE;
	HRESULT hr = 0;
	CComPtr<IDispatch> activeXObject;
	CComQIPtr<IHTMLObjectElement>htmlObjectElement(qiElem);
	CComQIPtr<IHTMLEmbedElement>htmlEmbedElement(qiElem);
	CComQIPtr<IHTMLElement> pFrameObj;

	//对object标签和Embed标签类型需要模拟鼠标点击处理
	if (htmlObjectElement != NULL || htmlEmbedElement != NULL)
	{	
		g_CheckInputLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到影响用户输入焦点网站：%s,Object文件上传！"), CString(m_strNsName));

		bool bEmbedInFrame = false;
		if (SUCCEEDED(CheckObjInFrame(qiElem, bEmbedInFrame, pFrameObj)))
		{	
			//保留输入焦点点击完成之后还原换到之前焦点窗口
			HWND hForeWnd = ::GetForegroundWindow();
			ChangeFocusToEngine(hForeWnd);

			//记录当前鼠标位置
			GetOrSetCursorPos(true);

			VARIANT va;
			va.vt = VT_BOOL;
			va.boolVal = TRUE;
			qiElem->scrollIntoView(va);

			CComQIPtr<IHTMLRect> pRect; 
			CComQIPtr<IHTMLElement2> QiElem2 = qiElem;

			QiElem2->getBoundingClientRect(&pRect);
			RECT rt = {0, 0, 0, 0};
			pRect->get_left(&rt.left);
			pRect->get_right(&rt.right);
			pRect->get_top(&rt.top);
			pRect->get_bottom(&rt.bottom);

			POINT pix;
			if (g_mapObject.find(m_strTaskID) != g_mapObject.end())
			{
				pix.x = g_mapObject[m_strTaskID].x;
				pix.y = g_mapObject[m_strTaskID].y;
			}
			else
			{
				pix.x = rt.left + (rt.right - rt.left) / 4;
				pix.y = rt.top + (rt.bottom - rt.top) / 2;

				if (bEmbedInFrame)
				{
					CComQIPtr<IHTMLElement2> qFrame1 = pFrameObj;
					CComQIPtr<IHTMLRect> pRect1;

					qFrame1->getBoundingClientRect(&pRect1);
					RECT rt1 = { 0, 0, 0, 0 };
					pRect1->get_left(&rt1.left);
					pRect1->get_right(&rt1.right);
					pRect1->get_top(&rt1.top);
					pRect1->get_bottom(&rt1.bottom);

					pix.x += rt1.left;
					pix.y += rt1.top;
				}

			}
			//将Object对象坐标转换为屏幕坐标
			ClientToScreen(&pix); 
			//ClientToScreen(&rt); 
			
			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模拟单击准备！"));

			RECT rtUploadWnd;
			::GetWindowRect(AfxGetMainWnd()->m_hWnd, &rtUploadWnd);
			if (rmNormal == theApp.GetCurrentRunMode())
			{
				setWndTransparent(AfxGetMainWnd()->m_hWnd,1);
			}
			::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, rtUploadWnd.left,rtUploadWnd.top,rtUploadWnd.right-rtUploadWnd.left,rtUploadWnd.bottom-rtUploadWnd.top, SWP_SHOWWINDOW);

			while(::IsWindowVisible(AfxGetMainWnd()->m_hWnd) != TRUE)
			{
				m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("Showwindow但窗口未显示出来，循环等待！"));
			}
			
			g_ocrInput.GetSystemInputMutex();
			m_bOwnSystemInput = true;

			SetCursorPos(pix.x, pix.y);		
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP,  0, 0, 0, NULL);

			m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模拟单击完毕！"));

			//非采编模式时恢复鼠标等位置
			if (((CSumengine2App*)AfxGetApp())->GetCurrentRunMode() != rmCaibian)
			{	
				//还原鼠标位置
				GetOrSetCursorPos(false);
			}
			::SetForegroundWindow(hForeWnd);
			return true;
		}
	}

	return false;
}

void CSumengine2View::ActionGetAttribute()
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);

	std::vector<CStdString> vRes;
	CStdString strOutData = _T("");
	CStdString strErrMsg = _T("");
	CStdString strTargetVar = _T("");

	//获取要提取属性与其目标变量的对应关系，属性值保存到目标变量中返回
	std::map<CStdString, CStdString> vMapAttrVar;
	FetchMapOfIIDKs(m_objStepData.GetSymbol(), vMapAttrVar);

	//遍历需要提取的属性
	std::vector<T_STEP_RETURN> vCompare;
	m_objStepData.GetStepReturn(vCompare);

	if (srFinishedOK == eRes)
	{		
		for (int i = 0; i < vCompare.size(); i++)
		{
			CStdString strAttribute = vCompare[i].strAttrib;
			
			if (!GetIIDK(vMapAttrVar, strAttribute, strTargetVar))
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取数据时没有找到与属性%s对应的目标变量！"), CString(strAttribute));
				g_msgbox.Show(_T("错误"), _T("提取数据时没有找到与属性对应的目标变量！"));
				continue;
			}

			if (DoFetchIIDK(QiElem, strAttribute, vCompare[i].strRule, vRes))
			{
				for (int j = 0; j < vRes.size(); j++)
				{
					SetSetpOutData(strTargetVar, vRes[j]);      //保存提取结果，注意是写到与属性关联的目标变量中
				}
			}
			else
			{
				CStdString str = m_objStepData.GetRetErr();
				SetSetpOutData(strTargetVar, m_objStepData.GetRetErr());
			
				if (!strErrMsg.empty())
					strErrMsg +=  _T("/");

				strErrMsg += strAttribute;
			}
		}

		if (!strErrMsg.empty())
		{
			SetStepErrDesc(_T("提取数据失败:") + strErrMsg);
			STEP_FINISH(srFinishedFail);
		}
		else
			STEP_FINISH(srFinishedOK);
	}
	else
	{
		for (int i = 0; i < vCompare.size(); i++)
		{
			CStdString strAttribute = vCompare[i].strAttrib;

			if (!GetIIDK(vMapAttrVar, strAttribute, strTargetVar))
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取数据时没有找到与属性%s对应的目标变量！"), CString(strAttribute));
				g_msgbox.Show(_T("错误"), _T("提取数据时没有找到与属性对应的目标变量！"));
				continue;
			}
			else
			{
				CStdString str = m_objStepData.GetRetErr();
				SetSetpOutData(strTargetVar, m_objStepData.GetRetErr());
			}

			/*if (DoFetchIIDK(QiElem, strAttribute, vCompare[i].strRule, vRes))
			{
				for (int j = 0; j < vRes.size(); j++)
				{
					SetSetpOutData(strTargetVar, vRes[j]);      //保存提取结果，注意是写到与属性关联的目标变量中
				}
			}
			else
			{
				CStdString str = m_objStepData.GetRetErr();
				SetSetpOutData(strTargetVar, m_objStepData.GetRetErr());

				if (!strErrMsg.empty())
					strErrMsg +=  _T("/");

				strErrMsg += strAttribute;
			}*/
		}
	}

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取数据失败！"));
	STEP_FINISH(eRes);
}

void CSumengine2View::ActionGetIIDKs()
{
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes)
	{		
		std::vector<CStdString> vRes;
		CStdString strOutData = _T("");
		CStdString strErrMsg = _T("");
	    CStdString strIIDK = _T("");

		//遍历需要提取的属性
		std::vector<T_STEP_RETURN> vCompare;
		m_objStepData.GetStepReturn(vCompare);

		std::map<CStdString, CStdString> mapIIDK;
		FetchMapOfIIDKs(m_objStepData.GetSymbol(), mapIIDK);         //从symbol中解析出动参-属性列表

		for (int i = 0; i < vCompare.size(); i++)
		{
			CStdString strAttribute = vCompare[i].strAttrib;
			CStdString strRule = vCompare[i].strRule;

			if (!GetIIDK(mapIIDK, strAttribute, strIIDK))
			{
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取动参时没有找到与属性%s对应的动参！"), CString(strAttribute));
				g_msgbox.Show(_T("错误"), _T("提取动参时没有找到与属性对应的目标变量！"));
				continue;
			}


			if (DoFetchIIDK(QiElem, strAttribute, strRule, vRes))
			{
				if (vRes.size() > 1)
				{
					CStdString strMsg = _T("");
					strMsg.Format(_T("提取动参%s时匹配到%d个值！"), strIIDK, vRes.size());
					g_msgbox.Show(_T("提示"), strMsg);
					m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__, CString(strMsg));
				}
				else if (vRes.size() == 1)
				{
					m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__, _T("提取动参成功，%s=%s"), CString(strAttribute), CString(vRes[0]));
				}

				//提取数据到动参列表
				if (m_pMapIIDKs)
					(*m_pMapIIDKs)[strIIDK] = vRes[0];
			}
			else
			{
				if (!strErrMsg.empty())
					strErrMsg +=  _T("/");

				strErrMsg += strAttribute;
			}
		}

		if (!strErrMsg.empty())
		{
			SetStepErrDesc(_T("提取动参失败:") + strErrMsg);
			STEP_FINISH(srFinishedFail);
		}
		else
			STEP_FINISH(srFinishedOK);
	}

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("提取动参失败！"));
	STEP_FINISH(eRes);
}

//判断是不是只有outtext且内容只有NOT里面有值的成功标记，并且集合中不止一个对象
bool CSumengine2View::IsSingleOuttextSuccItem()
{	
	int iTotalElems = m_objQiElemCollection.size();
	CHtmElementLocate locate;
	//locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
	T_ITEM *pItem = m_objStepData.GetItem();
	if (pItem && (pItem->mapAttributes.size() == 1) && (iTotalElems >= 1))
	{
		std::map<CStdString, CStdString>::iterator iter = pItem->mapAttributes.begin();
		if (iter->first == _T("outtext"))
		{
			T_FILTER_STRUCTURE tfilter;
			//根据与或非结构进行属性匹配
			locate.TransStucture(iter->second, tfilter);
			if ((tfilter.straAnd.size() == 0) && (tfilter.straOr.size() == 0) && (tfilter.straNot.size() > 0))
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("成功标记检查成功，定位节点是outtext且仅仅只有not类型，忽略匹配标记，找到元素个数为%d！"), iTotalElems);
				return true;
			}
		}
	}

	

	return false;
}

void CSumengine2View::ActionSuccFeature()
{
	if (IsSingleOuttextSuccItem())
	{
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		T_ITEM *pItem = m_objStepData.GetItem();
		std::map<CStdString, CStdString>::iterator iter = pItem->mapAttributes.begin();
		CStdString strOutText = iter->second;
		CStdString strErrInfo = _T("");
		CStdString strPopupMsg = GetFreshPopupMsg(m_strCurStepID);
		int iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)GetHtmlDocument(), strOutText, strPopupMsg, strErrInfo);
		if (0 == iFindRes)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("在源码中检查标记成功！ %s"), CString(strOutText));
			STEP_FINISH(srFinishedOK);
		}
		else
		{	
			if (!strErrInfo.IsEmpty())
			{
				SetStepErrDesc(strErrInfo);
			}
			else
			{
				SetStepErrDesc(_T("在源码中检查标记失败!"));
			}

			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("在源码中检查标记失败！ %s"), CString(strOutText));
			STEP_FINISH(srFinishedFail);    //直接返回srFinishedError的话，因不会切换视图，将可能导致无法找到新页面中的成功标记
		}
	}

	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes)
	{		
		//整理需要比对的属性
		std::vector<T_STEP_RETURN> vCompare;
		T_ITEM itemChkAttribute;
		m_objStepData.GetStepReturn(vCompare);

		for (int i = 0; i < vCompare.size(); i++)
		{
			itemChkAttribute.mapAttributes[vCompare[i].strAttrib] = vCompare[i].strValue;
		}

		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

		if (NULL != QiElem)
		{//有定位属性的成功标记，定位后分别比对标记属性
			
			//比对成功特征
			if (locate.ItemFeatureFit(QiElem, itemChkAttribute))
			{
				m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("成功标记检查成功，%s！"), CString(m_objStepData.GetDescrition()));
				STEP_FINISH(srFinishedOK);
			}
			else
			{
				CStdString strLogError = _T("");
				CStdString strStepDesc = GetPureObjectDescription() + _T(":");

				//摄取详细错误信息
				std::map<CStdString, CStdString>::iterator itAttr = itemChkAttribute.mapAttributes.begin();
				for (; itAttr != itemChkAttribute.mapAttributes.end(); ++itAttr)
				{
					strStepDesc += locate.GetAttributeValue(QiElem, itAttr->first) + _T("\t");

					//打印日志时记录属性名信息
					CStdString strChkError = _T("");
					strChkError.Format(_T("%s=%s"), itAttr->first, locate.GetAttributeValue(QiElem, itAttr->first));
					strLogError.append(strChkError);
				}

				SetStepErrDesc(strStepDesc);
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("成功标记检查失败：%s，值:%s！"), CString(m_objStepData.GetDescrition()), CString(strLogError));

				STEP_FINISH(srFinishedFail);
			}
		}
		else
		{//无定位属性的成功标记，直接在网页源码及弹框信息中查找outtext属性值
			CStdString strOutText = itemChkAttribute.mapAttributes[_T("outtext")];
			CStdString strErrInfo = _T("");

			CStdString strPopupMsg = GetFreshPopupMsg(m_strCurStepID);
			int iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)GetHtmlDocument(), strOutText, strPopupMsg, strErrInfo);
			//int iFindRes = FindInBodyTxt(strOutText, strPopupMsg, strErrInfo) ;
			if (0 == iFindRes)
			{
				m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("在源码中检查标记成功！ %s"), CString(strOutText));
				STEP_FINISH(srFinishedOK);
			}
			else
			{	
				if (!strErrInfo.IsEmpty())
				{
					SetStepErrDesc(strErrInfo);
				}
				else
				{
					SetStepErrDesc(_T("在源码中检查标记失败!"));
				}
				m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("在源码中检查标记失败！ %s"), CString(strOutText));
				STEP_FINISH(srFinishedFail);    //直接返回srFinishedError的话，因不会切换视图，将可能导致无法找到新页面中的成功标记
			}
		}
	}

	m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("成功标记检查失败！"));
	STEP_FINISH(srFinishedFail);
}

bool CSumengine2View::FitSaveSuccFeature()
{	
	//重新获取一次元素
	m_objStepData = m_objSaveSuccStep;

	int itotals = 0;
	GetTargetElemCollection(itotals);

	if (IsSingleOuttextSuccItem())
	{
		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());
		T_ITEM *pItem = m_objStepData.GetItem();
		std::map<CStdString, CStdString>::iterator iter = pItem->mapAttributes.begin();
		CStdString strOutText = iter->second;
		CStdString strErrInfo = _T("");
		CStdString strPopupMsg = GetFreshPopupMsg(m_strCurStepID);
		int iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)GetHtmlDocument(), strOutText, strPopupMsg, strErrInfo);
		if (0 == iFindRes)
		{
			m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("在源码中检查标记成功！ %s"), CString(strOutText));
			return true;
		}
		else
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("在源码中检查标记失败！ %s"), CString(strOutText));
			return false;    //直接返回srFinishedError的话，因不会切换视图，将可能导致无法找到新页面中的成功标记
		}
	}

	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);
	if (srFinishedOK == eRes)
	{
		//整理需要比对的属性
		std::vector<T_STEP_RETURN> vCompare;
		T_ITEM itemChkAttribute;
		m_objStepData.GetStepReturn(vCompare);

		for (int i = 0; i < vCompare.size(); i++)
		{
			itemChkAttribute.mapAttributes[vCompare[i].strAttrib] = vCompare[i].strValue;
		}

		CHtmElementLocate locate;
		locate.SetHtmlDocument((IHTMLDocument2*)GetHtmlDocument());

		if (NULL != QiElem)
		{//有定位属性的成功标记，定位后分别比对标记属性

			//比对成功特征
			if (locate.ItemFeatureFit(QiElem, itemChkAttribute))
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("成功标记检查成功，%s！"), CString(m_objStepData.GetDescrition()));
				return true;
			}
			else
			{
				CStdString strLogError = _T("");
				CStdString strStepDesc = GetPureObjectDescription() + _T(":");

				//摄取详细错误信息
				std::map<CStdString, CStdString>::iterator itAttr = itemChkAttribute.mapAttributes.begin();
				for (; itAttr != itemChkAttribute.mapAttributes.end(); ++itAttr)
				{
					strStepDesc += locate.GetAttributeValue(QiElem, itAttr->first) + _T("\t");

					//打印日志时记录属性名信息
					CStdString strChkError = _T("");
					strChkError.Format(_T("%s=%s"), itAttr->first, locate.GetAttributeValue(QiElem, itAttr->first));
					strLogError.append(strChkError);
				}
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("成功标记检查失败：%s，值:%s！"), CString(m_objStepData.GetDescrition()), CString(strLogError));

				return false;
			}
		}
		else
		{//无定位属性的成功标记，直接在网页源码及弹框信息中查找outtext属性值
			CStdString strOutText = itemChkAttribute.mapAttributes[_T("outtext")];
			CStdString strErrInfo = _T("");

			CStdString strPopupMsg = GetFreshPopupMsg(m_strCurStepID);
			int iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)GetHtmlDocument(), strOutText, strPopupMsg, strErrInfo);
			//int iFindRes = FindInBodyTxt(strOutText, strPopupMsg, strErrInfo) ;
			if (0 == iFindRes)
			{
				m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("在源码中检查标记成功！ %s"), CString(strOutText));
				return true;
			}
			else
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("在源码中检查标记失败！ %s"), CString(strOutText));
				return false;    //直接返回srFinishedError的话，因不会切换视图，将可能导致无法找到新页面中的成功标记
			}
		}
	}

	m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("成功标记检查失败！"));
	return false;

}
//将文本流加载到视图中
HRESULT CSumengine2View::LoadFromStream(CString strPageHTML, CSumengine2View *pCopyView, _bstr_t &bstrCharset)
{
	HGLOBAL	 hMem;
	LPSTREAM	pStream;
	IPersistStreamInit	*pPersistStream;

	MSHTML::IHTMLDocument2Ptr ptrDoc = pCopyView->GetHtmlDocument();
	if(!ptrDoc)
	{
		return E_FAIL;
	}
#ifdef xiaoqiang
	//add html release
	((MSHTML::IHTMLDocument2Ptr)ptrDoc)->Release();
#endif
	/*if ( CompareBSTRNoCase(bstrCharset, _T("gb2312")) == 0 )
	{
	CStringA strAPageHtml ;
	strAPageHtml = strPageHTML ;
	hMem = ::GlobalAlloc(GPTR, (strAPageHtml.GetLength() + 1) * sizeof(char) );
	lstrcpyA((LPSTR)hMem, strAPageHtml) ;
	}
	else if ( CompareBSTRNoCase(bstrCharset, _T("utf-8")) == 0 )
	{
	hMem = ::GlobalAlloc(GPTR, (strPageHTML.GetLength() + 1) * sizeof(TCHAR) );
	lstrcpy((LPTSTR)hMem, strPageHTML);
	}*/

	CStringA strAPageHtml ;
	strAPageHtml = strPageHTML ;
	hMem = ::GlobalAlloc(GPTR, (strAPageHtml.GetLength() + 1) * sizeof(char) );
	lstrcpyA((LPSTR)hMem, strAPageHtml) ;

	HRESULT hr = ::CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if(FAILED(hr))
	{
		::GlobalFree(hMem);
		hMem = NULL;
		return E_FAIL;
	}
	hr = ptrDoc.QueryInterface(IID_IPersistStreamInit, &pPersistStream);
	if(FAILED(hr))
	{
		::GlobalFree(hMem);
		hMem = NULL;
		return E_FAIL;
	}
	if(pPersistStream == NULL)
	{
		pStream->Release();
		return E_FAIL;
	}
	hr = pPersistStream->InitNew();
	hr = pPersistStream->Load(pStream);

	hr = pPersistStream->Release();
	pStream->Release();	
	//::GlobalFree(hMem);

	return S_OK ;
}

int CSumengine2View::FindInBodyTxt(CStdString strOuttext, CStdString strPopupMsg, CStdString &strErrInfo)
{
	//获取当前页面HTML源码
	CHtmElementLocate locate ;
	CComQIPtr<IHTMLDocument2> pIHTMLDoc2;// = (IHTMLDocument2 *)GetHtmlDocument() ;
	pIHTMLDoc2.Attach((IHTMLDocument2 *)GetHtmlDocument());
	if (!pIHTMLDoc2)
	{
		return 2 ;
	}

	CString strPageHTML = locate.GetPageHTML(pIHTMLDoc2) ;

	_bstr_t bstrCharset ;
	pIHTMLDoc2->get_charset(&bstrCharset.GetBSTR()) ;

	if ( strPageHTML.IsEmpty() )
	{
		strErrInfo = _T("页面为空！") ;
		return 2 ;
	}

	//创建视图拷贝
	CStdString tempStr = this->GetTaskID();
	//MessageBox(_T("创建视图拷贝"));

	CSumengine2View *pCurViewCopy = theApp.NewTaskView(_T("00"), _T("临时视图"), 0, true) ;
	pCurViewCopy->SetIsTempView(TRUE) ;
//	pCurViewCopy->GetParentFrame()->ShowWindow(SW_MINIMIZE) ;  //是否最小化临时视图窗口
	pCurViewCopy->Navigate2(  _T("about:blank") ) ;  //初始化空白页面


 	//激活视图所属的框架
 	//GetParentFrame()->ActivateFrame(SW_MAXIMIZE);
	ForceFrameWndToCurrent(GetParentFrame());

	strPageHTML.Replace(_T("UTF-8"), _T("gb2312")) ;
	strPageHTML.Replace(_T("utf-8"), _T("gb2312")) ;

	HRESULT hr = LoadFromStream(strPageHTML, pCurViewCopy, bstrCharset) ;  //将HTML源码导入拷贝视图
	if ( !SUCCEEDED(hr) )
	{
		strErrInfo = _T("将HTML导入临时视图失败！") ;
		return 2 ;
	}	

	//strPageHTML = locate.GetPageHTML((IHTMLDocument2 *)pCurViewCopy->GetHtmlDocument()) ;
 	//等待navigateComplete事件
 	pCurViewCopy->CreateNavigateEvent() ;
 	do 
 	{
		DispUIMessage();
 		//WaitWithUIMsgDisp(20) ;
 	} while ( WAIT_OBJECT_0 != WaitForSingleObject(pCurViewCopy->m_hNavigateEvent, 0) );
 	//CloseHandle(pCurViewCopy->m_hNavigateEvent);

	//从拷贝视图中去除隐藏标记之后查找成功标记
	CComQIPtr<IHTMLDocument2> pTempDoc1;// = (IHTMLDocument2 *)pCurViewCopy->GetHtmlDocument();
	pTempDoc1.Attach((IHTMLDocument2 *)pCurViewCopy->GetHtmlDocument());
	if (!pTempDoc1)
	{
		strErrInfo = _T("获取临时视图失败！") ;
		return 2 ;
	}
	int iFindRes = locate.FindTextInHtmlSrc(pTempDoc1, strOuttext, strPopupMsg, strErrInfo);

	//关闭临时拷贝视图
	//CFrameWnd *hhh = pCurViewCopy->GetParentFrame();
	pCurViewCopy->GetParentFrame()->SendMessage(WM_CLOSE) ;
	//delete pCurViewCopy;
	//delete pCurViewCopy->GetParentFrame();

	//CWnd *kkk = pCurViewCopy->GetParent();
	//kkk = pCurViewCopy->GetParent()->GetParent();
	//pCurViewCopy->GetParent()->GetParent()->SendMessage(WM_CHILDWND_DELETE);

 	//if ( 0 != iFindRes )
 	//{
 	//	//MessageBox(_T("在临时视图查找失败"));
 
 		//LPDISPATCH pTempDoc = GetHtmlDocument();
 		//if(!pTempDoc)
 		//{
 		//	return 2 ;
 		//}
 		//iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)pTempDoc, strOuttext, strPopupMsg, strErrInfo) ;
 		//pTempDoc->Release();
 		//iFindRes = locate.FindTextInHtmlSrc((IHTMLDocument2*)GetHtmlDocument(), strOuttext, strPopupMsg, strErrInfo) ;
 	//}

	//MessageBox(_T("Navigate2 over"));

	return iFindRes ;
}

void CSumengine2View::CreateNavigateEvent()
{
	if ( !m_hNavigateEvent )
	{
		m_hNavigateEvent = CreateEvent(NULL, TRUE, FALSE, NULL) ;
	}
}

//获取成功标记步骤尚未检查的弹框信息（同一成功标记步骤对每个弹框信息仅检查一次）
CStdString CSumengine2View::GetFreshPopupMsg(CStdString strStepID)
{
	CStdString strRetText = _T("");

	std::map<CStdString, T_POPUP_HISTORY_INFO>::iterator itStepChk = m_mapStepChkHistory.find(strStepID);
	if (itStepChk == m_mapStepChkHistory.end())
	{
		//步骤未检查过弹框信息时返回当前所有的弹框信息
		strRetText = m_strUIMsg;       

		//记录本次使用的弹框信息
		m_mapStepChkHistory[strStepID].strLastAll = m_strUIMsg;
		m_mapStepChkHistory[strStepID].strStepLastUse = strRetText;
	}
	else
	{	
		//非步骤重试
		if (!m_bIsStepRetry)
		{
			//步骤已检查过弹框信息时，返回上次检查之后出现的弹框信息
			strRetText = m_strUIMsg.Mid(itStepChk->second.strLastAll.GetLength());

			//记录本次使用的弹框信息
			m_mapStepChkHistory[strStepID].strLastAll = m_strUIMsg; 
			m_mapStepChkHistory[strStepID].strStepLastUse = strRetText;
		}
		else
		{
			strRetText = m_mapStepChkHistory[strStepID].strStepLastUse;
		}
	}

	return strRetText;
}

BOOL CSumengine2View::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar)
{
	if(rmCaibian == theApp.GetCurrentRunMode())
	{
		if(dispid == DISPID_AMBIENT_DLCONTROL)
		{	
			pvar->vt = VT_I4;     
			pvar->lVal = m_dwAmbientProperty;// | (~DLCTL_BGSOUNDS) | (~DLCTL_VIDEOS);  
			return TRUE;//这样做屏蔽图片导致验证码无效了
		}
		
		//return CLhpHtmlView::OnAmbientProperty(pSite, dispid, pvar);
		//如果返回true能和XP等系统兼容，但是不能前进后退刷新，采编模式只在自己的机子上做，不会出现不兼容的情况，所以这样返回
	}

	// 非浏览模式时根据采编数据设置是否屏图等标记
	if (dispid == DISPID_AMBIENT_DLCONTROL && rmBrowser != theApp.GetCurrentRunMode())     
	{
		pvar->vt = VT_I4;     
		pvar->lVal = m_dwAmbientProperty;   
		//return CLhpHtmlView::OnAmbientProperty(pSite, dispid, pvar);
		return TRUE;  
	}     

	return CLhpHtmlView::OnAmbientProperty(pSite, dispid, pvar);
	//return TRUE;
}

//保存下载的验证码路径
void CSumengine2View::SetOcrPicPath(const CStdString &strImgPath)
{
	m_strImgCodePath = strImgPath;
	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("图片下载路径! %s"), CString(m_strImgCodePath));
}

//下载验证码图片
bool CSumengine2View::DownloadOcrPic(CStdString strURL, CStdString strImg)
{
	HANDLE hThread = NULL;
	DWORD dwExitCode = 0xFFFF;

	T_DOWNLOAD_OCR_PIC_PARAM param;
	param.pView = this;
	param.strURL = strURL;
	param.strImgFile = strImg;

	hThread = CreateThread(NULL, 0, &CSumengine2View::ThreadProcDownloadPic, (LPVOID)&param, 0, NULL);
	if (!hThread)
	{
		m_log.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("创建下载图片线程失败，%d!"), ::GetLastError());
	}

	m_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("准备下载图片：%s!"), CString(strImg));
	while (GetExitCodeThread(hThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
	{
		WaitWithUIMsgDisp(10);
	}

	CloseHandle(hThread);

	return true;
}

DWORD WINAPI CSumengine2View::ThreadProcDownloadPic(LPVOID lpParameter)
{
	T_DOWNLOAD_OCR_PIC_PARAM Param = *(T_DOWNLOAD_OCR_PIC_PARAM*)lpParameter;
	CStdString strPath =_T("");

	//下载验证码
	Param.pView->GetImageCode(Param.strURL, Param.strImgFile);
	return 1;
}

//处理久久信息网选择图片问题
void CSumengine2View::SelectPicFromWeb()
{
	KillTimer(TIMER_CHECK_POP);

	Sleep(2000);

	HWND hMiandlg = NULL;
	HWND hIExplorer = NULL;
	int iindex = 0;

	while (iindex++ < 3)
	{
		hMiandlg = ::FindWindow(_T("Internet Explorer_TridentDlgFrame"), _T("发布图片 - 久久信息网 -- 网页对话框"));

		if (NULL == hMiandlg)
		{
			Sleep(1000);
			continue;
		}
		hIExplorer = ::FindWindowEx(hMiandlg, 0, _T("Internet Explorer_Server"), NULL);
		if (NULL == hIExplorer)
		{
			Sleep(1000);
			continue;
		}
		break;
	}

	if (NULL == hMiandlg || NULL == hIExplorer)
	{
		hMiandlg = ::FindWindow(_T("Internet Explorer_TridentDlgFrame"), _T("发布图片 - 久久信息网 -- 网页对话框"));

		if (NULL != hMiandlg)
		{
			::SendMessage(hMiandlg, WM_CLOSE, 0, 0);
		}
		return;
	}

	//此处去关联document
	HINSTANCE hInst = ::LoadLibrary(_T("OLEACC.DLL"));

	CComPtr<IHTMLDocument2> spDoc;
	CComPtr<IHTMLDocument3> spDoc3;
	CComPtr<IHTMLElement> PElementRadio;
	HRESULT hr = 0;
	CComQIPtr<IHTMLElementCollection> coll;

	LRESULT lRes;

	UINT nMsg = ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
	::SendMessageTimeout(hIExplorer, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD*)&lRes);

	LPFNOBJECTFROMLRESULT pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress(hInst, "ObjectFromLresult");
	if (pfObjectFromLresult != NULL)
	{
		HRESULT hr;
		hr = (*pfObjectFromLresult)(lRes, IID_IHTMLDocument, 0, (void**)&spDoc);

		spDoc3 = spDoc;
		spDoc3->getElementById(_T("pic_radio0"), &PElementRadio);
		PElementRadio->click();

		::SendMessage(hMiandlg, WM_CLOSE, 0, 0);
	}

}

BOOL CSumengine2View::HandleFlashCode(CString strUniqueID,CString& strPath)
{	
	//重新获取一次元素
	int iTotalElems = 0;
	GetTargetElemCollection(iTotalElems);
	
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);

	if (srFinishedOK == eRes && NULL != QiElem)
	{
		BSTR strOutHtml;
		QiElem->get_outerHTML(&strOutHtml);

		CString strTempOutHtml = strOutHtml;

		if (strTempOutHtml.IsEmpty())
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取falsh验证码外部网页源码失败!"));
			return FALSE;
		}

		CString strFlashURl;
		if (!GetFlashURL(strFlashURl, strTempOutHtml))
		{
			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取falshURL失败!OUTHTML:%s"), strTempOutHtml);
			return FALSE;
		}

		//拼接Flash本地html,用来传给验证码框
		CString strFlashURLALL = ComBineUrl(GetLocationURL(), strFlashURl);/*GetLocationURL() + _T("/") + strFlashURl*/;
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("flash 图片全路径为:%s"), strFlashURLALL);

		strPath = comBineFlashHtml(strFlashURLALL, strUniqueID);

		return TRUE;
	}
	else
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("定位flash验证码对象出错!"));
	}

	return FALSE;
}

void CSumengine2View::ChangeFocusToEngine(HWND hForeWnd)
{
	HWND hMainWnd = AfxGetMainWnd()->m_hWnd;
	HWND hWnd = hMainWnd;
	DWORD dwForeID;
	DWORD dwCurID;

	dwCurID = ::GetCurrentThreadId();
	dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
	AttachThreadInput(dwForeID,dwCurID,TRUE);

	::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetForegroundWindow(hWnd);
	AttachThreadInput(dwForeID, dwCurID,FALSE);
}

bool CSumengine2View::WaitEnterFlag()
{
	if (!g_bSyncStepEnter)
	{
		return false;
	}
	//重置步骤激活标记
	m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("当前步骤:%s,获取了步骤进入标记!"), CString(m_objStepData.GetDescrition()));
	m_bIsGetSyncEnterStep = true;
	g_bSyncStepEnter = FALSE;
	return true;
}

void CSumengine2View::ReleaseEnterFlag()
{
	if (!g_bSyncStepEnter && m_bIsGetSyncEnterStep)
	{	
		m_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("当前步骤:%s,释放了步骤进入标记!"), CString(m_objStepData.GetDescrition()));
		g_bSyncStepEnter = TRUE;
		m_bIsGetSyncEnterStep = false;
	}
}

bool CSumengine2View::HandleDragCode(CStdString& strInputTitle, CStdString& strShowCodeHeight)
{	
	//此处为了避免与普通验证码框同时弹出，需要互斥
	while (!g_ocrInput.SyncInputFocusWithOcrDlg())
	{
		WaitWithUIMsgDisp(100);
	}
	m_bOwnInputFocus = true;

	//如用户一键登录网站,不需要改变内核大小及元素
	if (rmBrowser == theApp.GetCurrentRunMode())
	{
		if (HandleDragCodeForBrowser())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	//首先判断是否保留了预成功标记，如果没有则直接失败
	if (m_pSaveSuccStep == NULL)
	{
		SetStepErrDesc(_T("未正确保留预成功标记!"));
		return false;
	}

	//获取目标节点
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);

	//如果找到定位元素则
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		//保留定位元素本身及其所有的包含元素，其他元素则直接隐藏，另外返回元素本身的宽和高
		long dwWidth = 0;
		long dwHeigth = 0;

		if (HideobjectAndGetPos(QiElem, &dwWidth, &dwHeigth, strShowCodeHeight))
		{
			//根据宽高显示出内核
			if (dwHeigth == 0 || dwWidth == 0)
			{
				SetStepErrDesc(_T("识别拖动/点击型验证码失败!无法正确获取元素大小!"));
				return false;
			}
			else
			{
				CString strShowTime = _T("");
				RECT rt;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
				int nWidth = rt.right;
				int nHight = rt.bottom;

				//设置标题显示及超时时间(单位s)
				m_strShowTitle = CString(strInputTitle.GetBuffer());
				m_iShowDragCodeTime = 60;
				strShowTime.Format(_T("%s:剩余%d秒"), m_strShowTitle, m_iShowDragCodeTime);
				((CMainFrame*)AfxGetMainWnd())->SetWindowText(strShowTime);
				GetDocument()->SetTitle(_T(""));

				//移到屏幕中间
				if (rmNormal == theApp.GetCurrentRunMode())
				{
					::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, (nWidth / 2 - dwWidth), (nHight / 2 - dwHeigth), dwWidth + 5, dwHeigth + 15, SWP_SHOWWINDOW);
				}
				else
				{
					::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOP, (nWidth / 2 - dwWidth), (nHight / 2 - dwHeigth), dwWidth + 5, dwHeigth + 15, SWP_SHOWWINDOW);
				}
				
				//保留此时的step定位信息
				if (NULL == m_pCurrentStep)
				{
					m_objCurrentStep = m_objStepData;
					m_pCurrentStep = &m_objCurrentStep;
				}

				//检测结果定时器(定时器时间不能太短,否则界面会很卡)
				SetTimer(TIMER_CHECK_DRAGRESULT,350, NULL);
				//超时定时器
				SetTimer(TIMER_CHECK_SHOW_TIMEOUT, 1000, NULL);

				return true;
			}
		}
		else
		{
			SetStepErrDesc(_T("识别拖动/点击型验证码失败,调整验证码元素失败!"));
			return false;
		}

	}
	else
	{
		SetStepErrDesc(_T("识别拖动/点击型验证码失败,无法定位验证码元素!"));
		return false;
	}
}

//游览模式不需要移动元素及内核位置，也不需要设置定时器
bool CSumengine2View::HandleDragCodeForBrowser()
{
	//首先判断是否保留了预成功标记，如果没有则直接失败
	if (m_pSaveSuccStep == NULL)
	{
		SetStepErrDesc(_T("未正确保留预成功标记!"));
		return false;
	}

	//获取目标节点
	CComQIPtr<IHTMLElement> QiElem;
	E_RESULT eRes = GetTargetElement(QiElem);

	//如果找到定位元素则
	if (srFinishedOK == eRes && NULL != QiElem)
	{
		//检测结果定时器(定时器时间不能太短,否则view会卡)
		SetTimer(TIMER_CHECK_DRAGRESULT, 1000, NULL);
		return true;
	}
	else
	{
		return false;
	}
}

bool CSumengine2View::HandleDragResult()
{
	//如果匹配到了成功标记
	if (FitSaveSuccFeature())
	{
		return true;
	}
	else
	{
		//为了防止预成功标记错误，此处还应该对验证码元素是否存在及是否隐藏进行成功判断
		if (rmNormal == theApp.GetCurrentRunMode() || rmShow == theApp.GetCurrentRunMode())
		{
			m_objStepData = m_objCurrentStep;
			int itotals = 0;
			GetTargetElemCollection(itotals);

			CComQIPtr<IHTMLElement> QiElem;
			E_RESULT eRes = GetTargetElement(QiElem);

			if (srFinishedOK == eRes && NULL != QiElem)
			{
				//获取style dis_play属性
				CComBSTR strStyle;
				HRESULT hr = 0;
				CComQIPtr<IHTMLStyle> pStyle;
				hr = QiElem->get_style(&pStyle);

				if (FAILED(hr) || !pStyle)
				{
					return false;
				}
				else
				{
					pStyle->get_display(&strStyle);
					if (!CString(strStyle).CompareNoCase(_T("none")))
					{
						return true;
					}
					else
					{
						return false;
					}

				}

			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

bool CSumengine2View::HideobjectAndGetPos(CComQIPtr<IHTMLElement>&qiElem, long* dwWidth, long* dwHigth, CStdString& strShowCodeHeight)
{
	CComPtr<IDispatch> pDispath = GetHtmlDocument();
	CComQIPtr<IHTMLDocument2> pDoc2;
	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElement2> pElement2;
	CComQIPtr<IHTMLElement> QiBody;
	CComQIPtr<IHTMLBodyElement> QiBody1;
	HRESULT hret;

	hret = pDispath->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc2);
	if (FAILED(hret))
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("获取htmldocment失败!"));
		return false;
	}

	//获取元素style属性
	CComQIPtr<IHTMLStyle> QiStyleObj;
	qiElem->get_style(&QiStyleObj);
	VARIANT valstyle;
	valstyle.vt = VT_BSTR;

	if (!strShowCodeHeight.IsEmpty() && (0x30 < strShowCodeHeight[0] && strShowCodeHeight[0] < 0x39))
	{
		//重设元素显示高度
		CString strHeight = _T("");
		strHeight.Format(_T("%spx"), strShowCodeHeight.GetBuffer());
		valstyle.bstrVal = strHeight.AllocSysString();
		QiStyleObj->put_height(valstyle);
		SysFreeString(valstyle.bstrVal);

		//设置背景颜色为白色
		valstyle.bstrVal = _T("white");
		QiStyleObj->put_backgroundColor(valstyle);
	}

	valstyle.bstrVal = _T("0%");
	QiStyleObj->put_left(valstyle);

	//设置上边距
	valstyle.bstrVal = _T("0%");
	QiStyleObj->put_top(valstyle);

	//设置position
	CComQIPtr<IHTMLStyle2>QiStyleObj2;
	QiStyleObj2 = QiStyleObj;
	QiStyleObj2->put_position(_T("fixed"));

	//设置margin-left
	valstyle.bstrVal = _T("0");
	QiStyleObj->put_marginLeft(valstyle);
	QiStyleObj->put_marginTop(valstyle);
	QiStyleObj->put_marginBottom(valstyle);

	//获取元素调整后的宽高
	pElement2 = qiElem;
	pElement2->get_clientWidth(dwWidth);
	pElement2->get_clientHeight(dwHigth);


	//去掉滚动条
	pDoc2->get_body(&QiBody);
	QiBody->QueryInterface(IID_IHTMLBodyElement, (void**)&QiBody1);

	QiBody1->put_scroll(L"no");

	////获取所有元素
	//pDoc2->get_all(&pAllColls);
	//pAllColls->get_length(&lcount);

	//for (int i = 0; i < lcount; ++i)
	//{
	//	VARIANT index = { 0 };
	//	V_VT(&index) = VT_I4;
	//	V_I4(&index) = i;
	//	long icount = i;
	//	pAllColls->item(index, index, &pDispatTemp);
	//	pElement = pDispatTemp;
	//	hret = pElement->get_tagName(&bstr);
	//	CString str = bstr;

	//	//排除html和body标签
	//	if (!str.CompareNoCase(_T("HTML")) || !str.CompareNoCase(_T("BODY")) || !str.CompareNoCase(_T("script")) ||
	//		!str.CompareNoCase(_T("meta")) || !str.CompareNoCase(_T("head")) || !str.CompareNoCase(_T("link")))
	//	{
	//		continue;
	//	}

	//	//方式1(得排除html和body,否则无法显示)
	//	VARIANT_BOOL bVar = VARIANT_FALSE;
	//	qiElem->contains(pElement, &bVar);

	//	if (VARIANT_TRUE != bVar)
	//	{
	//		CComQIPtr<IHTMLStyle> QiStyle;
	//		pElement->get_style(&QiStyle);
	//		QiStyle->put_display(_T("none"));
	//	}
	//	else
	//	{
	//		continue;
	//	}
	//}

	return true;
}

BOOL CSumengine2View::HandlePictureCode(BOOL bFirst, BOOL bCopy, CStdString strImgPath, CStdString strParam,
												CString strUniqueID, CString& strPath, CStdString strInputPrompt)
{	
	bool bFirstSucc = false;

	if (bFirst || bCopy)
	{
		bFirstSucc = VerifyCodeHandlePicture(strImgPath, bCopy, bFirst);
	}
	//不截图处理而且不是第一次
	if (bFirstSucc == false || (bFirst == false && bCopy == false))
	{
		int nTime = 0;
		while (nTime < 1)
		{
			WaitWithUIMsgDisp(10);

			//下载验证码
			GetVerifyCodeImgSrc(strParam);
			m_log.Trace(LOGL_TOP, LOGC_ERROR, __TFILE__, __LINE__, _T("验证码路径：%s，下载路径：%s"), CString(strParam), CString(strImgPath));
			DownloadOcrPic(strParam, strImgPath);

			m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("验证码下载完成!"));
			if (ChkImageValid(strImgPath))
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("验证码下载成功!"));
				break;
			}
			else
			{
				nTime++;
				continue;
			}
		}
	}
	//如果图片不是有效图片
	if (ChkImageValid(strImgPath) == false)
	{
		if (bFirst)
		{
			SetStepErrDesc(_T("下载验证码失败！"));
			return FALSE;
		}
		else
		{
			if (m_strOldImg.IsEmpty())
			{
				m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("m_strOldImg值为空啊"));
				return FALSE;
			}
			strImgPath = m_strOldImg;
		}
	}
	else
	{
		//微信调整图片为130*130
		if (evtWeiXin == m_eVerifyCodeType)
		{
			if (!StretchWeinXinImage(strImgPath))
			{
				SetStepErrDesc(_T("调整微信图片大小失败!"));
				return FALSE;
			}
		}
	}

	CStringArray strArrary;
	if (strInputPrompt == _T("验证码输入提示"))
	{
		strInputPrompt = _T("");
	}
	strArrary.Add(strImgPath);
	ComBineHtml(strArrary, E_Single_Pic, strUniqueID, strPath, strInputPrompt.GetBuffer());

	//确保下一次的时候刷新不到验证码，有旧的验证码可用
	m_strOldImg = strImgPath;

	return TRUE;
}

BOOL CSumengine2View::HandleQuestionCode(BOOL bFirst, BOOL bCopy, CStdString strImgPath, CStdString& strParam, 
													CString strUniqueID, CString& strPath, CStdString strInputPrompt)
{
	if (false == VerifyCodeHandleQuestion(strParam, bFirst, bCopy))
	{
		m_log.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("点击验证码失败"));
		SetStepErrDesc(_T("下载验证码失败！"));
		return FALSE;
	}
	//默认值时不显示提示
	if (strInputPrompt == _T("验证码输入提示"))
	{
		strInputPrompt = _T("");
	}

	CStringArray strArrary;
	strArrary.Add(strParam);
	ComBineHtml(strArrary, E_TEXT_PIC, strUniqueID, strPath, strInputPrompt.GetBuffer());

	return TRUE;
}

BOOL CSumengine2View::HandleUserInputCode(CStdString strParam, CStdString& strAutoOcrResult,
									CString strUniqueID, CString& strPath, CStdString strInputPrompt)
{	
	CStringArray strArrary;
	if (evtTelephone == m_eVerifyCodeType)
	{
		if (strInputPrompt == _T("验证码输入提示") || strInputPrompt.IsEmpty())
		{
			strInputPrompt.Format(_T("请确认您在<font color=\"red\">%s</font>的注册手机号！"), m_strNsName);
		}
		strInputPrompt += _T("<br>");
		strAutoOcrResult = strParam;

		strArrary.Add(strInputPrompt);
		ComBineHtml(strArrary, E_TEXT_PIC, strUniqueID, strPath);
	}
	else if (evtTelephoneCode == m_eVerifyCodeType)
	{
		if (strInputPrompt == _T("验证码输入提示") || strInputPrompt.IsEmpty())
		{
			strInputPrompt.Format(_T("请输入<font color=\"red\">%s</font>发给您的手机验证码！<br><p style=\"font - size:12px; \">提示：因网站或短信平台等第三方原因有可能收不到验证码，此时直接点击\"确定\"即可</p>"), m_strNsName);
		}
		strInputPrompt += _T("<br>");

		strArrary.Add(strInputPrompt);
		ComBineHtml(strArrary, E_TEXT_PIC, strUniqueID, strPath);
	}
	else if (evtEMail == m_eVerifyCodeType)
	{
		if (strInputPrompt == _T("验证码输入提示") || strInputPrompt.IsEmpty())
		{
			strInputPrompt.Format(_T("请确认您在<font color=\"red\">%s</font>的注册邮箱！"), m_strNsName);
		}
		strInputPrompt += _T("<br>");
		strAutoOcrResult = strParam;
		strArrary.Add(strInputPrompt);
		ComBineHtml(strArrary, E_TEXT_PIC, strUniqueID, strPath);
	}
	else if (evtEMailCode == m_eVerifyCodeType)
	{
		if (strInputPrompt == _T("验证码输入提示") || strInputPrompt.IsEmpty())
		{
			strInputPrompt.Format(_T("请查收邮件并输入<font color=\"red\">%s</font>发给您的邮箱验证码！"), m_strNsName);
		}
		strInputPrompt += _T("<br>");
		strArrary.Add(strInputPrompt);
		ComBineHtml(strArrary, E_TEXT_PIC, strUniqueID, strPath);
		m_ocrData.lgTimeWait = 120;
	}

	return TRUE;
}
