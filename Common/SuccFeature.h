#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include <vector>

//成功标记类
class CSuccFeature:public CEnDeCode
{
protected :
	E_FILTER_TYPE m_efilterType ;   //成功标记的类型
	T_FILTER_STRUCT m_filter;      //成功标记

public:
	E_FILTER_TYPE GetFilterType() const ;
	void SetFilterType(E_FILTER_TYPE &filterType) ;


	void GetFilter( std::vector<T_FILTER_ITEM> &vecFilters) ;
	void SetFilter(const std::vector<T_FILTER_ITEM> &vecFilters , E_FILTER_TYPE filterType);     
	void SetOneFilter(T_FILTER_ITEM &tFilterItem , E_FILTER_TYPE filterType) ;
	
	bool RemoveFilterByFeature(CStdString &strFeature );    //根据成功特征删除一个特征
	bool RemoveFilterByAttributeName(CStdString &strAttributeName); //根据提取成功特征的属性名删除一个特征
	void ClearFilter();

	bool CheckFilter();           //检查成功标记

protected:
	//封装T_ITEM
	CStdString TransItemsToSource(T_ITEM *pItem) ;
	//解封数据到T_ITEM
	void ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem) ;

	//封装T_ITEM结构中的属性mapAttributes
	CStdString TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes) ;
	//解封T_ITEM结构中的属性mapAttributes
	void ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes) ;

	CStdString TransFiltersToSource(std::vector<T_FILTER_ITEM> &vecFilters) ;
	void ExtraFiltersFromSource(CStdString &strSourceData , std::vector<T_FILTER_ITEM> &vecFilters) ;

	CStdString TransFilterStructToSource() ;
	void ExtraFilterStructFromSource(CStdString &strSourceData) ;
};