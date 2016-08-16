#include "StdAfx.h"
#include "UploadFile.h"
#include "SelectFile.h"

CLogTrace g_uploadlog(_T("Core\\uploadfile.log"), NULL);


CUploadFile::CUploadFile(void)
{
	m_hTaskInSignal = NULL;
	m_hThread = NULL;
	m_bStop = false;
	m_bAutoCloseDlg = true;
}


CUploadFile::~CUploadFile(void)
{
	Stop();
}

bool CUploadFile::Init(HWND hWnd, bool bAutoClose)
{
	m_bAutoCloseDlg = bAutoClose;
	m_hMainFrameWnd = hWnd;
	m_hTaskInSignal = CreateEvent(NULL, FALSE, TRUE, NULL);
	
	m_hThread = CreateThread(NULL, 0, &CUploadFile::ThreadProc, this, 0, NULL );
	if (m_hThread != NULL && m_hTaskInSignal != NULL)
	{
		g_uploadlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("上传文件对象启动成功！"));
		return true;
	}
	else
	{
		g_uploadlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("上传文件对象启动失败，err:%d！"), GetLastError());
		return false;
	}
}

void CUploadFile::Stop(void)
{
	m_bStop = true;
	SetEvent(m_hTaskInSignal);   //释放等待线程
	
	if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 2000))
	{
		TerminateThread(m_hThread, 0);
	}
}

void CUploadFile::PushTask(T_UPLOAD_FILE_PARAM &task)
{
	g_uploadlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("压入上传文件任务，网站：%s,文件：%s"), CString(task.strWebName), CString(task.strFile));
	
	CLocalLock lock(&m_lock);
	m_vUploadList.push_back(task);

	SetEvent(m_hTaskInSignal);
}

DWORD WINAPI CUploadFile::ThreadProc(LPVOID lpParameter)
{
	CUploadFile *pThis = (CUploadFile*)lpParameter;

	while(!pThis->m_bStop)
	{
		WaitForSingleObject(pThis->m_hTaskInSignal, 3000);

		{
			CLocalLock lock(&pThis->m_lock);

			if (pThis->m_vUploadList.size() == 0)
			{
				CSelectFile selFile;
				T_UPLOAD_FILE_PARAM Param;
				if (selFile.FindUploadDialog(pThis->m_hMainFrameWnd))
				{
					if (pThis->m_bAutoCloseDlg)
					{
						pThis->EnsureCloseUploadDiloag();				
						g_uploadlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("找到遗留未关闭的上传对话框【ID：%08x】将关闭之"), selFile.GetDialogHwnd());
					}
				}

				continue;
			}

			Sleep(100);    //View点击浏览按钮后，等待上传对话框弹出
			std::vector<T_UPLOAD_FILE_PARAM>::iterator itTask = pThis->m_vUploadList.begin();
			for (; itTask < pThis->m_vUploadList.end(); ++itTask)
			{
				T_UPLOAD_FILE_PARAM Param = *itTask;
				CSelectFile selFile;
				int iRetry = 0;
				T_UPLOAD_FILE_RESULT tRes;

RETRY:
				//bool bRes = false;
				bool bRes = selFile.SetFile(pThis->m_hMainFrameWnd, Param.strFile, Param.strErrorMsg, Param.strWebName);

				if (bRes)
				{
					tRes.bRes = true;
				}
				else if (iRetry++ < 20)//最多重试50次,5s
				{	
					Sleep(100);
					goto RETRY;
				}

				g_uploadlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("上传结果：%s！网站：%s，当前重试次数%d"), tRes.bRes ? _T("成功") : _T("失败"), CString( Param.strWebName), iRetry);

				Sleep(50);
				if(!bRes)
				{
					//关闭上传对话框
					selFile.CloseUploadDialog();
				}	

				pThis->EnsureCloseUploadDiloag();

				//保存结果到返回列表
				if(pThis->m_vUploadList.size() > 0)
				{
					pThis->m_mapUploadResult[Param.strWebName] = tRes;
					pThis->m_vUploadList.erase(itTask);
					SetEvent(Param.hUploadFinishSignal);   //通知View上传结束
				}
			}
		}
	}
	return 0;
}

bool CUploadFile::GetUploadResult(CStdString strWebName, T_UPLOAD_FILE_RESULT &res)
{
	CLocalLock lock(&m_lock);
	std::map<CStdString, T_UPLOAD_FILE_RESULT>::iterator itResult = m_mapUploadResult.find(strWebName);
	if (itResult != m_mapUploadResult.end())
	{
		res = m_mapUploadResult[strWebName];
		m_mapUploadResult.erase(itResult);

		g_uploadlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("取上传结果成功！网站：%s,剩余%d个结果"), CString(strWebName), m_mapUploadResult.size());

		return true;
	}

	g_uploadlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("取上传结果失败！网站：%s,剩余%d个结果"), CString(strWebName), m_mapUploadResult.size());
	return false;
}

//确认关闭上传文件对话框
void CUploadFile::EnsureCloseUploadDiloag(void)
{
	for (int i = 0; i < 3; i++)
	{
		CSelectFile sel;
		if (!sel.FindUploadDialog(m_hMainFrameWnd))
		{
			return;
		}
		Sleep(100);

		//g_uploadlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("检测到上传文件对话框未关闭，将尝试关闭！"));
		sel.CloseUploadDialog();
	}

	//g_uploadlog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("尝试关闭上传对话框失败！"));
}