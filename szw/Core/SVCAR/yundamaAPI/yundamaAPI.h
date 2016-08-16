#ifdef YUNDAMAAPI_EXPORTS
#define YUNDAMA_API extern "C" __declspec(dllexport)
#else
#define YUNDAMA_API extern "C" __declspec(dllimport)
#endif

YUNDAMA_API void WINAPI YDM_SetAppInfo(long nAppId, LPCSTR lpAppKey);

YUNDAMA_API long WINAPI YDM_Login(LPCSTR lpUserName, LPCSTR lpPassWord);

YUNDAMA_API long WINAPI YDM_DecodeByPath(LPCSTR lpFilePath, long nCodeType, LPSTR pCodeResult);

YUNDAMA_API long WINAPI YDM_DecodeByBytes(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, long nCodeType, LPSTR pCodeResult);

YUNDAMA_API long WINAPI YDM_EasyDecodeByPath(LPCSTR lpUserName, LPCSTR lpPassWord, long nAppId, LPCSTR lpAppKey, LPCSTR lpFilePath, long nCodeType, long nTimeOut, LPSTR pCodeResult);

YUNDAMA_API long WINAPI YDM_EasyDecodeByBytes(LPCSTR lpUserName, LPCSTR lpPassWord, long nAppId, LPCSTR lpAppKey, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, long nCodeType, long nTimeOut, LPSTR pCodeResult);

YUNDAMA_API long WINAPI YDM_EasyGetBalance(LPCSTR lpUserName, LPCSTR lpPassWord, long nAppId, LPCSTR lpAppKey);

YUNDAMA_API long WINAPI YDM_EasyReport(LPCSTR lpUserName, LPCSTR lpPassWord, long nAppId, LPCSTR lpAppKey, long nCaptchaId, bool bCorrect);

YUNDAMA_API long WINAPI YDM_EasyReg(long nAppId, LPCSTR lpAppKey, LPCSTR lpUserName, LPCSTR lpPassWord, LPCSTR lpEmail, LPCSTR lpMobile, LPCSTR lpQQUin);

YUNDAMA_API long WINAPI YDM_EasyPay(LPCSTR lpUserName, LPCSTR lpPassWord, long nAppId, LPCSTR lpAppKey, LPCSTR lpCard);

YUNDAMA_API long WINAPI YDM_UploadByPath(LPCSTR lpFilePath, long nCodeType);

YUNDAMA_API long WINAPI YDM_UploadByBytes(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, long nCodeType);

YUNDAMA_API long WINAPI YDM_GetResult(long nCaptchaId, LPSTR pCodeResult);

YUNDAMA_API long WINAPI YDM_GetBalance(LPCSTR lpUserName, LPCSTR lpPassWord);

YUNDAMA_API long WINAPI YDM_Report(long nCaptchaId, bool bCorrect);

YUNDAMA_API long WINAPI YDM_Pay(LPCSTR lpUserName, LPCSTR lpPassWord, LPCSTR lpCard);

YUNDAMA_API long WINAPI YDM_Reg(LPCSTR lpUserName, LPCSTR lpPassWord, LPCSTR lpEmail, LPCSTR lpMobile, LPCSTR lpQQUin);

YUNDAMA_API void WINAPI YDM_SetTimeOut(long nTimeOut);

YUNDAMA_API void WINAPI YDM_SetBaseAPI(LPCSTR lpBaseAPI);