// TimeSeries.cpp: implementation of the CTimeSeries class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tlAdvn.h"
#include "TimeSeries.h"
            

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

const double BOUNDARY=1E-15;
CTimeSeries::CTimeSeries()
{

	m_pResult=NULL;
	m_pDataInterface=NULL;
//	m_pRsltVariable=NULL;
	m_bIsToEnd=false;
	m_nModel=0;

	//ARIMA MODEL
	m_bConstant=true;
	m_nParam=1;
	m_nSquareSum=1;
	m_nAutoReg=1;
	m_nDiff=0;
	m_nMaxIteration=120;
	m_nMoveAv=0;
	m_nForcast=0;
	m_bResidue=false;
	
	//AR MODEL
	m_bConstantAR=true;
	m_nParamAR=1;
	m_nSquareSumAR=1;
	m_nAutoRegAR=1;
	m_nDiffAR=0;
	m_nMaxIterationAR=10;
	m_nMoveAvAR=0;
	m_nForcastAR=0;
	m_bResidueAR=false;

	//MA MODEL
	m_bConstantMA=true;
	m_nParamMA=1;
	m_nSquareSumMA=1;
	m_nAutoRegMA=0;
	m_nDiffMA=0;
	m_nMaxIterationMA=120;
	m_nMoveAvMA=1;
	m_nForcastMA=0;	
	m_nModel=1;
	m_bResidueMA=false;
	

}

CTimeSeries::~CTimeSeries()
{
	m_tVarInfo.DeleteObject();
	m_tVarInfoAR.DeleteObject();
	m_tVarInfoMA.DeleteObject();

}

CResult * CTimeSeries::OnTimeSeries(CDataInterface *pDataInterface,int nModel)
{
	m_pDataInterface = pDataInterface;
//	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
	m_nModel=nModel;
	m_bIsToEnd=false;
	m_nModel=nModel;
	CDoubleVector temVec;
	m_eVector=temVec;

	//AR MODEL
	if(nModel==1)
	{
		briefrTable=NULL;
		m_pResult = new CResult("时间序列-自回归模型分析");
		if (true)
		{
			int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
			CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
			if (nOutNote)
			{			
				briefrTable=new CTLTable();
				briefrTable->InsertColumn(0,"结果说明");
			}

			int p=m_nAutoRegAR;
			int q=m_nMoveAvAR;
			int d=m_nDiffAR;
			int L=12;
			int cons=0;
			double pEps=0.0001;
			double sEps=1e-5;
			int MaxIteration=m_nMaxIterationAR;
			int MaxIter=20;
			if(m_nParamAR==0)
				pEps=0.01;
			else if(m_nParamAR==1)
				pEps=0.001;
			else if(m_nParamAR==2)
				pEps=0.0001;
			else 
				pEps=0.00001;
			if(m_nSquareSumAR==0)
				sEps=0.0001;
			else if(m_nSquareSumAR==1)
				sEps=0.00001;
			else
				sEps=0.000001;
			if(m_bConstantAR)
				cons=1;
			else
				cons=0;
			int i = 0;
			for(i = 0;i < m_tVarInfoAR.iCount;i++)
			{	
				int nIndex = m_tVarInfoAR.pValue[i];
				m_nCurVarNum = nIndex;
				bool bIsToEnd=false;
				m_bIsToEnd=false;
				CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
				if(bIsToEnd )
				{
					CTString str;
					str=pDataInterface->GetFieldName(nIndex);
					str+="有空值或缺失值,不能做时间序列分析！";
					if(m_pRsltElementText==NULL)
						m_pRsltElementText=new CRsltElementText("警告");
					m_pRsltElementText->AddString(str);
					continue;				
				}
				Arima(vect,p,q,d,cons,m_nForcastAR,pEps,sEps,MaxIteration,MaxIter); //求和自回归滑动平均
//				m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
//				if(!Arima(vect,p,q,d,cons,m_nForcastAR,pEps,sEps,MaxIteration,MaxIter)) //求和自回归滑动平均
//				{
////					delete m_pRsltVariable;
//				}
//				else
//					m_pResult->Add(m_pRsltVariable);

			}
			// add by liytao 20030819
			if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
					if (m_pResult->Count()<=0)
						m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
					else
						m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
			}
			else if (briefrTable!=NULL)
				delete briefrTable;
			if(m_pRsltElementText)
				m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
		else 
		{
			delete m_pResult;
			m_pResult=NULL;
			return NULL;
		}
	}

	////////MA MODEL
	if(nModel==2)
	{
		briefrTable=NULL;
		m_pResult = new CResult("时间序列-移动平均模型分析");

		if (true)
		{
			int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
			CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
			if (nOutNote)
			{
				briefrTable=new CTLTable();
				briefrTable->InsertColumn(0,"结果说明");
			}	
			int p=m_nAutoRegMA;
			int q=m_nMoveAvMA;
			int d=m_nDiffMA;
			int L=12;
			int cons=0;
			double pEps=0.0001;
			double sEps=1e-5;
			int MaxIteration=m_nMaxIterationMA;
			int MaxIter=20;
			if(m_nParamMA==0)
				pEps=0.01;
			else if(m_nParamMA==1)
				pEps=0.001;
			else if(m_nParamMA==2)
				pEps=0.0001;
			else 
				pEps=0.00001;
			if(m_nSquareSumMA==0)
				sEps=0.0001;
			else if(m_nSquareSumMA==1)
				sEps=0.00001;
			else
				sEps=0.000001;
			if(m_bConstantMA)
				cons=1;
			else
				cons=0;

			int i = 0;
			for(i = 0;i < m_tVarInfoMA.iCount;i++)
			{	
				int nIndex = m_tVarInfoMA.pValue[i];
				m_nCurVarNum = nIndex;
				bool bIsToEnd=false;
				m_bIsToEnd=false;
				CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
				if(bIsToEnd )
				{
					CTString str;
					str=pDataInterface->GetFieldName(nIndex);
					str+="有空值或缺失值,不能做时间序列分析！";
					if(m_pRsltElementText==NULL)
						m_pRsltElementText=new CRsltElementText("警告");
					m_pRsltElementText->AddString(str);
					continue;				
				}
//				m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
				Arima(vect,p,q,d,cons,m_nForcastMA,pEps,sEps,MaxIteration,MaxIter);
//				if(!Arima(vect,p,q,d,cons,m_nForcastMA,pEps,sEps,MaxIteration,MaxIter))
//					delete m_pRsltVariable; //求和自回归滑动平均
//				else
//					m_pResult->Add(m_pRsltVariable);

			}
			// add by liytao 20030819
			if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
					if (m_pResult->Count()<=0)
						m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
					else
						m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
			}
			else if (briefrTable!=NULL)
				delete briefrTable;

			if(m_pRsltElementText)
				m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
		else 
		{
			delete m_pResult;
			m_pResult=NULL;
			return NULL;
		}
	}

	///ARIMA MODEL
	if(nModel==3)
	{
		briefrTable=NULL;
		m_pResult = new CResult("时间序列-ARIMA模型分析");
//		CTimeseriesDlg dlg(pDataInterface,this);
//		
//		if( dlg.DoModal() == IDOK )
		if (true)
		{
			
			int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
			CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
			if (nOutNote)
			{
				briefrTable=new CTLTable();
				briefrTable->InsertColumn(0,"结果说明");
			}

			int p=m_nAutoReg;
			int q=m_nMoveAv;
			int d=m_nDiff;
			int L=12;
			int cons=0;
			double pEps=0.0001;
			double sEps=1e-5;
			int MaxIteration=0;//?dlg.m_nMaxIteration;
			int MaxIter=20;
			if(m_nParam==0)
				pEps=0.01;
			else if(m_nParam==1)
				pEps=0.001;
			else if(m_nParam==2)
				pEps=0.0001;
			else 
				pEps=0.00001;
			if(m_nSquareSum==0)
				sEps=0.0001;
			else if(m_nSquareSum==1)
				sEps=0.00001;
			else
				sEps=0.000001;
			if(m_bConstant)
				cons=1;
			else
				cons=0;

			int i = 0;
			for(i = 0;i < m_tVarInfo.iCount;i++)
			{	
				int nIndex = m_tVarInfo.pValue[i];
				m_nCurVarNum = nIndex;
				bool bIsToEnd=false;
				m_bIsToEnd=false;
				CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
				if(bIsToEnd )
				{
					CTString str;
					str=pDataInterface->GetFieldName(nIndex);
					str+="有空值或缺失值,不能做时间序列分析！";
					if(m_pRsltElementText==NULL)
						m_pRsltElementText=new CRsltElementText("警告");
					m_pRsltElementText->AddString(str);
					continue;				
				}
//				m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
				Arima(vect,p,q,d,cons,m_nForcast,pEps,sEps,MaxIteration,MaxIter);
//				if(!Arima(vect,p,q,d,cons,m_nForcast,pEps,sEps,MaxIteration,MaxIter))
//						delete m_pRsltVariable; //求和自回归滑动平均
//				else
//					m_pResult->Add(m_pRsltVariable);
			}

			// add by liytao 20030819
			if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
					if (m_pResult->Count()<=0)
						m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
					else
						m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
			}
			else if (briefrTable!=NULL)
				delete briefrTable;

			if(m_pRsltElementText)
				m_pResult->Add(m_pRsltElementText);
			return m_pResult;
		}
		else 
		{
			delete m_pResult;
			m_pResult=NULL;
			return NULL;
		}
	}
	return NULL;
} 

bool CTimeSeries::GetValueFromVOAR(CDataInterface * pDataInterface, string VO)
{
//	TIMESERIESAR
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/MODEL [PARAMETER({1**} {0**} {0**})] [NOCONST]
//		/CRITERIA=[MAXSTEP({10**})] [COEFCHANGE({0.001**})] [SSECHANGE({0.001**})]
//		/PRINT [RESIDUAL]
//		/RESULT [PREDICT({1})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfoAR)))
		return FALSE;
	
	m_nAutoRegAR = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr);
	m_nDiffAR = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr, 2);
	m_nMoveAvAR = 0;
	m_bConstantAR = !AnaSyntax::findstr("/MODEL", "NOCONST", VOstr);

	m_nMaxIterationAR = 10;
	if (AnaSyntax::findstr("/CRITERIA", "MAXSTEP", VOstr))
		m_nMaxIterationAR = AnaSyntax::getvalue("/CRITERIA", "MAXSTEP", VOstr);

	int coefchange = (AnaSyntax::getvalue("/CRITERIA", "COEFCHANGE", VOstr)*100000);
	if (coefchange == 1 ) 
		m_nParamAR = 3;
	else if (coefchange == 10)
		m_nParamAR = 2;
	else if (coefchange == 1000)
		m_nParamAR = 0;
	else
		m_nParamAR = 1;
	int ssechange = (AnaSyntax::getvalue("/CRITERIA", "SSECHANGE", VOstr)*100000);
	if (ssechange == 10)
		m_nSquareSumAR = 2;
	else if (ssechange == 1000)
		m_nSquareSumAR = 0;
	else
		m_nSquareSumAR = 1;

	m_bResidueAR = AnaSyntax::findstr("/PRINT", "RESIDUAL", VOstr);
	m_nForcastAR = 0;
	if (AnaSyntax::findstr("/RESULT", "PREDICT", VOstr))
		m_nForcastAR = AnaSyntax::getvalue("/RESULT", "PREDICT", VOstr);

	return TRUE;

}

