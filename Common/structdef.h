#pragma once

#include "enumdef.h"
#include "StdString.h"
#include "EnDeCode.h"
#include <map>
#include <vector>


//������/ֵ
struct T_Name_Value
{
	CStdString strName;       
	CStdString strValue;
};

//����������
struct T_ITEM
{
	CStdString strTagName;                         //��ǩ����
	std::map<CStdString, CStdString> mapAttributes;   //�����б� ������/ֵ

	T_ITEM *pParent ;        //���׽ڵ�
	T_ITEM *pBrother ;     //�ֵܽڵ�

	//���ӽڵ�������Ҫ�ı���
	//std::vector<T_ITEM *> m_vecNodes;//��ǰ�ڵ�������ӽڵ㣬����Ǹ��ڵ�Ļ��������ֻ��һ��������m_nPosΪ-1�ж�
	//std::vector<int> m_vecPos;//��ǰ�ڵ�λ���ֵܽڵ�ĵڼ�������0��ʼ������������ϲ�ĸ��ڵ����û���������ֵΪ-1
	std::map<int, T_ITEM *> m_mapNodesAttr;//����������忴����2�У�û�и��ڵ����������СΪ0
	//int m_nLevel;//��ǰ�ڵ�λ�ڸ��ڵ�ĵڼ��㣬���ڵ�Ϊ��0�㣬��ʱ����
	
	T_ITEM()
	{
		strTagName = _T("");
		pParent = NULL;
		pBrother = NULL;
		mapAttributes.clear();
		m_mapNodesAttr.clear();
	}

	//��ȡ����������
	void GetAttruteNames(std::vector<CStdString> &vName)
	{
		vName.clear();

		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.begin();
		for (; itAttr != mapAttributes.end(); ++itAttr)
			vName.push_back(itAttr->first);
	}

	//����ָ�����Ե�ֵ
	CStdString GetAttribute(CStdString strName)
	{
		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.find(strName);
		if (itAttr != mapAttributes.end())
			return mapAttributes[strName];		
		else
			return _T("");
	}

	//��������Ƿ����
	bool AttributeExist(CStdString strName)
	{
		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.begin();
		for (; itAttr != mapAttributes.end(); ++itAttr)
		{
			if (!strName.CompareNoCase(itAttr->first))
				return true;
		}

		return false;
	}

	void MaKeAttributeLower()
	{	
		
		if (mapAttributes.size() == 0)
		{
			return;
		}
		std::map<CStdString, CStdString> mapTempAttribute;
		mapTempAttribute.clear();

		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.begin();

		for (; itAttr != mapAttributes.end(); ++itAttr)
		{	
			CStdString strAttributesName = itAttr->first;
			strAttributesName.MakeLower();
			mapTempAttribute[strAttributesName] = itAttr->second;
		}
		
		mapAttributes.clear();
		mapAttributes = mapTempAttribute;
	}
};


//����Ҫ���ص�����
struct T_STEP_RETURN
{
	CStdString strAttrib;
	CStdString strRule;
	CStdString strValue;
};

//�����������
struct  T_FILTER_STRUCTURE
{
	std::vector<CStdString > straAnd;
	std::vector<CStdString > straNot;
	std::vector<CStdString > straOr;
};

//������������
struct T_FILTER_ITEM
{
	T_ITEM tItem ;             //��������λ
	CStdString strAttributeName ;   //��ȡ������������
	CStdString strFeature ;             //�ɼ��ĳɹ����
};

//��������
struct T_FILTER_STRUCT
{
	std::vector<T_FILTER_ITEM> vFilterAnd;
	std::vector<T_FILTER_ITEM> vFilterNot;
	std::vector<T_FILTER_ITEM> vFilterOr;
};


//ִ�п�ѭ������
struct T_LOOP_SETTING:public CEnDeCode
{
	E_LOOP_TYPE eLoopType ;  //ѭ������
	CStdString strValue ;   //����ֵ
	bool bSubBlockRandom;   //�Ƿ����ִ���ӿ�
	//int  nRandomCount;      //ִ���ӿ�Ĵ���
	CStdString strRandomCount;//ִ���ӿ�Ĵ���

	T_LOOP_SETTING()
	{
		Clear() ;
	}

	void Clear()
	{
		eLoopType = eltLoopMax ;
		strValue.Empty() ;
		bSubBlockRandom = false;
		strRandomCount = _T("0");
	}

