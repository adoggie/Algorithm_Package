//////////////////////////////////////////////////////////////////////
// CrossTab.cpp                                                     //
//																	//
// Define:        �����crosstabulation�㷨��ʵ��                   //
// Creator:       �Ķ�ɽ                                              //
// Date:          2006.03.20                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Define.h"
#include "crosstab.h"

const int OUTPUT_SUMMARY = 1;  //��¼����ͳ��
const int OUTPUT_CHISQUR = 2;  //����
const int OUTPUT_CORRLAT = 4;  //���ϵ��
const int DISPCOL_MAX    = 200;//�������ʾ���������
const int DISPROWCOL_MAX = 10000;//�������ʾ�����������
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrossTab::CCrossTab()
{
	m_nType = 0;
	m_bOutputChiSquare = FALSE;
	m_bOutputCorrelation = FALSE;
	m_bOutputExpCount = FALSE;
}

CCrossTab::~CCrossTab()
{
	if( m_tColInfo.iCount>0 )
		m_tColInfo.DeleteObject();
	if( m_tRowInfo.iCount>0 )
		m_tRowInfo.DeleteObject();
}

//	CROSSTABLE
//		/DATA=[FILE]
//		/ROWVARLIST [varlist]
//		/COLVARLIST [varlist]
//		/WEIGHT [varname]
//		/DISPLAY [PERCENT] [CHISQUARE] [CORR]
BOOL CCrossTab::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	CTStringArray xname;
	TIntStruct tVarTemp;

	AnaWord VOstr;
	VOstr.Import(VO);
	//������Ȩ��
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		if (tVarTemp.iCount == 1)
			m_nWeight = tVarTemp.pValue[0];
		else
			return FALSE;
		tVarTemp.DeleteObject();
		m_bWeight = TRUE;
	}
	else
	{
		m_bWeight = FALSE;
	}
	//����������
	xname.RemoveAll();
	AnaSyntax::getVAR("/ROWVARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tRowInfo)))
		return FALSE;

	xname.RemoveAll();
	AnaSyntax::getVAR("/COLVARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tColInfo)))
		return FALSE;
	
	m_bOutputChiSquare = AnaSyntax::findstr("/DISPLAY", "CHISQUARE", VOstr);
	m_bOutputCorrelation = AnaSyntax::findstr("/DISPLAY", "CORR", VOstr);
	m_bOutputExpCount = AnaSyntax::findstr("/DISPLAY", "PERCENT", VOstr);
	return TRUE;
}

// �ⲿ����������
CResult * CCrossTab::OnCrossTab(CDataInterface *pDataInterface, string VO)
{
	m_pResult = NULL;
	m_nType = m_nCrossTableCnt = 0;
	if (!pDataInterface) return m_pResult;
	//�������
	m_pResult = new CResult("�����");
	//���ӿ�ָ������
	m_pDataInterface = pDataInterface;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	CrossTab();
	return m_pResult;
}

//�õ���ǩ
CTString CCrossTab::GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue)
{
	BOOL bShowLabel = FALSE;
	CTString szText(""),szLabel("");

	if (m_pDataInterface == NULL)
	{
		szLabel.Format("%.4f",fValue);
		return szLabel;
	}
	bShowLabel = m_pDataInterface->GetShowValueLabel();
	CFieldType fieldtype = m_pDataInterface->GetFieldType(nColIndex);
	CField *pField = m_pDataInterface->FieldByIndex(nColIndex);
	if (pField == NULL)
		return szLabel;
	if (fieldtype == fString)
	{
		szText = dataMap.GetStrValue(nPos,int(fValue));
		if (bShowLabel)
		{
			if (!pField->GetValueVL(szText,szLabel))
				szLabel = szText;
		}
		else
		{
			szLabel = szText;
		}
	}
	else
	if (fieldtype == fDouble)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(fValue,szLabel))
				szLabel.Format("%.4f",fValue);
		}
		else
		{
			szLabel.Format("%.4f",fValue);
		}
	}
	else
	if (fieldtype == fInt)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(int(fValue),szLabel))
				szLabel.Format("%.0f",fValue);
		}
		else
		{
			szLabel.Format("%.0f",fValue);
		}
	}
	else
	if (fieldtype == fBoolean)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(bool(fValue),szLabel))
				szLabel.Format("%.0f",fValue);
		}
		else
		{
			szLabel.Format("%.0f",fValue);
		}
	}
	else
	{
		szLabel.Format("%.0f",fValue);
	}
	return szLabel;
}

