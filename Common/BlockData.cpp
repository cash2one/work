///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：步骤数据操作
// 生成日期：2013.04.15
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
#include "BlockData.h"

void CBlockData::Clear()
{
	m_vobjTargetData.clear() ;

	m_lngActiveObject = -1 ;
	m_lngPerWait = 0 ;
	m_bFaultTolerant = false ;
	m_bLoopFlag = false ;

	m_strDescription.Empty() ;

	m_tLoopSetting.Clear() ;
	m_tJumpSetting.Clear();
}

void CBlockData::AddObject(CStepData &obj)
{
	m_vobjTargetData.push_back(obj) ;
}

void CBlockData::DelObject()
{
	if ( m_lngActiveObject >= m_vobjTargetData.size() )
	{
		return ;
	}

	/*if (m_vobjTargetData[m_lngActiveObject].GetAction() == amSetDataArea)
	{
		DeleteMotherStep(m_vobjTargetData[m_lngActiveObject]);
	}*/
	m_vobjTargetData.erase(m_vobjTargetData.begin()+m_lngActiveObject) ;
}

void CBlockData::DeleteMotherStep(CStepData motherStep)//删除嵌套子块申请的内容
{
	CBlockData *pCurBlock = motherStep.m_pSubBlock;
	if(pCurBlock != NULL)
	{
		int countStep = pCurBlock->GetCount();
		CStepData subStep;
		for(int i = 0; i < countStep; i++)
		{
			pCurBlock->GetStep(i, subStep);
			if(subStep.GetAction() == amSetDataArea)
			{
				DeleteMotherStep(subStep);
			}
		}
		delete pCurBlock;
	}
}

void CBlockData::GetStep(long lngIndex, CStepData &obj)
{
	if ( lngIndex >= m_vobjTargetData.size() || lngIndex < 0)
	{
		return ;
	}

	obj = m_vobjTargetData[lngIndex] ;
}

void CBlockData::GetCurStep(CStepData &obj)
{
	if ( m_lngActiveObject >= m_vobjTargetData.size() )
	{
		return ;
	}

	obj = m_vobjTargetData[m_lngActiveObject] ;
}


CStepData *CBlockData::GetCurStep()
{
	if ( m_lngActiveObject >= m_vobjTargetData.size() )
	{
		return NULL;
	}

	return &m_vobjTargetData[m_lngActiveObject] ;
}

CStepData *CBlockData::GetStepByDescription(CStdString &strDescription)
{
	for ( int i=0 ; i<GetCount() ; ++i)
	{
		if (m_vobjTargetData[i].GetDescrition() == strDescription)
		{
			return &m_vobjTargetData[i] ;
		}
	}

	return NULL ;
}

void CBlockData::SwapStepData(int nObjIndex1 , int nObjIndex2)
{
	if (nObjIndex1>=m_vobjTargetData.size() || nObjIndex2>=m_vobjTargetData.size() || nObjIndex1==nObjIndex2)
	{
		return ;
	}

	int nLarge , nSmall ;
	nLarge = nSmall = -1 ;

	nLarge = nObjIndex1 > nObjIndex2 ? nObjIndex1 : nObjIndex2 ;
	nSmall = nObjIndex1 > nObjIndex2 ? nObjIndex2 : nObjIndex1 ;

	CStepData SDTmp = m_vobjTargetData[nLarge] ;
	m_vobjTargetData.erase(m_vobjTargetData.begin()+nLarge) ;
	m_vobjTargetData.insert(m_vobjTargetData.begin()+nSmall , SDTmp) ;
}

void CBlockData::InsertStepData(int nPostion , CStepData &stepData)
{
	if ( nPostion > m_vobjTargetData.size() )
	{
		return ;
	}

	std::vector<CStepData>::iterator IterPos ;
	IterPos = m_vobjTargetData.begin() + nPostion ;
	m_vobjTargetData.insert(IterPos , stepData) ;
}

int CBlockData::GetCount()
{
	return m_vobjTargetData.size() ;
}

bool CBlockData::GetFaultTolerant() const
{
	return m_bFaultTolerant ;
}

void CBlockData::SetFaultTolerant(bool &bFaultTolerant)
{
	m_bFaultTolerant = bFaultTolerant ;
}

bool CBlockData::GetLoopFlag() const
{
	return m_bLoopFlag ;
}

