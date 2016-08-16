#include "SimulateInputString.h"

//#include "stdafx.h"

//CHAR转换为WCHAR需要用到此文件
#include <stdlib.h>
//CHAR转换为WCHAR时，要本地化，否则转换中文字符乱码
#include <locale.h>
//list容器
#include <list>


SendString::SendString()
{
	;
}

SendString::SendString(const WCHAR *wszText)
{
	m_strText = wszText;
}

SendString::SendString(const CHAR *szText)
{
	m_strText = szTOwstr(szText);
}

void SendString::SetString(const WCHAR *wszText)
{
	m_strText = wszText;
}

void SendString::SetString(const CHAR *szText)
{
	m_strText = szTOwstr(szText);
}

void SendString::AddString(const WCHAR *wszText)
{
	m_strText.append(wszText);
}

void SendString::AddString(const CHAR *szText)
{
	m_strText.append(szTOwstr(szText));
}

const SendString& SendString::operator = (const WCHAR *wszText)
{
	SetString(wszText);
	return *this;
}

const SendString& SendString::operator = (const CHAR *szText)
{
	SetString(szText);
	return *this;
}

const SendString& SendString::operator += (const WCHAR *wszText)
{
	AddString(wszText);
	return *this;
}

const SendString& SendString::operator += (const CHAR *szText)
{
	AddString(szText);
	return *this;
}


std::wstring SendString::szTOwstr(const CHAR *szText)
{
	//CHAR型字符串转换为wstring
	std::string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs"); 
	size_t _Dsize = strlen(szText) + 1;
	WCHAR *wszText = new WCHAR[_Dsize];
	wmemset(wszText,0,_Dsize);
	size_t nValue;
	mbstowcs_s(&nValue,wszText,_Dsize,szText,_TRUNCATE);
	std::wstring wstrText = wszText;
	delete []wszText;
	setlocale(LC_ALL, curLocale.c_str());
	return wstrText;
} 

INT SendString::Send(INT nTimes /* = 1 */)
{	
	//错误代码-1，字符串内容为空
	if (m_strText.empty())
		return -1;
	//错误代码-2，输入次数必须大于等于1
	if (nTimes < 1)
		return -2;
	//aIput为INPUT结构数组，nIput为INPUT结构数组元素数量
	INPUT *aIput = NULL;
	UINT nIput = 0;

	if (CheckString())
	{
		//有连续相同的字符，采取分隔处理
		aIput = Divide(&nIput);
	}
	else
	{
		//没有连续相同的字符，采取不分隔处理
		aIput = NoDivede(&nIput);
	}
	
	//开始发送模拟按键
	UINT nBeSend;
	for (INT i=0;i<nTimes;i++)
	{
		nBeSend = SendInput(nIput,aIput,sizeof(INPUT));
	}

	//清空INPUT结构数组并释放内存
	delete [] aIput; 
	
	if (nBeSend>0)
		//返回值大于0表示每次成功插入多少个事件
		return nBeSend;
	else
		//错误代码-3，模拟失败，成功模拟输入0个字符
		return -3;
}

INPUT* SendString::Divide(UINT *nIput)
const
{
	//nWord为字符个数
	UINT nWord = (UINT)m_strText.length();

	//创建KEYBDINPUT结构数组，将每个字符填充进去
	KEYBDINPUT *aKey = new KEYBDINPUT[nWord];
	//清空结构体
	memset(aKey,0,nWord*sizeof(KEYBDINPUT));
	for (UINT i=0;i<nWord;i++)
	{
		aKey[i].wScan = (WORD)m_strText[i];
		aKey[i].dwFlags = KEYEVENTF_UNICODE;
	}

	//将KEYBDINPUT结构数组放入list容器中
	std::list<KEYBDINPUT> Keylist;
	for (UINT i=0;i<nWord;i++)
	{
		Keylist.push_back(aKey[i]);
	}

	//删除KEYBDINPUT结构数组并释放所占的空间
	delete []aKey;

	//准备好要填充的分隔按键KEYBDINPUT结构
	KEYBDINPUT KEYdivision[4] = {NULL};
	KEYdivision[0].wVk = VK_LEFT;
	KEYdivision[1].wVk = VK_LEFT;
	KEYdivision[1].dwFlags = KEYEVENTF_KEYUP;
	KEYdivision[2].wVk = VK_RIGHT;
	KEYdivision[3].wVk = VK_RIGHT;
	KEYdivision[3].dwFlags = KEYEVENTF_KEYUP;

	//开始处理，如果相邻两个KEYBDINPUT结构代表的字符相同则插入分隔按键
	//获取第一个KEYBDINPUT结构的字符，迭代器指向第二个结构
	std::list<KEYBDINPUT>::iterator KeyIterator;
	WORD word = Keylist.front().wScan;
	KeyIterator = Keylist.begin();
	KeyIterator++;
	//循环对比查找代表相同字符的结构
	for(UINT i=0;i<nWord-1;i++)
	{
		if (word == KeyIterator->wScan)
		{
			//出现相邻的代表相同字符的结构，插入分隔按键
			for (int i=0;i<4;i++)
			{
				Keylist.insert(KeyIterator,KEYdivision[i]);
			}
		}
		word = KeyIterator->wScan;
		KeyIterator++;
	}

	//创建INPUT结构数组
	UINT nPut = (UINT)Keylist.size();
	INPUT *aPut = new INPUT[nPut];
	//清空结构体
	memset(aPut,0,nPut*sizeof(INPUT));
	//KEYBDINPUT结构体的list迭代器指向第一个元素准备循环填充INPUT结构
	KeyIterator = Keylist.begin();
	for (UINT i=0;i<nPut;i++)
	{
		aPut[i].type = INPUT_KEYBOARD;
		aPut[i].ki = *KeyIterator;
		KeyIterator++;
	}

	//返回INPUT结构数量
	*nIput = nPut;
	//返回INPUT结构指针，使用完后注意delete
	return aPut;
}

INPUT* SendString::NoDivede(UINT *nIput)
const
{
	//nWord为字符个数
	UINT nWord = (UINT)m_strText.length();

	//创建KEYBDINPUT结构数组，将每个字符填充进去
	KEYBDINPUT *aKey = new KEYBDINPUT[nWord];
	//清空结构体
	memset(aKey,0,nWord*sizeof(KEYBDINPUT));
	for (UINT i=0;i<nWord;i++)
	{
		aKey[i].wScan = (WORD)m_strText[i];
		aKey[i].dwFlags = KEYEVENTF_UNICODE;
	}

	//创建INPUT结构数组
	 INPUT *aPut = new INPUT[nWord];
	memset(aPut,0,nWord*sizeof(INPUT));
	for (UINT i=0;i<nWord;i++)
	{
		aPut[i].type = INPUT_KEYBOARD;
		aPut[i].ki = aKey[i];
	}

	//删除KEYBDINPUT结构数组并释放所占的空间
	delete []aKey;
	//返回INPUT结构数量
	*nIput = nWord;
	//返回INPUT结构指针，使用完后注意delete
	return aPut;
}

bool SendString::CheckString()
const
{
	//检查是否有相邻的相同的字符
	WCHAR wch = m_strText[0];
	UINT nWord = (UINT)m_strText.length();
	for (UINT i=1;i<nWord;i++)
	{
		if (wch == m_strText[i])
		{
			return true;
		}
		wch = m_strText[i];
	}
	return false;
}