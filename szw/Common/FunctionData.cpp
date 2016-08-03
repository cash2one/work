///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：网站数据类实现
// 生成日期：2013.04.17
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
#include "FunctionData.h"

void CFunctionData::clear()
{
	m_vobjTaskData.clear() ;
	m_lngActiveObject = 0 ;
}

void CFunctionData::AddObject(CTaskData &obj)
{
	m_vobjTaskData.push_back(obj) ;
}

void CFunctionData::DelObject()
{
	if ( m_lngActiveObject >= m_vobjTaskData.size() )
	{
		return ;
	}
	m_vobjTaskData.erase(m_vobjTaskData.begin()+m_lngActiveObject) ;
}

void CFunctionData::GetCurTask(CTaskData &obj)
{
	if ( m_lngActiveObject >= m_vobjTaskData.size() )
	{
		return ;
	}
	obj = m_vobjTaskData[m_lngActiveObject] ;
}

int CFunctionData::GetCount()
{
	return m_vobjTaskData.size() ;
}

long CFunctionData::GetActiveIndex() const
{
	return m_lngActiveObject ;
}

void CFunctionData::SetActiveIndex(long lIndex)
{
	m_lngActiveObject = lIndex ;
}

E_TaskType CFunctionData::GetCurTaskType() const
{
	if ( m_lngActiveObject >= m_vobjTaskData.size() )
	{
		return ttDataNull ;
	}
	CTaskData taskData = m_vobjTaskData[m_lngActiveObject] ;
	return taskData.GetType() ;
}

bool CFunctionData::GetEmbedInfo(E_StationFunction eFunc, T_EmbedItemData &EmbedItem, const CStdString& strWebID)//特定function下的嵌入网站数据要执行的task，没有就为空
{
	for(int i = 0; i < m_vobjTaskData.size(); i++)
	{
		if(m_vobjTaskData[i].m_vecEmbedItem.size() > 0)
		{
			std::vector<T_EmbedItemData> vecEmbedItem = m_vobjTaskData[i].m_vecEmbedItem;
			for(int j = 0; j < vecEmbedItem.size(); j++)
			{
				if (vecEmbedItem[j].eFunction == eFunc && !vecEmbedItem[j].strWebId.CompareNoCase(strWebID))//找出需要执行的task替换嵌入数据块功能中的模块
				{
					EmbedItem = vecEmbedItem[j];
					return true;
				}
			}
		}
	}
	return false;
}

CStdString CFunctionData::GetSource()
{
	CStdString strData = TransTasksToSource();
	return Encode(strData) ;
}

void CFunctionData::SetSource(CStdString &strSourceData)
{
	clear() ;

	CStdString strData = Decode(strSourceData) ;
	ExtraTasksFromSource(strData) ;
}

//private

CStdString CFunctionData::TransTasksToSource()
{
	CStdString strData = _T("") ;

	//封装任务数据 --begin
	CStdString strTaskData = _T("") ;
	for ( int i=0 ; i<m_vobjTaskData.size() ; ++i)
	{
		strTaskData += m_vobjTaskData[i].GetSource() + _T(",") ;
	}
	if ( !strTaskData.IsEmpty() )
	{
		strTaskData = strTaskData.Left(strTaskData.GetLength()-1) ;
	}
	strData += Encode(strTaskData) + _T(",") ;
	//--end 封装任务数据

	CStdString strActiveObj = _T("") ;
	strActiveObj.Format( _T("%ld") , m_lngActiveObject) ;
	strData += Encode(strActiveObj) ;

	return strData ;
}

void CFunctionData::ExtraTasksFromSource(CStdString &strData)
{
	std::vector<CStdString> vecFunctionData ;
	int nSize = SplitStringWithSeparator(strData , ENCODE_SEPARATOR , vecFunctionData) ;

	int i = 0 ;
	//解封任务数据  Begin--
	if ( i<nSize )
	{
		CStdString strTaskData = Decode( vecFunctionData[i++] ) ;
		std::vector<CStdString> vecTaskData ;
		SplitStringWithSeparator(strTaskData , ENCODE_SEPARATOR , vecTaskData) ;
		for (int j=0 ; j<vecTaskData.size() ; ++j)
		{
			CTaskData TDTmp ;
			TDTmp.SetSource(vecTaskData[j]) ;
			m_vobjTaskData.push_back(TDTmp) ;
		}
	}
	//--end 解封任务数据

	if ( i<nSize )
	{
		m_lngActiveObject = _ttol( Decode(vecFunctionData[i++]) ) ;
	}
	
}