#ifndef _IRECOGNIZE_H_
#define _IRECOGNIZE_H_

// 云识别基类
class IRecognize
{
public:
	/************************************************************************/
	/* 自动识别函数
	/* param strImgPath	本地图片路径
	/* param strResult 识别结果
	/* return long 识别结果码，成功大于0                                      
	/************************************************************************/
	virtual long Recognize(LPCTSTR strImgPath, LPTSTR strResult) = 0;

	/************************************************************************/
	/* 提交错误码函数
	/* param nResultCode  自动识别函数返回的结果码
	/* return long 函数返回状态                                      
	/************************************************************************/
	virtual long ReportError(long nResultCode) = 0;

	/************************************************************************/
	/* 初始化函数
	/* return bool 初始化成功返回true，否则false                                  
	/************************************************************************/
	virtual bool InitRecognize() = 0;

	// 析构函数
	virtual ~IRecognize()
	{}
};

#endif