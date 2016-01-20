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


//�ϲ㴫�ݸ�ExServer���ݵĽṹ��
struct T_Task_Data_Exser
{
	CStdString strEnginData; //����ʹ�õ�����
	CStdString strExOCRParam;//Ƕ�����ݵ���֤�����

	std::map<DWORD,CStdString> mapEmbedData; //Ƕ������
};

///////////////////////////////////////////////////////////////////////////
////////////////////  ���¶������ϲ㷵�ؽ���Ľṹ///////////////////////
///////////////////////////////////////////////////////////////////////////

//MC��Exserver��ȡ���ݵ����õ��Ľṹ
//��Ϊ����: ����ɹ��Ľṹ��;����ʧ�ܵĽṹ��;����������м�����

enum E_RESULT_INFO
{

	e_TASK_RESULT_AUTH_CODE_ERROR = 980,           //��֤�����
	e_TASK_RESULT_RECOMPANY = 981,				//��˾���ظ�
	e_TASK_RESULT_SOME_SUCCESS = 982,            //���ֳɹ�
	e_TASK_RESULT_REEMAIL = 983,				//�����ظ�
	e_TASK_RESULT_ACCOUNT_NOT_AUDIT = 984,      //�˺�δ���
	e_TASK_RESULT_REG_LACK_INFO = 985,          //ע��ȱ������
	e_TASK_RESULT_ADD_LACK_INFO = 986,          //����ȱ������
	e_TASK_RESULT_POST_LACK_INFO = 987,         //����ȱ������
	e_TASK_RESULT_REPHONENUMBER = 988,          //�ֻ����ظ�

    e_TASK_RESULT_NETZIPPED_DATA_ERR = 990,        //׼������ʧ��
    e_TASK_RESULT_REGMAIL_FAIL = 991 ,//            '�Զ�ע��ɹ�������Ҫ������֤
    e_TASK_RESULT_REGAUDIT_FAIL = 992 ,//         '�Զ�ע��ɹ�������Ҫ���ȷ��
	e_TASK_RESULT_ADDPICFAIL = 993, //				'����ͼƬ�ϴ�ʧ��     //14.6.25
    e_TASK_RESULT_INVALIDACCOUNT = 994      ,//            '��¼ʧ��
    e_TASK_RESULT_DUPLICATE = 995     ,//            '�ظ�
    e_TASK_RESULT_FAIL = 996          ,//            'ʧ��
    e_TASK_RESULT_POSTFULL = 997          ,//            '����
	e_TASK_RESULT_ADDEXTRAFAIL = 998          ,//            '���Ƶ�����ʧ��
    e_TASK_RESULT_SUCC = 999          ,//            '�ɹ�

	
};
               


#ifndef STRUCT_INPUTDIALOG_IN_OUT_INFO
#define STRUCT_INPUTDIALOG_IN_OUT_INFO

struct T_TaskResultInfo
{
	E_RESULT_INFO eResultFlag;	//�����ʶ�������жϸ������ǳɹ�����ʧ��
	std::list<CStdString> listOcrCode;          //��֤����Ϣ
	CStdString  strResultInfo;                   //�����Ϣ
	CStdString  strHtml;                       //����ɹ����html�����ļ�����
	CStdString	strCompPopular;               //��˾֪����
	CStdString	strInfoID;                    //��ϢID
	CStdString  strMailCount;                //�ʼ�����
	CStdString  strMigrateResult;            //���ظ��ϲ�Ǩ�����ݽ��(Ҳ��������Ʒ�б����ֶ�)
	DWORD		dwType;                      //һ����ҹ�������� 15����˾���� 16�� ��Ʒ����
	DWORD		dwSuccFieldCnt;				//Ǩ�Ƴɹ��ֶ�����
	
	T_TaskResultInfo()
	{
		eResultFlag = e_TASK_RESULT_FAIL;		//Ĭ��������ʧ�ܵ�
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

