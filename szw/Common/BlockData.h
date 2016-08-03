#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include "StepData.h"
#include <vector>

//一个区块包含的数据
class CBlockData:public CEnDeCode
{
public:
	std::vector<CStepData> m_vobjTargetData;      //页面全部对象及其操作
	long m_lngActiveObject;                 //当前活跃对象
	long m_lngPerWait;                      //前置延时
	bool m_bFaultTolerant;                  //页面是否容错
	bool m_bLoopFlag;             //循环标记
//	T_LOOP_SETTING m_lsLoopSetting;       //循环设置
	CStdString m_strDescription;   //数据块描述

	T_LOOP_SETTING m_tLoopSetting;  // 循环条件设置

	T_JUMP_SETTING m_tJumpSetting; //跳转条件设置

public:	
	CBlockData() {Clear();} ;
	void Clear() ;

	void AddObject(CStepData &obj);   //添加操作对象
	void DelObject();                   //移除当前的活跃对象
	void DeleteMotherStep(CStepData motherStep);
	void GetStep(long lngIndex, CStepData &obj) ;
	void GetCurStep(CStepData &obj) ;
	CStepData *GetCurStep() ;   //返回步骤数据的指针
	CStepData *GetStepByDescription(CStdString &strDescription) ;
	void SwapStepData(int nObjIndex1 , int nObjIndex2) ;  //交换两个步骤的位置
	void InsertStepData(int nPostion , CStepData &stepData) ;

	int GetCount();        //获取操作对象的个数

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

	long GetConJumpType() const;//条件跳转类型
	void SetConJumpType(long lIndex);

	CStdString GetLeftCondition() const;
	void SetLeftCondition(CStdString str);

	CStdString GetRightCondition() const;
	void SetRightCondition(CStdString str);

	long GetConditionComp() const;
	void SetConditionComp(long lIndex);
	///////////////////////////////////////////

	//移动操作对象
	void MoveForward();
	void MoveNext();

	//采编数据与数据结构转换
	CStdString GetSource();
	void SetSource(const CStdString &strSource);

private:
	CStdString TransTargetsToSource();
	void ExtraTargetsFromSource(CStdString &strData);
};
