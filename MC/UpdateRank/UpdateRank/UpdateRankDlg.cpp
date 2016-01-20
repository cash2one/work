// UpdateRankDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UpdateRank.h"
#include "UpdateRankDlg.h"
#include "afxdialogex.h"
#include <atlimage.h>
#include <afxinet.h>



#define   GETSEARCHDATA     WM_USER + 0x01
#define   GETREDIRECTURL    WM_USER + 0x02


// CUpdateRankDlg �Ի���

IMPLEMENT_DYNAMIC(CUpdateRankDlg, CDialog)

CUpdateRankDlg::CUpdateRankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateRankDlg::IDD, pParent)
	, hThread(NULL)
{
}

CUpdateRankDlg::~CUpdateRankDlg()
{
	if (hThread != NULL)
		CloseHandle(hThread);

	for (int i=0; i<m_vBackData.size(); i++)
	{
		delete m_vBackData[i];
	}

	m_vBackData.clear();
}

void CUpdateRankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_WEB_BROWSER, m_WebBrowser);
}


BEGIN_MESSAGE_MAP(CUpdateRankDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CUpdateRankDlg::OnBnClickedOk)
	ON_MESSAGE(GETSEARCHDATA, &GetSearchData)
	ON_MESSAGE(GETREDIRECTURL, &RedirectURL)
END_MESSAGE_MAP()


// CUpdateRankDlg ��Ϣ�������


BOOL CUpdateRankDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);//����������ȥ��.

	WINDOWPLACEMENT wp;
	wp.length=sizeof(WINDOWPLACEMENT);
	wp.flags=WPF_RESTORETOMAXIMIZED;
	wp.showCmd=SW_HIDE;
	SetWindowPlacement(&wp);
	
	m_WebBrowser.SetSilent(TRUE);
	SetInternetFeature();

	hThread = CreateThread(NULL, NULL, GetUpdateRank, this, 0, NULL);

	if (hThread == NULL)
		OnBnClickedOk();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BEGIN_EVENTSINK_MAP(CUpdateRankDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CPrintScreenDlg)
	ON_EVENT(CUpdateRankDlg, IDC_WEB_BROWSER, 250 /* BeforeNavigate2 */, OnBeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CUpdateRankDlg::OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
	// TODO: Add your control notification handler code here

}


/*
	@brief  ����ҳ�淢����������Ѱ�Ұٶȹȸ���������  �߳�
	@param  lp  thisָ��
	@return 
	*/
DWORD WINAPI CUpdateRankDlg::GetUpdateRank( LPVOID lp )
{
	CUpdateRankDlg *pPhotoDlg = (CUpdateRankDlg*)lp;
	Sleep(5000);
	while(true)
	{
		pKeyWordDataInfo pCurKeyWordDataInfo = GetSearKeyWordInfo();
		if (pCurKeyWordDataInfo == NULL)
			break;
		// ���ڲ���
		// pCurKeyWordDataInfo->iTotoalFlag = 0x40;

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("�ؼ���:%s  ��ʼץȡ���� ������־Ϊ��%d"), pCurKeyWordDataInfo->strKeyWordName, pCurKeyWordDataInfo->iFlag);

		DWORD dwFlagTime;

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_BAIDU) && bSearchFlag[BADIDU_INDEX])  //ץȡ�ٶ�
		{
			EnterCriticalSection(&critCount);
			g_iBaidu++;
			LeaveCriticalSection(&critCount);
			dwFlagTime = GetTickCount();

			for (int i=0; i<3; i++) //����ǰ��ҳ
			{			
				if (g_iDelayForKeyWord != -1)
				{
					Sleep(g_iDelayForKeyWord);
				}
				pCurKeyWordDataInfo->iCurPage = i;
				pCurKeyWordDataInfo->strUrl.Format(_T("http://www.baidu.com/s?wd=%s&pn=%d"), pCurKeyWordDataInfo->strKeyHex.GetString(), i*10);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo))
					break;    //�Ѿ�ץȡ���գ�����ץȡ�����������
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡ�ٶȿ��ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime/1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());
							
		}

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_PHONEBAIDU) && bSearchFlag[PHONEBAIDU_INDEX])  //ץȡ�ֻ��ٶ�
		{
			dwFlagTime = GetTickCount();

			// ֻץ��ҳ
			pCurKeyWordDataInfo->iCurPage = 0;
			pCurKeyWordDataInfo->strUrl.Format(_T("http://wap.baidu.com/s?word=%s&pn=%d"), pCurKeyWordDataInfo->strKeyHex.GetString(), 0);

			pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo);

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡ�ֻ��ٶȿ��ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime/1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());

		}

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_PHONESOGOU) && bSearchFlag[PHONESOGOU_INDEX])  //ץȡ�ֻ��ѹ�
		{
			dwFlagTime = GetTickCount();

			// ֻץ��ҳ
			pCurKeyWordDataInfo->iCurPage = 0;
			pCurKeyWordDataInfo->strUrl.Format(_T("http://wap.sogou.com/web/searchList.jsp?keyword=%s&p=%d"), pCurKeyWordDataInfo->strKeyHex.GetString(), 1);

			pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo);

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡ�ֻ��ѹ����ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime / 1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());

		}

		Sleep(10);

		if ((pCurKeyWordDataInfo->iFlag == SEARCH_360) && bSearchFlag[S360_INDEX])  //ץȡ360
		{
			EnterCriticalSection(&critCount);
			g_i360++;
			LeaveCriticalSection(&critCount);
			dwFlagTime = GetTickCount();

			for (int i=0; i<3; i++) //����ǰ��ҳ
			{				
				if (g_iDelayForKeyWord != -1)
				{
					Sleep(g_iDelayForKeyWord);
				}
				pCurKeyWordDataInfo->iCurPage = i;
				pCurKeyWordDataInfo->strUrl.Format(_T("http://www.haosou.com/s?q=%s&pn=%d"),pCurKeyWordDataInfo->strKeyHex.GetString(),i+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo))
					break;    //�Ѿ�ץȡ���գ�����ץȡ�����������
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡ360���ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime/1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());
		}

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_SOGOU) && bSearchFlag[SOGOU_INDEX])  //ץȡ�ѹ�
		{
			EnterCriticalSection(&critCount);
			g_iSogou++;
			LeaveCriticalSection(&critCount);
			dwFlagTime = GetTickCount();


			for (int i=0; i<3; i++) //����ǰ��ҳ
			{				
				if (g_iDelayForKeyWord != -1)
				{
					Sleep(g_iDelayForKeyWord);
				}
				pCurKeyWordDataInfo->iCurPage = i;
				pCurKeyWordDataInfo->strUrl.Format(_T("http://www.sogou.com/web?query=%s&page=%d"),pCurKeyWordDataInfo->strKeyHex.GetString(),i+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo))
					break;    //�Ѿ�ץȡ���գ�����ץȡ�����������
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡsogou���ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime/1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());
		}

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_BING) && bSearchFlag[BING_INDEX])  //ץȡ��Ӧ
		{
			EnterCriticalSection(&critCount);
			g_iBing++;
			LeaveCriticalSection(&critCount);
			dwFlagTime = GetTickCount();

			for (int i=0; i<3; i++) //����ǰ��ҳ
			{					
				if (g_iDelayForKeyWord != -1)
				{
					Sleep(g_iDelayForKeyWord);
				}
				pCurKeyWordDataInfo->iCurPage = i;
				pCurKeyWordDataInfo->strUrl.Format(_T("http://cn.bing.com/search?q=%s&first=%d"),pCurKeyWordDataInfo->strKeyHex.GetString(),i*10+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo))
					break;    //�Ѿ�ץȡ���գ�����ץȡ�����������
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡBING���ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime/1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());
		}

		Sleep(10);
		if ((pCurKeyWordDataInfo->iFlag == SEARCH_YOUDAO) && bSearchFlag[YOUDAO_INDEX])  //ץȡ�е�
		{
			EnterCriticalSection(&critCount);
			g_iYouDao++;
			LeaveCriticalSection(&critCount);
			dwFlagTime = GetTickCount();

			for (int i = 0; i < 3; i++) //����ǰ��ҳ
			{
				if (g_iDelayForKeyWord != -1)
				{
					Sleep(g_iDelayForKeyWord);
				}
				pCurKeyWordDataInfo->iCurPage = i;
				pCurKeyWordDataInfo->strUrl.Format(_T("http://www.youdao.com/search?q=%s&start=%d"), pCurKeyWordDataInfo->strKeyHex.GetString(), i * 10);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurKeyWordDataInfo))
					break;    //�Ѿ�ץȡ���գ�����ץȡ�����������
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡYOUDAO���ջ���ʱ��,%.2f,��,�ؼ���Ϊ��%s"), dwFlagTime / 1000.0, pCurKeyWordDataInfo->strKeyWordName.GetString());
		}


		if (pCurKeyWordDataInfo != NULL)
		{
			delete pCurKeyWordDataInfo;
			pCurKeyWordDataInfo = NULL;
		}

		//��������
		pPhotoDlg->BackMsg();

	}

	pPhotoDlg->PostMessage(WM_QUIT);

	return 0;

}


#define  IRETRYTIMES    3       //���Դ���
#define  TIMEOUTCOUTS   8      //��ʱʱ��
/*
	@brief  �ȴ�web�ؼ���ҳ��
	@param  sData  
	@return 
	*/
