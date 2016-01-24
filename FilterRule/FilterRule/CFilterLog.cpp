// CFilterLog.cpp : 实现文件
//

#include "stdafx.h"
#include "FilterRule.h"
#include "CFilterLog.h"
#include "afxdialogex.h"


// CCFilterLog 对话框

IMPLEMENT_DYNAMIC(CCFilterLog, CDialogEx)

CCFilterLog::CCFilterLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCFilterLog::IDD, pParent)
{

}

CCFilterLog::~CCFilterLog()
{
}

void CCFilterLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT_LOGVER, m_EdtVer);
	DDX_Control(pDX, IDC_CBO_CMPTYPELOG, m_cboCmptype);
	DDX_Control(pDX, IDC_CHK_SUBMITPHOTO, m_chkSubmitPhoto);
	DDX_Control(pDX, IDC_CBO_SPLIT, m_cboSplit);
	DDX_Control(pDX, IDC_CBO_CMPTYPEPOST, m_cboCmpPostType);
	DDX_Control(pDX, IDC_CHK_POSTUPGRADE, m_chkPostUpgrade);
	DDX_Control(pDX, IDC_CHK_BOOTSTART, m_chkBootStart);
}


BEGIN_MESSAGE_MAP(CCFilterLog, CDialogEx)
//	ON_BN_CLICKED(IDC_BUTTON1, &CCFilterLog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_LOADLOGXML, &CCFilterLog::OnBnClickedBtnLoadlogxml)
	ON_BN_CLICKED(IDC_BTN_SAVELOG, &CCFilterLog::OnBnClickedBtnSavelog)
//	ON_BN_CLICKED(IDC_BTN_CANCELLOG, &CCFilterLog::OnBnClickedBtnCancellog)
	ON_CBN_SELCHANGE(IDC_CBO_CMPTYPELOG, &CCFilterLog::OnCbnSelchangeCboCmptypelog)
	ON_BN_CLICKED(IDC_RAD_USERNAME, &CCFilterLog::OnBnClickedRadUsername)
	ON_BN_CLICKED(IDC_RAD_USERMAC, &CCFilterLog::OnBnClickedRadUsermac)
	ON_BN_CLICKED(IDC_RAD_VERSION, &CCFilterLog::OnBnClickedRadVersion)
	ON_BN_CLICKED(IDC_RAD_POSTUPGRADE, &CCFilterLog::OnBnClickedRadPostupgrade)
	ON_BN_CLICKED(IDC_RAD_FIRSTBOOT, &CCFilterLog::OnBnClickedRadFirstboot)
END_MESSAGE_MAP()


// CCFilterLog 消息处理程序


//void CCFilterLog::OnBnClickedButton1()
//{
//	// TODO:  在此添加控件通知处理程序代码
//}


