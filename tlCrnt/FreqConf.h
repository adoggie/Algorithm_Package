// FreqConf.h: interface for the CFreqConf class.
//
//���±�д:�Ķ�ɽ          �ı�дʱ�䣺06.03.28
//�ļ�����;��Ƶ��ͳ�ƽṹ
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREQCONF_H__7BA6BBFF_4922_4361_840F_311B73CBC0A6__INCLUDED_)
#define AFX_FREQCONF_H__7BA6BBFF_4922_4361_840F_311B73CBC0A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define RANGE_NUM     1 //�������
#define RANGE_LENGTH  2 //���䳤��

class CFreqConf
{
public:
	int    m_nFldIndex;//��������
	int    m_nRangeOpt;//ѡ������쳤��
	double m_fRangeNum;//������������䳤��
	double m_fRangeTo; //��������
	double m_fRangeFrom;  //��ʼ����
	double m_fWeight;     //Ȩ��
	double m_fTotalWeight;//ǰ��Ȩ��
	CFreqConf()
	{
		m_nFldIndex = 0;
		m_nRangeOpt = RANGE_NUM;
		m_fRangeNum = m_fRangeTo = m_fRangeFrom = 0.0;
		m_fWeight = m_fTotalWeight = 0;
	}
	virtual ~CFreqConf()
	{
	};
	
};

#endif // !defined(AFX_FREQCONF_H__7BA6BBFF_4922_4361_840F_311B73CBC0A6__INCLUDED_)
