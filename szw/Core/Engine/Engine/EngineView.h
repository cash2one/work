
// Sumengine2View.h : CSumengine2View 类的接口
//

#pragma once

#include "StepData.h"
#include "MyPopupMenu.h"
#include "EngineDoc.h"
#include "LhpHtmlView.h"
//#include "CShockwaveFlash.h"


#ifndef QI_ELEMENT_COLLECTION
#define QI_ELEMENT_COLLECTION std::vector<CComQIPtr<IHTMLElement>>
#endif

#define UTF_8 0 
#define GB2312 1

typedef struct _T_POPUP_HISTORY_INFO
{
	CStdString strLastAll;            //截止到上次步骤运行时，所有的弹框信息
	CStdString strStepLastUse;        //上次步骤运行时对应的新鲜弹框信息
}T_POPUP_HISTORY_INFO;

class CHtmElementLocate;

class CSumengine2View : public CLhpHtmlView/*CHtmlView*/
{

protected: // 仅从序列化创建
	CSumengine2View();
	DECLARE_DYNCREATE(CSumengine2View)
	
	//获取URL，不再从childFrm获取，从而可以release计数
	void GetViewURL(_bstr_t &bstr);

	//不带对象定位信息的步骤数据（如人工输入）
	void GetStepData(T_MENU_SELECTED &param);

	//返回包含对象定位信息的步骤数据
	//int m_nLevel;//树的实际层级
	bool GetStepDataWithElemInfo(T_MENU_SELECTED &param, int &iTotalFound);

	//从获取的Item中填充step信息
	void FillStepDataFromItem(T_ITEM *pItem, CComQIPtr<IHTMLElement> QiElem, T_MENU_SELECTED &param, CHtmElementLocate &elemLoc, bool bVagueLoc, int nLevel);

	//显示处于坐标位置的对象的HTML代码
	void ShowHTMLbyCursorPos(CPoint pt);

	//释放元素的属性结构
	void FreeItem(T_ITEM *&pItem);

	//取得用于控制步骤循环的参数
	void GetStepLoopParams(int &iStart, int &iEnd, int index);

	//获取目标操作对象的集合
	bool GetTargetElemCollection(int &iTotalElems);
	
	//获取当前要操作的目标对象
	E_RESULT GetTargetElement(CComQIPtr<IHTMLElement> &pElem);

	//提取元素所有的可用属性供成功标记进行比对选择或作为提取数据的依据
	void GetElemAllAttributes(CHtmElementLocate &locate, IHTMLElement *pElem, std::vector<T_STEP_RETURN> &vAttr);

	//分发界面消息，避免界面假死
	void DispUIMessage(void);

	//清除上步操作的相关数据
	void ClearLastStepResult(void);

	//获取对象本身的描述（不含操作描述）
	CStdString GetPureObjectDescription();

	//延时且不阻塞UI消息
	void WaitWithUIMsgDisp(int nWait);

	//发送键盘消息
	bool SendKeys(CStdString strKeys);

	BYTE GetVKeyCode(CStdString strVKey);

	//赋值操作 wFillMethod = 0表示普通复制，wFillMethod = 1表示模拟键盘事件赋值
	bool FillElement(CComQIPtr<IHTMLElement> pElem, CStdString strSymbol,int iFillMethod = 0);
	
	//获取select元素的值
	CStdString GetSelectObjVal(CComQIPtr<IHTMLSelectElement> pSelect, int nIndex);
	CStdString GetSelectObjText(CComQIPtr<IHTMLSelectElement> pSelect, int nIndex);

	//从字符串中分离出动参及其对应的属性列表
	void FetchMapOfIIDKs(CStdString strSymbol, std::map<CStdString, CStdString>&mapIIDK);

	//从动参-属性列表中取出与指定属性对应的动参名称
	bool GetIIDK(std::map<CStdString, CStdString> &mapIIDK, CStdString strAttr, CStdString &strIIDK);
	
