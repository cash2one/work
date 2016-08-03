#ifndef _ZBASE64
#define _ZBASE64
#pragma warning(disable:4786) 
#include <string>
#include <vector>
using namespace std;

class CZBase64
{
private:

	//Base64编码解码表
	char* m_Base64_Table;
	string m_strKey ;
	std::vector<char> m_vecBufString ;
	int m_strLength ;
	int m_nSeedLen ;
	string strSeeds ;
	void InitializeSeeds(string *astrSeeds) ;
	string DeEncrypt(string &strInput) ;
	void AddStr(string szText) ;
public:
	//构造
	CZBase64();
	~CZBase64() ;
	//编码
	string EncodeBase64(const string strSource);
	//解码
	string DecodeBase64(const string strSource);

	
	string EncryptText(string &strInput , string &strKey) ;
	string DecryptText(string &strInput , string &strKey) ;

	int Base64Encrypt(char *pchDest, char *pchData, char *pchKey) ;
	int Base64Decrypt(char *pchData, char *pchDest, char *pchKey) ;
};
#endif