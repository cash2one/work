#pragma once

//说明:
//A: T_AllRealData 结构体中添加新成员的同时,需要同步添加以下2个地方
//   1: 以下枚举值的添加
//   2: 在InitMapRealData的地方,需要调用CFindDataByID的成员函数AddByEnumID

//B: 为方便结构体与枚举大家更容易理解,结构体与枚举(以下的),只有首字母不同

//C: 大家根据实际需要修改T_AllRealData中的成员类型,目前统一都是list

//D: T_AllRealData中成员的各个结点的结构体的修改不会影响以下代码

enum
{
	E_LocalGlobalData = 1,
	E_GUIData ,   
	E_AllTaskData,

	E_MAP_SaveAccountPass,//add by yzh
	E_MAP_NetStation,//add by yzh

	E_SearchParam,
	E_ServerNetStationInfoGroup,
	E_FtpFileName,

	E_ServerEngineFileInfoGroup,
};




class IGlobalDataBlock
{
public:
	virtual DWORD &GetGlobalID()=0;
	virtual void * ReadRealData(DWORD dwDataType,DWORD dwIndex,
		DWORD dwClassType, E_DataUseState dwUserState)=0;
	virtual DWORD WriteRealData(DWORD dwIndex,DWORD dwDataType,
		void *Point,DWORD dwClassType,
		E_DataUseState dwUserState)=0;
	virtual void *DeleteDataNode(DWORD dwIndex,DWORD dwDataType,DWORD dwClassType,
									E_DataUseState dwUserState)=0;
	virtual DWORD Reset(DWORD dwDataType,DWORD dwClassType)=0;
}; 