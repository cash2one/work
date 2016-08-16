//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
// 版本：
// 文件说明：执行控制类，用于控制数据块的步骤执行流程
// 生成日期：
// 作者：何培田
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//
/////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BlockData.h"
#include "TaskData.h"
#include "CommClient.h"
#include <mshtmdid.h>
#include <stack>

#define STEP_HTML_OF_ONE_CYCLE std::vector<CStdString>
#define HISTORY_STEP_HTML std::vector<STEP_HTML_OF_ONE_CYCLE>

class CSumengine2View;

//处理步骤结果的相关数据
struct T_STEP_CTRL_PARAM
{
	CSumengine2View *pActiveView;      //任务视图 
	int nTimeoutRetry;                 //已重试超时次数
	int nFailRetry;                    //已重试失败次数
	bool bVertifyFail;                 //是不是由于验证码成功标记失败导致失败
	long lngActiveStep;                //当前步骤
	bool bDoNextStep;                  //是否继续下一步
	E_RESULT eStepRes;                 //步骤执行结果
  	CStepData objCurStep;              //当前步骤对象
	bool bBreak;                       //是否停止执行剩余步骤
	int nViewSwitchTimes;              //切换视图的次数
	CSumengine2View *pFirstFailView;   //保存切换视图前的活动视图

	void Reset()
	{
		pActiveView = NULL;
		nTimeoutRetry = 0;
		nFailRetry = 0;
		lngActiveStep = 0;
		bDoNextStep = false;
		objCurStep.Clear();
		bBreak = false;
		nViewSwitchTimes = 0;
		pFirstFailView = NULL;
		bVertifyFail = false;
	}
};

//保存任务视图的相关信息
struct T_TASK_VIEW_INFO
{
	CSumengine2View *pView;    
	DWORD dwCreationTime;          //任务视图创建时间
	int iFailCount;                //查找对象失败的次数

	T_TASK_VIEW_INFO()
	{
		pView = NULL;
		dwCreationTime = 0;
		iFailCount = 0;
	}
};

class CExecMgr
{
public:
	CExecMgr(CCommClient *pCommChannel, CStdString strTaskID);
	~CExecMgr(void);

	//压入执行数据块
	void PushBlockData(CStdString strData, CStdString strNsName);	
	//停止任务关闭和其相关的视图资源
	bool Stop(void); 
	void SetStopFlag(void);
	CStdString GetTaskWebName();
	BOOL IsMutexConfilict();
	void ReleaseConfilictMutex();
	void SendConfilictResultToMC();
private:
	CStdString m_strTaskID;                 //管理模块对应的任务ID，用于日志记录
	CCommClient *m_pCommChannel;            //与上层通信的SOCKET接口
	std::vector<CStdString> m_vBlockData;   //缓存数据块
	CLock m_lock;
	HANDLE m_hThread;
	bool m_bStop;
	int m_nOneTaskMaxPage;                  //单个任务允许的最大页面数
	CLogTrace m_log;                        //存放本网站操作日志
	CStdString m_strNsName;                 //网站名称
	DWORD m_dwAmbientVal;                   //是否屏图等标记
	CStdString m_strOcrResultInfo;          //验证码输入信息
	int m_nRunMode;                         //运行模式：采编/显示/正常
	int m_nBlockTargetLoopTimes;            //执行块的最大循环次数
	int m_nCurVLIndexValue;                 //当前Block执行在第几次，这个值决定运行时步骤指定的值
	int m_nOrcResultFailTimes;              //每次block由于验证码成功标记失败就会增加一次
	int m_nRestartFrom1thStepTimes;         //成功标记失败后从头执行的次数(需要成功标记设置失败后从头执行的标记)

	bool m_bExitProcessOneCircle;
	bool m_bIdentifyOK;             //识别验证码步骤是否OK
	CSumengine2View *m_pActiveView;                    //本任务的活动视图指针，优先在此页面执行
	std::vector<CSumengine2View *> m_vTaskViewWnd;     //本任务产生的所有视图
	std::map<CStdString, CStdString> m_mapIIDKs;                   //本任务的动参表
	std::map<CSumengine2View *, T_TASK_VIEW_INFO> m_mapTaskInfos;   //保存所有任务视图的相关相信息
	CStdString m_strQuickLook;									 //任务快照
	HANDLE m_hTaskConfilict;									 //网站任务重合判断互斥体		

	STEP_HTML_OF_ONE_CYCLE m_vStepHTML;     //一次循环所执行的所有对象的HTML
	bool GetFrontBlockData(CStdString &strData);  
	void ParseBlockLoopSetting(const CBlockData &blockData);
	int GetSubBlockLoopTimes(const CBlockData &blockData);//子块需要重复操作的次数
	void Exec(T_DATA_IN dataIn);          //执行数据块（步骤集）
	CStdString m_strParamOCR;//验证码参数

