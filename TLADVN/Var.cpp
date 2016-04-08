// Var.cpp: implementation of the CVar class.
//
//////////////////////////////////////////////////////////////////////
#include "tlAdvn.h"
#include "StdAfx.h"
#include "Var.h"
const double epst=2.22044604925031e-016;

//////////////////////////////////////////////////////////////////////
//                         向量自回归模型							//
//						pzq lhq   2005.12							//
//////////////////////////////////////////////////////////////////////

CVar::CVar()
{
	m_bXWithconst = false;
	m_nForcast = 0;
	m_nLagnum = 2;
}

CVar::~CVar()
{
	m_tVarInfo.DeleteObject();
	m_tVarInfo_D.DeleteObject();
}

bool CVar::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//VARMODEL
//	/DATA=[FILE]
//	/REGVAR [varlsit]
//	/EXOGENOUS [varlsit]
//	/OUTPUT [MODELFIT] [RESID]
//	/OPTION [LAG({2**})] [NOCONST] [ONEFORECAST]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/EXOGENOUS", VOstr, ynames);
	if (ynames.GetSize() > 0)
	{
		if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
			return false;
	}
	CTStringArray xname;
	AnaSyntax::getVAR("/REGVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return false;
	
	m_bXWithconst = !AnaSyntax::findstr("/OPTION", "NOCONST", VOstr);
	m_nLagnum = 2;
	if (AnaSyntax::findstr("/OPTION", "LAG", VOstr))
		m_nLagnum = AnaSyntax::getvalue("/OPTION", "LAG", VOstr);
	m_nForcast = AnaSyntax::findstr("/OPTION", "ONEFORECAST", VOstr);
	m_bChartLine = AnaSyntax::findstr("/OUTPUT", "MODELFIT", VOstr);
	m_bChartScatterPlot = AnaSyntax::findstr("/OUTPUT", "RESID", VOstr);

	return true;
}

CResult * CVar::OnVAR(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult;
	CDoubleMatrix Matrix_X, Matrix_Y;					//回归矩阵和外生变量矩阵

	pResult=new CResult("向量自回归模型");
	m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}

	m_nRegre = m_tVarInfo.iCount;					//给出模型的回归变量的个数
	m_nExto = m_tVarInfo_D.iCount;					//给出外生变量的个数

	
	GetMatrixData(Matrix_X, Matrix_Y);			//从界面读数据
	m_mxX=Matrix_X;
	
	if (Varest(Matrix_X,m_bXWithconst,Matrix_Y,m_nLagnum,m_nForcast))	//Cvarest函数给出是否中途跳出的信息
	{
		DrawResult();								//画出输出结果
	}
	else
		return pResult;
	
	return pResult;
}

void CVar::GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y)//从界面读数据
{
	TCDataMatrix pDataSet;
	m_strVarSel.RemoveAll();						//擦除回归变量名信息
	m_strDepend.RemoveAll();						//擦除外生变量名信息
	CDWordArray indexary;
	int i,j;

	for( i=0; i<m_nRegre; i++ )						//读出回归变量的名字，存入字符串
	{			
		indexary.Add(m_tVarInfo.pValue[i]);
		m_strDepend.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
	}

	int n = m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815取回界面数据
	indexary.RemoveAll();

	for( j=0; j<m_nExto; j++)						//读出外生变量的名字，存入字符串
	{
		indexary.Add(m_tVarInfo_D.pValue[j]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[j]));
	}
	
	m_pDataInterface->GetColsData( indexary, Matrix_Y, 6 );//Modify by xds 20060815取回界面数据

}

