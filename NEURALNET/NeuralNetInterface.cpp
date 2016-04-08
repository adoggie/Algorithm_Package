//////////////////////////////////////////////////////////////////////
// NeuralNetInterface.cpp                                           //
//																	//
// Define:        神经网络分析系统导出类实现                        //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "Resource.h"
#include "NeuralNet.h"
//#include "DlgARTSetting.h"
//#include "DlgGenicSetting.h"
//#include "DlgRBFSetting.h"
// #include "PropBasic.h"
#include "NeuralNetInterface.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT WM_CALCULATE_OVER = RegisterWindowMessage("WM_CALCULATE_OVER");
const UINT WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuralNetInterface::CNeuralNetInterface()
{
	m_pNeuralNet = new CNeuralNet;
	m_pNeuralNet->Initialize();
}

CNeuralNetInterface::~CNeuralNetInterface()
{
	if( m_pNeuralNet!=NULL )
		delete m_pNeuralNet;
}

///////////////////////////////////////////////////////////
// 神经网络系统分析操作接口函数
///////////////////////////////////////////////////////////
void CNeuralNetInterface::Run()
{
	CResult * pResult = NULL;
	//pResult = m_pNeuralNet->Run();
}

void CNeuralNetInterface::DeleteAll()
{
	m_pNeuralNet->DeleteFiles();
}

unsigned int CNeuralNetInterface::WorkThreadProc()
{
	return 0;
}

bool CNeuralNetInterface::SetDataSource( CDataInterface * pData, CTStringArray& arFieldRole )
{
	if( arFieldRole.GetSize()==0 )
	{
		return false;
	}

	return m_pNeuralNet->SetDataSource( pData, arFieldRole );
}

bool CNeuralNetInterface::SetDataParam(CNeuralNetVO	*pNeuralNet)
{
	return m_pNeuralNet->SetDataParam(pNeuralNet);
}

void CNeuralNetInterface::Abort()
{
	m_pNeuralNet->Abort();
}
