///////////////////////////////////////////////////////////////////////////////
//
// ��Ȩ����(C), 2013, ��Ѷ����Ϣ���޹�˾
//
// �汾��1.0
// �ļ�˵����������������ƥ�����ؽӿ�
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
#include <vector>

struct SPLIT
{
	CStdString strTarget;
	CStdString strReplace;
};

namespace FeatureFit
{
	

	//�жϽ��
	bool FitDefine(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault);
	bool FitDefine3(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, bool &bCanDelete, CStdString &);
	//�жϽ�����������ȶ��ʧ��ʱ��Ϊ�ȶ�ʧ���
	bool FitDefine2(CStdString strCompare, const std::vector<CStdString> &vstrRefer, bool bSingleFit, bool bDefault, CStdString &strErrItem);

	//�������ַ����ֽ��"����"���߼��ṹ
	void GetFSData(CStdString strInput, CStdString strMainDelimiter, CStdString strSubDelimitor, T_FILTER_STRUCTURE &tResult);

	//����"����"���߼����ж������ַ����Ƿ����"����"��������Ķ��壬�����ִ�Сд
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, std::vector<CStdString> &sFindCanDelete);
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strFindCanDelete);
	bool FitFilter(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult);
	
	//����"����"���߼����ж������ַ����Ƿ����"����"��������Ķ��岢����ʧ��ԭ��
	int FitFilter2(CStdString strCompare, const T_FILTER_STRUCTURE &tRefer, bool bDefult, CStdString &strErrInfo);

	bool SplitSubStr(std::vector<SPLIT> &vecTmpSplit, CStdString str);
}