//////////////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2007, 商讯网信息有限公司
// 版本：
// 文件说明：
// 生成日期：
// 作者：
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//


#include "common_stru.h"
#include "common_stru_exserver.h"
#include "StdString.h"
#include "IThreadUnit.h"

class CWebService :public IThreadUnit
{
	private:
	enum { 
		NETSTATION_ALL_LIST_TYPE = 99,

	}; // 枚举常量

	DWORD dwIsAllNetstationLastUpdateGetted;
	DWORD m_dwAllStationLastUpdateGetting ;//正在获取全部网站的最后更新时间
	DWORD m_dwWebServiceAllStationLastUpdateSended ; //服务器缓存的网站帐号密码的消息已经发送了,
	TCHAR m_szCurrPath[MAX_PATH] ;		//当前目录
	
	DWORD m_dwNetLastupdateIndex;       //网站最后更新时间的全局数据索引, 和PPD交互用的
	
public:
	CWebService(void);
	~CWebService(void);
	//线程初始化
	virtual DWORD CreateThread();  //{return 0;}
	//线程退出
	virtual DWORD DestoryThread(){return 0;}	
	DWORD DispatchMessage(T_Message *Msg);


private:

	void GetWebServiceData(DWORD dwIndex);     //自动同步和webservice 交互,获取必要的数据
	
	void GetAllStationLastUpdateALL();

	DWORD GetAllStationLastUpdateALLReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile);//获取栏目最后更新时间

	DWORD SendToFtpGetServerFile(const int dwItemListCode,const int dwListType ,const DWORD dwAuto);
	DWORD GetServerFileFromFtp(const DWORD dwDataIndexID);

    DWORD GetAllStationLastUpdateALLOK(const CStdString &Source);	

	DWORD GetAllStationLastUpdateALLGettedPPD(const DWORD dwDataIndexID);            


	DWORD GetNetDataFromFTP(const DWORD dwMessageType,const DWORD dwWebID);

	DWORD SaveLastUpdateToMap(DWORD dwIndex,const CStdString &strData);

    DWORD GetNetstationAllInfoReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile); //获取网站信息数据文件的返回
 };
