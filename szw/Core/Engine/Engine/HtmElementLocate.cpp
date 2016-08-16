#include "StdAfx.h"
#include "HtmElementLocate.h"
CLogTrace g_htmlLocate(_T("Core\\locate.log"), NULL);  //保存定位元素的日志
#include "FeatureFit.h"

using namespace FeatureFit;

CHtmElementLocate::CHtmElementLocate(void)
{
	m_pHtmlDocument = NULL;
	m_pHtmlDocBackup = NULL;
	m_sMatchedLocValue.clear();
	m_vecLevelItem.clear();
	m_nLevel = 0;
}


CHtmElementLocate::~CHtmElementLocate(void)
{
#ifdef xiaoqiang 
	if(m_pHtmlDocBackup != m_pHtmlDocument)
	{
		if(m_pHtmlDocBackup != NULL)
		{
			m_pHtmlDocBackup->Release();
			m_pHtmlDocBackup = NULL;
		}
	}
#endif

	if (NULL != m_pHtmlDocument)
	{
		m_pHtmlDocument->Release();
		m_pHtmlDocument = NULL;
	}
}

//设置HTML文档对象
void CHtmElementLocate::SetHtmlDocument(IHTMLDocument2 *pDoc)
{
#ifdef xiaoqiang 
	if(m_pHtmlDocBackup != m_pHtmlDocument)
	{
		if(m_pHtmlDocBackup != NULL)
		{
			m_pHtmlDocBackup->Release();
			m_pHtmlDocBackup = NULL;
		}
	}
#endif

	if (NULL != m_pHtmlDocument)
	{
		m_pHtmlDocument->Release();
		m_pHtmlDocument = NULL;
	}

	m_pHtmlDocument = pDoc;
	m_pHtmlDocBackup = m_pHtmlDocument;
}

//查找第一个有strTag属性的元素，比如a,body等属性
IHTMLElement *CHtmElementLocate::GetFirstTagElement(CString strTag, bool bExist)
{
	IHTMLElement *pElement = NULL;
	CComQIPtr<IHTMLElementCollection> coll;
	if (NULL == m_pHtmlDocument)
	{
		return NULL;
	}

	CComQIPtr<IHTMLDocument3> pDoc3 = m_pHtmlDocument;
	LONG lngCount = 0;
	HRESULT hr = 0;
	LPCTSTR lpStr = (LPCTSTR)strTag.GetBuffer();
	if(bExist)//查找存在这个strTag标记的
	{
		pDoc3->getElementsByTagName(_bstr_t(lpStr), &coll);
		coll->get_length(&lngCount);
		for (LONG i = 0; i < lngCount; i++)
		{
			_variant_t vIndex = i;
			CComDispatchDriver spElem;
			hr = coll->item(vIndex, vIndex, &spElem);
			if (FAILED(hr) || NULL == spElem)
			{
				continue;
			}
			else
			{
				spElem.QueryInterface(&pElement);
				return pElement;
			}
		}
	}
	else
	{
		m_pHtmlDocument->get_all(&coll);
		coll->get_length(&lngCount);
		for (LONG i = 0; i < lngCount; i++)
		{
			_variant_t vIndex = i;
			CComDispatchDriver spElem;
			hr = coll->item(vIndex, vIndex, &spElem);
			if (FAILED(hr) || NULL == spElem)
			{
				continue;
			}
			else
			{
				spElem.QueryInterface(&pElement);
				_bstr_t bstrVal;
				pElement->get_tagName(&bstrVal.GetBSTR());
				CString strTmp = (LPCTSTR)bstrVal;
				if(strTmp == strTag)
				{
					continue;
				}
				else
				{
					return pElement;
				}
			}
		}
	}
	return NULL;
}

//返回指定点对应的HTML元素
IHTMLElement *CHtmElementLocate::GetElement(CPoint pt)
{
	IHTMLElement *pElement = NULL;

	if (NULL == m_pHtmlDocument)
		return NULL;

	m_pHtmlDocument->elementFromPoint(pt.x, pt.y, &pElement);

	if (!pElement)
		return NULL;

	//如果元素是框架，则将点转为相对于框架的坐标并取出框架中的该元素
	CStdString strTagName = GetTagName(pElement);
	if (!strTagName.CompareNoCase(_T("frame")) || !strTagName.CompareNoCase(_T("iframe")))
	{
		HRESULT hr = GetFrameDocument(pElement, m_pHtmlDocument, pt);
		if (SUCCEEDED(hr))
		{
			pElement = GetElement(pt);
		}
	}

	return pElement;
}

////返回指定点对应的HTML元素定位(精确定位/模糊定位)信息
////由调用方释放T_ITEM内存
//T_ITEM *CHtmElementLocate::GetLocateInfo(CPoint pt, bool bVagueLocate)
//{
//	T_ITEM *pRetItem = NULL;
//	CComQIPtr<IHTMLElement> pElem;
//	pElem = GetElement(pt);
//	if (NULL != pElem)
//	{
//		pRetItem = GetLocateInfo(pElem, bVagueLocate);
//	}
//
//	return pRetItem;
//}

//从父节点填充所有的节点树
void CHtmElementLocate::FillParentTree(T_ITEM *pItem, CComQIPtr<IHTMLElement> pElem)
{
	if (NULL == pElem)
	{
		return;
	}

	CStdString strText = GetOuterHTML(pElem);

	//首先获取父节点的属性
	//pTopParentItem = GetWantedAttributes(pElemTopParent);//获取当前节点的所有属性
	//GetNodesWantedAttributes(pElemTopParent, pTopParentItem);//获取当前节点的所有属性

	//获取父节点的所有孩子
	HRESULT hr = 0;
	CComDispatchDriver spDirver;
	CComQIPtr<IHTMLElementCollection> QiChildColl;
	LONG lngTotalChild = 0;
	hr = pElem->get_children(&spDirver);
	if (FAILED(hr) || NULL == spDirver)
	{
		return;
	}
	hr = spDirver->QueryInterface(IID_IHTMLElementCollection, (void**)&QiChildColl);
	QiChildColl->get_length(&lngTotalChild);
	for (LONG i = 0; i < lngTotalChild; i++)
	{
		_variant_t vIndex = i;
		//_variant_t vIndex1 = 0;
		CComDispatchDriver spElem;
		hr = QiChildColl->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
		{
			continue;
		}
		CComQIPtr<IHTMLElement> QiSubElem;
		spElem.QueryInterface(&QiSubElem);

		if(NULL == QiSubElem)
		{
			continue;
		}
		CStdString strText1 = GetOuterHTML(QiSubElem);

		//T_ITEM *pCurParentItem = NULL;
		T_ITEM *pItemI = new T_ITEM;
		GetNodesWantedAttributes(QiSubElem, pItemI);//获取当前节点的所有属性
		pItem->m_mapNodesAttr[i] = pItemI;
		FillParentTree(pItemI, QiSubElem);
	}
}

//返回当前填充的父节点删除功能的节点
T_ITEM *CHtmElementLocate::GetItemTreeFromElement(CComQIPtr<IHTMLElement> pTopParentElem, CComQIPtr<IHTMLElement> pElementCur/*, int nLevel*/)
{
	if (NULL == pTopParentElem)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("父节点为无效的IHTMLElement指针，无法定位！"));
		return NULL;
	}
	if (NULL == pElementCur)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("当前节点为无效的IHTMLElement指针，无法定位！"));
		return NULL;
	}
	/*if (0 == nLevel)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("层级不能为0！"));
		return NULL;
	}*/

	T_ITEM *pItemInfo = new T_ITEM;
	//GetWantedAttributes(pElementCur, pItemInfo);//获取当前节点的所有属性
	GetNodesWantedAttributes(pElementCur, pItemInfo);//获取当前节点的所有属性
	
	//获取当前节点父节点树信息的填充
	//T_ITEM *pTopParentItem = NULL;
	T_ITEM *pTopParentItem = new T_ITEM;
	GetNodesWantedAttributes(pTopParentElem, pTopParentItem);//获取当前节点的所有属性
	pItemInfo->m_mapNodesAttr[0] = pTopParentItem;//最上层父节点有且只有一个
	FillParentTree(pTopParentItem, pTopParentElem);
	//pItemInfo->m_mapNodesAttr[0] = pTopParentItem;//最上层父节点有且只有一个
	return pItemInfo;
}

//返回指定HTML元素的定位(精确定位/模糊定位)信息
//由调用方释放内存
T_ITEM *CHtmElementLocate::GetLocateInfo(CComQIPtr<IHTMLElement> pElement, bool bVagueLocate, int &iTotalFound)
{
	if (NULL == pElement)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetLocateInfo收到无效的IHTMLElement指针，无法定位！"));
	    return NULL;
	}

	T_ITEM *pItemInfo = GetWantedAttributes(pElement);

	//模糊定位时直接返回，不再进一步处理
	if (bVagueLocate || NULL == pItemInfo)
	{
		return pItemInfo;
	}

	//通过坐标点取元素时可能会深入到iframe中，从面改变当前操作的Document
	//所以，定位元素前需将当前操作Document设置回原始的Doument，以保证采数据时和正常执行时的环境一致
	m_pHtmlDocument = m_pHtmlDocBackup;

	//检查是否可以精确定位对象,不可以的话递归处理父节点
	//此时，如果简化失败则不通过父结点定位且此结点的有效属性都保留
	if (!Simptify(pItemInfo, iTotalFound))
	{
		return NULL;

// 		CComQIPtr<IHTMLElement, &IID_IHTMLElement> parentElem;
// 		pElement->get_parentElement(&parentElem);
// 
// 		if (parentElem)
// 			pItemInfo->pParent = GetLocateInfo(parentElem, bVagueLocate);
	}

	RemoveInvalidProperty(*pItemInfo);

	return pItemInfo;
}

void CHtmElementLocate::GetSelectElementValue(CComQIPtr<IHTMLElement> pElement, CStdString &strValue)
{
	BSTR bstr;
	CComQIPtr<IHTMLSelectElement> QiSelect;
	HRESULT hr = pElement.QueryInterface(&QiSelect);
	if(SUCCEEDED(hr))
	{
		bstr = SysAllocString(L"");  
		QiSelect->get_value(&bstr);
		strValue = bstr;
	}
}

void CHtmElementLocate::GetSelectElementText(CComQIPtr<IHTMLElement> pElement, CStdString &strText)
{
	BSTR bstr;
	CComQIPtr<IHTMLSelectElement> QiSelect;
	HRESULT hr = pElement.QueryInterface(&QiSelect);
	if(SUCCEEDED(hr))
	{
		IHTMLOptionElement *pOptElem;
		IDispatch *ppvdispOption;
		long number = -1;
		QiSelect->get_selectedIndex(&number);
		if(number >= 0)
		{
			_variant_t index = number;
			hr = QiSelect->item(index, index, &ppvdispOption);
			if(SUCCEEDED(hr))
			{
				hr = ppvdispOption->QueryInterface(IID_IHTMLOptionElement, (void **)&pOptElem);
				ppvdispOption->Release();
				if(SUCCEEDED(hr)) 
				{
					// Add the option text to a list box.
					hr = pOptElem->get_text(&bstr);
					pOptElem->Release();
					if(SUCCEEDED(hr))
					{
						strText = bstr;
					}
				}
			}
		}
	}
}

//节点专用的，pItemInfo已经申请空间，照抄GetWantedAttributes的
T_ITEM * CHtmElementLocate::GetNodesWantedAttributes(CComQIPtr<IHTMLElement> pElement, T_ITEM *pItemInfo)
{
	if (NULL == pItemInfo)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetLocateInfo 申请T_ITEM失败！"));
		return NULL;
	}

	CStdString strTmpValue = _T("");
	pItemInfo->strTagName = GetTagName(pElement);
	strTmpValue = GetOuterText(pElement);
	if (strTmpValue.GetLength() > 0)
	{
		if (0 != pItemInfo->strTagName.CompareNoCase(_T("select")))
		{
			strTmpValue= TrimDataBeforeSave(strTmpValue);
			if (strTmpValue.GetLength() > 0)
				pItemInfo->mapAttributes[_T("outtext")] = strTmpValue;
		}
		else//对提取select元素的所有selected.text,selected.value的提取
		{
			CStdString strValue = _T("");
			CStdString strText  = _T("");
			GetSelectElementValue(pElement, strValue);
			GetSelectElementText(pElement, strText);
			if(_T("") != strValue)
			{
				pItemInfo->mapAttributes[_T("SelectValue")] = strValue;
			}
			if(_T("") != strText)
			{
				pItemInfo->mapAttributes[_T("SelectText")] = strText;
			}
		}
	}

	GetAdjacentText(pElement, true, strTmpValue);
	if (strTmpValue.GetLength() > 0)
    	pItemInfo->mapAttributes[_T("pertext")] = TrimDataBeforeSave(strTmpValue);  //对影响比较的特殊数据进行转换

	GetAdjacentText(pElement, false, strTmpValue);
	if (strTmpValue.GetLength() > 0)
    	pItemInfo->mapAttributes[_T("surtext")] = TrimDataBeforeSave(strTmpValue);  //对影响比较的特殊数据进行转换

	//获取元素的所有有效属性
	std::map<_bstr_t, _bstr_t> mapAttribs;
	GetAllAttributes(pElement, mapAttribs);

	//对元素的所有属性进行过滤，提取定位元素用的属性
	//现在不对元素进行过滤，提取所有元素
	std::map<_bstr_t, _bstr_t>::iterator itAttr = mapAttribs.begin();
	for (; itAttr != mapAttribs.end(); ++itAttr)
	{
		CStdString strName = (TCHAR*)itAttr->first;
		CStdString strVale = (TCHAR*)itAttr->second;
		if(strVale != _T(""))
		{
			pItemInfo->mapAttributes[strName] = strVale;
		}
	}

	//兼容不规范的style写法，像百度注册 直接以不同style的属性作为显示邮箱提示信息的方法
	//且只写了style，没有为其指定值，此时在做成功标记时要借助style属性
	if (!pItemInfo->AttributeExist(_T("style")))
	{
		pItemInfo->mapAttributes[_T("style")] = GetVisiblity(pElement);
	}
	
	//input类型有可能是radio，如果是，就要添加选项checked表示是否选中,先写上
	if((0 == pItemInfo->strTagName.CompareNoCase(_T("input"))) && (pItemInfo->mapAttributes[_T("type")] == _T("radio")))
	{
		CComQIPtr<IHTMLInputElement> QiInputRadio;
		HRESULT hr = pElement.QueryInterface(&QiInputRadio);
		if(SUCCEEDED(hr))
		{
			VARIANT_BOOL bCheck;
			QiInputRadio->get_checked(&bCheck);
			if (bCheck == VARIANT_TRUE)
			{
				pItemInfo->mapAttributes[_T("_RadioCheck")] = _T("1");
			}
			else
			{
				pItemInfo->mapAttributes[_T("_RadioCheck")] = _T("0");
			}
		}
	}

	return pItemInfo;
}

