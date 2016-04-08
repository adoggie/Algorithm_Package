//////////////////////////////////////////////////////////////////////
// Sample.cpp:                                                      //
//																	//
// Define:        输入采样数据类实现                                //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "Sample.h"

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

CSample::CSample( int nDataType, int nSampleNum, int nDemision )
{
	m_nDataType = nDataType;
	m_nSampleNum = nSampleNum;
	m_nDemision = nDemision;

	switch( m_nDataType )
	{
	case DOUBLETYPE:
		m_aDoubleData.create(m_nSampleNum * m_nDemision);
		break;
	case INTEGERTYPE:
		m_aIntData.create(m_nSampleNum * m_nDemision);
		break;
	case BOOLEANTYPE:
		m_aBoolData.create(m_nSampleNum * m_nDemision);
		break;
	default:
		break;
	}
}

CSample::~CSample()
{
}
