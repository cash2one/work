///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：网站数据类实现
// 生成日期：2013.04.18
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
#include "StationData.h"

void CStationData::Clear()
{
	m_strNsHost.Empty() ;
	m_strNsName.Empty() ;
	m_dwNsCode = -1 ;

	m_mapsfTask.clear() ;
	
	std::map<E_TaskType , CTaskData*>::iterator mapIter ;
	for ( mapIter=m_mapTaskData.begin() ; mapIter!=m_mapTaskData.end() ; ++mapIter)
	{
		if (mapIter->second)
		{
			delete mapIter->second ;
			mapIter->second = NULL ;
		}
	}
	m_mapTaskData.clear() ;

	m_bIsNoImage = FALSE ;
	m_dwViewCount = 2 ;
}

int CStationData::GetTaskCount()
{
	return m_mapTaskData.size() ;
}

DWORD CStationData::GetNsCode() const
{
	return m_dwNsCode ;
}

void CStationData::SetNsCode(DWORD &dwNsCode)
{
	m_dwNsCode = dwNsCode ;
}

CStdString CStationData::GetNsName() const
{
	return m_strNsName ;
}

void CStationData::SetNsName(const CStdString &strNsName)
{
	m_strNsName = strNsName ;
}

CStdString CStationData::GetNsHost() const 
{
	return m_strNsHost ;
}

void CStationData::SetNsHost(const CStdString &strNsHost)
{
	m_strNsHost = strNsHost ;
}

BOOL CStationData::GetIsNoImages()
{
	return m_bIsNoImage ;
}

void CStationData::SetIsNoImages(BOOL bIsNoImages)
{
	m_bIsNoImage = bIsNoImages ;
}

DWORD CStationData::GetViewCount()
{
	return m_dwViewCount ;
}

void CStationData::SetViewCount(DWORD dwViewCount)
{
	m_dwViewCount = dwViewCount ;
}

void CStationData::AddTaskData(E_TaskType ttType , CTaskData *pTaskData)
{
	if (!pTaskData)
	{
		return ;
	}

	m_mapTaskData[ttType] = new CTaskData ;
	m_mapTaskData[ttType]->CopyByPointer(pTaskData) ;
}

void CStationData::DelTaskData(E_TaskType ttType)
{
	std::map<E_TaskType , CTaskData*>::iterator mapIter ;
	mapIter = m_mapTaskData.find(ttType) ;
	if ( mapIter != m_mapTaskData.end() )
	{
		m_mapTaskData.erase(mapIter) ;
	}
}

CTaskData* CStationData::GetTaskData(E_TaskType ttType)
{
	std::map<E_TaskType , CTaskData*>::iterator mapIter ;
	mapIter = m_mapTaskData.find(ttType) ;
	if ( mapIter != m_mapTaskData.end() )
	{
		return mapIter->second ;
	}

	return NULL ;	
}

void CStationData::GetTaskData(int nIndex , CTaskData &taskData)
{
	if ( nIndex >= m_mapTaskData.size() )
	{
		return ;
	}

	std::map<E_TaskType , CTaskData*>::iterator mapIter ;
	mapIter = m_mapTaskData.begin() ;
	while(nIndex--)
	{
		mapIter ++ ;
	}
	if (mapIter != m_mapTaskData.end() )
	{
		taskData.CopyByPointer(mapIter->second) ;
	}

	return ;
}

void CStationData::SetFunctionRoute(E_StationFunction sfVal, CStdString strTask)
{
	m_mapsfTask[sfVal] = strTask ;
}

CStdString CStationData::GetFunctionRoute(E_StationFunction sfVal)
{
	return m_mapsfTask[sfVal] ;
}

void CStationData::GetAllFunction(std::vector<E_StationFunction> &vecAllFunc)
{
	vecAllFunc.clear() ;
	std::map<E_StationFunction , CStdString>::iterator mapIter ;
	for (mapIter=m_mapsfTask.begin() ; mapIter!=m_mapsfTask.end() ; ++mapIter)
	{
		vecAllFunc.push_back(mapIter->first) ;
	}
}

