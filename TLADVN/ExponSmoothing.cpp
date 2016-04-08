// ExponSmoothing.cpp: implementation of the CExponSmoothing class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ExponSmoothing.h"
#include "TimeSeansonal.h"
#include <vector>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExponSmoothing::CExponSmoothing()
{
	m_nSeason=-1;
	m_nTrend=-1;
	m_nPeriodicity=1;
	m_nPredictNum=0;
	m_FNames.RemoveAll();

	m_dbalpha=0;
	m_nAlpha = 0;
	m_dbdelta=0;
	m_nDelta = 0;
	m_dbgamma=0;
	m_nGamma = 0;
	m_dbphi=0;
	m_nPhi = 0;
	m_bShowPredict= false;
	m_bShowBestModels=false;

}

CExponSmoothing::~CExponSmoothing()
{

}

void CExponSmoothing::InsertSSECols(int nSeason,int nTrend,CTLTable* pSSETable)
{

	int i;
	pSSETable->InsertColumn(0,"编号");
	i=1;

	if (nTrend==NOTREND&&nSeason==NOSEASONAL)
	{
		//alpha,
		pSSETable->InsertColumn(i,"Alpha");
		i++;
			
	}
	if (  nSeason==ADDITIVE&&nTrend==NOTREND)
	{
		//alpha,delta
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		i=i+2;

	}

	if (  nSeason==MULTIPLY&&nTrend==NOTREND)
	{
		//alpha,delta,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Gamma");
		i=i+3;

	}


	if (nTrend==LINEAR&&nSeason==NOSEASONAL)
	{
		//alpha,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Gamma");
		i=i+2;
	}


	if (  nSeason==ADDITIVE&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Gamma");
		i=i+3;
	}
	if (  nSeason==MULTIPLY&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Gamma");
		i=i+3;
	}

	if (  nSeason==NOSEASONAL&&nTrend==EXPONENTIAL)
	{
		//alpha,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Gamma");
		i=i+2;
	}
	if (  nSeason==ADDITIVE&&nTrend==EXPONENTIAL)
	{
		//alpha,delta,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Gamma");
		i=i+3;
		

	}


	if (  nSeason==MULTIPLY&&nTrend==EXPONENTIAL)

	{
		//alpha,delta,gamma
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Gamma");
		i=i+3;
	}

	if (  nSeason==NOSEASONAL&&nTrend==DAMPED)
	{
		//alpha,gamma,phi

		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Gamma");
		pSSETable->InsertColumn(i+2,"Phi");
		i=i+3;
	}
	if (  nSeason==ADDITIVE&&nTrend==DAMPED)
	{
		//alpha,delta,phi
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Phi");
		i=i+3;
	}


	if (  nSeason==MULTIPLY&&nTrend==DAMPED)

	{
		//alpha,delta,phi
		pSSETable->InsertColumn(i,"Alpha");
		pSSETable->InsertColumn(i+1,"Delta");
		pSSETable->InsertColumn(i+2,"Phi");
		i=i+3;

	}
	pSSETable->InsertColumn(i,"SSE值");

}
void CExponSmoothing::SetSSEItem(int nSeason,int nTrend,CTLTable* pSSETable,int nItem,SmoothSetting *pSetting,double dbSSE)
{
	CTString strVal;
	strVal.Format("%d",nItem+1);
	pSSETable->InsertItem(nItem,strVal);
	int j=1;
	if (nTrend==NOTREND&&nSeason==NOSEASONAL)
	{
		//alpha,
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		j++;
			
	}
	if (  nSeason==ADDITIVE&&nTrend==NOTREND)
	{
		//alpha,delta
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		j=j+2;
	}

	if (  nSeason==MULTIPLY&&nTrend==NOTREND)
	{
		//alpha,delta,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbgamma);
		j=j+3;
	}


	if (nTrend==LINEAR&&nSeason==NOSEASONAL)
	{
		//alpha,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbgamma);
		j=j+2;
	}


	if (  nSeason==ADDITIVE&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbgamma);
		j=j+3;
	}
	if (  nSeason==MULTIPLY&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbgamma);
		j=j+3;
	}

	if (  nSeason==NOSEASONAL&&nTrend==EXPONENTIAL)
	{
		//alpha,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbgamma);
		j=j+2;

	}
	if (  nSeason==ADDITIVE&&nTrend==EXPONENTIAL)
	{
		//alpha,delta,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbgamma);
		j=j+3;
		

	}


	if (  nSeason==MULTIPLY&&nTrend==EXPONENTIAL)

	{
		//alpha,delta,gamma
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbgamma);
		j=j+3;
	}

	if (  nSeason==NOSEASONAL&&nTrend==DAMPED)
	{
		//alpha,gamma,phi

		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbgamma);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbphi);
		j=j+3;
	}
	if (  nSeason==ADDITIVE&&nTrend==DAMPED)
	{
		//alpha,delta,phi
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbphi);
		j=j+3;
	}


	if (  nSeason==MULTIPLY&&nTrend==DAMPED)

	{
		//alpha,delta,phi
		pSSETable->SetItemText(nItem,j,pSetting->m_dbalpha);
		pSSETable->SetItemText(nItem,j+1,pSetting->m_dbdelta);
		pSSETable->SetItemText(nItem,j+2,pSetting->m_dbphi);
		j=j+3;

	}
	pSSETable->SetItemText(nItem,j,dbSSE);

}