void CCrossTab::CrossTab()
{
	CDWordArray   arrCol;
	CIntVector    vecRowInd;  //����
	CDoubleMatrix dataMatrix;
	CMapManager   dataMap;
	CDoubleMatrix weightMatrix;
	CDoubleVector vecCol, vecRow;
	CPtrArray arrColumn,arrRow;
	int nRow = 0, nCol = 0, nRowCnt = 0;
	
	if (m_tRowInfo.iCount < 1 || m_tColInfo.iCount < 1)
		return;
	m_nCurRows = 0;
	m_pSummaryTable     = NULL;//���ܱ�
	m_pCorrelationTable = NULL;//���ϵ����
	m_pChiSquareTable   = NULL;//������
	//������
	int nAllRowCnt = m_pDataInterface->GetRowCount();
	//��ȡȨ��
	if (m_bWeight)
	{
		arrCol.RemoveAll();
		arrCol.Add(m_nWeight);
		nRowCnt = m_pDataInterface->GetColsData(arrCol,weightMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning.Format("ע�� ����ѡ���Ȩ�����ݲ��ʺ�����ǰ������������ѡ��");
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
		}
		//�ܵ�Ȩ��
		m_n64Weight = __int64(weightMatrix(0).sum());
		weightMatrix.destroy();
	}
	else
	{
		m_n64Weight = nAllRowCnt;
	}
	//��ȡ����
	for (nRow=0;nRow<m_tRowInfo.iCount;nRow++)
	{
		for (nCol=0;nCol<m_tColInfo.iCount;nCol++)
		{
			arrCol.RemoveAll();
			dataMap.Clear();
			if (m_bWeight)
			{
				arrCol.Add(m_nWeight);
			}
			arrCol.Add(m_tRowInfo.pValue[nRow]);
			arrCol.Add(m_tColInfo.pValue[nCol]);
			//����ȱʡֵɾ��
			nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,dataMap);
			if (nRowCnt < 1)
			{
				CTString szWarning("");
				if (m_bWeight)
					szWarning.Format("ע�� �б���:%s-�б���:%s-Ȩ��:%s-û�к��ʵļ�¼��������ͳ�Ʒ�����",
					m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_tColInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_nWeight).GetData());
				else
					szWarning.Format("ע�� �б���%s-�б���%s-û�к��ʵļ�¼��������ͳ�Ʒ�����",
					m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_tColInfo.pValue[nRow]).GetData());
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
				pWarningTextRslt->AddString(szWarning);
				m_pResult->Add(pWarningTextRslt);
				continue;
			}
			//���ݴ���-��¼ͳ��
			CreateSummaryTable(dataMatrix,nRow,nCol);
			//�����б�����Ϣ
			CDoubleVector a(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector b(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector c(dataMatrix(0));
			BOOL bRet = CalcFldInf(a,b,c,&arrColumn,CALC_COLFLD);
			//�����б�����Ϣ
			CDoubleVector a2(dataMatrix(m_bWeight ? 1 : 0));
			CDoubleVector b2(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector c2(dataMatrix(0));
			if (bRet) bRet = CalcFldInf(a2,b2,c2,&arrRow,CALC_ROWFLD);
			/////////////////////////////////////////////////////////////////////
			//���ݴ���-�����
			if (bRet)
			{
				CreateCrossTable(dataMatrix,dataMap,&arrRow,&arrColumn,nRow,nCol);
				//���ϵ��
				CreateStatisticTable(dataMatrix,&arrRow, &arrColumn,nRow,nCol);
				m_nCurRows ++;
			}
			//�ͷ��ڴ�
			Release(&arrRow);
			Release(&arrColumn);
		}
	}
}

// ������¼ͳ�Ʊ�
void CCrossTab::CreateSummaryTable(CDoubleMatrix &dataMatrix, int nRowIndex, int nColIndex)
{
	int     nRowCnt = 0;
	double  fWeight = 0.0;
	CTString szText("");
	//��ṹ
	if (m_pSummaryTable == NULL)
	{
		m_pSummaryTable = new CTLTable;
		m_pSummaryTable->SetTitle("��¼����ͳ��");
		m_pSummaryTable->SetCols(4);

		m_pSummaryTable->InsertColumn(0, "");
		if (m_bWeight)
		{
			m_pSummaryTable->InsertColumn(1, "��ЧȨ�غ�");
			m_pSummaryTable->InsertColumn(2, "ȱʧȨ�غ�");
			m_pSummaryTable->InsertColumn(3, "��Ȩ�غ�");
		}
		else
		{
			m_pSummaryTable->InsertColumn(1, "��Ч��¼��");
			m_pSummaryTable->InsertColumn(2, "ȱʧ��¼��");
			m_pSummaryTable->InsertColumn(3, "�ܼ�¼��");
		}
	}
	//������
	szText.Format("%s �� %s",m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRowIndex]).GetData(),m_pDataInterface->GetFieldName(m_tColInfo.pValue[nColIndex]).GetData());
	m_pSummaryTable->InsertItem(m_nCurRows,szText);
	if (m_bWeight)
	{
		fWeight = dataMatrix(0).sum();
		szText.Format("%.0f",fWeight);
		m_pSummaryTable->SetItemText(m_nCurRows,1,szText);
		szText.Format("%.0f",double(m_n64Weight - fWeight));
		m_pSummaryTable->SetItemText(m_nCurRows,2,szText);
		szText.Format("%u",m_n64Weight);
		m_pSummaryTable->SetItemText(m_nCurRows,3,szText);
	}
	else
	{
		nRowCnt = dataMatrix.mrow();
		szText.Format("%d",nRowCnt);
		m_pSummaryTable->SetItemText(m_nCurRows,1,szText);
		szText.Format("%u",__int64(m_n64Weight - nRowCnt));
		m_pSummaryTable->SetItemText(m_nCurRows,2,szText);
		szText.Format("%u",m_n64Weight);
		m_pSummaryTable->SetItemText(m_nCurRows,3,szText);
	}
	if ((m_nType & OUTPUT_SUMMARY) == 0)
	{
		CRsltElementTable * pSummaryTableRslt = new CRsltElementTable( "��¼����ͳ��", m_pSummaryTable );
		m_pResult->Add(pSummaryTableRslt);
		m_nType |= OUTPUT_SUMMARY;
	}
}