void CCFilterLog::OnBnClickedBtnLoadlogxml()
{
	// TODO:  在此添加控件通知处理程序代码
	acl::string strBuf;
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = "导入xml文件";
	fileDlg.m_ofn.lpstrFilter = "XML文件(*.xml)\0*.xml";

	if (IDOK == fileDlg.DoModal())
	{
		acl::string strFile = fileDlg.GetFolderPath()+"\\" + fileDlg.GetFileName();
		
		CFile file(strFile, CFile::modeRead);
		DWORD dwLen;
		dwLen = file.GetLength();
		char* pBuf = new char[dwLen + 1];
		memset(pBuf, 0, dwLen+1);
		file.Read(pBuf, dwLen);
		strBuf = pBuf;
		delete []pBuf;
		file.Close();
	}
	else
	{
		return;
	}

	acl::xml xml(strBuf);
	std::vector<acl::xml_node*> vXmlnode;
	string strName, strValue;
	const acl::xml_attr* xmlAtt;
	const acl::xml_attr* xmlNextAttr;

	//获取推送升级第一个标签(推送升级)

	vXmlnode = xml.getElementsByTagName("updaterule");
	for (int i = 0; i < vXmlnode.size(); i++)
	{
		/*OnBnClickedRadPostupgrade();*/
		m_chkPostUpgrade.SetCheck(TRUE);
		xmlAtt = vXmlnode[i]->first_attr();
		strName = xmlAtt->get_name();
		if (strName == "version")
		{
			strValue = xmlAtt->get_value();
			SetDlgItemText(IDC_EDIT_POSTVER, strValue);
		}
		//比较类型
		xmlNextAttr = vXmlnode[i]->next_attr();
		if (xmlNextAttr != NULL)
		{
			strName = xmlNextAttr->get_name();
			if (strName == "cmptype")
			{
				strValue = xmlAtt->get_value();
				SetCompareType(strValue,&m_cboCmpPostType);
			}
		}

	}

	//获取第一个标签   （开机启动）
	vXmlnode = xml.getElementsByName("writeregedit");
	for (int i = 0; i < vXmlnode.size(); i++)
	{
		OnBnClickedRadFirstboot();
		m_chkBootStart.SetCheck(TRUE);
		xmlAtt = vXmlnode[i]->first_attr();
		strName = xmlAtt->get_name();
		if (strName == "runflag")
		{
			strValue = xmlAtt->get_value();
			if (strValue == _T("1"))
			{
				m_chkSubmitPhoto.SetCheck(TRUE);
			}
		}
	}


	//获取第一个标签  （获取日志）
	vXmlnode = xml.getElementsByTagName("sumitlog");
	for (int i = 0; i< vXmlnode.size(); i++)
	{
		xmlAtt = vXmlnode[i]->first_attr();
		strName = xmlAtt->get_name();
		if (strName == "type")
		{
			strValue = xmlAtt->get_value();
			SetRad(GetLogType(strValue));
		}

		xmlNextAttr = vXmlnode[i]->next_attr();
		if (xmlNextAttr != NULL)
		{
			strName = xmlNextAttr->get_name();
			if (strName == "sumittype")
			{
				strValue = xmlNextAttr->get_value();
				SetDlgItemText(IDC_EDIT_LOGTYPE, strValue);
/*
				if (strValue == "1")
				{
					SetDlgItemText()
					/ *m_chkSubmitPhoto.SetCheck(TRUE);* /
				}*/
			}
		}

	}
	
	string strMac = "";
	//第二个标签
	vXmlnode = xml.getElementsByTagName("fileter");

	if (m_LogType == E_LOG_VER)
	{
		//过滤版本
		for (int i = 0; i < vXmlnode.size(); i++)
		{
			//版本value值
			xmlAtt = vXmlnode[i]->first_attr();
			strName = xmlAtt->get_name();
			if (strName == "value")
			{
				strValue = xmlAtt->get_value();
				SetDlgItemText(IDC_EDIT_LOGVER, strValue);
			}
			//比较类型
			xmlNextAttr = vXmlnode[i]->next_attr();
			if (xmlNextAttr != NULL)
			{
				strName = xmlNextAttr->get_name();
				if (strName == "cmptype")
				{
					strValue = xmlAtt->get_value();
					SetCompareType(strValue,&m_cboCmptype);
				}
			}
		}

	}
	else
	{
		//非过滤版本（用户账号或mac);
		for (int i = 0; i < vXmlnode.size(); i++)
		{
			xmlAtt = vXmlnode[i]->first_attr();
			if (xmlAtt != NULL)
			{
				strName = xmlAtt->get_name();
				if (strName == "value")
				{
					strValue = xmlAtt->get_value();
					strMac += strValue + "\r\n";
				}
			}
		}
		SetDlgItemText(IDC_EDIT_MACORUSER, strMac);
	}

	
}


