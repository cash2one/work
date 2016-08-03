// PagePhotoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UpdateRank.h"
#include "PagePhotoDlg.h"
#include "afxdialogex.h"
#include <atlimage.h>
#include <afxinet.h>



#define   GETSEARCHDATA     WM_USER + 0x01
#define   GETREDIRECTURL    WM_USER + 0x02


// CPagePhotoDlg 对话框

IMPLEMENT_DYNAMIC(CPagePhotoDlg, CDialog)

CPagePhotoDlg::CPagePhotoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPagePhotoDlg::IDD, pParent)
	, hThread(NULL)
{
}

CPagePhotoDlg::~CPagePhotoDlg()
{
	if (hThread != NULL)
		CloseHandle(hThread);

	for (int i=0; i<vectBackData.size(); i++)
	{
		delete vectBackData[i];
	}

	vectBackData.clear();
}

void CPagePhotoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_WEB_BROWSER, m_WebBrowser);
}


BEGIN_MESSAGE_MAP(CPagePhotoDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CPagePhotoDlg::OnBnClickedOk)
	ON_MESSAGE(GETSEARCHDATA, &GetSearchData)
	ON_MESSAGE(GETREDIRECTURL, &RedirectURL)
END_MESSAGE_MAP()


// CPagePhotoDlg 消息处理程序


BOOL CPagePhotoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);//从任务栏中去掉.

	WINDOWPLACEMENT wp;
	wp.length=sizeof(WINDOWPLACEMENT);
	wp.flags=WPF_RESTORETOMAXIMIZED;
	wp.showCmd=SW_HIDE;
	SetWindowPlacement(&wp);
	
	m_WebBrowser.SetSilent(TRUE);
	SetInternetFeature();

	hThread = CreateThread(NULL, NULL, GetPagePhoto, this, 0, NULL);

	if (hThread == NULL)
		OnBnClickedOk();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BEGIN_EVENTSINK_MAP(CPagePhotoDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CPrintScreenDlg)
	ON_EVENT(CPagePhotoDlg, IDC_WEB_BROWSER, 250 /* BeforeNavigate2 */, OnBeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CPagePhotoDlg::OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
	// TODO: Add your control notification handler code here

}


/*
	@brief  根据页面发过来的数据寻找百度谷歌搜索数据  线程
	@param  lp  this指针
	@return 
	*/
DWORD WINAPI CPagePhotoDlg::GetPagePhoto( LPVOID lp )
{
	CPagePhotoDlg *pPhotoDlg = (CPagePhotoDlg*)lp;

	while(true)
	{
		SearchData *pCurSearchData = GetSearKeyWord();
		if (pCurSearchData == NULL)
			break;
		// 用于测试
		// pCurSearchData->iTotoalFlag = 0x40;

		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("关键词:%s  开始抓取快照 搜索标志为：%d"), pCurSearchData->strKeyWord.GetString(), pCurSearchData->iTotoalFlag);

		DWORD dwFlagTime;

		if ((pCurSearchData->iTotoalFlag & SEARCH_BAIDU) && bSearchFlag[BADIDU_INDEX])  //抓取百度
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_BAIDU;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("http://www.baidu.com/s?wd=%s&pn=%d"),pCurSearchData->strKeyHex.GetString(),i*10);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取百度快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
							
		}

		if ((pCurSearchData->iTotoalFlag & SEARCH_PHONEBAIDU) && bSearchFlag[PHONEBAIDU_INDEX])  //抓取手机百度
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_PHONEBAIDU;

			// 只抓首页
			pCurSearchData->iCurPage = 0;
			pCurSearchData->strUrl.Format(_T("http://wap.baidu.com/s?word=%s&pn=%d"),pCurSearchData->strKeyHex.GetString(),0);

			pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData);

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取手机百度快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());

		}

		Sleep(50);

		if ((pCurSearchData->iTotoalFlag & SEARCH_GOOGLE) &&  bSearchFlag[GOOGLE_INDEX])  //抓取谷歌  谷歌经常出现打不开情况，如果打不开，则跳过
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_GOOGLE;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("https://www.google.com.hk/search?q=%s&start=%d"),pCurSearchData->strKeyHex.GetString(),i*10);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取谷歌快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
		}

		if ((pCurSearchData->iTotoalFlag & SEARCH_360) && bSearchFlag[S360_INDEX])  //抓取360
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_360;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("http://www.haosou.com/s?q=%s&pn=%d"),pCurSearchData->strKeyHex.GetString(),i+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取360快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
		}

		if ((pCurSearchData->iTotoalFlag & SEARCH_SOSO) && bSearchFlag[SOSO_INDEX])  //抓取搜搜
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_SOSO;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("http://www.soso.com/q?query=%s&pg=%d"),pCurSearchData->strKeyHex.GetString(),i+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取soso快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
		}

		if ((pCurSearchData->iTotoalFlag & SEARCH_SOGOU) && bSearchFlag[SOGOU_INDEX])  //抓取搜狗
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_SOGOU;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("http://www.sogou.com/web?query=%s&page=%d"),pCurSearchData->strKeyHex.GetString(),i+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取sogou快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
		}

		if ((pCurSearchData->iTotoalFlag & SEARCH_BING) && bSearchFlag[BING_INDEX])  //抓取必应
		{
			dwFlagTime = GetTickCount();
			pCurSearchData->iFlag = SEARCH_BING;

			for (int i=0; i<3; i++) //搜索前三页
			{								
				pCurSearchData->iCurPage = i;
				pCurSearchData->strUrl.Format(_T("http://cn.bing.com/search?q=%s&first=%d"),pCurSearchData->strKeyHex.GetString(),i*10+1);

				if (pPhotoDlg->WaitURlAndSaveImageFile(*pCurSearchData))
					break;    //已经抓取快照，不再抓取搜索后面快照
			}

			dwFlagTime = GetTickCount() - dwFlagTime;
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取BING快照花费时间,%.2f,秒,关键词为：%s"), dwFlagTime/1000.0, pCurSearchData->strKeyWord.GetString());
		}


		if (pCurSearchData != NULL)
		{
			delete pCurSearchData;
			pCurSearchData = NULL;
		}

		//返回数据
		pPhotoDlg->BackMsg();				
	}

	pPhotoDlg->PostMessage(WM_QUIT);

	return 0;

}


#define  IRETRYTIMES    3       //重试次数
#define  TIMEOUTCOUTS   8      //超时时间
/*
	@brief  等待web控件打开页面
	@param  sData  
	@return 
	*/