	CStdString GetSource()
	{
		CStdString strData ;
		
		CStdString strLoopType ;
		strLoopType.Format( _T("%d"), eLoopType) ;
		strData += Encode(strLoopType) + _T(",") ;

		strData += Encode(strValue) + _T(",");

		CStdString strSubBlockRandom;
		strSubBlockRandom.Format(_T("%d"), bSubBlockRandom);
		strData += Encode(strSubBlockRandom) + _T(",");

		//CStdString strRandomCount;
		//strRandomCount.Format(_T("%d"), nRandomCount);
		//strData += Encode(strRandomCount);
		strData += Encode(strRandomCount);

		return Encode(strData) ;
	}

	void SetSource(CStdString &strData) 
	{
		std::vector<CStdString> vecLoopSetting ;
		int nSize = SplitStringWithSeparator(strData ,ENCODE_SEPARATOR , vecLoopSetting) ;

		int i = 0 ;
		if ( i<nSize )
		{
			eLoopType = (E_LOOP_TYPE)_ttoi( Decode(vecLoopSetting[i++]) ) ;
		}

		if ( i<nSize )
		{
			strValue = Decode(vecLoopSetting[i++]) ;
		}

		if (i < nSize)
		{
			bSubBlockRandom = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i < nSize)
		{
			strRandomCount = Decode(vecLoopSetting[i++]);
			//nRandomCount = _ttoi(Decode(vecLoopSetting[i++]));
		}
	}
};

//��ת��������
struct T_JUMP_SETTING:public CEnDeCode
{
	bool bJump;//�Ƿ���ת
	bool bConditionFirst;//��������
	bool bSuccJump; //�ɹ���ת
	long nSucc;//�ɹ�ִ�к���ת���ĸ�ģ��
	bool bFailJump; //ʧ����ת
	long nFail;//�ɹ�ִ�к���ת���ĸ�ģ��
	bool bConditionJump; //������ת
	long nCondition;//�ɹ�ִ�к���ת���ĸ�ģ��
	long nJumpType;//��ת���ͣ�0��ʾ������ת��1��ʾ�ַ�����ת
	CStdString strLeft;//�������
	CStdString strRight;//�Ҳ�����
	long nCompare;//�Ƚϣ����ڵ���С�ڣ�
	
	T_JUMP_SETTING()
	{
		Clear() ;
	}

	void Clear()
	{
		bJump = false;//�Ƿ���ת
		bConditionFirst = false;//��������
		bSuccJump = false; //�ɹ���ת
		nSucc = 0;//�ɹ�ִ�к���ת���ĸ�ģ��
		bFailJump = false; //ʧ����ת
		nFail = 0;//�ɹ�ִ�к���ת���ĸ�ģ��
		bConditionJump = false; //������ת
		nJumpType = 0;
		nCondition = 0;//�ɹ�ִ�к���ת���ĸ�ģ��
		strLeft = _T("");//�������
		strRight = _T("");//�Ҳ�����
		nCompare = 0;//�Ƚϣ����ڵ���С�ڣ�
	}

	CStdString GetSource()
	{
		CStdString strData;
		CStdString strTmp;

		strTmp.Format(_T("%d"), bJump) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), bConditionFirst) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), bSuccJump) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), nSucc) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), bFailJump) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), nFail) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), bConditionJump) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), nCondition) ;
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), nJumpType) ;
		strData += Encode(strTmp) + _T(",");

		//strTmp.Format(_T("%d"), nLeft) ;
		strData += Encode(strLeft) + _T(",");

		//strTmp.Format(_T("%d"), nRight) ;
		strData += Encode(strRight) + _T(",");

		strTmp.Format(_T("%d"), nCompare) ;
		strData += Encode(strTmp) + _T(",");

		return Encode(strData) ;
	}

	void SetSource(CStdString &strData) 
	{
		std::vector<CStdString> vecLoopSetting ;
		int nSize = SplitStringWithSeparator(strData ,ENCODE_SEPARATOR , vecLoopSetting);

		int i = 0;
		if (i<nSize)
		{
			bJump = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			bConditionFirst = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			bSuccJump = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			nSucc = _ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			bFailJump = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			nFail = _ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			bConditionJump = (bool)_ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			nCondition = _ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			nJumpType = _ttoi(Decode(vecLoopSetting[i++]));
		}

		if (i<nSize)
		{
			strLeft = Decode(vecLoopSetting[i++]);
		}

		if (i<nSize)
		{
			strRight = Decode(vecLoopSetting[i++]);
		}

		if (i<nSize)
		{
			nCompare = _ttoi(Decode(vecLoopSetting[i++]));
		}
	}
};