//void CStationData::GetEmbedInfo(E_StationFunction eFunc, std::vector<T_EmbedItemData> &vecEmbedItem)//特定function下的嵌入网站数据要执行的task，没有就为空
//{
//	vecEmbedItem.clear();
//	std::map<E_StationFunction, CStdString>::iterator mapIter;
//	mapIter = m_mapsfTask.find(eFunc);
//	if(mapIter != m_mapsfTask.end())
//	{
//		CStdString strTmp = mapIter->second;
//		std::vector<CStdString> vecTaskDataIndex;
//		SplitStringWithSeparator(strTmp, _T("/"), vecTaskDataIndex);
//		for (int i = 0; i < vecTaskDataIndex.size(); ++i)
//		{
//			CStdString strIndex = vecTaskDataIndex[i];
//			int nIndex = _ttoi(vecTaskDataIndex[i]);
//			if (nIndex == (int)ttDataStationFunction)
//			{
//				std::map<E_TaskType, CTaskData*>::iterator iter0 = m_mapTaskData.find(ttDataStationFunction);
//				if(iter0 != m_mapTaskData.end())
//				{
//					vecEmbedItem = m_mapTaskData[ttDataStationFunction]->m_vecEmbedItem;
//				}
//				break;
//			}
//		}
//	}
//}

CStdString CStationData::GetAllEmbedWedID()
{
	CStdString strRet = _T("");
	std::map<E_TaskType, CTaskData*>::iterator iter0 = m_mapTaskData.find(ttDataStationFunction);
	if(iter0 != m_mapTaskData.end())
	{
		CTaskData* task = iter0->second;
		std::vector<T_EmbedItemData> vecEmbed = task->m_vecEmbedItem;
		for(int i = 0; i < vecEmbed.size(); i++)
		{
			if (0 == i)
			{
				strRet =  vecEmbed[i].strWebId;
			}
			else if(-1 == strRet.Find(vecEmbed[i].strWebId))
			{
				strRet = strRet + _T(",") + vecEmbed[i].strWebId;
			}
		}
	}
	return strRet;
}

void CStationData::DeleteFuntion(E_StationFunction sfVal)
{
	std::map<E_StationFunction , CStdString>::iterator mapIter ;
	mapIter = m_mapsfTask.find(sfVal) ;
	if ( mapIter != m_mapsfTask.end() )
	{
		m_mapsfTask.erase( mapIter ) ;
	}
}

CStdString CStationData::GetSource()
{
	CStdString strData = TransTargetToSource() ;
	return Encode(strData) ;
}

void CStationData::SetSource(CStdString &strNsData)
{
	Clear() ;

	CStdString strData = Decode(strNsData) ;
	ExtraTargetFromSource(strData) ;
}

bool CStationData::LoadData(E_StationFunction sfVal, CFunctionData &cfData)
{
	CStdString strTaskIndex = m_mapsfTask[sfVal] ;

	if (strTaskIndex.IsEmpty())
	{
		return false ;
	}

	cfData.clear() ;
	std::vector<CStdString> vecTaskDataIndex ;
	SplitStringWithSeparator(strTaskIndex , _T("/") , vecTaskDataIndex) ;

	for (int i=0 ; i<vecTaskDataIndex.size() ; ++i)
	{
		CStdString strIndex = vecTaskDataIndex[i] ;
		int nIndex = _ttoi(vecTaskDataIndex[i]) ;
		if ( strIndex!=_T("0") && nIndex==0)
		{
			return false ;
		}
		
		cfData.AddObject( *m_mapTaskData[(E_TaskType)nIndex] ) ;
	}

	return true ;
}

//private

