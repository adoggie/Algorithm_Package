// VEC.cpp: implementation of the CVEC class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "VEC.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVEC::CVEC()
{
	m_tIn.m_nEquNum = 1;
	m_tIn.m_nLagnum = 2;
}

CVEC::~CVEC()
{
	
}

BOOL CVEC::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//VECTEST
//	/DATA=[FILE]
//	/REGVAR [varlsit]
//	/EXOGENOUS [varlsit]
//	/OPTION [{NONE},{CONST},{CONLINEAR},{CONTREND},{TWICETREND}]
// [LAG({2**})] [FUNNUM({1**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/EXOGENOUS", VOstr, ynames);
	if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
		return FALSE;
	CTStringArray xname;
	AnaSyntax::getVAR("/REGVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;
	m_tIn.m_nRegre = m_tVarInfo.iCount;				//给出模型的检验变量的个数
	m_tIn.m_nExto = m_tVarInfo_D.iCount;

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
	m_tIn.m_nEquNum = 1;
	if (AnaSyntax::findstr("/OPTION", "FUNNUM", VOstr))
		m_tIn.m_nEquNum = AnaSyntax::getvalue("/OPTION", "FUNNUM", VOstr);

	return TRUE;
}

CResult* CVEC::OnRun(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	CDoubleMatrix Matrix_X, Matrix_Y;					//检验矩阵和外生变量矩阵
	pResult=new CResult("误差修正模型");
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}

	//给出外生变量的个数
	m_pResult = pResult;
	GetMatrixData(Matrix_X, Matrix_Y);			//从界面读数据
	m_tIn.m_mxX=Matrix_X;
	m_tIn.m_mxY=Matrix_Y;
	if (VECest())							//Cvarest函数给出是否中途跳出的信息
	{
		DrawResult();						//输出结果
		return pResult;
	}
	else
	{
		CTString strWarning = "无法计算，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
}