// ����ͳ������
void CCrossTab::CreateStatisticTable(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol)
{
	CTString szText("");
	BOOL bCorrHaveString = FALSE, bChiHaveString = FALSE;
	CTString szRowName = m_pDataInterface->GetFieldName( m_tRowInfo.pValue[nRow] );
	CTString szColName = m_pDataInterface->GetFieldName( m_tColInfo.pValue[nCol] );
	szText = szRowName + " �� " + szColName;
	
	if( m_bOutputCorrelation ) // ������ϵ����
	{
		if (m_pCorrelationTable == NULL)
		{
			m_pCorrelationTable = new CTLTable;
			m_pCorrelationTable->SetTitle( "���ϵ����" );
			m_pCorrelationTable->SetRows( m_tRowInfo.iCount * m_tColInfo.iCount + 1 );
			m_pCorrelationTable->SetCols( 3 );
			m_pCorrelationTable->InsertColumn( 0, "");
			m_pCorrelationTable->InsertColumn( 1, "Ƥ��ѷ\n���ϵ��");
			m_pCorrelationTable->InsertColumn( 2, "���������\n(˫β)");
		}
		//�������ϵ��
		bCorrHaveString = CalcCorrlation(dataMatrix,nRow,nCol);
		//���ϵ��
		szText = szRowName + " �� " + szColName;
		m_pCorrelationTable->InsertItem(m_nCurRows,szText);
		if (bCorrHaveString)
		{
			szText.Format("%.04f",m_fPearsonCorr);
			m_pCorrelationTable->SetItemText(m_nCurRows,1,szText);
			szText.Format("%.04f",2 * m_fSig);
			m_pCorrelationTable->SetItemText(m_nCurRows,2,szText);
		}
		else
		{
			m_pCorrelationTable->SetItemText(m_nCurRows,1,"NA");
			m_pCorrelationTable->SetItemText(m_nCurRows,2,"NA");
		}
	}
	if( m_bOutputChiSquare ) //������������ɶ�
	{
		if (m_pChiSquareTable == NULL)
		{
			m_pChiSquareTable = new CTLTable;
			m_pChiSquareTable->SetTitle( "������" );
			m_pChiSquareTable->SetRows( m_tRowInfo.iCount * m_tColInfo.iCount + 1 );
			m_pChiSquareTable->SetCols( 6 );
			m_pChiSquareTable->InsertColumn( 0, "");
			m_pChiSquareTable->InsertColumn( 1, "Ƥ��ѷ\n����");
			m_pChiSquareTable->InsertColumn( 2, "Ƥ��ѷ����\n������\n(˫β)");
			m_pChiSquareTable->InsertColumn( 3, "��Ȼ��\n����" );
			m_pChiSquareTable->InsertColumn( 4, "��Ȼ�ȿ���\n������\n(˫β)");
			m_pChiSquareTable->InsertColumn( 5, "����\n���ɶ�" );
		}
		//���㿨�������ɶ�
		bChiHaveString = CalcChiSquare(dataMatrix,pArrRow,pArrCol);
		//���������ɶ�
		szText = szRowName + " �� " + szColName;
		m_pChiSquareTable->InsertItem(m_nCurRows,szText);
		if (m_n64Weight > 1 && m_nDF > 0 )
		{
			m_pChiSquareTable->SetItemText(m_nCurRows, 1, m_fPChiSquare);
			m_pChiSquareTable->SetItemText(m_nCurRows, 2, m_fPChiSquareSig);
			m_pChiSquareTable->SetItemText(m_nCurRows, 3, m_fLChiSquare);
			m_pChiSquareTable->SetItemText(m_nCurRows, 4, m_fLChiSquareSig);
			m_pChiSquareTable->SetItemText(m_nCurRows, 5, m_nDF);
		}
		else
		{
			m_pChiSquareTable->SetItemText(m_nCurRows, 1, "NA" );
			m_pChiSquareTable->SetItemText(m_nCurRows, 2, "NA" );
			m_pChiSquareTable->SetItemText(m_nCurRows, 3, "NA" );
			m_pChiSquareTable->SetItemText(m_nCurRows, 4, "NA" );
			m_pChiSquareTable->SetItemText(m_nCurRows, 5, "NA" );
		}
	}
	// �����δ�������ֵ�����NA����
	if (m_bOutputCorrelation && (m_nType & OUTPUT_CHISQUR) == 0)
	{
		CRsltElementTable *pCorrTableRslt = new CRsltElementTable( "���ϵ����", m_pCorrelationTable );
		m_pResult->Add(pCorrTableRslt);
		m_nType |= OUTPUT_CHISQUR;
	}
	if (m_bOutputChiSquare && (m_nType & OUTPUT_CORRLAT) == 0)
	{
		CRsltElementTable *pChiTableRslt = new CRsltElementTable( "������", m_pChiSquareTable );
		m_pResult->Add(pChiTableRslt);
		m_nType |= OUTPUT_CORRLAT;
	}
	if (m_pCorrelationTable && !bCorrHaveString)
		m_pCorrelationTable->SetTail( "\n NA�����������Ͳ��ʺϻ�����Ч��¼����ĳ�ֶ�Ϊ�������޷�����ͳ�Ƽ��㡣" );
	if (m_pChiSquareTable && !bChiHaveString)
		m_pChiSquareTable->SetTail( "\n NA�����������Ͳ��ʺϻ�����Ч��¼����ĳ�ֶ�Ϊ�������޷�����ͳ�Ƽ��㡣" );
}

