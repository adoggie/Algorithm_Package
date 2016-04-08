// AlgorithmBase.cpp: implementation of the CAlgorithmBase class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "AlgorithmBase.h"

#define MAXLEN 80

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlgorithmBase::CAlgorithmBase()
{
	m_fp				= NULL;
	m_pDataInterface	= NULL;
	m_pResult			= NULL;
//	m_pCorrelation		= NULL;
	m_nRow				= 0;										
	m_nCol				= 0;										
	m_nSel				= 0;										
//	m_pData				= NULL;
}

CAlgorithmBase::~CAlgorithmBase()
{

}

void CAlgorithmBase::WriteData(CDoubleMatrix matrix,char * pszTitle)
{	
#ifdef _DEBUG
	int i=0, j=0;
	int nLen = matrix.mcol();
	int nRow = matrix.mrow();
	printf("\n***Matrix********%s*****begin*****\n",pszTitle);
	for (i=0; i<nLen; i++)
	{
		for (j=0; j<nRow; j++)
			printf("%.5f\t",matrix(j, i));
		printf("\n-------------------------------");
	}
	printf("\n----Matrix-------%s-----end-------\n",pszTitle);
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//							 调试用的数据输出用的写文件函数
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CAlgorithmBase::WriteData(double * pData, int nRow, int nCol,char * pszTitle)
{
#ifdef _DEBUG
	m_fp = fopen("factor_data.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%10.4f",pData[i*nCol+j]);
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//						调试用的数据输出用的写文件函数
//
////////////////////////////////////////////////////////////////////////////////////////////
void CAlgorithmBase::WriteData(int * pData, int nRow, int nCol,char * pszTitle)
{
#ifdef _DEBUG
	m_fp = fopen("factor_data.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%10d",pData[i*nCol+j]);
		}
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}



bool CAlgorithmBase::WriteData(CComplexMatrix m, char *pszName)
{
#ifdef _DEBUG
	int nRow = m.mrow();
	int nCol = m.mcol();
	m_fp = fopen("factor_data.txt","a");

    if( !m_fp )
            return false;
    fprintf(m_fp,"\n\n\n\n");
    if(pszName)
    {
		fprintf(m_fp,"%s\n\n",pszName);
    }
    for(int i=0;i<nRow;i++)
    {
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%f",real(m(i,j)));
			if(imag(m(i,j))>0)
				fprintf(m_fp,"+");
			else
				fprintf(m_fp,"-");
			fprintf(m_fp,"%fi  ",fabs(imag(m(i,j))));
		}
		fprintf(m_fp,"\n");
    }
    fclose(m_fp);
    m_fp = NULL;	
#endif
	return true;

}