//取得元素用于定位的属性
T_ITEM *CHtmElementLocate::GetWantedAttributes(CComQIPtr<IHTMLElement> pElement)
{
	T_ITEM *pItemInfo = new T_ITEM;
	if (NULL == pItemInfo)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetLocateInfo 申请T_ITEM失败！"));
		return NULL;
	}

	CStdString strTmpValue = _T("");
	pItemInfo->strTagName = GetTagName(pElement);
	strTmpValue = GetOuterText(pElement);
	if (strTmpValue.GetLength() > 0)
	{
		if (0 != pItemInfo->strTagName.CompareNoCase(_T("select")))
		{
			strTmpValue= TrimDataBeforeSave(strTmpValue);
			if (strTmpValue.GetLength() > 0)
				pItemInfo->mapAttributes[_T("outtext")] = strTmpValue;
		}
		else//对提取select元素的所有selected.text,selected.value的提取
		{
			CStdString strValue = _T("");
			CStdString strText  = _T("");
			GetSelectElementValue(pElement, strValue);
			GetSelectElementText(pElement, strText);
			if(_T("") != strValue)
			{
				pItemInfo->mapAttributes[_T("SelectValue")] = strValue;
			}
			if(_T("") != strText)
			{
				pItemInfo->mapAttributes[_T("SelectText")] = strText;
			}
		}
	}

	GetAdjacentText(pElement, true, strTmpValue);
	if (strTmpValue.GetLength() > 0)
    	pItemInfo->mapAttributes[_T("pertext")] = TrimDataBeforeSave(strTmpValue);  //对影响比较的特殊数据进行转换

	GetAdjacentText(pElement, false, strTmpValue);
	if (strTmpValue.GetLength() > 0)
    	pItemInfo->mapAttributes[_T("surtext")] = TrimDataBeforeSave(strTmpValue);  //对影响比较的特殊数据进行转换

	//获取元素的所有有效属性
	std::map<_bstr_t, _bstr_t> mapAttribs;
	GetAllAttributes(pElement, mapAttribs);

	//对元素的所有属性进行过滤，提取定位元素用的属性
	//现在不对元素进行过滤，提取所有元素
	std::map<_bstr_t, _bstr_t>::iterator itAttr = mapAttribs.begin();
	for (; itAttr != mapAttribs.end(); ++itAttr)
	{
		CStdString strName = (TCHAR*)itAttr->first;
		CStdString strVale = (TCHAR*)itAttr->second;
		if(strVale != _T(""))
		{
			pItemInfo->mapAttributes[strName] = strVale;
		}
		//以下对所有值进行筛选，只保留定义了属性的值，现在改成只要有值就留下来
		/*//先判断Name
		bool bNeedAdd = false;
		if (IsValidAttributeName((TCHAR*)itAttr->first))
			bNeedAdd = true;
		else
			bNeedAdd = false;

		if (bNeedAdd)
		{
			CComBSTR bstrName((TCHAR*)itAttr->first);
			TransAttributeNameForIE(bstrName);
			if (IsValidValue((TCHAR*)itAttr->second, bstrName))
				bNeedAdd = true;
			else
				bNeedAdd = false;
		}

		if (bNeedAdd)
		{
			CStdString strName = (TCHAR*)itAttr->first;
			CStdString strVale = TrimDataBeforeSave((TCHAR*)itAttr->second);
			pItemInfo->mapAttributes[strName] = strVale;
		}*/
	}

	//兼容不规范的style写法，像百度注册 直接以不同style的属性作为显示邮箱提示信息的方法
	//且只写了style，没有为其指定值，此时在做成功标记时要借助style属性
	if (!pItemInfo->AttributeExist(_T("style")))
	{
		pItemInfo->mapAttributes[_T("style")] = GetVisiblity(pElement);
	}
	
	//input类型有可能是radio，如果是，就要添加选项checked表示是否选中,先写上
	if((0 == pItemInfo->strTagName.CompareNoCase(_T("input"))) && (pItemInfo->mapAttributes[_T("type")] == _T("radio")))
	{
		CComQIPtr<IHTMLInputElement> QiInputRadio;
		HRESULT hr = pElement.QueryInterface(&QiInputRadio);
		if(SUCCEEDED(hr))
		{
			VARIANT_BOOL bCheck;
			QiInputRadio->get_checked(&bCheck);
			if (bCheck == VARIANT_TRUE)
			{
				pItemInfo->mapAttributes[_T("_RadioCheck")] = _T("1");
			}
			else
			{
				pItemInfo->mapAttributes[_T("_RadioCheck")] = _T("0");
			}
		}
	}

	return pItemInfo;
}

//取得元素的所有属性
void CHtmElementLocate::GetAllAttributes(CComQIPtr<IHTMLElement> pElement, std::map<_bstr_t, _bstr_t>&mapAttribs)
{
	CComDispatchDriver pACDisp;
	CComQIPtr<IHTMLAttributeCollection> pAttrColl;
	CComQIPtr<IHTMLDOMNode> pElemDN;
	CComQIPtr<IHTMLInputElement> qiInputBtnElem;
	CComQIPtr<IHTMLStyle> pStyle ;

	LONG lACLength = 0;
	VARIANT vACIndex;
	VARIANT vValue;
	VARIANT_BOOL vbSpecified;
	HRESULT hr = 0;
	CString strElemMsg = _T("");

	/*参见MSDN中IHTMLAttributeCollection
	Use the following procedure to get an IHTMLAttributeCollection interface pointer for an element. 

	1.	Call QueryInterface on the element interface to request an IHTMLDOMNode interface.
	2.	Call IHTMLDOMNode::attributes on the IHTMLDOMNode interface to get an IDispatch interface pointer for the element's IHTMLAttributeCollection interface.
	3.	Call QueryInterface on the IDispatch interface to request an IHTMLAttributeCollection interface.
	*/
	hr = pElement->QueryInterface(IID_IHTMLDOMNode, (void**)&pElemDN);
	if (FAILED(hr) || !pElemDN)
		goto endFunc;

	hr = pElemDN->get_attributes(&pACDisp);
	if (FAILED(hr) || !pACDisp)
		goto endFunc;

	hr = pACDisp->QueryInterface(IID_IHTMLAttributeCollection, (void**)&pAttrColl);
	if (FAILED(hr) || !pAttrColl)
		goto endFunc;

	pAttrColl->get_length(&lACLength);

	vACIndex.vt = VT_I4;
	for (int i = 0; i < lACLength; i++)
	{
		vACIndex.lVal = i;

		//CStdString strTmp3 = GetOuterHTML(pElement);

		CComDispatchDriver pItemDisp;
		CComQIPtr<IHTMLDOMAttribute> pItem;

		pAttrColl->item(&vACIndex, &pItemDisp);
		hr = pItemDisp->QueryInterface(IID_IHTMLDOMAttribute, (void**)&pItem);
		if (FAILED(hr) || !pItem)
			goto endFunc;

		pItem->get_specified(&vbSpecified);
// 		if (vbSpecified == VARIANT_TRUE)
// 		{
			//仅取有效属性
			_bstr_t bstrName;
			pItem->get_nodeName(&bstrName.GetBSTR());
			pItem->get_nodeValue(&vValue);

			CString strTempName = (LPCTSTR)bstrName;
			strTempName.MakeLower();

			if (vValue.vt == VT_BSTR)
				mapAttribs[(_bstr_t)strTempName] = vValue.bstrVal;
			else if (vValue.vt == VT_I4)
			{
				CString strTmp = _T("");
				strTmp.Format(_T("%d"), vValue.intVal);
				mapAttribs[(_bstr_t)strTempName] = strTmp;
			}
		//}
	}

	//为INPUT 元素提取Value(环球经贸网 注册时行业选择)
	pElement->QueryInterface(IID_IHTMLInputElement, (void**)&qiInputBtnElem);
	if (NULL != qiInputBtnElem)
	{
		_bstr_t bstrVal;
		qiInputBtnElem->get_value(&bstrVal.GetBSTR());
		//((IHTMLInputElement *)qiInputBtnElem)->Release();
		if (0 != CompareBSTR(bstrVal, _T("")))
			mapAttribs[_T("value")] = bstrVal;

		/*qiInputBtnElem->get_type(&bstrVal.GetBSTR()) ;
		if ( 0 != CompareBSTR(bstrVal, _T("")) )
		{
			mapAttribs[_T("type")] = bstrVal ;
		}*/
	}

	//读取style Display:none 属性 获取是否隐藏
	hr = pElement->get_style(&pStyle) ;
	if ( FAILED(hr) || !pStyle )
	{
		;;
	}
	else
	{
		_bstr_t bstrVal;

		pStyle->get_display(&bstrVal.GetBSTR()) ;
		/*_bstr_t bstrVal1;
		pStyle->get_cssText(&bstrVal1.GetBSTR()) ;*/
		if ( 0 != CompareBSTR(bstrVal, _T("")) )
		{
			mapAttribs[_T("style")] = bstrVal ;
		}	
	}

	return;

endFunc:
	g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("GetAllAttributes失败，hr=0x%08x！"), hr);
}

void CHtmElementLocate::SetLevel(int nLevel)
{
	m_nLevel = nLevel;
}

int CHtmElementLocate::GetLevel()
{
	return m_nLevel;
}

//根据定位信息返回一个元素对象(精确定位),唯一定位成功后返回true,否则返回false
bool CHtmElementLocate::FromLocateInfo(T_ITEM *pItem, CComQIPtr<IHTMLElement> &pRetElem, int &iFound)
{
    HRESULT hr = 0;
	_bstr_t bstrName((LPCTSTR)(pItem->strTagName));

	if (NULL == m_pHtmlDocument)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLDocument接口无效，FromLocateInfo失败！"));
		return false;
	}

	if (NULL == pItem)
		return false;

	//有父节点的先定位父节点
	if (NULL != pItem->pParent)
	{
		if (FromLocateInfo(pItem->pParent, pRetElem, iFound))
		{
			//通过父节点定位本节点
			CComQIPtr<IHTMLElement> ptmpElem;
			if(SeekElementByParent(pRetElem, pItem, ptmpElem))
			{
				pRetElem = ptmpElem;
				iFound = 1;
				return true;
			}
		}

		return false;
	}

	//没有父节点直接定位本身
	T_ITEM targetItem;
	targetItem.strTagName = pItem->strTagName;

	//过滤无效属性
	std::map<CStdString, CStdString>::iterator itAttr = pItem->mapAttributes.begin();
	for (; itAttr !=  pItem->mapAttributes.end(); ++itAttr)
	{
		if (itAttr->second.Trim().GetLength() > 0)
			targetItem.mapAttributes[itAttr->first] = itAttr->second;
	}

	return SeekTargetElement(&targetItem, iFound, pRetElem);
}

//根据定位信息返回满足条件的元素对象集合(模糊定位)
bool CHtmElementLocate::FromLocateInfo(T_ITEM *pItem, QI_ELEMENT_COLLECTION &vResElem, int &iFound, vector<CStdString> &sFindCanDelete)
{
	if (NULL == m_pHtmlDocument)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLDocument接口无效，FromLocateInfo失败！"));
		return false;
	}

	vResElem.clear();
	iFound = SeekTargetInDocument(m_pHtmlDocument, *pItem, vResElem, sFindCanDelete);

	g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模糊定位时找到%d对象！"), iFound);

	return iFound > 0 ? true : false;
}

bool CHtmElementLocate::FromLocateInfo(T_ITEM *pItem, QI_ELEMENT_COLLECTION &vResElem, int &iFound)
{
	if (NULL == m_pHtmlDocument)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLDocument接口无效，FromLocateInfo失败！"));
		return false;
	}

	vResElem.clear();
	iFound = SeekTargetInDocument(m_pHtmlDocument, *pItem, vResElem);

	g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("模糊定位时找到%d对象！"), iFound);

	return iFound > 0 ? true : false;
}

