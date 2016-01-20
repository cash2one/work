#ifndef _NEW_MAIN_CTRL_COMMON_ENUM_H_
#define _NEW_MAIN_CTRL_COMMON_ENUM_H_
enum E_THIRD_CHAR_TYPE
{
	E_INVALID_TYPE = 0,
	NEED_EMAIL_ACTIVE = 1,
	GET_NORMALLY,
	GET_USERNAME_FROM_FITKEY,
	GET_PSD_FROM_FITKEY,
	GET_BOTH_FROM_FITKEY,
	NEED_CHECK_INFORM,
	THIRD_VALUE_UNKNOWN,//始终放在最后
};
enum E_Task_Level //任务级别
{
	ETOtherEmpty,			//保留最高级
	ETPostSync,				//发布任务同步
	ETPostAsync,			//发布任务异步
	ETRegaccountAsync,		//账号注册任务异步
};
//数据源从那里来,并带有更细节的信息
enum E_DataFrom
{
	E_DATAFROM_BEGIN = 99,
/******************** GUI相关 ***********************************/
    E_FROMGUI_BEGIN,
    E_INIT_CLIENTID,//初始化clientid
    E_STOP_TASK,//停止任务
    E_PUSH_NORMAL_TASK_DATA,//压一般的任务
    E_PUSH_REGISTER_TASK_DATA,//压注册任务
	E_GET_ERROR_MESSAGE_CODE,  //获取错误信息提示ID
	E_GET_TASK_STATE_ITEM_FILE, //获取栏目文件任务状态
	E_GET_EXCUTE_TASK_RESULT, //获取当前任务结果的信息


    E_FROMGUI_END = 999,


/******************** 内部用到的控制信息 *****************************/


    E_DATAFROM_END,
};


//取栏目网站列表类型
enum ItemList_ExType
{
	ItemListAccountManage = 6,                   //6:网站帐号管理
	//在这里添加其它栏目网站列表类型
};

//任务类型 added for chenqs
enum E_TASK_TYPE
{
	ETTypeNone = -1,
	ETTypePost,				//发布任务
	ETTypeRegaccount,		//账号注册任务
	ETypeAutoLogin,         //自动登陆
	ETypeRefresh,         //刷新
	ETypeDelete,         //删除
	ETypeDomainpost,    //域名推广
	ETypeWebopost,    //微博推广
	ETypeCompanyPopular, //公司知名度
	ETypeRegEmail,	    //邮箱注册
	ETypeMailCount,   //邮件数量查询
	ETTypeReRegaccount,		//重新账号注册任务
	ETTypeUserInfoMigrate, //一键搬家
	ETTypeModify,			//修改资料
	ETTypeGetPdtList,	//提取产品列表
	ETTypeMailActivate, //邮箱激活
	ETTypeShopTraffic, //刷新商铺
};



//发布任务的 细分类型
enum Post_Type
{
    PT_TradePost = 0,                            //商贸供求发布
    PT_ProductPost = 1,                          //产品发布发布
    PT_SearchEnginePost = 2,                     //搜索引擎发布
    PT_BbsPost = 3,                              //bbs发布           
    PT_Search = 4,                               //一般搜索
    PT_GetMessage = 5,                           //接收留言          
    PT_PostMessage = 6,                          //回复留言          
    PT_Translate = 7,                            //翻译
    //在这里添加其它发布任务的细分类型
};

enum E_DataUseState
{
    eDataUseStateBegin = -1,
    eWithoutUse = 0 ,//没有被任何代码段操作,
    eUseBlockWithOne,//被一代码段进行写操作(阻塞式的)
    eUseCommReadWithMulti,//同一时间被多代码段进行读操作(非阻塞式的)
    eWaitting,//等待状态,等待全部读操作完成,
	eUseCommRT,//RT 态(实时模式,非阻塞)
    eDataUseStateEnd,
	
};



//数据组织类用到的各个代码段(类级别)的枚举
enum E_CODE_INDEX
{
    E_PREPAREDATA_CLASS = 1,//PrepareData类
    E_THREADWORK_CLASS,//ThreadWork类
    E_THREADMANAGER_CLASS,//ThreadManager类
	E_THREADTASKMGR_CLASS,//TaskMgrThread类
	E_GUIDATAIN_CLASS, 
	E_Save_CLASS,
	E_MAINCTRL_ClASS,
        
    E_WEBSERVICE_CLASS,  //获取文件列表信息准备数据的过程,Cwebservice
};


