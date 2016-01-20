#ifndef __FIND_DATA_BY_ID__
#define __FIND_DATA_BY_ID__

#include "common_stru.h"
#include "common_stru_exserver.h"
#include "IGlobalDataBlock.h"

//根据数据枚举ID,定位数据
class CGlobalDataBlock : public IGlobalDataBlock
{
	
public:
    CGlobalDataBlock();

    virtual ~CGlobalDataBlock();
	DWORD &GetGlobalID();
	void * ReadRealData(DWORD dwDataType,DWORD dwIndex,
						DWORD dwClassType, E_DataUseState dwUserState);
	DWORD WriteRealData(DWORD dwIndex,DWORD dwDataType,
											void *Point,DWORD dwClassType,
												 E_DataUseState dwUserState);
	void* DeleteDataNode(DWORD dwIndex,DWORD dwDataType,DWORD dwClassType,
									E_DataUseState dwUserState);
	DWORD Reset(DWORD dwDataType,DWORD dwClassType);

private:
 
	std::map<DWORD,std::map<DWORD,void *> *> m_pAllRealData;
	CLock m_lock;

};

#endif