bool CAlgorithmBase::WriteData(CIntMatrix m, char *pszName)
{
#ifdef _DEBUG
	int nRow = m.mrow();
	int nCol = m.mcol();
	m_fp = fopen("factor_data.txt","a");

    if( !m_fp )
            return false;
    fprintf(m_fp,"\n\n\n\n");
    if(pszName)
    {
		fprintf(m_fp,"%s\n\n",pszName);
    }
    for(int i=0;i<nRow;i++)
    {
		for(int j=0;j<nCol;j++)
		{
			fprintf(m_fp,"%10d ",m(i,j));
		}
		fprintf(m_fp,"\n");
    }
    fclose(m_fp);
    m_fp = NULL;	
#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
//			拷贝指针数组中地内容，但是指向地内存不变，
//
/////////////////////////////////////////////////////////////////////////////////////////
void CAlgorithmBase::PtrArrayCopy(CPtrArray *pSource, CPtrArray *pDes)
{
	for(int i=0;i<pSource->GetSize();i++)
	{
		pDes->Add( pSource->GetAt(i) );
	}
}

void CAlgorithmBase::WriteData(CDoubleVector vect, char *pszTitle)
{
#ifdef _DEBUG
	int i=0;
	int nLen = vect.vlen();
	printf("\n***Vec********%s*****Begin*****\n",pszTitle);
	for (i=0; i<nLen; i++)
	{
		if (i!=0 && i%5==0)
		{
			printf("\n");
		}
		printf("%.5f\t",vect(i));
	}
	printf("\n---Vec---------%s----end--------\n",pszTitle);
	
#endif
}

void CAlgorithmBase::WriteData(CIntVector vect, char *pszTitle)
{
#ifdef _DEBUG
	m_fp = fopen("factor_data.txt","a");
	if( !m_fp )
		return;
	fprintf(m_fp,"\n\n\n\n");
	if(pszTitle)
	{
		fprintf(m_fp,"%s\n\n",pszTitle);
	}
	for(int i=0;i<vect.vlen();i++)
	{
		fprintf(m_fp,"%10d",vect[i]);
		fprintf(m_fp,"\n");
	}
	fclose(m_fp);
	m_fp = NULL;
#endif
}
 
CDoubleMatrix CAlgorithmBase::VectorToMatrix(const CDoubleVector &vect)
{
	CDoubleMatrix matrix(vect.vlen(),1);
	for(int i=0;i<vect.vlen();i++)
	{
		matrix(i,0) = vect.GetValue(i);
	}
	return matrix;
}
//Added by ywj 2005-04-29
CDoubleMatrix CAlgorithmBase::VectorToMatrix(const CDoubleVector &vect,int nRow,int nCol)
{
	CDoubleMatrix matrix(nRow,nCol,0.0);
	int Length = nRow * nCol;
	int i=0;
	int j=0;
	int k =0;
    if(Length>=vect.vlen())
		Length = vect.vlen();
	//按照列填充。
	while(i<Length)
	{
		matrix(j,k) = vect.GetValue(i);
		i++;
		j++;
		if(j==nRow)
		{
			j =0;
			k++;
		}
	}
	return matrix;
}

CDoubleVector CAlgorithmBase::MatrixToVector(const CDoubleMatrix &matrix)
{
	CDoubleVector vect(matrix.mrow() * matrix.mcol());
	int iCount= 0;
	for(int i=0;i<matrix.mcol();i++)
	{
		for(int j = 0 ;j< matrix.mrow(); j++)
		vect[iCount++] = matrix.GetValue(j,i);
	}
	return vect;
}


//////////////////////////////////////////////////////////////////////////////////////
//
//						转置矩阵a, m行n列		
//
//////////////////////////////////////////////////////////////////////////////////////
double * CAlgorithmBase::MatrixTran(double *a, int m, int n)
{
	assert(a);
	double * pTran = NULL;
	pTran = new double [m*n];
	memset(pTran,0,sizeof(double)*m*n);
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			pTran[j*m+i]  = a[i*n + j];
		}
	}
	return pTran;
}

CDoubleVector CAlgorithmBase::MatrixTran(CDoubleVector Vect_A, int m, int n)
{
	assert(Vect_A.vlen()>0);
	CDoubleVector Vect_Tran(m*n);
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			Vect_Tran[j+i*n]  = Vect_A[i + j*m];
		}
	}
	return Vect_Tran;
}

void CAlgorithmBase::MatrixToPointer(CDoubleMatrix matrix, double **pt)
{
	*pt = new double [matrix.mcol()*matrix.mrow()];
	*pt << matrix;
}

void CAlgorithmBase::FreePtrArray(CPtrArray *pArray)
{
	for(int i=0;i<pArray->GetSize();i++)
	{
		delete pArray->GetAt(i);
	}
	pArray->RemoveAll();
}



double * CAlgorithmBase::GetDataSingle(CDataInterface *pDataInterface,int nIndex)
{
	TDataSet * pDataArray=NULL;
	if(nIndex>=0)
	{		
		int nCaseNumb = pDataInterface->GetColData(nIndex,pDataArray,1);//取回数据		
		if(nCaseNumb > 0)
		{
			double * pData = new double [nCaseNumb];
			//if(!CDataInterface::IsEqual(pDataArray,nCaseNumb))
			{				
				for(int j=0;j<nCaseNumb;j++)
				{
					pData[j] = pDataArray[j].dValue;
				}
				delete [] pDataArray;
				return pData;
			}
			//else
			{
			//	delete [] pData;pData = NULL;
			//	return NULL;		
			}
		}
		else
			return NULL;
	}	
	else 
		return NULL;
}



