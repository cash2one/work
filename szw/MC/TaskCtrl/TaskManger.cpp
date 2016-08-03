
#include "StdAfx.h"
#include ".\taskmanger.h"
#include "Reg.h"
#include <ShellAPI.h>
#include <IPHlpApi.h>
#include "CommFunc.h"
#include <afxinet.h>
#include "FileReadAndSave.h"

#pragma comment(lib,"Iphlpapi.lib")

CTaskManger::CTaskManger(void)
:IThreadUnit(E_TASKMGR_THREAD,0xFFFF)	//第1个参数是线程类别，第2个参数(秒)是超时上限(0xFFFF表示不超时)
{
	m_TaskDoingMax = 40;
	m_TaskDoingCount = 0;
	m_strSuccAccount = _T("");
// 	vNsCodeOfObject.clear();
// 	vNsCodeOfObject.push_back(40000519);
// 	vNsCodeOfObject.push_back(40006512);
// 	vNsCodeOfObject.push_back(40001567);
// 	vNsCodeOfObject.push_back(49994692);
// 	vNsCodeOfObject.push_back(40012911);
}

CTaskManger::~CTaskManger(void)
{
	
}

//修改为优先获取三代任务，没有则获取二代任务
BOOL CTaskManger::GetIdleTask(DWORD &dwTaskIndex,DWORD &dwLevel)
{
	if(m_TaskDescMap.size()==0) return FALSE;
	std::map<DWORD,T_TaskDesc*>::iterator pIter;
	for(pIter=m_TaskDescMap.begin();pIter!=m_TaskDescMap.end();pIter++)
	{
		DWORD index= pIter->first;

		T_TaskDesc *pTaskDesc = pIter->second;	
		if(pTaskDesc->Data->eTaskType == ETypeMailCount)
		{
			if ((m_mapTaskCount[ETTypeRegaccount] > 0) || (m_mapTaskCount[ETTypeReRegaccount]>0))
			{
				continue;
			}
		}

// 		for (int i = 0; i< vNsCodeOfObject.size(); ++i)
// 		{
// 			if ((vNsCodeOfObject[i] == pTaskDesc->dwWebId) && (m_TaskDescMap.size() > 6))
// 			{
// 				continue;
// 			}
// 		}

		if(pTaskDesc==NULL)
		{
			continue;
		}
		
		if(pTaskDesc->TaskState == e_dataprepared) //任务数据!=原始数据 = 数据准备完成的才能进行下压到ExSer
		{
			switch(pTaskDesc->eLevel) 
			{
			case ETRegaccountAsync://注册级
				{
					dwTaskIndex = pTaskDesc->dwTaskIndex;
					dwLevel = 0;
					return TRUE;
				}
				break;
			case ETPostAsync://发布级
				{
					dwTaskIndex = pTaskDesc->dwTaskIndex;
					dwLevel = 0;
					return TRUE;
				}
				break;
			case ETPostSync://发布需注册(一般这种都是数据准备好的)
				{
					if(pTaskDesc->dwPrvTaskID>0) //前置任务>0
					{
						dwTaskIndex = pTaskDesc->dwTaskIndex;
						dwLevel = 0;
						return TRUE;
					}
					else
					{
						dwTaskIndex = pTaskDesc->dwTaskIndex;
						dwLevel = 1;
						return TRUE;
					}
				}
				break;
			default:
				g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("任务类型无法确定 taskid = %d pTaskDesc->eLevel:%d"), dwTaskIndex, pTaskDesc->eLevel);
			}
		}
	}

	return FALSE;
}


BOOL CTaskManger::SetTaskState(DWORD dwTaskIndex,E_TASKSTATE eTaskState)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		pTaskDesc->eLevel = pTaskDesc->Data->eTaskLevel; //级别
		pTaskDesc->TaskState = eTaskState;
		/*switch(pTaskDesc->TaskState)
		{
		case e_updateerr:
		case e_updatefinished:
			{
				pTaskDesc->TaskState  = e_dataprepare;
				PostMessage(E_DATAPRE_THREAD, E_PPD_RESTART_MSG, dwTaskIndex);
			}
			break;
		}*/
		return TRUE;
	}
	return FALSE;
}


