#pragma once

//˵��:
//A: T_AllRealData �ṹ��������³�Ա��ͬʱ,��Ҫͬ���������2���ط�
//   1: ����ö��ֵ�����
//   2: ��InitMapRealData�ĵط�,��Ҫ����CFindDataByID�ĳ�Ա����AddByEnumID

//B: Ϊ����ṹ����ö�ٴ�Ҹ��������,�ṹ����ö��(���µ�),ֻ������ĸ��ͬ

//C: ��Ҹ���ʵ����Ҫ�޸�T_AllRealData�еĳ�Ա����,Ŀǰͳһ����list

//D: T_AllRealData�г�Ա�ĸ������Ľṹ����޸Ĳ���Ӱ�����´���

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