BOOL CUpdateRankDlg::WaitURlAndSaveImageFile(const KeyWordDataInfo &sData)
{	
//	g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ENTER WAITEURL"));
	BOOL bReady = FALSE;
	BOOL bResult = FALSE;
	int  iTimeleft;
	long lMessageBack = -1;

	pKeyWordDataInfo pData = new KeyWordDataInfo();
	try
	{
		if (pData != NULL)
		{
			pData->iCurPage       = sData.iCurPage;
			pData->iFlag          = sData.iFlag;
			pData->strKey         = sData.strKey;
			pData->strKeyHex      = sData.strKeyHex;
			pData->vCompanys      = sData.vCompanys;
			pData->strKeyWordName = sData.strKeyWordName;
			pData->strUrl         = sData.strUrl;
			pData->strWebFlag     = sData.strWebFlag;
			pData->strWebList     = sData.strWebList;
			pData->strComany      = sData.strComany;
			pData->vAllCompanys   = sData.vAllCompanys;

			Sleep(10);
			for (int i = 0; i<IRETRYTIMES; i++)  //���ٱȽ����򲻿���ҳ���������
			{
				iTimeleft = 0;
				int iSend = -2;
				if (IsWindow(this->GetSafeHwnd()))
				{
					iSend = (BOOL)SendMessage(GETREDIRECTURL, 0, (LPARAM)pData);
				}
				else
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("������Ч�������߳�"));
					// ��������ǰ�Ȱ�����ѹ��������
					EnterCriticalSection(&critSection);
					g_vAllKeyWords.push_back(pData);
					LeaveCriticalSection(&critSection);
					// ��������
					ExitThread(-1);
				}
				//������Ҫͬ������Ϣ�����߳�ȥ��������ֹ��ͬ�̲߳������ֱ�������,��Ҫ�����Ч��
				if (iSend == 1)
				{
					do
					{
						if (IsWindow(this->m_hWnd))
						{
							bReady = (BOOL)SendMessage(GETREDIRECTURL, 1, (LPARAM)pData);
						}
						else
						{
							g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("������Ч�������߳�"));
							// ��������ǰ�Ȱ�����ѹ��������
							EnterCriticalSection(&critSection);
							g_vAllKeyWords.push_back(pData);
							LeaveCriticalSection(&critSection);
							// ��������
							ExitThread(-1);
						}

						if (bReady || (iTimeleft > TIMEOUTCOUTS))
						{
							if (iTimeleft > TIMEOUTCOUTS)
							{
								g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��ʱ%d�����Ѽ�����ɴ���,ҳ��Ϊ��%s"), i + 1, sData.strUrl);
								if (IsWindow(this->m_hWnd))
								{
									SendMessage(GETREDIRECTURL, 2, (LPARAM)pData);
								}
								else
								{
									g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("������Ч�������߳�"));
									// ��������ǰ�Ȱ�����ѹ��������
									EnterCriticalSection(&critSection);
									g_vAllKeyWords.push_back(pData);
									LeaveCriticalSection(&critSection);
									// ��������
									ExitThread(-1);
								}
							}

							if (IsWindow(this->m_hWnd))
							{
								lMessageBack = SendMessage(GETSEARCHDATA, 0, (LPARAM)pData);
							}
							else
							{
								g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("������Ч�������߳�"));
								// ��������ǰ�Ȱ�����ѹ��������
								EnterCriticalSection(&critSection);
								g_vAllKeyWords.push_back(pData);
								LeaveCriticalSection(&critSection);
								// ��������
								ExitThread(-1);
							}
							break;
						}

						iTimeleft++;
						Sleep(500);

					} while (1);
				}

				if (lMessageBack != OPEN_PAGE_FAILED)  //ҳ���޷��򿪣���Ҫ����
				{				
					if (lMessageBack == NO_PAIMING)
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("δץȡ������,��ַ:%s"), sData.strUrl);

						if (sData.iCurPage == 0 && sData.iFlag == SEARCH_PHONEBAIDU) // �ֻ��ٶȵ�һҳûץ�����գ��������Ϊ0����Ϣ������
						{
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ֻ��ٶ�δץȡ�����գ��������Ϊ0����Ϣ������"));
							BackDataInfo* pBack = new BackDataInfo();
							pBack->strKeyWordName = sData.strKeyWordName;
							pBack->iFlag = sData.iFlag;
							pBack->strKey = sData.strKey;
							pBack->strCompanyName = sData.strComany;
							m_vBackData.push_back(pBack);
						}

						if (sData.iCurPage == 0 && sData.iFlag == SEARCH_PHONESOGOU) // �ֻ��ѹ���һҳûץ�����գ��������Ϊ0����Ϣ������
						{
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ֻ��ѹ�δץȡ�����գ��������Ϊ0����Ϣ������"));
							BackDataInfo* pBack = new BackDataInfo();
							pBack->strKeyWordName = sData.strKeyWordName;
							pBack->iFlag = sData.iFlag;
							pBack->strKey = sData.strKey;
							pBack->strCompanyName = sData.strComany;
							m_vBackData.push_back(pBack);
						}

						if (sData.iCurPage == 2)  //��ǰ����ҳ
						{
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��ǰ����ҳ�򿪳ɹ�δץȡ�����գ��������Ϊ0����Ϣ������"));
							BackDataInfo* pBack = new BackDataInfo();
							pBack->strKeyWordName = sData.strKeyWordName;
							pBack->iFlag = sData.iFlag;
							pBack->strKey = sData.strKey;
							pBack->strCompanyName = sData.strComany;
							m_vBackData.push_back(pBack);
						}
						bResult = FALSE;  //������ҳ�棬ûץȡ������
					}
				
					if (lMessageBack == PAIMING_EXIST)
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ץȡ�����գ��ؼ���:%s,��ַ:%s"), sData.strKeyWordName, sData.strUrl);
						bResult = TRUE;  //�п���
					}
					if (lMessageBack == ENGINE_APPEAR_VERIFYCODE)  //ҳ�������֤��
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("%s������֤ȡ������"), GetSearchFlag(pData->iFlag));

						EnterCriticalSection(&critSearchFlag);
						if (pData->iFlag == SEARCH_BAIDU)
						{
							bSearchFlag[BADIDU_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_360)
						{
							bSearchFlag[S360_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_SOGOU)
						{
							bSearchFlag[SOGOU_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_BING)
						{
							bSearchFlag[BING_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_PHONEBAIDU)
						{
							bSearchFlag[PHONEBAIDU_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_PHONESOGOU)
						{
							bSearchFlag[PHONESOGOU_INDEX] = FALSE;
						}
						else if (pData->iFlag == SEARCH_YOUDAO)
						{
							bSearchFlag[YOUDAO_INDEX] = FALSE;
						}

						LeaveCriticalSection(&critSearchFlag);
						bResult = TRUE;
					}
					break;
				}
				else
				{
					// ����
					if (i < (IRETRYTIMES - 1))
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("ҳ���޷��򿪣�����,��ַ:%s,������%d"), sData.strUrl, i + 1);
					}
					else
					{
						// ����3���˻�û���ύҳ�����ύ����
					}
				}
			}

			delete pData;
		}
	}
	catch (...)
	{
		if (pData != NULL)
		{
			delete pData;
		}
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("ִ��SendMessage�����쳣��"));
	}
	return bResult;
}

///*
//	@brief  ȡ�ðٶ���Ŀ�߶ȣ�����������
//	@param  ���˱�ǩ����
//	@param  sData  �ؼ����������
//	@param  strFileName  Ҫ�����ͼƬ·��   ���
//	@return ���ط��������ĸ߶�
//	*/
//
//std::vector<long> CUpdateRankDlg::CalBaiduHeight(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pTables == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection baidu is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	for (long l = sData.iCurPage*10 + 1; l <= sData.iCurPage*10 + 10; l++)
//	{
//		IDispatch  *pDisp = NULL;
//		CString strTemp;
//
//		strTemp.Format(_T("%d"), l);
//
//		VARIANT index = {0};
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = 0;
//
//		VARIANT varID = {0};
//		VariantInit(&varID);
//		varID.vt = VT_BSTR;
//
//		varID.bstrVal = strTemp.AllocSysString();
//		hr = pTables->item(varID,index,&pDisp);
//		SysFreeString(varID.bstrVal);
//
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//
//		CComBSTR bstr;
//		CString strPage;
//
//		pElement->get_innerHTML(&bstr);
//		strPage= bstr;	
//
//
//		BOOL bFind = FindIsOwnWebInBaidu(strPage, sData);
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1);
//			{
//				//if (strPage.Find(strCompany.GetString()) == -1)  //����ע�͵������������Ƿ��й�˾����������ַ�鿴
//				{
//					CString strUrl = GetUrlFormContent(strPage);
//					if (strUrl.Find(_T("http:")) == -1)
//						strUrl.Insert(0, _T("http:"));
//
//					int iResult = HasCompanyInHtml(strUrl, sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ����ҳ������쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ����ҳ������쳣,���������¸���Ŀ"));
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//				if(strPage.Find(_T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image:")) == -1)
//				{
//					//�Ӻ��
//					strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//					strPage.Append(_T("</div>"));
//					hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//					if (FAILED(hr))
//					{
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ���Ӻ��ʧ��"));
//					}
//				}
//
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag          = SEARCH_BAIDU;
//					pBdata->iRank          = l;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey         = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(l);
//
//			}
//		}		
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_BAIDU);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//	
//	return vHeights;
//}


/*
@brief  �ڰٶ�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return 
   0��������
   1��������
   2������ҳ����
*/
int  CUpdateRankDlg::GetBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection baidu is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;

	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	for (long l = sData.iCurPage * 10 + 1; l <= sData.iCurPage * 10 + 10; l++)
	{
		IDispatch  *pDisp = NULL;
		CString strTemp;

		strTemp.Format(_T("%d"), l);

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = 0;

		VARIANT varID = { 0 };
		VariantInit(&varID);
		varID.vt = VT_BSTR;

		varID.bstrVal = strTemp.AllocSysString();
		hr = pTables->item(varID, index, &pDisp);
		SysFreeString(varID.bstrVal);

		if (FAILED(hr) || pDisp == NULL)
			continue;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();


		CComBSTR bstr;
		CString strPage;

		pElement->get_innerHTML(&bstr);
		strPage = bstr;


		BOOL bFind = FindIsOwnWebInBaidu(strPage, sData);
		if (bFind)
		{
			//ֻҪ��Ŀ�е�URL�����ǵ���¼��վ�ڣ�����ȥ���ҷ������
			// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1);
			// 			{
			//if (strPage.Find(strCompany.GetString()) == -1)  //����ע�͵������������Ƿ��й�˾����������ַ�鿴
			//{
			CString strUrl = GetUrlFormContent(strPage);
			if (strUrl.Find(_T("http:")) == -1)
				strUrl.Insert(0, _T("http:"));

			int iResult = HasCompanyInHtml(strUrl, sData);
			if (iResult != FOUND_COMPANY)
			{
				if (iResult == OPEN_PAGE_FAILED)
					iPageFailTimes++;
				else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ��û����Ŀ����ҳ���ҵ���˾��"));
					
				continue;
			}
			//}
			if (strPage.Find(_T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image:")) == -1)
			{
				//�Ӻ��
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ���Ӻ��ʧ��"));
				}
			}

			if (vHeights.size() == 0)
			{
				pBdata = new BackDataInfo();
				ASSERT(pBdata != NULL);

				pBdata->iFlag = SEARCH_BAIDU;
				pBdata->iRank = l;
				pBdata->strKeyWordName = sData.strKeyWordName;
				pBdata->strKey = sData.strKey;
				pBdata->strCompanyName = sData.strComany;
			}
			vHeights.push_back(l);

			//}
		}
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_BAIDU);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}

///*
//	@brief  ȡ���ֻ��ٶ���Ŀ�߶ȣ�����������
//	@param  ���˱�ǩ����
//	@param  sData  �ؼ����������
//	@param  strFileName  Ҫ�����ͼƬ·��   ���
//	@return ���ط��������ĸ߶�
//	add by zhoulin
//	*/
//std::vector<long> CUpdateRankDlg::CalPhoneBaiduHeight(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pTables == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection phonebaidu is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//	CComQIPtr<IHTMLAnchorElement> pAnchor;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	int iRank = 0;
//	long lDivCounts = 0;
//	pTables->get_length(&lDivCounts);
//
//	for (long l = 0; l <= lDivCounts; l++)
//	{
//		IDispatch  *pDisp = NULL;
//		IDispatch *pTemp = NULL;
//
//		VARIANT index = {0};
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = l;
//
//		hr = pTables->item(COleVariant(l),index,&pDisp);
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//		CComQIPtr<IHTMLElementCollection> pChild;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//		CComBSTR bstr;
//		CString strPage;
//		pElement->get_innerHTML(&bstr);
//		strPage = bstr;
//
//		if(strPage.Compare(_T("")) == 0)
//			continue;
//
//		if(strPage.Find(_T("class=\"resitem\""), 0) != -1)
//		{
//			continue;
//		}
//
//		pElement->get_children(&pTemp);
//		pChild = pTemp;
//
//		long childCount = 0;
//		pChild->get_length(&childCount);
//
//		if(childCount > 3)
//			continue;
//
//		if(strPage.Find(_T("class=\"abs\""), 0) != -1 || strPage.Find(_T("class=abs"), 0) != -1)
//		{
//			iRank++;
//		}
//
//		CComBSTR bstrURL;
//		CString strUrl = _T("");
//
//		// �滻a��ǩ��href����
//		for(long n = 0; n <= childCount; ++n)
//		{
//			VARIANT indexC = {0};
//			V_VT(&indexC) = VT_I4;
//			V_I4(&indexC) = n;
//
//			CComQIPtr<IHTMLElement> pElementHerf;
//			IDispatch *pTemp2 = NULL;
//			hr = pChild->item(COleVariant(n), indexC, &pTemp2);
//
//			if (FAILED(hr) || pTemp2 == NULL)
//				continue;
//
//			pAnchor = pTemp2;
//
//			if(pAnchor != NULL)
//			{
//				pElementHerf = pTemp2;
//				pAnchor->get_href(&bstrURL);
//				strUrl = bstrURL;
//
//				VARIANT strHref = {0};
//				VariantInit(&strHref);
//				strHref.vt = VT_BSTR;
//				strHref.bstrVal = bstrURL;
//
//				hr = pElementHerf->setAttribute(BSTR(_T("href")), strHref);
//
//				break;
//			}
//		}
//
////		iRank++;
//
//		BOOL bFind = FindIsOwnWebInPhoneBaidu(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				if(strUrl.Compare(_T("")) != 0)
//				{
//					int iResult = HasCompanyInHtml(strUrl, sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//						    bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ����ҳ������쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ����ҳ������쳣,���������¸���Ŀ"));
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//				else
//				{
//					continue;
//				}
//
//			    //�Ӻ��
//				pElement->get_innerHTML(&bstr);
//				strPage = bstr;
//
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//			    strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//				l++;
//				lDivCounts++;
//
//			    if (FAILED(hr))
//				 {
//					 g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ���Ӻ��ʧ��"));
//				 }
//			   
//
//			  if (vHeights.size() == 0)
//			  {
//				  pBdata = new BackDataInfo();
//				  ASSERT(pBdata != NULL);
//
//				  pBdata->iFlag = SEARCH_PHONEBAIDU;
//				  pBdata->iRank = sData.iCurPage*10 + iRank;
//				  pBdata->strKeyWordName = sData.strKeyWordName;
//				  pBdata->strKey = sData.strKey;
//				  pBdata->strCompanyName = sData.strComany;
//			 }
//			 vHeights.push_back(sData.iCurPage*10 + iRank);
//		  }	
//	   }
//	}
//
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_PHONEBAIDU);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//
//
//	return vHeights;
//
//}