CResult * CTimeSeries::OnTimeSeriesAR(CDataInterface *pDataInterface, string VO)
{
    m_nModel =1;
	m_pDataInterface = pDataInterface;

//	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
	m_bIsToEnd=false;
// 	m_nModel=nModel;
	CDoubleVector temVec;
	m_eVector=temVec;

	briefrTable=NULL;
	m_pResult = new CResult("时间序列-自回归模型分析");
	if (!GetValueFromVOAR(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{			
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}

	int p=m_nAutoRegAR;
	int q=m_nMoveAvAR;
	int d=m_nDiffAR;
	int L=12;
	int cons=0;
	double pEps=0.0001;
	double sEps=1e-5;
	int MaxIteration=m_nMaxIterationAR;
	int MaxIter=20;
	if(m_nParamAR==0)
		pEps=0.01;
	else if(m_nParamAR==1)
		pEps=0.001;
	else if(m_nParamAR==2)
		pEps=0.0001;
	else 
		pEps=0.00001;
	if(m_nSquareSumAR==0)
		sEps=0.0001;
	else if(m_nSquareSumAR==1)
		sEps=0.00001;
	else
		sEps=0.000001;
	if(m_bConstantAR)
		cons=1;
	else
		cons=0;

	int i = 0;
	for(i = 0;i < m_tVarInfoAR.iCount;i++)
	{	
		int nIndex = m_tVarInfoAR.pValue[i];
		m_nCurVarNum = nIndex;
		bool bIsToEnd=false;
		m_bIsToEnd=false;
		CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
		if(bIsToEnd )
		{
			CTString str;
			str=pDataInterface->GetFieldName(nIndex);
			str+="有空值或缺失值,不能做时间序列分析！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			continue;				
		}
//		m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
//				m_pResult->Add(m_pRsltVariable);
		Arima(vect,p,q,d,cons,m_nForcastAR,pEps,sEps,MaxIteration,MaxIter); //求和自回归滑动平均
//		if(!Arima(vect,p,q,d,cons,m_nForcastAR,pEps,sEps,MaxIteration,MaxIter)) //求和自回归滑动平均
//		{
//			delete m_pRsltVariable;
//		}
//		else
//			m_pResult->Add(m_pRsltVariable);

	}
	// add by liytao 20030819
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (m_pResult->Count()<=0)
				m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;

	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
} 

bool CTimeSeries::GetValueFromVOMA(CDataInterface * pDataInterface, string VO)
{
// 	TIMESERIESMA
// 		/DATA=[FILE]
// 		/VARLIST [varlist]
// 		/MODEL [PARAMETER({0**} {0**} {1**})] [NOCONST]
// 		/CRITERIA=[MAXSTEP({10**})] [COEFCHANGE({0.001**})] [SSECHANGE({0.001**})]
// 		/PRINT [RESIDUAL]
// 		/RESULT [PREDICT({1})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfoMA)))
		return FALSE;
	
	m_nAutoRegMA = 0;
	m_nDiffMA = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr, 2);
	m_nMoveAvMA = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr, 3);
	m_bConstantMA = !AnaSyntax::findstr("/MODEL", "NOCONST", VOstr);
	
	m_nMaxIterationMA = 50;
	if (AnaSyntax::findstr("/CRITERIA", "MAXSTEP", VOstr))
		m_nMaxIterationMA = AnaSyntax::getvalue("/CRITERIA", "MAXSTEP", VOstr);
	
	int coefchange = (AnaSyntax::getvalue("/CRITERIA", "COEFCHANGE", VOstr)*100000);
	if (coefchange == 1 ) 
		m_nParamMA = 3;
	else if (coefchange == 10)
		m_nParamMA = 2;
	else if (coefchange == 1000)
		m_nParamMA = 0;
	else
		m_nParamMA = 1;
	int ssechange = (AnaSyntax::getvalue("/CRITERIA", "SSECHANGE", VOstr)*100000);
	if (ssechange == 10)
		m_nSquareSumMA = 2;
	else if (ssechange == 1000)
		m_nSquareSumMA = 0;
	else
		m_nSquareSumMA = 1;
	
	m_bResidueMA = AnaSyntax::findstr("/PRINT", "RESIDUAL", VOstr);
	m_nForcastMA = 0;
	if (AnaSyntax::findstr("/RESULT", "PREDICT", VOstr))
		m_nForcastMA = AnaSyntax::getvalue("/RESULT", "PREDICT", VOstr);
	
	return TRUE;

}

CResult * CTimeSeries::OnTimeSeriesMA(CDataInterface *pDataInterface, string VO)
{
	m_nModel =2;
	m_pDataInterface = pDataInterface;
//	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
// 	m_nModel=nModel;
	m_bIsToEnd=false;
// 	m_nModel=nModel;
	CDoubleVector temVec;
	m_eVector=temVec;
	////////MA MODEL
	briefrTable=NULL;
	m_pResult = new CResult("时间序列-移动平均模型分析");

	if (!GetValueFromVOMA(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}	
	int p=m_nAutoRegMA;
	int q=m_nMoveAvMA;
	int d=m_nDiffMA;
	int L=12;
	int cons=0;
	double pEps=0.0001;
	double sEps=1e-5;
	int MaxIteration=m_nMaxIterationMA;
	int MaxIter=20;
	if(m_nParamMA==0)
		pEps=0.01;
	else if(m_nParamMA==1)
		pEps=0.001;
	else if(m_nParamMA==2)
		pEps=0.0001;
	else 
		pEps=0.00001;
	if(m_nSquareSumMA==0)
		sEps=0.0001;
	else if(m_nSquareSumMA==1)
		sEps=0.00001;
	else
		sEps=0.000001;
	if(m_bConstantMA)
		cons=1;
	else
		cons=0;

	int i = 0;
	for(i = 0;i < m_tVarInfoMA.iCount;i++)
	{	
		int nIndex = m_tVarInfoMA.pValue[i];
		m_nCurVarNum = nIndex;
		bool bIsToEnd=false;
		m_bIsToEnd=false;
		CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
		if(bIsToEnd )
		{
			CTString str;
			str=pDataInterface->GetFieldName(nIndex);
			str+="有空值或缺失值,不能做时间序列分析！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			continue;				
		}
//		m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
		Arima(vect,p,q,d,cons,m_nForcastMA,pEps,sEps,MaxIteration,MaxIter);
//		if(!Arima(vect,p,q,d,cons,m_nForcastMA,pEps,sEps,MaxIteration,MaxIter))
//			delete m_pRsltVariable; //求和自回归滑动平均
//		else
//			m_pResult->Add(m_pRsltVariable);

	}
	// add by liytao 20030819
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (m_pResult->Count()<=0)
				m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
} 

bool CTimeSeries::GetValueFromVOARIMA(CDataInterface * pDataInterface, string VO)
{
	// 	TIMESERIESARIMA
	// 		/DATA=[FILE]
	// 		/VARLIST [varlist]
	// 		/MODEL [PARAMETER({0**} {0**} {1**})] [NOCONST]
	// 		/CRITERIA=[MAXSTEP({10**})] [COEFCHANGE({0.001**})] [SSECHANGE({0.001**})]
	// 		/PRINT [RESIDUAL]
	// 		/RESULT [PREDICT({1})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_nAutoReg = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr);
	m_nDiff = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr, 2);
	m_nMoveAv = AnaSyntax::getvalue("/MODEL", "PARAMETER", VOstr, 3);
	m_bConstant = !AnaSyntax::findstr("/MODEL", "NOCONST", VOstr);
	
	m_nMaxIteration = 50;
	if (AnaSyntax::findstr("/CRITERIA", "MAXSTEP", VOstr))
		m_nMaxIteration = AnaSyntax::getvalue("/CRITERIA", "MAXSTEP", VOstr);
	
	int coefchange = (AnaSyntax::getvalue("/CRITERIA", "COEFCHANGE", VOstr)*100000);
	if (coefchange == 1 ) 
		m_nParam = 3;
	else if (coefchange == 10)
		m_nParam = 2;
	else if (coefchange == 1000)
		m_nParam = 0;
	else
		m_nParam = 1;
	int ssechange = (AnaSyntax::getvalue("/CRITERIA", "SSECHANGE", VOstr)*100000);
	if (ssechange == 10)
		m_nSquareSum = 2;
	else if (ssechange == 1000)
		m_nSquareSum = 0;
	else
		m_nSquareSum = 1;
	
	m_bResidue = AnaSyntax::findstr("/PRINT", "RESIDUAL", VOstr);
	m_nForcast = 0;
	if (AnaSyntax::findstr("/RESULT", "PREDICT", VOstr))
		m_nForcast = AnaSyntax::getvalue("/RESULT", "PREDICT", VOstr);
	
	return TRUE;

}

