#include "StdAfx.h"
#include "VolMute.h"


CProcessVolMute::CProcessVolMute(void)
{
	m_pEnumerator = NULL;
	m_pDeviceOut = NULL;
	m_pAudioSessionManager = NULL;
	m_pAudioSessionControl = NULL;
	m_bInited = false;
}


CProcessVolMute::~CProcessVolMute(void)
{
	try
	{
		if (m_pAudioSessionControl != NULL)
		{
			m_pAudioSessionControl->Release();
			m_pAudioSessionControl = NULL;
		}

		if (m_pDeviceOut != NULL)
		{
			m_pDeviceOut->Release();
			m_pDeviceOut = NULL;
		}

		if (m_pAudioSessionManager != NULL)
		{
			m_pAudioSessionManager->Release();
			m_pAudioSessionManager = NULL;
		}

		if (m_pEnumerator != NULL)
		{
			m_pEnumerator->Release();
			m_pEnumerator = NULL;
		}
	}
	catch (...)
	{
	}
}

//设置程序静音（需Vista以上系统）
E_MUTE_RESULT CProcessVolMute::SetMute(bool bMute)
{
	if (IsSystemMoreThanVista())
	{
		HRESULT hr = S_FALSE;

		//初始化且仅初始化一次
		if (!m_bInited && SUCCEEDED(TxInitMuted()))
			m_bInited = true;

		if (m_bInited)
		{
			hr = TxSetMuted(bMute);
		}

		if (SUCCEEDED(hr))
			return eSucceed;
		else
			return eFaild;
	}

	return eNotSupport;
}

// 初始化静音
HRESULT CProcessVolMute::TxInitMuted()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	HRESULT hr = S_FALSE;

	//实例化 MMDeviceEnumerator 枚举器
	hr = CoCreateInstance(_uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, _uuidof(IMMDeviceEnumerator), (LPVOID *)&m_pEnumerator);
	if (FAILED(hr))
	{
		return hr;
	}

	// 默认设备的IMMDevice接口
	hr = m_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_pDeviceOut);
	if (FAILED(hr))
	{
		return hr;
	}

	// 得到音频会话接口IAudioSessionManager
	hr = m_pDeviceOut->Activate(_uuidof(m_pAudioSessionManager), CLSCTX_ALL, NULL, (void **)&m_pAudioSessionManager);
	if (FAILED(hr))
	{
		return hr;
	}

	// 设置会话(Session)参数
	hr = m_pAudioSessionManager->GetAudioSessionControl(NULL, 0, &m_pAudioSessionControl);
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}

// 设置静音
HRESULT CProcessVolMute::TxSetMuted(bool bMuted)
{
	HRESULT hr = S_FALSE;
	ISimpleAudioVolume *pAudioVolume = NULL;

	hr = m_pAudioSessionManager->GetSimpleAudioVolume(NULL, 0, &pAudioVolume);
	if (SUCCEEDED(hr))
	{
		hr = pAudioVolume->SetMute(bMuted, &GUID_NULL);
	}

	if (pAudioVolume != NULL)
	{
		pAudioVolume->Release();
		pAudioVolume = NULL;
	}

	return hr;
}

// 是否为Vista及以上系统
BOOL CProcessVolMute::IsSystemMoreThanVista()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;    // Win 95 needs this

	if (dwPlatformId == 2)
	{
		if ((dwMajorVersion >= 6)/* && (dwMinorVersion == 1)*/)
		{
			return TRUE;
		}
	}

	return FALSE;
}
