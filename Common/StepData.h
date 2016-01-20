#pragma once

#include "enumdef.h"
#include "structdef.h"
#include "EnDeCode.h"
#include <vector>

class CBlockData;

//ִ��һ�����������
class CStepData: public CEnDeCode
{
private:
	E_ActionMethod m_amActionMethod;     //ִ�в���
	CStdString m_strDescription;    //��������
	int m_nClickType;//�������0��ģ�������1,�������2�������תҳ��3
	int m_nIndentifyType;//�Զ�ʶ��0��Զ�̴���1���˹�����2
	int m_nJumpUrlType;//��תҳ�����ͣ���ҳ�����ҳ0����ʱǿ��ˢ��1����ʱ��ˢ��2
	int m_nFillType;//��ֵ���ͣ�ֱ�Ӹ�ֵ0�����������¼�1
	bool m_bRestartFrom1thStepIfFail; //�ɹ����ʧ�ܺ��Ƿ��ִ�п�ĵ�1����ʼִ�� 

	int m_nLevel;                 //m_pItem�ĸ��ڵ�ɾ�����ܵĲ㼶
	T_ITEM *m_pItem;             //���ڶ�λԪ�صı�ǣ���Ҫʱ���õ����ڵ������

	CStdString m_strSymbol;         //�����Ӧ��ͨ���/ֵ
    bool m_bVaguelyLocate;       //ģ����λ
	//long    m_lngSurWait;        //��������ʱ,����ʱ����ȥ�ҳɹ�����
	CStdString m_strSurWait;//��������ʱ,����ʱ����ȥ�ҳɹ�����
	bool m_bFaultTolerant;                  //�����Ƿ��ݴ�
	std::vector<T_STEP_RETURN> m_vRet;    //����Ҫ���ص�����

	CStdString m_strErrDescription;
	bool m_bPageShot ;  //�Ƿ񷵻ص�ǰҳ��Ŀ���

	CStdString m_strVLSymbol ;   //ģ����λָ������ͨ���

	//������ȡ�����ݴ�ɹ��ķ���ֵ
	CStdString m_strRetErr;


public:
	CBlockData *m_pSubBlock;

public:
	//���졢�������캯��
	CStepData();
	CStepData(const CStepData &stepData) ;
	CStepData & operator = (const CStepData &stepData) ;
	~CStepData();

	void Clear();      //��ն���

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

	//�����Ķ��󷵻�����
	CStdString GetReturnData();

	//���ض��������(���λ�ȡ����Ա�����ݲ�����ʽƴװ)
	CStdString GetSource();  
	//���ö��������(��ԭ��������)
	void SetSource(const CStdString &strData);  

	CStdString GetRetErr() ;
	void SetRetErr(CStdString &strErr);

private:
	CStdString TransTargetsToSource();
	void ExtraTargetsFromSource(CStdString &strSourceData);

	//��װT_ITEM
	CStdString TransItemsToSource(T_ITEM *pItem) ;
	//������ݵ�T_ITEM
	void ExtraItemsFromSource(CStdString &strItemData , T_ITEM *pItem) ;

	//��װT_ITEM�ṹ�е�����mapAttributes
	CStdString TransAttributesToSource(std::map<CStdString , CStdString> &mapAttributes) ;
	//���T_ITEM�ṹ�е�����mapAttributes
	void ExtraAttributesFromSource(CStdString &strAttributeData , std::map<CStdString , CStdString> &mapAttributes) ;

	CStdString TransStepReturnToSource(std::vector<T_STEP_RETURN> &vRet) ;
	void ExtraStepReturnFromSource(CStdString &strStepReturnData , std::vector<T_STEP_RETURN> &vRet) ;

	void FreeItem(T_ITEM *&pItem);
	//--------------------------------------
	void FreeSubBlock(CBlockData *&pBlock);
};
