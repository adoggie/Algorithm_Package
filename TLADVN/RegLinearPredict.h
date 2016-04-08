/*// RegLinearPredict.h: interface for the CRegLinearPredict class.
//                          线性回归预测模块
//                   功能： 运行回归分析得到回归方程后，输入给定的新的观测自变量数据，
//                          得到预测值及其置信区间
//                   作者： 余卫军
//                   时间： 2005年10月26日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_)
#define AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "RegressionLinear.h"
//预测模块的选项设置结构类
class AFX_EXT_CLASS CRegLinearPredictVo
{
public:
	CRegLinearPredictVo()
	{
		m_bConfidInterval = false;
		pRegressLinearResult = NULL;//默认为空
	}
	bool   m_bConfidInterval;//是否需要置信区间0－不需要，1－需要，默认0不需要
	int m_nDependIndex;//指定因变量下标
	TIntStruct m_tVarInfo;//自变量下标集，按照模型的变量顺序填写。
    TRegressLinearResult*  pRegressLinearResult;//回归分析模型
protected:
private:
};

class AFX_EXT_CLASS CRegLinearPredict  : public CAlgorithmBase
{
public:
	CRegLinearPredict();
	virtual ~CRegLinearPredict();
	//返回一个矩阵。
	//如果没有要求输出置信区间，那么得到的就是一个n×1的矩阵，否则为n×3的矩阵，其中n为新的观测个数
	CDoubleMatrix OnRegLinearPredict(CDataInterface* pDataInterface,CRegLinearPredictVo* pVo);
public:
	bool	m_bConfidInterval;//是否给出置信区间。默认值为 1；
	double	m_dConfidence;//置信水平，取值0－1之间，默认值为0.95
	int		m_nDependIndex;//因变量在新的数据文件中的下标
private:
	CDoubleMatrix GetPredictY(CDoubleMatrix vect_NewX//新的自变量观测值
			       , TRegressLinearResult* pRegResult);//线性回归模型
	
	//获取数据，如果成功，返回1，否则返回0
    bool GetData(CDataInterface* pDataInterface,TRegressLinearResult* pRegResult,CDoubleMatrix& Matrix_NewX ,CDoubleMatrix& Matrix_Y);
};

#endif // !defined(AFX_REGLINEARPREDICT_H__D78B81F7_7899_4863_8A8E_2E317C046D21__INCLUDED_)
*/
