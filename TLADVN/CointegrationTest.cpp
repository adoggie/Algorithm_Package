// CointegrationTest.cpp: implementation of the CCointegrationTest class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "CointegrationTest.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
//						协整检验 潘中岐 2005.12						//
//////////////////////////////////////////////////////////////////////

CCointegrationTest::CCointegrationTest()
{

}

CCointegrationTest::~CCointegrationTest()
{
	m_tVarInfo.DeleteObject();			//检验变量struct
	m_tVarInfo_D.DeleteObject();		//外生变量struct
	m_strVarSel.RemoveAll();						//擦除检验变量名信息
	m_strDepend.RemoveAll();						//擦除外生变量名信息
}

BOOL CCointegrationTest::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//COINTEGRATION
//	/DATA=[FILE]
//	/TESTVAR [varlsit]
//	/EXOGENOUS [varlsit]
//	/OPTION [{NONE},{CONST},{CONLINEAR},{CONTREND},{TWICETREND}]
// [LAG({2**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/EXOGENOUS", VOstr, ynames);
	if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
		return false;
	CTStringArray xname;
	AnaSyntax::getVAR("/TESTVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return false;
	
	if (AnaSyntax::findstr("/OPTION", "CONST", VOstr))
		m_tIn.m_nModel = 1;
	else if (AnaSyntax::findstr("/OPTION", "CONLINEAR", VOstr))
		m_tIn.m_nModel = 2;
	else if (AnaSyntax::findstr("/OPTION", "CONTREND", VOstr))
		m_tIn.m_nModel = 3;
	else if (AnaSyntax::findstr("/OPTION", "TWICETREND", VOstr))
		m_tIn.m_nModel = 4;
	else
		m_tIn.m_nModel = 0;

	m_tIn.m_nLagnum = 2;
	if (AnaSyntax::findstr("/OPTION", "LAG", VOstr))
		m_tIn.m_nLagnum = AnaSyntax::getvalue("/OPTION", "LAG", VOstr);

	return true;
}

CResult* CCointegrationTest::OnRun(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	CDoubleMatrix Matrix_X, Matrix_Y;					//检验矩阵和外生变量矩阵
	pResult=new CResult("Johansen 协整检验");
	m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}

	m_tIn.m_nRegre = m_tVarInfo.iCount;				//给出模型的检验变量的个数
	m_tIn.m_nExto = m_tVarInfo_D.iCount;			//给出外生变量的个数
	GetMatrixData(Matrix_X, Matrix_Y);			//从界面读数据
	m_tIn.m_mxX=Matrix_X;
	m_tIn.m_mxY=Matrix_Y;
	if (coinTest())							//Cvarest函数给出是否中途跳出的信息
		DrawResult();						//输出结果

	return pResult;
}

//////////////////////////////////////////////////////////
//					获取数据							//
//////////////////////////////////////////////////////////

void CCointegrationTest::GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y)//从界面读数据
{
	m_strVarSel.RemoveAll();						//擦除检验变量名信息
	m_strDepend.RemoveAll();						//擦除外生变量名信息
	CDWordArray indexary;
	CDWordArray indexarx;
	int i = 0 , j = 0;
	
	for( i=0; i<m_tVarInfo.iCount; i++ )			//读出检验变量的名字，存入字符串
	{			
		indexary.Add(m_tVarInfo.pValue[i]);
		m_strDepend.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
	}
	
	for( j=0; j<m_tVarInfo_D.iCount; j++)			//读出外生变量的名字，存入字符串
	{
		indexarx.Add(m_tVarInfo_D.pValue[j]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[j]));
	}
	
	m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815取回界面数据
	if (m_tVarInfo_D.iCount!=0)
		m_pDataInterface->GetColsData( indexarx, Matrix_Y, 6 );//Modify by xds 20060815

}

//////////////////////////////////////////////////////////////
//						协整检验主函数						//
//////////////////////////////////////////////////////////////

