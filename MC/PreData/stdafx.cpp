// stdafx.cpp : 只包括标准包含文件的源文件
// TestDll.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

IGlobalDataBlock* g_GlobalDataBlock = NULL;
IConfigInfo *g_Config = NULL;

CLogTrace g_sysLog(_T("PreData.log"), NULL);

CStdString strCity = _T("");
CStdString strProv = _T("");




DWORD LimitedStr(const CStdString &strInStr, CStdString &strOutStr, 
                 E_PPD_TRIM_TYPE eType, CStdString strCutStr, int nLimtLen)
{
    int nTempLen = strCutStr.GetLength();
    int nInLen = strInStr.GetLength();
    if (!nInLen) return 1;//输入的字符串为空串

    if (PPD_BBodyLen_TYPE == eType && 0 == nLimtLen) return 2;//限制长度为空,并且当前的类型为限制长度类型

    //if ((PPD_BBodyC_TYPE == eType || 
    //     PPD_BBodyE_TYPE == eType || 
    //     PPD_WWebsH_TYPE == eType || 
    //     PPD_BBodyCI_TYPE == eType ) && (0 == nTempLen))
    //     return 3;//当前类型与输入的字符串不符

    strOutStr = strInStr;

    switch(eType)
    {
    case PPD_BBodyC_TYPE:
    case PPD_BBodyE_TYPE:
    case PPD_WWebsH_TYPE:
    case PPD_BBodyCI_TYPE:
        strOutStr.Replace(strCutStr, _T(""));
        break;
    case PPD_BBodyLen_TYPE:
        {
            if (nInLen < nTempLen)
                return 0;

            strOutStr = strOutStr.Left(nLimtLen - 1);
        }
        break;
    default:
        return 100;
    }

    return 0;
}