void CCrossTab::CreateCrossTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol)
{
	CTString szText("");
	CFieldType fColType;
	int nVlen = 0, nColCount = 0, nColStart = 0;
	
	int nColIndex = m_bWeight ? 2 : 1;

	m_nCrossTableCnt ++;
	nVlen = dataMatrix(0).vlen();
	nColCount = pArrCol->GetSize();
	//��ӽ����ṹ��Ϣ
	CTString szRowName = m_pDataInterface->GetFieldName( m_tRowInfo.pValue[nRow] );
	CTString szColName = m_pDataInterface->GetFieldName( m_tColInfo.pValue[nCol] );
	CTString szTableName = szRowName + " �� " + szColName + " �����";
	CTLTable * pCrossTable = new CTLTable;
	pCrossTable->SetTitle( szTableName ); // ���ñ�ͷ
	if (nColCount > DISPCOL_MAX)
	{
		szText.Format( "ע����ѡ�ı������ʺ�����ǰ��������������%d���޷���ʾ�ñ�", DISPCOL_MAX);
		pCrossTable->SetRows( 1 );
		pCrossTable->SetCols( 0 );
		pCrossTable->InsertColumn(0, szText);
		CRsltElementTable *pRsltElement = new CRsltElementTable(szTableName, pCrossTable);
		if (!m_pResult->InsetAt(m_nCrossTableCnt,pRsltElement))
			m_pResult->Add(pRsltElement);
		return;
	}
	if (nColCount * pArrRow->GetSize() > DISPROWCOL_MAX)
	{
		szText.Format( "ע����ѡ�ı������ʺ�����ǰ��������������������%d���޷���ȫ��ʾ %s��", DISPROWCOL_MAX,szTableName.GetData());
		pCrossTable->SetRows( 1 );
		pCrossTable->SetCols( 0 );
		pCrossTable->InsertColumn(0, szText);
		CRsltElementTable *pRsltElement = new CRsltElementTable(szTableName, pCrossTable);
		if (!m_pResult->InsetAt(m_nCrossTableCnt,pRsltElement))
			m_pResult->Add(pRsltElement);
		return;
	}
	//������
	pCrossTable->SetCols(nColCount + (m_bOutputExpCount ? 3 : 2));
	pCrossTable->InsertColumn( 0, "" );
	nColStart = 1;
	if( m_bOutputExpCount )
	{
		pCrossTable->InsertColumn(1, "");
		nColStart = 2;
	}
	//�ֶ�����
	fColType = m_pDataInterface->GetFieldType(m_tColInfo.pValue[nCol]);
	for (int i=0;i<nColCount;i++)
	{
		CFldInfo *pFldInf = (CFldInfo *)pArrCol->GetAt(i);
		szText = GetLabel(dataMap,nColIndex,m_tColInfo.pValue[nCol],pFldInf->fValue);
		pCrossTable->InsertColumn(nColStart,szText);
		nColStart ++;
	}
	pCrossTable->InsertColumn(nColStart, "�ܼ�");
	//�����¼�������
	InsertCrossTab(dataMatrix, dataMap, pArrRow, pArrCol, pCrossTable, nRow);
	CRsltElementTable * pRsltElement = new CRsltElementTable( szTableName, pCrossTable );
	if (!m_pResult->InsetAt(m_nCrossTableCnt,pRsltElement))
		m_pResult->Add(pRsltElement);
}

BOOL CCrossTab::InsertCrossTab(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, CTLTable * pCrossTable, int nRow)
{
	int nRowStart = 0;
	CFldInfo *pRowInf = NULL;

	int nRowSize = pArrRow->GetSize();
	//�ܵ���ЧȨ�ػ��¼��
	double f64Weight = m_bWeight ? dataMatrix(0).sum() : dataMatrix(0).vlen();
	nRowStart = 0;
	for (int i=0;i<nRowSize;i++)
	{
		pRowInf = (CFldInfo *)pArrRow->GetAt(i);
		//������
		InsertCorssTabOfRow(dataMap,pCrossTable,pRowInf,pArrCol,nRowStart,nRow,f64Weight);
		nRowStart ++;
	}
	//�ܵ�
	InsertCorssTabOfRow(dataMap,pCrossTable,pRowInf,pArrCol,nRowStart,nRow,f64Weight,TRUE);	
	return TRUE;
}

