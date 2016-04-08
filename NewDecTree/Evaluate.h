// Evaluate.h: interface for the CEvaluate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_)
#define AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DecTree.h"

class AFX_EXT_CLASS CEvaluate  
{
public:
	CEvaluate(CDecTree *pDecTree);
	virtual ~CEvaluate();

	void EV(CDoubleMatrix &m, CResult *pResult);
	//是否在给定的范围内
	bool In(int nValue);
private:
	//标签
	CTString GetLabel(int nValue);
	//评估-计算目标变量的频率
	void CalcFrequence(int nValue, CTArray<int,int>&arrFreq);
	CTArray<int,int> CalcFrequence(CDoubleVector &v); 
	//评估-计算目标变量与预测变量间的差异和
	void CalcEvalue(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleMatrix &me);
	//计算GAIN,LIFT图表值
	void CalcChart(CDoubleMatrix &m, CPtrArray *pArr);
	//计算准确率，覆盖率，正确率
	void CalcText(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleVector &v);
	//计算置信度范围
	void CalcConfidence(double fValue, double fConfidence, CPtrArray *pArr);
	//释放内存
	void Release(CPtrArray *pArr);
private:
	double    m_fValCnt;  //实际在范围内的总数
	CDecTree *m_pDecTree;
private:
	class CEv
	{
	public:
		CEv()
		{
			fX = 0;
			fY = 0;
			fConfidence = 0;
		};
	public:
		double fY;      //Y-转换为Gain值
		double fX;      //X-转换为百分位数
		double fConfidence;  //置信度
	};
};

#endif // !defined(AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_)