BOOL CTaskManger::SetTaskStartTime(DWORD dwTaskIndex,DWORD dwTime)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		pTaskDesc->dwStartTime = dwTime;
		return TRUE;
	}
	return FALSE;
}

T_TaskDesc *CTaskManger::GetTask(DWORD dwTaskIndex)
{
	std::map<DWORD,T_TaskDesc *>::iterator pIter;
	pIter = m_TaskDescMap.find(dwTaskIndex);
	if(pIter!=m_TaskDescMap.end()) return pIter->second;
	return NULL;
}

DWORD CTaskManger::RemoveTask(DWORD dwTaskIndex)
{
	T_TaskData *pTaskData =
		(T_TaskData *)g_GlobalDataBlock->DeleteDataNode(
		dwTaskIndex,E_AllTaskData,E_THREADTASKMGR_CLASS,eUseBlockWithOne);
	
	std::map<DWORD,T_TaskDesc *>::iterator pIter=m_TaskDescMap.find(dwTaskIndex);
	if(pIter != m_TaskDescMap.end())
	{
		T_TaskDesc *pTaskDesc = pIter->second;
		g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("RemoveTask webid: [ %d ]"),pTaskDesc->Data->dwWebID);
		m_TaskDescMap.erase(dwTaskIndex);
		_DELETE(pTaskDesc);
	}
      
	_DELETE(pTaskData);

	return 0;
}

//任务调度
DWORD CTaskManger::SchedulerTask()
{
	BOOL bRet = FALSE;
	BOOL bEngineTask = FALSE;
	
	//分配本地任务
	for(int i=0;i<m_TaskDoingMax;i++)
	{
		DWORD dwTaskIndex = 0;
		DWORD dwLevel = 0;
		bRet = this->GetIdleTask(dwTaskIndex,dwLevel);
		if(bRet != TRUE)
			break;
		if(m_TaskDoingCount ==0)
		{
			g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("任务调度进入4代内核执行 taskid:%d"), dwTaskIndex);
		}

		m_TaskDoingCount++;
		SetTaskState(dwTaskIndex,e_doing);
		PostMessage(E_4KERNELWORK_THREAD, E_4KERNEL_TASK_IN, dwTaskIndex, dwLevel);
		
	       
	}

	return 0;
}


