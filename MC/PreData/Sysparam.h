#pragma once

enum DATETIME
{
	D_YEAR = 0,
	D_MONTH = 1,
	D_DAY = 2
};
class CSysparam
{
public:
	CSysparam(void);
	~CSysparam(void);
	
	//����:��ȡϵͳ��Ϣ
	int GetSysparam(std::vector<TParamItem>& tParamItemTable, 
						T_TaskData* pTaskData);

	//����:ϵͳ��Ϣ���Ժ���
	int GetSysparamForTest(std::vector<TParamItem>& tParamItemTable);


	//����:������滻ֵ������Ҫ��(��������,���ȵ�����һ���ط�����)����
	int TrimFun(const CStdString &strIn, CStdString& strOut,E_PPD_TRIM_TYPE eType);

	/*
	@brief  �����ַ�������ȡ�׶�����
	@param  strIn Ҫ��ȡ���ַ���
	@return ���������ַ���
	*/
	CStdString GetNumberFromString(const CStdString &strIn);

	/*
	@brief  ���������ַ�������ȡ��Ӧ�������� ��ʽ��2011-01-02
	@param  strIn Ҫ��ȡ���ַ���
	@param  dType ����
	@return �����ַ���
	*/
	CStdString GetDateFromString(const CStdString &strIn, DATETIME dType);

private:
	std::vector<CStdString> m_strDupName;
	std::map<int,CStdString> m_EProvValue;

};
