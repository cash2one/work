///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：任务数据类实现
// 生成日期：2013.04.16
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
#include "TaskData.h"

CTaskData::CTaskData()
{
	Clear() ;
}

void CTaskData::SetEmbedWebFlag(bool flag)
{
	m_bIsEmbedWeb = flag;
}

bool CTaskData::GetIsEmbedWeb()
{
	return m_bIsEmbedWeb;
}

void CTaskData::CopyByPointer(CTaskData *pTaskData)
{
	m_vobjBlockData = pTaskData->m_vobjBlockData ;
	m_lngActiveObject = pTaskData->m_lngActiveObject ;
	m_ttTaskType = pTaskData->m_ttTaskType ;
	m_lngPerWait = pTaskData->m_lngPerWait ;
	m_bFaultTolerant = pTaskData->m_bFaultTolerant ;
	m_mapTaskFilters = pTaskData->m_mapTaskFilters ;
	m_bIsEmbedWeb = pTaskData->m_bIsEmbedWeb;
	m_vecEmbedItem = pTaskData->m_vecEmbedItem;
}

void CTaskData::Clear()
{
	m_vobjBlockData.clear() ;

	m_lngPerWait = 0 ;
	m_bFaultTolerant = false ;
	m_lngActiveObject = -1 ;
	m_ttTaskType = ttDataEmpty ;
	m_bIsEmbedWeb = false;
	m_mapTaskFilters.clear() ;
	m_vecEmbedItem.clear();
}

void CTaskData::AddObject(CBlockData &obj)
{
	m_vobjBlockData.push_back(obj) ;
}

void CTaskData::DelObject()
{
	if ( m_lngActiveObject >= m_vobjBlockData.size() )
	{
		return ;
	}

	/*int countStep = m_vobjBlockData[m_lngActiveObject].GetCount();
	for(int i = 0; i < countStep; i++)
	{
	CStepData SubStep;
	m_vobjBlockData[m_lngActiveObject].GetStep(i, SubStep);
	m_vobjBlockData[m_lngActiveObject].DeleteMotherStep(SubStep);
	}*/

	m_vobjBlockData.erase(m_vobjBlockData.begin()+m_lngActiveObject) ;
}

bool CTaskData::GetCurObject(CBlockData &obj)
{
	if (m_lngActiveObject >= m_vobjBlockData.size() )
	{
		return false ;
	}

	obj = m_vobjBlockData[m_lngActiveObject] ;
	return true ;
}

CBlockData *CTaskData::GetCurObject()
{
	if ( m_lngActiveObject >= m_vobjBlockData.size() )
	{
		return NULL ;
	}

	return &m_vobjBlockData[m_lngActiveObject] ;
}

//交换两个数据块的位置
void CTaskData::SwapBlockData(int nObjIndex1 , int nObjIndex2)
{
	if (nObjIndex1>=m_vobjBlockData.size() || nObjIndex2>=m_vobjBlockData.size() || nObjIndex1==nObjIndex2)
	{
		return ;
	}

	int nLarge , nSmall ;
	nLarge = nSmall = -1 ;

	nLarge = nObjIndex1 > nObjIndex2 ? nObjIndex1 : nObjIndex2 ;
	nSmall = nObjIndex1 > nObjIndex2 ? nObjIndex2 : nObjIndex1 ;

	CBlockData BDTmp = m_vobjBlockData[nLarge] ;
	m_vobjBlockData.erase(m_vobjBlockData.begin()+nLarge) ;
	m_vobjBlockData.insert(m_vobjBlockData.begin()+nSmall, BDTmp) ;
}

int CTaskData::GetCount()
{
	return m_vobjBlockData.size() ;
}

E_TaskType CTaskData::GetType() const
{
	return m_ttTaskType ;
}

void CTaskData::SetType(E_TaskType pType)
{
	m_ttTaskType = pType ;
}

bool CTaskData::GetFaultTolerant() const
{
	return m_bFaultTolerant ;
}

void CTaskData::SetFaultTolerant( bool &bFaultTolerant)
{
	m_bFaultTolerant = bFaultTolerant ;
}

long CTaskData::GetActiveIndex() const
{
	return m_lngActiveObject ;
}

void CTaskData::SetActiveIndex(long lIndex)
{
	m_lngActiveObject = lIndex ;
}

long CTaskData::GetPerWait() const
{
	return m_lngPerWait ;
}

void CTaskData::SetPerWait(long lngWait) 
{
	m_lngPerWait = lngWait ;
}

void CTaskData::GetFilterStructure(std::map<E_RESULT_FILTER_TYPE , T_FILTER_STRUCTURE> &mapFilterStruct)
{
	mapFilterStruct = m_mapTaskFilters ;
}

T_FILTER_STRUCTURE *CTaskData::GetFilterStructure(E_RESULT_FILTER_TYPE eType)
{
	return &m_mapTaskFilters[eType] ;
}

void CTaskData::SetFilterStructure(std::map<E_RESULT_FILTER_TYPE , T_FILTER_STRUCTURE> &mapFilterStruct )
{
	m_mapTaskFilters = mapFilterStruct ;
}

