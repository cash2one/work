///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2013, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：封装右键菜单类
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
#include <map>

class CMyPopupMenu
{
public:
	CMyPopupMenu(void);
	~CMyPopupMenu(void);

	//弹出菜单
	BOOL TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect = 0);
	
	//获取选中的菜单项信息
	bool GetMenuItemSelected(UINT cmdId, T_MENU_SELECTED &menu);

private:
	CMenu m_Menu;
	CPoint m_pt;                                      //弹出菜单时光标所在位置
	CStdString m_strCfgFile;                             
	std::map<CStdString, CStdString> m_mapIdentifyItems;        //填充菜单的菜单项与通配符的对应关系
	std::map<CStdString, CStdString> m_mapFillItems;        //填充菜单的菜单项与通配符的对应关系
	std::map<CStdString, CStdString> m_mapUploadfileItems;  //上传文件菜单的菜单项与通配符的对应关系

	void InitMenu();
	CMenu *CreateSubMenu(/*CMenu *pParent,*/ CStdString strItemName, UINT &itemID, std::map<CStdString, CStdString>&maps);
	bool ChkFillItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menu);
	bool ChkIdentifyItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menu);
	bool ChkUploadFileItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menu);
};

