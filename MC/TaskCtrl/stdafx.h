// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <winbase.h>
#include <atlbase.h>
#include <tchar.h>
#include <list>
#include <deque>
#include <map>
#include <vector>
#include <atlstr.h>

#include "stdstring.h"
#include "stringutils.h"
#include "trace.h"
#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "IServerSocket.h"
#include "IGlobalDataBlock.h"
#include "IConfigInfo.h"

extern CLogTrace g_tskLog;
extern IGlobalDataBlock *g_GlobalDataBlock;
extern IServerSocket *g_Socket;
extern IConfigInfo *g_Config;


// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
//#define cjhtest   //����ר��
//#define cjhdebug