BOOL CPagePhotoDlg::WaitURlAndSaveImageFile(const SearchData &sData)
{	
	BOOL bReady = FALSE;
	BOOL bResult = FALSE;
	int  iTimeleft;
	long lMessageBack = 0;

	SearchData *pData = new SearchData;
	if (pData != NULL)
	{
		pData->iCurPage = sData.iCurPage;
		pData->iFlag = sData.iFlag;
		pData->vCompanys = sData.vCompanys;
		pData->strKeyWord = sData.strKeyWord;
		pData->strUrl = sData.strUrl;

		for (int i=0; i<IRETRYTIMES; i++)  //网速比较慢打不开网页情况，重试
		{
			iTimeleft = 0;

			//下面主要同步发消息给主线程去操作，防止不同线程操作出现崩溃现象,主要做监控效果
			if (SendMessage(GETREDIRECTURL, 0, (LPARAM)pData) == 1) 
			{
				do
				{
					bReady = (BOOL)SendMessage(GETREDIRECTURL, 1, (LPARAM)pData);
					if (bReady || (iTimeleft > TIMEOUTCOUTS))
					{
						if (iTimeleft > TIMEOUTCOUTS)
						{
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("超时%d，当已加载完成处理,页面为：%s"), i+1, sData.strUrl);
							SendMessage(GETREDIRECTURL, 2, (LPARAM)pData);
						}

						lMessageBack = SendMessage(GETSEARCHDATA, 0, (LPARAM)pData);

						
						break;
					}

					iTimeleft++;
					Sleep(500);

				}while (1);
			}

			if (lMessageBack != 2)  //页面无法打开，需要重试
			{
				if (lMessageBack == 0)
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("未抓取到快照,网址:%s"), sData.strUrl);

					if(sData.iCurPage == 0 && sData.iFlag == SEARCH_PHONEBAIDU) // 手机百度第一页没抓到快照，添加排名为0的信息到容器
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("手机未抓取到快照，添加排名为0的信息到容器"));
						BackData* pBack = new BackData;
						pBack->strKeyword = sData.strKeyWord;
						pBack->iFlag = sData.iFlag;
						vectBackData.push_back(pBack);
					}

					if (sData.iCurPage == 2)  //当前第三页
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("当前第三页打开成功未抓取到快照，添加排名为0的信息到容器"));
						BackData* pBack = new BackData;

						pBack->strKeyword = sData.strKeyWord;
						pBack->iFlag = sData.iFlag;
						vectBackData.push_back(pBack);
					}
					bResult = FALSE;  //正常打开页面，没抓取到快照
				}
				else if (lMessageBack == 1)
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("抓取到快照，关键词:%s,网址:%s"), sData.strKeyWord, sData.strUrl);
					bResult = TRUE;  //有快照
				}
				else if (lMessageBack == 3)  //页面出现验证码
				{
					bSuccess = FALSE;   //返回失败结果
					g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("%s遇到验证取消搜索"), GetSearchFlag(pData->iFlag));

					EnterCriticalSection(&critSearchFlag);
					if (pData->iFlag == SEARCH_BAIDU)
					{
						bSearchFlag[BADIDU_INDEX] = FALSE;
					}
					else if (pData->iFlag == SEARCH_GOOGLE)
					{
						bSearchFlag[GOOGLE_INDEX] = FALSE;
					}
					else if (pData->iFlag == SEARCH_360)
					{
						bSearchFlag[S360_INDEX] = FALSE;
					}
					else if (pData->iFlag == SEARCH_SOSO)
					{
						bSearchFlag[SOSO_INDEX] = FALSE;
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

					LeaveCriticalSection(&critSearchFlag);

					bResult = TRUE;   //这里返回true  取消下一当前搜索引擎下一步操作
				}
				else if (lMessageBack == 4)
				{
					bResult = TRUE;
				}
				break;
			}
			else
			{
				if (i < (IRETRYTIMES-1))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("页面无法打开，重试,网址:%s,次数：%d"), sData.strUrl, i+1);
				}
				else
				{
					bSuccess = FALSE;   //返回失败结果
				}
			}
		}

		delete pData;
	}

	return bResult;
}

/*
	@brief  取得百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::CalBaiduHeight(IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection baidu is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;

	BackData *pBdata = NULL;
	HRESULT hr;

	for (long l = sData.iCurPage*10 + 1; l <= sData.iCurPage*10 + 10; l++)
	{
		IDispatch  *pDisp = NULL;
		CString strTemp;

		strTemp.Format(_T("%d"), l);

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = 0;

		VARIANT varID = {0};
		VariantInit(&varID);
		varID.vt = VT_BSTR;

		varID.bstrVal = strTemp.AllocSysString();
		hr = pTables->item(varID,index,&pDisp);
		SysFreeString(varID.bstrVal);

		if (FAILED(hr) || pDisp == NULL)
			continue;

		pElement = pDisp;
		pDisp->Release();


		CComBSTR bstr;
		CString strPage;

		pElement->get_innerHTML(&bstr);
		strPage= bstr;	


		BOOL bFind = FindIsOwnWebInBaidu(strPage);
		if (bFind)
		{
	/*		if(sData.strKeyWord.Compare(_T("粘合机")) == 0)
			{
				::MessageBox(NULL, _T("粘合机"), _T("判定"), MB_OK);
			}
    */
			if (strPage.Find(sData.strKeyWord.GetString()) != -1);
			{
				//if (strPage.Find(strCompany.GetString()) == -1)  //这里注释掉，就是无论是否有公司名，都打开网址查看
				{
					CString strUrl = GetUrlFormContent(strPage);
					if (strUrl.Find(_T("http:")) == -1)
						strUrl.Insert(0, _T("http:"));

					int iResult = HasCompanyInHtml(strUrl, sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ：打开页面出现异常不处理"));
							break;
						}
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ：没有在目标网址中找到公司名"));
						continue;
					}
				}
				if(strPage.Find(_T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image:")) == -1)
				{
					//加红框
					strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
					strPage.Append(_T("</div>"));
					hr = pElement->put_innerHTML((BSTR)strPage.GetString());

					if (FAILED(hr))
					{
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("baidu ：加红框失败"));
					}
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackData;
					ASSERT(pBdata != NULL);

					pBdata->iCurPage = sData.iCurPage + 1;
					pBdata->iFlag = SEARCH_BAIDU;
					pBdata->iRank = l;
					pBdata->strKeyword = sData.strKeyWord;
				}
				vHeights.push_back(l);

			}
		}		
	}

	//if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	//{
	//	pBdata = new BackData;
	//	ASSERT(pBdata != NULL);

	//	pBdata->iCurPage = 0;
	//	pBdata->iFlag = SEARCH_BAIDU;
	//	pBdata->iRank = 0;
	//	pBdata->strKeyword = strTitle;
	//}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_BAIDU);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}
	

	return vHeights;
}


/*
	@brief  取得手机百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	add by zhoulin
	*/
