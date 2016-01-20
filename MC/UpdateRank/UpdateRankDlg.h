#pragma once
#include "webbrowser2.h"
// CUpdateRankDlg �Ի���
#include <mshtml.h>   
#include <atlbase.h>   
#include <oleacc.h> 
#include <string>
//#include "DataInfo.h"

class CUpdateRankDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateRankDlg)

public:
	CUpdateRankDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateRankDlg();

// �Ի�������
	enum { IDD = IDD_WEB_DLG };

protected:

	HANDLE hThread;
	CWebBrowser2 m_WebBrowser;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_EVENTSINK_MAP()
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg LRESULT  GetSearchData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  RedirectURL(WPARAM wParam, LPARAM lParam);


private:
	std::vector<pBackDataInfo> m_vBackData;

private:
	/*
	Add By ����
	@brief  �Ƴ�Ҫ����HTML�������script��ǩ
	@param  string strHTMLData HTML����
	@return void
	*/
	void RemoveAllScriptTag(string &strHTMLData);

	/*
	Add By ����
	@brief  ��HTML��style��ǩ�����滻��css����
	@param  string strHTMLData HTML����
	@param  char* strData Ҫ�滻������
	@return void
	*/
	void ReplaceStyleToCss(string &strHTMLData, const char* strData);

	/*
	Add By ����
	@brief  �ж�HTML�Ƿ���Ҫ����
	@param  int iFlag ������־
	@return bool
	*/
	bool IsNeedChange(int iFlag);

	/*
	Add By ����
	@brief  ����������־��ȡstyle��ǩ��Ҫ�滻������
	@param  int iFlag ������־
	@param  string& szCss �滻���ݡ������
	@return void
	*/
	void ChangeData(int iFlag, string& szCss);

