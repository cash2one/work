#pragma once

//�������ݽṹ
typedef struct _WEB_TASK_DATA_INFOPOST
{
	CStdString strPostInfo;          //����������Ϣ
	CStdString strWebInfo;          //������վ��Ϣ
	CStdString strSystemInfo;        //ϵͳ������Ϣ

	CStdString strConfigInfo;        //������Ϣ
	CStdString strAccountPassword;   //����ע����վ���˺�����
	DWORD dwPostType;                //��������

	CStdString strLocalProductImage;  //���ز�ƷͼƬ����·��
	CStdString strLocalTradeImage;    //����Ӫҵִ�ձ���·��
	CStdString strLocalCompImage;    //���ع�˾��Ƭ����·��
	CStdString strLocalOrgImage;     //������֯����ͼƬ·��
	CStdString strLocalTaxImage;	 //����˰��֤��ͼƬ·��

	CStdString strServerMintus;      //��������ʱ��(������)
	CStdString strTimerTaskID;             //��ʱ����ID
}WEB_TASK_DATA_INFOPOST;

//ע�����ݽṹ
typedef struct _WEB_TASK_DATA_REGISTRY
{
	CStdString strWebInfo;          //������վ��Ϣ
	CStdString strSystemInfo;        //ϵͳ������Ϣ
	CStdString strConfigInfo;        //ϵͳ������Ϣ

	CStdString strTradeImage;  //Ӫҵִ��ͼƬ����·��
	CStdString strLocalProctImage;   //��ƷͼƬ  ע���ò���
	CStdString strLocalCompImage;   //��˾��Ƭ

	CStdString strServerMintus;      //��������ʱ��(������)
	CStdString strTimerTaskID;             //��ʱ����ID
}WEB_TASK_DATA_REGISTRY;

//ע�����ݽṹ
typedef struct _WEB_TASK_DATA_REFRESHANDDELETE
{
	CStdString strWebInfo;          //������վ��Ϣ

	CStdString strConfigInfo;        //������Ϣ
	CStdString strAccountPassword;   //����ע����վ���˺�����

	CStdString strServerMintus;      //��������ʱ��(������)
	CStdString strTimerTaskID;             //��ʱ����ID
}WEB_TASK_DATA_REFRESHANDDELETE;

//�����ƹ�
typedef struct _WEB_TASK_DATA_DOMAINPOST
{
	CStdString strWebInfo;          //������վ��Ϣ
	CStdString strTaskInfo;         //��������
	CStdString strConfigInfo;        //������Ϣ
}WEB_TASK_DATA_DOMAINPOST;

//΢���ƹ�
typedef struct _WEB_TASK_DATA_WEBOPOST
{
	CStdString strWebInfo;          //������վ��Ϣ
	CStdString strTaskInfo;         //��������
	CStdString strAccountPassword;   //����ע����վ���˺�����
	CStdString strConfigInfo;        //������Ϣ


}WEB_TASK_DATA_WEBOPOST;

//�޸�����
typedef struct _WEB_TASK_DATA_INFOMODIFY
{
	CStdString strWebInfo;			//ȫ����վ��Ϣ
	CStdString strSystemInfo;		//ϵͳ��Ϣ
	CStdString strConfigInfo;       //������Ϣ
	CStdString strAccountPassword;   //����ע����վ���˺�����

	CStdString strTradeImage;  //Ӫҵִ��ͼƬ����·��
	CStdString strLocalCompImage;   //��˾��Ƭ

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
	@brief   �����ƷͼƬ��Ӫҵִ�յ�����
	@param   strProductImageName ��ƷͼƬ·��
	@param   strTradeImageName Ӫҵִ��·��
	@return  
	*/
	DWORD SetTaskImageToGlobal(const CStdString& strProductImageName,const CStdString& strTradeImageName);

	/*
	@brief   �����Ʒ��Ϣ������
	@param   ��Ʒ��Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveProductToLocal(const CStdString& strData);

	/*
	@brief   ����ϵͳ��Ϣ��ȫ���ڴ�
	@param   ϵͳ��Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveSystemInfoToLocal(const CStdString& strData);

	/*
	@brief   ���淢����Ϣ���ڴ�
	@param   ��վ��Ϣ�ַ��� 
	@return  
	*/
	DWORD SavePostInfoToLocal(const CStdString &strData);
	
	/*
	@brief   ������վ��Ϣ���ڴ�
	@param   ��վ��Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveWebInfoToLocal(const CStdString &strData);

	/*
	@brief   ���浥������վ��Ϣ���ڴ�
	@param   ��վ��Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveWebInfoToLocalOfOneTask(const CStdString &strData);

	/*
	@brief   ����������Ϣ
	@param   ������Ϣ�ַ���
	@return  
	*/
	DWORD SaveConfigInfoToLocal(const CStdString &strData);


	/*
	@brief   �����ʺ��������Ϣ���ڴ�
	@param   strData �ʺ�������Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveAccountPasswordToLocal(const CStdString& strData);

	/*
	@brief   ���������ƹ�������Ϣ
	@param   strData �����ƹ�������Ϣ�ַ��� 
	@return  
	*/
	DWORD SaveDomainInfoToLocal(const CStdString& strData);

	/*
	@brief   ����΢��������Ϣ
	@param   strData ����΢��������Ϣ 
	@return  
	*/
	DWORD SaveWeboInfoToLocal(const CStdString& strData);
};
