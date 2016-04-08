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
	// ��̬�����ά����
	//TMatrix tMatrix;
	m_pTMatrix.resize (nRow);

	for(unsigned int i=0; i < nRow; i++)
	{
		m_pTMatrix[i].resize(nCol);//zlq �Ƴ���
		memset(&m_pTMatrix[i][0],0,sizeof(double)*nCol);
	}

	// �Զ��������ֵ
	m_nRow	= nRow;
	m_nCol	= nCol;
	//m_pTMatrix = tMatrix;

}


CBPMatrix::CBPMatrix(const CBPMatrix& cMatrixB)
{
	// Initialize the variable
	m_nRow = cMatrixB.m_nRow ;
	m_nCol = cMatrixB.m_nCol ;
	m_pTMatrix = cMatrixB.m_pTMatrix ;//��������
 
	
}


/////////////////////////////////////////////////////////////////////////////
// CBPMatrix member functions
//

CBPMatrix CBPMatrix::operator +(const CBPMatrix& cMatrixB)
{
	// Ҫ���������ӵ�����: ������Ŀ���!
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
// 		::AfxMessageBox (("ִ����ӵ���������ά�������!"),MB_OK | MB_ICONERROR);
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
	// Ҫ���������ӵ�����: ��������Ŀ���!
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
// 		::AfxMessageBox (("ִ���������������ά�������!"),MB_OK | MB_ICONERROR);
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
// 		::AfxMessageBox (("ִ����˵���������ά����������˵�����!"),MB_OK | MB_ICONERROR);
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
// 		::AfxMessageBox(("�Ⱥ��������ߵľ����ά�������!"),MB_OK | MB_ICONERROR);
		return *this;	// return invalid value
	}	

	// ��������ֵ
	m_nRow = cMatrixB.m_nRow ;
	m_nCol = cMatrixB.m_nCol ;
	m_pTMatrix = cMatrixB.m_pTMatrix ;// ��ֵ����

 
	
	return *this;
}


