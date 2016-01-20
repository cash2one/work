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
	BOOL SetPostion(DWORD Pos);	//�����ļ�λ��
	DWORD GetPostion();

public:
	DWORD Open(TCHAR *FileName);	//��
	DWORD Close();				//�ر�	
	DWORD ToMByteToWChar(IN PCHAR startPos,OUT CStdString &str);
	PCHAR WCharToMbyte(CStdString &str);
};
