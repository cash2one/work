// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// DataUpdate.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

IGlobalDataBlock *g_GlobalDataBlock = NULL;

IServerSocket *g_ServerSocket = NULL;

IConfigInfo *g_Config = NULL;
CLogTrace g_Datalog(_T("SyncDatlog.log"),NULL);

#define LOGFILE ".\\DataUpdateLog.txt"
/*int WriteToLog(TCHAR* str)
{
	g_Datalog.Trace(LOGL_TOP,LOGT_PROMPT, __TFILE__,__LINE__, _T("%s"),str);
	return 0;
// 	FILE* log;
// 	log = fopen(LOGFILE, "a+");
// 	if (log == NULL){
//  		printf("Log file open failed.");
// 		return -1;
// 	}
// 	char strLog[500]={0};
// 	int nLen = WideCharToMultiByte(CP_ACP, 0,str , -1,strLog , 500,NULL,NULL);
// 	//fprintf(log, "%s\n",strLog);
// 	printf("%s\n",strLog);
// 	fclose(log);
// 	return 0;
}*/
// TODO: �� STDAFX.H ��
//�����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
