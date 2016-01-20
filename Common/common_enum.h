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
	THIRD_VALUE_UNKNOWN,//ʼ�շ������
};
enum E_Task_Level //���񼶱�
{
	ETOtherEmpty,			//������߼�
	ETPostSync,				//��������ͬ��
	ETPostAsync,			//���������첽
	ETRegaccountAsync,		//�˺�ע�������첽
};
//����Դ��������,�����и�ϸ�ڵ���Ϣ
enum E_DataFrom
{
	E_DATAFROM_BEGIN = 99,
/******************** GUI��� ***********************************/
    E_FROMGUI_BEGIN,
    E_INIT_CLIENTID,//��ʼ��clientid
    E_STOP_TASK,//ֹͣ����
    E_PUSH_NORMAL_TASK_DATA,//ѹһ�������
    E_PUSH_REGISTER_TASK_DATA,//ѹע������
	E_GET_ERROR_MESSAGE_CODE,  //��ȡ������Ϣ��ʾID
	E_GET_TASK_STATE_ITEM_FILE, //��ȡ��Ŀ�ļ�����״̬
	E_GET_EXCUTE_TASK_RESULT, //��ȡ��ǰ����������Ϣ


    E_FROMGUI_END = 999,


/******************** �ڲ��õ��Ŀ�����Ϣ *****************************/


    E_DATAFROM_END,
};


//ȡ��Ŀ��վ�б�����
enum ItemList_ExType
{
	ItemListAccountManage = 6,                   //6:��վ�ʺŹ���
	//���������������Ŀ��վ�б�����
};

//�������� added for chenqs
enum E_TASK_TYPE
{
	ETTypeNone = -1,
	ETTypePost,				//��������
	ETTypeRegaccount,		//�˺�ע������
	ETypeAutoLogin,         //�Զ���½
	ETypeRefresh,         //ˢ��
	ETypeDelete,         //ɾ��
	ETypeDomainpost,    //�����ƹ�
	ETypeWebopost,    //΢���ƹ�
	ETypeCompanyPopular, //��˾֪����
	ETypeRegEmail,	    //����ע��
	ETypeMailCount,   //�ʼ�������ѯ
	ETTypeReRegaccount,		//�����˺�ע������
	ETTypeUserInfoMigrate, //һ�����
	ETTypeModify,			//�޸�����
	ETTypeGetPdtList,	//��ȡ��Ʒ�б�
	ETTypeMailActivate, //���伤��
	ETTypeShopTraffic, //ˢ������
};



//��������� ϸ������
enum Post_Type
{
    PT_TradePost = 0,                            //��ó���󷢲�
    PT_ProductPost = 1,                          //��Ʒ��������
    PT_SearchEnginePost = 2,                     //�������淢��
    PT_BbsPost = 3,                              //bbs����           
    PT_Search = 4,                               //һ������
    PT_GetMessage = 5,                           //��������          
    PT_PostMessage = 6,                          //�ظ�����          
    PT_Translate = 7,                            //����
    //����������������������ϸ������
};

enum E_DataUseState
{
    eDataUseStateBegin = -1,
    eWithoutUse = 0 ,//û�б��κδ���β���,
    eUseBlockWithOne,//��һ����ν���д����(����ʽ��)
    eUseCommReadWithMulti,//ͬһʱ�䱻�����ν��ж�����(������ʽ��)
    eWaitting,//�ȴ�״̬,�ȴ�ȫ�����������,
	eUseCommRT,//RT ̬(ʵʱģʽ,������)
    eDataUseStateEnd,
	
};



//������֯���õ��ĸ��������(�༶��)��ö��
enum E_CODE_INDEX
{
    E_PREPAREDATA_CLASS = 1,//PrepareData��
    E_THREADWORK_CLASS,//ThreadWork��
    E_THREADMANAGER_CLASS,//ThreadManager��
	E_THREADTASKMGR_CLASS,//TaskMgrThread��
	E_GUIDATAIN_CLASS, 
	E_Save_CLASS,
	E_MAINCTRL_ClASS,
        
    E_WEBSERVICE_CLASS,  //��ȡ�ļ��б���Ϣ׼�����ݵĹ���,Cwebservice
};


//�߳���� add by yzh
enum E_ThreadType
{
	E_TASKMGR_THREAD = 0x01,    //��������߳�
	E_4KERNELWORK_THREAD = 0x02,       //4�����߳�

	E_DATAPRE_THREAD = 0x05,    //����׼���߳�
	E_SAVE_THREAD = 0x06,       //�������������߳�
    E_WEBSERVICE_THREAD = 0x07, //WebService�߳�
	E_UPDATE_THREAD = 0x08,     //��վ���������߳�
};

//��Ϣ���� add by yzh
enum E_MessageType
{
	//����
	eMTypeNone = -1,
	E_NEWERR,//��new�����ڴ�ʧ��

	//��������
	eTask_PushGUITask,	//GUIѹ����
	eTask_PrepareFailed,	//����δ׼��������
	eTask_PrepareFinished,  //����׼����ɵ�
	eTask_TaskSucceed,		//����ɹ�����
	eTask_TaskFailed,		//����ʧ�ܽ���

	//����׼���߳�
	E_PPD_START_MSG,//��ʼ׼��
	E_PPD_GETNETDATAFROMSERVER_MSG,//��Ҫ��server��ȡ��Ϣ
	E_PPD_SERVERRETURNDATA_MSG,//server������Ϣ
	E_PPD_UNKNOWN_MSG,//δ֪��Ϣ
	E_PPD_NETCODELESSZERO_MSG,//��վ���С��0(һ�㲻����)
	E_PPD_CONNDBOK_MSG,//����DB�ɹ�

	E_PPD_RETURNNETSTATIONBASEINFO_MSG,//���ص�����վ�Ļ�����Ϣ
	E_PPD_SERVERRETURNERR_MSG,//����server��������ʧ��
	E_PPD_LASTUPDATE_GETTED_MSG,    //��վ������ʱ���Ѿ���ȡ��
	E_PPD_LASTUPDATEGETERR_MSG,//��ȫ��������վ������ʱ��ʧ��
	E_PPD_NOWEBDATA_MSG,//û����վ����
	E_PPD_RESTART_MSG,//��������׼����Ϣ������������
	E_PPD_CHECKISNULL_MSG,//��֤�����ʺ�Ϊ��
	E_PPD_SERVER_RETURN_BASE_ERR_MSG,//����server��������ʧ�� ������Ϣ

	//SaveThread�߳���ص���Ϣ
	E_SAVE_INITDATASAVE, 
	E_SAVE_TASKFINISHED,
	E_SAVE_FIRSTDATA,

    //WebService�߳���ص���Ϣ
    E_WEBS_START,         //webservice ��ʼ��Ϣ
	E_WEBS_INITDOWNLOADFILE,             //��ʼ��ʱ���ر����ļ�
    E_WEBS_GET_NETSTATION_ITEM,          //��ȡ������վ����Ŀ��Ϣ
    E_WEBS_GET_NETSTATION_BASEINFO,      //��ȡ������վ�Ļ�����Ϣ
    E_WEBS_GET_ALL_NETSTATION_LASTUPDATE,  //��ȡ������վ��������ʱ��

	E_WEBS_LASTUPDATE_GETTED,       //��վ������ʱ���Ѿ���ȡ��
	E_WEBS_GET_NETSTATION_INFO_SAVED,      //���������վ����Ϣ

	//�����߳������Ϣ
	E_FTP_GET_NETSTATION_BASEINFO_SAVED,      //FTP������ɵ�����վ�Ļ�����Ϣ
	E_FTP_GET_FILE_FROM_SERVER,				//FTP��ȡ�������ļ�
	E_HTTP_GET_FILE_FROM_SERVER,
	E_FTP_GET_FILE_FROM_SERVER_SAVED,		//FTP������ɷ������ļ�

	//�����߳������Ϣ
	E_4KERNEL_TASK_IN,     //����������
	E_4KERNEL_TASK_OUT,   //����������

};

enum CodeOCRResultType
{
	UNKNOW,  
	AUTO_ERROR_BUT_CORRECT_IN,
	AUTO_ERROR_AND_ERROR_IN,						//	�жϲ�����
	AUTO_ERROR_BUT_NO_IN,
	AUTO_CORRECT_AND_NO_IN,
	AUTO_ERROR_AND_UNKNOW_IN,
	NO_CONTENT,												//ͬatuo_error_but_no_in
};
 
enum E_ACCOUNT_STATE
{
	E_ACCOUNT_STATE_VALID = 1       ,              //��Ч
	E_ACCOUNT_STATE_INVALID = 2     ,              //��Ч
	E_ACCOUNT_STATE_AUDITING = 3    ,              //�����
	E_ACCOUNT_STATE_EMAILCONFIRM = 4,              //�ʼ�����
	E_ACCOUNT_STATE_NOTNEEDREG = 5  ,              //����Ҫע��
	E_ACCOUNT_STATE_NEEDREG = 6     ,              //��Ҫע�ᣬ��û��ע�����վ
		
};

#endif