void CTaskData::SetFilterStructure(E_RESULT_FILTER_TYPE eType , T_FILTER_STRUCTURE &filterStructure)
{
	m_mapTaskFilters[eType] = filterStructure ;
}

void CTaskData::DelFilterStructure(E_RESULT_FILTER_TYPE eType)
{
	std::map<E_RESULT_FILTER_TYPE , T_FILTER_STRUCTURE>::iterator mapIter ;
	mapIter = m_mapTaskFilters.find(eType) ;

	if (mapIter != m_mapTaskFilters.end())
	{
		m_mapTaskFilters.erase(mapIter) ;
	}
}

void CTaskData::MoveForward()
{
	m_lngActiveObject -= 1 ;
}

void CTaskData::MoveNext()
{
	m_lngActiveObject += 1 ;
}

CStdString CTaskData::GetSource() 
{
	CStdString strSource = TransTaskDataToSource() ;
	return Encode(strSource) ;
}

void CTaskData::SetSource(CStdString &strSource)
{
	Clear() ;
	CStdString strData = Decode(strSource) ;
	ExtraTaskDataFromSource(strData) ;
}

//private
CStdString CTaskData::TransTaskDataToSource()
{
	CStdString strData = _T("") ;

	//执行数据块的封装 --begin
	CStdString strBlockData = _T("") ;
	for (int i=0 ; i<m_vobjBlockData.size() ; ++i)
	{
		strBlockData += m_vobjBlockData[i].GetSource() + _T(",") ;
	}
	if ( !strBlockData.IsEmpty() )
	{
		strBlockData = strBlockData.Left(strBlockData.GetLength()-1) ;
	}
	strData += Encode(strBlockData) + _T(",") ;
	//--end 执行数据块的封装

	CStdString strActiveObject = _T("") ;
	strActiveObject.Format( _T("%ld") , m_lngActiveObject) ;
	strData += Encode(strActiveObject) + _T(",") ;

	CStdString strTaskType = _T("") ;
	strTaskType.Format(_T("%d") , m_ttTaskType) ;
	strData += Encode(strTaskType) + _T(",") ;

	CStdString strPerWait = _T("") ;
	strPerWait.Format(_T("%ld") , m_lngPerWait) ;
	strData += Encode(strPerWait) + _T(",") ;

	CStdString strFaultTolerant = _T("") ;
	strFaultTolerant.Format(_T("%d") , m_bFaultTolerant) ;
	strData += Encode(strFaultTolerant) + _T(",") ;

	strData += TransTaskFiltersToSource(m_mapTaskFilters) + _T(",");

	CStdString strIsEmbedWeb = _T("") ;
	strIsEmbedWeb.Format(_T("%ld"), m_bIsEmbedWeb);
	strData += Encode(strIsEmbedWeb) + _T(",") ;

	//m_vecEmbedItem的封装 --begin
	CStdString strEmbedData = _T("") ;
	for (int i = 0; i < m_vecEmbedItem.size(); ++i)
	{
		strEmbedData += m_vecEmbedItem[i].GetSource() + _T(",");
	}
	if (!strEmbedData.IsEmpty())
	{
		strEmbedData = strEmbedData.Left(strEmbedData.GetLength() - 1);
	}
	strData += Encode(strEmbedData);

	return strData ;
}

