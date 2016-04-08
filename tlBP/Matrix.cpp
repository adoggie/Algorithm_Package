/////////////////////////////////////////////////////////////////////////////
// Matrix.cpp : Implementation of the class Matrix
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Matrix.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "../Public/TCLFile.h"
#ifndef _UNIX
#include <conio.h>
#endif
#include <stdio.h>

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBPMatrix::CBPMatrix()
{
	m_nRow = 0;
	m_nCol = 0;

	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}
}


CBPMatrix::~CBPMatrix()
{	

}

//define matrix and vector

CBPMatrix::CBPMatrix(unsigned int nRow,unsigned int nCol)
{
	// 动态分配二维数组
	//TMatrix tMatrix;
	m_pTMatrix.resize (nRow);

	for(unsigned int i=0; i < nRow; i++)
	{
		m_pTMatrix[i].resize(nCol);//zlq 移出来
		memset(&m_pTMatrix[i][0],0,sizeof(double)*nCol);
	}

	// 对对象变量赋值
	m_nRow	= nRow;
	m_nCol	= nCol;
	//m_pTMatrix = tMatrix;

}


CBPMatrix::CBPMatrix(const CBPMatrix& cMatrixB)
{
	// Initialize the variable
	m_nRow = cMatrixB.m_nRow ;
	m_nCol = cMatrixB.m_nCol ;
	m_pTMatrix = cMatrixB.m_pTMatrix ;//复制数据
 
	
}


/////////////////////////////////////////////////////////////////////////////
// CBPMatrix member functions
//

CBPMatrix CBPMatrix::operator +(const CBPMatrix& cMatrixB)
{
	// 要满足矩阵相加的条件: 行列数目相等!
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
// 		::AfxMessageBox (("执行相加的两个矩阵维数不相等!"),MB_OK | MB_ICONERROR);
	}

	CBPMatrix	cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [i][j] + cMatrixB.m_pTMatrix [i][j];
		}
	}

	return	cMatrix;

}


CBPMatrix CBPMatrix::operator -(const CBPMatrix& cMatrixB)
{
	// 要满足矩阵相加的条件: 行列数数目相等!
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
// 		::AfxMessageBox (("执行相减的两个矩阵维数不相等!"),MB_OK | MB_ICONERROR);
	}

	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [i][j] - cMatrixB.m_pTMatrix [i][j];
		}
	}

	return	cMatrix;

}


CBPMatrix CBPMatrix::operator *(const CBPMatrix& cMatrixB)
{
	if( m_nCol != cMatrixB.m_nRow )
	{
// 		::AfxMessageBox (("执行相乘的两个矩阵维数不满足相乘的条件!"),MB_OK | MB_ICONERROR);
	}
	
	CBPMatrix cResultMatrix(m_nRow,cMatrixB.m_nCol);

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrixB.m_nCol; j++)
		{
			for(unsigned int m=0; m < m_nCol; m++)
			{
				cResultMatrix.m_pTMatrix [i][j] +=  m_pTMatrix [i][m] * cMatrixB.m_pTMatrix [m][j];
			}
		}
	}

	return cResultMatrix;
}


CBPMatrix CBPMatrix::operator * (const double nValue)
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] =m_pTMatrix [i][j] * nValue;
		}
	}

	return cMatrix;
}


CBPMatrix& CBPMatrix::operator =(const CBPMatrix& cMatrixB)
{
	if( (m_nRow != cMatrixB.m_nRow) || (m_nCol != cMatrixB.m_nCol) )
	{
// 		::AfxMessageBox(("等号左右两边的矩阵的维数不相等!"),MB_OK | MB_ICONERROR);
		return *this;	// return invalid value
	}	

	// 给变量赋值
	m_nRow = cMatrixB.m_nRow ;
	m_nCol = cMatrixB.m_nCol ;
	m_pTMatrix = cMatrixB.m_pTMatrix ;// 赋值操作

 
	
	return *this;
}


CBPMatrix& CBPMatrix::operator += (const CBPMatrix& cMatrixB)
{
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
		//printf("错误!执行相加的两个矩阵维数不相等!\n");
// 		::AfxMessageBox (("运算符的两边矩阵的维数不相等!"),MB_OK | MB_ICONERROR);
		return *this;	// return invalid value
	}
	
	// 赋值操作
	for(unsigned int i=0; i < cMatrixB.m_nRow; i++)
	{
		for(unsigned int j=0; j< cMatrixB.m_nCol; j++)
		{
			m_pTMatrix [i][j] += cMatrixB.m_pTMatrix [i][j];
		}
	}
	
	return *this;

}


CBPMatrix CBPMatrix::Transpose()
{
	CBPMatrix cMatrix(m_nCol,m_nRow);

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [j][i] = m_pTMatrix [i][j];
		}
	}

	return cMatrix;
}

/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::MergeColumnsToColumnVector()
{
	CBPMatrix cMatrix(m_nRow * m_nCol,(unsigned int)1);

	// 对矩阵赋值
	for(unsigned int j=0; j < m_nCol; j++)
	{
		for(unsigned int i=0; i < m_nRow; i++)
		{
			cMatrix.m_pTMatrix [i + j * m_nRow][(unsigned int)0] = m_pTMatrix [i][j];
		}
	}

	return cMatrix;

}