//根据定位信息返回满足条件的元素对象,定位时优先尝试用局部属性，然后再使用全部属性
bool CHtmElementLocate::FromLocateInfoWithSimptify(T_ITEM *pItem, CComQIPtr<IHTMLElement> &pElem, T_ITEM &itemUsedAttr)
{
	if (NULL == m_pHtmlDocument)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLDocument接口无效，FromLocateInfo失败！"));
		return false;
	}

	if (NULL == pItem)
		return false;

	int iFound = 0;

	//尝试采用id+tagname唯一定位
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.strTagName = pItem->strTagName;
	itemUsedAttr.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//尝试采用name+tagname唯一定位
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//尝试采用 id+name+tagname
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
	itemUsedAttr.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//尝试采用 id+name+outertext+tagname
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
	itemUsedAttr.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
	itemUsedAttr.mapAttributes[_T("outtext")] = pItem->mapAttributes[_T("outtext")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//尝试采用 id+name+tagname+pertext
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
	itemUsedAttr.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
	itemUsedAttr.mapAttributes[_T("pertext")] = pItem->mapAttributes[_T("pertext")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//尝试采用 id+outertext+tagname+surtext
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
	itemUsedAttr.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
	itemUsedAttr.mapAttributes[_T("surtext")] = pItem->mapAttributes[_T("surtext")];
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	//上述方法都定位不到则尝试使用全部有效属性
	itemUsedAttr.mapAttributes.clear();
	itemUsedAttr.mapAttributes = pItem->mapAttributes;
	if (SeekTargetElement(&itemUsedAttr, iFound, pElem, true))
		return true;

	return false;
}


bool CHtmElementLocate::IsValidAttributeName(CComBSTR comBstrAtribName)
{
	//这些参数没有价值 需排除
	BSTR bstrExAttrib[] = 
	{_T("autocomplete"), 
	_T("onclick"), _T("onmousemove"), _T("onchange"), _T("onmouseover"), _T("oninput"), _T("onpropertychange"), _T("onmove"), _T("onselectstart"), _T("oncontrolselect"), _T("onkeypress"), _T("oncut"), _T("onrowenter"), _T("onmousedown"), _T("onmouseleave"), _T("onmouseout"), _T("onmouseenter"), _T("onmouseup"),
	_T("onpaste"), _T("onreadystatechange"), _T("onbeforedeactivate"), _T("onkeydown"), _T("onlosecapture"), _T("ondrag"), _T("ondragstart"), _T("oncellchange"), _T("onfilterchange"), _T("onrowsinserted"), _T("ondatasetcomplete"),
	_T("onmousewheel"), _T("ondragenter"), _T("onblur"), _T("onresizeend"), _T("onerrorupdate"), _T("onbeforecopy"), _T("ondblclick"), _T("onkeyup"), _T("onresizestart"), _T("onmoveend"), _T("onresize"), _T("ondrop"), _T("onpage"), _T("onrowsdelete"), _T("onfocusout"),
	_T("ondatasetchanged"), _T("ondeactivate"), _T("ondragover"), _T("onhelp"), _T("ondragend"), _T("onbeforeeditfocus"), _T("onfocus"), _T("onscroll"), _T("onbeforeactivate"), _T("onbeforecut"), _T("oncopy"), _T("onfocusin"), _T("onbeforeupdate"), _T("ondataavailable"), _T("onmovestart"),
	_T("onlayoutcomplete"), _T("onafterupdate"), _T("ondragleave"), _T("oncontextmenu"), _T("onrowexit"), _T("onbeforepaste"), _T("onactivate"), _T("onunload"), _T("onselect"), _T("onload"), _T("onbeforeprint"), _T("onafterprint"), _T("onbeforeunload"), _T("onerror"), _T("onabort"),
	/*_T("size"), _T("maxlength"), _T("height"), _T("width"), */_T("vspace"), _T("colspan"), _T("align"), _T("valign"), _T("rowspan"),
	_T("cellpadding"), _T("cellspacing"), _T("bgcolor"), _T("hspace"), _T("vspace"), _T("border"), /*_T("style"),*/
	_T("topmargin"), _T("leftmargin"), _T("rightmargin"), _T("bottommargin"), _T("frameborder"),
	_T("language")};

	if (!comBstrAtribName)
		return false;

	comBstrAtribName.ToLower();

	for (int i = 0; i < sizeof(bstrExAttrib) / sizeof(bstrExAttrib[0]); i++)
	{
		if (comBstrAtribName == bstrExAttrib[i])
		{
			return false;
		}
	}

	//排除"jquery*"
	CStdString strAttrName = comBstrAtribName;
	if (m_regEx.Like(strAttrName, _T("jquery.*")))
		return false;

	return true;
}

bool CHtmElementLocate::IsValidValue(CComBSTR comBstrVal, CComBSTR comBstrAtribName)
{
	//这些都是默认值,没有识别价值
	BSTR bstrExVal[] = 
	{_T("null"), _T(""), _T("0"), _T("false"), _T("2147483647"),_T("inherit")};

	if (!comBstrVal || !comBstrAtribName)
		return false;

	comBstrVal.ToLower();
	comBstrAtribName.ToLower();

	for (int i = 0; i < sizeof(bstrExVal) / sizeof(bstrExVal[0]); i++)
	{
		if (comBstrVal == bstrExVal[i])
		{
			return false;
		}
	}

	if (comBstrVal == CComBSTR(_T("1")) && comBstrAtribName == CComBSTR(_T("loop")))
		return false;

	if (comBstrVal == CComBSTR(_T("fileopen")) && comBstrAtribName == CComBSTR(_T("start")))
		return false;

	if (comBstrVal == CComBSTR(_T("soft")) && comBstrAtribName == CComBSTR(_T("wrap")))
		return false;

	//排除function*(*)*
	CStdString strVal = comBstrAtribName;
	if (m_regEx.Like(strVal, _T("function.*(.*).*")))
		return false;
	
	return true;
}

//去掉值为空的属性
void CHtmElementLocate::RemoveInvalidProperty(T_ITEM &item)
{
ReDo:
	bool bCheckEnd = true;
	std::map<CStdString, CStdString>::iterator itAttr = item.mapAttributes.begin();
	for (; itAttr != item.mapAttributes.end(); ++itAttr)
	{
		if (itAttr->second.Trim().GetLength() == 0)
		{
	    	bCheckEnd = false;
			item.mapAttributes.erase(itAttr);
			break;     //一定要跳出循环，否则迭代器++时会异常
		}
	}

	if (!bCheckEnd)
		goto ReDo;
}

//由于IE不支持读取 class 属性，需要按照 className 来读取，类似的还有其他的情况
void CHtmElementLocate::TransAttributeNameForIE(CComBSTR &strAttribName)
{
	strAttribName.ToLower();

	if (strAttribName == _T("class"))
		strAttribName = _T("className");

	else if (strAttribName == _T("for"))
		strAttribName = _T("htmlFor");

	else if (strAttribName == _T("tabindex"))
		strAttribName = _T("tabIndex");

	else if (strAttribName == _T("readonly"))
		strAttribName = _T("readOnly");

	else if (strAttribName == _T("usemap"))
		strAttribName = _T("useMap");

	else if (strAttribName == _T("contenteditable"))
		strAttribName = _T("contentEditable");

	else if (strAttribName == _T("maxlength"))
		strAttribName = _T("maxLength");

	else if (strAttribName == _T("cellspacing"))
		strAttribName = _T("cellSpacing");

	else if (strAttribName == _T("cellpadding"))
		strAttribName = _T("cellPadding");

	else if (strAttribName == _T("rowspan"))
		strAttribName = _T("rowSpan");

	else if (strAttribName == _T("colspan"))
		strAttribName = _T("colSpan");

	else if (strAttribName == _T("frameborder"))
		strAttribName = _T("frameBorder");
}

CStdString CHtmElementLocate::TrimDataBeforeSave(CStdString comBstrData)
{
	CStdStrUtils strUtil;
	CStdString strData(comBstrData);
	CString strResult = _T("");
	
// 	strData = _T("position:relative;width:650px;");

	//只取一定长度，超长没有意义
	strData = strData.Left(4096);
	strData.ToLower();

	//所有涉及元素的长/宽/位置的数据，都需要处理成*，以防止网页根据窗口大小自动调整，导致查找不到元素
	//pt px em ex in cm % mm pc
	TCHAR *tszToReplace[] = {_T("pt"), _T("px"), _T("em"), _T("ex"), _T("in"), _T("cm"), _T("mm"), _T("pc"), _T("%")};

	for (int i = 0; i < sizeof(tszToReplace) / sizeof(tszToReplace[0]); i++)
	{
		int iPos = -1;
		CStdString strPartData = _T("");

		do 
		{
			iPos = strData.Find(tszToReplace[i], iPos + 1);
			if (iPos == -1)
	            break;

			strPartData = GetPartTextByEnd(strData, tszToReplace[i], iPos);
			if (strUtil.IsNumber(strPartData))
			{
				strData = strData.Left(iPos - strPartData.GetLength() - 1) + _T("*") + strData.Mid(iPos + strPartData.GetLength());
                iPos = iPos - strPartData.GetLength();
			}
		} while (true);
	}

	return strData;
}

//提取紧邻的前文后者后文
void CHtmElementLocate::GetAdjacentText(CComQIPtr<IHTMLElement> pElement, bool bFindPerFix, CStdString &strText)
{
	HRESULT hr = 0;
	CComQIPtr<IHTMLElement2> QiElem2 = pElement;
	CComQIPtr<IHTMLElement> QiElemParent;
	CComQIPtr<IHTMLElementCollection> QiChildColl;

	LONG lngTotalChild = 0;
	LONG lngPos = 0;
	LONG LngSearchStart = 0;
	LONG lngSearchEnd = 0;
	LONG lngStep = 0;
	_bstr_t bstrVal;

	//找到父节点
	hr = pElement->get_parentElement(&QiElemParent);
	if (FAILED(hr) || NULL == QiElemParent)
		return;

	//If TypeName(objParent) = "HTMLDocument" Then Exit Sub

	//判断自身的位置
	lngPos = GetElemPosInParentContainer(QiElemParent, pElement, lngTotalChild, QiChildColl);

	//向前/后 查找 Html 以及 文字，以找到有效文字为终止
	if (bFindPerFix)
	{
		LngSearchStart = lngPos - 1;
		lngSearchEnd = 0;
		lngStep = -1;
		
		hr = QiElem2->getAdjacentText(_T("beforeBegin"), &bstrVal.GetBSTR());
	}
	else
	{
		LngSearchStart = lngPos + 1;
		lngSearchEnd = lngTotalChild - 1;
		lngStep = 1;

		QiElem2->getAdjacentText(_T("afterEnd"), &bstrVal.GetBSTR());
	}

	strText = (LPCTSTR)bstrVal.GetBSTR();
	if (strText.Trim().GetLength() < 2)
	{
		for (LONG i = LngSearchStart; bFindPerFix ? i >= lngSearchEnd : i <= lngSearchEnd; i+=lngStep)
		{
			_variant_t index = i;
			CComDispatchDriver spElem;

			hr = QiChildColl->item(index, index, &spElem);
			if (FAILED(hr) || NULL == spElem)
			{
				g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取前文失败，hr=0x%08x"), hr);
				break;
			}

			CComQIPtr<IHTMLElement> QiTempElem;
			spElem.QueryInterface(&QiTempElem);
			GetInnerText(QiTempElem, bFindPerFix, strText);

			if (strText.Trim().GetLength() > 1)
				break;
		}
	}

	if (strText.Trim().GetLength() > 1)
		return;
	else
		GetAdjacentText(QiElemParent, bFindPerFix, strText);
}

CStdString CHtmElementLocate::GetPartTextByEnd(CStdString strHtml, CStdString strEnd, int iPos)
{
	if (strHtml.GetLength() > iPos + strEnd.GetLength())
	{
		TCHAR pCh = strHtml.GetAt(iPos + strEnd.GetLength());

		switch (pCh)
		{
		case '\"':
		case ' ':
		case '\'':
		case ':':
		case ';':
		case ',':
		case VK_TAB:
		case 10:
		case 13:
			break;
		default:
			return _T("");
		}

// 		if (pCh == '\"' || pCh == ' ' || pCh == '\'' || pCh == ':' 
// 			|| pCh == ';' || pCh == ','
// 			|| pCh == VK_TAB || pCh == 10 || pCh == 13)
// 		{
// 			//Case 48 To 57, 65 To 90, 97 To 122, 95, Is < 0 继续处理
// 			;
// 		}
// 		else 
// 		{
// 			return _T("");
// 		}
		
		//向前找，找到前置部分
		for (int i = iPos -1; i >= 1; i--)
		{
			TCHAR pCh = strHtml.GetAt(i);
			if (pCh == '\"' || pCh == ' ' || pCh == '\'' || pCh == ':' 
				|| pCh == ';' || pCh == ','
				|| pCh == VK_TAB || pCh == 10 || pCh == 13)
			{
				//找到了头
				return strHtml.Mid(i + 1, iPos - i - 1);
			}
		}
	}

	return _T("");
}

//在指定父节点下查找元素
bool CHtmElementLocate::SeekElementByParent(CComQIPtr<IHTMLElement> pParent, T_ITEM *pItem, CComQIPtr<IHTMLElement> &qiTargetElem)
{
	if (NULL == pParent || NULL == pItem)
		return false;

	int iFoundCount = 0;
	HRESULT hr = 0;
	CComDispatchDriver spDriver;
	CComQIPtr<IHTMLElementCollection> elemColl;

	hr = pParent->get_children(&spDriver);
	if (FAILED(hr) || NULL == spDriver)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("SeekElementByParent失败,hr=0x%08x！"), hr);
		return false;
	}

	hr = spDriver->QueryInterface(IID_IHTMLElementCollection, (void**)&elemColl);
	if (SUCCEEDED(hr))
	{
		LONG lngCount = 0;
		elemColl->get_length(&lngCount);

		//逐个匹配，找到所有符合条件的元素
		for (LONG i = 0; i < lngCount; i++)
		{
			_variant_t vIndex = i;
			CComDispatchDriver spElem;
			hr = elemColl->item(vIndex, vIndex, &spElem);
			if (FAILED(hr) || NULL == spElem)
				break;

			CComQIPtr<IHTMLElement> QiElem;
			spElem.QueryInterface(&QiElem);
			if (ItemFeatureFit(QiElem, *pItem))
			{
				iFoundCount++;   //统计匹配个数
				qiTargetElem = QiElem;
			}
		}
	}

	if (iFoundCount == 1)
		return true;

	g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("SeekElementByParent失败,hr=0x%08x！"), hr);
	return false;
}

