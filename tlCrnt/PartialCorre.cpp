// PartialCorre.cpp: implementation of the CPartialCorre class.
//				偏相关分析
//					马良庄    2006/03/21
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PartialCorre.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPartialCorre::CPartialCorre()
{
	m_adCorr = NULL;
	m_anNofCorr = NULL;
	m_adWeightofCorr = NULL;
	m_adPartialCorr = NULL;
	m_adPartialSig = NULL;
	m_anDF = NULL;
	m_bOneTail = false;
	m_bWeightMissing = false;
	m_dCases = 0.0;
	m_pTempCorr = NULL;
	m_nWeight = -1;
}

CPartialCorre::~CPartialCorre()
{
	Clear();

	if( m_tVarStruct.iCount>0 )
		m_tVarStruct.DeleteObject();//存放已选中的变量
	if( m_tContStruct.iCount>0 )
		m_tContStruct.DeleteObject();//存放选中的控制变量
}

void CPartialCorre::Clear()
{
	if( m_adCorr )
	{
		delete[] m_adCorr;
		m_adCorr = NULL;
	}
	if( m_adWeightofCorr )
	{
		delete[] m_adWeightofCorr;
		m_adWeightofCorr = NULL;
	}
	if( m_adPartialCorr )
	{
		delete[] m_adPartialCorr;
		m_adPartialCorr = NULL;
	}
	if( m_adPartialSig )
	{
		delete[] m_adPartialSig;
		m_adPartialSig = NULL;
	}
	if( m_anNofCorr )
	{
		delete[] m_anNofCorr;
		m_anNofCorr = NULL;
	}
	if( m_anDF )
	{
		delete[] m_anDF;
		m_anDF = NULL;
	}
	if( m_pTempCorr )
	{
		delete[] m_pTempCorr;
		m_pTempCorr = NULL;
	}
}

BOOL CPartialCorre::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//PARTIALCORR
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/BYVAR [varlist]
//	/WEIGHT [varname]
//	/SIGN [{ONETAIL},{TWOTAIL**}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	if (AnaSyntax::exist("/WEIGHT", VOstr) )
	{
		CTStringArray xname;
		TIntStruct tVarWeight;
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarWeight)))
			return FALSE;
		if (tVarWeight.iCount == 1)
			m_nWeight = tVarWeight.pValue[0];
		else
			return FALSE;
	}

	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarStruct)))
		return FALSE;
	xname.RemoveAll();

	AnaSyntax::getVAR("/BYVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tContStruct)))
		return FALSE;

	m_bOneTail = AnaSyntax::findstr("/SIGN", "ONETAIL", VOstr);
	
	return TRUE;
}

CResult* CPartialCorre::OnPartialCorre(CDataInterface * pDataInterface, string VO)
{
	CResult* pResult;
	pResult=new CResult("偏相关分析");
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}	

	CPtrArray arVarData;
	CPtrArray arCtrlData; 

	GetData( pDataInterface,arVarData,arCtrlData, 5 );

	// 读取权重值
	double * pWeightCases = NULL;
	int nValidWeightCases;
	bool bContinue = true;
	m_bWeightMissing = false;
	if( m_nWeight > -1 )
	{
		pWeightCases = new double[pDataInterface->GetRowCount()];
		nValidWeightCases = pDataInterface->GetWeightData( pWeightCases );
		if( nValidWeightCases<1 )
		{
			delete pWeightCases;
			pWeightCases = NULL;
			bContinue = false;
		}
		if( nValidWeightCases < pDataInterface->GetRowCount() )
			m_bWeightMissing = true;
	}

	if( bContinue )
	{
		int nVarCol = m_tVarStruct.iCount;
		int nCtrlCol = m_tContStruct.iCount;

		m_adCorr = new double[(nVarCol+nCtrlCol)*(nVarCol+nCtrlCol)];
		m_anNofCorr = new int[(nVarCol+nCtrlCol)*(nVarCol+nCtrlCol)]; 
		m_adWeightofCorr = new double[(nVarCol+nCtrlCol)*(nVarCol+nCtrlCol)];
		m_adPartialCorr = new double[nVarCol*nVarCol];
		m_adPartialSig = new double[nVarCol*nVarCol];
		m_anDF = new int[nVarCol*nVarCol];
		m_dCases = 0.0;
		if( nCtrlCol>1 )
		{
			int nTempCorrSize = (nCtrlCol-1)*((nVarCol+nCtrlCol)*(nVarCol+nCtrlCol-1))/2;
			m_pTempCorr = new TTempCorr[nTempCorrSize];
		}

 		Calculate( arVarData, arCtrlData, pWeightCases, pDataInterface->GetRowCount() );        

		if( m_adCorr )
		{
			delete[] m_adCorr;
			m_adCorr = NULL;
		}
		if( m_anNofCorr )
		{
			delete[] m_anNofCorr;
			m_anNofCorr = NULL;
		}
		if( m_adWeightofCorr )
		{
			delete[] m_adWeightofCorr;
			m_adWeightofCorr = NULL;
		}
		if( m_pTempCorr )
		{
			delete[] m_pTempCorr;
			m_pTempCorr = NULL;
		}
		if( pWeightCases )
		{
			delete[] pWeightCases;
			pWeightCases = NULL;
		}
	}

	CreateResult(pDataInterface,pResult);
	int i=0;
	for(i=0; i<arVarData.GetSize(); i++ )
	{
		TDataSet * pData = (TDataSet*)arVarData.GetAt(i);
		delete[] pData;
	}
	arVarData.RemoveAll();

	for( i=0; i<arCtrlData.GetSize(); i++ )
	{
		TDataSet * pData = (TDataSet*)arCtrlData.GetAt(i);
		delete[] pData;
	}
	arCtrlData.RemoveAll();

	Clear();
	
	return pResult;
}