//线程类别 add by yzh
enum E_ThreadType
{
	E_TASKMGR_THREAD = 0x01,    //任务管理线程
	E_4KERNELWORK_THREAD = 0x02,       //4代核线程

	E_DATAPRE_THREAD = 0x05,    //数据准备线程
	E_SAVE_THREAD = 0x06,       //保存任务数据线程
    E_WEBSERVICE_THREAD = 0x07, //WebService线程
	E_UPDATE_THREAD = 0x08,     //网站数据升级线程
};

//消息类型 add by yzh
enum E_MessageType
{
	//公共
	eMTypeNone = -1,
	E_NEWERR,//用new申请内存失败

	//任务分配的
	eTask_PushGUITask,	//GUI压任务
	eTask_PrepareFailed,	//数据未准备的数据
	eTask_PrepareFinished,  //数据准备完成的
	eTask_TaskSucceed,		//任务成功结束
	eTask_TaskFailed,		//任务失败结束

	//数据准备线程
	E_PPD_START_MSG,//开始准备
	E_PPD_GETNETDATAFROMSERVER_MSG,//需要从server获取信息
	E_PPD_SERVERRETURNDATA_MSG,//server返回信息
	E_PPD_UNKNOWN_MSG,//未知消息
	E_PPD_NETCODELESSZERO_MSG,//网站编号小于0(一般不会有)
	E_PPD_CONNDBOK_MSG,//连接DB成功

	E_PPD_RETURNNETSTATIONBASEINFO_MSG,//返回单个网站的基本信息
	E_PPD_SERVERRETURNERR_MSG,//数据server更新数据失败
	E_PPD_LASTUPDATE_GETTED_MSG,    //网站最后更新时间已经获取了
	E_PPD_LASTUPDATEGETERR_MSG,//从全局区读网站最后更新时间失败
	E_PPD_NOWEBDATA_MSG,//没有网站数据
	E_PPD_RESTART_MSG,//任务重新准备消息，不更新数据
	E_PPD_CHECKISNULL_MSG,//验证任务帐号为空
	E_PPD_SERVER_RETURN_BASE_ERR_MSG,//数据server更新数据失败 基本信息

	//SaveThread线程相关的消息
	E_SAVE_INITDATASAVE, 
	E_SAVE_TASKFINISHED,
	E_SAVE_FIRSTDATA,

    //WebService线程相关的消息
    E_WEBS_START,         //webservice 开始消息
	E_WEBS_INITDOWNLOADFILE,             //初始化时下载必需文件
    E_WEBS_GET_NETSTATION_ITEM,          //获取单个网站的栏目信息
    E_WEBS_GET_NETSTATION_BASEINFO,      //获取单个网站的基本信息
    E_WEBS_GET_ALL_NETSTATION_LASTUPDATE,  //获取所有网站的最后更新时间

	E_WEBS_LASTUPDATE_GETTED,       //网站最后更新时间已经获取了
	E_WEBS_GET_NETSTATION_INFO_SAVED,      //保存完成网站的信息

	//更新线程相关信息
	E_FTP_GET_NETSTATION_BASEINFO_SAVED,      //FTP保存完成单个网站的基本信息
	E_FTP_GET_FILE_FROM_SERVER,				//FTP获取服务器文件
	E_HTTP_GET_FILE_FROM_SERVER,
	E_FTP_GET_FILE_FROM_SERVER_SAVED,		//FTP保存完成服务器文件

	//引擎线程相关信息
	E_4KERNEL_TASK_IN,     //新引擎任务
	E_4KERNEL_TASK_OUT,   //有任务结果读

};

enum CodeOCRResultType
{
	UNKNOW,  
	AUTO_ERROR_BUT_CORRECT_IN,
	AUTO_ERROR_AND_ERROR_IN,						//	判断不出来
	AUTO_ERROR_BUT_NO_IN,
	AUTO_CORRECT_AND_NO_IN,
	AUTO_ERROR_AND_UNKNOW_IN,
	NO_CONTENT,												//同atuo_error_but_no_in
};
 
enum E_ACCOUNT_STATE
{
	E_ACCOUNT_STATE_VALID = 1       ,              //有效
	E_ACCOUNT_STATE_INVALID = 2     ,              //无效
	E_ACCOUNT_STATE_AUDITING = 3    ,              //审核中
	E_ACCOUNT_STATE_EMAILCONFIRM = 4,              //邮件接收
	E_ACCOUNT_STATE_NOTNEEDREG = 5  ,              //不需要注册
	E_ACCOUNT_STATE_NEEDREG = 6     ,              //需要注册，但没有注册的网站
		
};

#endif