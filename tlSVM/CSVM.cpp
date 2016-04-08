// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include "stdafx.h"
#include "../Public/System.h"
#include "CSVM.h"
#include "SVMThread.h"
//##ModelId=3DD99DBE028A

int CSVM::Examine(int nIndex)
{
	// TODO: Add your specialized code here.
	int k,y2,i1;
	double a2,e1,e2,r2,tmax;
	double aa;
	// get value from database
	y2=GetY(nIndex);
	a2=GetAlpha(nIndex);
	//e2=Learn(nIndex)-y2; modified by liytao Nov21
	if (a2>0&&a2<m_dbCeilalpha)
		e2=GetError(nIndex);
	else
		e2=Learn(nIndex)-y2;
		
	// modify end
	
	r2=e2*y2;

	if ((r2<-m_tolerance&&a2<m_dbCeilalpha)||(r2>m_tolerance&&a2>0))
	{
		
		for (i1 = -1,tmax = 0, k = 0; k <m_nEndSupporti ; k++)
		{
			aa=GetAlpha(k);
			if(aa>0 && aa<m_dbCeilalpha)
			{
				e1=GetError(k);
				double temp;
				temp=fabs(e1-e2);	
				if(temp>tmax)
				{
					tmax=temp;
					i1=k;
				}
			}
		}
		if (i1>=0)
		{
			if (TakeStep(nIndex,i1))
			//if (TakeStep(i1,nIndex))
				return 1;
		}
		//else 
		//loop 2
		int jj;
		double rnd0_1=rand()/RAND_MAX;
		int rnd=(int)(rnd0_1*m_nEndSupporti);
		for(jj=rnd; jj<m_nEndSupporti+rnd; jj++)
		{
			int j=jj % m_nEndSupporti;
			aa=GetAlpha(j);
			if ((aa>0) && (aa<m_dbCeilalpha))
			{
				if (TakeStep(nIndex,j))
				//if (TakeStep(j,nIndex))
				{
					return 1;
				}
			}
		}
		//loop 3
		rnd0_1=rand()/RAND_MAX;
		rnd=(int)(rnd0_1*m_nEndSupporti);
		for( jj=rnd; jj<m_nEndSupporti+rnd; jj++)
		{
			int j=jj % m_nEndSupporti;
			if (TakeStep(nIndex,j))
			//if (TakeStep(j,nIndex))
			{
				return 1;
			}
		}
		
	}
	return 0;
}

