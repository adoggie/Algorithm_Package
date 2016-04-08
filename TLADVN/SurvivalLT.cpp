/* 
* Copyright (c) 2002,�Ϻ�������Ϣ�������޹�˾��Ϣ������
* All rights reserved.
*
* �ļ����ƣ�SurvivalLT.cpp
* �ļ���ʾ�������ù���ƻ���
* ժ    Ҫ��ʵ�������
*
* ��ǰ�汾��1.0
* ��    �ߣ������
* ������ڣ�2002-11-27
*/
#include "StdAfx.h"
// #include "tlAdvn.h"
#include "SurvivalLT.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

/*
* ���캯������������
*/
CSurvivalLT::CSurvivalLT()
{
	m_ptLiftTbl = NULL;
	m_bBeyond = false;

	////////////////////////////////////////////////////////////
	// ��ӣ����
	// Ŀ�ģ���ʼ�����û����õĳ�Ա����
	m_nTimeField		= -1;
	m_nStatusField		= -1;
	m_bSingleEventValue = true;
	m_dSingleValue		= 0;
	m_dRangeFrom		= 0;
	m_dRangeTo			= 0;
	m_dMaxTime			= 0;
	m_dStep				= 0;
	m_pDataInterface	= NULL;
	//
	////////////////////////////////////////////////////////////
}

CSurvivalLT::~CSurvivalLT()
{
}

/*
* �ӶԻ����д��ݹ����Ĳ���
*/

CResult * CSurvivalLT::OnSurvivalLT(CDataInterface * pDataInterface, string VO)
{
	assert( pDataInterface!=NULL );

	m_pDataInterface = pDataInterface;
	m_pResult = NULL; //��ֹ��һ����ȷ���к�,�ڶ�����������ʱ����! zlq
	if(GetValueFromVO(pDataInterface, VO) && Initialize())
		m_pResult = Analyze();
	return m_pResult;
}

bool CSurvivalLT::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	SURVIVALLT
//		/DATA=[FILE]
//		/TIMEVAR [varlist]
//		/STATUSVAR [varlist]
//		/WEIGHT [varlist]
//		/CRITERIA=[MAXTIME{(**)}] [STEPLENTH{(**)}] [{EVENT(**)},{RANGE{**,**}}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/TIMEVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nTimeField = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();
	
	AnaSyntax::getVAR("/STATUSVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	yname = ynames.GetAt(0);
	m_nStatusField = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();

	m_bSingleEventValue = AnaSyntax::findstr("/CRITERIA","EVENT",VOstr);
	if (m_bSingleEventValue) 
		m_dSingleValue = AnaSyntax::getvalue("/CRITERIA","EVENT",VOstr);
	else if (AnaSyntax::findstr("/CRITERIA","RANGE",VOstr))
	{
		m_dRangeFrom = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr);
		m_dRangeTo = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr, 2);
	}
	else 
		return FALSE;

	m_dMaxTime = AnaSyntax::getvalue("/CRITERIA", "MAXTIME", VOstr);
	m_dStep = AnaSyntax::getvalue("/CRITERIA", "STEPLENTH", VOstr);

	return TRUE;
}

