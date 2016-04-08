// SurvivalCox.cpp: implementation of the CSurvivalCox class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "SurvivalCox.h"

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

CSurvivalCox::CSurvivalCox()
{
	m_pDataInterface = NULL;
	m_pResult = NULL;
	//add by  liytao 20030121
	m_pCovarFNOs=NULL;
	m_pCovarValid=NULL;
	m_pData=NULL;
	m_pStatus=NULL;
	m_pSurvTime=NULL;
	m_pWeight=NULL;
	//add end

}

CSurvivalCox::~CSurvivalCox()
{
	//if( m_pDlg )
	//	delete m_pDlg;
	//m_pDlg=NULL;
	delete []m_pCovarFNOs;
	delete []m_pCovarValid;
}

CResult * CSurvivalCox::OnSurvivalCox( CDataInterface * pDataInterface, string VO)
{
	assert( pDataInterface!=NULL );
	m_pDataInterface = pDataInterface;
	m_pResult = NULL; //防止第一次正确运行后,第二条件不满足时错误! zlq
	
	if (GetValueFromVO(pDataInterface, VO) && Initialize())
	{
		m_pResult = Analyze();
		return m_pResult;
	}
	else
	{
		CResult* pResult=new CResult("比例风险模型");
		CTString strWarning = "参数设置不合理，无法计算！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	return NULL;
}

bool CSurvivalCox::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	SURVIVALCOX
//		/DATA=[FILE]
//		/TIMEVAR [varlist]
//		/STATUSVAR [varlist]
//		/COVARIATE [varlist]
//		/WEIGHT [varlist]
//		/CRITERIA=[{EVENT(**)},{RANGE{**,**}}]
//		/RESULT [BASEFUNC]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/TIMEVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nTimeFNO = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();
	
	AnaSyntax::getVAR("/STATUSVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	yname = ynames.GetAt(0);
	m_nStatusFNO = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();
	
	CTStringArray xname;
	AnaSyntax::getVAR("/COVARIATE", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	xname.RemoveAll();
	int nSize= m_tVarInfo.iCount;
	if (nSize<=0)
	{
		return FALSE;
	}
	m_pCovarFNOs=new int[nSize];
	m_pCovarValid=new int[nSize];//add by liytao 20030117
	int i=0;
	for (i=0;i<nSize;i++)
	{
		*(m_pCovarFNOs+i)=m_tVarInfo.GetIDAt(i);
		*(m_pCovarValid+i)=0;//add by liytao 20030117
	}
	m_nColNum=nSize;

	m_bWeighted = AnaSyntax::exist("/WEIGHT",VOstr);
	if (m_bWeighted)
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return FALSE;
		yname = ynames.GetAt(0);
		m_nWeightFNO = pDataInterface->IndexByName(yname);
		ynames.RemoveAll();
	}
	
	m_bSingleEventValue = AnaSyntax::findstr("/CRITERIA","EVENT",VOstr);
	if (m_bSingleEventValue) 
		m_dSingleValue = AnaSyntax::getvalue("/CRITERIA","EVENT",VOstr);
	else if (AnaSyntax::findstr("/CRITERIA","RANGE",VOstr))
	{
		m_dRangeFrom = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr);
		m_dRangeTo = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr, 2);
	}
	else 
		return FALSE;

	m_bDispBaseLn = AnaSyntax::findstr("/RESULT","BASEFUNC",VOstr);
	return TRUE;
}