//bRemoveEqual为真时，去除数据相同列
double * CAlgorithmBase::GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual)
{
        double * pData = NULL;
        //取回数据，组织输入数据矩阵
        int nCaseNumb,m = tVarInfo.iCount +1;
        TDataSet * pDataArray=NULL;
        int nColNum=0;
        //数据检验
		int i=0;
        for(i=0;i<tVarInfo.iCount;i++)
        {
                nColNum=tVarInfo.pValue[i];
                if( nColNum < 0 )
                {
//                         AfxMessageBox("取数据时出错!");
                        return NULL;
                }
                nCaseNumb = pDataInterface->GetColData(nColNum,pDataArray,1);//取回数据
                assert(nCaseNumb > 0 );
                if(bRemoveEqual && IsEqual(pDataArray,nCaseNumb))
                {
                        //表示所有数据相同，应该除去
                        tVarInfo.RemoveAt(i);
                        i--;
                }
        }
        m = tVarInfo.iCount;
        for(i=0;i<m;i++)
        {
                if( !pData  )
                        pData = new double [pDataInterface->GetRowCount() * m ];
                
                nColNum=tVarInfo.pValue[i];
                if( nColNum < 0 )
                {
//                         AfxMessageBox("取数据时出错!");
                        return NULL;
                }
                nCaseNumb = pDataInterface->GetColData(nColNum,pDataArray,1);//取回数据
                assert(nCaseNumb > 0 );
                //赋值
                //检查输入的数据，如果样本值相同就剔除
                for(int j=0;j<nCaseNumb;j++)
                        pData[i+j*m] = pDataArray[j].dValue;
                
                if( pDataArray )
                {
                     delete [] pDataArray;
                     pDataArray=NULL;
                }
        }                
        if ( tVarInfo.iCount <= 0 )
        {
//                 AfxMessageBox("你选择的数据无效!");
                return NULL;
        }
        return pData;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			         对特征值和特征向量按照特征值排序  m:行数，n：列数
//					 nIndex == 0 从小到大排序 nIndex == 1 从大到小排序
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CAlgorithmBase::SortEigenVect(double * pEigenValue, double * pEigenVector,int m,int n,int nIndex)
{
	assert( pEigenValue && pEigenVector );
	//;////TRACE("最大值%d \n",m*n);
	int i=0;
	for(i=0;i<n;i++)
	{
		//找到最大的特征值
		double min = 1e10;
		double max = -1e10;
		int    maxmin_nIndex = -1;
		int j=0;
		for(j=i;j<n;j++)
		{
			if( (pEigenValue[j*n+j] < min && nIndex == 0)
				|| (pEigenValue[j*n+j] > max && nIndex == 1)
				) 
			{   
				min = pEigenValue[j*n+j];
				max = pEigenValue[j*n+j];
				maxmin_nIndex = j;
			}
		}
		//将那一列数据和i列数据交换
		double temp;
//{{{START 2003-2-25 Eric
		if(maxmin_nIndex>=0)
//}}}END  2003-2-25 Eric
		{
			for(j=0;j<m;j++)
			{
				temp					   = pEigenVector[j*n+maxmin_nIndex];
				pEigenVector[j*n+maxmin_nIndex] = pEigenVector[j*n+i];
				pEigenVector[j*n+i]		   = temp;
			}

			temp											= pEigenValue[maxmin_nIndex*n + maxmin_nIndex];
			pEigenValue[maxmin_nIndex*n + maxmin_nIndex]	= pEigenValue[i*n + i];
			pEigenValue[i*n + i]							= temp;
		}
	}
}

void CAlgorithmBase::SortEigenVect(CDoubleVector& vect_EigenValue, CDoubleVector& vect_EigenVector,int m,int n,int nIndex)
{
	assert( vect_EigenValue.vlen() && vect_EigenVector.vlen() );
	//;////TRACE("最大值%d \n",m*n);
	int i=0;
	for(i=0;i<n;i++)
	{
		//找到最大的特征值
		double min = 1e300;
		double max = -1e300;
		//double min = vect_EigenValue[0];
		//double max = vect_EigenValue[0];
		int    maxmin_nIndex = -1;
		int j=0;
		for(j=i;j<n;j++)
		{
			if( (vect_EigenValue[j*n+j] < min && nIndex == 0)
				|| (vect_EigenValue[j*n+j] > max && nIndex == 1)
				) 
			{   
				min = vect_EigenValue[j*n+j];
				max = vect_EigenValue[j*n+j];
				maxmin_nIndex = j;
			}
		}
		//将那一列数据和i列数据交换
		double temp;
		//{{{START 2003-2-25 Eric
		if(maxmin_nIndex>=0)
			//}}}END  2003-2-25 Eric
		{
			for(j=0;j<m;j++)
			{
				temp					   = vect_EigenVector[j * n+ maxmin_nIndex ];
				vect_EigenVector[j * n+ maxmin_nIndex ] = vect_EigenVector[j* n+i ];
				vect_EigenVector[j * n+ i ]		   = temp;
			}
			
			temp											= vect_EigenValue[maxmin_nIndex*n + maxmin_nIndex];
			vect_EigenValue[maxmin_nIndex * n + maxmin_nIndex]	= vect_EigenValue[i * n + i];
			vect_EigenValue[i * n + i]							= temp;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			         对特征值和特征向量按照特征值排序 
//					 nIndex == 0 从小到大排序 nIndex == 1 从大到小排序
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool CAlgorithmBase::SortEigenVect(CDoubleMatrix  &matrix_EigValue,CDoubleMatrix  &matrix_EigVector , int nIndex)
{
	int nRow = matrix_EigVector.mrow();
	int nCol = matrix_EigVector.mcol();

	CDoubleVector  Vect_EigValue(nRow * nCol);
	CDoubleVector  Vect_EigVector(nRow * nCol);
	 Vect_EigVector= MatrixToVector(matrix_EigVector);
	 Vect_EigValue= MatrixToVector(matrix_EigValue);
	SortEigenVect(Vect_EigValue,Vect_EigVector,nRow,nCol,nIndex);
	matrix_EigValue = VectorToMatrix(Vect_EigValue,nRow,nCol);
	matrix_EigVector = VectorToMatrix(Vect_EigVector,nRow,nCol);

	return true;	
}




////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		求解相关系数矩阵，pData : 输入数据矩阵 m: 行 n: 列，返回相关系数矩阵的指针,
//		注意需要输入的矩阵是标准化后的，可以调用Formalize()函数进行标准化
//
////////////////////////////////////////////////////////////////////////////////////////////////////

CDoubleVector  CAlgorithmBase::RelationMatrix(CDoubleVector& Vect_Data, int m, int n)
{
	CDoubleVector Vect_RelationData(n * n);

	double sum(0);
	int i,j,k;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			sum = 0;
			for(k=0;k<m;k++)
			{
				sum += Vect_Data[k*n+j]*Vect_Data[k*n+i];				//Data(k,j)*Data(k,i) ;
			}
			Vect_RelationData[i*n+j] = sum/(m-1);
		}
	}


	m_VectCorrelation.create( n * n);
	m_VectCorrelation =copy(Vect_RelationData);
	return Vect_RelationData;
}
///////////////////////////////////////////////////////////////////////////////////////
//
//			 nIndex == 0 求解矩阵matrix的相关系数矩阵
//           nIndex == 1 求解矩阵matrix的协方差矩阵
//			               返回结果矩阵
//
///////////////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CAlgorithmBase::RelationMatrix(CDoubleMatrix matrix,int nIndex)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleVector Vect_Data (nRow * nCol);
	Vect_Data = MatrixToVector(matrix);
	Formalize(Vect_Data,nRow,nCol,nIndex);
	CDoubleVector Vect_Relation = RelationMatrix(Vect_Data,nRow,nCol);
	CDoubleMatrix matrix_R = VectorToMatrix(Vect_Data,nCol,nCol);
	return matrix_R;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//								标准化按列
//								nIndex：0：要除于标准差   1：不用除于
//
///////////////////////////////////////////////////////////////////////////////////////////
bool CAlgorithmBase::Formalize(const int nIndex)
{
	assert(m_nRow>0&&m_nCol>0&&m_VectData.vlen()>0);
	CDoubleVector Vect_Average = Average(1);
	CDoubleVector Vect_Dev     = Dev(1)    ;

	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		int j=0;
		for(j=0;j<m_nRow;j++)
		{
			switch(nIndex)
			{
			case 0:
				if( IsZero(Vect_Dev[i]) )
				{
					m_VectData[j*m_nCol+i] = 0;
				}
				else
				{
					m_VectData[j*m_nCol+i] = ( Data(j,i) - Vect_Average[i] )/Vect_Dev[i];
				}
				break;
			case 1:
				m_VectData[j*m_nCol+i] = ( Data(j,i) - Vect_Average[i] );
				break;
			}
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//							求标准差,nIndex 为0 按行，为1 按列
//
////////////////////////////////////////////////////////////////////////////////////////////////
//double * CAlgorithmBase::Dev(int nIndex)
CDoubleVector CAlgorithmBase::Dev(int nIndex)
{
	assert( m_VectData.vlen()>0 && m_nCol > 0 && m_nRow > 0 );
	CDoubleVector Vect_Average = Average(nIndex);
	CDoubleVector Vect_Dev;
	int i=0,j=0;
	int nLen = 0;
	double sum(0);
	switch(nIndex)
	{
	case 0:
		//对每一行
		Vect_Dev.create( m_nRow);
		nLen = m_nRow;
		for(i=0;i<m_nRow;i++)
		{
			sum = 0;
			for(j=0;j<m_nCol;j++)
			{
				sum += (Data(i,j)-Vect_Average[i])*(Data(i,j)-Vect_Average[i]);
			}
			if( m_nCol == 1 )
			{
				Vect_Dev[i] = 0;
			}
			else
				Vect_Dev[i] = sqrt(sum / (m_nCol-1));
		}
		break;
	case 1:
		Vect_Dev.create(m_nCol);
		nLen = m_nCol;
		for(i=0;i<m_nCol;i++)
		{
			sum = 0;
			for(j=0;j<m_nRow;j++)
			{
				sum += (Data(j,i)-Vect_Average[i])*(Data(j,i)-Vect_Average[i]);
			}
			if( m_nRow == 1 )
			{
				Vect_Dev[i] = 0;
			}
			else
				Vect_Dev[i] = sqrt(sum/(m_nRow-1));
		}
		break;
	}
	return Vect_Dev;
}


///////////////////////////////////////////////////////////////////////////////////
//
//		求解平均值，pData : 输入数据矩阵 nRow: 行 nCol: 列,  返回平均值数组的指针
//
///////////////////////////////////////////////////////////////////////////////////
CDoubleVector CAlgorithmBase::Average(CDoubleVector& Vect_Data, int nRow, int nCol, int nIndex)
{
	assert( Vect_Data.vlen()>0 && nCol > 0 && nRow > 0 );
	CDoubleVector Vect_Average;
	double sum(0);
	int i,j;
	int nLen = 0;
	switch(nIndex)
	{
	case 0:
		//对每一行求平均值
		Vect_Average.create(nRow);
		nLen = nRow;
		for(i=0;i<nRow;i++)
		{
			sum = 0;
			for(j=0;j<nCol;j++)
			{
				sum += Vect_Data[i*nCol+j];
			}
			Vect_Average[i] = sum / nCol;
		}
		break;
	case 1:
		//对每一列求平均值
		Vect_Average.create(nCol);
		nLen = nCol;
		for(i=0;i<nCol;i++)
		{
			sum = 0;
			for(j=0;j<nRow;j++)
			{
			//	sum += Vect_Data[j*nCol+i];
				sum += Vect_Data[i*nRow+j];

			}
			Vect_Average[i] = sum / nRow;
		}
		break;
	}
	return Vect_Average;
}

double CAlgorithmBase::Data(const int nRow, const int nCol)
{
//  yzmModify 不应该有两块存储数据容器（m_pData和m_VectData）
//	assert(nRow < m_nRow && nCol < m_nCol && m_pData);
	assert(nRow < m_nRow && nCol < m_nCol);
	return m_VectData[nRow*m_nCol+nCol];
}
// yzmAdd  可写取数据
double& CAlgorithmBase::wData(const int nRow, const int nCol)
{
	assert(nRow < m_nRow && nCol < m_nCol);
	return m_VectData[nRow*m_nCol+nCol];
}


CDoubleVector CAlgorithmBase::Average(int nIndex)
{
	assert( m_VectData.vlen()>0 && m_nCol > 0 && m_nRow > 0 );
	CDoubleVector  Vect_Average;
	double sum(0);
	int i,j;
	int nLen = 0;
	switch(nIndex)
	{
	case 0:
		//对每一行求平均值
		Vect_Average.create( m_nRow);
		nLen = m_nRow;
		for(i=0;i<m_nRow;i++)
		{
			sum = 0;
			for(j=0;j<m_nCol;j++)
			{
				sum += Data(i,j);
			}
			Vect_Average[i] = sum / m_nCol;
		}
		break;
	case 1:
		//对每一列求平均值
		Vect_Average.create( m_nCol);
		nLen = m_nCol;
		for(i=0;i<m_nCol;i++)
		{
			sum = 0;
			for(j=0;j<m_nRow;j++)
			{
				sum += Data(j,i);
			}
			Vect_Average[i] = sum / m_nRow;
		}
		break;
	}
	return Vect_Average;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//										判断是否为0
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CAlgorithmBase::IsZero(double data)
{
	return fabs( data ) < ZERO ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////
//
//					    求解标准差，pData : 输入数据矩阵 nRow: 行 nCol: 列,  
//
///////////////////////////////////////////////////////////////////////////////////
CDoubleVector CAlgorithmBase::Dev(CDoubleVector Vect_Data, int nRow, int nCol, int nIndex)
{
	assert( Vect_Data.vlen()>0 && nCol > 0 && nRow > 0 );
	CDoubleVector  Vect_Average = Average(Vect_Data,nRow,nCol,nIndex);
	CDoubleVector Vect_Dev;
	int i=0,j=0;
	int nLen = 0;
	double sum(0);
	switch(nIndex)
	{
	case 0:
		//对每一行
		Vect_Dev.create(nRow);
		nLen = nRow;
		for(i=0;i<nRow;i++)
		{
			sum = 0;
			for(j=0;j<nCol;j++)
			{
				sum += (Vect_Data[i*nCol+j]-Vect_Average[i])*(Vect_Data[i*nCol+j]-Vect_Average[i]);
			}
			if ( nCol == 1 )
			{
				Vect_Dev[i] = 0;
			}
			else
				Vect_Dev[i] = sqrt(sum / (nCol-1));
		}
		break;
	case 1:
		Vect_Dev.create(nCol);
		nLen = nCol;
		for(i=0;i<nCol;i++)
		{
			sum = 0;
			for(j=0;j<nRow;j++)
			{
				sum += (Vect_Data[j+i*nRow]-Vect_Average[i])*(Vect_Data[j+i*nRow]-Vect_Average[i]);
			}
			if( nRow == 1 )
			{
				Vect_Dev[i] = 0;
			}
			else
				Vect_Dev[i] = sqrt(sum/(nRow-1));
		}
		break;
	}
	//delete [] pAverage;
	//Vect_Average.destroy();
	return Vect_Dev;
}

///////////////////////////////////////////////////////////////////////////////////
//
//		   标准化矩阵，pData : 输入数据矩阵 nRow: 行 nCol: 列,
//
///////////////////////////////////////////////////////////////////////////////////

bool CAlgorithmBase::Formalize(CDoubleVector& Vect_Data, int nRow, int nCol, int nIndex)
{
	assert(nRow>0&&nCol>0&&Vect_Data);
	CDoubleVector  Vect_Average = Average(Vect_Data,nRow,nCol,1);//new double [nCol];
	TraceVector(Vect_Average);
	CDoubleVector	Vect_Dev     = Dev(Vect_Data,nRow,nCol,1)    ;//new double [nCol];
	TraceVector(Vect_Dev);
	int i=0;
	for(i=0;i<nCol;i++)
	{
		int j=0;
		for(j=0;j<nRow;j++)
		{
			switch(nIndex)
			{
			case 0:
				if( IsZero(Vect_Dev[i]) )
				{
					Vect_Data[j*nCol+i] = 0;
				}
				else
				{
					Vect_Data[j*nCol+i] = ( Vect_Data[j*nCol+i] - Vect_Average[i] )/Vect_Dev[i];
				}
				break;
			case 1:
				Vect_Data[j*nCol+i] = ( Vect_Data[j*nCol+i] - Vect_Average[i] );
				break;
			}
		}
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////
//
//     设置输入数据的行列数，行为观测点数，列为变量数
//     程序为这个块数据开辟内存
//
///////////////////////////////////////////////////////////////////////////////////////////
void CAlgorithmBase::SetRowsCols(const int nRow, const int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
	m_VectData.create(nRow * nCol);
}

void CAlgorithmBase::SetVectData(double*pData,const int nRow, const int nCol)
{
	if ((m_nRow!=nRow) || (m_nCol!=nCol))
		SetRowsCols(nRow,nCol);
	for (int i=0;i<nRow;i++){
		for (int j=0;j<nCol;j++)
			wData(i,j)=pData[i*nCol+j];
	}
}

CIntMatrix CAlgorithmBase::VectorToMatrix(CIntVector vect)
{
	CIntMatrix matrix(vect.vlen(),1);
	for(int i=0;i<vect.vlen();i++)
	{
		matrix(i,0) = vect[i];
	}
	return matrix;
}
CIntMatrix CAlgorithmBase::VectorToMatrix(CIntVector vect,int nRow,int nCol)
{
	CIntMatrix matrix(nRow,nCol,0);
	int Length = nRow * nCol;
	int i=0;
	int j=0;
	int k =0;
    if(Length>=vect.vlen())
		Length = vect.vlen();
	//按照列填充。
	while(i<Length)
	{
		matrix(j,k) = vect[i];
		i++;
		j++;
		if(j==nRow)
		{
			j =0;
			k++;
		}
	}
	return matrix;
}

CIntVector CAlgorithmBase::DoubleToInt(CDoubleVector vect)
{
	CIntVector vect1(vect.vlen());
	for(int i=0;i<vect.vlen();i++)
	{
		vect1[i] = (int)vect[i];
	}
	return vect1;
}

CIntMatrix CAlgorithmBase::IntToDouble(CDoubleMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CIntMatrix matrix1(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix1(i,j) = (int)matrix(i,j);
		}
	}
	return matrix1;
}


CDoubleMatrix CAlgorithmBase::IntToDouble(CIntMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleMatrix matrix1(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix1(i,j) = matrix(i,j);
		}
	}
	return matrix1;
}

CDoubleVector CAlgorithmBase::IntToDouble(CIntVector vect)
{
	CDoubleVector vect1(vect.vlen());
	for(int i=0;i<vect.vlen();i++)
	{
		vect1[i] = (double)vect[i];
	}
	return vect1;
}



CDoubleMatrix CAlgorithmBase::IntMatrixToDouble(CIntMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleMatrix matrix2(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix2(i,j) = (double)matrix(i,j);
		}
	}
	return matrix2;
}

CIntMatrix CAlgorithmBase::DoubleMatrixToInt(CDoubleMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CIntMatrix matrix2(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix2(i,j) = (int)matrix(i,j);
		}
	}
	return matrix2;
}

CDoubleMatrix CAlgorithmBase::MatrixDelCol(CDoubleMatrix matrix, int nColNo)
{
	assert( nColNo <= matrix.mcol() );

	int nrow = matrix.mrow();

	int ncol = matrix.mcol();

	CDoubleMatrix x(nrow,ncol-1);

	int k(0);

	for(int i=0;i<ncol;i++)
	{
		if(i != nColNo)
		{
			for(int j=0;j<nrow;j++)
			{
				x(j,k) = matrix(j,i);
			}
			k++;
		}		
	}
	return x;
}

void CAlgorithmBase::ProcessWeight(CDoubleMatrix &matrix, CDoubleVector &vect_weight)
{
	int nRow = matrix.mrow();
	int nRow_weight = vect_weight.vlen();
	assert( nRow == nRow_weight );
	int count = 0;
	int i=0;
	for(i=0;i<nRow;i++)
	{
		if( vect_weight[i] > 0.001 )
		{
			count++;
		}		
	}
	CDoubleMatrix matrix_out(count,matrix.mcol());
	CDoubleVector vect_out(count);
	int k = 0;
	for(i=0;i<nRow;i++)
	{
		if( vect_weight[i] > 0 ) 
		{
			int j=0;
			for(j=0;j<matrix.mcol();j++)
			{
				matrix_out(k,j) = matrix(i,j);
				vect_out[k] = vect_weight[i];				
			}
			k++;
		}
	}
	matrix = copy(matrix_out);
	vect_weight = copy(vect_out);
}

bool CAlgorithmBase::IsVectorEqual(CDoubleVector vect1, CDoubleVector vect2)
{
	int len = vect1.vlen();
	int len2 = vect2.vlen();
	if( len != len2 )
		return false;
	int i=0;
	for(i=0;i<len;i++)
	{
		if( !IsZero( vect1(i) - vect2(i) ) )
		{
			return false;
		}
	}
	return true;
}


void CAlgorithmBase::SortMatrix1(CDoubleMatrix &matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	int i=0;
	for(i=0;i<nRow;i++)
	{
		int j=0;
		for(j=0;j<nRow-1;j++)
		{
			if( matrix(j,0) > matrix(j+1,0) )
			{
				int k=0;
				for(k=0;k<nCol;k++)
				{
					double temp = matrix(j,k);
					matrix(j,k) = matrix(j+1,k);
					matrix(j+1,k) = temp;
				}
			}
		}
	}
}

void CAlgorithmBase::ProcessVector(CDoubleVector &vect, long &ltmp)
{
	int nRow = vect.vlen();
	TraceVector(vect);
}


void CAlgorithmBase::TraceVector(const CDoubleVector &vect,bool bFlag,int nValue)
{
}	
void CAlgorithmBase::TraceVector(const CIntVector &vect)
{
}	
long CAlgorithmBase::GetVectorLen()
{
	FILE * fp = fopen("c:/Test.txt","r");
	if( fp == NULL )
		return 0 ;
	int nLen = 0;
	CTString str;
	fscanf(fp,"%s\n",str); 
	nLen = atoi(str);
	for(int i=0;i<nLen;i++)
	{
		double ftmp = 0;
		fscanf(fp,"%s\n",str);
	}
	fscanf(fp,"%s\n",str);
	fclose(fp);
	return atol(str);
}

void CAlgorithmBase::ProcessMissValue(CPtrArray *par, CDoubleMatrix &matrix, CDoubleVector &weight,CDataAccess * pAccess)
{
	long nRecordCount = pAccess->GetRecordCount();
	pAccess->First();
	for(int i=0;i<nRecordCount;i++)
	{
		bool isMissing = false;
		int * pt;
		for(int j=0;j<par->GetSize();j++)
		{
			pt = (int *)par->GetAt(j);
			CField * pField = pAccess->m_pFieldList->FieldByIndex(*pt);
			isMissing = pField->IsNull();
			if( isMissing )
			{
				weight(i) = 0;
				break;
			}
		}
		pAccess->Next();
	}
	::WriteData(weight,"weight_inProcessMissvlaue");
}

long CAlgorithmBase::TransMatrix(CDoubleMatrix &matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	matrix = GetTransMatirx();
	return GetMatrixLen();
}

long CAlgorithmBase::TraceMatrix(const CDoubleMatrix &matrix)
{
	return 0;
}


int CAlgorithmBase::GetMatrixLen()
{
	FILE * fp = fopen("c:/matrix.txt","r");
	if( !fp )
		return 0;
	CTString str;
	int nRow =0;
	int nCol =0;
	fscanf(fp,"%s\n",str);
	nRow = atoi(str);
	fscanf(fp,"%s\n",str);
	nCol = atoi(str);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			fscanf(fp,"%s\n",str);
		}
	}
//	fscanf(fp,"%s\n",str);// zxg ## 2003-1-26
	fclose(fp);
	return atoi(str);
}

CDoubleMatrix CAlgorithmBase::GetTransMatirx()
{
//	int nLen = GetMatrixLen();// zxg ## 2003-1-26
	FILE * fp = fopen("c:/matrix.txt","r");
	if( !fp )
		return 0;
	CTString str;
	int nRow =0;
	int nCol =0;
	fscanf(fp,"%s\n",str);
	nRow = atoi(str);
	fscanf(fp,"%s\n",str);
	nCol = atoi(str);
	CDoubleMatrix matrix(nCol,nRow);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)//&&nLen<MAXLEN  // zxg ## 2003-1-26
		{	

			fscanf(fp,"%s\n",str);
			matrix(j,i) = atof(str);
		}
	}
//	fscanf(fp,"%s\n",str); // zxg ## 2003-1-26
	fclose(fp);
	return matrix;
}

