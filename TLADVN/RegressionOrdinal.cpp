// RegressionOrdinal.cpp: implementation of the CRegressionOrdinal class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "RegressionOrdinal.h"

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

CRegressionOrdinal::CRegressionOrdinal()
{
	m_nDependIndex  = -1;
	m_pDataInterface= NULL;
//	m_bProcessExcept= true;
//	m_fC			= 0.01f;
//	this->m_nIterator = 150;
//	this->m_nNRR = 0;
//	m_nResponse = -1;
//	m_nFact	= -1;
//	m_nObserve = -1;
//	m_nFactor = -1;
//	m_nMethod = 0;
//	m_nMissMethod  = 0;
}

CRegressionOrdinal::~CRegressionOrdinal()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
	if( m_tVarInfo_Cov.iCount>0 )
		m_tVarInfo_Cov.DeleteObject();
}

int CRegressionOrdinal::GetData(CDataInterface *pDataInterface, double*& pWeight,double*& pDependData, double*& pFactorData,  double*& pCovData, int& nCaseNum )
{
	m_strMissField.Empty();
	//取回数据，组织输入数据矩阵
	int i,j,m = m_tVarInfo.iCount +1;
	TDataSet * pDataArray=NULL;
	int nColNum=0;
	bool nDataError = false;
	int nResult = 0;	//返回取数据操作错误代码，0表示成功

	CPtrArray rColArray;
	m = m_tVarInfo.iCount + 1;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		nColNum=m_tVarInfo.pValue[i];
		if( nColNum < 0 )
		{
			nResult = 1;
			break;;
		}
		pDataInterface->GetColData(nColNum,pDataArray,5);//取回数据
		
		if( pDataArray )
			rColArray.Add( pDataArray );
	}		


	//取回因变量的数据
	if(m_nDependIndex>=0)
	{
		pDataInterface->GetColData(m_nDependIndex,pDataArray,5);//取回数据
		if( pDataArray )
			rColArray.Add( pDataArray );
	}
	//取协变量的数据
	if( m_tVarInfo_Cov.iCount )
	{
		for(i=0;i<m_tVarInfo_Cov.iCount;i++)
		{
			nColNum=m_tVarInfo_Cov.pValue[i];
			if( nColNum < 0 )
			{
				nResult = 1;
				break;;
			}
			pDataInterface->GetColData(nColNum,pDataArray,5);//取回数据
			
			if( pDataArray )
				rColArray.Add( pDataArray );
		}		
	}

	if( nResult )
	{
		for( i=0; i<rColArray.GetSize(); i++ )
			delete[] (TDataSet*)rColArray.GetAt(i);

		return nResult;
	}

	//取回权重列数据
	double * pWeightOrg = new double[pDataInterface->GetRowCount()];
	if( pDataInterface->GetWeightNumb()>-1 )
	{
		pDataInterface->GetWeightData( pWeightOrg );
	}
	else
	{
		for( i=0; i<pDataInterface->GetRowCount(); i++ )
		{
			pWeightOrg[i] = 1;
		}
	}

	//处理缺失值
	//计算实际样本数
	nCaseNum = 0;
	for( i=0; i<pDataInterface->GetRowCount(); i++ )
	{
		if( pWeightOrg[i]==0 )
			continue;

		for( j=0; j<rColArray.GetSize(); j++ )
		{
			TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(j);
			if( pDataSet[i].iIndex<0 )
				break;
		}

		if( j<rColArray.GetSize() )
			pWeightOrg[i] = 0;
		else
			nCaseNum++;
	}

	if( nCaseNum==0 )
	{
		nResult = 4;

		delete[] pWeightOrg;

		for( i=0; i<rColArray.GetSize(); i++ )
			delete[] (TDataSet*)rColArray.GetAt(i);

		return nResult;
	}

	// 检验数据
	int nVarNum = m_tVarInfo.iCount, nCovNum = m_tVarInfo_Cov.iCount;
	for( i=rColArray.GetSize()-1; i>=0; i-- )
	{
		TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(i);
		bool bFirst=true, bEqual=true;
		double dFirst;
		for( j=0; j<pDataInterface->GetRowCount(); j++ )
		{
			if( pWeightOrg[j]>0 )
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
			if( i>m_tVarInfo.iCount )
			{
     			m_strMissField += pDataInterface->GetFieldName(m_tVarInfo_Cov.pValue[i-m_tVarInfo.iCount-1]);
				m_tVarInfo_Cov.RemoveAt(i-m_tVarInfo.iCount-1);
				nCovNum--;
			}
			else if( i==m_tVarInfo.iCount )
			{
				nResult = 2;		//因变量为常量
				break;
			}
			else 
			{
				m_strMissField += pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				m_tVarInfo.RemoveAt(i);
				nVarNum--;

				if( nVarNum==0 )
					nResult = 3;
			}
			delete[] (TDataSet*)rColArray.GetAt(i);
			rColArray.RemoveAt(i);
		}
	}

	//给数据矩阵赋值
	//得到删除缺失值后的权重数组
	if( nCaseNum>0 && !nResult )
	{
		pFactorData = new double[nVarNum*nCaseNum];
		pDependData = new double[ nCaseNum ];
		pWeight = new double[ nCaseNum ];
		if( m_tVarInfo_Cov.iCount )
			pCovData = new double[nCovNum*nCaseNum];

		int nCount=0;
		for( i=0; i<pDataInterface->GetRowCount(); i++ )
		{
			if( pWeightOrg[i]<=0 )
				continue;

			//分层量矩阵
			for( j=0; j<nVarNum; j++ )
			{
				TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(j);
				pFactorData[j+nCount*nVarNum] = pDataSet[i].dValue;
			}
			//因变量数组
			TDataSet * pDependSet = (TDataSet*)rColArray.GetAt(j);
			pDependData[nCount] = pDependSet[i].dValue;
			//权重数组
			pWeight[nCount] = pWeightOrg[i];
			//协变量矩阵
			if( nCovNum )
			{
				for(  j=0; j<nCovNum; j++ )
				{
					TDataSet * pDataSet = (TDataSet*)rColArray.GetAt(j+nVarNum+1);
					pCovData[j+nCount*nCovNum] = pDataSet[i].dValue;
				}
			}
			nCount++;
		}
	}

	delete[] pWeightOrg;
	for( i=0; i<rColArray.GetSize(); i++ )
		delete[] (TDataSet*)rColArray.GetAt(i);

	return nResult;
}

