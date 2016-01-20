#pragma once

#define _NEW(p, type) \
    (p) = new type;\
    if (!(p))\
    {\
    printf("new operator fail!!");\
    }

#define _NEWA(p, type, count) \
    (p) = new type[count];\
    if (!(p))\
    {\
	printf("new operator fail!!");\
    }

#define _DELETE(p) \
    if (p)\
    {\
    delete (p);\
    (p) = NULL;\
    }

#define _DELETEA(p) \
    if (p)\
    {\
    delete[] (p);\
    (p) = NULL;\
    }

const DWORD BIZMC_OK = 0;                    //主控函数成功正常调用的返回值 ,统一正常为0
const DWORD BIZMC_FAILURE = 1;                  //主控函数非正常结束的返回值(前置条件或后置条件校验不正常,通常是参数校验不正确)
const DWORD MAX_MID_LENGTH = 50;          //中间字符长度
const DWORD MAX_LOW_LENGTH = 10;         //长度较短字符串
const DWORD MAX_NAME_LENGTH = 300; //帐号密码的最大字符串
const DWORD MAX_SYSTEM_INFO_DEFAULT_TCHAR_COUNT = 20;   //系统设置信息的默认字符数
const DWORD MAX_SUBJECT_LENGTH = 2000; //任务主题的最大字符数
const DWORD MAX_PRODUCT_NORMAL_LENGTH = 50;   //产品其他信息大字符数
const DWORD MAX_PRODUCT_DETAIL_LENGTH = 5000; //产品详细信息的最大字符数
const DWORD MAX_CONTEXT_LENGTH = 10000; //任务内容的最大字符数
const DWORD MAX_ACCOUNT_PASSWORD_LENGTH = 50; //帐号密码的最大字符串
const DWORD MAX_URL_LENGTH = 600; //文件路径的最大字符数
const DWORD MAX_TASK_ITEM_SINGLE_NET_COUNT = 200000; //每个网站之间间隔的数量,即最多派生的任务数.也就是最多发布的产品数量
const DWORD MAX_TASK_ITEM_COUNT = 1000; //最大任务涉及的网站数据条数
const DWORD MAX_SYSTEM_SETUP_INFO_DATA_COUNT = 252; //系统设置信息数组的最大值
const DWORD MAX_EMAIL_NAME_LENGTH = 50; //电子邮件地址的最大字符数
const DWORD MAX_FILE_PATH_LENGTH = 300; //文件路径的最大字符数
const DWORD MAX_TASK_ITEM_COMPANY_TCHAR_COUNT = 100000;   //单个页面返回的公司信息的字符数
const DWORD MAX_RETURN_TO_ACTIVEX_RESULT_COUNT = 200;   // 一次最多返回给GUI的任务结果的数量
const DWORD MAX_TASK_ITEM_RESULT_EMAIL_COUNT = 120;   //单个页面返回的最大的邮件地址和名称的数量
const DWORD MAX_NET_COUNT_LASTUPDATED = 2000;  //从服务器一次获取的最后更新时间的最大网站数.
const DWORD USER_ID_COUNT = 16;       //用户ID的最大字符数
const DWORD MAX_ID_STRING_LENGTH = 21;          //ID转换为字符串的最大值 2^64 为 20字符,加上结束符合21个
const DWORD CLIENTID_TYPE_INTERVAL = 268435456;    //clientID类型之间的间隔值
const DWORD MAX_TICK_COUNT_LOCAL = 1728000000;//最大的时钟计数


const TCHAR * const  REGISTER_NETSTAION_LIST  = _T("A9999");
const TCHAR * const  NOT_REGISTER_NETSTAION_LIST  = _T("A9998");
const TCHAR * const  GUESTBOOK_NETSTAION_LIST = _T( "A333");
const TCHAR * const  PROMOTION_NETSTAION_LIST = _T( "A888");
const TCHAR * const  NETSTAION_LASTUPDATE_LIST = _T( "StationLastupdate");

const TCHAR * const  NETSTATION_LIST_SPLIT  = _T("(;N)");
const TCHAR * const  SIMPLE_LIST_SPLIT  = _T("(;0)");
const TCHAR * const  ACTIVE_LIST_SPLIT  = _T("(;10)");
const TCHAR * const  PRE_SPLIT          = _T("(;");
const TCHAR * const  TAIL_SPLIT         = _T(")");
const DWORD LOCALGLOBALDATAINDEX = 1000; //全局数据T_LocalGlobalData节点编号
const DWORD MAG_ACCOUNT_INDEX = 999; //管理帐号密码的map索引  add by yzh
const DWORD MAG_STATION_INDEX = 998; //管理NStation的索引     add by yzh

const DWORD PREFIX_DATA_SOCKET_BYTES_LENGTH = 8;  // 监听SOCKET 的实际数据的前缀的字节数

const TCHAR * const GET_SERVERLIST_SUCCESSKEY_CHAR  = _T("<string xmlns=\"wwwsum\"");    //和本地服务器交互成功的关键字