void CCFilterLog::OnBnClickedBtnSavelog()
{
	// TODO:  在此添加控件通知处理程序代码
	string  buf,strTmp;
	string	strSplite;
	CString  strValue;
	int iChk;
	std::vector<string> vStrValues;

	xml xml;
	xml_node& root = xml.get_root();
	if (m_chkPostUpgrade.GetCheck())
	{
		xml_node& updaterule = xml.create_node("updaterule");
		root.add_child(updaterule);

		GetDlgItemText(IDC_EDIT_POSTVER, strValue);
		updaterule.add_attr("version", strValue);
		
		m_cboCmptype.GetLBText(m_cboCmpPostType.GetCurSel(), strValue);
		updaterule.add_attr("cmptype", strValue);
	}
	
	if (m_chkBootStart.GetCheck())
	{
		xml_node& writeregedit = xml.create_node("writeregedit");
		root.add_child(writeregedit);

		iChk = m_chkSubmitPhoto.GetCheck();
		if (iChk == 1)
		{
			writeregedit.add_attr("runflag", iChk);
		}
	}
	

	{
		xml_node& sumit = xml.create_node("sumitlog");
		root.add_child(sumit);

		xml_node& filter = xml.create_node("fileter");
		sumit.add_child(filter);

		switch (m_LogType)
		{
		case E_LOG_VER:
			sumit.add_attr("type", "version");
			GetDlgItemText(IDC_EDIT_LOGVER, strValue);
			filter.add_attr("value", strValue);
			m_cboCmptype.GetLBText(m_cboCmptype.GetCurSel(), strValue);
			filter.add_attr("cmptype", strValue);
			break;
		case E_LOG_MAC:
		case E_LOG_USER:
			//Value值
			if (m_LogType == E_LOG_MAC)
			{
				sumit.add_attr("type", "usermac");
			}
			else
			{
				sumit.add_attr("type", "username");
			}
			GetDlgItemText(IDC_EDIT_MACORUSER, strValue);
			strTmp = strValue;
			//获取分割符号
			iChk = m_cboSplit.GetCurSel();
			strSplite = SelectSplit(iChk);

			//如果分隔符为空
			if (strSplite.length() <= 0)
			{
				g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("检测到未设置分隔符，使用默认回车分割。"));
				strSplite = "\r\n";
			}
			vStrValues = strTmp.split2(strSplite);

			for (int i = 0; i < vStrValues.size(); i++)
			{
				filter.add_attr("value", vStrValues[i]);
				if (i != vStrValues.size() - 1)
				{
					filter = xml.create_node("fileter");
					sumit.add_child(filter);
				}
			}
			//是否提交快照/或者错误验证码

			GetDlgItemText(IDC_EDIT_LOGTYPE, strValue);
			if (strValue.GetLength()<=0)
			{
				strValue = _T("0");
			}			
			sumit.add_attr("sumittype", strValue);
			
			break;
		default:
			break;
		}
	}
	//保存
	xml.build_xml(buf);
	string strRes;
	strRes.format("<root>%s</root>", buf.c_str());
	string strSavepath = GetCurPath();
	FILE* xmlFile;
	fopen_s(&xmlFile,strSavepath + "\\FilterRule.xml", "w");
	fprintf_s(xmlFile, strRes);
	fclose(xmlFile);

	MessageBox(_T("生成成功！"));

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("日志过滤xml生成完成。"));
}


//void CCFilterLog::OnBnClickedBtnCancellog()
//{
//	// TODO:  在此添加控件通知处理程序代码
//	OnOK();
//}


void CCFilterLog::OnCbnSelchangeCboCmptypelog()
{
	// TODO:  在此添加控件通知处理程序代码
}

//程序初始化
void CCFilterLog::InitData()
{
	//默认单选过滤MAC
	OnBnClickedRadUsermac();

	//取日志比较类型
	m_cboCmptype.InsertString(0, "大于");
	m_cboCmptype.InsertString(1, "小于");
	m_cboCmptype.InsertString(2, "等于");
	m_cboCmptype.SetCurSel(0);

	//推送比较类型
	m_cboCmpPostType.InsertString(0, "大于");
	m_cboCmpPostType.InsertString(1, "小于");
	m_cboCmpPostType.InsertString(2, "等于");
	m_cboCmpPostType.SetCurSel(0);

	//分隔符
	m_cboSplit.InsertString(0, "回车(\r\n)");
	m_cboSplit.InsertString(1, "逗号(,)");
	m_cboSplit.InsertString(2, "分号（;)");
	m_cboSplit.InsertString(3, "也可自定义...");

	m_cboSplit.SetCurSel(0);
	m_cboCmpPostType.SetCurSel(0);
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("初始化过滤下载日志模块"));
}

//获取提取日志类型字符串
E_LOG_TYPE CCFilterLog::GetLogType(const acl::string& strType)
{

	string strTmp = strType;
	strTmp = strTmp.lower();

	if (strTmp == "username")
	{
		return E_LOG_USER;
	}
	else if (strTmp == "usermac")
	{
		return E_LOG_MAC;
	}
	else if (strTmp == "version")
	{
		return E_LOG_VER;
	}

	return E_LOG_MAC;

}

//根据类型选定单选框
void CCFilterLog::SetRad(E_LOG_TYPE eType)
{
	switch (eType)
	{
	case E_LOG_MAC:		
		OnBnClickedRadUsermac();
		break;
	case E_LOG_USER:		
		OnBnClickedRadUsername();
		break;
	case E_LOG_VER:
		OnBnClickedRadVersion();
		break;
	case E_POST_UPGRADE:
		OnBnClickedRadPostupgrade();
		break;
	case E_FIRST_BOOT:
		OnBnClickedRadFirstboot();
		break;
	default:
		break;
	}

	m_LogType = eType;
}

