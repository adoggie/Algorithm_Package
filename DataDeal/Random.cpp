// Random.cpp: implementation of the CRandom class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Random.h"
#include "../cpknl/cpknl.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../DataInterface/DataInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRandom::CRandom()
{
	m_nSeed   = 0;
	m_bRandom = false;
	
	m_dMean   = 0;  //均值
	m_dVar    = 0;  //方差
	m_nNumber = 0;  //总数 

	m_szMethod  = "";
	m_szVarName = "";
}

CRandom::~CRandom()
{

}

bool CRandom::Parse(const string &VO)
{
	AnaWord aw;
	AnaSyntax syn;
	CTString szTemp("");
	
	aw.Import(VO);
	if (AnaSyntax::exist(SEED,aw)) 
	{
		m_bRandom = true;
		m_nSeed = atoi(AnaSyntax::getSingleStr(SEED, aw));
	}	
	else
	{
		m_nSeed   = 0;
		m_bRandom = false;
	}
	if (AnaSyntax::exist(VARDEST, aw))
	{
		m_szVarName = AnaSyntax::getSingleStr(VARDEST, aw);
	}
	if (AnaSyntax::exist(SAMPLENUM,aw)) 
	{
		m_nNumber = atoi(AnaSyntax::getSingleStr(SAMPLENUM, aw));
	}
	if (m_nNumber < MIN_NUM) m_nNumber = MIN_NUM;
	if (AnaSyntax::exist(MEAN,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(MEAN, aw));
	}
	if (AnaSyntax::exist(E,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(E, aw));
	}
	if (AnaSyntax::exist(VAR,aw)) 
	{
		m_dVar = atof(AnaSyntax::getSingleStr(VAR, aw));
	}
	if (AnaSyntax::exist(MIN,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(MIN, aw));
	}
	if (AnaSyntax::exist(MAX,aw)) 
	{
		m_dVar = atof(AnaSyntax::getSingleStr(MAX, aw));
	}
	if (AnaSyntax::exist(COUNT,aw)) 
	{
		m_dVar = atof(AnaSyntax::getSingleStr(COUNT, aw));
	}
	if (AnaSyntax::exist(P,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(P, aw));
	}
	if (AnaSyntax::exist(LAMDA,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(LAMDA, aw));
	}
	if (AnaSyntax::exist(ALPHA,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(ALPHA, aw));
	}
	if (AnaSyntax::exist(BETA,aw)) 
	{
		m_dVar = atof(AnaSyntax::getSingleStr(BETA, aw));
	}
	if (AnaSyntax::exist(FREEDOM,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(FREEDOM, aw));
	}
	if (AnaSyntax::exist(FREEDOM1,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(FREEDOM1, aw));
	}
	if (AnaSyntax::exist(FREEDOM2,aw)) 
	{
		m_dVar = atof(AnaSyntax::getSingleStr(FREEDOM2, aw));
	}
	if (AnaSyntax::exist(PARAMETER,aw)) 
	{
		m_dMean = atof(AnaSyntax::getSingleStr(PARAMETER, aw));
	}
	//方法
	if (AnaSyntax::exist(METHOD,aw)) 
	{
		m_szMethod = AnaSyntax::getSingleStr(METHOD, aw);
	}
	else
	{
		return false;
	}
	return true;
}

bool CRandom::Random(const string &VO, CDoubleMatrix &m)
{
	if (!Parse(VO)) return false;
	//随机数工厂
	CRandFactory factory(m_bRandom,m_nSeed);
	if (m_szMethod == NORMAL)          //正态分布
	{
		m = factory.CreateNormalRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == LOGNORMAL)  //对数正态分布
	{
		m = factory.CreateLogNormRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == UNIFORM)    //均匀分布
	{
		m = factory.CreateUniformRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == NUNIFORM)   //离散分布
	{
		m = factory.CreateDisUniformRand(m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == BINOMIAL)   //二项式分布
	{
		m = factory.CreateBinomialRand(m_dVar,m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == NBINOMIAL)  //负二项式分布
	{
		m = factory.CreateNBinoRand(m_dVar,m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == EXPONENTIAL) //指数分布
	{
		m = factory.CreateExponentialRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == POISSON)     //泊松分布
	{
		m = factory.CreatePoissonRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == GAMMA)       //Г分布
	{
		m = factory.CreateGammaRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == BENOLLY)     //伯努利分布
	{
		m = factory.CreateBenollyRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == CAUCH)       //柯西分布
	{
		m = factory.CreateCauchRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == CHI2)        //卡方分布
	{
		m = factory.CreateChi2Rand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == GEO)         //几何分布
	{
		m = factory.CreateGeoRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == RAY)         //瑞利分布
	{
		m = factory.CreateRayRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == F)           //F分布
	{
		m = factory.CreateFRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == T)           //T分布
	{
		m = factory.CreateTRand(m_dMean,m_nNumber,1);
	}
	else if (m_szMethod == Z)           //Z分布
	{
		m = factory.CreateZRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == BT)        //Beta分布
	{
		m = factory.CreateBetaRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else if (m_szMethod == WEIB)        //Weibull分布
	{
		m = factory.CreateWeibRand(m_dMean,m_dVar,m_nNumber,1);
	}
	else
	{
		return false;
	}
	return true;
}

bool CRandom::Verify()
{
	return true;
}

bool CRandom::Run(CDataInterface *pData, const string &VO)
{
	int i=0, nRow=0;
	CDoubleMatrix m;
	
	m_pData = pData;
	if (!m_pData)
		return false;
	if (!Random(VO, m))
		return false;
	nRow = pData->GetRowCount();
	CField *pField = pData->m_DataAccess.m_pFieldList->CreateField(fDouble);
	if (m_szVarName.GetLength() > 0)
	{
		if (pData->m_DataAccess.m_pFieldList->FieldByName(m_szVarName) != NULL)
			m_szVarName += "001";
		pField->SetFieldName(m_szVarName);
	}
	pData->m_DataAccess.m_pFieldList->Add(pField);
	pData->m_DataAccess.First();	
	for(i=0; i<m_nNumber; i++)
	{
		if(i<nRow)
		{
			pData->m_DataAccess.Edit();	
			pField->SetAsDouble(m(0)(i));
			pData->m_DataAccess.Next();
			pData->m_DataAccess.Post();	
		}
		else
		{
			pData->m_DataAccess.Append();	
			pField->SetAsDouble(m(0)(i));
			pData->m_DataAccess.Post();	
		}
	}
	return true;
}