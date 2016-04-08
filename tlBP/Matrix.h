/////////////////////////////////////////////////////////////////////////////
// Matrix.h : 
//		Interface of the class CBPMatrix
// Author : freeia
// Modified Date : 3/11/2003
// E-mail : freeia@163.com
// Company/School : hdcad
/////////////////////////////////////////////////////////////////////////////

#ifndef _MATRIX_H
#define _MATRIX_H

#include <vector>
#include "MemoStream.h"


typedef std::vector <double> VDOUBLE;
typedef std::vector  <VDOUBLE> TMatrix;


class  CBPMatrix  
{	

	/************************************************************************
	*				the interface function of the class CBPMatrix 			*
	************************************************************************/
public:
	int GetMaxValueCol(int nRow);
	static bool FindPos(double *pSortAry,int nBufLen,double value,int&nPos);
	int GetDistinct(int nCol,double*pSortAry,int nBufLen);
	double* GetColArray(int nIndex);
	void SetCol(int nCol);
	void ReSize(int nRow,int nCol,bool bZero = true);
	void Serialize(CMemoStream*pStream,bool IsStoring);
	void Print(CTString strName );
	void Print();

	/////////////////////////////////////////////////////////////////////////
	// Construction and Destruction	
	CBPMatrix();

	CBPMatrix(const CBPMatrix& cMatrixB);

	virtual ~CBPMatrix();

	TMatrix	m_pTMatrix;				// ָ������ͷָ��

	/////////////////////////////////////////////////////////////////////////
	// According to the parameters nRow & nCol to construct a matrix
	CBPMatrix(unsigned int nRow, unsigned int nCol);


	/////////////////////////////////////////////////////////////////////////
	// This function initialize the matrix :
	//		the matrix which has been initialized has 0 row & 0 column, and
	// all elements in it is zeros.
	// 
	void Initialize();

	/////////////////////////////////////////////////////////////////////////
	// This function initialize the matrix :
	//		the matrix which has been initialized has 0 row & 0 column, and
	// all elements in it is zeros.
	// 
	void InitializeZero();

	/////////////////////////////////////////////////////////////////////////
	// To make random in the elements of the matrix and the elements of the 
	// matrix has been randomized between -1 and 1.These elements must be
	// decimal fractions.
	// 
	void RandomInitialize();

	/////////////////////////////////////////////////////////////////////////
	// Overload Operations

	// 'CBPMatrix + CBPMatrix'
	CBPMatrix operator + (const CBPMatrix& cMatrixB);	
	// 'CBPMatrix - CBPMatrix'
	CBPMatrix operator - (const CBPMatrix& cMatrixB);	
	// 'CBPMatrix * CBPMatrix'
	CBPMatrix operator * (const CBPMatrix& cMatrixB);	
	// 'CBPMatrix * double'
	CBPMatrix operator * (double nValue);	
	// 'CBPMatrix = CBPMatrix'
	CBPMatrix& operator = (const CBPMatrix& cMatrixB);	
	// 'CBPMatrix += CBPMatrix'
	CBPMatrix& operator += (const CBPMatrix& cMatrixB);	
	// 'CBPMatrix .* CBPMatrix'
	CBPMatrix operator / (const CBPMatrix& cMatrixB);	


	/////////////////////////////////////////////////////////////////////////
	// Transpose the matrix
	//
	CBPMatrix Transpose();

	/////////////////////////////////////////////////////////////////////////
	// Inverse the matrix
	//
	CBPMatrix Inverse();

	/////////////////////////////////////////////////////////////////////////
	// Load the data from the file and reset the rows and the colums of the 
	// matrix
	//
	bool LoadDataFromFile(CTString& strFileName);

	/////////////////////////////////////////////////////////////////////////
	// Save the data from the CBPMatrix object to the specified file
	//
	bool SaveDataToFile(CTString strFileName);
	bool SaveDataToArray (CTStringArray& strArray);
	
