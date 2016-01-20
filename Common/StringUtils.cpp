
#include "stdafx.h" // comment if not using precompiled headers in MVC++
#include "StringUtils.h"

DWORD StringUtils::SplitStringToInt(const CStdString& input, const CStdString& delimiter, std::vector<int>& results, bool includeEmpties)
{
	int numFound,intValue;
	TCHAR TempTch[MAX_ID_STRING_LENGTH];
	std::vector<CStdString> result;
	numFound = SplitString(input,delimiter,result,includeEmpties);

	if (numFound > 0)
	{
		for(int i=0;i<numFound;i++)
		{		
			_tcscpy(TempTch , result[0]);
			intValue =_ttol( TempTch);
			results.push_back(intValue);
		}
	}

	return numFound;
}

DWORD  StringUtils::SplitStringToIntArray(const CStdString& input, const CStdString& delimiter, int * & intresult , bool includeEmpties )
{
	int numFound,intValue;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	std::vector<CStdString> result;
	numFound = SplitString(input,delimiter,result,includeEmpties);

	if (numFound > 0)
	{
		int * intRet  = new int[numFound];
		for(int i=0;i<numFound;i++)
		{		
			_tcscpy(TempTch , result[0]);
			intValue =_ttol( TempTch);
			intRet[i] = intValue;
		}

		intresult = intRet;
		
		if (intRet)							//zhl  ��ֹ�ڴ�й©
		{
			delete[] intRet;
			intRet=NULL;
		}
	}

	return numFound;
}

//��,GUI���͹������ض���ʽ��VBStringת��Ϊһά���� ���ò��Ѿ������������ռ�,ֱ�����ͺ���,
DWORD StringUtils::SplitVBStringTo1DIntArray(const CStdString& input, const CStdString& delimiter, int * & intresult , bool includeEmpties /* = true */)
{
	int numFound,intValue;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	std::vector<CStdString> result;
	numFound = SplitString(input,delimiter,result,includeEmpties);

	if (numFound > 1)
	{
		for(int i = 2; i < numFound; i++)
		{		
			_tcscpy(TempTch , result[i]);
			intValue =_ttol( TempTch);
			intresult[i - 2] = intValue;
		}
	}

	return 0;
}

//��,GUI���͹������ض���ʽ��VBStringת��Ϊ��ά���� ���ò��Ѿ������������ռ�,ֱ�����ͺ���,
DWORD StringUtils::SplitVBStringTo2DIntArray(const CStdString& strData, const CStdString& delimiter, 
	const CStdString& delimiter2, int & intresult , 
	bool includeEmpties)
{

	int ItemCount;
	CStdString szDelimiter,szDelimiter1;
	CStdString szTemp,szTempItem;
	TCHAR TempTch[MAX_ID_STRING_LENGTH];
	std::vector<CStdString> results;
	std::vector<CStdString> resultItem;

	int nCount = SplitString(strData,delimiter,results,true);
	int intindex;

	if (2 >=nCount ) 
	{
		return 0;
	}
	else
	{
		for(int i=2;i<nCount;i++)
		{		
			szTemp=results[i];
			resultItem.clear () ;
			ItemCount =SplitString(szTemp, delimiter2,resultItem,true)	;

			intindex = i-2;


			for(int j = 0; j < ItemCount; j++)
			{		
				_tcscpy(TempTch , resultItem[j]);
				//                    intresult[intindex][j] = _ttol(TempTch);
			}
		}   

	}

	return 0;
}






//��ȡVB���������ַ�����һά����Ĵ�С�ͷָ���
DWORD StringUtils::Get1DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount )
{
	DWORD dwIndex,dwSecond;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strCount;

	intCount = 0;

	dwIndex = input.Find(TAIL_SPLIT);

	delimiter = input.Mid(0,dwIndex +1);

	dwSecond = input.Find(PRE_SPLIT,dwIndex);

	strCount = input.Mid(dwIndex +1,dwSecond - dwIndex -1);

	if (strCount.length() < MAX_ID_STRING_LENGTH)
	{
		_tcscpy(TempTch,strCount);
		intCount = _ttol(TempTch);
	}

	return 0;
}


