// BPNetInterface.cpp: implementation of the CBPNetInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BPNetInterface.h"
#include "../Public/TString.h"

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

CBPNetInterface::CBPNetInterface()
{
	m_pNeuralNet = new CBPNet();
// 	m_pNeuralNet->Initialize();
}

CBPNetInterface::~CBPNetInterface()
{
	if( m_pNeuralNet!=NULL )
		delete m_pNeuralNet;
}

///////////////////////////////////////////////////////////
// 神经网络系统分析操作接口函数
///////////////////////////////////////////////////////////
//CResult * CBPNetInterface::Run()
//{
//	CResult * pResult = NULL;
//	pResult = m_pNeuralNet->Run( );
//	pResult->Print();
//	return pResult;
//}

//bool	CBPNetInterface::SetDataParam(TBPNetVO *pVO)
//{
//	return m_pNeuralNet->SetDataParam(pVO);
//}
//
//bool CBPNetInterface::SetDataSource( CDataInterface * pData,  CPtrArray &arFieldRole)
//{
// 	if(arFieldRole.GetSize() ==0)
//	{
//		return false;
//	}
//
//	return m_pNeuralNet->SetDataSource( pData, arFieldRole);
//}
//

void CBPNetInterface::Abort()
{
	m_pNeuralNet->Abort();
}