// Converts a IHTMLWindow2 object to a IHTMLDocument2. Returns NULL in case of failure.
// It takes into account accessing the DOM across frames loaded from different domains.
IHTMLDocument2 *CHtmElementLocate::HtmlWindowToHtmlDocument(CComQIPtr<IHTMLWindow2> spWindow)
{
	ATLASSERT(spWindow != NULL);

	IHTMLDocument2 *spDocument = NULL;
	HRESULT hRes = spWindow->get_document(&spDocument);

	if ((S_OK == hRes) && (NULL != spDocument))
	{
		// The html document was properly retrieved.
		return spDocument;
	}

	// hRes could be E_ACCESSDENIED that means a security restriction that
	// prevents scripting across frames that loads documents from different internet domains.
	CComQIPtr<IWebBrowser2>  spBrws;// = HtmlWindowToHtmlWebBrowser(spWindow);
	spBrws.Attach(HtmlWindowToHtmlWebBrowser(spWindow));
	if (spBrws == NULL)
	{
		return CComQIPtr<IHTMLDocument2>();
	}

#ifdef xiaoqiang
	//CComQIPtr cao
	((IWebBrowser2 *)spBrws)->Release();
#endif
	// Get the document object from the IWebBrowser2 object.
	CComQIPtr<IDispatch> spDisp;
	hRes = spBrws->get_Document(&spDisp);
	if ((S_OK == hRes) && (NULL != spDisp))
	{
		spDisp.QueryInterface(&spDocument);
		return spDocument;
	}

	return CComQIPtr<IHTMLDocument2>();
}


// Converts a IHTMLWindow2 object to a IWebBrowser2. Returns NULL in case of failure.
CComQIPtr<IWebBrowser2> CHtmElementLocate::HtmlWindowToHtmlWebBrowser(CComQIPtr<IHTMLWindow2> spWindow)
{
	ATLASSERT(spWindow != NULL);

	CComQIPtr<IServiceProvider>  spServiceProvider = spWindow;
	if (spServiceProvider == NULL)
	{
		return CComQIPtr<IWebBrowser2>();
	}

	CComQIPtr<IWebBrowser2> spWebBrws;
	HRESULT hRes = spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spWebBrws);
	if (hRes != S_OK)
	{
		return CComQIPtr<IWebBrowser2>();
	}

	return spWebBrws;
}


//获取前/后/本身元素的文本属性
void CHtmElementLocate::GetInnerText(CComQIPtr<IHTMLElement> pElem, bool bFindPerFix, CStdString &strText)
{
	LONG lngTotalChild = 0;
	LONG lngPos = 0;
	LONG LngSearchStart = 0;
	LONG lngSearchEnd = 0;
	LONG lngStep = 0;
	CComQIPtr<IHTMLElementCollection> QiColl;
	CComQIPtr<IHTMLElement2> QiElem2 = pElem;
	_bstr_t bstrVal;
	HRESULT hr = 0;

	lngTotalChild = GetChildCollection(pElem, &QiColl);
	if (lngTotalChild > 0)
	{
		if (bFindPerFix)
		{
			LngSearchStart = lngTotalChild - 1;
			lngSearchEnd = 0;
			lngStep = -1;

			QiElem2->getAdjacentText(_T("beforeEnd"), &bstrVal.GetBSTR());
		}
		else
		{
			LngSearchStart = 0;
			lngSearchEnd = lngTotalChild - 1;
			lngStep = 1;

			QiElem2->getAdjacentText(_T("afterBegin"), &bstrVal.GetBSTR());
		}

		strText = (LPCTSTR)bstrVal.GetBSTR();
		if (strText.Trim().GetLength() < 2)
		{
			//先在子节点中取text作为前后文
			for (LONG i = LngSearchStart; bFindPerFix ? i >= lngSearchEnd : i <= lngSearchEnd; i+=lngStep)
			{
				_variant_t index = i;
				CComDispatchDriver spElem;

				hr = QiColl->item(index, index, &spElem);
				if (FAILED(hr) || NULL == spElem)
				{
					g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("获取前文失败，hr=0x%08x"), hr);
					break;
				}

				CComQIPtr<IHTMLElement> QiTempElem;
				spElem.QueryInterface(&QiTempElem);
				GetInnerText(QiTempElem, bFindPerFix, strText);

				if (strText.Trim().GetLength() > 1)
					return;
			}

			//若从子结点中取前后文失败则取当前结点的text
			if (strText.Trim().GetLength() < 2)
			{
				GetTextForAdjentText(pElem, bFindPerFix, strText);
			}
		}
	}
	else
	{
		GetTextForAdjentText(pElem, bFindPerFix, strText);
	}
}

//提取元素的text相关属性作为前后文使用
void CHtmElementLocate::GetTextForAdjentText(CComQIPtr<IHTMLElement> pElem, bool bFindPerFix, CStdString &strText)
{
	CStdString strValue = _T("");
	_bstr_t bstrVal;
	if (bFindPerFix)
	{
		pElem->get_outerText(&bstrVal.GetBSTR());
		strValue = (LPCTSTR)bstrVal.GetBSTR();

		//优先采用outerText
		if (strValue.Trim().GetLength() > 1)
		{
			strText = strValue + strText;
			return;
		}

		//然后采用title
		strValue = GetProperyByName(pElem, _T("title"));
		if (strValue.Trim().GetLength() > 1)
		{
			strText = strValue + strText;
			return;
		}

		// 			//最后采用Alt
		// 			strValue = GetProperyByName(pElem, _T("alt"));
		// 			if (strValue.Trim().GetLength() > 0)
		// 			{
		// 				strText = strValue + strText;
		// 				return;
		// 			}
	}
	else
	{
		pElem->get_outerText(&bstrVal.GetBSTR());
		strValue = (LPCTSTR)bstrVal.GetBSTR();

		//优先采用outerText
		if (strValue.Trim().GetLength() > 1)
		{
			strText = strText + strValue;
			return;
		}

		//然后采用title
		strValue = GetProperyByName(pElem, _T("title"));
		if (strValue.Trim().GetLength() > 1)
		{
			strText = strText + strValue;
			return;
		}

		// 			//最后采用Alt
		// 			strValue = GetProperyByName(pElem, _T("alt"));
		// 			if (strValue.Trim().GetLength() > 0)
		// 			{
		// 				strText = strText + strValue;
		// 				return;
		// 			}
	}

}

//在父节点下查找指定结点的位置
LONG CHtmElementLocate::GetElemPosInParentContainer(CComQIPtr<IHTMLElement> pParent, CComQIPtr<IHTMLElement> pElem, LONG &lngTotalChild, CComQIPtr<IHTMLElementCollection> &childColl)
{
	if (NULL == pParent)
		return -1;

	HRESULT hr = 0;

	//1. 查询父结点下的子结点集合
	CComDispatchDriver spDirver;
	hr = pParent->get_children(&spDirver);
	if (FAILED(hr) || NULL == spDirver)
		return -1;

	hr = spDirver->QueryInterface(IID_IHTMLElementCollection, (void**)&childColl);
	if (FAILED(hr) || NULL == childColl)
		return -1;

	childColl->get_length(&lngTotalChild);

	//2. 获取目标节点的IUnknown接口
	IUnknownPtr pUnKnownThis;
	pElem->QueryInterface(&pUnKnownThis);

	for (LONG i = 0; i < lngTotalChild; i++)
	{
		_variant_t vIndex = i;
		CComDispatchDriver spElem;
		hr = childColl->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			continue;

		IUnknownPtr pTempElem;
		spElem->QueryInterface(&pTempElem);

		//3. 比较集合下节点的IUnknow接口与目标节点的IUnKnown接口
 		if (pTempElem == pUnKnownThis)
 		{
			//pTempElem->Release();
 			return i;
 		}

		//pTempElem->Release();
	}

	//pUnKnownThis->Release();

	return -1;
}

//获取子节点信息，返回子节点个数及其集合
LONG CHtmElementLocate::GetChildCollection(CComQIPtr<IHTMLElement> pElem, IHTMLElementCollection **coll)
{
	if (NULL == pElem)
		return -1;

	HRESULT hr = 0;
	LONG lngTotalChild = 0;
	CComDispatchDriver spDirver;
	hr = pElem->get_children(&spDirver);
	if (FAILED(hr) || NULL == spDirver)
		return -1;

	hr = spDirver->QueryInterface(IID_IHTMLElementCollection, (void**)coll);
	if (FAILED(hr) || NULL == coll)
		return -1;

	(*coll)->get_length(&lngTotalChild);

	return lngTotalChild;
}

//使用给定的子结点替换列表中的父节点
bool CHtmElementLocate::ReplaceParentElemWithChild(QI_ELEMENT_COLLECTION &elems, CComQIPtr<IHTMLElement> qiElem)
{
	if (elems.empty())
		return false;

	HRESULT hr = 0;
	QI_ELEMENT_COLLECTION::iterator itElem = elems.begin();

	for (; itElem != elems.end(); ++itElem)
	{
		VARIANT_BOOL bVar = VARIANT_FALSE;
		hr = (*itElem)->contains(qiElem, &bVar);
		if (VARIANT_TRUE == bVar)
		{
			*itElem = qiElem;
		    return true;
		}
	}

	return false;
}

//根据属性名返回指定元素的固有属性
CStdString CHtmElementLocate::GetProperyByName(CComQIPtr<IHTMLElement> pElem, _bstr_t bstrName)
{
// 	CComDispatchDriver spElem = pElem;
// 	CComVariant vVal;
// 	spElem.GetPropertyByName(bstrName.GetBSTR(), &vVal);
// 
// 	if (vVal.vt == VT_BSTR && NULL != vVal.bstrVal)
// 		return (LPCTSTR)vVal.bstrVal;
// 	else 
// 		return _T("");

	//获取元素的所有有效属性
	std::map<_bstr_t, _bstr_t> mapAttribs;
	GetAllAttributes(pElem, mapAttribs);
	return mapAttribs[bstrName];
}


