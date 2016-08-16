///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：为上传文件对话框赋值要上传的文件路径
// 生成日期：
// 作者：何培田
//
// 修改历史：
// 1. 日期：
//   作者：
//   修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "lock.h"
class CSelectFile
{
public:
	CSelectFile(void);
	~CSelectFile(void);

	bool FindUploadDialog(HWND hParentWnd);
	bool SetFile(HWND hParentWnd, CStdString strFile, CStdString &strErroMsg, CStdString strWebName);
	void CloseUploadDialog(void);
	HWND GetDialogHwnd(void);

private:
	HWND m_hDlgWnd;              //上传文件对话框窗口句柄
	HWND m_hInputWnd;            //路径输入控件句柄
	HWND m_hBtnOpen;             //‘打开’按钮句柄
	HWND m_hBtnCancel;           //'取消'按钮句柄
	DWORD m_dwcount;			 //记录同一个窗口句柄进入设置透明的次数

	//等待文件对话框显示完毕
	void WaitWindowShow(void);

	//获取选择文件对话框窗口及其相关句柄
	bool GetSelFileHwnds(HWND hOwner);

	//获取文件输入框的句柄
	HWND GetPathInputHWnd(HWND hSelFileHwnd);
	
	//获取打开按钮句柄
	HWND GetOpenBtnHwnd(HWND hSelFileHwnd);

	//获取取消按钮句柄
	HWND GetCancelBtnHwnd(HWND hSelFileHwnd);

	//为输入框赋文件路径
	bool SetFilePath(HWND hInput, CStdString strFile);

	//点击打开按钮确定
	bool Confirm(HWND hSelFileHwnd, CStdString strFile);

	//查看指定的上传文件对话框是否还在
	bool SelFileDlgExist(HWND hSelFileHwnd);
};


