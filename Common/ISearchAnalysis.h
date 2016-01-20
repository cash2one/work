// ISearchAnalysis.h: interface for the ISearchAnalysis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISearchAnalysis_H__028649C0_F20C_494C_7054_0C44B25C8046__INCLUDED_)
#define AFX_ISearchAnalysis_H__028649C0_F20C_494C_7054_0C44B25C8046__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ISearchAnalysis  
{
public:
	virtual TCHAR * GetInfo(TCHAR *Param,int & retLen) = 0;
	virtual TCHAR * GetIndexInfo(TCHAR *Param,int & retLen) = 0;
	virtual BOOL FreeMemory(TCHAR *pFree) = 0;
};

typedef ISearchAnalysis* (*ProcCreateSearchObject)();
typedef BOOL (*ProcDestorySearchObject)(ISearchAnalysis* IObj);

#endif // !defined(AFX_ISearchAnalysis_H__028649C0_F20C_494C_7054_0C44B25C8046__INCLUDED_)