void CPartialCorre::CreateResult(CDataInterface *pDataInterface, CResult *&pResult)
{
	int i,j;
	int y = 10;

	if( m_adPartialCorr==NULL ) // 无合法数据输入提示
	{
		CTString	strWarning;
		strWarning.Format( " 注：没有合法的数据输入，无法执行相关分析！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return;
	}

	if( m_dCases<3.0 ) 
	{
		CTString	strWarning;
		strWarning.Format( " 注：只有%0.4f个输入样本，无法执行相关分析！", m_dCases );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return;
	}

	if( m_bWeightMissing )
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：有一个以上的样本数据因权重为零、负数或缺失而删除！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
	}

	//创建相关系数矩阵表
	CTLTable * pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle( "偏相关分析" );

	CTString str;
	str = "控制变量：";
	for(i=0;i<m_tContStruct.iCount-1;i++)
         str += pDataInterface->GetFieldName(m_tContStruct.pValue[i]) + " ，";
    str += pDataInterface->GetFieldName(m_tContStruct.pValue[i]);

	pTable->SetHead(str);
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"");

	for( i=0 ; i<m_tVarStruct.iCount; i++ )
		pTable->InsertColumn( i+2, pDataInterface->GetFieldName(m_tVarStruct.pValue[i]) );
	
	for(i=0;i<m_tVarStruct.iCount;i++)
	{
		pTable->InsertItem( i*3, pDataInterface->GetFieldName(m_tVarStruct.pValue[i]), false );
		pTable->InsertItem( i*3+1, "", false );
		pTable->InsertItem( i*3+2, "" );
	}

	bool bNA = false;
	for(i=0;i<m_tVarStruct.iCount;i++)
	{
	
		CTString strNamei;	
		strNamei=pDataInterface->GetFieldName(m_tVarStruct.pValue[i]);

		pTable->SetItemText( i*3, 1, "相关系数" );
		pTable->SetItemText( i*3+1, 1, "P    值" );
		pTable->SetItemText( i*3+2, 1, "自由度" );

		for(j=0;j<m_tVarStruct.iCount;j++)
		{
			if( fabs(m_adPartialSig[i*m_tVarStruct.iCount+j])>=1.0 )
			{
				pTable->SetItemText( i*3, j+2, "NA");
				pTable->SetItemText( i*3+1, j+2, "NA");
				bNA = true;
			}
			else
			{
				pTable->SetItemText(i*3, j+2, m_adPartialCorr[i*m_tVarStruct.iCount+j]);
				if( i!=j )
				{
					if( 1-fabs(m_adPartialCorr[i*m_tVarStruct.iCount+j])>0 )
					{
						if( m_bOneTail )
							pTable->SetItemText( i*3+1, j+2, m_adPartialSig[i*m_tVarStruct.iCount+j]);
						else
							pTable->SetItemText( i*3+1, j+2, m_adPartialSig[i*m_tVarStruct.iCount+j] * 2);
					}
					else
					{
						pTable->SetItemText( i*3+1, j+2, "NA");
						bNA = true;
					}
				}
			}
			pTable->SetItemText( i*3+2, j+2, m_anDF[i*m_tVarStruct.iCount+j]);
		}
	}

	CTString strTail;
	if( m_bOneTail )
		strTail.Format( " * 显著性为单尾" );
	else
		strTail.Format( " * 显著性为双尾" );

	strTail += "\n * 整行排除缺失值";

	if( bNA )
		strTail += "\n NA: 因至少一个变量无效无法计算";

	pTable->SetTail( strTail );

	CRsltElementTable * pVariable=new CRsltElementTable( "偏相关系数矩阵", pTable);
	pResult->Add(pVariable);			
}


