
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
		
		if (intRet)							//zhl  防止内存泄漏
		{
			delete[] intRet;
			intRet=NULL;
		}
	}

	return numFound;
}

//把,GUI传送过来的特定格式的VBString转换为一维数组 调用层已经分配好了数组空间,直接填充就好了,
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

//把,GUI传送过来的特定格式的VBString转换为二维数组 调用层已经分配好了数组空间,直接填充就好了,
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






//获取VB传过来的字符串的一维数组的大小和分隔符
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


//获取VB传过来的字符串的二维数组的大小和分隔符
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
		//要求以中文开头且推荐名为空时生成随机汉字
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
			szTime.Format(_T("%d"),year);  //中文名后加年份
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
	TCHAR cCNXing[] = _T("赵钱孙李周吴郑王冯陈褚卫蒋沈韩杨朱秦尤许何吕施张孔曹严华金魏陶\
姜戚谢邹喻柏水窦章云苏潘葛范彭郎鲁韦昌马苗凤花方俞任袁柳鲍\
史唐费岑薛雷贺倪汤滕殷罗毕郝邬安常于时傅齐康余元卜顾孟\
黄穆萧尹姚邵湛汪祁毛禹狄米戴宋茅庞熊纪舒屈项祝董梁杜阮席\
季贾路娄危江童颜郭梅盛林刁钟徐邱骆高夏蔡田樊胡凌霍虞万柯管\
卢莫房丁宣邓郁单洪包诸左石崔吉龚程邢陆荣甄曲储靳邴井段富巫\
乌焦巴弓牧隗山谷侯郗仲伊宫宁仇戎祖武符刘景詹束龙叶幸司韶郜\
黎蓟印宿白怀从鄂咸赖卓屠蒙池乔郁苍闻莘翟谭贡姬申扶堵冉雍桑\
牛燕尚温庄瞿阎慕茹习艾向古易");

	TCHAR cCNMing[] = _T("梦琪忆柳之桃慕青问兰尔岚元香初夏沛菡傲珊曼文乐菱痴珊恨玉惜文香寒\
新柔语蓉海安夜蓉涵柏水桃醉蓝春儿语琴从彤傲晴语兰又菱碧彤元霜怜梦紫\
寒妙彤曼易南莲紫翠雨寒易烟如萱若南寻真晓亦向珊慕灵以蕊寻雁映易雪柳\
孤岚笑霜海云凝天沛珊寒云冰旋宛儿绿真盼儿晓霜碧凡夏菡曼香若烟半梦雅\
绿冰蓝灵槐平安书翠翠风香巧代云梦曼幼翠友巧听寒梦柏醉易访旋亦玉凌萱\
访卉怀亦笑蓝春翠靖柏夜蕾冰夏梦松书雪乐枫念薇靖雁寻春恨山从寒忆香觅\
波静曼凡旋以亦念露芷蕾千兰新波代真新蕾雁玉冷卉紫山千琴恨天傲芙盼山\
怀蝶冰兰山柏翠萱恨松问旋从南白易问筠如霜半芹丹珍冰彤亦寒寒雁怜云寻\
文乐丹翠柔谷山之瑶冰露尔珍谷雪乐萱涵菡海莲傲蕾青槐冬浩宇易梦惜雪宛\
海之柔夏青亦瑶妙菡春竹痴梦紫蓝晓巧幻柏元风冰枫访蕊南春芷蕊凡蕾凡柔\
安蕾天荷含玉书兰雅琴书瑶春雁从安夏槐念芹怀萍代曼幻珊谷丝秋翠白晴海\
露代荷含玉书蕾听白访琴灵雁秋春雪青乐瑶含烟涵双平蝶雅蕊傲之灵薇绿春\
含蕾从梦从蓉初丹。听兰听蓉语芙夏彤凌瑶忆翠幻灵怜菡紫南依珊妙竹访烟\
怜蕾映寒友绿冰萍惜霜凌香芷蕾雁卉迎梦元柏代萱紫真千青凌寒紫安寒安怀\
蕊秋荷涵雁以山凡梅盼曼翠彤谷冬新巧冷安千萍冰烟雅阳友绿南松诗云飞风\
寄灵书芹幼蓉以蓝笑寒忆寒秋烟芷巧水香映之醉波幻莲夜山芷卉向彤烨辉轩\
田豪泽杰啸");

	
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
// 函数功能描述：生成随机字符串
// 输入：随机字符串长度, 随机字符串类型
// 输出：strOut
// 返回值：返回0成功
// 其它说明：
//
///////////////////////////////////////////////////////////////////////////////
DWORD StringUtils::RandStr(int nLen, E_Password_Style eType, 
							CStdString& strOut)
{

	TCHAR cNum[] = _T("0123456789");
	TCHAR clletter[] = _T("abcdefghijklmnopqrstuvwxyz");
	TCHAR cNumlLetter[] = _T("0123456789abcdefghijklmnopqrstuvwxyz");
	TCHAR cNumULLetter[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	TCHAR cCNWords[] = _T("赵钱孙李周吴郑王冯陈褚卫蒋沈韩杨朱秦尤许何吕施张孔曹严华金魏陶\
姜戚谢邹喻柏水窦章云苏潘葛奚范彭郎鲁韦昌马苗凤花方俞任袁柳酆鲍\
史唐费廉岑薛雷贺倪汤滕殷罗毕郝邬安常乐于时傅皮卞齐康伍余元卜顾\
孟平黄和穆萧尹姚邵湛汪祁毛禹狄米贝明臧计伏成戴谈宋茅庞熊纪舒屈\
项祝董梁杜阮蓝闵席季麻强贾路娄危江童颜郭梅盛林刁钟徐邱骆高夏蔡\
田樊胡凌霍虞万支柯昝管卢莫经房裘缪干解应宗丁宣贲邓郁单杭洪包诸\
左石崔吉钮龚程嵇邢滑裴陆荣翁荀羊於惠甄曲家封芮羿储靳汲邴糜松井\
段富巫乌焦巴弓牧隗山谷车侯宓蓬全郗班仰秋仲伊宫宁仇栾暴甘钭厉戎\
祖武符刘景詹束龙叶幸司韶郜黎蓟薄印宿白怀蒲台从鄂索咸籍赖卓蔺屠\
蒙池乔阴郁胥能苍双闻莘党翟谭贡劳逄姬申扶堵冉宰郦雍却璩桑桂濮牛\
寿通边扈燕冀郏浦尚农温别庄晏柴瞿阎充慕连茹习宦艾鱼容向古易慎戈\
廖庚终暨居衡步都耿满弘匡国文寇广禄阙东殴殳沃利蔚越夔隆师巩厍聂\
晁勾敖融冷訾辛阚那简饶空曾毋沙乜养鞠须丰巢关蒯相查后荆红游竺权\
逯盖益桓公万俟司马上官欧阳夏侯诸葛闻人东方赫连皇甫尉迟公羊澹台\
公冶宗政濮阳淳于单于太叔申屠公孙仲孙轩辕令狐钟离宇文长孙慕容鲜\
于闾丘司徒司空亓官司寇仉督子车颛孙端木巫马公西漆雕乐正壤驷公良\
拓拔夹谷宰父谷粱晋楚闫法汝鄢涂钦段干百里东郭南门呼延归海羊舌微\
生岳帅缑亢况后有琴梁丘左丘东门西门商牟佘佴伯赏南宫墨哈谯笪年爱阳佟");
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
		//增加生成中文账号的逻辑 20100114,yzk
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

				if (iFlag == 0) //大写
				{
					int nrand = mrand() % (lstrlen(cNumlLetter)-1);
					TCHAR c= cNumlLetter[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					temp.MakeUpper();
					strOut+=temp;
				}
				else  //汉字
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

				if (iFlag == 0) //大写
				{
					int nrand = mrand() % (lstrlen(cNumlLetter)-1);
					TCHAR c= cNumlLetter[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}
				else  //汉字
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

				if (iFlag == 0) //数字
				{
					int nrand = mrand() % (lstrlen(cNum)-1);
					TCHAR c= cNum[nrand];
					CStdString temp;
					temp.Format(_T("%c"),c);
					strOut+=temp;
				}
				else  //汉字
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
// 函数功能描述：判断字符串是否都是数字类型
// 输入：pszInput－待判断的字符串
// 输出：
// 返回值：成功返回TRUE，失败返回FALSE
// 其它说明：
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
// 函数功能描述：判断字符串是否都是英文字符类型
// 输入：pszInput－待判断的字符串
// 输出：
// 返回值：成功返回TRUE，失败返回FALSE
// 其它说明：
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
//根据第一位的类型，选择返回字符串
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
