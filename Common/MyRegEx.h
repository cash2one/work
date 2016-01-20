///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2013, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵������Boost��������ʽ���з�װ���ṩ����ƥ��Ľӿ�
//          ʹ��ǰ����ȷ����Boost�Ⲣ���úù���Ŀ¼��
//          ������ο���http://blog.csdn.net/foxbryant/article/details/8857042
//          
// �������ڣ�
// ���ߣ�������
//
// �޸���ʷ��
// 1. ���ڣ�
//   ���ߣ�
//   �޸����ݣ�
// 2. 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EnDeCode.h"

class CMyRegEx
{
private:
	CEnDeCode EDTool ;
public:
	CMyRegEx(void);
	~CMyRegEx(void);

	//�����봮��������ƥ�䣬ƥ��ɹ�����true,���򷵻�false
	bool Like(CStdString strInput, CStdString strRegEx);

	//�����봮��������ƥ�䣬������ƥ�䵽��ֵ
	bool Match(CStdString strInput, CStdString strRegEx, std::vector<CStdString> &vRes);

private:
	//bool MatchCharList(CStdString strInput , CStdString) ;

	//��ʱֻ֧�ִ�*�ŵ��ַ�����ƥ��
	//��ȡ[charlist]ƥ���
	void ExtractCharList(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//��ȡ*ƥ���
	void ExtractAsterisk(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//��ȡ��ƥ���
	void ExtractQuestionMark(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
	//��ȡ#ƥ���
	void ExtractSharpMark(CStdString strInPut , std::vector<CStdString> &vecMatchSymbols , std::vector<CStdString> &vecMatchWords) ;
};