void CCrossTab::InsertCorssTabOfRow(CMapManager &dataMap, CTLTable * pCrossTable, CFldInfo* pRowInf, CPtrArray *pArrCol, int nRowStart, int nRow, double fTotalWeight, BOOL bLast)
{
	CTString szText("");
	CFieldType fRowType;
	int nVlen = 0, nRowIndex = 0;
	int nColIndex = 0, nColStart = 0;
	CPtrArray arrTmp;
	CFldInfo *pColInf = NULL, *pTmpInf = NULL;

	//��ʼ��
	nColStart = m_bOutputExpCount ? 2 : 1;
	//����
	nRowIndex = m_bWeight ? 1 : 0;
	//����
	nColIndex = m_bWeight ? 2 : 1;
	fRowType = m_pDataInterface->GetFieldType(m_tRowInfo.pValue[nRow]);
	if (m_bOutputExpCount) //����ٷ���
	{
		//�Ƿ�Ϊ���
		if (bLast)
		{
			pCrossTable->InsertItem(nRowStart*3, "�ܼ�", FALSE);
			pCrossTable->SetItemText(nRowStart*3, 1, "����");
			pCrossTable->InsertItem(nRowStart*3+1, "");
			pCrossTable->SetItemText(nRowStart*3+1, 1, "�ٷֱ�%");
			for (int i=0;i<pArrCol->GetSize();i++)
			{
				pColInf = (CFldInfo *)pArrCol->GetAt(i);
				szText.Format("%.0f",pColInf->fWeight);
				pCrossTable->SetItemText(nRowStart*3, pColInf->nIndex,szText);
				if (fTotalWeight > DOUBLE_ZERO)
					szText.Format("%.4f",100*pColInf->fWeight/fTotalWeight);
				else
					szText = _T("NA");
				pCrossTable->SetItemText(nRowStart*3+1, pColInf->nIndex, szText);
			}
			szText.Format("%.0f",fTotalWeight);
			pCrossTable->SetItemText(nRowStart*3,pArrCol->GetSize() + 2, szText);
			return;
		}
		/////////////////////////////////////////////////
		if (fRowType == fString)
		{
			//szText = dataMap.GetStrValue(nRowIndex,int(fRowValue));
			szText = GetLabel(dataMap,nRowIndex,m_tRowInfo.pValue[nRow],pRowInf->fValue);
			pCrossTable->InsertItem(nRowStart*3, szText, FALSE);
			pCrossTable->SetItemText(nRowStart*3, 1, "����");
			pCrossTable->InsertItem(nRowStart*3+1, "", FALSE);
			pCrossTable->SetItemText(nRowStart*3+1, 1, "�аٷֱ�%");
			pCrossTable->InsertItem(nRowStart*3+2, "" );
			pCrossTable->SetItemText(nRowStart*3+2, 1, "�аٷֱ�%");
		}
		else
		{
			/*
			if (fRowType == fInt)
				szText.Format("%.0f",fRowValue);
			else
				szText.Format("%.4f",fRowValue);
			*/
			szText = GetLabel(dataMap,nRowIndex,m_tRowInfo.pValue[nRow],pRowInf->fValue);
			pCrossTable->InsertItem(nRowStart*3, szText, FALSE);
			pCrossTable->SetItemText(nRowStart*3, 1, "����");
			pCrossTable->InsertItem(nRowStart*3+1, "", FALSE);
			pCrossTable->SetItemText(nRowStart*3+1, 1, "�б���%");
			pCrossTable->InsertItem(nRowStart*3+2, "");
			pCrossTable->SetItemText(nRowStart*3+2, 1, "�б���%");
		}
		//�Ծ�������������м���
		CalcFldInf(pRowInf->v,pRowInf->v,pRowInf->w,&arrTmp,CALC_TMPFLD);
		nVlen = arrTmp.GetSize();
		for (int i=0;i<nVlen;i++)
		{
			pTmpInf = (CFldInfo *)arrTmp.GetAt(i);
			//���
			pColInf = GetFldInfByValue(pTmpInf->fValue,pArrCol);
			if (pColInf)
			{
				for (int k=nColStart;k<pColInf->nIndex;k++)
				{
					pCrossTable->SetItemText(nRowStart*3, k, "0");
					pCrossTable->SetItemText(nRowStart*3+1, k, "0.0000");
					pCrossTable->SetItemText(nRowStart*3+2, k, "0.0000");
				}
				szText.Format("%.0f",pTmpInf->fWeight);
				pCrossTable->SetItemText(nRowStart*3, pColInf->nIndex, szText);
				if (pRowInf->fWeight > DOUBLE_ZERO)
					szText.Format("%.4f",100*pTmpInf->fWeight/pRowInf->fWeight);
				else
					szText = _T("NA");
				pCrossTable->SetItemText(nRowStart*3+1, pColInf->nIndex, szText);
				if (pColInf->fWeight > DOUBLE_ZERO)
					szText.Format("%.4f",100*pTmpInf->fWeight/pColInf->fWeight);
				else
					szText = _T("NA");
				pCrossTable->SetItemText(nRowStart*3+2, pColInf->nIndex, szText);
				nColStart = pColInf->nIndex+1;
			}
		}
		for (int k=pColInf->nIndex+1;k<pArrCol->GetSize() + 2;k++)
		{
			pCrossTable->SetItemText(nRowStart*3, k, "0");
			pCrossTable->SetItemText(nRowStart*3+1, k, "0.0000");
			pCrossTable->SetItemText(nRowStart*3+2, k, "0.0000");
		}
		//ĳ���ܵ�
		szText.Format("%.0f",pRowInf->fWeight);
		pCrossTable->SetItemText(nRowStart*3, pArrCol->GetSize() + 2, szText);	
		if (fTotalWeight > DOUBLE_ZERO)
			szText.Format("%.4f",100*pRowInf->fWeight/fTotalWeight);
		else
			szText = _T("NA");
		pCrossTable->SetItemText(nRowStart*3+1, pArrCol->GetSize() + 2, szText);
		//////////////////////////////////////////////////
	}
	else
	{
		//�Ƿ�Ϊ���
		if (bLast)
		{
			pCrossTable->InsertItem(nRowStart, "�ܼ�", TRUE);
			for (int i=0;i<pArrCol->GetSize();i++)
			{
				pColInf = (CFldInfo *)pArrCol->GetAt(i);
				szText.Format("%.0f",pColInf->fWeight);
				pCrossTable->SetItemText(nRowStart, pColInf->nIndex,szText);
			}
			szText.Format("%.0f",double(fTotalWeight));
			pCrossTable->SetItemText(nRowStart,pArrCol->GetSize() + 1, szText);
			return;
		}
		/////////////////////////////////////////////////
		if (fRowType == fString)
		{
			//szText = dataMap.GetStrValue(nRowIndex,int(fRowValue));
			szText = GetLabel(dataMap,nRowIndex,m_tRowInfo.pValue[nRow],pRowInf->fValue);
			pCrossTable->InsertItem(nRowStart, szText, TRUE);
		}
		else
		{
			/*
			if (fRowType == fInt)
				szText.Format("%.0f",fRowValue);
			else
				szText.Format("%.4f",fRowValue);
			*/
			szText = GetLabel(dataMap,nRowIndex,m_tRowInfo.pValue[nRow],pRowInf->fValue);
			pCrossTable->InsertItem(nRowStart, szText, TRUE);
		}
		//�Ծ�������������м���
		CalcFldInf(pRowInf->v,pRowInf->v,pRowInf->w,&arrTmp,CALC_TMPFLD);
		nVlen = arrTmp.GetSize();
		for (int i=0;i<nVlen;i++)
		{
			pTmpInf = (CFldInfo *)arrTmp.GetAt(i);
			//���
			pColInf = GetFldInfByValue(pTmpInf->fValue,pArrCol);
			if (pColInf)
			{
				for (int k=nColStart;k<pColInf->nIndex;k++)
				{
					pCrossTable->SetItemText(nRowStart, k, "0");
				}
				szText.Format("%.0f",pTmpInf->fWeight);
				pCrossTable->SetItemText(nRowStart, pColInf->nIndex, szText);
				nColStart = pColInf->nIndex+1;
			}
		}
		for (int k=pColInf->nIndex+1;k<pArrCol->GetSize() + 1;k++)
		{
			pCrossTable->SetItemText(nRowStart, k, "0");
		}
		//ĳ���ܵ�
		szText.Format("%.0f",pRowInf->fWeight);
		pCrossTable->SetItemText(nRowStart, pArrCol->GetSize() + 1, szText);
		//////////////////////////////////////////////////
	}
	//�ͷ��ڴ�
	Release(&arrTmp);
}

