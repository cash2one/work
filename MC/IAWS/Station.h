#pragma once
#include "StdString.h"


class CStation
{
public:
	CStation(void);
	~CStation(void);

	CStdString NScode; //��վ���
	CStdString NSname; //��վ����
	CStdString NSrstep; //ע�Ჽ��
	CStdString NSraction; //ע�����
	CStdString NSrconst; //
	CStdString NSrkeyword; //
	CStdString NSrreffer; //
	CStdString NSrsucc; //�ɹ����
	CStdString NSpstep; //��������
	CStdString NSpaction; //��������
	CStdString NSpconst; //
	CStdString NSpkeyword; //
	CStdString NSpreffer; //
	CStdString NSpsucc; //�ɹ����
	CStdString NSdstep; //���ز���
	CStdString NSdaction; //���ز���
	CStdString NSdconst; //
	CStdString NSdreffer; //
	CStdString nsMemo; //
	CStdString NSsstep; //��������
	CStdString NSsaction; //��������
	CStdString NSsconst; //
	CStdString NSskeyword; //
	CStdString NSsreffer; //
	CStdString NSssucc; //�ɹ����
	CStdString nsDetailMark; //��ϸ��Ϣ��ַ���
	CStdString nsWebFilter; //��ҳͷβ���˱��
	CStdString nsValidUrlMark; //��ַ���˲���
	CStdString nsCompanyFiter; //������Ϣ���˱��: ��Ϣ��ָ�<@@>����<@@>����<@@>��˾<@@>�ʼ�<@@>�绰<@@>����<@@>����<@@>��ַ<@@>��ϵ��<@@>���<@@>��ַ<@@>��ҵ
	CStdString nsNextPageMark; //��һҳ���
	CStdString NShost; //������ַ
	CStdString nsAdvanceSearchSymbol; //����ת�����
	CStdString NSPublishDate; //
	DWORD NSClass; //��վ����
	//CStdString NSAccount; //
	//CStdString NSPass; //
	//CStdString NSrFlag; //
	//CStdString NSpFlag; //
	//CStdString NSsFlag; //
	//CStdString NSeFlag; //
	CStdString IsDemo; // �Ƿ�Ϊ��ʾ�汾��վ
	CStdString nsEaction; //ID�ʼ�����
	CStdString nsEconst; //
	CStdString nsEkeyword; //
	CStdString nsEreffer; //
	CStdString nsEstep; //ID�ʼ�����
	CStdString nsEsucc; //�ɹ����
	CStdString nsERefreshDate; //
	//CStdString nsLFlag; //
	CStdString nsLAction; //
	CStdString nsLConst; //
	CStdString nsLKeyword; //
	CStdString nsLreffer; //
	CStdString nsLstep; //
	CStdString nsLsucc; //��½�ɹ��ؼ���
	CStdString nsLRefreshDate; //
	CStdString nsDeepDetailMark; //szRelateUrlFilter
	CStdString nsPRefreshDate; //
	CStdString nsSRefreshDate; //
	CStdString nsRRefreshDate; //
	CStdString nsPRePostKey; //�ظ������ؼ���
	CStdString nsRRePostKey; //�ظ�ע��ؼ���
	CStdString nsoperator; //
	CStdString RegWebPath; //
	CStdString NsOnlyDown; //������
	CStdString nsDisplay; //�Ƿ����Ա��
	CStdString nstype; //��������
	CStdString nsonlypost; //������
	CStdString nsarea; //
	CStdString nsScriptExplain; //ScriptUrl�����ַ���
	CStdString nsWebRedirect; //ҳ�����¶���
	CStdString nsMainHost; //����վ��ַ
	CStdString nsLMRefreshDate; //
	CStdString nsBLRefreshDate; //
	CStdString nsNNTFileName; //ģ���ļ�����
	CStdString nsImageProcessCode; //ͼ��Ԥ����ʽ
	CStdString NsNeedFitLanmu; //
	CStdString nsTeleZone; //
	CStdString nsImageProcessRate; //
	CStdString nsIsValidForDemoOrTrial; //
	CStdString nsBaseInfoLastUpdate; //�������վ������Ϣ��������ʱ��
	CStdString nsVariableLastUpdate; //�������վ������������ʱ��
	CStdString nsItemLastUpdate; //�������վ��Ŀ��������ʱ��
	CStdString nsEnabledUpdatedInvalid; //
	CStdString nsRowLastupdated; //
	CStdString nsRowLastupdatedBigint; //
	CStdString nsNetArea; //��������
	CStdString nsLMRefreshDateTemp; //
	CStdString nsBLRefreshDateTemp; //
	CStdString nsRowLastupdatedBigintTemp; //
	

	CStdString szFixCookies                ;// String                   //�̶��ģ�ֻ��ָ�������ʹ�õ�cookies
	CStdString ProgressData                ;// String                   //��������¼session����
	BOOL blnLocalDataIsNew           ;// Boolean                  //true��ʾ���������Ѿ������µ�
	DWORD PExeNumber                   ;                     //�Ѵ��ڵ�������(������ʱʹ��)

	int GetDataFromString(CStdString strData);   //ͨ���ַ�������������

    int GetItemFromString(const int iNscode,const CStdString strNscode,const CStdString strData,T_NetstationItemGroup &NetItem);


    int GetAllStationLastUpdateALL(const CStdString strData,T_ServerNetStationInfoGroup &ServerLastupdateGroup);
};
