
// Sumengine2.h : Sumengine2 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "CommClient.h"
#include "ExecMgr.h"
#include "UploadFile.h"

// CSumengine2App:
// 有关此类的实现，请参阅 Sumengine2.cpp
//

class CSumengine2App : public CWinApp
{
public:
	CSumengine2App();
	//~CSumengine2App();

	void OnFileNew();
	static CCommClient m_sCommClient;

	CSumengine2View *NewTaskView(CStdString strTaskID, CStdString strNsName, DWORD dwAmbientProperty, bool bIsTmpView = false);
	int  GetCurrentRunMode(void);       //获取内核的当前运行模式
	BOOL GetYunTaskFlag(void);			//获取当前推广任务类型
	TCHAR *GetSuccQuickLookDir(void);
	TCHAR *GetFailQuickLookDir(void);
	bool IsNewTmpView(){ return m_bIsNewTmpView; } ;
	void UploadFile(T_UPLOAD_FILE_PARAM &param);
	bool GetUploadResult(CStdString strWebName, T_UPLOAD_FILE_RESULT &res);

private:
	_TDOSPARAM m_tDosParam;							//程序调用内核下发的参数
	TCHAR m_tszSuccQuickLookDir[MAX_PATH];         //成功快照的存放目录
	TCHAR m_tszFailQuickLookDir[MAX_PATH];         //成功快照的存放目录
	CString m_strFlashOcx;
	LONG m_nFlashRefCount;
	CUploadFile m_updateFileObj;
	CLock m_lockNewView;			               //新建任务视图
	bool m_bIsNewTmpView ;				           //标记新创建的视图是否为临时视图
	HANDLE m_hStopThread;				           //检测是否有停止任务的线程
	//初始化DLL及相应接口
	bool InitLibs(void);	
	//初始化人工打验证码dll
	bool InitManmadeCode(BOOL bIsRemoteCode = FALSE);	
	//清除上次的验证码
	void ClearOldVerifyCodes(void);     
	void InitQuickLookDirectory(void);
	void InitCfgSetting(void);
	bool StringToPoint(XYPoint &pt, CStdString str);
	CString GetFlashOcxFile(void);
	void FillVecSilentWeb(CString strFliterWord, CStdString strSilent);
	void GetUrlMapData(void);
	void AnalysisDosParam(int iArgCount,wchar_t** arg);
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};

extern CSumengine2App theApp;