/////////////////////////////////////////////////////////////////////////////
// Get the total value of the matrix
/////////////////////////////////////////////////////////////////////////////

double CBPMatrix::GetTotalElementValue()
{
	double	nTotalValue = 0;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for( unsigned int j=0; j < m_nCol; j++)
		{
			nTotalValue += m_pTMatrix [i][j];
		}
	}

	return nTotalValue;
}

/////////////////////////////////////////////////////////////////////////////
// Get System Error
/////////////////////////////////////////////////////////////////////////////

double	CBPMatrix::GetSystemError() const
{
	double	nSystemError = 0;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for( unsigned int j=0; j < m_nCol; j++)
		{
			nSystemError += m_pTMatrix [i][j] * m_pTMatrix [i][j];
		}
	}

	return nSystemError;

}

/////////////////////////////////////////////////////////////////////////////
// Make all the matrix elements to be changed into absolute value
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::AbsoluteValue ()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = fabs( m_pTMatrix [i][j]);
		}
	}

	return cMatrix;

}


CBPMatrix CBPMatrix::Inverse()
{
	/////////////////////////////////////////////////////////////////////////
	// Using Gauss - Jordan Method
	// 参考书目: 计算机数值方法 --->施吉林 陈桂枝
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// 判断是否是可逆阵:
	//		可逆阵一定是方阵!!!

	if ( m_nRow != m_nCol)
	{
		//printf("错误!矩阵的行列数不相等,是非可逆阵!\n");
// 		::AfxMessageBox (("矩阵的行列数不相等,是非可逆阵!"),MB_OK | MB_ICONERROR);
	}

	CBPMatrix cMatrix = *this;

	//***********************************************************************
	// 思路:(非常规思维!)
	//		动态分配整型数组(2*m_nCol)来存储每次交换的行坐标的值
	//		不论有没有行交换都记录在数组中,
	//		1.没进行行交换的两个数据相等,在SwapMatrixRow()函数中
	//		检测到两个值相等立即返回,在SwapMatrixCol()函数中也一样,
	//		检测到两个值相等立即返回,不占用系统资源;
	//		2.不相等的就交换
	//***********************************************************************
    
	//	分配内存
	int *pIntArray = new int [2*m_nCol];

	// nSetp --- 约化步数,按列展开	
	for(unsigned int k=0; k < cMatrix.m_nCol; k++)
	{
		/////////////////////////////////////////////////////////////////////
		// 进行行交换 ---> 游戏规则:
		// 为保证计算过程的数值稳定性,在第k步约化时,先在{a(ik)}|i=k->n中选按
		// 模最大者作为约化主元素,并交换矩阵相应的行

		// 标记主元素
		double nMaxElement = cMatrix.m_pTMatrix [k][k];
		// 标记主元素所在的行数
		unsigned int nMainRow = k;

		for(unsigned int nCount = k+1; nCount < cMatrix.m_nCol; nCount++)
		{
			if( fabs(nMaxElement) < fabs(cMatrix.m_pTMatrix [nCount][k]) )
			{
				nMaxElement = cMatrix.m_pTMatrix [nCount][k];
				nMainRow = nCount;
			}
		}

		// 将欲交换的行数存在数组中
		pIntArray [2*k] = k;
		pIntArray [2*k+1] = nMainRow;
	

		// 交换行
		cMatrix.SwapMatrixRow(k,nMainRow);

		//Display();

		//	判断是否是可逆阵
		if(cMatrix.m_pTMatrix [k][k] == 0)
		{
			//printf("错误!此矩阵为非可逆阵!\n");
// 			::AfxMessageBox (("此矩阵为非可逆阵,没有逆矩阵!"),MB_OK | MB_ICONERROR);
		}

		cMatrix.m_pTMatrix [k][k] = 1/(cMatrix.m_pTMatrix [k][k]);
		

		// 算主列
		for(unsigned int i=0; i < cMatrix.m_nRow; i++)
		{	
			if( i != k)
				cMatrix.m_pTMatrix [i][k] = -(cMatrix.m_pTMatrix [k][k]) * (cMatrix.m_pTMatrix [i][k]); 
			
			//int nTempValue = m_pTMatrix [i][k];
			
		}
		
		//printf("\n");

		// 约化非主行
		for(unsigned int m=0; m < cMatrix.m_nRow; m++)
		{
			if ( m == k)
				continue;

			for(unsigned int n=0; n < cMatrix.m_nCol; n++)
			{
				if ( n == k)
					continue;

				cMatrix.m_pTMatrix [m][n] += cMatrix.m_pTMatrix [m][k] * cMatrix.m_pTMatrix [k][n];

				//printf("%10f ",m_pTMatrix [m][n]);

			}

			//printf("\n");

		}

		// 算主行
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			if( j != k)
				cMatrix.m_pTMatrix [k][j] = (cMatrix.m_pTMatrix [k][k]) * (cMatrix.m_pTMatrix [k][j]);

		}

	}

	
	/////////////////////////////////////////////////////////////////////////
	// 进行列交换 ---> 对交换行后的矩阵进行列交换 ---> 还原矩阵
	// 游戏规则:
	// 将开始矩阵中进行的行交换 ---> 现用相对应的列交换进行还原,即可得到所求的
	// 逆矩阵

	for(int i=2*m_nCol-1; i > 0; i--)
	{
		cMatrix.SwapMatrixCol(pIntArray[i],pIntArray[i-1]);
		i--;
	}

	delete []pIntArray;

	return cMatrix;

}


