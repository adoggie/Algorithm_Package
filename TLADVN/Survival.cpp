//////////////////////////////////////////////////////////////////////
// Survival.cpp:                                                    //
//																	//
// Define:        生存分析基类实现                                  //
// Creator:       屈韬                                              //
// Date:          2002.11.25                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tlAdvn.h"
#include "Survival.h"

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

CSurvival::CSurvival()
{
	m_pDataInterface = NULL;
	m_pResult		 = NULL;
}

CSurvival::~CSurvival()
{
}

CResult * CSurvival::OnSurvival( CDataInterface * pDataInterface, CSurvivalVO &SurvivalVO)
{
	assert( pDataInterface!=NULL );
	
	m_pDataInterface = pDataInterface;
	pSurvivalVO = &SurvivalVO;
	m_pResult = NULL; //防止第一次正确运行后,第二条件不满足时错误! zlq
	if( GetParamSetting()!=false && Initialize()!=false )
		m_pResult = Analyze();
	return m_pResult;
}
