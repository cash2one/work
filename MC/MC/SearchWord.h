#pragma once
class CSearchWord
{
public:
	CSearchWord();
	~CSearchWord();

	//�ӿڣ�����ؼ���
	void ProcessKeyWord(const CStdString &strWord);
private:
	//���������ؼ�������
	BOOL ParseSearchKeyWordData(const CStdString &strData);
	//��URL��ȡԴ
	CString urlopen(CString url);

	//��ȡ������Ϣ
	BOOL ReadConfig();

	//���ֽ�ת���ֽ�
	char* CStringToMutilChar(CString& str, int& chLength, WORD wPage);
	//URL����
	CString  UrlEncode(CString sIn);

	//���ֽ�ת���ֽ�
	CString MulitToStringT(LPSTR str);

	//��ȡ��ҳ������
	BOOL GetWebData(const CString &strData,const CString &strKey);

	//����Ƿ������ǵ���ó��վ��
	BOOL CheckWeb(const CString &strData);

	//��ȡ������ó��վ
	void GetDefaultWeb();

	//��ȡ����
	void GetMainDomain(CString &strUrl);

	/*
	@brief  ���ַ�ת���ɶ��ֽ�
	@param  pChar  ���ַ�
	@return ���ض��ֽ�
	*/
	char* WideToChar(const wchar_t *pWide, DWORD dwCode = CP_ACP);

	BYTE toHex(const BYTE &x);

private:
	std::vector<CStdString> m_vStrKeyWord; //�ؼ�������

	int m_dwPrivilege;	//��Ȩֵ
	int m_dwStandard;		//��׼ֵ
	int m_dwDifftime;     //�����ʵ�ʱ���

	CStringA g_strAllWeb;

};

