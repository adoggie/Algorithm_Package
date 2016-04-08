//////////////////////////////////////////////////////////////////////
// CrossTab.cpp                                                     //
//																	//
// Define:        交叉表crosstabulation算法类实现                   //
// Creator:       夏东山                                              //
// Date:          2006.03.20                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Define.h"
#include "crosstab.h"

const int OUTPUT_SUMMARY = 1;  //记录处理统计
const int OUTPUT_CHISQUR = 2;  //卡方
const int OUTPUT_CORRLAT = 4;  //相关系数
const int DISPCOL_MAX    = 200;//交叉表显示的最大列数
const int DISPROWCOL_MAX = 10000;//交叉表显示的最大行列数
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
	//解析－权重
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
	//解析－变量
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

// 外部调用主函数
CResult * CCrossTab::OnCrossTab(CDataInterface *pDataInterface, string VO)
{
	m_pResult = NULL;
	m_nType = m_nCrossTableCnt = 0;
	if (!pDataInterface) return m_pResult;
	//构造对象
	m_pResult = new CResult("交叉表");
	//赋接口指针数据
	m_pDataInterface = pDataInterface;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	CrossTab();
	return m_pResult;
}

//得到标签
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
	CIntVector    vecRowInd;  //索引
	CDoubleMatrix dataMatrix;
	CMapManager   dataMap;
	CDoubleMatrix weightMatrix;
	CDoubleVector vecCol, vecRow;
	CPtrArray arrColumn,arrRow;
	int nRow = 0, nCol = 0, nRowCnt = 0;
	
	if (m_tRowInfo.iCount < 1 || m_tColInfo.iCount < 1)
		return;
	m_nCurRows = 0;
	m_pSummaryTable     = NULL;//汇总表
	m_pCorrelationTable = NULL;//相关系数表
	m_pChiSquareTable   = NULL;//卡方表
	//总行数
	int nAllRowCnt = m_pDataInterface->GetRowCount();
	//读取权重
	if (m_bWeight)
	{
		arrCol.RemoveAll();
		arrCol.Add(m_nWeight);
		nRowCnt = m_pDataInterface->GetColsData(arrCol,weightMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning.Format("注： 您所选择的权重数据不适合做当前分析，请重新选择！");
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
		}
		//总的权重
		m_n64Weight = __int64(weightMatrix(0).sum());
		weightMatrix.destroy();
	}
	else
	{
		m_n64Weight = nAllRowCnt;
	}
	//获取数据
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
			//暂用缺省值删除
			nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,dataMap);
			if (nRowCnt < 1)
			{
				CTString szWarning("");
				if (m_bWeight)
					szWarning.Format("注： 行变量:%s-列变量:%s-权重:%s-没有合适的记录进行描述统计分析。",
					m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_tColInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_nWeight).GetData());
				else
					szWarning.Format("注： 行变量%s-列变量%s-没有合适的记录进行描述统计分析。",
					m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRow]).GetData(),
					m_pDataInterface->GetFieldName(m_tColInfo.pValue[nRow]).GetData());
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
				pWarningTextRslt->AddString(szWarning);
				m_pResult->Add(pWarningTextRslt);
				continue;
			}
			//数据处理-记录统计
			CreateSummaryTable(dataMatrix,nRow,nCol);
			//计算列变量信息
			CDoubleVector a(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector b(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector c(dataMatrix(0));
			BOOL bRet = CalcFldInf(a,b,c,&arrColumn,CALC_COLFLD);
			//计算行变量信息
			CDoubleVector a2(dataMatrix(m_bWeight ? 1 : 0));
			CDoubleVector b2(dataMatrix(m_bWeight ? 2 : 1));
			CDoubleVector c2(dataMatrix(0));
			if (bRet) bRet = CalcFldInf(a2,b2,c2,&arrRow,CALC_ROWFLD);
			/////////////////////////////////////////////////////////////////////
			//数据处理-交叉表
			if (bRet)
			{
				CreateCrossTable(dataMatrix,dataMap,&arrRow,&arrColumn,nRow,nCol);
				//相关系数
				CreateStatisticTable(dataMatrix,&arrRow, &arrColumn,nRow,nCol);
				m_nCurRows ++;
			}
			//释放内存
			Release(&arrRow);
			Release(&arrColumn);
		}
	}
}