bool CCointegrationTest::coinTest()
{

	//给出两个检验的临界值表

	//特征值轨迹检验临界值表
	m_tOut.m_mxTraceCri.create(12,5,0.0);
	double criteron1[12]={4.129906,12.3209,24.27596,40.17493,60.06141,83.93712,111.7805,143.6691,179.5098,219.4016,263.2603,311.1288};	
	double criteron2[12]={9.164546,20.26184,35.19275,54.07904,76.97277,103.8473,134.678,169.5991,208.4374,251.265,298.1594,348.9784};
	double criteron3[12]={3.841466,15.49471,29.79707,47.85613,69.81889,95.75366,125.6154,159.5297,197.3709,239.2354,285.1425,334.9837};
	double criteron4[12]={12.51798,25.87211,42.91525,63.8761,88.8038,117.7082,150.5585,187.4701,228.2979,273.1889,322.0692,374.9076}; 
	double criteron5[12]={3.841466,18.39771,35.0109,55.24578,79.34145,107.3466,139.2753,175.1715,215.1232,259.0294,306.8944,358.7184};	
	
	m_tOut.m_mxTraceCri(0)=criteron1;
	m_tOut.m_mxTraceCri(1)=criteron2;
	m_tOut.m_mxTraceCri(2)=criteron3;
	m_tOut.m_mxTraceCri(3)=criteron4;
	m_tOut.m_mxTraceCri(4)=criteron5;


	//最大特征值检验临界值表
	m_tOut.m_mxMaxCri.create(12,5,0.0);
	double critero1[12]={4.129906,11.2248,17.7973,24.15921,30.43961,36.63019,42.77219,48.8772,54.96577,61.03407,67.07555,73.09094};
	double critero2[12]={9.164546,15.8921,22.29962,28.58808,34.80587,40.9568,47.07897,53.18784,59.24,65.30016,71.33542,77.3818};
	double critero3[12]={3.841466,14.2646,21.13162,27.58434,33.87687,40.07757,46.23142,52.36261,58.43354,64.50472,70.53513,76.57843};
	double critero4[12]={12.51798,19.38704,25.82321,32.11832,38.33101,44.4972,50.59985,56.70519,62.75215,68.81206,74.83748,80.87025}; 
	double critero5[12]={3.841466,17.14769,24.25202,30.81507,37.16359,43.41977,49.58633,55.72819,61.8055,67.91026,73.94036,79.97865};
	
	m_tOut.m_mxMaxCri(0)=critero1;
	m_tOut.m_mxMaxCri(1)=critero2;
	m_tOut.m_mxMaxCri(2)=critero3;
	m_tOut.m_mxMaxCri(3)=critero4;
	m_tOut.m_mxMaxCri(4)=critero5;
	

	//计算滞后阶数，若为0，则改为1
	int lag=m_tIn.m_nLagnum;
	if (m_tIn.m_nLagnum==0)
	{
		lag=1;
	}
	//若只有一个检验变量则显示提示
	if (m_tIn.m_nRegre==1)
	{
		CTString strWarning = "选入变量过少，不适合进行此分析!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;
	}
	
	//若时间序列过短，则显示提示
	int nXrow=m_tIn.m_mxX.mrow();
	int nT=nXrow-lag-1;
	if (nT<=0||nXrow<4) 
	{
		CTString strWarning = "时间序列过短，不适合进行此分析";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;
	}

	//调用JJtest函数给出特征值和特征向量，以及估计矩阵
	CDoubleVector vrEigvalue;
	CDoubleMatrix mxVV;
	CDoubleMatrix mxEigvec=JJtest(m_tIn.m_mxX,m_tIn.m_mxY,m_tIn.m_nModel,lag,vrEigvalue,mxVV);
	if(mxEigvec.mrow()==1)
	{
		return false;
	}

	int ndim = mxEigvec.mcol();
	//对特征向量进行正规化变换，给出特征值的变换
	CDoubleMatrix mxVECstand=mxEigvec.dot(mxEigvec);
	CDoubleVector vrEigvalue1(ndim);
	int j=0; 
	for (j=0; j<ndim; j++)
	{
		if ((1.0-vrEigvalue(j))<1e-10)
		{
			CTString strWarning = "模型选择不当，请重新选择!";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return false;
		}
		vrEigvalue1(j)=log(1.0-vrEigvalue(j));
		mxEigvec(j)=mxEigvec(j)/sqrt(mxVECstand(j,j));
	}


	//给出特征值轨迹检验和最大特征值检验。
	CDoubleVector vrEigTrace(m_tIn.m_nRegre,0);
	CDoubleVector vrEigMax(m_tIn.m_nRegre,0);
	for (j=0; j<m_tIn.m_nRegre; j++)
	{
		CDoubleVector vrEigtemp=vrEigvalue1.slice(j,m_tIn.m_nRegre-j);
		vrEigTrace(j)=-double(nT)*sum(vrEigtemp);
		vrEigMax(j)=-double(nT)*vrEigvalue1(j);
	}
	

	//给出检验得到的有效协整关系的数目
	//要考虑自变量个数可能超过12 个，此时要在最开始的几个检验临界值向量前添加0
	//不考虑有几个协整向量，只考虑有n个变量，就输出n－1个协整关系，同时输出对应的临界值表，
	//是否成立，由用户判断
	m_tOut.m_vrMaxCrit.create(m_tIn.m_nRegre,0);
	m_tOut.m_vrTraceCrit.create(m_tIn.m_nRegre,0);

	//给出模型含有的协整关系数目
	int index=min(12,m_tIn.m_nRegre);
	int k=0; 
	for (k=0; k<index; k++)
	{
		m_tOut.m_vrMaxCrit(k)=m_tOut.m_mxMaxCri(k,m_tIn.m_nModel);
		m_tOut.m_vrTraceCrit(k)=m_tOut.m_mxTraceCri(k,m_tIn.m_nModel);
	}
	for (j=0; j<m_tIn.m_nRegre; j++)
	{
		if (m_tIn.m_nRegre-j<=12)
		{
			if (vrEigvalue(j)<m_tOut.m_mxMaxCri(m_tIn.m_nRegre-j-1,m_tIn.m_nModel))
			{
				m_tOut.m_nIndex = j;
			}
			else
			{	
				m_tOut.m_nIndex = m_tIn.m_nRegre-1;
			}
		}
	}
	

	//对特征向量实行Johansen正规化
	CDoubleMatrix mxTe =  mxEigvec.dot(mxEigvec);
	for (j=0; j<ndim; j++)
	{
		for (int k=0;k<ndim;k++)
		{
			mxEigvec(k,j)=mxEigvec(k,j)/sqrt(mxTe(j,j));
		}
		
	}
	mxTe =  mxEigvec.dot(mxVV)*mxEigvec;
	for (j=0; j<ndim; j++)
	{
		for (int k=0;k<ndim;k++)
		{
			mxEigvec(k,j)=mxEigvec(k,j)/sqrt(mxTe(j,j));
		}

	}


	//注释的这些操作是下一步要进行的
//	CDoubleMatrix mxA_hat=mxEigvec.slice(0,0,m_tIn.m_nRegre,m_tOut.m_nIndex);

	m_tOut.m_vrEigValue=vrEigvalue;
	m_tOut.m_vrEigTrace=vrEigTrace;
	m_tOut.m_vrEigMax=vrEigMax;


//	CDoubleMatrix mxGama=-mxUV*mxEigvec;
//	CDoubleMatrix mxCauchy0 = trans(mxUV)*mxA_hat*trans(mxA_hat);
	CDoubleMatrix mxtemp=trans(mxEigvec);
	
//给出模型的协整关系系数
	m_tOut.m_mxCoff=mxtemp.slice(0,0,m_tIn.m_nRegre,ndim);

//给出分情况的协整系数，系数已经归一化
	m_tOut.m_mxCoff1.destruction();
	for (k=1; k<m_tOut.m_nIndex+1; k++)
	{
		CDoubleMatrix mxtemp1=mkeye(mxtemp,k);
		if (m_tOut.m_mxCoff1.mcol()==0)
		{
			m_tOut.m_mxCoff1=mxtemp1.copy();
		}
		else
			m_tOut.m_mxCoff1=m_tOut.m_mxCoff1.commat(mxtemp1,2);
	}

	return true;
}


	//JJ检验子函数，获得相关矩阵的特征值、特征向量，以及其他矩阵。
