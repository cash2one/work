//处理Alltask任务
#pragma once
#include "Lock.h"
#include "CommServer.h"
#include "TaskData.h"
#include "NetData.h"
#include "StationData.h"
#include "BlockData.h"
#include <map>
#include <queue>
#include <vector>

class CTaskProcess
{
public:
	CTaskProcess(void);
	~CTaskProcess(void);

	/*
	@breif  是否为Object对象的网站
	@param	dwWebid 网站ID*/
/*	BOOL IsObjectWeb(const DWORD &dwWebid);*/

private:
	/*
	@breif  连接内核；
	@param Comserver socket连接；
	@param iPort    端口号
	@param fn   回调函数；
	*/
	void ConnectKernel(CCommServer &Comserver,int &iPort, pfnDataHandler fn);  
	/*
	@breif 启动4代内核
	@param Comserver socket连接；
	@param iPort    端口号
	@param hndEngine 内核进程句柄
	@param lRunEngine	运行中的内核；
	@Return True：连接成功
	*/
	BOOL RunEngine(CCommServer &Comserver, int iPort,HANDLE &hndEngine,long &lRunEngine);

	/*
	@breif  OBJECT网站需单线程执行
	@param	dwWebid 网站ID
	@param  lngRunCnt object类型网站数量
	@param  lngRuningCnt 运行中的数量
	@param  iEventCnt	事件对应任务索引的数量
	*/
	BOOL RunWebBySingle( DWORD dwWebid,long &lngRunCnt,int iEventCnt,long lngRuningCnt,BOOL bIsObject = FALSE);

	/*
	@brief 获取任务数据块数据
	@param stationData 当前网站数据；
	@param funData 功能块数据；
	@param curFunction 功能类型；
	@param pTask  任务数据；
	@param iStep  执行步骤索引；
	@return  任务块数据；
	*/
	CTaskData* GetTaskData(CStationData &stationData, CFunctionData &funcData, 
				E_StationFunction &curFunction,T_TaskData *pTask,long iStep,CStdString &strOCR, DWORD dwMailId);

	/*
	@brief 组织要发送给内核的数据
	@param dwWebID 网站ID;
	@param stationData 当前网站数据；
	@param pTData	任务块数据；
	@return 字符串数据
	*/
	CStdString GetSendData(DWORD dwWebID, CStationData &stationData, CTaskData *pTData, CStdString &strOCRParam);

	/*
	@brief 替换发送数据
	@param cParam 要用到的数据；
	@param strTaskData	任务数据；
	@param dwFalg; 0:产品替换名称，1.刷新商铺地址
	*/
	void ReplaceSendData(CommonParam &cParam, CStdString &strTaskData);

	/*
	@brief 单线程运行时清除运行数量
	@param iSingle  是否运行双内核；
	@param  lngRunCnt object类型网站数量
	@param  lngRuningCnt 运行中的数量
	*/
	void ClearRunObjectCnt(int iSingle, long &lngRunCnt, long &lngRuningCnt);

	/*
	@brief 结束主控
	@param eTaskType;
	*/
	void ExitMC(E_TASK_TYPE eTaskType);

	/*
	@brief 获取网站失败结果类型并返回
	@param pTask	任务数据；
	@param pTData   任务块数据；
	@param tDataOut 返回数据；
	@param vResult  失败集合；
	@return  返回需要执行的数据；
	*/
	int GetFailedResult(T_TaskData *pTask,CTaskData *pTData,T_DATA_OUT &tDataOut,std::vector<CStdString> &vResult);

	/*
	@brief 对执行成功网站分类处理
	@param pTask	任务数据；
	@param tDataOut 返回数据；
	*/
	int GetSuccResult(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief 公司名重复做替换操作
	@param strData  被替换的字符串；
	@param strReplace 要替换的字符； 
	@Return TRUE: 替换成功
	*/
	BOOL ReplaceCompName(CStdString &strData, CStdString &strReplace, int &iStep);

/*private:*/
	/*
	@breif 根据通配符找对象类型
	@param temp  通配符
	@return 变量类型
	*/
	int KeyToVal(CStdString& temp);

	/*
	@brief 将字符串中换行符超过3个的，换成只有三个；
	@param  strSource  源字符串；
	@return 
	*/
	void ReplaceNewLine(CStdString &strSource);

	/*
	@breif 获取内核返回的产品列表数据
	@param strSource 源数据
	@param strResult 要返回的数据
	*/
	void GetProductList( CStdString strSource, CStdString &strResult);

	/*
	@breif  根据通配符名取得值
	@param  strData 要查找的字符串
	@param  strName 通配符名
	@param  [out ]strValue值
	*/
	void GetValueByName(const CStdString &strData, const CStdString &strName, CStdString &strValue);

	/*
	@brief 对注册成功数据处理；
	@param pTask	任务数据；
	@param tDataOut 返回数据；
	*/
	void ProcessRegData(T_TaskData *pTask,T_DATA_OUT &tDataOut);

	/*
	@brief 对刷新成功数据处理；
	@param pTask	任务数据；
	@param tDataOut 返回数据；
	*/
	void ProcessRefreData(T_TaskData *pTask,T_DATA_OUT &tDataOut);

	/*
	@brief 对提取产品列表成功数据处理；
	@param pTask	任务数据；
	@param tDataOut 返回数据；
	@param dwTaskType 任务类型
	*/
	void ProcessGetPdtData(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief 对一键搬家成功数据处理；
	@param pTask	任务数据；
	@param tDataOut 返回数据；
	@param dwTaskType 任务类型
	*/
	void ProcessMigrateData(T_TaskData *pTask,T_DATA_OUT &tDataOut,DWORD dwTaskType);

	/*
	@brief 处理一键搬家通配符替换
	@param tDataOut  内核返回的数据
	@param strMigrateData 通配符字符串;
	*/
	DWORD ReplaceMigrateData(T_DATA_OUT &tDataOut, CStdString &strMigrateData);

	/*
	@brief 提取到的图片上传到服务器
	@param strParam   通配符；
	@param strParamValue 通配符的值;
	@param strMigrateData 要替换的字符串;
	@param dwCnt 替换失败数量；
	*/
	DWORD UpLoadPic(CStdString &strParam,CStdString &strParamValue,CStdString &strMigrateData,DWORD &dwCnt);

	/*
	@brief 对Win8以下系统进行提权
	*/
	BOOL RaisePrivileges();

	/*
	@brief 设置IE运行模式
	@param eVer IE版本
	*/
	void SetIECoreVersion(E_IeLimit eVer);

	/*
	@breif 判断是否64位系统
	*/
	BOOL IsWOW64();

	/*
	@breif 获取IE版本；
	*/
	int GetIEVersion();

private:
	/*std::vector<DWORD> vNsCodeOfObject; //包含对象的网站ID;*/
	BOOL   bIsWait;     //当前网站需单线程执行（针对OBJECT网站）
	
/*private:*/
	CNetData m_NetData;

	friend class CAllTaskData;
};