void CCFilterLog::OnBnClickedRadUsername()
{
	// TODO:  在此添加控件通知处理程序代码
	m_LogType = E_LOG_USER;
	((CButton*)GetDlgItem(IDC_RAD_USERNAME))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RAD_USERMAC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_VERSION))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_POSTUPGRADE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_FIRSTBOOT))->SetCheck(FALSE);
	GetDlgItem(IDC_CBO_CMPTYPELOG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MACORUSER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_SUBMITPHOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CBO_SPLIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LOGTYPE)->EnableWindow(TRUE);
	/*SetDlgItemText(IDC_CHK_SUBMITPHOTO, _T("是否提交快照"));*/
}


void CCFilterLog::OnBnClickedRadUsermac()
{
	// TODO:  在此添加控件通知处理程序代码
	m_LogType = E_LOG_MAC;
	((CButton*)GetDlgItem(IDC_RAD_USERMAC))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RAD_USERNAME))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_VERSION))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_POSTUPGRADE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_FIRSTBOOT))->SetCheck(FALSE);
	GetDlgItem(IDC_CBO_CMPTYPELOG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MACORUSER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHK_SUBMITPHOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CBO_SPLIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LOGTYPE)->EnableWindow(TRUE);
	/*SetDlgItemText(IDC_CHK_SUBMITPHOTO, _T("是否提交快照"));*/
}


void CCFilterLog::OnBnClickedRadVersion()
{
	// TODO:  在此添加控件通知处理程序代码
	m_LogType = E_LOG_VER;	
	((CButton*)GetDlgItem(IDC_RAD_VERSION))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RAD_USERMAC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_USERNAME))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_POSTUPGRADE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_FIRSTBOOT))->SetCheck(FALSE);
	GetDlgItem(IDC_CBO_CMPTYPELOG)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MACORUSER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_SUBMITPHOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CBO_SPLIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGTYPE)->EnableWindow(TRUE);
	/*SetDlgItemText(IDC_CHK_SUBMITPHOTO, _T("是否提交快照"));*/
}

//设置版本号比较类型
void CCFilterLog::SetCompareType(const string& strCmp, CComboBox* cbo)
{
	if (strCmp == "大于")
	{
		cbo->SetCurSel(0);
	}
	else if (strCmp == "小于")
	{
		cbo->SetCurSel(1);
	}
	else 
	{
		cbo->SetCurSel(2);
	}
}

//选择分割类型；
string CCFilterLog::SelectSplit(int Index)
{
	switch (Index)
	{
	case 0:
		return "\r\n";
		break;
	case 1:
		return ",";
		break;
	case 2:
		return ";";
		break;
	default:
		break;
	}

	return "";
}


void CCFilterLog::OnBnClickedRadPostupgrade()
{
	// TODO:  在此添加控件通知处理程序代码
	m_LogType = E_POST_UPGRADE;
	((CButton*)GetDlgItem(IDC_RAD_POSTUPGRADE))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RAD_USERMAC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_VERSION))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_USERNAME))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_FIRSTBOOT))->SetCheck(FALSE);
	GetDlgItem(IDC_CBO_CMPTYPELOG)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MACORUSER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_SUBMITPHOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_CBO_SPLIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGTYPE)->EnableWindow(FALSE);
	/*SetDlgItemText(IDC_CHK_SUBMITPHOTO, _T("是否提交快照"));*/
}

//开机启动
void CCFilterLog::OnBnClickedRadFirstboot()
{
	// TODO:  在此添加控件通知处理程序代码
	m_LogType = E_FIRST_BOOT;
	/*SetDlgItemText(IDC_CHK_SUBMITPHOTO, _T("是否写开机启动"));*/
	((CButton*)GetDlgItem(IDC_RAD_FIRSTBOOT))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RAD_POSTUPGRADE))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_USERMAC))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_VERSION))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RAD_USERNAME))->SetCheck(FALSE);

	GetDlgItem(IDC_CBO_CMPTYPELOG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MACORUSER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_SUBMITPHOTO)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_LOGVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CBO_SPLIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOGTYPE)->EnableWindow(FALSE);
}
