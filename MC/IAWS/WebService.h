//////////////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2007, ��Ѷ����Ϣ���޹�˾
// �汾��
// �ļ�˵����
// �������ڣ�
// ���ߣ�
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
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

	}; // ö�ٳ���

	DWORD dwIsAllNetstationLastUpdateGetted;
	DWORD m_dwAllStationLastUpdateGetting ;//���ڻ�ȡȫ����վ��������ʱ��
	DWORD m_dwWebServiceAllStationLastUpdateSended ; //�������������վ�ʺ��������Ϣ�Ѿ�������,
	TCHAR m_szCurrPath[MAX_PATH] ;		//��ǰĿ¼
	
	DWORD m_dwNetLastupdateIndex;       //��վ������ʱ���ȫ����������, ��PPD�����õ�
	
public:
	CWebService(void);
	~CWebService(void);
	//�̳߳�ʼ��
	virtual DWORD CreateThread();  //{return 0;}
	//�߳��˳�
	virtual DWORD DestoryThread(){return 0;}	
	DWORD DispatchMessage(T_Message *Msg);


private:

	void GetWebServiceData(DWORD dwIndex);     //�Զ�ͬ����webservice ����,��ȡ��Ҫ������
	
	void GetAllStationLastUpdateALL();

	DWORD GetAllStationLastUpdateALLReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile);//��ȡ��Ŀ������ʱ��

	DWORD SendToFtpGetServerFile(const int dwItemListCode,const int dwListType ,const DWORD dwAuto);
	DWORD GetServerFileFromFtp(const DWORD dwDataIndexID);

    DWORD GetAllStationLastUpdateALLOK(const CStdString &Source);	

	DWORD GetAllStationLastUpdateALLGettedPPD(const DWORD dwDataIndexID);            


	DWORD GetNetDataFromFTP(const DWORD dwMessageType,const DWORD dwWebID);

	DWORD SaveLastUpdateToMap(DWORD dwIndex,const CStdString &strData);

    DWORD GetNetstationAllInfoReturn(const DWORD dwItemListCode,const DWORD dwListType,const CStdString &strFile); //��ȡ��վ��Ϣ�����ļ��ķ���
 };
