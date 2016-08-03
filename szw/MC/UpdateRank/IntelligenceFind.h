#pragma once


class CIntelligenceFind
{
public:
	CIntelligenceFind();
	~CIntelligenceFind();

public:
	/*
	@brief �����Ƿ��������������
	@param strData  Ҫ���ҵ����ݣ�
	@param strParam1 ��������1��
	@param strParam2 ��������2��
	@param iFlag 0:2���Ƿ񶼴��ڡ�1:2���Ƿ񶼲����ڡ�2����һ���������ڣ��ڶ������������ڣ�
	@param iCmpType 0:��Ƚϣ� 1����Ƚ�
	@return �����Ƿ���ҳɹ�
	*/
	BOOL FindResult(const CString &strData, const CString &strParam1,const CString &strParam2,int iFlag=0,int icmpType=0);

	/*
	@brief �����Ƿ��������������
	@param strData  Ҫ���ҵ����ݣ�
	@param strParam1 ��������1��
	@param strParam2 ����2��
	@param strParam3 ����3��
	@param iFlag 0:3���Ƿ񶼴��ڡ�1:3���Ƿ񶼲����ڡ�2��һ���������ڣ��������������ڡ�4�������������ڣ�һ�����������ڣ�
	@param iCmpType 0:��Ƚϣ� 1����Ƚ�
	@return �����Ƿ���ҳɹ�
	*/
	BOOL FindResult(const CString &strData, const CString &strParam1, const CString &strParam2, const CString &strParam3, int iFlag = 0, int icmpType = 0);


	/*
	@brief ��ȡ���ҵ��ַ�����λ��
	@param strData  Ҫ���ҵ����ݣ�
	@param Ҫ���ҵĲ���
	@param Ҫ���ҵĲ���2
	@param iInitPos ��ʼ����λ�ã�
	@return ���ز��ҵ��Ľ����
	*/
	int GetFindPos(const CString &strData, const CString &strPara1, const CString& strPara2,int &iInitPos);

	/*
	@breif ��ȡָ��λ���ַ���
	@param strData Ҫ������ȡ���ַ�����
	@param strPara1 ����1
	...
	@param iInitPos ��ʼλ�ã�
	@return ���ؽ�ȡ�����ַ���
	*/
	CString GetFindData(const CString &strData, const CString &strPara1,const CString &strPara2,
						const CString &strPara3,int iInitPos=0);
};

