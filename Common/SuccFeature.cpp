///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：成功标记类实现
// 生成日期：2013.04.19
// 作者：宋亚非
//
// 修改历史：
// 1. 日期：
//    作者：
//    修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SuccFeature.h"

E_FILTER_TYPE CSuccFeature::GetFilterType() const
{
	return m_efilterType ;
}

void CSuccFeature::SetFilterType(E_FILTER_TYPE &filterType)
{
	m_efilterType = filterType ;
}

void CSuccFeature::GetFilter( std::vector<T_FILTER_ITEM> &vecFilters )
{
	switch (m_efilterType)
	{
	case flAnd :
		vecFilters = m_filter.vFilterAnd ;
		break;
	case flNot :
		vecFilters = m_filter.vFilterNot ;
		break;
	case flOr :
		vecFilters = m_filter.vFilterOr ;
		break;
	}
}

void CSuccFeature::SetFilter(const std::vector<T_FILTER_ITEM> &vecFilters, E_FILTER_TYPE filterType)
{
	m_efilterType = filterType ;
	switch (filterType)
	{
	case flAnd :
		m_filter.vFilterAnd = vecFilters ;
		break;
	case flNot :
		m_filter.vFilterNot = vecFilters ;
		break;
	case flOr :
		m_filter.vFilterOr = vecFilters ;
		break;
	}
}

void CSuccFeature::SetOneFilter(T_FILTER_ITEM &tFilterItem , E_FILTER_TYPE filterType)
{
	m_efilterType = filterType ;
	switch (filterType)
	{
	case flAnd :
		m_filter.vFilterAnd.push_back(tFilterItem) ;
		break;
	case	flNot :
		m_filter.vFilterNot.push_back(tFilterItem) ;
		break;
	case flOr :
		m_filter.vFilterOr.push_back(tFilterItem) ;
		break;
	}
}

bool CSuccFeature::RemoveFilterByFeature(CStdString &strFeature )
{
	std::vector<T_FILTER_ITEM> &vFilterItems  = m_filter.vFilterAnd;
	std::vector<T_FILTER_ITEM>::iterator vIter ;
	switch (m_efilterType)
	{
	case flAnd :
		vFilterItems = m_filter.vFilterAnd ;
		break;
	case flNot :
		vFilterItems = m_filter.vFilterNot ;
		break;
	case flOr :
		vFilterItems = m_filter.vFilterOr ;
		break;
	}

	for (vIter=vFilterItems.begin() ; vIter!=vFilterItems.end() ; ++vIter)
	{
		T_FILTER_ITEM tmp = *vIter ;
		if ( vIter->strFeature == strFeature)
		{
			vFilterItems.erase(vIter) ;
			return true ;
		}
	}

	return false ;
}

bool CSuccFeature::RemoveFilterByAttributeName(CStdString &strAttributeName)
{
	std::vector<T_FILTER_ITEM> &vFilterItems  = m_filter.vFilterAnd;
	std::vector<T_FILTER_ITEM>::iterator vIter ;
	switch (m_efilterType)
	{
	case flAnd :
		vFilterItems = m_filter.vFilterAnd ;
		break;
	case flNot :
		vFilterItems = m_filter.vFilterNot ;
		break;
	case flOr :
		vFilterItems = m_filter.vFilterOr ;
		break;
	}

	for (vIter=vFilterItems.begin() ; vIter!=vFilterItems.end() ; ++vIter)
	{
		T_FILTER_ITEM tmp = *vIter ;
		if ( vIter->strAttributeName == strAttributeName)
		{
			vFilterItems.erase(vIter) ;
			return true ;
		}
	}

	return false ;
}

void CSuccFeature::ClearFilter()
{
	m_filter.vFilterAnd.clear() ;
	m_filter.vFilterNot.clear() ;
	m_filter.vFilterOr.clear() ;
}

bool CSuccFeature::CheckFilter()
{

	return false ;
}


CStdString CSuccFeature::TransItemsToSource(T_ITEM *pItem)
{
	CStdString strItem = _T("") ;

	if (pItem)
	{
		strItem += Encode(pItem->strTagName) + _T(",") ;

		if (pItem->mapAttributes.size() == 0)
		{
			strItem += _T(",") ;
		}
		else
		{
			strItem += TransAttributesToSource(pItem->mapAttributes) + _T(",") ;
		}

		if (pItem->pParent)
		{
			strItem += _T(",") ;
		}
		else
		{
			strItem += TransItemsToSource(pItem->pParent) + _T(",") ;
		}

		if (pItem->pBrother)
		{
			 ;
		}
		else
		{
			strItem += TransItemsToSource(pItem->pBrother) ;
		}
	}

	return Encode(strItem) ;
}