std::vector<long> CPagePhotoDlg::CalPhoneBaiduHeight(IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("IHTMLElementCollection phonebaidu is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLAnchorElement> pAnchor;
	BackData *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lDivCounts = 0;
	pTables->get_length(&lDivCounts);

	for (long l = 0; l <= lDivCounts; l++)
	{
		IDispatch  *pDisp = NULL;
		IDispatch *pTemp = NULL;

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pTables->item(COleVariant(l),index,&pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;
		CComQIPtr<IHTMLElementCollection> pChild;

		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		CString strPage;
		pElement->get_innerHTML(&bstr);
		strPage = bstr;

		if(strPage.Compare(_T("")) == 0)
			continue;

		if(strPage.Find(_T("class=\"resitem\""), 0) != -1)
		{
			continue;
		}

		pElement->get_children(&pTemp);
		pChild = pTemp;

		long childCount = 0;
		pChild->get_length(&childCount);

		if(childCount > 3)
			continue;

		if(strPage.Find(_T("class=\"abs\""), 0) != -1 || strPage.Find(_T("class=abs"), 0) != -1)
		{
			iRank++;
		}

		CComBSTR bstrURL;
		CString strUrl = _T("");

		// 替换a标签的href属性
		for(long n = 0; n <= childCount; ++n)
		{
			VARIANT indexC = {0};
			V_VT(&indexC) = VT_I4;
			V_I4(&indexC) = n;

			CComQIPtr<IHTMLElement> pElementHerf;
			IDispatch *pTemp2 = NULL;
			hr = pChild->item(COleVariant(n), indexC, &pTemp2);

			if (FAILED(hr) || pTemp2 == NULL)
				continue;

			pAnchor = pTemp2;

			if(pAnchor != NULL)
			{
				pElementHerf = pTemp2;
				pAnchor->get_href(&bstrURL);
				strUrl = bstrURL;

				VARIANT strHref = {0};
				VariantInit(&strHref);
				strHref.vt = VT_BSTR;
				strHref.bstrVal = bstrURL;

				hr = pElementHerf->setAttribute(BSTR(_T("href")), strHref);

				break;
			}
		}

//		iRank++;

		BOOL bFind = FindIsOwnWebInPhoneBaidu(strPage);

		if (bFind)
		{
			if (strPage.Find(sData.strKeyWord.GetString()) != -1)
			{
				if(strUrl.Compare(_T("")) != 0)
				{
					int iResult = HasCompanyInHtml(strUrl, sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ：打开页面出现异常不处理"));
							break;
						}
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ：没有在目标网址中找到公司名"));
						continue;
					}
				}
				else
				{
					continue;
				}

			    //加红框
				pElement->get_innerHTML(&bstr);
				strPage = bstr;

				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
			    strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				l++;
				lDivCounts++;

			    if (FAILED(hr))
				 {
					 g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("phonebaidu ：加红框失败"));
				 }
			   

			  if (vHeights.size() == 0)
			  {
				  pBdata = new BackData;
				  ASSERT(pBdata != NULL);

				  pBdata->iCurPage = sData.iCurPage + 1;
				  pBdata->iFlag = SEARCH_PHONEBAIDU;
				  pBdata->iRank = sData.iCurPage*10 + iRank;
				  pBdata->strKeyword = sData.strKeyWord;
			 }
			 vHeights.push_back(sData.iCurPage*10 + iRank);
		  }	
	   }
	}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_PHONEBAIDU);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}


	return vHeights;

}


	/*
	@brief  保存bmp图片
	@param  hBitmap  bmp图片数据
	@param  lpszFileName  文件路径
	@return 保存成功返回true
	*/
BOOL CPagePhotoDlg::SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)    
{    
	HDC hDC;    
	//当前分辨率下每象素所占字节数    
	int iBits;    
	//位图中每象素所占字节数    
	WORD wBitCount;    
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数    
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;    
	//位图属性结构    
	BITMAP Bitmap;    
	//位图文件头结构    
	BITMAPFILEHEADER bmfHdr;    
	//位图信息头结构    
	BITMAPINFOHEADER bi;    
	//指向位图信息头结构    
	LPBITMAPINFOHEADER lpbi;    
	//定义文件，分配内存句柄，调色板句柄    
	HANDLE fh, hDib, hPal,hOldPal=NULL;    

	//计算位图文件每个像素所占字节数    
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
	//为位图内容分配内存     
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));    
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
	*lpbi = bi;    
	// 处理调色板      
	hPal = GetStockObject(DEFAULT_PALETTE);    
	if (hPal)    
	{     
		hDC = ::GetDC(NULL);    
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
		RealizePalette(hDC);    
	}    

	// 获取该调色板下新的像素值    
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)     
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);    
	//恢复调色板    
	if (hOldPal)    
	{    
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);    
		RealizePalette(hDC);    
		::ReleaseDC(NULL, hDC);    
	}    
	//创建位图文件      
	fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,     
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);     

	if (fh == INVALID_HANDLE_VALUE)    
		return FALSE;     
	// 设置位图文件头     
	bmfHdr.bfType = 0x4D42; // "BM"    
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;    
	bmfHdr.bfSize = dwDIBSize;    
	bmfHdr.bfReserved1 = 0;    
	bmfHdr.bfReserved2 = 0;    
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;    
	// 写入位图文件头    
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    
	// 写入位图文件其余内容    
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);    
	//清除    
	GlobalUnlock(hDib);    
	GlobalFree(hDib);    
	CloseHandle(fh);    
	return TRUE;    
}

/*
	@brief  根据关键词取得图片路径
	@param  strTitle  关键词
	@param  strFile  文件路径  输出
	@param  strComp  公司名
	@param  iFlag  搜索标志
	@return 保存成功返回true
	*/
BOOL CPagePhotoDlg::GetImageFilePath(const CString &strTitle, const CString &strComp, CString &strFile, int iFlag )
{
#define CHARACTLEN 15
	TCHAR szFile[MAX_PATH] = {0};
	TCHAR szCharact[CHARACTLEN] = _T("\\/:*?\"<>|+%#&=");  //文件特殊字符去掉
	strFile.Empty();

	CString strNewTitle = strTitle;
	CString strNewComp = strComp;

	for (int i=0; i<CHARACTLEN-1; i++)
	{
		strNewTitle.Replace(szCharact[i], _T('_'));
		strNewComp.Replace(szCharact[i], _T('_'));
	}	

	if (GetModuleFileName(NULL, szFile, MAX_PATH))
	{
		PathAppend(szFile, _T("..\\..\\"));
		//PathAppend(szFile, _T("..\\"));
		
		strFile = szFile;
		strFile.Append(_T("image\\keyword\\"));
		strFile.Append(strNewTitle);
		strFile.Append(_T("_"));
		strFile.Append(strNewComp);

		if (iFlag == SEARCH_BAIDU)
			strFile.Append(_T("_BAIDU.html"));
		else if (iFlag == SEARCH_GOOGLE)
			strFile.Append(_T("_GOOGLE.html"));
		else if (iFlag == SEARCH_360)
			strFile.Append(_T("_360.html"));
		else if (iFlag == SEARCH_SOSO)
			strFile.Append(_T("_SOSO.html"));
		else if (iFlag == SEARCH_SOGOU)
			strFile.Append(_T("_SOGOU.html"));
		else if (iFlag == SEARCH_BING)
			strFile.Append(_T("_BING.html"));
		else if (iFlag == SEARCH_PHONEBAIDU)
			strFile.Append(_T("_PHONEBAIDU.html"));

		return TRUE;		
	}

	return FALSE;
}

void CPagePhotoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//if (nIDEvent == 1)
	//{
	//	if (WaitURlAndSaveImageFile(_T("120ml广口瓶"), _T("广州市艾儿克塑料制品厂")))
	//	{
	//		KillTimer(nIDEvent);
	//		SetEvent(hEvent);
	//	}
	//	
	//	//GetPagePhoto(this);
	//}

	CDialog::OnTimer(nIDEvent);
}


void CPagePhotoDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}


/*
@brief  取得百度谷歌快照
@param
@param
@return 0表示正常打开无快照  1表示有快照  2表示页面无法打开   3表示出现验证码  4打开页面时出现异常的

*/
LRESULT CPagePhotoDlg::GetSearchData( WPARAM wParam, LPARAM lParam )
{
	LRESULT dwResult = 0;
	HRESULT hret;

	CComPtr<IHTMLDocument2> pDocument2;
	CComPtr<IHTMLElement> pElement;

	CComPtr<IDispatch> pDispath;
	CComPtr<IDispatch> pDispathTemp;

	CComQIPtr<IHTMLElementCollection> pAllColls;
	CComQIPtr<IHTMLElementCollection> pTags;

	SearchData  *pSdata = (SearchData*)lParam;

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
				|| pSdata->iFlag == SEARCH_SOSO
				|| pSdata->iFlag == SEARCH_SOGOU
				|| pSdata->iFlag == SEARCH_PHONEBAIDU)
			{
				hret = pAllColls->tags(COleVariant(_T("div")), &pDispathTemp);
			}
			else if (pSdata->iFlag == SEARCH_GOOGLE
				|| pSdata->iFlag == SEARCH_360
				|| pSdata->iFlag == SEARCH_BING)
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
				vHeight = CalBaiduHeight(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_PHONEBAIDU)
			{
				vHeight = CalPhoneBaiduHeight(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_GOOGLE)
			{
				vHeight = CalGoogleHeight(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_360)
			{
				vHeight = Cal360Height(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_SOSO)
			{
				vHeight = CalSoSoHeight(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_SOGOU)
			{
				vHeight = CalSoGouHeight(pTags, *pSdata, strFileName, bCrash);
			}
			else if (pSdata->iFlag == SEARCH_BING)
			{
				vHeight = CalBingHeight(pTags, *pSdata, strFileName, bCrash);
			}


			if (!bCrash)
			{
				if (vHeight.size() == 0)
				{
					int iResult = JudgeHtml(pAllColls, pSdata);

					if (iResult == 1)
					{
						if (pSdata->iFlag == SEARCH_SOSO)
						{
							static int iSosoFaile = 0;
							iSosoFaile++;

							if (iSosoFaile >= 5)
							{
								bSearchFlag[SOSO_INDEX] = FALSE;
							}
						}

						dwResult = 2;  //页面无法正常打开
					}
					else if (iResult == 2)
					{
						dwResult = 3;  //页面出现验证码
					}
					break;
				}
			}
			else
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("关键词：%s 出现异常，不返回，搜索引擎为：%s"), pSdata->strKeyWord.GetString(), GetSearchFlag(pSdata->iFlag));
				dwResult = 4;
				break;
			}
			

			ChangeButtonStyle(pAllColls, pSdata);

			dwResult = 1;
			SaveToHtml(pAllColls, strFileName);
		}while (0);
	}
	catch(...)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("处理html时，出现异常"));
	}
	

	return dwResult;
}


	/*
	@brief  取得谷歌条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::CalGoogleHeight(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection google is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	CComQIPtr<IHTMLElement2> pElement2;

	BackData *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l),index,&pDisp);
		if (FAILED(hr) || pDisp == NULL)
			break;

		pElement = pDisp;
		pDisp->Release();

		CComBSTR bstr;
		pElement->get_outerHTML(&bstr);
		CString strPage(bstr);

		strPage.MakeLower();
		if (strPage.Find(_T("<h3 ")) == -1)
		{
			continue;
		}

		iRank++;
		//谷歌不搜索，注释掉
		//strPage = bstr;
		//if (strPage.Find(strTitle.GetString()) != -1
		//	&& FindIsOwnWebInGoogle(strPage))
		//{
		//	if (strPage.Find(strCompany.GetString()) == -1)
		//	{
		//		if (strPage.Find(_T("公司")) != -1)
		//		{
		//			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("google：没有匹配到公司名跳过"));
		//			continue;
		//		}
		//		else
		//		{
		//			if (!HasCompanyInHtml(GetUrlFormContent(strPage), strCompany))
		//			{
		//				g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("google ：没有在目标网址中找到公司名"));
		//				continue;
		//			}
		//		}
		//	}
		//	pElement2 = pElement;

		//	CComPtr<IHTMLRect> pRect;
		//	if (SUCCEEDED(pElement2->getBoundingClientRect(&pRect)))
		//	{
		//		long lHeight = 0;
		//		if (SUCCEEDED(pRect->get_top(&lHeight)))
		//		{
		//			if (vHeights.size() == 0)
		//			{
		//				pBdata = new BackData;
		//				ASSERT(pBdata != NULL);

		//				pBdata->iCurPage = iPage + 1;
		//				pBdata->iFlag = SEARCH_GOOGLE;
		//				pBdata->iRank = iPage*10 + iRank;
		//				pBdata->strKeyword = strTitle;

		//				//g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("发现谷歌有条目收录，当前第%d页，第%d条"), pBdata->iCurPage, pBdata->iRank);
		//			}
		//			vHeights.push_back(lHeight);
		//		}

		//	}
		//}		
	}


	/*if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	{
		pBdata = new BackData;
		ASSERT(pBdata != NULL);

		pBdata->iCurPage = 0;
		pBdata->iFlag = SEARCH_GOOGLE;
		pBdata->iRank = 0;
		pBdata->strKeyword = strTitle;
	}*/

	if (pBdata != NULL)
	{
		CString strCompany;
		if (sData.vCompanys.size() > 0)
		{
			strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
		}

		pBdata->iCount = vHeights.size();
		GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_GOOGLE);

		pBdata->strPagePath = strFileName;
		vectBackData.push_back(pBdata);
	}

	return vHeights;
}



/*
	@brief  判断百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL  CPagePhotoDlg::FindIsOwnWebInBaidu(CString strHtml)
{
	int iPos1,iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();

	iPos1 = iPos2 = 0;
	iPos1 = strHtml.Find(_T("<span"), iPos2);

	while(true)
	{
		if(iPos1 != -1)
		{
			iPos1 = strHtml.Find(_T(">"), iPos1);
			iPos2 = strHtml.Find(_T("</span>"), iPos1);
			if (iPos2 != -1)
			{
				//截取网址  14表示<SPAN class=g>这个字符串长度
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
					if (strWeb.Find(pChar) != -1)
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
	@brief  判断手机百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	add by zhoulin
	*/
BOOL CPagePhotoDlg::FindIsOwnWebInPhoneBaidu(CString strHtml)
{
	int iPos1,iPos2;
	BOOL bReturn = FALSE;
	strHtml.MakeLower();

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
				if (strWeb.Find(pChar) != -1)
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
	@brief  判断谷歌搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL  CPagePhotoDlg::FindIsOwnWebInGoogle(CString strHtml)
{
	BOOL bReturn = FALSE;

	//截取网址  14表示<SPAN class=g>这个字符串长度
	CString strUrl = GetUrlFormContent(strHtml);

	GetMainDomain(strUrl);

	char *pChar = WideToChar(strUrl.GetString());
	if (pChar != NULL)
	{
		if (strWeb.Find(pChar) != -1)
		{
			bReturn = TRUE;
		}
		delete []pChar;
	}

	return bReturn;

}

LRESULT CPagePhotoDlg::RedirectURL( WPARAM wParam, LPARAM lParam )
{
	BOOL bBusy;
	long lReadyState;
	SearchData *pData = (SearchData*)lParam;

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
				/*m_WebBrowser.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
				Sleep(10);*/
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
		return 1;  //出现异常，完成
	}

	

	return 0;
}


