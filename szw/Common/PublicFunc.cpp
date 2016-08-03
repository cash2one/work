#include "stdafx.h"
#include "PublicFunc.h"
#include <WinInet.h>

#pragma comment(lib, "Wininet.lib")

CStdString ComBineUrl(CStdString strBase, CStdString strRelative)
{
	BOOL bSucc;
	CStdString strOut;
	unsigned long nLen = 0;
	TCHAR* dd = NULL;
	InternetCombineUrl(strBase,strRelative,dd,&nLen,ICU_BROWSER_MODE);

	if(nLen > 0)
	{
		dd = new TCHAR[nLen+1];
		::ZeroMemory(dd,nLen+1);
		bSucc = InternetCombineUrl(strBase,strRelative,dd,&nLen,ICU_BROWSER_MODE);

		if(bSucc)
		{
			CStdString strResult (dd);
			delete []dd;
			return strResult;
		}

		else
		{
			delete []dd;
			return _T("");
		}
	}

	return _T("");
}

CString ComBineUrl(CString strBase, CString strRelative)
{
	BOOL bSucc;
	CString strOut;
	unsigned long nLen = 0;
	TCHAR* dd = NULL;
	InternetCombineUrl(strBase,strRelative,dd,&nLen,ICU_BROWSER_MODE);

	if(nLen > 0)
	{
		dd = new TCHAR[nLen+1];
		::ZeroMemory(dd,nLen+1);
		bSucc = InternetCombineUrl(strBase,strRelative,dd,&nLen,ICU_BROWSER_MODE);

		if(bSucc)
		{
			CString strResult (dd);
			delete []dd;
			return strResult;
		}

		else
		{
			delete []dd;
			return _T("");
		}
	}

	return _T("");
}