	bool GetImageCode(CStdString strURL, /*CStdString strIndex, */CStdString strPath);
	bool ChkImageValid(const CStdString &strImage);

	
	bool StretchImage(const CStdString &strImage, int iMaxWidth, int iMaxHeight);

	bool StretchWeinXinImage(const CStdString &strImage);

	//执行数据/动参提取，根据传入的参数不同可提取对象属性、网页弹框等信息
	bool DoFetchIIDK(CComQIPtr<IHTMLElement> pElem, CStdString strAttr, CStdString strRule, std::vector<CStdString> &vRes);

	//获取来自当前网页的所有弹框消息
	void GetPopupText(std::vector<CStdString> &vText);

	//返回一个以任务为ID为前缀的唯一ID字符串（用作验证码文件名等）
	CStdString GetUniqueIdBaseOnTask();   

	//返回URL相对于当前网站的绝对路径
	CStdString GetAbsoluteURL(CStdString strRelative);

	//返回一个唯一的验证码文件名路径
	CStdString GetFileNameForVerifyCode();   

	//获取识别参数
	void GetIdentifyParam(CStdString strSymbol, E_VERIFY_TYPE &vtype, CStdString &strParam, 
						CStdString &strURL, CStdString &strPromptInfo, CStdString &strRegOrP,CStdString& strShowCodeHeight);

	//获取页面上的验证码图片并保存到指定文件
	bool GetOriginVerifyCodeFromPage(CStdString strPath/*, bool bCopy*/);

	//获取验证码的URL
	bool GetVerifyCodeImgSrc(CStdString &strURL);

	//获取文字类型验证码的Outtext
	bool GetVerifyCodeOuttext(CStdString &strText);

	//从剪贴板获取图片并保存到指定文件
	bool SaveClipboardPicture(CStdString strPath);

	//获取成功标记步骤尚未检查的弹框信息
	CStdString GetFreshPopupMsg(CStdString strStepID);

	//检查页面状态，超时次数小于3且页面状态为BUSY时返回true,否则返回false
	bool ChkPageLoadState(BOOL &bIsBusy, READYSTATE &rState);

	//检查点击操作后页面状态
	bool ChkClickPageState(BOOL &bIsBusy, READYSTATE &rState);

	//释放步骤拥有的输入焦点互斥信号，允许验证码窗口弹出
	void ReleaseInputFocusMutex();

	//获取验证码弹框返回结果  by song
	void GetCodeResult() ;
	void GetOrSetCursorPos(bool bGet = true);

	//对元素操作的再次封装（记录或提示操作失败信息）
	bool ElemClick(CComQIPtr<IHTMLElement> &qiElem);	
	bool ElemFocus(CComQIPtr<IHTMLElement2> &qiElem);
	bool ElemBlur(CComQIPtr<IHTMLElement2> &qiElem);
	bool FireEvent(CComQIPtr<IHTMLElement> &qiElem, _bstr_t bstrEventName);

	void DoAction(void);
	void ActionClick(void);
	void ActionDelNode();
	void ActionFill(void);
	//bCopy是不是截图出来的验证码,bFirst表示是不是第一次获取验证码
	void ActionIdentify(bool bFirst, CString strPath = _T(""), bool bCopy = false);

	//自定义表单操作(八方自定义APIPost接口提交)
	void ActionPost(void);

	//图片/微信验证码下载识别
	BOOL HandlePictureCode(BOOL bFirst, BOOL bCopy, CStdString strImgPath, CStdString strParam, 
								CString strUniqueID, CString& strPath, CStdString strInputPrompt);

	//问题型验证码处理
	BOOL HandleQuestionCode(BOOL bFirst, BOOL bCopy, CStdString strImgPath, CStdString& strParam,
		CString strUniqueID, CString& strPath, CStdString strInputPrompt);