//##ModelId=3DD99EC803A4
int CSVM::TakeStep(int nIndexi, int nIndexj)
{
	int yi,yj,s;
	double alphai,alphaj,erri,errj,dbLow,dbHigh,eta,a1,a2,c1,c2,LD_l,LD_h,new_b,delta_b,tempi,tempj;
	// TODO: Add your specialized code here.
	if (nIndexi==nIndexj)
	{
		return 0;
	}
	//erri
	alphai=GetAlpha(nIndexi);
	yi=GetY(nIndexi);
	if ((alphai>0)&&(alphai<m_dbCeilalpha))
	{	
		erri=GetError(nIndexi);
	}
	else 
	{
		erri=Learn(nIndexi)-yi;
	}
	//errj
	alphaj=GetAlpha(nIndexj);
	yj=GetY(nIndexj);
	if ((alphaj>0)&&(alphaj<m_dbCeilalpha))
	{
		errj=GetError(nIndexj);
	}
	else
	{
		errj=Learn(nIndexj)-yj;
	}
	s=yi*yj;
	
	//ComputeLH(nIndexi,nIndexj,&dbLow, &dbHigh);
	//Compute start
	

	double gama;

	if (yi==yj)
	{
		gama=alphai+alphaj;
		if (gama>m_dbCeilalpha)
		{
			dbLow=gama-m_dbCeilalpha;
			dbHigh=m_dbCeilalpha;
		}
		else
		{
			dbLow=0;
			dbHigh=gama;
		}
	}
	else
	{
		gama=alphai-alphaj;
		if (gama>0)
		{
			dbLow=0;
			dbHigh=m_dbCeilalpha-gama;
		}
		else
		{
			dbLow=-gama;
			dbHigh=m_dbCeilalpha;
		}
	}
	//compute end
	//if (fabs(dbLow-dbHigh)<m_equalprecision)
	if (dbLow==dbHigh)
	{
		return 0;
	}
	//eta=2*k12-k11-k22;
	double k11,k12,k22;
	k11=Kernel(nIndexi,nIndexi);
	k12=Kernel(nIndexi,nIndexj);
	k22=Kernel(nIndexj, nIndexj);
	eta=2*Kernel(nIndexi,nIndexj)-Kernel(nIndexi,nIndexi)-Kernel(nIndexj, nIndexj);

	if (eta<0)
	{
		a2=alphaj-yj*(erri-errj)/eta;
		if (a2<dbLow)
		{
			a2=dbLow;
		}
		else
		{
			if (a2>dbHigh)
			{
				a2=dbHigh;
			}
		}
	}
	else 
	{
		c1=eta/2;
		c2=yj*(erri-errj)-eta*alphaj;
		LD_l=c1*dbLow*dbLow+c2*dbLow;
		LD_h=c1*dbHigh*dbHigh+c2*dbHigh;
		if((LD_l-LD_h)>m_eps)
		{
			a2=dbLow;
		}
		else
		{
			if((LD_l-LD_h)<-m_eps)
			{
				a2=dbHigh;
			}
			else
			{
				a2=alphaj;
			}
		
		}
	}
	if (fabs(a2-alphaj)<m_eps*(a2+alphaj+m_eps))
	{
		return 0;
	}
	a1=alphai+s*(alphaj-a2);
	//modified by liytao Nov21
	if (a1<0)
	{
		a2+=s*a1;
		a1=0;

	}
	else if (a1>m_dbCeilalpha)
	{
		a2+=s*(a1-m_dbCeilalpha);
		a1=m_dbCeilalpha;
	}
	//modify end
	
	
	//update b

	if((a1>0)&&(a1<m_dbCeilalpha))
	{
		new_b= m_b + erri + yi * (a1 - alphai) * Kernel(nIndexi,nIndexi) + yj * (a2 - alphaj) * Kernel(nIndexi,nIndexj);
	}
	else
	{
		if((a2>0) && (a2<m_dbCeilalpha))
		{
			new_b=m_b + errj + yi * (a1 - alphai) * Kernel(nIndexi,nIndexj) + yj * (a2 - alphaj) * Kernel(nIndexj,nIndexj);
		}
		else
		{
			//modified by liytao 20021227
			//new_b=((new_b= m_b + erri + yi * (a1 - alphai) * Kernel(nIndexi,nIndexi) + yj * (a2 - alphaj) * Kernel(nIndexi,nIndexj))+(new_b=m_b + errj + yi * (a1 - alphai) * Kernel(nIndexi,nIndexj) + yj * (a2 - alphaj) * Kernel(nIndexj,nIndexj)))/2;
			double new_b1,new_b2;
			new_b1= m_b + erri + yi * (a1 - alphai) * Kernel(nIndexi,nIndexi) + yj * (a2 - alphaj) * Kernel(nIndexi,nIndexj);	
			new_b2=m_b + errj + yi * (a1 - alphai) * Kernel(nIndexi,nIndexj) + yj * (a2 - alphaj) * Kernel(nIndexj,nIndexj);
			new_b=(new_b1+new_b2)/2;
		}
	}
	delta_b = new_b - m_b;
	m_b = new_b;
	
	//update weight[]
	if(m_bIsLinear==true)

	{
		tempi = yi * (a1 - alphai);
		tempj = yj * (a2 - alphaj);
//		if (m_bMemDB)
//		{
			for(int i=0; i<m_nColNum-1; i++)
			//(*m_weight)[i]+=tempi*(*vi)[i]+tempj*(*vj)[i];
			pWeight(i)= pWeight(i)+tempi*pVec((m_nColNum-1)*nIndexi+i)+tempj*pVec((m_nColNum-1)*nIndexj+i);
//		}
//		else
//		{
//
//			GetVector(nIndexi,m_dbVi);
//			GetVector(nIndexj,m_dbVj);
//			for(int i=0; i<m_nColNum-1; i++)
//			m_weight[i]+=tempi*m_dbVi[i]+tempj*m_dbVj[i];
// 		}
	}

	//update Error[]
	tempi = yi * (a1 - alphai);
	tempj = yj * (a2 - alphaj);
	for(int i=0; i<m_nEndSupporti; i++)
	{
		double dbkerni,dbkernj;
		double temp;
		alphai=GetAlpha(i);
		if((alphai>0) && (alphai<m_dbCeilalpha))
		{
			dbkerni=Kernel(nIndexi,i);
			dbkernj=Kernel(nIndexj,i);
			temp=GetError(i);
			temp+= tempi *dbkerni  + tempj*dbkernj - delta_b;
			SetError(i, temp);
		}
	}
	SetError(nIndexi,0);
	SetError(nIndexj,0);

	SetAlpha(nIndexi,a1);
	SetAlpha(nIndexj,a2);
	return 1;
}

//##ModelId=3DD99F2B01F8
double CSVM::SMOCalc()
{
	int nNumChange=0,nVec;
	double alpha;
	int examineAll=1;
	int nPos=6;
	
	while (nNumChange>0||examineAll)
	{
//		if (m_pPrgWnd!=NULL)
//			m_pPrgWnd->SendMessage(m_prgMsg,nPos,0);

		nNumChange=0;

		if( examineAll)
		{
			for(nVec=0;nVec<m_nRowNum;nVec++)
			{
				//add by liytao 20021223
				if (m_pbCheck!=NULL)
				{
					if (*m_pbCheck)
						return 1; 
				}
				//add end
 				nNumChange+=Examine(nVec);			
			}

		}
		else
		{

			for(nVec=0;nVec<m_nRowNum;nVec++)
			{
				//add by liytao 20021223
				if (m_pbCheck!=NULL)
				{
					if (*m_pbCheck)
						return 1; 
				}
				//add end
				alpha=GetAlpha(nVec);
				if ((alpha!=0)&&(alpha!=m_dbCeilalpha))
					nNumChange+=Examine(nVec);			
			}
		}

	    if (examineAll==1)
			examineAll=0;
		else if (nNumChange==0)
			examineAll=1;
		if (nPos<60)
			nPos++;

	}

	return (double)0;
}