/*
* �����������ʼ�����������������Ҫ�Ĳ���
*/
bool CSurvivalLT::Initialize(void)
{
	//��Ҫ�ľֲ�����
	int i, nSum = 0, nMaxPeriod = 0;
	int	nWeightNum = m_pDataInterface->GetWeightNumb();		//�õ�Ȩ����
	int nRowCount = m_pDataInterface->GetRowCount();			//�õ��������
	CField * pFieldTime = NULL, * pFieldWeight = NULL, *pFieldStatus = NULL;

	// ���������������С
	double dPeriodNum = m_dMaxTime / m_dStep;
	m_nPeriodNum = (int)dPeriodNum;
	if( dPeriodNum > m_nPeriodNum )
		m_nPeriodNum++; // ����һ�����ʱ���

	m_nPeriodNum++; // ����0ʱ���

//	MEMORYSTATUS ms;
//	GlobalMemoryStatus( &ms );


	m_ptLiftTbl	= new TLiftTbl[m_nPeriodNum];				//�����ڴ���ý��ֵ
	for( i=0; i<m_nPeriodNum; i++ )
		m_ptLiftTbl[i].dIntStartTime = i * m_dStep;

	if( m_ptLiftTbl[m_nPeriodNum-1].dIntStartTime>m_dMaxTime )
		m_ptLiftTbl[m_nPeriodNum-1].dIntStartTime = m_dMaxTime;

	//�õ�һ�����ݿ����ָ��
	CDataAccess  &rDataAccess = m_pDataInterface->m_DataAccess;
	//�Ƶ����¼��һ��
	rDataAccess.First();

	while ( !rDataAccess.Eof() )
	{
		double dTime, dWeight;
		pFieldTime = rDataAccess.m_pFieldList->FieldByIndex( m_nTimeField );
		pFieldStatus = rDataAccess.m_pFieldList->FieldByIndex( m_nStatusField );

		if( m_pDataInterface->IsMissingValue( pFieldTime, true ) || m_pDataInterface->IsMissingValue( pFieldStatus, true ) )
		{
			rDataAccess.Next();
			continue;
		}
		// �жϸü�¼�����ĸ�ʱ���
		dTime = pFieldTime->GetAsDouble();


		if( nWeightNum>=0 ) // ������Ȩ��
		{
			pFieldWeight = rDataAccess.m_pFieldList->FieldByIndex( nWeightNum );
			if( m_pDataInterface->IsMissingValue( pFieldWeight, true ) )
				dWeight = 0;
			else
			{
				dWeight = pFieldWeight->GetAsDouble();
				if( dWeight!=int(dWeight) )
					dWeight = 0;
			}

			nSum += (int)dWeight;
		}
		else
			nSum++;

		int nPeriod;
		if( dTime>=0 )
		{
			if( dTime>=m_dMaxTime )
			{
				if( !m_bBeyond )
					m_bBeyond = true;
			}
			int k=0; 
			for( k=0; k<m_nPeriodNum; k++ )
				if( dTime<m_ptLiftTbl[k].dIntStartTime )
					break;
			nPeriod = k-1;

			if( nPeriod>nMaxPeriod )
				nMaxPeriod = nPeriod;

			if( m_bSingleEventValue) // ʹ�õ�һ״̬�ж�ֵ
			{
				if( pFieldStatus->GetAsDouble()==m_dSingleValue ) // ״̬Ϊ��
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dTermnlEvents += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dTermnlEvents += (int)dWeight;
					}
				}
				else	// ״̬Ϊ��
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dDuringIntrvl += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dDuringIntrvl += (int)dWeight;
					}
				}
			}
			else	// ʹ�÷�Χ״̬�ж�ֵ
			{
				if( pFieldStatus->GetAsDouble()>=m_dRangeFrom && 
							pFieldStatus->GetAsDouble()<=m_dRangeTo ) // ״̬Ϊ��
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dTermnlEvents += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;
						
						m_ptLiftTbl[nPeriod].dTermnlEvents += (int)dWeight;
					}
				}
				else	// ״̬Ϊ��
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dDuringIntrvl += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dDuringIntrvl += (int)dWeight;
					}
				}
			}
		}

		rDataAccess.Next();
	} // End of While

	m_nPeriodNum = nMaxPeriod + 1;

	m_ptLiftTbl[0].dThisIntrvl = nSum;

	return true;
}	

