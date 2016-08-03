#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include <vector>

class CBlockData;

//执行一步所需的数据
class CStepData: public CEnDeCode
{
private:
	E_ActionMethod m_amActionMethod;     //执行操作
	CStdString m_strDescription;    //对象描述
	int m_nClickType;//正常点击0，模拟鼠标点击1,点击弹框2，点击跳转页面3
	int m_nIndentifyType;//自动识别0，远程打码1，人工输入2
	int m_nJumpUrlType;//跳转页面类型，新页面打开网页0，打开时强制刷新1，打开时不刷新2
	int m_nFillType;//赋值类型，直接赋值0，产生按键事件1
	bool m_bRestartFrom1thStepIfFail; //成功标记失败后是否从执行块的第1步开始执行 

	int m_nLevel;                 //m_pItem的父节点删除功能的层级
	T_ITEM *m_pItem;             //用于定位元素的标记，必要时会用到父节点的数据

	CStdString m_strSymbol;         //对象对应的通配符/值
    bool m_bVaguelyLocate;       //模糊定位
	//long    m_lngSurWait;        //操作后延时,在延时后再去找成功特征
	CStdString m_strSurWait;//操作后延时,在延时后再去找成功特征
	bool m_bFaultTolerant;                  //对象是否容错
	std::vector<T_STEP_RETURN> m_vRet;    //对象要返回的数据

	CStdString m_strErrDescription;
	bool m_bPageShot ;  //是否返回当前页面的快照

	CStdString m_strVLSymbol ;   //模糊定位指定索引通配符

	//增加提取数据容错成功的返回值
	CStdString m_strRetErr;


public:
	CBlockData *m_pSubBlock;

public:
	//构造、拷贝构造函数
	CStepData();
	CStepData(const CStepData &stepData) ;
	CStepData & operator = (const CStepData &stepData) ;
	~CStepData();

	void Clear();      //清空对象

	E_ActionMethod GetAction() const;
	void SetAction(E_ActionMethod method);

	CStdString GetDescrition() const;
	void SetDescription(const CStdString &strDesc);

	void GetTagName(CStdString &strName , T_ITEM *pItem) const;
	void SetTagName(const CStdString &strName , T_ITEM *pItem);

	void GetAttribute(const CStdString &strName, CStdString &strVal , T_ITEM *pItem);
	void SetAttribute(const CStdString &strName, const CStdString &strVal , T_ITEM *pItem);
	void DelAttribute(const CStdString &strName , T_ITEM *pItem);
	void DelAttribute(const CStdString &strName);

	int GetClickType() const;
	void SetClickType(int nNormalClick);
	int GetIndentifyType() const;
	void SetIndentifyType(int nIndentify);
	int GetJumpType() const;
	void SetJumpType(int nJump);
	int GetFillType() const;
	void SetFillType(int nFill);

	bool GetFlagOfRestartFrom1thStep();
	void SetFlagOfRestartFrom1thStep(bool bFlag);

	int GetLevel() const;
	void SetLevel(int level);

	void SetItem(T_ITEM *Item) ;
	void SetItem(T_ITEM *ItemSource , T_ITEM *&ItemDest) ;
	void GetItem(T_ITEM *&Item) ;
	T_ITEM *GetItem();
	//--------------------------------------
	void FillStepContent(CStepData* pStepTmp, CStepData* pSubStep);
	void FillSubBlock(CBlockData *pBlockSource, CBlockData *&pBlockDest);

	CStdString GetSymbol() const;
	void SetSymbol(const CStdString &strSymbol);

	void SetPageShot(bool &bPageShot) ;
	bool GetPageShot() const;

	void SetValuelyLocate(bool &bValuelyLocate) ;
	bool GetValuelyLocate() const ;

	void SetFaultTolerant(bool &bFaultTolerant) ;
	bool GetFaultTolerant() const ;

	void GetStepReturn(std::vector<T_STEP_RETURN> &vStepReturn) ;
	void SetStepReturn(std::vector<T_STEP_RETURN> &vStepReturn) ;
	void AddStepReturn(T_STEP_RETURN &stepReturn) ;
	void DelStepReturnByAttrib(CStdString &strAttrib) ;
	void SetMatchRuleByAttrib(const CStdString &strAttrib , CStdString &strMatchRule) ;
	void SetRetValueByAttrib(const CStdString &strAttrib , CStdString &strValue) ;
	bool StepNeedReturnData(void) const;

	//long GetSurWait() const;
	//void SetSurWait(long lngSurWait);
	CStdString GetSurWait() const;
	void SetSurWait(CStdString strSurWait);

	void GetErrDescrition(CStdString &strErrDesc) const;
	void SetErrDescription(const CStdString &strErrDesc);

	CStdString GetVLSymbol() ;
	void SetVLSymbol(CStdString &strVLSymbol) ;

	//编码后的对象返回数据
	CStdString GetReturnData();

	//返回对象的数据(依次获取各成员的数据并按格式拼装)
	CStdString GetSource();  
	//设置对象的数据(还原对象数据)
	void SetSource(const CStdString &strData);  

	CStdString GetRetErr() ;
	void SetRetErr(CStdString &strErr);

private:
	CStdString TransTargetsToSource();
	void ExtraTargetsFromSource(CStdString &strSourceData);

	//封装T_ITEM
	CStdString TransItemsToSource(T_ITEM *pItem) ;
	//解封数据到T_ITEM
	void ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem) ;

	//封装T_ITEM结构中的属性mapAttributes
	CStdString TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes) ;
	//解封T_ITEM结构中的属性mapAttributes
	void ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes) ;

	CStdString TransStepReturnToSource(std::vector<T_STEP_RETURN> &vRet) ;
	void ExtraStepReturnFromSource(CStdString &strStepReturnData , std::vector<T_STEP_RETURN> &vRet) ;

	void FreeItem(T_ITEM *&pItem);
	//--------------------------------------
	void FreeSubBlock(CBlockData *&pBlock);
};