//把图片或者问答问题的答案,拼凑成html文件,写到code目录下
DWORD WINAPI ComBineHtml(CStringArray &ImagePathArray,EPicShowType ePicShowType,
						 CString strIndex,CString &strRetPath, CString strInputPrompt)
{
	//本地安装路径
// 	TCHAR Path[MAX_PATH];
// 	::GetCurrentDirectory(MAX_PATH,Path);
	TCHAR *Path = GetInstallPath();

	CString strShowBuf,strHtmlName, strTmpPath;
	INT_PTR dwCount = ImagePathArray.GetSize();

	strShowBuf = _T("<meta http-equiv=\"content-type\" content=\"text/html;charset=GB2312\">\r\n\<table width=\"200\" height=\"110\" border=\"0\"><tr><td align=\"center\" valign=\"middle\">");
	if (E_Single_Pic == ePicShowType && 1 ==  dwCount)
	{
		CString strImagePath = ImagePathArray.GetAt(0);
		strImagePath.Format(_T("<img src=\"%s\" />"),CString(strImagePath));
		strShowBuf += strImagePath + _T("<br>");
		strShowBuf += strInputPrompt + _T("</td></tr></table>");
		strTmpPath.Format(_T("%s\\image\\Code\\%s_1.html"),Path,strIndex);				//单图片问题路径 1 
	}
	else if (E_Multi_Pic == ePicShowType)
	{
		INT_PTR dwCount = ImagePathArray.GetSize();
		CString strPathAll,strImagePath;
		for (INT_PTR i = 0; i < dwCount;i++)
		{
			strImagePath = ImagePathArray.GetAt(i);
			if(strImagePath.Left(3).Compare(_T("<#>")) != 0)
			{
				strImagePath.Format(_T("<img src=\"%s\" />"),CString(strImagePath));
				strPathAll += strImagePath;
			}
			else
			{
				if(strImagePath.GetLength() > 3)
				{
					strImagePath = strImagePath.Mid(3,strImagePath.GetLength() - 3);
					strPathAll += strImagePath;
				}
			}
			strImagePath.Empty();
		}
		/*CString strTmp;
			strTmp = _T("<span class=\"form-input\"> \
						<select id=\"UserAbo\" name=\"blood\"> \
						<option value=\"0\">请选择</option> \
						<option value=\"1\" >A型</option> \
						<option value=\"2\" >B型</option> \
						<option value=\"3\" >O型</option> \
						<option value=\"4\" >AB型</option> \
						<option value=\"5\" >其他</option> \
						</select>");
			strPathAll += strTmp;*/
		strShowBuf += strPathAll + _T("</td></tr></table>");
		strTmpPath.Format(_T("%s\\image\\Code\\%s_2.html"),Path,strIndex);				//多图片问题路径  2
	}
	else if (E_TEXT_PIC == ePicShowType && 1 ==  dwCount)
	{
		CString strTextConent;
		strTextConent = ImagePathArray.GetAt(0);
		strShowBuf += strTextConent + _T("<br>");
		strShowBuf += strInputPrompt + _T("</td></tr></table>");
		strTmpPath.Format(_T("%s\\image\\Code\\%s_3.html"),Path,strIndex);					//问题路径      3
	}
	else if (E_SELECTOPT_Pic == ePicShowType&& 2 == dwCount)							//下拉框         4
	{
		//下载图片到本地或者 在html中加入 <a base href = "host"> 
		CString csBaseHref = ImagePathArray.GetAt(1);
		strShowBuf +=ImagePathArray.GetAt(0) + _T("<br>");
		strShowBuf += strInputPrompt + _T("</td></tr></table>");
		strTmpPath.Format(_T("%s\\image\\Code\\%s_4.html"),Path,strIndex);
		strShowBuf.Format(_T("<base href =\" %s\">%s"),csBaseHref,CString(strShowBuf));
	}


	//后面保存成到本地Code目录下的html文件
	if (strRetPath != _T(""))
	{
		strTmpPath = strRetPath;       //调用时已经指定了文件路径则使用指定的路径，否则使用自动生成的路径
	}

	int nPathLen = strTmpPath.GetLength();
	if (0 == nPathLen)
	{
// 		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__,\
// 			_T("strRetPath的长度为空"));
		return 1;
	}

	char pPath[300] = {0};
	TCHAR tcPath[MAX_PATH] = {0};
	TCHAR szDriver[_MAX_DRIVE] = {0};
	TCHAR szPath[_MAX_PATH] = {0};

	_tsplitpath(strTmpPath, szDriver, szPath, NULL, NULL);
	ZeroMemory(tcPath, MAX_PATH * sizeof(TCHAR));
	_tcscat(tcPath, szDriver);
	_tcscat(tcPath, szPath);
	int nFlag = SHCreateDirectoryEx(NULL, tcPath, NULL);

	if (ERROR_SUCCESS  == nFlag ||
		ERROR_FILE_EXISTS == nFlag ||
		ERROR_ALREADY_EXISTS == nFlag)
	{
		FILE *pFFail = _tfopen(strTmpPath,_T("w+"));
		if (pFFail && strShowBuf)
		{		
			char *lpszText2 = _com_util::ConvertBSTRToString(_bstr_t(strShowBuf));

			size_t dwSize =strlen(lpszText2);

			fwrite(lpszText2,sizeof(char),dwSize,pFFail);

			
			delete[] lpszText2;
		}	
		if(pFFail!=NULL)fclose(pFFail);
	}			

	strRetPath = strTmpPath;
	return 1;
}

bool setWndTransparent( HWND hwnd, byte wTransparency/*=1*/ )
{
	DWORD dwExStyle=GetWindowLong(hwnd,GWL_EXSTYLE); 

	if((dwExStyle&0x80000)!=0x80000) 
	{
		SetWindowLong(hwnd,GWL_EXSTYLE,GetWindowLong(hwnd,GWL_EXSTYLE)^0x80000);
	}

	HINSTANCE hInst = LoadLibrary(_T("User32.DLL")); 

	if(hInst) 
	{ 
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)
		{
			fun(hwnd,0,wTransparency,2); 
		}
		else
		{
			return false;
		}
		FreeLibrary(hInst); 
	}
	else
	{
		return false;
	}

	return true;
}

