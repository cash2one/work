// IMsgQueue.cpp: implementation of the IMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IMsgQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//�������DLL����ôҪ��API�Ѻ����������ڴ棬����ʹ��new����������DLL�ڴ���
T_Message* IMsgQueue::New_Message()
{
	T_Message *ret = new T_Message;
	return ret;
}
void IMsgQueue::Free_Message(T_Message *pMsg)
{
	delete pMsg;
	pMsg = NULL;
}
