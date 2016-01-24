// CFilterPost.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FilterRule.h"
#include "CFilterPost.h"
#include "afxdialogex.h"


// CCFilterPost �Ի���

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


// CCFilterPost ��Ϣ�������


void CCFilterPost::OnBnClickedBtnLoadinstall()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	acl::string strBuf;
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = "����xml�ļ�";
	fileDlg.m_ofn.lpstrFilter = "XML�ļ�(*.xml)\0*.xml";

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


	//��ǩ
	vXmlnode = xml.getElementsByTagName("fileter");
	for (int i = 0; i < vXmlnode.size(); i++)
	{	
		GetXmlAttr(vXmlnode[i]);
	}
}


void CCFilterPost::OnBnClickedBtnSaveinstall()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	string buf;
	CString strValue;
	xml xml;
	xml_node& root = xml.get_root();
	xml_node& install = xml.create_node("intallupdate");
	root.add_child(install);

	//���߰�
	xml_node& filter = xml.create_node("fileter");
	install.add_child(filter);
	//����
	filter.add_attr("type", "0");
	//�汾
	GetDlgItemText(IDC_EDIT_MVER, strValue);
	filter.add_attr("version", strValue);
	//�ȽϷ���
	m_cboMainCmp.GetLBText(m_cboMainCmp.GetCurSel(), strValue);
	filter.add_attr("cmptype", strValue);
	//���ص�ַ
	GetDlgItemText(IDC_EDIT_MDOWNURL, strValue);
	filter.add_attr("downloadurl", strValue);
	//md5ֵ
	GetDlgItemText(IDC_EDIT_MMD5, strValue);
	filter.add_attr("md5", strValue);
	//---�����ǿ�ѡ�ģ�
	GetDlgItemText(IDC_EDIT_MLMTTIME, strValue);
	strValue = strValue.Trim();
	if (strValue.GetLength() > 0)
	{
		filter.add_attr("limit_time", strValue);
		GetDlgItemText(IDC_EDIT_MLMTCNT, strValue);
		filter.add_attr("limit_count", strValue);
	}

	//���ư�
	filter = xml.create_node("fileter");
	install.add_child(filter);
	//����
	filter.add_attr("type", "1");
	//�汾
	GetDlgItemText(IDC_EDIT_CVER, strValue);
	filter.add_attr("version", strValue);
	//�ȽϷ���
	m_CustmonCmp.GetLBText(m_CustmonCmp.GetCurSel(), strValue);
	filter.add_attr("cmptype", strValue);
	//���ص�ַ
	GetDlgItemText(IDC_EDIT_CDOWNURL, strValue);
	filter.add_attr("downloadurl", strValue);
	//md5ֵ
	GetDlgItemText(IDC_EDIT_CMD5, strValue);
	filter.add_attr("md5", strValue);
	//---�����ǿ�ѡ�ģ�
	GetDlgItemText(IDC_EDIT_CLMTTIME, strValue);
	strValue = strValue.Trim();
	if (strValue.GetLength() > 0)
	{
		filter.add_attr("limit_time", strValue);
		GetDlgItemText(IDC_EDIT_CLMTCNT, strValue);
		filter.add_attr("limit_count", strValue);
	}
	//����
	xml.build_xml(buf);
	string strRes;
	strRes.format("<root>%s</root>", buf.c_str());
	string strSavepath = GetCurPath();
	FILE* xmlFile;
	fopen_s(&xmlFile, strSavepath + "\\InstallUpdateRule.xml", "w");
	fprintf_s(xmlFile, strRes);
	fclose(xmlFile);

	MessageBox(_T("���ɳɹ���"));

	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("���Ͱ�װ������xml������ɡ�"));
}

//��ʼ������
void CCFilterPost::InitData()
{
	//���߰�Ƚ�����
	m_cboMainCmp.InsertString(0, "����");
	m_cboMainCmp.InsertString(1, "С��");
	m_cboMainCmp.InsertString(2, "����");
	m_cboMainCmp.SetCurSel(0);

	//���ư�Ƚ�����
	m_CustmonCmp.InsertString(0, "����");
	m_CustmonCmp.InsertString(1, "С��");
	m_CustmonCmp.InsertString(2, "����");
	m_CustmonCmp.SetCurSel(0);

	m_strCurType = "0";
	g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("��ʼ�����Ͱ�װ������ģ��"));
}

//���ð汾�űȽ�����
void CCFilterPost::SetCompareType(const string& strCmp,CComboBox* cbo)
{
	if (strCmp == "����")
	{
		cbo->SetCurSel(0);
	}
	else if (strCmp == "С��")
	{
		cbo->SetCurSel(1);
	}
	else
	{
		cbo->SetCurSel(2);
	}

	g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("δ�ҵ���Ӧ�ıȽ�����:%s"),strCmp.c_str());
}

//��дֵ����Ӧ�Ŀ��
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
		g_log.Trace(LOGL_LOW, LOGT_ERROR, __TFILE__, __LINE__, _T("������δȷ���� %s"),strName.c_str());
	}
}

//��ȡ���Ժ�ֵ
bool CCFilterPost::GetXmlAttr(xml_node* node)
{
	if (node == NULL)
	{
		g_log.Trace(LOGL_LOW, LOGT_PROMPT, __TFILE__, __LINE__, _T("���յ���nodeΪNULL"));
		return false;
	}

	string strName, strValue;
	const acl::xml_attr* xmlAtt;

	xmlAtt = node->first_attr();
	while (xmlAtt != NULL)
	{
		strName = xmlAtt->get_name();
		strValue = xmlAtt->get_value();
		//ת��Сд
		strName = strName.lower();
		SetValutToCtl(strName, strValue);
		//��ȡ��һ������
		xmlAtt = node->next_attr();
	}

	return true;
}
