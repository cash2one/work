#pragma once

#include "enumdef.h"
#include "StdString.h"
#include "EnDeCode.h"
#include <map>
#include <vector>


//属性名/值
struct T_Name_Value
{
	CStdString strName;       
	CStdString strValue;
};

//对象标记数据
struct T_ITEM
{
	CStdString strTagName;                         //标签类型
	std::map<CStdString, CStdString> mapAttributes;   //属性列表 属性名/值

	T_ITEM *pParent ;        //父亲节点
	T_ITEM *pBrother ;     //兄弟节点

	//增加节点树所需要的变量
	//std::vector<T_ITEM *> m_vecNodes;//当前节点的所有子节点，如果是父节点的话，这里就只有一个，根据m_nPos为-1判断
	//std::vector<int> m_vecPos;//当前节点位于兄弟节点的第几个（从0开始），如果是最上层的父节点或者没有树，这个值为-1
	std::map<int, T_ITEM *> m_mapNodesAttr;//代码变量意义看上面2行，没有父节点这个变量大小为0
	//int m_nLevel;//当前节点位于父节点的第几层，父节点为第0层，暂时不用
	
	T_ITEM()
	{
		strTagName = _T("");
		pParent = NULL;
		pBrother = NULL;
		mapAttributes.clear();
		m_mapNodesAttr.clear();
	}

	//获取所有属性名
	void GetAttruteNames(std::vector<CStdString> &vName)
	{
		vName.clear();

		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.begin();
		for (; itAttr != mapAttributes.end(); ++itAttr)
			vName.push_back(itAttr->first);
	}

	//返回指定属性的值
	CStdString GetAttribute(CStdString strName)
	{
		std::map<CStdString, CStdString>::iterator itAttr = mapAttributes.find(strName);
		if (itAttr != mapAttributes.end())
			return mapAttributes[strName];		
		else
			return _T("");
	}

	//检查属性是否存在
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


//对象要返回的数据
struct T_STEP_RETURN
{
	CStdString strAttrib;
	CStdString strRule;
	CStdString strValue;
};

//任务过滤特征
struct  T_FILTER_STRUCTURE
{
	std::vector<CStdString > straAnd;
	std::vector<CStdString > straNot;
	std::vector<CStdString > straOr;
};

//单个过滤特征
struct T_FILTER_ITEM
{
	T_ITEM tItem ;             //特征对象定位
	CStdString strAttributeName ;   //获取特征的属性名
	CStdString strFeature ;             //采集的成功标记
};

//过滤特征
struct T_FILTER_STRUCT
{
	std::vector<T_FILTER_ITEM> vFilterAnd;
	std::vector<T_FILTER_ITEM> vFilterNot;
	std::vector<T_FILTER_ITEM> vFilterOr;
};


//执行块循环条件
struct T_LOOP_SETTING:public CEnDeCode
{
	E_LOOP_TYPE eLoopType ;  //循环类型
	CStdString strValue ;   //类型值
	bool bSubBlockRandom;   //是否随机执行子块
	//int  nRandomCount;      //执行子块的次数
	CStdString strRandomCount;//执行子块的次数

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

//跳转条件设置
struct T_JUMP_SETTING:public CEnDeCode
{
	bool bJump;//是否跳转
	bool bConditionFirst;//条件优先
	bool bSuccJump; //成功跳转
	long nSucc;//成功执行后跳转到哪个模块
	bool bFailJump; //失败跳转
	long nFail;//成功执行后跳转到哪个模块
	bool bConditionJump; //条件跳转
	long nCondition;//成功执行后跳转到哪个模块
	long nJumpType;//跳转类型，0表示整型跳转，1表示字符串跳转
	CStdString strLeft;//左侧条件
	CStdString strRight;//右侧条件
	long nCompare;//比较（大于等于小于）
	
	T_JUMP_SETTING()
	{
		Clear() ;
	}

	void Clear()
	{
		bJump = false;//是否跳转
		bConditionFirst = false;//条件优先
		bSuccJump = false; //成功跳转
		nSucc = 0;//成功执行后跳转到哪个模块
		bFailJump = false; //失败跳转
		nFail = 0;//成功执行后跳转到哪个模块
		bConditionJump = false; //条件跳转
		nJumpType = 0;
		nCondition = 0;//成功执行后跳转到哪个模块
		strLeft = _T("");//左侧条件
		strRight = _T("");//右侧条件
		nCompare = 0;//比较（大于等于小于）
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

////循环条件(目前只支持根据循环次数执行，后续可扩展为按照条件执行，如删除时只删除过期信息)
////至少执行一次，必要时将数据块设置为容错
//struct T_LOOP_SETTING
//{
//	DWORD dwMode;                  //指定使用何种循环方式(1:以循环次数，2:以找到结束特征）
//	DWORD dwLoopTimes;             //循环次数，执行时用提取的动参替换
//	CStdString strKeywordsInfo;       //定位动态的标志，如"页共<@@>页",其中<@@>表示动参
//
//	//std::vector<CStdString> vKeywordsInfo;   //动参列表 以<@@@>为分隔多个动参描述，动参处以<@@>表示
//};

//上层压给内核的数据结构
struct T_DATA_IN : public CEnDeCode
{
	CStdString strCmd;              //命令
	CStdString strNsName;           //网站名称(方便记录日志)
	CStdString strInDataset;        //数据块
	BOOL bNoImage;                  //执行任务时不加载图片
	DWORD dwMaxViewCount;           //该任务最多使用的页面数量
	CStdString strOCRParam;			//验证码参数

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

//内核返回给上层的数据结构
struct T_DATA_OUT : public CEnDeCode
{
	CStdString strResponseType;     //响应类型
	E_RESULT eResult;               //任务结果
	CStdString strErrDesc;          //错误描述
	CStdString strQuickLook;        //快照数据格式：网页BODY文本<@@@>[快照路径列表]
	                                //快照路径列表的格式：快照1<@@>快照2...<@@>快照N
	CStdString strOutDataset;       //返回的对象数据集
	CStdString strTaskID;           //任务ID
	CStdString strCodeOcrInfo;      //验证码识别信息 格式：验证结果|是否显示验证码|识别串|验证串|验证码类型

	CStdString strCutImg;           //关键词截图路径

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
	//CStdString strTaskID;        //任务ID
	E_EXCHANGE_STATE state;      //任务状态
	T_DATA_IN dataIn;            //任务数据
	T_DATA_OUT dataOut;          //任务结果
};

struct T_EmbedItemData : public CEnDeCode//用来存储list中的每一项的内容,在嵌入功能数据表中的每一项
{
	CStdString strWebId;//网站的ID
	int nFunction;		//网站Id中的选中功能
	int nData;			//网站选中功能中的选中数据
	E_StationFunction eFunction;
	//说明这样定义数据的目的，如果要查找对应字符串，可以根据g_pEditDlg的 m_vecFuncDataType，m_vecTaskType中，不用再取

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