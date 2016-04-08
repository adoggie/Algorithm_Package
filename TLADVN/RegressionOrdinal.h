// RegressionOrdinal.h: interface for the CRegressionOrdinal class.
//          有序回归
//       标注：余卫军
//       时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGRESSIONORDINAL_H__9AE910A5_8516_4DDB_B6AA_8607B132FB30__INCLUDED_)
#define AFX_REGRESSIONORDINAL_H__9AE910A5_8516_4DDB_B6AA_8607B132FB30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"


class AFX_EXT_CLASS CRegressionOrdinal : public CAlgorithmBase  
{
public:
	CRegressionOrdinal();
	virtual ~CRegressionOrdinal();

	CTLTable * CreateWarningTable();
	TIntStruct m_tVarInfo_Cov; //协变量下标集
	CResult * OnOrdinal(CDataInterface * pDataInterface, string VO);
	double * GetData(CDataInterface *pDataInterface);
	int  m_nDependIndex;//因变量下标

protected:
	CIntVector SortVector(CIntVector vect);
	void SortPtrArray(CPtrArray * par);
private:
	BOOL GetValueFromVO(CDataInterface * pDataInterface, string VO);
	int GetData( CDataInterface *pDataInterface, double*& pWeight, double*& pDependData, double*& pFactorData, double*& pCovData, int& nCaseNum );
	void cal_R(CDoubleVector  LH,int n);
	void GetXxx(CDoubleMatrix X, CDoubleVector Y, CDoubleVector weight, CDoubleMatrix &newX_matrix, CDoubleMatrix &newY_vector, CIntMatrix &set);
	void InitParma();
	int GrpToIdx(CIntVector vect,CIntVector &vect1);
	int QuickFind( CTArray<int, int>& arSort, int nData, int iBegin, int iEnd );

	CIntMatrix GetYaMatirx(CIntVector vect,int num);
	void GoodnessFitting(CDoubleMatrix X ,CDoubleMatrix  Y ,CDoubleVector  betaVector , int Jnum ,int varNum);
	void ModelFiting(CDoubleMatrix X,CDoubleMatrix Y,CDoubleVector betaVector,CDoubleVector initialBetaVector, int Jnum, int varNum,int cNum ,CDoubleVector& LH,double &ChiSquare,int &df,double &sig,CDoubleVector weight);
	CIntMatrix Zhankai(CIntMatrix x, int& nCategoryNum);
	void Ordinal_statis(CDoubleVector vector_lastBeta,CDoubleMatrix matrix_inform,int Jnum,int varNum,CTStringArray * parString);
	CDoubleMatrix OrdinalRe(CDoubleMatrix matrix_x,CIntMatrix vector_y,CDoubleVector vector_preBeta,int Jnum,int varNum,double epsilon,CDoubleVector &vector_lastBeta,CDoubleVector weight);
	bool UseOrdinal(CDoubleMatrix matrix_f,CDoubleVector vector_Y,int Num_cov,CDoubleVector weight,CDoubleMatrix matrix_cov =NULL);
	CDoubleVector GetBetaVector_Ordinal(CDoubleMatrix matrix_x,CIntMatrix matrix_y,CDoubleVector vector_beta,int Jnum,int varNum,CDoubleMatrix& matrix_inform,CDoubleVector weight);
	int    delta(int j,int k);
	double GetXValue(CDoubleMatrix matrix_x,int Jnum,int varNum,int i,int k,int j);
	CDoubleVector GetInitialBeta(CIntMatrix matrix_class,int nvarNum);
	CIntMatrix GetYVector(CIntVector vector_y,CIntMatrix matrix_class);
	CIntMatrix GetClassSet(CIntVector vector_y);

	CTString m_strErrorInfo;
	CTStringArray m_strArrayFieldName;
	bool m_bStopCalculate;
	CTString m_strMissField;

	CTLTable* briefrTable;
	CTString strVar;

};

#endif // !defined(AFX_REGRESSIONORDINAL_H__9AE910A5_8516_4DDB_B6AA_8607B132FB30__INCLUDED_)
