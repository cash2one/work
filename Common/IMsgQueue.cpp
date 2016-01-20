// IMsgQueue.cpp: implementation of the IMsgQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IMsgQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//如果是在DLL中哪么要用API堆函数来创建内存，不能使用new创建到本地DLL内存中
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
