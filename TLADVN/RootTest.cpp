// RootTest.cpp: implementation of the CRootTest class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "RootTest.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

///////////////////////////////////////////////////////////////
//						算法主程序							///
//						pzq 2005.12							///
///////////////////////////////////////////////////////////////

CRootTest::CRootTest()
{
	m_nTestnum = 1;
}

CRootTest::~CRootTest()
{
	
}

BOOL CRootTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//ROOTTEST
//	/DATA=[FILE]
//	/VARLIST [varlsit]
//	/METHOD [{AUGMENT},{PHILLIPS},{KPSS}] [{NONET},{CONSTT},{CONTREND}]
//	/OPTION [DIFF({0**})] [LAG({0**})] [TRUNCAT({0**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;
	m_nRegre = m_tVarInfo.iCount;

	if (AnaSyntax::findstr("/METHOD", "PHILLIPS", VOstr))
		m_nTestnum = 2;
	else if (AnaSyntax::findstr("/METHOD", "KPSS", VOstr))
		m_nTestnum = 3;
	else
		m_nTestnum = 1;

	if (AnaSyntax::findstr("/METHOD", "NONET", VOstr))
		m_tIn.m_nModel = 0;
	else if (AnaSyntax::findstr("/METHOD", "CONSTT", VOstr))
		m_tIn.m_nModel = 1;
	else if (AnaSyntax::findstr("/METHOD", "CONTREND", VOstr))
		m_tIn.m_nModel = 2;
	else
		return FALSE;

	m_tIn.m_nLagnum = 0;
	if (AnaSyntax::findstr("/OPTION", "LAG", VOstr))
		m_tIn.m_nLagnum = AnaSyntax::getvalue("/OPTION", "LAG", VOstr);
	m_tIn.m_nDifnum = 0;
	if (AnaSyntax::findstr("/OPTION", "DIFF", VOstr))
		m_tIn.m_nDifnum = AnaSyntax::getvalue("/OPTION", "DIFF", VOstr);
	m_tIn.m_nTruncat = 0;
	if (AnaSyntax::findstr("/OPTION", "TRUNCAT", VOstr))
		m_tIn.m_nTruncat = AnaSyntax::getvalue("/OPTION", "TRUNCAT", VOstr);

	return TRUE;
}

//函数主程序
CResult * CRootTest::OnRun(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	pResult=new CResult("序列单位根检验");
	m_pResult = pResult;
	CDoubleMatrix Matrix_X;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	GetMatrixData(Matrix_X);
	m_tIn.m_mxX = Matrix_X;
	
	///判断数据输入是否正确
	if ((m_tIn.m_nDifnum<0) | (m_tIn.m_nLagnum<0)) 
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	if (m_nTestnum==1)    //使用ADF检验
	{
		
		if(ADFtest())
			DrawResult();
		return pResult;
	}
	else if (m_nTestnum==2) //使用PP检验
	{
		
		if (PPtest())
			DrawResult();
		return pResult;
	}
	else if (m_nTestnum==3)	//使用KPSS检验
	{
		
		if (KPSStest())
			DrawResult();
		return pResult;
	}
	else 
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
}

//获得界面数据
void CRootTest::GetMatrixData(CDoubleMatrix & Matrix_X)
{
	m_strVarSel.RemoveAll();						//擦除回归变量名信息
	CDWordArray indexary;
	int i;
	
	for( i=0; i<m_nRegre; i++ )						//读出回归变量的名字，存入字符串
	{			
		indexary.Add(m_tVarInfo.pValue[i]);
		m_strVarSel.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
	}
	
	m_pDataInterface->GetColsData( indexary, Matrix_X, 6 );//Modify by xds 20060815取回界面数据
}