CDoubleMatrix CCointegrationTest::JJtest(CDoubleMatrix mxX,CDoubleMatrix mxY,int dChoice,int lag,CDoubleVector& vrEigvalue,CDoubleMatrix& mxVV)
{

	int n=mxX.mrow();
	int nt=n-lag-1;

	//给出外生新变量
	CDoubleMatrix  mxYnew;
	if (mxY.mcol()!=0)
	{
		mxYnew=mxY.slice(lag+1,0,n-lag-1,m_tIn.m_nExto);
	}


/////////////////////////////////////////////////////////////////////
//																/////
//	构造两个向量自回归模型，一个为滞后自回归，一个为差分自回归	/////
//																/////
/////////////////////////////////////////////////////////////////////	
	
	//构造回归变量矩阵
	CDoubleMatrix mxXlag=mxX.slice(lag,0,n-lag-1,m_tIn.m_nRegre);
	CDoubleMatrix mxXtemp=mxX.slice(lag+1,0,n-lag-1,m_tIn.m_nRegre);
	CDoubleMatrix mxXdif=mxXtemp-mxXlag;

	CDoubleMatrix mxXnew;

	CDoubleMatrix mxXtemp1;
	CDoubleMatrix mxXtemp2;


	//产生常数项矩阵和趋势项矩阵
	CDoubleVector vrUnit(nt,1);
	CDoubleVector vrTrend(nt);
	int j=0; 
	for (j=0; j<nt; j++)
	{
		vrTrend(j)=j+1;
	}
	j=1;
	//产生新的滞后观测矩阵
	while (j<=lag) 
	{
		mxXtemp1=mxX.slice(1+lag-j,0,n-lag-1,m_tIn.m_nRegre);
		mxXtemp2=mxX.slice(lag-j,0,n-lag-1,m_tIn.m_nRegre);
		mxXtemp2=mxXtemp1-mxXtemp2;

		if (mxXnew.mcol()==0)
		{
			mxXnew=mxXtemp2;
		}
		else
			mxXnew=mxXnew.commat(mxXtemp2,1);
//		if(m_tIn.m_nModel==3)
		//		{
		//			mxXnew = mxXnew.commat(vrUnit,1);
		//		}
		
				
		j++;
	}
	//摧毁矩阵，释放空间
	mxXtemp.destroy();
	mxXtemp1.destroy();
	mxXtemp2.destroy();



	//根据设定的模型的不同，得到不同的向量自回归模型
	//此选项包括三个有效选择，0，2，4，其他选项暂时不需要
	//根据选项改变回归项的组成
	switch(m_tIn.m_nModel)
	{
	case 0:
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 1:
		mxXlag=mxXlag.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 2:
		mxXnew=mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 3:
		mxXlag=mxXlag.commat(vrTrend,1);
		mxXnew = mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	case 4:
		mxXnew=mxXnew.commat(vrTrend,1);
		mxXnew=mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	default:
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;
	}

	vrUnit.destroy();
	vrTrend.destroy();

	//生成X'x矩阵 和 inv(x'x)矩阵
	CDoubleMatrix mxXX=mxXnew.dot(mxXnew);
	CDoubleMatrix mxXXinv=inv(mxXX);
	CDoubleMatrix mxINFO(1,1,0.0);
	if (det(mxXX)<DoubleZero)
	{
		CTString strWarning = "矩阵奇异，请重新设定模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}
	

	CDoubleMatrix mxPI=mxXXinv*(mxXnew.dot(mxXdif));
	CDoubleMatrix mxTheta=mxXXinv*(mxXnew.dot(mxXlag));

	//计算两个向量自回归模型的残差矩阵
	CDoubleMatrix mxU=mxXdif-mxXnew*mxPI;
	CDoubleMatrix mxV=mxXlag-mxXnew*mxTheta;
	

	//给出Johansen极大似然估计的关键矩阵，Suu，Svv，Suv
	CDoubleMatrix mxUU=mxU.dot(mxU)/(1.0*n);
	mxVV=(mxV.dot(mxV))/(1.0*n);
	CDoubleMatrix mxUV=(mxU.dot(mxV))/(1.0*n);
	
	if (det(mxUU)<=DoubleZero || det(mxVV)<=DoubleZero)
	{
		CTString strWarning = "矩阵奇异，请重新设定模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}

	//构造M矩阵
	CDoubleMatrix mxM = inv(mxVV)*(trans(mxUV))*inv(mxUU)*mxUV;
	CComplexMatrix mxEigVectComp;


	//得到特征值和特征向量矩阵
	CComplexMatrix mxEigValueComp=mxM.eig(mxEigVectComp);
	CDoubleMatrix mxEigVect=CDoubleMatrix(mxEigVectComp);
	CDoubleMatrix mxEigValue=CDoubleMatrix(mxEigValueComp);
	TraceMatrix(mxEigValue);
	mxEigValueComp.destroy();
	mxEigVectComp.destroy();
	mxEigValueComp.destroy();
	int ndim = mxEigValue.mcol();

	//生成特征值向量
	vrEigvalue.create(ndim);
	for (j=0; j< ndim; j++)
	{
		vrEigvalue(j)=mxEigValue(j,j);
	}
	if (sum(vrEigvalue)<=DoubleZero)
	{
		CTString strWarning = "矩阵奇异，请重新设定模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}
	//将特征向量根据对应的特征值的大小，按照降序排列
	CDoubleVector vrEigvectemp(ndim,1);
	double Eigvaluetemp=0;
	for (int k=0; k<ndim; k++)
	{
		for (int l=k+1; l<ndim; l++)
		{
			if (vrEigvalue(k)<vrEigvalue(l))
			{
				Eigvaluetemp = vrEigvalue(k);
				vrEigvalue(k)=vrEigvalue(l);
				vrEigvalue(l)=Eigvaluetemp;

				vrEigvectemp = mxEigVect(k);
				mxEigVect(k) = mxEigVect(l);
				mxEigVect(l) = vrEigvectemp;
			}
		}
	}

	//将这几个矩阵整合为一个矩阵mxEigvalue2，返回
	return mxEigVect;
}




void CCointegrationTest::DrawResult()
{
	//结果函数
	
/////////创建第一个表——轨迹检验表
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);

	pTable1->SetTitle("轨迹检验");
	pTable1->InsertColumn(0, "");
	pTable1->InsertColumn(0, "特征值");
	pTable1->InsertColumn(0, "检验统计量");
	pTable1->InsertColumn(0, "临界值");
// 	pTable1->InsertColumn(0, "P值", 120);		暂时不输出P值

	pTable1->InsertItem(0, "无协整关系");
	int j=1;
	for (j=1; j<m_tIn.m_nRegre; j++)
	{
		CTString str;
		str.Format("%d", j);
		pTable1->InsertItem(j, "至多"+str+"个");
	}

	int i=0; 
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable1->SetItemText(i, 1, m_tOut.m_vrEigValue(i));
		pTable1->SetItemText(i, 2, m_tOut.m_vrEigTrace(i));
		pTable1->SetItemText(i, 3, m_tOut.m_vrTraceCrit(m_tIn.m_nRegre-i-1));
	}
	
	CRsltElementTable *pETable1 = new CRsltElementTable("轨迹检验", pTable1);
	m_pResult->Add(pETable1);
	

/////////创建第二个表——最大特征值检验表
	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("最大特征值检验");
	pTable2->InsertColumn(0, "");
	pTable2->InsertColumn(0, "特征值");
	pTable2->InsertColumn(0, "检验统计量");
	pTable2->InsertColumn(0, "临界值");
//	pTable2->InsertColumn(0, "P值", 120);
	
	pTable2->InsertItem(0, "无协整关系");
	for (j=1; j<m_tIn.m_nRegre; j++)
	{
		CTString str;
		str.Format("%d", j);
		pTable2->InsertItem(j, "至多"+str+"个");
	}	
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable2->SetItemText(i, 1, m_tOut.m_vrEigValue(i));
		pTable2->SetItemText(i, 2, m_tOut.m_vrEigMax(i));
		pTable2->SetItemText(i, 3, m_tOut.m_vrMaxCrit(m_tIn.m_nRegre-i-1));
	}

	CRsltElementTable *pETable2 = new CRsltElementTable("最大特征值检验", pTable2);
	m_pResult->Add(pETable2);
	

	
/////////创建第三个表——协整关系表
	//创建单个方程总结表
	CTLTable * pTable3 = new CTLTable;
	pTable3->CreateTable(2, 2);	
	pTable3->SetTitle("协整系数估计");
	pTable3->InsertColumn(0,"");

	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable3->InsertColumn(0, m_strDepend[i]);
	}

	int ndim = m_tOut.m_mxCoff.mcol();
	switch(m_tIn.m_nModel) 
	{
	case 1:

		pTable3->InsertColumn(0,"常数项");
		break;
	case 3:
		pTable3->InsertColumn(0,"趋势项");
		break;
	default:
		;
	}
	
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		CTString str;
		str.Format("%d", i+1);
		pTable3->InsertItem(i, "协整关系"+str);
		for (int k=0; k<ndim; k++)
		{
			pTable3->SetItemText(i, k+1, m_tOut.m_mxCoff(i,k));
		}
	}

	CRsltElementTable *pETable3 = new CRsltElementTable("协整系数估计", pTable3);
	m_pResult->Add(pETable3);


