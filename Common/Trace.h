///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2007, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵������־��ӡ�࣬���Խ���־��ӡ��ָ�����ڻ��ļ���
// �������ڣ�2007-4-4
// ���ߣ����ױ�
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_)
#define AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlstr.h>
#include "Lock.h"
#include "ZBase64.h"

//��־���������¼��֣�
//
//                             ����ʽ��͵�ʽ���ж���ӡ  ֻ�ڵ��԰��ӡ
//ͬʱ�򴰿ڣ�������ھ����Ϊ
//NULL�����ļ���Debug�������  LOGL_TOP                  LOGL_MID
//
//ֻ���ļ���Debug�������      LOGL_LOW                  LOGL_LOW
//
//ֻ��Debug�������                                      LOGL_DBG
enum
{
    LOGL_TOP = 0,
    LOGL_HIG = 1,
    LOGL_MID = 2,
    LOGL_LOW = 3,
    LOGL_DBG = 4,
};

//��־����
enum
{
    LOGT_ERROR = 0, //���ش��󣬳����Ѿ�������޷���������
    LOGT_WARNING = 1, //�澯��һ����󣬳���ɼ�������
    LOGT_PROMPT = 2, //��ʾ�������ڹؼ������һЩ��Ϣ
	LOGT_COMPRESS = 3,//ѹ����������־̫���޷�ȫ���ӡ����ѹ������ȫ����ӡ������
	LOGT_DEBUG = 4, //���ԣ����ڶ�δ�Ӧ�ظ������ݣ���ʵ��û����ô�
	
};

//��־��ɫ
#define LOGRGB(r,g,b) \
    ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
enum
{
    LOGC_ERROR = LOGRGB(180, 0, 0),
    LOGC_WARNING = LOGRGB(160, 0, 120),
    LOGC_PROMPT = LOGRGB(0, 0, 0),
	LOGC_DEBUG = LOGRGB(0,0,180),
};

//��󱸷���־�ļ�����
#ifdef _UDPSERVER
#define MAX_LOGFILEBAK_COUNT          9
#else
#define MAX_LOGFILEBAK_COUNT          2
#endif

#define MAX_LEN_OF_LOG_LINE        1024 * 10 //һ��Log����ַ���
#define MAX_SIZE_OF_LOGFILE        (2 * 1024 * 1024) //һ��Log�ļ�����ַ���
#define MAX_LINE_OF_LOGWND         (2 * 1024) //Log�����������
#define LEN_SHORT_SOURCEFILE_NAME  25 //��ʾ��Դ�ļ�������
#define MAX_CHK_SIZE_PER_TIMES     50 //����ļ���С�ļ������

#define TRACE_WND_MSG              (WM_USER + 101) //֪ͨLog���ڴ�ӡ���Զ�����Ϣ
#define TRACE_PARAM_CLEAR          10 //���Log���ڵ���Ϣ����

#define LOG_ENCRYPT_KEY "suminfo4gerlog"

#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define __TFILE__ __WFILE__
#else
#define __TFILE__ __FILE__
#endif
#ifdef _DEBUG
    //#define cjhdebug
   // #define taskdebug
#endif
//��־��ӡ�࣬���Խ���־��ӡ��ָ���ļ���
class CLogTrace
{
private:
	int m_nOutLogInfo;   //�Ƿ���Ҫ�����
	int m_nEncryptLog ; //�Ƿ���Ҫ���ܣ�
	int m_nOutLogLevel;  //�����־���͵ȼ���
	int m_nSaveAllLog;  //����������־��
public:
	//���������������ڴ�����̬��־�ļ�
	CLogTrace(); 
	bool IsLogFileOpened();    //�Ƿ񴴽�����־�ļ�
	
	void OpenLogFile(const TCHAR * pszTraceFileName, HWND hWnd = NULL);   //������־�ļ�
	///////////////////////////////////

    //�ڹ��캯���д���־�ļ��������봰�ھ�� 
    CLogTrace(const TCHAR * pszTraceFileName, HWND hWnd);
    
    //�����������йر���־�ļ�
    virtual ~CLogTrace();
    
    //��׼��־��ӡ
    void Trace(BYTE cLevel, BYTE cType, const TCHAR * pszSourceFile,
        DWORD dwLine, LPCTSTR lpszFormat, ...);
    
    //����־��ӡ���������ڡ�ʱ�䡢���͡��ļ�������������Ϣ
    void TraceSim(BYTE cLevel, LPCTSTR lpszFormat, ...);

    //������־�ִ���������־���ڴ�ӡʱȡ��
    const TCHAR * GetLogBuf();

    //������־���ھ��
    void SetLogWnd(HWND hLogWnd);

	bool IsLogFileEnable(); //�Ƿ�Ҫ�����־�ļ�

	//�йؼ�����־��Ϣ ���Ƿ� 
	bool IsEncryptLog() ;  //�Ƿ���������־
	bool SetEncryptFlag() ;  //�ļ��ײ�д����ܱ�־
	bool GetEncryptFlag(int &nFlag) ; //���ļ��ײ���ȡ���ܱ�־
	bool CreatLogFile() ;//����log�ļ� ����������
private:

    //����ɫ�ļ���־��ӡ
    void TraceColor(BYTE cLevel, DWORD dwColor);
	void ZipEecodeStdString( const CString &strSource, CString &strZipped);   //ѹ��
	void GetLogType(CString &strType, DWORD &dwColor,BYTE cType);  //��ȡ��־�����ı�����ɫ��
private:

    CLock m_lock; //��
	CZBase64 m_base64Encrypt ;
    
    TCHAR  m_szFullLogFileName[MAX_PATH]; //��־�ļ�ȫ��
    HANDLE m_hLogFile; //��־�ļ����
    HWND   m_hLogWnd; //��־���ھ��
    bool   m_bFirstCreate; //�Ƿ��״δ�����־�ļ�

    TCHAR   m_szBuffer[2][MAX_LEN_OF_LOG_LINE]; //������־�Ļ���
};

#define TOPLOG(x) g_log.Trace(LOGL_TOP,LOGT_PROMPT,__TFILE__, __LINE__, x)
	
#endif // !defined(AFX_TRACE_H__C1BED5DD_9AEB_4CE1_A5EF_CB2B9894E976__INCLUDED_)