CStdString CHtmElementLocate::GetTagName(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_tagName(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetInnerText(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_innerText(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

LONG CHtmElementLocate::GetOffsetHeight(CComQIPtr<IHTMLElement> pElem)
{
	long lngHeight = 0;
	pElem->get_offsetHeight(&lngHeight);

	return lngHeight;
}

LONG CHtmElementLocate::GetOffsetWidth(CComQIPtr<IHTMLElement> pElem)
{
	long lngWidth = 0;
	pElem->get_offsetWidth(&lngWidth);

	return lngWidth;
}

void CHtmElementLocate::SetVisiblity(CComQIPtr<IHTMLElement> pElem, bool bShow)
{
	CComQIPtr<IHTMLStyle> QiStyle;
	pElem->get_style(&QiStyle);

	if (bShow)
	{
		QiStyle->put_display(_T(""));  //非none即显示
	}
	else
	{
		QiStyle->put_display(_T("none"));
	}
}

CStdString CHtmElementLocate::GetVisiblity(CComQIPtr<IHTMLElement> pElem)
{
	CComQIPtr<IHTMLStyle> QiStyle;
	pElem->get_style(&QiStyle);
	_bstr_t bstr;

	if (QiStyle)
	{
		QiStyle->get_display(&bstr.GetBSTR());
	}

	if(NULL == bstr.GetBSTR())
	{
		QiStyle->get_cssText(&bstr.GetBSTR());		
	}

	return (LPCTSTR)bstr;
}

CStdString CHtmElementLocate::GetStyle(CComQIPtr<IHTMLElement> pElem)
{
	//暂时先提取Visibility属性(值为：none、block...),其它属性可根据需要添加
	CStdString strStyle = _T("");
	strStyle += GetVisiblity(pElem);

	return strStyle;
}

long CHtmElementLocate::GetSelectElementLen(CComQIPtr<IHTMLSelectElement> pElem)
{
	long lenth = 0;
	pElem->get_length(&lenth);
	return lenth;
}

//模拟触发事件
HRESULT CHtmElementLocate::FireEvent(CComQIPtr<IHTMLElement> pElem, _bstr_t bstrEventName)
{
	HRESULT hr = S_FALSE;
	CComPtr<IHTMLDocument4> pDoc4;
	hr = m_pHtmlDocument->QueryInterface(IID_IHTMLDocument4,(void**)&pDoc4); 
	if (hr==S_OK)
	{ 
		CComPtr<IHTMLEventObj> pEvent; 
		pDoc4->createEventObject(NULL, &pEvent); 
		CComQIPtr<IHTMLEventObj2> pEvent2(pEvent); 
		//CComBSTR bstrEventName.GetBSTR() = bstrEventName; //L"onmousedown"; 
		pEvent2->put_type(bstrEventName.GetBSTR()); 
		//pEvent2->put_button(2); 
 		long x1, y1, x2, y2; 
 		pElem->get_offsetLeft(&x1); 
 		pElem->get_offsetTop(&y1); 
 		pElem->get_offsetWidth(&x2); 
 		pElem->get_offsetHeight(&y2); 
 		pEvent2->put_offsetX(x1+(x2>>1)); 
 		pEvent2->put_offsetY(y1+(y2>>1)); 
		CComQIPtr<IHTMLElement3, &IID_IHTMLElement3> pElemFrame3(pElem); 
		VARIANT_BOOL result; 
		CComVariant vEvent = pEvent; 
		hr = pElemFrame3->fireEvent(bstrEventName.GetBSTR(), &vEvent, &result); 
	} 

	return hr;
}

CStdString CHtmElementLocate::GetOptionElementVal(IHTMLOptionElement *pElem)
{
	_bstr_t bstrVal;
	pElem->get_value(&bstrVal.GetBSTR());
	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetOptionElementText(IHTMLOptionElement *pElem)
{
	_bstr_t bstrVal;
	pElem->get_text(&bstrVal.GetBSTR());
	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetInputElementType(CComQIPtr<IHTMLInputElement> pElem)
{
	if (NULL == pElem)
	{
		g_msgbox.Show(_T("错误"), _T("IHTMLInputElement指针无效，查询类型失败！"));
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("IHTMLInputElement指针无效，查询类型失败！"));

		return _T("");
	}
	_bstr_t bstrVal;
	pElem->get_type(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetOuterHTML(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_outerHTML(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetPageHTML(CComQIPtr<IHTMLDocument2> pDoc)
{
	CStdString strHTMLSrc = _T("") ;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc ;

	if ( NULL == pDoc3 )
	{
		return strHTMLSrc ;
	}

	CComQIPtr<IHTMLElement> pDocElem ;
	pDoc3->get_documentElement(&pDocElem) ;
	if ( NULL == pDocElem )
	{
		return strHTMLSrc ;
	}

	_bstr_t bstrHTMLSrc = _T("") ;
	pDocElem->get_outerHTML(&bstrHTMLSrc.GetBSTR()) ;
	strHTMLSrc = bstrHTMLSrc ;

	return strHTMLSrc ;
}

//CStdString CHtmElementLocate::GetPageHTML(MSHTML::IHTMLDocument2Ptr pDoc)
//{
//	CStdString strHTMLSrc = _T("") ;
//	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc ;
//
//	if ( NULL == pDoc3 )
//	{
//		return strHTMLSrc ;
//	}
//
//	CComQIPtr<IHTMLElement> pDocElem ;
//	pDoc3->get_documentElement(&pDocElem) ;
//	if ( NULL == pDocElem )
//	{
//		return strHTMLSrc ;
//	}
//
//	_bstr_t bstrHTMLSrc = _T("") ;
//	pDocElem->get_outerHTML(&bstrHTMLSrc.GetBSTR()) ;
//	strHTMLSrc = bstrHTMLSrc ;
//
//	return strHTMLSrc ;
//}

CStdString CHtmElementLocate::GetOuterText(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_outerText(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetInnerHTML(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_innerHTML(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}

CStdString CHtmElementLocate::GetOutHTML(CComQIPtr<IHTMLElement> pElem)
{
	_bstr_t bstrVal;
	pElem->get_outerHTML(&bstrVal.GetBSTR());

	return (LPCTSTR)bstrVal;
}
//获取元素的指定属性
CStdString CHtmElementLocate::GetAttributeValue(CComQIPtr<IHTMLElement> pElem, CStdString strAttrName)
{
	CStdString strValue = _T("");

	//自定义属性需要特殊处理
	if (!strAttrName.CompareNoCase(_T("outtext")))
	{
		strValue = GetOuterText(pElem).Left(4096);          //只取一定长度,超长的没有意义
	}
	else if (!strAttrName.CompareNoCase(_T("pertext")))
	{
		GetAdjacentText(pElem, true, strValue);
	}
	else if (!strAttrName.CompareNoCase(_T("surtext")))
	{
		GetAdjacentText(pElem, false, strValue);
	}
	else if (!strAttrName.CompareNoCase(_T("style")))
	{
		strValue = GetStyle(pElem);
	}
	else if (!strAttrName.CompareNoCase(_T("html")))
	{
		strValue = GetOuterHTML(pElem);
	}
	else if(!strAttrName.CompareNoCase(_T("SelectText")))
	{
		GetSelectElementText(pElem, strValue);
	}
	else if(!strAttrName.CompareNoCase(_T("SelectValue")))
	{
		GetSelectElementValue(pElem, strValue);
	}
	else
	{
		//获取HTML元素本身的属性
		strValue = GetProperyByName(pElem, (LPCTSTR)strAttrName);
	}

	return strValue;
}


//获取元素的所有可用属性（包括自定义的如前后文属性）
void CHtmElementLocate::GetAllUsedAttributes(CComQIPtr<IHTMLElement> pElem, T_ITEM *&pItem)
{
	pItem = GetWantedAttributes(pElem);
}

bool CHtmElementLocate::Simptify(T_ITEM *pItem, int &iTotalFound, bool bAuto)
{
	if (NULL == pItem)
		return false;

	T_ITEM tmpItem;

	if (bAuto)
	{
		//尝试采用id+tagname唯一定位
		tmpItem.mapAttributes.clear();
		tmpItem.strTagName = pItem->strTagName;
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用name+tagname唯一定位
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 id+name+tagname
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 id+name+outertext+tagname
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		tmpItem.mapAttributes[_T("outtext")] = pItem->mapAttributes[_T("outtext")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 id+name+value+tagname
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		tmpItem.mapAttributes[_T("value")] = pItem->mapAttributes[_T("value")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 id+name+tagname+pertext
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		tmpItem.mapAttributes[_T("pertext")] = pItem->mapAttributes[_T("pertext")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 id+outertext+tagname+surtext
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("id")] = pItem->mapAttributes[_T("id")];
		tmpItem.mapAttributes[_T("name")] = pItem->mapAttributes[_T("name")];
		tmpItem.mapAttributes[_T("surtext")] = pItem->mapAttributes[_T("surtext")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;

		//尝试采用 pertext+surtext+tagname
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes[_T("pertext")] = pItem->mapAttributes[_T("pertext")];
		tmpItem.mapAttributes[_T("surtext")] = pItem->mapAttributes[_T("surtext")];
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;


		//上述方法都定位不到则尝试使用全部有效属性
		tmpItem.mapAttributes.clear();
		tmpItem.mapAttributes = pItem->mapAttributes;
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;
	}
	else
	{
		//////组合，id/name 是一定要使用的，从自由组合条件中删除，而且这两个组合最先尝试效率最佳
		//手动简化时使用全部有效属性
		tmpItem.mapAttributes = pItem->mapAttributes;
		if (SimptifyByAttributes(tmpItem, iTotalFound, pItem))
			return true;
	}

	return false;

}

//用指定属性（集）定位对象，若定位成功则将这些属性作为对象的定位属性
bool CHtmElementLocate::SimptifyByAttributes(T_ITEM &tmpItem, int &iCount, T_ITEM *pRowItem)
{
	T_ITEM targetItem;
	targetItem.strTagName = tmpItem.strTagName;
	targetItem.mapAttributes = tmpItem.mapAttributes;
	iCount = 0;

	CComQIPtr<IHTMLElement> qiElem;
	SeekTargetElement(&targetItem, iCount, qiElem);
	if (iCount == 1)
	{
		//通过指定属性（集）定位成功，将这些属性作为简化后的定位属性
		pRowItem->mapAttributes.clear();
		pRowItem->mapAttributes = targetItem.mapAttributes;

		CStdString strMsg = _T("元素定位成功，属性：\r\n");
		std::map<CStdString, CStdString>::iterator it = targetItem.mapAttributes.begin();
		for (; it != targetItem.mapAttributes.end(); ++it)
		{
			strMsg += it->first + _T("=") + it->second;
		}
		g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("%s"), CString(strMsg));

		return true;
	}

	return false;
}

//根据指定属性定位元素,定位成功返回true并传回找到的元素指针
//否则返回false
bool CHtmElementLocate::SeekTargetElement(T_ITEM *pItem, int &iFoundCount, CComQIPtr<IHTMLElement> &qiElem, bool bReChekHide)
{
	//过滤无效属性
	RemoveInvalidProperty(*pItem);

	if (pItem->mapAttributes.empty())
		return false;

	QI_ELEMENT_COLLECTION qiElemColl;
	int iElemCount = SeekTargetInDocument(m_pHtmlDocument, *pItem, qiElemColl);

	bool bAdd = false;
	int iVisbleIndex = 0;         //可见元素的序号

	//仅保留可见对象（表单除外）
	for (int i = 0; i < iElemCount; i++)
	{
		bAdd = false;
		if (!GetTagName(qiElemColl[i]).CompareNoCase(_T("form")))
			bAdd = true;
		else
		{
			if (GetOffsetHeight(qiElemColl[i]) > 0 && GetOffsetWidth(qiElemColl[i]) > 0)
				bAdd = true;
		}

		if (bAdd)
		{
			iFoundCount++;
			iVisbleIndex = i;
		}
	}

	//特定情况下，不可见对象需要处理
	if (iFoundCount == 0 && iElemCount >= 0)
	{
		if (bReChekHide)
		{
			iFoundCount = iElemCount;

			for (int i = 0; i < iElemCount; i++)
			{
				//把隐藏的重新设为可见
				SetVisiblity(qiElemColl[i], true);

				IHTMLElement *pElemParent = NULL;
				qiElemColl[i]->get_parentElement(&pElemParent);

				do 
				{
					if (NULL == pElemParent)
						break;

					CComQIPtr<IHTMLElement> pTmpElem = pElemParent;

					SetVisiblity(pElemParent, true);
					pTmpElem->get_parentElement(&pElemParent);
				} while (true);
			}
		}
	}

	//返回找到的可见对象
	if (iFoundCount == 1)
	{
		qiElem = qiElemColl.at(iVisbleIndex);
		return true;
	}

	//记录找到的多个对象，便于定位问题
	if (iFoundCount > 1)
	{
		CString strText = _T("");
		strText.Format(_T("精确定位时找到%d个对象："), iFoundCount);
		for (int i = 0; i < iFoundCount; i++)
		{
			strText += _T("\r\n");
			strText += GetOuterHTML(qiElemColl.at(i));
		}

		g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,strText);
	}

	return false;
}

//从 Document对象（包括子框架）中，查找符合tarItem目标特征的所有元素
LONG CHtmElementLocate::SeekTargetInDocument(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem,  QI_ELEMENT_COLLECTION &qiElemColl, vector<CStdString> &sFindCanDelete)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3 || tarItem.mapAttributes.size() == 0)
		return 0;

	if (tarItem.strTagName.GetLength() == 0)
		pDoc->get_all(&coll);
	else
	{
		CStdString strTp = tarItem.strTagName;
		if(strTp.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("---------%s == %d---------"), tarItem.strTagName, strTp.GetLength());
		}
		
		CString strTar = CString(strTp) + _T("");
		if(strTar.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("---------%s == %d---------"), strTar, strTar.GetLength());
		}

		LPCTSTR lpStr = (LPCTSTR)strTar;
		if(strTp.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("---------%s---------"),lpStr);
		}

		//g_htmlLocate.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("---------%s---------"), lpStr);
		pDoc3->getElementsByTagName(_bstr_t(lpStr), &coll);
		//pDoc3->getElementsByTagName(_bstr_t((LPCTSTR)tarItem.strTagName), &coll);
	}

	HRESULT hr = 0;
	LONG lngFitCount = 0;
	LONG lngCount = 0;
	if(NULL == coll)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------coll == NULL---------"));
		return 0;
	}
	coll->get_length(&lngCount);

	//逐个匹配，找到所有符合条件的元素
	for (LONG i = 0; i < lngCount; i++)
	{
		_variant_t vIndex = i;
    	CComDispatchDriver spElem;
		hr = coll->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			break;

		CComQIPtr<IHTMLElement> QiElem;
		spElem.QueryInterface(&QiElem);
		if (ItemFeatureFit(QiElem, tarItem, sFindCanDelete))
		{
			//如果新找到的元素是已找到元素的子元素，则保留该子元素并从找到列表中移除父元素
			//此做法是为了考虑多个控件嵌套的情况，此时仅认为是找到了一个子元素，避免唯一定位失败
			if (!ReplaceParentElemWithChild(qiElemColl, QiElem))
			{
				//列表中不存在当前元素的父元素则直接保存该元素到列表中
				qiElemColl.push_back(QiElem);
				lngFitCount++;
			}
		}
	}

	if (tarItem.strTagName.GetLength() == 0)
	{
		//在这种情况下，需要将父子进行过滤，只取最小的子节点
		//暂不处理...
	}

	//文档中，可能包含框架，继续在框架中搜索
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("frame"));
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("iframe"));

	return lngFitCount;
}

void CHtmElementLocate::DelNodeInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM *tarItem, _bstr_t bstrFrameTag)
{
	LONG lngFrameCount = 0;
	CComQIPtr<IHTMLElementCollection> QiFrameColl;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3)
	{
		return ;
	}

	pDoc3->getElementsByTagName(bstrFrameTag, &QiFrameColl);
	if(NULL == QiFrameColl)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("QiFrameColl == NULL"));
		return ;
	}
	QiFrameColl->get_length(&lngFrameCount);
	for (LONG i = 0; i < lngFrameCount; i++)
	{
		_variant_t vIndex = i;
		CComQIPtr<IHTMLElement> QiFrame;
		CComDispatchDriver spElem;
		QiFrameColl->item(vIndex, vIndex, &spElem);
		if(NULL == spElem)
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("spElem == NULL"));
			continue;
		}
		HRESULT hr = spElem.QueryInterface(&QiFrame);
		if(NULL == QiFrame)
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("QiFrame == NULL"));
			continue;
		}
		IHTMLDocument2 *pFrameDoc2 = NULL;
		hr = GetFrameDocument2(QiFrame, pFrameDoc2);
		if (NULL != pFrameDoc2)
		{
			DelNodeInDocument(pFrameDoc2, tarItem);
		}
	}
}

