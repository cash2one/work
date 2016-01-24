// CFilterPost.cpp : 实现文件
//

#include "stdafx.h"
#include "FilterRule.h"
#include "CFilterPost.h"
#include "afxdialogex.h"


// CCFilterPost 对话框

IMPLEMENT_DYNAMIC(CCFilterPost, CDialogEx)

CCFilterPost::CCFilterPost(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCFilterPost::IDD, pParent)
{

}

CCFilterPost::~CCFilterPost()
{
}

void CCFilterPost::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_MCMPTYPE, m_cboMainCmp);
	DDX_Control(pDX, IDC_CBO_CCMPTYPE, m_CustmonCmp);
}


BEGIN_MESSAGE_MAP(CCFilterPost, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOADINSTALL, &CCFilterPost::OnBnClickedBtnLoadinstall)
	ON_BN_CLICKED(IDC_BTN_SAVEINSTALL, &CCFilterPost::OnBnClickedBtnSaveinstall)
END_MESSAGE_MAP()


// CCFilterPost 消息处理程序


void CCFilterPost::OnBnClickedBtnLoadinstall()
{
	// TODO:  在此添加控件通知处理程序代码
	// TODO:  在此添加控件通知处理程序代码
	acl::string strBuf;
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = "导入xml文件";
	fileDlg.m_ofn.lpstrFilter = "XML文件(*.xml)\0*.xml";

	if (IDOK == fileDlg.DoModal())
	{
		acl::string strFile = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();

		CFile file(strFile, CFile::modeRead);
		DWORD dwLen;
		dwLen = file.GetLength();
		char* pBuf = new char[dwLen + 1];
		memset(pBuf, 0, dwLen + 1);
		file.Read(pBuf, dwLen);
		strBuf = pBuf;
		delete[]pBuf;
		file.Close();
	}
	else
	{
		return;
	}

	acl::xml xml(strBuf);
	std::vector<acl::xml_node*> vXmlnode;


	//标签
	vXmlnode = xml.getElementsByTagName("fileter");
	for (int i = 0; i < vXmlnode.size(); i++)
	{	
		GetXmlAttr(vXmlnode[i]);
	}
}


void CCFilterPost::OnBnClickedBtnSaveinstall()
{
	// TODO:  在此添加控件通知处理程序代码
	string buf;
	CString strValue;
	xml xml;
	xml_node& root = xml.get_root();
	xml_node& install = xml.create_node("intallupdate");
	root.add_child(install);

	//主线版
	xml_node& filter = xml.create_node("fileter");
	install.add_child(filter);
	//类型
	filter.add_attr("type", "0");
	//版本
	GetDlgItemText(IDC_EDIT_MVER, strValue);
	filter.add_attr("version", strValue);
	//比较方法
	m_cboMainCmp.GetLBText(m_cboMainCmp.GetCurSel(), strValue);
	filter.add_attr("cmptype", strValue);
	//下载地址
	GetDlgItemText(IDC_EDIT_MDOWNURL, strValue);
	filter.add_attr("downloadurl", strValue);
	//md5值
	GetDlgItemText(IDC_EDIT_MMD5, strValue);
	filter.add_attr("md5", strValue);
	//---以下是可选的；
	GetDlgItemText(IDC_EDIT_MLMTTIME, strValue);
	strValue = strValue.Trim();
	if (strValue.GetLength() > 0)
	{
		filter.add_attr("limit_time", strValue);
		GetDlgItemText(IDC_EDIT_MLMTCNT, strValue);
		filter.add_attr("limit_count", strValue);
	}

	//定制版
	filter = xml.create_node("fileter");
	install.add_child(filter);
	//类型
	filter.add_attr("type", "1");
	//版本
	GetDlgItemText(IDC_EDIT_CVER, strValue);
	filter.add_attr("version", strValue);
	//比较方法
	m_CustmonCmp.GetLBText(m_CustmonCmp.GetCurSel(), strValue);
	filter.add_attr("cmptype", strValue);
	//下载地址
	GetDlgItemText(IDC_EDIT_CDOWNURL, strValue);
	filter.add_attr("downloadurl", strValue);
	//md5值
	GetDlgItemText(IDC_EDIT_CMD5, strValue);
	filter.add_attr("md5", strValue);
	//---以下是可选的；
	GetDlgItemText(IDC_EDIT_CLMTTIME, strValue);
	strValue = strValue.Trim();
	if (strValue.GetLength() > 0)
	{
		filter.add_attr("limit_time", strValue);
		GetDlgItemText(IDC_EDIT_CLMTCNT, strValue);
		filter.add_attr("limit_count", strValue);
	}
	//保存
	xml.build_xml(buf);
	string strRes;
	strRes.format("<root>%s</root>", buf.c_str());
	string strSavepath = GetCurPath();
	FILE* xmlFile;
	fopen_s(&xmlFile, strSavepath + "\\InstallUpdateRule.xml", "w");
	fprintf_s(xmlFile, strRes);
	fclose(xmlFile);

	MessageBox(_T("生成成功！"));

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("推送安装包过滤xml生成完成。"));
}

