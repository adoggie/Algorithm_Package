// BlurryInterface.cpp : implementation file
//

#include "StdAfx.h"
#include "BlurryInterface.h"
#include "BlurryCluster.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
const UINT WM_CALCULATE_OVER = RegisterWindowMessage("WM_CALCULATE_OVER");
*/

/////////////////////////////////////////////////////////////////////////////
// CBlurryInterface


CBlurryInterface::CBlurryInterface(bool bFirstRun)
{
	if(bFirstRun)
	{
		m_dLambda=0.8;
		m_nSelect=0;
	}
	m_pBlurryCluster=new CBlurryCluster;
}

void CBlurryInterface::SetDataSource(CDataInterface *pDataInterface,CPtrArray *pRolePtrArray)
{
	m_pPtrArray=pRolePtrArray;
	m_pDataInterface=pDataInterface;
}


CBlurryInterface::~CBlurryInterface()
{
	if(m_pBlurryCluster)
		delete m_pBlurryCluster;
	m_pBlurryCluster=NULL;
}

bool	CBlurryInterface::SetDataParam(CBlurryVO *pVO)
{
	m_pBlurryCluster->SetDataParam(pVO);
	return true;
}

//#define fDouble 'F'     //双精度浮点型数据
//#define fInt 'N'       //整型数据
//#define fString 'C'    //字符串型数据
//#define fDate 'D'      //日期型数据
//#define fCurrency 'M'  //货币型数据
//#define fBoolean 'L'   //布尔型数据
//#define fBlob 'B'      //型数据
CResult * CBlurryInterface::RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction *&itt,CBlurryVO *pVO)
{
	int FlagChar=0,FlagCurrency=0,FlagFloat=0,FlagDate=0;
	for(int i=0;i<pVO->arFeildInfo.GetSize();i++)
	{
		CTString strField = (pVO->arFeildInfo.GetAt(i));
		CField *pField = pDataInterface->FieldByName(strField);
		CFieldType fdType = pField->GetFieldType();
		if(fdType==fString)
			FlagChar++;
		if(fdType==fDouble)
			FlagFloat++;
		if(fdType==fCurrency)
			FlagCurrency++;
		if(fdType==fDate)
			FlagDate++;
	}
	m_nSelect = pVO->nSelect;//add by xds 20061009
	return m_pBlurryCluster->RunTrain(pDataInterface,pEnv,itt,pVO);
}

CResult * CBlurryInterface::Run(CDataInterface *pDataInterface,CPtrArray *pPtrArray)
{

//在调用之前，要执行如下判断：
	int FlagChar=0,FlagCurrency=0,FlagFloat=0,FlagDate=0;
	for(int i=0;i<pPtrArray->GetSize();i++)
	{
		TFieldRole *p=(TFieldRole *)(pPtrArray->GetAt(i));
		if(p->strRole==CTString("输入变量"))
		{
			if(p->strFieldType==CTString("字符型"))
				FlagChar++;
			if(p->strFieldType==CTString("浮点型"))
				FlagFloat++;
			if(p->strFieldType==CTString("货币型"))
				FlagCurrency++;
			if(p->strFieldType==CTString("日期型"))
				FlagDate++;
		}
	}
 	return m_pBlurryCluster->Run(pDataInterface,pPtrArray);
}

void CBlurryInterface::SetAbortAttib()
{
	m_pBlurryCluster->m_bIsToEnd=true;
}
