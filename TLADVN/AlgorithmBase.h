// AlgorithmBase.h: interface for the CAlgorithmBase class.
//						高级统计算法基类
//         功能： 提供高级统计所有算法的公共方法和属性
//                      标注：余卫军   
//                     2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALGORITHMBASE_H__9B717505_CE25_45EB_AFC8_AC63365BFC9C__INCLUDED_)
#define AFX_ALGORITHMBASE_H__9B717505_CE25_45EB_AFC8_AC63365BFC9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define  ZERO 1e-100
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
typedef std::pair<int,int> TIntPair;
typedef TCDataArray<TIntPair> TIntPairAry;

class AFX_EXT_CLASS CAlgorithmBase// : public CObject  
{
public:
	CAlgorithmBase();
	virtual ~CAlgorithmBase();
	TIntStruct m_tVarInfo;

//protected:
	CDoubleMatrix CombieMatrix(CDoubleMatrix matrix,CDoubleVector);
	void ProcessMissValue(CPtrArray *par,CDoubleVector &weight,CDataAccess * pAccess,int nMethod = 0 );
	CDoubleVector GetWeightVector(CDataInterface * pDataInterface);
	CDoubleMatrix GetTransMatirx();
	int GetMatrixLen();
	long TraceMatrix(const CDoubleMatrix &matrix);
	long TransMatrix(CDoubleMatrix &matrix);
	void ProcessMissValue(CPtrArray * par,CDoubleMatrix &matrix,CDoubleVector &weight,CDataAccess * pAccess);
	long GetVectorLen();
	void TraceVector(const CDoubleVector &vect,bool bFlag = true,int nValue = -1);
	void TraceVector(const CIntVector &vect);
	void ProcessVector(CDoubleVector &vect,long &ltmp);
	
	void SortMatrix1(CDoubleMatrix &matrix);
	//void SortPtrArray_Double(CPtrArray * par);
	bool IsVectorEqual(CDoubleVector vect1,CDoubleVector vect2);
	void ProcessWeight(CDoubleMatrix &matrix,CDoubleVector &vect_weight);
	CDoubleMatrix MatrixDelCol(CDoubleMatrix matrix , int nCol);
	CIntMatrix DoubleMatrixToInt(CDoubleMatrix matrix);
	CDoubleMatrix IntMatrixToDouble(CIntMatrix matrix);
	CDoubleVector	IntToDouble(CIntVector vect);
	CIntVector		DoubleToInt(CDoubleVector vect);
	CIntMatrix		IntToDouble(CDoubleMatrix matrix);
	CIntMatrix		VectorToMatrix(CIntVector vect);
	CIntMatrix		VectorToMatrix(CIntVector vect,int nRow,int nSize);//Added by Ywj 2005-4-29
	CDoubleMatrix   IntToDouble(CIntMatrix matrix);

	double   Data(const int nRow,const int nCol);	
	double&  wData(const int nRow, const int nCol); // yzmAdd  可写取数据
	void	 SetRowsCols(const int nRow,const int nCol);
	void     SetVectData(double *pData,const int nRow, const int nCol); //yzmAdd  将一组数据拷贝到m_VectData中

	CIntVector  CheckAvailData(CDataInterface *pDataInterface, CDWordArray &colArray);
//	double * m_pData;
	CDoubleVector  m_VectData;

	CDoubleVector  Dev(int nIndex=0);
	CDoubleVector  Average(int nIndex=0);
	int     m_nRow;											
	int     m_nCol;										
	int     m_nSel;										

	CDoubleVector	MatrixToVector(const CDoubleMatrix &matrix);

	CDoubleMatrix	VectorToMatrix(const CDoubleVector &vect);
	
	CDoubleMatrix	VectorToMatrix(const CDoubleVector &vect,int nRow,int nCol);//Added by Yuweijun 2005-04-29
   
	double *		MatrixTran(double * a,int m,int n);
	CDoubleVector   MatrixTran(CDoubleVector Vect_A, int m, int n);

	void			FreePtrArray(CPtrArray * pArray);
     
	void WriteData(int * pData, int nRow, int nCol,char * pszTitle);
	void WriteData(double *pData,int nRow,int nCol,char * pszTitle=NULL);
	void WriteData(CIntVector vectr, char *pszTitle);
	bool WriteData(CIntMatrix m, char *pszName);
	bool WriteData(CComplexMatrix matrix,char * pszName);
	void WriteData(CDoubleMatrix matrix,char * pszName);
	void WriteData(CDoubleVector vect,char * pszTitle);

	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;

//	double		   * m_pCorrelation;
	
	CDoubleVector	 m_VectCorrelation;//Ywj 2005-04-29

	double *		GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual=true);
	void			MatrixToPointer(CDoubleMatrix matrix,double ** pt);
	void			PtrArrayCopy(CPtrArray * pSource,CPtrArray * pDes);

	bool			SortEigenVect(CDoubleMatrix  &matrix_EigValue,CDoubleMatrix  &matrix_EigVector , int nIndex=0);
	void			SortEigenVect(double * pEigenValue,double * pEigenVector,int m,int n,int nIndex = 0);
	void			SortEigenVect(CDoubleVector& pEigenValue, CDoubleVector& pEigenVector,int m,int n,int nIndex);
		
	double *		RelationMatrix(double * pData,int m,int n);
	CDoubleMatrix	RelationMatrix(CDoubleMatrix matrix,int nIndex=0);
	CDoubleVector   RelationMatrix(CDoubleVector& Vect_Data, int m, int n);
		
	//bool Formalize(double * pData,int nRow,int nCol,int nIndex=0);
	bool Formalize(CDoubleVector& Vect_Data,int nRow,int nCol,int nIndex=0);
	bool Formalize(const int nIndex=0);		
	
	bool IsZero( double data );

	//double * Dev(double * pData,int nRow,int nCol,int nIndex);
	CDoubleVector Dev(CDoubleVector Vect_Data,int nRow,int nCol,int nIndex);
	
	//double * Average(double * pData,int nRow,int nCol,int nIndex);
	CDoubleVector Average(CDoubleVector& Vect_Data,int nRow,int nCol,int nIndex);

	double * GetDataSingle(CDataInterface *pDataInterface,int nIndex);
	
	FILE * m_fp;	


};

#endif // !defined(AFX_ALGORITHMBASE_H__9B717505_CE25_45EB_AFC8_AC63365BFC9C__INCLUDED_)





