///////////////////////////////////////////////////////////////
//						算法主程序							///
//						pzq 2005.12							///
///////////////////////////////////////////////////////////////
bool CVar::Varest(CDoubleMatrix& mxX,bool bXWithconst,CDoubleMatrix& mxExto,int nLagnum,int nForcastnum)
{
	//参数设置：mxX为输入的回归变量，bXWithconst为x是否含有
	//常数项的bool型指示变量，mxExto为模型含有的外生变量列表
	//nLagnum为模型的自回归滞后阶数，nForcastnum为模型的是否向前预报
	//估计方法使用极大似然方法

	if (nLagnum<=0) 
	{
		CTString strWarning = "模型参数设置错误!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}

	int	nExtom= m_nExto;
	int m=m_nRegre;									//给出回归变量的个数

	int n=m_mxX.mrow();
	int T=n-nLagnum;								//给出有效观察个数
	
	
	//模型设置错误的交互信息
	if (m_nRegre==0)								//未设置回归变量
	{
		CTString strWarning = "请先选择要分析的变量!";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}
	if ((T<=(m_nRegre*nLagnum+bXWithconst))|| T<=10)		//时间序列过短
	{
		CTString strWarning = "时间序列过短，不适合进行此分析";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}

	
	int np=m_nRegre*nLagnum+bXWithconst+nExtom;			//给出总共的回归变量个数

	CDoubleMatrix mxXPric;
	CDoubleMatrix mxU(T+1,np);
	if (m_nExto==0)									//不含有外生变量时
	{
		for (int i=0;i<T+1;i++)
			for (int j=0;j<nLagnum;j++)
				for (int k=0;k<m_nRegre;k++)
					mxU(i,j*m_nRegre+k+bXWithconst)=m_mxX(i-j+nLagnum-1,k);


		if (bXWithconst)							//若含有常数项时
		{
			mxU(0)=1;
		}
		mxXPric = mxU.slice(T,0,1,np);
		mxU = mxU.slice(T,np);
	}
	else											//含有外生变量时
	{
		for (int i=0;i<T;i++)
		{
			for (int j=0;j<nLagnum;j++)
				for (int k=0;k<m_nRegre;k++)
					mxU(i,j*m_nRegre+k+bXWithconst)=m_mxX(i-j+nLagnum-1,k);
				
			for (int l=0;l<nExtom;l++)
				mxU(i,nLagnum*m_nRegre+bXWithconst+l)=mxExto(i+nLagnum,l);

		}
		if (bXWithconst)							//若含有常数项时
		{
			mxU(0)=1;
		}
	}
	CDoubleMatrix mxXY(m_nRegre,np,0.0);				//给出XY矩阵
	CDoubleMatrix mxUt(np,np,0.0);					//给出Ut矩阵

	CDoubleMatrix mxXt=m_mxX.slice(nLagnum,0,T,m_nRegre);
	int i;
	for (i=0; i<np; i++)
	{
		for (int j=0; j<np; j++)
		{
			mxUt(i,j) = mxU(i).dot(mxU(j));
		}
	}
	for (i=0; i<m_nRegre; i++)
	{
		for (int k=0; k<np;k++)
		{
			mxXY(i,k) = mxXt(i).dot(mxU(k));
		}
	}

	
	if (det(mxUt)<DoubleZero)						//矩阵过于奇异
	{	
		CTString strWarning = "矩阵过于奇异！请重新选择";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}
	//计算系数估计

	CDoubleMatrix matrix_upu=mxU.dot(mxU);
	if (det(matrix_upu)<DoubleZero)
	{
		CTString strWarning = "矩阵过于奇异！请重新选择";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}
	CDoubleMatrix matrix_invupu=inv(matrix_upu);
	m_mxestB=mxXY*inv(mxUt);						//系数估计矩阵

	mxXY.destroy();
	mxUt.destroy();

	m_mxestB=m_mxestB.trans();					//回归系数估计矩阵
	
	m_mxYhat = mxU*m_mxestB;					//拟合预测值
	m_mxResidul = mxXt-m_mxYhat;
	if (m_nExto==0) 
	{
		m_mxForcast = mxXPric*m_mxestB;
	}
	//m_mxForcast=mxU.slice(T-1,0,1,mxU.mcol())*m_mxestB;//变量预测值
	//计算模型统计量

	mxU.destroy();

	CDoubleMatrix mxRese(m_nRegre,m_nRegre,0.0);
	int j;
	for (j=0;j<m_nRegre;j++)
	{
		for (int k=0;k<m_nRegre;k++)
		{
			mxRese(j,k)=m_mxResidul(j).dot(m_mxResidul(k));
		}
	}
	m_dDRC=det((CDoubleMatrix)(mxRese/(T*1.0)));									//计算模型的DRC
	if (m_dDRC<=DoubleZero)
	{
		CTString strWarning = "矩阵过于奇异！请重新选择";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return false;		
	}
	m_dAdjDRC=det((CDoubleMatrix)(mxRese/(T*1.0-bXWithconst-m_nRegre*nLagnum-nExtom)));	//计算模型的调整DRC

	mxRese.destroy();
	
	double dLp=log(m_dDRC);
	double dSBCtemp=dLp/m_nRegre;
	m_bSBC=(dSBCtemp)-(1.0-double(np)/double(T))*log(double(T));//计算SBC准则
	double dFPEtem=double(T)*(double(T-np))/(double(T)+double(np));
	m_bFPE=dSBCtemp-log(dFPEtem);							//计算FPE准则

		
	m_bLogl=(1.0+log(2*pi))*(-1.0)*(T*m_nRegre)/2-log(m_dDRC)*T/2;	//计算对数似然值
	//计算AIC准则
	m_bAIC=2*m_bLogl/(double)T*(-1)+2*double(m_nRegre)*double(bXWithconst+nLagnum*m_nRegre+nExtom)/double(T);
    //计算SC准则
	m_bSC=2.0*m_bLogl/T*(-1.0)+(double(m_nRegre)*(double(bXWithconst)+double(nLagnum)*double(m_nRegre)+nExtom))*log(T)/double(T);
	
	//计算分组变量的回归统计量
						
    m_vrFST.create(m_nRegre,0);										//F统计量
	m_vrSST.create(m_nRegre,0);										//SST
	m_vrSSE.create(m_nRegre,0);										//SSE
	m_vrRsqu.create(m_nRegre,0);										//R^2
	m_vrAdjRsqu.create(m_nRegre,0);									//调整的R^2	

	CDoubleVector mxMeanx(m_nRegre,0);
	m_vrSDD=st_d(mxXt);
	m_vrMD = mean(mxXt);

	mxXt.destroy();


	for (j=0;j<m_nRegre;j++)
	{
		m_vrSST(j) = pow(m_vrSDD(j),2)*(n-nLagnum-1);
		m_vrSSE(j) = m_mxResidul(j).dot(m_mxResidul(j));
		m_vrRsqu(j)=1.0-m_vrSSE(j)/m_vrSST(j);
		m_vrAdjRsqu(j)=1.0-m_vrSSE(j)/m_vrSST(j)*(double(T-1))/double((T-(bXWithconst+nLagnum*m_nRegre)-nExtom));
	}

	//计算分组的F－statistic
	if (bXWithconst+nLagnum*m_nRegre+nExtom>1) 
		for(i=0;i<m_nRegre;i++)
			m_vrFST(i)=(m_vrRsqu(i)/(double)(bXWithconst+nLagnum*m_nRegre+nExtom-1))/((1-m_vrRsqu(i))/(double)(T-(bXWithconst+nLagnum*m_nRegre)-nExtom));
	
	//计算分组的SE equation；
	CDoubleVector vrSertem(m_nRegre,0);
	for (i=0; i<m_nRegre;i++)
		vrSertem(i)=sqrt(m_vrSSE(i)/(double)(T-(bXWithconst+nLagnum*m_nRegre)-nExtom));
	m_vrSER=vrSertem;
	m_mxparastde.create(np,m_nRegre,0.0);
	m_mxparattest.create(np,m_nRegre,0.0);
	for (i=0;i<m_nRegre; i++)
		for (int j=0; j<np;j++)
		{
			m_mxparastde(j,i)=sqrt(matrix_invupu(j,j))*m_vrSER(i);
			m_mxparattest(j,i)=m_mxestB(j,i)/m_mxparastde(j,i);
		}
	// 计算分组的logL，AIC，SC
	CDoubleVector logLgroup(m_nRegre);
	CDoubleVector AICgroup(m_nRegre);
	CDoubleVector SCgroup(m_nRegre);
	for(i=0; i<m_nRegre;i++)
	{
		logLgroup(i)=double(T)*(-0.5)*(1+log(2*pi)+log(m_vrSSE(i)/double(T)));
		AICgroup(i)=(-2)*logLgroup(i)/T+double(2)*(bXWithconst+nLagnum*m_nRegre+nExtom)/double(T);
		SCgroup(i)=(-2)*logLgroup(i)/T+(bXWithconst+nLagnum*m_nRegre+nExtom)*log((double)T)/double(T);
	}
	m_vrLogLG=logLgroup;
	m_vrAICG=AICgroup;
	m_vrSCG=SCgroup;
	
	return true;
} 



///////////////////////////////////////////////
///					输出结果				///
///////////////////////////////////////////////

void CVar::DrawResult()
{
	if(m_nRegre > 0)							//创建回归系数矩阵估计表
	{
		CTLTable * pTable1 = new CTLTable;
		pTable1->CreateTable(2, 2);	
		pTable1->SetTitle("回归系数矩阵估计");
		pTable1->InsertColumn(0,"");
		pTable1->InsertColumn(0,"");
		for(int i=0; i<m_nRegre; i++)
		{
			pTable1->InsertColumn(0, m_strDepend[i]);
		}
		
		int number = 0;
		int j = 0;
		if(m_bXWithconst==1)
		{
			pTable1->InsertItem(number, "常数项");
			for(int k=0;k<(int)(m_bXWithconst);k++)
			{
				pTable1->SetItemText(number++, 1, "系数");
				for (j=0; j<m_nRegre; j++)
				{
					pTable1->SetItemText(3*k,j+2,m_mxestB(k,j));
				}
				pTable1->InsertItem(number, "");
				pTable1->SetItemText(number++, 1, "标准误");
				for (j=0; j<m_nRegre; j++)
				{
					pTable1->SetItemText(3*k+1,j+2,m_mxparastde(k,j));
				}
				pTable1->InsertItem(number, "");
				pTable1->SetItemText(number++, 1, "t值");
				for (j=0; j<m_nRegre; j++)
				{
					pTable1->SetItemText(3*k+2,j+2,m_mxparattest(k,j));
				}				
			}
		}
		
		for(j=0; j<m_nLagnum; j++)						//插入系数标准误和系数检验的T值
		{
			for(int m=0; m<m_nRegre; m++)
			{
				CTString strstr;
				strstr.Format("%d",j+1);
				CTString str = m_strDepend[m] + "(-" + strstr + ")";
				pTable1->InsertItem(number, str);
				int n;
				for(n=0; n<m_nRegre; n++)
				{
					pTable1->SetItemText(number,n+2,m_mxestB(j*m_nRegre+m+m_bXWithconst,n));				
				}
				pTable1->SetItemText(number++, 1, "系数");
				pTable1->InsertItem(number, "");
				for(n=0; n<m_nRegre; n++)
				{
					pTable1->SetItemText(number,n+2,m_mxparastde(j*m_nRegre+m+m_bXWithconst,n));				
				}
				pTable1->SetItemText(number++, 1, "标准误");
				pTable1->InsertItem(number, "");
				for(n=0; n<m_nRegre; n++)
				{
					pTable1->SetItemText(number,n+2,m_mxparattest(j*m_nRegre+m+m_bXWithconst,n));				
				}
				pTable1->SetItemText(number++, 1, "t值");
			}			
		}
		
		
		for(int k=0; k<(m_nExto); k++)
		{
			pTable1->InsertItem(number, m_strVarSel[k]);
			for (j=0; j<m_nRegre;j++)
			{
				pTable1->SetItemText(number,j+2,m_mxestB(k+m_nRegre*m_nLagnum+m_bXWithconst,j));
			}
			pTable1->SetItemText(number++, 1, "系数");
			pTable1->InsertItem(number, "");
			for (j=0; j<m_nRegre;j++)
			{
				pTable1->SetItemText(number,j+2,m_mxparastde(k+m_nRegre*m_nLagnum+m_bXWithconst,j));
			}
			pTable1->SetItemText(number++, 1, "标准误");
			pTable1->InsertItem(number, "");
			for (j=0; j<m_nRegre;j++)
			{
				pTable1->SetItemText(number,j+2,m_mxparattest(k+m_nRegre*m_nLagnum+m_bXWithconst,j));
			}
			pTable1->SetItemText(number++, 1, "t值");
		}
		
		CRsltElementTable *pETable1 = new CRsltElementTable("回归系数矩阵估计", pTable1);
		m_pResult->Add(pETable1);
		//创建单个方程总结表
		CTLTable * pTable2 = new CTLTable;
		pTable2->CreateTable(2, 2);	
		pTable2->SetTitle("单个方程总结");
 		pTable2->InsertColumn(0,"");
		for(int	n=0; n<m_nRegre; n++)
		{
			pTable2->InsertColumn(0, m_strDepend[n]);
		}
		
		pTable2->InsertItem(0, "复相关系数");
		for ( j=0; j<m_nRegre; j++)
			pTable2->SetItemText(0,j+1,m_vrRsqu(j));

		pTable2->InsertItem(1, "调整的复相关系数");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(1,j+1,m_vrAdjRsqu(j));

		pTable2->InsertItem(2, "SSE");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(2,j+1,m_vrSSE(j));

		pTable2->InsertItem(3, "SST");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(3,j+1,m_vrSST(j));
		
		pTable2->InsertItem(4, "均值");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(4,j+1,m_vrMD(j));
		
		pTable2->InsertItem(5, "标准差");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(5,j+1,m_vrSDD(j));

		pTable2->InsertItem(6, "回归标准误");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(6,j+1,m_vrSER(j));

		pTable2->InsertItem(7, "F统计量");
		if (m_bXWithconst+m_nRegre*m_nLagnum+m_nExto>1)
		{
			for ( j=0; j<m_nRegre;j++)
				pTable2->SetItemText(7,j+1,m_vrFST(j));	
		}
		else
		{
			for ( j=0; j<m_nRegre;j++)
				pTable2->SetItemText(7,j+1,"不存在");
		}


		pTable2->InsertItem(8, "对数似然值");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(8,j+1,m_vrLogLG(j));

		pTable2->InsertItem(9, "AIC准则");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(9,j+1,m_vrAICG(j));

		pTable2->InsertItem(10, "SC准则");
		for ( j=0; j<m_nRegre;j++)
			pTable2->SetItemText(10,j+1,m_vrSCG(j));

		CRsltElementTable *pETable2 = new CRsltElementTable("单个方程总结", pTable2);
		m_pResult->Add(pETable2);




													//创建整个模型拟和检验准则表
		CTLTable * pTable3 = new CTLTable;
		pTable3->CreateTable(2, 2);	
		pTable3->SetTitle("拟和检验准则");
		pTable3->InsertColumn(0, "检验准则");
		pTable3->InsertColumn(1, "取值");
		
		pTable3->InsertItem(0, "SBC");
		pTable3->SetItemText(0,1,m_bSBC);

		pTable3->InsertItem(1, "FPE");
		pTable3->SetItemText(1,1,m_bFPE);

		pTable3->InsertItem(2, "对数似然(Loglikelihood)");
		pTable3->SetItemText(2,1,m_bLogl);

		pTable3->InsertItem(3, "决定残差协方差(DRC)");
		pTable3->SetItemText(3,1,m_dDRC);

		pTable3->InsertItem(4, "调整的决定残差协方差(DRC)");
		pTable3->SetItemText(4,1,m_dAdjDRC);

		pTable3->InsertItem(5, "赤池信息量(AIC)");
		pTable3->SetItemText(5,1,m_bAIC);

		pTable3->InsertItem(6, "施瓦茨信息量(SC)");
		pTable3->SetItemText(6,1,m_bSC);
		CRsltElementTable *pETable3 = new CRsltElementTable("拟和检验准则", pTable3);
		m_pResult->Add(pETable3);
	
		

													//创建预测值表
		if (m_nForcast == 1) 
		{
			CTLTable * pTable4 = new CTLTable;
			pTable4->CreateTable(2, 2);	
			pTable4->SetTitle("预测");
			pTable4->InsertColumn(0,"变量名");
			pTable4->InsertColumn(0,"预测值");
			for(int m=0; m<m_nRegre; m++)
			{
				pTable4->InsertItem(m, m_strDepend[m]);
				pTable4->SetItemText(m,1,m_mxForcast(0,m));	
			}
				
			CRsltElementTable *pETable4 = new CRsltElementTable("预测值表", pTable4);
			m_pResult->Add(pETable4);
		}
	


		//创建残差散点图
		CTChartLine *pTChartLine;
		CRsltElementChart *pREC;
		TDataPointValVal *pDPSV1;
		TDataPointValVal *pDPSV2;
		int m_nXRow = m_mxResidul.mrow();
		int stepofj = 1;

		//若观测值超过3000，则采用平均抽样方法抽点
		const int MAX = 3000;
		if (m_nXRow > MAX)							
			stepofj = int(m_nXRow / MAX) ;
		int nXRow = m_nXRow / stepofj;
		pDPSV1 = new TDataPointValVal[nXRow];
		pDPSV2 = new TDataPointValVal[nXRow];
		
		if (m_bChartScatterPlot)
		{
			for(int ii=0; ii<m_nRegre; ii++)
			{
				CTString m_stry_name = m_strDepend.GetAt(ii);
				CDoubleVector residulvect=m_mxResidul(ii);
				CDoubleVector yhatvect = m_mxYhat(ii);
				for(j=0; j<nXRow; j++)
				{ 
					pDPSV1[j].fXVal = j * stepofj + 1;
					pDPSV2[j].fXVal = j * stepofj + 1;
					pDPSV1[j].fYVal = residulvect(j * stepofj);
					pDPSV2[j].fYVal = yhatvect(j * stepofj);
				}
				CRsltElementChart *pREC11 = NULL;
				TDataPointValVal DSPSV1;
				for(j=0; j<nXRow; j++)
				{			
					DSPSV1.fYVal = (residulvect(j * stepofj));	
					DSPSV1.fXVal = (yhatvect(j * stepofj));
				}
				//残差散点图
				TDataPointValVal* pDPVV =new TDataPointValVal[nXRow];        
	
				for(int i=0; i<nXRow; i++)
				{
					pDPVV[i].fXVal = (*(pDPSV2 + i)).fYVal;//预测); - (pDPSV2+i)->fYVal
					pDPVV[i].fYVal = (*(pDPSV1 + i)).fYVal;//实际-预测);
				}
				CTChartScatterPlot* pCL = new CTChartScatterPlot;
				pCL->SetTitle("残差散点图");
				pCL->SetXAxilLabel("预测 - "+ m_stry_name);
				pCL->SetYAxilLabel("残差");
				pCL->SetScatter(pDPVV, nXRow, ValVal, "");
				CRsltElementChart* pResidScat = new CRsltElementChart(m_stry_name + " - 残差散点图", pCL);
				m_pResult->Add(pResidScat);
				delete []pDPVV;
			}
		}

		//创建拟和图
		if ((m_bChartLine))
		{
			for(int jj=0; jj<m_nRegre; jj++)
			{				
				CTString m_stry_name = m_strDepend[jj];
				CDoubleVector yhatvect1 = m_mxYhat(jj);			//回顾变量的预测值
				CDoubleMatrix m_yini=m_mxX.slice(m_nLagnum,jj,m_mxX.mrow()-m_nLagnum,1);//m_regvar(jj);
				CDoubleVector vect_y=m_yini(0);				//原始变量
				for(j=0; j<nXRow; j++)
				{
					pDPSV1[j].fXVal = j * stepofj + 1;
					pDPSV2[j].fXVal = j * stepofj + 1;
					pDPSV1[j].fYVal = vect_y(j * stepofj);
					pDPSV2[j].fYVal = yhatvect1(j * stepofj);
				}
				pTChartLine = new CTChartLine;
				pREC = new CRsltElementChart(m_stry_name + "-拟合图",  pTChartLine);
				m_pResult->Add(pREC);    
				pTChartLine->SetLine(pDPSV1, nXRow, ValVal, "原始数据"); 
				pTChartLine->SetLine(pDPSV2, nXRow, ValVal, "拟合数据"); 
				double fxMin = DBL_MAX;
				double fxMax = -DBL_MAX;
				double fyMin = vect_y.Min() - (vect_y.Max()-vect_y.Min())/4;
				double fyMax = vect_y.Max() + (vect_y.Max()-vect_y.Min())/4;
			}
		}
		
		delete[] pDPSV1;//移动到外面，防止非预测时的内存泄漏 //#_S 2004-12-30 zlq
		delete[] pDPSV2;
		pDPSV1 = pDPSV2 = NULL;	
		
	}	
}



