/*
@brief  ���ֻ��ٶ�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/
int CUpdateRankDlg::GetPhoneBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection phonebaidu is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLAnchorElement> pAnchor;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lDivCounts = 0;
	pTables->get_length(&lDivCounts);

	for (long l = 0; l <= lDivCounts; l++)
	{
		IDispatch  *pDisp = NULL;
		IDispatch *pTemp = NULL;

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pTables->item(COleVariant(l), index, &pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;
		CComQIPtr<IHTMLElementCollection> pChild;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		CString strPage;
		pElement->get_innerHTML(&bstr);
		strPage = bstr;

		if (strPage.Compare(_T("")) == 0)
			continue;

		if (strPage.Find(_T("class=\"resitem\""), 0) != -1)
		{
			continue;
		}

		pElement->get_children(&pTemp);
		pChild = pTemp;

		long childCount = 0;
		pChild->get_length(&childCount);

		if (childCount > 3)
			continue;

		if (strPage.Find(_T("class=\"abs\""), 0) != -1 || strPage.Find(_T("class=abs"), 0) != -1)
		{
			iRank++;
		}

		CComBSTR bstrURL;
		CString strUrl = _T("");

		// �滻a��ǩ��href����
		for (long n = 0; n <= childCount; ++n)
		{
			VARIANT indexC = { 0 };
			V_VT(&indexC) = VT_I4;
			V_I4(&indexC) = n;

			CComQIPtr<IHTMLElement> pElementHerf;
			IDispatch *pTemp2 = NULL;
			hr = pChild->item(COleVariant(n), indexC, &pTemp2);

			if (FAILED(hr) || pTemp2 == NULL)
				continue;

			pAnchor = pTemp2;

			if (pAnchor != NULL)
			{
				pElementHerf = pTemp2;
				pAnchor->get_href(&bstrURL);
				strUrl = bstrURL;

				VARIANT strHref = { 0 };
				VariantInit(&strHref);
				strHref.vt = VT_BSTR;
				strHref.bstrVal = bstrURL;

				hr = pElementHerf->setAttribute(BSTR(_T("href")), strHref);

				break;
			}
		}

		//		iRank++;

		BOOL bFind = FindIsOwnWebInPhoneBaidu(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				if (strUrl.Compare(_T("")) != 0)
				{
					int iResult = HasCompanyInHtml(strUrl, sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}
				else
				{
					continue;
				}

				//�Ӻ��
				pElement->get_innerHTML(&bstr);
				strPage = bstr;

				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				l++;
				lDivCounts++;

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ���Ӻ��ʧ��"));
				}


				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_PHONEBAIDU;
					pBdata->iRank = sData.iCurPage * 10 + iRank;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(sData.iCurPage * 10 + iRank);
			//}
		}
	}


	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_PHONEBAIDU);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}


///*
//@brief  ȡ���ֻ��ѹ���Ŀ�߶ȣ�����������
//@param  ���˱�ǩ����
//@param  sData  �ؼ����������
//@param  strFileName  Ҫ�����ͼƬ·��   ���
//@return ���ط��������ĸ߶�
//add by zhoulin
//*/
//std::vector<long> CUpdateRankDlg::CalPhoneSougouHeight(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pTables == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection phoneSougou is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//	CComQIPtr<IHTMLAnchorElement> pAnchor;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	int iRank = 0;
//	long lDivCounts = 0;
//	pTables->get_length(&lDivCounts);
//
//	for (long l = 0; l <= lDivCounts; l++)
//	{
//		IDispatch  *pDisp = NULL;
//		IDispatch *pTemp = NULL;
//
//		VARIANT index = { 0 };
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = l;
//
//		hr = pTables->item(COleVariant(l), index, &pDisp);
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//		CComQIPtr<IHTMLElementCollection> pChild;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//		CComBSTR bstr;
//		CString strPage;
//		pElement->get_innerHTML(&bstr);
//		strPage = bstr;
//
//		if (strPage.Compare(_T("")) == 0)
//			continue;
//
//		if (strPage.Find(_T("class=\"resitem\""), 0) != -1)
//		{
//			continue;
//		}
//
//		pElement->get_children(&pTemp);
//		pChild = pTemp;
//
//		long childCount = 0;
//		pChild->get_length(&childCount);
//
//		if (childCount > 4)
//			continue;
//
//		if (strPage.Find(_T("class=\"abs\""), 0) != -1 || strPage.Find(_T("class=abs"), 0) != -1)
//		{
//			iRank++;
//		}
//
//		CComBSTR bstrURL;
//		CString strUrl = _T("");
//
//		// �滻a��ǩ��href����
//		for (long n = 0; n <= childCount; ++n)
//		{
//			VARIANT indexC = { 0 };
//			V_VT(&indexC) = VT_I4;
//			V_I4(&indexC) = n;
//
//			CComQIPtr<IHTMLElement> pElementHerf;
//			IDispatch *pTemp2 = NULL;
//			hr = pChild->item(COleVariant(n), indexC, &pTemp2);
//
//			if (FAILED(hr) || pTemp2 == NULL)
//				continue;
//
//			pAnchor = pTemp2;
//
//			if (pAnchor != NULL)
//			{
//				pElementHerf = pTemp2;
//				pAnchor->get_href(&bstrURL);
//				strUrl = bstrURL;
//
//				VARIANT strHref = { 0 };
//				VariantInit(&strHref);
//				strHref.vt = VT_BSTR;
//				strHref.bstrVal = bstrURL;
//
//				hr = pElementHerf->setAttribute(BSTR(_T("href")), strHref);
//
//				break;
//			}
//		}
//
//		//		iRank++;
//
//		BOOL bFind = FindIsOwnWebInPhoneSougou(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				if (strUrl.Compare(_T("")) != 0)
//				{
//					int iResult = HasCompanyInHtml(strUrl, sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ����ҳ������쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ����ҳ������쳣,���������¸���Ŀ"));
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//				else
//				{
//					continue;
//				}
//
//				//�Ӻ��
//				pElement->get_innerHTML(&bstr);
//				strPage = bstr;
//
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//				strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//				l++;
//				lDivCounts++;
//
//				if (FAILED(hr))
//				{
//					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ���Ӻ��ʧ��"));
//				}
//
//
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag = SEARCH_PHONESOGOU;
//					pBdata->iRank = sData.iCurPage * 10 + iRank;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(sData.iCurPage * 10 + iRank);
//			}
//		}
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_PHONESOGOU);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//
//	return vHeights;
//
//}

/*
@brief  ���ֻ��ѹ�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/
int CUpdateRankDlg::GetPhoneSougouPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection phoneSougou is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLAnchorElement> pAnchor;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lDivCounts = 0;
	pTables->get_length(&lDivCounts);

	for (long l = 0; l <= lDivCounts; l++)
	{
		IDispatch  *pDisp = NULL;
		IDispatch *pTemp = NULL;

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pTables->item(COleVariant(l), index, &pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;
		CComQIPtr<IHTMLElementCollection> pChild;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		CString strPage;
		pElement->get_innerHTML(&bstr);
		strPage = bstr;

		if (strPage.Compare(_T("")) == 0)
			continue;

		if (strPage.Find(_T("class=\"resitem\""), 0) != -1)
		{
			continue;
		}

		pElement->get_children(&pTemp);
		pChild = pTemp;

		long childCount = 0;
		pChild->get_length(&childCount);

		if (childCount > 4)
			continue;

		if (strPage.Find(_T("class=\"abs\""), 0) != -1 || strPage.Find(_T("class=abs"), 0) != -1)
		{
			iRank++;
		}

		CComBSTR bstrURL;
		CString strUrl = _T("");

		// �滻a��ǩ��href����
		for (long n = 0; n <= childCount; ++n)
		{
			VARIANT indexC = { 0 };
			V_VT(&indexC) = VT_I4;
			V_I4(&indexC) = n;

			CComQIPtr<IHTMLElement> pElementHerf;
			IDispatch *pTemp2 = NULL;
			hr = pChild->item(COleVariant(n), indexC, &pTemp2);

			if (FAILED(hr) || pTemp2 == NULL)
				continue;

			pAnchor = pTemp2;

			if (pAnchor != NULL)
			{
				pElementHerf = pTemp2;
				pAnchor->get_href(&bstrURL);
				strUrl = bstrURL;

				VARIANT strHref = { 0 };
				VariantInit(&strHref);
				strHref.vt = VT_BSTR;
				strHref.bstrVal = bstrURL;

				hr = pElementHerf->setAttribute(BSTR(_T("href")), strHref);

				break;
			}
		}

		//		iRank++;

		BOOL bFind = FindIsOwnWebInPhoneSougou(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				if (strUrl.Compare(_T("")) != 0)
				{
					int iResult = HasCompanyInHtml(strUrl, sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}
				else
				{
					continue;
				}

				//�Ӻ��
				pElement->get_innerHTML(&bstr);
				strPage = bstr;

				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				l++;
				lDivCounts++;

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phoneSougou ���Ӻ��ʧ��"));
				}


				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_PHONESOGOU;
					pBdata->iRank = sData.iCurPage * 10 + iRank;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(sData.iCurPage * 10 + iRank);
			//}
		}
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_PHONESOGOU);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;

}

	/*
	@brief  ����bmpͼƬ
	@param  hBitmap  bmpͼƬ����
	@param  lpszFileName  �ļ�·��
	@return ����ɹ�����true
	*/
