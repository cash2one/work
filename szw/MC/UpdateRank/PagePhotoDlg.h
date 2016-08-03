#pragma once
#include "webbrowser2.h"
// CPagePhotoDlg 对话框
#include <mshtml.h>   
#include <atlbase.h>   
#include <oleacc.h> 

class CPagePhotoDlg : public CDialog
{
	DECLARE_DYNAMIC(CPagePhotoDlg)

public:
	CPagePhotoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPagePhotoDlg();

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
	std::vector<BackData*>  vectBackData;



public:
	/*
	@brief  取得百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalBaiduHeight( IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得手机百度条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	add by zhoulin
	*/
	std::vector<long> CalPhoneBaiduHeight( IHTMLElementCollection *pTables, const SearchData &sData, CString &strFileName, BOOL &bResult);


	/*
	@brief  取得谷歌条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalGoogleHeight(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得360条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> Cal360Height(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult);


	/*
	@brief  取得搜搜条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalSoSoHeight(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult);

	/*
	@brief  取得搜狗条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalSoGouHeight(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult);



	/*
	@brief  取得必应条目高度，及反馈数据
	@param  过滤标签内容
	@param  sData  关键词相关数据
	@param  strFileName  要保存的图片路径   输出
	@return 返回符合条件的高度
	*/
	std::vector<long> CalBingHeight(IHTMLElementCollection *pLis, const SearchData &sData, CString &strFileName, BOOL &bResult);

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
	static void GetMainDomain(CString &strUrl);

	/*
	@brief  请求url 判断里面的html源码是否包含公司名
	@param  strUrl  网址
	@param  strCompay  公司名
	@return 0没有， 1有，  2出现异常打不开
	*/
	static int HasCompanyInHtml(const CString &strUrl, const SearchData &sData);

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
	static DWORD WINAPI GetPagePhoto(LPVOID lp);

	/*
	@brief  等待web控件打开页面
	@param  sData  
	@return 
	*/
	BOOL WaitURlAndSaveImageFile(const SearchData &sData);

	
	/*
	@brief  判断百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInBaidu(CString strHtml);

	/*
	@brief  判断手机百度搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	add by zhoulin
	*/
	BOOL  FindIsOwnWebInPhoneBaidu(CString strHtml);


	/*
	@brief  判断谷歌搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInGoogle(CString strHtml);

	/*
	@brief  判断360搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebIn360(CString strHtml);

	/*
	@brief  判断Bing搜索的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInBing(CString strHtml);


	/*
	@brief  判断搜搜的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInSoSo(CString strHtml);

	/*
	@brief  判断搜狗的网址是否在   收录的网站
	@param  strHtml  某个条目的html代码
	@return TRUE表存在
	*/
	BOOL  FindIsOwnWebInSoGou(CString strHtml);


	/*
	@brief  判断抓取数据是否包含3个搜索引擎，缺少则补上
	@param  pData数据
	*/
	void AlterData(std::vector<BackData*> *pData);

	/*
	@brief  把抓取的数据返回给上层
	*/
	void BackMsg();


	/*
	@brief 将html 上传到ftp服务器
	@param [in/out]html路径
	*/
	void SaveHtmlToServer(CString &strFilePath);


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
	*/
	void SaveToHtml(IHTMLElementCollection *pHtml, const CString &strFileName);


	void ChangeButtonStyle(IHTMLElementCollection *pHtml, const SearchData *pSearchData);      //改变搜索按钮页面跳转
	void SetInternetFeature(void);          //屏蔽web控件网页跳转声音


	/*
	@brief 当前页面没抓取到快照时，判断该页是否无法打还是出现验证码等情况
	@param pHtml  html对象
	@param pSearchData   搜索关键词信息
	@return 0表示正常页面  1表示无法打开页面，页面为空或者无法显示此页，  2表示出现验证码情况  
	*/
	int  JudgeHtml(IHTMLElementCollection *pHtml, const SearchData *pSearchData);    

};