/*
	@brief  取得360条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::Cal360Height( IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection 360 is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackData *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l),index,&pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;

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

		strPage = bstr;
		iRank++;

		BOOL bFind = FindIsOwnWebIn360(strPage);

		if (bFind)
		{
			if (strPage.Find(sData.strKeyWord.GetString()) != -1)
			{
				//if (strPage.Find(strCompany.GetString()) == -1)      //这里注释掉，就是无论是否有公司名，都打开网址查看
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ：打开页面出现异常不处理"));
							break;
						}
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ：没有在目标网址中找到公司名"));
						continue;
					}
				}

				//加红框
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("360 ：加红框失败"));
				}
				
				if (vHeights.size() == 0)
				{
					pBdata = new BackData;
					ASSERT(pBdata != NULL);

					pBdata->iCurPage = sData.iCurPage + 1;
					pBdata->iFlag = SEARCH_360;
					pBdata->iRank = sData.iCurPage*10 + iRank;
					pBdata->strKeyword = sData.strKeyWord;
				}
				vHeights.push_back(sData.iCurPage*10 + iRank);
			}	
		}			
	}

	//if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	//{
	//	pBdata = new BackData;
	//	ASSERT(pBdata != NULL);

	//	pBdata->iCurPage = 0;
	//	pBdata->iFlag = SEARCH_360;
	//	pBdata->iRank = 0;
	//	pBdata->strKeyword = strTitle;
	//}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_360);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}
	

	return vHeights;
}

/*
	@brief  判断360搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL CPagePhotoDlg::FindIsOwnWebIn360(CString strHtml )
{
	return FindIsOwnWebInGoogle(strHtml);  //360和谷歌取得网址方式一样
}

/*
	@brief  根据url取得
	@param  [in/out]strUrl  网址
	*/
void CPagePhotoDlg::GetMainDomain(CString &strUrl )
{
	int iPos1 = -1;
	int iPos2 = -1;

	TCHAR  szUrl[3][5] = {_T(".com"), _T(".cn"), _T(".net")};

	for(int i=0; i<3; i++)
	{
		iPos2 = strUrl.Find(szUrl[i]);

		if (iPos2 != -1)
		{
			if (i == 1) //避免有几个cn情况
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
	@brief  请求url 判断里面的html源码是否包含公司名
	@param  strUrl  网址
	@param  strCompay  公司名
	*/
int CPagePhotoDlg::HasCompanyInHtml( const CString &strUrl, const SearchData &sData)
{
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	int iResult = 0;

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);
	if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("解析目标网址:%s失败"), strUrl.GetString());
		return iResult;
	}

	for (int i=0; i<IRETRYTIMES; i++)
	{
		CInternetSession m_session(IE_AGENT);
		CHttpFile *pFile = NULL;
		CHttpConnection *pConnection = NULL;
		CString strData;
		strData.Empty();

		try
		{
			m_session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT , 5*1000);
			m_session.SetOption(INTERNET_OPTION_SEND_TIMEOUT , 5*1000);
			m_session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT , 5*1000);
			m_session.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT , 5*1000);
			m_session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT , 5*1000);
			//m_session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);          // 1次重试

			pConnection = m_session.GetHttpConnection(strServer, nPort);

			if (pConnection == NULL)
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("打开目标网址:%s连接失败"), strUrl.GetString());
				return iResult;
			}

			pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject.GetString());

			if (pFile != NULL)
			{
				DWORD dwRet = 0;

				pFile->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 5*1000, 0);
				pFile->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 5*1000, 0);

				pFile->SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, 5*1000, 0);
				pFile->SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 5*1000, 0);


				pFile->AddRequestHeaders(_T("Accept: text/html"));
				pFile->AddRequestHeaders(_T("Accept-Language: zh-CN"));
				pFile->AddRequestHeaders(_T("Content-Type:text/html; charset=utf-8"));  //统一用utf-8防止乱码

				pFile->SendRequest();
				pFile->QueryInfoStatusCode(dwRet);			

				if (dwRet == HTTP_STATUS_OK)
				{
					DWORD dwRead = 0;
					char  szBufa[8192] = {0};
					wchar_t szBufw[8192] = {0};
					DWORD dwCode = CP_ACP;

					CString strTemp;
					do 
					{
						dwRead = pFile->Read(szBufa, sizeof(szBufa)-1);

						if (strData.IsEmpty())
						{
							strTemp= szBufa;
							strTemp.MakeLower();

							int iPos1 = strTemp.Find(_T("charset="));

							if (iPos1 != -1)
							{
								CString strEncode = strTemp.Mid(iPos1, ENCODELENGTH);

								if (strEncode.Find(_T("gb2312")) != -1
									|| strEncode.Find(_T("gbk")) != -1)
								{
									g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网址:%s  查找到gbk 或gb2312 编码"), strUrl.GetString());
									dwCode = CP_ACP;
								}
								else
								{
									g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网址:%s  查找到charset= 但找不到gbk 或gb2312 编码,用utf-8"), strUrl.GetString());
									dwCode = CP_UTF8;
								}
							}
							else
							{
								g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("网址:%s  查找不到charset=字样，用utf-8编码"), strUrl.GetString());
								dwCode = CP_UTF8;
							}
						}

						MultiByteToWideChar( dwCode, 0, szBufa,
							strlen(szBufa)+1, szBufw, sizeof(szBufw)/sizeof(szBufw[0]) );

						strData.Append(szBufw);
						memset(szBufa, 0, sizeof(szBufa));
						memset(szBufw, 0, sizeof(szBufw));

					} while (dwRead > 0);
				}
				else if (dwRet == HTTP_STATUS_NOT_FOUND)
				{
					i = IRETRYTIMES;//  这里不重试
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("打开目标网站出现404"));
				}

				g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("状态码为:%d目标网址:%s"), dwRet, strUrl.GetString());

				pFile->Close();
				delete pFile;
			}

			pConnection->Close();
			delete pConnection;

			m_session.Close();
		}
		catch (...)
		{
			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("请求读取数据异常,错误码：%d，目标网址:%s"), GetLastError(), strUrl.GetString());
			//return FALSE;
		}

		if (strData.GetLength() > 0)
		{
			int iSize = sData.vCompanys.size();

			for (int i=0; i<iSize; i++)
			{
				if (strData.Find(sData.vCompanys[i].GetString()) != -1)
				{
					iResult = 1;
					break;
				}
			}

			break;
		}
		else
		{
			if (i != IRETRYTIMES)  //非404情况
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("请求目标网址:%s失败,当前次数:%d"), strUrl.GetString(), i+1);

				if (i == IRETRYTIMES - 1)
				{
					iResult = 2;
					bSuccess = FALSE;
					g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("当前打开网址失败：%s,关键词：%s,搜索引擎网址：%s"), strUrl.GetString(), sData.strKeyWord.GetString(), sData.strUrl.GetString());
				}
			}			
		}

		Sleep(200);
	}
	

	return iResult;
}