CBPMatrix& CBPMatrix::operator += (const CBPMatrix& cMatrixB)
{
	if(m_nRow != cMatrixB.m_nRow || m_nCol != cMatrixB.m_nCol )
	{
		//printf("����!ִ����ӵ���������ά�������!\n");
// 		::AfxMessageBox (("����������߾����ά�������!"),MB_OK | MB_ICONERROR);
		return *this;	// return invalid value
	}
	
	// ��ֵ����
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
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::MergeColumnsToColumnVector()
{
	CBPMatrix cMatrix(m_nRow * m_nCol,(unsigned int)1);

	// �Ծ���ֵ
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
	// �ο���Ŀ: �������ֵ���� --->ʩ���� �¹�֦
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// �ж��Ƿ��ǿ�����:
	//		������һ���Ƿ���!!!

	if ( m_nRow != m_nCol)
	{
		//printf("����!����������������,�Ƿǿ�����!\n");
// 		::AfxMessageBox (("����������������,�Ƿǿ�����!"),MB_OK | MB_ICONERROR);
	}

	CBPMatrix cMatrix = *this;

	//***********************************************************************
	// ˼·:(�ǳ���˼ά!)
	//		��̬������������(2*m_nCol)���洢ÿ�ν������������ֵ
	//		������û���н�������¼��������,
	//		1.û�����н����������������,��SwapMatrixRow()������
	//		��⵽����ֵ�����������,��SwapMatrixCol()������Ҳһ��,
	//		��⵽����ֵ�����������,��ռ��ϵͳ��Դ;
	//		2.����ȵľͽ���
	//***********************************************************************
    
	//	�����ڴ�
	int *pIntArray = new int [2*m_nCol];

	// nSetp --- Լ������,����չ��	
	for(unsigned int k=0; k < cMatrix.m_nCol; k++)
	{
		/////////////////////////////////////////////////////////////////////
		// �����н��� ---> ��Ϸ����:
		// Ϊ��֤������̵���ֵ�ȶ���,�ڵ�k��Լ��ʱ,����{a(ik)}|i=k->n��ѡ��
		// ģ�������ΪԼ����Ԫ��,������������Ӧ����

		// �����Ԫ��
		double nMaxElement = cMatrix.m_pTMatrix [k][k];
		// �����Ԫ�����ڵ�����
		unsigned int nMainRow = k;

		for(unsigned int nCount = k+1; nCount < cMatrix.m_nCol; nCount++)
		{
			if( fabs(nMaxElement) < fabs(cMatrix.m_pTMatrix [nCount][k]) )
			{
				nMaxElement = cMatrix.m_pTMatrix [nCount][k];
				nMainRow = nCount;
			}
		}

		// ������������������������
		pIntArray [2*k] = k;
		pIntArray [2*k+1] = nMainRow;
	

		// ������
		cMatrix.SwapMatrixRow(k,nMainRow);

		//Display();

		//	�ж��Ƿ��ǿ�����
		if(cMatrix.m_pTMatrix [k][k] == 0)
		{
			//printf("����!�˾���Ϊ�ǿ�����!\n");
// 			::AfxMessageBox (("�˾���Ϊ�ǿ�����,û�������!"),MB_OK | MB_ICONERROR);
		}

		cMatrix.m_pTMatrix [k][k] = 1/(cMatrix.m_pTMatrix [k][k]);
		

		// ������
		for(unsigned int i=0; i < cMatrix.m_nRow; i++)
		{	
			if( i != k)
				cMatrix.m_pTMatrix [i][k] = -(cMatrix.m_pTMatrix [k][k]) * (cMatrix.m_pTMatrix [i][k]); 
			
			//int nTempValue = m_pTMatrix [i][k];
			
		}
		
		//printf("\n");

		// Լ��������
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

		// ������
		for(unsigned int j=0; j < cMatrix.m_nCol; j++)
		{
			if( j != k)
				cMatrix.m_pTMatrix [k][j] = (cMatrix.m_pTMatrix [k][k]) * (cMatrix.m_pTMatrix [k][j]);

		}

	}

	
	/////////////////////////////////////////////////////////////////////////
	// �����н��� ---> �Խ����к�ľ�������н��� ---> ��ԭ����
	// ��Ϸ����:
	// ����ʼ�����н��е��н��� ---> �������Ӧ���н������л�ԭ,���ɵõ������
	// �����

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

	// �����洢��ȡ�ı��ļ���һ�е�����
	CTString	strData;				 
	// ������¼�ı��ļ���һ���ж���������?
	unsigned int	nRow = 0;		

	/////////////////////////////////////////////////////////////////////////
	// Step 1: �õ��ļ���������Ŀ�������ı��ļ���������Ŀ�����ö���(����)����
	// ����Ŀ
	//

	while(dataFile.ReadString(strData) != false)
	{
		++nRow;
	}

	// �����ı��ļ������ݵ��������ö���(����)������
	m_nRow = nRow;
	SetMatrixRowNumber(m_nRow);

	// ���¶�λ��ǰ�ļ�ָ�뵽�ļ���ͷ
	dataFile.Seek(0);

	dataFile.ReadString (strData);
	strData.TrimLeft ();
	strData.TrimRight ();

	char	SPACE_CHARACTER = ' ';
	// ������¼�ı��ļ���һ���ж�����?
	unsigned int	nCol = 0;						
	
	// �ո�����ַ����е�λ������
	int nIndex = 0;

	do
	{
		nIndex = strData.Find (SPACE_CHARACTER);

		++nCol;

		strData = strData.Right (strData.GetLength () - nIndex -1);

		// ȥ������Ŀո�
		strData.TrimLeft ();
	}while(nIndex != -1);

	// �����ı��ļ������ݵ��������ö���(����)������
	m_nCol = nCol;
	SetMatrixColNumber(m_nCol);

	// End of Getting the Rows and Cols of the Text File
	/////////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////////
	// Step 2: �����ı��ļ��е����ݶԾ���ֵ,�����ÿ�е������Ƿ�͵�һ�е�
	// �������,�������ʾ������Ϣ
	//

	// ���¶�λ��ǰ�ļ�ָ�뵽�ļ���ͷ
	dataFile.Seek(0);

	// �Ծ����е�Ԫ��װ���ı��ļ�������
	for(unsigned int i=0; i < m_nRow; i++)
	{
		dataFile.ReadString (strData);
		strData.TrimLeft ();
		strData.TrimRight ();

		// ��֤ÿ�е������Ƿ����һ�е��������
		unsigned int nVerifyColNum = 0;

		do
		{
			nIndex = strData.Find (SPACE_CHARACTER);

			++nVerifyColNum;

			if(nIndex != -1)
			{
				// ��ȡ�ַ��������ַ���,����ȡһ��double��ʵ������
				CTString strDoubleNumber = strData.Left (nIndex);	

				// ���ַ���ת��Ϊdouble��ʵ��
				double RealNumber = atof(strDoubleNumber);

				m_pTMatrix [i][nVerifyColNum - 1] = RealNumber;

				strData = strData.Right (strData.GetLength () - nIndex -1);

				// ȥ������Ŀո�
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

			CTString strMessage =  CTString(("�ı��ļ���")) + strRowNumber + CTString(("��һ����")) + strVerifyColNumber + CTString(("��,���һ���е�����")) + strColNumber + CTString(("�����!"));

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


	// ������(����)�е�����д���ļ�
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
// 		::AfxMessageBox(("���ܴ����ļ�!"),MB_OK | MB_ICONERROR);
		dataFile.Close ();
		return false;
	}
	
	dataFile.SeekToEnd ();

	// ������(����)�е�����д���ļ�
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
// �Ծ����е�Ԫ�ؽ���һ�β���:
//		ʹ�����Ϊ��λ��
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::Eye()
{
	// Verify whether the rows is equal to the columns or not
	if(m_nRow != m_nCol)
	{
// 		::AfxMessageBox (("�˾���������������!����ת��Ϊ��λ��!"),MB_OK | MB_ICONERROR);
		return;
	}
	//===uc ��Ҫ�Ż�
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
//		CBPMatrix& cMatrix:		������������Դ
//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��,��0��ʼ
// Purpose:
//		This function will copy all the data of the cMatrix
// Notes:
//		�˶��������������!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::GetMatrixData(CBPMatrix& cMatrix, unsigned int nIndex)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("�����ľ�����������!"),MB_OK | MB_ICONERROR);
		return;
	}

	if((m_nRow - nIndex) < (cMatrix.m_nRow * cMatrix.m_nCol))
	{
// 		::AfxMessageBox (("��������Ŀռ���������!"),MB_OK | MB_ICONERROR);
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
//		CBPMatrix& cMatrix:		�����ľ���
//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
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
// 		::AfxMessageBox (("�����������������,����������!"),MB_OK | MB_ICONERROR);
		return;
	}

	// Verify whether the number of the object element is enough to be copyed
	if((m_nRow - nIndex) < (cMatrix.m_nRow * cMatrix.m_nCol))
	{
// 		::AfxMessageBox (("�����е�Ԫ����������!"),MB_OK | MB_ICONERROR);
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
//		CBPMatrix& cMatrix:		�����ľ���
//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
//		unsigned int nRow:		�����������ڱ��������е�������
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
// 		::AfxMessageBox (("�����������������,����������!"),MB_OK | MB_ICONERROR);
		return;
	}

	// Verify whether the number of the object element is enough to be copyed
	if((m_nRow - nIndex) < cMatrix.m_nCol )
	{
// 		::AfxMessageBox (("�����Ԫ����������!"),MB_OK | MB_ICONERROR);
		return;
	}

	for(unsigned int i=0; i < cMatrix.m_nCol; i++)
	{
		cMatrix.m_pTMatrix [nRow][i] = m_pTMatrix [nIndex + i][(unsigned int)0];
	}

}


/////////////////////////////////////////////////////////////////////////////
// Parameter:
//		CBPMatrix& cMatrix:		������������Դ
//		unsigned int nIndex:	�������������ڶ����еĿ�ʼ����λ��
//		unsigned int nRow:		�������������ڱ����������е�������(��0��ʼ)
// Purpose:
//		This function will copy all the data of the cMatrix
//	Notes:
//		�˶��������������!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::GetMatrixRowData(CBPMatrix& cMatrix, unsigned int nIndex, unsigned int nRow)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("�����ľ�����������!"),MB_OK | MB_ICONERROR);
		return;
	}

	if((m_nRow - nIndex) < cMatrix.m_nCol)
	{
// 		::AfxMessageBox (("��������Ŀռ���������!"),MB_OK | MB_ICONERROR);
		return;
	}
	
	for(unsigned int i=0; i < cMatrix.m_nCol; i++)
	{
		m_pTMatrix [nIndex + i][(unsigned int)0] = cMatrix.m_pTMatrix [nRow][i];
	}

}
/*
*�������þ������������ݲ�����
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
*�������þ������������ݲ�����
* bNeedZero �Ƿ���Ҫ���� Ĭ������
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
// ���þ����������
void CBPMatrix::SetMatrixRowAndCol(unsigned int nRow,unsigned int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;

	// �����ڴ�
	m_pTMatrix.resize (m_nRow);
	for(unsigned int i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

}


/////////////////////////////////////////////////////////////////////////////
// Initialize()
// �����ʼ������,�����������Ŀ����ʼ��Ϊ��,�����е�Ԫ��ȫ����ʼ��Ϊ��
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
// �����ʼ������,�����������Ŀ����ʼ��Ϊ��,�����е�Ԫ��ȫ����ʼ��Ϊ��
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::InitializeZero()
{
	m_nRow = 0;
	m_nCol = 0;

	m_pTMatrix.resize (m_nRow);

}

/////////////////////////////////////////////////////////////////////////////
// RandomInitialize()
// �������е�Ԫ�������ʼ������,Ԫ�ص�ֵ��(-1,1)֮���С��
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
// ����������Ӿ���Ԫ�ص�����һ��������
// Parameter:
//		[out]	cMatrix ----> ������Ӿ��󷵻صĽ��
//		[in]	nStartX ----> �Ӿ����ھ����е���ʼ����,��Ӧ��,������1��ʼ
//		[in]	nStartY ----> �Ӿ����ھ����е���ʼ����,��Ӧ��,������1��ʼ
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopySubMatrix(CBPMatrix& cMatrix,unsigned int nStartX,unsigned int nStartY)
{
	if((m_nRow  < cMatrix.m_nRow + nStartY ) | (m_nCol  < cMatrix.m_nCol + nStartX))
	{
// 		::AfxMessageBox (("�������ľ���ά��С��Ҫ�����ľ�������Ҫ��ά��!"),MB_OK | MB_ICONERROR);
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
//		[in]	cMatrix ----> �������ľ���
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopyMatrix(const CBPMatrix& cMatrix)
{
	InitializeZero();
	m_nRow	= cMatrix.m_nRow ;
	m_nCol	= cMatrix.m_nCol ;
	
	m_pTMatrix	= cMatrix.m_pTMatrix ;
	//===uc  �����ƺ�û����
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
//	��һ���������п������ݵ�һ��������
//	Parameter:
//		[out]	cMatrix ----> �����ķ��ؽ��
//		[in]	nIndex	----> ���������е�����ֵ
//	Notes:
//		�������Ķ��������������!!!
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::CopySubMatrixFromVector(CBPMatrix& cMatrix,unsigned int nIndex)
{
	if(m_nCol != 1)
	{
// 		::AfxMessageBox (("�������ľ�����������!!!"),MB_OK | MB_ICONERROR);
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
// �Ծ��������չ
//	ʵ�ֹ���:
//		�Ծ��������������չ,nTimes��ÿ����չ�Ĵ���
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpyi(const CBPMatrix &cMatrix, unsigned int nTimes)
{
	m_nRow	=	cMatrix.m_nRow ;
	m_nCol	=	cMatrix.m_nCol *	nTimes;

	// ���ݿռ�����ڴ�
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);

	}
	//cMatrix.Print();
	// ��ֵ
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
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpyd(const  CBPMatrix &cMatrix)
{
	m_nRow	=	cMatrix.m_nRow ;
	m_nCol	=	cMatrix.m_nCol * cMatrix.m_nRow ;

	// ���ݿռ�����ڴ�
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

	// ������ֵ
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
/////////////////////////////////////////////////////////////////////////////

void CBPMatrix::nncpy(const CBPMatrix& cMatrix,unsigned int nTimes)
{
	m_nRow = cMatrix.m_nRow ;
	m_nCol = cMatrix.m_nCol * nTimes;

	// ���ݿռ�����ڴ�
	m_pTMatrix.resize (m_nRow);
	unsigned int i;
	for(i=0; i < m_nRow; i++)
	{
		m_pTMatrix[i].resize (m_nCol);
		memset(&m_pTMatrix[i][0],0,sizeof(double)*m_nCol);
	}

	// �Ծ���ֵ
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// �Ծ����е�Ԫ�ؽ���һ�β���:
//		ʹ�������е����Ӧ����Ԫ�����
//	Parameter:
//		nRowIndex	---->	������ֵ(���㿪ʼ)
//							�Դ�����Ϊ��׼,ʹ����������������Ӧ���е�ֵ
//							��������Ӧ���е�ֵ���
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// �Ծ��������е�Ԫ�ؽ���һ�η����Ա任:
//		�任���ֵy��任ǰ��ֵ�Ĺ�ϵ��:
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
// ʵ�ֶԵ�˲�����������
/////////////////////////////////////////////////////////////////////////////

CBPMatrix CBPMatrix::operator / (const CBPMatrix& cMatrixB)
{
	CBPMatrix cMatrix = *this;

	if( (m_nRow != cMatrixB.m_nRow) || (m_nCol != cMatrixB.m_nCol) )
	{
// 		::AfxMessageBox (("���������ά�������,����������˵�����!"),MB_OK | MB_ICONERROR);
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
// ���� 'double - CBPMatrix' �����
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
// ����ϲ������
//	�ϲ�����:
//		1. ����ϲ��������������������������;
//		2. ����ϲ�������������������Բ����;
//		3. �ϲ��󷵻صľ�������������ϲ��ľ�����������,�����ǲ���ϲ���
//			�������������֮��;
/////////////////////////////////////////////////////////////////////////////

CBPMatrix MergeMatrix(const CBPMatrix& cMatrixA,const CBPMatrix& cMatrixB)
{
	//	�������
	if( cMatrixA.GetMatrixRowNumber () != cMatrixB.GetMatrixRowNumber () )
	{
// 		::AfxMessageBox (("����ϲ���������������������!"),MB_OK | MB_ICONERROR);
	
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
// 	TRACE("************  ��ӡ���� start  ************\r\n");
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

	TRACE("************  ��ӡ���� end    ************\r\n");
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

//���·����С
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
 *	�õ�ָ���У����㿪ʼ�������ݣ����ռ����ⲿ�ͷ�
 *  ����ָ������ռ��ָ�룬Ϊ����ʧ��
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
 *���������ҵ�λ��
 * pSortAry [in] ���� 
 * nBufLen  [in] �����С 
 * value    [in] ���ҵ�ֵ
 * nPos     [out] ����λ��
 * �ҵ����򷵻�true       nPos�򷵻�����λ��
 * û���ҵ����򷵻�false  nPos�򷵻ظ�����Ӧ�������λ��
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
 *����ָ�����еõ������в���ͬ���ݵĸ����� like select distinct fieldname from table etc.
 *nCol:      ����  ָ������
 *pSortAry:  ���  ����������ݵ����飬�ռ����ⲿ���롣��С��nBufLenָ����
 *                 �����ݴ�С����������
 *nBufLen:   ����  ָ��pSortAry�Ĵ�С�����������ݸ��������˴�Сʱ�����أ�1
 *����:      �������ݵĸ���  0 ��û������  ��1���ռ䲻��ʧ��
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
				return -1;//�ռ䲻��

			if (nPos < nCurLen)//���м䣬����Ҫ�ƶ�����
				memmove(pSortAry+nPos+1,pSortAry+nPos,sizeof(double)*(nCurLen-nPos));

			pSortAry[nPos] = m_pTMatrix[i][nCol];
			nCurLen++;//��ǰ��������

		}
	}
	return nCurLen;
	
}
/*�õ�ָ�����е������������ֵ���ڵ��У������кţ����㿪ʼ
*nRow ָ�����кţ����㿪ʼ
*���� ����-1
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
