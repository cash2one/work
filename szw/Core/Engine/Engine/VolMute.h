#pragma once

#include <mmdeviceapi.h>
#include <audiopolicy.h>

//设置静音返回结果
enum E_MUTE_RESULT
{
	eSucceed,
	eFaild,
	eNotSupport,
};

class CProcessVolMute
{
public:
	CProcessVolMute(void);
	~CProcessVolMute(void);

public:
	//设置程序静音（需Vista以上系统）
	E_MUTE_RESULT SetMute(bool bMute);

private:
	// 初始化静音
	HRESULT TxInitMuted();

	// 设置静音
	HRESULT TxSetMuted(bool bMuted);

	// 是否为Vista及以上系统
	BOOL IsSystemMoreThanVista();


private:
	bool m_bInited;
	IMMDeviceEnumerator       *m_pEnumerator;
	IMMDevice                 *m_pDeviceOut;
	IAudioSessionManager      *m_pAudioSessionManager;
	IAudioSessionControl      *m_pAudioSessionControl;
};