/*
	@brief  根据百度条目html源码取得对应链接url
	@param  strPage  源码
	*/
CString CPagePhotoDlg::GetUrlFormContent(const CString &strPage )
{
	CString strUrl;
	int iPos1 = strPage.Find(_T("href="));
	int iPos2 = -1;
// 	if (iPos1 != -1)
// 		iPos1 = strPage.Find(_T("href="), iPos1 + 6);

	if (iPos1 != -1)
	{
		iPos2 = strPage.Find(_T('\"'), iPos1 + 6);  //加6  href="  包含6个字符
		if (iPos2 != -1 && iPos2 > iPos1+6)
		{
			strUrl = strPage.Mid(iPos1 + 6, iPos2-iPos1-6);
		}
	}

	return strUrl;
}

/*
	@brief  判断抓取数据是否包含3个搜索引擎，缺少则补上
	@param  pData数据
	*/
void CPagePhotoDlg::AlterData( std::vector<BackData*> *pData )
{
	if (pData != NULL)
	{
		if (pData->size() == 0 || pData->size() == 3)
			return;

		BackData *pBack = NULL;
		int iIndex = 6;

		for (int i=0; i<pData->size(); i++)
		{
			pBack = pData->at(i);
			iIndex = iIndex - pBack->iFlag;

			if (pBack->iFlag == 3)  //360搜索引擎  去掉0x4
				iIndex -= 1;
		}

		if (iIndex == 4 || iIndex == 2 || iIndex == 0) //缺少百度 
		{
			BackData *pNew = new BackData;

			pNew->iCurPage = 0;
			pNew->iFlag = SEARCH_BAIDU;
			pNew->iRank = 0;
			pNew->strKeyword = pBack->strKeyword;

			pData->push_back(pNew);
		}

		if (iIndex == 5 || iIndex == 2 || iIndex == 1)  //缺少谷歌
		{
			BackData *pNew = new BackData;

			pNew->iCurPage = 0;
			pNew->iFlag = SEARCH_GOOGLE;
			pNew->iRank = 0;
			pNew->strKeyword = pBack->strKeyword;

			pData->push_back(pNew);
		}

		if (iIndex == 5 || iIndex == 3 || iIndex == 4)  //缺少360
		{
			BackData *pNew = new BackData;

			pNew->iCurPage = 0;
			pNew->iFlag = SEARCH_360-1;
			pNew->iRank = 0;
			pNew->strKeyword = pBack->strKeyword;

			pData->push_back(pNew);
		}
	}
}

/*
	@brief  把抓取的数据返回给上层
	*/
void CPagePhotoDlg::BackMsg()
{
	
	if (vectBackData.size() > 0)
	{
		int i = 0;
		BOOL bHasData = FALSE;

		for (i = 0; i<vectBackData.size(); i++)
		{
			BackData *pBackData = vectBackData[i];
			//SaveHtmlToServer(pBackData->strPagePath);

			pBackData->strPagePath = PathFindFileName(pBackData->strPagePath);
		}

		CStdString strResult = _T("BackResult(;0)QuickPhoto(;0)");
		for (i=0; i<vectBackData.size(); i++)
		{
			BackData *pBackData = vectBackData[i];
			if (pBackData != NULL)
			{
				if (pBackData->strPagePath.IsEmpty() && pBackData->iRank > 0)
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("关键词为：%s,搜索引擎为:%s,上传快照失败，不返回"), pBackData->strKeyword, GetSearchFlag(pBackData->iFlag));
					delete pBackData;
					continue;
				}
				strResult.Format(_T("%s%s(;0)%d(;0)%d(;0)%d(;0)%s(;0)%s(;1)")
					, strResult.c_str()
					, pBackData->strKeyword
					, pBackData->iCurPage
					, pBackData->iCount
					, pBackData->iRank
					, pBackData->strPagePath
					, GetSearchFlag(pBackData->iFlag));

				bHasData = TRUE;
				delete pBackData;
				
			}
		}

		if (bHasData)
		{
			strResult = strResult.Left(strResult.length()-4);
			BYTE * pByData = (BYTE *)strResult.c_str();

			EnterCriticalSection(&critSendMsg);
			g_Socket->SendData(strResult.size() * 2,E_GET_EXCUTE_TASK_RESULT,(char*)pByData);
			LeaveCriticalSection(&critSendMsg);

			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("返回快照数据：%s"), strResult.c_str());
		}

		vectBackData.clear();
	}
}


/*
	@brief 将html 上传到ftp服务器
	@param [in/out]html路径
	*/