BOOL CRegressionOrdinal::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	REGORDINAL
//		/DATA=[FILE]
//		/DEPENDENT [varlist]
//		/INDEPENDENT [varlsit]
//		/COVARIATE [varlist]
	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(yname);
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	CTStringArray cname;
	AnaSyntax::getVAR("/COVARIATE", VOstr, cname);
	if (!(pDataInterface->getVarID(cname,m_tVarInfo_Cov)))
		return FALSE;
	return TRUE;

}


CResult * CRegressionOrdinal::OnOrdinal(CDataInterface *pDataInterface, string VO)
{
	InitParma();
	m_pDataInterface = pDataInterface;
	m_pResult = NULL;
	m_pResult = new CResult("有序回归");
	if (!(GetValueFromVO(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	briefrTable=NULL;
	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{			
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}
	strVar=pDataInterface->GetFieldName(m_nDependIndex);
	//
	InitParma();
	double * pWeight = NULL, *pCovData = NULL, *pDependData = NULL, *pFactorData = NULL;
	int nCaseNum=0;

	int nResult = GetData(pDataInterface, pWeight, pDependData, pFactorData, pCovData, nCaseNum);

	switch( nResult )
	{
	case 1:
		{
			CTString strWarning = "注： 读取记录出错，回归分析终止。";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return m_pResult;
		}
		break;
	case 2:
		{
			CTString strWarning = "注： 因变量为常数，不能进行回归分析。";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return m_pResult;
		}
		break;
	case 3:
		{
			CTString strWarning = "注： 所有分层量均为常数，不能进行回归分析。";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return m_pResult;
		}
		break;
	case 4:
		{
			CTString strWarning = "注： 有效样本数为0，不能进行回归分析。";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return m_pResult;
		}
		break;

	}

	if( !m_strMissField.IsEmpty() )
	{
		CTString strWarning = "注： 变量 " + m_strMissField + " 为常数，不适合进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}// zxg ## 2003-2-12

	int m = m_tVarInfo.iCount + 1;
	if( nCaseNum<=m)
	{
		CTString strWarning = "注： 有效记录数太少，无法进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	CDoubleMatrix matrix_factor(pFactorData,nCaseNum,m_tVarInfo.iCount);
	CDoubleVector vector_weight( pWeight, nCaseNum );
	CDoubleVector vector_dependent( pDependData, nCaseNum );
	bool bFlag(false);
	if( pCovData )
	{
		CDoubleMatrix matrix_cov(pCovData,nCaseNum,m_tVarInfo_Cov.iCount);
		delete[] pCovData;
		pCovData = NULL;

		bFlag = UseOrdinal(matrix_factor,vector_dependent,m_tVarInfo_Cov.iCount,vector_weight,matrix_cov);
	}
	else
		bFlag = UseOrdinal(matrix_factor,vector_dependent,0,vector_weight);

	//add by liytao 20030813
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

void CRegressionOrdinal::InitParma()
{
	m_bStopCalculate = false;
	m_strErrorInfo = _T("");
}

void CRegressionOrdinal::Ordinal_statis(CDoubleVector vector_lastBeta,CDoubleMatrix matrix_inform,int Jnum,int varNum,CTStringArray * parString)
{
	if( !m_strErrorInfo.IsEmpty() )
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( m_strErrorInfo );
		m_pResult->Add( pWarningTextRslt );
	}

	CDoubleVector SE = sqrt( diag( pinv( (CDoubleMatrix)(0.0-matrix_inform) ) ));
	
	CDoubleVector W = ( vector_lastBeta / SE ) * ( vector_lastBeta / SE );

	CDoubleVector P = 1.0 - chi2cdf(W,1);
	
	CDoubleVector W1 =( vector_lastBeta / sqrt( diag( matrix_inform ))) * ( vector_lastBeta / sqrt( diag( matrix_inform ))) ;

	CIntVector dfg(Jnum + varNum - 1);

	int i=0;
	for(i=0;i<Jnum + varNum - 1;i++)
	{
		dfg[i] = 1;
	}

	CDoubleVector Lbound = vector_lastBeta - norminv(1-0.025,0,1) * SE;

	CDoubleVector Ubound = vector_lastBeta + norminv(1-0.025,0,1) * SE;

	WriteData(Lbound,"lbound");
	
	//TRACE("ROW = %d \n",vector_lastBeta.vlen()+1);

	CTLTable * pTable = new CTLTable;

	pTable->SetTitle("参数估计");

	pTable->InsertColumn(0,"");

	pTable->InsertColumn(1,"估计值");

	pTable->InsertColumn(2,"标准差");

	pTable->InsertColumn(3,"沃尔德");

	pTable->InsertColumn(4,"自由度");

	pTable->InsertColumn(5,"显著性");	

	pTable->InsertColumn(6,"95%置信区间\n下界");

	pTable->InsertColumn(7,"95%置信区间\n上界");

	CTString str;

	for(i=0;i<vector_lastBeta.vlen();i++)
	{
		pTable->InsertItem(i,parString->GetAt(i));//  "");
		pTable->SetItemText(i,1,vector_lastBeta[i]);
		pTable->SetItemText(i,2,SE[i]);
		pTable->SetItemText(i,3,W[i]);
		pTable->SetItemText(i,4,dfg[i]);
		pTable->SetItemText(i,5,P[i]);
		pTable->SetItemText(i,6,Lbound[i]);
		pTable->SetItemText(i,7,Ubound[i]);

		CTString strVars,strInfl;
		strVars="在"+parString->GetAt(i)+"时"+"对"+strVar;
		CTString strVal;
		strVal.Format("(概率检验值=%f)",P[i]); 
		if(P[i]<=0.05)
			strInfl="产生的影响较大";
		else
			strInfl="产生的影响较小";
		if(briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型中"+strVars+strInfl+strVal);
	}

	CRsltVariable * pETable= new CRsltElementTable("参数估计",pTable);

	m_pResult->Add(pETable);
}

CDoubleMatrix CRegressionOrdinal::OrdinalRe(CDoubleMatrix matrix_x, CIntMatrix vector_y, CDoubleVector vector_preBeta, int Jnum, int varNum, double epsilon, CDoubleVector &vector_lastBeta,CDoubleVector weight)
{
	double error = 1e100;

	int cycleNum = 1;

	CDoubleVector vector_passBeta(vector_preBeta.vlen());

	CDoubleMatrix matrix_inform(Jnum-1+varNum,Jnum-1+varNum);

	m_bStopCalculate = false;

	epsilon = 0.0001;

	CDoubleMatrix oldinfo;
	while( error > epsilon )
	{
		vector_passBeta = GetBetaVector_Ordinal(matrix_x,vector_y,vector_preBeta,Jnum,varNum,matrix_inform,weight);
		
		double a=det(matrix_inform);
		if(fabs(a)<DOUBLE_ZERO)
		{
			m_bStopCalculate = true;
			break;
		}


		vector_passBeta = vector_preBeta - ldiv(matrix_inform,vector_passBeta);
	
		error =  Max(abs((CDoubleVector)(( vector_passBeta - vector_preBeta ) / vector_passBeta )));
		vector_preBeta = vector_passBeta;
		oldinfo=matrix_inform;
		cycleNum++;
	}
	matrix_inform=oldinfo;

	if( m_bStopCalculate )
	{
		m_strErrorInfo += "注意： 极大似然估计不存在，输出结果可能不正确！";
		if( cycleNum>1 )
			m_bStopCalculate = false;
	}
	
	vector_lastBeta = vector_passBeta;
	return matrix_inform;
}

bool CRegressionOrdinal::UseOrdinal(CDoubleMatrix matrix_f,CDoubleVector vector_Y,int Num_cov,CDoubleVector weight,CDoubleMatrix matrix_cov)
{

	CTStringArray arString;

	CTString str;

	int nRow = matrix_f.mrow() ;
	
	int nCol = matrix_f.mcol() + Num_cov + 1;

	int Num_f = matrix_f.mcol();

	CDoubleMatrix test;

	int i=0;
	for(i=0;i<Num_f;i++)
	{
		test = test | matrix_f.col(i);
	}
	for(i=0;i<Num_cov;i++)
	{
		test = test | matrix_cov.col(i);
	}

	CDoubleMatrix newX;
	CDoubleMatrix newY;
	CIntMatrix matrix_classset;
	CDoubleMatrix test_Backup( test );
	CDoubleVector vectorY_Backup( vector_Y );
	matrix_classset = GetClassSet( DoubleToInt(vector_Y) );

	CDoubleVector tt = CDoubleVector(vector_Y);

	test = test | (CVector<double>)(tt);

	double epsilon = 1e-6;

	CDoubleMatrix xMatrix;

	CIntMatrix xTemp;
	int nCategoryNum = 0;
	int m = 1;

	if( Num_f > 0 )
	{
		int i=0;
		for(i=0;i<Num_f;i++)
		{
			CDoubleVector ttt(matrix_f.col(i));

			CIntMatrix mmm(ttt.vlen(),1);
			int ii=0;
			for(ii=0;ii<ttt.vlen();ii++)
			{
				mmm(ii,0) = (int)ttt[ii];
			}

			CIntMatrix matrix_xf = Zhankai( mmm, nCategoryNum );

			if( nCategoryNum > 50 )
				break;

			int rf = matrix_xf.mrow();
			int cf = matrix_xf.mcol();

			int j=0;
			for(j=0;j<cf-1;j++)
			{
				xMatrix  = xMatrix | IntToDouble(matrix_xf.col(j));
			}
			for(j=0;j<cf;j++)
			{
				xTemp = xTemp | matrix_xf.col(j);
			}
		}
	}

	if( nCategoryNum>50 )
	{
		CTString strWarning = "注： 分层数量太大，没有足够的内存空间进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;
	}

	if( xMatrix.mrow() < xMatrix.mcol() )
	{
		CTString strWarning = "注： 样本数少于分层数，无法进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;
	}

	if(Num_cov > 0)
	{
		xMatrix = xMatrix | matrix_cov;
	}

	CIntVector vector_temp_y(vector_Y.vlen());

	for(i=0;i<vector_Y.vlen();i++)
	{
		vector_temp_y[i] = (int)vector_Y[i];
	}

	for(i=0;i<matrix_classset.mcol()-1;i++)
	{
		str.Format("%s = %d",m_pDataInterface->GetFieldName(m_nDependIndex).GetData(),matrix_classset(0,i));
		arString.Add(str);
	}
	
	for(i=0;i<matrix_f.mcol();i++)
	{
		CIntMatrix matrix_class_probit = GetClassSet(DoubleToInt(matrix_f.col(i)));
		for(int j=0;j<matrix_class_probit.mcol()-1;j++)
		{
			str.Format("%s = %d",m_pDataInterface->GetFieldName( m_tVarInfo.pValue[i] ).GetData(),matrix_class_probit(0,j));
			arString.Add(str);
		}
	}

	for(i=0;i<this->m_tVarInfo_Cov.iCount;i++)
	{
		arString.Add(m_pDataInterface->GetFieldName(m_tVarInfo_Cov.pValue[i]));	
	}

	int Jnum = matrix_classset.mcol();

	int row = xMatrix.mrow();

	int varNum = xMatrix.mcol();

	xMatrix = -xMatrix;

	CIntMatrix vector_y = GetYVector(vector_temp_y,matrix_classset);

	if( vector_y.mcol()>100 )
	{
// 		AfxMessageBox("输入的因变量不正确，无法进行回归分析!");
		return false;
	}

	GetXxx(test_Backup,vectorY_Backup,weight,newX,newY,matrix_classset);

	CDoubleVector vector_preBeta = GetInitialBeta(matrix_classset,varNum);

	CDoubleVector vector_initialBeta = vector_preBeta;

	CDoubleVector vector_lastBeta(vector_preBeta.vlen());

	CDoubleMatrix matrix_inform = OrdinalRe(xMatrix,vector_y,vector_preBeta,Jnum,varNum,epsilon,vector_lastBeta,weight);

	if( m_bStopCalculate )
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( "注： 输入数据不适合进行回归分析，计算终止！" );
		m_pResult->Add( pWarningTextRslt );

		return false;
	}

	Ordinal_statis(vector_lastBeta,matrix_inform,Jnum,varNum,&arString);

	arString.RemoveAll();

	CDoubleVector LH(2);

	double ChiSquare;

	int df;

	double sig;

	ModelFiting(xMatrix,IntMatrixToDouble(vector_y),vector_lastBeta,vector_initialBeta,Jnum,varNum,Num_cov,LH,ChiSquare,df,sig,weight);

	CDoubleMatrix matrix_x,matrix_y;

	CDoubleMatrix X;
	nCategoryNum = 0;
	if( Num_f>0 )
	{
		for(i=0;i<Num_f;i++)
		{
			CIntMatrix xf = Zhankai( VectorToMatrix( DoubleToInt(newX.col(i))), nCategoryNum);
			int rf = xf.mrow();
			int cf = xf.mcol();
			for(int j=0;j<cf-1;j++)
			{
				X = X | IntToDouble( xf.col(j) );
			}
		}		
	}
	if( Num_cov > 0 )
	{
		for( i=Num_f; i<Num_f+Num_cov; i++ )
			X = X | ((CDoubleVector)newX.col(i));
		//X = xMatrix;
	}
	GoodnessFitting((-X),newY,vector_lastBeta,Jnum,varNum);

	int n = 0;

	for(i=0;i<matrix_classset.mcol();i++)
	{
		n += matrix_classset(1,i);
	}

	cal_R(LH,n);

	return true;
}

double * CRegressionOrdinal::GetData(CDataInterface *pDataInterface)
{
	double * pData = NULL;
	m_strMissField.Empty();
	//取回数据，组织输入数据矩阵
	int nCaseNumb,m = m_tVarInfo.iCount +1;
	TDataSet * pDataArray=NULL;
	int nColNum=0;
	bool nDataError = false;
	//数据检验
	int i=0;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		nColNum=m_tVarInfo.pValue[i];
		if( nColNum < 0 )
		{
// 			AfxMessageBox("取数据时出错!");
			return NULL;
		}
		nCaseNumb = pDataInterface->GetColData(nColNum,pDataArray,1);//取回数据
		assert(nCaseNumb > 0 );
		if(IsEqual(pDataArray,nCaseNumb))
		{
			//表示所有数据相同，应该除去
			if( !m_strMissField.IsEmpty())
				m_strMissField += ",";
			m_strMissField += pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
			m_tVarInfo.RemoveAt(i);
			i--;
		}
	}
	// zxg ## 2003-2-12
	// 由于自变量为常量被删除，自变量为0个，无统计的意义
	if( m_tVarInfo.iCount <= 0)
	{
// 		AfxMessageBox("自变量整列数据为常数，您选择的数据没有意义!");
		return NULL;
	}

	m = m_tVarInfo.iCount + 1;
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		if( !pData  )
			pData = new double [pDataInterface->GetRowCount() * m ];
		
		nColNum=m_tVarInfo.pValue[i];
		if( nColNum < 0 )
		{
// 			AfxMessageBox("取数据时出错!");
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
	//取回因变量的数据
	if(m_nDependIndex>=0)
	{
		nCaseNumb = pDataInterface->GetColData(m_nDependIndex,pDataArray,1);//取回数据
		if(nCaseNumb > 0)
		{
			if(!IsEqual(pDataArray,nCaseNumb))
			{				
				for(int j=0;j<nCaseNumb;j++)
					pData[i+j*m] = pDataArray[j].dValue;
			}
			else
				nDataError=true;
			if( pDataArray )
			{
				delete [] pDataArray;
				pDataArray=NULL;
			}
		}
		else
			nDataError=true;
	}		
	if ( m_tVarInfo.iCount <= 0  || nDataError)
	{
// 		AfxMessageBox("你选择的数据无效!");
		return NULL;
	}
	return pData;
}

CIntMatrix CRegressionOrdinal::GetClassSet(CIntVector vector_y1)
{	
	int nRow = vector_y1.vlen();
	//排序
	CIntVector vector_y = SortVector(vector_y1);
	
	int nMin = vector_y[0];
	
	int nMax = vector_y[nRow-1];

	//得到多少不同的数
	int count = 1;
	int i=1;
	for(i=1;i<nRow;i++)
	{
		if( vector_y[i] != vector_y[i-1] )
			count++;
	}
	
	CIntMatrix matrix(2,count);

	int k = 0;
	matrix(0,k) = vector_y[0];
	matrix(1,k) = 1;

	for(i=1;i<nRow;i++)
	{	
		if( vector_y[i] != vector_y[i-1] )
		{
			k++;
			matrix(0,k) = vector_y[i];
			matrix(1,k) = 1;
		}	
		else
		{
			matrix(1,k)++;
		}
	}

	return matrix;
}
CIntMatrix CRegressionOrdinal::Zhankai(CIntMatrix x, int& nCategoryNum)
{
//	WriteData(x,"x");

	int n = x.mrow();
	int nx = x.mcol();

	CTArray<int, int> ar_ngroups;
//	CPtrArray par_dfg;
	CIntMatrix a;
	CIntMatrix mat;
//	CIntMatrix a(n,n,0);
//	CIntMatrix mat(n,n,0);
	
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

		int gn = GrpToIdx(temp,gi);


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

CIntMatrix CRegressionOrdinal::GetYVector(CIntVector vector_y, CIntMatrix matrix_class)
{
	int nRow1 = vector_y.vlen();
	int nCol1 = 1;
	int nRow2 = matrix_class.mrow();
	int nCol2 = matrix_class.mcol();
	assert(nCol2-1>0);
	CIntMatrix matrix_temp(nRow1,nCol2-1);
	/*   注释，vector_y[i]直接拿到下面 //#_S 2004-11-23 $ 10 zlq
	*/
	int j = 0;
	for(int i=0;i<nRow1;i++)
	{
		
		for(j=0;j<nCol2-1;j++)
		{
			matrix_temp(i,j) =( vector_y[i] <= matrix_class(0,j) )?1:0;

		}
	}
	return matrix_temp;	
}

//////////////////////////////////////////////////////////////////////////////////
//
//					      得到初始化的beta值（ordinal回归用）	
//
//////////////////////////////////////////////////////////////////////////////////
CDoubleVector CRegressionOrdinal::GetInitialBeta(CIntMatrix matrix_class,int nvarNum)
{
	int nJnum  = matrix_class.mcol();
	double nscale = 0;
	CDoubleVector vector_temp(nJnum-1 + nvarNum);
	for(int i=0;i<nJnum-1;i++)
	{
		double sum1 =  sum(  matrix_class.row(1) );
		double tm  =  matrix_class(1,i);
		nscale +=  tm / sum1 ;
		vector_temp[i] = log( nscale /(1.0 - nscale));
	}
	return vector_temp;	
}

void CRegressionOrdinal::ModelFiting(CDoubleMatrix X,CDoubleMatrix Y,CDoubleVector betaVector,CDoubleVector initialBetaVector, int Jnum, int varNum,int cNum ,CDoubleVector& LH,double &ChiSquare,int &df,double &sig,CDoubleVector weight)
{
	int row = X.mrow();
            
	int col = X.mcol();

	//CDoubleMatrix LH(2,1);	
		
	CDoubleVector theta(Jnum-1);

	int i=0;
	for(i=0;i<Jnum-1;i++)
	{

		theta[i] = initialBetaVector[i];
		//TRACE("theta = %f \n ",theta[i] );

	}

	double lTemp = 0;
	for(i=0;i<row;i++)
	{
		lTemp = 0;
		int j=0;
		for(j=0;j<Jnum-1-1;j++)
		{
            double btm  =  exp( theta(j+1) ) - exp( theta(j) );
			if(btm>0)
			{

				double A_ij  =  exp( theta(j    ) ) * (  1+exp( theta(j+1))  ) / btm ;

				double A_ijj =  exp( theta(j+1) ) * (  1+exp( theta(j) )  ) / btm ;
		
				//LH(0) = LH(0) + Y( i , j ) * log( A_ij ) - Y( i,j+1 ) * log( A_ijj );
				lTemp += Y( i , j ) * log( A_ij ) - Y( i,j+1 ) * log( A_ijj );
			}
		}
		//TRACE("yi=%f\n",Y(i,Jnum-1-1));
		lTemp =lTemp+  Y( i , Jnum-1-1 ) * ( theta( Jnum - 1-1 )  ) - log( 1 +  exp( theta( Jnum - 1-1 ) ) );
		//LH(0) = LH(0) + Y( i , Jnum-1-1 ) * ( theta( Jnum - 1-1 )  ) - log( 1 +  exp( theta( Jnum - 1-1 ) ) );
		double ad= lTemp * weight(i);
		LH(0) =LH(0)+ ad;//lTemp * weight(i);
		//TRACE("LH(0)=%f\n",LH(0));
	}	


	LH(0)=-2*LH(0);
	//TRACE("%f\n",LH(0));		
	CDoubleVector beta(varNum);

	for(i=0;i<Jnum-1;i++)
	{
		theta[i] = betaVector[i];

	}

	for(i=0;i<varNum;i++)
	{

		beta[i] = betaVector[Jnum + i - 1];

	}	
		
	for(i=0;i<row;i++)
	{
		lTemp = 0;
		int j=0;
		for(j=0;j<Jnum-1-1;j++)
		{
			double btm  =  exp( theta(j+1) ) - exp( theta(j) );

			if(btm>0)
			{

			    double A_ij  =  exp( theta(j) ) * (  1.0 + exp( theta(j+1)+ X.row(i).dot(beta)  )  ) / btm ;

				 double A_ijj =  exp( theta(j+1) ) * (  1.0 + exp( theta(j)+X.row(i).dot(beta) )  ) / btm ;
          
				//LH(1) = LH(1) + Y( i , j ) * log( A_ij ) - Y( i , j+1 ) * log( A_ijj );
				lTemp += Y( i , j ) * log( A_ij ) - Y( i , j+1 ) * log( A_ijj );
			}
		}
		WriteData(X.row(0),"zfm_X.row");
		WriteData(beta,"zfm_beta");
		
		double ddd= theta( Jnum - 1 -1) + X.row(i).dot(beta);
		if( ddd>709 )
			lTemp  += Y( i ,Jnum-1-1 ) * ( theta( Jnum - 1 -1 ) + X.row(i).dot(beta)) - ddd;
		else
			lTemp  += Y( i ,Jnum-1-1 ) * ( theta( Jnum - 1 -1 ) + X.row(i).dot(beta)) - log( 1 +  exp( theta( Jnum - 1 -1) + X.row(i).dot(beta)));
		LH(1) += weight(i) * lTemp;





		//LH(1) = LH(1) + Y( i ,Jnum-1-1 ) * ( theta( Jnum - 1 -1 ) + X.row(i).dot(beta)) - log( 1 +  exp( theta( Jnum - 1 -1) + X.row(i).dot(beta)));

	}
	
	LH(1) = -2*LH(1);

	//TRACE("%f\n",LH(1));
			
	ChiSquare = LH(0) - LH(1);

	df=varNum;	
	
	sig =  1 - chi2cdf(ChiSquare,df);

	CTLTable * pTable = new CTLTable;

	pTable->SetTitle("模型拟合信息");

	pTable->InsertColumn(0,"模型");

	pTable->InsertColumn(1,"－2对数似然");

	pTable->InsertColumn(2,"卡方");

	pTable->InsertColumn(3,"自由度");

	pTable->InsertColumn(4,"显著性");

	pTable->InsertItem(0,"截距");

	pTable->InsertItem(1,"最终");

	pTable->SetItemText(0,1,LH[0]);

	pTable->SetItemText(1,1,LH[1]);

	pTable->SetItemText(1,2,ChiSquare);

	pTable->SetItemText(1,3,df);

	pTable->SetItemText(1,4,sig);

	pTable->SetTail("连接模型 : Logit");

	CRsltVariable * pETable= new CRsltElementTable("模型拟合信息",pTable);

	m_pResult->Add(pETable);

	//add by liytao 20030813
	if(briefrTable)
	{
		if (sig<=0.05)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型有意义");
		else
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型无意义");
	}
	//add end
}

void CRegressionOrdinal::GoodnessFitting(CDoubleMatrix X ,CDoubleMatrix  Y ,CDoubleVector  betaVector , int Jnum ,int varNum)
{
	CDoubleVector Chi_Square(2);

	CIntVector df(2);

	CDoubleVector sig(2);

	int row = X.mrow();

	int col = X.mcol();

	CDoubleVector theta(Jnum-1);

	int i=0;
	for(i=0;i<Jnum-1;i++)
	{
		theta[i] = betaVector[i];
	}

	CDoubleVector beta(varNum);

	for(i=0;i<varNum;i++)
	{
		beta[i] = betaVector[i+Jnum-1];
	}

	CDoubleVector N_i = sum( Y );

	WriteData(N_i,"N_i");	
	
	
	CDoubleMatrix pi_ij(row,Jnum);

	for(i=0;i<row;i++)
	{
		pi_ij(i,0)=exp(theta(0)+ X.row(i).dot(beta) ) / ( 1.0 + exp(theta(0)+X.row(i).dot(beta)));	

		int j=1;
		for(j=1;j<Jnum-1;j++)
		{

			pi_ij(i,j) = exp( theta(j) + X.row(i).dot(beta) ) / ( 1.0 + exp (theta(j)+ X.row(i).dot(beta) )) -  exp(theta(j-1)+X.row(i).dot(beta)) / ( 1 + exp(theta(j-1)+X.row(i).dot(beta)));

		}	

		pi_ij(i,Jnum-1) = 1 - exp( theta(Jnum-1-1)+X.row(i).dot(beta)) / ( 1 + exp(theta(Jnum-1-1)+X.row(i).dot(beta)));

	}
	
	CDoubleVector Ni=sum(Y);
	
	for(i=0;i<row;i++)
	{
		for(int j=0;j<Jnum;j++)
		{

			Chi_Square(0)=Chi_Square(0) + pow(  Y(i , j )-Ni(i)*pi_ij(i,j) , 2.0 ) / ( Ni(i)*pi_ij(i,j) );

			if(Ni(i)>0 && pi_ij(i,j)>0 && Y(i,j)>0)
			{
				
				Chi_Square(1) = Chi_Square(1) + 2*Y(i,j) * log(  Y(i,j) / ( Ni(i)*pi_ij(i,j) )  );
			}
		}
	}			
			
	df(0)=row*(Jnum-1)- ( (Jnum-1)+varNum);

	df(1)=df(0);

	CTLTable * pTable = new CTLTable;
//	pTable->SetRows(3);
//	pTable->SetCols(6);

	pTable->SetTitle("拟合优度");
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"卡方检验");
	pTable->InsertColumn(2,"自由度");
	pTable->InsertColumn(3,"显著性");

	pTable->InsertItem(0,"皮尔逊");
	pTable->InsertItem(1,"离差");

	for(i=0;i<2;i++)
	{
		pTable->SetItemText(i,1,Chi_Square[i]);
		pTable->SetItemText(i,2,df[i]);
		if( df[i]>0 )
			pTable->SetItemText(i,3,1-chi2cdf(Chi_Square[i],df[i]));//  sig[i]);
	}
	pTable->SetTail("连接模型 : Logit");
	CRsltVariable * pETable= new CRsltElementTable("拟合优度",pTable);
	m_pResult->Add(pETable);
	if(briefrTable)
	{
		if( (1-chi2cdf(Chi_Square[0],df[0]))<=0.05)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型拟和效果不好");
		else
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型拟和效果良好");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//						计算beta向量（ordinal回归用）				
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDoubleVector CRegressionOrdinal::GetBetaVector_Ordinal(CDoubleMatrix matrix_x,CIntMatrix matrix_y,CDoubleVector vector_beta,int Jnum,int varNum,CDoubleMatrix& matrix_inform,CDoubleVector weight)
{
	//CTestTime test1;
	int nRow = matrix_x.mrow();
	int nCol = matrix_x.mcol();
	CDoubleVector vector_theta( Jnum-1 );

	int i=0;
	for(i=0;i<Jnum-1;i++)
	{
		vector_theta[i] = vector_beta[i];
	}
	CDoubleVector beta(varNum);
	int j=0;
	for(i=Jnum-1;i<Jnum-1+varNum;i++)
	{
		beta[j++] = vector_beta[i];
	}
	CDoubleMatrix matrix_q(nRow,Jnum - 1 + varNum);

	double tp    = 0;
	double tp0   = 0;
	double temp  = 0;
	double temp0 = 0;
	double temp1 = 0;
	double temp2 = 0;
	int k=0;
	for(k=0;k<Jnum-1+varNum;k++)
	{
		for(i=0;i<nRow;i++)
		{
			double qTemp = 0;
			for(j=0;j<Jnum-2;j++)
			{
				if( k >= 0 && k < Jnum -1 )
				{
					temp0 = delta(j+1,k) * exp( vector_theta[j+1] ) - delta( j,k) * exp( vector_theta[j] );
				}
				else
				{
					temp0 = 0;
				}
				temp0 /= ( exp( vector_theta[j+1] ) - exp( vector_theta[j] ) );
				temp   = exp( vector_theta[j+1] + matrix_x.row(i).dot(beta) ) 
					/ ( 1.0 + exp( vector_theta[j+1] + matrix_x.row(i).dot(beta) ) );
				double a_ij   = matrix_y(i,j) * ( delta(j,k) + GetXValue(matrix_x,Jnum,varNum,i,k,j+1) * temp - temp0);
				temp1  = exp( vector_theta[j] + matrix_x.row(i).dot(beta) ) / ( 1.0 + exp( vector_theta[j] + matrix_x.row(i).dot(beta) ));
				//a_ijp1代表a_i(j+1)
				double ttttt = GetXValue(matrix_x,Jnum,varNum,i,k,j);
				double a_ijp1 = matrix_y(i,j+1) * ( delta(j+1,k) + GetXValue(matrix_x,Jnum,varNum,i,k,j)*temp1 - temp0);
				qTemp += ( a_ij - a_ijp1 );
			}
			tp = exp( vector_theta[Jnum - 2] + matrix_x.row(i).dot(beta) );
			temp2 =tp / ( 1 + tp ) ;
			double a_i_J = matrix_y(i,Jnum - 2) * GetXValue(matrix_x,Jnum,varNum,i,k,Jnum-2) - temp2 * GetXValue(matrix_x,Jnum,varNum,i,k,Jnum-2);
			qTemp += a_i_J;
			matrix_q(i,k) = qTemp * weight(i) ;
		}
	}
	CDoubleMatrix matrix_qqq = copy(matrix_q);
	CDoubleVector vector_q = sum( trans(matrix_q) ); //按列求和
	for(k=0;k<Jnum-1+varNum;k++) //非常耗时 ===temp
	{
		int l=0;
		for(l=0;l<Jnum-1+varNum;l++)
		{
			double informTemp = 0;
			int i=0;
			for(i=0;i<nRow;i++)
			{
				int j=0;
				for(j=0;j<Jnum-2;j++)
				{
					if( 0 <= k && k < Jnum-1 && l >= 0 && l < Jnum-1 )
					{
					 	temp0 = delta(j+1,k) * delta(j,l)
							+	delta(j,k)   * delta(j+1,l)
							-	delta(j+1,k) * delta(j+1,l)
							-	delta(j,k)   * delta(j,l);
					}
					else
						temp0 = 0;
					tp    = exp(vector_theta[j+1])  - exp(vector_theta[j]);
					tp0   = exp( vector_theta[j+1]  + vector_theta[j] );
					temp0 *= tp0 / ( tp * tp );
					tp = exp( vector_theta[j+1]  + matrix_x.row(i).dot(beta) );
					temp  =  tp	/ ((1+tp) * (1+tp));
					double b_i_j_l = matrix_y(i,j) * ( GetXValue(matrix_x,Jnum,varNum,i,k,j+1) * GetXValue(matrix_x,Jnum,varNum,i,l,j+1) * temp - temp0);
					tp = exp( vector_theta[j] + matrix_x.row(i).dot(beta) );
					temp1   = tp/((1 + tp)*(1 + tp));
					double b_i_jp1_l = matrix_y(i,j+1) * ( GetXValue(matrix_x,Jnum,varNum,i,k,j) * GetXValue(matrix_x,Jnum,varNum,i,l,j) * temp1 - temp0);
					informTemp += weight(i) * (b_i_j_l - b_i_jp1_l);					
				}
				tp = exp( vector_theta[Jnum-2] + matrix_x.row(i).dot(beta) ); 
				temp2 = tp / ((1 + tp) * (1 + tp));
				double b_i_J_l = - GetXValue(matrix_x,Jnum,varNum,i,k,Jnum-2)
					        *      GetXValue(matrix_x,Jnum,varNum,i,l,Jnum-2)
					        *	   temp2;
				informTemp += b_i_J_l * weight(i) ;
			}
			matrix_inform(k,l) = informTemp;
		}
	}
	return vector_q;
}

void CRegressionOrdinal::cal_R(CDoubleVector LH, int n)
{
	double L1=LH(0)/(-2);
	double L2=LH(1)/(-2);
	
	double Cox_and_Snell=1-pow(exp(L1-L2),2.0/n);
	double Nagelkerke=Cox_and_Snell/(1-pow(exp(L1),2.0/n));
	double McFadden=1-(L2/L1);

	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("准R平方");

	pTable->InsertColumn(0,"参数");
	pTable->InsertColumn(1,"参数值");

	pTable->InsertItem(0,"Cox and Snell");
	pTable->InsertItem(1,"Nagelkerke");
	pTable->InsertItem(2,"McFadden");

	pTable->SetItemText(0,1,Cox_and_Snell);
	pTable->SetItemText(1,1,Nagelkerke);
	pTable->SetItemText(2,1,McFadden);

	pTable->SetTail("连接模型: Logit");

	CRsltVariable * pETable= new CRsltElementTable("准R平方",pTable);
	m_pResult->Add(pETable);
}

void CRegressionOrdinal::GetXxx(CDoubleMatrix X, CDoubleVector Y, CDoubleVector weight, CDoubleMatrix &newX_matrix, CDoubleMatrix &newY_matrix, CIntMatrix &set)
{
	int r = X.mrow();
	int c = X.mcol();

	CDoubleMatrix newX;
	CDoubleMatrix newY;


	int isFirst=0;
	int NumFirst=0;
	int isIn=0;
	
	CPtrArray * pSet = new CPtrArray;
	int * pData = new int ;
	*pData = Y(0);
	pSet->Add(pData);
	int i=1;
	for(i=1;i<r;i++)
	{
		int col = pSet->GetSize();
		isIn=0;
		for(int j=0;j<col;j++)
		{
			if( IsZero( Y(i)- *(int*)pSet->GetAt(j) ) )
			{
				isIn=1;
				break;
			}
		}
		if( isIn==0 )
		{
			pData = new int;
			*pData = Y(i);
			pSet->Add(pData);
		}
	}
	SortPtrArray(pSet);
	int a = 1;
	int b = pSet->GetSize();
	int m=0;
	int k=1;
	isIn = 0;

	while( isIn==0 || isFirst == 1 )
	{
		CDoubleVector temp = X.row(NumFirst);
		newX = newX | temp;
		newY = newY | CDoubleVector(b);
		isFirst=0;   
		isIn=0;
		k=1;
		for(i = NumFirst;i<r;i++)
		{		
			if( CAlgorithmBase::IsVectorEqual(temp,X.row(i)) )
			{
				for(int j=0;j<b;j++)
				{
					int * pddd = (int *)pSet->GetAt(j);
					if( IsZero( Y(i) - *pddd )  )
					{
						newY(j,m) += weight(i);
						break;
					}
				}
				k=k+1;
			}
			else 
			{
				for(int j=0;j<m;j++)
				{
					if( CAlgorithmBase::IsVectorEqual( X.row(i) , newX.col(j) ) )
					{
						isIn=1;
						//TRACE("isIn = %d\n",isIn);
						break;
					}
					else
					{
						isIn = 0;
					}
				}		
			
				if(  isIn!=1 )
				{
					if( isFirst==0 )
					{
						NumFirst=i;
						//TRACE("NumFirst = %d\n",i+1);
						isFirst=1;
					}
				}
				k=1;
			}
		}
		if(  k==r-NumFirst+1 )
			break;
		m=m+1;
	}
	newX_matrix = trans(newX);
	newY_matrix = trans(newY);
	CIntMatrix mmm( 2,pSet->GetSize() );
	for(i=0;i<pSet->GetSize();i++)
	{
		mmm(0,i) = *(int*)pSet->GetAt(i);
		for(int j=0;j<newY_matrix.mrow();j++)
		{
			mmm(1,i) += newY_matrix(j,i);
		}
	}
	set = copy(mmm);
	FreePtrArray(pSet);
	delete pSet;
}

CIntVector CRegressionOrdinal::SortVector(CIntVector vect1)
{
	CIntVector vect = copy(vect1);
	int nLen = vect.vlen();
	int temp;
/*	for(int i=0;i<nLen-1;i++)
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
*/

	int i, j, k = nLen/2;
	while( k>0 )
	{ 
		for( j=k; j<=nLen-1; j++ )
		{
			temp = vect[j]; 
			i = j - k;

			while( (i>=0) && (vect[i]>temp) )
			{
				vect[i+k] = vect[i];
				i = i-k;
			}
			vect[i+k] = temp;
		}
		k = k/2;
	}

	return vect;
}

CIntMatrix CRegressionOrdinal::GetYaMatirx(CIntVector vect,int num)
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

int CRegressionOrdinal::GrpToIdx(CIntVector vect, CIntVector &vect1)
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

int CRegressionOrdinal::QuickFind( CTArray<int, int>& arSort, int nData, int iBegin, int iEnd )
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

int CRegressionOrdinal::delta(int j,int k)
{
	return j==k ? 1:0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//														
//
//////////////////////////////////////////////////////////////////////////////////////////////
double CRegressionOrdinal::GetXValue(CDoubleMatrix matrix_x,int Jnum,int varNum,int i,int k,int j)
{
	double x = 0;
	if( 0 <= k && k < Jnum-1 )
	{
		x = delta(j,k);
	}
	else //if( Jnum < k && k < Jnum-1+varNum )
	{
		x = matrix_x(i,k-Jnum+1);
	}
	return x;
}

/////////////////////////////////////////////////////////////////////////
//
//					对指针数组par中的数进行排序
//
/////////////////////////////////////////////////////////////////////////
void CRegressionOrdinal::SortPtrArray(CPtrArray *par)
{
	CPtrArray arTemp;
	PtrArrayCopy(par,&arTemp);
	par->RemoveAll();
	int * pIndex = NULL;
	while( arTemp.GetSize() > 0 )
	{
		int nmin = 1e10;
		int nminIndex = -1;
		for(int i=0;i<arTemp.GetSize();i++)
		{
			pIndex = (int *)arTemp.GetAt(i);
			if( *pIndex < nmin )
			{
				nmin      = *pIndex;
				nminIndex = i;
			}
		}
		par->Add(arTemp.GetAt(nminIndex));
		arTemp.RemoveAt(nminIndex);
	}
}





