BOOL CUpdateRankDlg::SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)    
{    
	HDC hDC;    
	//��ǰ�ֱ�����ÿ������ռ�ֽ���    
	int iBits;    
	//λͼ��ÿ������ռ�ֽ���    
	WORD wBitCount;    
	//�����ɫ���С�� λͼ�������ֽڴ�С ��λͼ�ļ���С �� д���ļ��ֽ���    
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;    
	//λͼ���Խṹ    
	BITMAP Bitmap;    
	//λͼ�ļ�ͷ�ṹ    
	BITMAPFILEHEADER bmfHdr;    
	//λͼ��Ϣͷ�ṹ    
	BITMAPINFOHEADER bi;    
	//ָ��λͼ��Ϣͷ�ṹ    
	LPBITMAPINFOHEADER lpbi;    
	//�����ļ��������ڴ�������ɫ����    
	HANDLE fh, hDib, hPal,hOldPal=NULL;    

	//����λͼ�ļ�ÿ��������ռ�ֽ���    
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);    
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);    
	DeleteDC(hDC);    
	if (iBits <= 1)    
		wBitCount = 1;    
	else if (iBits <= 4)    
		wBitCount = 4;    
	else if (iBits <= 8)    
		wBitCount = 8;    
	else   
		wBitCount = 24;    
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);    
	bi.biSize = sizeof(BITMAPINFOHEADER);    
	bi.biWidth = Bitmap.bmWidth;    
	bi.biHeight = Bitmap.bmHeight;    
	bi.biPlanes = 1;    
	bi.biBitCount = wBitCount;    
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = 0;    
	bi.biXPelsPerMeter = 0;    
	bi.biYPelsPerMeter = 0;    
	bi.biClrImportant = 0;    
	bi.biClrUsed = 0;    
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;    
	//Ϊλͼ���ݷ����ڴ�     
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));    
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
	*lpbi = bi;    
	// �����ɫ��      
	hPal = GetStockObject(DEFAULT_PALETTE);    
	if (hPal)    
	{     
		hDC = ::GetDC(NULL);    
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
		RealizePalette(hDC);    
	}    

	// ��ȡ�õ�ɫ�����µ�����ֵ    
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)     
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);    
	//�ָ���ɫ��    
	if (hOldPal)    
	{    
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);    
		RealizePalette(hDC);    
		::ReleaseDC(NULL, hDC);    
	}    
	//����λͼ�ļ�      
	fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,     
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);     

	if (fh == INVALID_HANDLE_VALUE)    
		return FALSE;     
	// ����λͼ�ļ�ͷ     
	bmfHdr.bfType = 0x4D42; // "BM"    
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;    
	bmfHdr.bfSize = dwDIBSize;    
	bmfHdr.bfReserved1 = 0;    
	bmfHdr.bfReserved2 = 0;    
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;    
	// д��λͼ�ļ�ͷ    
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    
	// д��λͼ�ļ���������    
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);    
	//���    
	GlobalUnlock(hDib);    
	GlobalFree(hDib);    
	CloseHandle(fh);    
	return TRUE;    
}

/*
	@brief  ���ݹؼ���ȡ��ͼƬ·��
	@param  strTitle  �ؼ���
	@param  strFile  �ļ�·��  ���
	@param  strComp  ��˾��
	@param  iFlag  ������־
	@return ����ɹ�����true
	*/
BOOL CUpdateRankDlg::GetImageFilePath(const CString &strTitle, const CString &strComp, CString &strFile, int iFlag )
{
	TCHAR szFile[MAX_PATH] = {0};
	strFile.Empty();

	CString strNewTitle = strTitle;
	CString strNewComp = strComp;
	strNewComp.TrimLeft();
	strNewComp.TrimRight();
	ReplaceHtmlChar(strNewComp);
	ReplaceHtmlChar(strNewTitle);

	if (GetModuleFileName(NULL, szFile, MAX_PATH))
	{
		PathAppend(szFile, _T("..\\..\\"));
		//PathAppend(szFile, _T("..\\"));
		
		strFile = szFile;
		if (g_bIsDebug)
		{
			strFile.Append(_T("image\\DEBUG\\"));
		}
		else
		{
			strFile.Append(_T("image\\keyword\\"));
		}		
		strFile.Append(strNewComp);
		strFile.Append(_T("\\"));
		strFile.Append(strNewTitle);
		strFile.Append(_T("_"));
		strFile.Append(strNewComp);

		if (iFlag == SEARCH_BAIDU)
			strFile.Append(_T("_BAIDU.html"));
		else if (iFlag == SEARCH_360)
			strFile.Append(_T("_360.html"));
		else if (iFlag == SEARCH_SOGOU)
			strFile.Append(_T("_SOGOU.html"));
		else if (iFlag == SEARCH_BING)
			strFile.Append(_T("_BING.html"));
		else if (iFlag == SEARCH_PHONEBAIDU)
			strFile.Append(_T("_PHONEBAIDU.html"));
		else if (iFlag == SEARCH_PHONESOGOU)
			strFile.Append(_T("_PHONESOGOU.html"));
		else if (iFlag == SEARCH_YOUDAO)
			strFile.Append(_T("_YOUDAO.html"));

		return TRUE;		
	}

	return FALSE;
}

void CUpdateRankDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ


	CDialog::OnTimer(nIDEvent);
}


void CUpdateRankDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}


/*
@brief  ȡ�ðٶȹȸ����
@param
@param
@return 0��ʾ�������޿���  1��ʾ�п���  2��ʾҳ���޷���   3��ʾ������֤��

*/
LRESULT CUpdateRankDlg::GetSearchData( WPARAM wParam, LPARAM lParam )
{
	LRESULT dwResult = OPEN_PAGE_FAILED;
	HRESULT hret;
	int iPaiMingResult = OPEN_PAGE_FAILED;
	CComPtr<IHTMLDocument2> pDocument2;
	CComPtr<IHTMLElement> pElement;

	CComPtr<IDispatch> pDispath;
	CComPtr<IDispatch> pDispathTemp;

	CComQIPtr<IHTMLElementCollection> pAllColls;
	CComQIPtr<IHTMLElementCollection> pTags;

	KeyWordDataInfo  *pSdata = (KeyWordDataInfo*)lParam;

	try
	{
		do
		{
			pDispath = m_WebBrowser.GetDocument();
			if ( pDispath == NULL ) 
			{
				break;
			}

			hret = pDispath->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument2);
			if (FAILED(hret) ) 
			{
				break;
			}

			hret = pDocument2->get_all(&pAllColls);
			if (FAILED(hret) ) 
			{
				break;
			}

			if (pSdata->iFlag == SEARCH_BAIDU
				|| pSdata->iFlag == SEARCH_SOGOU
				|| pSdata->iFlag == SEARCH_PHONEBAIDU
				|| pSdata->iFlag == SEARCH_PHONESOGOU)
			{
				hret = pAllColls->tags(COleVariant(_T("div")), &pDispathTemp);
			}
			else if (pSdata->iFlag == SEARCH_360
				|| pSdata->iFlag == SEARCH_BING
				|| pSdata->iFlag == SEARCH_YOUDAO)
			{
				hret = pAllColls->tags(COleVariant(_T("li")), &pDispathTemp);
			}

			if (FAILED(hret) ) 
			{
				break;
			}

			pTags = pDispathTemp;

			std::vector<long> vHeight;
			CString strFileName;

			BOOL bCrash = FALSE;
			if (pSdata->iFlag == SEARCH_BAIDU)
			{
				//vHeight = CalBaiduHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetBaiduPhoto(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_PHONEBAIDU)
			{
				//vHeight = CalPhoneBaiduHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetPhoneBaiduPhoto(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_PHONESOGOU)
			{
				//vHeight = CalPhoneSougouHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetPhoneSougouPhoto(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_360)
			{
				//vHeight = Cal360Height(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = Get360Photo(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_SOGOU)
			{
				//vHeight = CalSoGouHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetSoGouPhoto(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_BING)
			{
				//vHeight = CalBingHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetBingPhoto(pTags, *pSdata, strFileName, bCrash);
			}
			if (pSdata->iFlag == SEARCH_YOUDAO)
			{
				//vHeight = CalYouDaoHeight(pTags, *pSdata, strFileName, bCrash);
				iPaiMingResult = GetYouDaoPhoto(pTags, *pSdata, strFileName, bCrash);
			}

			if (iPaiMingResult == NO_PAIMING || iPaiMingResult == OPEN_PAGE_FAILED)
			{
				int iResult = JudgeHtml(pAllColls, pSdata);

				if (iResult == OPEN_PAGE_NORMAL)
				{
					dwResult = NO_PAIMING;  //ҳ�������򿪵�������
				}
				else if (iResult == ENGINE_APPEAR_VERIFYCODE)
				{
					dwResult = ENGINE_APPEAR_VERIFYCODE;  //ҳ�������֤��
				}
				break;
			}

			ChangeButtonStyle(pAllColls, pSdata);

			dwResult = PAIMING_EXIST;
			SaveToHtml(pAllColls, strFileName, pSdata->iFlag);
		}while (0);
	}
	catch(...)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("����htmlʱ�������쳣"));
	}
	

	return dwResult;
}



/*
	@brief  �жϰٶ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
BOOL  CUpdateRankDlg::FindIsOwnWebInBaidu(CString strHtml, const KeyWordDataInfo &sData)
{
	int iPos1,iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();
	CStringA strWebList = sData.strWebList;
	strWebList.MakeLower();


	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(_T("<span"), iPos2);

	while(true)
	{
		Sleep(10);
		if(iPos1 != -1)
		{
			iPos1 = strHtml.Find(_T(">"), iPos1);
			iPos2 = strHtml.Find(_T("</span>"), iPos1);
			if (iPos2 != -1)
			{
				//��ȡ��ַ  14��ʾ<SPAN class=g>����ַ�������
				CString strUrl = strHtml.Mid(iPos1+1, iPos2-iPos1-1);
				int iPos3 = strUrl.Find(_T("/"));

				if (iPos3 == -1)
					iPos3 = strUrl.Find(_T("..."));

				if (iPos3 == -1)
					iPos3 = strUrl.Find(_T("&nbsp;"));

				if (iPos3 != -1)
				{
					strUrl = strUrl.Left(iPos3);
				}

				GetMainDomain(strUrl);

				char *pChar = WideToChar(strUrl.GetString());
				if (pChar != NULL)
				{
					if (strWebList.Find(pChar) != -1)
					{
						bReturn = TRUE;
					}
					delete []pChar;
				}

				if (strUrl.GetLength() > 0)
				{
					if (strUrl.GetAt(0) == '.' && strUrl.GetAt(strUrl.GetLength()-1) == '.')
						break;
				}

			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}

		if (bReturn)
			break;


		iPos1 = strHtml.Find(_T("<span"), iPos2);
	}
	

	return bReturn;

}


/*
	@brief  �ж��ֻ��ٶ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	add by zhoulin
	*/
BOOL CUpdateRankDlg::FindIsOwnWebInPhoneBaidu(CString strHtml, const KeyWordDataInfo &sData)
{
	int iPos1,iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();

	CStringA strWebList = sData.strWebList;
	strWebList.MakeLower();

	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(_T("<span class=\"site\">"), iPos2);

	if (iPos1 == -1)
		iPos1 = strHtml.Find(_T("<span class=site>"), iPos2);

	if(iPos1 != -1)
	{
		iPos1 = strHtml.Find(_T(">"), iPos1);
		iPos2 = strHtml.Find(_T("</span>"), iPos1);

		if(iPos2 != -1)
		{
			CString strUrl = strHtml.Mid(iPos1+1, iPos2-iPos1-1);

			GetMainDomain(strUrl);

			char *pChar = WideToChar(strUrl.GetString());

			if (pChar != NULL)
			{
				if (strWebList.Find(pChar) != -1)
				{
					bReturn = TRUE;
				}
				delete []pChar;
			}
		}
	}

	return bReturn;
}

/*
@brief  �ж��ֻ��ѹ���������ַ�Ƿ���   ��¼����վ
@param  strHtml  ĳ����Ŀ��html����
@return TRUE�����
add by zhoulin
*/
BOOL CUpdateRankDlg::FindIsOwnWebInPhoneSougou(CString strHtml, const KeyWordDataInfo &sData)
{
	int iPos1, iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();

	CStringA strWebList = sData.strWebList;
	strWebList.MakeLower();

	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(_T("<span class=\"site\">"), iPos2);

	if (iPos1 == -1)
		iPos1 = strHtml.Find(_T("<span class=site>"), iPos2);

	if (iPos1 != -1)
	{
		iPos1 = strHtml.Find(_T(">"), iPos1);
		iPos2 = strHtml.Find(_T("</span>"), iPos1);

		if (iPos2 != -1)
		{
			CString strUrl = strHtml.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			GetMainDomain(strUrl);

			char *pChar = WideToChar(strUrl.GetString());

			if (pChar != NULL)
			{
				if (strWebList.Find(pChar) != -1)
				{
					bReturn = TRUE;
				}
				delete[]pChar;
			}
		}
	}

	return bReturn;
}

/*
@brief  �ж�360��������ַ�Ƿ���   ��¼����վ
@param  strHtml  ĳ����Ŀ��html����
@return TRUE�����
add by zhoulin
*/
BOOL  CUpdateRankDlg::FindIsOwnWebIn360(CString strHtml, const KeyWordDataInfo &sData)
{
	int iPos1, iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();

	CStringA strWebList = sData.strWebList;
	strWebList.MakeLower();

	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(_T("<cite>"), iPos2);

	if (iPos1 != -1)
	{
		iPos1 = strHtml.Find(_T(">"), iPos1);
		iPos2 = strHtml.Find(_T("</cite>"), iPos1);

		if (iPos2 != -1)
		{
			CString strUrl = strHtml.Mid(iPos1 + 1, iPos2 - iPos1 - 1);

			GetMainDomain(strUrl);

			char *pChar = WideToChar(strUrl.GetString());

			if (pChar != NULL)
			{
				if (strWebList.Find(pChar) != -1)
				{
					bReturn = TRUE;
				}
				delete[]pChar;
			}
		}
	}

	return bReturn;
}

/*
	@brief  �жϹȸ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
BOOL  CUpdateRankDlg::FindIsOwnWebInGoogle(CString strHtml, const KeyWordDataInfo &sData)
{
	BOOL bReturn = FALSE;

	CStringA strWebList = sData.strWebList;
	strWebList.MakeLower();

	//��ȡ��ַ  14��ʾ<SPAN class=g>����ַ�������
	CString strUrl = GetUrlFormContent(strHtml);

	GetMainDomain(strUrl);

	char *pChar = WideToChar(strUrl.GetString());
	if (pChar != NULL)
	{
		if (strWebList.Find(pChar) != -1)
		{
			bReturn = TRUE;
		}
		delete []pChar;
	}

	return bReturn;

}

LRESULT CUpdateRankDlg::RedirectURL( WPARAM wParam, LPARAM lParam )
{
	//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ENTER REDICURL"));
	BOOL bBusy;
	long lReadyState;
	KeyWordDataInfo *pData = (KeyWordDataInfo*)lParam;

	try
	{
		if (wParam == 0)
		{
			if (pData != NULL)
			{
				VARIANT vFlags;

				VariantInit(&vFlags);
				vFlags.vt=VT_I4;
				vFlags.lVal= navNoHistory|navNoReadFromCache|navNoWriteToCache;		
			//	m_WebBrowser.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
			//	Sleep(10);
				m_WebBrowser.Navigate(pData->strUrl, &vFlags, NULL, NULL, NULL);


				//VariantClear(&vFlags);

				return 1;
			}
		}
		else if (wParam == 1)
		{
			bBusy = m_WebBrowser.GetBusy();
			lReadyState = m_WebBrowser.GetReadyState();	

			if ( !bBusy && lReadyState >= READYSTATE_COMPLETE )
			{
				return 1;
			}
		}
		else if (wParam == 2)
		{
			m_WebBrowser.Stop();
		}
	}
	catch(...)
	{
		return 1;  //�����쳣�����
	}

	

	return 0;
}


///*
//	@brief  ȡ��360��Ŀ�߶ȣ�����������
//	@param  ���˱�ǩ����
//	@param  sData  �ؼ����������
//	@param  strFileName  Ҫ�����ͼƬ·��   ���
//	@return ���ط��������ĸ߶�
//	*/
//std::vector<long> CUpdateRankDlg::Cal360Height( IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pLis == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection 360 is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	int iRank = 0;
//	long lLiCounts = 0;
//	pLis->get_length(&lLiCounts);
//
//	for (long l = 0; l <= lLiCounts; l++)
//	{
//		IDispatch  *pDisp = NULL;
//
//		VARIANT index = {0};
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = l;
//
//		hr = pLis->item(COleVariant(l),index,&pDisp);
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//		CComBSTR bstr;
//		pElement->get_innerHTML(&bstr);
//		CString strPage(bstr);
//
//		strPage.MakeLower();
//
//		if (strPage.Find(_T("<h3 ")) == -1)
//		{
//			continue;
//		}
//
//		if (strPage.Find(_T("res-linkinfo")) == -1)
//		{
//			continue;
//		}
//
//		iRank++;
//
//		BOOL bFind = FindIsOwnWebIn360(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				//if (strPage.Find(strCompany.GetString()) == -1)      //����ע�͵������������Ƿ��й�˾����������ַ�鿴
//				{
//					CString strURL = GetUrlFormContent(strPage);
//					strURL.Replace(_T("&amp;"), _T("&"));
//					strURL = GetRealURL(strURL);
//					int iResult = HasCompanyInHtml(strURL, sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ����ҳ������쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ����ҳ������쳣,���������¸���Ŀ"));
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//
//				//�Ӻ��
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//				strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//				if (FAILED(hr))
//				{
//					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ���Ӻ��ʧ��"));
//				}
//				
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag = SEARCH_360;
//					pBdata->iRank = sData.iCurPage*10 + iRank;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(sData.iCurPage*10 + iRank);
//			}	
//		}			
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_360);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//	
//	return vHeights;
//}

/*
@brief  ���ֻ��ѹ�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/

int CUpdateRankDlg::Get360Photo(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection 360 is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l), index, &pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		pElement->get_innerHTML(&bstr);
		CString strPage(bstr);

		strPage.MakeLower();

		if (strPage.Find(_T("<h3 ")) == -1)
		{
			continue;
		}

		if (strPage.Find(_T("res-linkinfo")) == -1)
		{
			continue;
		}

		iRank++;

		BOOL bFind = FindIsOwnWebIn360(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				//if (strPage.Find(strCompany.GetString()) == -1)      //����ע�͵������������Ƿ��й�˾����������ַ�鿴
				{
					CString strURL = GetUrlFormContent(strPage);
					strURL.Replace(_T("&amp;"), _T("&"));
					strURL = GetRealURL(strURL);
					int iResult = HasCompanyInHtml(strURL, sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}

				//�Ӻ��
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ���Ӻ��ʧ��"));
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_360;
					pBdata->iRank = sData.iCurPage * 10 + iRank;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(sData.iCurPage * 10 + iRank);
		//	}
		}
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_360);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}


///*
//@brief  ץȡ�е���������
//*/
//std::vector<long> CUpdateRankDlg::CalYouDaoHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pLis == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection YOUDAO is NULL"));
//		return vHeights;
//	}
//
//	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAOץȡ"));
//
//	CComQIPtr<IHTMLElement> pElement;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	int iRank = 0;
//	long lLiCounts = 0;
//	pLis->get_length(&lLiCounts);
//
//	for (long l = 0; l <= lLiCounts; l++)
//	{
//		IDispatch  *pDisp = NULL;
//
//		VARIANT index = { 0 };
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = l;
//
//		hr = pLis->item(COleVariant(l), index, &pDisp);
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//		CComBSTR bstr;
//		pElement->get_innerHTML(&bstr);
//		CString strPage(bstr);
//
//		strPage.MakeLower();
//
//		if (strPage.Find(_T("<h3 ")) == -1)
//		{
//			continue;
//		}
//
//		if (strPage.Find(_T("tl")) == -1)
//		{
//			continue;
//		}
//
//		iRank++;
//
//		BOOL bFind = FindIsOwnWebInGoogle(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				//if (strPage.Find(strCompany.GetString()) == -1)      //����ע�͵������������Ƿ��й�˾����������ַ�鿴
//				{
//					CString strUrl = GetUrlFormContent(strPage);
//					if (strUrl.Find(_T("http:")) == -1)
//						strUrl.Insert(0, _T("http:"));
//					int iResult = HasCompanyInHtml(strUrl, sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ����ҳ������쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAO ����ҳ������쳣,���������¸���Ŀ"));
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAO ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//
//				//�Ӻ��
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//				strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//				if (FAILED(hr))
//				{
//					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAO ���Ӻ��ʧ��"));
//				}
//
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag = SEARCH_YOUDAO;
//					pBdata->iRank = sData.iCurPage * 10 + iRank;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(sData.iCurPage * 10 + iRank);
//			}
//		}
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_YOUDAO);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//
//	return vHeights;
//}