//��ȡVB���������ַ����Ķ�ά����Ĵ�С�ͷָ���
DWORD StringUtils::Get2DSplitCharAndLengthVBString(const CStdString& input, CStdString& delimiter, int & intCount , CStdString& delimiter2, int & intCount2 )
{


	DWORD dwIndex,dwSecond;
	TCHAR TempTch[MAX_ID_STRING_LENGTH] = {0};
	CStdString strCount;

	intCount = 0;

	intCount2 = 0;

	dwIndex = input.Find(TAIL_SPLIT);

	delimiter = input.Mid(0,dwIndex +1);

	dwSecond = input.Find(PRE_SPLIT,dwIndex);

	strCount = input.Mid(dwIndex +1,dwSecond - dwIndex -1);

	if (strCount.length() < MAX_ID_STRING_LENGTH)
	{
		_tcscpy(TempTch,strCount);
		intCount = _ttol(TempTch);
	}


	dwIndex = input.Find(TAIL_SPLIT,dwSecond);

	delimiter2 = input.Mid(dwSecond ,dwIndex - dwSecond +1);

	dwSecond = input.Find(PRE_SPLIT,dwIndex);

	strCount = input.Mid(dwIndex +1,dwSecond - dwIndex -1);

	if (strCount.length() < MAX_ID_STRING_LENGTH)
	{
		_tcscpy(TempTch,strCount);
		intCount2 = _ttol(TempTch);
	}


	return 0;

}

int StringUtils::mrand()//2147483640
{
	int dwRet = 2147483640;
	LARGE_INTEGER l;
	QueryPerformanceCounter(&l);
	dwRet = l.QuadPart % 2147483640;
	dwRet++;
	return dwRet;
}

