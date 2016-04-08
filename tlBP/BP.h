// BP.h: interface for the CBP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_)
#define AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Matrix.h"
#include "BPArithmetic.h"

class CBPNetOutput;

class CBP  
{
	CBPArithmetic m_bpa;
	// 用于存储输入的样本数据的矩阵
	CBPMatrix		m_mxDemoDataInput;
	// 样本的输入层的数值矩阵
	CBPMatrix		m_mxInput;
	// 输入层到隐含层的权值矩阵
	CBPMatrix		m_mxIToHWeight;
	// 隐含层的阀值矩阵
	CBPMatrix		m_mxHideThreshold;
	// 隐含层到输出层的权值矩阵
	CBPMatrix		m_mxHToOWeight;
	// 输出层的阀值矩阵
	CBPMatrix		m_mxOutThreshold;
	// 隐含层的输出值的矩阵
	CBPMatrix		m_mxHide;
	// 输出层的输出值的矩阵
	CBPMatrix		m_mxOut;
	// 用来仿真时输入的网络参数矩阵
	CBPMatrix		m_mxSimuNet;

	// 系统误差
	double		m_SystemErrorNew;
	bool		m_bSimulateDataFlag;// 是样本训练(0)还是数据仿真(1)

public:
	int m_nInputLayerNumber;//输入层数目:
	int	m_nHideLayerNumber;//隐含层数目:
	int	m_nOutputLayerNumber;//输出层数目:

	int		m_nComboArithmetic;//用来训练的算法 0:LM  1:BP
	int m_nComboFunc;//用来训练的激活函数 0:Sigmoid 1:tanh 2:Tansig
	double m_nSystemErrorLevel;//系统精度:
	//double	m_nSystemError;
	//unsigned int	m_nTrainTimes;
	unsigned int	m_nMaxTrainTimes;//最大训练次数:
	double	m_dStep;//步长
	

	CBPMatrix m_mxTrainOut;  //保存训练结果(存储模拟过输出层的输出值矩阵)
public:
	CBP();
	virtual ~CBP();
public:
	double		m_SystemErrorOld;
	void Reset();
	void Serialize(CMemoStream *pStream, bool IsStoring);
	void SetWnd();
	void Abort();
	// 样本训练
	bool TrainingDemo(CBPMatrix &mxSour);
	// 数据仿真  mxSimuResult用来存放仿真数据结果的矩阵
	bool SimulateData(CBPMatrix &mxSour,CBPMatrix &mxSimuResult);
	bool	CreatePredModel(CBPNetOutput *&pOutStruc);
	void	initPred(CBPNetOutput &Output);
	//模型
	virtual bool Export(TCLFile &theFile);
	virtual bool Import(TCLFile &theFile);
private:
	// 反复训练
	bool DemoDataTrainRepeat();
	// 前向计算初始化
	void ForwardCalculateInit();
};

#endif // !defined(AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_)