/*
@brief  ���е�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/
int CUpdateRankDlg::GetYouDaoPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection YOUDAO is NULL"));
		return iRetValue;
	}

	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAOץȡ"));

	CComQIPtr<IHTMLElement> pElement;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l), index, &pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		pElement->get_innerHTML(&bstr);
		CString strPage(bstr);

		strPage.MakeLower();

		if (strPage.Find(_T("<h3 ")) == -1)
		{
			continue;
		}

		if (strPage.Find(_T("tl")) == -1)
		{
			continue;
		}

		iRank++;

		BOOL bFind = FindIsOwnWebInGoogle(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				//if (strPage.Find(strCompany.GetString()) == -1)      //����ע�͵������������Ƿ��й�˾����������ַ�鿴
				{
					CString strUrl = GetUrlFormContent(strPage);
					if (strUrl.Find(_T("http:")) == -1)
						strUrl.Insert(0, _T("http:"));
					int iResult = HasCompanyInHtml(strUrl, sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAO ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}

				//�Ӻ��
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("YOUDAO ���Ӻ��ʧ��"));
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_YOUDAO;
					pBdata->iRank = sData.iCurPage * 10 + iRank;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(sData.iCurPage * 10 + iRank);
			//}
		}
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_YOUDAO);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}


/*
	@brief  ����urlȡ��
	@param  [in/out]strUrl  ��ַ
	*/
void CUpdateRankDlg::GetMainDomain(CString &strUrl )
{
	int iPos1 = -1;
	int iPos2 = -1;

	TCHAR  szUrl[3][5] = {_T(".com"), _T(".cn"), _T(".net")};

	for(int i=0; i<3; i++)
	{
		iPos2 = strUrl.Find(szUrl[i]);

		if (iPos2 != -1)
		{
			if (i == 1) //�����м���cn���
			{
				if (strUrl.Find(szUrl[i], iPos2+1) != -1)
				{
					iPos2 = strUrl.Find(szUrl[i], iPos2+1);
				}
			}

			strUrl = strUrl.Left(iPos2);
			iPos1 = strUrl.ReverseFind(_T('.'));
			break;
		}
	}
	if (iPos1 != -1)
	{
		strUrl = strUrl.Right(strUrl.GetLength()-iPos1);
		strUrl.Append(_T("."));
	}
}

#define ENCODELENGTH 30

/*
	@brief  ����url �ж������htmlԴ���Ƿ������˾��
	@param  strUrl  ��ַ
	@param  strCompay  ��˾��
	@����ֵ��-1������ҳʧ�� 0��δ�ҵ���˾�� 1���ҵ���˾�� 2���Ҳ�����ҳ��404��
	*/