void CVEC::GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y)//从界面读数据
{
	TCDataMatrix pDataSet;
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

bool CVEC::VECest()
{

	//若滞后阶数为负，则显示错误
	if (m_tIn.m_nLagnum<0)
	{
		CTString strWarning = "模型设置错误，请重新指定滞后阶数!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	//计算滞后阶数，若为0，则改为1
	int lag=m_tIn.m_nLagnum;
	if (m_tIn.m_nLagnum==0)
	{
		lag=1;
		m_tIn.m_nLagnum=1;
	}

	//显示信息
	if (m_tIn.m_nRegre==1)
	{
		CTString strWarning = "选入变量过少，不适合进行此分析!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	
	//模型设置不适当，给出错误信息
	if (m_tIn.m_nEquNum<1||m_tIn.m_nEquNum>=m_tIn.m_nRegre)
	{
		CTString strWarning = "模型设置不正确，请重新指定协整方程个数!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	int nXrow=m_tIn.m_mxX.mrow();
	if (nXrow-lag*m_tIn.m_nRegre-m_tIn.m_nExto-m_tIn.m_nEquNum<=0)
	{
		CTString strWarning = "观测数据个数太少，不适合此分析!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	int nt=nXrow-lag-1;
	int nXcol = m_tIn.m_mxX.mcol();

	
//给出初始化的统计量
	m_tOut.m_vrSSE.create(nXcol,0);
	m_tOut.m_vrSST.create(nXcol,0);
	m_tOut.m_vrRsqu.create(nXcol,0);
	m_tOut.m_vrAdjR.create(nXcol,0);
	m_tOut.m_vrSSR.create(nXcol,0);
	m_tOut.m_vrSER.create(nXcol,0);
	m_tOut.m_vrDw.create(nXcol,0);
	m_tOut.m_vrMD.create(nXcol,0);
	m_tOut.m_vrSDD.create(nXcol,0);
	m_tOut.m_vrFtest.create(nXcol,0);
	m_tOut.m_vrDRC.create(nXcol,0);
	m_tOut.m_vrLogL.create(nXcol,0);
	m_tOut.m_vrAIC.create(nXcol,0);
	m_tOut.m_vrSC.create(nXcol,0);

	//计算新的X观测矩阵
	CDoubleMatrix mxXnew;
	
	//调用JJtest函数给出特征值和特征向量，以及估计矩阵
	CDoubleMatrix mxEigVec=JJtest(m_tIn.m_mxX,m_tIn.m_mxY,m_tIn.m_nModel,lag,mxXnew);
	if (mxEigVec.mrow()==1)
	{
		return FALSE;
	}
	
	//计算新的向量自回归模型
	CDoubleMatrix mxXlag=m_tIn.m_mxX.slice(lag,0,nt,m_tIn.m_nRegre);
	CDoubleMatrix mxY=m_tIn.m_mxX.slice(lag+1,0,nt,m_tIn.m_nRegre);
	mxY = mxY-mxXlag;

	//给出趋势项和常数项
	CDoubleVector vrUnit(nt,1);
	CDoubleVector vrTrend(nt,1);
	int j;	
	for (j=0; j<nt; j++)
	{
		vrTrend(j)=j+1;
	}
	
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		mxXlag=mxXlag.commat(vrUnit,1);
		break;
	case 2:
		break;
	case 3:
//		mxTrend = mxTrend+m_tIn.m_nLagnum*mxUnit;
		//		mxXlag=mxXlag.commat(mxTrend,1);
		mxXlag=mxXlag.commat(vrTrend,1);
		break;
	case 4:
		break;
	default:
		return FALSE;///////////////////////////////////////////
	return FALSE;
	}
	
	CDoubleMatrix mxW0 = mxXlag*mxEigVec;
	CDoubleMatrix mxTrend;
	mxTrend = mxTrend | vrTrend;
	CDoubleMatrix mxUnit;
	mxUnit = mxUnit | vrUnit;

	if (m_tIn.m_nModel==2||m_tIn.m_nModel==3)
	{
		CDoubleVector vrMean = -mean(mxW0);
		mxXlag = mxXlag.commat(vrUnit,1);
		mxEigVec = mxEigVec.commat(vrMean,2);
		mxW0 = mxXlag*mxEigVec;
//		TraceMatrix(mxEigVec);
	}
	else if(m_tIn.m_nModel==4)
	{
 //		mxTrend = mxTrend + (m_tIn.m_nLagnum)*mxUnit;
		CDoubleMatrix mxC1 = mxTrend.commat(mxUnit,1);
		CDoubleMatrix mxXtemp = mxC1.dot(mxC1);
		mxC1 = -mxC1.dot(mxW0);
		mxXtemp = inv(mxXtemp)*mxC1;
		mxEigVec=mxEigVec.commat(mxXtemp,2);
		mxXtemp = mxXlag.commat(vrTrend,1);
		mxXtemp = mxXtemp.commat(vrUnit,1);
		mxW0 = mxXtemp*mxEigVec;
	}
	mxXlag.destroy();
	//给出外生新变量
	CDoubleMatrix  mxYnew;

	//若没有外生变量
	if (m_tIn.m_mxY.mcol()!=0)
	{
		mxYnew=m_tIn.m_mxY.slice(lag,0,nt,m_tIn.m_nExto);
	}

	vrUnit.destroy();
	vrTrend.destroy();

	m_tOut.m_mxCoff = mxEigVec;

	
	CDoubleMatrix mxZ0=mxW0.commat(mxXnew,1);

	CDoubleMatrix mxZ0Z0 = mxZ0.dot(mxZ0);
	mxZ0Z0 = inv(mxZ0Z0);
	CDoubleMatrix mxBeta = mxZ0Z0*(mxZ0.dot(mxY));
	mxZ0Z0.destroy();

	CDoubleMatrix mxResidul = mxY-mxZ0*mxBeta;
	
	//计算总体模型的拟合优度值

	int ndex = mxEigVec.mrow(); 
	if (m_tIn.m_nModel==3||m_tIn.m_nModel==4) 
	{
		for(j=0; j<m_tIn.m_nEquNum; j++)
			m_tOut.m_mxCoff(ndex-1,j) =m_tOut.m_mxCoff(ndex-1,j)-m_tOut.m_mxCoff(ndex-2,j)*m_tIn.m_nLagnum; 
		
	}
	
	m_tOut.m_mxCoff1 = mxBeta;
	CDoubleMatrix mxRese(m_tIn.m_nRegre,m_tIn.m_nRegre,0.0);
	for (j=0;j<m_tIn.m_nRegre;j++)
	{
		for (int k=0;k<m_tIn.m_nRegre;k++)
		{
			mxRese(j,k)=mxResidul(j).dot(mxResidul(k));
		}
	}
	
	//给出回归的自由度
	int ndegree = 0;
	int ndegree1 = 0;
	switch(m_tIn.m_nModel)
	{
	case 0:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum;
		ndegree1 = ndegree*m_tIn.m_nRegre+m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	case 1:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum;
		ndegree1 = ndegree*m_tIn.m_nRegre+(m_tIn.m_nRegre+1)*m_tIn.m_nEquNum;
		break;
	case 2:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+1;
		ndegree1 = ndegree*m_tIn.m_nRegre + m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	case 3:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+1;
		ndegree1 = ndegree*m_tIn.m_nRegre+(m_tIn.m_nRegre+1)*m_tIn.m_nEquNum;
		break;
	case 4:
		ndegree = m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto+m_tIn.m_nEquNum+2;
		ndegree1 = ndegree*m_tIn.m_nRegre+m_tIn.m_nRegre*m_tIn.m_nEquNum;
		break;
	default:
		;
	}

	//预防出错
	if (nt-ndegree<=0)
	{
		CTString strWarning = "观测数据个数太少，不适合此分析!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	m_tOut.m_dDRC=det((CDoubleMatrix)(mxRese/(nt*1.0)));				//计算模型的DRC	
	if (m_tOut.m_dDRC<DoubleZero)
	{
		CTString strWarning = "观测数据个数太少，不适合此分析!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	m_tOut.m_dAdjDRC=det((CDoubleMatrix)(mxRese/(nt*1.0-ndegree)));	//计算模型的调整DRC
	
	mxRese.destroy();
	double dLp=log(m_tOut.m_dDRC);
	double dSBCtemp=dLp/m_tIn.m_nRegre;
	m_tOut.m_dSBC=(dSBCtemp)-(1.0-double(ndegree)/double(nt))*log(double(nt));//计算SBC准则
	double dFPEtem=double(nt)*(double(nt-ndegree))/(double(nt)+double(ndegree));
	m_tOut.m_dFPE=dSBCtemp-log(dFPEtem);							//计算FPE准则
	
	
	m_tOut.m_dLogl=(1.0+log(2*pi))*(-1.0)*(nt*m_tIn.m_nRegre)/2-log(m_tOut.m_dDRC)*nt/2;	//计算对数似然值
	//计算AIC准则
	m_tOut.m_dAIC=2.0*m_tOut.m_dLogl/(double)nt*(-1)+2.0*double(ndegree1)/double(nt);
    //计算SC准则
	m_tOut.m_dSC=2.0*m_tOut.m_dLogl/nt*(-1.0)+double(ndegree1)*log(nt)/double(nt);
	
	int nBetarow = mxBeta.mrow();
	m_tOut.m_mxSEReg.create(nBetarow,m_tIn.m_nRegre,0.0);
	m_tOut.m_mxTvalue.create(nBetarow,m_tIn.m_nRegre,0.0);

	//对每个方程分别计算统计量
	for(int VARindex=0;VARindex<m_tIn.m_nRegre;VARindex++)
	{
		CDoubleMatrix mxBetai=mxBeta.slice(0,VARindex,nBetarow,1);
		CDoubleMatrix mxResi = mxResidul.slice(0,VARindex,nt,1);
		CDoubleMatrix mxYnewi = mxY.slice(0,VARindex,nt,1);
		
		//调用Paratest函数进行处理
		
		if (!Paratest(mxBetai,mxResi,mxYnewi,mxZ0,VARindex))
		{
			return FALSE;
		}
	}	
	return TRUE;
}	



//Johansen估计子函数

CDoubleMatrix CVEC::JJtest(CDoubleMatrix mxX,CDoubleMatrix mxY,int dChoice,int lag,CDoubleMatrix &mxXnew)
{
	//JJ检验子函数，获得相关矩阵的特征值、特征向量，以及其他矩阵。
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
	
	CDoubleMatrix mxXtemp1;
	CDoubleMatrix mxXtemp2;
	
	//产生常数项矩阵和趋势项矩阵
	CDoubleVector vrUnit(nt,1);
	CDoubleVector vrTrend(nt);
	int j;
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
		j++;
	}
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
		vrTrend = vrTrend + double(m_tIn.m_nLagnum+1)*vrUnit;
		mxXnew=mxXnew.commat(vrTrend,1);
		mxXnew=mxXnew.commat(vrUnit,1);
		mxXnew=mxXnew.commat(mxYnew,1);
		break;
	default:
		return FALSE;///////////////////////////////////////////
	return FALSE;
	}
	
	vrUnit.destroy();
	vrTrend.destroy();

	//生成X'x矩阵 和 inv(x'x)矩阵
	CDoubleMatrix mxXX=mxXnew.dot(mxXnew);
	CDoubleMatrix mxINFO(1,1,0.0);
	if (det(mxXX)<DoubleZero)
	{
		CTString strWarning = "矩阵奇异，请重新设定模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}

	CDoubleMatrix mxXXinv=inv(mxXX);
	
	CDoubleMatrix mxPI = mxXXinv*(mxXnew.dot(mxXdif));
	CDoubleMatrix mxTheta = mxXXinv*(mxXnew.dot(mxXlag));
	
	//计算两个向量自回归模型的残差矩阵
	CDoubleMatrix mxU=mxXdif-mxXnew*mxPI;
	CDoubleMatrix mxV=mxXlag-mxXnew*mxTheta;
	CDoubleMatrix mxM;
	CDoubleMatrix mxVV;

	//给出Johansen极大似然估计的关键矩阵，Suu，Svv，Suv
	CDoubleMatrix mxUU=mxU.dot(mxU)/(n*1.0);
	mxVV=(mxV.dot(mxV))/(n*1.0);
	CDoubleMatrix mxUV=(mxU.dot(mxV))/(n*1.0);
	if ((det(mxUU)<DoubleZero)||(det(mxVV)<DoubleZero))
	{
		CTString strWarning = "矩阵奇异，请重新设定模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return mxINFO;
	}
	
	CComplexMatrix mxEigVectComp;
	//构造M矩阵
	mxM = inv(mxVV)*(trans(mxUV))*inv(mxUU)*mxUV;	
	
	//得到特征值和特征向量矩阵
	CComplexMatrix mxEigValueComp=mxM.eig(mxEigVectComp);
	CDoubleMatrix mxEigVect=CDoubleMatrix(mxEigVectComp);
	CDoubleMatrix mxEigValue=CDoubleMatrix(mxEigValueComp);
	
	mxEigValueComp.destroy();
	mxEigVectComp.destroy();
	mxEigValueComp.destroy();
	int nDim = mxEigValue.mcol();
	//生成特征值向量
	CDoubleVector vrEigvalue(nDim);
	for (j=0; j< nDim; j++)
	{
		vrEigvalue(j)=mxEigValue(j,j);
	}
	
	//将特征向量根据对应的特征值的大小，按照降序排列
	CDoubleVector vrEigvectemp(nDim,1);
	double Eigvaluetemp=0;
	for (int k=0; k< nDim; k++)
	{
		for (int l=k+1; l<nDim; l++)
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
	
	//对特征向量进行正规化变换，给出特征值的变换
	CDoubleMatrix mxVECstand=mxEigVect.dot(mxEigVect);
	for (j=0; j<nDim; j++)
	{
		mxEigVect(j)=mxEigVect(j)/sqrt(mxVECstand(j,j));
	}
	
	///////////////////////////////////////
	
	//对特征向量实行Johansen正规化
	CDoubleMatrix mxTe =  mxEigVect.dot(mxEigVect);
	for (j=0; j<nDim; j++)
	{
		for (int k=0;k<nDim;k++)
		{
			mxEigVect(k,j)=mxEigVect(k,j)/sqrt(mxTe(j,j));
		}
		
	}
	mxTe =  mxEigVect.dot(mxVV)*mxEigVect;

	for (j=0; j<nDim; j++)
	{
		for (int k=0;k<nDim;k++)
		{
			mxEigVect(k,j)=mxEigVect(k,j)/sqrt(mxTe(j,j));
		}
		
	}
		
	CDoubleMatrix mxtemp=trans(mxEigVect);
	
	//给出模型的协整关系系数
	mxEigVect=mkeye(mxtemp,m_tIn.m_nEquNum);
	mxEigVect=trans(mxEigVect);

	return mxEigVect;
}

//回归方程检验函数
BOOL CVEC::Paratest(CDoubleMatrix mxBeta,CDoubleMatrix mxRes,CDoubleMatrix mxYnew,CDoubleMatrix mxXnew,int VARindex)
{
	//将自变量，因变量，残差，和方程序号带入，给出各个检验统计量，和方程拟合统计量
	int nN=mxXnew.mrow();
	int nB=mxXnew.mcol();
	CDoubleMatrix mxSSE=mxRes.dot(mxRes);
	CDoubleVector vrSST=var(mxYnew);
	
	//分别计算SSE，SSR，SST，R，adjR，F－test，SER
	m_tOut.m_vrSSE(VARindex)=mxSSE(0,0);
	m_tOut.m_vrSST(VARindex)=vrSST(0)*(nN-1);
	m_tOut.m_vrRsqu(VARindex)=1-m_tOut.m_vrSSE(VARindex)/m_tOut.m_vrSST(VARindex);
	m_tOut.m_vrAdjR(VARindex)=1-(m_tOut.m_vrSSE(VARindex)/double(nN-nB))/(m_tOut.m_vrSST(VARindex)/double(nN-1));
	m_tOut.m_vrFtest(VARindex)=m_tOut.m_vrRsqu(VARindex)/(1-m_tOut.m_vrRsqu(VARindex))/(nB-1)*(nN-nB);
	m_tOut.m_vrSSR(VARindex)=m_tOut.m_vrSST(VARindex)-m_tOut.m_vrSSE(VARindex);
	m_tOut.m_vrSER(VARindex)=sqrt(m_tOut.m_vrSSE(VARindex)/(nN-nB));
	
	//计算DW统计量
	m_tOut.m_vrDw(VARindex) = 0;
	int j;
	for (j=0; j<nN-1;j++)
	{
		m_tOut.m_vrDw(VARindex)=m_tOut.m_vrDw(VARindex) + pow(mxRes(j+1,0) - mxRes(j,0),2)/m_tOut.m_vrSSE(VARindex);
	}
	
	//计算MD，SDD
	CDoubleVector meantemp=mean(mxYnew);
	m_tOut.m_vrMD(VARindex)=meantemp(0);
	m_tOut.m_vrSDD(VARindex)=sqrt(m_tOut.m_vrSST(VARindex)/(nN-1));
	
	//计算回归系数矩阵的标准误，T值和P值
	CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
	if (det(mxXnewXnew)<DoubleZero)
	{
		CTString strWarning = "矩阵奇异，请重新选择模型!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	CDoubleMatrix mxinvX= inv(mxXnewXnew);
	
	for(j=0;j<nB;j++)
	{
		double setemp=mxinvX(j,j)*m_tOut.m_vrSSE(VARindex)/(nN-nB);
		
		m_tOut.m_mxSEReg(j,VARindex)=sqrt(setemp);
		m_tOut.m_mxTvalue(j,VARindex)=mxBeta(j,0)/m_tOut.m_mxSEReg(j,VARindex);
	}
	
	//计算DRC，AIC，SC准则
	m_tOut.m_vrDRC(VARindex)=m_tOut.m_vrSSE(VARindex)/nN;
	double dT1=nN;
	m_tOut.m_vrLogL(VARindex)=(1+log(2*pi))*(-dT1)/2-dT1/2*log(m_tOut.m_vrDRC(VARindex));
	m_tOut.m_vrAIC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+2*nB/dT1;
	m_tOut.m_vrSC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+nB*log(dT1)/dT1;
	return TRUE;
}



////////////////////////////////////////////////////////////////
///////					结果							////////
////////////////////////////////////////////////////////////////

void CVEC::DrawResult()
{
	//结果函数
								//创建回归系数矩阵估计表
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);	
	pTable1->SetTitle("协整关系矩阵估计");
	pTable1->InsertColumn(0,"");
	int i;
	for ( i=0; i<m_tIn.m_nEquNum;i++)
	{
		CTString st;
		st.Format("%d",i+1);
		st="第"+st+"个协整方程";
		pTable1->InsertColumn(0,st);
	}


	int number = 0;
	int m;
	for(m=0; m<m_tIn.m_nRegre; m++)
	{
		CTString str = m_strDepend[m] + "(-1)" ;
		pTable1->InsertItem(number, str);
		number++;
	}
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		pTable1->InsertItem(number,"常数项");
		number++;
		break;
	case 2:
		pTable1->InsertItem(number,"常数项");
		number++;
		break;
	case 3:
		pTable1->InsertItem(number,"趋势项");
		number++;
		pTable1->InsertItem(number,"常数项");
		number++;
		break;
	case 4:
		pTable1->InsertItem(number,"趋势项");
		number++;
		pTable1->InsertItem(number,"常数项");
		number++;
		break;
	default:
		;
	}
	int j;
	for (j=0; j<number; j++)
	{
		for (int k=0; k<m_tIn.m_nEquNum; k++)
		{
			pTable1->SetItemText(j,k+1,m_tOut.m_mxCoff(j,k));
		}
	}

	CRsltElementTable *pETable1 = new CRsltElementTable("协整关系矩阵估计", pTable1);
	m_pResult->Add(pETable1);
	

	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("误差修正模型估计");
	pTable2->InsertColumn(0,"");
	pTable2->InsertColumn(0,"");
	for(i=0; i<m_tIn.m_nRegre; i++)
	{
		pTable2->InsertColumn(0, m_strDepend[i]);
	}

	number=0;
	for (i=0; i<m_tIn.m_nEquNum; i++)
	{
		CTString STR;
		STR.Format("%d",i+1);
		pTable2->InsertItem(number,"协整方程"+STR);
		pTable2->SetItemText(number++, 1, "系数");
		
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");

		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "T值");
	}

	for(j=0; j<m_tIn.m_nLagnum; j++)						//插入系数标准误和系数检验的T值
	{
		CTString strstr;
		strstr.Format("%d",j+1);
		for(m=0; m<m_tIn.m_nRegre; m++)
		{
			CTString str = m_strDepend[m] + "(-" + strstr + ")";
			
			pTable2->InsertItem(number, str);
			for(int k=0; k<m_tIn.m_nRegre; k++)
			{
				pTable2->SetItemText(number,k+2,"");				
			}
			pTable2->SetItemText(number++, 1, "系数");
			pTable2->InsertItem(number, "");
			pTable2->SetItemText(number++, 1, "标准误");
			pTable2->InsertItem(number, "");
			pTable2->SetItemText(number++, 1, "t值");
		}			
	}
	
	switch(m_tIn.m_nModel)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:	
		pTable2->InsertItem(number,"常数项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "t值");
		break;
	case 3:
		pTable2->InsertItem(number,"常数项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "t值");
		break;
	case 4:
		pTable2->InsertItem(number, "趋势项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "t值");
		
		pTable2->InsertItem(number,"常数项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "t值");
		break;
	default:
		;
	}

	for(int k=0; k<(m_tIn.m_nExto); k++)
	{
		pTable2->InsertItem(number, m_strVarSel[k]);

		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");

		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "t值");
	}	
	
	


	number=0;
	for (i=0; i<m_tOut.m_mxCoff1.mrow(); i++)
	{

		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxCoff1(i,j));
		}
		number++;
		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxSEReg(i,j));
		}
		number++;
		for (j=0; j<m_tIn.m_nRegre;j++)
		{
			pTable2->SetItemText(number,j+2,m_tOut.m_mxTvalue(i,j));
		}
		number++;
	}
	
		CRsltElementTable *pETable2 = new CRsltElementTable("误差修正模型估计", pTable2);
		m_pResult->Add(pETable2);		
		
		

		
		//创建单个方程总结表
		CTLTable * pTable3 = new CTLTable;
		pTable3->CreateTable(2, 2);	
		pTable3->SetTitle("单个方程总结");
		pTable3->InsertColumn(0,"");
		for(int	n=0; n<m_tIn.m_nRegre; n++)
		{
			pTable3->InsertColumn(0, m_strDepend[n]);
		}
		
		pTable3->InsertItem(0, "复相关系数");
		for ( j=0; j<m_tIn.m_nRegre; j++)
			pTable3->SetItemText(0,j+1,m_tOut.m_vrRsqu(j));
		
		pTable3->InsertItem(1, "调整的复相关系数");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(1,j+1,m_tOut.m_vrAdjR(j));
		
		pTable3->InsertItem(2, "SSE");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(2,j+1,m_tOut.m_vrSSE(j));
		
		pTable3->InsertItem(3, "SST");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(3,j+1,m_tOut.m_vrSST(j));
		
		pTable3->InsertItem(4, "均值");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(4,j+1,m_tOut.m_vrMD(j));
		
		pTable3->InsertItem(5, "标准差");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(5,j+1,m_tOut.m_vrSDD(j));
		
		pTable3->InsertItem(6, "回归标准误");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(6,j+1,m_tOut.m_vrSER(j));
		
		pTable3->InsertItem(7, "F统计量");
		if (m_tIn.m_nRegre*m_tIn.m_nLagnum+m_tIn.m_nExto>=1)
		{
			for ( j=0; j<m_tIn.m_nRegre;j++)
				pTable3->SetItemText(7,j+1,m_tOut.m_vrFtest(j));	
		}
		else
		{
			for ( j=0; j<m_tIn.m_nRegre;j++)
				pTable3->SetItemText(7,j+1,"不存在");
		}
		
		
		pTable3->InsertItem(8, "对数似然值");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(8,j+1,m_tOut.m_vrLogL(j));
		
		pTable3->InsertItem(9, "AIC准则");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(9,j+1,m_tOut.m_vrAIC(j));
		
		pTable3->InsertItem(10, "SC准则");
		for ( j=0; j<m_tIn.m_nRegre;j++)
			pTable3->SetItemText(10,j+1,m_tOut.m_vrSC(j));
		
		CRsltElementTable *pETable3 = new CRsltElementTable("单个方程总结", pTable3);
		m_pResult->Add(pETable3);
		
		
		
		
		//创建整个模型拟和检验准则表
		CTLTable * pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);	
		pTable4->SetTitle("拟和检验准则");
		pTable4->InsertColumn(0, "检验准则");
		pTable4->InsertColumn(1, "取值");
		
		pTable4->InsertItem(0, "SBC");
		pTable4->SetItemText(0,1,m_tOut.m_dSBC);
		
		pTable4->InsertItem(1, "FPE");
		pTable4->SetItemText(1,1,m_tOut.m_dFPE);
		
		pTable4->InsertItem(2, "对数似然(Loglikelihood)");
		pTable4->SetItemText(2,1,m_tOut.m_dLogl);
		
		pTable4->InsertItem(3, "决定残差协方差(DRC)");
		pTable4->SetItemText(3,1,m_tOut.m_dDRC);
		
		pTable4->InsertItem(4, "调整的决定残差协方差(DRC)");
		pTable4->SetItemText(4,1,m_tOut.m_dAdjDRC);
		
		pTable4->InsertItem(5, "赤池信息量(AIC)");
		pTable4->SetItemText(5,1,m_tOut.m_dAIC);
		
		pTable4->InsertItem(6, "施瓦茨信息量(SC)");
		pTable4->SetItemText(6,1,m_tOut.m_dSC);
		CRsltElementTable *pETable4 = new CRsltElementTable("拟和检验准则", pTable4);
		m_pResult->Add(pETable4);
}





