	void ReplaceLoopIndex(CStdString &strDataSet, int &nLoopTimes, CBlockData &objBlockData) ;  //替换循环执行块中通配符%LoopIndex%的值，以循环序号定位页数

	bool m_bFirstModuleFail;
	bool ProcessOneCycle(CBlockData &objBlockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strRetDataSet, int &iCycleTimes, BOOL bIsSubRandom = FALSE);

	//返回执行结果
	void SendExecResult(const CStdString &strCmd, const CStdString &strRetDataSet, T_STEP_CTRL_PARAM &tParam);

	//需要跳转到那个模块
	int JumpToWhichModule(const CBlockData &objBlockData, int curIndex, int nBlockCount, bool bSuccess);

	//判断条件跳转的条件是否成立
	bool IsConditionJumpOK(const CBlockData &objBlockData);
	
	//判断类型为eltLoopMax的循环是否结束
	bool IsUnlimitedLoopEnd(STEP_HTML_OF_ONE_CYCLE &vStepHTML, HISTORY_STEP_HTML &vHistory);

	//判断有限次循环是否结束
	bool IsLimitedLoopEnd(int iCurLoopTimes);

	//检查是否是多成功标记的情况（当前失败步骤为成功标记，
	//且下一步也是成功标记的返回true，否则返回false)
	bool IgnoreStepError(CBlockData &blockData, int iCurStep); 

	//检查验证码是否正确，并根据情况重试
	bool HandleOcrResult(T_STEP_CTRL_PARAM *pParam, CBlockData &blockData, long &nStepIndex);
	void GetORCFlag(bool bSucc, T_STEP_CTRL_PARAM *pParam, int &nAutoReg, int &nLongRange, int &nManmade, int &nVerifyType);

	//检查步骤控制信号，直到可以继续执行
	//显示模式支持自动暂停(失败后)及手动暂停;
	//采编模式支持手动暂停
	void WaitForStepContinueSignal(void);

	//步骤失败后是否需要暂停执行
	bool PauseStepExec(T_STEP_CTRL_PARAM &tStepCtrl, BOOL bBlockFaultTolerant);

	//步骤执行(容错)成功，准备下一步操作
	void PrepareDoNextStep(T_STEP_CTRL_PARAM &tStepCtrl);

	void TolerantFailRet(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData);

	//延时
	void Wait(int nTime);
	//处理通配符延时操作#Random-x:y#
	void Wait(const CStdString& strSleepTime);

	//获取更大随机数
	unsigned long ulrand(void);

	//从失败视图中提取页面的弹框提示信息
	CStdString GetUiMsgFromFailedViews(void);

	//跳过成功标记步骤
	void MoveToNextStep(CBlockData &blockData, long &tarStepIndex);

	//跳到操作为“下一页”的步骤，跳转成功返回true，否则false
	bool SkipToNextPageStep(CBlockData &blockData, long &tarStepIndex);

	void HandleRecognizeResult(T_STEP_CTRL_PARAM &tStepCtrl);
	void DealStepResult(CBlockData &blockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strReturnData, int &iVagueIndex, bool bFirstCall = true);

	//一下对DealStepResult方法进一步封装(各种结果情况进行处理)
	void HandleStepResultOK(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData);

	void HandleStepResultTimeOut(T_STEP_CTRL_PARAM &tStepCtrl);

	void HandleStepResultFinishError(T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData);

	void HandleStepResultFail(E_RESULT res,T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData);

	void HandleStepResultOutofRange(CBlockData &blockData, T_STEP_CTRL_PARAM &tStepCtrl, CStdString &strBlockReturnData, int &iVagueIndex);

	//检查视图是否存在
	bool IsViewExist(CSumengine2View *pView); 

	//保存页面快照，bFlag指定是成功快照还是失败快照
	void SaveQuickLook(CSumengine2View *pView, BOOL bFlag);

	void UpdateTaskViews(void);           //获取当前任务的所有视图
	bool IsInvalidViewCreatedByOnNewWindow2(CSumengine2View *pView);  //检查视图是否由OnNewWindow2打开且Document已经被释放了
	CSumengine2View *GetTaskView(bool bSwitchActiveView);

	//检测是否有需要关闭的视图，如有则关闭之（其它条件满足时默认关闭后打开的页面）
	void CloseUnusedViews(CSumengine2View *&pActiveView, bool bCloseLatestOpen = true);          

	void ReportException(void);           //异常时处理

	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

	//获取随机步骤
	void GetRandomVec(int iRandomCount,int iStepCount,vector<int>&vecRandomStep);

	BOOL IsMatchStep(int iStepindex, vector<int>&vecRandomStep);

	//等待CPU使用率下降到指定值
	void WaitForCpuUsage();
};