void CBPMatrix::SwapMatrixRow(unsigned int nRow1,unsigned int nRow2)
{
	if( nRow1 == nRow2)
		return;

	double *pArray = new double;

	for(unsigned int i=0; i < m_nCol; i++)
	{
		// Swap the datum of the two rows
		pArray[0] = m_pTMatrix [nRow1][i];
		m_pTMatrix [nRow1][i] = m_pTMatrix [nRow2][i];
		m_pTMatrix [nRow2][i] = pArray[0];
	}

	delete pArray;
}


void CBPMatrix::SwapMatrixCol(unsigned int nCol1,unsigned int nCol2)
{
	if( nCol1 == nCol2)
		return;

	double *pArray = new double;
	for(unsigned int i=0; i < m_nRow; i++)
	{
		// Swap the datum of the two columns
		pArray[0] = m_pTMatrix [i][nCol1];
		m_pTMatrix [i][nCol1] = m_pTMatrix [i][nCol2];
		m_pTMatrix [i][nCol2] = pArray[0];
	}
	
	delete pArray;
}


bool CBPMatrix::LoadDataFromFile(CTString& strFileName)
{
	CMemoStream dataFile;
	LPCTSTR	lpszFileName = "";

	// CTString convert to LPCTSTR
	strFileName.TrimLeft ();
	strFileName.TrimRight ();
	//strFileName.Format (lpszFileName);

	lpszFileName = (LPCTSTR)strFileName;

	// 用来存储提取文本文件中一行的数据
	CTString	strData;				 
	// 用来记录文本文件中一共有多少行数据?
	unsigned int	nRow = 0;		

	/////////////////////////////////////////////////////////////////////////
	// Step 1: 得到文件的行列数目并根据文本文件的行列数目来设置对象(矩阵)的行
	// 列数目
	//

	while(dataFile.ReadString(strData) != false)
	{
		++nRow;
	}

	// 根据文本文件的数据的行数设置对象(矩阵)的行数
	m_nRow = nRow;
	SetMatrixRowNumber(m_nRow);

	// 重新定位当前文件指针到文件开头
	dataFile.Seek(0);

	dataFile.ReadString (strData);
	strData.TrimLeft ();
	strData.TrimRight ();

	char	SPACE_CHARACTER = ' ';
	// 用来记录文本文件中一行有多少列?
	unsigned int	nCol = 0;						
	
	// 空格符在字符串中的位置索引
	int nIndex = 0;

	do
	{
		nIndex = strData.Find (SPACE_CHARACTER);

		++nCol;

		strData = strData.Right (strData.GetLength () - nIndex -1);

		// 去掉多余的空格
		strData.TrimLeft ();
	}while(nIndex != -1);

	// 根据文本文件的数据的列数设置对象(矩阵)的列数
	m_nCol = nCol;
	SetMatrixColNumber(m_nCol);

	// End of Getting the Rows and Cols of the Text File
	/////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////
	// Step 2: 根据文本文件中的数据对矩阵赋值,并检测每行的列数是否和第一行的
	// 列数相等,不相等提示出错信息
	//

	// 重新定位当前文件指针到文件开头
	dataFile.Seek(0);

	// 对矩阵中的元素装入文本文件的数据
	for(unsigned int i=0; i < m_nRow; i++)
	{
		dataFile.ReadString (strData);
		strData.TrimLeft ();
		strData.TrimRight ();

		// 验证每行的列数是否与第一行的列数相等
		unsigned int nVerifyColNum = 0;

		do
		{
			nIndex = strData.Find (SPACE_CHARACTER);

			++nVerifyColNum;

			if(nIndex != -1)
			{
				// 提取字符串的子字符串,即提取一个double型实数数据
				CTString strDoubleNumber = strData.Left (nIndex);	

				// 将字符串转换为double型实数
				double RealNumber = atof(strDoubleNumber);

				m_pTMatrix [i][nVerifyColNum - 1] = RealNumber;

				strData = strData.Right (strData.GetLength () - nIndex -1);

				// 去掉多余的空格
				strData.TrimLeft ();
			}
			else
			{
				double RealNumber = atof(strData);

				m_pTMatrix [i][nVerifyColNum - 1] = RealNumber;
			}

		}while(nIndex != -1);

		if(nVerifyColNum != m_nCol)
		{
			CTString strRowNumber;
			strRowNumber.Format("%d",i + 1);

			CTString strColNumber;
			strColNumber.Format("%d",m_nCol);

			CTString strVerifyColNumber;
			strVerifyColNumber.Format("%d",nVerifyColNum);

			CTString strMessage =  CTString(("文本文件第")) + strRowNumber + CTString(("行一共有")) + strVerifyColNumber + CTString(("列,与第一行中的列数")) + strColNumber + CTString(("不相等!"));

			LPCTSTR lpszText = "";
			lpszText = (LPCTSTR)strMessage;

			//strMessage.FormatMessage (lpszText);
			//::AfxMessageBox (lpszText,MB_OK);


// 			::AfxMessageBox (lpszText,MB_OK | MB_ICONERROR);
			return false;
		}


	}


	return true;
}