BOOL CCrossTab::CalcCorrlation(CDoubleMatrix &dataMatrix, int nRow, int nCol)
{
	int nRowIndex = 0, nColIndex = 0;
	double fWeight = 1, fWeightSum = 0;
	double fTemp_i = 0, fTemp_j = 0, fTemp_ij = 0, fTemp_ii = 0, fTemp_jj = 0;
	
	//����
	nRowIndex = m_bWeight ? 1 : 0;
	//����
	nColIndex = m_bWeight ? 2 : 1;
	int nVlen = dataMatrix.mrow();
	//�Ƿ�������ϵ��
	if (!m_bOutputCorrelation ||
		m_pDataInterface->GetFieldType(m_tRowInfo.pValue[nRow]) == fString ||
		m_pDataInterface->GetFieldType(m_tColInfo.pValue[nCol]) == fString )
		return FALSE;
	if (m_bWeight)
	{
		fWeightSum = dataMatrix(0).sum();
		if (fWeightSum >= 2)
		{
			for (int i=0;i<nVlen;i++)
			{
				fWeight = dataMatrix(0)(i);
				// ����
				fTemp_i += dataMatrix(nRowIndex)(i) * fWeight;
				fTemp_j += dataMatrix(nColIndex)(i) * fWeight;
				fTemp_ij += dataMatrix(nRowIndex)(i) * dataMatrix(nColIndex)(i) * fWeight;
				fTemp_ii += dataMatrix(nRowIndex)(i) * dataMatrix(nRowIndex)(i) * fWeight;
				fTemp_jj += dataMatrix(nColIndex)(i) * dataMatrix(nColIndex)(i) * fWeight;
			}
		}
	}
	else
	{
		fWeightSum = nVlen;
		for (int i=0;i<nVlen;i++)
		{
			// ����
			fTemp_i += dataMatrix(nRowIndex)(i);
			fTemp_j += dataMatrix(nColIndex)(i);
			fTemp_ij += dataMatrix(nRowIndex)(i) * dataMatrix(nColIndex)(i);
			fTemp_ii += dataMatrix(nRowIndex)(i) * dataMatrix(nRowIndex)(i);
			fTemp_jj += dataMatrix(nColIndex)(i) * dataMatrix(nColIndex)(i);
		}
	}
	//�������Ƥ��ѷ���ϵ����������
	if (fWeightSum-2.0 < DOUBLE_ZERO && fWeightSum - 2.0 > -DOUBLE_ZERO)
		return FALSE;
	double fCrossDevs = fTemp_ij - (fTemp_i * fTemp_j / fWeightSum);
	double fCrossDevSelf_i = fTemp_ii - (fTemp_i * fTemp_i / fWeightSum );
	double fCrossDevSelf_j = fTemp_jj - (fTemp_j * fTemp_j / fWeightSum );
	if( fWeightSum < 2 || nVlen ==1 || 
		(fCrossDevSelf_i > -DOUBLE_ZERO && fCrossDevSelf_i < DOUBLE_ZERO) ||
		(fCrossDevSelf_j > -DOUBLE_ZERO && fCrossDevSelf_j < DOUBLE_ZERO))
		m_fPearsonCorr = 1;
	else
		m_fPearsonCorr = fCrossDevs / sqrt(fCrossDevSelf_i * fCrossDevSelf_j);

	if( 1-fabs(m_fPearsonCorr) < DOUBLE_ZERO || fWeightSum <= 2.0 )
		m_fSig = 0.0;
	else
	{
		double t = fabs(m_fPearsonCorr) * sqrt((fWeightSum - 2)/
				   (1 - m_fPearsonCorr * m_fPearsonCorr));
		m_fSig = 1 - tcdf( t, fWeightSum-2 );
	}
	return TRUE;
}