//////////////////////////////////////////////////////
//					ADF检验主程序					//
//////////////////////////////////////////////////////
BOOL CRootTest::ADFtest()
{
	
	int nXcol=m_tIn.m_mxX.mcol();
	int nXrow=m_tIn.m_mxX.mrow();
	
	//判断是否时间序列过短而无法检验
	if ((m_tIn.m_nDifnum+2*m_tIn.m_nLagnum+2*m_tIn.m_nModel>=nXrow) || nXrow<4) 
	{
		CTString strWarning = "时间序列过短，不适合进行此分析";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	
	//使用Vdif差分函数给出差分后的自变量
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXcol=m_tIn.m_mxX.mcol();
	nXrow=mxXIni.mrow();
	
	//创造新的全局输出变量
	m_tOut.m_vrTest.create(nXcol,0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	
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
	
	
	//给出检验的临界值表
	CDoubleMatrix mxCriticalValue(6,3,0.0);
	double Cvalue1[6]={-1.95,-1.95,-1.95,-1.95,-1.95,-1.95};
	double Cvalue2[6]={-3,-2.93,-2.89,-2.88,-2.87,-2.86};
	double Cvalue3[6]={-3.6,-3.5,-3.45,-3.43,-3.42,-3.41};
	mxCriticalValue(0)=Cvalue1;
	mxCriticalValue(1)=Cvalue2;
	mxCriticalValue(2)=Cvalue3;
	if (nXrow-1<=25)
	{
		m_tOut.m_dTdf=mxCriticalValue(0,m_tIn.m_nModel);
	}
	else if(nXrow-1>25 && nXrow-1<=50)
	{
		m_tOut.m_dTdf=mxCriticalValue(1,m_tIn.m_nModel);
	}
	else if (nXrow-1>50 && nXrow-1<=100) 
	{
		m_tOut.m_dTdf=mxCriticalValue(2,m_tIn.m_nModel);
	}
	else if (nXrow-1>100 && nXrow-1<=250) 
	{
		m_tOut.m_dTdf=mxCriticalValue(3,m_tIn.m_nModel);
	}
	else if (nXrow-1>250 && nXrow-1<=500)
	{
		m_tOut.m_dTdf=mxCriticalValue(4,m_tIn.m_nModel);
	}
	else if (nXrow-1>500)
	{
		m_tOut.m_dTdf=mxCriticalValue(5,m_tIn.m_nModel);
	}
	
	
	// 逐个对各个变量进行检验
	int VARindex=0;
	for(VARindex=0;VARindex<nXcol;VARindex++)
	{

		//构造初始回归矩阵
		CDoubleMatrix mxXi=mxXIni.slice(0,VARindex,nXrow-1,1);
		CDoubleMatrix mxY=mxXIni.slice(1,VARindex,nXrow-1,1);
		CDoubleMatrix mxDeltaX=mxY-mxXi;
		mxY.destroy();
		
		
		if (m_tIn.m_nLagnum==0)					//滞后阶数为0时， 
		{
			CDoubleMatrix mxYnew=mxDeltaX.copy();
			mxDeltaX.destroy();

			CDoubleMatrix mxXnew=mxXi.copy();
			if (m_tIn.m_nModel==1)				//含有常数项
			{
				CDoubleMatrix mxUnit(nXrow-1,1,1.0);
				mxXnew=mxXi.commat(mxUnit,1);
				mxUnit.destroy();
			}
			else if(m_tIn.m_nModel==2)			//含有常数项和趋势项
			{
				CDoubleMatrix mxUnit(nXrow-1,1,1.0);
				CDoubleMatrix tem(nXrow-1,1,0.0);
				int j=0 ;
				for(j=0 ; j<nXrow-1 ; j++)
					tem(j,0)=j;
				mxXnew=mxXi.commat(mxUnit,1);
				mxXnew=mxXnew.commat(tem,1);
				mxUnit.destroy();
				tem.destroy();
			}
			mxXi.destroy();

			//给出系数估计矩阵Beta
			CDoubleMatrix mxBeta=inv(mxXnew.dot(mxXnew))*(mxXnew.dot(mxYnew));
			CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
			m_tOut.m_mxBeta(VARindex)=mxBeta(0);
			
			//调用函数Paratest给出各个方程的统计量
			if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex))
			{
				return FALSE;
			}
			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
			//计算检验统计量值
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);			
		}
		else					//滞后阶数不为0时
		{
			CDoubleMatrix mxDeltaXt(nXrow-m_tIn.m_nLagnum-1,m_tIn.m_nLagnum,1.0);
			int j=0; 
			for (j=0; j<m_tIn.m_nLagnum;j++)
			{
				for (int k=0; k<nXrow-m_tIn.m_nLagnum-1;k++)
				{
					mxDeltaXt(k,j)=mxDeltaX(m_tIn.m_nLagnum+k-1-j,0);					
				}
			}
		//给出新的回归变量
			CDoubleMatrix mxXinew=mxXi.slice(m_tIn.m_nLagnum,0,nXrow-m_tIn.m_nLagnum-1,1);
			CDoubleMatrix mxYnew=mxDeltaX.slice(m_tIn.m_nLagnum,0,nXrow-m_tIn.m_nLagnum-1,1);

			mxDeltaX.destroy();
			CDoubleMatrix mxUnit(nXrow-m_tIn.m_nLagnum-1,1,1.0);
			CDoubleMatrix tem(nXrow-1-m_tIn.m_nLagnum,1,0.0);
			CDoubleMatrix mxXnew;
			
		//给出趋势项
			for(j=0 ; j<nXrow-1-m_tIn.m_nLagnum ; j++)
				tem(j,0)=j+m_tIn.m_nLagnum+2;
			
			//对不同的模型给出不同的选择
			switch(m_tIn.m_nModel)
			{
			case 0:			//不含常数项
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				break;
			case 1:			//含阐述项
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				mxXnew = mxXnew.commat(mxUnit,1);
				break;
			case 2:			//含常数项和趋势项
				mxXnew = mxXinew.commat(mxDeltaXt,1);
				mxXnew = mxXnew.commat(mxUnit,1);
				mxXnew = mxXnew.commat(tem,1);			
				break;
			default:
				{
					CTString strWarning = "参数设置错误，请重新设置!";
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
					return FALSE;
				}
			}
			mxUnit.destroy();
			tem.destroy();

			//计算回归系数矩阵和残差矩阵
			CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
			if (det(mxXnewXnew)<DoubleZero)
			{
				CTString strWarning = "矩阵过于奇异，请重新选择模型!";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				return FALSE;
			}
			CDoubleMatrix mxBeta=inv(mxXnewXnew)*(mxXnew.dot(mxYnew));
			CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
			
			//将系数估计矩阵传回全局变量
			m_tOut.m_mxBeta(VARindex)=mxBeta(0);
			if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex))
			{
				return FALSE;
			}

			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
			
			//将检验得到的统计量传回全局变量
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////
//				调用PPtest函数给出pp检验				//
//////////////////////////////////////////////////////////
BOOL CRootTest::PPtest()
{
	int nXcol=m_tIn.m_mxX.mcol();
	int nXrow=m_tIn.m_mxX.mrow();

	//判断是否时间序列过短，给出相应的提示
	if (m_tIn.m_nDifnum+m_tIn.m_nLagnum>=nXrow || nXrow<4) 
	{
		CTString strWarning = "时间序列过短，不适合进行此分析";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	//调用差分函数Vdif，给出差分后的序列
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXcol=mxXIni.mcol();
	nXrow=mxXIni.mrow();
	
	//创建输出全局变量
	m_tOut.m_vrTest.create(nXcol,0.0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nLagnum+m_tIn.m_nModel,nXcol,0.0);
	
	m_tOut.m_vrSSE.create(nXcol,0.0);
	m_tOut.m_vrSST.create(nXcol,0.0);
	m_tOut.m_vrRsqu.create(nXcol,0.0);
	m_tOut.m_vrAdjR.create(nXcol,0.0);
	m_tOut.m_vrSSR.create(nXcol,0.0);
	m_tOut.m_vrSER.create(nXcol,0.0);
	m_tOut.m_vrDw.create(nXcol,0.0);
	m_tOut.m_vrMD.create(nXcol,0.0);
	m_tOut.m_vrSDD.create(nXcol,0.0);
	m_tOut.m_vrFtest.create(nXcol,0.0);
	m_tOut.m_vrDRC.create(nXcol,0.0);
	m_tOut.m_vrLogL.create(nXcol,0.0);
	m_tOut.m_vrAIC.create(nXcol,0.0);
	m_tOut.m_vrSC.create(nXcol,0.0);
	
	//给出检验的临界值	
	CDoubleMatrix mxCriticalValue(6,3,0.0);
	double Cvalue1[6]={-1.95,-1.95,-1.95,-1.95,-1.95,-1.95};
	double Cvalue2[6]={-3,-2.93,-2.89,-2.88,-2.87,-2.86};
	double Cvalue3[6]={-3.6,-3.5,-3.45,-3.43,-3.42,-3.41};
	mxCriticalValue(0)=Cvalue1;
	mxCriticalValue(1)=Cvalue2;
	mxCriticalValue(2)=Cvalue3;
	if (nXrow-1<=25)
	{
		m_tOut.m_dTdf=mxCriticalValue(0,m_tIn.m_nModel);
	}
	else if(nXrow-1>25 && nXrow-1<=50)
	{
		m_tOut.m_dTdf=mxCriticalValue(1,m_tIn.m_nModel);
	}
	else if (nXrow-1>50 && nXrow-1<=100) 
	{
		m_tOut.m_dTdf=mxCriticalValue(2,m_tIn.m_nModel);
	}
	else if (nXrow-1>100 && nXrow-1<=250) 
	{
		m_tOut.m_dTdf=mxCriticalValue(3,m_tIn.m_nModel);
	}
	else if (nXrow-1>250 && nXrow-1<=500)
	{
		m_tOut.m_dTdf=mxCriticalValue(4,m_tIn.m_nModel);
	}
	else if (nXrow-1>500)
	{
		m_tOut.m_dTdf=mxCriticalValue(5,m_tIn.m_nModel);
	}
	
	
	//逐个计算每个变量检验
	int VARindex=0;
	for(VARindex=0;VARindex<nXcol;VARindex++)
	{	
		CDoubleMatrix mxXnew=mxXIni.slice(0,VARindex,nXrow-1,1);
		CDoubleMatrix mxY=mxXIni.slice(1,VARindex,nXrow-1,1);
		CDoubleMatrix mxYnew=mxY-mxXnew;
		
		mxY.destroy();

		CDoubleMatrix mxUnit(nXrow-1,1,1.0);

		//给出趋势项
		CDoubleMatrix tem(nXrow-1,1,0.0);
		int j=0 ;
		for(j=0 ; j<nXrow-1 ; j++)
			tem(j,0)=j;

		//分类讨论各种模型
		if (m_tIn.m_nModel==1)
		{
			mxXnew=mxXnew.commat(mxUnit,1);
		}
		else if(m_tIn.m_nModel==2)
		{

			mxXnew=mxXnew.commat(mxUnit,1);
			tem = tem + (m_tIn.m_nDifnum+1.0)*mxUnit;
			mxXnew=mxXnew.commat(tem,1);
		}

		mxUnit.destroy();
		tem.destroy();
		
		CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
		if (det(mxXnewXnew)<DoubleZero)
		{
			CTString strWarning = "矩阵过于奇异，请重新选择模型!";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		//得到回归系数Beta
		CDoubleMatrix mxInvX=inv(mxXnewXnew);
		CDoubleMatrix mxBeta=mxInvX*(mxXnew.dot(mxYnew));
		
		//得到回归残差矩阵
		CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;

		m_tOut.m_mxBeta(VARindex)=mxBeta(0);
		
		//调用Paratest函数得到单个模型的统计量
		if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex)) 
		{
			return FALSE;
		}
		
		//计算检验的统计量，使用易丹辉《Eviews应用》中的计算公式
		if (m_tIn.m_nLagnum==0)		//当设置窗宽为0时
		{
			m_tOut.m_vrTest(VARindex) = m_tOut.m_mxTvalue(0,VARindex);
			mxBeta.destroy();
			mxRes.destroy();
			mxYnew.destroy();
			mxXnew.destroy();
		}
		else						//当设置窗宽不为0时
		{
			int nB=mxXnew.mcol();
			int nXrow=mxXnew.mrow();
			
			mxBeta.destroy();
			mxYnew.destroy();
			mxXnew.destroy();

			double dStdb=m_tOut.m_mxSEReg(0,VARindex);
			double tvalue=m_tOut.m_mxTvalue(0,VARindex);
			double dGamm0=0;
			int j=0; 
			for(j=0; j<nXrow ; j++)
				dGamm0=dGamm0+mxRes(j,0)*mxRes(j,0);
			double brandwith=m_tIn.m_nLagnum;
			double dSigma=dGamm0/(nXrow-nB);
			dGamm0=dGamm0/nXrow;
			CDoubleMatrix mxGama(m_tIn.m_nTruncat,1,0.0);
			for(j=1;j<=brandwith;j++)
			{
				for(int k=j+1;k<=nXrow;k++)
				{
					mxGama(j-1,0)=mxGama(j-1,0)+mxRes(k-1,0)*mxRes(k-j-1,0);
				}
			}
			mxGama=mxGama/(nXrow*1.0);
			
			double dOmiga2=dGamm0;
			for (j=1;j<=brandwith;j++)
				dOmiga2=dOmiga2+2*mxGama(j-1,0)*(1-j/((double)brandwith+1));
			m_tOut.m_vrTest(VARindex)=sqrt(dGamm0/dOmiga2)*tvalue-(dOmiga2-dGamm0)*(nXrow)*dStdb/(2*sqrt(dOmiga2*dSigma));
		}

	}
	return TRUE;
	
}