bool CExponSmoothing::GetValueFromVO(CDataInterface *pDataInterface, string VO)
{
//	EXPSMOOTH
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/SEASONFACT [{NONE**},{ADD},{MULTIP}] [PERIODICITY({4**})]
//		/TREND [{NONE**},{LINEAR},{EXP},{DAMPED}]
//		/PREDICT [PRE({4})]
//		/ALPHA=[{[VALUEALPHA{0.0}]},{[GRIDALPHA(0 1 0.1)]}]
//		/GAMMA=[{[VALUEGAMMA{0.0}]},{[GRIDGAMMA(0 1 0.2)]}]
//		/DELTA=[{[VALUEDELTA{0.0}]},{[GRIDDELTA(0 1 0.2)]}]
//		/PHI=[{[VALUEPHI{0.0}]},{[GRIDPHI(0 0.8 0.2)]}]
//		/OUTPUT [BESTTENOUT]
	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_LstSelVar)))
		return FALSE;

	if (AnaSyntax::findstr("/SEASONFACT","ADD",VOstr)) 
		m_nSeason = 1;
	else if (AnaSyntax::findstr("/SEASONFACT","MULTIP",VOstr))
		m_nSeason = 2;
	else
		m_nSeason = 0;
	if (m_nSeason !=0)
		m_nPeriodicity = AnaSyntax::getvalue("/SEASONFACT","PERIODICITY",VOstr);
	if (m_nPeriodicity == -1)
		m_nPeriodicity = 4;

	if (AnaSyntax::findstr("/TREND","LINEAR",VOstr))
		m_nTrend = 1;
	else if (AnaSyntax::findstr("/TREND","EXP",VOstr))
		m_nTrend = 2;
	else if (AnaSyntax::findstr("/TREND","DAMPED",VOstr))
		m_nTrend = 3;
	else
		m_nTrend = 0;

	m_bShowPredict = AnaSyntax::exist("/PREDICT", VOstr);
	if (m_bShowPredict)
		m_nPredictNum = AnaSyntax::getvalue("/PREDICT","PRE",VOstr);
	m_bShowBestModels = AnaSyntax::findstr("/OUTPUT", "BESTTENOUT", VOstr);
	
	if (AnaSyntax::exist("/ALPHA", VOstr)) 
	{
		if (AnaSyntax::findstr("/ALPHA","VALUEALPHA",VOstr)) 
		{
			m_nAlpha = 0;
			m_dbalpha= AnaSyntax::getvalue("/ALPHA","VALUEALPHA",VOstr);
		}
		else if (AnaSyntax::findstr("/ALPHA","GRIDALPHA",VOstr)) 
		{
			m_nAlpha =1;
			m_alphaconf.m_dbFrom = AnaSyntax::getvalue("/ALPHA","GRIDALPHA",VOstr);
			m_alphaconf.m_dbTo = AnaSyntax::getvalue("/ALPHA","GRIDALPHA",VOstr,2);
			m_alphaconf.m_dbStep = AnaSyntax::getvalue("/ALPHA","GRIDALPHA",VOstr,3);
		}
		else
			return FALSE;
	}

	if (AnaSyntax::exist("/GAMMA", VOstr)) 
	{
		if (AnaSyntax::findstr("/GAMMA","VALUEGAMMA",VOstr)) 
		{
			m_nGamma = 0;
			m_dbgamma = AnaSyntax::getvalue("/GAMMA","VALUEGAMMA",VOstr);
		}
		else if (AnaSyntax::findstr("/GAMMA","GRIDGAMMA",VOstr)) 
		{
			m_nGamma =1;
			m_gammaconf.m_dbFrom = AnaSyntax::getvalue("/GAMMA","GRIDGAMMA",VOstr);
			m_gammaconf.m_dbTo = AnaSyntax::getvalue("/GAMMA","GRIDGAMMA",VOstr,2);
			m_gammaconf.m_dbStep = AnaSyntax::getvalue("/GAMMA","GRIDGAMMA",VOstr,3);
		}
		else
			return FALSE;
	}

	if (AnaSyntax::exist("/DELTA", VOstr)) 
	{
		if (AnaSyntax::findstr("/DELTA","VALUEDELTA",VOstr)) 
		{
			m_nDelta = 0;
			m_dbdelta = AnaSyntax::getvalue("/DELTA","VALUEDELTA",VOstr);
		}
		else if (AnaSyntax::findstr("/DELTA","GRIDDELTA",VOstr)) 
		{
			m_nDelta = 1;
			m_deltaconf.m_dbFrom = AnaSyntax::getvalue("/DELTA","GRIDDELTA",VOstr);
			m_deltaconf.m_dbTo = AnaSyntax::getvalue("/DELTA","GRIDDELTA",VOstr,2);
			m_deltaconf.m_dbStep = AnaSyntax::getvalue("/DELTA","GRIDDELTA",VOstr,3);
		}
		else
			return FALSE;
	}

	if (AnaSyntax::exist("/PHI", VOstr)) 
	{
		if (AnaSyntax::findstr("/PHI","VALUEPHI",VOstr)) 
		{
			m_nPhi = 0;
			m_dbphi = AnaSyntax::getvalue("/PHI","VALUEPHI",VOstr);
		}
		else if (AnaSyntax::findstr("/PHI","GRIDPHI",VOstr)) 
		{
			m_nPhi =1;
			m_phiconf.m_dbFrom = AnaSyntax::getvalue("/PHI","GRIDPHI",VOstr);
			m_phiconf.m_dbTo = AnaSyntax::getvalue("/PHI","GRIDPHI",VOstr,2);
			m_phiconf.m_dbStep = AnaSyntax::getvalue("/PHI","GRIDPHI",VOstr,3);
		}
		else
			return FALSE;
	}
	return TRUE;
}	

