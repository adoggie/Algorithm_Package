// Association.cpp: implementation of the CAssociation class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Association.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CRuleOutput::Import(string szFileName)
{
	return true;
}

bool CRuleOutput::Export(string szFileName)
{
	return true;
}

CAssociation::CAssociation()
{

}

CAssociation::~CAssociation()
{

}

CResult *CAssociation::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, string strVO)
{
	return NULL;
}


CResult* CAssociation::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string strVO, string strModel)
{
	return NULL;
}