//////////////////////////////////////////////////////////
//					调用KPSS检验主程序					//
//////////////////////////////////////////////////////////
BOOL CRootTest::KPSStest()
{
	int nXrow=m_tIn.m_mxX.mrow();
	int nXcol=m_tIn.m_mxX.mcol();
	
	//给出模型是否过短的判断
	if (m_tIn.m_nDifnum+m_tIn.m_nLagnum>=nXrow||nXrow<4) 
	{
		CTString strWarning = "时间序列过短，不适合进行此分析";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}
	//初始化各个输出结果。
	m_tOut.m_vrTest.create(nXcol,0);
	m_tOut.m_mxBeta.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxSEReg.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxTvalue.create(1+m_tIn.m_nModel,nXcol,0.0);
	m_tOut.m_mxPvalue.create(1+m_tIn.m_nModel,nXcol,0.0);
	
	m_tOut.m_vrSSE.create(nXcol,0.0);
	m_tOut.m_vrSST.create(nXcol,0.0);
	m_tOut.m_vrRsqu.create(nXcol,0.0);
	m_tOut.m_vrAdjR.create(nXcol,0.0);
	m_tOut.m_vrSSR.create(nXcol,0.0);
	m_tOut.m_vrSER.create(nXcol,0.0);
	m_tOut.m_vrDw.create(nXcol,0.0);
	m_tOut.m_vrMD.create(nXcol,0.0);
	m_tOut.m_vrSDD.create(nXcol,0.0);
	m_tOut.m_vrFtest.create(nXcol,0.0);
	m_tOut.m_vrDRC.create(nXcol,0.0);
	m_tOut.m_vrLogL.create(nXcol,0.0);
	m_tOut.m_vrAIC.create(nXcol,0.0);
	m_tOut.m_vrSC.create(nXcol,0.0);
	
	//调用Vdif函数给出差分后的自变量
	CDoubleMatrix mxXIni=Vdif(m_tIn.m_mxX,m_tIn.m_nDifnum);
	nXrow=mxXIni.mrow();
	CDoubleMatrix mxUnit(nXrow,1,1.0);
	CDoubleMatrix tem(nXrow,1,0.0);
	int j=0 ; 
	for(j=0 ; j<nXrow ; j++)
		tem(j,0)=j;
	
	//逐个给出每个变量的检验
	int VARindex=0;
	for(VARindex=0;VARindex<m_nRegre;VARindex++)
	{
		CDoubleMatrix mxXnew;
		CDoubleMatrix mxYnew = mxXIni.slice(0,VARindex,nXrow,1);
		
		if (m_tIn.m_nModel==1)		//
		{
			mxXnew=mxUnit;
		}
		else
		{
			tem = tem + (m_tIn.m_nDifnum * 1.0)*mxUnit;
			mxXnew=mxUnit.commat(tem,1);
		}

		//给出模型的回归系数估计
		CDoubleMatrix mxXnewXnew = mxXnew.dot(mxXnew);
		if (det(mxXnewXnew)<DoubleZero)
		{
			CTString strWarning = "矩阵过于奇异，请重新选择模型!";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return FALSE;
		}
		CDoubleMatrix mxBeta=inv(mxXnewXnew)*(mxXnew.dot(mxYnew));
		m_tOut.m_mxBeta(VARindex)=mxBeta(0);

		//给出方程的残差
		CDoubleMatrix mxRes=mxYnew-mxXnew*mxBeta;
		
		//调用Paratest函数给出单个方程的统计量
		if (!Paratest(mxBeta,mxRes,mxYnew,mxXnew,VARindex)) 
		{
			return FALSE;
		}
		mxBeta.destroy();
		mxYnew.destroy();
		//mxXnew.destroy();

		//计算模型的单位根检验统计量，使用Eviews帮助上的方法
		if (m_tIn.m_nModel==1)
		{
			m_tOut.m_dTdf=0.463;
		}
		else m_tOut.m_dTdf=0.146;
		CDoubleVector vrSt(nXrow,0);
		vrSt(0)=mxRes(0,0);
		int k=1; 
		for(k=1; k<nXrow; k++)
		{

			vrSt(k)=vrSt(k-1)+mxRes(k,0);
			
		}


		CDoubleMatrix mxGama0 = mxRes.dot(mxRes);
		double dGamm0=mxGama0(0,0);
		
		if(m_tIn.m_nLagnum==0)		//当窗宽为0时
		{

			double test=vrSt.dot(vrSt);
			double test1=dGamm0*nXrow;
			m_tOut.m_vrTest(VARindex)=test/test1;
		}
		else						//当窗宽为非0时
		{
			double brandwith=m_tIn.m_nLagnum;
			dGamm0=dGamm0/nXrow;
			CDoubleMatrix mxGama(brandwith,1,0.0);
			//计算Gama
			for(j=1;j<=brandwith;j++)
			{

				CDoubleMatrix mxtemp1 = mxRes.slice(j,0,nXrow-j,1.0);
				CDoubleMatrix mxtemp2 = mxRes.slice(0,0,nXrow-j,1.0);
				CDoubleMatrix mxtemp3 = mxtemp1.dot(mxtemp2);
				mxGama(j-1,0) = mxtemp3(0,0);
				mxtemp1.destroy();
				mxtemp2.destroy();
			}
			mxGama=mxGama/(nXrow*1.0);


			double dOmiga2=dGamm0;
			for (j=1;j<=brandwith;j++)
				dOmiga2=dOmiga2+2*mxGama(j-1,0)*(1-j/((double)brandwith+1));
			//计算检验统计量

			double dtemp =  vrSt.dot(vrSt);
			m_tOut.m_vrTest(VARindex) = dtemp/(pow(nXrow,2)*dOmiga2);

		}
		mxRes.destroy();
		
	}
	return TRUE;
}