void CHtmElementLocate::DelTargetAllNodes(T_ITEM *tarItem)
{
	if (m_pHtmlDocument == NULL)
	{
		return;
	}

	DelNodeInDocument(m_pHtmlDocument, tarItem);
}

void CHtmElementLocate::DelNodeInDocument(CComQIPtr<IHTMLDocument2> pDoc2, T_ITEM *tarItem)
{
	HRESULT hr = 0;
	CComQIPtr<IHTMLElementCollection> coll;
	//CComQIPtr<IHTMLDocument2> pDoc2 = m_pHtmlDocument;

	if (NULL == pDoc2 || tarItem->mapAttributes.size() == 0)
	{
		return ;
	}

	long lngCount = 0;
	pDoc2->get_all(&coll);
	coll->get_length(&lngCount);

	//逐个匹配，找到所有符合条件的元素
	for (LONG i = 0; i < lngCount; i++)
	{
		_variant_t vIndex = i;
		CComDispatchDriver spElem;
		hr = coll->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			break;

		CComQIPtr<IHTMLElement> QiElem;
		spElem.QueryInterface(&QiElem);
		if (ItemFeatureFit(QiElem, *tarItem))//找到这个元素符合当前节点所有属性
		{
			if (NULL != QiElem)
			{
				IHTMLDOMNode *pNode;
				HRESULT hr1 = QiElem->QueryInterface(&pNode);
				if (SUCCEEDED(hr1))
				{
					pNode->removeNode(VARIANT_TRUE, &pNode);
				}
			}
		}
	}

	//文档中，可能包含框架，继续在框架中搜索
	DelNodeInFrame(pDoc2, tarItem, _T("frame"));
	DelNodeInFrame(pDoc2, tarItem, _T("iframe"));

	return ;
}

//从 Document对象（包括子框架）中，查找符合tarItem目标特征的所有元素
LONG CHtmElementLocate::SeekTargetInDocument(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem,  QI_ELEMENT_COLLECTION &qiElemColl)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3 || tarItem.mapAttributes.size() == 0)
		return 0;

	if (tarItem.strTagName.GetLength() == 0)
		pDoc->get_all(&coll);
	else
	{
		CStdString strTp = tarItem.strTagName;
		if(strTp.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------%s == %d---------"), tarItem.strTagName, strTp.GetLength());
		}

		CString strTar = CString(strTp) + _T("");
		if(strTar.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------%s == %d---------"), strTar, strTar.GetLength());
		}

		LPCTSTR lpStr = (LPCTSTR)strTar;
		if(strTp.GetLength() > 1000)//太长的字符串就打印出来看看是什么
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------%s---------"),lpStr);
		}

		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------%s---------"), lpStr); 

		try
		{
			pDoc3->getElementsByTagName(_bstr_t(lpStr), &coll);
		}
		catch (...)
		{
			;
		}
		
	}

	HRESULT hr = 0;
	LONG lngFitCount = 0;
	LONG lngCount = 0;
	if(NULL == coll)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------coll == NULL---------"));
		return 0;
	}
	coll->get_length(&lngCount);

	//逐个匹配，找到所有符合条件的元素
	for (LONG i = 0; i < lngCount; i++)
	{
		_variant_t vIndex = i;
		CComDispatchDriver spElem;
		hr = coll->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			break;

		CComQIPtr<IHTMLElement> QiElem;
		spElem.QueryInterface(&QiElem);
		if (ItemFeatureFit(QiElem, tarItem))//找到这个元素符合当前节点所有属性
		{
			if (IsParentNodesFit(QiElem, tarItem))//找到当前节点，如果当前节点有父节点删除的功能的，进一步过滤
			{
				//如果新找到的元素是已找到元素的子元素，则保留该子元素并从找到列表中移除父元素
				//此做法是为了考虑多个控件嵌套的情况，此时仅认为是找到了一个子元素，避免唯一定位失败
				if (!ReplaceParentElemWithChild(qiElemColl, QiElem))
				{
					//列表中不存在当前元素的父元素则直接保存该元素到列表中
					qiElemColl.push_back(QiElem);
					lngFitCount++;
				}
			}
			
		}
	}

	if (tarItem.strTagName.GetLength() == 0)
	{
		//在这种情况下，需要将父子进行过滤，只取最小的子节点
		//暂不处理...
	}

	//文档中，可能包含框架，继续在框架中搜索
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("frame"));
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("iframe"));

	return lngFitCount;
}

T_ITEM *CHtmElementLocate::FindLastItemFormItemTree(T_ITEM *pItemTree)//查找一个Item树的最后一个节点
{
	T_ITEM *pTarItem = pItemTree;

	std::map<int, T_ITEM *>::iterator iter;
	while(pTarItem->m_mapNodesAttr.size() > 0)
	{
		iter = pTarItem->m_mapNodesAttr.begin();
		pTarItem = iter->second;
	}

	return pTarItem;
}