//##ModelId=3DD99FAF01E4
double CSVM::Evaluate()
{
	// TODO: Add your specialized code here.
	int nWrongSum=0,nRightSum=0;
	bool bRs;
	for(int i=0;i<m_nRowNum;i++)
	{
		bRs=EvaluateVector(i);
		if (bRs)
		nRightSum++;
		else
		nWrongSum++;
	}
	return  ((double)nRightSum/((double)nRightSum+(double)nWrongSum));
	//return (double)0;
}



bool CSVM::InitData()
{
	CFieldList *pFieldList;
	int i;

	//add by liytao 20021205
	
	//check field type and Y value
	pFieldList=m_pDataSource->m_pFieldList;
	int nFC=pFieldList->GetFieldCount();
	if (nFC<=1)
		return false;
	CField *fld;
	bool bInvalid=false;
	double dbV;
	int nV;
	for (i=0;i<nFC&&!bInvalid;i++)
	{
		fld=pFieldList->FieldByIndex(i);
		if (fld->GetFieldType()==fString)
			bInvalid=true;
	}
	if (bInvalid)
		return false;
	m_pDataSource->First();
	
	while (!m_pDataSource->Eof()&&!bInvalid)
	{
		fld=pFieldList->FieldByIndex(nFC-1);
		if (fld->IsNull())
		{
			bInvalid=true;
			continue;
		}
		if (fld->GetFieldType()==fDouble)
		{
			dbV=fld->GetAsDouble();
			if (fabs(dbV-1)>m_equalprecision&&fabs(dbV+1)>m_equalprecision)
				bInvalid=true;
		}
		if (fld->GetFieldType()==fInt)
		{
			nV=fld->GetAsInteger();
			if (nV!=-1&&nV!=1)
				bInvalid=true;
		}

		m_pDataSource->Next();
	}

	if (bInvalid)
		return false;

	//check end


	//add end

	
	
	
	m_nColNum=m_pDataSource->m_pFieldList->GetFieldCount();
	m_nRowNum=m_pDataSource->GetRecordCount();

//	if (!m_bModeSet)
//	{
//		if ((m_nColNum-1)*m_nRowNum<MAX_DATASIZE)
//			m_bMemDB=true;
//		else
//			m_bMemDB=false;
//	}
//	m_bMemDB=false; //test add by liytao 20030124 
//	if (m_bMemDB)
//	{

		//create vector
		pAlpha.create(m_nRowNum*2,0);	//alpha and err
		pVec.create((m_nColNum-1)*m_nRowNum,0); //vectors
		pWeight.create(m_nColNum-1);
		pY.create(m_nRowNum); 
		pFieldList=m_pDataSource->m_pFieldList;
		int nCur=0;
		nFC=pFieldList->GetFieldCount();
		
		m_pDataSource->First();
		while (!m_pDataSource->Eof())
		{
			//init vector
			for (int i=0;i<nFC-1;i++)
			{
				pVec(nCur*(m_nColNum-1)+i)=pFieldList->FieldByIndex(i)->GetAsDouble();	
			}
			//init y
			pY(nCur)=pFieldList->FieldByIndex(nFC-1)->GetAsInteger();	



			m_pDataSource->Next();
			nCur++;
		}
		m_nEndSupporti=m_nRowNum;
		return true;
//	}
	//create vector
//	m_dbVi.create(m_nColNum-1);
//	m_dbVj.create(m_nColNum-1);
	
	//set
	//create alpha array table
	m_pAlphaAccess=new CDataAccess(); 
	m_pAlphaAccess->NewFile();
	pFieldList=m_pAlphaAccess->m_pFieldList;
	pFieldList->Clear(); 
	CField *		pTempField;
	pTempField = pFieldList->CreateField('F');
	pTempField->SetFieldName("Alpha");
	pFieldList->Add(pTempField);
	pTempField =pFieldList->CreateField('F');
	pTempField->SetFieldName("Error");
	pFieldList->Add(pTempField);
	m_pAlphaAccess->Post(); //add by liytao 20030124

	for(i =0;i<m_nRowNum;i++)
	{
		m_pAlphaAccess->Append();
		pFieldList->FieldByIndex(0)->SetAsDouble(0);
		pFieldList->FieldByIndex(1)->SetAsDouble(0);
		m_pAlphaAccess->Post();
	}
	//m_pAlphaAccess->SaveFile(m_strAlphaTableName);
	//m_pAlphaAccess->LoadFile(m_strAlphaTableName);
	m_nEndSupporti=m_nRowNum;
	//set m_weight=0	
// 	m_weight.create(m_nColNum-1,0);
	return true;
}
CSVM::CSVM()
{
	m_tolerance=0.1;
	m_dbCeilalpha=0.05;
	m_equalprecision=0.0000001;
	m_eps=0.001;
	m_b=0;
	m_twosigmasquared=2;
	m_bIsLinear=false;//true;//
//	m_bMemDB=false;
//	m_bModeSet= false;
	this->m_pDataSource=NULL;
	this->m_pPredictAccess=NULL;
	// add by liytao 20021212
	m_pAlphaAccess=NULL;

	m_dbCorrectRate=0;
	m_SVNum=0;
	m_pbCheck=NULL;
	//add end
	//add by liytao 20021227
//	m_pPrgWnd=NULL;
//	m_pRecIndexs=NULL;
	//add end
}