void CPartialCorre::GetData(CDataInterface *pDataInterface, CPtrArray &arVarData, CPtrArray &arCtrlData, int nGetType)
{
	TDataSet * pDataArray = NULL;

	// 从数据库读取用户选定的所有字段数据

	// 读取变量字段
	if( m_tVarStruct.iCount>0 )
	{
		for( int i=0; i<m_tVarStruct.iCount; i++ )
		{			
			int nRowCount = pDataInterface->GetColData( m_tVarStruct.pValue[i], pDataArray, nGetType );//取回数据

			if( pDataArray )
			{
				arVarData.Add( pDataArray ); // 将各字段数据放入字段数组
				pDataArray = NULL;
			}
		}
	}

	// 读取控制字段
	if( m_tContStruct.iCount>0 )
	{
		for( int i=0; i<m_tContStruct.iCount; i++ )
		{			
			int nRowCount = pDataInterface->GetColData( m_tContStruct.pValue[i], pDataArray, nGetType );//取回数据

			if( pDataArray )
			{
				arCtrlData.Add( pDataArray ); // 将各字段数据放入字段数组
				pDataArray = NULL;
			}
		}
	}

}

void CPartialCorre::Calculate( CPtrArray &arVarData, CPtrArray &arCtrlData, double aWeight[], int n )
{
	int i, j, k;
	int nVarCol = arVarData.GetSize();
	int nCtrlCol = arCtrlData.GetSize();

	CPtrArray arData;
	arData.Copy( arVarData );
	arData.Append( arCtrlData ); // 将用户选择的所有字段组成一组
	CalcZeroCorr( arData, aWeight, n ); // 计算所有字段的两两相关系数及样本数
	arData.RemoveAll();

	if( m_dCases<3.0 )
		return;

	CTArray<int, int> arCol;
	for( i=0; i<nVarCol-1; i++ )
	{
		for( j=i+1; j<nVarCol; j++ )
		{
			// 计算i,j字段的偏相关系数
			arCol.Add( i );
			arCol.Add( j );
			for( k=0; k<nCtrlCol; k++ )
				arCol.Add( nVarCol+k ); // 加入控制变量

			// 递归调用计算偏相关系数
			bool bError = false;
			m_adPartialCorr[i*nVarCol+j] = CalcPartialCorr( arCol, nVarCol, nCtrlCol, bError );
			
			// 求自由度
			double dDF = MinSumWeight( arCol, nVarCol+nCtrlCol ) - nCtrlCol - 2;
			m_anDF[i*nVarCol+j] = dDF>0 ? int(dDF+0.5) : 0;

			// 求显著性
			if( bError || dDF<=0 ) 
			{
				m_adPartialSig[i*nVarCol+j] = -1;
			}
			else if( m_adPartialCorr[i*nVarCol+j]<=-1.0 )
			{
				m_adPartialCorr[i*nVarCol+j] = -1.0;
				m_adPartialSig[i*nVarCol+j] = 0;
			}
			else if( m_adPartialCorr[i*nVarCol+j]>=1 )
			{
				m_adPartialCorr[i*nVarCol+j] = 1.0;
				m_adPartialSig[i*nVarCol+j] = 0;
			}
			else
			{
				double dT = m_adPartialCorr[i*nVarCol+j] * 
								sqrt( dDF / (1-m_adPartialCorr[i*nVarCol+j]*m_adPartialCorr[i*nVarCol+j]));

				m_adPartialSig[i*nVarCol+j] = 1 - tcdf( fabs(dT), dDF );
			}

			arCol.RemoveAll();
		}
	}

	// 复制对称矩阵的上半部到下半部
	for( j=0; j<nVarCol; j++ )
	{
		for( i=j; i<nVarCol; i++ )
		{
			if( i==j )
			{
				m_adPartialCorr[i*nVarCol+j] = 1.0;
				m_adPartialSig[i*nVarCol+j] =0.0;
				m_anDF[i*nVarCol+j] = 0;
			}
			else 
			{
				m_adPartialCorr[i*nVarCol+j] = m_adPartialCorr[i+nVarCol*j];
				m_adPartialSig[i*nVarCol+j] = m_adPartialSig[i+nVarCol*j];
				m_anDF[i*nVarCol+j] = m_anDF[i+nVarCol*j];
			}
		}
	}
}