//创建第四个表——分类系数估计表
	CTLTable * pTable4 = new CTLTable;
	pTable4->CreateTable(2, 2);	
	pTable4->SetTitle("分类系数估计");
	pTable4->InsertColumn(0, "");
	
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable4->InsertColumn(0, m_strDepend[i]);
	}

	switch(m_tIn.m_nModel) 
	{
	case 1:
		
		pTable4->InsertColumn(0,"常数项");
		break;
	case 3:
		pTable4->InsertColumn(0,"趋势项");
		break;
	default:
		;
	}

	int number=0;
	for(i=0; i<m_tOut.m_nIndex; i++)
	{
		CTString str;
		str.Format("%d", i+1);
		if(i == 0)
		{
			pTable4->InsertItem(number, "含有"+str+"协整关系");
		}
		else
		{
			pTable4->InsertItem(number, "含有"+str+"协整关系");
		}
		number++;
		for (int j=0; j<i; j++)
		{
			if (j==i-1)
			{
				pTable4->InsertItem(number+j, "");
			}
			else
			{
				pTable4->InsertItem(number+j, "");
			}
		}
		number = number+i;
	}
	for (i=0; i<number; i++)
	{
		for (int k=0; k<ndim; k++)
		{
			pTable4->SetItemText(i, k+1, m_tOut.m_mxCoff1(i,k));
		}
	}
	

	CRsltElementTable *pETable4 = new CRsltElementTable("分类系数估计", pTable4);
	m_pResult->Add(pETable4);

}