void CSuccFeature::ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem)
{
	std::vector<CStdString> vecItemData ;
	SplitStringWithSeparator(strItemData , ENCODE_SEPARATOR , vecItemData) ;

	int i = 0 ;
	pItem->strTagName = Decode(vecItemData[i++]) ;

	//读取mapAttributes begin--
	CStdString strAttributesData = Decode( vecItemData[i++] ) ;
	if ( !strAttributesData.IsEmpty() )
	{
		ExtraAttributesFromSource(strAttributesData , pItem->mapAttributes) ;
	}
	//--end  读取mapAttributes

	CStdString strParentItem = Decode(vecItemData[i++]) ;
	if (strParentItem.IsEmpty())
	{
		pItem->pParent = NULL ;
	}
	else
	{
		pItem->pParent = new T_ITEM ;
		ExtraItemsFromSource(strParentItem , pItem->pParent) ;
	}

	CStdString strBrotherItem = Decode(vecItemData[i++]) ;
	if (strBrotherItem.IsEmpty())
	{
		pItem->pBrother = NULL ;
	}
	else
	{
		pItem->pBrother = new T_ITEM ;
		ExtraItemsFromSource(strBrotherItem , pItem->pBrother) ;
	}
}


CStdString CSuccFeature::TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes)
{
	std::map<CStdString , CStdString>::iterator mapIter ;
	CStdString strAttribute = _T("") ;
	for (mapIter=mapAttributes.begin() ; mapIter!=mapAttributes.end() ; ++mapIter)
	{
		CStdString strNameValue = Encode(mapIter->first) + _T(",") + Encode(mapIter->second) ;
		strAttribute += Encode(strNameValue) + _T(",") ;
	}
	strAttribute = strAttribute.Left(strAttribute.GetLength()-1) ;

	return Encode(strAttribute) ;
}

void CSuccFeature::ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes)
{
	std::vector<CStdString> vecAttributeData ;
	SplitStringWithSeparator(strAttributeData , ENCODE_SEPARATOR  , vecAttributeData) ;

	std::vector<CStdString> strVecKeyorValue ;
	for (int i=0 ; i<vecAttributeData.size() ; ++i)
	{
		SplitStringWithSeparator(Decode(vecAttributeData[i]) , ENCODE_SEPARATOR , strVecKeyorValue) ;
		mapAttributes[Decode(strVecKeyorValue[0])] = Decode(strVecKeyorValue[1]) ;
	}
}

CStdString CSuccFeature::TransFiltersToSource(std::vector<T_FILTER_ITEM> &vecFilters)
{
	CStdString strData = _T("") ;
	for (int i=0 ; i<vecFilters.size() ; ++i)
	{
		CStdString strFilterItem = _T("") ;
		strFilterItem += TransItemsToSource(&vecFilters[i].tItem) + _T(",") ;
		strFilterItem += Encode(vecFilters[i].strAttributeName) + _T(",") ;
		strFilterItem += Encode(vecFilters[i].strFeature) ;

		strData += Encode(strFilterItem) + _T(",") ;
	}
	strData = strData.Left( strData.GetLength()-1 ) ;

	return Encode(strData) ;
}

void CSuccFeature::ExtraFiltersFromSource(CStdString &strSourceData , std::vector<T_FILTER_ITEM> &vecFilters)
{
	std::vector<CStdString> vFiltersData ;
	SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vFiltersData) ;

	for ( int i=0 ; i<vFiltersData.size() ; ++i)
	{
		std::vector<CStdString> vecFilterItem ;
		SplitStringWithSeparator(Decode(vFiltersData[i]) , ENCODE_SEPARATOR , vecFilterItem) ;

		T_FILTER_ITEM tFilterItem ;
		ExtraItemsFromSource(Decode(vecFilterItem[0]) , &tFilterItem.tItem) ;
		tFilterItem.strAttributeName = Decode(vecFilterItem[1]) ;
		tFilterItem.strFeature = Decode(vecFilterItem[2]) ;

		vecFilters.push_back(tFilterItem) ;
	}
}

CStdString CSuccFeature::TransFilterStructToSource()
{
	CStdString strData = _T("") ;

	//封装vFilterAnd --begin
	strData += TransFiltersToSource(m_filter.vFilterAnd) + _T(",") ;
	//--end 封装vFilterAnd

	//封装vFilterNot --begin
	strData += TransFiltersToSource(m_filter.vFilterNot) + _T(",") ;
	//--end 封装vFilterAnd

	//封装vFilterOr --begin
	strData += TransFiltersToSource(m_filter.vFilterOr) ;
	//--end 封装vFilterOr

	return Encode(strData) ;
}

void CSuccFeature::ExtraFilterStructFromSource(CStdString &strSourceData)
{
	std::vector<CStdString> vecFilterStructData ;
	SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vecFilterStructData) ;

	int i = 0 ;
	ExtraFiltersFromSource(Decode(vecFilterStructData[i++]) , m_filter.vFilterAnd) ;
	ExtraFiltersFromSource(Decode(vecFilterStructData[i++]) , m_filter.vFilterNot) ;
	ExtraFiltersFromSource(Decode(vecFilterStructData[i++]) , m_filter.vFilterOr) ;
}