void CBlockData::SetLoopFlag(bool &bLoopFlag)
{
	m_bLoopFlag = bLoopFlag ;
}

long CBlockData::GetActiveIndex() const
{
	return m_lngActiveObject ;
}

void CBlockData::SetActiveIndex(long lIndex)
{
	m_lngActiveObject = lIndex ;
}

long CBlockData::GetPerWait() const
{
	return m_lngPerWait ;
}

void CBlockData::SetPerWait(long lngWait)
{
	m_lngPerWait = lngWait ;
}

CStdString CBlockData::GetDescription() const
{
	return m_strDescription ;
}

void CBlockData::SetDescription(const CStdString &strDescription)
{
	m_strDescription = strDescription ;
}

E_LOOP_TYPE CBlockData::GetLoopType() const
{
	return m_tLoopSetting.eLoopType ;
}

void CBlockData::SetLoopType(E_LOOP_TYPE &eLoopType)
{
	m_tLoopSetting.eLoopType = eLoopType ;
}

CStdString CBlockData::GetLoopValue() const
{
	return m_tLoopSetting.strValue ;
}

void CBlockData::SetLoopValue(const CStdString &strLoopValue)
{
	m_tLoopSetting.strValue = strLoopValue ;
}

bool CBlockData::GetSubRandom() const
{
	return m_tLoopSetting.bSubBlockRandom;
}

void CBlockData::SetSubRandom(bool &bSubRandom)
{
	m_tLoopSetting.bSubBlockRandom = bSubRandom;
}

//int CBlockData::GetRandomCount() const
//{
//	return m_tLoopSetting.nRandomCount;
//}
//
//void CBlockData::SetRandomCount(int nRandomCount)
//{
//	m_tLoopSetting.nRandomCount = nRandomCount;
//}

CStdString CBlockData::GetRandomCount() const
{
	return m_tLoopSetting.strRandomCount;
}

void CBlockData::SetRandomCount(CStdString strRandomCount)
{
	m_tLoopSetting.strRandomCount = strRandomCount;
}

////////////////////////////////////////////////////////////////////
bool CBlockData::GetIsJump() const
{
	return m_tJumpSetting.bJump;
}

void CBlockData::SetIsJump(bool &bLoopFlag) 
{
	m_tJumpSetting.bJump = bLoopFlag;
}

bool CBlockData::GetIsConditionFirst() const 
{
	return m_tJumpSetting.bConditionFirst;
}

void CBlockData::SetIsConditionFirst(bool &bLoopFlag) 
{
	m_tJumpSetting.bConditionFirst = bLoopFlag;
}

bool CBlockData::GetIsJumpSucc() const 
{
	return m_tJumpSetting.bSuccJump;
}

void CBlockData::SetIsJumpSucc(bool &bLoopFlag) 
{
	m_tJumpSetting.bSuccJump = bLoopFlag;
}

bool CBlockData::GetIsJumpFail() const 
{
	return m_tJumpSetting.bFailJump;
}

void CBlockData::SetIsJumpFail(bool &bLoopFlag) 
{
	m_tJumpSetting.bFailJump = bLoopFlag;
}

bool CBlockData::GetIsJumpCondition() const 
{
	return m_tJumpSetting.bConditionJump;
}

void CBlockData::SetIsJumpCondition(bool &bLoopFlag) 
{
	m_tJumpSetting.bConditionJump = bLoopFlag;
}

long CBlockData::GetSuccToWhich() const
{
	return m_tJumpSetting.nSucc;
}

void CBlockData::SetSuccToWhich(long lIndex)
{
	m_tJumpSetting.nSucc = lIndex;
}

long CBlockData::GetFailToWhich() const
{
	return m_tJumpSetting.nFail;
}

void CBlockData::SetFailToWhich(long lIndex)
{
	m_tJumpSetting.nFail = lIndex;
}

long CBlockData::GetConditionToWhich() const
{
	return m_tJumpSetting.nCondition;
}

void CBlockData::SetConditionToWhich(long lIndex)
{
	m_tJumpSetting.nCondition = lIndex;
}

long CBlockData::GetConJumpType() const
{
	return m_tJumpSetting.nJumpType;
}

void CBlockData::SetConJumpType(long lIndex)
{
	m_tJumpSetting.nJumpType = lIndex;
}