bool CHtmElementLocate::CompareSourceAttrIsRight(T_ITEM *pSource, T_ITEM *pDest)
{
	if(pDest == NULL || pSource == NULL)
	{
		return false;
	}

	std::map<CStdString, CStdString>::iterator iterSource = pSource->mapAttributes.begin();
	std::map<CStdString, CStdString>::iterator iterDest;

	for(; iterSource != pSource->mapAttributes.end(); ++iterSource)
	{
		iterDest = pDest->mapAttributes.find(iterSource->first);
		/*if (iterDest->first == _T("父节点"))
		{
		continue;
		}
		*/
		if (iterDest != pDest->mapAttributes.end())
		{
			if(iterSource->second != iterDest->second)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CHtmElementLocate::IsTreeHaveOnlyOneNodePath(T_ITEM *pItemTree)
{
	T_ITEM *pTmpItem = NULL;
	int nNumTmp = pItemTree->m_mapNodesAttr.size();
	std::map<int, T_ITEM *>::iterator iter = pItemTree->m_mapNodesAttr.begin();
	
	while(nNumTmp > 0)//没有到最后一层，最后一层没有节点，全属性
	{
		if(nNumTmp == 1)//如果当前层只有一个节点说明是对的，继续往下
		{
			pTmpItem = iter->second;
			nNumTmp = pTmpItem->m_mapNodesAttr.size();
			if(0 != nNumTmp)
			{
				iter = pTmpItem->m_mapNodesAttr.begin();
			}
		}
		else if (nNumTmp > 1)//路径不止一条的时候，不做任何处理，这种情况也不匹配，直接判错
		{
			return false;
		}
	}

	return true;
}

bool CHtmElementLocate::IsLevelComfit(T_ITEM* constructTreeItem, T_ITEM *TargetTree)
{
	//bool CHtmElementLocate::ItemFeatureFit(IHTMLElement *pElem, T_ITEM &tarItem)
	//{
	//	CStdString strMatchedValue = _T("");
	//	std::vector<CStdString> vAttributeName;
	//	tarItem.GetAttruteNames(vAttributeName);

	//	bool bNeedCompare = true;
	//	CStdString strFeature = _T("");
	//	for (int i = 0; i < vAttributeName.size(); i++)
	//	{
	//		bNeedCompare = true;
	//		strFeature = _T("");
	//		strMatchedValue = _T("");

	//		//strFeature = GetAttributeValue(pElem, _T("html"));  //for test

	//		if (!vAttributeName[i].CompareNoCase(_T("html")))
	//		{
	//			bNeedCompare = false;
	//		}
	//		else if (!vAttributeName[i].CompareNoCase(_T("_RadioCheck")))
	//		{
	//			CStdString strTag = GetTagName(pElem);
	//			CStdString strType = GetAttributeValue(pElem, _T("type"));
	//			if(strTag.CompareNoCase(_T("input")) == 0 && strType.CompareNoCase(_T("radio")) == 0)
	//			{
	//				CComQIPtr<IHTMLElement> pElement = pElem;
	//				CComQIPtr<IHTMLInputElement> QiInputRadio;
	//				HRESULT hr = pElement.QueryInterface(&QiInputRadio);
	//				if(SUCCEEDED(hr))
	//				{
	//					VARIANT_BOOL bCheck;
	//					QiInputRadio->get_checked(&bCheck);

	//					CStdString strTmp = tarItem.GetAttribute(vAttributeName[i]);
	//					if ( (bCheck == VARIANT_TRUE && strTmp[0] == '1') || (bCheck == VARIANT_FALSE && strTmp[0] == '0') )
	//					{
	//						continue;
	//					}
	//					else
	//					{
	//						return false;
	//					}
	//				}
	//				else
	//				{
	//					return false;
	//				}
	//			}
	//			else
	//			{
	//				return false;
	//			}
	//		}
	//		else
	//		{
	//			strFeature = GetAttributeValue(pElem, vAttributeName[i]);

	//			CStdString strTmp = tarItem.GetAttribute(vAttributeName[i]);
	//			if((strTmp == _T("为空")) && (strFeature == _T("")))
	//			{
	//				continue;
	//			}
	//			else if ((strTmp == _T("为空")) && (strFeature != _T("")))
	//			{
	//				return false;
	//			}
	//		}

	//		if (bNeedCompare)
	//		{
	//			//根据与或非结构进行属性匹配
	//			T_FILTER_STRUCTURE tfilter;

	//			TransStucture(tarItem.mapAttributes[vAttributeName[i]], tfilter);
	//			if (!FitFilter(strFeature, tfilter, true, strMatchedValue))
	//				return false;
	//			else if (strMatchedValue != _T(""))
	//			{
	//				//保存匹配成功的值
	//				m_sMatchedLocValue.push_back(strMatchedValue);
	//			}
	//		}
	//	}

	//	return true;
	//}
	std::map<CStdString, CStdString>::iterator iterAttr;
	std::map<CStdString, CStdString>::iterator iterTmpAttr;
	std::map<int, T_ITEM *>::iterator iterItem;
	std::map<int, T_ITEM *>::iterator iterTmpItem;

	//比较属性
	iterAttr = TargetTree->mapAttributes.begin();
	for(; iterAttr != TargetTree->mapAttributes.end(); ++iterAttr)
	{
		CStdString strMatchedValue = _T("");
		T_FILTER_STRUCTURE tfilter;
		bool bIsFit = false;
		
		if(!iterAttr->first.CompareNoCase(_T("html")))
		{
			continue;
		}

		//if (!iterAttr->first.CompareNoCase(_T("_RadioCheck")))//到这里不可能，所有不做处理了
		//{
		//	iterTmpAttr = constructTreeItem->mapAttributes.find(_T("_RadioCheck"));
		//	if(iterTmpAttr == constructTreeItem->mapAttributes.end())
		//	{
		//		return false;
		//	}
		//}

		TransStucture(TargetTree->mapAttributes[iterAttr->first], tfilter);
		iterTmpAttr = constructTreeItem->mapAttributes.begin();
		for(; iterTmpAttr != constructTreeItem->mapAttributes.end(); ++iterTmpAttr)
		{
			if(!iterAttr->second.CompareNoCase(_T("为空")))
			{
				if(iterTmpAttr->second == _T("") && iterAttr->first == iterTmpAttr->first)
				{
					bIsFit = true;
					break;
				}
			}
			else
			{
				if (FitFilter(iterTmpAttr->second, tfilter, true, strMatchedValue))
				{
					bIsFit = true;
					break;
				}
			}
		}

		if(bIsFit == false)//没有找到iterAttr属性的值的匹配项
		{
			return false;
		}
		/*iterTmpAttr = constructTreeItem->mapAttributes.find(iterAttr->first);
		if(iterTmpAttr == constructTreeItem->mapAttributes.end())
		{
		return false;
		}
		else
		{
		if(iterTmpAttr->second == iterAttr->second)
		{
		continue;
		}
		else
		{
		return false;
		}
		}*/
	}

	//比较子item
	iterItem = TargetTree->m_mapNodesAttr.begin();
	for(; iterItem != TargetTree->m_mapNodesAttr.end(); ++iterItem)
	{
		iterTmpItem = constructTreeItem->m_mapNodesAttr.find(iterItem->first);
		if(iterTmpItem == constructTreeItem->m_mapNodesAttr.end())
		{
			return false;
		}
		else
		{
			return IsLevelComfit(iterTmpItem->second, iterItem->second);
		}
	}

	return true;
}

T_ITEM* CHtmElementLocate::ConstructParentTree(IHTMLElement *pElem)//根据当前节点和层级，构造出一棵树，显示所有子节点和信息
{
	IHTMLElement *pTopParentElem = NULL;
	IHTMLElement *pTmpElem = pElem;
	int nLevelTree = m_nLevel;

	//找出顶层的节点
	while(nLevelTree > 0)
	{
		pTmpElem->get_parentElement(&pTopParentElem);
		pTmpElem = pTopParentElem;
		if(pTopParentElem == NULL)//没有那么多层，就不能构造那么多层的树，就是失败的
		{
			return false;
		}
		else
		{
			nLevelTree--;
		}
	}

	T_ITEM *pTopItemInfo = new T_ITEM;//顶层树节点构造
	GetNodesWantedAttributes(pTopParentElem, pTopItemInfo);//获取当前节点的所有属性
	FillParentTree(pTopItemInfo, pTopParentElem);
	return pTopItemInfo;


	//从父节点填充所有的节点树
	//void CHtmElementLocate::FillParentTree(T_ITEM *pItem, CComQIPtr<IHTMLElement> pElem)
	//{
	//	if (NULL == pElem)
	//	{
	//		return;
	//	}

	//	CStdString strText = GetOuterHTML(pElem);

	//	//首先获取父节点的属性
	//	//pTopParentItem = GetWantedAttributes(pElemTopParent);//获取当前节点的所有属性
	//	//GetNodesWantedAttributes(pElemTopParent, pTopParentItem);//获取当前节点的所有属性

	//	//获取父节点的所有孩子
	//	HRESULT hr = 0;
	//	CComDispatchDriver spDirver;
	//	CComQIPtr<IHTMLElementCollection> QiChildColl;
	//	LONG lngTotalChild = 0;
	//	hr = pElem->get_children(&spDirver);
	//	if (FAILED(hr) || NULL == spDirver)
	//	{
	//		return;
	//	}
	//	hr = spDirver->QueryInterface(IID_IHTMLElementCollection, (void**)&QiChildColl);
	//	QiChildColl->get_length(&lngTotalChild);
	//	for (LONG i = 0; i < lngTotalChild; i++)
	//	{
	//		_variant_t vIndex = i;
	//		//_variant_t vIndex1 = 0;
	//		CComDispatchDriver spElem;
	//		hr = QiChildColl->item(vIndex, vIndex, &spElem);
	//		if (FAILED(hr) || NULL == spElem)
	//		{
	//			continue;
	//		}
	//		CComQIPtr<IHTMLElement> QiSubElem;
	//		spElem.QueryInterface(&QiSubElem);

	//		if(NULL == QiSubElem)
	//		{
	//			continue;
	//		}
	//		CStdString strText1 = GetOuterHTML(QiSubElem);

	//		//T_ITEM *pCurParentItem = NULL;
	//		T_ITEM *pItemI = new T_ITEM;
	//		GetNodesWantedAttributes(QiSubElem, pItemI);//获取当前节点的所有属性
	//		pItem->m_mapNodesAttr[i] = pItemI;
	//		FillParentTree(pItemI, QiSubElem);
	//	}
	//}
}

bool CHtmElementLocate::IsParentNodesFit(IHTMLElement *pElem, T_ITEM &tarItem)//所有父节点是否都符合过滤条件
{
	T_ITEM* ConstructTreeItem = NULL;
	T_ITEM* TargetTreeItem = NULL;
	if(tarItem.m_mapNodesAttr.size() > 0)
	{
		TargetTreeItem = tarItem.m_mapNodesAttr[0];
	}

	if(m_nLevel <= 0)//意味着不用匹配树，跟之前没有父节点删除功能一样处理
	{
		return true;
	}

	//构造树
	ConstructTreeItem = ConstructParentTree(pElem);
	if (ConstructTreeItem == NULL)
	{
		return false;
	}

	//匹配树
	return IsLevelComfit(ConstructTreeItem, TargetTreeItem);
}

//bool CHtmElementLocate::IsParentNodesFit(IHTMLElement *pElem, T_ITEM &tarItem, int nFirst)//所有父节点是否都符合过滤条件
//{
//	IHTMLElement* pParentElem = NULL;
//	
//	if (nFirst == 0)//第一次进入，有可能树根本就是空的，
//	{
//		if(tarItem.m_mapNodesAttr.size() == 0)//没有父节点的时候，不用处理，跟平常一样，
//		{
//			return true;
//		}
//	
//		//如果父节点以下有2个或者更多的节点，也不做处理，因为当前节点只有可能有一条路径到达,检查有且仅有一条路径
//		if(false == IsTreeHaveOnlyOneNodePath(&tarItem))
//		{
//			return false;
//		}
//	
//		//做完循环还没返回，证明是正确路径，需要匹配节点,此时只有一条路径
//		//首先获取m_vecLevelItem列,方便做比对，比对当前节点和父节点下的子节点是否是一致的，不一致就不对
//		//这个采的过程中未必采的节点没有子节点，所以一定要比对
//		std::map<int, T_ITEM *>::iterator iter = tarItem.m_mapNodesAttr.begin();
//		while(1)
//		{
//			m_vecLevelItem.push_back(iter->second);
//			if(iter->second->m_mapNodesAttr.size() > 0)
//			{
//				iter = iter->second->m_mapNodesAttr.begin();
//			}
//			else if (0 == iter->second->m_mapNodesAttr.size())
//			{
//				//m_vecLevelItem.push_back(iter->second);
//				break;
//			}
//		}
//	
//		int nCount = nFirst + 1;
//		return IsParentNodesFit(pElem, tarItem, nCount);
//	}
//	else 
//	{
//		int nLevel = m_vecLevelItem.size();
//		T_ITEM *pItem = NULL;
//		if(nLevel < nFirst)//保证m_vecLevelItem不被越界访问
//		{
//			return false;//越界
//		}
//		pItem = m_vecLevelItem[nLevel - nFirst];
//	
//		if (nFirst == 1)//此时需要比对叶子节点是不是和当前节点的属性相同
//		{
//			if(CompareSourceAttrIsRight(&tarItem, pItem) == false)
//			{
//				return false;
//			}
//		}
//	
//		//如果属性全部匹配，说明节点是正确的，继续比较下一个节点
//		//剩下就是要找出节点链是否一致，一致就能返回成功，否则返回失败
//		if(ItemFeatureFit(pElem, *pItem))//如果当前节点和当前的pItem匹配，继续比较路径的父节点
//		{
//			int nCount = nFirst + 1;
//			if (nCount == nLevel)//每次比较以后都要检查是不是到了最后一层了，如果是最后一层的检测，则返回真，整个返回真,否则继续比较下去
//			{
//				return true;
//			}
//			pElem->get_parentElement(&pParentElem);
//			return IsParentNodesFit(pParentElem, tarItem, nCount);
//		}
//		else
//		{
//			return false;
//		}
//	}
//	
//	return true;
//}

bool CHtmElementLocate::ItemFeatureFit(IHTMLElement *pElem, T_ITEM &tarItem, vector<CStdString> &sFindCanDelete)
{
	std::vector<CStdString> vAttributeName;
	tarItem.GetAttruteNames(vAttributeName);

	bool bNeedCompare = true;
	CStdString strFeature = _T("");
	for (int i = 0; i < vAttributeName.size(); i++)
	{
		bNeedCompare = true;
		strFeature = _T("");

		//CStdString strHtml = GetOuterHTML(pElem);
		strFeature = GetAttributeValue(pElem, _T("html"));  //for test

		if (!vAttributeName[i].CompareNoCase(_T("html")))
		{
			bNeedCompare = false;
		}
		else
		{
			strFeature = GetAttributeValue(pElem, vAttributeName[i]);
		}

		if (bNeedCompare)
		{
			//根据与或非结构进行属性匹配
			T_FILTER_STRUCTURE tfilter;
			TransStucture(tarItem.mapAttributes[vAttributeName[i]], tfilter);
			if (!FitFilter(strFeature, tfilter, true, sFindCanDelete))
				return false;
		}
	}

	return true;
}

bool CHtmElementLocate::ItemFeatureFit(IHTMLElement *pElem, T_ITEM &tarItem)
{
	CStdString strMatchedValue = _T("");
	std::vector<CStdString> vAttributeName;
	tarItem.GetAttruteNames(vAttributeName);

	bool bNeedCompare = true;
	CStdString strFeature = _T("");
	for (int i = 0; i < vAttributeName.size(); i++)
	{
		bNeedCompare = true;
		strFeature = _T("");
        strMatchedValue = _T("");

		//strFeature = GetAttributeValue(pElem, _T("html"));  //for test

		if (!vAttributeName[i].CompareNoCase(_T("html")))
		{
			bNeedCompare = false;
		}
		else if (!vAttributeName[i].CompareNoCase(_T("_RadioCheck")))
		{
			if (pElem == NULL)
			{
				return false;
			}
			CStdString strTag = GetTagName(pElem);
			CStdString strType = GetAttributeValue(pElem, _T("type"));
			if(strTag.CompareNoCase(_T("input")) == 0 && strType.CompareNoCase(_T("radio")) == 0)
			{
				CComQIPtr<IHTMLElement> pElement = pElem;
				CComQIPtr<IHTMLInputElement> QiInputRadio;
				HRESULT hr = pElement.QueryInterface(&QiInputRadio);
				if(SUCCEEDED(hr))
				{
					VARIANT_BOOL bCheck;
					QiInputRadio->get_checked(&bCheck);

					CStdString strTmp = tarItem.GetAttribute(vAttributeName[i]);
					if ( (bCheck == VARIANT_TRUE && strTmp[0] == '1') || (bCheck == VARIANT_FALSE && strTmp[0] == '0') )
					{
						continue;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			strFeature = GetAttributeValue(pElem, vAttributeName[i]);

			CStdString strTmp = tarItem.GetAttribute(vAttributeName[i]);
			if((strTmp == _T("为空")) && (strFeature == _T("")))
			{
				continue;
			}
			else if ((strTmp == _T("为空")) && (strFeature != _T("")))
			{
				return false;
			}
		}

		if (bNeedCompare)
		{
			//根据与或非结构进行属性匹配
			T_FILTER_STRUCTURE tfilter;

			TransStucture(tarItem.mapAttributes[vAttributeName[i]], tfilter);
			if (!FitFilter(strFeature, tfilter, true, strMatchedValue))
				return false;
			else if (strMatchedValue != _T(""))
			{
                //保存匹配成功的值
				m_sMatchedLocValue.push_back(strMatchedValue);
			}
		}
	}

	return true;
}

CStdString CHtmElementLocate::RemoveX$XText(CStdString &strText)
{
	CStdString strTarget(_T(""));
	CStdString strMid = strText;

	int kfc  = strText.Find(_T("X$X"));
	while(kfc != -1)//如果文件中没有找到第一个字符串X$X，说明没有隐藏元素的文本了，结束删除,以。。。X$X---X$X。。。。。X$X----X$X。。。为例子
	{
		CStdString strLeft;
		strLeft = strMid.Left(kfc);
		strTarget += strLeft; 

		strMid = strMid.Mid(kfc);
		if (strMid.GetLength() > 3)
		{
			strMid = strMid.Mid(3);//跳过X$X
		}
		else
		{
			strTarget += strMid;
			return strTarget;//长度小于3，说明有左边的X$X，没有右边的，不处理这样的情况
		}
		
		//---X$X。。。。。X$X----X$X。。。
		kfc  = strMid.Find(_T("X$X"));//第二次查找X$X，删除掉中间的部分
		if (kfc != -1)
		{
			strMid = strMid.Mid(kfc + 3);
		}
		else
		{
			strTarget += strMid;
			return strTarget;
		}

		//。。。。。X$X----X$X。。。
		kfc = strMid.Find(_T("X$X"));
	}

	strTarget += strMid;
	strText = strTarget;
	return strTarget;
	//int y = strText.Find(_T("DDDD"));//y == -1
}

//////////////////////////////////////////////////////////////
//在网页源码中查找字符串（遍历Frame）
//参数：pDoc:目标文档指针
//      strText:要查找的字符串（与或非结构表示）
//      strExAppendText:在HTML内容后额外添加的文本，一般为弹框信息
//      strErrInfo:找到的失败字符串
//////////////////////////////////////////////////////////////
int CHtmElementLocate::FindTextInHtmlSrc(CComQIPtr<IHTMLDocument2> pDoc, CStdString strText, CStdString strExAppendText, CStdString &strErrInfo)
{
	T_FILTER_STRUCTURE tfilter;
	CStdString strBodyText = _T("");
	CStdString strTarget = _T("");

	GetBodyText(pDoc, strBodyText, false); 

	strBodyText += strExAppendText;

	strTarget = RemoveX$XText(strBodyText);

	//根据与或非结构进行属性匹配
	TransStucture(strText, tfilter);
 	return FitFilter2(strBodyText, tfilter, true, strErrInfo);
}

//在指定文本中按与或非条件查找字符串
int CHtmElementLocate::FindTextInString(CStdString strSrcString,CStdString strText, CStdString &strErrInfo)
{
	T_FILTER_STRUCTURE tfilter;

	//根据与或非结构进行属性匹配
	TransStucture(strText, tfilter);
 	return FitFilter2(strSrcString, tfilter, true, strErrInfo);
}

//获取BODY的innertext(遍历其中的frame)
void CHtmElementLocate::GetBodyText(CComQIPtr<IHTMLDocument2> pDoc, CStdString &strBodyText, bool bIsGetHideElement)
{	
	if ( !bIsGetHideElement )
	{
		RemoveHideOuttext(pDoc);	
	}
	
	CComQIPtr<IHTMLElement> QiBody;

	//先从页面源码中查找
	if (NULL == pDoc)
		return;

	pDoc->get_body(&QiBody);

	if (QiBody)
	{
		//strBodyText += GetInnerText(QiBody);
		strBodyText += GetOuterText(QiBody);
	}

	//文档中可能包含框架，累加各个框架的文本内容
	GetFrameBodyText(pDoc, _T("frame"), strBodyText);
	GetFrameBodyText(pDoc, _T("iframe"), strBodyText);

	if (!bIsGetHideElement)
	{
		AddHideOuttext(pDoc);	
	}
}

void CHtmElementLocate::AddHideOuttext(CComQIPtr<IHTMLDocument2> pDoc)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if ( NULL == pDoc3 )
		return ;

	pDoc->get_all(&coll);

	HRESULT hr = 0;
	LONG lngCount = 0;
	coll->get_length(&lngCount);

	//逐个处理每个元素 并移除含有隐藏属性的标签元素的outtext
	CStdString strOuttext ;
	for (LONG i = 0; i < lngCount; i++)
	{
		_variant_t vIndex = i;
		CComDispatchDriver spElem;
		hr = coll->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			break;

		CComQIPtr<IHTMLElement> QiElem;
		spElem.QueryInterface(&QiElem);
		std::map<_bstr_t, _bstr_t> mapAttribs;
		GetAllAttributes(QiElem, mapAttribs);
		strOuttext = GetInnerHTML(QiElem);

		std::map<_bstr_t, _bstr_t>::iterator mapIter = mapAttribs.find( _T("style") ) ;

		if ( mapAttribs.end() != mapIter && ( 0==CompareBSTR(mapIter->second, _T("none")) || 0==CompareBSTR(mapIter->second, _T("hidden")) )
			&& !strOuttext.IsEmpty() )
		{
			int num = strOuttext.GetLength();
			CString strX = _T("");
			if(num > 6)
			{
				strX = strOuttext.Mid(3, num - 6);
				QiElem->put_innerHTML(strX.GetBuffer());
			}
			else
			{
				g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------错误strOuttext = %s---------"), strX);
			}
		}
	}
}

void CHtmElementLocate::RemoveHideOuttext(CComQIPtr<IHTMLDocument2> pDoc)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if ( NULL == pDoc3 )
		return ;

	pDoc->get_all(&coll);

	HRESULT hr = 0;
	LONG lngCount = 0;
	coll->get_length(&lngCount);

	//逐个处理每个元素 并移除含有隐藏属性的标签元素的outtext
	CStdString strOuttext ;
	for (LONG i = 0; i < lngCount; i++)
	{
		_variant_t vIndex = i;
		CComDispatchDriver spElem;
		hr = coll->item(vIndex, vIndex, &spElem);
		if (FAILED(hr) || NULL == spElem)
			break;

		CComQIPtr<IHTMLElement> QiElem;
		spElem.QueryInterface(&QiElem);
		std::map<_bstr_t, _bstr_t> mapAttribs;
		GetAllAttributes(QiElem, mapAttribs);

		strOuttext = GetInnerHTML(QiElem);
		_bstr_t strScript;
		QiElem->get_tagName(&strScript.GetBSTR());
		std::map<_bstr_t, _bstr_t>::iterator mapIter = mapAttribs.find( _T("style") ) ;

		if ( mapAttribs.end() != mapIter && ( 0==CompareBSTR(mapIter->second, _T("none")) || 0==CompareBSTR(mapIter->second, _T("hidden")))
			&& !strOuttext.IsEmpty() || 0 == CompareBSTR(strScript, _T("script")) || 0 == CompareBSTR(strScript, _T("SCRIPT")))
		{
			CString strX = _T("X$X") + CString(strOuttext) + _T("X$X");
			QiElem->put_innerHTML(strX.GetBuffer());
		}
	}
}

//在frame中查找字符串
bool CHtmElementLocate::GetFrameBodyText(CComQIPtr<IHTMLDocument2> pDoc, _bstr_t bstrFrameTag, CStdString &strHtmlContent)
{
	LONG lngFrameCount = 0;
	LONG lngFitCount = 0;
	CComQIPtr<IHTMLElementCollection> QiFrameColl;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3)
		return false;

	pDoc3->getElementsByTagName(bstrFrameTag, &QiFrameColl);
	if (NULL == QiFrameColl)
	{
		return false;
	}

	QiFrameColl->get_length(&lngFrameCount);
	for (LONG i = 0; i < lngFrameCount; i++)
	{
		_variant_t vIndex = i;
		CComQIPtr<IHTMLElement> QiFrame;
		CComDispatchDriver spElem;
		QiFrameColl->item(vIndex, vIndex, &spElem);
		HRESULT hr = spElem.QueryInterface(&QiFrame);

		IHTMLDocument2 *pFrameDoc2 = NULL;
		hr = GetFrameDocument2(QiFrame, pFrameDoc2);
		if (NULL != pFrameDoc2)
		{
		    GetBodyText(pFrameDoc2, strHtmlContent);
		}
	}

	return false;
}

LONG CHtmElementLocate::SeekTargetInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl, _bstr_t bstrFrameTag)
{
	LONG lngFrameCount = 0;
	LONG lngFitCount = 0;
	CComQIPtr<IHTMLElementCollection> QiFrameColl;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3)
		return 0;

	pDoc3->getElementsByTagName(bstrFrameTag, &QiFrameColl);
	if(NULL == QiFrameColl)
	{
		g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------QiFrameColl == NULL---------"));
		return 0;
	}
	QiFrameColl->get_length(&lngFrameCount);
	for (LONG i = 0; i < lngFrameCount; i++)
	{
		_variant_t vIndex = i;
		CComQIPtr<IHTMLElement> QiFrame;
		CComDispatchDriver spElem;
		QiFrameColl->item(vIndex, vIndex, &spElem);
		if(NULL == spElem)
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------spElem == NULL---------"));
			//continue;
		}
		HRESULT hr = spElem.QueryInterface(&QiFrame);
		if(NULL == QiFrame)
		{
			g_htmlLocate.Trace(LOGL_TOP,LOGT_ERROR,__TFILE__, __LINE__,_T("---------QiFrame == NULL---------"));
			//continue;
		}
		IHTMLDocument2 *pFrameDoc2 = NULL;
		hr = GetFrameDocument2(QiFrame, pFrameDoc2);
		if (NULL != pFrameDoc2)
		{
			lngFitCount += SeekTargetInDocument(pFrameDoc2, tarItem, qiElemColl);
#ifdef xiaoqiang
			pFrameDoc2->Release();
#endif
			//CComQIPtr<IHTMLDocument2> pTmpDoc = pFrameDoc2;
			//lngFitCount += SeekTargetInDocument(pTmpDoc, tarItem, qiElemColl);
			//pFrameDoc2->Release();
		}
	}

	return lngFitCount;
}

