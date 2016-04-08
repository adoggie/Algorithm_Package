// CointegrationTest.h: interface for the CCointegrationTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_)
#define AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CCointegrationTest : public CAlgorithmBase  
{
public:
	CResult* OnRun(CDataInterface *pDataInterface, string VO);
	CCointegrationTest();
	virtual ~CCointegrationTest();
	TIntStruct		m_tVarInfo;			//检验变量struct
	TIntStruct		m_tVarInfo_D;		//外生变量struct	
private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();
	void GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//从界面读数据		

	CTStringArray	m_strDepend;        //检验变量名称字符串
	CTStringArray	m_strVarSel;        //外生变量名称字符串

	struct VI							//定义输入变量结构
	{
		CDoubleMatrix m_mxX;			//输入检验变量矩阵
		CDoubleMatrix m_mxY;			//外生变量
		int m_nLagnum;					//序列滞后阶数
		int	m_nRegre;					//校验变量个数
		int	m_nExto;					//外生变量个数
		int m_nModel;					//选择模型
	} m_tIn;
	struct VO 
	{
		CDoubleVector m_vrEigValue;		//特征值
		CDoubleVector m_vrEigTrace;		//轨迹检验统计量
		CDoubleVector m_vrEigMax;		//最大特征值统计量
		CDoubleMatrix m_mxMaxCri;		//最大特征值检验临界值矩阵
		CDoubleMatrix m_mxTraceCri;		//特征值轨迹检验特征值矩阵
		CDoubleVector m_vrTraceCrit;	//要用到的特征值轨迹临界值
		CDoubleVector m_vrMaxCrit;		//要用到的最大特征值检验临界值
		CDoubleMatrix m_mxCoff;			//经过Johansen归一化的特征向量矩阵
		double m_nIndex;				//得到的协整关系的个数
		CDoubleMatrix m_mxCoff1;		//每种协整模式的系数的综合矩阵

	} m_tOut;
	bool coinTest();					//协整检验主函数，采用Johansen方法，参考汉密尔顿《时间序列分析》
	CDoubleMatrix JJtest(CDoubleMatrix mxX,			//要检验的变量列表
						CDoubleMatrix mxY,			//给出的外生变量
						int dChoice,				//模型的设置选项
						int lag,					//给出滞后的阶数
						CDoubleVector& mxEigvec,
						CDoubleMatrix& mxVV);	
	//计算给定变量的JJ检验

};

#endif // !defined(AFX_COINTEGRATIONTEST_H__AB39CB63_9C8C_4A10_A525_125B0A1539FF__INCLUDED_)





















