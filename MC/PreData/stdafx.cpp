// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// TestDll.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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
    if (!nInLen) return 1;//������ַ���Ϊ�մ�

    if (PPD_BBodyLen_TYPE == eType && 0 == nLimtLen) return 2;//���Ƴ���Ϊ��,���ҵ�ǰ������Ϊ���Ƴ�������

    //if ((PPD_BBodyC_TYPE == eType || 
    //     PPD_BBodyE_TYPE == eType || 
    //     PPD_WWebsH_TYPE == eType || 
    //     PPD_BBodyCI_TYPE == eType ) && (0 == nTempLen))
    //     return 3;//��ǰ������������ַ�������

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