void CPagePhotoDlg::SaveHtmlToServer( CString &strFilePath)
{
	CString strRemotePath = _T("/QuickPhoto/QY2.0/");
	CString strTemp = PathFindFileName(strFilePath);
	strRemotePath.Append(strTemp);

	BOOL bRes = FALSE;

	if (PathFileExists(strFilePath))
	{
		/*EnterCriticalSection(&critPostFile);

		if (ftpFile.IsFtpMode())
		{
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ftp上传模式"));
			bRes = ftpFile.PostFileToServer(strFilePath.GetString(), strRemotePath.GetString());

			if (bRes)
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ftp上传成功, 快照为:%s"), strFilePath.GetString());
			}
			else
			{
				g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("ftp上传失败, 快照为:%s"), strFilePath.GetString());
			}	
		}
		else
		{
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("http上传模式"));
		}
				

		if (!bRes)
		{
			g_pageLog.Trace(LOGL_TOP, LOGT_PROMPT, __TFILE__, __LINE__, _T("http开始上传,文件名为:%s"), strFilePath.GetString());
			bRes = httpFile.LoginAndUploadFile(strFilePath);
		}

		LeaveCriticalSection(&critPostFile);*/
	}

	if (bRes)
	{
		strFilePath = strTemp;
	}
	else
	{
		strFilePath = _T("");
	}
}


	/*
	@brief  取得搜搜条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::CalSoSoHeight( IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection soso is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackData *pBdata = NULL;
	HRESULT hr;

	for (long l = 1; l <= 10; l++)
	{
		IDispatch  *pDisp = NULL;
		CString strTemp;

		strTemp.Format(_T("rb_%d"), l-1);

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = 0;

		VARIANT varID = {0};
		VariantInit(&varID);
		varID.vt = VT_BSTR;

		varID.bstrVal = strTemp.AllocSysString();
		hr = pTables->item(varID,index,&pDisp);
		SysFreeString(varID.bstrVal);

		if (FAILED(hr) || pDisp == NULL)
			continue;

		pElement = pDisp;
		pDisp->Release();


		CComBSTR bstr;
		pElement->get_innerHTML(&bstr);
		CString strPage(bstr);

		BOOL bFind = FindIsOwnWebInSoSo(strPage);

		if (bFind)
		{
			if (strPage.Find(sData.strKeyWord.GetString()) != -1)
			{
				//if (strPage.Find(strCompany.GetString()) == -1)     //这里注释掉，就是无论是否有公司名，都打开网址查看
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("SOSO ：打开页面异常不处理"));
							break;
						}
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("SOSO ：没有在目标网址中找到公司名"));
						continue;
					}
				}

				//加红框
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("soso ：加红框失败"));
				}

				
				if (vHeights.size() == 0)
				{
					pBdata = new BackData;
					ASSERT(pBdata != NULL);

					pBdata->iCurPage = sData.iCurPage + 1;
					pBdata->iFlag = SEARCH_SOSO;
					pBdata->iRank = sData.iCurPage*10 + l;
					pBdata->strKeyword = sData.strKeyWord;
				}
				vHeights.push_back(l);
				
			}
		}
				
	}

	//if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	//{
	//	pBdata = new BackData;
	//	ASSERT(pBdata != NULL);

	//	pBdata->iCurPage = 0;
	//	pBdata->iFlag = SEARCH_SOSO;
	//	pBdata->iRank = 0;
	//	pBdata->strKeyword = strTitle;
	//}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_SOSO);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}
	

	return vHeights;
}

/*
	@brief  取得搜狗条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::CalSoGouHeight( IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pTables == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection sogou is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackData *pBdata = NULL;
	HRESULT hr;

	for (long l = 1; l <= 10; l++)
	{
		IDispatch  *pDisp = NULL;
		CString strTemp;

		strTemp.Format(_T("rb_%d"), l-1);

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = 0;

		VARIANT varID = {0};
		VariantInit(&varID);
		varID.vt = VT_BSTR;

		varID.bstrVal = strTemp.AllocSysString();
		hr = pTables->item(varID,index,&pDisp);
		SysFreeString(varID.bstrVal);

		if (FAILED(hr) || pDisp == NULL)
			continue;

		pElement = pDisp;
		pDisp->Release();


		CComBSTR bstr;
		pElement->get_innerHTML(&bstr);
		CString strPage(bstr);

		BOOL bFind = FindIsOwnWebInSoGou(strPage);

		if (bFind)
		{
			if (strPage.Find(sData.strKeyWord.GetString()) != -1)
			{
				//if (strPage.Find(strCompany.GetString()) == -1)    //这里注释掉，就是无论是否有公司名，都打开网址查看
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ：打开页面出现异常不处理"));
							break;
						}
						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ：没有在目标网址中找到公司名"));
						continue;
					}
				}

				//加红框
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());

				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("sogou ：加红框失败"));
				}

				if (vHeights.size() == 0)
				{
					pBdata = new BackData;
					ASSERT(pBdata != NULL);

					pBdata->iCurPage = sData.iCurPage + 1;
					pBdata->iFlag = SEARCH_SOGOU;
					pBdata->iRank = sData.iCurPage*10 + l;
					pBdata->strKeyword = sData.strKeyWord;
				}
				vHeights.push_back(l);
		
				
			}
		}		
	}

	//if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	//{
	//	pBdata = new BackData;
	//	ASSERT(pBdata != NULL);

	//	pBdata->iCurPage = 0;
	//	pBdata->iFlag = SEARCH_SOGOU;
	//	pBdata->iRank = 0;
	//	pBdata->strKeyword = strTitle;
	//}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_SOGOU);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}
	

	return vHeights;
}


/*
	@brief  判断搜搜的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL CPagePhotoDlg::FindIsOwnWebInSoSo( CString strHtml )
{
	//int iPos1,iPos2;
	//BOOL bReturn = FALSE;
	//strHtml.MakeLower();

	//iPos1 = iPos2 = 0;
	//iPos1 = strHtml.Find(_T("<cite id=\"cacheresult_info_"));

	//if(iPos1 != -1)
	//{
	//	iPos2 = strHtml.Find(_T("</cite>"), iPos1);
	//	if (iPos2 != -1)
	//	{
	//		//截取网址  14表示<SPAN class=g>这个字符串长度
	//		CString strUrl = strHtml.Mid(iPos1+30, iPos2-iPos1-30);
	//		int iPos3 = strUrl.Find(_T("/"));

	//		if (iPos3 == -1)
	//			iPos3 = strUrl.Find(_T("..."));

	//		if (iPos3 == -1)
	//			iPos3 = strUrl.Find(_T("&nbsp;"));

	//		if (iPos3 != -1)
	//		{
	//			strUrl = strUrl.Left(iPos3);
	//		}

	//		GetMainDomain(strUrl);

	//		char *pChar = WideToChar(strUrl.GetString());
	//		if (pChar != NULL)
	//		{
	//			if (strWeb.Find(pChar) != -1)
	//			{
	//				bReturn = TRUE;
	//			}
	//			delete []pChar;
	//		}

	//	}
	//}

	//return bReturn;

	return FindIsOwnWebInGoogle(strHtml);
}


/*
	@brief  判断搜狗的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL CPagePhotoDlg::FindIsOwnWebInSoGou( CString strHtml )
{
	return FindIsOwnWebInGoogle(strHtml);
}

/*
	@brief 根据搜索标志得到搜索引擎名称
	@param iFlag  搜索标志
	@return 搜索引擎名称
	*/
CString CPagePhotoDlg::GetSearchFlag( int iFlag )
{
	CString strSearch;

	switch(iFlag)
	{
	case SEARCH_BAIDU:       //百度
		strSearch = _T("百度");
		break;
	case SEARCH_GOOGLE:       //谷歌
		strSearch = _T("谷歌");
		break;
	case SEARCH_360:       //360搜索
		strSearch = _T("360搜索");
		break;
	case SEARCH_SOSO:       //soso搜索
		strSearch = _T("soso搜搜");
		break;
	case SEARCH_SOGOU:       //搜狗
		strSearch = _T("搜狗");
		break;
	case SEARCH_BING:       //必应
		strSearch = _T("必应");
		break;
	case SEARCH_PHONEBAIDU:
		strSearch = _T("手机百度");
		break;
	}

	return strSearch;
}