CStdString CStationData::TransTargetToSource()
{
	CStdString strData = _T("") ;

	CStdString strNsCode = _T("") ;
	strNsCode.Format(_T("%d") , m_dwNsCode) ;
	strData += Encode(strNsCode) + _T(",") ;

	strData += Encode(m_strNsName) + _T(",") ;
	strData += Encode(m_strNsHost) + _T(",") ;

	//封装任务数据 begin--
	CStdString strTaskData = _T("") ;
	std::map<E_TaskType , CTaskData*>::iterator mapIter ;
	for ( mapIter=m_mapTaskData.begin() ; mapIter!=m_mapTaskData.end() ; ++mapIter)
	{
		CStdString strTmp;
		strTmp.Format(_T("%d") , mapIter->first) ;
		CStdString strKeyData = Encode( strTmp) ;

		CTaskData *pTaskData = mapIter->second ;
		CStdString strValueData = _T("") ;
		if (pTaskData)
		{
			strValueData = pTaskData->GetSource() ;
		}
		
		CStdString strKeyValueData = Encode(strKeyData + _T(",") + strValueData) ;
		strTaskData += strKeyValueData + _T(",") ;
	}
	if ( !strTaskData.IsEmpty() )
	{
		strTaskData = strTaskData.Left(strTaskData.GetLength()-1) ;
	}
	strData += Encode(strTaskData) + _T(",") ;
	//--end 封装任务数据

	//封装功能与任务的对应列表 begin--
	CStdString strVSFTaskData = _T("") ;
	std::map<E_StationFunction , CStdString>::iterator mapIter2 ;
	for (mapIter2=m_mapsfTask.begin() ; mapIter2!=m_mapsfTask.end() ; ++mapIter2)
	{
		CStdString strEStationFunciton = _T("") ;
		strEStationFunciton.Format( _T("%d") , mapIter2->first) ;

		CStdString strKeyValueData = Encode(strEStationFunciton) + _T(",") + Encode(mapIter2->second) ;

		strVSFTaskData += Encode(strKeyValueData) + _T(",") ;
	}
	if ( !strVSFTaskData.IsEmpty() )
	{
		strVSFTaskData = strVSFTaskData.Left(strVSFTaskData.GetLength()-1) ;
	}
	strData += Encode(strVSFTaskData) + _T(",");
	//--end 封装功能与任务的对应列表

	CStdString strIsNoImages ;
	strIsNoImages.Format(_T("%d") , m_bIsNoImage) ;
	strData += Encode(strIsNoImages) + _T(",") ;

	CStdString strViewCount ;
	strViewCount.Format( _T("%d") , m_dwViewCount) ;
	strData += Encode(strViewCount) ;

	return strData ;
}

void CStationData::ExtraTargetFromSource(CStdString &strSourceData)
{
	std::vector<CStdString> vecStationData ;
	CEnDeCode::SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vecStationData) ;

	int i = 0 ;
	int nSize = vecStationData.size() ;
	if ( i < nSize )
	{
		m_dwNsCode = _ttoi( Decode(vecStationData[i++]) ) ;
	}
	if ( i < nSize )
	{
		m_strNsName = Decode(vecStationData[i++]) ;
	}
	if ( i < nSize )
	{
		m_strNsHost = Decode(vecStationData[i++]) ;
	}
	

	//解封任务数据 begin --
	if ( i<nSize )
	{
		CStdString strTaskData = Decode( vecStationData[i++] ) ;
		std::vector<CStdString> vecTaskData ;
		SplitStringWithSeparator(strTaskData , ENCODE_SEPARATOR , vecTaskData) ;
		for (int j=0 ; j<vecTaskData.size() ; ++j)
		{
			std::vector<CStdString> vecKeyValueData ;
			SplitStringWithSeparator(Decode(vecTaskData[j]) , ENCODE_SEPARATOR , vecKeyValueData) ;

			if (vecKeyValueData.size() != 2 )
			{
				continue; 
			}
			E_TaskType eTT = (E_TaskType)_ttoi( Decode(vecKeyValueData[0]) ) ;
			CTaskData *TDTmp = new CTaskData ;
			TDTmp->SetSource( vecKeyValueData[1] ) ;
			m_mapTaskData[eTT] = TDTmp ;
		}
	}
	//--end 解封到任务数据

	//解封网站功能与任务的对应列表 begin --
	if ( i<nSize )
	{
		CStdString strSFTaskData = Decode( vecStationData[i++] ) ;
		std::vector<CStdString> vecSFTaskData ;
		CEnDeCode::SplitStringWithSeparator(strSFTaskData , ENCODE_SEPARATOR , vecSFTaskData) ;
		for (int j=0 ; j<vecSFTaskData.size() ; ++j)
		{
			std::vector<CStdString> vecKeyValue ;
			SplitStringWithSeparator(Decode(vecSFTaskData[j]) , ENCODE_SEPARATOR , vecKeyValue) ;

			if (vecKeyValue.size() != 2)
			{
				continue ;
			}

			E_StationFunction EFTmp = (E_StationFunction)_ttoi(Decode(vecKeyValue[0])) ;
			m_mapsfTask[EFTmp] = Decode( vecKeyValue[1] ) ;
		}
	}
	//--end 解封网站功能与任务的对应列表

	if ( i<nSize )
	{
		m_bIsNoImage = _ttoi( Decode(vecStationData[i++]) ) ;
	}

	if ( i<nSize )
	{
		m_dwViewCount = _ttoi( Decode(vecStationData[i++]) ) ;
	}
}

std::map<E_TaskType, CTaskData*>* CStationData::get_MapTaskData()
{
	return &m_mapTaskData;
}