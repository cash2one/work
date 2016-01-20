#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include "StepData.h"
#include <vector>

//һ���������������
class CBlockData:public CEnDeCode
{
public:
	std::vector<CStepData> m_vobjTargetData;      //ҳ��ȫ�����������
	long m_lngActiveObject;                 //��ǰ��Ծ����
	long m_lngPerWait;                      //ǰ����ʱ
	bool m_bFaultTolerant;                  //ҳ���Ƿ��ݴ�
	bool m_bLoopFlag;             //ѭ�����
//	T_LOOP_SETTING m_lsLoopSetting;       //ѭ������
	CStdString m_strDescription;   //���ݿ�����

	T_LOOP_SETTING m_tLoopSetting;  // ѭ����������

	T_JUMP_SETTING m_tJumpSetting; //��ת��������

public:	
	CBlockData() {Clear();} ;
	void Clear() ;

	void AddObject(CStepData &obj);   //��Ӳ�������
	void DelObject();                   //�Ƴ���ǰ�Ļ�Ծ����
	void DeleteMotherStep(CStepData motherStep);
	void GetStep(long lngIndex, CStepData &obj) ;
	void GetCurStep(CStepData &obj) ;
	CStepData *GetCurStep() ;   //���ز������ݵ�ָ��
	CStepData *GetStepByDescription(CStdString &strDescription) ;
	void SwapStepData(int nObjIndex1 , int nObjIndex2) ;  //�������������λ��
	void InsertStepData(int nPostion , CStepData &stepData) ;

	int GetCount();        //��ȡ��������ĸ���

	//E_PageMethod GetMethod() const;
	//void SetMethod(E_PageMethod pmMethod);

	bool GetFaultTolerant() const;
	void SetFaultTolerant(bool &bFaultTolerant);

	bool GetLoopFlag() const ;
	void SetLoopFlag(bool &bLoopFlag) ;

	long GetActiveIndex() const;
	void SetActiveIndex(long lIndex);

	long GetPerWait() const;
	void SetPerWait(long lngWait);

	CStdString GetDescription() const ;
	void SetDescription(const CStdString &strDescription) ;

	E_LOOP_TYPE GetLoopType() const ;
	void SetLoopType(E_LOOP_TYPE &eLoopType) ;

	CStdString GetLoopValue() const ;
	void SetLoopValue(const CStdString &strLoopValue) ;

	bool GetSubRandom() const;
	void SetSubRandom(bool &bSubRandom);

	//int GetRandomCount() const;
	//void SetRandomCount(int nRandomCount);
	CStdString GetRandomCount() const;
	void SetRandomCount(CStdString strRandomCount);

	///////////////////////////////////////////
	bool GetIsJump() const ;
	void SetIsJump(bool &bLoopFlag) ;

	bool GetIsConditionFirst() const ;
	void SetIsConditionFirst(bool &bLoopFlag) ;

	bool GetIsJumpSucc() const ;
	void SetIsJumpSucc(bool &bLoopFlag) ;

	bool GetIsJumpFail() const ;
	void SetIsJumpFail(bool &bLoopFlag) ;

	bool GetIsJumpCondition() const ;
	void SetIsJumpCondition(bool &bLoopFlag) ;

	long GetSuccToWhich() const;
	void SetSuccToWhich(long lIndex);

	long GetFailToWhich() const;
	void SetFailToWhich(long lIndex);

	long GetConditionToWhich() const;
	void SetConditionToWhich(long lIndex);

	long GetConJumpType() const;//������ת����
	void SetConJumpType(long lIndex);

	CStdString GetLeftCondition() const;
	void SetLeftCondition(CStdString str);

	CStdString GetRightCondition() const;
	void SetRightCondition(CStdString str);

	long GetConditionComp() const;
	void SetConditionComp(long lIndex);
	///////////////////////////////////////////

	//�ƶ���������
	void MoveForward();
	void MoveNext();

	//�ɱ����������ݽṹת��
	CStdString GetSource();
	void SetSource(const CStdString &strSource);

private:
	CStdString TransTargetsToSource();
	void ExtraTargetsFromSource(CStdString &strData);
};