CResult* CExponSmoothing::OnExponSmoothing(CDataInterface *pDataInterface, const string VO)
{
	
	CResult *pResult=NULL;
	m_pDataInterface = pDataInterface;
	pResult = new CResult("时间序列-指数平滑模型分析");
	if (!(GetValueFromVO(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	strDBName=pDataInterface->m_DataAccess.GetFileName();
	m_FNames.RemoveAll();
	int i=0;
	for(i=0;i<m_LstSelVar.iCount;i++)
	{
		CTString sTemp =pDataInterface->GetFieldName(m_LstSelVar.GetIDAt(i));
		m_FNames.Add(sTemp);
	}

 	int nFNames=m_FNames.GetSize();
	if (nFNames<=0)
		return NULL;

	int nRecCount=pDataInterface->GetRowCount();
	if ( nRecCount<=0)
	{
		CRsltElementText *pText=new CRsltElementText("警告");
		CTString strInfo;
		strInfo=_T("无有效记录,无法计算");
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;

	}
	for (i=0;i<m_FNames.GetSize();i++)
	{

		CDataAccess* pAccess=&m_pDataInterface->m_DataAccess;
		CField* pField=pAccess->m_pFieldList->FieldByName(m_FNames.GetAt(i));
		double *pdbVal=new double [nRecCount];
		int nCount=0;
		pAccess->First();
		while (!pAccess->Eof())
		{
			if (!pField->IsNull()){
				pdbVal[nCount]=pField->GetAsDouble();
				nCount++;
			}
			pAccess->Next();
		}
		if (nCount<=0)
		{

			CTString strInfo;
			strInfo=CTString("警告--")+m_FNames.GetAt(i);
			CRsltElementText *pText=new CRsltElementText(strInfo);
			strInfo=_T("无有效记录,无法计算");
			pText->AddString(strInfo);
			pResult->Add(pText);
			delete []pdbVal;
			continue;
		}

		CDoubleVector Vec(nCount);
		int j=0;
		for(j=0;j<nCount;j++)	
		{
				Vec[j]=pdbVal[j];
		}
		GridExponSmooth(Vec,m_FNames.GetAt(i),m_nSeason,m_nTrend,pResult);
		//ExponSmooth(Vec,m_FNames.GetAt(i),m_nSeason,m_nTrend,m_dbalpha,m_dbdelta,m_dbgamma,m_dbphi,pResult,&dbSSE);
	}
	return pResult;
}

double CExponSmoothing::Mean(CDoubleVector Vec)
{
	int nsize=Vec.vlen();
	double sum=0;
	int i=0;
	for (i=0;i<nsize;i++)
	{
		double s=Vec[i];
		sum=sum+Vec[i]/nsize;
	}
	return sum;

}
int CExponSmoothing::Mean(CDoubleVector Vec,int nPeriodicity,int nPNO,double* pdbSum)
{
	if ( nPeriodicity<=0)
		return -1;
	int nsize=Vec.vlen();
	int nPNum=nsize/nPeriodicity;
	if (nPNum<=0||nPNum<nPNO+1)
		return -1;

	double sum=0;
	for (int i=nPeriodicity*nPNO;i<nPeriodicity*nPNO+nPeriodicity;i++)
	{
		//double s=Vec[i];
		sum=sum+Vec[i]/nPeriodicity;
	}
	*pdbSum=sum;
	return 1;

}
double CExponSmoothing::GetSSE(CDoubleVector ErrVec)
{
	int nsize=ErrVec.vlen();
	double sum=0;
	for (int i=0;i<nsize;i++)
		sum=sum+ErrVec[i]*ErrVec[i];
	return sum;

}

bool CExponSmoothing::ExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,double dbalpha,double dbdelta,double dbgamma,double dbphi,CResult* pResult,double *pSSE)
{
	
	//
	//NOSEASONAL    0 //无季节变量
	//ADDITIVE	1	//加法
	//MULTIPLY	2	//乘法

	//NOTREND		0	//无趋势
	//LINEAR		1	//线性
	//EXPONENTIAL 2	//指数
	//DAMPED		3	//衰减

	int i,j,t,nLen=XVec.vlen(); 
	CDoubleVector	S(nLen,0),XFit(nLen,0),Err(nLen,0),T(nLen,0);
	CTLTable *pTable=NULL;
	CRsltElementTable *pE=NULL;
	CTString strTitle;
	if (  nSeason==NOSEASONAL&&nTrend==NOTREND)
	{
		//xfit0=s0;e1=x1-xfit0;st=st-1+alpha*et;xfitt=st; 
		S[0]=Mean(XVec);
		XFit[0]=S[0];
		Err[0]=XVec[0]-XFit[0];
		for (t=1;t<nLen;t++)
		{
			S[t]=S[t-1]+dbalpha*Err[t-1];
			XFit[t]=S[t];
			Err[t]=XVec[t]-XFit[t];
		}
		*pSSE=GetSSE(Err);//add by liytao 20030417
		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRNOSEASONAL;
		strTitle+=",";
		strTitle+=STRNOTREND;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;残差平方和(SSE)=%.2f",dbalpha,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		t=nLen+1;
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum);
			double dbErr=Err[nLen-1];
			double dbS=S[nLen-1];
			PredFit[0]=dbS+dbalpha*dbErr;
			for (j=1;j<m_nPredictNum;j++)
				PredFit[j]=dbS+dbalpha*dbErr;

			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);
			}
		}
	}

	if (  nSeason==ADDITIVE&&nTrend==NOTREND)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,1);
		bool bInvalid=false;
		S[0]=0;
		int nPNum=nLen/m_nPeriodicity;
		int i=0;
		for (i=0;i<nPNum&&!bInvalid;i++)
		{
			double dbMean;
			if (Mean(XVec,m_nPeriodicity,i,&dbMean)<0)
				bInvalid=true;
			else
				S[0]=S[0]+dbMean/nPNum;
		}
		if ( bInvalid)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STRNOTREND);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]+I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbalpha*Err[t-1];
			XFit[t]=S[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t];


		}

		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbalpha*Err[t-1];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=S[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRADDITIVE;
		strTitle+=",";
		strTitle+=STRNOTREND;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");
		
		CTString strVal;
		for (t=0;t<nPNum*m_nPeriodicity;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		t=nLen+1;
		if( m_bShowPredict&&m_nPredictNum>0)
		{

			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS+PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS;//+dbalpha*PredErr[t-1];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS+PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;

			}
			if( m_nPredictNum>m_nPeriodicity)
			{


				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS;//+dbalpha*PredErr[t-1];
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS+PredI[t];
					//PredErr[t]=PredErr[t-m_nPeriodicity];
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
				}

			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);			
			}
		}		
	}

	if (  nSeason==MULTIPLY&&nTrend==NOTREND)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,0);
		bool bInvalid=false;
		S[0]=0;
		int nPNum=nLen/m_nPeriodicity;
		int i=0;
		for (i=0;i<nPNum&&!bInvalid;i++)
		{
			double dbMean;
			if (Mean(XVec,m_nPeriodicity,i,&dbMean)<0)
				bInvalid=true;
			else
				S[0]=S[0]+dbMean/nPNum;
		}
		if ( bInvalid)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STRNOTREND);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]*I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0]/S[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbalpha*Err[t-1]/I[t];
			XFit[t]=S[t]*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbalpha*Err[t-1]/I[t-m_nPeriodicity];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=S[t]*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName);
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRMULTIPLY;
		strTitle+=",";
		strTitle+=STRNOTREND;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nPNum*m_nPeriodicity;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		t=nLen+1;
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS*PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS;//+dbalpha*PredErr[t-1];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS*PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;

			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS;//+dbalpha*PredErr[t-1];
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS*PredI[t];
					//PredErr[t]=PredErr[t-m_nPeriodicity];
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);
			}
		}
	}
	if (  nSeason==NOSEASONAL&&nTrend==LINEAR)
	{
		T[0]=(XVec[nLen-1]-XVec[0])/(nLen-1);	
		S[0]=XVec[0]-T[0]/2;

		XFit[0]=S[0]+T[0];
		Err[0]=XVec[0]-XFit[0];
		for (t=1;t<nLen;t++)
		{
			S[t]=S[t-1]+T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1];	
		

			XFit[t]=S[t]+T[t];
			Err[t]=XVec[t]-XFit[t];
		}
		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRNOSEASONAL;
		strTitle+=",";
		strTitle+=STRLINEAR;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Gamma=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbgamma,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");
		
		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);			
		}
		t=nLen+1;

		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum);
			double dbSPrev,dbTPrev,dbSCur,dbTCur,dbErr;
			dbErr=Err[nLen-1];
			dbSPrev=S[nLen-1];
			dbTPrev=T[nLen-1];
			dbSCur=dbSPrev+dbTPrev+dbalpha*dbErr;
			dbTCur=dbTPrev+dbalpha*dbgamma*dbErr;	
			PredFit[0]=dbSCur+dbTCur;

			dbSPrev=dbSCur;
			dbTPrev=dbTCur;
			for (t=1;t<m_nPredictNum;t++)
			{
				dbSCur=dbSPrev+dbTPrev;
				dbTCur=dbTPrev;	
				PredFit[t]=dbSCur+dbTCur;
				dbSPrev=dbSCur;
				dbTPrev=dbTCur;
			}

			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);			
			}
		}
	}
	if (  nSeason==ADDITIVE&&nTrend==LINEAR)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,1);
		bool bInvalid=false;
		S[0]=0;
		int nPNum=nLen/m_nPeriodicity;
		
		double dbMean1,dbMeank;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STRLINEAR);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		if (Mean(XVec,m_nPeriodicity,nPNum-1,&dbMeank)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STRLINEAR);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}
		T[0]=(dbMeank-dbMean1)/((nPNum-1)*m_nPeriodicity);
		S[0]=dbMean1-T[0]*m_nPeriodicity/2;

		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]+T[0]+I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1];
			XFit[t]=S[t]+T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t];
		}
		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=S[t]+T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRADDITIVE;
		strTitle+=",";
		strTitle+=STRLINEAR;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;Gamma=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,dbgamma,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nPNum*m_nPeriodicity;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);			
		}
		t=nLen+1;
		if( m_bShowPredict&&m_nPredictNum>0)
		{

			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+T[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1];
			double dbT=T[nPNum*m_nPeriodicity-1]+dbalpha*dbgamma*Err[nPNum*m_nPeriodicity-1];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS+dbT+PredI[0];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS+dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS+dbT+PredI[t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;
			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS+dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS+dbT+PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);
			}
		}
	}
	if (  nSeason==MULTIPLY&&nTrend==LINEAR)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,0);
		bool bInvalid=false;
		S[0]=0;
		int nPNum=nLen/m_nPeriodicity;
		double dbMean1,dbMeank;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STRLINEAR);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}
		if (Mean(XVec,m_nPeriodicity,nPNum-1,&dbMeank)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STRLINEAR);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		T[0]=(dbMeank-dbMean1)/((nPNum-1)*m_nPeriodicity);
		S[0]=dbMean1-T[0]*m_nPeriodicity/2;

		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=(S[0]+T[0])*I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0]/S[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+T[t-1]+dbalpha*Err[t-1]/I[t];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/I[t];
			XFit[t]=(S[t]+T[t])*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+T[t-1]+dbalpha*Err[t-1]/I[t-m_nPeriodicity];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/I[t-m_nPeriodicity];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=(S[t]+T[t])*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRMULTIPLY;
		strTitle+=",";
		strTitle+=STRLINEAR;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;Gamma=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,dbgamma,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nPNum*m_nPeriodicity;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);			
		}
		t=nLen+1;
		if( m_bShowPredict&&m_nPredictNum>0)
		{

			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+T[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			double dbT=T[nPNum*m_nPeriodicity-1]+dbalpha*dbgamma*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=(dbS+dbT)*PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS+dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=(dbS+dbT)*PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;
			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS+dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=(dbS+dbT)*PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);
			}
		}
	}
	if (  nSeason==NOSEASONAL&&nTrend==EXPONENTIAL)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,1);
		bool bInvalid=false;
		for (t=0;t<nLen&&!bInvalid;t++)
		{
			if ( XVec[t]<=0)
				bInvalid=true;
		}
		if (bInvalid)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)要求数据为正数\n",STRNOSEASONAL,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}

		T[0]=XVec[1]/XVec[0];
		S[0]=XVec[0]/sqrt(T[0]);
		XFit[0]=S[0]*T[0];
		Err[0]=XVec[0]-XFit[0];
		for (t=1;t<nLen;t++)
		{
			S[t]=S[t-1]*T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/S[t-1];	
			XFit[t]=S[t]*T[t];
			Err[t]=XVec[t]-XFit[t];
		}
		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRNOSEASONAL;
		strTitle+=",";
		strTitle+=STREXPONENTIAL;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Gamma=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbgamma,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");
		
		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);			
		}
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum);
			double dbSPrev,dbTPrev,dbSCur,dbTCur,dbErr;
			dbErr=Err[nLen-1];
			dbSPrev=S[nLen-1];
			dbTPrev=T[nLen-1];
			dbSCur=dbSPrev*dbTPrev+dbalpha*dbErr;
			dbTCur=dbTPrev+dbalpha*dbgamma*dbErr/dbSPrev;	
			PredFit[0]=dbSCur*dbTCur;

			dbSPrev=dbSCur;
			dbTPrev=dbTCur;
			for (t=1;t<m_nPredictNum;t++)
			{
				dbSCur=dbSPrev*dbTPrev;
				dbTCur=dbTPrev;	
				PredFit[t]=dbSCur*dbTCur;
				dbSPrev=dbSCur;
				dbTPrev=dbTCur;
			}

			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);
			}
		}
	}
	if (  nSeason==ADDITIVE&&nTrend==EXPONENTIAL)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,1);
		bool bInvalid=false;
		for (t=0;t<nLen&&!bInvalid;t++)
		{
			if ( XVec[t]<=0)
				bInvalid=true;
		}
		if (bInvalid)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)要求数据为正数\n",STRADDITIVE,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}
		int nPNum=nLen/m_nPeriodicity;
		double dbMean1,dbMean2;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}
		if (Mean(XVec,m_nPeriodicity,1,&dbMean2)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		T[0]=exp((log(dbMean2)-log(dbMean1))/m_nPeriodicity);
		S[0]=exp(log(dbMean1)-log(T[0])*m_nPeriodicity/2);
		
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]*T[0]+I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]*T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/S[t-1];
			XFit[t]=S[t]*T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t];
		}

		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]*T[t-1]+dbalpha*Err[t-1];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/S[t-1];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=S[t]*T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRADDITIVE;
		strTitle+=",";
		strTitle+=STREXPONENTIAL;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Gamma=%.2f;Delta=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbgamma,dbdelta,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");
		
		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}

		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]*T[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1];
			double dbT=T[nPNum*m_nPeriodicity-1]+dbalpha*dbgamma*Err[nPNum*m_nPeriodicity-1]/S[nPNum*m_nPeriodicity-1];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS*dbT+PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS*dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS*dbT+PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;

			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS*dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS*dbT+PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);				
			}
		}
	}
	if (  nSeason==MULTIPLY&&nTrend==EXPONENTIAL)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,0);
		bool bInvalid=false;
		for (t=0;t<nLen&&!bInvalid;t++)
		{
			if ( XVec[t]<=0)
				bInvalid=true;
		}
		if (bInvalid)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)要求数据为正数\n",STRMULTIPLY,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}

		int nPNum=nLen/m_nPeriodicity;
		double dbMean1,dbMean2;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		if (Mean(XVec,m_nPeriodicity,1,&dbMean2)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STREXPONENTIAL);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		T[0]=exp((log(dbMean2)-log(dbMean1))/m_nPeriodicity);
		S[0]=exp(log(dbMean1)-log(T[0])*m_nPeriodicity/2);
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]*T[0]*I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0]/S[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]*T[t-1]+dbalpha*Err[t-1]/I[t];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/(I[t]*S[t-1]);
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t]/S[t];
			XFit[t]=S[t]*T[t]*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]*T[t-1]+dbalpha*Err[t-1]/I[t-m_nPeriodicity];
			T[t]=T[t-1]+dbalpha*dbgamma*Err[t-1]/(I[t-m_nPeriodicity]*S[t-1]);
			//I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t]/S[t];
			XFit[t]=S[t]*T[t]*I[t-m_nPeriodicity];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha)*Err[t]/S[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRMULTIPLY;
		strTitle+=",";
		strTitle+=STREXPONENTIAL;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Gamma=%.2f;Delta=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbgamma,dbdelta,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");		
		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]*T[nPNum*m_nPeriodicity-1]+dbalpha*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			double dbT=T[nPNum*m_nPeriodicity-1]+dbalpha*dbgamma*Err[nPNum*m_nPeriodicity-1]/(I[(nPNum-1)*m_nPeriodicity]*S[nPNum*m_nPeriodicity-1]);
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS*dbT*PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS*dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS*dbT*PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;
			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS*dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS*dbT*PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);
			}
		}
	}
	if (  nSeason==NOSEASONAL&&nTrend==DAMPED)
	{
		T[0]=(XVec[nLen-1]-XVec[0])/((nLen-1)*dbphi);
		S[0]=XVec[0]-T[0]/2;
		XFit[0]=S[0]+dbphi*T[0];
		Err[0]=XVec[0]-XFit[0];
		
		for (t=1;t<nLen;t++)
		{
			S[t]=S[t-1]+dbphi*T[t-1]+dbalpha*Err[t-1];
			T[t]=dbphi*T[t-1]+dbalpha*dbgamma*Err[t-1];	
			XFit[t]=S[t]+dbphi*T[t];
			Err[t]=XVec[t]-XFit[t];
		}
		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRNOSEASONAL;
		strTitle+=",";
		strTitle+=STRDAMPED;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Gamma=%.2f;Phi=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbgamma,dbphi,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum);
			double dbSPrev,dbTPrev,dbSCur,dbTCur,dbErr;
			dbErr=Err[nLen-1];
			dbSPrev=S[nLen-1];
			dbTPrev=T[nLen-1];
			dbSCur=dbSPrev+dbphi*dbTPrev+dbalpha*dbErr;
			dbTCur=dbphi*dbTPrev+dbalpha*dbgamma*dbErr;	
			PredFit[0]=dbSCur+dbphi*dbTCur;
			dbSPrev=dbSCur;
			dbTPrev=dbTCur;
			for (t=1;t<m_nPredictNum;t++)
			{
				dbSCur=dbSPrev;
				dbTCur=dbphi*dbTPrev;	
				PredFit[t]=dbSCur+dbphi*dbTCur;
				dbSPrev=dbSCur;
				dbTPrev=dbTCur;
			}

			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j);
				pTable->InsertItem(nLen+j,strVal);
				pTable->SetItemText(nLen+j,1,PredFit[j]);			
			}
		}
	}
	if (  nSeason==ADDITIVE&&nTrend==DAMPED)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,1);
		bool bInvalid=false;
		int nPNum=nLen/m_nPeriodicity;
		double dbMean1,dbMeank;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STRDAMPED);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		if (Mean(XVec,m_nPeriodicity,nPNum-1,&dbMeank)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRADDITIVE,STRDAMPED);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		T[0]=(dbMeank-dbMean1)/((nPNum-1)*m_nPeriodicity*dbphi);
		S[0]=dbMean1-T[0]*m_nPeriodicity/2;
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=S[0]+dbphi*T[0]+I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbphi*T[t-1]+dbalpha*(2-dbalpha)*Err[t-1];
			T[t]=dbphi*T[t-1]+dbalpha*(dbalpha-dbphi+1)*Err[t-1];
			XFit[t]=S[t]+dbphi*T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha)*Err[t];
		}
		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbphi*T[t-1]+dbalpha*(2-dbalpha)*Err[t-1];
			T[t]=dbphi*T[t-1]+dbalpha*(dbalpha-dbphi+1)*Err[t-1];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=S[t]+dbphi*T[t]+I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha*(2-dbalpha))*Err[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRADDITIVE;
		strTitle+=",";
		strTitle+=STRDAMPED;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;Phi=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,dbphi,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");

		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+dbphi*T[nPNum*m_nPeriodicity-1]+dbalpha*(2-dbalpha)*Err[nPNum*m_nPeriodicity-1];
			double dbT=dbphi*T[nPNum*m_nPeriodicity-1]+dbalpha*(dbalpha-dbphi+1)*Err[nPNum*m_nPeriodicity-1];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=dbS+dbphi*dbT+PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS+dbphi*dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbphi*dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=dbS+dbphi*dbT+PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;
			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS+dbphi*dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbphi*dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=dbS+dbphi*dbT+PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);				
			}
		}
	}
	if (  nSeason==MULTIPLY&&nTrend==DAMPED)
	{
		CTimeSeansonal  timeseason;
		CDoubleVector I(nLen,0),T(nLen,0);
		CDoubleMatrix matrix_I=timeseason.GetSAF(XVec,m_nPeriodicity,0);
		bool bInvalid=false;
		int nPNum=nLen/m_nPeriodicity;
		double dbMean1,dbMeank;
		if (Mean(XVec,m_nPeriodicity,0,&dbMean1)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STRDAMPED);

			pText->AddString(str);
			pResult->Add(pText);
			return false;
		}
		if (Mean(XVec,m_nPeriodicity,nPNum-1,&dbMeank)<0)
		{
			CRsltElementText *pText=new CRsltElementText("警告"); 
			CTString str;
			str.Format("该模型(%s,%s)周期数设置过大,无法计算\n",STRMULTIPLY,STRDAMPED);

			pText->AddString(str);
			pResult->Add(pText);
			return false;

		}
		T[0]=(dbMeank-dbMean1)/((nPNum-1)*m_nPeriodicity*dbphi);
		S[0]=dbMean1-T[0]*m_nPeriodicity/2;
		for (i=0;i<m_nPeriodicity;i++)
			I[i]=matrix_I(0,i);

		XFit[0]=(S[0]+dbphi*T[0])*I[0];
		Err[0]=XVec[0]-XFit[0];
		I[0]=I[0]+dbdelta*(1-dbalpha)*Err[0]/S[0];
		for (t=1;t<m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbphi*T[t-1]+dbalpha*(2-dbalpha)*Err[t-1]/I[t];
			T[t]=dbphi*T[t-1]+dbalpha*(dbalpha-dbphi+1)*Err[t-1]/I[t];
			XFit[t]=(S[t]+dbphi*T[t])*I[t];
			Err[t]=XVec[t]-XFit[t];
			I[t]=I[t]+dbdelta*(1-dbalpha*(2-dbalpha))*Err[t]/S[t];
		}
		for (t=m_nPeriodicity;t<nPNum*m_nPeriodicity;t++)
		{
			S[t]=S[t-1]+dbphi*T[t-1]+dbalpha*(2-dbalpha)*Err[t-1]/I[t-m_nPeriodicity];
			T[t]=dbphi*T[t-1]+dbalpha*(dbalpha-dbphi+1)*Err[t-1]/I[t-m_nPeriodicity];
			I[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
			XFit[t]=(S[t]+dbphi*T[t])*I[t];
			Err[t]=XVec[t]-XFit[t];
			if (t%m_nPeriodicity==1)
				I[t]=I[t-m_nPeriodicity];
			else
				I[t]=I[t-m_nPeriodicity]+dbdelta*(1-dbalpha*(2-dbalpha))*Err[t]/S[t];
		}

		*pSSE=GetSSE(Err);//add by liytao 20030417

		pTable=new CTLTable;

		strTitle.Format("指数平滑模型---%s",strFName.GetData());
		pE=new CRsltElementTable(strTitle,pTable);
		pResult->Add(pE);

		CTString strTmp;
		strTitle.Empty();
		strTitle+="在模型为";
		strTitle+=STRMULTIPLY;
		strTitle+=",";
		strTitle+=STRDAMPED;
		strTitle+="时\n";
		strTmp.Format("Alpha=%.2f;Delta=%.2f;Phi=%.2f;残差平方和(SSE)=%.2f",dbalpha,dbdelta,dbphi,GetSSE(Err));
		strTitle+=strTmp;
		pTable->SetTitle(strTitle);

		pTable->InsertColumn(0,"样本号");
		pTable->InsertColumn(1,"拟合值");
		pTable->InsertColumn(2,"残差值");
		
		CTString strVal;
		for (t=0;t<nLen;t++)
		{
			strVal.Format("%d",t+1);
			pTable->InsertItem(t,strVal);
			pTable->SetItemText(t,1,XFit[t]);
			pTable->SetItemText(t,2,Err[t]);
		}
		if( m_bShowPredict&&m_nPredictNum>0)
		{
			CDoubleVector PredFit(m_nPredictNum),PredI(m_nPredictNum),PredT(m_nPredictNum);//,PredErr(m_nPredictNum);
			double dbS=S[nPNum*m_nPeriodicity-1]+dbphi*T[nPNum*m_nPeriodicity-1]+dbalpha*(2-dbalpha)*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			double dbT=dbphi*T[nPNum*m_nPeriodicity-1]+dbalpha*(dbalpha-dbphi+1)*Err[nPNum*m_nPeriodicity-1]/I[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			PredFit[0]=(dbS+dbphi*dbT)*PredI[0];
			//PredErr[0]=Err[(nPNum-1)*m_nPeriodicity];
			PredI[0]=I[(nPNum-1)*m_nPeriodicity];
			double dbPrevS=dbS,dbPrevT=dbT;
			for (t=1;t<m_nPeriodicity&&t<m_nPredictNum;t++)
			{
				dbS=dbPrevS+dbphi*dbPrevT;//+dbalpha*PredErr[t-1];
				dbT=dbphi*dbPrevT;
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
				PredFit[t]=(dbS+dbphi*dbT)*PredI[t];
				//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
				PredI[t]=I[(nPNum-1)*m_nPeriodicity+t];//+dbdelta*(1-dbalpha)*PredErr[t];
				dbPrevS=dbS;
				dbPrevT=dbT;
			}
			if( m_nPredictNum>m_nPeriodicity)
			{
				for (t=m_nPeriodicity;t<m_nPredictNum;t++)
				{
					dbS=dbPrevS+dbphi*dbPrevT;//+dbalpha*PredErr[t-1];
					dbT=dbphi*dbPrevT;
					PredI[t]=PredI[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*Err[t]; modified by liytao 20030414
					PredFit[t]=(dbS+dbphi*dbT)*PredI[t];
					//PredErr[t]=Err[(nPNum-1)*m_nPeriodicity+t];
					PredI[t]=I[t-m_nPeriodicity];//+dbdelta*(1-dbalpha)*PredErr[t];
					dbPrevS=dbS;
					dbPrevT=dbT;
				}
			}
			for (j=0;j<m_nPredictNum;j++)
			{
				strVal.Format("预测样本%d",j+1);
				pTable->InsertItem(m_nPeriodicity*nPNum+j,strVal);
				pTable->SetItemText(m_nPeriodicity*nPNum+j,1,PredFit[j]);
			}
		}
	}
	return true;
}

bool CExponSmoothing::GridExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,CResult* pResult)
{
	
	//
	//NOSEASONAL    0 //无季节变量
	//ADDITIVE	1	//加法
	//MULTIPLY	2	//乘法

	//NOTREND		0	//无趋势
	//LINEAR		1	//线性
	//EXPONENTIAL 2	//指数
	//DAMPED		3	//衰减
	SmoothConf *palpha=NULL,*pdelta=NULL,*pgamma=NULL,*pphi=NULL;
	if (nTrend==NOTREND&&nSeason==NOSEASONAL)
	{
		//alpha,
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
			
	}
	if (  nSeason==ADDITIVE&&nTrend==NOTREND)
	{
		//alpha,delta
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;

	}

	if (  nSeason==MULTIPLY&&nTrend==NOTREND)
	{
		//alpha,delta,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;

	}


	if (nTrend==LINEAR&&nSeason==NOSEASONAL)
	{
		//alpha,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
	}


	if (  nSeason==ADDITIVE&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
	}
	if (  nSeason==MULTIPLY&&nTrend==LINEAR)
	{
		//alpha,delta,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
	}

	if (  nSeason==NOSEASONAL&&nTrend==EXPONENTIAL)
	{
		//alpha,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;

	}
	if (  nSeason==ADDITIVE&&nTrend==EXPONENTIAL)
	{
		//alpha,delta,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
		

	}


	if (  nSeason==MULTIPLY&&nTrend==EXPONENTIAL)

	{
		//alpha,delta,gamma
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
	}

	if (  nSeason==NOSEASONAL&&nTrend==DAMPED)
	{
		//alpha,gamma,phi

		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nGamma>0)
			pgamma=&m_gammaconf;
		if (m_nPhi>0)
			pphi=&m_phiconf;
	}
	if (  nSeason==ADDITIVE&&nTrend==DAMPED)
	{
		//alpha,delta,phi
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nPhi>0)
			pphi=&m_phiconf;
	}


	if (  nSeason==MULTIPLY&&nTrend==DAMPED)

	{
		//alpha,delta,phi
		if (m_nAlpha>0)
			palpha=&m_alphaconf;
		if (m_nDelta>0)
			pdelta=&m_deltaconf;
		if (m_nPhi>0)
			pphi=&m_phiconf;

	}

	GridSearch(XVec,strFName,nSeason,nTrend,palpha,pdelta,pgamma,pphi,pResult);


	return true;

}