//初始化数据
void CCFilterPost::InitData()
{
	//主线版比较类型
	m_cboMainCmp.InsertString(0, "大于");
	m_cboMainCmp.InsertString(1, "小于");
	m_cboMainCmp.InsertString(2, "等于");
	m_cboMainCmp.SetCurSel(0);

	//定制版比较类型
	m_CustmonCmp.InsertString(0, "大于");
	m_CustmonCmp.InsertString(1, "小于");
	m_CustmonCmp.InsertString(2, "等于");
	m_CustmonCmp.SetCurSel(0);

	m_strCurType = "0";
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("初始化推送安装包设置模块"));
}

//设置版本号比较类型
void CCFilterPost::SetCompareType(const string& strCmp,CComboBox* cbo)
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

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("未找到对应的比较类型:%s"),strCmp.c_str());
}

//填写值到对应的框框；
void CCFilterPost::SetValutToCtl(const string& strName, const string& strValue)
{
	if (strName == "type")
	{
		m_strCurType = strValue;
		return;
	}

	if (m_strCurType == "1")
	{
		if (strName == "version")
		{
			SetDlgItemText(IDC_EDIT_CVER, strValue);
		}
		else if (strName == "cmptype")
		{
			SetCompareType(strValue, &m_CustmonCmp);
		}
		else if (strName == "downloadurl")
		{
			SetDlgItemText(IDC_EDIT_CDOWNURL, strValue);
		}
		else if (strName == "md5")
		{
			SetDlgItemText(IDC_EDIT_CMD5, strValue);
		}
		else if (strName == "limit_time")
		{
			SetDlgItemText(IDC_EDIT_CLMTTIME, strValue);
		}
		else if (strName == "limit_count")
		{
			SetDlgItemText(IDC_EDIT_CLMTCNT, strValue);
		}
		else
		{

		}
		return;
	}
	else if (m_strCurType == "0")
	{
		if (strName == "version")
		{
			SetDlgItemText(IDC_EDIT_MVER, strValue);
		}
		else if (strName == "cmptype")
		{
			SetCompareType(strValue, &m_cboMainCmp);
		}
		else if (strName == "downloadurl")
		{
			SetDlgItemText(IDC_EDIT_MDOWNURL, strValue);
		}
		else if (strName == "md5")
		{
			SetDlgItemText(IDC_EDIT_MMD5, strValue);
		}
		else if (strName == "limit_time")
		{
			SetDlgItemText(IDC_EDIT_MLMTTIME, strValue);
		}
		else if (strName == "limit_count")
		{
			SetDlgItemText(IDC_EDIT_MLMTCNT, strValue);
		}
		else
		{

		}
		return;
	}
	else
	{
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("类型尚未确定！ %s"),strName.c_str());
	}
}

//获取属性和值
bool CCFilterPost::GetXmlAttr(xml_node* node)
{
	if (node == NULL)
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("接收到的node为NULL"));
		return false;
	}

	string strName, strValue;
	const acl::xml_attr* xmlAtt;

	xmlAtt = node->first_attr();
	while (xmlAtt != NULL)
	{
		strName = xmlAtt->get_name();
		strValue = xmlAtt->get_value();
		//转换小写
		strName = strName.lower();
		SetValutToCtl(strName, strValue);
		//读取下一个属性
		xmlAtt = node->next_attr();
	}

	return true;
}
