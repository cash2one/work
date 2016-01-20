// PrepareData.h: interface for the CPrepareData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_)
#define AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//�๦���ڶ����: 9

#include "StdString.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "StringUtils.h"
#include "common_enum.h"
#include "IThreadUnit.h"
#include "Sysparam.h"
#include "NetData.h"
#include "ZBase64.h"
#include "DES.h"

//�������ݵ�ǰ��׼������
class CPrepareData
{
public:
	CPrepareData();
	virtual ~CPrepareData();

	//��������
	DWORD ProcessData(const int& nNetCode, const E_TASK_TYPE& eTaskType, 
						TRapalceDataMsg& tCompanyInfo, T_TaskData* tTaskData,DWORD dwIndex, DWORD dwPostType = -1);
	
	DWORD GetValueFromDB(T_NetstationBaseInfo& tNetStationData);

	//�滻ͨ���
	DWORD ReplaceParam(const CStdString& strIn, CStdString& strOut, 
				const int& nKey, TRapalceDataMsg& tCompanyInfo);
	BOOL Equate(CStdString& src, CStdString& equate,
				TRapalceDataMsg& tCompanyInfo);
	//����û���滻��%ͨ���
	void ReplacePercent(CStdString& strOut,std::vector<CStdString> &vNoRepalace);

	//�滻�ں˴����ͨ���
	void ReplaceSumengineParam(CStdString& strData);

	//����ͨ���ȡ�������С����
	void GetMaxMin(const CStdString &strIn, int &iMax, int &iMin );

	//����վ���α������д���
	BOOL DealVarible(CStdString &strIn, 
				CStdString &strOut, std::vector<TParamItem>& tSearchdata, DWORD nNetLanType);
	//�õ�ĳ��ʱ���ʽ�ĵ�ǰʱ���ַ�����
    DWORD GetCurrentTimeByFormats(const CStdString strTimeFormat, 
				CStdString& strTimeOut);

	//���ע���õ����ʻ���Ϣ�����б�Ҫ������Ӧ�޸�
	DWORD CheckRegCount(CStdString &strInAndOut, DWORD dLen, E_Account_Style eFirstFlag);
	
	//����ַ�����һλ�ǲ�����Ҫ�����ͣ����ǣ��Զ��޸�
	DWORD CheckFirstChar(CStdString &strInAndOut, E_Account_Style eFlag);

    void AddDefaultSubStr(const CStdString& strIn, const CStdString& strDelimiter, 
				std::vector<CStdString>& strOut);
   

    BOOL IsAimStr(CStdString str);
    BOOL IsChar(TCHAR ch);
    int KeyToVal(CStdString& temp);

	/*
		@breif ������վdat�ļ���ļ�������
		@param strCipher  ��������
		@param strPlain  ���ܳ���������
	*/
	void DecodeStationData(const CStdString &strCipher , CStdString &strPlain) ;

	/*
		@breif ����ͨ���ȡ�ÿ�͸�
		@param strTongPei  ͨ���
		@param [out] iWidth  ��
		@param [out] iHeight ��
	*/
	void GetWidthAndHeight(const CStdString &strTongPei, int &iWidth, int &iHeight);

	/*
		@breif �ı�ͼƬ��С
		@param strTongPei  ͨ���
		@param [out] strImage  ��ͼƬ·��
		@return TRUEΪ�ɹ�
	*/
	BOOL GetResizeImage(const CStdString &strTongPei, CStdString &strImage,DWORD dwFlag = 0,int iIndex = 0);

	//����:�����Body��ֵ�г���Ҫ��ĺ���
	int TrimLenFun(CStdString strIn, CStdString& strOut, int nTrimLen,
					E_PPD_TRIM_TYPE eType);

	//��ʼ�����Ҷ���
	int InitCountry();

	//��ʼ��ʡ������
	int InitProvValue();

	//��ʼ���Ա�
	void InitSex();

	//��ʼ����˾����
	void InitCompanyKind();

	//��ʼ����Ч��
	void InitExpire();

	//��ʼ����˾����
	void InitPersonsCount();

	//��ʼ��ע���ʽ�
	void InitMoneyInfo();

	//void GetFullTime(int& nYear, int& nMouth, int& nDay, int nType, int nNum);
	int GetDateNum(int nMouth, int nYear);

	//��ȡ����Ĭ��ֵ
	int GetParamValue(int nValueId, int nParamType, int nLanType,CStdString& strParamValue);

	//��ȡ���Ĳ���Ĭ��ֵ
	int GetCParamValue(int nValueId, int nParamType, CStdString& strParamValue);
	//��ȡӢ�Ĳ���Ĭ��ֵ
	int GetEParamValue(int nValueId, int nParamType, CStdString& strParamValue);

	//�������в���
	void LoadAllParam();

private:
	/*
	@brief ͼƬת��
	@param strImagePath; Ҫת����ͼƬ��ַ��
	@param dwType;    ת������ (1.jpg,2.png,3.bmp,4.gif)
	*/
	BOOL ImageConvert(CStdString &strSrcPath, DWORD dwType);

	/*
	@brief DES������֤�����
	@param iNetID  ��վID
	@param strEncryptData ��������
	@return ���ܺ������
	*/
	CStdString DesImageCode(const int &iNetID, CStdString& strEncryptData);

public:
	CNetData m_NetData;
private:
	E_TASK_TYPE m_eCurrentTaskType;

	CZBase64 m_Base64 ;

	CDES des;
	//����
	std::map<int,CStdString> m_strCCountry;
	std::map<int,CStdString> m_strECountry;

	//ʡ��
	std::map<int,CStdString> m_CProvValue;
	std::map<int,CStdString> m_EProvValue;

	//�Ա�
	std::map<int,CStdString> m_strCSex;
	std::map<int,CStdString> m_strESex;

	//��˾����
	std::map<int,CStdString> m_strCCompanyKind;
	std::map<int,CStdString> m_strECompanyKind;

	//��Ч��
	std::map<int,CStdString> m_strCExpire;
	std::map<int,CStdString> m_strEExpire;

	//��˾����
	std::map<int,CStdString> m_strCPersonsCount;
	std::map<int,CStdString> m_strEPersonsCount;

	//ע���ʽ�
	std::map<int,CStdString> m_strCMoneyInfo;
	std::map<int,CStdString> m_strEMoneyInfo;

	//���в���
	std::vector<CStdString> m_vstrParam;

	static const CStdString PARAM;

	CStdString m_strPagePhotoFlag; //ץȡ���յı��
};


#endif // !defined(AFX_PREPAREDATA_H__1E8CFFAD_E858_4328_B34A_2058C35518D9__INCLUDED_)