double CPartialCorre::CalcPartialCorr( CTArray<int, int>& arCol, int nVar, int nCtrl, bool& bError )
{
	int nTempPos, m, k = nVar + nCtrl;
	double dRslt;

	if( arCol.GetSize()<2 )
		assert(false);    // 递归调用必须大于等于2

	if( arCol.GetSize()==2 )
	{
		return m_adCorr[arCol[0]*k+arCol[1]];
	}
	else
	{
		// 计算偏相关系数中间值位置
		m = ((k-1)*k)/2;
		if( arCol[0]>arCol[1] )
			nTempPos = (arCol.GetSize()-3)*m+(arCol[1]*(2*k-arCol[1]-3))/2+(arCol[0]-1);
		else if( arCol[0]<arCol[1] )
			nTempPos = (arCol.GetSize()-3)*m+(arCol[0]*(2*k-arCol[0]-3))/2+(arCol[1]-1);
		else 
			assert( false );

		if( arCol.GetSize()-2<nCtrl && m_pTempCorr && !m_pTempCorr[nTempPos].bNeedCalc)
		{
			// 若中间值已经算出，返回偏相关系数中间值
			if( m_pTempCorr[nTempPos].bError )
			{
				bError = true;
				return -1;
			}
			else
				return m_pTempCorr[nTempPos].dCorr;
		}
		else
		{
			CTArray<int, int> arCol_ij, arCol_i, arCol_j;

			// 降阶计算i,j的偏相关系数
			arCol_ij.Copy( arCol );
			arCol_ij.RemoveAt( arCol_ij.GetSize()-1 );
			double dR_ij = CalcPartialCorr( arCol_ij, nVar, nCtrl, bError ); 
			if( bError )
			{
				if( arCol.GetSize()-2<nCtrl && m_pTempCorr )
				{
					m_pTempCorr[nTempPos].bNeedCalc = false;
					m_pTempCorr[nTempPos].bError = true;
				}
				return -1;
			}

			// 计算i与所有控制变量的相关系数
			arCol_i.Copy( arCol_ij );
			arCol_i.RemoveAt( 1 );
			arCol_i.InsertAt( 1, arCol.GetAt( arCol.GetSize()-1 ) );
			double dR_i = CalcPartialCorr( arCol_i, nVar, nCtrl, bError );
			if( bError || fabs(1-fabs(dR_i))<1e-20 )
			{
				if( arCol.GetSize()-2<nCtrl && m_pTempCorr )
				{
					// 设置中间值为计算失效
					m_pTempCorr[nTempPos].bNeedCalc = false;
					m_pTempCorr[nTempPos].bError = true;
				}
				bError = true;
				return -1;
			}
			arCol_i.RemoveAll();

			// 计算j与所有控制变量的相关系数
			arCol_j.Copy( arCol_ij );
			arCol_j.RemoveAt( 0 );
			arCol_j.InsertAt( 1, arCol.GetAt( arCol.GetSize()-1 ) );
			double dR_j = CalcPartialCorr( arCol_j, nVar, nCtrl, bError );
			if( bError || fabs(1-fabs(dR_j))<1e-20 || (1-dR_i*dR_i)*(1-dR_j*dR_j)<0 )
			{
				if( arCol.GetSize()-2<nCtrl && m_pTempCorr )
				{
					// 设置中间值为计算失效
					m_pTempCorr[nTempPos].bNeedCalc = false;
					m_pTempCorr[nTempPos].bError = true;
				}
				bError = true;
				return -1;
			}
			arCol_j.RemoveAll();
			arCol_ij.RemoveAll();

			dRslt = ( dR_ij - dR_i*dR_j ) / ( sqrt( (1-dR_i*dR_i)*(1-dR_j*dR_j) ));
			if( arCol.GetSize()-2<nCtrl && m_pTempCorr )
			{
				// 为中间值赋值
				m_pTempCorr[nTempPos].bNeedCalc = false;
				m_pTempCorr[nTempPos].bError = false;
				m_pTempCorr[nTempPos].dCorr = dRslt;
			}
		}

	    return dRslt;
	}
}

