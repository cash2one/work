#pragma once
#include "MyRegEx.h"

#ifndef QI_ELEMENT_COLLECTION
#define QI_ELEMENT_COLLECTION std::vector<CComQIPtr<IHTMLElement>>
#endif

class CHtmElementLocate
{
public:
	CHtmElementLocate(void);
	~CHtmElementLocate(void);

	//设置HTML文档对象
	void SetHtmlDocument(IHTMLDocument2 *pDoc);

	//返回指定点对应的HTML元素
	IHTMLElement *GetElement(CPoint pt);

	//查找第一个有strTag属性的元素，比如a,body等属性,bExist指明要找的类型是包含strtag还是不包含
	IHTMLElement *GetFirstTagElement(CString strTag, bool bExist);

	//返回指定HTML元素的定位(精确定位/模糊定位)信息
	//由调用方释放T_ITEM内存
	T_ITEM *GetLocateInfo(CComQIPtr<IHTMLElement> pElement, bool bVagueLocate, int &iTotalFound);

	//返回当前填充的父节点删除功能的节点
	T_ITEM *GetItemTreeFromElement(CComQIPtr<IHTMLElement> pElemTopParent, CComQIPtr<IHTMLElement> pElementCur/*, int nLevel*/);

	//从父节点填充所有的节点树
	void FillParentTree(T_ITEM *pItem, CComQIPtr<IHTMLElement> pElem);
	T_ITEM * GetNodesWantedAttributes(CComQIPtr<IHTMLElement> pElement, T_ITEM *pItemInfo);

// 	//返回指定点对应的HTML元素定位(精确定位/模糊定位)信息
// 	//由调用方释放T_ITEM内存
// 	T_ITEM *GetLocateInfo(CPoint pt, bool bVagueLocate);
	
	//根据定位信息返回一个元素对象(精确定位)
	int m_nLevel;//定位父节点的层级
	void SetLevel(int nLevel);
	int GetLevel();
	bool FromLocateInfo(T_ITEM *pItem, CComQIPtr<IHTMLElement> &pElem, int &iFound);

	//根据定位信息返回满足条件的元素对象集合(模糊定位)
	bool FromLocateInfo(T_ITEM *pItem, QI_ELEMENT_COLLECTION &vResElem, int &iFound, vector<CStdString> & m_sFindCanDelete);
	bool FromLocateInfo(T_ITEM *pItem, QI_ELEMENT_COLLECTION &vResElem, int &iFound);
	//删除特定节点功能
	void DelTargetAllNodes(T_ITEM *tarItem);

	//根据定位信息返回满足条件的元素对象,定位时优先尝试用局部属性，然后再使用全部属性
	bool FromLocateInfoWithSimptify(T_ITEM *pItem, CComQIPtr<IHTMLElement> &pElem, T_ITEM &UsedAttr);

	//判断指定元素是否符合T_ITEM中所有的属性特征
	bool ItemFeatureFit(IHTMLElement *pElem, T_ITEM &tarItem);
	bool ItemFeatureFit(IHTMLElement *pElem, T_ITEM &tarItem, vector<CStdString> &sFindCanDelete);

	//所有父节点是否都符合过滤条件,一般情况下没有父节点删除功能，直接过的
	bool IsParentNodesFit(IHTMLElement *pElem, T_ITEM &tarItem);
	bool IsLevelComfit(T_ITEM* constructTreeItem, T_ITEM *TargetTree);
	T_ITEM* ConstructParentTree(IHTMLElement *pElem);//根据当前节点和层级，构造出一棵树，显示所有子节点和信息
	T_ITEM *FindLastItemFormItemTree(T_ITEM *pItemTree);//找出Item树的最后一个叶子节点
	bool CompareSourceAttrIsRight(T_ITEM *pSource, T_ITEM *pDest);//比较目标Item中是否包含所有源Item的属性
	bool IsTreeHaveOnlyOneNodePath(T_ITEM *pItemTree);//树是否只含有一条路径

	//在网页源码中查找字符串（遍历Frame）
	int FindTextInHtmlSrc(CComQIPtr<IHTMLDocument2> pDoc,CStdString strText, CStdString strExAppendText, CStdString &strErrInfo);

	//在指定文本中按与或非条件查找字符串
	int FindTextInString(CStdString strSrcString,CStdString strText, CStdString &strErrInfo);

	//简化定位元素的属性
	bool Simptify(T_ITEM *pItem, int &iTotalFound, bool bAuto = true);

	//获取元素的指定属性
	CStdString GetAttributeValue(CComQIPtr<IHTMLElement> pElem, CStdString strAttrName);

