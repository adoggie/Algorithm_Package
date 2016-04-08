//////////////////////////////////////////////////////////////////////
// Topology.cpp:                                                    //
//																	//
// Define:        神经网络拓扑结构类实现                            //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../Public/TString.h"
#include "KohonenTopology.h"
#include "ARTTopology.h"
#include "BPTopology.h"
#include "RBTopology.h"
#include "Topology.h"

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
CTopology::CTopology()
{
}

CTopology::CTopology( int nType, int nSampleSize, int nDimension, int nClusterNum,
					 long lRuntime, int nIterateNum )
{
	m_nTopologyType = nType;
	m_nSampleSize = nSampleSize;
	m_nClusterNum = nClusterNum;
	m_nDimension = nDimension;
	m_dwRuntime = lRuntime;
	m_unIterateNum = nIterateNum;
	m_bAbort = false;
}

CTopology::~CTopology()
{

}

CTopology * CTopology::Initialization(int nType, ...)
{
	int nSampleSize=0, nDimension=0, nClusterNum=0, nIterateNum=0;
	long lRuntime=0;
	double fVigilance=0, fMaxRate=0, fMinRate=0;
	va_list argument;
	
	va_start( argument, nType );
	nSampleSize = va_arg( argument, int );
	nDimension = va_arg( argument, int );
	nClusterNum = va_arg( argument, int );
	lRuntime = va_arg( argument, long );
	nIterateNum = va_arg( argument, int );
	fVigilance = va_arg( argument, double );
	fMaxRate = va_arg( argument, double );
	fMinRate = va_arg( argument, double );
	va_end( argument );

	switch( nType )
	{
	case 0:
		return (CTopology *)(new CARTTopology( nType, 
											nSampleSize, nDimension, 
											nClusterNum, 
											lRuntime, nIterateNum,
											fVigilance));
		break;
	case 1:
		return (CTopology *)(new CKohonenTopology( nType, 
											nSampleSize, nDimension, 
											nClusterNum, 
											lRuntime, nIterateNum,
											fMaxRate, fMinRate));
		break;
	default:
		return NULL;
		break;
	}
		
}

bool CTopology::GetRowData(CDataInterface *pDataInterface, CTStringArray &arFieldRole, unsigned int nRow, CDoubleVector& adDataRow)
{
	CDataAccess& m_rDataAccess = pDataInterface->m_DataAccess;

	m_rDataAccess.SetCurRecNo( nRow );

	CFieldList * pFieldList = m_rDataAccess.m_pFieldList;
	int nFieldCount = arFieldRole.GetSize();
	//int nFieldCount = pFieldList->GetFieldCount();

	int nInputField = 0;
	for( int nCol=0; nCol<nFieldCount; nCol++)
	{
		CTString strRole = arFieldRole.GetAt(nCol);
		CField * pField = pFieldList->FieldByName(strRole);
		if( pField->IsNull() )
		{
			//AfxMessageBox( IDS_NULLVALUEDATA, MB_ICONSTOP );
			return false;
		}
		adDataRow[nInputField++] = pField->GetAsDouble();
	}
	return true;
}






