bool CBPMatrix::SaveDataToArray (CTStringArray& strArray)
{


	// 将对象(矩阵)中的数据写进文件
	for(unsigned int i=0; i < m_nRow; i++)
	{
		CTString strItem = "";
		for(unsigned int j=0; j < m_nCol; j++)
		{
			 CTString strRealNumber;

			 strRealNumber.Format ("%.16f  ", m_pTMatrix [i][j]);

			 strItem = strItem + strRealNumber;
		}
		
		if( i != m_nRow - 1)
		{
			strItem = strItem + "\n";
		}
		strArray.Add(strItem);

	}

	return true;
}

bool CBPMatrix::SaveDataToFile (CTString strFileName)
{
	TCLFile dataFile;
	LPCTSTR	lpszFileName = "";

	// CTString convert to LPCTSTR
	strFileName.TrimLeft ();
	strFileName.TrimRight ();

	lpszFileName = (LPCTSTR)strFileName;

	if(!dataFile.Open (lpszFileName,TCLFile::modeCreate | TCLFile::modeReadWrite))
	{
// 		::AfxMessageBox(("不能创建文件!"),MB_OK | MB_ICONERROR);
		dataFile.Close ();
		return false;
	}
	
	dataFile.SeekToEnd ();

	// 将对象(矩阵)中的数据写进文件
	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			 CTString strRealNumber;

			 strRealNumber.Format ("%.16f  ", m_pTMatrix [i][j]);

			 // Using for debugging
			 //double nReadNumber = m_pTMatrix [i][j];
			 dataFile.Write ((const char*)strRealNumber,strRealNumber.GetLength ());
		}
		
		if( i != m_nRow - 1)
		{
			//char ReturnNewline[] = "\r\n";
			char ReturnNewline[] = "\n";
			
			dataFile.Write (ReturnNewline, (sizeof(ReturnNewline) - 1)/sizeof(char));
		}

	}


	dataFile.Close ();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵中的元素进行一次操作:
//		使矩阵变为单位阵
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::Eye()
{
	// Verify whether the rows is equal to the columns or not
	if(m_nRow != m_nCol)
	{
// 		::AfxMessageBox (("此矩阵的行列数不相等!不能转变为单位阵!"),MB_OK | MB_ICONERROR);
		return;
	}
	//===uc 需要优化
	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			if(i == j)
			{
				m_pTMatrix [i][j] =	1;
			}
			else
			{
				m_pTMatrix [i][j] =	0;
			}
		}

	}


}


/////////////////////////////////////////////////////////////////////////////
// Parameter:
//		CBPMatrix& cMatrix:		被拷贝的数据源
//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置,从0开始
// Purpose:
//		This function will copy all the data of the cMatrix
// Notes:
//		此对象必须是列向量!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::GetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("拷贝的矩阵不是列向量!"),MB_OK | MB_ICONERROR);
		return;
	}

	if((m_nRow - nIndex) < (cMatrix.m_nRow * cMatrix.m_nCol))
	{
// 		::AfxMessageBox (("拷贝矩阵的空间容量不足!"),MB_OK | MB_ICONERROR);
		return;
	}

	for(unsigned int i=0; i < cMatrix.m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			m_pTMatrix [nIndex + i * cMatrix.m_nCol + j][0] = cMatrix.m_pTMatrix [i][j];
		}

	}

}


/////////////////////////////////////////////////////////////////////////////
// Parameter:
//		CBPMatrix& cMatrix:		被填充的矩阵
//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
// Purpose:
//		This function will copy part of the object data into cMatrix
// Notes:
//		The object must be column vector!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::SetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex)
{
	// Verify whether the colunm number is 1
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("本矩阵对象不是列向量,不满足条件!"),MB_OK | MB_ICONERROR);
		return;
	}

	// Verify whether the number of the object element is enough to be copyed
	if((m_nRow - nIndex) < (cMatrix.m_nRow * cMatrix.m_nCol))
	{
// 		::AfxMessageBox (("对象中的元素数量不足!"),MB_OK | MB_ICONERROR);
		return;
	}


	for(unsigned int i=0; i < cMatrix.m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [nIndex + i * cMatrix.m_nCol + j][0];
		}
	}

}


