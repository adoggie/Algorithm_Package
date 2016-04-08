// RegressionLogistic.h: interface for the CRegressionLogistic class.
//				Logistic 回归
//              标注： 余卫军
//              时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGRESSIONLOGISTIC_H__F84D4F23_E5B9_41BA_966B_38042B9844E1__INCLUDED_)
#define AFX_REGRESSIONLOGISTIC_H__F84D4F23_E5B9_41BA_966B_38042B9844E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"


class AFX_EXT_CLASS CRegressionLogistic : public CAlgorithmBase  
{
public:
	CRegressionLogistic();
	virtual ~CRegressionLogistic();
	CTLTable * CreateWarningTable();

	bool Logistics(CDoubleMatrix matrix,CDoubleVector vector_weight,CResult * pResult);
	void StatisForLogistic(CDoubleMatrix matrix_covBeta,CDoubleVector vector_beta,int nColNum);//计算并输出结果
	CResult * OnLogistics(CDataInterface *pDataInterface, string VO);
	CResult* OnRegPred(CDataInterface * pDataInterface, string VO);
 
	TIntStruct m_tVarCategrery;
	CTStringArray m_strInteractiveArray;
	int  m_nDependIndex;


	CTLTable* briefrTable;
	CTString strDependName;

private:
	bool GainLeftPlot(CDataInterface *pDataInterface);//画Gain-Left图、模型评估
	bool GetValueFromVO(CDataInterface *pDataInterface, string VO);
	double GetT(CDoubleMatrix xMatrix,CDoubleVector yVector,CDoubleVector nVector,CDoubleVector weight,CDoubleVector preBeta,CDoubleVector S);
	double GetLikeHood(CDoubleMatrix xMatrix,CDoubleVector yVector,CDoubleVector nVector,CDoubleVector weight,CDoubleVector preBetaVector);
	void GetHessian(CDoubleVector probVector,CDoubleMatrix xMatrix,CDoubleMatrix yMatrix,CDoubleVector nVector,CDoubleVector weight,CDoubleVector &vect_q,CDoubleMatrix &hessen);
	void InitParma();

	//CDoubleMatrix GetCategrery(TDataSet * pDataArray,int nRow);
	CDoubleMatrix GetCategrery(TDColAry * pDataArray,int nRow);
	
	void GetDataLogistic_Sub(CDoubleMatrix &matrix,int ntbegin,int nt,CTString strField,CTString strField2,CDataInterface * pDataInterface);
	bool IsCategreryVar(CTString strName,int &ntbegin);

//	CDoubleMatrix GetDataForLogistics(CDataInterface * pDataInterface,CTStringArray * pstrarray,TIntStruct pIntCorrelation);

	CDoubleVector InitialBeta(CDoubleMatrix matrix_X,CDoubleMatrix matrix_Y,CDoubleVector vector_n);

	//CDoubleMatrix GetDataForLogistics(CDataInterface * pDataInterface,CTStringArray * pstrarray,double*& pWeight,int& nCaseNum);
	CDoubleMatrix GetDataForLogistics(CDataInterface * pDataInterface,CTStringArray * pstrarray,CDoubleVector& pWeight,int& nCaseNum);
	
	//bool IsBinary( TDataSet * pData, double * pWeight, int nCaseNum );
	bool IsBinary( TDColAry * pData, CDoubleVector pWeight, int nCaseNum );

	//bool GetCategoryMatrix( double pData[], CDoubleMatrix*& tempMatrix, int nCaseNum );
	bool GetCategoryMatrix( CDoubleVector pData, CDoubleMatrix*& pMatrix, int nRow );
	
	//void GenerateMatrix( CTArray< double*, double* >& arNewColArry, CTArray< CDoubleMatrix*, CDoubleMatrix* >& arCategory, CTArray<int, int>& arInteractive, int iIndex[], CDoubleMatrix& matrix, CDataInterface * pDataInterface, int nCaseNum );
	void GenerateMatrix( CTArray< CDoubleVector *, CDoubleVector *>& arNewColArry, CTArray< CDoubleMatrix*, CDoubleMatrix* >& arCategory, CTArray<int, int>& arInteractive, CIntVector iIndex, CDoubleMatrix& matrix, CDataInterface * pDataInterface, int nCaseNum );

	int GetCategoryNum( int nCol );
	bool GetPredValueVo(CDataInterface * pDataInterface, string VO);//m_strModel作为模型已经在算法中存在了
	bool GetValueFromModel(CDataInterface* pDataInterface, string Model);
	//模型保存
	bool Export();
private:
	CTStringArray m_strArrayFieldName;//自变量名字
	int m_nXCol;//因变量个数
	CTString m_strMissField;
	CTString m_strErrorInfo;
	bool m_bStopCalculate;
	bool m_bSaveModel;//是否保持模型
	bool m_bEvaluation;//是否进行模型评估
	string m_ModelFile;//保存模型的路径
	TIntStruct m_tVarPred;//预测比例的下标，按参数的顺序排列的。
	CDoubleVector m_CoefVec;//系数向量
	CTString m_strDepPred;//预测变量名字
	CResult * m_pResult;
};

#endif // !defined(AFX_REGRESSIONLOGISTIC_H__F84D4F23_E5B9_41BA_966B_38042B9844E1__INCLUDED_)