////ѭ������(Ŀǰֻ֧�ָ���ѭ������ִ�У���������չΪ��������ִ�У���ɾ��ʱֻɾ��������Ϣ)
////����ִ��һ�Σ���Ҫʱ�����ݿ�����Ϊ�ݴ�
//struct T_LOOP_SETTING
//{
//	DWORD dwMode;                  //ָ��ʹ�ú���ѭ����ʽ(1:��ѭ��������2:���ҵ�����������
//	DWORD dwLoopTimes;             //ѭ��������ִ��ʱ����ȡ�Ķ����滻
//	CStdString strKeywordsInfo;       //��λ��̬�ı�־����"ҳ��<@@>ҳ",����<@@>��ʾ����
//
//	//std::vector<CStdString> vKeywordsInfo;   //�����б� ��<@@@>Ϊ�ָ�����������������δ���<@@>��ʾ
//};

//�ϲ�ѹ���ں˵����ݽṹ
struct T_DATA_IN : public CEnDeCode
{
	CStdString strCmd;              //����
	CStdString strNsName;           //��վ����(�����¼��־)
	CStdString strInDataset;        //���ݿ�
	BOOL bNoImage;                  //ִ������ʱ������ͼƬ
	DWORD dwMaxViewCount;           //���������ʹ�õ�ҳ������
	CStdString strOCRParam;			//��֤�����

	T_DATA_IN()
	{
		strCmd = _T("");
		strNsName = _T("");
		strInDataset = _T("");
		strOCRParam = _T("");
		bNoImage = FALSE;
		dwMaxViewCount = 2;
	}

	CStdString GetSource() 
	{
		CStdString strData = Encode(strCmd) + _T(",") + Encode(strNsName) + _T(",") + Encode(strInDataset) + _T(",");

		CStdString strTmp = _T("");
		strTmp.Format(_T("%d"), bNoImage);
		strData += Encode(strTmp) + _T(",");

		strTmp.Format(_T("%d"), dwMaxViewCount);
		strData += Encode(strTmp) + _T(",");

		strData += Encode(strOCRParam);
		
		return Encode(strData) ;
	}

	void SetSource(CStdString &strData) 
	{
		std::vector<CStdString> vecDataIn ;
		int nSize = SplitStringWithSeparator(Decode(strData) , ENCODE_SEPARATOR , vecDataIn) ;

		int i = 0 ;
		if ( i<nSize )
		{
			strCmd = Decode( vecDataIn[i++] ) ;
		}
		if ( i<nSize )
		{
			strNsName = Decode( vecDataIn[i++] );
		}
		if ( i<nSize )
		{
			strInDataset = Decode( vecDataIn[i++] ) ;
		}
		if ( i<nSize )
		{
			bNoImage = _ttoi(Decode( vecDataIn[i++] )) ;
		}	
		if ( i<nSize )
		{
			dwMaxViewCount = _ttol(Decode( vecDataIn[i++] )) ;
		}
		if (i< nSize)
		{
			strOCRParam = Decode(vecDataIn[i++]);
		}
	}
};

//�ں˷��ظ��ϲ�����ݽṹ
struct T_DATA_OUT : public CEnDeCode
{
	CStdString strResponseType;     //��Ӧ����
	E_RESULT eResult;               //������
	CStdString strErrDesc;          //��������
	CStdString strQuickLook;        //�������ݸ�ʽ����ҳBODY�ı�<@@@>[����·���б�]
	                                //����·���б�ĸ�ʽ������1<@@>����2...<@@>����N
	CStdString strOutDataset;       //���صĶ������ݼ�
	CStdString strTaskID;           //����ID
	CStdString strCodeOcrInfo;      //��֤��ʶ����Ϣ ��ʽ����֤���|�Ƿ���ʾ��֤��|ʶ��|��֤��|��֤������

	CStdString strCutImg;           //�ؼ��ʽ�ͼ·��

	T_DATA_OUT()
	{
		strResponseType = _T("");
		eResult = srNoneResult;
		strErrDesc = _T("");
		strQuickLook = _T("");
		strOutDataset = _T("");
		strTaskID = _T("");
		strCodeOcrInfo = _T("");
		strCutImg = _T("");
	}

	CStdString GetSource() 
	{
		CStdString strData = _T("") ;
		strData += Encode(strResponseType) + _T(",") ;

		CStdString strEresult = _T("") ;
		strEresult.Format( _T("%d") , eResult) ;
		strData += Encode(strEresult) + _T(",") ;
		strData += Encode(strErrDesc) + _T(",") ;
		strData += Encode(strQuickLook) + _T(",") ;
		strData += Encode(strOutDataset) + _T(",") ;
		strData += Encode(strTaskID) + _T(",") ;
		strData += Encode(strCodeOcrInfo) + _T(",") ;
		strData += Encode(strCutImg);

		return Encode(strData) ;
	}

	void SetSource(CStdString &strData) 
	{
		std::vector<CStdString> vecDataOut ;
		int nSize = SplitStringWithSeparator( Decode(strData) , ENCODE_SEPARATOR , vecDataOut) ;

		int i = 0 ;
		if ( i< nSize )
		{
			strResponseType = Decode( vecDataOut[i++] ) ;
		}
		if ( i<nSize )
		{
			eResult = (E_RESULT)_ttoi( Decode(vecDataOut[i++]) ) ;
		}
		if ( i<nSize )
		{
			strErrDesc = Decode( vecDataOut[i++] ) ;
		}
		if ( i<nSize )
		{
			strQuickLook = Decode( vecDataOut[i++] ) ;
		}
		if ( i<nSize )
		{
			strOutDataset = Decode( vecDataOut[i++] ) ;
		}
		if ( i<nSize )
		{
			strTaskID = Decode(vecDataOut[i++] ) ;
		}
		if ( i<nSize )
		{
			strCodeOcrInfo = Decode(vecDataOut[i++] ) ;
		}
		if( i<nSize )
		{
			strCutImg = Decode(vecDataOut[i++] ) ;
		}
	}
};

struct T_EXCHANGE_DATA
{
	//CStdString strTaskID;        //����ID
	E_EXCHANGE_STATE state;      //����״̬
	T_DATA_IN dataIn;            //��������
	T_DATA_OUT dataOut;          //������
};

struct T_EmbedItemData : public CEnDeCode//�����洢list�е�ÿһ�������,��Ƕ�빦�����ݱ��е�ÿһ��
{
	CStdString strWebId;//��վ��ID
	int nFunction;		//��վId�е�ѡ�й���
	int nData;			//��վѡ�й����е�ѡ������
	E_StationFunction eFunction;
	//˵�������������ݵ�Ŀ�ģ����Ҫ���Ҷ�Ӧ�ַ��������Ը���g_pEditDlg�� m_vecFuncDataType��m_vecTaskType�У�������ȡ

	CStdString GetSource() 
	{
		CStdString strData = _T("") ;
		strData += Encode(strWebId) + _T(",");

		CStdString strFunction = _T("");
		strFunction.Format( _T("%d"), nFunction);
		strData += Encode(strFunction) + _T(",");

		CStdString strSubData = _T("");
		strSubData.Format( _T("%d"), nData);
		strData += Encode(strSubData) + _T(",");

		CStdString strSubFunc = _T("");
		strSubFunc.Format( _T("%d"), eFunction);
		strData += Encode(strSubFunc);

		return Encode(strData) ;
	}

	void SetSource(CStdString &strData) 
	{
		std::vector<CStdString> vecDataEmbed;
		int nSize = SplitStringWithSeparator(Decode(strData), ENCODE_SEPARATOR, vecDataEmbed);

		int i = 0 ;
		if (i < nSize)
		{
			strWebId = Decode(vecDataEmbed[i++]);
		}

		if (i < nSize)
		{
			nFunction = (E_RESULT)_ttoi(Decode(vecDataEmbed[i++]));
		}

		if (i < nSize)
		{
			nData = (E_RESULT)_ttoi( Decode(vecDataEmbed[i++]));
		}

		if (i < nSize)
		{
			eFunction = (E_StationFunction)_ttoi( Decode(vecDataEmbed[i++]));
		}
	}
};

#define MAIN_SPLITOR _T("<@@@>")
#define SUB_SPLITOR  _T("<@@>")