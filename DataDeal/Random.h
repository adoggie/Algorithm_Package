// Random.h: interface for the CRandom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOM_H__7BCB5AB2_6505_4A3F_9686_DF337518034E__INCLUDED_)
#define AFX_RANDOM_H__7BCB5AB2_6505_4A3F_9686_DF337518034E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../cpknl/cpknl.h"

const char RANDOM[]      = "RANDOM";

const int  MIN_NUM       = 5; //��С������
const char VARDEST[]     = "/VARDEST";

const char SEED[]       = "/SEED";
const char E[]          = "/E";
const char MEAN[]       = "/MEAN";
const char VAR[]        = "/VAR";
const char SAMPLENUM[]  = "/SAMPLENUMBER";
const char MAX[]        = "/MAX";
const char MIN[]        = "/MIN";
const char COUNT[]      = "/COUNT";
const char P[]          = "/P";
const char LAMDA[]      = "/LAMDA";
const char ALPHA[]      = "/ALPHA";
const char BETA[]       = "/BETA";
const char FREEDOM[]    = "/FREEDOM";
const char FREEDOM1[]   = "/FREEDOM1";
const char FREEDOM2[]   = "/FREEDOM2";
const char PARAMETER[]  = "/PARAMETER";
const char METHOD[]      = "/METHOD"; 
const char NORMAL[]      = "NORMAL"; 
const char LOGNORMAL[]   = "LOGNORMAL"; 
const char UNIFORM[]     = "UNIFORM"; 
const char NUNIFORM[]    = "NUNIFORM"; 
const char BINOMIAL[]    = "BINOMIAL"; 
const char NBINOMIAL[]   = "NBINOMIAL"; 
const char EXPONENTIAL[] = "EXPONENTIAL"; 
const char POISSON[]     = "POISSON"; 
const char GAMMA[]       = "GAMMA";
const char BENOLLY[]     = "BENOLLY";
const char CAUCH[]       = "CAUCH";
const char CHI2[]        = "CHI2"; 
const char GEO[]         = "GEO"; 
const char RAY[]         = "RAY"; 
const char F[]           = "F"; 
const char T[]           = "T"; 
const char Z[]           = "Z";
const char BT[]          = "BETA";
const char WEIB[]        = "WEIB";

class CDataInterface;
class CField;
class CRandom  
{
public:
	CRandom();
	virtual ~CRandom();
public:
	bool Run(CDataInterface *pData, const string &VO);
protected:
	bool Verify();
	bool Parse(const string &VO);
	bool Random(const string &VO,CDoubleMatrix &m);
private:
	int    m_nSeed;  //����
	bool   m_bRandom;//�Ƿ��������
	double m_dMean;  //��ֵ
	double m_dVar;   //����
	int    m_nNumber;//����
	CTString m_szMethod; //����
	CTString m_szVarName;//��������
	CDataInterface *m_pData;
};

#endif // !defined(AFX_RANDOM_H__7BCB5AB2_6505_4A3F_9686_DF337518034E__INCLUDED_)