BOOL CCrossTab::CalcChiSquare(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol)
{
	int i = 0, j = 0, k = 0;

	int nVlen   = dataMatrix.mrow();
	int nRowCnt = pArrRow->GetSize();
	int nColCnt = pArrCol->GetSize();
	int nRow = m_bWeight ? 1 : 0;
	int nCol = m_bWeight ? 2 : 1;
	CFldInfo *pInf = NULL;
		
	if (m_n64Weight < DOUBLE_ZERO) return FALSE;
	
	CDoubleVector wVec(nRowCnt*nColCnt,0);//�С���
	if (m_bWeight)
	{
		for(k=0;k<nVlen;k++)
		{	
			for (i=0;i<nRowCnt;i++)
			{ 
				pInf = (CFldInfo *)pArrRow->GetAt(i);
				if (dataMatrix(nRow)(k) == pInf->fValue)
					break;
			}
			for (j=0;j<nColCnt;j++)
			{
				pInf = (CFldInfo *)pArrCol->GetAt(j);
				if (dataMatrix(nCol)(k) == pInf->fValue)
					break;
			}
			wVec(i*nColCnt+j) += dataMatrix(0)(k);
		}
	}
	else
	{
		for(k=0;k<nVlen;k++)
		{	
			for (i=0;i<nRowCnt;i++)
			{ 
				pInf = (CFldInfo *)pArrRow->GetAt(i);
				if (dataMatrix(nRow)(k) == pInf->fValue)
					break;
			}
			for (j=0;j<nColCnt;j++)
			{
				pInf = (CFldInfo *)pArrCol->GetAt(j);
				if (dataMatrix(nCol)(k) == pInf->fValue)
					break;
			}
			wVec(i*nColCnt+j) += 1;
		}
	}	
	if (m_bOutputChiSquare) //������������ɶ�
	{
		// ��Ƥ��ѷ��������Ȼ�ȿ��������ɶ�
		m_fPChiSquare = 0;
		m_fLChiSquare = 0;
		m_fPChiSquareSig = 0;
		m_fLChiSquareSig = 0;
		
		for (i=0; i<nRowCnt; i++)
		{
			CFldInfo *pRowInf = (CFldInfo *)pArrRow->GetAt(i);
			for (j=0; j<nColCnt; j++)
			{
				CFldInfo *pColInf = (CFldInfo *)pArrCol->GetAt(j);
				double e =  (double)pRowInf->fWeight*pColInf->fWeight/m_n64Weight;
				if (fabs(e) > DOUBLE_ZERO)
					m_fPChiSquare += (wVec(i*nColCnt+j)-e) * (wVec(i*nColCnt+j)-e) / e;
				if (wVec(i*nColCnt+j) > DOUBLE_ZERO)
					m_fLChiSquare += wVec(i*nColCnt+j) * log(e/wVec(i*nColCnt+j));
			}
		}
		m_fLChiSquare *= -2;
		m_nDF = (nColCnt-1) * (nRowCnt-1);
		if (m_nDF > DOUBLE_ZERO)
		{
			m_fPChiSquareSig = 1 - chi2cdf(m_fPChiSquare, m_nDF);
			m_fLChiSquareSig = 1 - chi2cdf(m_fLChiSquare, m_nDF);
		}
	}
	return TRUE;
}

