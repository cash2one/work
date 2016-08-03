
#ifndef _ISERVERSOCKET__
#define _ISERVERSOCKET__
#include <windows.h>

class IServerSocket
{

public:
   
	virtual	int SendData(DWORD dwLen,DWORD Flag,char *buff) = 0;
	virtual int UnInit() = 0;
};

#endif