CResult * CTimeSeries::OnTimeSeriesARIMA(CDataInterface *pDataInterface, string VO)
{
	m_nModel =3;
	m_pDataInterface = pDataInterface;
	m_pResult = NULL;
	m_pResult = new CResult("时间序列-ARIMA模型分析");
	if (!GetValueFromVOARIMA(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
//	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
// 	m_nModel=nModel;
	m_bIsToEnd=false;
// 	m_nModel=nModel;
	CDoubleVector temVec;
	m_eVector=temVec;

	///ARIMA MODEL
	briefrTable=NULL;
	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}

	int p=m_nAutoReg;
	int q=m_nMoveAv;
	int d=m_nDiff;
	int L=12;
	int cons=0;
	double pEps=0.0001;
	double sEps=1e-5;
	int MaxIteration=m_nMaxIteration;
	int MaxIter=20;
	if(m_nParam==0)
		pEps=0.01;
	else if(m_nParam==1)
		pEps=0.001;
	else if(m_nParam==2)
		pEps=0.0001;
	else 
		pEps=0.00001;
	if(m_nSquareSum==0)
		sEps=0.0001;
	else if(m_nSquareSum==1)
		sEps=0.00001;
	else
		sEps=0.000001;
	if(m_bConstant)
		cons=1;
	else
		cons=0;

	int i = 0;
	for(i = 0;i < m_tVarInfo.iCount;i++)
	{	
		int nIndex = m_tVarInfo.pValue[i];
		m_nCurVarNum = nIndex;
		bool bIsToEnd=false;
		m_bIsToEnd=false;
		CDoubleVector vect = GetSingleColData(nIndex,bIsToEnd);
		if(bIsToEnd )
		{
			CTString str;
			str=pDataInterface->GetFieldName(nIndex);
			str+="有空值或缺失值,不能做时间序列分析！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			continue;				
		}
		Arima(vect,p,q,d,cons,m_nForcast,pEps,sEps,MaxIteration,MaxIter);
//		m_pRsltVariable =new CRsltElementTable(m_pDataInterface->GetFieldName(m_nCurVarNum));
//		if(!Arima(vect,p,q,d,cons,m_nForcast,pEps,sEps,MaxIteration,MaxIter))
//			delete m_pRsltVariable; //求和自回归滑动平均
//		else
//			m_pResult->Add(m_pRsltVariable);
	}

	// add by liytao 20030819
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (m_pResult->Count()<=0)
				m_pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				m_pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;

	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
} 


CDoubleVector CTimeSeries::GetSingleColData(int index,bool &bIsToEnd)
{
	TDataSet *pDataSet=NULL;
	int nRow=m_pDataInterface->GetColData(index,pDataSet,5);
	CDoubleVector vect(nRow);
	for(int i=0;i<nRow;i++)
	{
		if(pDataSet[i].iIndex>=0)
			vect(i)=pDataSet[i].dValue;
		else
		{
			bIsToEnd=true;
			break;
		}
	}
	if(pDataSet)
		delete []pDataSet;
	return vect;	
}



void CTimeSeries::ac_pac(CDoubleVector Z_Vector, int K,
						 CDoubleVector &gamma_Vector,
						 CDoubleVector &rho_Vector)
{
	int n=Z_Vector.vlen();
	double mu=mean(Z_Vector);
	CDoubleVector W_Vector=Z_Vector-mu;
	int k=0;
	for(k=0;k<=K;k++)
	{
		gamma_Vector(k)=0;
		for(int i=0;i<n-k;i++)
		{
			gamma_Vector(k)=gamma_Vector(k)+W_Vector(i)*W_Vector(i+k);
		}
		gamma_Vector(k)=gamma_Vector(k)/n;
	}
	for(k=1;k<=K;k++)
		rho_Vector(k)=gamma_Vector(k)/gamma_Vector(0);

}


int CTimeSeries::delta(int j,int k)
{
	if(j==k)
		return 1;
	else
		return 0;
}


CDoubleVector CTimeSeries::GetARBeta(CDoubleVector Z_Vector,
								   CDoubleVector preBeta_Vector,
								   int p,int cons,double pEps,
								   double sEps,int MaxIteration,			   
								   CDoubleMatrix &covErr_Matrix)
{
	CDoubleVector beta_Vector;
	int r=Z_Vector.vlen();
	int c=1;
	double paramError=9e8;
	double sumsqError=9e18;
	int cir=0;
	double lambda=1e-4;
	CDoubleVector e_Vector(r-p+1);
	CDoubleMatrix FF_Matrix(r-p+1,p+1);	
	CDoubleVector X_Vector=Z_Vector-preBeta_Vector(0);
	double oldQ=dot(X_Vector,X_Vector);
//	CDoubleMatrix oldcovErr_Matrix(p+cons,p+cons);
	CDoubleMatrix step_Matrix;
	//WriteData(X_Vector,"X_Vector");
	while(!(paramError<pEps&&sumsqError<sEps)&&cir<MaxIteration)
	{	
		X_Vector=Z_Vector-preBeta_Vector(0);
		int i=0;
		for(i=p;i<r;i++)
		{
			e_Vector(i-p)=X_Vector(i);
			for(int j=0;j<p;j++)
				e_Vector(i-p)=e_Vector(i-p)-preBeta_Vector(j+1)*X_Vector(i-j-1);
		}
		//WriteData(e_Vector,"e_Vector");
		double tmp=-1;
		for(i=0;i<p;i++)
			tmp=tmp+preBeta_Vector(i+1);
		for(i=p;i<r;i++)
		{
			FF_Matrix(i-p,0)=2*e_Vector(i-p)*tmp;
			for(int j=0;j<p;j++)
			{
				FF_Matrix(i-p,j+1)=-2*e_Vector(i-p)*X_Vector(i-j-1);
			}
		}
		TraceMatrix(FF_Matrix);
		CDoubleMatrix XX_Matrix=trans(FF_Matrix)*FF_Matrix;
		TraceMatrix(XX_Matrix);
		//WriteData(XX_Matrix,"XX_Matrix");
		CDoubleMatrix D_Matrix(XX_Matrix.mrow(),XX_Matrix.mcol());
		//WriteData(FF_Matrix,"FF_Matrix");
		for(i=0;i<XX_Matrix.mrow();i++)
		{
			for(int j=0;j<D_Matrix.mcol();j++)
			{
				if(i==j)
					D_Matrix(i,j)=XX_Matrix(i,j);
				else
					D_Matrix(i,j)=0;
			}
		}
		TraceMatrix(D_Matrix);
		
		CDoubleMatrix e_Matrix(r-p+1,1);
		for(i=0;i<r-p+1;i++)
			e_Matrix(i,0)=e_Vector(i);
		CDoubleMatrix temp_Matrix=trans(e_Matrix)*FF_Matrix*trans(FF_Matrix)*e_Matrix;
		TraceMatrix(temp_Matrix);
//		lambda=temp_Matrix(0,0)/oldQ;
//		if(lambda<1)
//			lambda=1000;
		lambda = lambda/10.0;
		CDoubleMatrix temp1_Matrix=lambda*D_Matrix;
		temp1_Matrix=temp1_Matrix+XX_Matrix;
		//WriteData(temp1_Matrix,"temp1_Matrix");
		//WriteData(XX_Matrix,"XX_Matrix");
		if(fabs(det(temp1_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			CDoubleVector vect;
			return vect;
		}
		step_Matrix=inv(temp1_Matrix);
		beta_Vector=preBeta_Vector-(step_Matrix*trans(FF_Matrix)*e_Vector);
	//	WriteData(step_Matrix,"step_Matrix");
	//	WriteData(beta_Vector,"beta_Vector");
		CDoubleVector temp_Vector=beta_Vector-preBeta_Vector;
		temp_Vector=temp_Vector.abs();
		double paramError=temp_Vector.Max();
 		X_Vector=Z_Vector-beta_Vector(0);
		double Q=dot(e_Vector,e_Vector);
		sumsqError=fabs((Q-oldQ)/oldQ);
		if(Q>=oldQ)
		{
			beta_Vector=copy(preBeta_Vector);
			Q=oldQ;
			break;
		}

		preBeta_Vector=copy(beta_Vector);
		oldQ=Q;
		cir=cir+1;
	}
	if(cir>MaxIteration)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="在指定的迭代条件下不收敛！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
	}
	//WriteData(step_Matrix,"step_Matrix");
	//WriteData(FF_Matrix,"FF_Matrix");
	covErr_Matrix=step_Matrix*trans(FF_Matrix)*FF_Matrix*step_Matrix;
	return beta_Vector;
}
CDoubleVector CTimeSeries::GetARBeta2(CDoubleVector Z_Vector,
								   CDoubleVector preBeta_Vector,
								   int p,int cons,double pEps,
								   double sEps,int MaxIteration,			   
								   CDoubleMatrix &covErr_Matrix)
{
	CDoubleVector beta_Vector;
	int r=Z_Vector.vlen();
	int c=1;
	double paramError=9e8;
	double sumsqError=9e18;
	int cir=0;
	double lambda=0.1;
	CDoubleMatrix X_Matrix(r-p,p+1);
	CDoubleMatrix Y_Matrix(r-p,1);
	
	
	int i =0;
	for( i =0;i< r-p;i++)
		for(int j =0; j< p+1;j++)
		{
			if(j==0)
				X_Matrix(i,j) = 1;
			else
				X_Matrix(i,j) = Z_Vector[i+p-j]; 
		}
	for( i =0;i< r-p;i++)
		Y_Matrix(i,0) = Z_Vector[i+p];
//	TraceMatrix(Y_Matrix);
	CDoubleMatrix XX_Matrix=trans(X_Matrix)*X_Matrix;
	//TraceMatrix(XX_Matrix);
	CDoubleMatrix D_Matrix(XX_Matrix.mrow(),XX_Matrix.mcol());
	D_Matrix =diag(diag(XX_Matrix));
	
	CDoubleVector Beta_Vector;
	CDoubleVector Ep_Matrix;
	double oldQ;//dot(X_Vector,X_Vector);
//	CDoubleMatrix oldcovErr_Matrix(p+cons,p+cons);
	CDoubleMatrix step_Matrix;
	CDoubleMatrix temp1_Matrix;
	//WriteData(X_Vector,"X_Vector");
	double Sum =0;
	for(i =1 ;i< p+1; i++)
		Sum += preBeta_Vector(i);
	preBeta_Vector(0) = preBeta_Vector(0)*(1-Sum);
	TraceVector(preBeta_Vector);
	temp1_Matrix = Y_Matrix - X_Matrix*VectorToMatrix( preBeta_Vector,p+1,1);
	Ep_Matrix = MatrixToVector(temp1_Matrix);
	//TraceVector(Ep_Matrix);
	oldQ = dot(Ep_Matrix,Ep_Matrix);
 	//TRACE("oldQ = %.16f\n",oldQ);	
	while(!(paramError<pEps&&sumsqError<sEps)&&cir<MaxIteration)
	{		
		lambda = 1e-5/pow(10,cir);
		temp1_Matrix=lambda*D_Matrix;
		temp1_Matrix=temp1_Matrix+XX_Matrix;

		if(fabs(det(temp1_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			CDoubleVector vect;
			return vect;
		}
		
		step_Matrix=inv(temp1_Matrix);
		TraceMatrix(step_Matrix);
		CDoubleMatrix Delta = step_Matrix*trans(X_Matrix);
		Delta = Delta * VectorToMatrix(Ep_Matrix,Ep_Matrix.vlen(),1);
		TraceMatrix(Delta);

		Beta_Vector=preBeta_Vector+MatrixToVector(Delta);
		CDoubleVector temp_Vector=(Beta_Vector-preBeta_Vector)/(preBeta_Vector+1e-20);
		temp_Vector=temp_Vector.abs();
		paramError=temp_Vector.Max();
		//TRACE("paramError= %.6f \n",paramError);

		temp1_Matrix = Y_Matrix - X_Matrix*VectorToMatrix( Beta_Vector,p+1,1);
		Ep_Matrix = MatrixToVector(temp1_Matrix);
		double Q=dot(Ep_Matrix,Ep_Matrix);
		//TRACE("Q = %.16f\n",Q);
		sumsqError=fabs((Q-oldQ)/oldQ);
		//TRACE("sumsqError= %.16f  \n cir =%d \n ",sumsqError,cir);
//		if(Q>=oldQ)
//		{
//			Beta_Vector=copy(preBeta_Vector);
//			Q=oldQ;
//			break;
//		}


		preBeta_Vector=copy(Beta_Vector);
		TraceVector(preBeta_Vector);
		oldQ=Q;
		cir=cir+1;
	}
	if(cir>MaxIteration)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="在指定的迭代条件下不收敛！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
	}
	covErr_Matrix=step_Matrix*trans(X_Matrix)*X_Matrix*step_Matrix;
	TraceMatrix(covErr_Matrix);
	beta_Vector= preBeta_Vector;
	return beta_Vector;
}
void CTimeSeries::GetArimaBeta2(CDoubleVector Z_Vector,
							  CDoubleVector preBeta_Vector,
							  int p,int q,int cons,
							  double pEps,double sEps,int MaxIteration,
							  double &Q,double &ResVar,double &StdE,int &df,double &AIC,
							  CDoubleVector &beta_Vector,
							  CDoubleMatrix &covErr_Matrix)

{
	int r=Z_Vector.vlen();
	int c=1;
	CDoubleVector ww_Vector(r-p);
	int i=0;
	for(i=0;i<r-p;i++)
		ww_Vector(i)=Z_Vector(p+i);
	int Num=0;
	if(p>=q)
		Num=p;
	else
		Num=q;
	Num=Num*10;
	double paramError=9e8;
	double sumsqError=9e18;
	int cir=0;
	double lambda=1;
	CDoubleVector I_Vector(Num);
	CDoubleVector F_Vector;
	CDoubleVector e_Vector;
	CDoubleVector w_Vector;
	double mu=mean(Z_Vector);
	CDoubleVector Tmp_Vector=Z_Vector-mu;
	double oldQ=dot(Tmp_Vector,Tmp_Vector);
	CDoubleMatrix step_Matrix;
	CDoubleMatrix FF_Matrix;
	while(!(paramError<pEps&&sumsqError<sEps)&&cir<MaxIteration)
	{
	//	CDoubleVector I_Vector(Num);
		if(cons==1)
		{
			double temp=preBeta_Vector(0);
			CDoubleVector w_Vector=Z_Vector-temp;
			CDoubleVector tempPreBeta_Vector(p+q);
			for(int i=0;i<p+q;i++)
				tempPreBeta_Vector(i)=preBeta_Vector(1+i);
			
			
			//WriteData(tempPreBeta_Vector,"tempPreBeta_Vector");
			//WriteData(w_Vector,"w_Vector");
			F_Vector=GetI(w_Vector,tempPreBeta_Vector,p,q,Num,I_Vector);
			
			//WriteData(I_Vector,"I_Vector");
			//WriteData(F_Vector,"F_Vector");
			CDoubleMatrix II_Matrix=GetII(I_Vector,tempPreBeta_Vector,p,q,Num);
			
			//WriteData(II_Matrix,"II_Matrix");
			FF_Matrix=GetFF(w_Vector,I_Vector,II_Matrix,p,q,Num,cons);
			//WriteData(FF_Matrix,"FF_Matrix");
			e_Vector=ww_Vector-preBeta_Vector(0)-F_Vector;
			//WriteData(e_Vector,"e_Vector");
		}
		else
		{
			
			F_Vector=GetI(Z_Vector,preBeta_Vector,p,q,Num,I_Vector);
			CDoubleMatrix II_Matrix=GetII(I_Vector,preBeta_Vector,p,q,Num);
			FF_Matrix=GetFF(Z_Vector,I_Vector,II_Matrix,p,q,Num,cons);
			e_Vector=ww_Vector-F_Vector;
		}
		Q=dot(e_Vector,e_Vector);
		if(Q>oldQ&&cir>0)
		{
			beta_Vector=copy(preBeta_Vector);
			Q=oldQ;
			break;
		}
		sumsqError=fabs((Q-oldQ)/Q);
		CDoubleMatrix XX_Matrix=trans(FF_Matrix)*FF_Matrix;
		CDoubleMatrix D_Matrix(XX_Matrix.mrow(),XX_Matrix.mrow());
		int i=0;
		for(i=0;i<D_Matrix.mrow();i++)
		{
			for(int j=0;j<D_Matrix.mcol();j++)
			{
				if(i==j)
					D_Matrix(i,j)=XX_Matrix(i,j);
				else
					D_Matrix(i,j)=0;
			}

		}
		lambda=lambda/1.01;
		CDoubleMatrix temp_Matrix=lambda*D_Matrix;
		temp_Matrix=temp_Matrix+XX_Matrix;
		if(fabs(det(temp_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			return ;
		}

		step_Matrix=inv(temp_Matrix);
		beta_Vector=preBeta_Vector+(step_Matrix*trans(FF_Matrix)*e_Vector);
		CDoubleVector temp_Vector=beta_Vector-preBeta_Vector;
		temp_Vector=temp_Vector.abs();		
		paramError=temp_Vector.Max();
		oldQ=Q;
		preBeta_Vector=copy(beta_Vector);
		cir=cir+1;
	}
	if(cir>MaxIteration)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="在指定的迭代条件下不收敛！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
	}

	if(cons==0)
		df=r-p-q;
	else
		df=r-p-q-1;
	ResVar=oldQ/df;
	StdE=sqrt(ResVar);
	if(cons==0)
		AIC=r*log(ResVar)+2*(p+q+1);
	else
		AIC=r*log(ResVar)+2*(p+q+2);
	//WriteData(step_Matrix,"step_Matrix");
	//WriteData(FF_Matrix,"FF_Matrix");
	covErr_Matrix=step_Matrix*trans(FF_Matrix)*FF_Matrix*step_Matrix;
	covErr_Matrix=ResVar*covErr_Matrix;
	//WriteData(covErr_Matrix,"covErr_Matrix");
	//WriteData(beta_Vector,"beta_Vector");
	//	return covErr_Matrix;

		//zkc 2003.10.21 begin appending
		//在主界面添加残差输出
	if((m_nModel==1&& m_bResidueAR)||(m_nModel==2&&m_bResidueMA)||(m_nModel==3&&m_bResidue))
	{
	
		CTString strFieldName="残差";
		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		while(IsExisted(m_pDataInterface,strFieldName))
		{
			strFieldName+="_1";
		}
		pField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess .First();
		for(i = 0;i<e_Vector.vlen();i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			pField->SetAsDouble(e_Vector(i));
			m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}
		m_eVector=e_Vector;
	}

		//to end the appending.	
	
}
void CTimeSeries::GetArimaBeta(CDoubleVector Z_Vector,
							  CDoubleVector preBeta_Vector,
							  int p,int q,int cons,
							  double pEps,double sEps,int MaxIteration,
							  double &Q,double &ResVar,double &StdE,int &df,double &AIC,
							  CDoubleVector &beta_Vector,
							  CDoubleMatrix &covErr_Matrix)

{
	TraceVector(preBeta_Vector);
	TraceVector(Z_Vector);
	int r=Z_Vector.vlen();
	int c=1;
	CDoubleVector ww_Vector(r-p);
	int i=0;
	for(i=0;i<r-p;i++)
		ww_Vector(i)=Z_Vector(p+i);
	int Num=0;
	if(p>=q)
		Num=p;
	else
		Num=q;
	Num=Num*10;
	double paramError=9e8;
	double sumsqError=9e18;
	int cir=0;
	double lambda=1e-4;
	CDoubleVector I_Vector(Num);
	CDoubleVector F_Vector;
	CDoubleVector e_Vector;
	CDoubleVector w_Vector;
	double mu=mean(Z_Vector);
	CDoubleVector Tmp_Vector=Z_Vector-mu;
	double oldQ=dot(Tmp_Vector,Tmp_Vector);
	CDoubleMatrix step_Matrix;
	CDoubleMatrix FF_Matrix;
	while(!(paramError<pEps&&sumsqError<sEps)&&cir<MaxIteration)
	{
	//	CDoubleVector I_Vector(Num);
		if(cons==1)
		{
			double temp=preBeta_Vector(0);
			CDoubleVector w_Vector=Z_Vector-temp;
			CDoubleVector tempPreBeta_Vector(p+q);
			for(int i=0;i<p+q;i++)
				tempPreBeta_Vector(i)=preBeta_Vector(1+i);
			
			
			F_Vector=GetI(w_Vector,tempPreBeta_Vector,p,q,Num,I_Vector);
			//TraceVector(F_Vector);
			CDoubleMatrix II_Matrix=GetII(I_Vector,tempPreBeta_Vector,p,q,Num);
			//TraceMatrix(II_Matrix);
			FF_Matrix=GetFF(w_Vector,I_Vector,II_Matrix,p,q,Num,cons);
			//TraceMatrix(FF_Matrix);
			e_Vector=ww_Vector-preBeta_Vector(0)-F_Vector;
		}
		else
		{
			
			F_Vector=GetI(Z_Vector,preBeta_Vector,p,q,Num,I_Vector);
			//TraceVector(F_Vector);
			CDoubleMatrix II_Matrix=GetII(I_Vector,preBeta_Vector,p,q,Num);
			FF_Matrix=GetFF(Z_Vector,I_Vector,II_Matrix,p,q,Num,cons);
			e_Vector=ww_Vector-F_Vector;
		}
		Q=dot(e_Vector,e_Vector);
		//TRACE("Q%d = %.16f\n",cir,Q);
		if(Q>oldQ&&cir>0)
		{
			beta_Vector=copy(preBeta_Vector);
			Q=oldQ;
			break;
		}

		sumsqError=fabs((Q-oldQ)/Q);
		CDoubleMatrix XX_Matrix=trans(FF_Matrix)*FF_Matrix;
		CDoubleMatrix D_Matrix(XX_Matrix.mrow(),XX_Matrix.mrow());
		int i=0;
		for(i=0;i<D_Matrix.mrow();i++)
		{
			for(int j=0;j<D_Matrix.mcol();j++)
			{
				if(i==j)
					D_Matrix(i,j)=XX_Matrix(i,j);
				else
					D_Matrix(i,j)=0;
			}

		}
		lambda=lambda/1.50;
		CDoubleMatrix temp_Matrix=lambda*D_Matrix;
		temp_Matrix=temp_Matrix+XX_Matrix;
		if(fabs(det(temp_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			return ;
		}

		step_Matrix=inv(temp_Matrix);
		beta_Vector=preBeta_Vector+(step_Matrix*trans(FF_Matrix)*e_Vector);
		CDoubleVector temp_Vector=beta_Vector-preBeta_Vector;
		temp_Vector=temp_Vector.abs();		
		paramError=temp_Vector.Max();
		oldQ=Q;
		preBeta_Vector=copy(beta_Vector);
		cir=cir+1;
	}
	//TRACE("Q = %.16f\n",Q);
	//TraceVector(preBeta_Vector);
	if(cir>=MaxIteration)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="在指定的迭代条件下不收敛！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
	}

	if(cons==0)
		df=r-p-q;
	else
		df=r-p-q-1;
	ResVar=oldQ/df;
	StdE=sqrt(ResVar);
	if(cons==0)
		AIC=r*log(ResVar)/log(10)+2*(p+q+1);
	else
		AIC=r*log(ResVar)/log(10)+2*(p+q+2);
	//WriteData(step_Matrix,"step_Matrix");
	//WriteData(FF_Matrix,"FF_Matrix");
	covErr_Matrix=step_Matrix*trans(FF_Matrix)*FF_Matrix*step_Matrix;
	covErr_Matrix=ResVar*covErr_Matrix;
	//WriteData(covErr_Matrix,"covErr_Matrix");
	//WriteData(beta_Vector,"beta_Vector");
	//	return covErr_Matrix;

		//zkc 2003.10.21 begin appending
		//在主界面添加残差输出
	if((m_nModel==1&& m_bResidueAR)||(m_nModel==2&&m_bResidueMA)||(m_nModel==3&&m_bResidue))
	{
	
		CTString strFieldName="残差";
		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		while(IsExisted(m_pDataInterface,strFieldName))
		{
			strFieldName+="_1";
		}
		pField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess .First();
		for(i = 0;i<e_Vector.vlen();i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			pField->SetAsDouble(e_Vector(i));
			m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}
		m_eVector=e_Vector;
	}

		//to end the appending.	
	
}
CDoubleMatrix CTimeSeries::GetArimaStatis(CDoubleVector beta_Vector,int df,int cons,
											 CDoubleMatrix covErr_Matrix,
											 CDoubleVector &stdError_Vector,
											 CDoubleVector &t_Vector,CDoubleVector &P_Vector)
{
	//求标准误
	CDoubleVector stdError1_Vector(covErr_Matrix.mrow());
	stdError_Vector=stdError1_Vector;
	int i=0;
	for(i=0;i<covErr_Matrix.mrow();i++)
		stdError_Vector(i)=sqrt(covErr_Matrix(i,i));
	t_Vector=beta_Vector/stdError_Vector;
	CDoubleVector t1_Vector=-1.0*(t_Vector.abs());
	P_Vector=2.0*tcdf(t1_Vector,df);
	//WriteData(t_Vector,"t_Vector");
	//WriteData(P_Vector,"P_Vector");
	//求相关阵
	int r=beta_Vector.vlen();
	CDoubleMatrix RR_Matrix(r,r);
	for(i=0;i<r;i++)
	{
		for(int j=0;j<r;j++)
			RR_Matrix(i,j)=covErr_Matrix(i,j)/sqrt(covErr_Matrix(i,i)*covErr_Matrix(j,j));
	}
	//WriteData(RR_Matrix,"RR_Marix");
	return RR_Matrix;
}



CDoubleVector CTimeSeries::GetARMAPreBeta(CDoubleVector X_Vector,int p,int d,int q,
										  int cons,int MaxIteration,int &NumCir,double &mu)
{
	CDoubleVector W_Vector;
	if(cons==1)
	{
		mu=mean(X_Vector);
		W_Vector=X_Vector-mu;
	}
	else
	{
		mu=0;
		W_Vector=copy(X_Vector);
	}
	int kappa=p+q+1;
	int n=W_Vector.vlen();
	CDoubleVector gamma_Vector(kappa+1);
	CDoubleVector rho_Vector(kappa+1);

//	//WriteData(W_Vector,"W_Vector");

	ac_pac(W_Vector,kappa,gamma_Vector,rho_Vector);

//	//WriteData(gamma_Vector,"gamm_Vector");

	int i=0;
	for(i=0;i<kappa+1;i++)
		rho_Vector(i)=gamma_Vector(i)/gamma_Vector(0);
	CDoubleVector NumIteration_Vector(2);
	CDoubleVector PHIEst_Vector,theta_Vector;
	CDoubleVector preBeta_Vector;

	//WriteData(rho_Vector,"rho_Vector");

	if(p>0)
	{
		NumIteration_Vector(0)=sigmaPhi(p,0,rho_Vector,W_Vector,kappa,MaxIteration,PHIEst_Vector,theta_Vector);
		if(m_bIsToEnd)
		{
			CDoubleVector vect;			
			return vect;
		}
		double ns=NumIteration_Vector(0);
		//WriteData(PHIEst_Vector,"PHIEst_Vector");
		//WriteData(theta_Vector,"theta_Vector");
		preBeta_Vector=copy(PHIEst_Vector);
	}
	if(q>0)
	{		
		NumIteration_Vector(1)=sigmaPhi(p,q,rho_Vector,W_Vector,kappa,MaxIteration,PHIEst_Vector,theta_Vector);
		if(m_bIsToEnd)
		{
			CDoubleVector vect;
			return vect;
		}

		int nLen=preBeta_Vector.vlen()+theta_Vector.vlen();
		CDoubleVector temp_Vector(nLen);
		int i=0;
		for(i=0;i<preBeta_Vector.vlen();i++)
			temp_Vector(i)=preBeta_Vector(i);
		for(i=preBeta_Vector.vlen();i<nLen;i++)
			temp_Vector(i)=theta_Vector(i-preBeta_Vector.vlen());
		preBeta_Vector=temp_Vector;
		//WriteData(preBeta_Vector,"preBeta_Vector");
	}
	NumCir=NumIteration_Vector(0)>NumIteration_Vector(1)?NumIteration_Vector(0):NumIteration_Vector(1);
	//WriteData(preBeta_Vector,"preBeta_Vector");
	return preBeta_Vector;
}


int CTimeSeries::sigmaPhi(int p,int q,CDoubleVector rho_Vector,
						 CDoubleVector W_Vector,int kappa,
						 int MaxIteration,
						 CDoubleVector &PHIEst_Vector,
						 CDoubleVector &theta_Vector)
{
	int n_Size=W_Vector.vlen();	
	//估计rhoEst(P*P)
	CDoubleMatrix rhoEst_Matrix(p,p);//构造rhoEst,并赋值均为0；
	//p=6;
	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			if(i==j)//对角线元素赋值。
			{
				rhoEst_Matrix(i,j)=rho_Vector[q];
			}
			if(i<j)  //上三角元素赋值。
			{
				if((q+i-j)<0)
					rhoEst_Matrix(i,j)=rho_Vector[-q-i+j]; //下标是从0开始，所以在Matlab源程序的下标基础上再减1。
				else if((q+i-j)==0)
					rhoEst_Matrix(i,j)=1;
				else
					rhoEst_Matrix(i,j)=rho_Vector[q+i-j];
			}
			if(i>j)  //下三角元素赋值
			{
				rhoEst_Matrix(i,j)=rho_Vector[q+i-j];//
			}
		}
	}
	//WriteData(rho_Vector,"rho_RowVector");
	//WriteData(rhoEst_Matrix,"rhoEst_Matrix");
	
	
	//计算列向量rhoEstCol(p*1)
	 CDoubleVector rhoEstCol_Vector(p);
	 for(i=0;i<p;i++)
	 {
		rhoEstCol_Vector[i]=rho_Vector[q+i+1];////////////////
	 }
	 //WriteData(rhoEstCol_Vector,"rhoEstCol_Vector");

	 //计算PHIEst(p*1)=rhoEst_Matrix'*rho_RowVector
	 CDoubleVector PHIEst1_Vector(p);
	 PHIEst_Vector=PHIEst1_Vector;
	 if(p>0)
	 {
		 if(fabs(det(rhoEst_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			return -1;
		}

		 CDoubleMatrix inv_rhoEstMatrix=inv(rhoEst_Matrix);
		 PHIEst_Vector=inv_rhoEstMatrix*rhoEstCol_Vector;
	 }
	 //计算差分序列滑动平均,得WApo

   //  //WriteData(PHIEst_Vector,"PHIEst_Vector");

	 CDoubleVector WApo_Vector(n_Size);
	 int t=0;
	 for(t=0;t<n_Size;t++)
	 {
		 WApo_Vector[t]=W_Vector[t];
		 if(t>0)
		 {
			 for(i=0;i<p;i++)
			 {
				 if((t-i)==0)
				 {
					 break;
				 }
				 WApo_Vector[t]=WApo_Vector[t]-PHIEst_Vector[i]*W_Vector[t-i-1];//下标是从0开始，所以在Matlab源程序的下标基础上再减1。
			 }
		 }
	 }
	 //WriteData(WApo_Vector,"WApo_Vector");


	
	 //计算WApo的自协方差gammaWApo
//	WApo_Vector=WApo_Vector
	CDoubleVector gammaWApo_RowVector(kappa+1);
	CDoubleVector rhoWApo_RowVector(kappa+1);
	ac_pac(WApo_Vector,kappa,gammaWApo_RowVector,rhoWApo_RowVector);
	//WriteData(gammaWApo_RowVector,"gammaWApo_RowVector");


	//迭代求解
	double sigmaSquE=0.0;
	if(q==0)
		sigmaSquE=gammaWApo_RowVector[0];
	CDoubleVector theta1_Vector(q);
	theta_Vector=theta1_Vector;
	CDoubleVector theta2_Vector(q);
	int cir=0;
	if(q>0)
	{
		double deltaTheta1=2.0;
		double deltaTheta2=1.0;
		while(deltaTheta1>deltaTheta2&&cir<MaxIteration)
		{	
			deltaTheta1=deltaTheta2;
			sigmaSquE=1.0;		
			for(i=0;i<q;i++)
			{
				sigmaSquE=sigmaSquE+theta_Vector[i]*theta_Vector[i];
				
			}
		
			sigmaSquE=gammaWApo_RowVector[0]/sigmaSquE;
			
			int j=0;
			for(j=0;j<q;j++) //kk替代Matlab源程序中的k
			{
				theta2_Vector[j]=-gammaWApo_RowVector[j+1]/sigmaSquE;
				
				if(j<(q-1))
				{
					for(i=0;i<q-j-1;i++)
					{
						theta2_Vector[j]=theta2_Vector[j]+theta_Vector[i]*theta_Vector[j+i+1];
					
					}
				}
			}
			CDoubleVector Temp_Vector=theta2_Vector-theta_Vector;
			////WriteData(Temp_Vector,"Temp_Vector");
			deltaTheta2=Max(abs(Temp_Vector));
			theta_Vector=copy(theta2_Vector);
			cir=cir+1;
		}
	}
	//WriteData(theta_Vector,"theta_Vector");
	return cir;
}

CDoubleVector CTimeSeries::GetArmaPredict(CDoubleVector XOrg_Vector,
										 CDoubleVector Beta_Vector,
										 int p,int q,int L,int cons)
{
	int N=XOrg_Vector.vlen();
	int c=1;
	double mu=0;
	CDoubleVector X_Vector=copy(XOrg_Vector);
	CDoubleVector betaR_Vector=copy(Beta_Vector);
	if(cons==1)
	{
		mu=Beta_Vector(0);
		CDoubleVector Beta1_Vector(Beta_Vector.vlen()-1);
		for(int i=0;i<Beta1_Vector.vlen();i++)
			Beta1_Vector(i)=Beta_Vector(i+1);
		Beta_Vector=Beta1_Vector;
		X_Vector=X_Vector-mu;
	}
	//WriteData(Beta_Vector,"Beta_Vector");
	CDoubleVector Z_Vector(N+L);
	if(p>0&&q==0)
	{
		int i=0;
		for(i=0;i<N;i++)
			Z_Vector(i)=X_Vector(i);
		for(i=N;i<N+L;i++)
		{
			int j=0;
			for(j=0;j<p;j++)
			{
				Z_Vector(i)=Z_Vector(i)+Beta_Vector(j)*Z_Vector(i-j-1);
			}
		}
	}
	else
	{
		CDoubleVector phi_Vector(p);
		int i=0;
		for(i=0;i<phi_Vector.vlen();i++)
			phi_Vector(i)=Beta_Vector(i);
		CDoubleVector theta_Vector(q);
		for(i=0;i<q;i++)
		{
			theta_Vector(i)=Beta_Vector(p+i);
		}
		CDoubleVector ep_Vector(N);
		double ZA=mean(X_Vector);
		//WriteData(ep_Vector,"ep_Vector");
		//WriteData(phi_Vector,"phi_Vector");
		//WriteData(theta_Vector,"theta_Vector");

		for(i=0;i<N;i++)
		{
		//	//WriteData(X_Vector,"X_Vector");
			double tmpone=0,tmptwo=0,tmpthr=0;
			int loop=0;
			if(i+1<=p)
				loop=i+1-1;
			else
				loop=p;

			int j=0;
			for(j=0;j<loop;j++)
			{
				tmpone=tmpone+phi_Vector(j)*X_Vector(i-j-1);
			}
			if(i+1<=q)
				loop=i+1-1;
			else
				loop=q;
			for(j=0;j<loop;j++)
				tmptwo=tmptwo+theta_Vector(j)*ep_Vector(i-j-1);
			for(j=p-1;j>=i;j--)
				tmpthr=tmpthr+phi_Vector(j);
			ep_Vector(i)=X_Vector(i)-tmpone+tmptwo-tmpthr*ZA;
		}
		//WriteData(ep_Vector,"ep_Vector");
		//WriteData(phi_Vector,"phi_Vector");
		//WriteData(theta_Vector,"theta_Vector");
		///求预报
		for(i=0;i<X_Vector.vlen();i++)
		{
			Z_Vector(i)=X_Vector(i);
		}
		for(i=N;i<N+L;i++)
			Z_Vector(i)=0;
		//WriteData(Z_Vector,"Z_Vector");
		for(i=0;i<L;i++)
		{
			double tmpone=0;
			double tmptwo=0;
			int j=0;
			for(j=0;j<p;j++)
				tmpone=tmpone+phi_Vector(j)*Z_Vector(N+i-j-1);
			if(i+1>q)
				tmptwo=0;
			else
			{
				for(j=i;j<q;j++)
					tmptwo=tmptwo+theta_Vector(j)*ep_Vector(N+i-j-1);
			}
			Z_Vector(N+i)=tmpone-tmptwo;
		}
	}
	CDoubleVector temp_Vector(L);
	int i=0;
	for(i=0;i<L;i++)
		temp_Vector(i)=Z_Vector(N+i);	
	if(cons==1)
	{
		temp_Vector=temp_Vector+mu;
	}
	//WriteData(temp_Vector,"temp_Vector");
	Beta_Vector=copy(betaR_Vector);//恢复
	return temp_Vector;
}
bool CTimeSeries::IsExisted(CDataInterface *p,CTString str)
{
	for(int i=0;i<p->GetColCount();i++)
	{
		if(p->GetFieldName(i)==str)
		{
			return true;
		}
	}
	return false;
}
void CTimeSeries::GetARStatis(CDoubleVector X_Vector,
							 CDoubleVector beta_Vector,int cons,
							 CDoubleMatrix covErr_Matrix,
							 double &SSQ,double &ResVar,double &AIC,
							 CDoubleMatrix &S_Matrix,
							 CDoubleMatrix &RR_Matrix,
							 CDoubleVector &stdError_Vector,
							 CDoubleVector &t_Vector,CDoubleVector &P_Vector,double &StdE,int &df)
{
	if(cons==0)
	{
		int row=X_Vector.vlen();
		int p=beta_Vector.vlen();
		CDoubleVector e_Vector(row);
		int i=p;
		for(i=p;i<row;i++)
		{
			e_Vector(i)=X_Vector(i);
			for(int j=0;j<p;j++)
			{
				e_Vector(i)=e_Vector(i)-beta_Vector(j)*X_Vector(i-j-1);
			}
		}

		//求残差平方和
		SSQ=dot(e_Vector,e_Vector);
		df=row-p;
		ResVar=SSQ/df;
		
		//zkc 2003.10.21 begin appending
		//在主界面添加残差输出
		if((m_nModel==1&& m_bResidueAR)||(m_nModel==2&&m_bResidueMA)||(m_nModel==3&&m_bResidue))
		{
			
			CTString strFieldName="残差";
			CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			while(IsExisted(m_pDataInterface,strFieldName))
			{
				strFieldName+="_1";
			}
			pField->SetFieldName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
			
			int nRecNo = 1;
			m_pDataInterface->m_DataAccess .First();
			for(i = 0;i<e_Vector.vlen();i++)
			{
				m_pDataInterface->m_DataAccess.Edit();			
				pField->SetAsDouble(e_Vector(i));
				m_pDataInterface->m_DataAccess.Post();			
				m_pDataInterface->m_DataAccess.Next();
			}
			//to end the appending.	
			m_eVector=e_Vector;

		}

		//求残方差
		StdE=sqrt(ResVar);

		//求标准误
//		AIC=row*log(ResVar)/log(10)+2*p;

		//求协方差阵
		CDoubleMatrix A_Matrix(row-p,p);
		CDoubleVector alpha_Vector(row-p);
		for(i=0;i<row-p;i++)
		{
			alpha_Vector(i)=X_Vector(p+i);
			for(int j=0;j<p;j++)
				A_Matrix(i,j)=X_Vector(i+p-j-1);
		}
		CDoubleMatrix temp_Matrix=trans(A_Matrix)*A_Matrix;
		if(fabs(det(temp_Matrix))<BOUNDARY)
		{	
			CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
			str+="做时间序列分析时，模型设置不合理！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			m_bIsToEnd=true;
			return ;
		}

		S_Matrix=ResVar*inv(temp_Matrix);
		TraceMatrix(S_Matrix);
		CDoubleVector stdError1_Vector(S_Matrix.mrow());
		stdError_Vector=stdError1_Vector;
		for(i=0;i<S_Matrix.mrow();i++)
		{
			stdError_Vector(i)=sqrt(S_Matrix(i,i));
		}
		t_Vector=beta_Vector/stdError_Vector;
		CDoubleVector t1_Vector=-1.0*(t_Vector.abs());
		//WriteData(t1_Vector,"t1_Vector");
		P_Vector=2.0*tcdf(t1_Vector,df);

		CDoubleMatrix RR1_Matrix(p,p);	
		RR_Matrix=RR1_Matrix;
		for(i=0;i<p;i++)
		{	for(int j=0;j<p;j++)
				RR_Matrix(i,j)=S_Matrix(i,j)/sqrt(S_Matrix(i,i)*S_Matrix(j,j));
		}
		AIC=row*log(ResVar)/log(10)+2*(p+1);
	
	}
	else
	{
		int row=X_Vector.vlen();
		int p=beta_Vector.vlen()-1;
		CDoubleVector e_Vector(row);
		CDoubleVector X1_Vector=copy(X_Vector);
		//X1_Vector=X1_Vector-beta_Vector(0);
		CDoubleVector tmpBeta_Vector(beta_Vector.vlen()-1);
		int i=0;
		for(i=0;i<tmpBeta_Vector.vlen();i++)
			tmpBeta_Vector(i)=beta_Vector(i+1);
		double dConst =  beta_Vector(0);
		for(i=p;i<row;i++)
		{
			e_Vector(i)=X1_Vector(i)-dConst;
			for(int j=0;j<p;j++)
			{
				e_Vector(i)=e_Vector(i)-tmpBeta_Vector(j)*X1_Vector(i-j-1);
			}
		}
		//求均值项
		double Mu = dConst/(1-tmpBeta_Vector.sum());
		beta_Vector(0) = Mu;
		//求残差平方和

		SSQ=dot(e_Vector,e_Vector);
		df=row-p-1;
		ResVar=SSQ/df;
		
		//zkc 2003.10.21 begin appending

		
		//在主界面添加残差输出

		if((m_nModel==1&& m_bResidueAR)||(m_nModel==2&&m_bResidueMA)||(m_nModel==3&&m_bResidue))
		{
			
			CTString strFieldName="残差";
			CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			while(IsExisted(m_pDataInterface,strFieldName))
			{
				strFieldName+="_1";
			}
			pField->SetFieldName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
			
			int nRecNo = 1;
			m_pDataInterface->m_DataAccess .First();
			for(i = 0;i<e_Vector.vlen();i++)
			{
				m_pDataInterface->m_DataAccess.Edit();			
				pField->SetAsDouble(e_Vector(i));
				m_pDataInterface->m_DataAccess.Post();			
				m_pDataInterface->m_DataAccess.Next();
			}	
			m_eVector=e_Vector;
		}
		//to end the appending.	
	
		
		
		//求残方差
		StdE=sqrt(ResVar);

		//求标准误
		AIC=row*log(ResVar)+2*(p+2);

		//求协方差阵
	
		S_Matrix=ResVar*covErr_Matrix;
		TraceMatrix(S_Matrix);
			TraceMatrix(covErr_Matrix);
		CDoubleVector stdError1_Vector(S_Matrix.mrow());
		stdError_Vector=stdError1_Vector;
		for(i=0;i<S_Matrix.mrow();i++)
		{
			stdError_Vector(i)=sqrt(S_Matrix(i,i));
		}
		//stdError_Vector(0) = stdError_Vector(0)/sqrt(df);
		t_Vector=beta_Vector/stdError_Vector;
		CDoubleVector t1_Vector=-1.0*(t_Vector.abs());
		P_Vector=2.0*tcdf(t1_Vector,df);
		CDoubleMatrix RR1_Matrix(p+1,p+1);
		RR_Matrix=RR1_Matrix;
		for(i=0;i<p+1;i++)
		{	for(int j=0;j<p+1;j++)
				RR_Matrix(i,j)=S_Matrix(i,j)/sqrt(S_Matrix(i,i)*S_Matrix(j,j));
		}
		
	//	AIC=row*log(ResVar)/log(10)+2*(p+1);
	}
	//WriteData(S_Matrix,"S_Matrix");
	//WriteData(RR_Matrix,"RR_Matrix");
	//WriteData(stdError_Vector,"stdError_Vector");
	//WriteData(t_Vector,"t_Vector");
	//WriteData(P_Vector,"P_Vector");
}


CDoubleVector CTimeSeries::GetAutoregression(CDoubleVector XOrg_Vector,int p,int cons)
{
	int r=XOrg_Vector.vlen();
	int c=1;
	CDoubleVector X_Vector=copy(XOrg_Vector);
	double Xmean=0;
	if(cons==1)
	{
		Xmean=mean(X_Vector);
		X_Vector=X_Vector-Xmean;
	}
	CDoubleMatrix A_Matrix(r-p,p);
	CDoubleVector alpha_Vector(r-p);
	int i=0;
	for(i=0;i<r-p;i++)
	{
		alpha_Vector(i)=X_Vector(p+i);
		for(int j=0;j<p;j++)
		{
			A_Matrix(i,j)=X_Vector(i+p-j-1);
		}
	}
	CDoubleMatrix AA_Matrix=trans(A_Matrix)*A_Matrix;
	CDoubleVector Aalpha_Vector=trans(A_Matrix)*alpha_Vector;
	if(fabs(det(AA_Matrix))<BOUNDARY)
	{	
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="做时间序列分析时，模型设置不合理！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		m_bIsToEnd=true;
		CDoubleVector vect;		
		return vect;
	}

	CDoubleVector phi_Vector=inv(AA_Matrix)*Aalpha_Vector;
	if(cons==1)
	{
		CDoubleVector temp_Vector(phi_Vector.vlen()+1);
		for(int i=0;i<temp_Vector.vlen();i++)
		{
			if(i==0)
				temp_Vector(i)=Xmean;
			else
				temp_Vector(i)=phi_Vector(i-1);
		}
	//	WriteData(temp_Vector,"temp_Vector");
		return temp_Vector;
	}
//	WriteData(phi_Vector,"phi_Vector");
	return phi_Vector;

}

CDoubleVector CTimeSeries::GetAutoregression2(CDoubleVector XOrg_Vector,int p,int cons)
{
	int r=XOrg_Vector.vlen();
	int c=1;
	CDoubleVector X_Vector=copy(XOrg_Vector);
	double Xmean=0;
	if(cons==1)
	{
		Xmean=mean(X_Vector);
		X_Vector=X_Vector-Xmean;
	}
	CDoubleMatrix A_Matrix(r-p,p);
	CDoubleVector alpha_Vector(r-p);
	int i=0;
	for(i=0;i<r-p;i++)
	{
		alpha_Vector(i)=X_Vector(p+i);
		for(int j=0;j<p;j++)
		{
			A_Matrix(i,j)=X_Vector(i+p-j-1);
		}
	}
	CDoubleMatrix AA_Matrix=trans(A_Matrix)*A_Matrix;
	CDoubleVector Aalpha_Vector=trans(A_Matrix)*alpha_Vector;
	if(fabs(det(AA_Matrix))<BOUNDARY)
	{	
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="做时间序列分析时，模型设置不合理！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		m_bIsToEnd=true;
		CDoubleVector vect;		
		return vect;
	}
	
	CDoubleVector phi_Vector=inv(AA_Matrix)*Aalpha_Vector;
	if(cons==1)
	{
		CDoubleVector temp_Vector(phi_Vector.vlen()+1);
		for(int i=0;i<temp_Vector.vlen();i++)
		{
			if(i==0)
				temp_Vector(i)=Xmean;
			else
				temp_Vector(i)=phi_Vector(i-1);
		}
		//	WriteData(temp_Vector,"temp_Vector");
		return temp_Vector;
	}
	//	WriteData(phi_Vector,"phi_Vector");
	return phi_Vector;
	
}


double CTimeSeries::GetCni(int n,int i)
{
	if(i>n||n<0||i<0)
		return 0;
	double flag=i%2==0?1:-1;
	return flag*factorial(n)/factorial(n-i)/factorial(i);
}

CDoubleVector CTimeSeries::GetDiffPredict(CDoubleVector X_Vector,CDoubleVector Z_Vector,int d)
{
	int Num=X_Vector.vlen();
	int NumPre=Z_Vector.vlen();
	CDoubleVector Y_Vector(Num+NumPre);
	int i=0;
	for(i=0;i<Num;i++)
		Y_Vector(i)=X_Vector(i);
	for(i=0;i<NumPre;i++)
		Y_Vector(Num+i)=Z_Vector(i);
	//WriteData(Y_Vector,"Y_Vector");
	for(i=Num;i<Num+NumPre;i++)
	{
		for(int j=0;j<d;j++)
		{
			double temp=GetCni(d,j+1);
			Y_Vector(i)=Y_Vector(i)-temp*Y_Vector(i-j-1);
		//	double temp2=Y_Vector(i);
		}
	}
	CDoubleVector temp_Vector(NumPre);
	for(i=0;i<NumPre;i++)
		temp_Vector(i)=Y_Vector(Num+i);
	//WriteData(temp_Vector,"temp_Vector");
	return temp_Vector;
}

CDoubleMatrix CTimeSeries::GetFF(CDoubleVector w_Vector,
								CDoubleVector I_Vector,
								CDoubleMatrix II_Matrix,int p,int q,
								int Num,int cons)
{
	int row=w_Vector.vlen();
	int pq=p+q;
	if(cons==0)
	{
		CDoubleMatrix FF_Matrix(row-p,pq);
		for(int i=p;i<row;i++)
			for(int j=0;j<pq;j++)
				for(int k=0;k<Num;k++)
					if(i-k>0)
						FF_Matrix(i-p,j)=FF_Matrix(i-p,j)+II_Matrix(k,j)*w_Vector(i-k-1);
		return FF_Matrix;
	}
	else
	{
		CDoubleMatrix FF_Matrix(row-p,pq+1);
		for(int i=p;i<row;i++)
		{
			FF_Matrix(i-p,0)=1;
			int k;
			for(k=0;k<Num;k++)
			{
				if(i-k>0)
					FF_Matrix(i-p,0)=FF_Matrix(i-p,0)-I_Vector(k);
			}
			for(int j=0;j<pq;j++)
			{
				for(k=0;k<Num;k++)
					if(i-k>0)
						FF_Matrix(i-p,j+1)=FF_Matrix(i-p,j+1)+II_Matrix(k,j)*w_Vector(i-k-1);
			}
		}
		return FF_Matrix;
	}
}

CDoubleVector CTimeSeries::GetI(CDoubleVector w_Vector,
							   CDoubleVector beta_Vector,int p,int q,int Num,
							   CDoubleVector &I_Vector)
{
	CDoubleVector I1_Vector(Num);
	I_Vector=copy(I1_Vector);
	int i=0;
	for(i=0;i<Num;i++)
	{
		if(i+1<=p)
			I_Vector(i)=beta_Vector(i);
		else
			I_Vector(i)=0;
		int k=0;
		if(i+1<=q)
		{
			I_Vector(i)=I_Vector(i)-beta_Vector(p+i);
			k=i-1+1;
		}
		else
		{
			k=q;
		}
		if(i+1>1)
		{
			for(int j=0;j<k;j++)
				I_Vector(i)=I_Vector(i)+beta_Vector(p+j)*I_Vector(i-j-1);
		}
	}
	int row=w_Vector.vlen();
	CDoubleVector F_Vector(row-p);
	for(i=p;i<row;i++)
	{
		for(int j=0;j<Num;j++)
		{
			if(i-j>0)
				F_Vector(i-p)=F_Vector(i-p)+I_Vector(j)*w_Vector(i-j-1);
		}
	}
	return F_Vector;
}


CDoubleMatrix CTimeSeries::GetII(CDoubleVector I_Vector,CDoubleVector beta_Vector,
								int p,int q,int Num)
{
	int pq=p+q;
	CDoubleMatrix II_Matrix(Num,pq);
	int i=0;
	for(i=0;i<Num;i++)
	{
		for(int s=0;s<pq;s++)
		{
			if(i+1<=p)
				II_Matrix(i,s)=delta(i,s);
			else
				II_Matrix(i,s)=0;
		
			int k=0;
			if(i+1<=q)
			{
				II_Matrix(i,s)=II_Matrix(i,s)-delta(p+i,s);
				k=i-1+1;
			}
			else
				k=q;
			for(int j=0;j<k;j++)
			{
				if(i>j)
					II_Matrix(i,s)=II_Matrix(i,s)+delta(p+j,s)*I_Vector(i-j-1)+beta_Vector(p+j)*II_Matrix(i-j-1,s);
			}
		}
	}
	return II_Matrix;

}
/*
CDoubleMatrix GetIKS(CDoubleVector X_Vector,CDoubleVector Beta_Vector,
					 CDoubleVector I_Vector,CDoubleMatrix II_Matrix,
					 int p,int q,int Num,CDoubleVector e_Vector)
{
	CPtrArray III_Matrix;
	int pq=p+q;
	int r=X_Vector.vlen();
	int c=1;
	for(int i=0;i<Num;i++)
	{
		CDoubleMatrix *pMatrix=new CDoubleMatrix(pq,pq);
		III_Matrix.Add((void *)pMatrix);
	}

	for(i=0;i<Num;i++)
	{
		CDoubleMatrix *pMatrix=(CDoubleMatrix *)(III_Matrix.GetAt(i));
		for(int k=0;k<pq;k++)
		{
			for(int s=0;s<pq;s++)
			{
				int m=0;
				if(i+1<=q)
				{
					m=i-1+1;
				}
				else
					m=q;
				for(int j=0;j<m;j++)
				{
					if(i>j)
					{
						CDoubleMatrix *p2Matrix=(CDoubleMatrix *)(III_Matrix.GetAt(i-j));

						(*pMatrix)(k,s)=(*pMatrix)(k,s)+delta(p+j,k)*II_Matrix(i-j-1,s)+delta(p+j,s)*II_Matrix(i-j-1,k)
										+Beta_Vector(p+j)*((*p2Matrix)(k,s));
					}
				}
			}
		}
	}
	CDoubleVector S_Vector(pq);
	for(i=0;i<pq;i++)
	{
		for(int t=p;t<r;t++)
		{
			double temp=0;
			for(int j=0;j<Num;j++)
			{
				if(t-j>0)
					temp=temp-II_Matrix(j,i)*X_Vector(t-j-1);
			}
			S_Vector(i)=S_Vector(i)+2*e_Vector(t-p)*temp;
		}
	}
	CDoubleMatrix Hessian_Matrix(pq,pq);
	for(int k=0;k<pq;k++)
	{
		for(int s=0;s<pq;s++)
		{
			for(int t=p;t<r;t++)
			{
				double temp1=0;
				double temp2=0;
				double temp3=0;
				for(int j=0;j<Num;j++)
				{
					if(t-j>0)
					{		
						CDoubleMatrix *pMatrix=(CDoubleMatrix *)(III_Matrix.GetAt(j));
						temp1=temp1+II_Matrix(j,s)*X_Vector(t-j-1);
						temp2=temp2+II_Matrix(j,k)*X_Vector(t-j-1);
						temp3=temp3+X_Vector(t-j-1)*((*pMatrix)(k,s));
					}
				}
				Hessian_Matrix(k,s)=Hessian_Matrix(k,s)+2*(temp1*temp2-e_Vector(t-p-1)*temp3);
			}
		}
	}
	return Hessian_Matrix;

}
*/

bool CTimeSeries::Arima(CDoubleVector X_Vector,int p,int q,int d,int cons,
					   int L,double pEps,double sEps,int MaxIteration,
					   int MaxIter)
{

	//求差分
	CTArray<double,double> DiffArray;
	int nLen=X_Vector.vlen();
	if(nLen<=p+q+d)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="的样本数量应大于自回归、差分与移动平均之和！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}
	int i=0;
	for(i=0;i<nLen;i++)
	{
		DiffArray.Add(X_Vector(i));
	}
	
	for(i=0;i<d;i++)
	{
		for(int j=0;j<DiffArray.GetSize()-1;j++)
		{
			DiffArray[j]=DiffArray[j+1]-DiffArray[j];
		}
		DiffArray.RemoveAt(DiffArray.GetSize()-1);
	}
	
	int nSize=DiffArray.GetSize();
	CDoubleVector w_Vector(nSize);
	for(i=0;i<nSize;i++)
		w_Vector(i)=DiffArray[i];
	double average=mean(w_Vector);
	double sum=0;
	for(i=0;i<nSize;i++)
	{
		sum=(w_Vector(i)-average)*(w_Vector(i)-average);
	}
	if(sum<1e-12)
	{
		CTString str=m_pDataInterface->GetFieldName(m_nCurVarNum);
		str+="做时间序列分析时，模型设置不合理！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}

	//WriteData(w_Vector,"w_Vector");
	if(p>0&&q==0)//简单的AR(p)
	{
		int NumIteration =0;
		double mu =0;
		CDoubleVector beta_Vector(p+(cons==1));//=GetAutoregression(w_Vector,p,cons);
	//	TraceVector(beta_Vector);
		CDoubleVector preBeta_Vector=GetARMAPreBeta(w_Vector,p,d,q,cons,MaxIter,NumIteration,mu);
		TraceVector(preBeta_Vector);
		if(m_bIsToEnd)
			return false;
		CDoubleMatrix covErr_Matrix(1,1);
		double SSQ=0,ResVar=0,AIC=0,P=0;
		CDoubleMatrix S_Matrix;
		CDoubleVector stdError_Vector,t_Vector;
		CDoubleMatrix RR_Matrix;
		CDoubleVector P_Vector;
		double StdE=0;
		int df=0;

		if(cons==1)
		{	
			beta_Vector[0]=average;
			for(int i=0;i<p;i++)
				beta_Vector[i+1]=preBeta_Vector[i];
			TraceVector(beta_Vector);
			beta_Vector=GetARBeta2(w_Vector,beta_Vector,p,cons,pEps,sEps,MaxIteration,covErr_Matrix);
			TraceVector(beta_Vector);
			if(m_bIsToEnd)
				return false;
		}
		else
		{
			for(int i=0;i<p;i++)
				beta_Vector[i]=preBeta_Vector[i];
		}	
		GetARStatis(w_Vector,beta_Vector,cons,covErr_Matrix,
			SSQ,ResVar,AIC,S_Matrix,RR_Matrix,stdError_Vector,t_Vector,P_Vector,StdE,df);
			if(m_bIsToEnd)
				return false;
	TraceMatrix(covErr_Matrix);
		CDoubleVector Z_Vector=GetArmaPredict(w_Vector,beta_Vector,p,q,L,cons);
		//WriteData(Z_Vector,"Z_Vector");
		if(d>0)
			Z_Vector=GetDiffPredict(X_Vector,Z_Vector,d);
		
		//WriteData(Z_Vector,"Z_Vector");		
		CDoubleVector pStdE_Vector,lBound_Vector,uBound_Vector;
		if(L>0)
			GetBelieveDegree(Z_Vector,StdE,beta_Vector,p,d,q,cons,pStdE_Vector,lBound_Vector,uBound_Vector);
		CreateResult(SSQ,ResVar,StdE,df,AIC,beta_Vector,stdError_Vector,t_Vector,P_Vector,
					S_Matrix,RR_Matrix,Z_Vector,pStdE_Vector,lBound_Vector,uBound_Vector,L,p,X_Vector);
	}
	else
	{
		double mu=0;
		int NumCir=0;
		CDoubleVector beta_Vector;
		CDoubleMatrix covErr_Martrix;
		double SSQ=0,ResVar=0,StdE=0,AIC=0;
		int df=0;
		int NumIteration=0;
		CDoubleVector preBeta_Vector=GetARMAPreBeta(w_Vector,p,d,q,cons,MaxIter,NumIteration,mu);
		if(m_bIsToEnd)
			return false;
		if(cons==1)
		{
			CDoubleVector temp_Vector(1+preBeta_Vector.vlen());
			temp_Vector(0)=mu;
			for(int i=0;i<preBeta_Vector.vlen();i++)
				temp_Vector(i+1)=preBeta_Vector(i);
			preBeta_Vector=copy(temp_Vector);
		}
		CDoubleMatrix covErr_Matrix;
		GetArimaBeta(w_Vector,preBeta_Vector,p,q,cons,pEps,sEps,MaxIteration,
			SSQ,ResVar,StdE,df,AIC,beta_Vector,covErr_Matrix);
		TraceMatrix(covErr_Matrix);
		if(m_bIsToEnd)
			return false;
		CDoubleVector t_Vector,stdError_Vector;
		CDoubleVector P_Vector;
		TraceVector(beta_Vector);
		CDoubleMatrix RR_Matrix=GetArimaStatis(beta_Vector,df,cons,covErr_Matrix, stdError_Vector,t_Vector,P_Vector);
		CDoubleVector Z_Vector=GetArmaPredict(w_Vector,beta_Vector,p,q,L,cons);
		//WriteData(stdError_Vector,"stdError_Vector");
		//WriteData(Z_Vector,"Z_Vector");
		if(d>0)
			Z_Vector=GetDiffPredict(X_Vector,Z_Vector,d);
		CDoubleVector pStdE_Vector,lBound_Vector,uBound_Vector;
		if(L>0)
			GetBelieveDegree(Z_Vector,StdE,beta_Vector,p,d,q,cons,pStdE_Vector,lBound_Vector,uBound_Vector);
		CreateResult(SSQ,ResVar,StdE,df,AIC,beta_Vector,stdError_Vector,t_Vector,P_Vector,
					covErr_Matrix,RR_Matrix,Z_Vector,pStdE_Vector,lBound_Vector,uBound_Vector,L,p,X_Vector);
	
	}
	return true;
}

CDoubleVector CTimeSeries::GetPhi(CDoubleVector beta_Vector,int p,int d)
{
	if(d==0)
	{
		CDoubleVector phi_Vector=copy(beta_Vector);
		return phi_Vector;
	}
	CDoubleVector phi_Vector(p+d);
	for(int i=0;i<p+d;i++)
	{
		if(i+1<=d)
			phi_Vector(i)=GetCni(d,i+1);
		if(i+1<=p)
			phi_Vector(i)=phi_Vector(i)-beta_Vector(i);
		for(int j=0;j<i;j++)
		{
			if(i-j<=d&&j+1<=p)
				phi_Vector(i)=phi_Vector(i)-GetCni(d,i-j)*beta_Vector(j);
		}
	}
	return phi_Vector;

}


CDoubleVector CTimeSeries::GetPsi(CDoubleVector beta_Vector,int p,int d,int q,int L,int cons)
{
	if(L==1)
	{	
		CDoubleVector Psi_Vector(1);
		return Psi_Vector;
	}
	CDoubleVector phi_Vector(1);
	CDoubleVector theta_Vector(1);
	if(cons==1)
	{
		if(p>0)
		{
			CDoubleVector tempPhi_Vector(p);
			for(int i=0;i<p;i++)
				tempPhi_Vector(i)=beta_Vector(i+1);
			phi_Vector=tempPhi_Vector;
		}
		if(q>0)
		{
			CDoubleVector tempTheta_Vector(q);
			for(int i=0;i<q;i++)
				tempTheta_Vector(i)=beta_Vector(p+1+i);
			theta_Vector=tempTheta_Vector;
		}
	}
	else
	{
		if(p>0)
		{
			CDoubleVector tempPhi_Vector(p);
			for(int i=0;i<p;i++)
				tempPhi_Vector(i)=beta_Vector(i);
			phi_Vector=tempPhi_Vector;
		}
		if(q>0)
		{
			CDoubleVector tempTheta_Vector(q);
			for(int i=0;i<q;i++)
				tempTheta_Vector(i)=beta_Vector(p+i);
			theta_Vector=tempTheta_Vector;
		}
	}
	CDoubleVector phiExt_Vector;
	d=0;
	if(d>0)
		phiExt_Vector=GetPhi(phi_Vector,p,d);
	else
		phiExt_Vector=phi_Vector;
	CDoubleVector Psi_Vector(L-1);
	for(int i=0;i<L-1;i++)
	{
		if(i+1<=p+d)
			Psi_Vector(i)=phiExt_Vector(i);
		if(i+1<=q)
			Psi_Vector(i)=Psi_Vector(i)-theta_Vector(i);
		for(int j=0;j<i;j++)
		{
			if(j+1<=p+d)
				Psi_Vector(i)=Psi_Vector(i)+phiExt_Vector(j)*Psi_Vector(i-j-1);
		}
	}
	return Psi_Vector;
}

CDoubleVector CTimeSeries::GetGGValue(CDoubleVector beta_Vector,int p,int q,int L,int cons)
{
	CDoubleVector GValue_Vector(L-1);
	CDoubleVector phi_Vector(1);
	CDoubleVector theta_Vector(1);
	if(cons==1)
	{
		if(p>0)
		{
			CDoubleVector tempPhi_Vector(p);
			for(int i=0;i<p;i++)
				tempPhi_Vector(i)=beta_Vector(1+i);
			phi_Vector=tempPhi_Vector;
			GValue_Vector(0)=phi_Vector(0);
		}
		if(q>0)
		{
			CDoubleVector tempTheta_Vector(q);
			for(int i=0;i<q;i++)
				tempTheta_Vector(i)=beta_Vector(p+1+i);
			theta_Vector=tempTheta_Vector;
			GValue_Vector(0)=GValue_Vector(0)+theta_Vector(0);
		}
	}
	else
	{
		if(p>0)
		{
			CDoubleVector tempPhi_Vector(p);
			for(int i=0;i<p;i++)
				tempPhi_Vector(i)=beta_Vector(i);
			phi_Vector=tempPhi_Vector;
			GValue_Vector(0)=phi_Vector(0);
		}
		if(q>0)
		{
			CDoubleVector tempTheta_Vector(q);
			for(int i=0;i<q;i++)
				tempTheta_Vector(i)=beta_Vector(p+i);
			theta_Vector=tempTheta_Vector;
			GValue_Vector(0)=GValue_Vector(0)+theta_Vector(0);
		}
	}
	if(p==0)
	{
		double Num=0;
		if(q<L)
			Num=q;
		else
			Num=L-1;
		for(int i=0;i<Num;i++)
			GValue_Vector(i)=theta_Vector(i);
		return GValue_Vector;
	}
	int i=1;
	for(i=1;i<L-1;i++)
	{
		if(i+1<=p)
			GValue_Vector(i)=phi_Vector(i);
		if(i+1<=q)
			GValue_Vector(i)=GValue_Vector(i)+theta_Vector(i);
		for(int j=0;j<i;j++)
		{
			if(j+1<=p)
				GValue_Vector(i)=GValue_Vector(i)+phi_Vector(j)*GValue_Vector(i-j-1);
		}
	}
	return GValue_Vector;
}


void CTimeSeries::GetBelieveDegree(CDoubleVector Z_Vector,double ResVar,
					  CDoubleVector beta_Vector,int p,int d,
					  int q,int cons,
					  CDoubleVector &pStdE_Vector,
					  CDoubleVector &lBound_Vector,
					  CDoubleVector &uBound_Vector)
{
	int Num=Z_Vector.vlen();
	CDoubleVector psi_Vector=GetPsi(beta_Vector,p,d,q,Num,cons);
	double PsiAdd=1;
	CDoubleVector GValue_Vector;
	if(Num>1)
	{
		GValue_Vector=GetGGValue(beta_Vector,p,q,Num,cons);
	}
	CDoubleVector lBound1_Vector(Num),uBound1_Vector(Num),pStdE1_Vector(Num);
	lBound_Vector=lBound1_Vector;
	uBound_Vector=uBound1_Vector;
	pStdE_Vector=pStdE1_Vector;
	for(int i=0;i<Num;i++)
	{
		lBound_Vector(i)=Z_Vector(i)-norminv(1-0.025,0,1)*sqrt(PsiAdd)*ResVar;
		uBound_Vector(i)=Z_Vector(i)+norminv(1-0.025,0,1)*sqrt(PsiAdd)*ResVar;
		if(i+1<Num)
			PsiAdd=PsiAdd+psi_Vector(i)*psi_Vector(i);
		double temp=1;
		for(int j=0;j<i;j++)
			temp=temp+GValue_Vector(j)*GValue_Vector(j);
		pStdE_Vector(i)=ResVar*sqrt(temp);
	}
}


void CTimeSeries::CreateResult(double SSQ,double ResVar,double StdE,int df,double AIC,
							   CDoubleVector beta_Vector,CDoubleVector StdError_Vector,
							   CDoubleVector t_Vector,CDoubleVector P_Vector,
							   CDoubleMatrix covErr_Matrix,CDoubleMatrix RR_Matrix,
							   CDoubleVector Z_Vector,CDoubleVector pStdE_Vector,
							   CDoubleVector lBound_Vector,CDoubleVector uBound_Vector,
							   int nForcast,int nAutoReg,CDoubleVector X_Vector)
{

	/////////////////////////输出参数/////////////////////////
	CTString strName=m_pDataInterface->GetFieldName(m_nCurVarNum);
	CRsltVariable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	CTString strTitle = _T("");
	
	pTable->SetTitle("最终参数");
	pTable->SetHead(strName);

	pTable->InsertColumn(0,"参数名");
	pTable->InsertColumn(1,"数值");
	
	pTable->InsertItem(0,"残差平方和",false);
	pTable->SetItemText(0,1,SSQ);

	pTable->InsertItem(1,"残差方差",false);
	pTable->SetItemText(1,1,ResVar);

	pTable->InsertItem(2,"标准误",false);
	pTable->SetItemText(2,1,StdE);	
	pTable->InsertItem(3,"AIC",false);
	pTable->SetItemText(3,1,AIC);	
	pTable->InsertItem(4,"自由度",false);
	pTable->SetItemText(4,1,df);	

	pETable= new CRsltElementTable("最终参数",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	
	if(m_eVector.vlen()>0)
	{
		NormalTest(m_pResult,m_eVector);	
	}
	
	//////////////////////////输出参数估计。/////////////////////////

	CTString strBrief;
	if (briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"在变量"+strName+"的模型中:");

	pTable = new CTLTable;
	pTable->SetTitle("\n\n\n参数估计,采用条件最小二乘法");
	pTable->SetHead(strName);
	int nCol=5;
	int nRow=beta_Vector.vlen();
//	pTable->CreateTable(nRow+1,5);
//	pTable->SetCols(5);
//	pTable->SetRows(nRow+1);

	pTable->InsertColumn(0," ");
	pTable->InsertColumn(1,"系数");
	pTable->InsertColumn(2,"系数标准误");
	pTable->InsertColumn(3,"T");
	pTable->InsertColumn(4,"近似Pr(>|T|)");
	int i=0;
	for(i=0;i<nRow;i++)
	{
		if((m_bConstantAR && m_nModel == 1)||
			(m_bConstantMA && m_nModel == 2) || 
			(m_bConstant && m_nModel == 3) )
		{
			if(i==0)
			{
				//add by liytao 20030819
				CTString strBrief,strVal;
				strVal.Format("(概率检验值=%f)",P_Vector(i));
				if (P_Vector(i)<=0.05)
					strBrief="常数项在模型中起显著作用";
				else
					strBrief="常数项在模型中不起显著作用";
				if (briefrTable)
					briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
				//add end

				pTable->InsertItem(i,"均值项",false);
			}
			else
			{
				CTString strVal;
				strVal.Format("(概率检验值=%f)",P_Vector(i));

				if(i<=nAutoReg)
				{
					CTString str;
					str.Format("自回归%d",i);
					//add by liytao 20030819
					CTString strBrief;
					if (P_Vector(i)<=0.05)
						strBrief=str+"在模型中起显著作用";
					else
						strBrief=str+"在模型中不起显著作用";
					if (briefrTable)
						briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
					//add end

					pTable->InsertItem(i,str,false);
				}
				else
				{
					CTString str;
					str.Format("移动平均%d",i-nAutoReg);
					//add by liytao 20030819
					CTString strBrief;
					if (P_Vector(i)<=0.05)
						strBrief=str+"在模型中起显著作用";
					else
						strBrief=str+"在模型中不起显著作用";
					if (briefrTable)
						briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
					//add end
					pTable->InsertItem(i,str,false);
				}
			}
			
			pTable->SetItemText(i,1,beta_Vector(i));
			pTable->SetItemText(i,2,StdError_Vector(i));
			pTable->SetItemText(i,3,t_Vector(i));
			pTable->SetItemText(i,4,P_Vector(i));
		}
		else
		{
			if(i+1<=nAutoReg)
			{
				CTString str;
				str.Format("自回归%d",i+1);
				//add by liytao 20030819
				CTString strBrief,strVal;
				strVal.Format("(概率检验值=%f)",P_Vector(i));
				if (P_Vector(i)<=0.05)
					strBrief=str+"在模型中起显著作用";
				else
					strBrief=str+"在模型中不起显著作用";
				if (briefrTable)
					briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
				//add end

				pTable->InsertItem(i,str,false);
			}
			else
			{
				CTString str;
				str.Format("移动平均%d",i+1-nAutoReg);
				//add by liytao 20030819
				CTString strBrief,strVal;
				strVal.Format("(概率检验值=%f)",P_Vector(i));
				if (P_Vector(i)<=0.05)
					strBrief=str+"在模型中起显著作用";
				else
					strBrief=str+"在模型中不起显著作用";
				if (briefrTable)
					briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
				//add end
				pTable->InsertItem(i,str,false);
			}
			pTable->SetItemText(i,1,beta_Vector(i));
			pTable->SetItemText(i,2,StdError_Vector(i));
			pTable->SetItemText(i,3,t_Vector(i));
			pTable->SetItemText(i,4,P_Vector(i));

		}
	}
	pETable= new CRsltElementTable("参数估计",pTable);//申请一个表对象类的对象
//	m_pRsltVariable->Add(pETable);
	m_pResult->Add(pETable);

//////////////输出协方差////////////////////////

	pTable = new CTLTable;
	pTable->SetTitle("\n\n\n协方差");
	pTable->SetHead(strName);
	nCol=covErr_Matrix.mcol();
	nRow=covErr_Matrix.mrow();
//	pTable->CreateTable(nRow+1,nCol+1);
//	pTable->SetCols(nCol+1);
//	pTable->SetRows(nRow+1);

	pTable->InsertColumn(0," ");
	for(i=0;i<nCol;i++)
	{
		if((m_bConstantAR && m_nModel == 1)||
			(m_bConstantMA && m_nModel == 2) || 
			(m_bConstant && m_nModel == 3) )
		{
			if(i==0)
			{
				pTable->InsertColumn(i+1,"常数项");
			}
			else
			{
				if(i<=nAutoReg)
				{
					CTString str;
					str.Format("自回归%d",i);
					pTable->InsertColumn(i+1,str);
				}
				else
				{
					CTString str;
					str.Format("移动平均%d",i-nAutoReg);
					pTable->InsertColumn(i+1,str);
				}
			}
			
		}
		else
		{
			if(i+1<=nAutoReg)
			{
				CTString str;
				str.Format("自回归%d",i+1);
				pTable->InsertColumn(i+1,str);
			}
			else
			{
				CTString str;
				str.Format("移动平均%d",i+1-nAutoReg);
				pTable->InsertColumn(i+1,str);
			}
		}
	}

	for(i=0;i<nRow;i++)
	{
		if((m_bConstantAR && m_nModel == 1)||
			(m_bConstantMA && m_nModel == 2) || 
			(m_bConstant && m_nModel == 3) )
		{
			if(i==0)
			{
				pTable->InsertItem(i,"常数项",false);
			}
			else
			{
				if(i<=nAutoReg)
				{
					CTString str;
					str.Format("自回归%d",i);
					pTable->InsertItem(i,str,false);
				}
				else
				{
					CTString str;
					str.Format("移动平均%d",i-nAutoReg);
					pTable->InsertItem(i,str,false);
				}
			}			
		}
		else
		{
			if(i+1<=nAutoReg)
			{
				CTString str;
				str.Format("自回归%d",i+1);
				pTable->InsertItem(i,str,false);
			}
			else
			{
				CTString str;
				str.Format("移动平均%d",i+1-nAutoReg);
				pTable->InsertItem(i,str,false);
			}
		}
		for(int j=0;j<nCol;j++)
		{
			pTable->SetItemText(i,j+1,covErr_Matrix(i,j));
		}
	}
	pETable= new CRsltElementTable("协方差",pTable);//申请一个表对象类的对象
//	m_pRsltVariable->Add(pETable);
	m_pResult->Add(pETable);


	//////////////////////////输出参数相关矩阵/////////////////
	pTable = new CTLTable;
	pTable->SetTitle("\n\n\n相关系数");
	pTable->SetHead(strName);
	nCol=RR_Matrix.mcol();
	nRow=RR_Matrix.mrow();
//	pTable->SetCols(nCol+1);
//	pTable->SetRows(nRow+1);

	pTable->InsertColumn(0," ");
	for(i=0;i<nCol;i++)
	{
		if((m_bConstantAR && m_nModel == 1)||
			(m_bConstantMA && m_nModel == 2) || 
			(m_bConstant && m_nModel == 3) )
		{
			if(i==0)
			{
				pTable->InsertColumn(i+1,"常数项");
			}
			else
			{
				if(i<=nAutoReg)
				{
					CTString str;
					str.Format("自回归%d",i);
					pTable->InsertColumn(i+1,str);
				}
				else
				{
					CTString str;
					str.Format("移动平均%d",i-nAutoReg);
					pTable->InsertColumn(i+1,str);
				}
			}
			
		}
		else
		{
			if(i+1<=nAutoReg)
			{
				CTString str;
				str.Format("自回归%d",i+1);
				pTable->InsertColumn(i+1,str);
			}
			else
			{
				CTString str;
				str.Format("移动平均%d",i+1-nAutoReg);
				pTable->InsertColumn(i+1,str);
			}
		}
	}

	for(i=0;i<nRow;i++)
	{
		if((m_bConstantAR && m_nModel == 1)||
			(m_bConstantMA && m_nModel == 2) || 
			(m_bConstant && m_nModel == 3) )
		{
			if(i==0)
			{
				pTable->InsertItem(i,"常数项",false);
			}
			else
			{
				if(i<=nAutoReg)
				{
					CTString str;
					str.Format("自回归%d",i);
					pTable->InsertItem(i,str,false);
				}
				else
				{
					CTString str;
					str.Format("移动平均%d",i-nAutoReg);
					pTable->InsertItem(i,str,false);
				}
			}			
		}
		else
		{
			if(i+1<=nAutoReg)
			{
				CTString str;
				str.Format("自回归%d",i+1);
				pTable->InsertItem(i,str,false);
			}
			else
			{
				CTString str;
				str.Format("移动平均%d",i+1-nAutoReg);
				pTable->InsertItem(i,str,false);
			}
		}
		for(int j=0;j<nCol;j++)
		{
			pTable->SetItemText(i,j+1,RR_Matrix(i,j));
		}
	}
	pETable= new CRsltElementTable("相关系数",pTable);//申请一个表对象类的对象
//	m_pRsltVariable->Add(pETable);
	m_pResult->Add(pETable);


	///////////////////////////////////////////
		//////////////////////////输出预测值/////////////////////////
	if(nForcast>0)
	{
		pTable = new CTLTable;
		pTable->SetTitle("\n\n\n预测值");
		pTable->SetHead(strName);
		nRow=Z_Vector.vlen();
//		pTable->CreateTable(nRow+1,5);
//		pTable->SetCols(5);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0," ");
		pTable->InsertColumn(1,"预测值");
		pTable->InsertColumn(1,"标准误");
		pTable->InsertColumn(1,"95%下限");
		pTable->InsertColumn(1,"95%上限");
		int nSize=m_pDataInterface->GetRowCount();
		for(i=0;i<nRow;i++)
		{
			CTString str1;
			str1.Format("%d",i+nSize+1);
			pTable->InsertItem(i,str1,false);	
			pTable->SetItemText(i,1,Z_Vector(i));
			pTable->SetItemText(i,2,pStdE_Vector(i));
			pTable->SetItemText(i,3,lBound_Vector(i));
			pTable->SetItemText(i,4,uBound_Vector(i));

		}
		pETable= new CRsltElementTable("预测值",pTable);//申请一个表对象类的对象
//		m_pRsltVariable->Add(pETable);
		m_pResult->Add(pETable);
	}

	////////////////////输出预测图形//////////////////
	///////////////////////////////////////////////
	if(nForcast>0)
	{
		CTChartLine* pTChartLine;
		CRsltElementChart* pREC;
		pTChartLine=new CTChartLine;
		CTString strX = "预测范围";
		pTChartLine->SetXAxilLabel(strX);
		CTString strY = "值";
		pTChartLine->SetYAxilLabel(strY);
		pREC=new CRsltElementChart("预测图",pTChartLine);
//		m_pRsltVariable->Add(pREC);
		m_pResult->Add(pREC);
    
		TDataPointValVal* pDPSV;
		//得到Variable数据//////////////////////////////////////////////////////////
		CTString strTemp;
		pDPSV=new TDataPointValVal[X_Vector.vlen()];
		int j;
		for(j=0;j<X_Vector.vlen();j++)
		{
		//	strTemp.Format("%d",);
			pDPSV[j].fXVal =j+1;//strTemp;
			pDPSV[j].fYVal =X_Vector[j] ;

		}
 		pTChartLine->SetLine(pDPSV,X_Vector.vlen(),ValVal); 
		delete[] pDPSV;

		pDPSV=new TDataPointValVal[Z_Vector.vlen()];
		for(int i=0;i<Z_Vector.vlen();i++)
		{
			//strTemp.Format("%d",);
			pDPSV[i].fXVal =j+i+1;//strTemp;
			pDPSV[i].fYVal =Z_Vector[i] ;


		}
		
 		pTChartLine->SetLine(pDPSV,Z_Vector.vlen(),ValVal); 
		delete[] pDPSV;
	}

}

bool CTimeSeries::NormalTest(CResult* pResult,CDoubleVector DataArray)
{
	double mean1=mean(DataArray);
	double nSize=DataArray.vlen();
	int nLen=nSize;
	CDoubleVector WeightArray(nLen,1);
	double variance=0;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		variance=variance+(DataArray(i)-mean1)*(DataArray(i)-mean1)*WeightArray(i);
	}
	if(variance+1==1.0)
	{
		CTString str="方差为0，不能做正态分布检验！";
		CRsltElementText *pNRsltElementText=new CRsltElementText("警告");
		pNRsltElementText->AddString(str);
		pResult->Add(pNRsltElementText);
		return false;
	}
	variance=variance/(nSize-1);
	variance=sqrt(variance);
	double Dmax=0,Dmin=0;
	double tempmax=0;
	double tempmin=0;
	double Dup=0;
	double Dlow=0;
	for(i=0;i<nLen;i++)
	{
		if(i==0)
			Dup=0-normcdf(DataArray(i),mean1,variance);
		else
			Dup=CalkCommonFx(DataArray,WeightArray,DataArray(i-1),nSize)-normcdf(DataArray(i),mean1,variance);
		Dlow=CalkCommonFx(DataArray,WeightArray,DataArray(i),nSize)-normcdf(DataArray(i),mean1,variance);
		if(i==0)
		{
			Dmax=Dup>=Dlow?Dup:Dlow;
			Dmin=Dup<=Dlow?Dup:Dlow;
		}
		else
		{
			tempmax=Dup>=Dlow?Dup:Dlow;
			tempmin=Dup<=Dlow?Dup:Dlow;
			Dmax=Dmax>tempmax?Dmax:tempmax;
			Dmin=Dmin<tempmin?Dmin:tempmin;
		}
			
	}
	double Dmax1=Dmax>0?Dmax:-Dmax;
	double Dmin1=Dmin>0?Dmin:-Dmin;
	double maxDiff=Dmax1>Dmin1?Dmax1:Dmin1;
	double Z=sqrt(nSize)*maxDiff;
	double P=0;
	if(Z+1==1.0)
		P=0;
	else
	{
		double Q=exp(-1.233701*pow(Z,-2));
		if(Z<0.27)
			P=1;
		else if(Z>=0.27&&Z<1)
			P=(1-2.506628/Z*(pow(Q,1)+pow(Q,9)+pow(Q,25)));
		else if(Z>=1&&Z<3.1)
		{
			Q=exp(-2*Z*Z);
			P=(2*(Q-pow(Q,4)+pow(Q,9)-pow(Q,16)));
		}
		else
			P=0;
	}
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable = new CTLTable;
//	pTable->CreateTable(NULL);
	pTable->SetTitle("残差正态检验");
//	pTable->SetCols(7);
//	pTable->SetRows(2);

	pTable->InsertColumn(0,"均数");
	pTable->InsertColumn(1,"标准差");
	pTable->InsertColumn(2,"绝对差");
	pTable->InsertColumn(3,"最大正差");
	pTable->InsertColumn(4,"最小负差");
	pTable->InsertColumn(5,"Z");
	pTable->InsertColumn(6,"显著性");
	CTString strTemp;
	strTemp.Format("%.4f",mean1);
	pTable->InsertItem(i,strTemp);
	pTable->SetItemText(i,1,variance);
	pTable->SetItemText(i,2,maxDiff);
	pTable->SetItemText(i,3,Dmax);
	pTable->SetItemText(i,4,Dmin);
	pTable->SetItemText(i,5,Z);
	pTable->SetItemText(i,6,P);
	CTString str="残差正态检验";
	pETable=new CRsltElementTable(str,pTable);//申请一个表对象类的对象
	pResult->Add(pETable);


	
	CTChartLine* pTChartLine;
	CRsltElementChart* pREC;
	TDataPointValVal* pDPSV1;
	//得到Variable数据//////////////////////////////////////////////////////////
	pDPSV1=new TDataPointValVal[nLen];	
	for(int j=0;j<nLen;j++)
	{
		//	strTemp.Format("%d",);
		pDPSV1[j].fXVal =j+1;//strTemp;
		pDPSV1[j].fYVal =DataArray(j);
	}

	pTChartLine=new CTChartLine(pDPSV1,nLen,ValVal,"残差序列图","序列","残差值");
	pREC=new CRsltElementChart("残差",pTChartLine);
	pResult->Add(pREC);   
	delete[] pDPSV1;	
	return true;

}