int CUpdateRankDlg::HasCompanyInHtml( CString &strUrl, const KeyWordDataInfo &sData)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	int iResult = OPEN_PAGE_FAILED;
	if (g_bIsDebug)
	{
		strUrl = g_strDebugUrl;
		CString strMsg = _T("��ʼ���Ե���Url: ") + strUrl;
		AfxMessageBox(strMsg);
	}

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);
	if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����Ŀ����ַ:%sʧ��"), strUrl.GetString());
		return iResult;
	}

	if (g_iDalayForSite != -1)
	{
		Sleep(g_iDalayForSite);
	}

	char *pszTmpBufold = NULL;
	char *pszTmpBufNew = NULL;
	
	for (int i=0; i<IRETRYTIMES; i++)
	{
		CInternetSession m_session(IE_AGENT);
		CHttpFile *pFile = NULL;
		CHttpConnection *pConnection = NULL;
		CString strData;
		strData.Empty();

		try
		{
			//ԭ��Ϊ5��,���ڸ�Ϊ10��
			m_session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT , 10*1000);
			m_session.SetOption(INTERNET_OPTION_SEND_TIMEOUT , 10*1000);
			m_session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT , 10*1000);
			m_session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT , 10*1000);
			m_session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT , 10*1000);
			//m_session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);          // 1������

			pConnection = m_session.GetHttpConnection(strServer, nPort);

			if (pConnection == NULL)
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��Ŀ����ַ:%s����ʧ��"), strUrl.GetString());
				return iResult;
			}

			pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject.GetString(), 0, 1, 0, 0, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD);

			if (pFile != NULL)
			{
				DWORD dwRet = 0;
				//ԭ��Ϊ5��,���ڸ�Ϊ10��
				pFile->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10*1000, 0);
				pFile->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 10*1000, 0);

				pFile->SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 10*1000, 0);
				pFile->SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 10*1000, 0);


				pFile->AddRequestHeaders(_T("Accept: text/html"));
				pFile->AddRequestHeaders(_T("Accept-Language: zh-CN"));
				pFile->AddRequestHeaders(_T("Content-Type:text/html; charset=utf-8"));  //ͳһ��utf-8��ֹ����

				pFile->SendRequest();
				pFile->QueryInfoStatusCode(dwRet);			

				if (dwRet == HTTP_STATUS_OK)
				{
					CString strContentType = _T("");
					if (!pFile->QueryInfo(HTTP_QUERY_CONTENT_TYPE, strContentType))
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��HTTPͷ�л�ȡContentTypeʧ��err��%d����ַ��%s"), GetLastError(), strUrl.GetString());
					}


					DWORD dwRead = 0;
					char  szBufa[8192] = {0};

					DWORD dwTotalRecievedLen = 0;
					DWORD dwOldLen = 0;

					CString strTemp;
					do 
					{
						dwRead = pFile->Read(szBufa, sizeof(szBufa)-1);
						
						if (dwRead > 0)
						{
							dwTotalRecievedLen += dwRead;

							pszTmpBufNew = new char[dwTotalRecievedLen];
							if (pszTmpBufold)
							{
								memcpy(pszTmpBufNew, pszTmpBufold, dwOldLen);
								delete pszTmpBufold;
								pszTmpBufold = NULL;
							}

							memcpy(pszTmpBufNew + dwOldLen, szBufa, dwRead);
							pszTmpBufold = pszTmpBufNew;
							dwOldLen = dwTotalRecievedLen;
						}
					} while (dwRead > 0);

					strTemp = pszTmpBufNew;

					int iPos1 = strTemp.Find(_T("charset="));
					if (iPos1 != -1)
					{
						strContentType = strTemp.Mid(iPos1, ENCODELENGTH);
						g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("����ҳ���ҵ�charset=������ֵ��%s  ��ַ��%s"), strContentType.GetString(), strUrl.GetString());
					}
					else if (strContentType != _T(""))
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ҳ���Ҳ���charset=������ʹ��httpЭ��ָ���ı����ʽ��ֵ��%s ��ַ��%s"), strContentType.GetString(), strUrl.GetString());
					}
					else
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ҳ��httpЭ��ͷ�ж��Ҳ���charset=���������ֱ���gbk��UTF8���������ҳ��%s"), strUrl.GetString());
					}
					
					if (dwTotalRecievedLen > 0)
					{
						GetPageData(pszTmpBufNew, dwTotalRecievedLen, strContentType, strData);
					}									
					
					if (pszTmpBufNew)
					{
						delete pszTmpBufNew;
						pszTmpBufNew = NULL;
					}
				}
				else 
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("����ҳʧ�ܣ�״̬�룺%d, ��ַ��%s"), dwRet, strUrl.GetString());
					if (dwRet == HTTP_STATUS_NOT_FOUND)
					{
						i = IRETRYTIMES;//  ���ﲻ����
						iResult = PAGE_NOT_FOUND;
					}
				}

				pFile->Close();
				delete pFile;
			}

			pConnection->Close();
			delete pConnection;

			m_session.Close();
		}
		catch (...)
		{
			if (pszTmpBufNew)
				delete pszTmpBufNew;

			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�����ȡ�����쳣,�����룺%d��Ŀ����ַ:%s"), GetLastError(), strUrl.GetString());
			return OPEN_PAGE_FAILED;
		}

		if (strData.GetLength() > 0)
		{
			int iSize = sData.vCompanys.size();
			iResult = NOT_FOUND_COMPANY;        //Ĭ��δ�ҵ�

			for (int i=0; i<iSize; i++)
			{
				if (strData.Find(sData.vCompanys[i].GetString()) != -1)
				{
					iResult = FOUND_COMPANY;  //�ҵ�
					break;
				}
			}

			break;
		}
		else
		{
			if (i != IRETRYTIMES)  //��404���
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("����Ŀ����ַ:%sʧ��,��ǰ����:%d"), strUrl.GetString(), i+1);

				if (i == IRETRYTIMES - 1)
				{
					iResult = OPEN_PAGE_FAILED;
				//	bSuccess = FALSE;
					g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("��ǰ����ַʧ�ܣ�%s,�ؼ��ʣ�%s,����������ַ��%s"), strUrl.GetString(), sData.strKeyWordName.GetString(), sData.strUrl.GetString());
				}
			}			
		}

		Sleep(200);
	}
	

	return iResult;
}

/*
@brief  ����ҳ����ת��Ϊָ���ı����ʽ������
@param  pszRawData  ���յ�����ҳԴ����
@param  dwRawDataLen Դ���ݳ���
@param  strContentType �����������͵��ַ���
@return ָ������������
*/
int CUpdateRankDlg::GetPageData(const char *pszRawData, DWORD dwRawDataLen, CString &strContentType, CString &strRetData)
{
	strContentType.MakeLower();
	
	if (strContentType.Find(_T("gb2312")) != -1
		|| strContentType.Find(_T("gbk")) != -1)
	{
		strRetData = ToWideCharString(pszRawData, dwRawDataLen, CP_ACP);
	}
	else if (strContentType.Find(_T("utf-8")) != -1)
	{
		strRetData = ToWideCharString(pszRawData, dwRawDataLen, CP_UTF8);
	}
	else
	{
		strRetData = ToWideCharString(pszRawData, dwRawDataLen, CP_ACP);
		strRetData += ToWideCharString(pszRawData, dwRawDataLen, CP_UTF8);
	}

	return 1;
}

/*
@brief  ����ҳ����ת��Ϊָ���ı����ʽ������
@param  pszRawData  Ҫת��������
@param  dwRawDataLen ���ݳ���
@param  codePage ת������
@return ָ������������
*/
CString CUpdateRankDlg::ToWideCharString(const char *pszRawData, DWORD dwRawDataLen, DWORD codePage)
{
	wchar_t *pwszBuf = NULL;
	CString strRetData = _T("");
	int len = MultiByteToWideChar(codePage, 0, pszRawData, -1, NULL, 0);
	pwszBuf = new wchar_t[len + 1];
	MultiByteToWideChar(codePage, 0, pszRawData, -1, pwszBuf, len);
	
	strRetData = CString(W2T(pwszBuf));
	delete []pwszBuf;

	return strRetData;
}

/*
	@brief  ���ݰٶ���ĿhtmlԴ��ȡ�ö�Ӧ����url
	@param  strPage  Դ��
	*/
CString CUpdateRankDlg::GetUrlFormContent(const CString &strPage )
{
	CString strUrl;
	int iPos1 = strPage.Find(_T("href="));
	int iPos2 = -1;
// 	if (iPos1 != -1)
// 		iPos1 = strPage.Find(_T("href="), iPos1 + 6);

	if (iPos1 != -1)
	{
		iPos2 = strPage.Find(_T('\"'), iPos1 + 6);  //��6  href="  ����6���ַ�
		if (iPos2 != -1 && iPos2 > iPos1+6)
		{
			strUrl = strPage.Mid(iPos1 + 6, iPos2-iPos1-6);
		}
	}

	return strUrl;
}


/*
	@brief  ��ץȡ�����ݷ��ظ��ϲ�
	*/
void CUpdateRankDlg::BackMsg()
{
	
	if (m_vBackData.size() > 0)
	{
		int i = 0;
		BOOL bHasData = FALSE;

		CStdString strResult = _T("");
		for (i = 0; i < m_vBackData.size(); i++)
		{
			Sleep(1);
			BackDataInfo *pBackData = m_vBackData[i];
			if (pBackData != NULL)
			{
				if (!pBackData->strPagePath.IsEmpty())
				{
					CString strFileName = PathFindFileName(pBackData->strPagePath);
					CString strConName = pBackData->strCompanyName;
					ReplaceHtmlChar(strConName);
					pBackData->strPagePath = strConName + _T("/") + strFileName;
				}
				if (pBackData->strPagePath.IsEmpty() && pBackData->iRank > 0)
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("���ݳ���!�ؼ���Ϊ��%s,��������Ϊ:%s"), pBackData->strKeyWordName, GetSearchFlag(pBackData->iFlag));
					delete pBackData;
					continue;
				}
				strResult.Format(_T("%s%s(;0)%s(;0)%s(;0)%d(;0)%s(;1)")
					, strResult.c_str()
					, pBackData->strKey
					, pBackData->strKeyWordName
					, GetSearchFlag(pBackData->iFlag)
					, pBackData->iRank
					, pBackData->strPagePath);

				bHasData = TRUE;
				delete pBackData;
				pBackData = NULL;
			}
		}
		if (bHasData)
		{
//			strResult = strResult.Left(strResult.length()-4);
			BYTE * pByData = (BYTE *)strResult.c_str();

			EnterCriticalSection(&critSendMsg);
			g_Socket->SendData(strResult.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
			LeaveCriticalSection(&critSendMsg);

			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("���ؿ������ݣ�%s"), strResult.c_str());
		}

		m_vBackData.clear();
	}
}


///*
//	@brief  ȡ���ѹ���Ŀ�߶ȣ�����������
//	@param  ���˱�ǩ����
//	@param  sData  �ؼ����������
//	@param  strFileName  Ҫ�����ͼƬ·��   ���
//	@return ���ط��������ĸ߶�
//	*/
//std::vector<long> CUpdateRankDlg::CalSoGouHeight( IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pTables == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection sogou is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	for (long l = 1; l <= 10; l++)
//	{
//		IDispatch  *pDisp = NULL;
//		CString strTemp;
//
//		strTemp.Format(_T("rb_%d"), l-1);
//
//		VARIANT index = {0};
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = 0;
//
//		VARIANT varID = {0};
//		VariantInit(&varID);
//		varID.vt = VT_BSTR;
//
//		varID.bstrVal = strTemp.AllocSysString();
//		hr = pTables->item(varID,index,&pDisp);
//		SysFreeString(varID.bstrVal);
//
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//
//		CComBSTR bstr;
//		pElement->get_innerHTML(&bstr);
//		CString strPage(bstr);
//
//		BOOL bFind = FindIsOwnWebInSoGou(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				//if (strPage.Find(strCompany.GetString()) == -1)    //����ע�͵������������Ƿ��й�˾����������ַ�鿴
//				{
//					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//						//	g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ����ҳ������쳣������"));
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ����ҳ������쳣,���������¸���Ŀ"));
//						//	break;
//							continue;
//						}
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//
//				//�Ӻ��
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//				strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//
//				if (FAILED(hr))
//				{
//					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ���Ӻ��ʧ��"));
//				}
//
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag = SEARCH_SOGOU;
//					pBdata->iRank = sData.iCurPage*10 + l;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(l);
//		
//				
//			}
//		}		
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_SOGOU);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}	
//
//	return vHeights;
//}

/*
@brief  ���ѹ�ҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/
int CUpdateRankDlg::GetSoGouPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection sogou is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	for (long l = 1; l <= 10; l++)
	{
		IDispatch  *pDisp = NULL;
		CString strTemp;

		strTemp.Format(_T("rb_%d"), l - 1);

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = 0;

		VARIANT varID = { 0 };
		VariantInit(&varID);
		varID.vt = VT_BSTR;

		varID.bstrVal = strTemp.AllocSysString();
		hr = pTables->item(varID, index, &pDisp);
		SysFreeString(varID.bstrVal);

		if (FAILED(hr) || pDisp == NULL)
			continue;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();


		CComBSTR bstr;
		pElement->get_innerHTML(&bstr);
		CString strPage(bstr);

		BOOL bFind = FindIsOwnWebInSoGou(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				//if (strPage.Find(strCompany.GetString()) == -1)    //����ע�͵������������Ƿ��й�˾����������ַ�鿴
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}

				//�Ӻ��
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ���Ӻ��ʧ��"));
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_SOGOU;
					pBdata->iRank = sData.iCurPage * 10 + l;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(l);


			//}
		}
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_SOGOU);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}


/*
	@brief  �ж��ѹ�����ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
BOOL CUpdateRankDlg::FindIsOwnWebInSoGou(CString strHtml, const KeyWordDataInfo &sData)
{
	return FindIsOwnWebInGoogle(strHtml, sData);
}

/*
	@brief ����������־�õ�������������
	@param iFlag  ������־
	@return ������������
	*/
