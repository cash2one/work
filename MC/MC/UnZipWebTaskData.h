#pragma once

//发布数据结构
typedef struct _WEB_TASK_DATA_INFOPOST
{
	CStdString strPostInfo;          //发布内容信息
	CStdString strWebInfo;          //所有网站信息
	CStdString strSystemInfo;        //系统设置信息

	CStdString strConfigInfo;        //配置信息
	CStdString strAccountPassword;   //所有注册网站的账号密码
	DWORD dwPostType;                //任务类型

	CStdString strLocalProductImage;  //本地产品图片保存路径
	CStdString strLocalTradeImage;    //本地营业执照保存路径
	CStdString strLocalCompImage;    //本地公司照片保存路径
	CStdString strLocalOrgImage;     //本地组织代码图片路径
	CStdString strLocalTaxImage;	 //本地税务证书图片路径

	CStdString strServerMintus;      //服务器的时间(分钟数)
	CStdString strTimerTaskID;             //定时任务ID
}WEB_TASK_DATA_INFOPOST;

//注册数据结构
typedef struct _WEB_TASK_DATA_REGISTRY
{
	CStdString strWebInfo;          //所有网站信息
	CStdString strSystemInfo;        //系统设置信息
	CStdString strConfigInfo;        //系统设置信息

	CStdString strTradeImage;  //营业执照图片保存路径
	CStdString strLocalProctImage;   //产品图片  注册用不上
	CStdString strLocalCompImage;   //公司照片

	CStdString strServerMintus;      //服务器的时间(分钟数)
	CStdString strTimerTaskID;             //定时任务ID
}WEB_TASK_DATA_REGISTRY;

//注册数据结构
typedef struct _WEB_TASK_DATA_REFRESHANDDELETE
{
	CStdString strWebInfo;          //所有网站信息

	CStdString strConfigInfo;        //配置信息
	CStdString strAccountPassword;   //所有注册网站的账号密码

	CStdString strServerMintus;      //服务器的时间(分钟数)
	CStdString strTimerTaskID;             //定时任务ID
}WEB_TASK_DATA_REFRESHANDDELETE;

//域名推广
typedef struct _WEB_TASK_DATA_DOMAINPOST
{
	CStdString strWebInfo;          //所有网站信息
	CStdString strTaskInfo;         //任务数据
	CStdString strConfigInfo;        //配置信息
}WEB_TASK_DATA_DOMAINPOST;

//微博推广
typedef struct _WEB_TASK_DATA_WEBOPOST
{
	CStdString strWebInfo;          //所有网站信息
	CStdString strTaskInfo;         //发布内容
	CStdString strAccountPassword;   //所有注册网站的账号密码
	CStdString strConfigInfo;        //配置信息


}WEB_TASK_DATA_WEBOPOST;

//修改资料
typedef struct _WEB_TASK_DATA_INFOMODIFY
{
	CStdString strWebInfo;			//全部网站信息
	CStdString strSystemInfo;		//系统信息
	CStdString strConfigInfo;       //配置信息
	CStdString strAccountPassword;   //所有注册网站的账号密码

	CStdString strTradeImage;  //营业执照图片保存路径
	CStdString strLocalCompImage;   //公司照片

}WEB_TASK_DATA_INFOMODIFY;

class CUnZipWebTaskData
{
public:
	CUnZipWebTaskData(void);
	~CUnZipWebTaskData(void);
	
	//
	DWORD TransInfoPostToTaskData(const CStdString &strSource, WEB_TASK_DATA_INFOPOST &stWebTaskData);


	DWORD TransRegistryToTaskData(const CStdString &strSource, WEB_TASK_DATA_REGISTRY &stWebTaskData);

	DWORD TransRefreshAndDeleteToTaskData(const CStdString &strSource, WEB_TASK_DATA_REFRESHANDDELETE &stWebTaskData);

	DWORD TransDomainPostToTaskData(const CStdString &strSource, WEB_TASK_DATA_DOMAINPOST &stWebTaskData);

	DWORD TransWeboPostToTaskData(const CStdString &strSource, WEB_TASK_DATA_WEBOPOST &stWebTaskData);

	DWORD TransInfoModifyToTaskData(const CStdString &strSource, WEB_TASK_DATA_INFOMODIFY &stWebTaskData);

	/*
	@brief   保存产品图片与营业执照到本地
	@param   strProductImageName 产品图片路径
	@param   strTradeImageName 营业执照路径
	@return  
	*/
	DWORD SetTaskImageToGlobal(const CStdString& strProductImageName,const CStdString& strTradeImageName);

	/*
	@brief   保存产品信息到本地
	@param   产品信息字符串 
	@return  
	*/
	DWORD SaveProductToLocal(const CStdString& strData);

	/*
	@brief   保存系统信息到全局内存
	@param   系统信息字符串 
	@return  
	*/
	DWORD SaveSystemInfoToLocal(const CStdString& strData);

	/*
	@brief   保存发布信息到内存
	@param   网站信息字符串 
	@return  
	*/
	DWORD SavePostInfoToLocal(const CStdString &strData);
	
	/*
	@brief   保存网站信息到内存
	@param   网站信息字符串 
	@return  
	*/
	DWORD SaveWebInfoToLocal(const CStdString &strData);

	/*
	@brief   保存单任务网站信息到内存
	@param   网站信息字符串 
	@return  
	*/
	DWORD SaveWebInfoToLocalOfOneTask(const CStdString &strData);

	/*
	@brief   保存配置信息
	@param   配置信息字符串
	@return  
	*/
	DWORD SaveConfigInfoToLocal(const CStdString &strData);


	/*
	@brief   保存帐号密码等信息到内存
	@param   strData 帐号密码信息字符串 
	@return  
	*/
	DWORD SaveAccountPasswordToLocal(const CStdString& strData);

	/*
	@brief   保存域名推广任务信息
	@param   strData 域名推广任务信息字符串 
	@return  
	*/
	DWORD SaveDomainInfoToLocal(const CStdString& strData);

	/*
	@brief   保存微博任务信息
	@param   strData 保存微博任务信息 
	@return  
	*/
	DWORD SaveWeboInfoToLocal(const CStdString& strData);
};
