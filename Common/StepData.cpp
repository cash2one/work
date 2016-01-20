///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：步骤数据操作
// 生成日期：2013.04.09
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
//#include "BlockData.h"
#include "StepData.h"
#include "BlockData.h"

CStepData::CStepData():m_pItem(NULL)
{
	Clear();
}

CStepData::CStepData(const CStepData &stepData)
{
	m_amActionMethod = stepData.m_amActionMethod ;
	m_strDescription = stepData.m_strDescription ;

	m_nLevel = stepData.m_nLevel;
	m_pItem = NULL;
	SetItem(stepData.m_pItem) ;
	
	m_strSymbol = stepData.m_strSymbol ;
	m_bVaguelyLocate = stepData.m_bVaguelyLocate ;
	//m_lngSurWait = stepData.m_lngSurWait ;
	m_strSurWait = stepData.m_strSurWait;
	m_bFaultTolerant = stepData.m_bFaultTolerant ;
	m_vRet = stepData.m_vRet ;
	m_strErrDescription = stepData.m_strErrDescription ;
	m_bPageShot = stepData.m_bPageShot ;
	m_strVLSymbol = stepData.m_strVLSymbol ;
	m_nClickType = stepData.m_nClickType;
	m_nIndentifyType = stepData.m_nIndentifyType;
	m_nJumpUrlType = stepData.m_nJumpUrlType;
	m_nFillType = stepData.m_nFillType;
	m_bRestartFrom1thStepIfFail = stepData.m_bRestartFrom1thStepIfFail;
	m_strRetErr = stepData.m_strRetErr;

	m_pSubBlock = stepData.m_pSubBlock;
}

CStepData & CStepData::operator =(const CStepData &stepData)
{
	if ( this == &stepData)
	{
		return *this ;
	}

	this->Clear() ;

	m_amActionMethod = stepData.m_amActionMethod ;
	m_strDescription = stepData.m_strDescription ;
	m_nClickType = stepData.m_nClickType;
	m_nIndentifyType = stepData.m_nIndentifyType;
	m_nJumpUrlType = stepData.m_nJumpUrlType;
	m_nFillType = stepData.m_nFillType;
	m_bRestartFrom1thStepIfFail = stepData.m_bRestartFrom1thStepIfFail;
	m_nLevel = stepData.m_nLevel;
	SetItem(stepData.m_pItem) ;

	m_strSymbol = stepData.m_strSymbol ;
	m_bVaguelyLocate = stepData.m_bVaguelyLocate ;
	//m_lngSurWait = stepData.m_lngSurWait ;
	m_strSurWait = stepData.m_strSurWait;
	m_bFaultTolerant = stepData.m_bFaultTolerant ;
	m_vRet = stepData.m_vRet ;
	m_strErrDescription = stepData.m_strErrDescription ;
	m_bPageShot = stepData.m_bPageShot ;
	m_strVLSymbol = stepData.m_strVLSymbol ;

	m_strRetErr = stepData.m_strRetErr;

	m_pSubBlock = stepData.m_pSubBlock;

	return *this ;
}

void CStepData::Clear()//初始化 清理数据
{
	m_amActionMethod = amNone ;
	m_strDescription = _T("") ;
	m_nClickType = 0;//正常点击
	m_nIndentifyType = 1;//默认为远程打码
	m_nJumpUrlType = 1;
	m_nFillType = 0;
	m_bRestartFrom1thStepIfFail = false;
	m_nLevel = 0;
	FreeItem(m_pItem);
	m_pItem = NULL;

	m_strSymbol = _T("") ;
	m_bVaguelyLocate = FALSE ;
	//m_lngSurWait = 0 ;
	m_strSurWait = _T("0");
	m_bFaultTolerant = FALSE ;
	m_vRet.clear() ;
//	m_strTargetText = _T("") ;
	m_strDescription = _T("") ;
	m_bPageShot = FALSE ;
	m_strVLSymbol = _T("");

	m_strRetErr = _T("");

	m_pSubBlock = NULL;//只有在存在子step的情况下有效
}