CString CUpdateRankDlg::GetSearchFlag( int iFlag )
{
	CString strSearch;

	switch(iFlag)
	{
	case SEARCH_BAIDU:       //�ٶ�
		strSearch = _T("�ٶ�");
		break;
	case SEARCH_360:       //360����
		strSearch = _T("360����");
		break;
	case SEARCH_SOGOU:       //�ѹ�
		strSearch = _T("�ѹ�");
		break;
	case SEARCH_BING:       //��Ӧ
		strSearch = _T("��Ӧ");
		break;
	case SEARCH_PHONEBAIDU:
		strSearch = _T("�ֻ��ٶ�");
		break;
	case SEARCH_PHONESOGOU:
		strSearch = _T("�ֻ��ѹ�");
		break;
	case SEARCH_YOUDAO:
		strSearch = _T("�е�");
		break;
	}

	return strSearch;
}

/*
@brief  360�İ�󷵻ص�URL����360����ҳ�淵�ص�URL�����ҵ�������URL
@param  strURL  360����ҳ��õ���URL
@return ����������URL(URL�����)
*/
CString CUpdateRankDlg::GetRealURL(CString strURL)
{
	CString strTemp = strURL;
	int iPos1, iPos2;
	iPos1 = iPos2 = 0;
	iPos1 = strTemp.Find(_T("url="), iPos2);

	if (iPos1 != -1)
	{
		iPos2 = strTemp.Find(_T("&q"), iPos1);

		if (iPos2 != -1)
		{
			strTemp = strTemp.Mid(iPos1 + 4, iPos2 - iPos1 - 4);
			strTemp = UrlDecode(strTemp.GetBuffer());
		}

	}

	return strTemp;
}

///*
//	@brief  ȡ�ñ�Ӧ��Ŀ�߶ȣ�����������
//	@param  ���˱�ǩ����
//	@param  sData  �ؼ����������
//	@param  strFileName  Ҫ�����ͼƬ·��   ���
//	@return ���ط��������ĸ߶�
//	*/
//std::vector<long> CUpdateRankDlg::CalBingHeight( IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
//{
//	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
//	if (pLis == NULL)
//	{
//		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection bing is NULL"));
//		return vHeights;
//	}
//
//	CComQIPtr<IHTMLElement> pElement;
//	BackDataInfo *pBdata = NULL;
//	HRESULT hr;
//
//	int iRank = 0;
//	long lLiCounts = 0;
//	pLis->get_length(&lLiCounts);
//
//	for (long l = 0; l <= lLiCounts; l++)
//	{
//		IDispatch  *pDisp = NULL;
//
//		VARIANT index = {0};
//		V_VT(&index) = VT_I4;
//		V_I4(&index) = l;
//
//		hr = pLis->item(COleVariant(l),index,&pDisp);
//		if (FAILED(hr) || pDisp == NULL)
//			continue;
//
//		Sleep(30);
//		pElement = pDisp;
//		pDisp->Release();
//
//		CComBSTR bstr;
//		pElement->get_outerHTML(&bstr);
//		CString strPage(bstr);
//
//		strPage.MakeLower();
//
//		if (strPage.Find(_T("b_algo")) == -1)
//		{
//			continue;
//		}
//
//		bstr.~CComBSTR();
//		pElement->get_innerHTML(&bstr);
//
//		strPage = bstr;
//		iRank++;
//
//		BOOL bFind = FindIsOwnWebInBing(strPage, sData);
//
//		if (bFind)
//		{
//			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
//			{
//				//if (strPage.Find(strCompany.GetString()) == -1)  //����ע�͵������������Ƿ��й�˾����������ַ�鿴
//				{
//					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
//					if (iResult != 1)
//					{
//						if (iResult == 2)
//						{
//							bResult = TRUE;
//							/*g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ����ҳ���쳣������"));
//							break;*/
//							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ����ҳ���쳣,���������¸���Ŀ"));
//							continue;
//						}
//
//						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ��û����Ŀ����ַ���ҵ���˾��"));
//						continue;
//					}
//				}
//
//				//�Ӻ��
//				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
//				strPage.Append(_T("</div>"));
//				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
//				
//				if (FAILED(hr))
//				{
//					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ���Ӻ��ʧ��"));
//				}
//			
//				if (vHeights.size() == 0)
//				{
//					pBdata = new BackDataInfo();
//					ASSERT(pBdata != NULL);
//
//					pBdata->iFlag = SEARCH_BING;
//					pBdata->iRank = sData.iCurPage*10 + iRank;
//					pBdata->strKeyWordName = sData.strKeyWordName;
//					pBdata->strKey = sData.strKey;
//					pBdata->strCompanyName = sData.strComany;
//				}
//				vHeights.push_back(sData.iCurPage*10 + iRank);
//
//			}
//		}	
//	}
//
//	if (pBdata != NULL)
//	{
//		CString strCompany = sData.strComany;
//
//		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_BING);
//
//		pBdata->strPagePath = strFileName;
//		m_vBackData.push_back(pBdata);
//	}
//
//	return vHeights;
//}

/*
@brief  �ڱ�Ӧҳ����ץȡ����
@param  ���˱�ǩ����
@param  sData  �ؼ����������
@param  strFileName  Ҫ�����ͼƬ·��   ���
@return	0��������	1��������	2������ҳ����
*/
int CUpdateRankDlg::GetBingPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult)
{
	int iRetValue = OPEN_PAGE_FAILED;
	int iPageFailTimes = 0;
	std::vector<long> vHeights;  //���ظ߶ȣ������ж�������
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection bing is NULL"));
		return iRetValue;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackDataInfo *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = { 0 };
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l), index, &pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;

		Sleep(30);
		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		pElement->get_outerHTML(&bstr);
		CString strPage(bstr);

		strPage.MakeLower();

		if (strPage.Find(_T("b_algo")) == -1)
		{
			continue;
		}

		bstr.~CComBSTR();
		pElement->get_innerHTML(&bstr);

		strPage = bstr;
		iRank++;

		BOOL bFind = FindIsOwnWebInBing(strPage, sData);

		if (bFind)
		{
// 			if (strPage.Find(sData.strKeyWordName.GetString()) != -1)
// 			{
				//if (strPage.Find(strCompany.GetString()) == -1)  //����ע�͵������������Ƿ��й�˾����������ַ�鿴
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != FOUND_COMPANY)
					{
						if (iResult == OPEN_PAGE_FAILED)
							iPageFailTimes++;
						else if (iResult == NOT_FOUND_COMPANY || iResult == PAGE_NOT_FOUND)
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ��û����Ŀ����ҳ���ҵ���˾��"));

						continue;
					}
				}

				//�Ӻ��
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ���Ӻ��ʧ��"));
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackDataInfo();
					ASSERT(pBdata != NULL);

					pBdata->iFlag = SEARCH_BING;
					pBdata->iRank = sData.iCurPage * 10 + iRank;
					pBdata->strKeyWordName = sData.strKeyWordName;
					pBdata->strKey = sData.strKey;
					pBdata->strCompanyName = sData.strComany;
				}
				vHeights.push_back(sData.iCurPage*10 + iRank);

			//}
		}	
	}

	if (pBdata != NULL)
	{
		CString strCompany = sData.strComany;
		iRetValue = PAIMING_EXIST;
		GetImageFilePath(sData.strKeyWordName, strCompany, strFileName, SEARCH_BING);

		pBdata->strPagePath = strFileName;
		m_vBackData.push_back(pBdata);
	}
	else
	{
		//ȷ��������
		if (iPageFailTimes == 0)
		{
			iRetValue = NO_PAIMING;
		}

		//����������ҳ��ʧ��ʱ������������Ϣ
	}

	return iRetValue;
}

/*
	@brief  �ж�Bing��������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
BOOL CUpdateRankDlg::FindIsOwnWebInBing(CString strHtml, const KeyWordDataInfo &sData)
{
	return FindIsOwnWebInGoogle(strHtml, sData);
}


/*
	@brief ��html���浽һ���ļ�
	@param pHtml  html����
	@param strFileName  html�ļ�·��
	*/
void CUpdateRankDlg::SaveToHtml( IHTMLElementCollection *pHtml, const CString &strFileName, int iFlag)
{

	if (iFlag == SEARCH_YOUDAO)
	{
		SaveToHtmlYouDao(pHtml, strFileName);
		return;
	}
	CComQIPtr<IDispatch> pDispatHtml;
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElementCollection> pTags;

	pHtml->tags(COleVariant(_T("html")), &pDispatHtml);
	pTags = pDispatHtml;

	VARIANT index = { 0 };
	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;

	::DeleteFile(strFileName);

	if (pTags != NULL)
	{
		pTags->item(index, index, &pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			CComBSTR bstr;

			pElement->get_outerHTML(&bstr);
			FILE *pFile = _wfopen(strFileName, _T("w+"));

			if (pFile != NULL)
			{
				char *pA = WideToChar(bstr, CP_UTF8);

				if (IsNeedChange(iFlag))
				{
					string strHTML = pA;
					string strCss = "";

					ChangeData(iFlag, strCss);
					RemoveAllScriptTag(strHTML);
					ReplaceStyleToCss(strHTML, strCss.c_str());
					fwrite(strHTML.c_str(), 1, strHTML.length(), pFile);
				}
				else
				{
					if (pA)
					{
						CStringA strHtml = pA;
						fwrite(strHtml.GetBuffer(), 1, strHtml.GetLength(), pFile);
					}
				}
				delete[]pA;
				fclose(pFile);
			}
		}
	}

	/*CComQIPtr<IDispatch> pDispatHtml;
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElementCollection> pTags;

	pHtml->tags(COleVariant(_T("html")), &pDispatHtml);
	pTags = pDispatHtml;

	VARIANT index = {0};
	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;

	::DeleteFile(strFileName);

	if (pTags != NULL)
	{
		pTags->item(index, index,&pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			CComBSTR bstr;

			pElement->get_outerHTML(&bstr);
			FILE *pFile = _wfopen(strFileName, _T("w+"));

			if (pFile != NULL)
			{
				char *pA = WideToChar(bstr, CP_UTF8);

				if (IsNeedChange(iFlag))
				{
					string strHTML = pA;
					string strCss = "";

					ChangeData(iFlag, strCss);
					RemoveAllScriptTag(strHTML);
					ReplaceStyleToCss(strHTML, strCss.c_str());
					fwrite(strHTML.c_str(), 1, strHTML.length(), pFile);
				}
				else
				{
					if (pA)
					{
						CStringA strHtml = pA;
						if (iFlag == SEARCH_YOUDAO)
						{
							strHtml.Replace("/css/tgl.css", "../css/tgl.css");
							strHtml.Replace("/css/left_nav.css", "../css/left_nav.css");
						}
						fwrite(strHtml.GetBuffer(), 1, strHtml.GetLength(), pFile);
					}
				}
				delete[]pA;
				fclose(pFile);
			}
		}
	}*/
	
}

