#pragma once
#include <Winuser.h>

class CKeyboardInput { // KEYBDINPUT
private:
	INPUT m_keyborad ;

public:
	CKeyboardInput( int iScanCode, bool bDown = TRUE, int iTime = 0 ):m_keyborad() 
	{
		m_keyborad.type = INPUT_KEYBOARD ;
		//m_keyboard.ki.wScan = iScanCode ;
		m_keyborad.ki.wVk = iScanCode;
		m_keyborad.ki.dwFlags = /*KEYEVENTF_SCANCODE | */(bDown ? 0 : KEYEVENTF_KEYUP) ;
		m_keyborad.ki.time = iTime ;
		m_keyborad.ki.dwExtraInfo = 0 ;
	}

	DWORD scan() const 
	{
		return m_keyborad.ki.wScan ;
	}

	DWORD time() const 
	{
		return m_keyborad.ki.time ;
	}

	int Send( ) const 
	{
		if( m_keyborad.ki.time ) Sleep( m_keyborad.ki.time ) ;
		return SendInput( 1, const_cast<LPINPUT>( &m_keyborad ), sizeof(INPUT) ) ;
	}
} ;