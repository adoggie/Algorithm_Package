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

	TMatrix	m_pTMatrix;				// 指向矩阵的头指针

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
	//		[out]	cMatrix ----> 矩阵的子矩阵返回的结果
	//		[in]	nStartX ----> 子矩阵在矩阵中的起始坐标,对应行,索引从1开始
	//		[in]	nStartY ----> 子矩阵在矩阵中的起始坐标,对应列,索引从1开始
	//
	void CopySubMatrix(CBPMatrix& cMatrix,unsigned int nStartX,unsigned int nStartY);

	/////////////////////////////////////////////////////////////////////////
	// Copy Matrix
	//	Parameter:
	//		[in]	cMatrix ----> be copied matrix
	//
	void CopyMatrix(const CBPMatrix& cMatrix);

	/////////////////////////////////////////////////////////////////////////
	// 将矩阵的所有的元素按列合成一列
	//	例如：
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
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = f(x) = 1 / (1 + exp(-x))	( 0 < f(x) < 1)
	//
	CBPMatrix Sigmoid();

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = f'(x) = (1 / (1 + exp(-x)))'	( 0 < f(x) < 1)
	//			  = exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
	//
	CBPMatrix SigmoidDerivative();


	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = tanh(x) = (1 - exp(-x)) / (1 + exp(-x))
	//					 =  1 - 2 * exp(-x) / (1 + exp(-x))	( -1 < f(x) < 1)
	//
	CBPMatrix tanh(); 

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = tanh'(x) = ((1 - exp(-x)) / (1 + exp(-x)))'	( -1 < f(x) < 1)
	//					 = 	2*exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
	//
	CBPMatrix tanhDerivative();

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = Tansig(x) = 2 / (1 + exp(-2 * x)) -1
	//
	CBPMatrix Tansig();

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中所有的元素进行一次非线性变换:
	//		变换后的值y与变换前的值的关系是:
	//			y = Tansig'(x) = (2 / (1 + exp(-2 * x)) -1)'
	//				= (2 / (1 + exp(-2 * x)) -1) * (2 / (1 + exp(-2 * x)) -1) -1
	//
	CBPMatrix TansigDerivative();

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中的元素进行一次操作:
	//		使所有行中的相对应的列元素相等
	//	Parameter:
	//		nRowIndex	---->	行索引值(the index starts from 0)
	//							以此行做为标准,使矩阵中其余的行相对应的列的值
	//							与此行相对应的列的值相等	
	//
	void MakeAllColumnElementsSameValue(unsigned int nRowIndex);

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵中的元素进行一次操作:
	//		使矩阵变为单位阵
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
	//		CBPMatrix& cMatrix:		被拷贝的数据源
	//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
	// Purpose:
	//		This function will copy all the data of the cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void GetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		被填充的矩阵
	//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
	// Purpose:
	//		This function will copy part of the object data into cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void SetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		被拷贝的数据源
	//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
	//		unsigned int nRow:		被拷贝的数据在被拷贝对象中的行索引(从0开始)
	// Purpose:
	//		This function will copy all the data of the cMatrix
	// Notes:
	//		The object must be column vector!!!
	//
	void GetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow);

	/////////////////////////////////////////////////////////////////////////
	// Parameter:
	//		CBPMatrix& cMatrix:		被填充的矩阵
	//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
	//		unsigned int nRow:		被填充的数据在被填充对象中的行索引
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
	// 对矩阵进行拓展
	//	实现功能:
	//		对矩阵的列数进行拓展,nTimes是每列拓展的次数
	//
	void nncpyi(const CBPMatrix &cMatrix, unsigned int nTimes);

	/////////////////////////////////////////////////////////////////////////
	// 对矩阵进行拓展
	//	实现功能:
	//		对矩阵的列数进行拓展
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
	// 对矩阵进行拓展
	//	实现功能:
	//		对矩阵的列数进行拓展,nTimes是每列拓展的次数
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

	unsigned int m_nRow;			// 矩阵所拥有的行数
	unsigned int m_nCol;			// 矩阵所拥有的列数


	/////////////////////////////////////////////////////////////////////////
	// 注意:
	//		在重新设置矩阵的行数和列数后,矩阵中的元素被重新初始化为0
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// 设置矩阵的行数
	//
	void SetMatrixRowNumber(unsigned int nRow,bool bNeedZero = true);

	/////////////////////////////////////////////////////////////////////////
	// 设置矩阵的列数
	//
	void SetMatrixColNumber(unsigned int nCol,bool bNeedZero = true);

	/////////////////////////////////////////////////////////////////////////
	// 设置矩阵的行列数
	//
	void SetMatrixRowAndCol(unsigned int nRow,unsigned int nCol);


	/////////////////////////////////////////////////////////////////////////
	// 交换矩阵的两行
	//
	void SwapMatrixRow(unsigned int nRow1,unsigned int nRow2);

	/////////////////////////////////////////////////////////////////////////
	// 交换矩阵的两列
	//
	void SwapMatrixCol(unsigned int nCol1,unsigned int nCol2);


};

__declspec(dllexport) CBPMatrix operator - (double nValue,const CBPMatrix& cMatrixB);
__declspec(dllexport) CBPMatrix MergeMatrix(const CBPMatrix& cMatrixA,const CBPMatrix& cMatrixB);

/////////////////////////////////////////////////////////////////////////////
// overload operator 'double - CBPMatrix'



/////////////////////////////////////////////////////////////////////////
// 矩阵合并运算符
//	合并规则:
//		1. 参与合并运算的两个矩阵的行数必须相等;
//		2. 参与合并的两个矩阵的列数可以不相等;
//		3. 合并后返回的矩阵的行数与参与合并的矩阵的行数相等,列数是参与合并的
//			两个矩阵的列数之和;
//




#endif // _MATRIX_H