CSVM::CSVM(CDataAccess *pSourceAccess)
{
	m_tolerance=0.1;
	m_dbCeilalpha=0.05;
	m_equalprecision=0.0000001;
	m_eps=0.001;
	m_b=0;
	m_twosigmasquared=2;
	m_bIsLinear=false;//true;//
//	m_bMemDB=false;
//	m_bModeSet= false;
	this->m_pDataSource=pSourceAccess;
	this->m_pPredictAccess=NULL;


	// add by liytao 20021212
	m_pAlphaAccess=NULL;
	m_dbCorrectRate=0;

	//add end
	m_pbCheck=NULL;
	//add by liytao 20021227
// 	m_pPrgWnd=NULL;
	m_pRecIndexs=NULL;
	//add end
}

CSVM::~CSVM()
{
	EmptyData();
}


double CSVM::Learn(int nIndexi)
{
	double dbRs,dbSum;
	double alphai,yi;
	
	alphai=GetAlpha(nIndexi);
	int i;
// 	if (!m_bMemDB)
// 		GetVector(nIndexi,m_dbVi);
	yi=GetY(nIndexi);
	if (m_bIsLinear)
	{
		//dbRs= ::dot(*m_weight,*vi)-m_b;
//		if (m_bMemDB)
			dbRs=DotWeight(nIndexi)-m_b;
//		else
// 			dbRs= ::dot(m_weight,m_dbVi)-m_b;
	}
	else
	{
		dbSum=0;
		for(i =0;i<m_nEndSupporti;i++)
		{
			//m_pDataSource->SetCurRecNo(i);
			alphai=GetAlpha(i);
			yi=GetY(i);
			if (alphai>0)
				dbSum+=alphai*yi*Kernel(i,nIndexi);
		}
		dbRs=dbSum-m_b;

	}
	//delete vi;
	return dbRs;
}

double CSVM::Kernel(int nIndexi, int nIndexj)
{
	double dbRs=0,dbSum=0;
	//CDoubleVector vi(m_nColNum-1),vj(m_nColNum-1);//*vi ,*vj;

//	if (m_bMemDB)
//	{
		if (m_bIsLinear)
		{
			dbRs=DotProduct(nIndexi,nIndexj);

		}
		else
		{

			//KERNFUNC_GAUSS                         //高斯核函数
			//KERNFUNC_MULTINORMINAL         //多项式核函数
			if (m_nKernFuncKind==KERNFUNC_GAUSS)
			{
				//exp(-(||xi1-xi2||)^2/two_sigma_squared)
				dbSum=DotProduct(nIndexi,nIndexj);
				dbSum*=-2;
			//dbSum+=::dot(vi,vi)+::dot(vj,vj);
				dbSum+=DotProduct(nIndexi,nIndexi)+DotProduct(nIndexj,nIndexj);
				dbRs=exp(-dbSum/m_twosigmasquared);
			}
			//KERNFUNC_MULTINORMINAL		
			if (m_nKernFuncKind==KERNFUNC_MULTINORMINAL)
			{
				//(xi.xj+1)^q
				dbSum=1;
				for (int i=0;i<m_nOrder;i++)
				{
					dbSum*=DotProduct(nIndexi,nIndexj)+1;
				}
				dbRs=dbSum;
			}

		}
		return dbRs;
//	}


	
//	GetVector(nIndexi,m_dbVi);
//	GetVector(nIndexj,m_dbVj);
//	dbRs=Kernel(m_dbVi,m_dbVj);
//		
//	return dbRs;	
}
double CSVM::Kernel(CDoubleVector vi, CDoubleVector vj)
{
	double dbRs,dbSum;
	if (m_bIsLinear)
	{
		//dbRs=::dot(vi,vj);
		dbRs=::dot(vi,vj);

	}
	else
	{

		//KERNFUNC_GAUSS                         //高斯核函数
		//KERNFUNC_MULTINORMINAL         //多项式核函数
		if (m_nKernFuncKind==KERNFUNC_GAUSS)
		{
		//exp(-(||xi1-xi2||)^2/two_sigma_squared)
		dbSum=::dot(vi,vj);
		dbSum*=-2;
		dbSum+=::dot(vi,vi)+::dot(vj,vj);
		dbRs=exp(-dbSum/m_twosigmasquared);
	
		}
		//KERNFUNC_MULTINORMINAL		
		if (m_nKernFuncKind==KERNFUNC_MULTINORMINAL)
		{
		//(xi.xj+1)^q
			dbSum=1;
			for (int i=0;i<m_nOrder;i++)
			{
				dbSum*=::dot(vi,vj)+1;
			}
			dbRs=dbSum;
		}

	}
	return dbRs;

}