void CTaskData::ExtraTaskDataFromSource(CStdString strData)
{
	std::vector<CStdString> vecTaskData ;
	int nSize = SplitStringWithSeparator(strData , ENCODE_SEPARATOR , vecTaskData) ;

	int i = 0 ; 
	//解封执行数据块数据   begin--
	if ( i<nSize )
	{
		std::vector<CStdString> vecBlockData ;
		SplitStringWithSeparator( Decode(vecTaskData[i++]) , ENCODE_SEPARATOR , vecBlockData ) ;
		for (int j=0 ; j<vecBlockData.size() ; ++j)
		{
			CBlockData BDTmp ;
			BDTmp.SetSource(vecBlockData[j]) ;
			m_vobjBlockData.push_back(BDTmp) ;
		}
	}
	//--end 解封执行数据块数据
	if ( i< nSize )
	{
		m_lngActiveObject = _ttol( Decode(vecTaskData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_ttTaskType = (E_TaskType)_ttoi( Decode(vecTaskData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_lngPerWait = _ttol( Decode(vecTaskData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_bFaultTolerant = _ttoi( Decode(vecTaskData[i++]) ) ;
	}
	
	//解封m_mapTaskFilters  begin--
	if ( i<nSize )
	{
		ExtraTaskFiltersFromSource(Decode(vecTaskData[i++]) , m_mapTaskFilters) ;
	}
	//--end 解封m_mapTaskFilters

	if ( i<nSize )
	{
		m_bIsEmbedWeb = (bool)_ttoi(Decode(vecTaskData[i++]));
	}

	if ( i<nSize )
	{
		std::vector<CStdString> vecEmbedData ;
		SplitStringWithSeparator(Decode(vecTaskData[i++]), ENCODE_SEPARATOR, vecEmbedData);
		for (int j = 0; j < vecEmbedData.size(); ++j)
		{
			T_EmbedItemData EmbedItem;
			EmbedItem.SetSource(vecEmbedData[j]);
			m_vecEmbedItem.push_back(EmbedItem);
		}
	}
}

CStdString CTaskData::TransTaskFiltersToSource(std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> &mapTaskFilters)
{
	CStdString strData = _T("") ;
	std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE>::iterator mapIter ;

	for (mapIter=mapTaskFilters.begin() ; mapIter!=mapTaskFilters.end() ; ++mapIter)
	{
		CStdString strFilterType = _T("") ;
		strFilterType.Format(_T("%d") , mapIter->first) ;

		CStdString strFilterStructure = TransFilterStructureToSource(mapIter->second) ;

		CStdString strMapData = Encode(strFilterType) + _T(",") + strFilterStructure ;
		strData += Encode(strMapData) + _T(",") ;
	}
	if ( !strData.IsEmpty() )
	{
		strData = strData.Left(strData.GetLength()-1) ;
	}

	return Encode(strData) ;
}

void CTaskData::ExtraTaskFiltersFromSource(CStdString &strSourceData , std::map<E_RESULT_FILTER_TYPE, T_FILTER_STRUCTURE> &mapTaskFilters)
{
	std::vector<CStdString> vecTaskFilter ;
	SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vecTaskFilter) ;

	for ( int i=0 ; i<vecTaskFilter.size() ; ++i)
	{
		std::vector<CStdString> vecKeyValue ;
		int nSize = SplitStringWithSeparator(Decode(vecTaskFilter[i]) , ENCODE_SEPARATOR , vecKeyValue) ;
		if ( nSize != 2)
		{
			continue; 
		}

		E_RESULT_FILTER_TYPE eFilterTypeTmp = (E_RESULT_FILTER_TYPE)_ttoi( Decode(vecKeyValue[0]) ) ;

		T_FILTER_STRUCTURE tFilterStructureTmp ;
		ExtraFilterStructureFromSource(Decode(vecKeyValue[1]) , tFilterStructureTmp) ;

		m_mapTaskFilters[eFilterTypeTmp] = tFilterStructureTmp ;
	}

}

CStdString CTaskData::TransFilterStructureToSource(T_FILTER_STRUCTURE &tFilterStructure)
{
	CStdString strData = _T("") ;

	CStdString strAndData = _T("") ;
	for (int i=0 ; i<tFilterStructure.straAnd.size() ; ++i)
	{
		strAndData += Encode(tFilterStructure.straAnd[i]) + _T(",") ;
	}
	if ( !strAndData.IsEmpty())
	{
		strAndData = strAndData.Left( strAndData.GetLength()-1 ) ;
	}

	CStdString strNotData = _T("") ;
	for (int i=0 ; i<tFilterStructure.straNot.size() ; ++i)
	{
		strNotData +=  Encode(tFilterStructure.straNot[i]) + _T(",") ;
	}
	if ( !strNotData.IsEmpty())
	{
		strNotData = strNotData.Left(strNotData.GetLength()-1) ;
	}

	CStdString strOrData = _T("") ;
	for (int i=0 ; i<tFilterStructure.straOr.size() ; ++i)
	{
		strOrData += Encode( tFilterStructure.straOr[i] ) + _T(",") ;
	}
	if (!strOrData.IsEmpty())
	{
		strOrData = strOrData.Left( strOrData.GetLength()-1 ) ;
	}

	strData = Encode(strAndData) + _T(",") + Encode(strNotData) + _T(",") + Encode(strOrData) ;
	
	return Encode(strData) ;
}

void CTaskData::ExtraFilterStructureFromSource(CStdString &strSourceData , T_FILTER_STRUCTURE &tFilterStructure)
{
	std::vector<CStdString> vecFilterStructure ;
	int nSize = SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vecFilterStructure) ;
	if ( nSize != 3 )
	{
		return ;
	}

	SplitStringWithSeparator(Decode(vecFilterStructure[0]) , ENCODE_SEPARATOR , tFilterStructure.straAnd) ;
	for (int i=0 ; i<tFilterStructure.straAnd.size() ; ++i)
	{
		tFilterStructure.straAnd[i] = Decode(tFilterStructure.straAnd[i]) ;
	}

	SplitStringWithSeparator(Decode(vecFilterStructure[1]) , ENCODE_SEPARATOR , tFilterStructure.straNot) ;
	for ( int i=0 ; i<tFilterStructure.straNot.size() ; ++i)
	{
		tFilterStructure.straNot[i] = Decode(tFilterStructure.straNot[i]) ;
	}

	SplitStringWithSeparator(Decode(vecFilterStructure[2]) , ENCODE_SEPARATOR , tFilterStructure.straOr) ;
	for (int i=0 ; i<tFilterStructure.straOr.size() ; ++i)
	{
		tFilterStructure.straOr[i] = Decode(tFilterStructure.straOr[i]) ;
	}
}