bool CSurvivalCox::Initialize()
{
	//
	CDataAccess *pAccess=&m_pDataInterface->m_DataAccess;	
	CFieldList* pFT=pAccess->m_pFieldList;
	CField *field;
	m_nRowNum=pAccess->GetRecordCount();
	if (m_nRowNum<=0)
		return false;

	m_pData=new double[m_nRowNum*m_nColNum];
	memset(m_pData,0x00,sizeof(double)*m_nRowNum*m_nColNum);
	m_pSurvTime=new double[m_nRowNum];
	memset(m_pSurvTime,0x00,sizeof(double)*m_nRowNum);
	m_pStatus=new int[m_nRowNum];
	memset(m_pStatus,0x00,sizeof(int)*m_nRowNum);


	//	add by liytao 20030116
	m_dbPrecision=0.001;
	m_Eps = 1.0/1024.0;
	m_dbW=0;
	m_pWeight=NULL;
	//add end


	//add by liytao 20030116
	int nRC=m_pDataInterface->m_DataAccess.GetRecordCount();
	if(m_pWeight!=NULL)
		delete []m_pWeight;
	m_pWeight=new double [nRC];
	memset(m_pWeight ,0x00,sizeof(double)*nRC);
	
	//add by liytao 20030117
	double *tempCovar= new double[m_nColNum];
	memset(tempCovar ,0x00,sizeof(double)*m_nColNum);
	//add end
	if( !m_bWeighted)
	{
		for (int i=0;i<nRC;i++)
			m_pWeight[i]=1;
	}
	//add end
	pAccess->First();
	int i=0,j=0;
	double dbVal;
	bool bExist=false,bHaveNegative=false;
	//int nTempRowNum=0;
	while(!pAccess->Eof())
	{
		bool bWrong=false;
		for (j=0;!bWrong&&j<m_nColNum;j++)
		{
			//if (pFT->FieldByIndex(*(m_pCovarFNOs+j))->IsNull()) modified by liytao 20030115
			field=pFT->FieldByIndex(*(m_pCovarFNOs+j));
			if (m_pDataInterface->IsMissingValue(field,true))
			{
				bWrong=true;
			}
			else
			*(m_pData+i*m_nColNum+j)=pFT->FieldByIndex(*(m_pCovarFNOs+j))->GetAsDouble();
			//add by liytao 20030117
			if (i==0)
				tempCovar[j]=m_pData[i*m_nColNum+j];
			else
			{
				if (m_pData[i*m_nColNum+j]!=tempCovar[j])
					m_pCovarValid[j]=1;
			}
			//add end
		
		}
		if( bWrong)
		{
			pAccess->Next();
			continue;
		}		
		//if (pFT->FieldByIndex(m_nTimeFNO)->IsNull())modified by liytao 20030115
		field=pFT->FieldByIndex(m_nTimeFNO);
		if (m_pDataInterface->IsMissingValue(field,true))
		{
			pAccess->Next();
			continue;
		}
		*(m_pSurvTime+i)=pFT->FieldByIndex(m_nTimeFNO)->GetAsDouble();
		//add by liytao20030116
		if( *(m_pSurvTime+i)<0)
		{
			bHaveNegative=true;
			//break; modified by liytao 20030122
			pAccess->Next();
			continue;
		}
		//add end

		//if (pFT->FieldByIndex(m_nStatusFNO)->IsNull())modified by liytao 20030115
		field=pFT->FieldByIndex(m_nStatusFNO);
		if( m_pDataInterface->IsMissingValue(field,true))
		{
			pAccess->Next();
			continue;
		}
		dbVal=pFT->FieldByIndex(m_nStatusFNO)->GetAsDouble();
		//add by liytao 20030116
		if( m_bWeighted)
		{
			field=pFT->FieldByIndex(m_nWeightFNO);
			if (m_pDataInterface->IsMissingValue(field,true))
			{
				pAccess->Next();
				continue;
			}
			*(m_pWeight+i)=pFT->FieldByIndex(m_nWeightFNO)->GetAsDouble();
			//add by liytao20030116
			if( *(m_pWeight+i)<=0)
			{
				bHaveNegative=true;
				//break; modified by liytao 20030122
				pAccess->Next();
				continue;
			}
			//add end
			m_dbW=m_dbW+*(m_pWeight+i);
		}
		else
			m_dbW=m_dbW+1;
		
		//add end
		if (m_bSingleEventValue)
		{
			if (fabs(dbVal-m_dSingleValue)<m_dbPrecision)
			{
				*(m_pStatus+i)=1;
				bExist=true;
			}
			else
				*(m_pStatus+i)=0;
		}
		else
		{
			//if ((dbVal-m_dRangeFrom)>=0&&(dbVal-m_dRangeTo)<=0) modified by liytao 20030120
			if (((dbVal>m_dRangeFrom)&&(dbVal<m_dRangeTo))||((dbVal>m_dRangeTo)&&(dbVal<m_dRangeFrom)))
			{
				*(m_pStatus+i)=1;
				bExist=true;
			}
			else
				*(m_pStatus+i)=0;
		}
		//add by liytao 20030117
		for (j=0;j<m_nColNum;j++) // 20030121
			tempCovar[j]=m_pData[i*m_nColNum+j];
		pAccess->Next();
		i++;
	}

	if (i<=0||fabs(m_dbW)<0.000001)
	{
		delete []tempCovar;
		if ( m_pWeight!=NULL)
			delete []m_pWeight;
		m_pWeight=NULL;
		if ( m_pCovarValid)
			delete []m_pCovarValid;
		m_pCovarValid=NULL;
		if (m_pCovarFNOs)
			delete [] m_pCovarFNOs;
		m_pCovarFNOs=NULL;
// 		AfxMessageBox("无有效记录存在,无法计算");
		return false;
	}
	
	m_nRowNum=i;
	int nErrCol=0;
	for (j=0;j<m_nColNum;j++)
	{
		if( m_pCovarValid[j]==0)
			nErrCol++;
	}
	if (nErrCol==m_nColNum)
	{
		delete []tempCovar;
		if ( m_pWeight!=NULL)
			delete []m_pWeight;
		m_pWeight=NULL;
		if ( m_pCovarValid)
			delete []m_pCovarValid;
		m_pCovarValid=NULL;
		if (m_pCovarFNOs)
			delete [] m_pCovarFNOs;
		m_pCovarFNOs=NULL;
		return false;
	}

	int nTempCol=m_nColNum;
	for (j=m_nColNum-1;j>=0;j--)
	{
		if( m_pCovarValid[j]==0)
		{
			DelCol(m_pData,m_nRowNum,nTempCol,j);
			nTempCol--;
		}
	}
	m_nColNum=nTempCol;
	//add end
	//如果该事件记录不存在则不能运算
	if (!bExist)
	{
		delete []tempCovar;
		if ( m_pWeight!=NULL)
			delete []m_pWeight;
		m_pWeight=NULL;
		if ( m_pCovarValid)
			delete []m_pCovarValid;
		m_pCovarValid=NULL;
		if (m_pCovarFNOs)
			delete [] m_pCovarFNOs;
		m_pCovarFNOs=NULL;
// 		AfxMessageBox("该事件记录不存在,无法计算");
		return false;
	}
	return true;
}