	//处理flash验证码-----以中国建材网注册验证码为例
	BOOL HandleFlashCode(CString strUniqueID, CString& strPath);

	//evtTelephone/evtTelephoneCode/evtEMail/evtEMailCode类型验证码处理
	BOOL HandleUserInputCode(CStdString strParam, CStdString& strAutoOcrResult,CString strUniqueID, CString& strPath, CStdString strInputPrompt);

	//evtDragCode点击拖动类型验证码处理
	BOOL HandleDragCode();

	void ActionJumpURL(CStdString strURL);
	void ActionKeyPress(CStdString strKeys);
	void ActionUploadFile(CStdString strSymbol);
	bool OpenObjectUploadCtrl(CComQIPtr<IHTMLElement> qiElem);
	void ActionShowWindow(bool bShow);
	//判断是不是只有outtext且内容只有NOT里面有值的成功标记，并且集合中不止一个对象
	bool IsSingleOuttextSuccItem();
	void ActionSuccFeature();

	//匹配保留成功标记,点击/拖动验证码
	bool FitSaveSuccFeature();

	void ActionFireEvent();
	void ActionGetAttribute();
	void ActionGetOnceKeyMove();
	void ActionGetIIDKs();
	void ActionMarkObject();
	//有2种情况的节点需要保存：验证码图片刷新，删除节点
	void ActionSaveItem(bool bDelete = false);
	//保存预成功标记，供点击/拖动型验证码使用
	void ActionSavePreSuccFlag();

	//处理像11-9这种算术验证码的情况
	bool VerifyCodeHandleQuestion(CStdString &strParam, bool bFirst, bool bCopy);

	//处理图片验证码的情况
	bool VerifyCodeHandlePicture(CStdString strParam, bool bCopy, bool bFirst);

	//将当前step object显示出来，便于做点击操作
	bool GetAndShowCurrentElement();        

	//对元素做mouse_event点击
	void ClickElement(CComQIPtr<IHTMLElement> qiElem,BOOL bIsDoubleClick = FALSE);  

	//设置当前窗口是否显示的标记
	void SetShowWindowFlag(bool bVal);     

	//获取窗口是否显示的标记
	bool GetShowWindowFlag(void);

	//清理所有的弹框消息，避免下次模块再进入时老的弹框消息还在干扰结果
	void ClearPopupUIMsg();				

	//add by zhumingxing 20150714 处理久久信息网文件选择框选择的问题---后续有类型的网站可根据网站ID来处理
	void SelectPicFromWeb();

	//切换焦点到内核进行，以便之后还原焦点到上一个焦点窗口
	void ChangeFocusToEngine(HWND hForeWnd);

	//对保存的元素进行点击，这样可以更换网页上需要点击的验证码
	bool ClickSaveElement();

	//等待抢占焦点标记被激活
	bool WaitEnterFlag();

	//满足条件激活步骤进入标记
	void ReleaseEnterFlag();

	//根据指定元素隐藏其他无关元素,并返回对应的宽和高
	bool HideobjectAndGetPos(CComQIPtr<IHTMLElement>&qiElem, long* dwWidth, long* dwHigth, CStdString& strShowCodeHeight);

	//处理点击/拖动类型验证码方法封装(正常模式、采编模式)
	bool HandleDragCode(CStdString& strInputTitle,CStdString& strShowCodeHeight);

	//处理点击/拖动类型验证码(游览模式)
	bool HandleDragCodeForBrowser();

