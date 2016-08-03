#pragma once
#include <vector>
#include <deque>
#include "stdstring.h"
#include "common_enum.h"
#include <map>
#include <list>
/*#pragma pack(1)*/

 //struct  T_FILTER_STRUCTURE
 //{
 //	std::vector<CStdString > straAnd;
 //	std::vector<CStdString > straNot;
 //	std::vector<CStdString > straOr;
 //};


//上层传递给ExServer数据的结构体
struct T_Task_Data_Exser
{
	CStdString strEnginData; //引擎使用的数据
	CStdString strExOCRParam;//嵌入数据的验证码参数

	std::map<DWORD,CStdString> mapEmbedData; //嵌入数据
};

///////////////////////////////////////////////////////////////////////////
////////////////////  以下定义向上层返回结果的结构///////////////////////
///////////////////////////////////////////////////////////////////////////

//MC向Exserver读取数据的所用到的结构
//分为三类: 任务成功的结构体;任务失败的结构体;任务产生的中间数据

enum E_RESULT_INFO
{

	e_TASK_RESULT_AUTH_CODE_ERROR = 980,           //验证码错误
	e_TASK_RESULT_RECOMPANY = 981,				//公司名重复
	e_TASK_RESULT_SOME_SUCCESS = 982,            //部分成功
	e_TASK_RESULT_REEMAIL = 983,				//邮箱重复
	e_TASK_RESULT_ACCOUNT_NOT_AUDIT = 984,      //账号未审核
	e_TASK_RESULT_REG_LACK_INFO = 985,          //注册缺少资料
	e_TASK_RESULT_ADD_LACK_INFO = 986,          //完善缺少资料
	e_TASK_RESULT_POST_LACK_INFO = 987,         //发布缺少资料
	e_TASK_RESULT_REPHONENUMBER = 988,          //手机号重复
	e_TASK_RESULT_FAILUPLOADPIC = 989,			//上传图片失败

    e_TASK_RESULT_NETZIPPED_DATA_ERR = 990,        //准备数据失败
    e_TASK_RESULT_REGMAIL_FAIL = 991 ,//            '自动注册成功，但需要邮箱验证
    e_TASK_RESULT_REGAUDIT_FAIL = 992 ,//         '自动注册成功，但需要审核确认
	e_TASK_RESULT_ADDPICFAIL = 993, //				'完善图片上传失败     //14.6.25
    e_TASK_RESULT_INVALIDACCOUNT = 994      ,//            '登录失败
    e_TASK_RESULT_DUPLICATE = 995     ,//            '重复
    e_TASK_RESULT_FAIL = 996          ,//            '失败
    e_TASK_RESULT_POSTFULL = 997          ,//            '发满
	e_TASK_RESULT_ADDEXTRAFAIL = 998          ,//            '完善的资料失败
    e_TASK_RESULT_SUCC = 999          ,//            '成功

	
};
               


#ifndef STRUCT_INPUTDIALOG_IN_OUT_INFO
#define STRUCT_INPUTDIALOG_IN_OUT_INFO

struct T_TaskResultInfo
{
	E_RESULT_INFO eResultFlag;	//结果标识，用来判断该任务是成功还是失败
	std::list<CStdString> listOcrCode;          //验证码信息
	CStdString  strResultInfo;                   //结果信息
	CStdString  strHtml;                       //任务成功后的html快照文件内容
	CStdString	strCompPopular;               //公司知名度
	CStdString	strInfoID;                    //信息ID
	CStdString  strMailCount;                //邮件数量
	CStdString  strMigrateResult;            //返回给上层迁移数据结果(也可以做产品列表返回字段)
	DWORD		dwType;                      //一键搬家功能任务块 15：公司数据 16： 产品数据
	DWORD		dwSuccFieldCnt;				//迁移成功字段数量
	
	T_TaskResultInfo()
	{
		eResultFlag = e_TASK_RESULT_FAIL;		//默认任务是失败的
		dwType = 0;
		dwSuccFieldCnt = 0;
		strHtml.Empty();
		strCompPopular.Empty();
		strInfoID.Empty();
		strMailCount.Empty();
		strResultInfo.Empty();
		strMigrateResult.Empty();

	}
	~T_TaskResultInfo()
	{
 
	}
};


#endif