// 创建记录统计表
void CCrossTab::CreateSummaryTable(CDoubleMatrix &dataMatrix, int nRowIndex, int nColIndex)
{
	int     nRowCnt = 0;
	double  fWeight = 0.0;
	CTString szText("");
	//表结构
	if (m_pSummaryTable == NULL)
	{
		m_pSummaryTable = new CTLTable;
		m_pSummaryTable->SetTitle("记录处理统计");
		m_pSummaryTable->SetCols(4);

		m_pSummaryTable->InsertColumn(0, "");
		if (m_bWeight)
		{
			m_pSummaryTable->InsertColumn(1, "有效权重和");
			m_pSummaryTable->InsertColumn(2, "缺失权重和");
			m_pSummaryTable->InsertColumn(3, "总权重和");
		}
		else
		{
			m_pSummaryTable->InsertColumn(1, "有效记录数");
			m_pSummaryTable->InsertColumn(2, "缺失记录数");
			m_pSummaryTable->InsertColumn(3, "总记录数");
		}
	}
	//表数据
	szText.Format("%s ＊ %s",m_pDataInterface->GetFieldName(m_tRowInfo.pValue[nRowIndex]).GetData(),m_pDataInterface->GetFieldName(m_tColInfo.pValue[nColIndex]).GetData());
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
		CRsltElementTable * pSummaryTableRslt = new CRsltElementTable( "记录处理统计", m_pSummaryTable );
		m_pResult->Add(pSummaryTableRslt);
		m_nType |= OUTPUT_SUMMARY;
	}
}