double CSVM::Kernel(double *pVi, double *pVj)
{
	double dbRs=0,dbSum=0;
	//CDoubleVector vi(m_nColNum-1),vj(m_nColNum-1);//*vi ,*vj;

	if (m_bIsLinear)
	{
		dbSum=DotProduct(pVi,pVj);

	}
	else
	{

		//KERNFUNC_GAUSS                         //高斯核函数
		//KERNFUNC_MULTINORMINAL         //多项式核函数
		if (m_nKernFuncKind==KERNFUNC_GAUSS)
		{
			//exp(-(||xi1-xi2||)^2/two_sigma_squared)

			dbSum=DotProduct(pVi,pVj);
			dbSum*=-2;
			//dbSum+=::dot(vi,vi)+::dot(vj,vj);
			dbSum+=DotProduct(pVi,pVi)+DotProduct(pVj,pVj);
			dbRs=exp(-dbSum/m_twosigmasquared);
		}
		//KERNFUNC_MULTINORMINAL		
		if (m_nKernFuncKind==KERNFUNC_MULTINORMINAL)
		{
			//(xi.xj+1)^q
			dbSum=1;
			for (int i=0;i<m_nOrder;i++)
			{
				dbSum*=DotProduct(pVi,pVj)+1;
			}
			dbRs=dbSum;
		}

	}
	return dbRs;

}

bool CSVM::Export(TCLFile &theFile)
{
	int i=0, nSize=0;

	theFile << m_bIsLinear;
	theFile << m_nOrder;
	theFile << m_nKernFuncKind;
	theFile << m_twosigmasquared;
	theFile << m_nEndSupporti;
	theFile << m_tolerance;
	theFile << m_b;
	theFile << m_dbCorrectRate;
	theFile << m_dbCeilalpha;
	theFile << m_equalprecision;
	theFile << m_eps;	
	theFile << m_nEndSupporti;
	//	CDoubleVector pVec;
	nSize = pVec.vlen();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pVec[i];
	}
	//	CIntVector pY;
	nSize = pY.vlen();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pY[i];
	}
	//	CDoubleVector pAlpha;
	nSize = pAlpha.vlen();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pAlpha[i];
	}
	//	CDoubleVector pWeight;
	nSize = pWeight.vlen();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pWeight[i];
	}
	theFile << m_RealYs.m_cType;	
	theFile << m_RealYs.m_nCount;
	for (i=0; i<m_RealYs.m_nCount; i++)
	{
		CSVMVariant *pV = (CSVMVariant *)m_RealYs.m_Vars.GetAt(i);
		switch(m_RealYs.m_cType)
		{
		case fBoolean:
			theFile << pV->bVal;
		case fInt:
		case fDate:
			theFile << pV->nVal;
			break;
		case fDouble:
			theFile << pV->dbVal;
			break;
		case fString:
			theFile << pV->strVal;
			break;
		}
	}
	return true;
}

bool CSVM::Import(TCLFile &theFile)
{
	int i=0, nSize=0;
	
	theFile >> m_bIsLinear;
	theFile >> m_nOrder;
	theFile >> m_nKernFuncKind;
	theFile >> m_twosigmasquared;
	theFile >> m_nEndSupporti;
	theFile >> m_tolerance;
	theFile >> m_b;
	theFile >> m_dbCorrectRate;
	theFile >> m_dbCeilalpha;
	theFile >> m_equalprecision;
	theFile >> m_eps;	
	theFile >> m_nEndSupporti;
	//	CDoubleVector pVec;
	theFile >> nSize;
	pVec.create(nSize);
	for (i=0; i<nSize; i++)
	{
		theFile >> pVec[i];
	}
	//	CIntVector pY;
	theFile >> nSize;
	pY.create(nSize);
	for (i=0; i<nSize; i++)
	{
		theFile >> pY[i];
	}
	//	CDoubleVector pAlpha;
	theFile >> nSize;
	pAlpha.create(nSize);
	for (i=0; i<nSize; i++)
	{
		theFile >> pAlpha[i];
	}
	//	CDoubleVector pWeight;
	theFile >> nSize;
	pWeight.create(nSize);
	for (i=0; i<nSize; i++)
	{
		theFile >> pWeight[i];
	}
	theFile >> m_RealYs.m_cType;	
	theFile >> m_RealYs.m_nCount;
	for (i=0; i<m_RealYs.m_nCount; i++)
	{
		CSVMVariant *pV = new CSVMVariant;
		switch(m_RealYs.m_cType)
		{
		case fBoolean:
			theFile >> pV->bVal;
		case fInt:
		case fDate:
			theFile >> pV->nVal;
			break;
		case fDouble:
			theFile >> pV->dbVal;
			break;
		case fString:
			theFile >> pV->strVal;
			break;
		}
		m_RealYs.m_Vars.Add(pV);
	}
	return true;
}