void CStepData::FreeItem(T_ITEM *&pItem)
{
	if (NULL == pItem)
		return;

	FreeItem(pItem->pBrother);
	if (NULL != pItem->pBrother)
	{
		delete pItem->pBrother;
		pItem->pBrother = NULL;
	}

	FreeItem(pItem->pParent);
	if (NULL != pItem->pParent)
	{
		delete pItem->pParent;
		pItem->pParent = NULL;
	}

	//FreeItem(pItem->pBrother);
	//if (NULL != pItem->pBrother)
	//{
	//	delete pItem->pBrother;
	//	pItem->pBrother = NULL;
	//}

	pItem->mapAttributes.clear();

	int numNode = pItem->m_mapNodesAttr.size();
	for(int i = 0; i < numNode; i++)
	{
		FreeItem(pItem->m_mapNodesAttr[i]);
	}

	delete pItem;
	pItem = NULL;
}

CStdString CStepData::GetRetErr()
{
	return m_strRetErr;
}

void CStepData::SetRetErr(CStdString &strErr)
{
	m_strRetErr = strErr;
}

E_ActionMethod CStepData::GetAction() const
{
	return m_amActionMethod ;
}

void CStepData::SetAction(E_ActionMethod method)
{
	m_amActionMethod = method ;
}

int CStepData::GetClickType() const
{
	return m_nClickType;
}

void CStepData::SetClickType(int nNormalClick)
{
	m_nClickType = nNormalClick;
}

int CStepData::GetIndentifyType() const
{
	return m_nIndentifyType;
}

void CStepData::SetIndentifyType(int nIndentify)
{
	m_nIndentifyType = nIndentify;
}

int CStepData::GetJumpType() const
{
	return m_nJumpUrlType;
}

void CStepData::SetJumpType(int nJump)
{
	m_nJumpUrlType = nJump;
}

int CStepData::GetFillType() const
{
	return m_nFillType;
}

void CStepData::SetFillType(int nFill)
{
	m_nFillType = nFill;
}

bool CStepData::GetFlagOfRestartFrom1thStep()
{
	return m_bRestartFrom1thStepIfFail;
}

void CStepData::SetFlagOfRestartFrom1thStep(bool bFlag)
{
	m_bRestartFrom1thStepIfFail = bFlag;
}


int CStepData::GetLevel() const
{
	return m_nLevel;
}

void CStepData::SetLevel(int level)
{
	m_nLevel = level;
}

CStdString CStepData::GetDescrition() const
{
	return m_strDescription ;
}

void CStepData::SetDescription(const CStdString &strDesc)
{
	m_strDescription = strDesc ;
}

void CStepData::GetTagName(CStdString &strName , T_ITEM *pItem) const
{
	if ( pItem )
	{
		strName = pItem->strTagName ;
	}
}

void CStepData::SetTagName(const CStdString &strName , T_ITEM *pItem)
{
	if ( pItem )
	{
		pItem->strTagName = strName ;
	}
}

void CStepData::GetAttribute(const CStdString &strName, CStdString &strVal , T_ITEM *pItem)
{
	if ( pItem )
	{
		strVal = pItem->mapAttributes[strName] ;
	}
}

void CStepData::SetAttribute(const CStdString &strName, const CStdString &strVal , T_ITEM *pItem)
{
	if ( pItem )
	{
		pItem->mapAttributes[strName] = strVal ;
	}
}

void CStepData::DelAttribute(const CStdString &strName , T_ITEM *pItem )
{
//	m_pItem->mapAttributes.erase(strName) ;
	if (!pItem)
	{
		return ;
	}
	std::map<CStdString , CStdString>::iterator iter ;
	iter = pItem->mapAttributes.find(strName) ;

	if (iter != pItem->mapAttributes.end())
	{
		pItem->mapAttributes.erase(iter) ;
	}
}

void CStepData::DelAttribute(const CStdString &strName)
{
	DelAttribute(strName , m_pItem) ;
}

void CStepData::SetItem(T_ITEM *Item)
{
	SetItem(Item , m_pItem) ;
}