//建立任务列表
DWORD CTaskManger::BuildTaskList(DWORD dwGUIDataIndex)
{
	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *pTaskItem = NULL;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData,LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS,eUseBlockWithOne);

	pTaskItem = (std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*> *)g_GlobalDataBlock->ReadRealData(E_MAP_NetStation,
		dwGUIDataIndex,E_Save_CLASS,eUseBlockWithOne);

	std::map<DWORD,std::map<DWORD,T_TaskNetItem*>*>::iterator iterType;
	std::map<DWORD,T_TaskNetItem*>::iterator iterTask;
	ASSERT(pTaskItem != NULL && pLocalGlobalData != NULL);

	CString strMac = GetPhysicAddr();
	if (strMac.GetLength() <= 0)
	{
		strMac = _T("00-00-00-00-00-00");
	}
	_tcsncpy(pLocalGlobalData->szMac, strMac, MAX_ID_STRING_LENGTH - 1);
	//将配置信息写入ini文件
	g_Config->WriteInt(_T("MC"), _T("INPUTOCR"), _T("MaxRetryTimes"), pLocalGlobalData->dwRetry);
	g_Config->WriteInt(_T("MC"), _T("INPUTOCR"), _T("Display"), pLocalGlobalData->dwShow);
	g_Config->WriteInt(_T("MC"), _T("INPUTOCR"), _T("ShowTime"), pLocalGlobalData->dwShowTime);
	g_Config->WriteInt(_T("MC"), _T("INPUTOCR"), _T("InputTime"), pLocalGlobalData->dwInputTime);
	g_Config->WriteInt(_T("MC"), _T("INPUTOCR"), _T("Type"), pLocalGlobalData->dwOCRType);

	for(iterType = pTaskItem->begin(); iterType != pTaskItem->end(); ++iterType)
	{
		pLocalGlobalData->dwTaskType = iterType->first;
		
		m_mapTaskCount[pLocalGlobalData->dwTaskType] = 0;
		for (iterTask = iterType->second->begin(); iterTask != iterType->second->end(); iterTask++)
		{
			T_TaskData *pTaskData = NULL;
			_NEW(pTaskData,T_TaskData);

			DWORD dwIndex = g_GlobalDataBlock->GetGlobalID();

			T_TaskNetItem *pTaskItem = (T_TaskNetItem *)iterTask->second;
			pTaskData->dwWebID = pTaskItem->dwWebId;

			pTaskData->eTaskType = (E_TASK_TYPE)pLocalGlobalData->dwTaskType;
			/*pTaskData->dwPostType = pLocalGlobalData->dwPostType;*/
			pTaskData->dwIndex = dwIndex;
			pTaskData->bUpdateDataFlag = pLocalGlobalData->bIsUpdate;

			pTaskData->szAccount = _T("");
			pTaskData->szPassword = _T("");
			CopyMemory(&(pTaskData->tTaskNetItem),pTaskItem,sizeof(T_TaskNetItem));

			pTaskData->tTaskResultInfo.eResultFlag = e_TASK_RESULT_FAIL; //默认是失败

			pTaskData->eTaskLevel = ETRegaccountAsync;
			if ((pLocalGlobalData->dwTaskType == ETTypeRegaccount) ||(pLocalGlobalData->dwTaskType == ETTypeReRegaccount))
				pTaskData->eTaskLevel = ETRegaccountAsync;
			else if (pLocalGlobalData->dwTaskType == ETTypePost)
				pTaskData->eTaskLevel = ETPostSync;

			g_GlobalDataBlock->WriteRealData(dwIndex,E_AllTaskData,pTaskData,E_THREADTASKMGR_CLASS,eUseBlockWithOne);
			g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网站ID: [ %d ]"),pTaskData->dwWebID);
		
		
			T_TaskDesc *pTaskDesc = NULL;
			_NEW(pTaskDesc,T_TaskDesc);

			pTaskDesc->Data = pTaskData;
			pTaskDesc->dwTaskIndex = dwIndex;
			pTaskDesc->TaskState = e_dataprepare;

			m_TaskDescMap.insert(Task_Pair(dwIndex,pTaskDesc));
			PostMessage(E_DATAPRE_THREAD,E_PPD_START_MSG,dwIndex);//发给数据准备
		}
		Sleep(50);
		m_mapTaskCount[pLocalGlobalData->dwTaskType]=iterType->second->size(); //  pTaskItem[pLocalGlobalData->dwTaskType].size();    //保存任务类型网站数量
	}
	g_log.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("分解任务数[%d] "),m_TaskDescMap.size());

	return (DWORD)m_TaskDescMap.size();
}

DWORD CTaskManger::TaskResult(DWORD dwTaskIndex, BOOL bSend)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	T_TaskData *pTaskData = NULL;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS, eUseBlockWithOne);
	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwTaskIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);
	//处理注册数量，便于执行邮箱查询；
	if (pTaskData->eTaskType == ETTypeRegaccount)
	{
		m_mapTaskCount[ETTypeRegaccount] --;
	}
	else if (pTaskData->eTaskType == ETTypeReRegaccount)
	{
		m_mapTaskCount[ETTypeReRegaccount] --;
	}
	
	//取出成功的账号密码等
	if (pTaskData->eTaskType == ETTypeRegaccount 
		|| pTaskData->eTaskType ==  ETTypeReRegaccount)
	{
		CStdString strTmp = _T("");
		if (pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_SUCC
			|| pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_REGMAIL_FAIL
			|| pTaskData->tTaskResultInfo.eResultFlag == e_TASK_RESULT_REGAUDIT_FAIL)
		{
			strTmp.Format(_T("%d(;0)%s(;0)%s(;0)%d"),pTaskData->dwWebID,pTaskData->szAccount.c_str(),pTaskData->szPassword.c_str(),pTaskData->tTaskResultInfo.eResultFlag);
			m_strSuccAccount += strTmp + _T("(;2)");
		}
	}

	//如果当前任务是最后一个任务，则执行发送成功网站账号给上层
	if (bSend && !pLocalGlobalData->bIsYunTask)
	{
		PostUrlAccountData();  //检测是否有成功网站需要提交	
	}
	
	if(pTaskDesc!=NULL)
	{
		m_TaskDoingCount --;

		pTaskDesc->TaskState = e_save;
		PostMessage(E_SAVE_THREAD,E_SAVE_TASKFINISHED,dwTaskIndex);
	}

	return 0;
}