double CPartialCorre::MinSumWeight( CTArray<int,int>& arCol, int k )
{
	double dMinWeight = m_adWeightofCorr[arCol[0]*k+arCol[1]];

	// 求指定变量的最小相关样本数
	for( int i=0; i<arCol.GetSize()-1; i++ )
		for( int j=i+1; j<arCol.GetSize(); j++ )
			if( m_adWeightofCorr[arCol[i]*k+arCol[j]]<dMinWeight )
				dMinWeight = m_adWeightofCorr[arCol[i]*k+arCol[j]];

	return dMinWeight;
}

void CPartialCorre::CalcZeroCorr( CPtrArray& arData, double aWeight[], int n )
{
	int i, j, iRow, k = arData.GetSize();
	double * adCrossDevs = new double[k*k];
	double * adCrossDevSelf = new double[k*k]; // 两两相关的自身差积

	// 对i和j字段计算两两相关性
	for( i=0; i<k; i++ )
	{
		TDataSet* pFieldData_i = (TDataSet*)arData.GetAt( i );

		for( j=i; j<k; j++ )
		{
			double dWeight = 1, dWeightSum = 0;
			double dTemp_i, dTemp_j, dTemp_ij, dTemp_ii, dTemp_jj;

			TDataSet* pFieldData_j = (TDataSet*)arData.GetAt( j );

			// 求相关变量差积和相关样本数
			dTemp_i = dTemp_j = dTemp_ij = dTemp_ii = dTemp_jj = 0;
			for( iRow=0; iRow<n; iRow++ )
			{
				bool bMissing = false;
				for( int ind=0; ind<k && !bMissing; ind++ )
				{
					TDataSet * pTestData = (TDataSet*)arData.GetAt( ind );
					if( pTestData[iRow].iIndex<0 )
						bMissing = true;
				}

				if( bMissing )
					continue;

				if( aWeight )
				{
					if( aWeight[iRow]==0 )
						continue;
					else
						dWeight = aWeight[iRow];
				}
				dTemp_i += pFieldData_i[iRow].dValue * dWeight;
				dTemp_j += pFieldData_j[iRow].dValue * dWeight;
				dTemp_ij += pFieldData_i[iRow].dValue * pFieldData_j[iRow].dValue * dWeight;
				dTemp_ii += pFieldData_i[iRow].dValue * pFieldData_i[iRow].dValue * dWeight;
				dTemp_jj += pFieldData_j[iRow].dValue * pFieldData_j[iRow].dValue * dWeight;
				dWeightSum += dWeight;
			}

			m_dCases = dWeightSum;
			if( dWeightSum<3.0 )
			{
				delete[] adCrossDevs;
				delete[] adCrossDevSelf;
				return;
			}

			adCrossDevs[i*k+j] = dTemp_ij - (dTemp_i * dTemp_j / dWeightSum);
			adCrossDevSelf[i*k+j] = dTemp_ii - (dTemp_i * dTemp_i / dWeightSum );
			adCrossDevSelf[i+k*j] = dTemp_jj - (dTemp_j * dTemp_j / dWeightSum );
			m_adWeightofCorr[i*k+j] = dWeightSum;
			m_anNofCorr[i*k+j] = int(dWeightSum+0.5);
		}
	}

	// 计算i和j变量的相关系数
	for( i=0; i<k; i++ )
	{
		for( j=0; j<k; j++ )
		{
			if( i==j )
				m_adCorr[i*k+j] = 1;
			else if( i<j )
			{
				if( adCrossDevs[i*k+i]<1e-16 || adCrossDevs[j*k+j]<1e-16 )
				{
					m_adCorr[i*k+j] = -1;
				}
				else
				{
					m_adCorr[i*k+j] = adCrossDevs[i*k+j] / 
									  sqrt( adCrossDevSelf[i*k+j] * adCrossDevSelf[i+k*j] );
				}
			}
			else // 对称矩阵，复制上半部分到下半部分
			{
				m_adCorr[i*k+j] = m_adCorr[j*k+i];
				m_anNofCorr[i*k+j] = m_anNofCorr[j*k+i];
			}
		}
	}
/*
	CTString strLine, strData;
	for( i=0; i<k; i++ )
	{
		for( j=0; j<k; j++ )
		{
			strData.Format( "  %0.4f", m_adCorr[i*k+j] );
			strLine += strData;
		}
		strLine += "\n";
		TRACE( strLine );
		strLine = "";
	}
*/
	delete[] adCrossDevs;
	delete[] adCrossDevSelf;
}