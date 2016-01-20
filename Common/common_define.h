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

const DWORD BIZMC_OK = 0;                    //���غ����ɹ��������õķ���ֵ ,ͳһ����Ϊ0
const DWORD BIZMC_FAILURE = 1;                  //���غ��������������ķ���ֵ(ǰ���������������У�鲻����,ͨ���ǲ���У�鲻��ȷ)
const DWORD MAX_MID_LENGTH = 50;          //�м��ַ�����
const DWORD MAX_LOW_LENGTH = 10;         //���Ƚ϶��ַ���
const DWORD MAX_NAME_LENGTH = 300; //�ʺ����������ַ���
const DWORD MAX_SYSTEM_INFO_DEFAULT_TCHAR_COUNT = 20;   //ϵͳ������Ϣ��Ĭ���ַ���
const DWORD MAX_SUBJECT_LENGTH = 2000; //�������������ַ���
const DWORD MAX_PRODUCT_NORMAL_LENGTH = 50;   //��Ʒ������Ϣ���ַ���
const DWORD MAX_PRODUCT_DETAIL_LENGTH = 5000; //��Ʒ��ϸ��Ϣ������ַ���
const DWORD MAX_CONTEXT_LENGTH = 10000; //�������ݵ�����ַ���
const DWORD MAX_ACCOUNT_PASSWORD_LENGTH = 50; //�ʺ����������ַ���
const DWORD MAX_URL_LENGTH = 600; //�ļ�·��������ַ���
const DWORD MAX_TASK_ITEM_SINGLE_NET_COUNT = 200000; //ÿ����վ֮����������,�����������������.Ҳ������෢���Ĳ�Ʒ����
const DWORD MAX_TASK_ITEM_COUNT = 1000; //��������漰����վ��������
const DWORD MAX_SYSTEM_SETUP_INFO_DATA_COUNT = 252; //ϵͳ������Ϣ��������ֵ
const DWORD MAX_EMAIL_NAME_LENGTH = 50; //�����ʼ���ַ������ַ���
const DWORD MAX_FILE_PATH_LENGTH = 300; //�ļ�·��������ַ���
const DWORD MAX_TASK_ITEM_COMPANY_TCHAR_COUNT = 100000;   //����ҳ�淵�صĹ�˾��Ϣ���ַ���
const DWORD MAX_RETURN_TO_ACTIVEX_RESULT_COUNT = 200;   // һ����෵�ظ�GUI��������������
const DWORD MAX_TASK_ITEM_RESULT_EMAIL_COUNT = 120;   //����ҳ�淵�ص������ʼ���ַ�����Ƶ�����
const DWORD MAX_NET_COUNT_LASTUPDATED = 2000;  //�ӷ�����һ�λ�ȡ��������ʱ��������վ��.
const DWORD USER_ID_COUNT = 16;       //�û�ID������ַ���
const DWORD MAX_ID_STRING_LENGTH = 21;          //IDת��Ϊ�ַ��������ֵ 2^64 Ϊ 20�ַ�,���Ͻ�������21��
const DWORD CLIENTID_TYPE_INTERVAL = 268435456;    //clientID����֮��ļ��ֵ
const DWORD MAX_TICK_COUNT_LOCAL = 1728000000;//����ʱ�Ӽ���


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
const DWORD LOCALGLOBALDATAINDEX = 1000; //ȫ������T_LocalGlobalData�ڵ���
const DWORD MAG_ACCOUNT_INDEX = 999; //�����ʺ������map����  add by yzh
const DWORD MAG_STATION_INDEX = 998; //����NStation������     add by yzh

const DWORD PREFIX_DATA_SOCKET_BYTES_LENGTH = 8;  // ����SOCKET ��ʵ�����ݵ�ǰ׺���ֽ���

const TCHAR * const GET_SERVERLIST_SUCCESSKEY_CHAR  = _T("<string xmlns=\"wwwsum\"");    //�ͱ��ط����������ɹ��Ĺؼ���
