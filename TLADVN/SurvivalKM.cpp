//////////////////////////////////////////////////////////////////////
// SurvivalKM.cpp:                                                  //
//																	//
// Define:        �������KM�����㷨ʵ��                            //
// Creator:       �ŷ��������                                      //
// Date:          2002.11.25                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SurvivalKM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSurvivalKM::CSurvivalKM()
{
	
	m_nStatusPos = -1;
	m_nTimePos = -1;
	m_nFactorPos = -1;
	m_bIsRange = true;
	m_dSingle = 0;
	m_dRangeLow = 0;
	m_dRangeHigh = 0;
	m_pDataInterface = NULL;
	m_pResult = NULL;
}


CSurvivalKM::~CSurvivalKM()
{
}

CResult * CSurvivalKM::OnSurvivalKM( CDataInterface * pDataInterface, string VO)
{
	assert( pDataInterface!=NULL );
	
	m_pDataInterface = pDataInterface;
	m_pResult = NULL; //��ֹ��һ����ȷ���к�,�ڶ�����������ʱ����! zlq
	if( GetValueFromVO(pDataInterface, VO) && Initialize())
		m_pResult = Analyze();
	return m_pResult;
}

bool CSurvivalKM::GetValueFromVO(CDataInterface *pDataInterface, string VO)
{
//	SURVIVALKM
//		/DATA=[FILE]
//		/TIMEVAR [varlist]
//		/STATUSVAR [varlist]
//		/FACTOR [varlist]
//		/WEIGHT [varlist]
//		/CRITERIA=[{EVENT(**)},{RANGE{**,**}}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/TIMEVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nTimePos = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();
	
	AnaSyntax::getVAR("/STATUSVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	yname = ynames.GetAt(0);
	m_nStatusPos = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();

	m_nFactorPos = -1;
	if (AnaSyntax::exist("/FACTOR", VOstr))
	{
		AnaSyntax::getVAR("/FACTOR", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return FALSE;
		yname = ynames.GetAt(0);
		m_nFactorPos = pDataInterface->IndexByName(yname);
		ynames.RemoveAll();
	}

	m_bIsRange = !AnaSyntax::findstr("/CRITERIA","EVENT",VOstr);
	if (!m_bIsRange) 
		m_dSingle = AnaSyntax::getvalue("/CRITERIA","EVENT",VOstr);
	else if (AnaSyntax::findstr("/CRITERIA","RANGE",VOstr))
	{
		m_dRangeLow = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr);
		m_dRangeHigh = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr, 2);
	}
	else 
		return FALSE;

	return TRUE;
}

bool CSurvivalKM::Initialize()
{
//////////////////////////////////////////////////////////
	int i, nFieldWeight;
	CField * pFieldWeight = NULL;
	
	//�õ�һ�����ݿ����ָ��
	CDataAccess  &rDataAccess = m_pDataInterface->m_DataAccess;
	//�Ƶ����¼��һ��
	m_strTimeFieldName = rDataAccess.m_pFieldList->FieldByIndex( m_nTimePos )->GetFieldName();
	m_strTimeFieldLable = rDataAccess.m_pFieldList->FieldByIndex( m_nTimePos )->GetLabel();
	//�ж��Ƿ�������Ȩ��
	if( (nFieldWeight=m_pDataInterface->GetWeightNumb())>=0 )
		pFieldWeight = rDataAccess.m_pFieldList->FieldByIndex( nFieldWeight );

	rDataAccess.First();
	while ( !rDataAccess.Eof() )
	{
		CField * pFieldTime = rDataAccess.m_pFieldList->FieldByIndex( m_nTimePos );
		if( m_pDataInterface->IsMissingValue( pFieldTime, true ) )
		{
			rDataAccess.Next();
			continue;
		}

		double dTime = pFieldTime->GetAsDouble();
		if( dTime>=0 )
		{
			TKMTable * pCurKMTable = NULL;
			CField * pFieldFactor = NULL;
			double dWeight = 1;

			CField * pFieldStatus = rDataAccess.m_pFieldList->FieldByIndex( m_nStatusPos );
			if( m_pDataInterface->IsMissingValue( pFieldStatus, true ) ||
				pFieldWeight && m_pDataInterface->IsMissingValue( pFieldWeight, true ) )
			{
				rDataAccess.Next();
				continue;
			}

			if( pFieldWeight )
			{
				dWeight = pFieldWeight->GetAsDouble();
				if( dWeight!=int(dWeight) )
				{
					rDataAccess.Next();
					continue;
				}

			}

			// �ҵ���Ӧ��������
			TPeriodTable * pCurPeriodTable = NULL;
			if( m_nFactorPos>=0 )
				 pFieldFactor = rDataAccess.m_pFieldList->FieldByIndex( m_nFactorPos );

			if( pFieldFactor && m_pDataInterface->IsMissingValue( pFieldFactor, true ) )
			{
				rDataAccess.Next();
				continue;
			}

			double dFactorValue = 0;

			if( pFieldFactor )
				dFactorValue = pFieldFactor->GetAsDouble();
				
			for( i=0; i<m_aKMTable.GetSize(); i++ )
				if( dFactorValue==m_aKMTable[i]->dFactorValue )
					break;
			if( i<m_aKMTable.GetSize() ) // ���ҵ�������
				pCurKMTable = m_aKMTable[i];
			else	// �·��ֵ�����
			{
				pCurKMTable = new TKMTable;
				pCurKMTable->dFactorValue = dFactorValue;
				if( pFieldFactor )
					pCurKMTable->strFactorLabel = pFieldFactor->GetValueLable();
				// �����ʱ���
				TPeriodTable * pPeriodTable0 = new TPeriodTable;
				pCurKMTable->aPeriodTable.Add( pPeriodTable0 );

				// ��˳�����
				for( i=0; i<m_aKMTable.GetSize(); i++ )
					if( dFactorValue<m_aKMTable[i]->dFactorValue )
						break;
				m_aKMTable.InsertAt( i, pCurKMTable );
			}

			pCurKMTable->nCaseNum += int(dWeight);

			// �ҵ���Ӧ��ʱ����
			for( i=0; i<pCurKMTable->aPeriodTable.GetSize(); i++ )
				if( dTime==pCurKMTable->aPeriodTable[i]->dTime )
					break;
			if( i<pCurKMTable->aPeriodTable.GetSize() ) // ���ҵ���ʱ���
				pCurPeriodTable = pCurKMTable->aPeriodTable[i];
			else	// �·��ֵ�ʱ���
			{
				pCurPeriodTable = new TPeriodTable;
				pCurPeriodTable->dTime = dTime;

				// ��˳�����
				for( i=0; i<pCurKMTable->aPeriodTable.GetSize(); i++ )
					if( dTime<pCurKMTable->aPeriodTable[i]->dTime )
						break;
				pCurKMTable->aPeriodTable.InsertAt( i, pCurPeriodTable );
			}

			// �ж�״̬
			double dStatus = pFieldStatus->GetAsDouble();
			if( m_bIsRange )
			{
				if( dStatus<=m_dRangeHigh && dStatus>=m_dRangeLow ) // �¼�����
				{
					pCurPeriodTable->nDiedNum += int(dWeight);
					pCurKMTable->nEventNum += int(dWeight);
				}
				else												// ɾʧ
					pCurPeriodTable->nConsoredNum += int(dWeight);
			}
			else
			{
				if( dStatus==m_dSingle ) // �¼�����
				{
					pCurPeriodTable->nDiedNum += int(dWeight);
					pCurKMTable->nEventNum += int(dWeight);
				}
				else					 // ɾʧ
					pCurPeriodTable->nConsoredNum += int(dWeight);
			}
		}

		rDataAccess.Next();
	}

	return true;
}

CResult * CSurvivalKM::Analyze()
{
	int iType, iTime, j, iLastDiedTime;

	for( iType=0; iType<m_aKMTable.GetSize(); iType++ )
	{
		TKMTable * pCurKMTable = m_aKMTable[iType];
		int nPeriodNum = pCurKMTable->aPeriodTable.GetSize();

		iLastDiedTime = 1;
		for( j=nPeriodNum-1; j>0; j-- )
			if( pCurKMTable->aPeriodTable[j]->nDiedNum>0 )
				iLastDiedTime = j;

		double dTemp = 0;
		int nRiskNum = pCurKMTable->nCaseNum;
		pCurKMTable->aPeriodTable[0]->dCS = 1 - (double)pCurKMTable->aPeriodTable[0]->nDiedNum/(double)nRiskNum;
		dTemp = (double)pCurKMTable->aPeriodTable[0]->nDiedNum/
						((double)nRiskNum*(nRiskNum - pCurKMTable->aPeriodTable[0]->nDiedNum));
		pCurKMTable->aPeriodTable[0]->dSE = sqrt( dTemp ) * pCurKMTable->aPeriodTable[0]->dCS;
		for( iTime=1; iTime<nPeriodNum; iTime++ )
		{
			TPeriodTable * pPrevPeriodTable = pCurKMTable->aPeriodTable[iTime-1];
			TPeriodTable * pCurPeriodTable = pCurKMTable->aPeriodTable[iTime];

			nRiskNum = nRiskNum - pPrevPeriodTable->nDiedNum - pPrevPeriodTable->nConsoredNum;
			pCurPeriodTable->dCS = pPrevPeriodTable->dCS * (1-((double)pCurPeriodTable->nDiedNum/(double)nRiskNum) );
			dTemp += (double)pCurPeriodTable->nDiedNum/
						(double)(nRiskNum * (nRiskNum - pCurPeriodTable->nDiedNum));
			pCurPeriodTable->dSE = pCurPeriodTable->dCS * sqrt( dTemp );
		}

		if( pCurKMTable->aPeriodTable[nPeriodNum-1]->nConsoredNum==0 )
		{
			pCurKMTable->aPeriodTable[nPeriodNum-1]->dCS = 0;
			pCurKMTable->aPeriodTable[nPeriodNum-1]->dSE = 0;
		}

		// ����ƽ������ʱ��
		dTemp = 0;
		for( iTime=0; iTime<nPeriodNum-1; iTime++ )
			dTemp += pCurKMTable->aPeriodTable[iTime]->dCS * 
						( pCurKMTable->aPeriodTable[iTime+1]->dTime - pCurKMTable->aPeriodTable[iTime]->dTime );
		pCurKMTable->dMeanTime = dTemp;// + pCurKMTable->aPeriodTable[nPeriodNum-1]->dCS * 
						//( pCurKMTable->aPeriodTable[iLastDiedTime]->dTime - pCurKMTable->aPeriodTable[nPeriodNum-1]->dTime );


		// ��������ʱ����ֵ
		pCurKMTable->dMedianTime = -1; //pCurKMTable->aPeriodTable[0]->dTime;
	//	dTemp = fabs( pCurKMTable->aPeriodTable[0]->dCS - 0.5 );

		for( iTime=0; iTime<nPeriodNum; iTime++ )
		{
			if( ( pCurKMTable->aPeriodTable[iTime]->dCS - 0.5) <= 0 ) //dTemp )
			{
				//dTemp = fabs(pCurKMTable->aPeriodTable[iTime]->dCS - 0.5);
				pCurKMTable->dMedianTime = pCurKMTable->aPeriodTable[iTime]->dTime;
				break;
			}
		}
/*
		//////////////////////////////////////////////////////
		// ��������ʱ����ֵ
		// ���´��루2002.12.18��
		int MedianTime = 0;
		for( j=0; j<nPeriodNum; j++ )
		{
			if( pCurKMTable->aPeriodTable[j]->nDiedNum>0 )
				MedianTime++;
			if( MedianTime >= nPeriodNum/2 )
				break;
		}
		pCurKMTable->dMedianTime = pCurKMTable->aPeriodTable[MedianTime]->dTime;
		//
		//////////////////////////////////////////////////////
*/	}

	if( !OutputTables() )
		m_pResult = NULL;

	for( iType=0; iType<m_aKMTable.GetSize(); iType++ )
		delete m_aKMTable[iType];
	m_aKMTable.RemoveAll();

	return m_pResult;
}

bool CSurvivalKM::OutputTables()
{
	int iType, iTime;
	CTString strHead,strTemp, strTitle, strTableName;
	
	m_pResult = new CResult("KM����");

	if( m_aKMTable.GetSize()==0 ) // �޺Ϸ�����������ʾ
	{
		CTString	strWarning;
		strWarning.Format( " ע��û�кϷ����������룬�޷�����KM���̷�����" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return true;
	}

	strTableName = "KM����";

	if( m_strTimeFieldLable.IsEmpty() )
		strTitle.Format( "���ֶ� %s ����KM���̷���", m_strTimeFieldName.GetData() );
	else
		strTitle.Format( "���ֶ� %s[%s] ����KM���̷���", m_strTimeFieldName.GetData(), m_strTimeFieldLable.GetData());

	for( iType=0; iType<m_aKMTable.GetSize(); iType++ )
	{
		TKMTable * pCurKMTable = m_aKMTable[iType];
		int nPeriodNum = pCurKMTable->aPeriodTable.GetSize();

		CTLTable * pTable = new CTLTable;
		pTable->SetTitle( strTitle );

		if( m_nFactorPos>=0 )
		{
			if( pCurKMTable->strFactorLabel.IsEmpty() )
				strHead.Format( "�������ͣ�%g", pCurKMTable->dFactorValue );
			else
				strHead.Format( "�������ͣ�%s", pCurKMTable->strFactorLabel.GetData() );
			pTable->SetHead( strHead );
			strTableName = strHead;
		}

		// ������Ч����
		int nRows = 0;
		for( iTime=0; iTime<nPeriodNum; iTime++ )
		{
			TPeriodTable * pCurPeriodTable = pCurKMTable->aPeriodTable[iTime];
			if( pCurPeriodTable->nDiedNum!=0 )
				nRows++;
			if( pCurPeriodTable->nConsoredNum!=0 )
				nRows++;
		}

		if( nRows==0 )
		{
			delete pTable;
			pTable = NULL;

			continue;
		}
// 		pTable->CreateTable(nRows+1,6);
//		pTable->SetRows( nRows+1 );
//		pTable->SetCols(6);

		pTable->InsertColumn(0,"�۲�ʱ��");
		pTable->InsertColumn(1,"����״̬");
		pTable->InsertColumn(2,"�ۻ�\n������");
		pTable->InsertColumn(3,"�ۻ�\n�����ʵ�\n��׼��");
		pTable->InsertColumn(4,"�ۻ�\n�ս���");
		pTable->InsertColumn(5,"ʣ��\n�¼���");

		int nRemainedNum = pCurKMTable->nCaseNum, nEventNum = 0, iRow = 0;
		for( iTime=0; iTime<nPeriodNum; iTime++ )
		{
			TPeriodTable * pCurPeriodTable = pCurKMTable->aPeriodTable[iTime];
			if( pCurPeriodTable->nDiedNum!=0 )
			{
				strTemp.Format( "%g", pCurPeriodTable->dTime );
				pTable->InsertItem( iRow, strTemp );
				nRemainedNum -= pCurPeriodTable->nDiedNum;
				nEventNum += pCurPeriodTable->nDiedNum;
				pTable->SetItemText( iRow, 1, "�ս�" );
				pTable->SetItemText( iRow, 2, pCurPeriodTable->dCS );
				pTable->SetItemText( iRow, 3, pCurPeriodTable->dSE );
				pTable->SetItemText( iRow, 4, nEventNum );
				pTable->SetItemText( iRow, 5, nRemainedNum );
				iRow++;
			}
			if( pCurPeriodTable->nConsoredNum!=0 )
			{
				strTemp.Format( "%g", pCurPeriodTable->dTime );
				pTable->InsertItem( iRow, strTemp );
				nRemainedNum -= pCurPeriodTable->nConsoredNum;
				pTable->SetItemText( iRow, 1, "ɾʧ" );
				pTable->SetItemText( iRow, 4, nEventNum );
				pTable->SetItemText( iRow, 5, nRemainedNum );
				iRow++;
			}
		}
		
		CTString strRecord, strTime;
		strRecord.Format( "  �۲��¼����%d    ɾʧ��¼����%d     �ս��¼�����%d\n", pCurKMTable->nCaseNum, pCurKMTable->nCaseNum-nEventNum, nEventNum );

		if( nEventNum==0 )
			strTime = "* ��Ϊ�۲��¼ȫ��ɾʧ���޷����з���";
		else if( pCurKMTable->dMedianTime<0 )
			strTime.Format( "  ƽ������ʱ�䣺%0.2f", pCurKMTable->dMeanTime );
		else
			strTime.Format( "  ƽ������ʱ�䣺%0.2f    ��λ����ʱ�䣺%0.2f", pCurKMTable->dMeanTime, pCurKMTable->dMedianTime );

		strTemp = strRecord + strTime;

		pTable->SetTail(strTemp);

		CRsltVariable *pE  =new CRsltElementTable( strTableName, pTable );
		m_pResult->Add( pE );
	}

	if( m_nFactorPos>=0 )
	{
		// ���ͳ�Ʊ�
		// ������Ч����
		int nRow = 0;
		for( iType=0; iType<m_aKMTable.GetSize(); iType++ )
		{
			if( m_aKMTable[iType]->nCaseNum>0 )
				nRow++;
		}

		CTLTable * pTotalTable = new CTLTable;
		pTotalTable->SetTitle( strTitle );

		pTotalTable->InsertColumn(0,"����");
		pTotalTable->InsertColumn(1,"�۲���");
		pTotalTable->InsertColumn(2,"�ս���");
		pTotalTable->InsertColumn(3,"ɾʧ��");
		pTotalTable->InsertColumn(4,"ɾʧ�ٷֱ�");

		int nTotalCaseNum = 0;
		int nTotalEventNum = 0;
		for( iType=0; iType<m_aKMTable.GetSize(); iType++ )
		{
			TKMTable * pCurKMTable = m_aKMTable[iType];

			if( pCurKMTable->nCaseNum==0 )
				continue;

			if( pCurKMTable->strFactorLabel.IsEmpty() )
				strTemp.Format( "���ͣ�%g", pCurKMTable->dFactorValue );
			else
				strTemp.Format( "���ͣ�%s", pCurKMTable->strFactorLabel.GetData() );
			pTotalTable->InsertItem( iType, strTemp );

			pTotalTable->SetItemText( iType, 1, pCurKMTable->nCaseNum );
			pTotalTable->SetItemText( iType, 2, pCurKMTable->nEventNum );
			pTotalTable->SetItemText( iType, 3, pCurKMTable->nCaseNum-pCurKMTable->nEventNum );
			pTotalTable->SetItemText( iType, 4, (double)(pCurKMTable->nCaseNum-pCurKMTable->nEventNum)/(double)pCurKMTable->nCaseNum*100 );
			nTotalCaseNum += pCurKMTable->nCaseNum;
			nTotalEventNum += pCurKMTable->nEventNum;
		}

		pTotalTable->InsertItem( iType, "�ܼ�" );
		pTotalTable->SetItemText( iType, 1, nTotalCaseNum );
		pTotalTable->SetItemText( iType, 2, nTotalEventNum );
		pTotalTable->SetItemText( iType, 3, nTotalCaseNum-nTotalEventNum );
		pTotalTable->SetItemText( iType, 4, (double)(nTotalCaseNum-nTotalEventNum)/(double)nTotalCaseNum*100 );

		CRsltVariable *pETotal  =new CRsltElementTable( "ͳ�Ʊ�", pTotalTable );
		m_pResult->Add( pETotal );
	}

	return true;
}