	/////////////////////////////////////////////////////////////////////////
	// Get the matrix Row Number
	//
	unsigned int GetMatrixRowNumber() const
	{
		return m_nRow;
	}

	/////////////////////////////////////////////////////////////////////////
	// Get the matrix Colum Number
	//
	unsigned int GetMatrixColNumber() const
	{
		return m_nCol;
	}



	/////////////////////////////////////////////////////////////////////////
	//	Copy data from a vector to a matrix
	//	Parameter:
	//		[out]	cMatrix ----> the returned value
	//		[in]	nIndex	----> the index in vector
	//	Notes:
	//		the object copied must be vector!!!
	//
	void CopySubMatrixFromVector(CBPMatrix& cMatrix,unsigned int nIndex);

	/////////////////////////////////////////////////////////////////////////
	// Copy data from sub matrix to another matrix
	// Parameter:
	//		[out]	cMatrix ----> ������Ӿ��󷵻صĽ��
	//		[in]	nStartX ----> �Ӿ����ھ����е���ʼ����,��Ӧ��,������1��ʼ
	//		[in]	nStartY ----> �Ӿ����ھ����е���ʼ����,��Ӧ��,������1��ʼ
	//
	void CopySubMatrix(CBPMatrix& cMatrix,unsigned int nStartX,unsigned int nStartY);

	/////////////////////////////////////////////////////////////////////////
	// Copy Matrix
	//	Parameter:
	//		[in]	cMatrix ----> be copied matrix
	//
	void CopyMatrix(const CBPMatrix& cMatrix);

	/////////////////////////////////////////////////////////////////////////
	// ����������е�Ԫ�ذ��кϳ�һ��
	//	���磺
	//		matrix = [
	//			1	2	3
	//			4	5	6
	//			7	8	9
	//				]
	//		CBPMatrix cMatrix = matrix.MergeColumnsToColumnVector();
	//		cMatrix = 
	//			[	1
	//				4	
	//				7
	//				2
	//				5
	//				8
	//				3
	//				6
	//				9	]
	//
	CBPMatrix MergeColumnsToColumnVector();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = f(x) = 1 / (1 + exp(-x))	( 0 < f(x) < 1)
	//
	CBPMatrix Sigmoid();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = f'(x) = (1 / (1 + exp(-x)))'	( 0 < f(x) < 1)
	//			  = exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
	//
	CBPMatrix SigmoidDerivative();


	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = tanh(x) = (1 - exp(-x)) / (1 + exp(-x))
	//					 =  1 - 2 * exp(-x) / (1 + exp(-x))	( -1 < f(x) < 1)
	//
	CBPMatrix tanh(); 

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = tanh'(x) = ((1 - exp(-x)) / (1 + exp(-x)))'	( -1 < f(x) < 1)
	//					 = 	2*exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
	//
	CBPMatrix tanhDerivative();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = Tansig(x) = 2 / (1 + exp(-2 * x)) -1
	//
	CBPMatrix Tansig();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
	//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
	//			y = Tansig'(x) = (2 / (1 + exp(-2 * x)) -1)'
	//				= (2 / (1 + exp(-2 * x)) -1) * (2 / (1 + exp(-2 * x)) -1) -1
	//
	CBPMatrix TansigDerivative();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ����е�Ԫ�ؽ���һ�β���:
	//		ʹ�������е����Ӧ����Ԫ�����
	//	Parameter:
	//		nRowIndex	---->	������ֵ(the index starts from 0)
	//							�Դ�����Ϊ��׼,ʹ����������������Ӧ���е�ֵ
	//							��������Ӧ���е�ֵ���	
	//
	void MakeAllColumnElementsSameValue(unsigned int nRowIndex);

	/////////////////////////////////////////////////////////////////////////
	// �Ծ����е�Ԫ�ؽ���һ�β���:
	//		ʹ�����Ϊ��λ��
	//
	void Eye();

	/////////////////////////////////////////////////////////////////////////
	// Get System Error
	//
	double	GetSystemError() const;

