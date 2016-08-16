#pragma once
#include "CommonDef.h"
#include "Lock.h"

class COcrInput
{
public:
	COcrInput(void);
	~COcrInput(void);

	//初始化接口
	bool Init(void);

	//压入验证码信息，识别操作放在此函数中
	void PushOcrData(T_CodeData_IN &data);

	//获取用户确认后的验证码：
	//1.用户确认后，返回true及确认结果；
	//2.不显示验证码时返回false并把T_CodeData_IN的strAutoResult作为确认结果
	//3.设置了显示验证码但用户没有确认前调用此函数时返回false
	bool GetResult(CString strNsCode, T_CodeData_Back &tResult);

	//获取输入焦点的互斥信号
	bool SyncInputFocusWithOcrDlg(void);

	//释放输入焦点的互斥信号，允许验证码框弹出
	void ReleaseInputFocus();

	//与客户端云任务模块用户输入检测功能进行互斥
	void GetSystemInputMutex();

	//释放系统键盘互斥量
	void ReleaseSystemInputMutex();

	//为验证码输入框设置输入焦点
	void FocusOcrInput(void);

private:
	PUSH_OCR_DATA m_pfuncPushOCRData;
	SET_OCR_INPUTFOCUS m_pfuncSetOcrInputFocus;
	GET_CODEDATA_BACK m_pfunGetCodeDataBack ;

	HMODULE m_hDLL;
	HANDLE m_hMutexSyncFocus;               //同步输入焦点的互斥体
	HANDLE m_hMutexSystemInput;				//系统键盘事件互斥体
};