/////////////////////////////////////////////////////////////////////////////
// Parameter:
//		CBPMatrix& cMatrix:		被填充的矩阵
//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
//		unsigned int nRow:		被填充的数据在被填充对象中的行索引
// Purpose:
//		This function will copy part of the object data to fill the special 
// row of the cMatrix
//	Notes:
//		The object must be column vector!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::SetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow)
{
	// Verify whether the column number is 1
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("本矩阵对象不是列向量,不满足条件!"),MB_OK | MB_ICONERROR);
		return;
	}

	// Verify whether the number of the object element is enough to be copyed
	if((m_nRow - nIndex) < cMatrix.m_nCol )
	{
// 		::AfxMessageBox (("对象的元素数量不足!"),MB_OK | MB_ICONERROR);
		return;
	}

	for(unsigned int i=0; i < cMatrix.m_nCol; i++)
	{
		cMatrix.m_pTMatrix [nRow][i] = m_pTMatrix [nIndex + i][(unsigned int)0];
	}

}


/////////////////////////////////////////////////////////////////////////////
// Parameter:
//		CBPMatrix& cMatrix:		被拷贝的数据源
//		unsigned int nIndex:	被拷贝的数据在对象中的开始索引位置
//		unsigned int nRow:		被拷贝的数据在被拷贝对象中的行索引(从0开始)
// Purpose:
//		This function will copy all the data of the cMatrix
//	Notes:
//		此对象必须是列向量!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::GetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("拷贝的矩阵不是列向量!"),MB_OK | MB_ICONERROR);
		return;
	}

	if((m_nRow - nIndex) < cMatrix.m_nCol)
	{
// 		::AfxMessageBox (("拷贝矩阵的空间容量不足!"),MB_OK | MB_ICONERROR);
		return;
	}
	
	for(unsigned int i=0; i < cMatrix.m_nCol; i++)
	{
		m_pTMatrix [nIndex + i][(unsigned int)0] = cMatrix.m_pTMatrix [nRow][i];
	}

}
/*
*重新设置矩阵行数，数据并清零
*/
void CBPMatrix::SetMatrixRowNumber(unsigned int nRow,bool bNeedZero /*= true*/)
{
	m_nRow = nRow;

	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		if (bNeedZero)
		{
			memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
		}
	}

}

/*
*重新设置矩阵列数，数据并清零
* bNeedZero 是否需要清零 默认清零
*/
void CBPMatrix::SetMatrixColNumber(unsigned int nCol,bool bNeedZero /*= true*/)
{
	m_nCol = nCol;

	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		if (bNeedZero)
		{
			memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
		}
	}

}

/////////////////////////////////////////////////////////////////////////
// 设置矩阵的行列数
void CBPMatrix::SetMatrixRowAndCol(unsigned int nRow,unsigned int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;

	// 分配内存
	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

}


/////////////////////////////////////////////////////////////////////////////
// Initialize()
// 矩阵初始化函数,矩阵的行列数目被初始化为零,矩阵中的元素全部初始化为零
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::Initialize()
{
	m_nRow = 0;
	m_nCol = 0;

	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

}

/////////////////////////////////////////////////////////////////////////////
// InitializeZero()
// 矩阵初始化函数,矩阵的行列数目被初始化为零,矩阵中的元素全部初始化为零
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::InitializeZero()
{
	m_nRow = 0;
	m_nCol = 0;

	m_pTMatrix.resize (m_nRow);

}

/////////////////////////////////////////////////////////////////////////////
// RandomInitialize()
// 将矩阵中的元素随机初始化函数,元素的值在(-1,1)之间的小数
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::RandomInitialize ()
{
	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			m_pTMatrix [i][j] = (double)(rand() - (0.5*RAND_MAX)) / (0.5*RAND_MAX);
		}
	}
	
}


