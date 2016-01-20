#pragma once
#include "StdString.h"


class CStation
{
public:
	CStation(void);
	~CStation(void);

	CStdString NScode; //网站编号
	CStdString NSname; //网站名称
	CStdString NSrstep; //注册步骤
	CStdString NSraction; //注册参数
	CStdString NSrconst; //
	CStdString NSrkeyword; //
	CStdString NSrreffer; //
	CStdString NSrsucc; //成功标记
	CStdString NSpstep; //发布步骤
	CStdString NSpaction; //发布参数
	CStdString NSpconst; //
	CStdString NSpkeyword; //
	CStdString NSpreffer; //
	CStdString NSpsucc; //成功标记
	CStdString NSdstep; //下载步骤
	CStdString NSdaction; //下载参数
	CStdString NSdconst; //
	CStdString NSdreffer; //
	CStdString nsMemo; //
	CStdString NSsstep; //搜索步骤
	CStdString NSsaction; //搜索参数
	CStdString NSsconst; //
	CStdString NSskeyword; //
	CStdString NSsreffer; //
	CStdString NSssucc; //成功标记
	CStdString nsDetailMark; //详细信息网址标记
	CStdString nsWebFilter; //网页头尾过滤标记
	CStdString nsValidUrlMark; //网址过滤参数
	CStdString nsCompanyFiter; //附加信息过滤标记: 信息组分隔<@@>主题<@@>正文<@@>公司<@@>邮件<@@>电话<@@>传真<@@>区域<@@>地址<@@>联系人<@@>简介<@@>网址<@@>行业
	CStdString nsNextPageMark; //下一页标记
	CStdString NShost; //主机地址
	CStdString nsAdvanceSearchSymbol; //搜索转义符号
	CStdString NSPublishDate; //
	DWORD NSClass; //网站评级
	//CStdString NSAccount; //
	//CStdString NSPass; //
	//CStdString NSrFlag; //
	//CStdString NSpFlag; //
	//CStdString NSsFlag; //
	//CStdString NSeFlag; //
	CStdString IsDemo; // 是否为演示版本网站
	CStdString nsEaction; //ID邮件参数
	CStdString nsEconst; //
	CStdString nsEkeyword; //
	CStdString nsEreffer; //
	CStdString nsEstep; //ID邮件步骤
	CStdString nsEsucc; //成功标记
	CStdString nsERefreshDate; //
	//CStdString nsLFlag; //
	CStdString nsLAction; //
	CStdString nsLConst; //
	CStdString nsLKeyword; //
	CStdString nsLreffer; //
	CStdString nsLstep; //
	CStdString nsLsucc; //登陆成功关键字
	CStdString nsLRefreshDate; //
	CStdString nsDeepDetailMark; //szRelateUrlFilter
	CStdString nsPRefreshDate; //
	CStdString nsSRefreshDate; //
	CStdString nsRRefreshDate; //
	CStdString nsPRePostKey; //重复发布关键字
	CStdString nsRRePostKey; //重复注册关键字
	CStdString nsoperator; //
	CStdString RegWebPath; //
	CStdString NsOnlyDown; //仅搜索
	CStdString nsDisplay; //是否立显标记
	CStdString nstype; //发布类型
	CStdString nsonlypost; //仅发布
	CStdString nsarea; //
	CStdString nsScriptExplain; //ScriptUrl特征字符串
	CStdString nsWebRedirect; //页面重新定向
	CStdString nsMainHost; //主网站地址
	CStdString nsLMRefreshDate; //
	CStdString nsBLRefreshDate; //
	CStdString nsNNTFileName; //模板文件名称
	CStdString nsImageProcessCode; //图像预处理方式
	CStdString NsNeedFitLanmu; //
	CStdString nsTeleZone; //
	CStdString nsImageProcessRate; //
	CStdString nsIsValidForDemoOrTrial; //
	CStdString nsBaseInfoLastUpdate; //保存的网站基本信息的最后更新时间
	CStdString nsVariableLastUpdate; //保存的网站变量的最后更新时间
	CStdString nsItemLastUpdate; //保存的网站栏目的最后更新时间
	CStdString nsEnabledUpdatedInvalid; //
	CStdString nsRowLastupdated; //
	CStdString nsRowLastupdatedBigint; //
	CStdString nsNetArea; //所属区域
	CStdString nsLMRefreshDateTemp; //
	CStdString nsBLRefreshDateTemp; //
	CStdString nsRowLastupdatedBigintTemp; //
	

	CStdString szFixCookies                ;// String                   //固定的，只在指定步骤才使用的cookies
	CStdString ProgressData                ;// String                   //新增，记录session数据
	BOOL blnLocalDataIsNew           ;// Boolean                  //true表示本地数据已经是最新的
	DWORD PExeNumber                   ;                     //已存在的任务数(仅发布时使用)

	int GetDataFromString(CStdString strData);   //通过字符串填充类的数据

    int GetItemFromString(const int iNscode,const CStdString strNscode,const CStdString strData,T_NetstationItemGroup &NetItem);


    int GetAllStationLastUpdateALL(const CStdString strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup);
};