	//点击/拖动类型验证码识别结果处理
	bool HandleDragResult();

public:
	CStepData m_objBeforeStep;              //保留上一个步骤供点击刷新验证码
	CStepData *m_pBrforeStep;               //指向保留上一个步骤   
	CStepData m_objDelItemStep;             //删除的节点步骤
	CStepData *m_pDelItemStep;				
	CStepData m_objSaveSuccStep;			//保留预成功标记，点击拖动型验证码使用
	CStepData *m_pSaveSuccStep;				//预成功标记指针
	CStepData m_objCurrentStep;				//保留本次步骤(拖动验证码检测成功标记时会覆盖)
	CStepData* m_pCurrentStep;				//保留步骤指针
	int m_iShowDragCodeTime;				//显示拖动型验证码时间,默认60s超时
	CString m_strShowTitle;					//拖动点击型验证码显示标题
	CString m_strApiPhotoFileName;			//完善相册八方返回的图片名字，用来替换发布的imageflag字段通配符
protected:
	bool m_bIsGetSyncEnterStep;				//当前View是否拥有步骤同步标记
	CStdString m_strOldImg;
	CComQIPtr<IHTMLElement> m_targetElement;
	unsigned char* m_pBits;                 //比较内存内容，看看2个验证码是否相等
	CStdString m_strOldVerifyCode;          //如果验证码是字符串的形式，比较旧的问题形式的验证码是否一致
	CStdString m_strOldImgPath;				//保留旧的验证码路径

	CStdString m_strCutImg;                //截图路径
	CStdString m_strTaskID;				   //任务ID
	CStdString m_strNsName;                //网站名称
	static CMyPopupMenu m_sPopupMenu;
	CLogTrace m_log;                        //存放本网站操作日志
	HANDLE m_hNotifySignal;                 //步骤完成后的事件通知句柄
	CStepData m_objStepData;
	int m_iObjIndexForVagueLoc;             //模糊定位时目标操作对象的序号
	CStdString m_strCurStepID;              //当前步骤的ID
	CStdString m_strLastStepID;				//保留上一次步骤信息ID
	bool m_bIsStepRetry;				    //是否在进行步骤重试(主要用在成功标记重试，获取弹框信息上)
	bool m_bIsSameStepShowMessage;			//是否为同一个步骤弹框

	T_UPLOAD_FILE_PARAM m_tUploadFileParam;
	bool m_bOwnInputFocus;                  //是否拥有输入焦点互斥信号
	bool m_bOwnSystemInput;				//是否拥有系统键盘互斥信号

	//以下几个用于控制步骤循环的执行
	E_STEP_PHASE m_stepPhase;               //步骤当前所处的阶段
	int m_nStepLoopStartIndex;
	int m_nStepLoopEndIndex;

	IHTMLElement *m_pLastHoverElem;        //上次鼠标悬停的对象
	CStdString m_strLastHoverElemHTML;     //上次鼠标悬停对象的HTML代码
	bool m_bPageFinishLoad;                //页面加载完成标记
    bool m_bNewPageOpen;                   //有新页面打开
	bool m_bStop;                          //停止页面操作，相当于关闭页面标记
	int m_nClickStatCheckedTimes;          //点击操作后，已检查页面状态的次数，完成后清0
	int m_nTimeoutRetryedTimes;            //超时后已重试次数

	CString m_strUIMsg;                    //网站弹出的提示信息（alert/prompt提示的消息内容）

	std::map<CStdString, CStdString> *m_pMapIIDKs;      //指向本任务的动参表
	std::map<CStdString, T_POPUP_HISTORY_INFO> m_mapStepChkHistory;    //成功标记步骤检查弹框信息的历史记录

	CStdString m_strRetImgCode;          //用户确认的验证码

	E_RESULT m_resVal;                   //返回码
	CStdString m_strErrDescription;      //错误描述
	CStdString m_strOutData;             //返回的提取数据
	CStdString m_strQuickLookText;       //页面快照
	CStdString m_strStepHTML;            //步骤的HTML

