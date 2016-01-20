#include "StdAfx.h"
#include ".\AbstractFileData.h"

CAbstractFileData::CAbstractFileData(void)
{
	m_pByteBuf = NULL;
	m_hMapFile = INVALID_HANDLE_VALUE;
	SetPostion(0);
}

CAbstractFileData::~CAbstractFileData(void)
{
}
/*
返回0是失败,多数情况是没有这个文件
否则返回网站数据最后的更新时间
*/

DWORD CAbstractFileData::Open(TCHAR *FileName)
{
	HANDLE hFile = CreateFile(FileName, 
		GENERIC_READ, 
		FILE_SHARE_READ ,NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL); 

	if(hFile==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	m_dwBUFSize = GetFileSize(hFile, NULL);

	m_hMapFile = CreateFileMapping(
		hFile,                   // use paging file
		NULL,                    // default security 
		PAGE_READONLY,           // read/write access
		0,                       // max. object size 
		m_dwBUFSize,             // buffer size  
		NULL);                   // name of mapping object

	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE) 
	{ 
		_tprintf(_T("Could not create file mapping object with %s, error(%d).\n"), FileName, GetLastError());
		CloseHandle(hFile);
		return 0;
	}
	m_pByteBuf = (PBYTE) MapViewOfFile(
		m_hMapFile,                 // handle to mapping object
		FILE_MAP_READ,				// read/write permission
		0,                   
		0,                   
		m_dwBUFSize);     

	CloseHandle(hFile);

	if (m_pByteBuf == NULL) 
	{ 
		return 0;
	}
	SetPostion(0);

	return 1;
}

DWORD CAbstractFileData::Close()
{
	UnmapViewOfFile(m_pByteBuf);

	CloseHandle(m_hMapFile);
	SetPostion(0);
	m_pByteBuf = NULL;
	m_hMapFile = INVALID_HANDLE_VALUE;

	return 0;
}


BOOL CAbstractFileData::SetPostion(DWORD Pos)
{
	if(Pos>m_dwBUFSize)return FALSE;
	m_dwPos = Pos;

	return TRUE;
}

DWORD CAbstractFileData::GetPostion()
{
	return m_dwPos;
}


DWORD CAbstractFileData::ReadDWORD()
{
	DWORD dwResult = *((DWORD*)(m_pByteBuf+m_dwPos));
	m_dwPos+=sizeof(DWORD);
	return dwResult;
}

PCHAR CAbstractFileData::ReadStr()
{
	DWORD dwStartPos = m_dwPos;
	while((*(m_pByteBuf+(m_dwPos++)))!=0)
	{
		if(m_dwPos>m_dwBUFSize) return NULL;
	}
	PCHAR pResult = (PCHAR)(m_pByteBuf+dwStartPos);
	return pResult;
}

DWORD CAbstractFileData::ToMByteToWChar(IN PCHAR startPos,OUT CStdString &str)
{
	LPWSTR temp;
	DWORD unicodeLeng(0);
	if (startPos == NULL)
	{
		return NULL;
	}
	unicodeLeng = MultiByteToWideChar(CP_ACP,0,startPos,strlen(startPos) + 1,NULL,0);
	temp = new TCHAR[unicodeLeng];
	memset(temp , 0 ,sizeof(TCHAR) * unicodeLeng);
	MultiByteToWideChar(CP_ACP,0 ,startPos ,strlen(startPos) + 1 ,temp ,unicodeLeng);

	str  = temp ;
	delete[] temp;
	temp = NULL;
	return 0;
}
//转多字节
PCHAR CAbstractFileData::WCharToMbyte( CStdString &str )
{
	DWORD dwLen;
	char* tmp;

	dwLen= WideCharToMultiByte(CP_ACP,0,str.c_str(),-1,NULL,0,NULL,FALSE);
	tmp = new char[dwLen];
	memset(tmp,0,dwLen);
	WideCharToMultiByte(CP_ACP,0,str.c_str(),-1,tmp,dwLen,NULL,FALSE);

	return tmp;
}