//回归方程检验函数
BOOL CRootTest::Paratest(CDoubleMatrix mxBeta,CDoubleMatrix mxRes,CDoubleMatrix mxYnew,CDoubleMatrix mxXnew,int VARindex)
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
	int j=0; 
	for (j=0; j<nN-1;j++)
	{
		m_tOut.m_vrDw(VARindex)=m_tOut.m_vrDw(VARindex) + pow(mxRes(j+1,0) - mxRes(j,0),2)/m_tOut.m_vrSSE(VARindex);
	}
	
	//计算MD，SDD
	CDoubleVector meantemp=mean(mxYnew);
	m_tOut.m_vrMD(VARindex)=meantemp(0);
	m_tOut.m_vrSDD(VARindex)=sqrt(m_tOut.m_vrSST(VARindex)/(nN-1));
	
	//计算回归系数矩阵的标准误，T值和P值
	CDoubleMatrix mxinvX= inv(mxXnew.dot(mxXnew));
	for(j=0;j<nB;j++)
	{
		double setemp=mxinvX(j,j)*m_tOut.m_vrSSE(VARindex)/(nN-nB);
		m_tOut.m_mxSEReg(j,VARindex)=sqrt(setemp);
		m_tOut.m_mxTvalue(j,VARindex)=mxBeta(j,0)/m_tOut.m_mxSEReg(j,VARindex);
		m_tOut.m_mxPvalue(j,VARindex)=2*(1-tcdf(fabs(m_tOut.m_mxTvalue(j,VARindex)),nN-nB));
	}
	
	//计算DRC，AIC，SC准则
	m_tOut.m_vrDRC(VARindex)=m_tOut.m_vrSSE(VARindex)/nN;
	double dT1=nN;
	m_tOut.m_vrLogL(VARindex)=(1+log(2*pi))*(-dT1)/2-dT1/2*log(m_tOut.m_vrDRC(VARindex));
	m_tOut.m_vrAIC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+2*nB/dT1;
	m_tOut.m_vrSC(VARindex)=(-2)*m_tOut.m_vrLogL(VARindex)/dT1+nB*log(dT1)/dT1;
	return TRUE;
}