CFldInfo* CCrossTab::GetFldInfByValue(double fValue, CPtrArray *pArrColInf)
{
	double fZero = 0;
	CFldInfo *pColInf = NULL;

	int nSize = pArrColInf->GetSize();
	if (nSize < 0) return NULL;
	for (int i=0;i<nSize;i++)
	{
		pColInf = (CFldInfo *)pArrColInf->GetAt(i);
		fZero = fValue - pColInf->fValue;
		if (fZero > DOUBLE_ZERO || fZero < -DOUBLE_ZERO)
			continue;
		return pColInf;
	}
	return NULL;
}

void CCrossTab::CalcFldInf(double fValue, double fColValue, double fWeight, CPtrArray *pArrFld, int nSize, int nType)
{
	CFldInfo *pFld = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArrFld->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pFld = (CFldInfo *)pArrFld->GetAt(nMid);
		fTmp = pFld->fValue-fValue;
		if (fTmp < -DOUBLE_ZERO)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else
		if (fTmp > DOUBLE_ZERO)
		{
			nEnd = nMid-1;
		}
		else
		{
			if (nType == CALC_ROWFLD)
			{
				pFld->nIndex ++;
				pFld->v(pFld->nIndex) = fColValue;
				pFld->w(pFld->nIndex) = fWeight;
			}
			pFld->fWeight += fWeight;
			return;
		}
	}
	//����
	pFld = new CFldInfo;
	pFld->fValue  = fValue;
	pFld->fWeight = fWeight;
	if (nType == CALC_ROWFLD)
	{
		//��������
		pFld->nIndex = 0;
		pFld->v.create(nSize);
		pFld->w.create(nSize);
		pFld->v(pFld->nIndex) = fColValue;
		pFld->w(pFld->nIndex) = fWeight;
	}
	//����
	pArrFld->InsertAt(nMid,pFld);
}

BOOL CCrossTab::CalcFldInf(  CDoubleVector &v,CDoubleVector &vc, CDoubleVector &w, CPtrArray *pArrFld, int nType)
{
	int i=0,nVlen=0;
	int nIndexSep=0;
	double fValue=0, fColValue=0, fWeight=0;
	CFldInfo *pFld = NULL;
	
	nVlen = v.vlen();
	if (nType == CALC_COLFLD) //ȡ����Ϣ
	{
		nIndexSep = (m_bOutputExpCount ? 2 : 1);
	}
	//Ȩ��
	if (m_bWeight)
	{
		for (i=0;i<nVlen;i++)
		{
			fValue  = v(i);
			fWeight = w(i);
			if (nType == CALC_ROWFLD) fColValue = vc(i);//��-ȡ��Ӧ����ֵ
			CalcFldInf(fValue,fColValue,fWeight,pArrFld,nVlen,nType);
			if (nType == CALC_COLFLD && pArrFld->GetSize() > DISPCOL_MAX) return FALSE;
			if (nType == CALC_ROWFLD && pArrFld->GetSize() * m_nColSize > DISPROWCOL_MAX) return FALSE;
		}
	}
	else
	{
		for (i=0;i<nVlen;i++)
		{
			fValue  = v(i);
			if (nType == CALC_ROWFLD) fColValue = vc(i);//��-ȡ��Ӧ����ֵ
			CalcFldInf(fValue,fColValue,1,pArrFld,nVlen,nType);
			if (nType == CALC_COLFLD && pArrFld->GetSize() > DISPCOL_MAX) return FALSE;
			if (nType == CALC_ROWFLD && pArrFld->GetSize() * m_nColSize > DISPROWCOL_MAX) return FALSE;
		}
	}
	//��
	if (nType == CALC_ROWFLD)
	{
		nVlen = pArrFld->GetSize();
		for (i=0;i<nVlen;i++)
		{
			pFld = (CFldInfo *)pArrFld->GetAt(i);
			pFld->v.resize(pFld->nIndex+1);
			pFld->w.resize(pFld->nIndex+1);
		}
	}
	else
	if (nType == CALC_COLFLD)
	{
		nVlen = pArrFld->GetSize();
		for (i=0;i<nVlen;i++)
		{
			pFld = (CFldInfo *)pArrFld->GetAt(i);
			pFld->nIndex = i + nIndexSep;
		}
		m_nColSize = pArrFld->GetSize();
	}
	return TRUE;
}

void CCrossTab::Release(CPtrArray *pArrFld)
{
	int nSize = pArrFld->GetSize();
	if (nSize < 0) return;
	CFldInfo *pFld = NULL;
	for (int i=0;i<nSize;i++)
	{
		CFldInfo *pFld = (CFldInfo *)pArrFld->GetAt(i);
		delete pFld;
	}
	pArrFld->RemoveAll();
}
