#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include <vector>

//�ɹ������
class CSuccFeature:public CEnDeCode
{
protected :
	E_FILTER_TYPE m_efilterType ;   //�ɹ���ǵ�����
	T_FILTER_STRUCT m_filter;      //�ɹ����

public:
	E_FILTER_TYPE GetFilterType() const ;
	void SetFilterType(E_FILTER_TYPE &filterType) ;


	void GetFilter( std::vector<T_FILTER_ITEM> &vecFilters) ;
	void SetFilter(const std::vector<T_FILTER_ITEM> &vecFilters , E_FILTER_TYPE filterType);     
	void SetOneFilter(T_FILTER_ITEM &tFilterItem , E_FILTER_TYPE filterType) ;
	
	bool RemoveFilterByFeature(CStdString &strFeature );    //���ݳɹ�����ɾ��һ������
	bool RemoveFilterByAttributeName(CStdString &strAttributeName); //������ȡ�ɹ�������������ɾ��һ������
	void ClearFilter();

	bool CheckFilter();           //���ɹ����

protected:
	//��װT_ITEM
	CStdString TransItemsToSource(T_ITEM *pItem) ;
	//������ݵ�T_ITEM
	void ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem) ;

	//��װT_ITEM�ṹ�е�����mapAttributes
	CStdString TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes) ;
	//���T_ITEM�ṹ�е�����mapAttributes
	void ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes) ;

	CStdString TransFiltersToSource(std::vector<T_FILTER_ITEM> &vecFilters) ;
	void ExtraFiltersFromSource(CStdString &strSourceData , std::vector<T_FILTER_ITEM> &vecFilters) ;

	CStdString TransFilterStructToSource() ;
	void ExtraFilterStructFromSource(CStdString &strSourceData) ;
};