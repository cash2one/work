#include "StdAfx.h"
#include "MyPopupMenu.h"
#include <Windows.h>

CMyPopupMenu::CMyPopupMenu(void)
{
	InitMenu();       //加载菜单项
}


CMyPopupMenu::~CMyPopupMenu(void)
{
	if (m_Menu.m_hMenu)
	{
		int iItemCount = m_Menu.GetMenuItemCount();
		for (int i = iItemCount; i >= 0; i--)
			m_Menu.DeleteMenu(i, MF_BYPOSITION);
	}
}

void CMyPopupMenu::InitMenu()
{
	//m_strCfgFile.Format(_T("%s\\cfg\\menuconfig.ini"), GetInstallPath());
	m_strCfgFile.Format(_T("%s\\data2\\menuconfig.dat"), GetInstallPath());

	if (!PathFileExists(m_strCfgFile))
	{
		g_runlog.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__,_T("菜单文件不存在，不加载菜单！"));
		return;
	}
	m_Menu.CreatePopupMenu();

	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amClick, _T("点击"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amFill, _T("赋值"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amIdentify, _T("识别"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amJumpURL, _T("网页跳转"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amKeyPress, _T("模拟键盘"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amUploadFile, _T("上传文件"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amUserForm, _T("自定义表单"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amUserInput, _T("人工输入"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amShow, _T("显示"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amHide, _T("隐藏"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amGetAttribute, _T("提取数据"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amGetIIDK, _T("提取动参"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amSuccFeature, _T("成功标记"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amFireEvent, _T("触发事件"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amNextPage, _T("下一页"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amVerifyCodeSuccFeature, _T("验证码标记"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amMarkObject, _T("标记对象"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amSetDataArea, _T("嵌套子块"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amSaveItem, _T("保存节点"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amDownLoadFile, _T("提取文件"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amDeleteNode, _T("删除节点"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amSaveDelNode, _T("屏蔽节点"));
	m_Menu.AppendMenu(MF_STRING, WM_POPUP_MENU_BASE + amSaveSuccFeature, _T("预设成功标记"));
	
	UINT IdentifyCmdID = WM_POPUP_MENU_BASE + amMax;
	CMenu *pSub1 = CreateSubMenu(_T("识别"), IdentifyCmdID, m_mapIdentifyItems);
	if (pSub1)
		m_Menu.ModifyMenu(WM_POPUP_MENU_BASE + amIdentify, MF_BYCOMMAND | MF_POPUP, (UINT_PTR)pSub1->m_hMenu, _T("识别"));

	UINT fillCmdID = WM_POPUP_MENU_BASE + 500;
	CMenu *pSub = CreateSubMenu(_T("赋值"), fillCmdID, m_mapFillItems);    //为一级菜单预留500个ID
	if (pSub)
		m_Menu.ModifyMenu(WM_POPUP_MENU_BASE + amFill, MF_BYCOMMAND | MF_POPUP, (UINT_PTR)pSub->m_hMenu, _T("赋值"));

 	UINT uploadfileCmdID = WM_POPUP_MENU_BASE + 50;
	CMenu *pSubUploadfile = CreateSubMenu(_T("上传文件"), uploadfileCmdID, m_mapUploadfileItems);    //为一级菜单预留500个ID
	if (pSubUploadfile)
		m_Menu.ModifyMenu(WM_POPUP_MENU_BASE + amUploadFile, MF_BYCOMMAND | MF_POPUP, (UINT_PTR)pSubUploadfile->m_hMenu, _T("上传文件"));

	//禁用暂时不用的菜单项
	m_Menu.EnableMenuItem(WM_POPUP_MENU_BASE + amUserInput, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//m_Menu.EnableMenuItem(WM_POPUP_MENU_BASE + amShow, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	//m_Menu.EnableMenuItem(WM_POPUP_MENU_BASE + amHide, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
}

CMenu *CMyPopupMenu::CreateSubMenu(/*CMenu *pParent, */CStdString strItemName, UINT &itemID, std::map<CStdString, CStdString>&maps)
{
	DWORD dwLen = 0;
	TCHAR tszBuf[1024] = {0};
	TCHAR tszKey[128] = {0};
	int iCurPos = 0;
	
	dwLen = GetPrivateProfileString(strItemName, NULL, NULL, tszBuf, 1024, m_strCfgFile);
	if (dwLen > 0)
	{
		CMenu *pMenu = new CMenu;
		pMenu->CreatePopupMenu();

		//读取指定项名称下的所有Key、Value对
		do 
		{
			_tcscpy(tszKey, tszBuf + iCurPos);			
			iCurPos += _tcslen(tszKey) + 1;
			if (!_tcscmp(tszKey, _T("")))
				break;

			TCHAR tszValue[128] = {0};
			GetPrivateProfileString(strItemName, tszKey, NULL, tszValue, 128, m_strCfgFile);
			if (!_tcscmp(tszValue, _T("SecondLevel")))
			{
				//有子菜单的则递归创建子菜单
				CMenu *pSub = CreateSubMenu(tszKey, itemID, maps);
				if (pSub)
					pMenu->AppendMenu(MF_POPUP, (UINT_PTR)pSub->m_hMenu, tszKey);
			}
			else
			{
				pMenu->AppendMenu(MF_STRING, itemID, tszKey);

				//将菜单与通配符对应关系存入map
				maps[tszKey] = tszValue;          
			}

			itemID++;
		} while (1);

		return pMenu;
	}

	return NULL;
}

BOOL CMyPopupMenu::TrackPopupMenu(UINT nFlags, int x, int y,CWnd* pWnd, LPCRECT lpRect)
{
	if (m_Menu.m_hMenu)
	{
		m_Menu.TrackPopupMenu(nFlags, x, y, pWnd, lpRect);

		//保存当前光标所在位置
		m_pt.x = x;
		m_pt.y = y;

		return TRUE;
	}

	return FALSE;
}

//检查是否选中了赋值菜单功能，若是则返回相应菜单项信息
bool CMyPopupMenu::ChkFillItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menuItem)
{
	int iItemCount = pMenu->GetMenuItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		//是子菜单则递归查询
		CMenu *pSubMenu = pMenu->GetSubMenu(i);
		if (pSubMenu && ChkFillItemSelected(pSubMenu, cmdId, menuItem))
			return true;

		if (pMenu->GetMenuItemID(i) == cmdId)
		{
			CString strTemp = _T("");
			pMenu->GetMenuString(cmdId, strTemp, MF_BYCOMMAND);
			menuItem.strMenuString = strTemp;
			menuItem.strSymbol = m_mapFillItems[menuItem.strMenuString];

			return true;
		}			
	}

	return false;
}

//检查是否选中了识别菜单功能，若是则返回相应菜单项信息
bool CMyPopupMenu::ChkIdentifyItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menuItem)
{
	int iItemCount = pMenu->GetMenuItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		//是子菜单则递归查询
		CMenu *pSubMenu = pMenu->GetSubMenu(i);
		if (pSubMenu && ChkIdentifyItemSelected(pSubMenu, cmdId, menuItem))
			return true;

		if (pMenu->GetMenuItemID(i) == cmdId)
		{
			CString strTemp = _T("");
			pMenu->GetMenuString(cmdId, strTemp, MF_BYCOMMAND);
			menuItem.strMenuString = strTemp;
			menuItem.strSymbol = m_mapIdentifyItems[menuItem.strMenuString];

			return true;
		}			
	}

	return false;
}

//检查是否选中了上传文件菜单功能，若是则返回相应菜单项信息
bool CMyPopupMenu::ChkUploadFileItemSelected(CMenu *pMenu, UINT cmdId, T_MENU_SELECTED &menuItem)
{
	int iItemCount = pMenu->GetMenuItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		//是子菜单则递归查询
		CMenu *pSubMenu = pMenu->GetSubMenu(i);
		if (pSubMenu && ChkUploadFileItemSelected(pSubMenu, cmdId, menuItem))
			return true;

		if (pMenu->GetMenuItemID(i) == cmdId)
		{
			CString strTemp = _T("");
			pMenu->GetMenuString(cmdId, strTemp, MF_BYCOMMAND);
			menuItem.strMenuString = strTemp;
			menuItem.strSymbol = m_mapUploadfileItems[menuItem.strMenuString];

			return true;
		}			
	}

	return false;
}

//获取当前选中的菜单项信息
bool CMyPopupMenu::GetMenuItemSelected(UINT cmdId, T_MENU_SELECTED &menu)
{
    if (cmdId < WM_POPUP_MENU_BASE)
		return false;

	bool bRet = false;

	if (cmdId - WM_POPUP_MENU_BASE > 0 && cmdId - WM_POPUP_MENU_BASE < amMax)
	{
		menu.amMethod = (E_ActionMethod)(cmdId - WM_POPUP_MENU_BASE);
		bRet = true;
	}
	
	if (!bRet)
	{
		bRet = ChkFillItemSelected(m_Menu.GetSubMenu(1), cmdId, menu); 
		if (bRet)
			menu.amMethod = amFill;		
	}

	if (!bRet)
	{
		//检查是否是识别子菜单
		bRet = ChkIdentifyItemSelected(m_Menu.GetSubMenu(2), cmdId, menu); 
		if (bRet)
			menu.amMethod = amIdentify;
	}

	if (!bRet)
	{
		//检查是否是上传文件子菜单
		bRet = ChkUploadFileItemSelected(m_Menu.GetSubMenu(5), cmdId, menu); 
		if (bRet)
			menu.amMethod = amUploadFile;
	}
// 	//先判断是否是一级菜单操作
//  	switch (cmdId - WM_POPUP_MENU_BASE)
// 	{
// 	case amClick:
// 	//case amFill:
// 	case amIdentify:
// 	case amJumpURL:
// 	case amKeyPress:
// 	case amUploadFile:
// 	case amUserForm:
// 	case amUserInput:
// 	case amShow:
// 	case amHide:
// 	case amGetAttribute:
// 	case amGetIIDK:
// 	case amSuccFeature:
//  	case amFireEvent:
// 		menu.amMethod = (E_ActionMethod)(cmdId - WM_POPUP_MENU_BASE);
// 		bRet = true;
// 		break;
// 	default:
// 		bRet = ChkFillItemSelected(m_Menu.GetSubMenu(1), cmdId, menu); 
// 		if (bRet)
// 			menu.amMethod = amFill;
// 		else
// 		{
// 			//检查是否是识别子菜单
// 			bRet = ChkIdentifyItemSelected(m_Menu.GetSubMenu(2), cmdId, menu); 
// 			if (bRet)
// 				menu.amMethod = amIdentify;
// 
// 		}
// 		break;
// 	}

	//取一级菜单的显示名称
	CString strMenuString = _T("");
	m_Menu.GetMenuString(menu.amMethod - 1, strMenuString, MF_BYPOSITION);
	menu.strMenuString = strMenuString;

	menu.pt = m_pt;

	return bRet;
}