void CStepData::GetItem(T_ITEM *&Item) 
{
	SetItem(m_pItem , Item) ;
}

T_ITEM * CStepData::GetItem() 
{
	return m_pItem;
}

void CStepData::SetItem(T_ITEM *ItemSource , T_ITEM *&ItemDest)
{
	if ( !ItemSource )
	{
		ItemDest = NULL ;
		return ;
	}

	//这里必须删除父节点，不然有可能引起的不仅仅是内存泄露，还有父节点会覆盖等现象
	T_ITEM *pTmp = ItemDest;
	if(NULL != ItemDest)
	{
		if((ItemDest->mapAttributes.size() != 0 || ItemDest->m_mapNodesAttr.size() != 0)/* && ItemDest->strTagName != _T("")*/)
		{
			FreeItem(ItemDest);
		}
	}
	
	ItemDest = new T_ITEM ;
	ItemDest->strTagName = ItemSource->strTagName ;
	ItemDest->mapAttributes = ItemSource->mapAttributes ;

	//此时的SetItem必须为深复制，因为他要传给采编，就是在另一个进程里面了
	int nCount = ItemSource->m_mapNodesAttr.size();
	std::map<int, T_ITEM *>::iterator iter = ItemSource->m_mapNodesAttr.begin();
	for(; iter != ItemSource->m_mapNodesAttr.end(); ++iter)
	{
		ItemDest->m_mapNodesAttr[iter->first] = NULL;
		SetItem(ItemSource->m_mapNodesAttr[iter->first], ItemDest->m_mapNodesAttr[iter->first]);
	}
	/*for(int i = 0; i < nCount; i++)
	{
		ItemDest->m_mapNodesAttr[i] = NULL;
		SetItem(ItemSource->m_mapNodesAttr[i], ItemDest->m_mapNodesAttr[i]);
	}*/

	/*if (ItemSource->pParent)
	{
		SetItem(ItemSource->pParent , ItemDest->pParent) ;
	}

	if (ItemSource->pBrother)
	{
		//SetItem(ItemSource->pBrother , ItemSource->pBrother) ;
		SetItem(ItemSource->pBrother , ItemDest->pBrother) ;
		//ItemDest->pBrother = ItemSource->pBrother;
	}*/
}

void CStepData::SetPageShot(bool &bPageShot)
{
	m_bPageShot = bPageShot ;
}

bool CStepData::GetPageShot() const
{
	return m_bPageShot ;
}

void CStepData::SetValuelyLocate(bool &bValuelyLocate)
{
	m_bVaguelyLocate = bValuelyLocate ;
}

bool CStepData::GetValuelyLocate() const
{
	return m_bVaguelyLocate ;
}

void CStepData::SetFaultTolerant(bool &bFaultTolerant)
{
	m_bFaultTolerant = bFaultTolerant ;
}

bool CStepData::GetFaultTolerant() const 
{
	return m_bFaultTolerant ;
}

CStdString CStepData::GetSymbol() const
{
	return m_strSymbol ;
}

void CStepData::SetSymbol(const CStdString &strSymbol)
{
	m_strSymbol = strSymbol ;
}

void CStepData::GetStepReturn(std::vector<T_STEP_RETURN> &vStepReturn)
{
	vStepReturn.clear() ;
	vStepReturn = m_vRet ;
}

void CStepData::SetStepReturn(std::vector<T_STEP_RETURN> &vStepReturn)
{
	m_vRet.clear() ;
	m_vRet = vStepReturn ;
}

void CStepData::AddStepReturn(T_STEP_RETURN &stepReturn)
{
	m_vRet.push_back(stepReturn) ;
}

void CStepData::DelStepReturnByAttrib(CStdString &strAttrib)
{
	std::vector<T_STEP_RETURN>::iterator vecIter ;
	for ( int i=0 ; i<m_vRet.size() ; ++i)
	{
		if (m_vRet[i].strAttrib == strAttrib)
		{
			vecIter = m_vRet.begin() + i ;
			m_vRet.erase(vecIter) ;
			return ;
		}
	}
}