//差分函数，输入原始矩阵和差分阶数ndif，给出差分后的矩阵
CDoubleMatrix CRootTest::Vdif(CDoubleMatrix mxX,int ndif)
{
	int nN=mxX.mrow();
	int nM=mxX.mcol();
	if (ndif>0)
	{
		int nMr=mxX.mcol();
		int nNr=mxX.mrow();
		int nk=0;
		int nJ=0;
		for(int l=1;l<=ndif;l++)
		{
			nk=nN-l;
			nJ=nNr;
			CDoubleMatrix mxXd(nk,nMr);
			while (nk>0)
			{	
				for (int j=0; j<nMr; j++)
					mxXd(nk-1,j)=mxX(nJ-1,j)-mxX(nJ-2,j);
				nJ=nJ-1;nk=nk-1;
			}
			mxX=mxXd;
			nNr=mxX.mrow();
		}
	}
	return mxX;
}


//结果函数 
void CRootTest::DrawResult()
{
	CTLTable * pTable1 = new CTLTable;
	pTable1->CreateTable(2, 2);
	CTString strADF="Augmented Dickey-Fuller 检验";
	CTString strPP="Phillips-Perron 检验";
	CTString strKPSS="Kwiatkowski-Phillips-Schmidt-Shin 检验";
	CTString strtemp("单位根检验统计量");
	if (m_nTestnum==1)
	{
		strtemp=strtemp+"---"+strADF;	
		
	}else if (m_nTestnum==2)
	{
		strtemp=strtemp+"---"+strPP;
	}
	else if (m_nTestnum==3)
	{
		strtemp=strtemp+"---"+strKPSS;
	}
	pTable1->SetTitle(strtemp);
	pTable1->InsertColumn(0, "");
	
	int i=0; 
	for(i=0; i<m_nRegre; i++)
		pTable1->InsertColumn(0, m_strVarSel[i]);
	
	pTable1->InsertItem(0, "检验统计量");
	for(i=0; i<m_nRegre; i++)
		pTable1->SetItemText(0, i+1, m_tOut.m_vrTest(i));
	TraceVector(m_tOut.m_vrTest);
	pTable1->InsertItem(1, "临界值(水平5%)");
	for(i=0; i<m_nRegre; i++)
		pTable1->SetItemText(1, i+1, m_tOut.m_dTdf);
	
	CRsltElementTable *pETable1 = new CRsltElementTable(strtemp, pTable1);
	m_pResult->Add(pETable1);
	
	
	//创建单个方程总结表
	CTLTable * pTable2 = new CTLTable;
	pTable2->CreateTable(2, 2);	
	pTable2->SetTitle("系数估计");
	pTable2->InsertColumn(0,"");
	pTable2->InsertColumn(0,"");
	for(i=0; i<m_nRegre; i++)
		pTable2->InsertColumn(0, m_strVarSel[i]);
	if (m_nTestnum!=3) 
	{
		pTable2->InsertItem(0, "回归项");
		pTable2->SetItemText(0, 1, "系数");
		pTable2->InsertItem(1, "");
		pTable2->SetItemText(1, 1, "标准误");
		pTable2->InsertItem(2, "");
		pTable2->SetItemText(2, 1, "T值");
		pTable2->InsertItem(3, "");
		pTable2->SetItemText(3, 1, "P值");
	}
	int number=0;
	if (m_nTestnum==1)
	{
		for(i=1; i<=m_tIn.m_nLagnum; i++)
		{
			CTString str;
			str.Format("%d", i);
			CTString strstr;
			strstr = _T("滞后");
			strstr += str;
			pTable2->InsertItem(i*4, strstr);
			pTable2->InsertItem(i*4+1, "");
			pTable2->InsertItem(i*4+2, "");
			pTable2->InsertItem(i*4+3, "");
			
			pTable2->SetItemText(i*4, 1, "系数");
			pTable2->SetItemText(i*4+1, 1, "标准误");
			pTable2->SetItemText(i*4+2, 1, "T值");
			pTable2->SetItemText(i*4+3, 1, "P值");
		}
		number = i*4;
	}
	else if (m_nTestnum==2) 
	{
		number=4;
	} 
	
	if(m_tIn.m_nModel == 1 || m_tIn.m_nModel == 2)
	{
		pTable2->InsertItem(number, "常数项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "T值");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "P值");
	}
	if(m_tIn.m_nModel == 2)
	{
		pTable2->InsertItem(number, "趋势项");
		pTable2->SetItemText(number++, 1, "系数");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "标准误");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "T值");
		pTable2->InsertItem(number, "");
		pTable2->SetItemText(number++, 1, "P值");
	}
	int nRegre=0;
	if (m_nTestnum==1)
	{
		nRegre=1+m_tIn.m_nLagnum+m_tIn.m_nModel;
	}
	else if (m_nTestnum==2)
	{
		nRegre=1+m_tIn.m_nModel;
	}
	else
		nRegre=m_tIn.m_nModel;
	
	int j=0;
	for(j=0;j<nRegre;j++)
	{
		for(int k=0; k<m_nRegre; k++)
		{			
			pTable2->SetItemText(j*4, k+2, m_tOut.m_mxBeta(j,k));
			pTable2->SetItemText(j*4+1, k+2, m_tOut.m_mxSEReg(j,k));
			pTable2->SetItemText(j*4+2, k+2, m_tOut.m_mxTvalue(j,k));
			pTable2->SetItemText(j*4+3, k+2, m_tOut.m_mxPvalue(j,k));
		}
	}
	CRsltElementTable *pETable2 = new CRsltElementTable("系数估计", pTable2);
	m_pResult->Add(pETable2);
	
	//创建整个模型拟和检验准则表
	CTLTable * pTable3 = new CTLTable;
	pTable3->CreateTable(2, 2);	
	pTable3->SetTitle("单个方程统计量");
	pTable3->InsertColumn(0, "");
	for(i=0; i<m_nRegre; i++)
		pTable3->InsertColumn(0, m_strVarSel[i]);
	
	pTable3->InsertItem(0, "复相关系数");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(0,i+1,m_tOut.m_vrRsqu(i));
	
	pTable3->InsertItem(1, "调整的复相关系数");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(1,i+1,m_tOut.m_vrAdjR(i));
	
	pTable3->InsertItem(2, "SSE");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(2,i+1,m_tOut.m_vrSSE(i));
	
	pTable3->InsertItem(3, "SST");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(3,i+1,m_tOut.m_vrSST(i));
	
	pTable3->InsertItem(4, "均值");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(4,i+1,m_tOut.m_vrMD(i));
	
	pTable3->InsertItem(5, "标准差");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(5,i+1,m_tOut.m_vrSDD(i));
	
	pTable3->InsertItem(6, "回归标准误");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(6,i+1,m_tOut.m_vrSER(i));
	
	pTable3->InsertItem(7, "F统计量");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(7,i+1,m_tOut.m_vrFtest(i));
	
	pTable3->InsertItem(8, "对数似然值");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(8,i+1,m_tOut.m_vrLogL(i));
	
	pTable3->InsertItem(9, "AIC准则");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(9,i+1,m_tOut.m_vrAIC(i));
	
	pTable3->InsertItem(10, "SC准则");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(10,i+1,m_tOut.m_vrSC(i));
	
	pTable3->InsertItem(11, "DW统计量");
	for(i=0; i<m_nRegre; i++)
		pTable3->SetItemText(11,i+1,m_tOut.m_vrDw(i));
	CRsltElementTable *pETable3 = new CRsltElementTable("单个方程统计量", pTable3);
	m_pResult->Add(pETable3);
}





















































