CResult* CSurvivalCox::Analyze()
{

	CTLTable* briefrTable=NULL;

	// output result related
	double *b=new double[m_nRowNum];//coefficents: beta
	memset(b,0x00,sizeof(double)*m_nRowNum);
	double *SE=new double[m_nColNum];//StdErr
	memset(SE,0x00,sizeof(double)*m_nColNum);
	double *Sig=new double[m_nRowNum];//Sig.
	memset(Sig,0x00,sizeof(double)*m_nRowNum);

	double *Sf=new double[m_nRowNum];//survival 
	memset(Sf,0x00,sizeof(double)*m_nRowNum);
	double *Alpha=new double[m_nRowNum];//Alpha for baseline survival function
	memset(Alpha,0x00,sizeof(double)*m_nRowNum);
	
	double *Means=new double[m_nColNum];//Alpha for baseline survival function
	memset(Means,0x00,sizeof(double)*m_nColNum);


	//
	//procedural 
	double *AV=new double[m_nColNum];			//average of variables
	double *SD=new double[m_nColNum];			//sd of variables
	memset(AV,0x00,sizeof(double)*m_nColNum);
	memset(SD,0x00,sizeof(double)*m_nColNum);

	
	
	
	double	LLp = 2e+30;
	double  LL  = 1e+30;
	double LLn;
	int i,j;



	for(j=0;j<m_nColNum;j++)
	{
		AV[j] = 0.0;
		SD[j] = 0.0;
	}
	// get means of covars 
	double dbSum;

	for(i=0; i<m_nColNum; i++ ) 
	{	
	
		dbSum=0;
	/*	for (j=0;j<m_nRowNum;j++)
			dbSum+=m_pData[j*m_nColNum+i];
		Means[i]=dbSum/(double)m_nRowNum; modified by liytao 20030122*/
		for (j=0;j<m_nRowNum;j++)
			dbSum+=m_pData[j*m_nColNum+i]*m_pWeight[j];
		Means[i]=dbSum/m_dbW; 
	}



	for(i=0;i<m_nRowNum;i++)
	{
		for(j=0;j<m_nColNum;j++)
		{
			double dbV;
			dbV=*(m_pData+i*m_nColNum+j);

			/*AV[j] = AV[j] + dbV;
			SD[j] = SD[j] + dbV*dbV; modified by liytao 20030116*/
			AV[j] = AV[j] + m_pWeight[i]*dbV;
			SD[j] = SD[j] + m_pWeight[i]*dbV*dbV;
		}
	}
	for(j = 0; j<m_nColNum; j++) 
	{

		/*AV[j] = AV[j] / (double)m_nRowNum;
		SD[j] = SD[j] / (double)m_nRowNum; modified by liytao 20030116*/
		AV[j]=AV[j]/m_dbW;
		SD[j]=SD[j]/m_dbW;
		SD[j] = sqrt( fabs( SD[j] - AV[j]*AV[j] ) );
	}
	double temp;
	//sort	descendingly 
	for(i=0; i<m_nRowNum-1; i++) 
	{
		int iBig = i;
		for(j=i+1; j<m_nRowNum; j++) 
		{
			if( (m_pSurvTime[j]-m_Eps*m_pStatus[j]) > (m_pSurvTime[iBig]-m_Eps*m_pStatus[iBig]) )
				iBig = j;
		}
		if( iBig!=i ) 
		{
			temp		= m_pSurvTime[i]; 
			m_pSurvTime[i]	= m_pSurvTime[iBig]; 
			m_pSurvTime[iBig] = temp;

			temp		= m_pStatus[i];  
			m_pStatus[i]		= m_pStatus[iBig];   
			m_pStatus[iBig]	= (int)temp;

			//add by liytao 20030116
			temp=m_pWeight[i];
			m_pWeight[i]=m_pWeight[iBig];
			m_pWeight[iBig]=temp;
			//add end

			for(j=0; j<m_nColNum; j++ ) 
			{
				temp			= *(m_pData+i*m_nColNum+j); 
				*(m_pData+i*m_nColNum+j)		= *(m_pData+iBig*m_nColNum+j);
				*(m_pData+iBig*m_nColNum+j)	= temp;
			}
		}
	}//end sort

	//stat[i]=
	if( m_pStatus[0]>0 ) 
		m_pStatus[0] = m_pStatus[0]+2; 

	for(i=1; i<m_nRowNum; i++ ) 
	{
		if( m_pStatus[i]>0 && ( m_pStatus[i-1]==0 || m_pSurvTime[i-1]!=m_pSurvTime[i] ) ) 
			m_pStatus[i] = m_pStatus[i]+2;
	}
	if( m_pStatus[m_nRowNum-1]>0 ) 
		m_pStatus[m_nRowNum-1] = m_pStatus[m_nRowNum-1]+4;
	
	for(i=m_nRowNum-2; i>-1; i-- ) 
	{
		if( m_pStatus[i]>0 && ( m_pStatus[i+1]==0 || m_pSurvTime[i+1]!=m_pSurvTime[i] ) ) 
			m_pStatus[i] = m_pStatus[i]+4;
	}

	//standardizing data
	for(i = 0; i<m_nRowNum; i++) 
		for(j=0; j<m_nColNum; j++ ) 
			*(m_pData+i*m_nColNum+j) = (*(m_pData+i*m_nColNum+j)-AV[j]) / SD[j];

	//computing
	//initialization	
	int	k;
	double dbd;//modified by liytao 20030116
	double *a=new double[m_nColNum*(m_nColNum+1)];	
	memset(a,0x0,sizeof(double)*(m_nColNum*(m_nColNum+1)));

	double *s1=new double [m_nColNum];			//
	memset(s1,0x0,sizeof(double)*m_nColNum);
	double *s2=new double [m_nColNum*m_nColNum];		//
	memset(s2,0x0,sizeof(double)*(m_nColNum*m_nColNum));
	double *s=new double [m_nColNum];
	memset(s,0x0,sizeof(double)*m_nColNum);
	//d = 0;	
	dbd=0;// add by liytao 20030116
	for(j=0; j<m_nColNum; j++) 
		b[j] = 0;
	while(fabs(LLp-LL) > m_dbPrecision ) 
	{
		//initialization
		LLp = LL;
		LL  = 0;
		double s0	 = 0.0 ;
		int	   StatI = 0;
		for(j=0; j<m_nColNum; j++ ) 
		{
			s1[j]	   = 0;
			a[j*(m_nColNum+1)+m_nColNum] = 0;
			for(k=0; k<m_nColNum; k++ ) 
			{
				s2[j*m_nColNum+k] = 0;
				a[j*(m_nColNum+1)+k]  = 0;
			}
		}
	
		//add by liytao 20021212 
		for(i=0; i<m_nRowNum; i++ ) 
		{
			Alpha[i] = 1;
		}
		//add end
		for(i=0; i<m_nRowNum; i++ ) 
		{
		//	Alpha[i] = 1;
			temp	 = 0;
			for(j=0; j<m_nColNum; j++ ) 
				temp = temp + b[j]*m_pData[i*m_nColNum+j];
		
			temp = exp( temp );
			//s0   = s0 + temp; modified by liytao 20030116
			s0   = s0 + m_pWeight[i]*temp;
			for(j=0; j<m_nColNum; j++ ) 
			{
				/*s1[j] = s1[j] + m_pData[i*m_nColNum+j]*temp;
				for(k=0; k<m_nColNum; k++ ) 
					s2[j*m_nColNum+k] = s2[j*m_nColNum+k] + m_pData[i*m_nColNum+j]*m_pData[i*m_nColNum+k]*temp;
				modified by liytao 20030116*/
				s1[j] = s1[j] + m_pWeight[i]*m_pData[i*m_nColNum+j]*temp;
				for(k=0; k<m_nColNum; k++ ) 
					s2[j*m_nColNum+k] = s2[j*m_nColNum+k] + m_pWeight[i]*m_pData[i*m_nColNum+j]*m_pData[i*m_nColNum+k]*temp;
				
			}

			StatI = m_pStatus[i];
			if( StatI==2 || StatI==3 || StatI==6 || StatI==7 ) 
			{
				//d = 0; 
				dbd=0;//add by liytao 20030116
				for(j=0; j<m_nColNum; j++ ) 
					s[j] = 0;
			}
			if( StatI==1 || StatI==3 || StatI==5 || StatI==7 ) 
			{
				//d = d + 1;
				dbd=dbd+m_pWeight[i];//add by liytao 20030116
				for ( j=0; j<m_nColNum; j++ ) 
					//s[j] = s[j] + m_pData[i*m_nColNum+j]; modified by liytao 20030116
					s[j] = s[j] + m_pWeight[i]*m_pData[i*m_nColNum+j];
			}
			if( StatI==4 || StatI==5 || StatI==6 || StatI==7 ) 
			{
				for(j=0; j<m_nColNum; j++ ) 
				{
					LL		   = LL + s[j]*b[j];
					//a[j*(m_nColNum+1)+m_nColNum] = a[j*(m_nColNum+1)+m_nColNum] + s[j] - d*s1[j]/s0;
					a[j*(m_nColNum+1)+m_nColNum] = a[j*(m_nColNum+1)+m_nColNum] + s[j] - dbd*s1[j]/s0;
					for(k=0; k<m_nColNum; k++ ) 
						//a[j*(m_nColNum+1)+k] = a[j*(m_nColNum+1)+k] + d*( s2[j*m_nColNum+k]/s0 - s1[j]*s1[k]/( s0*s0 ) );
						a[j*(m_nColNum+1)+k] = a[j*(m_nColNum+1)+k] + dbd*( s2[j*m_nColNum+k]/s0 - s1[j]*s1[k]/( s0*s0 ) );
				}
				//LL = LL - d*log( s0 );
				LL = LL - dbd*log( s0 );
				/* modify by liytao 20021216
				if( d == 1 )
				{ 
					Alpha[i] = pow( (1-temp/s0), (1/temp) ) ;
				}
				else
				{ 

					Alpha[i] = exp( -d/s0 );
				}*/
				//Alpha[i] = pow( (1-d*temp/s0), (1/temp) ) ;
				Alpha[i] = pow( (1-dbd*temp/s0), (1/temp) ) ;


			}

		}//end for

		LL = -2*LL;	//-2 Log Likelihood(LL) 
		if( LLp == 1e+30 ) 
		{  
			LLn = LL;
		}


		//inverting matrix ...
		for(i=0; i<m_nColNum; i++) 
		{ 
			temp	= a[i*(m_nColNum+1)+i]; 
			a[i*(m_nColNum+1)+i] =		1;
			for(k=0; k<m_nColNum+1; k++) 
			{ 
				a[i*(m_nColNum+1)+k] = a[i*(m_nColNum+1)+k] / temp;
			}
			for(j=0; j<m_nColNum; j++) 
			{
				if( i != j ) 
				{ 
					temp    = a[j*(m_nColNum+1)+i]; 
					a[j*(m_nColNum+1)+i] =		0;
					for(k=0; k<m_nColNum+1; k++) 
					{
						a[j*(m_nColNum+1)+k] = a[j*(m_nColNum+1)+k] - temp*a[i*(m_nColNum+1)+k];
					}
				}
			}
		}

		//beta(k+1) = beta(k)+...
		for(j=0; j<m_nColNum; j++) 
		{
			b[j] = b[j] + a[j*(m_nColNum+1)+m_nColNum];
		}
			
	}//end while

	//b[j]=		SE[j]=		Sig[j]=
	for(j=0; j<m_nColNum; j++) 
	{	
		b[j]   = b[j] / SD[j];	
		SE[j]  = sqrt(a[j*(m_nColNum+1)+j]) / SD[j];
		double tempv=b[j]/SE[j];
		if ( tempv>10e+20||tempv<-(10e+20))
		{
			//free local vars
			delete []b;//coefficents: beta
			delete []SE;//StdErr
			delete []Sig;//Sig.
			delete []Sf;//survival 
			delete []Alpha;//Alpha for baseline survival function
			delete []Means;//Alpha for baseline survival function
			//procedural 
			delete []AV;			//average of variables
			delete []SD;			//sd of variables
			delete []a;	
			delete []s1;			//
			delete []s2;		//
			delete []s;


			if( m_pSurvTime)
				delete []m_pSurvTime;
			if (m_pStatus)
				delete []m_pStatus;
			if (m_pWeight)
				delete []m_pWeight;
			if( m_pData)
				delete []m_pData;
			m_pData=NULL;
			m_pSurvTime=NULL;
			m_pStatus=NULL;

			//m_dbPrecision=0.001;
			//m_Eps = 1.0/1024.0;
			m_pWeight=NULL;//add by 20030115
			m_bWeighted=false; //add by liytao 20030116
			m_nWeightFNO=-1;

// 			AfxMessageBox("协变量不能收敛，无法计算");
			return NULL;
		}
		Sig[j] = Sigdot( b[j]/SE[j] );
	}
	//end computing

	CDataAccess *pAccess=&m_pDataInterface->m_DataAccess;	
	CFieldList* pFL=pAccess->m_pFieldList;
	CFieldType ft=pFL->FieldByIndex(m_nTimeFNO)->GetFieldType();
	
	CResult* pResult=new CResult("比例风险模型");

	//add by liytao 20030115
	int nRC=m_pDataInterface->m_DataAccess.GetRecordCount();
	CRsltElementText* pText;
	pText=new CRsltElementText("记录信息");
	CTString str;
	str.Format("总记录数：%d，缺失记录数：%d ",nRC,nRC-m_nRowNum);

	//add by liytao 20030116
	if (m_bWeighted)
	{
		ft=pFL->FieldByIndex(m_nWeightFNO)->GetFieldType();
		if (ft==fDouble)
		{
			str=str+_T("(警告：权重变量是浮点型，结果可能无统计学意义)\n");
		}
	
	}
	pText->AddString(str);
	//add end
	pResult->Add(pText);
	//add end

	
	CTString timeFieldName=m_pDataInterface->GetFieldName(m_nTimeFNO);//add by liytao 20030819
	
	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{				
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"结果说明");
	}



	//add table1
	CTLTable * pTable2,*pTable3,*pTable = new CTLTable;

	CTString strHead,strTmp;
	pTable->SetTitle("变量值表");

	pTable->InsertColumn(0,"变量");
	pTable->InsertColumn(1,"Beta系数");
	pTable->InsertColumn(2,"标准误");
	pTable->InsertColumn(3,"显著性概率");
	pTable->InsertColumn(4,"优势比");
	pTable->InsertColumn(5,"优势比95%\n置信区间下限");
	pTable->InsertColumn(6,"优势比95%\n置信区间上限");
	
	CRsltVariable *pE2,*pE3,* pE = new CRsltElementTable("变量值表",pTable);
	pResult->Add(pE);
	for (j=0;j<m_nColNum;j++)
	{

		//add by liytao 20021212
		strTmp= pFL->FieldByIndex(m_pCovarFNOs[j])->GetFieldName();

		//add by liytao 20030819
		CTString strBrief,strVal;
		strVal.Format("(概率检验值=%f)",Sig[j]);
		if (Sig[j]<=0.05)
			strBrief="的影响较大";
		else
			strBrief="的影响较小";
		strBrief=strTmp+"对时间变量"+timeFieldName+strBrief;
		if(briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strBrief+strVal);
		//add end

		pTable->InsertItem(j,strTmp);
		pTable->SetItemText(j,1,b[j]);	
		pTable->SetItemText(j,2,SE[j]);	
		pTable->SetItemText(j,3,Sig[j]);
		pTable->SetItemText(j,4,exp(b[j]));
		pTable->SetItemText(j,5,exp(b[j]-1.96*SE[j]));
		pTable->SetItemText(j,6,exp(b[j]+1.96*SE[j]));		
	}
	double dbTmp;
	if (m_bDispBaseLn)
	{
		pTable2 = new CTLTable;
		pTable2->SetTitle("基于协变量均值的生存表");
		pTable2->InsertColumn(0,"时间");
		pTable2->InsertColumn(1,"生存率");
		pTable2->InsertColumn(2,"累积风险");
		pE2 = new CRsltElementTable("基于协变量均值的生存表",pTable2);
		pResult->Add(pE2);

		for(i=m_nRowNum-1; i>-1; i-- )
			Sf[i] = 1.0;
		Sf[m_nRowNum-1] = Alpha[m_nRowNum-1];
		if (ft==fDouble)
			strTmp.Format("%9.4f",m_pSurvTime[m_nRowNum-1]);
		else
			strTmp.Format("%.0f",m_pSurvTime[m_nRowNum-1]);
		dbTmp=m_pSurvTime[m_nRowNum-1];
		pTable2->InsertItem(0,strTmp);
		pTable2->SetItemText(0,1,Sf[m_nRowNum-1]);
		pTable2->SetItemText(0,2,-log(Sf[m_nRowNum-1]));
		j=1;
		for(i=m_nRowNum-2; i>-1; i-- ) 
		{	
			Sf[i] = Sf[i+1] * Alpha[i];	
			if (m_pStatus[i]<=0)
				continue;
			if (fabs(dbTmp-m_pSurvTime[i])<0.001)
				continue;
			if (ft==fDouble)
				strTmp.Format("%9.4f",m_pSurvTime[i]);
			else
				strTmp.Format("%.0f",m_pSurvTime[i]);
			pTable2->InsertItem(j,strTmp);
			pTable2->SetItemText(j,1,Sf[i]);
			if ( Sf[i]<=0)
				strTmp=_T("*");
			else
				pTable2->SetItemText(j,2,-log(Sf[i]));
			dbTmp=m_pSurvTime[i];
			j++;
		}
	}
	pTable3 = new CTLTable;
	pTable3->SetTitle("协变量均值表");
	pTable3->InsertColumn(0,"变量名");
	pTable3->InsertColumn(1,"均值");	
	pE3 = new CRsltElementTable("协变量均值表",pTable3);
	pResult->Add(pE3);
	
	for (i=0;i<m_nColNum;i++)
	{
		strTmp= pFL->FieldByIndex(m_pCovarFNOs[i])->GetFieldName();
		pTable3->InsertItem(i,strTmp);
		pTable3->SetItemText(i,1,Means[i]);
	}
	delete []b;//coefficents: beta
	delete []SE;//StdErr
	delete []Sig;//Sig.
	delete []Sf;//survival 
	delete []Alpha;//Alpha for baseline survival function
	delete []Means;//Alpha for baseline survival function
	delete []AV;			//average of variables
	delete []SD;			//sd of variables
	delete []a;	
	delete []s1;			//
	delete []s2;		//
	delete []s;

	if( m_pSurvTime)
		delete []m_pSurvTime;
	if (m_pStatus)
		delete []m_pStatus;
	if (m_pWeight)
		delete []m_pWeight;
	if( m_pData)
		delete []m_pData;
	m_pData=NULL;
	m_pSurvTime=NULL;
	m_pStatus=NULL;
	m_pWeight=NULL;//add by 20030115
	m_bWeighted=false; //add by liytao 20030116
	m_nWeightFNO=-1;
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (pResult->Count()<=0)
				pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;
	return pResult;
}

double CSurvivalCox::Sigdot(double x)
{
	double p;
	double t;
	int    a;
	
	x = x*x;	 
	p = exp( -0.5*x ) * sqrt( 2*x/PI );
	t = p; 
	a = 1; 
	while( t > 0.000001*p ) 
	{ 
		a = a + 2; 
		t = t * x/a; 
		p = p + t;
	}

    return ( 1-p );
}

int CSurvivalCox::DelCol(double *pData,int nRowNum,int nColNum, int nColNO)
{
	if (pData==NULL||nColNO<0||(nColNum>0&&nColNO>=nColNum)||(nRowNum<=0))	
		return -1;
	int i=nRowNum-1;
	for ( i=nRowNum-1;i>=0;i--)
	{
		for (int j=nColNum-1;j>=nColNO;j--)
		{
			for (int k=(nColNum*i+nColNO);k<(nColNum*nRowNum-1);k++)
				pData[k]=pData[k+1];
			
		}

	}

	/*double *p;
	for (int ii=0;ii<nRowNum;ii++)
	{
		p=pData+(nRowNum-1)*nColNum+ii;
		delete p;
	}*/
	return (nColNum-1)*nRowNum;


}
