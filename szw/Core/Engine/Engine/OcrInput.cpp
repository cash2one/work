#include "StdAfx.h"
#include "OcrInput.h"



COcrInput::COcrInput(void)
{
	m_pfuncPushOCRData = NULL;
	m_pfuncSetOcrInputFocus = NULL;
	m_hMutexSyncFocus = NULL;
	m_hMutexSystemInput = NULL;
}


COcrInput::~COcrInput(void)
{	
	if (NULL != m_hMutexSyncFocus)
	{	
		CloseHandle(m_hMutexSyncFocus);
		m_hMutexSyncFocus = NULL;
	}

	if (!m_hDLL)
	{
		FreeLibrary(m_hDLL);
	}	
}

//初始化接口
bool COcrInput::Init(void)
{
	CStdString strMsg = _T("");
	CStdString strDir = GetInstallPath();

	m_hDLL = LoadLibrary(strDir + _T("\\bin\\InputOCR.dll"));
	if(m_hDLL)
	{
		m_pfuncPushOCRData = (PUSH_OCR_DATA)GetProcAddress(m_hDLL, PUSHCODEDATA);
		m_pfuncSetOcrInputFocus = (SET_OCR_INPUTFOCUS)GetProcAddress(m_hDLL, SETOCRINPUTFOCUS);
		m_pfunGetCodeDataBack = (GET_CODEDATA_BACK)GetProcAddress(m_hDLL, GETCODEDATABACK) ;
	}

	if (!m_hDLL || !m_pfuncPushOCRData || !m_pfuncSetOcrInputFocus)
	{
		strMsg.Format(_T("加载验证码输入模块失败! err:%d"), GetLastError());
		MessageBox(NULL, strMsg, _T("程序错误"), MB_OK);
		return false;
	}

	m_hMutexSyncFocus = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("InputFocus"));
	if (NULL == m_hMutexSyncFocus)
	{
		strMsg.Format(_T("打开同步焦点对象失败! err:%d"), GetLastError());
		MessageBox(NULL, strMsg, _T("程序错误"), MB_OK);
		return false;
	}

	m_hMutexSystemInput = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("Engine_SystemInPutMutex"));

	return true;
}

//压入验证码信息，识别操作放在此函数中
void COcrInput::PushOcrData(T_CodeData_IN &data)
{
	ASSERT(m_pfuncPushOCRData != NULL);

	//识别验证码
	//...

	m_pfuncPushOCRData(data);
	FocusOcrInput();
}

//获取用户确认之后的验证码
bool COcrInput::GetResult(CString strNsCode, T_CodeData_Back &tResult)
{
	ASSERT( m_pfunGetCodeDataBack != NULL) ;

	//获取验证码返回结果
	 return m_pfunGetCodeDataBack(strNsCode, tResult) ;
}

//获取输入焦点的互斥信号
bool COcrInput::SyncInputFocusWithOcrDlg(void)
{
	//验证码框显示时，返回false
	DWORD dwRet = WaitForSingleObject(m_hMutexSyncFocus, 0);
	if (WAIT_TIMEOUT == dwRet)
		return false;

	return true;
}

//释放输入焦点的互斥信号，允许验证码框弹出
void COcrInput::ReleaseInputFocus()
{
	ReleaseMutex(m_hMutexSyncFocus);
}

//为验证码输入框设置输入焦点
void COcrInput::FocusOcrInput(void)
{
 	ASSERT(m_pfuncSetOcrInputFocus != NULL);
 
	if (m_pfuncSetOcrInputFocus())
 	{
		return;
 	}

	HWND hWndDlg = FindWindow(_T("#32770"), _T("验证码输入"));
	HWND hWndInputDlg = NULL;
	HWND hInputControl = NULL;

	do 
	{
		if (!hWndDlg)
		{
			return;
		}
			
		//找到单个验证码输入对话框
		hWndInputDlg = FindWindowEx(hWndDlg, NULL, _T("#32770"), NULL);
		if (hWndInputDlg)
		{	
			SwitchToThisWindow(hWndInputDlg, TRUE);
			//找到输入框
			hInputControl = FindWindowEx(hWndInputDlg, NULL, _T("Edit"), NULL);
			if (hInputControl) 
				SendMessage(hInputControl, WM_SETFOCUS, NULL, NULL);
		}

		hWndDlg = FindWindowEx(NULL, hWndDlg, _T("#32770"), _T("验证码输入"));
	} while (true);

}

void COcrInput::GetSystemInputMutex()
{
	if (NULL != m_hMutexSystemInput)
	{
		WaitForSingleObject(m_hMutexSystemInput, 0);
	}
}

void COcrInput::ReleaseSystemInputMutex()
{
	if (NULL != m_hMutexSystemInput)
	{
		ReleaseMutex(m_hMutexSystemInput);
	}
}