void CStepData::SetMatchRuleByAttrib(const CStdString &strAttrib , CStdString &strMatchRule)
{
	for ( int i=0 ; i<m_vRet.size() ; ++i)
	{
		if ( m_vRet[i].strAttrib == strAttrib )
		{
			m_vRet[i].strRule = strMatchRule ;
			return ;
		}
	}
}

void CStepData::SetRetValueByAttrib(const CStdString &strAttrib , CStdString &strValue)
{
	for ( int i=0 ; i<m_vRet.size() ; ++i)
	{
		if ( m_vRet[i].strAttrib == strAttrib )
		{
			m_vRet[i].strValue = strValue ;
			return ;
		}
	}
}

bool CStepData::StepNeedReturnData(void) const//是否要提取数据
{
	return !m_vRet.empty();
}

//long CStepData::GetSurWait() const
//{
//	return m_lngSurWait ;
//}
//
//void CStepData::SetSurWait(long lngSurWait)
//{
//	m_lngSurWait = lngSurWait ;
//}

CStdString CStepData::GetSurWait() const
{
	return m_strSurWait;
}

void CStepData::SetSurWait(CStdString strSurWait)
{
	m_strSurWait = strSurWait;
}

void CStepData::GetErrDescrition(CStdString &strErrDesc) const
{
	strErrDesc = m_strErrDescription ;
}

void CStepData::SetErrDescription(const CStdString &strErrDesc)
{
	m_strErrDescription = strErrDesc ;
}

CStdString CStepData::GetVLSymbol()
{
	return m_strVLSymbol ;
}

void CStepData::SetVLSymbol(CStdString &strVLSymbol)
{
	m_strVLSymbol = strVLSymbol ;
}

CStdString CStepData::GetReturnData()//暂无使用
{
	return _T("") ;
}

CStdString CStepData::GetSource()
{
	CStdString strData = TransTargetsToSource() ;
	return Encode(strData) ;
}

void CStepData::SetSource(const CStdString &strData)
{
	Clear() ;

	CStdString strSourceData = Decode(strData) ;
	ExtraTargetsFromSource(strSourceData) ;
}

CStdString CStepData::TransTargetsToSource()
{
	CStdString strData ;
	CStdString strTmp ;
	strData.Empty() ;

	CStdString strActionMethod = _T("") ;
	strActionMethod.Format(_T("%d") , m_amActionMethod) ;
	strData += Encode(strActionMethod) + _T(",") ;

	strData += Encode(m_strDescription) + _T(",") ;

	/*CStdString strLevel = _T("") ;
	strLevel.Format(_T("%d") , m_nLevel) ;
	strData += Encode(strLevel) + _T(",") ;*/
	strData += TransItemsToSource(m_pItem) + _T(",") ;

	strData += Encode(m_strSymbol) + _T(",") ;

	CStdString strValuelyLocate = _T("") ;
	strValuelyLocate.Format(_T("%d") , m_bVaguelyLocate) ;
	strData += Encode(strValuelyLocate) + _T(",") ;

	//CStdString strSurWait = _T("") ;
	//strSurWait.Format(_T("%ld") , m_lngSurWait) ;
	//strData += Encode(strSurWait) += _T(",") ;
	strData += Encode(m_strSurWait) + _T(",");

	CStdString strFaultTolerant = _T("") ;
	strFaultTolerant.Format(_T("%d") , m_bFaultTolerant) ;
	strData += Encode(strFaultTolerant) + _T(",") ;

	strData += TransStepReturnToSource(m_vRet) + _T(",") ;

	strData += Encode(m_strErrDescription) + _T(",") ;

	CStdString strPageShot = _T("") ;
	strPageShot.Format(_T("%d") , m_bPageShot) ;
	strData += Encode(strPageShot) + _T(",") ;

	strData += Encode(m_strVLSymbol) + _T(",") ;

	strData += Encode(m_strRetErr) + _T(",");

	if(m_pSubBlock != NULL)
	{
		strData += m_pSubBlock->GetSource() + _T(",");
	}
	else
	{
		CStdString strSubBlock = _T("") ;
		strData += Encode(strSubBlock) + _T(",");
	}

	CStdString strLevel = _T("") ;
	strLevel.Format(_T("%d"), m_nLevel) ;
	strData += Encode(strLevel) + _T(",") ;

	CStdString strClickType = _T("") ;
	strClickType.Format(_T("%d") , m_nClickType) ;
	strData += Encode(strClickType) + _T(",") ;

	CStdString strIndentify = _T("") ;
	strIndentify.Format(_T("%d") , m_nIndentifyType) ;
	strData += Encode(strIndentify) + _T(",");

	CStdString strFlagOfRestartFrom1thStep = _T("");
	strFlagOfRestartFrom1thStep.Format(_T("%d"), m_bRestartFrom1thStepIfFail);
	strData += Encode(strFlagOfRestartFrom1thStep) + _T(",");

	CStdString strJumpUrlType = _T("");
	strJumpUrlType.Format(_T("%d"), m_nJumpUrlType);
	strData += Encode(strJumpUrlType) + _T(",");

	CStdString strFillType = _T("");
	strFillType.Format(_T("%d"), m_nFillType);
	strData += Encode(strFillType) + _T(",");

	return strData ;
}

