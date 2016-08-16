#include "StdAfx.h"
#include "SelectFile.h"

CLogTrace g_SelFileLog(_T("core\\SelFile.log"), NULL);      //保存运行时日志
WNDPROC g_OldWndProc = NULL ;

CSelectFile::CSelectFile(void)
{
	m_hBtnCancel = NULL;
	m_hDlgWnd = NULL;
	m_hInputWnd = NULL;
	m_dwcount = 0;
}


CSelectFile::~CSelectFile(void)
{

}

bool CSelectFile::FindUploadDialog(HWND hParentWnd)
{
 	if (!GetSelFileHwnds(hParentWnd))
	{
		return false;
	} 
	else
	{
		return true;
	}
}

HWND CSelectFile::GetDialogHwnd(void)
{
	return m_hDlgWnd;
}

bool CSelectFile::SetFile(HWND hParentWnd, CStdString strFile, CStdString &strErrorMsg, CStdString strWebName)
{	
	g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("--------SetFile----网站%s,---begin"), CString(strWebName));

	if (!GetSelFileHwnds(hParentWnd))
	{
		strErrorMsg = _T("找不到文件选择对话框！") ;
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, (CString)strErrorMsg);
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("--------GetSelFileHwnds----网站%s,---fail---end"), CString(strWebName));
		return false;
	}

	//此处设置明主要是针对object上传图片类型，无法捕获其窗口创建事件
	setWndTransparent(m_hDlgWnd,0);

	//等待窗口初始化完成
	WaitWindowShow();

	if (!SetFilePath(m_hInputWnd, strFile))
	{
		strErrorMsg =  _T("赋值文件路径失败！") ;
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, (CString)strErrorMsg);
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("--------SetFilePath----网站%s,---fail---end"), CString(strWebName));
		return false;
	}
	g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("赋值文件路径成功！"));

	//确保赋值成功后文件还存在
	if (!PathFileExists(strFile))
	{
		g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("bug:点击确认之前文件不存在！"));
		return false;
	}

	if (!Confirm(m_hDlgWnd, strFile))
	{
		strErrorMsg =  _T("确定文件路径失败！") ;
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, (CString)strErrorMsg);
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("--------Confirm----网站%s,---fail---end"), CString(strWebName));
		return false;
	}

	g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("选择文件成功！"));
	g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("--------Confirm----网站%s,---successful---end"), CString(strWebName));
	return true;
}

void CSelectFile::CloseUploadDialog(void)
{
	if (NULL != m_hBtnCancel)
	{
		g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("CloseUploadDialog！"));

		SendMessage(m_hBtnCancel, WM_LBUTTONDOWN, NULL, NULL);
		SendMessage(m_hBtnCancel, WM_LBUTTONUP, NULL, NULL);

		Sleep(100);
		if (!SelFileDlgExist(m_hDlgWnd))
		{
			return;   //上传文件对话框已关闭
		}

		g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("点击取消按钮关闭上传文件对话框失败，将发送Close消息关闭..."));
		SendMessage(m_hDlgWnd, WM_CLOSE, NULL, NULL);
		Sleep(100);

		if (!SelFileDlgExist(m_hDlgWnd))
		{
			g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("发送Close消息关闭上传文件对话框成功！"));
			return;   //上传文件对话框已关闭
		}
		g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("发送Close消息关闭对话框失败，将直接EndDialog销毁窗口..."));
		
		if (::EndDialog(m_hDlgWnd, IDCANCEL))
		{
			g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("销毁窗口EndDialog成功!"));
			return;   //上传文件对话框已关闭
		}
		else
		{
			g_SelFileLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("销毁窗口EndDialog失败!lastError:%d"), GetLastError());
		}

	}
	else
	{
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("m_hBtnCancel为空，关闭上传文件对话框失败！"));
	}
}

//隐藏上传对话框
void CSelectFile::WaitWindowShow(void)
{
	int iWaitWndShowTimes = 100; 

	//等待窗口可见
 	while(!IsWindowVisible(m_hDlgWnd) && iWaitWndShowTimes-- > 0)
 	{
		Sleep(20);
	}

	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	int nWidth = rt.right;
	int nHight = rt.bottom;

	::SetWindowPos(m_hDlgWnd, HWND_BOTTOM, nWidth, nHight, -1, -1, SWP_NOSIZE | SWP_HIDEWINDOW);
}

//获取选择文件对话框窗口及其相关句柄
bool CSelectFile::GetSelFileHwnds(HWND hOwner)
{
	HWND hDlgWnd = FindWindow(_T("#32770"), NULL);

	do 
	{
		if (!hDlgWnd) 
		{
			return false;
		}

		//查找上传文件对话框窗口
		if (GetParent(hDlgWnd) == hOwner)
		{
			//找到了内核窗口下面的对话框窗口，从该窗口查找相关的控件句柄以确认窗口是上传文件窗口
			bool bFound = true;

			//查找文件输入框句柄
			m_hInputWnd = GetPathInputHWnd(hDlgWnd);
			if (!m_hInputWnd)
			{
				g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("找不到文件输入框！"));
				bFound = false;
			}

			if (bFound)
			{
				g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("找到文件输入框！"));

				m_hBtnOpen = GetOpenBtnHwnd(hDlgWnd);
				if (!m_hBtnOpen)
				{
					g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("找不到'打开/保存'按钮！"));
					bFound = false;
				}
			}

			if (bFound)
			{
				g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("找到'打开/保存'按钮！"));

				m_hBtnCancel = GetCancelBtnHwnd(hDlgWnd);
				if (!m_hBtnCancel)
				{
					g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("找不到'取消'按钮！"));
					bFound = false;
				}
			}

			if (bFound)
			{
				g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("找到'取消'按钮，查找窗口成功！"));
				m_hDlgWnd = hDlgWnd;

				return true;
			}
		}

		hDlgWnd = FindWindowEx(NULL, hDlgWnd, _T("#32770"), NULL);

	} while (true);

	return false;
}