CStdString CBlockData::GetLeftCondition() const
{
	return m_tJumpSetting.strLeft;
}

void CBlockData::SetLeftCondition(CStdString str)
{
	m_tJumpSetting.strLeft = str;
}

CStdString CBlockData::GetRightCondition() const
{
	return m_tJumpSetting.strRight;
}

void CBlockData::SetRightCondition(CStdString str)
{
	m_tJumpSetting.strRight = str;
}

long CBlockData::GetConditionComp() const
{
	return m_tJumpSetting.nCompare;
}

void CBlockData::SetConditionComp(long lIndex)
{
	m_tJumpSetting.nCompare = lIndex;
}
////////////////////////////////////////////////////////////////////

void CBlockData::MoveForward()
{
	m_lngActiveObject -= 1 ;
}

void CBlockData::MoveNext() 
{
	m_lngActiveObject += 1 ;
}

CStdString CBlockData::GetSource()
{
	CStdString strData = TransTargetsToSource() ;
	return Encode(strData) ;
}

void CBlockData::SetSource(const CStdString &strSource)
{
	Clear() ;

	CStdString strData = Decode(strSource) ;
	ExtraTargetsFromSource(strData) ;
}

//private

CStdString CBlockData::TransTargetsToSource()
{
	CStdString strData = _T("") ;

	//封装步骤数据  begin--
	CStdString strStepData = _T("") ;
	for (int i=0 ; i<m_vobjTargetData.size() ; ++i)
	{
		strStepData += m_vobjTargetData[i].GetSource() + _T(",") ;
	}
	if (!strStepData.IsEmpty())
	{
		strStepData = strStepData.Left( strStepData.GetLength()-1 ) ;
	}
	strData += Encode(strStepData) + _T(",") ;
	//--end 封装步骤数据

	CStdString strActiveObject = _T("") ;
	strActiveObject.Format(_T("%ld") , m_lngActiveObject) ;
	strData += Encode(strActiveObject) + _T(",") ;

	CStdString strPerWait = _T("") ;
	strPerWait.Format(_T("%ld") , m_lngPerWait) ;
	strData += Encode(strPerWait) + _T(",") ;

	CStdString strFaultTolerant = _T("") ;
	strFaultTolerant.Format(_T("%d") , m_bFaultTolerant) ;
	strData += Encode(strFaultTolerant) + _T(",") ;

	CStdString strbLoopFlag = _T("") ;
	strbLoopFlag.Format(_T("%d") , m_bLoopFlag) ;
	strData += Encode(strbLoopFlag)  + _T(",");

	strData += Encode(m_strDescription) + _T(",");

	strData += m_tLoopSetting.GetSource() + _T(",") ;

	strData += m_tJumpSetting.GetSource() ;

	return strData ;
}

void CBlockData::ExtraTargetsFromSource(CStdString &strData)
{
	std::vector<CStdString> vecBlockData ;
	int nSize = SplitStringWithSeparator(strData ,ENCODE_SEPARATOR , vecBlockData) ;

	int i = 0 ;
	//解封StepData begin--
	if ( i<nSize )
	{
		CStdString strStepData = Decode( vecBlockData[i++] ) ;
		std::vector<CStdString> vecStepData ;
		if ( !strStepData.IsEmpty() )
		{
			int nCount = SplitStringWithSeparator(strStepData , ENCODE_SEPARATOR , vecStepData) ;
			for (int j=0 ; j<nCount ; ++j)
			{
				CStepData stepDataTmp ;
				stepDataTmp.SetSource( vecStepData[j] ) ;
				m_vobjTargetData.push_back(stepDataTmp) ;
			}
		}
	}
	//--end 解封StepData

	if ( i<nSize )
	{
		m_lngActiveObject = _ttol( Decode(vecBlockData[i++]) ) ;
	}
	if ( i<nSize)
	{
		m_lngPerWait = _ttol( Decode(vecBlockData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_bFaultTolerant = _ttoi( Decode(vecBlockData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_bLoopFlag = _ttoi( Decode(vecBlockData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_strDescription = Decode(vecBlockData[i++]) ;
		m_strDescription.Replace(_T("&b"), _T(",")) ;
	}
	if ( i<nSize )
	{
		m_tLoopSetting.SetSource( Decode(vecBlockData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_tJumpSetting.SetSource(Decode(vecBlockData[i++]));
	}
}
