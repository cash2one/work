#pragma once
#include "stdafx.h"


class CUploadFile
{
public:
	CUploadFile(void);
	~CUploadFile(void);

	bool Init(HWND hWnd, bool bAutoClose);
	void Stop(void);
	void PushTask(T_UPLOAD_FILE_PARAM &task);
	bool GetUploadResult(CStdString strWebName, T_UPLOAD_FILE_RESULT &res);
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

public:
	HWND m_hMainFrameWnd;
	HANDLE m_hThread;
	HANDLE m_hTaskInSignal;             //任务入队事件
	std::vector<T_UPLOAD_FILE_PARAM> m_vUploadList;
	std::map<CStdString, T_UPLOAD_FILE_RESULT> m_mapUploadResult;
	CLock m_lock;
	bool m_bStop;
	bool m_bAutoCloseDlg;   //检测到未关闭对话框时是否自动关闭

	void EnsureCloseUploadDiloag(void);
};