CDoubleVector CAlgorithmBase::GetWeightVector(CDataInterface * pDataInterface)
{
	int nRow = pDataInterface->GetRowCount();
	if( pDataInterface->GetWeightNumb() < 0 )
	{
		CDoubleVector vect(nRow);
		for(int i=0;i<nRow;i++)
		{
			vect[i] = 1;
		}
		return vect;
	}
	double * pWeight = new double [ nRow ];
	pDataInterface->GetWeightData(pWeight);
	CDoubleVector vect(nRow);
	for(int i=0;i<nRow;i++)
	{
		vect[i] = pWeight[i];
	}
	delete [] pWeight;
	pWeight=NULL;
	return vect;
}

void CAlgorithmBase::ProcessMissValue(CPtrArray *par,CDoubleVector &weight,CDataAccess * pAccess,int nMethod)
{
	long nRecordCount = pAccess->GetRecordCount();
	pAccess->First();
	for(int i=0;i<nRecordCount;i++)
	{
		bool isMissing = false;
		int * pt;
		for(int j=0;j<par->GetSize()&&!isMissing;j++)
		{
			pt = (int *)par->GetAt(j);
			CField * pField = pAccess->m_pFieldList->FieldByIndex(*pt);
			isMissing = pField->IsNull();
			switch( nMethod )
			{
			case 0:
				if( isMissing )
				{
					weight(i) = 0;
					break;
				}
				break;
			case 1:
				break;
			case 2:
				break;
			}
		}
		pAccess->Next();
	}
}

CDoubleMatrix CAlgorithmBase::CombieMatrix(CDoubleMatrix matrix, CDoubleVector)
{
	return matrix;
}

CIntVector  CAlgorithmBase::CheckAvailData(CDataInterface *pDataInterface, CDWordArray & colArray)
{
	int  nCaseCount = pDataInterface->GetRowCount();
	CIntVector  Vect_Data(nCaseCount,1);
	TCDataMatrix DataMatrix;
	pDataInterface->GetColsData(colArray,DataMatrix,5);

	for(int j=0; j<colArray.GetSize(); j++ )
	{
		TDColAry * pDataSet = DataMatrix.at(j);
		for(int i=0; i<pDataInterface->GetRowCount(); i++ )
		{
			if( pDataSet->GetAt(i).iIndex<0 )
				Vect_Data[i] = 0;
		}
	}
	return Vect_Data;
}



