	//获取元素的所有可用属性（包括自定义的如前后文属性）
	void GetAllUsedAttributes(CComQIPtr<IHTMLElement> pElem, T_ITEM *&item);

	//获取元素的HTML源码
	CStdString GetOuterHTML(CComQIPtr<IHTMLElement> pElem);

	//获取网页HTML源码
	CStdString GetPageHTML(CComQIPtr<IHTMLDocument2> pDoc) ;
//	CStdString GetPageHTML(MSHTML::IHTMLDocument2Ptr pDoc) ;

	//返回标签名称
	CStdString GetTagName(CComQIPtr<IHTMLElement> pElem);
	CStdString GetInnerText(CComQIPtr<IHTMLElement> pElem);
	CStdString GetOuterText(CComQIPtr<IHTMLElement> pElem);

	CStdString GetInnerHTML(CComQIPtr<IHTMLElement> pElem);
	CStdString GetOutHTML(CComQIPtr<IHTMLElement> pElem);

	void GetSelectElementValue(CComQIPtr<IHTMLElement> pElement, CStdString &strValue);
	void GetSelectElementText(CComQIPtr<IHTMLElement> pElement, CStdString &strText);

	LONG GetOffsetHeight(CComQIPtr<IHTMLElement> pElem);
	LONG GetOffsetWidth(CComQIPtr<IHTMLElement> pElem);
	void SetVisiblity(CComQIPtr<IHTMLElement> pElem, bool bShow);
	CStdString GetVisiblity(CComQIPtr<IHTMLElement> pElem);

	CStdString GetStyle(CComQIPtr<IHTMLElement> pElem);

	CStdString GetInputElementType(CComQIPtr<IHTMLInputElement> pElem);

	long GetSelectElementLen(CComQIPtr<IHTMLSelectElement> pElem);
	CStdString GetOptionElementVal(IHTMLOptionElement *pElem);
	CStdString GetOptionElementText(IHTMLOptionElement *pElem);

	//获取BODY的innertext(包括frame的当前状态)
	void GetBodyText(CComQIPtr<IHTMLDocument2> , CStdString &strText, bool bIsGetHideElement=true);
	//去除网页中隐藏元素的outtext
	void RemoveHideOuttext(CComQIPtr<IHTMLDocument2> ); 
	//恢复去除网页中隐藏元素的outtext
	void AddHideOuttext(CComQIPtr<IHTMLDocument2> ); 
	CStdString RemoveX$XText(CStdString &);

	//模拟触发事件
	HRESULT FireEvent(CComQIPtr<IHTMLElement> pElem, _bstr_t bstrEventName);

	//获取单通配符多值定位时匹配成功的值
	void GetMatchedLocValue(vector<CStdString> &vMatchedVal);

// 	//尝试从IFrame得到其Document指针
// 	HRESULT GetFrameDocument2(CComQIPtr<IHTMLElement> pFrameElem, IHTMLDocument2 *&pDoc2);

	//获取相对于Frame的元素位置
	HRESULT GetPostionRelativeToFrame(CComQIPtr<IHTMLDocument2> pParentDoc, CComQIPtr<IHTMLElement> pElem, RECT &rt);

	//检查元素是否存在于Frame中
	bool IsElementInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem);

	//add by zhumingxing 20150807----返回frame，iframe元素--目前处理一层的情况 
	bool IsElementInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, CComQIPtr<IHTMLElement>& pElemt);

	//将输入字符串转换为与或非结构
	void TransStucture(CStdString strInput, T_FILTER_STRUCTURE &tResult);