void CStepData::ExtraTargetsFromSource(CStdString &strSourceData) 
{
	std::vector<CStdString> vecStepData ;
	int nSize = SplitStringWithSeparator(strSourceData , ENCODE_SEPARATOR , vecStepData) ;

	int i = 0 ;
	if ( i<nSize )
	{
		m_amActionMethod = (E_ActionMethod)_ttoi( Decode(vecStepData[i++]) ) ;
		m_strDescription = Decode(vecStepData[i++]) ;
		m_strDescription.Replace(_T("&b"), _T(",")) ;
	}
	
	//读取m_pItem begin--
	if ( i<nSize )
	{
		CStdString strItem = Decode( vecStepData[i++] ) ;
		if (strItem.IsEmpty())
		{
			//add cao
			if(m_pItem)
			{
				this->FreeItem(m_pItem);
				m_pItem = NULL;
			}
			////////////

			m_pItem = NULL ;
		}
		else
		{
			//add cao
			if(m_pItem)
			{
				this->FreeItem(m_pItem);
				m_pItem = NULL;
			}
			m_pItem = NULL;
			///////////////

			m_pItem = new T_ITEM ;
			ExtraItemsFromSource(strItem , m_pItem) ;
		}
	}
	//--end 读取m_pItem 

	if ( i<nSize )
	{
		m_strSymbol = Decode( vecStepData[i++] ) ;
		m_strSymbol.Replace(_T("&b"), _T(",")) ;
	}
	if ( i<nSize )
	{
		m_bVaguelyLocate = _ttoi( Decode(vecStepData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_strSurWait = Decode(vecStepData[i++]);
		//m_lngSurWait = _ttol( Decode(vecStepData[i++]) ) ;
	}
	if ( i<nSize )
	{
		m_bFaultTolerant = _ttoi( Decode(vecStepData[i++]) ) ;
	}

	//读取m_vRet begin--
	if ( i<nSize )
	{
		CStdString strStepReturnData = Decode( vecStepData[i++] ) ;
		if ( !strStepReturnData.IsEmpty() )
		{
			ExtraStepReturnFromSource(strStepReturnData , m_vRet) ;
		}
	}
	//--end 读取m_vRet 

	if ( i<nSize )
	{
		m_strErrDescription = Decode( vecStepData[i++] ) ;
	}
	if ( i<nSize )
	{
		m_bPageShot = _ttoi( Decode(vecStepData[i++]) ) ;
	}
	if ( i < nSize )
	{
		m_strVLSymbol = Decode( vecStepData[i++] ) ;
	}

	if ( i < nSize )
	{
		m_strRetErr = Decode(vecStepData[i++]);
	}

	if (i < nSize)
	{
		CStdString strSubBlock = vecStepData[i++];
		if (_T("") != strSubBlock)
		{
			CBlockData* pBlock = new CBlockData;
			pBlock->SetSource(strSubBlock);

			m_pSubBlock = pBlock;
			//这里没有对之前的m_pSubBlock进行释放，暂时没有做释放处理
		}
	}

	if (i < nSize)
	{
		m_nLevel = _ttoi( Decode(vecStepData[i++]) ) ;
	}

	if ( i<nSize )
	{
		m_nClickType = _ttoi( Decode(vecStepData[i++]) ) ;
	}

	if ( i<nSize )
	{
		m_nIndentifyType = _ttoi( Decode(vecStepData[i++]) ) ;
	}

	if (i < nSize)
	{
		m_bRestartFrom1thStepIfFail = _ttoi(Decode(vecStepData[i++]));
	}

	if (i < nSize)
	{
		m_nJumpUrlType = _ttoi(Decode(vecStepData[i++]));
	}

	if (i < nSize)
	{
		m_nFillType = _ttoi(Decode(vecStepData[i++]));
	}
}

CStdString CStepData::TransItemsToSource(T_ITEM *pItem)
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

		//if (pItem->pParent)
		{
			strItem += _T(",") ;
		}

		//if (pItem->pBrother)
		{
			strItem += _T(",") ;
		}
		
		////打包pItem
		std::map<int, T_ITEM *>::iterator iter = pItem->m_mapNodesAttr.begin();
		CStdString strNodeValue = _T("");
		CStdString strTmp = _T("");
		for (; iter != pItem->m_mapNodesAttr.end(); ++iter)
		{
			CStdString strKey = _T("") ;
			strKey.Format(_T("%d"), iter->first) ;

			strTmp = strKey + _T(",") + TransItemsToSource(iter->second);
			strNodeValue += Encode(strTmp) + _T(",") ;

			strTmp = _T("");
		}

		if (!strNodeValue.IsEmpty())
		{
			strNodeValue = strNodeValue.Left(strNodeValue.GetLength()-1);
			strItem += Encode(strNodeValue) + _T(",") ;
		}
		//TransParentNodeTreeToSource(pItem->m_mapNodesAttr[0]);
	}

	return Encode(strItem) ;
}

