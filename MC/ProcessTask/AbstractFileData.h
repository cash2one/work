#pragma once

#include <windows.h>

class CAbstractFileData
{
public:
	CAbstractFileData(void);
	virtual ~CAbstractFileData(void);
protected:
	HANDLE m_hMapFile;
	PBYTE m_pByteBuf;
	DWORD m_dwBUFSize;
	DWORD m_dwPos;
protected:
	DWORD ReadDWORD();
	PCHAR ReadStr();
	BOOL SetPostion(DWORD Pos);	//设置文件位置
	DWORD GetPostion();

public:
	DWORD Open(TCHAR *FileName);	//打开
	DWORD Close();				//关闭	
	DWORD ToMByteToWChar(IN PCHAR startPos,OUT CStdString &str);
	PCHAR WCharToMbyte(CStdString &str);
};
