#include "stdafx.h"
#include "GlobalDataBlock.h"



CGlobalDataBlock::CGlobalDataBlock()
{
	m_pAllRealData.clear();
}
	
CGlobalDataBlock::~CGlobalDataBlock()
{
	std::map<DWORD,std::map<DWORD,void *>*>::iterator iter = m_pAllRealData.begin();

	for (; iter != m_pAllRealData.end(); ++iter)
	{
		if ((*iter).second)
		{
			//add by hpt 9-28释放内层MAP
			std::map<DWORD, void*>::iterator iter2 = iter->second->begin();
			for (; iter2 != iter->second->end(); ++iter2)
			{
				delete iter2->second;
			}

			delete (*iter).second;
		}
	}

	m_pAllRealData.clear();
	//释放内存
}
DWORD &CGlobalDataBlock::GetGlobalID()
{
	static DWORD dwInnerID = 1000;

	//把数据加到数据组织类  //add by yzh
	CLocalLock lock(&m_lock);

	dwInnerID++;	

	return dwInnerID;
}

void * CGlobalDataBlock::ReadRealData(DWORD dwDataType,DWORD dwIndex,
									DWORD dwClassType, E_DataUseState dwUserState)
{
	CLocalLock lock(&m_lock);
	std::map<DWORD,void *> *pMapData=m_pAllRealData[dwDataType];
	if(pMapData==NULL)
	{
		m_pAllRealData[dwDataType] = new std::map<DWORD,void *>();
		pMapData = m_pAllRealData[dwDataType];
	}
	
	void  *RetPoint = NULL;
	std::map<DWORD,void *>::iterator pIter = (pMapData)->find(dwIndex);
	if(pIter!=(pMapData)->end())
	{
		RetPoint = pIter->second;
	}

	if(RetPoint == NULL) 
		printf("读错误 指针为空 类%d 类型 %d  编号 %d\n",dwClassType,dwDataType,dwIndex);

	return RetPoint;
}

DWORD CGlobalDataBlock::WriteRealData(DWORD dwIndex,DWORD dwDataType,
										void *Point,DWORD dwClassType,
											 E_DataUseState dwUserState)
{
 	CLocalLock lock(&m_lock);
	if(Point == NULL) 
		printf("写错误 指针为空 类%d 类型 %d  编号 %d\n",dwClassType,dwDataType,dwIndex);

	std::map<DWORD,void *> *pMapData=NULL;
	pMapData = m_pAllRealData[dwDataType];
	if(pMapData==NULL)
	{
		m_pAllRealData[dwDataType] = new std::map<DWORD,void *>();
		pMapData = m_pAllRealData[dwDataType];
	}
	(*pMapData)[dwIndex]=Point;
 
	return 0;
}

void * CGlobalDataBlock::DeleteDataNode(DWORD dwIndex,DWORD dwDataType,
									DWORD dwClassType,
									E_DataUseState dwUserState)
{
	CLocalLock lock(&m_lock);
	std::map<DWORD,void *> *pMapData=m_pAllRealData[dwDataType];
	if(pMapData==NULL)
	{
		m_pAllRealData[dwDataType] = new std::map<DWORD,void *>();
		pMapData = m_pAllRealData[dwDataType];
	}
	
	void  *Ret = NULL;

	std::map<DWORD,void *>::iterator pIter = (pMapData)->find(dwIndex);
	if(pIter!=(pMapData)->end())
	{
		Ret = pIter->second;
		pMapData->erase(dwIndex);
	}
	if(Ret == NULL)
		printf("删除错误 指针为空 类%d 类型 %d  编号 %d\n",dwClassType,dwDataType,dwIndex);
	return Ret;
}

DWORD CGlobalDataBlock::Reset(DWORD dwDataType, DWORD dwClassType)
{
	return 0;
}