/*
	@brief  取得必应条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
std::vector<long> CPagePhotoDlg::CalBingHeight( IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult)
{
	std::vector<long> vHeights;  //返回高度，可能有多条数据
	if (pLis == NULL)
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("IHTMLElementCollection bing is NULL"));
		return vHeights;
	}

	CComQIPtr<IHTMLElement> pElement;
	BackData *pBdata = NULL;
	HRESULT hr;

	int iRank = 0;
	long lLiCounts = 0;
	pLis->get_length(&lLiCounts);

	for (long l = 0; l <= lLiCounts; l++)
	{
		IDispatch  *pDisp = NULL;

		VARIANT index = {0};
		V_VT(&index) = VT_I4;
		V_I4(&index) = l;

		hr = pLis->item(COleVariant(l),index,&pDisp);
		if (FAILED(hr) || pDisp == NULL)
			continue;

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

		BOOL bFind = FindIsOwnWebInBing(strPage);

		if (bFind)
		{
			if (strPage.Find(sData.strKeyWord.GetString()) != -1)
			{
				//if (strPage.Find(strCompany.GetString()) == -1)  //这里注释掉，就是无论是否有公司名，都打开网址查看
				{
					int iResult = HasCompanyInHtml(GetUrlFormContent(strPage), sData);
					if (iResult != 1)
					{
						if (iResult == 2)
						{
							bResult = TRUE;
							g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ：打开页面异常不处理"));
							break;
						}

						g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ：没有在目标网址中找到公司名"));
						continue;
					}
				}

				//加红框
				strPage.Insert(0, _T("<div style=\"padding: 5px; border: 2px solid rgb(223, 0, 1); border-image: none; margin-bottom: 5px; box-shadow: 1px 1px 3px 0px rgba(0,0,0,0.6);\">"));
				strPage.Append(_T("</div>"));
				hr = pElement->put_innerHTML((BSTR)strPage.GetString());
				
				if (FAILED(hr))
				{
					g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("bing ：加红框失败"));
				}
			
				if (vHeights.size() == 0)
				{
					pBdata = new BackData;
					ASSERT(pBdata != NULL);

					pBdata->iCurPage = sData.iCurPage + 1;
					pBdata->iFlag = SEARCH_BING;
					pBdata->iRank = sData.iCurPage*10 + iRank;
					pBdata->strKeyword = sData.strKeyWord;
				}
				vHeights.push_back(sData.iCurPage*10 + iRank);

			}
		}	
	}

	//if (pBdata == NULL && iPage == 2)  //此搜索引擎无抓取到快照
	//{
	//	pBdata = new BackData;
	//	ASSERT(pBdata != NULL);

	//	pBdata->iCurPage = 0;
	//	pBdata->iFlag = SEARCH_BING;
	//	pBdata->iRank = 0;
	//	pBdata->strKeyword = strTitle;
	//}

	if (!bResult)
	{
		if (pBdata != NULL)
		{
			CString strCompany;
			if (sData.vCompanys.size() > 0)
			{
				strCompany = sData.vCompanys[0].Left(sData.vCompanys[0].GetLength()-1);
			}

			pBdata->iCount = vHeights.size();
			GetImageFilePath(sData.strKeyWord, strCompany, strFileName, SEARCH_BING);

			pBdata->strPagePath = strFileName;
			vectBackData.push_back(pBdata);
		}
	}
	

	return vHeights;
}

/*
	@brief  判断Bing搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
BOOL CPagePhotoDlg::FindIsOwnWebInBing( CString strHtml )
{
	return FindIsOwnWebInGoogle(strHtml);
}


/*
	@brief 将html保存到一个文件
	@param pHtml  html对象
	@param strFileName  html文件路径
	*/
void CPagePhotoDlg::SaveToHtml( IHTMLElementCollection *pHtml, const CString &strFileName )
{
	CComQIPtr<IDispatch> pDispatHtml;
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
				if (pA)
				{
					fwrite(pA, 1, strlen(pA), pFile);

					delete []pA;
				}
				

				fclose(pFile);
			}
		}
	}
	
}

void CPagePhotoDlg::ChangeButtonStyle( IHTMLElementCollection *pHtml, const SearchData *pSearchData)
{
	CComQIPtr<IDispatch> pDispatTemp;
	CComQIPtr<IHTMLElement> pElement;
	CString strUrl;
	CString strBtn;
	CComBSTR bstr;

	VARIANT index = {0};
	VARIANT varID = {0};

	strUrl.Format(_T(" onclick=\"javascript:location.href='%s'\" "), pSearchData->strUrl);

	V_VT(&index) = VT_I4;
	V_I4(&index) = 0;

	VariantInit(&varID);
	varID.vt = VT_BSTR;

	if (pSearchData->iFlag == SEARCH_BAIDU)
	{
		varID.bstrVal = _T("su");
	}
	else if (pSearchData->iFlag == SEARCH_360)
	{
		varID.bstrVal = _T("su");
	}
	else if (pSearchData->iFlag == SEARCH_SOSO)
	{
		varID.bstrVal = _T("searchHeaderSubmit");		
	}
	else if (pSearchData->iFlag == SEARCH_SOGOU)
	{
		varID.bstrVal = _T("searchBtn");		
	}
	else if (pSearchData->iFlag == SEARCH_BING)
	{
		varID.bstrVal = _T("sb_form_go");
	}
	else if (pSearchData->iFlag == SEARCH_PHONEBAIDU)
	{
		varID.bstrVal = _T("su");
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
			g_pageLog.Trace(LOGL_TOP, LOGT_WARNING, __TFILE__, __LINE__, _T("修改按钮样式失败  type:%d"), pSearchData->iFlag);

		SysFreeString(pStr);
	}
	else
	{
		g_pageLog.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("取不到相应按钮元素  type:%d"), pSearchData->iFlag);
	}

}


void CPagePhotoDlg::SetInternetFeature(void)
{
	LRESULT lr = 0;
	INTERNETFEATURELIST featureToEnable = FEATURE_RESTRICT_FILEDOWNLOAD ;

	//禁止下载文件
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_RESTRICT_FILEDOWNLOAD失败,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_RESTRICT_FILEDOWNLOAD失败,err:%d"), GetLastError());
	}

	//禁止网页导航及提交操作时发出声音
	featureToEnable = FEATURE_DISABLE_NAVIGATION_SOUNDS;
	if (SUCCEEDED(CoInternetSetFeatureEnabled(featureToEnable, SET_FEATURE_ON_PROCESS, true)))
	{
		//Check to make sure that the API worked as expected
		if (FAILED(CoInternetIsFeatureEnabled(featureToEnable,SET_FEATURE_ON_PROCESS)))
		{
			g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_DISABLE_NAVIGATION_SOUNDS失败,err:%d"), GetLastError());
		}
	}
	else
	{
		//The API returned an error while enabling pop-up management
		g_pageLog.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("设置FEATURE_DISABLE_NAVIGATION_SOUNDS失败,err:%d"), GetLastError());
	}
}

#define HTMLLENGTH  12000
int CPagePhotoDlg::JudgeHtml( IHTMLElementCollection *pHtml, const SearchData *pSearchData )
{
	int iResult = 0;
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



	if (pTags != NULL)
	{
		pTags->item(index, index,&pDispatTemp);

		pElement = pDispatTemp;
		if (pElement != NULL)
		{
			CComBSTR bstr;

			pElement->get_outerHTML(&bstr);
			
			strPage = bstr;
			if (strPage.GetLength() < HTMLLENGTH)
			{
				iResult = 1;
				g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("打开页面的html字符串小于%d, 非正常页面, 网址为：%s"), HTMLLENGTH, pSearchData->strUrl.GetString());

				if (strPage.Find(_T("验证码")) != -1)
				{
					iResult = 2;
					g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("发现验证码字样,当前搜索引擎撤消搜索, 网址为:%s"), pSearchData->strUrl.GetString());
					g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("网页HTML内容为：%s"), strPage.GetString());
				}
				else
				{
					g_pageLog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("页面不能正常打开，清除缓存"));
					DeleteSearchCache();
				}
			}

		}
	}


	return iResult;
}