/////////////////////////////////////////////////////////////////////////////
// 拷贝矩阵的子矩阵元素到另外一个矩阵中
// Parameter:
//		[out]	cMatrix ----> 矩阵的子矩阵返回的结果
//		[in]	nStartX ----> 子矩阵在矩阵中的起始坐标,对应行,索引从1开始
//		[in]	nStartY ----> 子矩阵在矩阵中的起始坐标,对应列,索引从1开始
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopySubMatrix(CBPMatrix& cMatrix,unsigned int nStartX,unsigned int nStartY)
{
	if((m_nRow  < cMatrix.m_nRow + nStartY ) | (m_nCol  < cMatrix.m_nCol + nStartX))
	{
// 		::AfxMessageBox (("被拷贝的矩阵维数小于要拷贝的矩阵所需要的维数!"),MB_OK | MB_ICONERROR);
		return;
	}

	for(unsigned int i=0;  i < cMatrix.m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [nStartY + i][nStartX + j];	
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// Copy Matrix
//	Parameter:
//		[in]	cMatrix ----> 被拷贝的矩阵
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopyMatrix(const CBPMatrix& cMatrix)
{
	InitializeZero();
	m_nRow	= cMatrix.m_nRow ;
	m_nCol	= cMatrix.m_nCol ;
	
	m_pTMatrix	= cMatrix.m_pTMatrix ;
	//===uc  以下似乎没有用
	/*
	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			//TRACE("old %f\r\n",m_pTMatrix [i][j]);
			double value = m_pTMatrix [i][j];
			m_pTMatrix [i][j] = cMatrix.m_pTMatrix [i][j];
			//TRACE("new %f\r\n",m_pTMatrix [i][j]);
			assert(value == m_pTMatrix [i][j]);
		}

	}*/

}

/////////////////////////////////////////////////////////////////////////////
//	从一个列向量中拷贝数据到一个矩阵中
//	Parameter:
//		[out]	cMatrix ----> 函数的返回结果
//		[in]	nIndex	----> 在列向量中的索引值
//	Notes:
//		被拷贝的对象必须是列向量!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopySubMatrixFromVector(CBPMatrix& cMatrix,unsigned int nIndex)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("被拷贝的矩阵不是列向量!!!"),MB_OK | MB_ICONERROR);
		return;
	}

	for(unsigned int j=0; j < cMatrix.m_nCol; j++)
	{
		for(unsigned int i=0; i < cMatrix.m_nRow; i++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [nIndex + j * cMatrix.m_nRow + i ][(unsigned int)0];
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵进列拓展
//	实现功能:
//		对矩阵的列数进行拓展,nTimes是每列拓展的次数
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpyi(const CBPMatrix &cMatrix, unsigned int nTimes)
{
	m_nRow	=	cMatrix.m_nRow ;
	m_nCol	=	cMatrix.m_nCol *	nTimes;

	// 根据空间分配内存
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);

	}
	//cMatrix.Print();
	// 赋值
	for(i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			for(unsigned int k=0; k < nTimes; k++)
			{
				m_pTMatrix [i][j * nTimes + k] = cMatrix.m_pTMatrix [i][j];
			}
		}
	}
	//this->Print();

}

/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpyd(const  CBPMatrix &cMatrix)
{
	m_nRow	=	cMatrix.m_nRow ;
	m_nCol	=	cMatrix.m_nCol * cMatrix.m_nRow ;

	// 根据空间分配内存
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

	// 给矩阵赋值
	for(i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			for(unsigned int k=0; k < cMatrix.m_nRow; k++)
			{
				if(i == (j * cMatrix.m_nRow + k) % cMatrix.m_nRow )
					m_pTMatrix [i][j * cMatrix.m_nRow + k] = cMatrix.m_pTMatrix [i][j];
			}
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpy(const CBPMatrix& cMatrix,unsigned int nTimes)
{
	m_nRow = cMatrix.m_nRow ;
	m_nCol = cMatrix.m_nCol * nTimes;

	// 根据空间分配内存
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

	// 对矩阵赋值
	for(i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < nTimes; j++)
		{
			for(unsigned int k=0; k < cMatrix.m_nCol; k++)
			{
				m_pTMatrix [i][j * cMatrix.m_nCol + k] = cMatrix.m_pTMatrix [i][k];
			}
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = f(x) = 1 / (1 + exp(-x))	( 0 < f(x) < 1)
//
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::Sigmoid()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = 1 / (1 + exp(-m_pTMatrix [i][j]));
		}

	}
	
	return cMatrix;
}


/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = tanh(x) = (1 - exp(-x)) / (1 + exp(-x))
//					 =  1 - 2 * exp(-x) / (1 + exp(-x))	( -1 < f(x) < 1)
//
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::tanh ()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = 1 - (2 * exp(-m_pTMatrix [i][j])) / (1 + exp(-m_pTMatrix [i][j]));
		}

	}
	
	return cMatrix;
}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = Tansig(x) = 2 / (1 + exp(-2 * x)) -1
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::Tansig()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = 2 / (1 + exp(- 2 * m_pTMatrix [i][j])) - 1;
		}
	}
	
	return cMatrix;

}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = Tansig'(x) = (2 / (1 + exp(-2 * x)) -1)'
//				= (2 / (1 + exp(-2 * x)) -1) * (2 / (1 + exp(-2 * x)) -1) -1
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::TansigDerivative()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = (2 / (1 + exp(- 2 * m_pTMatrix [i][j])) - 1) * (2 / (1 + exp(- 2 * m_pTMatrix [i][j])) - 1) - 1;
		}
	}

	return cMatrix;

}

/////////////////////////////////////////////////////////////////////////////
// 对矩阵中的元素进行一次操作:
//		使所有行中的相对应的列元素相等
//	Parameter:
//		nRowIndex	---->	行索引值(从零开始)
//							以此行做为标准,使矩阵中其余的行相对应的列的值
//							与此行相对应的列的值相等
/////////////////////////////////////////////////////////////////////////////
	
void CBPMatrix::MakeAllColumnElementsSameValue(unsigned int nRowIndex)
{
	for(unsigned int i=0; i < m_nRow; i++)
	{
		if(i == nRowIndex)
			continue;

		for(unsigned int j=0; j < m_nCol; j++)
		{
			m_pTMatrix [i][j] = m_pTMatrix [nRowIndex][j];
		}
	}

}