void GetWndTransparentValue(HWND hwnd, BYTE& balpha)
{
	BOOL bsuccess = GetLayeredWindowAttributes(hwnd, NULL, &balpha, NULL);

	if (bsuccess)
	{
		return;
	}
	else
	{
		balpha = 0;
	}
}

bool GetFlashURL(CString& strURL, const CString& strOutHtml)
{
	strURL.Empty();
	CString strtemp = strOutHtml;

	int ipos = strtemp.Find(_T("<PARAM NAME=\"Movie\""));

	if (ipos == -1)
	{
		return FALSE;
	}
	else
	{
		int ipos1 = strtemp.Find(_T(">"), ipos);

		if (ipos1 == -1)
		{
			return FALSE;
		}

		//解析出对应的URL
		CString strMovie = strtemp.Mid(ipos, (ipos1 - ipos) + 1);
		strMovie.Replace(_T("<PARAM NAME=\"Movie\""), _T(""));
		strMovie.Replace(_T("VALUE=\""), _T(""));
		strMovie.Replace(_T("\">"), _T(""));
		
		strMovie.TrimLeft();
		strMovie.TrimRight();

		strURL = strMovie;

		return TRUE;
	}
}

CString comBineFlashHtml(const CString& strURL, const CString& strUniID)
{	
	CString strPath;
	CString strContent;
	TCHAR *Path = GetInstallPath();

	strPath.Format(_T("%s\\image\\Code\\%s.html"), Path,strUniID);
	strContent = _T("<html>\r\n<body>\r\n<table width=\"200\" height=\"110\" border=\"0\"><tr><td align=\"center\" valign=\"middle\">");
	strContent += _T("<OBJECT id=playlist classid=clsid:D27CDB6E-AE6D-11cf-96B8-444553540000 width=80 height=32><PARAM NAME=\"FlashVars\" VALUE=\"\"><PARAM NAME=\"Movie\" VALUE=\"");
	strContent += strURL;
	strContent += _T("\"><PARAM NAME=\"WMode\" VALUE=\"Transparent\"><PARAM NAME=\"Play\" VALUE=\"0\"><PARAM NAME=\"Loop\" VALUE=\"-1\">");
	strContent += _T("<PARAM NAME=\"Quality\" VALUE=\"High\"><PARAM NAME=\"SAlign\" VALUE=\"\"><PARAM NAME=\"Menu\" VALUE=\"-1\">");
	strContent += _T("<PARAM NAME=\"Base\" VALUE=\"\"><PARAM NAME=\"AllowScriptAccess\" VALUE=\"\"><PARAM NAME=\"Scale\" VALUE=\"ShowAll\">");
	strContent += _T("<PARAM NAME=\"DeviceFont\" VALUE=\"0\"><PARAM NAME=\"EmbedMovie\" VALUE=\"0\"><PARAM NAME=\"BGColor\" VALUE=\"CCCCCC\">");
	strContent += _T("<PARAM NAME=\"SWRemote\" VALUE=\"\"><PARAM NAME=\"MovieData\" VALUE=\"\"><PARAM NAME=\"SeamlessTabbing\" VALUE=\"1\"><PARAM NAME=\"Profile\" VALUE=\"0\">");
	strContent += _T("<PARAM NAME=\"ProfileAddress\" VALUE=\"\"><PARAM NAME=\"ProfilePort\" VALUE=\"0\"><PARAM NAME=\"AllowNetworking\" VALUE=\"all\"><PARAM NAME=\"AllowFullScreen\" VALUE=\"false\">");
	strContent += _T("<PARAM NAME=\"AllowFullScreenInteractive\" VALUE=\"false\"><PARAM NAME=\"IsDependent\" VALUE=\"118\"></OBJECT>\r\n<br></td></tr></table>\r\n</body>\r\n</html>");

	char *lpszText2 = _com_util::ConvertBSTRToString(_bstr_t(strContent));

	CFile file;

	if (!file.Open(strPath, CFile::modeCreate  | CFile::modeWrite | CFile::typeBinary))
	{
		return _T("");
	}
	else
	{
		file.Write(lpszText2, strlen(lpszText2));
		file.Close();

		return strPath;
	}
}