CDoubleVector CSVM::GetVector(int nIndex)
{
	CDoubleVector pVTemp(m_nColNum-1);
	CFieldList *pFList;
	pFList=m_pDataSource->m_pFieldList;
	m_pDataSource->SetCurRecNo(nIndex+1);
	int i;
	for(i =0;i<m_nColNum-1;i++)
	{
		pVTemp[i]=pFList->FieldByIndex(i)->GetAsDouble();
	}
	return pVTemp;
}

void CSVM::GetVector(int nIndex, CDoubleVector& dbVector)
{
//	if (m_bMemDB)
//	{
		int i,nC;
		for(i =0;i<m_nColNum-1;i++)
		{
			nC=nIndex*(m_nColNum-1)+i;
			dbVector[i]=pVec(nC);
		}
//		return ;
//
//	}


//	CFieldList *pFList;
//	pFList=m_pDataSource->m_pFieldList;
//	m_pDataSource->SetCurRecNo(nIndex+1);
//	int i;
//	for(i =0;i<m_nColNum-1;i++)
//	{
//		dbVector[i]=pFList->FieldByIndex(i)->GetAsDouble();
//	}
//	return;
}


double CSVM::GetAlpha(int nIndex)
{
//	if (m_bMemDB)
//	{
		int nC;
		nC=nIndex*2;
		return pAlpha(nC);
//
//	}

//	double dbRs;
//	CFieldList	*pAlphaFList=m_pAlphaAccess->m_pFieldList;
//	m_pAlphaAccess->SetCurRecNo(nIndex+1);
//	dbRs=pAlphaFList->FieldByIndex(0)->GetAsDouble();	
//	return dbRs;
}

void CSVM::SetAlpha(int nIndex, double dbValue)
{
//	if (m_bMemDB)
//	{
		pAlpha(nIndex*2)=dbValue;
		return;
//	}

//	CFieldList	*pAlphaFList=m_pAlphaAccess->m_pFieldList;
//	m_pAlphaAccess->SetCurRecNo(nIndex+1);
//	m_pAlphaAccess->Edit();
//	pAlphaFList->FieldByIndex(0)->SetAsDouble(dbValue);
//	m_pAlphaAccess->Post();
}

int CSVM::GetY(int nIndex)
{
// 	int nRs;
//	if (m_bMemDB)
//	{
		return pY(nIndex);
//
//	}


//	CFieldList	*pFList=m_pDataSource->m_pFieldList;
//	m_pDataSource->SetCurRecNo(nIndex+1);
//	nRs=pFList->FieldByIndex(m_nColNum-1)->GetAsInteger();	
//	return nRs;
}


double CSVM::GetError(int nIndex)
{
//	if (m_bMemDB)
//	{
		int nC;
		nC=nIndex*2+1;
		return pAlpha(nC);
//
//	}
//	double dbRs;
//	CFieldList	*pAlphaFList=m_pAlphaAccess->m_pFieldList;
//	m_pAlphaAccess->SetCurRecNo(nIndex+1);
//	dbRs=pAlphaFList->FieldByIndex(1)->GetAsDouble();	
//	return dbRs;
}


void CSVM::SetError(int nIndex, double dbValue)
{
//	if (m_bMemDB)
//	{
		pAlpha(nIndex*2+1)=dbValue;
//		return;
//	}

//	CFieldList	*pAlphaFList=m_pAlphaAccess->m_pFieldList;
//	m_pAlphaAccess->SetCurRecNo(nIndex+1);
//	m_pAlphaAccess->Edit();
//	pAlphaFList->FieldByIndex(1)->SetAsDouble(dbValue);
//	m_pAlphaAccess->Post();
}



void CSVM::ComputeLH(int nIndexi,int nIndexj,double* pLVal,double *pHVal)
{

	int y1,y2;
	double alpha1,alpha2,gama;

	y1=GetY(nIndexi);
	y2=GetY(nIndexj);
	alpha1=GetAlpha(nIndexi);
	alpha2=GetAlpha(nIndexj);
	if (y1==y2)
	{
		gama=alpha1+alpha2;
		if (gama>m_dbCeilalpha)
		{
			*pLVal=gama-m_dbCeilalpha;
			*pHVal=m_dbCeilalpha;
		}
		else
		{
			*pLVal=0;
			*pHVal=gama;
		}
	}
	else
	{
		gama=alpha1-alpha2;
		if (gama>0)
		{
			*pLVal=0;
			*pHVal=m_dbCeilalpha-gama;
		}
		else
		{
			*pLVal=-gama;
			*pHVal=m_dbCeilalpha;
		}
	}
}




  
bool CSVM::EvaluateVector(int nIndex)
{
	double db;
	return EvaluateVector(nIndex,&db);
}