void CStepData::ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem)
{
	std::vector<CStdString> vecItemData ;
	int nSize = SplitStringWithSeparator(strItemData , ENCODE_SEPARATOR , vecItemData) ;

	int i = 0 ;
	if ( i<nSize )
	{
		pItem->strTagName = Decode(vecItemData[i++]) ;
		pItem->strTagName.Replace(_T("&b"), _T(",")) ;
	}

	if ( i<nSize )
	{
		CStdString strAttributesData = Decode( vecItemData[i++] ) ;
		if ( !strAttributesData.IsEmpty() )
		{
			ExtraAttributesFromSource(strAttributesData , pItem->mapAttributes) ;
		}
	}

	if ( i <nSize )
	{
		CStdString strParentItem = Decode(vecItemData[i++]) ;
		if (strParentItem.IsEmpty())
		{
			pItem->pParent = NULL ;
		}
	}

	if ( i<nSize )
	{
		CStdString strBrotherItem = Decode(vecItemData[i++]) ;
		if (strBrotherItem.IsEmpty())
		{
			pItem->pBrother = NULL ;
		}
	}

	//解压item父节点
	if (i < nSize)
	{
		CStdString strTreeItem = Decode(vecItemData[i++]);

		std::vector<CStdString> vecNodeData ;
		SplitStringWithSeparator(strTreeItem, ENCODE_SEPARATOR, vecNodeData);

		std::vector<CStdString> strVecKeyValue;
		int nSize = 0;
		CStdString strKey, strValue;
		for (int k = 0; k < vecNodeData.size(); k++)
		{
			nSize = SplitStringWithSeparator(Decode(vecNodeData[k]), ENCODE_SEPARATOR, strVecKeyValue) ;
			if ( nSize != 2)
			{
				continue; 
			}
			strKey = strVecKeyValue[0];
			strValue = strVecKeyValue[1];
			int nIndex = _ttoi(strKey);

			T_ITEM *pSubItem = new T_ITEM;
			pItem->m_mapNodesAttr[nIndex] = pSubItem;
			ExtraItemsFromSource(Decode(strValue), pSubItem);
		}
	}
}

