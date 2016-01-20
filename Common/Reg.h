///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2008, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵����ע����������ļ�
// �������ڣ�2008-8-12
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_)
#define AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReg  
{
public:
	CReg();
	virtual ~CReg();
	
	//��ȡĳ������ֵ
	BYTE *ReadValueOfKey(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;

	//д��ĳ������ֵ
	BOOL WriteValueOfKey(HKEY hKey, 
		const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName,
		const TCHAR *const pszKeyValue,
		DWORD dwKeyValue = 0,
		DWORD dwType = REG_SZ,
		BOOL bDefault = FALSE) const;

	//ȡ��ĳ�����������������
	void GetAllSubKey(HKEY hKey, const TCHAR *const pszSubKey, 
		std::vector<TCHAR*>&vecNameOfKey, std::vector<TCHAR*>&vecValueOfKey) const;
	
	//ȡ��ĳ����������������������
	void GetAllSubNameOfKey(HKEY hKey, const TCHAR *const pszSubKey, 
		std::vector<TCHAR*>&vecNameOfKey) const;
	
	//ɾ��ĳ�������ü��������������
	BOOL DeleteKey(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;

	BOOL DeleteValue(HKEY hKey, const TCHAR *const pszKeyPath, 
		const TCHAR *const pszKeyName) const;
private:
	//�õ�����ϵͳ������
	DWORD GetOSPlatformType() const;
};

#endif // !defined(AFX_REG_H__80BA29DA_7039_4A96_9F85_A4CCD45BDBE6__INCLUDED_)
