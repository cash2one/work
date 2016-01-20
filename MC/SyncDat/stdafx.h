// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

#define FILE_NOT_FIND     8  
#define CTRL_SOCK_ERROR   3
#define DATA_SOCK_ERROR   2
#define RET_SUCCESS       1  
#define RET_ERROR         0


#define GET_CLIENTID_TASK 444
#define RECV_TASK 555
#define RECV_TASK_TASKRET 556
#define SEND_TASK 666
#define PUBLISH_TASK 777
#define EXE_FILE_PUBLISH 887
#define EXE_FILE_REQUEST 888
#define EXE_FILE_REQUEST_OK 889
#define EXE_FILE_REQUEST_DENY 890


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


#include "common_enum.h"
#include "common_define.h"
#include "common_stru.h"
#include "common_stru_exserver.h"
#include "trace.h"
#include "IServerSocket.h"
#include "IGlobalDataBlock.h"
#include "IConfigInfo.h"


extern IGlobalDataBlock *g_GlobalDataBlock;

extern IServerSocket *g_ServerSocket;

extern IConfigInfo *g_Config;
extern CLogTrace g_Datalog;

#define DebugLog	1
// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�
