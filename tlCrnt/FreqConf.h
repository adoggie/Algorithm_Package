// FreqConf.h: interface for the CFreqConf class.
//
//重新编写:夏东山          改编写时间：06.03.28
//文件的用途：频率统计结构
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FREQCONF_H__7BA6BBFF_4922_4361_840F_311B73CBC0A6__INCLUDED_)
#define AFX_FREQCONF_H__7BA6BBFF_4922_4361_840F_311B73CBC0A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define RANGE_NUM     1 //区间个数
#define RANGE_LENGTH  2 //区间长度

class CFreqConf
{
public:
	int    m_nFldIndex;//变量索引
	int    m_nRangeOpt;//选项－个数§长度
	double m_fRangeNum;//区间个数或区间长度
	double m_fRangeTo; //结束长度
	double m_fRangeFrom;  //开始长度
	double m_fWeight;     //权重
	double m_fTotalWeight;//前总权重
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
