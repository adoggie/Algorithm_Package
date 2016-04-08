// Discriminant.h: interface for the CDiscriminant class.
//
//								判别分析
//                    功能：判别分析的基本方法和属性
//                    标注：  余卫军
//					  时间：2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_)
#define AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

//#include "Factor.h"
//#include "Matrix.h"	// Added by ClassView
#include "AlgorithmBase.h"



class AFX_EXT_CLASS CDiscriminant : public CAlgorithmBase  
{
public:
	int m_nDependIndex;
	CResult * OnDiscriminant(CDataInterface * pDataInterface,std::string &VO);
	bool StepWiseDiscriminant(CDoubleMatrix matrix_X,int allORstempwise,int distanceORfisher,double Entry,double Removal);
	CDiscriminant();
	virtual ~CDiscriminant();
	TIntStruct m_tVarInfo;

private:
	void Classify(CDoubleMatrix &finalClassData);    //判别分析回判过程（新加）
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);//从VO字符串中读取控制参数
	void CreateResultF(CDoubleVector F,CDoubleVector L);
	void CreateResultC1(CDoubleMatrix matrix_C1);
	void CreateResultR(CDoubleMatrix matrix_R);
	void CreateResultT1(CDoubleMatrix matrix_TC);
	void CreateResultdiscri(CDoubleMatrix discriminant_mx);   //输出回判结果小结（新加）
	void CreateResultdiscripercent(CDoubleMatrix discriminantpercent_mx);   //输出回判结果百分比小结（新加）
	void CreateResult();    //输出各样本回判结果（新加）
	void CreateResultC2(CDoubleMatrix * pt,char * pszTitle);
	void CreateResultIn1(CDoubleVector F,CDoubleVector L,CIntVector In,int vrb,CTString pszTitle);
	void CreateResultCO(CDoubleMatrix a,CDoubleVector b,int vrb,CDoubleVector vector_dnum);
	CDoubleMatrix DeleteSingleGroup(CDoubleMatrix matrix);
	void CreateResult(CDoubleMatrix matrix_a,CDoubleVector vector_b,CDoubleMatrix matrix_result);	
	void SortMatrix(CDoubleMatrix &matrix);
	void DataProcess(CDoubleMatrix &matrix);
	CIntVector Classification(CDoubleMatrix matrix_X, CDoubleMatrix matrix_a, CDoubleVector vector_b);
	void CpTable(CDoubleVector vector_original, CIntVector vector_predict, CDoubleMatrix &matrix_classresult, CDoubleMatrix &matrix_rateresult);
	double	m_dEntry;
	double	m_dRemoval;
	int		m_nCoefficient;
	double	m_nMethod;
	CDoubleMatrix m_finalClassData;
    CDoubleMatrix m_original_mx;
	bool	m_bC1;	//组间协方差
	bool	m_bC2;	//组内协方差
	bool	m_bT1;	//总协方差
	bool	m_bR;	//相关系数矩阵 
	bool	m_bFL;	//F和LAMBDA
	bool	m_bIn1;	//不选入分析变量
	bool	m_bDisResult; // 是否输出各样本回判结果（新加）
	CDoubleVector m_constant_Vec;  //常熟项系数（新加）
	CDoubleMatrix m_cofficient_mx;  //系数矩阵（新加）
	int m_group;    //组别数（新加）
	CDoubleVector m_class_Vec;
	CDoubleVector m_num_Vec; //每组样本个数  （新加）
	CDoubleVector m_oriclass_Vec;  //原来的类别（新加）
	CDoubleVector m_newclass_Vec;   //回判类别（新加）
	CDoubleMatrix m_discri_mx;      //回判小结矩阵（新加）
	CDoubleMatrix m_discripercent_mx;  //回判百分比小结矩阵（新加）
	CDoubleVector GetData(CDataInterface * pDataInterface,int &Sum);
	CDoubleVector GetGroupValue(CDoubleVector vect);
	int GetGroup(CDoubleVector vect);
	bool Coefficient(CDoubleVector vector_prob,CIntVector vector_In,double nSum,CIntVector vector_m,CDoubleMatrix matrix_w,CDoubleMatrix matrix_XGrpVrbMean,CPtrArray * parX3D,CDoubleMatrix &matrix_a,CDoubleVector &vector_b,int way,int vrb);
	bool In(CDoubleMatrix &matrix_w, CDoubleMatrix &matrix_t, double nSum, int g, int i, CIntVector &vector_In, double Entry, CDoubleVector &vector_LAMBDA, int &vrb, int &id, int p, int allORstepwise, double Removal);
	bool Sweep(CDoubleMatrix matrix_w, CDoubleMatrix matrix_t, int r,CDoubleMatrix &matrix_w1,CDoubleMatrix &matrix_t1);
};

#endif // !defined(AFX_DISCRIMINANT_H__0FB67880_68A6_422C_AE6D_F85FD932B819__INCLUDED_)
