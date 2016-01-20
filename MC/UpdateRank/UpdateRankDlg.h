#pragma once
#include "webbrowser2.h"
// CUpdateRankDlg 对话框
#include <mshtml.h>   
#include <atlbase.h>   
#include <oleacc.h> 
#include <string>
//#include "DataInfo.h"

class CUpdateRankDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateRankDlg)

public:
	CUpdateRankDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUpdateRankDlg();

// 对话框数据
	enum { IDD = IDD_WEB_DLG };

protected:

	HANDLE hThread;
	CWebBrowser2 m_WebBrowser;
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
	Add By 周林
	@brief  移除要保存HTML里的所有script标签
	@param  string strHTMLData HTML内容
	@return void
	*/
	void RemoveAllScriptTag(string &strHTMLData);

	/*
	Add By 周林
	@brief  把HTML里style标签内容替换成css引用
	@param  string strHTMLData HTML内容
	@param  char* strData 要替换的内容
	@return void
	*/
	void ReplaceStyleToCss(string &strHTMLData, const char* strData);

	/*
	Add By 周林
	@brief  判断HTML是否需要更改
	@param  int iFlag 搜索标志
	@return bool
	*/
	bool IsNeedChange(int iFlag);

	/*
	Add By 周林
	@brief  根据搜索标志获取style标签需要替换的内容
	@param  int iFlag 搜索标志
	@param  string& szCss 替换内容【输出】
	@return void
	*/
	void ChangeData(int iFlag, string& szCss);

public:
	/*
	@brief  取得百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	//std::vector<long> CalBaiduHeight( IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int  GetBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得手机百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	add by zhoulin
	*/
	//std::vector<long> CalPhoneBaiduHeight( IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetPhoneBaiduPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得手机搜狗条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	add by zhoulin
	*/
	//std::vector<long> CalPhoneSougouHeight(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetPhoneSougouPhoto(IHTMLElementCollection *pTables, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得谷歌条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalGoogleHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得360条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	//std::vector<long> Cal360Height(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int Get360Photo(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  抓取有道搜索快照
	*/
	//std::vector<long> CalYouDaoHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetYouDaoPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  360改版后返回的URL不对360搜索页面返回的URL处理找到真正的URL
	@param  strURL  360搜索页面得到的URL
	@return 返回真正的URL(URL解码后)
	*/
	CString GetRealURL(CString strURL);


	/*
	@brief  取得搜搜条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalSoSoHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得搜狗条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	//std::vector<long> CalSoGouHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetSoGouPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);



	/*
	@brief  取得必应条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	//std::vector<long> CalBingHeight(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);
	int GetBingPhoto(IHTMLElementCollection *pLis, const KeyWordDataInfo &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  保存bmp图片
	@param  hBitmap  bmp图片数据
	@param  lpszFileName  文件路径
	@return 保存成功返回true
	*/
	BOOL SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);

	/*
	@brief  根据url取得
	@param  [in/out]strUrl  网址
	*/
	static BOOL GetMainDomain(CString &strUrl,const std::vector<CString> &vHostlist);

	/*
	@brief  请求url 判断里面的html源码是否包含公司名
	@param  strUrl  网址
	@param  strCompay  公司名
	@return 0没有， 1有，  2出现异常打不开
	*/
	static int HasCompanyInHtml(CString &strUrl, const KeyWordDataInfo &sData, BOOL &bOwnOfficial);

	/*
	@brief  将网页数据转化为指定的编码格式并返回
	@param  pszRawData  接收到的网页源数据
	@param  strUrl 当前载入的网址
	@param  strContentType 包含编码类型的字符串
	@param  strRetData 指定编码后的数据
	@return 
	*/
	static int GetPageData(CStringA strRawData, CString& strUrl, CString &strContentType, CString &strRetData);

	/*
	@brief  将网页数据转化为指定的编码格式并返回
	@param  pszRawData  要转换的数据
	@param  dwRawDataLen 数据长度
	@param  codePage 转换代码
	@return 指定编码后的数据
	*/
	static CString ToWideCharString(LPSTR pszRawData, DWORD codePage);

	/*
	@brief  根据百度条目html源码取得对应链接url
	@param  strPage  源码
	*/
	static CString GetUrlFormContent(const CString &strPage);

	/*
	@brief  根据关键词取得图片路径
	@param  strTitle  关键词
	@param  strFile  文件路径  输出
	@param  strComp  公司名
	@param  iFlag  搜索标志
	@return 保存成功返回true
	*/
	static BOOL GetImageFilePath(const CString &strTitle, const CString &strComp, CString &strFile, int iFlag);

	/*
	@brief  根据页面发过来的数据寻找百度谷歌搜索数据   线程
	@param  lp  this指针
	@return 
	*/
	static DWORD WINAPI GetUpdateRank(LPVOID lp);

	/*
	@brief  等待web控件打开页面
	@param  sData  
	@return 
	*/
	BOOL WaitURlAndSaveImageFile(const KeyWordDataInfo &sData);

	
	/*
	@brief  判断百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@param  sData    搜索结构
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInBaidu(CString strHtml, const KeyWordDataInfo &sData,BOOL& bOwnOfficial,DWORD& dwWebId);

	/*
	@brief  判断手机百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	add by zhoulin
	*/
	BOOL  FindIsOwnWebInPhoneBaidu(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  判断手机搜狗搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	add by zhoulin
	*/
	BOOL  FindIsOwnWebInPhoneSougou(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);


	/*
	@brief  判断谷歌搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInGoogle(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  判断360搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebIn360(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);

	/*
	@brief  判断Bing搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInBing(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);


	/*
	@brief  判断搜狗的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInSoGou(CString strHtml, const KeyWordDataInfo &sData, BOOL& bOwnOfficia, DWORD& dwWebId);



	/*
	@brief  把抓取的数据返回给上层
	*/
	void BackMsg();


	/*
	@brief 根据搜索标志得到搜索引擎名称
	@param iFlag  搜索标志
	@return 搜索引擎名称
	*/
	CString GetSearchFlag(int iFlag);


	/*
	@brief 将html保存到一个文件
	@param pHtml  html对象
	@param strFileName  html文件路径
	@param iFlag 搜索引擎标志
	Add By 周林
	*/
	// void SaveToHtml(IHTMLElementCollection *pHtml, const CString &strFileName);
	void SaveToHtml(IHTMLElementCollection *pHtml, const CString &strFileName, int iFlag);

	// 有道特殊处理
	void SaveToHtmlYouDao(IHTMLElementCollection *pHtml, const CString &strFileName);

	void ChangeButtonStyle(IHTMLElementCollection *pHtml, const KeyWordDataInfo *pSearchData);      //改变搜索按钮页面跳转
	void SetInternetFeature(void);          //屏蔽web控件网页跳转声音


	/*
	@brief 当前页面没抓取到快照时，判断该页是否无法打还是出现验证码等情况
	@param pHtml  html对象
	@param pSearchData   搜索关键词信息
	@return 0表示正常页面  1表示无法打开页面，页面为空或者无法显示此页，  2表示出现验证码情况  
	*/
	int  JudgeHtml(IHTMLElementCollection *pHtml, const KeyWordDataInfo *pSearchData);    

	/*
	@brief 获取当前url的源文件；
	@param url 地址
	@return 源文本
	*/
	static CStringA urlopen(CString url);

	/*
	@brief 获取嵌入网址的源文件，返回是否包含在官网中
	@param strData c待检测的数据，iFlag 搜索引擎标记
	@return TRUE 包含在官网中
	*/
	static BOOL GetEmbedUrlData(CStringA& strData,int iFlag);

};