/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = f'(x) = (1 / (1 + exp(-x)))'	( 0 < f(x) < 1)
//			  = exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::SigmoidDerivative()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = exp(-m_pTMatrix [i][j]) / ((1 + exp(-m_pTMatrix [i][j])) * (1 + exp(-m_pTMatrix [i][j])));
		}

	}
	
	return cMatrix;
}


/////////////////////////////////////////////////////////////////////////////
// 对矩阵中所有的元素进行一次非线性变换:
//		变换后的值y与变换前的值的关系是:
//			y = tanh'(x) = ((1 - exp(-x)) / (1 + exp(-x)))'	( -1 < f(x) < 1)
//					 = 	2*exp(-x)/((1 + exp(-x))*(1 + exp(-x)))
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::tanhDerivative()
{
	CBPMatrix cMatrix = *this;

	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = 2 * exp(-m_pTMatrix [i][j]) / ((1 + exp(-m_pTMatrix [i][j])) * (1 + exp(-m_pTMatrix [i][j])));
		}

	}

	return cMatrix;
}


/////////////////////////////////////////////////////////////////////////////
// 实现对点乘操作符的重载
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::operator / (const CBPMatrix& cMatrixB)
{
	CBPMatrix cMatrix = *this;

	if( (m_nRow != cMatrixB.m_nRow) || (m_nCol != cMatrixB.m_nCol) )
	{
// 		::AfxMessageBox (("两个矩阵的维数不相等,不满足矩阵点乘的条件!"),MB_OK | MB_ICONERROR);
		return cMatrix;	// return a invalid value
	}
	
	for(unsigned int i=0; i < m_nRow; i++)
	{
		for(unsigned int j=0; j < m_nCol; j++)
		{
			cMatrix.m_pTMatrix [i][j] = m_pTMatrix [i][j] * cMatrixB.m_pTMatrix [i][j];
		}

	}

	return cMatrix;

}



//***************************************************************************
// ordinary function
//

/////////////////////////////////////////////////////////////////////////////
// 重载 'double - CBPMatrix' 运算符
/////////////////////////////////////////////////////////////////////////////

CBPMatrix operator - (double nValue,const CBPMatrix& cMatrixB)
{
	CBPMatrix	cMatrix = cMatrixB;

	for(unsigned int i=0; i < cMatrix.GetMatrixRowNumber (); i++)
	{
		for(unsigned int j=0; j < cMatrix.GetMatrixColNumber (); j++)
		{
			cMatrix.m_pTMatrix [i][j] = nValue - cMatrixB.m_pTMatrix [i][j];
		}
	}

	return cMatrix;
}



/////////////////////////////////////////////////////////////////////////////
// 矩阵合并运算符
//	合并规则:
//		1. 参与合并运算的两个矩阵的行数必须相等;
//		2. 参与合并的两个矩阵的列数可以不相等;
//		3. 合并后返回的矩阵的行数与参与合并的矩阵的行数相等,列数是参与合并的
//			两个矩阵的列数之和;
/////////////////////////////////////////////////////////////////////////////

CBPMatrix MergeMatrix(const CBPMatrix& cMatrixA,const CBPMatrix& cMatrixB)
{
	//	条件检测
	if( cMatrixA.GetMatrixRowNumber () != cMatrixB.GetMatrixRowNumber () )
	{
// 		::AfxMessageBox (("参与合并的两个矩阵的行数不相等!"),MB_OK | MB_ICONERROR);
	
		return cMatrixA;	// return invalid value
	}

	CBPMatrix cMatrix(cMatrixA.GetMatrixRowNumber (),cMatrixA.GetMatrixColNumber () + cMatrixB.GetMatrixColNumber ());

	for(unsigned int i=0; i < cMatrixA.GetMatrixRowNumber (); i++)
	{
		for(unsigned int j=0; j < cMatrixA.GetMatrixColNumber (); j++)
		{
			cMatrix.m_pTMatrix [i][j] = cMatrixA.m_pTMatrix [i][j];
		}

		for(unsigned int k=0; k < cMatrixB.GetMatrixColNumber (); k++)
		{
			cMatrix.m_pTMatrix [i][cMatrixA.GetMatrixColNumber () + k] = cMatrixB.m_pTMatrix [i][k];
		}

	}


	return cMatrix;
}

// End of ordinary function
//***************************************************************************

void CBPMatrix::Print()
{
// 	TRACE("************  打印矩阵 start  ************\r\n");
// 	TRACE("Row:%d    Col:%d\r\n",GetMatrixRowNumber(),GetMatrixColNumber());
/*
	for (int nRow=0;nRow<GetMatrixRowNumber();nRow++)
	{
		for (int nCol=0;nCol<GetMatrixColNumber();nCol++)
		{
			TRACE("\t%f",m_pTMatrix [nRow][nCol]);
		}
		TRACE("\r\n");
	}

	TRACE("************  打印矩阵 end    ************\r\n");
*/
}

void CBPMatrix::Print(CTString strName)
{
/*
	TRACE(strName);
	TRACE(" Row:%d    Col:%d\r\n",GetMatrixRowNumber(),GetMatrixColNumber());

	TRACE("\r\n");

*/
}

