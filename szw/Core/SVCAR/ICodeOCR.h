///////////////////////////////////////////////////////////////////////////////
//
// 版权所有(C), 2008, 商讯网信息有限公司
//
// 版本：1.0
// 文件说明：CodeOCR公共接口
// 生成日期：2008-1-29
// 作者：殷宗华
//
// 修改历史：
// 1. 日期：
//    作者：
//    修改内容：
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICODE_OCR_H_
#define _ICODE_OCR_H_


//CodeOcr模块提供给ExServer的接口
struct ICodeOCR
{
	virtual ~ICodeOCR(){};

//	virtual DWORD ReleaseDLL() = 0;
	
	// 函数功能描述：创建CodeOCR接口对象指针
	// 输入：接口ID、版本号
	// 输出：
	// 返回值：成功返回CodeOCR接口对象指针，创建失败返回NULL
	virtual DWORD SetModFilePath(CString strModPath) = 0;
    
	//释放成功返回1，否则返回0
    virtual DWORD Release(void) = 0;
	
	//打开本地临时图片文件
	//打开成功返回1，否则返回0	
	virtual DWORD OpenImageFile(CString strImagePath) = 0;
	
	//识别当前图片
	//输入参数：自动、参数或特殊识别所用到的参数列表
	//输出参数：识别结果，如果识别失败返回空字串
	//识别成功返回1，否则返回0	
	virtual DWORD Recognising(CString strRecParam, CString &strResult, CString &strPreProcess) = 0;
	
	//取回图片处理和识别的中间数据，仅供参数制作工具调用
	//输出参数：预处理结果图片及识别准确率参数
	//正确取回返回1，否则返回0	
	virtual DWORD GetProcessDataForTool(DWORD** &pszProcessData,DWORD &dwHeight,DWORD &dwWidth) = 0;
    
	virtual CString SimpleEncode(CString &strPlainText) = 0;

};

// #define CodeOCR_DLL_FILENAME _T("CodeOCR.Dll")
// 
// #define InitCodeOCRDllHelper(object) object(\
//     CodeOCR_DLL_FILENAME,                    \
//     _T("CreateCodeOCR"),                           \
//     IID_ICodeOCR,                           \
//     Ver_ICodeOCR                            \
// )

#endif