CStdString CStepData::TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes)
{
	std::map<CStdString , CStdString>::iterator mapIter ;
	CStdString strAttribute = _T("") ;
	for (mapIter=mapAttributes.begin() ; mapIter!=mapAttributes.end() ; ++mapIter)
	{
		CStdString strNameValue = Encode(mapIter->first) + _T(",") + Encode(mapIter->second) ;
		strAttribute += Encode(strNameValue) + _T(",") ;
	}
	if ( !strAttribute.IsEmpty() )
	{
		strAttribute = strAttribute.Left(strAttribute.GetLength()-1) ;
	}

	return Encode(strAttribute) ;
}

void CStepData::ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes)
{
	std::vector<CStdString> vecAttributeData ;
	SplitStringWithSeparator(strAttributeData , ENCODE_SEPARATOR  , vecAttributeData) ;

	std::vector<CStdString> strVecKeyorValue ;
	int nSize = 0 ;
	CStdString strKey, strValue ;
	for (int i=0 ; i<vecAttributeData.size() ; ++i)
	{
		nSize =SplitStringWithSeparator(Decode(vecAttributeData[i]) , ENCODE_SEPARATOR , strVecKeyorValue) ;
		if ( nSize != 2)
		{
			continue; 
		}
		strKey = Decode(strVecKeyorValue[0]) ;
		strKey.Replace(_T("&b"), _T(",")) ;
		strValue = Decode(strVecKeyorValue[1]) ;
		strValue.Replace(_T("&b"), _T(",")) ;

		mapAttributes[strKey] = strValue ;
	}
}

CStdString CStepData::TransStepReturnToSource(std::vector<T_STEP_RETURN> &vRet)
{
	CStdString strStepReturn = _T("") ;

	if (vRet.size() != 0)
	{
		std::vector<T_STEP_RETURN>::iterator  vIter;
		for (vIter=vRet.begin() ; vIter!=vRet.end() ; ++vIter)
		{
			CStdString strTmp = Encode(vIter->strAttrib) + _T(",") + Encode(vIter->strRule) + _T(",") + Encode(vIter->strValue) ;
			strStepReturn += Encode(strTmp) + _T(",") ;
		}
		if ( !strStepReturn.IsEmpty() )
		{
			strStepReturn = strStepReturn.Left(strStepReturn.GetLength()-1) ;
		}
	}

	return Encode(strStepReturn) ;
}

void CStepData::ExtraStepReturnFromSource(CStdString &strStepReturnData , std::vector<T_STEP_RETURN> &vRet)
{
	std::vector<CStdString> vecStepReturnData ;
	SplitStringWithSeparator(strStepReturnData , ENCODE_SEPARATOR , vecStepReturnData) ;

	std::vector<CStdString> vecStepReturnElement ;
	T_STEP_RETURN SRTmp ;
	int nSize = 0 ;
	for (int i=0 ; i<vecStepReturnData.size() ; ++i)
	{
		nSize = SplitStringWithSeparator(Decode(vecStepReturnData[i]) , ENCODE_SEPARATOR , vecStepReturnElement) ;
		if ( nSize != 3)
		{
			continue; 
		}

		SRTmp.strAttrib = Decode( vecStepReturnElement[0] ) ;
		SRTmp.strAttrib.Replace(_T("&b"), _T(",")) ;
		SRTmp.strRule = Decode( vecStepReturnElement[1] ) ;
		SRTmp.strRule.Replace(_T("&b"), _T(",")) ;
		SRTmp.strValue = Decode( vecStepReturnElement[2] ) ;
		SRTmp.strValue.Replace(_T("&b"), _T(",")) ;
		
		vRet.push_back(SRTmp) ;
	}
}

CStepData::~CStepData()
{
	//add cao
	if(m_pItem)
	{
		this->FreeItem(m_pItem);
	}
	////////////

	//m_pSubBlock的释放,不需要，这个是浅拷贝，释放以后再说
	//m_pSubBlock
}