int StringUtils::GetRand(int nStart,int nEnd)
{
	int nLen = nEnd - nStart;
	int nrand = mrand() % nLen;
	return nStart+nrand;
}
CStdString StringUtils::GetYunMu(int Num)
{
	CStdString strRet = _T("");
	int Number[24] = {14,17,14,8,15,15,15,15,13,8,15,15,18,17,14,15,12,15,10,18,9,8,13,15};
	TCHAR szYun0[14][5] = {{_T("a")},{_T("o")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ing")},{_T("eng")},{_T("ang")},{_T("in")},{_T("en")},{_T("an")},{_T("ie")},{_T("ao")}};
	TCHAR szYun1[17][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("iu")},{_T("an")},{_T("un")},{_T("an")},{_T("en")},{_T("ing")},{_T("on")},{_T("ie")}};
	TCHAR szYun2[14][5] = {{_T("a")},{_T("e")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun3[8][5] = {{_T("i")},{_T("u")},{_T("iu")},{_T("in")},{_T("un")},{_T("ing")},{_T("ue")},{_T("ie")}};
	TCHAR szYun4[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun5[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun6[15][5] = {{_T("a")},{_T("o")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ou")},{_T("ie")},{_T("en")},{_T("in")},{_T("ing")},{_T("eng")},{_T("ang")},{_T("an")},{_T("ao")}};
	TCHAR szYun7[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ing")},{_T("ong")},{_T("ie")}};
	TCHAR szYun8[13][5] = {{_T("a")},{_T("e")},{_T("u")},{_T("ai")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun9[8][5] = {{_T("i")},{_T("u")},{_T("iu")},{_T("in")},{_T("un")},{_T("ing")},{_T("ue")},{_T("ie")}};
	TCHAR szYun10[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun11[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun12[18][5] = {{_T("a")},{_T("o")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ou")},{_T("ie")},{_T("en")},{_T("in")},{_T("ing")},{_T("eng")},{_T("ang")},{_T("an")},{_T("e")},{_T("iu")},{_T("ao")},{_T("iao")}};
	TCHAR szYun13[17][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ao")},{_T("ou")},{_T("iu")},{_T("an")},{_T("en")},{_T("in")},{_T("ang")},{_T("eng")},{_T("ing")},{_T("ong")},{_T("ie")}};
	TCHAR szYun14[14][5] = {{_T("a")},{_T("e")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun15[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun16[12][5] = {{_T("e")},{_T("i")},{_T("u")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun17[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};
	TCHAR szYun18[10][5] = {{_T("a")},{_T("o")},{_T("u")},{_T("ei")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("ang")},{_T("eng")}};
	TCHAR szYun19[18][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ao")},{_T("ou")},{_T("iu")},{_T("an")},{_T("in")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ing")},{_T("ong")},{_T("ue")},{_T("ie")}};
	TCHAR szYun20[9][5] = {{_T("a")},{_T("o")},{_T("u")},{_T("ai")},{_T("ei")},{_T("an")},{_T("en")},{_T("ang")},{_T("eng")}};
	TCHAR szYun21[8][5] = {{_T("i")},{_T("u")},{_T("iu")},{_T("in")},{_T("un")},{_T("ing")},{_T("ue")},{_T("ie")}};
	TCHAR szYun22[13][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ao")},{_T("ou")},{_T("an")},{_T("in")},{_T("un")},{_T("ang")},{_T("ing")},{_T("ong")},{_T("ue")}};
	TCHAR szYun23[15][5] = {{_T("a")},{_T("e")},{_T("i")},{_T("u")},{_T("ai")},{_T("ei")},{_T("ui")},{_T("ao")},{_T("ou")},{_T("an")},{_T("en")},{_T("un")},{_T("ang")},{_T("eng")},{_T("ong")}};

	int yNum = Number[Num];
	int yun = GetRand(0,yNum);
	switch(Num) 
	{
		case 0 : // 
			strRet = szYun0[yun];
			break;
		case 1 : // 
			strRet = szYun1[yun];
			break;
		case 2 : // 
			strRet = szYun2[yun];
			break;
		case 3 : // 
			strRet = szYun3[yun];
			break;
		case 4 : // 
			strRet = szYun4[yun];
			break;
		case 5 : // 
			strRet = szYun5[yun];
			break;
		case 6 : // 
			strRet = szYun6[yun];
			break;
		case 7 : // 
			strRet = szYun7[yun];
			break;
		case 8 : // 
			strRet = szYun8[yun];
			break;
		case 9 : // 
			strRet = szYun9[yun];
			break;
		case 10 : // 
			strRet = szYun10[yun];
			break;
		case 11 : // 
			strRet = szYun11[yun];
			break;
		case 12 : // 
			strRet = szYun12[yun];
			break;
		case 13 : // 
			strRet = szYun13[yun];
			break;
		case 14 : // 
			strRet = szYun14[yun];
			break;
		case 15 : // 
			strRet = szYun15[yun];
			break;
		case 16 : // 
			strRet = szYun16[yun];
			break;
		case 17 : // 
			strRet = szYun17[yun];
			break;
		case 18 : // 
			strRet = szYun18[yun];
			break;
		case 19 : // 
			strRet = szYun19[yun];
			break;
		case 20 : // 
			strRet = szYun20[yun];
			break;
		case 21 : // 
			strRet = szYun21[yun];
			break;
		case 22 : // 
			strRet = szYun22[yun];
			break;
		case 23 : // 
			strRet = szYun23[yun];
			break;

		default:
			break;
    }
	return strRet;
}

DWORD StringUtils::GetRandEName(CStdString &strOut)
{
	TCHAR szSeng[24][4] = {
		{_T("b")},
		{_T("d")},
		{_T("g")},
		{_T("j")},
		{_T("zh")},
		{_T("z")},
		{_T("p")},
		{_T("t")},
		{_T("k")},
		{_T("q")},
		{_T("ch")},
		{_T("c")},
		{_T("m")},
		{_T("n")},
		{_T("h")},
		{_T("sh")},
		{_T("r")},
		{_T("s")},
		{_T("f")},
		{_T("l")},
		{_T("w")},
		{_T("x")},
		{_T("y")},
		{_T("z")}
	};

	int CharNum = GetRand(2,4);
	for(int i=0;i<CharNum;i++)
	{
		CStdString szTem = _T("");
		int s = GetRand(0,24);
		szTem.Format(_T("%s%s"),szSeng[s],GetYunMu(s));
		strOut += szTem;
	}

	return 0;
}

DWORD StringUtils::CreateUserName(CStdString& strOut)
{
	TCHAR szSeng[24][4] = {
		{_T("b")},
		{_T("d")},
		{_T("g")},
		{_T("j")},
		{_T("zh")},
		{_T("z")},
		{_T("p")},
		{_T("t")},
		{_T("k")},
		{_T("q")},
		{_T("ch")},
		{_T("c")},
		{_T("m")},
		{_T("n")},
		{_T("h")},
		{_T("sh")},
		{_T("r")},
		{_T("s")},
		{_T("f")},
		{_T("l")},
		{_T("w")},
		{_T("x")},
		{_T("y")},
		{_T("z")}
	};

	int CharNum = GetRand(2,4);
	CStdString szTemp = _T("");
	CStdString szTime = _T("");
	for(int i=0;i<CharNum;i++)
	{
		CStdString szTem = _T("");
		int s = GetRand(0,24);
		szTem.Format(_T("%s%s"),szSeng[s],GetYunMu(s));
		szTemp += szTem;
	}
	int year = GetRand(1940,2010);
	int month = GetRand(1,13);
	int day = GetRand(1,31);
	if(szTemp.GetLength() < 6)
	{
		szTime.Format(_T("%d%d%d"),year,month,day);
	}
	else
	{
		if(CharNum == 3)
		{
			szTime.Format(_T("%02d%02d"),month,day);
		}
		if(CharNum == 2)
		{
			szTime.Format(_T("%d"),year);
		}
	}
	szTemp += szTime;

	if(szTemp.GetLength() > 16)
	{
		strOut = szTemp.Left(16);
	}
	else
	{
		strOut = szTemp;
	}
	return 1;
}

DWORD StringUtils::CreateUserName2(CStdString& strOut, E_Account_Style eFirstChType)
{
	CStdString szTemp;
	CStdString szTime;

	CStdString strRecommendZN;
	CStdString strRecommendEN;

	if (asFIRST_CHINESE == eFirstChType)
	{
		//Ҫ�������Ŀ�ͷ���Ƽ���Ϊ��ʱ�����������
		GetRandCName(strRecommendZN);
	}
	else
	{
		GetRandEName(strRecommendEN);
	}
	
	MatchFirstCharStr(szTemp, eFirstChType, strRecommendZN, strRecommendEN);

	int year = GetRand(1940,2010);
	int month = GetRand(1,13);
	int day = GetRand(1,31);
	if(szTemp.GetLength() < 6)
	{
		if (asFIRST_CHINESE == eFirstChType)
		{
			szTime.Format(_T("%d"),year);  //������������
		}
		else if (asONLY_CHARACTER == eFirstChType)
		{
			GetRandEName(szTime);
		}
		else
		{
			szTime.Format(_T("%d%d%d"),year,month,day);
		}
	}
	else
	{
		if (asONLY_CHARACTER == eFirstChType)
		{
			GetRandEName(szTime);
		}
		else
		{
			szTime.Format(_T("%02d%02d"), month, day);
		}
			
	}
	szTemp += szTime;

	if(szTemp.GetLength() > 16)
	{
		strOut = szTemp.Left(16);
	}
	else
	{
		strOut = szTemp;
	}

	strOut.Replace(_T(" "), _T(""));
	return 1;
}

DWORD StringUtils::GetRandCName(CStdString &strOut)
{
	TCHAR cCNXing[] = _T("��Ǯ��������֣��������������������������ʩ�ſײ��ϻ���κ��\
����л������ˮ��������˸�����³Τ������ﻨ������Ԭ����\
ʷ�Ʒ��Ѧ�׺����������ޱϺ���������ʱ���뿵��Ԫ������\
��������Ҧ��տ����ë����״���é���ܼ�������ף��������ϯ\
����·¦Σ��ͯ�չ�÷ʢ�ֵ�����������Ĳ��﷮���������¹�\
¬Ī�������������������ʯ�޼�������½���������������θ���\
�ڽ��͹�����ɽ�Ⱥ�ۭ���������������������ղ����Ҷ��˾��۬\
�輻ӡ�ް׻��Ӷ�����׿���ɳ���������ݷ��̷���������ȽӺɣ\
ţ������ׯ����Ľ��ϰ�������");

	TCHAR cCNMing[] = _T("��������֮��Ľ���������Ԫ��������հ�ɺ���������ɺ����ϧ���㺮\
�������غ���ҹ�غ���ˮ�������������ٴ�ͮ�������������ͮԪ˪������\
����ͮ���������ϴ��꺮������������Ѱ��������ɺĽ������Ѱ��ӳ��ѩ��\
���Ц˪����������ɺ���Ʊ�����������ζ���˪�̷������������̰�����\
�̱����黱ƽ����������ɴ��������״����������ΰ����׷�����������\
�ûܻ���Ц�����侸��ҹ�ٱ���������ѩ�ַ���ޱ����Ѱ����ɽ�Ӻ�������\
����������������¶����ǧ���²������������������ɽǧ�ٺ����ܽ��ɽ\
��������ɽ�ش�������������ϰ���������˪���۵����ͮ�ຮ��������Ѱ\
���ֵ������ɽ֮����¶�����ѩ���溭�պ��������໱����������ϧѩ��\
��֮�������������մ�������������ɻð�Ԫ���������ϴ����ﷲ�ٷ���\
������ɺ�������������������Ӱ��Ļ����ۻ�Ƽ������ɺ��˿�����纣\
¶���ɺ����������׷��������ﴺѩ���������̺�˫ƽ�������֮��ޱ�̴�\
���ٴ��δ��س���������������ܽ��ͮ��������������������ɺ�������\
����ӳ�����̱�Ƽϧ˪�����������ӭ��Ԫ�ش�������ǧ���躮�ϰ�������\
����ɺ�����ɽ��÷������ͮ�ȶ������䰲ǧƼ����������������ʫ�Ʒɷ�\
����������������Ц���亮��������ˮ��ӳ֮������ҹɽ�ƻ���ͮ�ǻ���\
������Х");

	
	CStdString strMing = _T("");
	int nrand = mrand() % (lstrlen(cCNXing) - 1);
	TCHAR cXing = cCNXing[nrand];

	int CharMingNum = GetRand(1,3);
	for (int i = 0; i < CharMingNum; i++)
	{
		nrand = mrand() % (lstrlen(cCNMing) - 1);
		TCHAR cMing = cCNMing[nrand];
		CStdString temp;
		temp.Format(_T("%c"), cMing);
		strMing += temp;
	}

	strOut.Format(_T("%c%s"), cXing, strMing);
	strOut.Replace(_T(" "), _T(""));

	return 0;
}

//add 20080923
///////////////////////////////////////////////////////////////////////////////
//
// ����������������������ַ���
// ���룺����ַ�������, ����ַ�������
// �����strOut
// ����ֵ������0�ɹ�
// ����˵����
//
///////////////////////////////////////////////////////////////////////////////
DWORD StringUtils::RandStr(int nLen, E_Password_Style eType, 
							CStdString& strOut)
{

	TCHAR cNum[] = _T("0123456789");
	TCHAR clletter[] = _T("abcdefghijklmnopqrstuvwxyz");
	TCHAR cNumlLetter[] = _T("0123456789abcdefghijklmnopqrstuvwxyz");
	TCHAR cNumULLetter[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	TCHAR cCNWords[] = _T("��Ǯ��������֣��������������������������ʩ�ſײ��ϻ���κ��\
����л������ˮ��������˸��ɷ�����³Τ������ﻨ������Ԭ��ۺ��\
ʷ�Ʒ����Ѧ�׺����������ޱϺ�����������ʱ��Ƥ���뿵����Ԫ����\
��ƽ�ƺ�������Ҧ��տ����ë����ױ���갼Ʒ��ɴ�̸��é���ܼ�����\
��ף������������ϯ����ǿ��·¦Σ��ͯ�չ�÷ʢ�ֵ�����������Ĳ�\
�﷮���������֧���ù�¬Ī�������Ѹɽ�Ӧ�ڶ����ڵ������������\
��ʯ�޼�ť�������ϻ���½��������춻������ҷ����ഢ���������ɾ�\
�θ����ڽ��͹�����ɽ�ȳ������ȫۭ�����������������ﱩ��������\
���������ղ����Ҷ��˾��۬�輻��ӡ�ް׻���̨�Ӷ����̼���׿����\
�ɳ����������ܲ�˫��ݷ����̷�����̼������Ƚ��۪Ӻȴ�ɣ���ţ\
��ͨ�����༽ۣ����ũ�±�ׯ�̲����ֳ�Ľ����ϰ�°��������������\
�θ����߾Ӻⲽ�����������Ŀܹ�»�ڶ�Ź�����εԽ��¡ʦ������\
�˹��������������Ǽ��Ŀ�����ɳؿ������ᳲ�������󾣺�����Ȩ\
�ָ��滸����ٹ˾���Ϲ�ŷ���ĺ�������˶��������ʸ�ξ�ٹ����̨\
��ұ����������ڵ���̫����������������ԯ����������ĳ���Ľ����\
������˾ͽ˾������˾���붽�ӳ�����ľ����������������ṫ��\
�ذμй��׸����������Ʒ���۳Ϳ�նθɰ��ﶫ�����ź��ӹ麣����΢\
����˧�ÿ�������������������������Ĳ��٦�����Ϲ�ī�������갮��١");
	srand(time(0));
	switch(eType)
	{
	case psNO_INCLUDE_CHINESE:
		for(int i = 0; i < nLen; i++)
		{
			int nrand = mrand() % (lstrlen(cNumULLetter)-1);
			//DWORD nrand = GetTickCount() % (lstrlen(cNum)-1);
			TCHAR c= cNumULLetter[nrand];
			CStdString temp;
			temp.Format(_T("%c"),c);
			strOut+=temp;
		}
		break;
	case psALL_DIGITAL:
		{
			for(int i = 0; i < nLen; i++)
			{
				int nrand = mrand() % (lstrlen(cNum)-1);
				//DWORD nrand = GetTickCount() % (lstrlen(cNum)-1);
				TCHAR c= cNum[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}
		}
		break;
	case psALL_LOWER_CHART:
		{
			for(int i = 0; i < nLen; i++)
			{
				int nrand = mrand() % (lstrlen(clletter)-1);
				TCHAR c= clletter[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}
		}
		break;
	case psALL_UPPER_CHART:
		{
			for(int i = 0; i < nLen; i++)
			{
				int nrand = mrand() % (lstrlen(clletter)-1);
				TCHAR c= clletter[nrand];
				
				CStdString temp;
				temp.Format(_T("%c"),c);
				temp.MakeUpper();
				strOut+=temp;
			}
		}
		break;
		//�������������˺ŵ��߼� 20100114,yzk
	case psALL_CHINESE:
		{
			for (int i=0;i<nLen;i++)
			{
				int nrand = mrand() % (lstrlen(cCNWords)-1);
				TCHAR c= cCNWords[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}
		}
		break;
	case  psLOWER_CHART_AND_DIGITAL:
		{
			for(int i = 0; i < nLen - 2; i++)
			{
				int nrand = mrand() % (lstrlen(clletter)-1);
				TCHAR c= clletter[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}

			for(int w = 0; w <2; w++)
			{
				int nrand = mrand() % (lstrlen(cNum)-1);
				TCHAR c= cNum[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}

		}
		break;
	case psUPPER_CHART_AND_DIGITAL:
		{
			for(int i = 0; i < nLen -2; i++)
			{
				int nrand = mrand() % (lstrlen(clletter)-1);
				TCHAR c= clletter[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				temp.MakeUpper();
				strOut+=temp;
			}

			for(int w = 0; w < 2; w++)
			{
				int nrand = mrand() % (lstrlen(cNum)-1);
				TCHAR c= cNum[nrand];
				CStdString temp;
				temp.Format(_T("%c"),c);
				strOut+=temp;
			}
		}
		break;
	case psUPPER_CHART_AND_CHINESE:
		{
			for(int i = 0; i < nLen; i++)
			{
				int iFlag = mrand() % 2;

				if (iFlag == 0) //��д
				{
					int nrand = mrand() % (lstrlen(cNumlLetter)-1);
					TCHAR c= cNumlLetter[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					temp.MakeUpper();
					strOut+=temp;
				}
				else  //����
				{
					int nrand = mrand() % (lstrlen(cCNWords)-1);
					TCHAR c= cCNWords[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					temp.MakeUpper();
					strOut+=temp;
				}				
			}
		}
		break;
	case psLOWER_CHART_AND_CHINESE:
		{
			for(int i = 0; i < nLen; i++)
			{
				int iFlag = mrand() % 2;

				if (iFlag == 0) //��д
				{
					int nrand = mrand() % (lstrlen(cNumlLetter)-1);
					TCHAR c= cNumlLetter[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}
				else  //����
				{
					int nrand = mrand() % (lstrlen(cCNWords)-1);
					TCHAR c= cCNWords[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}				
			}
		}
		break;

	case psDIGITAL_AND_CHINESE:
		{
			for(int i = 0; i < nLen; i++)
			{
				int iFlag = mrand() % 2;

				if (iFlag == 0) //����
				{
					int nrand = mrand() % (lstrlen(cNum)-1);
					TCHAR c= cNum[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}
				else  //����
				{
					int nrand = mrand() % (lstrlen(cCNWords)-1);
					TCHAR c= cCNWords[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}				
			}
		}
		break;
	default:
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// ���������������ж��ַ����Ƿ�����������
// ���룺pszInput�����жϵ��ַ���
// �����
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL StringUtils::IsNumeric(const TCHAR *pszInput)
{
	CStdString strInput = pszInput;
	if (strInput.IsEmpty())
	{
		return FALSE;
	}
	//	ASSERT(!strInput.IsEmpty());
	
	for (int i = 0; i<strInput.GetLength(); i++)
	{
		int nTemp = strInput.GetAt(i);
		
		if (!_istdigit(nTemp))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// ���������������ж��ַ����Ƿ���Ӣ���ַ�����
// ���룺pszInput�����жϵ��ַ���
// �����
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
// ����˵����
//
//////////////////////////////////////////////////////////////////////////////////////
BOOL StringUtils::IsAlpha(const TCHAR *pszInput)
{
	CStdString strInput = pszInput;
	ASSERT(!strInput.IsEmpty());
	
	for (int i = 0; i<strInput.GetLength(); i++)
	{
		if (!_istalpha(strInput.GetAt(i)))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

//
//���ݵ�һλ�����ͣ�ѡ�񷵻��ַ���
DWORD StringUtils::MatchFirstCharStr(CStdString &strInAndOut, E_Account_Style eFlag, const CStdString &strZN,const CStdString &strEN)
{
	TCHAR chCmp[5] = {0};
	CStdString strRand;

	switch(eFlag)
	{
	case asFIRST_CHAR:
	case asNOT_INCLUDE_CHINESE:		
	case asONLY_CHARACTER:
		strInAndOut = strEN;
		break;
	case asFIRST_DIGITAL:
        RandStr(2, psALL_DIGITAL, strRand);
	    strInAndOut = strRand + strEN;
		break;
	case asFIRST_CHINESE:
		strInAndOut = strZN;
		break;
	default:
		break;
	}

	return 0;
}