	CString m_strOldURL;                 //跳转网页前保存旧的网址，以便超时时检测是否网页有变更，作为跳转成功的判断
	E_VERIFY_TYPE m_eVerifyCodeType;    //验证码类型
	CStdString m_strImgCodePath;        //验证码路径
	T_CodeData_IN m_ocrData;                //发送给验证码显示框的数据结构
	CStdString    m_strCurRegOrP;		   //当前使用的是R0，P0还是其他，每次识别都会初始化
	vector<CStdString> m_vecOcrRegister;	//R0，R1，。。。。。。。。
	vector<CStdString> m_vecOcrPublish;		//P0,P1,.............
	int    m_nVagueObjFound;			   //找到的模糊定位对象的个数
	DWORD m_dwCreationTime;                //View的创建时间
	CSumengine2View *m_pRelatedParentView;  //产生本视图的视图指针
	DWORD m_dwAmbientProperty;              //控制视图行为的标记
	bool m_flagObjectUpload;                //此标记为true表示内核窗口可能正显示，步骤失败时需隐藏
public:
	LONG m_RecognizeResult;                 //人工打码的返回值
	bool m_bIsTakeMoney;                   //上一个步骤是不是识别验证码，如果是识别验证吗的话，某些结果需要保留，因为人工打码扣钱就在这里
	int  m_nCurRetryTime;                  //当前这个步骤重试了多少次
	bool m_bAutoReg;								//当前验证码识别步骤是否执行过自动识别
	bool m_bLongRange;								//当前验证码识别步骤是否执行过远程打码
	bool m_bManmade;							    //当前验证码识别步骤是否执行过人工打码
	E_ORC_Indentify_Type m_eCodeOIT;                //当前步骤使用的真实打码方式
	QI_ELEMENT_COLLECTION m_objQiElemCollection;    //目标操作元素的集合
	vector<CStdString> m_sFindCanDelete;			//针对...<#>1003<#>1005<#>1008<#>...这种多个要删除的情况，如果能找到要删除的元素，就放在这个集合里
	HANDLE m_hNavigateEvent;						//临时拷贝视图成功加载的事件
	bool m_bIsTempView;								//判断视图类型是否为拷贝的临时视图

	//block执行前，初始化上面3个变量为false
	void InitCodeStyleFlag();

	//获取自动识别标志true
	bool GetAutoReg();   

	//获取远程打码标志true
	bool GetLongRange(); 

	//获取人工输入标志true
	bool GetManmade();   

	//获取当前步骤使用的打码方式
	E_ORC_Indentify_Type GetCodeOIT();

	//获取验证码类型
	E_VERIFY_TYPE GetVerifyCodeType();

	//根据strRP是R0或者P0返回对应的m_vecOcrRegister，m_vecOcrPublish中的值
	CStdString GetParamFromRP(CStdString &strRP);

	//主控传过来的验证码参数填充m_vecOcrRegister，m_vecOcrPublish，每个View创建之初就设置，整个推广完成都不会改变
	void SetOcrParamFillRP(CStdString strParamOcr);

	void SetIsTempView(bool bIsTempView){ m_bIsTempView=bIsTempView ;} ;
	void CreateNavigateEvent() ;
private:
	void SetStepResult(E_RESULT eRes);
	void SetQuickLook();
	//获取当前页面的大小
	int GetPageRect(RECTL &rect);
	void SetInternetFeature(void);
	void ForceFrameWndToCurrent(CFrameWnd *pFrameWnd);

	//切换Frame到前端时的互斥锁
	static CLock m_sLockFrameWnd;    

public:
	void SetSetpOutData(CStdString strAttr, CStdString strValue);

	//检查Object元素是否被包含在Frame中
	HRESULT CheckObjInFrame(CComQIPtr<IHTMLElement> qiObjElem, bool &bEmbedInFrame, CComQIPtr<IHTMLElement>& pFrame);

// 特性
public:
	CSumengine2Doc* GetDocument() const;

// 操作
public:
	//初始化执行模块（创建日志...）
	bool Init(CStdString strTaskID, CStdString strNsName, DWORD dwAmbientProperty);