private:
	IHTMLDocument2 *m_pHtmlDocument;              //当前操作的Document
	IHTMLDocument2 *m_pHtmlDocBackup;             //初始操作的Document的备份
	vector<CStdString> m_sMatchedLocValue;      //单通配符多值(格式：<#>Val1<#>Val2<#>...<#>)匹配时，用于存放匹配成功的值
	CMyRegEx m_regEx;    //正则表达式类
	std::vector<T_ITEM *> m_vecLevelItem;      //每个层级都有自己的一个节点与之对应 

	//取得元素用于定位的属性
	T_ITEM *GetWantedAttributes(CComQIPtr<IHTMLElement> pElement);

	//取得元素的所有属性
	void GetAllAttributes(CComQIPtr<IHTMLElement> pElement, std::map<_bstr_t, _bstr_t>&mapAttribs);

	//根据指定属性定位元素，返回定位到的元素个数
	bool SeekTargetElement(T_ITEM *pItem, int &iFoundCount, CComQIPtr<IHTMLElement> &qiElem, bool bReChekHide = false);

	//在document/frame中查找元素
	LONG SeekTargetInDocument(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl, vector<CStdString> &sFindCanDelete);
	LONG SeekTargetInDocument(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl); 
	LONG SeekTargetInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl, _bstr_t bstrFrameTag);

	//add by zhumingxing 20150807 处理object元素坐标处理问题
	LONG SeekTargetInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl, _bstr_t bstrFrameTag, CComQIPtr<IHTMLElement>& pElemt);

	//删除特定节点功能
	void DelNodeInDocument(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM *tarItem);
	void DelNodeInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM *tarItem, _bstr_t bstrFrameTag);//frame,iframe中删除节点

	//在指定父节点下查找元素
	bool SeekElementByParent(CComQIPtr<IHTMLElement> pParent, T_ITEM *pTargetItem, CComQIPtr<IHTMLElement> &qiTargetElem);

	// Converts a IHTMLWindow2 object to a IHTMLDocument2. Returns NULL in case of failure.
	// It takes into account accessing the DOM across frames loaded from different domains.
	IHTMLDocument2 *HtmlWindowToHtmlDocument(CComQIPtr<IHTMLWindow2> spWindow);

	// Converts a IHTMLWindow2 object to a IWebBrowser2. Returns NULL in case of failure.
	CComQIPtr<IWebBrowser2> HtmlWindowToHtmlWebBrowser(CComQIPtr<IHTMLWindow2> spWindow);

	//是否需要排除此属性
	bool IsValidAttributeName(CComBSTR strAtribName);
	bool IsValidValue(CComBSTR strVal, CComBSTR strAtribName);

	//去掉值为空的属性
	void RemoveInvalidProperty(T_ITEM &item);

	//转换IE不支持的属性名
	void TransAttributeNameForIE(CComBSTR &strAttribName); 

	CStdString TrimDataBeforeSave(CStdString strData);

	//提取紧邻的前文后者后文
	void GetAdjacentText(CComQIPtr<IHTMLElement> pElement, bool bFindPerFix, CStdString &strText);

	CStdString GetPartTextByEnd(CStdString strHtml, CStdString strEnd, int iPos);

	//在父节点下查找指定结点的位置
	LONG GetElemPosInParentContainer(CComQIPtr<IHTMLElement> pParent, CComQIPtr<IHTMLElement> pElem, LONG &lngTotalChild, CComQIPtr<IHTMLElementCollection> &QiChildColl);

	//获取子节点信息，返回子节点个数及其集合
	LONG GetChildCollection(CComQIPtr<IHTMLElement> pElem, IHTMLElementCollection **coll);

	//使用给定的子结点替换列表中的父节点
	bool ReplaceParentElemWithChild(QI_ELEMENT_COLLECTION &elems, CComQIPtr<IHTMLElement> qiElem);

	//检查找到的多个元素是为同一元素（如阿里巴巴登陆页面的两个iframe指向同一页面，此时会找到两个相同元素）
	bool MutliElemsIsTheSame(QI_ELEMENT_COLLECTION &elems);


	//获取前/后/本身元素的文本属性
	void GetInnerText(CComQIPtr<IHTMLElement> pElem, bool bFindPerFix, CStdString &strText);

	//提取元素的text相关属性作为前后文使用
	void GetTextForAdjentText(CComQIPtr<IHTMLElement> pElem, bool bFindPerFix, CStdString &strText);

	//根据属性名返回指定元素的固有属性
	CStdString GetProperyByName(CComQIPtr<IHTMLElement> pElem, _bstr_t bstrName);

	//用指定属性（集）定位对象，若定位成功则将这些属性作为对象的定位属性
	bool SimptifyByAttributes(T_ITEM &tmpItem, int &iFound, T_ITEM *pRowItem);

	//从frame的IHTMLElement指针获取IHTMLDocument2，并将pt转换为相对于框架的坐标
	HRESULT GetFrameDocument(CComQIPtr<IHTMLElement> pFrameElem, IHTMLDocument2 *&pDoc2, CPoint &pt);

	//尝试从IFrame得到其Document指针
	HRESULT GetFrameDocument2(CComQIPtr<IHTMLElement> pFrameElem, IHTMLDocument2 *&pDoc2);

	//将输入字符串转换为与或非结构
	//void TransStucture(CStdString strInput, T_FILTER_STRUCTURE &tResult);

	//在frame中查找字符串
	bool GetFrameBodyText(CComQIPtr<IHTMLDocument2> pDoc, _bstr_t bstrFrameTag, CStdString &strHtmlContent);
};