void CUpdateRankDlg::SaveToHtmlYouDao(IHTMLElementCollection *pHtml, const CString &strFileName)
{
	CComQIPtr<IDispatch> pDispatHtml;
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElementCollection> pTags;
	CComBSTR bstr;
	CComBSTR bstrdIV;
	CStringA strHtml = "<html>";
	pHtml->tags(COleVariant(_T("head")), &pDispatHtml);
	pTags = pDispatHtml;

	VARIANT index = { 0 };
	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;
	long icount = 0;

	::DeleteFile(strFileName);

	if (pTags != NULL)
	{
		pTags->item(index, index, &pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			pElement->get_outerHTML(&bstr);
		}
	}
	pHtml->tags(COleVariant(_T("div")), &pDispatHtml);
	pTags = pDispatHtml;
	if (pTags != NULL)
	{
		pTags->item(index, index, &pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			pElement->get_outerHTML(&bstrdIV);
		}
	}
	char *pA = WideToChar(bstr, CP_UTF8);
	char *pDiv = WideToChar(bstrdIV, CP_UTF8);
	strHtml.Append(pA);
	strHtml.Append("<body>");
	strHtml.Append(pDiv);
	strHtml.Append("</body>");
	strHtml.Append("</html>");

	FILE *pFile = _wfopen(strFileName, _T("w+"));

	if (pFile != NULL)
	{
		strHtml.Replace("/css/tgl.css", "../../css/tgl.css");
		strHtml.Replace("/css/left_nav.css", "../../css/left_nav.css");
		fwrite(strHtml.GetBuffer(), 1, strHtml.GetLength(), pFile);
	}
	delete[]pA;
	delete[]pDiv;
	fclose(pFile);
}

void CUpdateRankDlg::ChangeButtonStyle( IHTMLElementCollection *pHtml, const KeyWordDataInfo *pKeyWordDataInfo)
{
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CString strUrl;
	CString strBtn;
	CComBSTR bstr;

	VARIANT index = {0};
	VARIANT varID = {0};

	strUrl.Format(_T(" onclick=\"javascript:location.href='%s'\" "), pKeyWordDataInfo->strUrl);

	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;

	VariantInit(&varID);
	varID.vt = VT_BSTR;

	if (pKeyWordDataInfo->iFlag == SEARCH_BAIDU)
	{
		varID.bstrVal = _T("su");
	}
	else if (pKeyWordDataInfo->iFlag == SEARCH_360)
	{
		varID.bstrVal = _T("su");
	}
	else if (pKeyWordDataInfo->iFlag == SEARCH_SOGOU)
	{
		varID.bstrVal = _T("searchBtn");		
	}
	else if (pKeyWordDataInfo->iFlag == SEARCH_BING)
	{
		varID.bstrVal = _T("sb_form_go");
	}
	else if (pKeyWordDataInfo->iFlag == SEARCH_PHONEBAIDU)
	{
		varID.bstrVal = _T("su");
	}
	else if (pKeyWordDataInfo->iFlag == SEARCH_PHONESOGOU)
	{
		varID.bstrVal = _T("s_1");
	}
	else
	{
		varID.bstrVal = _T("NULL");
	}

	pHtml->item(varID,index,&pDispatTemp);
	pElement = pDispatTemp;

	if (pElement != NULL)
	{
		pElement->get_outerHTML(&bstr);

		strBtn = bstr;
		strBtn.Replace(_T("submit"), _T("button"));
		strBtn.Insert(6, strUrl);


		BSTR pStr = strBtn.AllocSysString();
		
		if (FAILED(pElement->put_outerHTML(pStr)))
			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�޸İ�ť��ʽʧ��  type:%d"), pKeyWordDataInfo->iFlag);

		SysFreeString(pStr);
	}
	else
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("ȡ������Ӧ��ťԪ��  type:%d"), pKeyWordDataInfo->iFlag);
	}

}


void CUpdateRankDlg::SetInternetFeature(void)
{
	LRESULT lr = 0;
	INTERNETFEATURELIST featureToEnable = FEATURE_RESTRICT_FILEDOWNLOAD ;

	//��ֹ�����ļ�
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("����FEATURE_RESTRICT_FILEDOWNLOADʧ��,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("����FEATURE_RESTRICT_FILEDOWNLOADʧ��,err:%d"), GetLastError());
	}

	//��ֹ��ҳ�������ύ����ʱ��������
	featureToEnable = FEATURE_DISABLE_NAVIGATION_SOUNDS;
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("����FEATURE_DISABLE_NAVIGATION_SOUNDSʧ��,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("����FEATURE_DISABLE_NAVIGATION_SOUNDSʧ��,err:%d"), GetLastError());
	}
}

#define HTMLLENGTH  12000
int CUpdateRankDlg::JudgeHtml( IHTMLElementCollection *pHtml, const KeyWordDataInfo *pKeyWordDataInfo )
{
	int iResult = OPEN_PAGE_FAILED;
	CString strPage;
	CComQIPtr<IDispatch> pDispatHtml;
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElementCollection> pTags;

	pHtml->tags(COleVariant(_T("html")), &pDispatHtml);
	pTags = pDispatHtml;

	VARIANT index = {0};
	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;

	Sleep(50);

	if (pTags != NULL)
	{
		pTags->item(index, index,&pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			CComBSTR bstr;

			pElement->get_outerHTML(&bstr);
			
			strPage = bstr;
			int iContentLen = strPage.GetLength();
			if (iContentLen < HTMLLENGTH)
			{
				iResult = 1;
				g_pLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ҳ���html�ַ�������Ϊ��%d, С��%d, ������ҳ��, ��ַΪ��%s"), iContentLen, HTMLLENGTH, pKeyWordDataInfo->strUrl.GetString());
				g_pLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ҳHTML����Ϊ(��10000���ַ�����%s"), strPage.Right(10000).GetString());

				if (strPage.Find(_T("��֤��")) != -1)
				{
					iResult = ENGINE_APPEAR_VERIFYCODE;
					if (pKeyWordDataInfo->iFlag == SEARCH_SOGOU)
					{
						g_pLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ѹ�������֤������,��ǰ�������泷������, ��ַΪ:%s,��ǰ��������ؼ��ָ���Ϊ:%d"), pKeyWordDataInfo->strUrl.GetString(), g_iSogou);
					}
					if (pKeyWordDataInfo->iFlag == SEARCH_BAIDU)
					{
						g_pLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�ٶȷ�����֤������,��ǰ�������泷������, ��ַΪ:%s,��ǰ��������ؼ��ָ���Ϊ:%d"), pKeyWordDataInfo->strUrl.GetString(), g_iBaidu);
					}
					if (pKeyWordDataInfo->iFlag == SEARCH_360)
					{
						g_pLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360������֤������,��ǰ�������泷������, ��ַΪ:%s,��ǰ��������ؼ��ָ���Ϊ:%d"), pKeyWordDataInfo->strUrl.GetString(), g_i360);
					}
					if (pKeyWordDataInfo->iFlag == SEARCH_BING)
					{
						g_pLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("��Ӧ������֤������,��ǰ�������泷������, ��ַΪ:%s,��ǰ��������ؼ��ָ���Ϊ:%d"), pKeyWordDataInfo->strUrl.GetString(), g_iBing);
					}
					if (pKeyWordDataInfo->iFlag == SEARCH_YOUDAO)
					{
						g_pLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("�е�������֤������,��ǰ�������泷������, ��ַΪ:%s,��ǰ��������ؼ��ָ���Ϊ:%d"), pKeyWordDataInfo->strUrl.GetString(), g_iYouDao);
					}
				}
				else
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ҳ�治�������򿪣�"));
				}

				DeleteSearchCache();
			}
			else
				iResult = OPEN_PAGE_NORMAL;
		}
	}


	return iResult;
}

/*
Add By ����
@brief  �Ƴ�Ҫ����HTML�������script��ǩ
@param  string strHTMLData HTML����
@return void
*/
void CUpdateRankDlg::RemoveAllScriptTag(string &strHTMLData)
{
	int ipos = -10;
	int iposL = strHTMLData.find("<script");
	int iposU = strHTMLData.find("<SCRIPT");
	if ((iposL != strHTMLData.npos) && (iposU == strHTMLData.npos))
	{
		ipos = iposL;
	}
	else if ((iposL == strHTMLData.npos) && (iposU != strHTMLData.npos))
	{
		ipos = iposU;
	}
	else
	{
		ipos = iposU <= iposL ? iposU : iposL;
	}

	while (ipos != strHTMLData.npos && ipos != -10)
	{
		Sleep(5);
		int ipos1 = -10;
		int iposL1 = strHTMLData.find("</script>");
		int iposU1 = strHTMLData.find("</SCRIPT>");
		if ((iposL1 != strHTMLData.npos) && (iposU1 == strHTMLData.npos))
		{
			ipos1 = iposL1;
		}
		else if ((iposL1 == strHTMLData.npos) && (iposU1 != strHTMLData.npos))
		{
			ipos1 = iposU1;
		}
		else
		{
			ipos1 = iposU1 <= iposL1 ? iposU1 : iposL1;
		}

		if (ipos1 != strHTMLData.npos && ipos1 != -10)
		{
			strHTMLData = strHTMLData.replace(ipos, ipos1 - ipos + 9, "");
		}
		else
		{
			return;
		}

		iposL1 = strHTMLData.find("<script");
		iposU1 = strHTMLData.find("<SCRIPT");
		if ((iposL1 != strHTMLData.npos) && (iposU1 == strHTMLData.npos))
		{
			ipos = iposL1;
		}
		else if ((iposL1 == strHTMLData.npos) && (iposU1 != strHTMLData.npos))
		{
			ipos = iposU1;
		}
		else
		{
			ipos = iposU1 <= iposL1 ? iposU1 : iposL1;
		}
	}
}

/*
Add By ����
@brief  ��HTML��style��ǩ�����滻��css����
@param  string strHTMLData HTML����
@param  char* strData Ҫ�滻������
@return void
*/
void CUpdateRankDlg::ReplaceStyleToCss(string &strHTMLData, const char* strData)
{
	int ipos = -10;
	int iposL = strHTMLData.find("<style");
	int iposU = strHTMLData.find("<STYLE");
	if ((iposL != strHTMLData.npos) && (iposU == strHTMLData.npos))
	{
		ipos = iposL;
	}
	else if ((iposL == strHTMLData.npos) && (iposU != strHTMLData.npos))
	{
		ipos = iposU;
	}
	else
	{
		ipos = iposU <= iposL ? iposU : iposL;
	}

	if (ipos != strHTMLData.npos && ipos != -10)
	{
		int ipos1 = -10;
		int iposL1 = strHTMLData.find("</style>");
		int iposU1 = strHTMLData.find("</STYLE>");
		if ((iposL1 != strHTMLData.npos) && (iposU1 == strHTMLData.npos))
		{
			ipos1 = iposL1;
		}
		else if ((iposL1 == strHTMLData.npos) && (iposU1 != strHTMLData.npos))
		{
			ipos1 = iposU1;
		}
		else
		{
			ipos1 = iposU1 <= iposL1 ? iposU1 : iposL1;
		}

		if (ipos1 != strHTMLData.npos && ipos1 != -10)
		{
			strHTMLData = strHTMLData.replace(ipos, ipos1 - ipos + 8, strData);
		}
	}
}

/*
Add By ����
@brief  �ж�HTML�Ƿ���Ҫ����
@param  CString strFileName �����ļ���
@return bool
*/
bool CUpdateRankDlg::IsNeedChange(int iFlag)
{
	if (iFlag == SEARCH_BAIDU || iFlag == SEARCH_360 || iFlag == SEARCH_BING || iFlag == SEARCH_SOGOU)
	{
		return true;
	}
	return false;
}

/*
Add By ����
@brief  ����������־��ȡstyle��ǩ��Ҫ�滻������
@param  int iFlag ������־
@param  string& szCss �滻���ݡ������
@return void
*/
void CUpdateRankDlg::ChangeData(int iFlag, string& szCss)
{
	switch (iFlag)
	{
	case SEARCH_BAIDU:
		szCss = "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../css/baidu.css\">";
		break;
	case SEARCH_360:
		szCss = "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../css/360.css\">";
		break;
	case SEARCH_BING:
		szCss = "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../css/bing.css\">";
		break;
	case SEARCH_SOGOU:
		szCss = "<link rel=\"stylesheet\" type=\"text/css\" href=\"../../css/sogou.css\">";
		break;
	default:
		break;
	}
}