//获取文件输入框的句柄
HWND CSelectFile::GetPathInputHWnd(HWND hSelFileHwnd)
{
	HWND hWnd = NULL;

	//先用ID查找
	hWnd = GetDlgItem(hSelFileHwnd, 0x47C);
	if (!hWnd)
	{
		//找不到再通过控件的父子关系查找
		HWND hContainerWnd = FindWindowEx(hSelFileHwnd, NULL, _T("ComboBoxEx32"), NULL);

		if (!hContainerWnd)
			hContainerWnd = hSelFileHwnd;

		hWnd = FindWindowEx(hContainerWnd, NULL, _T("ComboBox"), NULL);

		hWnd = FindWindowEx(hWnd, NULL, _T("Edit"), NULL);
	}


	if (!hWnd)
	{
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("找不到文件输入框！"));
	}
	else if (hWnd)
	{
		//排除误找到的目标（比比贴信息网确认删除对话框中找到的edit的RECT为：1,1,2,2）
		RECT rt ={0, 0, 0, 0};
		GetWindowRect(hWnd, &rt);
        
		if (rt.right - rt.left < 50)   //判断文件输入框的宽度，最少不能小于50（经验值）
		{
			g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("找到假的文件输入框，跳过！"));
			return NULL;
		}
	}

	return hWnd;
}

//获取打开按钮句柄
HWND CSelectFile::GetOpenBtnHwnd(HWND hSelFileHwnd)
{
	//先通过ID查找
	HWND hWnd = GetDlgItem(hSelFileHwnd, 1);

	//找不到再通过名称查找
	if (!hWnd)
	{
		hWnd = FindWindowEx(hSelFileHwnd, NULL, _T("Button"), _T("打开(&O)"));
	}
	if (!hWnd)
	{
		hWnd = FindWindowEx(hSelFileHwnd, NULL, _T("Button"), _T("保存(&S)"));
	}

	return hWnd;
}

//获取取消按钮句柄
HWND CSelectFile::GetCancelBtnHwnd(HWND hSelFileHwnd)
{
	//先通过ID查找
	HWND hWnd = GetDlgItem(hSelFileHwnd, 2);

	//找不到再通过名称查找
	if (!hWnd)
	{
		hWnd = FindWindowEx(hSelFileHwnd, NULL, _T("Button"), _T("取消"));
	}

	return hWnd;
}

//为输入框赋文件路径
bool CSelectFile::SetFilePath(HWND hInput, CStdString strFile)
{	
	g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("GetParent(hInput) = %d ,m_hDlgWnd = %d"), GetParent(hInput), m_hDlgWnd);
	if(!IsWindow(hInput))
	{
		g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("窗口不存在！"));
		return false;
	}

	int i = 10;
	do 
	{
		//设置路径
		try
		{
			TCHAR tszTmp[MAX_PATH] = {0};
			_stprintf(tszTmp, _T("%s"), strFile.GetBuffer());
			g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("SendMessage(hInput, WM_SETTEXT, NULL, %s)"),tszTmp);
			SetWindowText(hInput, _T(""));
			if(SetWindowText(hInput, tszTmp) == 0)
			{
				g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("设置失败！"));
			}
			else
			{
				EnableWindow(hInput, FALSE);
				SendMessage(hInput, WM_ENABLE, FALSE, 0);
				g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("设置成功！"));
				return true;
			}
		}
		catch(...)
		{
			g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("-------呵呵，shit---SendMessage-WM_SETTEXT消息异常"),CString(strFile));
			return false;
		}
	} while (i-- > 0);

	/*m_bOwnInputFocus*/
	g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("返回失败！"));
	return false;
}

//点击打开按钮确定
bool CSelectFile::Confirm(HWND hSelFileHwnd, CStdString strFile)
{
	//change
	int nCount = 1;

	for (int i = 0; i < nCount; i++)
	{
		if( (0 == SendMessage(m_hBtnOpen, WM_LBUTTONDOWN, NULL, NULL)) && (0 == SendMessage(m_hBtnOpen, WM_LBUTTONUP, NULL, NULL)) )
		{
			g_SelFileLog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("WM_LBUTTONDOWN OK！"));
			return true;
		}
		else
		{
			g_SelFileLog.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("WM_LBUTTONDOWN fail！"));
		}
	}

	return false;
}

//查看指定的上传文件对话框是否还在
bool CSelectFile::SelFileDlgExist(HWND hSelFileHwnd)
{
	HWND hDlgWnd = FindWindow(_T("#32770"), NULL);

	do 
	{
		if (!hDlgWnd) return false;

		//查找上传文件对话框窗口
		if (hDlgWnd == hSelFileHwnd)
		{
			return true;		
		}

		hDlgWnd = FindWindowEx(NULL, hDlgWnd, _T("#32770"), NULL);
	} while (true);

	return false;
}