LONG CHtmElementLocate::SeekTargetInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, QI_ELEMENT_COLLECTION &qiElemColl, _bstr_t bstrFrameTag, CComQIPtr<IHTMLElement>& pElemt)
{

	LONG lngFrameCount = 0;
	LONG lngFitCount = 0;
	CComQIPtr<IHTMLElementCollection> QiFrameColl;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;

	if (NULL == pDoc3)
		return 0;

	pDoc3->getElementsByTagName(bstrFrameTag, &QiFrameColl);
	if (NULL == QiFrameColl)
	{
		g_htmlLocate.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("---------QiFrameColl == NULL---------"));
		return 0;
	}
	QiFrameColl->get_length(&lngFrameCount);
	for (LONG i = 0; i < lngFrameCount; i++)
	{
		_variant_t vIndex = i;
		CComQIPtr<IHTMLElement> QiFrame;
		CComDispatchDriver spElem;
		QiFrameColl->item(vIndex, vIndex, &spElem);
		if (NULL == spElem)
		{
			g_htmlLocate.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("---------spElem == NULL---------"));
			//continue;
		}
		HRESULT hr = spElem.QueryInterface(&QiFrame);
		if (NULL == QiFrame)
		{
			g_htmlLocate.Trace(LOGL_TOP, LOGT_ERROR, __TFILE__, __LINE__, _T("---------QiFrame == NULL---------"));
			//continue;
		}
		IHTMLDocument2 *pFrameDoc2 = NULL;
		hr = GetFrameDocument2(QiFrame, pFrameDoc2);
		if (NULL != pFrameDoc2)
		{
			lngFitCount += SeekTargetInDocument(pFrameDoc2, tarItem, qiElemColl);
			
			//找到直接跳出返回frame
			if (lngFitCount > 0)
			{
				pElemt = QiFrame;
			}
#ifdef xiaoqiang
			pFrameDoc2->Release();
#endif
			//CComQIPtr<IHTMLDocument2> pTmpDoc = pFrameDoc2;
			//lngFitCount += SeekTargetInDocument(pTmpDoc, tarItem, qiElemColl);
			//pFrameDoc2->Release();
		}
	}

	return lngFitCount;

}

//从frame的IHTMLElement指针获取IHTMLDocument2
HRESULT CHtmElementLocate::GetFrameDocument(CComQIPtr<IHTMLElement> pFrameElem, IHTMLDocument2 *&pDoc2, CPoint &pt)
{
	CComQIPtr<IHTMLFrameBase2> pFrameBase2;
	HRESULT hr= pFrameElem->QueryInterface(IID_IHTMLFrameBase2,(void**)&pFrameBase2);
	
	/*获得html中鼠标下的元素:
	1、 首先拿到IHTMLDocument2接口
	2、拿到鼠标的位置GetCursorPos(&pt)
	3、IHTMLDocument2的函数elementFromPoint可以拿到位置下的一个IHTMLElement
	4、如果在Frame中可以从IHTMLDocument2拿到IDisplayServices接口
	5、在IDisplayServices接口中TransformPoint可以转换鼠标的相对于frame的值
	*/

	CComQIPtr<IHTMLWindow2> pWindow2;
	hr = pFrameBase2->get_contentWindow(&pWindow2);
	if (SUCCEEDED(hr))
	{		
		IHTMLDocument2 *pFrameDoc = HtmlWindowToHtmlDocument(pWindow2);
  		if (NULL != pFrameDoc)
  		{
  			CComQIPtr<IDisplayServices> dispSer;
			pDoc2->QueryInterface(&dispSer);

			hr = dispSer->TransformPoint(&pt, COORD_SYSTEM_GLOBAL, COORD_SYSTEM_FRAME, pFrameElem);       //页面坐标转换为相对于IFrame的坐标

			//多层IFrame嵌套时 最底层IFrame的转换可能导致坐标为负
			//此时再将坐标转换为相对页面的坐标
			if (pt.x < 0 || pt.y < 0)
				hr = dispSer->TransformPoint(&pt, COORD_SYSTEM_FRAME, COORD_SYSTEM_GLOBAL, pFrameElem);       //转换为相对于IFrame的坐标
		}


		//将IFrame的Document设为当前操作文档
		if (NULL != pFrameDoc) 
		{
			if (NULL != pDoc2)	pDoc2->Release();  //释放旧的文档资源
			pDoc2 = pFrameDoc;
		}
	} 
	
	return hr;
}

//获取相对于Frame的元素位置
HRESULT CHtmElementLocate::GetPostionRelativeToFrame(CComQIPtr<IHTMLDocument2> qiParentDoc, CComQIPtr<IHTMLElement> qiElem, RECT &rt)
{
	HRESULT hr = 0;
  			CComQIPtr<IDisplayServices> dispSer;
			qiParentDoc->QueryInterface(&dispSer);

			hr = dispSer->TransformRect(&rt, COORD_SYSTEM_FRAME, COORD_SYSTEM_GLOBAL, qiElem);       //IFrame转换为页面坐标

			//多层IFrame嵌套时 最底层IFrame的转换可能导致坐标为负
			//此时再将坐标转换为相对页面的坐标
			if (rt.left < 0 || rt.top < 0)
				hr = dispSer->TransformRect(&rt, COORD_SYSTEM_GLOBAL, COORD_SYSTEM_FRAME, qiElem);       //转换为相对于页面的坐标

	return hr;
}

//检查元素是否存在于Frame中
bool CHtmElementLocate::IsElementInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;
	QI_ELEMENT_COLLECTION qiElemColl;
	long lngFitCount = 0;

	//文档中，可能包含框架，继续在框架中搜索
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("frame"));
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("iframe"));

	if (lngFitCount > 0)
	{
		return true;
	}

	return false;

}

bool CHtmElementLocate::IsElementInFrame(CComQIPtr<IHTMLDocument2> pDoc, T_ITEM &tarItem, CComQIPtr<IHTMLElement>& pElemt)
{
	CComQIPtr<IHTMLElementCollection> coll;
	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;
	QI_ELEMENT_COLLECTION qiElemColl;
	long lngFitCount = 0;

	//文档中，可能包含框架，继续在框架中搜索
	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("frame"),pElemt);

	if (lngFitCount > 0)
	{
		return true;
	}

	lngFitCount += SeekTargetInFrame(pDoc, tarItem, qiElemColl, _T("iframe"), pElemt);

	if (lngFitCount > 0)
	{
		return true;
	}

	return false;
}

////尝试从IFrame得到其Document指针
HRESULT CHtmElementLocate::GetFrameDocument2(CComQIPtr<IHTMLElement> pFrameElem, IHTMLDocument2 *&pDoc2)
{
	CComQIPtr<IHTMLFrameBase2> pFrameBase2;
	HRESULT hr= pFrameElem->QueryInterface(IID_IHTMLFrameBase2,(void**)&pFrameBase2);
	
	/*获得html中鼠标下的元素:
	1、 首先拿到IHTMLDocument2接口
	2、拿到鼠标的位置GetCursorPos(&pt)
	3、IHTMLDocument2的函数elementFromPoint可以拿到位置下的一个IHTMLElement
	4、如果在Frame中可以从IHTMLDocument2拿到IDisplayServices接口
	5、在IDisplayServices接口中TransformPoint可以转换鼠标的相对于frame的值
	*/

	CComQIPtr<IHTMLWindow2> pWindow2;
	hr = pFrameBase2->get_contentWindow(&pWindow2);
	if (SUCCEEDED(hr))
	{		
		IHTMLDocument2 *pFrameDoc = HtmlWindowToHtmlDocument(pWindow2);
  		if (NULL == pFrameDoc)
  		{
			hr = S_FALSE;
  		}

		//将IFrame的Document设为当前操作文档
		if (NULL != pFrameDoc) 
		{
			if (NULL != pDoc2)	pDoc2->Release();  //释放旧的文档资源
			pDoc2 = pFrameDoc;
		}
	} 

	return hr;
}

//将输入字符串转换为与或非结构
void CHtmElementLocate::TransStucture(CStdString strInput, T_FILTER_STRUCTURE &tResult)
{
	GetFSData(strInput, MAIN_SPLITOR, SUB_SPLITOR, tResult);
}

//获取定位时匹配成功的值
void CHtmElementLocate::GetMatchedLocValue(vector<CStdString> &vMatchedVal)
{
	vMatchedVal.swap(m_sMatchedLocValue);
}