public:
	/*
	@brief  ȡ�ðٶ���Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	//std::vector<long> CalBaiduHeight( IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int  GetBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ȡ���ֻ��ٶ���Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	add by zhoulin
	*/
	//std::vector<long> CalPhoneBaiduHeight( IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetPhoneBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ȡ���ֻ��ѹ���Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	add by zhoulin
	*/
	//std::vector<long> CalPhoneSougouHeight(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetPhoneSougouPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ȡ�ùȸ���Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	std::vector<long> CalGoogleHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ȡ��360��Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	//std::vector<long> Cal360Height(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int Get360Photo(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ץȡ�е���������
	*/
	//std::vector<long> CalYouDaoHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetYouDaoPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  360�İ�󷵻ص�URL����360����ҳ�淵�ص�URL�����ҵ�������URL
	@param  strURL  360����ҳ��õ���URL
	@return ����������URL(URL�����)
	*/
	CString GetRealURL(CString strURL);


	/*
	@brief  ȡ��������Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	std::vector<long> CalSoSoHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ȡ���ѹ���Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	//std::vector<long> CalSoGouHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetSoGouPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);



	/*
	@brief  ȡ�ñ�Ӧ��Ŀ�߶ȣ�����������
	@param  ���˱�ǩ����
	@param  sData  �ؼ����������
	@param  strFileName  Ҫ�����ͼƬ·��   ���
	@return ���ط��������ĸ߶�
	*/
	//std::vector<long> CalBingHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetBingPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  ����bmpͼƬ
	@param  hBitmap  bmpͼƬ����
	@param  lpszFileName  �ļ�·��
	@return ����ɹ�����true
	*/
	BOOL SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);

	/*
	@brief  ����urlȡ��
	@param  [in/out]strUrl  ��ַ
	*/
	static BOOL GetMainDomain(CString &strUrl,const std::vector<CString> &vHostlist);

	/*
	@brief  ����url �ж������htmlԴ���Ƿ������˾��
	@param  strUrl  ��ַ
	@param  strCompay  ��˾��
	@return 0û�У� 1�У�  2�����쳣�򲻿�
	*/
	static int HasCompanyInHtml(CString &strUrl, const KeyWordDataInfo &sData, BOOL &bOwnOfficial);

	/*
	@brief  ����ҳ����ת��Ϊָ���ı����ʽ������
	@param  pszRawData  ���յ�����ҳԴ����
	@param  strUrl ��ǰ�������ַ
	@param  strContentType �����������͵��ַ���
	@param  strRetData ָ������������
	@return 
	*/
	static int GetPageData(CStringA strRawData, CString& strUrl, CString &strContentType, CString &strRetData);

	/*
	@brief  ����ҳ����ת��Ϊָ���ı����ʽ������
	@param  pszRawData  Ҫת��������
	@param  dwRawDataLen ���ݳ���
	@param  codePage ת������
	@return ָ������������
	*/
	static CString ToWideCharString(LPSTR pszRawData, DWORD codePage);

	/*
	@brief  ���ݰٶ���ĿhtmlԴ��ȡ�ö�Ӧ����url
	@param  strPage  Դ��
	*/
	static CString GetUrlFormContent(const CString &strPage);

	/*
	@brief  ���ݹؼ���ȡ��ͼƬ·��
	@param  strTitle  �ؼ���
	@param  strFile  �ļ�·��  ���
	@param  strComp  ��˾��
	@param  iFlag  ������־
	@return ����ɹ�����true
	*/
	static BOOL GetImageFilePath(const CString &strTitle, const CString &strComp, CString &strFile, int iFlag);

	/*
	@brief  ����ҳ�淢����������Ѱ�Ұٶȹȸ���������   �߳�
	@param  lp  thisָ��
	@return 
	*/
	static DWORD WINAPI GetUpdateRank(LPVOID lp);

	/*
	@brief  �ȴ�web�ؼ���ҳ��
	@param  sData  
	@return 
	*/
	BOOL WaitURlAndSaveImageFile(const KeyWordDataInfo &sData);

	
	/*
	@brief  �жϰٶ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@param  sData    �����ṹ
	@return TRUE�����
	*/
	BOOL  FindIsOwnWebInBaidu(CString strHtml, const KeyWordDataInfo &sData,BOOL& bOwnOfficial,DWORD& dwWebId);

	/*
	@brief  �ж��ֻ��ٶ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	add by zhoulin
	*/
	BOOL  FindIsOwnWebInPhoneBaidu(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  �ж��ֻ��ѹ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	add by zhoulin
	*/
	BOOL  FindIsOwnWebInPhoneSougou(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);


	/*
	@brief  �жϹȸ���������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
	BOOL  FindIsOwnWebInGoogle(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  �ж�360��������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
	BOOL  FindIsOwnWebIn360(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  �ж�Bing��������ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
	BOOL  FindIsOwnWebInBing(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);


	/*
	@brief  �ж��ѹ�����ַ�Ƿ���   ��¼����վ
	@param  strHtml  ĳ����Ŀ��html����
	@return TRUE�����
	*/
	BOOL  FindIsOwnWebInSoGou(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);



	/*
	@brief  ��ץȡ�����ݷ��ظ��ϲ�
	*/
	void BackMsg();


	/*
	@brief ����������־�õ�������������
	@param iFlag  ������־
	@return ������������
	*/
	CString GetSearchFlag(int iFlag);


	/*
	@brief ��html���浽һ���ļ�
	@param pHtml  html����
	@param strFileName  html�ļ�·��
	@param iFlag ���������־
	Add By ����
	*/
	// void SaveToHtml(IHTMLElementCollection *pHtml, const CString &strFileName);
	void SaveToHtml(IHTMLElementCollection *pHtml, const CString &strFileName, int iFlag);

	// �е����⴦��
	void SaveToHtmlYouDao(IHTMLElementCollection *pHtml, const CString &strFileName);

	void ChangeButtonStyle(IHTMLElementCollection *pHtml, const KeyWordDataInfo *pSearchData);      //�ı�������ťҳ����ת
	void SetInternetFeature(void);          //����web�ؼ���ҳ��ת����


	/*
	@brief ��ǰҳ��ûץȡ������ʱ���жϸ�ҳ�Ƿ��޷����ǳ�����֤������
	@param pHtml  html����
	@param pSearchData   �����ؼ�����Ϣ
	@return 0��ʾ����ҳ��  1��ʾ�޷���ҳ�棬ҳ��Ϊ�ջ����޷���ʾ��ҳ��  2��ʾ������֤�����  
	*/
	int  JudgeHtml(IHTMLElementCollection *pHtml, const KeyWordDataInfo *pSearchData);    

	/*
	@brief ��ȡ��ǰurl��Դ�ļ���
	@param url ��ַ
	@return Դ�ı�
	*/
	static CStringA urlopen(CString url);

	/*
	@brief ��ȡǶ����ַ��Դ�ļ��������Ƿ�����ڹ�����
	@param strData c���������ݣ�iFlag ����������
	@return TRUE �����ڹ�����
	*/
	static BOOL GetEmbedUrlData(CStringA& strData,int iFlag);

};