void CBPMatrix::Serialize(CMemoStream *pStream, bool IsStoring)
{
	if (IsStoring)
	{
		pStream->Write(&m_nRow,sizeof(int));
		pStream->Write(&m_nCol,sizeof(int));

		for (int nRow=0;nRow<m_nRow;nRow++)
		{
			for (int nCol=0;nCol<m_nCol;nCol++)
			{
				double value = m_pTMatrix [nRow][nCol];
				pStream->Write(&value ,sizeof(double));
			}
			
		}
	}
	else
	{
		pStream->Read(&m_nRow,sizeof(int));
		pStream->Read(&m_nCol,sizeof(int));

		m_pTMatrix.resize (m_nRow);
		for (int nRow=0;nRow<m_nRow;nRow++)
		{
			m_pTMatrix[nRow].resize (m_nCol);
			for (int nCol=0;nCol<m_nCol;nCol++)
			{
				double value = 0 ;
				pStream->Read(&value ,sizeof(double));
				m_pTMatrix [nRow][nCol] = value;
			}
			
		}
	}
}

//重新分配大小
void CBPMatrix::ReSize(int nRow, int nCol,bool bZero /*= true*/)
{
	m_nRow = nRow;
	m_nCol = nCol;

	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		if (bZero )
			memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}
}

void CBPMatrix::SetCol(int nCol)
{
	this->SetMatrixColNumber(nCol,false);
}
/*
 *	得到指定列（从零开始）的数据，，空间由外部释放
 *  返回指向数组空间的指针，为空则失败
 */
double* CBPMatrix::GetColArray(int nIndex)
{
	if (nIndex >= this->GetMatrixColNumber() || nIndex<0)
		return NULL;
	int nRow = this->GetMatrixRowNumber();
	if (nRow == 0)
		return NULL;
	double *pBuf = new double[nRow];
	for (int i=0;i<nRow; i++)
		*(pBuf+i) = this->m_pTMatrix[i][nIndex];
	return pBuf;
}
/*
 *从数组中找到位置
 * pSortAry [in] 数组 
 * nBufLen  [in] 数组大小 
 * value    [in] 查找的值
 * nPos     [out] 返回位置
 * 找到，则返回true       nPos则返回所在位置
 * 没有找到，则返回false  nPos则返回该数据应当插入的位置
 */
bool CBPMatrix::FindPos(double *pSortAry,int nBufLen,double value,int&nPos)
{
	int nLeft =0;
	int nRight = nBufLen-1;
	while(nLeft <= nRight)
	{
		int nMid = (nLeft+nRight)/2;
		if ( value >*(pSortAry+nMid))
		{
			nLeft = nMid+1;
		}
		else if ( value == *(pSortAry+nMid))
		{
			nPos = nMid;
			return true;
		}
		else
		{
			nRight= nMid -1;
		}
	}
	nPos = nLeft;
	return false;
}

/*
 *根据指定的列得到该列中不相同数据的个数。 like select distinct fieldname from table etc.
 *nCol:      输入  指定的列
 *pSortAry:  输出  存放相异数据的数组，空间由外部申请。大小由nBufLen指定。
 *                 该数据从小到大排序存放
 *nBufLen:   输入  指定pSortAry的大小。当相异数据个数超过此大小时，返回－1
 *返回:      相异数据的个数  0 ：没有数据  －1；空间不足失败
 */
int CBPMatrix::GetDistinct(int nCol, double *pSortAry, int nBufLen)
{
	if (nCol>=this->GetMatrixColNumber() || nCol<0)
		return 0;
	int nRow = this->GetMatrixRowNumber();
	if (nRow==0)
		return 0;
	int nCurLen = 0;

	for (int i=0;i<nRow;i++)
	{
		int nPos =0;
		if (FindPos(pSortAry,nCurLen,m_pTMatrix[i][nCol],nPos) ==false)
		{
			if (nCurLen>=nBufLen)
				return -1;//空间不足

			if (nPos < nCurLen)//在中间，则需要移动数据
				memmove(pSortAry+nPos+1,pSortAry+nPos,sizeof(double)*(nCurLen-nPos));

			pSortAry[nPos] = m_pTMatrix[i][nCol];
			nCurLen++;//当前长度增加

		}
	}
	return nCurLen;
	
}
/*得到指定的行的所有列中最大值所在的列，返回列号，从零开始
*nRow 指定的行号，从零开始
*出错 返回-1
*/
int CBPMatrix::GetMaxValueCol(int nRow)
{
	int nRowNum = this->GetMatrixRowNumber();
	if (nRowNum==0 || nRow>= nRowNum ||nRow <0)
		return -1;
	int nColNum = GetMatrixColNumber();
	if (nColNum ==0)
		return -1;
	VDOUBLE vdb = this->m_pTMatrix[nRow];
	double dMax = vdb[0];
	int nMaxIndex = 0;
	for (int i=1;i<nColNum; i++)
	{
		if (vdb[i]>dMax)
		{
			dMax = vdb[i];
			nMaxIndex = i;
		}
	}
	return nMaxIndex;
}