	/////////////////////////////////////////////////////////////////////////
	// Make all the matrix elements to be changed into absolute value
	//
	CBPMatrix AbsoluteValue();

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		������������Դ
	//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
	// Purpose:
	//		This function will copy all the data of the cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void GetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		�����ľ���
	//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
	// Purpose:
	//		This function will copy part of the object data into cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void SetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		������������Դ
	//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
	//		unsigned int nRow:		�������������ڱ����������е�������(��0��ʼ)
	// Purpose:
	//		This function will copy all the data of the cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void GetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		�����ľ���
	//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
	//		unsigned int nRow:		�����������ڱ��������е�������
	// Purpose:
	//		This function will copy part of the object data to fill the special 
	// row of the cMatrix
	//	Notes:
	//		The object must be column vector!!!
	//
	void SetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow);

	/////////////////////////////////////////////////////////////////////////
	// Get the total value of the matrix
	double GetTotalElementValue();

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������չ
	//	ʵ�ֹ���:
	//		�Ծ��������������չ,nTimes��ÿ����չ�Ĵ���
	//
	void nncpyi(const CBPMatrix &cMatrix, unsigned int nTimes);

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������չ
	//	ʵ�ֹ���:
	//		�Ծ��������������չ
	//	matrix =	[ 
	//			1	2	3
	//			4	5	6
	//			7	8	9
	//				]
	//
	//		nncpyd(matrix)	=	[
	//			1	0	0	2	0	0	3	0	0
	//			0	4	0	0	5	0	0	6	0
	//			0	0	7	0	0	8	0	0	9
	//							]
	void nncpyd(const CBPMatrix &cMatrix);

	/////////////////////////////////////////////////////////////////////////
	// �Ծ��������չ
	//	ʵ�ֹ���:
	//		�Ծ��������������չ,nTimes��ÿ����չ�Ĵ���
	//	matrix =	[ 
	//			1	2	3
	//			4	5	6
	//			7	8	9
	//				]
	//		nTimes = 2
	//
	//		nncpyd(matrix)	=	[
	//					1	2	3	1	2	3
	//					4	5	6	4	5	6
	//					7	8	9	7	8	9
	//							]
	//
	void nncpy (const CBPMatrix& cMatrix, unsigned int nTimes);



private:

	unsigned int m_nRow;			// ������ӵ�е�����
	unsigned int m_nCol;			// ������ӵ�е�����


	/////////////////////////////////////////////////////////////////////////
	// ע��:
	//		���������þ����������������,�����е�Ԫ�ر����³�ʼ��Ϊ0
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// ���þ��������
	//
	void SetMatrixRowNumber(unsigned int nRow,bool bNeedZero = true);

	/////////////////////////////////////////////////////////////////////////
	// ���þ��������
	//
	void SetMatrixColNumber(unsigned int nCol,bool bNeedZero = true);

	/////////////////////////////////////////////////////////////////////////
	// ���þ����������
	//
	void SetMatrixRowAndCol(unsigned int nRow,unsigned int nCol);


	/////////////////////////////////////////////////////////////////////////
	// �������������
	//
	void SwapMatrixRow(unsigned int nRow1,unsigned int nRow2);

	/////////////////////////////////////////////////////////////////////////
	// �������������
	//
	void SwapMatrixCol(unsigned int nCol1,unsigned int nCol2);


};

__declspec(dllexport) CBPMatrix operator - (double nValue,const CBPMatrix& cMatrixB);
__declspec(dllexport) CBPMatrix MergeMatrix(const CBPMatrix& cMatrixA,const CBPMatrix& cMatrixB);

/////////////////////////////////////////////////////////////////////////////
// overload operator 'double - CBPMatrix'



/////////////////////////////////////////////////////////////////////////
// ����ϲ������
//	�ϲ�����:
//		1. ����ϲ��������������������������;
//		2. ����ϲ�������������������Բ����;
//		3. �ϲ��󷵻صľ�������������ϲ��ľ�����������,�����ǲ���ϲ���
//			�������������֮��;
//




#endif // _MATRIX_H