/*
* �����������Ա���ʽ���������
*/
CResult * CSurvivalLT::Analyze(void)
{
	int i, j;
	double dTemp;

	m_ptLiftTbl[0].dToRisk = m_ptLiftTbl[0].dThisIntrvl - (m_ptLiftTbl[0].dDuringIntrvl / 2);
	m_ptLiftTbl[0].dTerminating	= m_ptLiftTbl[0].dTermnlEvents / m_ptLiftTbl[0].dToRisk; 
	m_ptLiftTbl[0].dSurviving = 1 - m_ptLiftTbl[0].dTerminating;
	m_ptLiftTbl[0].dSurvAtEnd = m_ptLiftTbl[0].dSurviving;
	m_ptLiftTbl[0].dDensty = (1 - m_ptLiftTbl[0].dSurvAtEnd) / m_dStep;
	m_ptLiftTbl[0].dRate = (2 * m_ptLiftTbl[0].dTerminating) / (m_dStep * (1 + m_ptLiftTbl[0].dSurviving));

	for ( i=1; i<m_nPeriodNum; i++)
	{
		m_ptLiftTbl[i].dThisIntrvl = m_ptLiftTbl[i-1].dThisIntrvl - m_ptLiftTbl[i-1].dDuringIntrvl - m_ptLiftTbl[i-1].dTermnlEvents;
		m_ptLiftTbl[i].dToRisk = m_ptLiftTbl[i].dThisIntrvl - (m_ptLiftTbl[i].dDuringIntrvl / 2);
		m_ptLiftTbl[i].dTerminating	= m_ptLiftTbl[i].dTermnlEvents / m_ptLiftTbl[i].dToRisk; 
		m_ptLiftTbl[i].dSurviving = 1 - m_ptLiftTbl[i].dTerminating;
		m_ptLiftTbl[i].dSurvAtEnd = m_ptLiftTbl[i-1].dSurvAtEnd * m_ptLiftTbl[i].dSurviving;
		m_ptLiftTbl[i].dDensty = (m_ptLiftTbl[i-1].dSurvAtEnd - m_ptLiftTbl[i].dSurvAtEnd) / m_dStep;
		m_ptLiftTbl[i].dRate = (2 * m_ptLiftTbl[i].dTerminating) / (m_dStep * (1 + m_ptLiftTbl[i].dSurviving));
	}

	// ���� SECS��SEPD��SEHR
	for( i=0; i<m_nPeriodNum; i++ )
	{
		dTemp = 0;
		for( j=0; j<i+1; j++ )
		{
			if( fabs(m_ptLiftTbl[j].dToRisk)<1e-10 || fabs(m_ptLiftTbl[j].dSurviving)<1e-10 )
			{
				dTemp = 0;
				break;
			}
			dTemp += m_ptLiftTbl[j].dTerminating / ( m_ptLiftTbl[j].dToRisk * m_ptLiftTbl[j].dSurviving );
		}
		m_ptLiftTbl[i].dSecs = m_ptLiftTbl[i].dSurvAtEnd * sqrt( dTemp );

		if (fabs(m_ptLiftTbl[i].dTerminating)<1e-10)
			m_ptLiftTbl[i].dSehr =0.0;
		else
		{
			double lamda = 2 * m_ptLiftTbl[i].dTerminating / (m_dStep * (1 + m_ptLiftTbl[i].dSurviving));
			m_ptLiftTbl[i].dSehr = sqrt(lamda * lamda * (1 - ((lamda * m_dStep) / 2)) *
								(1 + (lamda * m_dStep)/2) / (m_ptLiftTbl[i].dToRisk * 
								m_ptLiftTbl[i].dTerminating));
		}
	}

	if( fabs(m_ptLiftTbl[0].dTerminating)<1e-10 )
		m_ptLiftTbl[0].dSepd = 0;
	else
		m_ptLiftTbl[0].dSepd = m_ptLiftTbl[0].dTerminating / m_dStep *
							sqrt(m_ptLiftTbl[0].dSurviving / (m_ptLiftTbl[0].dToRisk * m_ptLiftTbl[0].dTerminating));
	for( i=1; i<m_nPeriodNum; i++ )
	{
		dTemp = 0;
		int j=0; 
		for (j=0; j<i; j++)
			dTemp += m_ptLiftTbl[j].dTerminating / ( m_ptLiftTbl[j].dToRisk * m_ptLiftTbl[j].dSurviving );

		if( fabs(m_ptLiftTbl[i].dTerminating)<1e-10 )
			m_ptLiftTbl[i].dSepd = 0;
		else
		{
			dTemp += m_ptLiftTbl[i].dSurviving / (m_ptLiftTbl[i].dToRisk * m_ptLiftTbl[i].dTerminating);

			m_ptLiftTbl[i].dSepd = (m_ptLiftTbl[i-1].dSurvAtEnd * m_ptLiftTbl[i].dTerminating * 
									 sqrt(dTemp)) / m_dStep;
		}
	}

	return OutputTable();
}