DWORD CTaskManger::TaskFialed(DWORD dwTaskIndex)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	T_TaskData *pTaskData = NULL;

	pTaskData = (T_TaskData*)g_GlobalDataBlock->ReadRealData(E_AllTaskData, dwTaskIndex, 
		E_Save_CLASS, eUseCommReadWithMulti);
	//处理注册数量，便于执行邮箱查询；

	if (pTaskData->eTaskType == ETTypeRegaccount)
	{
		m_mapTaskCount[ETTypeRegaccount] --;
	}
	else if (pTaskData->eTaskType == ETTypeReRegaccount)
	{
		m_mapTaskCount[ETTypeReRegaccount] --;
	}

	if(pTaskDesc!=NULL)
	{
		SetTaskState(dwTaskIndex,e_fialed);
	}
	return 0;
}
DWORD CTaskManger::TaskSucceed(DWORD dwTaskIndex)
{
	T_TaskDesc *pTaskDesc = GetTask(dwTaskIndex);
	if(pTaskDesc!=NULL)
	{
		SetTaskState(dwTaskIndex,e_succeed);		
	}
	return 0;
}

//如果没有发布任务，则删除快照文件夹；

void CTaskManger::DelPhotoPageFolder()
{
	//文件夹直接用CString时会出现句柄无效，所以先转成TCHAR
	//一键搬家和用户发布需要用到快照
	if (m_mapTaskCount[ETTypePost]>0 || 
		m_mapTaskCount[ETTypeUserInfoMigrate] > 0 ||
		m_mapTaskCount[ETTypeGetPdtList] > 0)
	{
		return;
	}
	CString strFolder = _T("");
	g_Config->ReadString(_T("MC"),_T("RESULTPATH"),_T("savepath"),strFolder);

	TCHAR tszPath[MAX_PATH] = {0};
	_sntprintf(tszPath, _TRUNCATE, _T("%s"), strFolder.GetBuffer());
	strFolder.ReleaseBuffer();

	SHFILEOPSTRUCT op;
	memset(&op, 0, sizeof(SHFILEOPSTRUCT));
	op.wFunc = FO_DELETE;
	op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	op.pFrom = tszPath;            
	op.pTo = NULL;
	op.lpszProgressTitle = NULL;

	if (0 != SHFileOperation(&op))
	{
		g_log.Trace(LOGL_TOP,LOGT_ERROR, __TFILE__,__LINE__, _T("删除文件夹%s失败！ err: %d"), tszPath, GetLastError());
	}
}

/*
@brief 获取Mac地址
@return Mac地址
*/
CString CTaskManger::GetMacAddress()
{
	CString strMac = _T("");

	ULONG ulSize = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAdapterAddr = new IP_ADAPTER_ADDRESSES();

	int nRel = GetAdaptersAddresses(AF_INET, 0, NULL, pAdapterAddr, &ulSize);

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		delete pAdapterAddr;

		//重新申请内存大小
		pAdapterAddr = (PIP_ADAPTER_ADDRESSES)new BYTE[ulSize];
		nRel = GetAdaptersAddresses(AF_INET, 0, NULL, pAdapterAddr, &ulSize);
	}

	if (ERROR_SUCCESS == nRel)
	{
		if (pAdapterAddr->PhysicalAddressLength == 6)
		{
			strMac.AppendFormat(_T("%02X-%02X-%02X-%02X-%02X-%02X"),
				(int)pAdapterAddr->PhysicalAddress[0],
				(int)pAdapterAddr->PhysicalAddress[1],
				(int)pAdapterAddr->PhysicalAddress[2],
				(int)pAdapterAddr->PhysicalAddress[3],
				(int)pAdapterAddr->PhysicalAddress[4],
				(int)pAdapterAddr->PhysicalAddress[5]);
		}
	}

	if (pAdapterAddr)
	{
		delete pAdapterAddr;
	}

	return strMac;
}

