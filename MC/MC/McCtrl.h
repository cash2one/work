#ifndef __MAIN_CTRL_VC_H__
#define __MAIN_CTRL_VC_H__


#include "ThreadManage.h"
#include "ConfigInfo.h"

#define MAX_HANDLES_IN_MCCTRL   5

class CMCCtrl 
{
public:
    CMCCtrl();

    ~CMCCtrl();

	CThreadManage *GetPtrOfThreadManage()
	{
		return &m_threadManage;
	}
	DWORD Start(BOOL bYun); //��ʼ
	DWORD Stop(); //ֹͣ
private:    
	CThreadManage m_threadManage;	//�̹߳������
};

#endif