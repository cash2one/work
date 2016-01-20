#pragma once

#include "StdString.h"
#include <vector>

#define ENCODE_SEPARATOR _T(",")

class CEnDeCode
{
private:

public:
	CStdString Encode(const CStdString &strSource) ;
	CStdString Decode(const CStdString &strCode) ;

	int SplitStringWithSeparator(const CStdString &strSource , const CStdString &strSeparator , std::vector<CStdString> &strVector , BOOL bIsIncludeEmpty=TRUE) ; 
};