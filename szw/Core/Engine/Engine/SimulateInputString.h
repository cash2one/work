//本类封装API函数SendInput的部分功能，用于模拟键盘输入字符串

//用于MFC等有预编译文件的工程中时
//在SimulateInputString.cpp最顶端添加#include "stdafx.h"
//并在stdafx.h中添加#include "SimulateInputString.h"
#pragma once
//只能用于Windows 2000及以上操作系统
#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0501
#endif
//需要用到winAPI函数
#include <Windows.h>
//字符串内容用string模板存储
#include <string>

class SendString 
{
public:
	//构造函数
	SendString();
	SendString(const WCHAR *wszText);
	SendString(const CHAR *szText);

	//设置字符串内容
	void SetString(const WCHAR *wszText);
	void SetString(const CHAR *szText);

	//追加字符串内容
	void AddString(const WCHAR *wszText);
	void AddString(const CHAR *szText);

	//重载运算符
	const SendString& operator = (const WCHAR *wszText);
	const SendString& operator = (const CHAR *szText);
	const SendString& operator += (const WCHAR *wszText);
	const SendString& operator += (const CHAR *szText);

	//模拟输入字符串，nTimes为重复模拟输入的次数
	INT Send(INT nTimes = 1);

protected:
	//保存字符串内容
	std::wstring m_strText;

	//CHAR型字符串转换为wstring
	std::wstring szTOwstr(const CHAR *szText);

	//对于连续相同的字符，采取分隔处理
	INPUT* Divide(UINT *nIput)const;

	//对于没有连续相同的字符，采取不分隔处理
	INPUT* NoDivede(UINT *nIput)const;
	//nIput用于接收返回值，为返回的INPUT结构数组元素个数

	//检查是否有连续相同的字符
	bool CheckString()const;
	
};