bool CSVM::EvaluateVector(int nIndex,double *pdbRs)
{
	double	dbRs,dbSum=0;
	double alphai;
	int y,yi;
	bool	bCorrect=false;
// 	if (!m_bMemDB)
// 		GetVector(nIndex,m_dbVi);
	y=GetY(nIndex);
	if (m_bIsLinear)
	{
		//w.v-b
		//dbRs=::dot(*pCurV,*m_weight)-m_b;
//		if (m_bMemDB)
			dbRs=DotWeight(nIndex)-m_b;
//		else
// 			dbRs=::dot(m_dbVi,m_weight)-m_b;
		if (dbRs>0&&y==1)
			bCorrect=true;
		if (dbRs<0&&y==-1)
			bCorrect=true;
	}
	else
	{
		for (int i=0;i<m_nRowNum;i++)
		{
			alphai=GetAlpha(i);
			if ((alphai-m_equalprecision)>0)
			{
				yi=GetY(i);
//				if (!m_bMemDB)
//				{
//					GetVector(i,m_dbVj);
//					dbSum+=Kernel(m_dbVi,m_dbVj)*alphai*yi;
//				}
//				else
					dbSum+=Kernel(nIndex,i)*alphai*yi;
					
			}
		}
		dbRs=dbSum-m_b;
		if (dbRs>0&&y==1)
			bCorrect=true;
		if (dbRs<0&&y==-1)
			bCorrect=true;
		
	}
	//delete pCurV;
	*pdbRs=dbRs;
	return bCorrect;
}

int CSVM::FindNextSVectorIndex(int nCur)
{
	double alpha;
	int nNext;
	bool bFound;
	if (nCur<0)
		nNext=0;
	else nNext=nCur+1;
	bFound=false;
	while (!bFound&& nNext<m_nRowNum)
	{
		
		alpha=GetAlpha(nNext);
		if(alpha<0)
			break;
		if (alpha==0)
			nNext++;
		if (alpha>0)
			bFound=true;
	}
	if (!bFound)
		return -1;
	else 
		return nNext;
}

bool CSVM::EmptyData()
{
	if (m_pAlphaAccess!=NULL)
	{
		//m_pAlphaAccess->SaveFile(m_strAlphaTableName); modified by liytao 20021220 
		delete m_pAlphaAccess;
	}

	

	if (m_pDataSource!=NULL)
		delete m_pDataSource;
	//add by liytao 20021220
	//if(m_pPredictAccess!=NULL)
	//	delete m_pPredictAccess;
	//add end
	return true;
}

int CSVM::GetRowNum()
{
	return m_nRowNum;
}
int CSVM::GetColNum()
{
	return m_nColNum;
}

void CSVM::SetParam(int  nLinearMethod,int nKernFuncKind,int nSigmasquared,int nOrder,double dbCeilAlpha,double dbTolerance,double dbEps)
{
	if (nLinearMethod == 0)
		m_bIsLinear= true;
	else
		m_bIsLinear = false;
	m_nKernFuncKind=nKernFuncKind;
	m_twosigmasquared=nSigmasquared;
	m_nOrder=nOrder;
	m_dbCeilalpha= dbCeilAlpha;
	m_eps=dbEps;
	m_tolerance=dbTolerance;


}

void CSVM::SetAlphaTableName(CTString strTName)
{
	int nBufferLength=100;
	CTString strFieldName;
	TCSystem::GetWorkPath(strFieldName);
	m_strAlphaTableName=strFieldName+_T("\\");
	m_strAlphaTableName+=strTName;
}

double CSVM::GetWeight(int nIndex)
{
	if (nIndex>m_nColNum-2||nIndex<0)
		return (double)-1;
//	if (m_bMemDB)
		return pWeight(nIndex);
//	else
// 		return m_weight[nIndex];
}

double CSVM::GetB()
{
	return m_b;
}

bool CSVM::IsLinear()
{
	return m_bIsLinear;
}

double CSVM::GetCeilAlpha()
{
	return m_dbCeilalpha;
}

double CSVM::GetTolerance()
{
	return m_tolerance;
}

double CSVM::DotProduct(int nIndexi, int nIndexj)
{
//	if (m_bMemDB)
//	{
		int i,nC;
		double dbVi,dbVj,dbSum=0;
		for(i =0;i<m_nColNum-1;i++)
		{
			nC=nIndexi*(m_nColNum-1)+i;
			dbVi=pVec(nC);
			nC=nIndexj*(m_nColNum-1)+i;
			dbVj=pVec(nC);

			dbSum+=dbVi*dbVj;
		}
		return dbSum;
//	}
	return 0;
}
double CSVM::DotProduct(double *pVi, double *pVj)
{
	//if (m_bMemDB) delete by liytao 20030124
//	{
		int i;
		double dbVi,dbVj,dbSum=0;
		for(i =0;i<m_nColNum-1;i++)
		{
			dbVi=*(pVi+i);
			dbVj=*(pVj+i);

			dbSum+=dbVi*dbVj;
		}
		return dbSum;
//	}
	return 0;
}
double CSVM::DotWeight(int nIndex)
{
//	if (m_bMemDB)
//	{
		int i,nC;
		double dbWeight,dbVj,dbSum=0;
		for(i =0;i<m_nColNum-1;i++)
		{
			dbWeight= pWeight(i);
			nC=nIndex*(m_nColNum-1)+i;
			dbVj=pVec(nC);

			dbSum+=dbWeight*dbVj;
		}
		return dbSum;
//	}
	return 0;
}