// 创建统计量表
void CCrossTab::CreateStatisticTable(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol)
{
	CTString szText("");
	BOOL bCorrHaveString = FALSE, bChiHaveString = FALSE;
	CTString szRowName = m_pDataInterface->GetFieldName( m_tRowInfo.pValue[nRow] );
	CTString szColName = m_pDataInterface->GetFieldName( m_tColInfo.pValue[nCol] );
	szText = szRowName + " ＊ " + szColName;
	
	if( m_bOutputCorrelation ) // 输出相关系数表
	{
		if (m_pCorrelationTable == NULL)
		{
			m_pCorrelationTable = new CTLTable;
			m_pCorrelationTable->SetTitle( "相关系数表" );
			m_pCorrelationTable->SetRows( m_tRowInfo.iCount * m_tColInfo.iCount + 1 );
			m_pCorrelationTable->SetCols( 3 );
			m_pCorrelationTable->InsertColumn( 0, "");
			m_pCorrelationTable->InsertColumn( 1, "皮尔逊\n相关系数");
			m_pCorrelationTable->InsertColumn( 2, "相关显著性\n(双尾)");
		}
		//计算相关系数
		bCorrHaveString = CalcCorrlation(dataMatrix,nRow,nCol);
		//相关系数
		szText = szRowName + " ＊ " + szColName;
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
	if( m_bOutputChiSquare ) //输出卡方及自由度
	{
		if (m_pChiSquareTable == NULL)
		{
			m_pChiSquareTable = new CTLTable;
			m_pChiSquareTable->SetTitle( "卡方表" );
			m_pChiSquareTable->SetRows( m_tRowInfo.iCount * m_tColInfo.iCount + 1 );
			m_pChiSquareTable->SetCols( 6 );
			m_pChiSquareTable->InsertColumn( 0, "");
			m_pChiSquareTable->InsertColumn( 1, "皮尔逊\n卡方");
			m_pChiSquareTable->InsertColumn( 2, "皮尔逊卡方\n显著性\n(双尾)");
			m_pChiSquareTable->InsertColumn( 3, "似然比\n卡方" );
			m_pChiSquareTable->InsertColumn( 4, "似然比卡方\n显著性\n(双尾)");
			m_pChiSquareTable->InsertColumn( 5, "卡方\n自由度" );
		}
		//计算卡方及自由度
		bChiHaveString = CalcChiSquare(dataMatrix,pArrRow,pArrCol);
		//卡方及自由度
		szText = szRowName + " ＊ " + szColName;
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
	// 如过有未输出的数值，输出NA警告
	if (m_bOutputCorrelation && (m_nType & OUTPUT_CHISQUR) == 0)
	{
		CRsltElementTable *pCorrTableRslt = new CRsltElementTable( "相关系数表", m_pCorrelationTable );
		m_pResult->Add(pCorrTableRslt);
		m_nType |= OUTPUT_CHISQUR;
	}
	if (m_bOutputChiSquare && (m_nType & OUTPUT_CORRLAT) == 0)
	{
		CRsltElementTable *pChiTableRslt = new CRsltElementTable( "卡方表", m_pChiSquareTable );
		m_pResult->Add(pChiTableRslt);
		m_nType |= OUTPUT_CORRLAT;
	}
	if (m_pCorrelationTable && !bCorrHaveString)
		m_pCorrelationTable->SetTail( "\n NA：因数据类型不适合或无有效记录数或某字段为常数，无法进行统计计算。" );
	if (m_pChiSquareTable && !bChiHaveString)
		m_pChiSquareTable->SetTail( "\n NA：因数据类型不适合或无有效记录数或某字段为常数，无法进行统计计算。" );
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
	//添加交叉表结构信息
	CTString szRowName = m_pDataInterface->GetFieldName( m_tRowInfo.pValue[nRow] );
	CTString szColName = m_pDataInterface->GetFieldName( m_tColInfo.pValue[nCol] );
	CTString szTableName = szRowName + " ＊ " + szColName + " 交叉表";
	CTLTable * pCrossTable = new CTLTable;
	pCrossTable->SetTitle( szTableName ); // 设置表头
	if (nColCount > DISPCOL_MAX)
	{
		szText.Format( "注：所选的变量不适合做当前分析，列数超过%d，无法显示该表！", DISPCOL_MAX);
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
		szText.Format( "注：所选的变量不适合做当前分析，行数×列数超过%d，无法完全显示 %s！", DISPROWCOL_MAX,szTableName.GetData());
		pCrossTable->SetRows( 1 );
		pCrossTable->SetCols( 0 );
		pCrossTable->InsertColumn(0, szText);
		CRsltElementTable *pRsltElement = new CRsltElementTable(szTableName, pCrossTable);
		if (!m_pResult->InsetAt(m_nCrossTableCnt,pRsltElement))
			m_pResult->Add(pRsltElement);
		return;
	}
	//列总数
	pCrossTable->SetCols(nColCount + (m_bOutputExpCount ? 3 : 2));
	pCrossTable->InsertColumn( 0, "" );
	nColStart = 1;
	if( m_bOutputExpCount )
	{
		pCrossTable->InsertColumn(1, "");
		nColStart = 2;
	}
	//字段类型
	fColType = m_pDataInterface->GetFieldType(m_tColInfo.pValue[nCol]);
	for (int i=0;i<nColCount;i++)
	{
		CFldInfo *pFldInf = (CFldInfo *)pArrCol->GetAt(i);
		szText = GetLabel(dataMap,nColIndex,m_tColInfo.pValue[nCol],pFldInf->fValue);
		pCrossTable->InsertColumn(nColStart,szText);
		nColStart ++;
	}
	pCrossTable->InsertColumn(nColStart, "总计");
	//插入记录－交叉表
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
	//总的有效权重或记录数
	double f64Weight = m_bWeight ? dataMatrix(0).sum() : dataMatrix(0).vlen();
	nRowStart = 0;
	for (int i=0;i<nRowSize;i++)
	{
		pRowInf = (CFldInfo *)pArrRow->GetAt(i);
		//插入行
		InsertCorssTabOfRow(dataMap,pCrossTable,pRowInf,pArrCol,nRowStart,nRow,f64Weight);
		nRowStart ++;
	}
	//总的
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

	//初始化
	nColStart = m_bOutputExpCount ? 2 : 1;
	//行序
	nRowIndex = m_bWeight ? 1 : 0;
	//列序
	nColIndex = m_bWeight ? 2 : 1;
	fRowType = m_pDataInterface->GetFieldType(m_tRowInfo.pValue[nRow]);
	if (m_bOutputExpCount) //输出百分数
	{
		//是否为最后
		if (bLast)
		{
			pCrossTable->InsertItem(nRowStart*3, "总计", FALSE);
			pCrossTable->SetItemText(nRowStart*3, 1, "计数");
			pCrossTable->InsertItem(nRowStart*3+1, "");
			pCrossTable->SetItemText(nRowStart*3+1, 1, "百分比%");
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
			pCrossTable->SetItemText(nRowStart*3, 1, "计数");
			pCrossTable->InsertItem(nRowStart*3+1, "", FALSE);
			pCrossTable->SetItemText(nRowStart*3+1, 1, "行百分比%");
			pCrossTable->InsertItem(nRowStart*3+2, "" );
			pCrossTable->SetItemText(nRowStart*3+2, 1, "列百分比%");
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
			pCrossTable->SetItemText(nRowStart*3, 1, "计数");
			pCrossTable->InsertItem(nRowStart*3+1, "", FALSE);
			pCrossTable->SetItemText(nRowStart*3+1, 1, "行比例%");
			pCrossTable->InsertItem(nRowStart*3+2, "");
			pCrossTable->SetItemText(nRowStart*3+2, 1, "列比例%");
		}
		//对具体的行向量进行计算
		CalcFldInf(pRowInf->v,pRowInf->v,pRowInf->w,&arrTmp,CALC_TMPFLD);
		nVlen = arrTmp.GetSize();
		for (int i=0;i<nVlen;i++)
		{
			pTmpInf = (CFldInfo *)arrTmp.GetAt(i);
			//输出
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
		//某行总的
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
		//是否为最后
		if (bLast)
		{
			pCrossTable->InsertItem(nRowStart, "总计", TRUE);
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
		//对具体的行向量进行计算
		CalcFldInf(pRowInf->v,pRowInf->v,pRowInf->w,&arrTmp,CALC_TMPFLD);
		nVlen = arrTmp.GetSize();
		for (int i=0;i<nVlen;i++)
		{
			pTmpInf = (CFldInfo *)arrTmp.GetAt(i);
			//输出
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
		//某行总的
		szText.Format("%.0f",pRowInf->fWeight);
		pCrossTable->SetItemText(nRowStart, pArrCol->GetSize() + 1, szText);
		//////////////////////////////////////////////////
	}
	//释放内存
	Release(&arrTmp);
}

BOOL CCrossTab::CalcCorrlation(CDoubleMatrix &dataMatrix, int nRow, int nCol)
{
	int nRowIndex = 0, nColIndex = 0;
	double fWeight = 1, fWeightSum = 0;
	double fTemp_i = 0, fTemp_j = 0, fTemp_ij = 0, fTemp_ii = 0, fTemp_jj = 0;
	
	//行序
	nRowIndex = m_bWeight ? 1 : 0;
	//列序
	nColIndex = m_bWeight ? 2 : 1;
	int nVlen = dataMatrix.mrow();
	//是否输出相关系数
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
				// 求差积
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
			// 求差积
			fTemp_i += dataMatrix(nRowIndex)(i);
			fTemp_j += dataMatrix(nColIndex)(i);
			fTemp_ij += dataMatrix(nRowIndex)(i) * dataMatrix(nColIndex)(i);
			fTemp_ii += dataMatrix(nRowIndex)(i) * dataMatrix(nRowIndex)(i);
			fTemp_jj += dataMatrix(nColIndex)(i) * dataMatrix(nColIndex)(i);
		}
	}
	//按差积求皮尔逊相关系数和显著性
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
	
	CDoubleVector wVec(nRowCnt*nColCnt,0);//列×行
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
	if (m_bOutputChiSquare) //输出卡方及自由度
	{
		// 求皮尔逊卡方和似然比卡方及自由度
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
	int nEnd = pArrFld->GetSize()-1;//Array已有序
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
	//插入
	pFld = new CFldInfo;
	pFld->fValue  = fValue;
	pFld->fWeight = fWeight;
	if (nType == CALC_ROWFLD)
	{
		//创建向量
		pFld->nIndex = 0;
		pFld->v.create(nSize);
		pFld->w.create(nSize);
		pFld->v(pFld->nIndex) = fColValue;
		pFld->w(pFld->nIndex) = fWeight;
	}
	//插入
	pArrFld->InsertAt(nMid,pFld);
}

BOOL CCrossTab::CalcFldInf(  CDoubleVector &v,CDoubleVector &vc, CDoubleVector &w, CPtrArray *pArrFld, int nType)
{
	int i=0,nVlen=0;
	int nIndexSep=0;
	double fValue=0, fColValue=0, fWeight=0;
	CFldInfo *pFld = NULL;
	
	nVlen = v.vlen();
	if (nType == CALC_COLFLD) //取列信息
	{
		nIndexSep = (m_bOutputExpCount ? 2 : 1);
	}
	//权重
	if (m_bWeight)
	{
		for (i=0;i<nVlen;i++)
		{
			fValue  = v(i);
			fWeight = w(i);
			if (nType == CALC_ROWFLD) fColValue = vc(i);//行-取对应的列值
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
			if (nType == CALC_ROWFLD) fColValue = vc(i);//行-取对应的列值
			CalcFldInf(fValue,fColValue,1,pArrFld,nVlen,nType);
			if (nType == CALC_COLFLD && pArrFld->GetSize() > DISPCOL_MAX) return FALSE;
			if (nType == CALC_ROWFLD && pArrFld->GetSize() * m_nColSize > DISPROWCOL_MAX) return FALSE;
		}
	}
	//缩
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