/*
@brief  通过网卡驱动IO取得mac地址
@param szMac Mac字符串空间, nBuffSize 空间大小
@return int状态码 成功返回0，失败返回-1
*/
BOOL CTaskManger::GetMac(TCHAR *szMac, int nBuffSize)
{
	BOOL bError = FALSE;
	if (szMac == NULL || nBuffSize < 18){
		return bError;
	}

	HKEY hKey = NULL;
	HKEY hKey2 = NULL;
	TCHAR szKey[MAX_PATH], szBuffer[MAX_PATH];
	TCHAR szServiceName[MAX_PATH];
	TCHAR szFileName[MAX_PATH] = { 0 };
	DWORD dwRet = 0;
	DWORD dwType = 0;
	DWORD cbData = 0;
	DWORD cName = _countof(szBuffer);
	if (RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\"), &hKey) != ERROR_SUCCESS){
		return bError;
	}

	for (int i = 0; RegEnumKeyEx(hKey, i, szBuffer, &cName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i, cName = _countof(szBuffer))
	{
		_tcscpy_s(szKey, MAX_PATH, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards\\"));
		_tcscat_s(szKey, MAX_PATH, szBuffer);
		if (RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hKey2) != ERROR_SUCCESS)
		{
			continue;
		}

		dwType = REG_SZ;
		cbData = MAX_PATH*sizeof(TCHAR);
		if (RegQueryValueEx(hKey2, _T("ServiceName"), NULL, &dwType, (LPBYTE)szServiceName, &cbData) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey2);

			_tcscpy_s(szFileName, MAX_PATH, _T("\\\\.\\"));
			_tcscat_s(szFileName, MAX_PATH, szServiceName);
			HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwInBuff = OID_802_3_PERMANENT_ADDRESS;
				BYTE outBuff[MAX_PATH];
				dwRet = DeviceIoControl(hFile, IOCTL_NDIS_QUERY_GLOBAL_STATS, &dwInBuff, sizeof(dwInBuff), outBuff, sizeof(outBuff), &cbData, NULL);

				//无论成功失败关闭文件句柄
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;

				if (dwRet)
				{
					_stprintf_s(szMac, nBuffSize / sizeof(TCHAR), _T("%02X-%02X-%02X-%02X-%02X-%02X"), outBuff[0], outBuff[1], outBuff[2], outBuff[3], outBuff[4], outBuff[5]);
					bError = TRUE;
					break;
				}
			}
		}
		else
		{
			RegCloseKey(hKey2);
		}

	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}

	return bError;
}

//取MAC
CString CTaskManger::GetPhysicAddr()
{
	CString strMac = _T("");
	TCHAR szMac[MAX_PATH] = {0};
	if (GetMac(szMac,MAX_PATH))
	{
		strMac = szMac;
	}
	else
	{
		strMac = GetMacAddress();
	}
	
	if (strMac.Find(_T("-")) == -1)
	{
		CReg reg;
		TCHAR* pContent = (TCHAR*)reg.ReadValueOfKey(HKEY_CURRENT_USER, _T("Software\\szw\\MasterZ"), _T("ClientDefaultMac"));
		strMac = pContent;
	}

	return strMac;
}