void CSVM::SetStoreMode(bool bMemDB)
{
//	m_bMemDB=bMemDB;
//	m_bModeSet=true;
}

CTString CSVM::GetKernFunc()
{
	if (IsLinear())
		return _T("");
	else
	{
		CTString str;
		if (m_nKernFuncKind==KERNFUNC_GAUSS)
		{
			//exp(-(||xi1-xi2||)^2/two_sigma_squared)
			str.Format("exp(-(||xi1-xi2||)^2/%d)",m_twosigmasquared);

		}
		//KERNFUNC_MULTINORMINAL		
		if (m_nKernFuncKind==KERNFUNC_MULTINORMINAL)
		{
				//(xi.xj+1)^q
			str.Format("(xi.xj+1)^%d",m_nOrder);
		
		}	
		return str;
	}
}

double CSVM::GetEps()
{
	return m_eps;
}

CDataAccess* CSVM::GetSourceAccess()
{
	return this->m_pDataSource;

}

void CSVM::SetSourceAccess(CDataAccess *pDataAccess)
{
	this->m_pDataSource=pDataAccess;

}

void CSVM::SetPredict(CDataAccess *pAccess)
{
	m_pPredictAccess=pAccess;
	if (m_pDataSource == NULL)
		m_pDataSource = pAccess;
}

int CSVM::GetPredictY(int nIndex)
{
	m_pPredictAccess->SetCurRecNo(nIndex+1);
	CDoubleVector dbVector(m_PredictVarIndex.nVarCount);
	double	dbRs,dbSum=0;
	double alphai;
	int yi;
	bool	bCorrect=false;
	CFieldList *pFList;
	pFList=m_pPredictAccess->m_pFieldList;
	int i;
	bool bInvalid=false;
	for(i =0;i<m_PredictVarIndex.nVarCount;i++)
	{
		//add by liytao 20021230
		//if(pFList->FieldByIndex(i)->IsNull()) add by liytao 20030124
		if(pFList->FieldByIndex(m_PredictVarIndex.pVarIndexs[i])->IsNull())
		{
			bInvalid=true;
			break;//add by liytao 20030126
		}
		//add end
		//dbVector[i]=pFList->FieldByIndex(i)->GetAsDouble();add by liytao 20030124
		dbVector[i]=pFList->FieldByIndex(m_PredictVarIndex.pVarIndexs[i])->GetAsDouble();
// 		TRACE("%.2f",dbVector[i]);
	}

	if( bInvalid)
	{
		return 0;
	}
	if (m_bIsLinear)
	{
		//w.v-b
		//dbRs=::dot(*pCurV,*m_weight)-m_b;
		int i,nC;
		double dbWeight,dbVj,dbSum=0;
//		if (m_bMemDB)
//		{
			for(i =0;i<m_PredictVarIndex.nVarCount;i++)
			{
				dbWeight=pWeight(i);
				nC=i;
				dbVj=dbVector(nC);
				dbSum+=dbWeight*dbVj;
			}
//		}
//		else
//		{
//			for(i =0;i<m_nColNum-1;i++)
//			{
//				dbWeight=m_weight[i];
//				//nC=nIndex*(m_nColNum-1)+i; modified by liytao 20021219
//				nC=i;
//				dbVj= dbVector(nC);
//
//				dbSum+=dbWeight*dbVj;
//			}
// 		}
		dbRs=dbSum-m_b;
	}
	else
	{
		CDoubleVector pVTemp(m_PredictVarIndex.nVarCount);
		CFieldList *pFList;
		pFList=m_pDataSource->m_pFieldList;
		dbSum=0;
		for (int i=0;i<m_pDataSource->GetRecordCount();i++)
		{
			alphai=GetAlpha(i);
			if ((alphai-m_equalprecision)>0)
			{
				yi=GetY(i);
// 				TRACE("%.2f",yi);
				m_pDataSource->SetCurRecNo(i+1);
				int j;
				for(j =0;j<m_PredictVarIndex.nVarCount;j++)
				{
					pVTemp[j]=pFList->FieldByIndex(j)->GetAsDouble();
// 					TRACE("vec:%.2f",pVTemp[j]);
				}	
				dbSum+=Kernel(dbVector,pVTemp)*alphai*yi;
// 				TRACE("Sum(i)=%.2f\n",dbSum);	
			}
		}
// 		TRACE("%.2f\n",m_b);
		dbRs=dbSum-m_b;
	}
	if (dbRs>0)
		return 1;
	else
		return -1;
}

void CSVM::SetCorrectRate(double dbVal)
{
	m_dbCorrectRate=dbVal;
}

double CSVM::GetCorrectRate()
{
	return m_dbCorrectRate;
}

void CSVM::SetStopCheck(bool *bCheck)
{
	if(bCheck!=NULL)
		m_pbCheck=bCheck;

}

bool CSVM::GetCheck()
{
	if (m_pbCheck!=NULL)
		return *m_pbCheck;
	else return false;

}

void CSVM::SetPrgWnd(/*CWnd *pPrgWnd,UINT prgMsg*/)
{
/*
	m_pPrgWnd=pPrgWnd;
	m_prgMsg=prgMsg;
*/
}