	CStdString GetTaskID();                      //获取与View关联的任务ID
	void SetTaskID(CStdString strTaskID);        //设置与View关联的任务ID
	CStdString GetViewTitle(void);               //获取视图标题
	CStdString GetViewRelatedURL(void);          //获取与视图关联的URL

	void SetNsName(CStdString strNsName);
	CStdString GetNsName();
	void SetIIDKs(std::map<CStdString, CStdString> *pMapIIDKs);
	//设置产生本视图的视图指针
	void SetParentView(CSumengine2View *pView); 

	//设置步骤结果
	void StepFinished(E_RESULT eRes/*, CStdString strErrDesc = _T("成功")*/);

	//返回步骤执行结果的接口
	CStdString GetCutImg();
	E_RESULT GetStepResult();
	void SetStepErrDesc(CStdString strDesc);
	CStdString GetStepErrDesc();
	CStdString GetUiMessage();
	CStdString GetQuickLookText();

	//生成页面快照图片并返回图片路径
	CStdString GenQuickLookPic(BOOL bFlag);   
	//返回提取数据
	CStdString GetStepOutData();   
	//返回模糊定位找到的对象个数
	int GetVagueObjFound();      
	//动参替换
	void ReplaceIIDKs(CStdString &strText);      

	//获取操作对象的源码 用于循环结束条件的比较
	CStdString GetStepObjHTML(void);       

	DWORD GetCreationTime(void);
	//下载验证码图片
	bool DownloadOcrPic(CStdString strURL, /*CStdString strIndex, */CStdString strImg);
	//保存下载的验证码路径
	void SetOcrPicPath(const CStdString &strImgPath);    
	static DWORD WINAPI ThreadProcDownloadPic(LPVOID lpParameter);
	//处理验证码图片返回
	static DWORD WINAPI ThreadPicResult(LPVOID lpParameter);
	//自动识别验证码，用模板库
	void autoIndentifyORC(T_CodeData_IN &orcData, const CStdString &strImgPath);
	//把问题验证码转换成图片
	bool TextToBmp(const CString &strBmpPath, const CString &strText);
	void saveBmpFile(HBITMAP bmp, const CString &strBmpPath);
	//自动打码
	void HandleAutoVertify(const CStdString &strImgPath);
	//远程打码
	void HandleLongRangeVertify(const CStdString &strImgPath);
	//人工打码
	void HandleManMadeVertify();

	//创建临时视图，以便获取页面文本
	int FindInBodyTxt(CStdString strOuttext, CStdString strPopupMsg, CStdString &strErrInfo) ;
	HRESULT LoadFromStream(CString strPageHTML,  CSumengine2View *pCopyView, _bstr_t &bstrCharset) ;

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	// 构造后第一次调用
	virtual void OnInitialUpdate(); 
	virtual HRESULT OnShowMessage(HWND hwnd,LPOLESTR lpstrText,LPOLESTR lpstrCaption,DWORD dwType,LPOLESTR lpstrHelpFile,DWORD dwHelpContext,LRESULT * plResult);

// 实现
public:
	virtual ~CSumengine2View();
	void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	int m_nCount;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUserCmdEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopTask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetJumpURL(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowURL(WPARAM wParam, LPARAM lParam);
	//鼠标消息触发的对象激活事件
	afx_msg LRESULT OnEventActiveElement(WPARAM wParam, LPARAM lParam);     

	//处理工具栏上的按钮消息
	afx_msg LRESULT OnMsgGoBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgGoForward(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgRefresh(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecieveStepData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDocViewInit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewActive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSubViewFinishedLoading(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFreeViewResource(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgQuickLookPic(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgQuickLookText(WPARAM wParam, LPARAM lParam);

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//屏蔽图片等操作函数重载
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
};

#ifndef _DEBUG  // Sumengine2View.cpp 中的调试版本
inline CSumengine2Doc* CSumengine2View::GetDocument() const
   { return reinterpret_cast<CSumengine2Doc*>(m_pDocument); }
#endif

