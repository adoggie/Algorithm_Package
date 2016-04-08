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

//#define fDouble 'F'     //˫���ȸ���������
//#define fInt 'N'       //��������
//#define fString 'C'    //�ַ���������
//#define fDate 'D'      //����������
//#define fCurrency 'M'  //����������
//#define fBoolean 'L'   //����������
//#define fBlob 'B'      //������
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

//�ڵ���֮ǰ��Ҫִ�������жϣ�
	int FlagChar=0,FlagCurrency=0,FlagFloat=0,FlagDate=0;
	for(int i=0;i<pPtrArray->GetSize();i++)
	{
		TFieldRole *p=(TFieldRole *)(pPtrArray->GetAt(i));
		if(p->strRole==CTString("�������"))
		{
			if(p->strFieldType==CTString("�ַ���"))
				FlagChar++;
			if(p->strFieldType==CTString("������"))
				FlagFloat++;
			if(p->strFieldType==CTString("������"))
				FlagCurrency++;
			if(p->strFieldType==CTString("������"))
				FlagDate++;
		}
	}
 	return m_pBlurryCluster->Run(pDataInterface,pPtrArray);
}

void CBlurryInterface::SetAbortAttib()
{
	m_pBlurryCluster->m_bIsToEnd=true;
}