void CExponSmoothing::GridSearch(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,SmoothConf * palphaconf,SmoothConf * pdeltaconf,SmoothConf * pgammaconf,SmoothConf * pphiconf,CResult*& pResult)
{
	std::vector<double> SSEs; 
	bool balphagrid=false,bdeltagrid=false,bgammagrid=false,bphigrid=false;
	if  (palphaconf!=NULL)
		balphagrid=true;		
	if  (pdeltaconf!=NULL)
		bdeltagrid=true;		
	if  (pgammaconf!=NULL)
		bgammagrid=true;		
	if  (pphiconf!=NULL)
		bphigrid=true;		
	double tempalpha,tempdelta,tempgamma,tempphi;
	CResult* pTempResult=NULL;
	SmoothSetting *pSetting=NULL;
	CPtrArray Results,Settings;
	if ( balphagrid)
	{
		tempalpha=palphaconf->m_dbFrom;
		
		while (tempalpha<=palphaconf->m_dbTo)
		{
			if (bdeltagrid)
			{
				tempdelta=pdeltaconf->m_dbFrom;
				while (tempdelta<=pdeltaconf->m_dbTo)
				{
					if( bgammagrid)
					{
						tempgamma=pgammaconf->m_dbFrom;
						while (tempgamma<=pgammaconf->m_dbTo)
						{
							if( bphigrid)
							{
								tempphi=pphiconf->m_dbFrom;
								while( tempphi<=pphiconf->m_dbTo)
								{
									double dbSSE;
									if( tempphi==0||tempphi==1)
									{
										pTempResult=new CResult("警告");
										CRsltElementText* pText=new CRsltElementText("警告");
										pText->AddString("Phi值不能为0或1");
										pTempResult->Add(pText);
										Results.Add(pTempResult);
										Settings.Add(NULL);
										SSEs.push_back(0);
									}
									else
									{
										pTempResult=new CResult;
										ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,tempdelta,tempgamma,tempphi,pTempResult,&dbSSE);
										Results.Add(pTempResult);
										pSetting=new SmoothSetting(tempalpha,tempdelta,tempgamma,tempphi);

										Settings.Add(pSetting);
										SSEs.push_back(dbSSE);
									}
									tempphi=tempphi+pphiconf->m_dbStep;

								}

							}
							else
							{
								double dbSSE;
								pTempResult=new CResult;
								ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,tempdelta,tempgamma,m_dbphi,pTempResult,&dbSSE);
								Results.Add(pTempResult);
								pSetting=new SmoothSetting(tempalpha,tempdelta,tempgamma,m_dbphi);
								Settings.Add(pSetting);
								SSEs.push_back(dbSSE);


							}


							tempgamma=tempgamma+pgammaconf->m_dbStep;

						}

					}
					else
					{

						if( bphigrid)
						{
							tempphi=pphiconf->m_dbFrom;
							while( tempphi<=pphiconf->m_dbTo)
							{
								double dbSSE;
								if( tempphi==0||tempphi==1)
								{
									pTempResult=new CResult("警告");
									CRsltElementText* pText=new CRsltElementText("警告");
									pText->AddString("Phi值不能为0或1");
									pTempResult->Add(pText);
									Results.Add(pTempResult);
									Settings.Add(NULL);
									SSEs.push_back(0);
								}
								else
								{
									pTempResult=new CResult;
									ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,tempdelta,m_dbgamma,tempphi,pTempResult,&dbSSE);
									Results.Add(pTempResult);
									pSetting=new SmoothSetting(tempalpha,tempdelta,m_dbgamma,tempphi);
									Settings.Add(pSetting);
									SSEs.push_back(dbSSE);
								}						
								tempphi=tempphi+pphiconf->m_dbStep;

							}

						}
						else
						{
							double dbSSE;
							pTempResult=new CResult;
							ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,tempdelta,m_dbgamma,m_dbphi,pTempResult,&dbSSE);
							pSetting=new SmoothSetting(tempalpha,tempdelta,m_dbgamma,m_dbphi);
							Settings.Add(pSetting);
							Results.Add(pTempResult);
							SSEs.push_back(dbSSE);


						}



					}
					tempdelta=tempdelta+pdeltaconf->m_dbStep;


				}

			}
			else
			{
				if( bgammagrid)
				{
					tempgamma=pgammaconf->m_dbFrom;
					while (tempgamma<=pgammaconf->m_dbTo)
					{
						if( bphigrid)
						{
							tempphi=pphiconf->m_dbFrom;
							while( tempphi<=pphiconf->m_dbTo)
							{
								double dbSSE;
								if( tempphi==0||tempphi==1)
								{
									pTempResult=new CResult("警告");
									CRsltElementText* pText=new CRsltElementText("警告");
									pText->AddString("Phi值不能为0或1");
									pTempResult->Add(pText);
									Results.Add(pTempResult);
									Settings.Add(NULL);
									SSEs.push_back(0);
								}
								else
								{
									pTempResult=new CResult;
									ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,m_dbdelta,tempgamma,tempphi,pTempResult,&dbSSE);
									pSetting=new SmoothSetting(tempalpha,m_dbdelta,tempgamma,tempphi);
									Settings.Add(pSetting);
									Results.Add(pTempResult);
									SSEs.push_back(dbSSE);
								}
								tempphi=tempphi+pphiconf->m_dbStep;
								
							}

						}
						else
						{
							double dbSSE;
							pTempResult=new CResult;
							ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,m_dbdelta,tempgamma,m_dbphi,pTempResult,&dbSSE);
							pSetting=new SmoothSetting(tempalpha,m_dbdelta,tempgamma,m_dbphi);
							Settings.Add(pSetting);
							Results.Add(pTempResult);
							SSEs.push_back(dbSSE);


						}


						tempgamma=tempgamma+pgammaconf->m_dbStep;

					}

				}
				else
				{

					if( bphigrid)
					{
						tempphi=pphiconf->m_dbFrom;
						while( tempphi<=pphiconf->m_dbTo)
						{
							double dbSSE;
							if( tempphi==0||tempphi==1)
							{
								pTempResult=new CResult("警告");
								CRsltElementText* pText=new CRsltElementText("警告");
								pText->AddString("Phi值不能为0或1");
								pTempResult->Add(pText);
								Results.Add(pTempResult);
								Settings.Add(NULL);
								SSEs.push_back(0);
							}
							else
							{
								pTempResult=new CResult;
								ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,m_dbdelta,m_dbgamma,tempphi,pTempResult,&dbSSE);
								pSetting=new SmoothSetting(tempalpha,m_dbdelta,m_dbgamma,tempphi);
								Settings.Add(pSetting);
								Results.Add(pTempResult);
								SSEs.push_back(dbSSE);
							}					
							tempphi=tempphi+pphiconf->m_dbStep;

						}

					}
					else
					{
						double dbSSE;
						pTempResult=new CResult;
						ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,tempalpha,m_dbdelta,m_dbgamma,m_dbphi,pTempResult,&dbSSE);
						pSetting=new SmoothSetting(tempalpha,m_dbdelta,m_dbgamma,m_dbphi);
						Settings.Add(pSetting);
						Results.Add(pTempResult);
						SSEs.push_back(dbSSE);


					}



				}






			}

			tempalpha=tempalpha+palphaconf->m_dbStep;

		}

	}
	else
	{

		if (bdeltagrid)
		{
			tempdelta=pdeltaconf->m_dbFrom;
			while (tempdelta<=pdeltaconf->m_dbTo)
			{
				if( bgammagrid)
				{
					tempgamma=pgammaconf->m_dbFrom;
					while (tempgamma<=pgammaconf->m_dbTo)
					{
						if( bphigrid)
						{
							tempphi=pphiconf->m_dbFrom;
							while( tempphi<=pphiconf->m_dbTo)
							{
								double dbSSE;
								if( tempphi==0||tempphi==1)
								{
									pTempResult=new CResult("警告");
									CRsltElementText* pText=new CRsltElementText("警告");
									pText->AddString("Phi值不能为0或1");
									pTempResult->Add(pText);
									Results.Add(pTempResult);
									Settings.Add(NULL);
									SSEs.push_back(0);
								}
								else
								{
									pTempResult=new CResult;
									ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,tempdelta,tempgamma,tempphi,pTempResult,&dbSSE);
									pSetting=new SmoothSetting(m_dbalpha,tempdelta,tempgamma,tempphi);
									Settings.Add(pSetting);
									Results.Add(pTempResult);
									SSEs.push_back(dbSSE);
								}						
								tempphi=tempphi+pphiconf->m_dbStep;

							}

						}
						else
						{
							double dbSSE;
							pTempResult=new CResult;
							ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,tempdelta,tempgamma,m_dbphi,pTempResult,&dbSSE);
							pSetting=new SmoothSetting(m_dbalpha,tempdelta,tempgamma,m_dbphi);
							Settings.Add(pSetting);
							Results.Add(pTempResult);
							SSEs.push_back(dbSSE);

						}


						tempgamma=tempgamma+pgammaconf->m_dbStep;

					}

				}
				else
				{

					if( bphigrid)
					{
						tempphi=pphiconf->m_dbFrom;
						while( tempphi<=pphiconf->m_dbTo)
						{
							double dbSSE;
							if( tempphi==0||tempphi==1)
							{
								pTempResult=new CResult("警告");
								CRsltElementText* pText=new CRsltElementText("警告");
								pText->AddString("Phi值不能为0或1");
								pTempResult->Add(pText);
								Results.Add(pTempResult);
								Settings.Add(NULL);
								SSEs.push_back(0);
							}
							else
							{
								pTempResult=new CResult;
								ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,tempdelta,m_dbgamma,tempphi,pTempResult,&dbSSE);
								pSetting=new SmoothSetting(m_dbalpha,tempdelta,m_dbgamma,tempphi);
								Settings.Add(pSetting);
								Results.Add(pTempResult);
								SSEs.push_back(dbSSE);
							}
							tempphi=tempphi+pphiconf->m_dbStep;

						}

					}
					else
					{
						double dbSSE;
						pTempResult=new CResult;
						ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,tempdelta,m_dbgamma,m_dbphi,pTempResult,&dbSSE);
						pSetting=new SmoothSetting(m_dbalpha,tempdelta,m_dbgamma,m_dbphi);
						Settings.Add(pSetting);
						Results.Add(pTempResult);
						SSEs.push_back(dbSSE);


					}



				}
				tempdelta=tempdelta+pdeltaconf->m_dbStep;


			}

		}
		else
		{
			if( bgammagrid)
			{
				tempgamma=pgammaconf->m_dbFrom;
				while (tempgamma<=pgammaconf->m_dbTo)
				{
					if( bphigrid)
					{
						tempphi=pphiconf->m_dbFrom;
						while( tempphi<=pphiconf->m_dbTo)
						{
							double dbSSE;
							if( tempphi==0||tempphi==1)
							{
								pTempResult=new CResult("警告");
								CRsltElementText* pText=new CRsltElementText("警告");
								pText->AddString("Phi值不能为0或1");
								pTempResult->Add(pText);
								Results.Add(pTempResult);
								Settings.Add(NULL);
								SSEs.push_back(0);
							}
							else
							{
								pTempResult=new CResult;
								ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,m_dbdelta,tempgamma,tempphi,pTempResult,&dbSSE);
								pSetting=new SmoothSetting(m_dbalpha,m_dbdelta,tempgamma,tempphi);
								Settings.Add(pSetting);
								Results.Add(pTempResult);
								SSEs.push_back(dbSSE);
							}
							tempphi=tempphi+pphiconf->m_dbStep;

						}

					}
					else
					{
						double dbSSE;
						pTempResult=new CResult;
						ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,m_dbdelta,tempgamma,m_dbphi,pTempResult,&dbSSE);
						pSetting=new SmoothSetting(m_dbalpha,m_dbdelta,tempgamma,m_dbphi);
						Settings.Add(pSetting);
						Results.Add(pTempResult);
						SSEs.push_back(dbSSE);


					}


					tempgamma=tempgamma+pgammaconf->m_dbStep;

				}

			}
			else
			{

				if( bphigrid)
				{
					tempphi=pphiconf->m_dbFrom;
					while( tempphi<=pphiconf->m_dbTo)
					{
						double dbSSE;
						if( tempphi==0||tempphi==1)
						{
							pTempResult=new CResult("警告");
							CRsltElementText* pText=new CRsltElementText("警告");
							pText->AddString("Phi值不能为0或1");
							pTempResult->Add(pText);
							Results.Add(pTempResult);
							Settings.Add(NULL);
							SSEs.push_back(0);
						}
						else
						{
							pTempResult=new CResult;
							ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,m_dbdelta,m_dbgamma,tempphi,pTempResult,&dbSSE);
							pSetting=new SmoothSetting(m_dbalpha,m_dbdelta,m_dbgamma,tempphi);
							Settings.Add(pSetting);
							Results.Add(pTempResult);
							SSEs.push_back(dbSSE);
						}				
						tempphi=tempphi+pphiconf->m_dbStep;

					}

				}
				else
				{
					double dbSSE;
					pTempResult=new CResult;
					ExponSmooth(XVec,strFName,m_nSeason,m_nTrend,m_dbalpha,m_dbdelta,m_dbgamma,m_dbphi,pTempResult,&dbSSE);
					pSetting=new SmoothSetting(m_dbalpha,m_dbdelta,m_dbgamma,m_dbphi);
					Settings.Add(pSetting);
					Results.Add(pTempResult);
					SSEs.push_back(dbSSE);


				}



			}






		}





	}
	int nResSize=SSEs.size();
	int *indexs=new int[nResSize];
	int i,j,smallindex=-1,tempindex;
	double tempdb;
	for (i=0;i<nResSize;i++)
		indexs[i]=i;
	for (i=0;i<nResSize;i++)
	{
		if(Settings[i]!=NULL)
		{
			for (j=i+1;j<nResSize;j++)
			{
				if( Settings[j]!=NULL)
				{
					if (SSEs[i]>SSEs[j])
					{
						tempdb=SSEs[i];
						SSEs[i]=SSEs[j];
						SSEs[j]=tempdb;
						tempindex=indexs[i];
						indexs[i]=indexs[j];
						indexs[j]=tempindex;

					}
				}
			}
		}
	}

	CTLTable *pTable=new CTLTable;
	pTable->SetTitle("SSE 一览表");
	CRsltElementTable * pE=new CRsltElementTable("SSE 一览表",pTable);
	pResult->Add(pE); 
	InsertSSECols(nSeason,nTrend,pTable);
	CTString strVal;
	int itemindex=0;
	if (m_bShowBestModels)
	{
		for ( i=0;i<10&&i<nResSize;i++)
		{
			pSetting =(SmoothSetting*)(Settings.GetAt(indexs[i]));
			if(pSetting!=NULL)
			{
				SetSSEItem(nSeason,nTrend,pTable,itemindex,pSetting,SSEs[i]);		
				if (smallindex<0)
					smallindex=i;
				else
				{
					if( SSEs[smallindex]>SSEs[i])
						smallindex=i;
				}
				itemindex++;
			}

		}
	}
	else
	{

		for ( i=0;i<nResSize;i++)
		{
			pSetting =(SmoothSetting*)(Settings.GetAt(indexs[i]));
			if(pSetting!=NULL)
			{
				SetSSEItem(nSeason,nTrend,pTable,itemindex,pSetting,SSEs[i]);		
				if (i==0)
					smallindex=0;
				else
				{
					if( SSEs[smallindex]>SSEs[i])
						smallindex=i;
				}
				itemindex++;
			}

		}



	}



	for ( i=0;i<nResSize;i++)
	{
		pTempResult=(CResult*)Results[indexs[i]];
		if ( i!=smallindex)
			delete pTempResult;
		else
		{
			for(int j=0;j<pTempResult->Count();j++)
				pResult->Add(pTempResult->Item(j));
		}
	}

	if (indexs)
		delete []indexs;



}