//提交账号信息；
void CTaskManger::PostUrlAccountData(BOOL bIsPost)
{
	if (m_strSuccAccount.GetLength() < 1)
	{
		return ;
	}
	CFileReadAndSave fileSave;
	CStdString strZipBack;
	CString strReponse;
	CString strPostUrl;
	CString strPostData;
	T_LocalGlobalData *pLocalGlobalData = NULL;

	pLocalGlobalData = (T_LocalGlobalData *)g_GlobalDataBlock->ReadRealData(E_LocalGlobalData, LOCALGLOBALDATAINDEX,
		E_GUIDATAIN_CLASS, eUseBlockWithOne);
	strPostData = pLocalGlobalData->szUserID;
	if (strPostData.GetLength() <= 0)
	{
		strPostData = _T("0");
	}
	strZipBack.Format(_T("%s(;1)%s(;1)"), strPostData.GetString(), pLocalGlobalData->aSystemInfo.systemInfo.szCompanName);
	m_strSuccAccount = strZipBack + m_strSuccAccount;

	if (!bIsPost)
	{
		m_strSuccAccount = _T("SaveAccount(;0)")+m_strSuccAccount;
		BYTE * pByData = (BYTE *)m_strSuccAccount.c_str();

		g_Socket->SendData(m_strSuccAccount.size() * 2, E_GET_EXCUTE_TASK_RESULT, (char*)pByData);

		g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("数据传给Yun模块上传:%s"), m_strSuccAccount.c_str());
		return;
	}

	g_Config->ReadString(_T("MC"), _T("DASHIZHENCI"), _T("apiAddress"), strPostUrl);
	strPostUrl = strPostUrl + _T("/api/Member/SaveWebsiteAccount");
	fileSave.ZipEncodeStdString(m_strSuccAccount,strZipBack);

	strPostData = URLEncode(strZipBack.GetBuffer());
	strPostData = _T("result=") + strPostData;

	int iRes = HttpPostData(strPostUrl,strPostData,strReponse);

	if (iRes != 0 || 0 != strReponse.CompareNoCase(_T("ok")))
	{
		g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("提交成功账号失败:%s,提交数据:%s,上层返回信息:%s"),strPostUrl.GetString(),strPostData.GetString(),strReponse.GetString());
	}

	g_SuccAcc.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("注册成功信息：%s，提交上层返回码:%s"), m_strSuccAccount.c_str(), strReponse.GetString());
}

//Post数据
int CTaskManger::HttpPostData(LPCTSTR strUrl,LPCTSTR strPostData, CString &strResponse)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	CInternetSession m_session(IE_AGENT);
	CHttpFile *pFile = NULL;
	CHttpConnection *pConnection = NULL;
	strResponse = _T("");

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return -1;
	}

	try
	{
		m_session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10 * 1000);
		m_session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10 * 1000);
		m_session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 10 * 1000);
		m_session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 10 * 1000);
		m_session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 10 * 1000);

		pConnection = m_session.GetHttpConnection(strServer, nPort);
		pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject.GetString(),
			NULL, 1, NULL, NULL, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD);

		pFile->AddRequestHeaders(_T("Accept: *,*/*"));
		pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
		pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded; charset=utf-8"));

		DWORD dwRet = 0;
		BOOL bRes =FALSE;
		if (strPostData != NULL)
		{
			WCharToMByte((LPWSTR)(strPostData), NULL, &dwRet, CP_UTF8);
			char *pszBuf = new char[dwRet];
			WCharToMByte((LPWSTR)(strPostData), pszBuf, &dwRet, CP_UTF8);
			bRes = pFile->SendRequest(NULL, 0, pszBuf, dwRet);
			delete[]pszBuf;
		}
		else
		{
			bRes = pFile->SendRequest();
		}		

		if (bRes)
		{
			DWORD dwRead = 0;
			char szChars[BUFFER_SIZE + 1] = { 0 };

			CStringA strRawResponse = "";
			do 
			{
				dwRead = pFile->Read(szChars, sizeof(szChars));
				strRawResponse += szChars;
				memset(szChars,0,sizeof(szChars));

			} while (dwRead > 0);

			//UTF8转宽字节
			DWORD dwSize = 0;
			MByteToWChar((LPSTR)(strRawResponse.GetString()), NULL, &dwSize, CP_UTF8);
			WCHAR *pwszBuf = new WCHAR[dwSize];
			MByteToWChar((LPSTR)(strRawResponse.GetString()), pwszBuf, &dwSize, CP_UTF8);

			strResponse = pwszBuf;
			delete[]pwszBuf;
		}

		pFile->Close();
		delete pFile;

		pConnection->Close();
		delete pConnection;

		m_session.Close();		
	}
	catch (...)
	{		
		g_log.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("请求读取数据异常,错误码：%d，目标网址:%s"), GetLastError(), strUrl);
		return -1;
	}

	return 0;
}