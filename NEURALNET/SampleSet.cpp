//////////////////////////////////////////////////////////////////////
// SampleSet.cpp                                                    //
//																	//
// Define:        ����������ݼ���ʵ��                              //
// Creator:       ���                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "TFieldRole.h"
#include "SampleSet.h"

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

CSampleSet::CSampleSet()
{
	m_pSet = NULL;
}

CSampleSet::~CSampleSet()
{
	if( m_pSet )
	{
		delete m_pSet;
		m_pSet = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
//�������ƣ�InitializeDataset
//���������CDataInterface * pDataInterface,  ���ݵĽӿ�
//			CPtrArray& arFieldRole, �����ֶε���Ϣ
//			int nDataType			���ݵ����ͣ��и����ͣ����ͣ���ֵ��
//�������ܣ���ʼ�����ݼ��������ݵ��뵽m_pSet��
//////////////////////////////////////////////////////////////////////////
bool CSampleSet::InitializeDataset( CDataInterface * pDataInterface, CTStringArray& strField, int nDataType )
{
	int nRowCount, nColCount, nInputCount=0;

	assert( pDataInterface!=NULL );

	nColCount = strField.GetSize();
	//nColCount = pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	nRowCount = pDataInterface->GetRowCount();
	nInputCount = strField.GetSize();

	m_pSet = new CSample( nDataType, nRowCount, nInputCount );
	assert( m_pSet!=NULL ); 

	CDoubleVector pData = m_pSet->GetBuffer();
	
	//////////////////////////////////////////////////////
	// ��pDataInterface��ȡ����
	//
	CDataAccess& m_rDataAccess = pDataInterface->m_DataAccess;
	m_rDataAccess.First();
	int nInputField, nRow = 0;
	while( !m_rDataAccess.Eof() )
	{
		nInputField = 0;
		CFieldList * pFieldList = m_rDataAccess.m_pFieldList;
		for( int nCol=0; nCol<strField.GetSize(); nCol++)
		{
			CTString strFieldName = strField.GetAt(nCol);
			CField * pField = pFieldList->FieldByName(strFieldName);

			if( pField->IsNull() )
			{
				nRow--;
				break;
			}
			else
			{
				pData[nRow*nInputCount + nInputField] = pField->GetAsDouble();
				nInputField++;
			}
		}
		m_rDataAccess.Next();
		nRow++;
	}
	m_pSet->SetSampleNum( nRow );
	return true;
}

CDoubleVector& CSampleSet::GetDataBuffer()
{
	return m_pSet->GetBuffer();
}





