CResult * CSurvivalLT::OutputTable()
{
	int i, nRow;

	m_pResult = new CResult("�������");

	for( i=0; i<m_nPeriodNum; i++ )
	{
		if( m_ptLiftTbl[i].dThisIntrvl==0 )
			break;
	}
	
	if( i<2 )
	{
		CTString	strWarning;
		strWarning.Format( " ע������Ч�ĺϷ����������룬�޷����������" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	else
		nRow = i;
	///////////////�����һ����///////////////
	CRsltVariable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("������");
// 	pTable->CreateTable(nRow+1,13);
//	pTable->SetCols(13);
//	pTable->SetRows(nRow + 1);

		
	pTable->InsertColumn(0, "ʱ������");
	pTable->InsertColumn(1, "ʱ�ι۲�\n����");	
	pTable->InsertColumn(2, "ʱ��ɾʧ\n����");	
	pTable->InsertColumn(3, "��Ч�۲�\n����");	
	pTable->InsertColumn(4, "�ս��¼�\n����");	
	pTable->InsertColumn(5, "�ս��¼�\n����");	
	pTable->InsertColumn(6, "�����¼�\n����");	
	pTable->InsertColumn(7, "�ۻ�\n������");	
	pTable->InsertColumn(8, "�����ܶ�");	
	pTable->InsertColumn(9, "������");	
	pTable->InsertColumn(10, "�ۻ�\n������\n�ı�׼��");	
	pTable->InsertColumn(11, "�����ܶ�\n�ı�׼��");	
	pTable->InsertColumn(12, "������\n�ı�׼��");
	
	CTString str;
	for( i=0; i<nRow; i++ )
	{
		if( i==nRow-1 && m_bBeyond )
			str.Format( "%g +", m_ptLiftTbl[i].dIntStartTime );
		else
			str.Format( "%g", m_ptLiftTbl[i].dIntStartTime );
		pTable->InsertItem( i, str );

		pTable->SetItemText( i, 1, m_ptLiftTbl[i].dThisIntrvl );
		pTable->SetItemText( i, 2, m_ptLiftTbl[i].dDuringIntrvl );
		pTable->SetItemText( i, 3, m_ptLiftTbl[i].dToRisk );
		pTable->SetItemText( i, 4, m_ptLiftTbl[i].dTermnlEvents );
		pTable->SetItemText( i, 5, m_ptLiftTbl[i].dTerminating );
		pTable->SetItemText( i, 6, m_ptLiftTbl[i].dSurviving );
		pTable->SetItemText( i, 7, m_ptLiftTbl[i].dSurvAtEnd );
		pTable->SetItemText( i, 8, m_ptLiftTbl[i].dDensty );
		pTable->SetItemText( i, 9, m_ptLiftTbl[i].dRate );
		pTable->SetItemText( i, 10, m_ptLiftTbl[i].dSecs );
		pTable->SetItemText( i, 11, m_ptLiftTbl[i].dSepd );
		pTable->SetItemText( i, 12, m_ptLiftTbl[i].dSehr );
	}

	if( m_bBeyond )
	{
		str.Format( "**" );
		pTable->SetItemText( m_nPeriodNum-1, 8, str );
		pTable->SetItemText( m_nPeriodNum-1, 9, str );
		pTable->SetItemText( m_nPeriodNum-1, 11, str );
		pTable->SetItemText( m_nPeriodNum-1, 12, str );
		str.Format( " **  ���ʱ��εļ���������" );
		pTable->SetTail( str );
	}

	pETable= new CRsltElementTable("������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);

	delete[] m_ptLiftTbl;
	m_ptLiftTbl = NULL;

	return m_pResult;
}