void CStepData::FreeSubBlock(CBlockData *&pBlock)
{
	if (NULL == pBlock)
	{
		return;
	}

	int numNode = pBlock->GetCount();
	CStepData *pSubStep;
	CBlockData *pSubBlock;
	for (int i = 0; i < numNode; i++)
	{
		pBlock->SetActiveIndex(i);
		pSubStep = pBlock->GetCurStep();
		T_ITEM* pItem = pSubStep->GetItem();
		FreeItem(pItem);
		pSubBlock = pSubStep->m_pSubBlock;
		FreeSubBlock(pSubBlock);
	}

	delete pBlock;
	pBlock = NULL;
}

void CStepData::FillSubBlock(CBlockData *pBlockSource, CBlockData *&pBlockDest)
{
	if (!pBlockSource)
	{
		pBlockDest = NULL;
		return;
	}
	//这里必须删除父节点，不然有可能引起的不仅仅是内存泄露，还有父节点会覆盖等现象
	if (NULL != pBlockDest)
	{
		FreeSubBlock(pBlockDest);
	}
	//此时的SetItem必须为深复制，因为他要传给采编，就是在另一个进程里面了
	pBlockDest = new CBlockData;
	pBlockDest->m_lngActiveObject = pBlockSource->m_lngActiveObject;                 //当前活跃对象
	pBlockDest->m_lngPerWait = pBlockSource->m_lngPerWait;                      //前置延时
	pBlockDest->m_bFaultTolerant = pBlockSource->m_bFaultTolerant;                  //页面是否容错
	pBlockDest->m_bLoopFlag = pBlockSource->m_bLoopFlag;             //循环标记
	pBlockDest->m_strDescription = pBlockSource->m_strDescription;   //数据块描述
	pBlockDest->m_tLoopSetting = pBlockSource->m_tLoopSetting;  // 循环条件设置
	pBlockDest->m_tJumpSetting = pBlockSource->m_tJumpSetting; //跳转条件设置

	CStepData* pStepTmp = NULL;
	int nCount = pBlockSource->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CStepData* pSubStep = new CStepData;
		pBlockSource->SetActiveIndex(i);
		pStepTmp = pBlockSource->GetCurStep();
		FillStepContent(pStepTmp, pSubStep);
		FillSubBlock(pStepTmp->m_pSubBlock, pSubStep->m_pSubBlock);
		pBlockDest->AddObject(*pSubStep);
	}
}

void CStepData::FillStepContent(CStepData* pStepTmp, CStepData* pSubStep)
{
	pSubStep->m_amActionMethod = pStepTmp->m_amActionMethod;
	pSubStep->m_strDescription = pStepTmp->m_strDescription;

	pSubStep->m_nLevel = pStepTmp->m_nLevel;

	//pSubStep->m_pItem = NULL;
	FreeItem(pSubStep->m_pItem);
	T_ITEM* pDest = pSubStep->GetItem();
	SetItem(pStepTmp->GetItem(), pDest);

	pSubStep->m_strSymbol = pStepTmp->m_strSymbol;
	pSubStep->m_bVaguelyLocate = pStepTmp->m_bVaguelyLocate;
	pSubStep->m_strSurWait = pStepTmp->m_strSurWait;
	pSubStep->m_bFaultTolerant = pStepTmp->m_bFaultTolerant;
	pSubStep->m_vRet = pStepTmp->m_vRet;
	pSubStep->m_strErrDescription = pStepTmp->m_strErrDescription;
	pSubStep->m_bPageShot = pStepTmp->m_bPageShot;
	pSubStep->m_strVLSymbol = pStepTmp->m_strVLSymbol;
	pSubStep->m_nClickType = pStepTmp->m_nClickType;
	pSubStep->m_nIndentifyType = pStepTmp->m_nIndentifyType;
	pSubStep->m_nJumpUrlType = pStepTmp->m_nJumpUrlType;
	pSubStep->m_nFillType = pStepTmp->m_nFillType;
	pSubStep->m_bRestartFrom1thStepIfFail = pStepTmp->m_bRestartFrom1thStepIfFail;
	pSubStep->m_strRetErr = pStepTmp->m_strRetErr;
}