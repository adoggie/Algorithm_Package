// RegressionProbit.cpp: implementation of the CRegressionProbit class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "RegressionProbit.h"

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

CRegressionProbit::CRegressionProbit()
{
	m_pDataInterface= NULL;
//	m_bProcessExcept= true;
	m_fC			= 0.01f;
	this->m_nIterator = 150;
	this->m_nNRR = 0;
	m_nResponse = -1;
//	m_nFact	= -1;
	m_nObserve = -1;
	m_nFactor = -1;
//	m_nMethod = 0;
//	m_nMissMethod  = 0;
}

CRegressionProbit::~CRegressionProbit()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
}

bool CRegressionProbit::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	REGPROBIT
//		/DATA=[FILE]
//		/RESPONSE [varlist]
//		/RECORD [varlist]
//		/LAYER [varlist]
//		/COVARIATE [varlist]
//		/METHOD=[{NONE**},{CUMPUTE},{CONST(0.01**)}]
//		/ITERATE [MAX({50**})]
	AnaWord VOstr;
	VOstr.Import(VO);

	CTStringArray ynames;
	AnaSyntax::getVAR("/RESPONSE", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nResponse = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();

	AnaSyntax::getVAR("/RECORD", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	yname = ynames.GetAt(0);
	m_nObserve = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();

	if (AnaSyntax::exist("/LAYER", VOstr))
	{
		AnaSyntax::getVAR("/LAYER", VOstr, ynames);
		if ((ynames.GetSize())!=1 ) 
			return FALSE;
		yname = ynames.GetAt(0);
		m_nFactor = pDataInterface->IndexByName(yname);
		ynames.RemoveAll();
	}

	CTStringArray xname;
	AnaSyntax::getVAR("/COVARIATE", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	xname.RemoveAll();
	
	if (AnaSyntax::findstr("/METHOD", "CUMPUTE", VOstr))
		m_nNRR = 1;
	else if (AnaSyntax::findstr("/METHOD", "CONST", VOstr))
	{
		m_nNRR = 2;
		m_fC = AnaSyntax::getvalue("/METHOD", "CONST", VOstr);
	}
	else
		m_nNRR = 0;

	m_nIterator = 50;
	if (AnaSyntax::findstr("/ITERATE", "MAX", VOstr))
		m_nIterator = AnaSyntax::getvalue("/ITERATE", "MAX", VOstr);
	
	return TRUE;
}

CResult * CRegressionProbit::OnProbit(CDataInterface *pDataInterface, string VO)
{
	briefrTable=NULL;
	InitParma();
	m_pDataInterface = pDataInterface;
	m_pResult = NULL;
	m_pResult 	= new CResult("概率单位回归");

	if (!(GetValueFromVO(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	if( pDataInterface->GetWeightNumb()>=0 )
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：概率单位回归忽略权重设置。" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}


	double * pFactorData = NULL;
	int nCaseNum;

	double * pData = GetData( pDataInterface, pFactorData, nCaseNum );

	if( !pData )
	{
		CTString strWarning = "注： 有效样本数为0，不能进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );

		return m_pResult;
	}
	
	if( !m_strMissField.IsEmpty() )
	{
		CTString strWarning = "注： 变量 " + m_strMissField + " 为常数，不适合进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}// zxg ## 2003-2-12

	if( m_tVarInfo.iCount==0 )
	{
		CTString strWarning = "注： 所有协变量均为常数，不能进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	int m = m_tVarInfo.iCount + 2;
	if( nCaseNum<m )
	{
		CTLTable * pTable = new CTLTable;
//		pTable->SetRows(1);
//		pTable->SetCols(1);

		pTable->InsertColumn(0,"注： 有效样本数太少(观察数必须大于等于反应数)，无法进行回归分析！");
		CRsltVariable * pETable= new CRsltElementTable("注意",pTable);
		m_pResult->Add(pETable);

		if( pData )
			delete[] pData;
		if( pFactorData )
			delete[] pFactorData;

		return m_pResult;
	}
	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{							
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}
	CDoubleMatrix matrix(pData,nCaseNum,m);
	delete[] pData;
	pData = NULL;

	bool bFactor = false;		
	CIntVector vect(nCaseNum);

	if( pFactorData )
	{
		for(int i=0;i<nCaseNum;i++)
		{
			vect(i) = (int)pFactorData[i];
		}
		delete [] pFactorData; 
		pData = NULL;	

		bFactor = true;
	}

	Probit(matrix,m_nNRR,m_nIterator,m_fC,bFactor,vect);

	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (m_pResult->Count()<=0)
				m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;

	return m_pResult;
}

CDoubleMatrix CRegressionProbit::Hessen(CDoubleMatrix X,CDoubleMatrix NR,CDoubleVector tVec,double C)
{  
	int row = X.mrow();
	int col = X.mcol();
	
	CDoubleMatrix HM(col+1,col+1);
	
	CDoubleVector Fx = normcdf ( tVec , 0 , 1 ) ;

	CDoubleVector P=C+(1-C)*Fx; 

	CDoubleVector FF=exp(-0.5 * tVec * tVec ) / sqrt( 2*pi );
	
	for(int i=0;i<row;i++)
	{
		double Pre= ( 2 * NR(i,1) * P(i) - NR(i,1) - NR(i,0) * P(i)*P(i) ) / pow( P(i)-P(i)*P(i),2 );

		double Beh=( NR(i,1)-NR(i,0) * P(i) ) / ( P(i)*(1-P(i)) );
	
		HM(0,0) +=  Pre * pow(1-Fx(i),2);      
		for(int j=0;j<col;j++)
		{
			HM(0,j+1) +=  ((1-C)*(1-Fx(i))*Pre-Beh  ) * X(i,j) * FF(i);
			HM(j+1,0) = HM(0,j+1);
			for(int k=0;k<col;k++)
			{
				HM(j+1,k+1)  += ( ( 1-C*C ) * Pre * FF(i) - (1-C) * Beh * tVec(i)) * FF(i) * X(i,j) * X(i,k);
			}				
		}
	}	
	return HM;
}

bool CRegressionProbit::Probit(CDoubleMatrix matrix,int nNRR,int nMaxIterator,double C,bool bFactor,CIntVector vector_factor)
{
	int i=0,j=0,k=0;
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleMatrix X;
	int chNum = 0;
	if( bFactor )
	{
		CIntMatrix intmatrix(nRow,1);
		for(i=0;i<nRow;i++)
		{
			intmatrix(i,0) = vector_factor(i);
		}

		CIntMatrix xf = Zhankai( intmatrix, chNum );
		if( chNum > nRow/2 || chNum>50 )
		{
			CTLTable * pTable = new CTLTable;
	//		pTable->SetRows(1);
	//			pTable->SetCols(1);
	
			pTable->InsertColumn(0,"注： 有效样本数太少或分层数目太多，无法进行回归分析！");
			CRsltVariable * pETable= new CRsltElementTable("注意",pTable);
			m_pResult->Add(pETable);

			return false;
		}
		X = CAlgorithmBase::IntToDouble( xf );
	}
	else
	{
		CDoubleVector vect(nRow);
		for(i=0;i<nRow;i++)
		{
			vect(i) = 1;
		}
		X = X | vect;
		chNum = 1;
	}
	
	CDoubleMatrix matrix_x;

	for(i=0;i<X.mcol();i++)
	{
		matrix_x = matrix_x | X.col(i);
	}

	for(i=0;i<nCol-2;i++)
	{
		matrix_x = matrix_x | matrix.col(i);
	}
	CDoubleMatrix matrix_NR(nRow,2);
	for(i=0;i<nRow;i++)
	{
		matrix_NR(i,0) = matrix(i,nCol-2);
		matrix_NR(i,1) = matrix(i,nCol-1);
	}

	if( nCol > 100 )
		return false;

	double dbStepLimit = 0.1;
	double dbEpsilon = 1e-6;
	
	int cycleNum = 0;

	bool bSingularity(false);

	nCol = matrix_x.mcol();
	
	if( nNRR == 0 )
	{
		CDoubleVector vector_preBeta(nCol);

		CDoubleVector vector_lastBeta(nCol);

		CDoubleMatrix matrix_Hessen(nCol);

		CalcBeta(matrix_x,matrix_NR,vector_preBeta,nNRR,nMaxIterator,dbStepLimit,dbEpsilon,C,vector_lastBeta,matrix_Hessen,cycleNum,bSingularity);

		CDoubleVector vector_t = MatrixToVector( (CDoubleMatrix)(matrix_x * VectorToMatrix(vector_lastBeta)));

		CDoubleVector vector_P = normcdf( vector_t,0,1);

		StatisForProbit( matrix_Hessen,vector_lastBeta, matrix_NR,nNRR,vector_P,nRow,matrix.mcol()-2,chNum,vector_factor);

	}
	else if( nNRR == 1 )
	{
		CDoubleVector vector_preBeta(nCol+1);

		vector_preBeta[0] = -200;//2003-1-26

		CDoubleVector vector_lastBeta(nCol+1);

		CDoubleMatrix matrix_Hessen(nCol+1,nCol+1);

		CalcBeta(matrix_x,matrix_NR,vector_preBeta,nNRR,nMaxIterator,dbStepLimit,dbEpsilon,C,vector_lastBeta,matrix_Hessen,cycleNum,bSingularity);

		CDoubleMatrix matrix_temp(nCol,1);

		int i=0;
		for(i=1;i<nCol+1;i++)
		{
			matrix_temp(i-1,0) = vector_lastBeta[i];
		}

		CDoubleVector vector_t = MatrixToVector( (CDoubleMatrix)(matrix_x * matrix_temp));

		double C = 1.0 / ( 1.0 + exp( - vector_lastBeta[0] / 50 ));

		vector_lastBeta[0] = C;

		CDoubleVector vector_Fx = normcdf( vector_t,0,1);

		CDoubleVector vector_P = C + ( 1-C ) * vector_Fx;

		matrix_Hessen = Hessen(matrix_x,matrix_NR,vector_t,0);
		StatisForProbit( matrix_Hessen,vector_lastBeta, matrix_NR,nNRR,vector_P,nRow,matrix.mcol()-2,chNum,vector_factor);
	}
	else
	{
		CDoubleVector vector_preBeta(nCol);

		CDoubleVector vector_lastBeta(nCol);

		CDoubleMatrix matrix_Hessen(nCol);

		CalcBeta(matrix_x,matrix_NR,vector_preBeta,nNRR,nMaxIterator,dbStepLimit,dbEpsilon,C,vector_lastBeta,matrix_Hessen,cycleNum,bSingularity);

		CDoubleVector vector_t = matrix_x * vector_lastBeta;

		CDoubleVector vector_Fx = normcdf( vector_t,0,1);

		CDoubleVector vector_P = C + (1-C) * vector_Fx;
		StatisForProbit( matrix_Hessen,vector_lastBeta, matrix_NR,nNRR,vector_P,nRow,matrix.mcol()-2,chNum,vector_factor);
	}
	return true;	
}

double * CRegressionProbit::GetData( CDataInterface *pDataInterface, double*& pFactorData, int& nCaseNum )
{
	double * pData = NULL;
	m_strMissField.Empty();
	int i,j;
	TDataSet * pDataArray=NULL;
	int nColNum=0;

	//取回数据，组织输入数据矩阵
	CPtrArray rColArray;
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{
		nColNum=m_tVarInfo.pValue[i];
		if( nColNum<0 )
		{
// 			AfxMessageBox("取数据时出错!");
			return NULL;
		}
		pDataInterface->GetColData( nColNum, pDataArray, 5 );//取回数据
		
		if( pDataArray )
			rColArray.Add( pDataArray );
	}		
	//取观察变量的数据
	if( m_nObserve>=0 )
	{
		pDataInterface->GetColData( m_nObserve, pDataArray, 5 );//取回数据
		if( pDataArray )
			rColArray.Add( pDataArray );
	}
	//取反应变量的数据
	if( m_nResponse>=0 )
	{
		pDataInterface->GetColData( m_nResponse, pDataArray, 5 );//取回数据
		if( pDataArray )
			rColArray.Add( pDataArray );
	}

	//取分层变量的数据
	TDataSet * pFactorArray = NULL;
	if( m_nFactor>=0 )
	{
		pDataInterface->GetColData( m_nFactor, pFactorArray, 5 );//取回数据
	}

	bool * pbMissed = new bool[pDataInterface->GetRowCount()];
	for( i=0; i<pDataInterface->GetRowCount(); i++ )
		pbMissed[i] = false;

	//处理缺失值
	//计算实际样本数
	nCaseNum = 0;
	for( i=0; i<pDataInterface->GetRowCount(); i++ )
	{
		bool bMissed = false;
		for( j=0; j<rColArray.GetSize(); j++ )
		{
			TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(j);
			if( pDataSet[i].iIndex<0 )
			{
				bMissed = true;
				break;
			}
		}

		if( !bMissed )
		{
			TDataSet * pObservData = (TDataSet*)rColArray.GetAt(rColArray.GetSize()-2);
			TDataSet * pResponseData = (TDataSet*)rColArray.GetAt(rColArray.GetSize()-1);
			if( pObservData[i].dValue<=0 || pResponseData[i].dValue<0 || 
				pObservData[i].dValue<pResponseData[i].dValue ||
				(pFactorArray && pFactorArray[i].iIndex<0) )
			{
				bMissed = true;
			}
		}

		if( bMissed )
			pbMissed[i] = true;
		else
			nCaseNum++;
	}

	int nCovNum = m_tVarInfo.iCount;
	if( nCaseNum>0 )
	{
		// 检验数据
		for( i=nCovNum-1; i>=0; i-- )
		{
			TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(i);
			bool bFirst=true, bEqual=true;
			double dFirst;
			for( j=0; j<pDataInterface->GetRowCount(); j++ )
			{
				if( !pbMissed[j] )
				{
					if( bFirst )
					{
						dFirst = pDataSet[j].dValue;
						bFirst = false;
					}
					else
					{
						if( pDataSet[j].dValue!=dFirst )
						{
							bEqual = false;
							break;
						}
					}
				}
			}
			if( bEqual )
			{
				if( !m_strMissField.IsEmpty())
					m_strMissField += ",";
   				m_strMissField += pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				m_tVarInfo.RemoveAt(i);
				nCovNum--;
				delete[] (TDataSet*)rColArray.GetAt(i);
				rColArray.RemoveAt(i);
			}
		}
	}

	//给数据矩阵赋值
	//得到删除缺失值后的权重数组
	if( nCaseNum>0 && nCovNum>0)
	{
		pData = new double[(nCovNum+2)*nCaseNum];
		if( m_nFactor>=0 )
			pFactorData = new double[nCaseNum];

		int nCount=0;
		for( i=0; i<pDataInterface->GetRowCount(); i++ )
		{
			if( pbMissed[i] )
				continue;

			for( j=0; j<nCovNum+2; j++ )
			{
				TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(j);
				pData[j+nCount*(nCovNum+2)] = pDataSet[i].dValue;
			}
			if( m_nFactor>=0 )
				pFactorData[nCount] = pFactorArray[i].dValue;

			nCount++;
		}
	}

	delete[] pbMissed;
	for( i=0; i<rColArray.GetSize(); i++ )
		delete[] (TDataSet*)rColArray.GetAt(i);
	if( pFactorArray )
		delete[] pFactorArray;

	return pData;
}

double * CRegressionProbit::GetDataForProbit(CDataInterface *pDataInterface)
{
	//取回因变量的数据
	int nCaseNum = pDataInterface->GetRowCount();
	int nCaseNumb = 0;
	int m = m_tVarInfo.iCount+2;
	double * pData = new double [m * nCaseNum];

	double * pDataCov    = CAlgorithmBase::GetData(pDataInterface,m_tVarInfo,true);

	if( !pDataCov )
	{
		delete [] pDataCov;
		delete [] pData;
		return NULL;
	}	

	double * pDataObserve = GetDataSingle(pDataInterface,m_nObserve);

	if( !pDataObserve )
	{
		delete [] pDataObserve;
		delete [] pData;
		delete [] pDataCov;
		return NULL;
	}

	int i=0;
	for(i=0;i<nCaseNum;i++)
	{
		if( IsZero(pDataObserve[i]) )
		{
			delete [] pData;
			delete [] pDataObserve;
			return NULL;
		}
	}

	double * pDataResponse= GetDataSingle(pDataInterface,m_nResponse);

	if( !pDataResponse )
	{
		delete [] pDataResponse;
		pDataResponse = NULL;
	}	

	for(i=0;i<nCaseNum;i++)
	{
		for(int j=0;j<m_tVarInfo.iCount;j++)
		{
			pData[i*m+j] = pDataCov[ i*m_tVarInfo.iCount + j];
		}
		pData[i*m+m_tVarInfo.iCount] = pDataObserve[i];
		pData[i*m+m_tVarInfo.iCount+1] = pDataResponse[i];	
	}

	delete [] pDataObserve;
	delete [] pDataResponse;
	delete [] pDataCov;
	return pData;
}

void CRegressionProbit::InitParma()
{
	m_bStopCalculate = false;
	m_strErrorInfo = _T("");
}

CIntMatrix CRegressionProbit::Zhankai(CIntMatrix x, int& nCategoryNum)
{

	int n = x.mrow();
	int nx = x.mcol();

	CTArray<int, int> ar_ngroups;
//	CPtrArray par_dfg;
//	CIntMatrix a(n,0);
//	CIntMatrix mat(n,0);
	CIntMatrix a;
	CIntMatrix mat;
	
	int i=0;
	for(i=0;i<nx;i++)
	{
		CIntVector gi;//,gn;
		//int gn=0;

		CIntVector temp(n);
		for(int j=0;j<n;j++)
		{
			temp[j] = (int)x(j,i);
		}
//		WriteData(temp,"temp");

		int gn = GrpToIdx(temp,gi);

//		WriteData(gi,"gi");
//		WriteData(gn,"gn");

		nCategoryNum += gn;//.vlen();

		if( nCategoryNum>50 )
			return mat;

		ar_ngroups.Add(gn);// .vlen());

		a = a | gi;

//		WriteData(a,"a");
	}


	for(i=0;i<nx;i++)
	{
		int groupnum = ar_ngroups.GetAt(i);

//		WriteData(a,"a");

		CIntMatrix temp = GetYaMatirx(a.col(i),ar_ngroups.GetAt(i));

//		WriteData(temp,"temp");

		for(int j=0;j<temp.mcol();j++)
		{
			mat = mat | temp.col(j); 
		}
	}
//	WriteData(mat,"mat");
	return mat;
}

void CRegressionProbit::CalcBeta(CDoubleMatrix matrix_X,CDoubleMatrix matrix_NR,CDoubleVector vector_preBeta,int nNRR,int nMaxIterator,double StepLimit,double Ep,int C,CDoubleVector &vector_passBeta,CDoubleMatrix &matrix_Hessen,int &cycleNum,bool &bSingularity)
{
	int nRow = matrix_X.mrow();
	int nCol = matrix_X.mcol();
	int k(0);
	bool daltold = 1;
	
	if( nNRR == 1 )
	{
		CDoubleVector vector_dalt(nCol+1);
		int i = 0;
		for(i = 0;i<nCol+1;i++)
		{
			vector_dalt[i] = 1;
		}
		while( !( k > nMaxIterator || abs(vector_dalt).Max() < Ep ))
		{			
			double c = vector_preBeta[0];

			CDoubleMatrix matrix_temp(nCol,1);

			for(i=0;i<nCol;i++)
			{
				matrix_temp(i,0) = vector_preBeta[1+i];
			}

			CDoubleVector vector_t = MatrixToVector( (CDoubleMatrix)(matrix_X * matrix_temp) ) ;

			CDoubleMatrix matrix_g(nCol+1,1);

			matrix_Hessen = HessenMat(matrix_X,matrix_NR,vector_t,nNRR,c,matrix_g);
		
			if( cond( matrix_Hessen ) > 9e14 )
			{
				bSingularity = true;
				break;
			}	

			vector_passBeta = vector_preBeta - MatrixToVector( (CDoubleMatrix)(pinv( matrix_Hessen ) * matrix_g * StepLimit ));

			CDoubleVector vector_daltBeta = vector_passBeta - vector_preBeta;

			double daltnew = 1.0 / ( 1.0 + exp( -vector_passBeta[0] / 50.0 ));

			vector_dalt[0] = daltnew - daltold;

			for(i=1;i<nCol+1;i++)
			{
				vector_dalt[i]  = vector_daltBeta[i];
			}

			vector_preBeta = vector_passBeta;

			daltold = daltnew;
			
			k++;
		}
	}
	else
	{
		CDoubleVector vector_dalt(nCol);

		int i=0;
		for(i=0;i<nCol;i++)
		{
			vector_dalt[i] = i+1;
		}

		while( !( k > nMaxIterator || abs(vector_dalt).Max() < Ep ))
		{
			CDoubleVector vector_t = MatrixToVector( (CDoubleMatrix)(matrix_X * VectorToMatrix( vector_preBeta )));

			CDoubleMatrix matrix_g(nCol+1,1);

			matrix_Hessen = HessenMat(matrix_X,matrix_NR,vector_t,nNRR,C,matrix_g);

			double temp = cond( matrix_Hessen );
			//TRACE("cond = %f \n",temp);
			if( temp > 9e14 )
			{
				bSingularity = true;
				break;
			}

			vector_passBeta = vector_preBeta - MatrixToVector( (CDoubleMatrix)(inv( matrix_Hessen ) * matrix_g * StepLimit));

			vector_dalt = vector_passBeta - vector_preBeta;

			vector_preBeta = vector_passBeta;

			k++;
		}
	}	
}

int CRegressionProbit::GrpToIdx(CIntVector vect, CIntVector &vect1)
{
//	CIntVector vect_sort = SortVector(vect);
//	CIntVector t1,t2;
//	grp2idx(vect_sort,t1,t2);
///////////////////////////////////////////////
	int i, j;
	
	CTArray<int, int> arSort;//v2排序前.        
	
	for( i=0; i<vect.vlen(); i++ )
	{
		bool bInArray = false;
		for( j=0; j<arSort.GetSize(); j++ )
		{
			if( vect(i)==arSort[j] )
			{
				bInArray = true;                                
				break;
			}
			else if( vect(i)<arSort[j] )
			{
				arSort.InsertAt( j, vect(i) );
				bInArray = true;                                
				break;
			}
		}
		if( !bInArray )
		{
			arSort.Add(vect(i));        
		}
	}
	
///////////////////////////////////////////////
//	vect2 = copy(t2);

	CIntVector vect_result(vect.vlen());

	for( i=0; i<vect.vlen(); i++ )
	{
/*		for( j=0; j<arSort.GetSize(); j++ )
		{
			if( vect[i] == arSort[j] )
			{
				vect_result[i] = j+1;
				break;
			}
		}		
*/	
		vect_result[i] = QuickFind( arSort, vect[i], 0, arSort.GetSize()-1 ) + 1;
	}
	//vect1 = copy( vect_result );
	vect1 = vect_result;

	return arSort.GetSize();
}

int CRegressionProbit::QuickFind( CTArray<int, int>& arSort, int nData, int iBegin, int iEnd )
{
	if( iBegin>iEnd )
		return -1;

	int iHalf = (iBegin + iEnd)/2;

	if( nData==arSort[iHalf] )
		return iHalf;	
	else if( nData>arSort[iHalf] )
		return QuickFind( arSort, nData, iHalf+1, iEnd );
	else
		return QuickFind( arSort, nData, iBegin, iHalf-1 );
}

CIntMatrix CRegressionProbit::GetYaMatirx(CIntVector vect,int num)
{
	CIntMatrix matrix(vect.vlen(),num);
	for(int i=0;i<vect.vlen();i++)
	{
		for(int j=1;j<=num;j++)
		{
			//TRACE("%d\n",vect[i]);
			if( vect[i] == j )
			{
				matrix(i,j-1) = 1;
				break;
			}
		}
	}
	return matrix;
}

CDoubleMatrix CRegressionProbit::HessenMat(CDoubleMatrix X,CDoubleMatrix NR,CDoubleVector tVec,int NRR,double C,CDoubleMatrix &g)
{
	int row = X.mrow();
	int col = X.mcol();
	
	int cons=50;

	CDoubleVector Fx = normcdf( tVec , 0 , 1 );

	CDoubleVector P(Fx.vlen());
	
	if( NRR==0 || NRR==2 )
	{
		if(NRR==0)

            C=0;

		P=C+(1-C)*Fx;  

		CDoubleVector FF=exp(- pow( tVec,2.0 ) / 2.0 ) / sqrt( 2 * pi );

		CDoubleVector pre = ( 2.0 * NR.col(1) * P - NR.col(0) * ( P * P ) - NR.col(1) ) / ( ( P- P * P ) * ( P - P * P ) );

		CDoubleVector pos = ( NR.col(1) - NR.col(0) * P ) / ( P - P * P );

		CDoubleVector Lbj= (1-C) * pos * FF;

		g = trans( X ) * VectorToMatrix(Lbj);
		
		CDoubleVector A= ( 1 - C * C ) * pre * FF * FF - ( 1 - C ) * pos * tVec * FF;  

		//A = diag( A );
		CDiagMatrix<double> diagmatrix(A);//#_S 2004-11-16 $ 16 zlq
		CDoubleMatrix HM = trans( X ) * diagmatrix * X;

		return HM;
	}
	else  
	{		
		double PX=1+exp(C/cons);

		CDoubleVector P = (exp(C/cons)+Fx)/PX;

		double Lc=0;
		
		CDoubleVector Lbj(col);

		CDoubleMatrix HM(col+1,col+1);
		
		CDoubleVector PC= exp( C / cons ) * ( 1.0 - Fx ) / ( cons * PX * PX );

		CDoubleVector FF=exp( -0.5 * tVec * tVec ) / ( sqrt( 2*pi ) * PX ) ;

		int i=0;
		for(i=0;i<row;i++)
		{
            double Pre=( 2.0 * NR(i,1) * P(i) - NR(i,1) - NR(i,0) * P(i) * P(i)) / pow( P(i)-P(i)*P(i) , 2.0 );

			double Beh=( NR(i,1)-NR(i,0) * P(i) ) / ( P(i) * ( 1 - P(i) ));
		
			Lc = Lc + Beh * PC(i);
		
			HM(0,0) += Pre * PC( i ) * PC( i ) + Beh * exp ( C / cons ) * ( 1-  exp(C/cons) ) * ( 1 - Fx(i) ) / ( cons * cons * pow(PX,3.0) );
							
			for(int j=0;j<col;j++)
			{
				double PBj = FF(i) * X(i,j);

				Lbj(j) = Lbj(j) + Beh*PBj;

				HM(0,j+1) = HM(0,j+1) + Pre * PC(i) * PBj - Beh * PBj * exp( C / cons ) / ( cons * PX );

				HM(j+1,0) = HM(0,j+1);

				for(int k=0;k<col;k++)
				{

					HM(j+1,k+1) = HM(j+1,k+1) + ( Pre*FF(i) - Beh * tVec(i) ) * FF(i) * X(i,j) * X(i,k);

				}
			}
		}      
		
		g(0,0) = Lc;

		for(i=0;i<Lbj.vlen();i++)
		{
			g(i+1,0) = Lbj(i);
		}

		return HM;
	}
}

CIntVector CRegressionProbit::SortVector(CIntVector vect1)
{
	CIntVector vect = copy(vect1);
	int nLen = vect.vlen();
	int temp;
	for(int i=0;i<nLen-1;i++)
	{
		for(int j=0;j<nLen-1;j++)
		{
			if( vect[j] >= vect[j+1] )
			{
				temp      = vect[j];
				vect[j]   = vect[j+1];
				vect[j+1] = temp;
			}
		}
	}
	return vect;
}

void CRegressionProbit::StatisForProbit(CDoubleMatrix Hessen,CDoubleVector lastBetaVector,CDoubleMatrix NR,int NRR,CDoubleVector P,int row,int col,int nNumFactor,CIntVector vect_factor)
{
	CDoubleVector SE = sqrt(diag(-inv(Hessen)));

	CDoubleVector CoeffSE=lastBetaVector/SE;

	CDoubleVector E= NR.col(0) * P;

	CDoubleVector Res = NR.col(1) - E;

	double Chi = sum( ( NR.col(1) - E ) * ( NR.col(1) - E ) / ( E * ( 1.0 - P ) ) );
	
	int DF(0);

/*	if(NRR==1)

		DF=row-col;

	else

		DF=row-col+1;*/
	if(NRR==1)
	{
		if (nNumFactor>=2)                       
			DF=row*(nNumFactor-1)-(col+nNumFactor+1);
		else
			DF=row-(col+nNumFactor+1);
	}
	else
	{
		if (nNumFactor>=2)
			DF=row*(nNumFactor-1)-(col+nNumFactor);
		else
			DF=row-(col+nNumFactor);
	}

	double Pro=1- chi2cdf(Chi,DF);	
	
	CTLTable * pTable = new CTLTable;
	CRsltVariable * pETable= new CRsltElementTable("回归系数",pTable);
	m_pResult->Add(pETable);

	pTable->SetTitle("回归系数");
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"系数");
	pTable->InsertColumn(2,"标准误");
	pTable->InsertColumn(3,"系数/标准误");	
	int j = 0;
	if( NRR == 1)
	{
		pTable->InsertItem(j++,"自然响应率");
	}

	int k=0;
	for(k=0;k<nNumFactor;k++)
	{	
		CTString str;
		if( m_nFactor >= 0 )
			str.Format("常数项( %s = %d )",m_pDataInterface->GetFieldName( m_nFactor ).GetData(),k);
		else
			str = _T("常数项");
		pTable->InsertItem(j++,str);
	}

	int i=0;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{		
		CTString str = m_pDataInterface->GetFieldName( m_tVarInfo.pValue[i] );		
		pTable->InsertItem(j++,str);
	}

	for(i=0;i<SE.vlen();i++)
	{
		pTable->SetItemText(i,1,lastBetaVector[i]);
		pTable->SetItemText(i,2,SE[i]);
		pTable->SetItemText(i,3,CoeffSE[i]);		
	}
	CTString strTail,str;
	str.Format("卡方 = %10.6f   自由度 = %d   显著性 = %10.6f",Chi,DF,Pro);
	pTable->SetTail(str);

	if(briefrTable)
	{
	if (Chi<=0.05)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型拟和效果不好");
	else
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型拟和效果良好");
	}

	CDoubleMatrix matrix;
	if( m_nFactor >= 0 )
	{
		matrix = matrix | IntToDouble(vect_factor) ;
	}

	matrix = matrix | NR.col(0);
	matrix = matrix | NR.col(1);
	matrix = matrix | E;
	matrix = matrix | Res;

	SortMatrix1(matrix);

	pTable = new CTLTable;
	pETable= new CRsltElementTable("期望频数和残差",pTable);
	m_pResult->Add(pETable);

	pTable->SetTitle("期望频数和残差");

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"观测总数");
	pTable->InsertColumn(2,"响应频数");
	pTable->InsertColumn(3,"期望频数");
	pTable->InsertColumn(4,"残    差");

	if( m_nFactor < 0 )
	{
		for(i=0;i<NR.mrow() ;i++)
		{
			str.Format("%d",i);
			pTable->InsertItem(i,str);
			pTable->SetItemText(i,1,NR(i,0));
			pTable->SetItemText(i,2,NR(i,1));
			pTable->SetItemText(i,3,E[i]);
			pTable->SetItemText(i,4,Res[i]);
		}
	}
	else
	{
		for(int i=0;i<matrix.mrow();i++)
		{
			int nt = matrix(i,0);
			str.Format("%d",nt);
			pTable->InsertItem(i,str);
			for(int j=1;j<matrix.mcol();j++)
			{
				pTable->SetItemText(i,j,matrix(i,j));
			}
		}
	}	
} 
