#ifndef _CRECOGFACTORY_H_
#define _CRECOGFACTORY_H_
#include "stdafx.h"

// 识别类厂
class CRecogFactory
{
public:
	// 根据类型返回指针
	static IRecognize* Create(int iType);
};

IRecognize* CRecogFactory::Create(int iType)
{
	IRecognize* pRecognize = NULL;

	switch(iType)
	{
	case 0:
		break;
	case 1:
		pRecognize = new CUUColdRecognize();
		break;
	case 2:
		pRecognize = new YDMRecognize();
		break;
	default:
		pRecognize = new YDMRecognize();
		break;
	}

	return pRecognize;
}

#endif