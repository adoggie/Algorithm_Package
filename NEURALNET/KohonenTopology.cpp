//////////////////////////////////////////////////////////////////////
// KohonenTopology.cpp:                                             //
//																	//
// Define:        Kohonen算法拓扑结构类实现                         //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "resource.h"
//#include "TFieldRole.h"
#include "KohonenTopology.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const UINT WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
const UINT WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKohonenTopology::CKohonenTopology( int nType, int nSampleSize, int nDimension, int nClusterNum, 
								   long lRuntime, int nIterateNum, double dMaxRate, double dMinRate )
					: CTopology(nType, nSampleSize, nDimension, nClusterNum,
								   lRuntime, nIterateNum )
{
	m_nChampCluster = 0;
	m_dMaxRate = dMaxRate;
	m_dMinRate = dMinRate;
	m_dInterimRate = 1.0;
	m_pKLayer = NULL;

	m_pKLayer = new CKLayer( nDimension, nClusterNum );
}

CKohonenTopology::~CKohonenTopology()
{
	if( m_pKLayer!=NULL )
		delete m_pKLayer;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Training
//输入参数：CDataInterface * pData, 数据的接口
//			CPtrArray& arFieldRole, 输入字段信息
//			CResult * pResult,  显示的结果
//			bool bOutputInfo	是否输出结果
//函数功能：训练规则
//////////////////////////////////////////////////////////////////////////
bool CKohonenTopology::Training(CDataInterface * pData, CTStringArray &arFieldRole, CResult *& pResult, bool bOutputInfo)
{
	CDoubleMatrix adDataAll;
	CDoubleVector adDataRow;
	NormInfo * aNormInfo =NULL;  // 保存样本各维是否需要标准化及最大与最小值
	CIntVector abValid(pData->GetRowCount()); // 保存记录是否含有非法数据

	int dim, knodes, pattern, nCol, nTreatedData = 0;
	double realValue;
	bool bTimeout = false, bDataOverload = false;
//	unsigned int dwCurrentTime = GetTickCount();
	CTString string, strVar1, strVar2, strProgressText;

	bOutputInfo = true;
	for( pattern=0; pattern<pData->GetRowCount(); pattern++ )
		abValid[pattern] = true;

	aNormInfo = new NormInfo[m_nDimension];
	
	if( m_nDimension * m_nSampleSize < DATAOVERLOAD )
	{
		bDataOverload = false;
		if( NormalizeSampleData( pData, arFieldRole, bDataOverload, aNormInfo, abValid, adDataAll )==false )
		{
			delete[] aNormInfo;
			return false;
		}
	}
	else
	{
		bDataOverload = true;
		if( NormalizeSampleData( pData, arFieldRole, bDataOverload, aNormInfo, abValid, adDataAll )==false )
		{
			delete[] aNormInfo;
			return false;
		}

	}

	for( pattern=0; pattern<pData->GetRowCount(); pattern++ )
	{
		if( !abValid[pattern] )
			nTreatedData++;
	}

	if( nTreatedData==pData->GetRowCount() )
	{
// 		AfxMessageBox( IDS_ALLDATAINVALIDATE, MB_ICONSTOP );
		m_bAbort = true;
	}

	if( m_bAbort )
	{
		delete[] aNormInfo;
		return false;
	}

/*
	strProgressText = _T("正在训练Kohonen网络结构...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	CTLTable* pTable=new CTLTable;
	pTable->CreateTable(m_nClusterNum+1,m_nDimension+1);
	string.Format( "%s", "Kohonen网络权值表" );
	pTable->SetTitle(string);

	pTable->InsertColumn(0, "聚类");
	for( nCol=0; nCol<arFieldRole.GetSize(); nCol++)
	{
		CTString strRole = arFieldRole.GetAt(nCol);
		CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strRole);
		pTable->InsertColumn( nCol+1, pField->GetFieldName());
		if( strVar1.IsEmpty() )
		{
			strVar1 = pField->GetLabel();
			if( strVar1.IsEmpty() )
				strVar1 = pField->GetFieldName();
		}
		else if( strVar2.IsEmpty() )
		{
			strVar2 = pField->GetLabel();
			if( strVar2.IsEmpty() )
				strVar2 = pField->GetFieldName();
		}
	}

	unsigned int ep = 0;
	
	double nInterval = (double)m_unIterateNum/100; 

	int nStep = 0;
	while( ((m_unIterateNum==0) || (ep<m_unIterateNum)) && (m_dInterimRate!=0.0) && !bTimeout && !m_bAbort )
	{
		for( pattern=0; pattern<m_nSampleSize; pattern++ )
		{
			if( !abValid[pattern] )
				continue;

			for( knodes=0; knodes<m_nClusterNum; knodes++ )
			{
				if( bDataOverload )
				{
					GetRowData( pData, arFieldRole, pattern+1, adDataRow );
				
					///////////////////////////////////////////////////////
					// 大数据按行处理数据标准化
					//
					for( dim=0; dim<m_nDimension; dim++ )
					{
						if( aNormInfo[dim].bNorm )
						{
							adDataRow[dim] = ( adDataRow[dim]-aNormInfo[dim].dMin ) / 
										   ( aNormInfo[dim].dMax - aNormInfo[dim].dMin );
						}

						m_pKLayer->SetNodeInputValue( knodes, dim, 
										adDataRow[dim] );
					}

				}
				else
				{
					for( dim=0; dim<m_nDimension; dim++ )
					{
						m_pKLayer->SetNodeInputValue( knodes, dim, 
										adDataAll(pattern,dim) );
						//Kohonen_Design.node_in_cluster_layer[knodes].input_value[dim] = Kohonen_Train.number_of_samples[pattern].data_in_sample[dim];}
					}
				}
			}

			NodesCompeteForActivation();
			//Kohonen_Design.kluster_nodes_compete_for_activation();

			UpdateNetwork( ep );
			//Kohonen_Design.update_the_Kohonen_network(ep, k_epochs);
		}
		if( ep>=nStep*nInterval )
		{
/*
			strProgressText.Format("正在训练Kohonen网络结构... 完成 %d%%", int(ep/nInterval));
			m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
			if( nStep%2==0 )
				m_pProgressWnd->SendMessage( WM_PROGRESS_STEP, 0, 0 );
*/

			nStep++;
		}

		ep++;

//		if( (m_dwRuntime!=0) && ((GetTickCount()-dwCurrentTime) > m_dwRuntime) )
//			bTimeout = true;
	};

	if( !m_bAbort )
	{
/*
		strProgressText = _T("正在进行样本聚类...");
		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/
		/////////////////////////////////////////////
		// 输出权值表结果
		//

		for( knodes=0; knodes<m_nClusterNum; knodes++ )
		{
			string.Format( "聚类%02d", knodes+1 );
			pTable->InsertItem( knodes, string);
		
			for( dim=0; dim<m_nDimension; dim++ )
				pTable->SetItemText( knodes, dim+1, m_pKLayer->GetNodeWeight( knodes, dim ));
		}

		CRsltElementTable *pVariable=new CRsltElementTable( "Kohonen神经网络权值表", pTable );
		pResult->Add(pVariable);

		///////////////////////////////////////////////////
		// 得出分类结果
		//
		CTLTable* pTable2 = NULL;
		TDataPointValVal** ppScatterDPS = NULL, ** ppClusterDPS = NULL;
		CTChartScatterPlot * pScatterplot = NULL, * pClusterplot = NULL;

		if( bOutputInfo )
		{
			pTable2=new CTLTable;
			string.Format( "%s", "样本分类结果" );
			pTable2->SetTitle(string);

			if( bDataOverload )
			{
				pTable2->CreateTable(m_nSampleSize+1,2);
				pTable2->InsertColumn(0, "样本");
				pTable2->InsertColumn(1, "聚类");
			}
			else
			{
				pTable2->CreateTable(m_nSampleSize+1,m_nDimension+1);
				pTable2->InsertColumn(0, "样本");
				for( nCol=0; nCol<arFieldRole.GetSize(); nCol++)
				{
					CTString strRole = arFieldRole.GetAt(nCol);
					CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strRole);
					pTable2->InsertColumn( nCol+1, pField->GetFieldName());
				}
				pTable2->InsertColumn(nCol+1, "聚类");
			}
		}

		int *pCount = new int[m_nClusterNum];
		memset(pCount,0,m_nClusterNum*sizeof(int));
		
		for( pattern=0; pattern<m_nSampleSize; pattern++ )
		{
			if( !abValid[pattern] )
				continue;
			for( knodes=0; knodes<m_nClusterNum; knodes++ )
			{
				if( bDataOverload )
				{
					GetRowData( pData, arFieldRole, pattern+1, adDataRow );
					for( dim=0; dim<m_nDimension; dim++ )
					{
						if( aNormInfo[dim].bNorm )
						{
							adDataRow[dim] = ( adDataRow[dim]-aNormInfo[dim].dMin ) / 
								( aNormInfo[dim].dMax - aNormInfo[dim].dMin );
						}
						m_pKLayer->SetNodeInputValue( knodes, dim, adDataRow[dim]);
					}
				}
				else
				{
					for( dim=0; dim<m_nDimension; dim++ )
					{
						m_pKLayer->SetNodeInputValue( knodes, dim, adDataAll(pattern,dim));
					}
				}
			}
			NodesCompeteForActivation();
			pCount[m_nChampCluster] += 1;
		}
		
		if( ppScatterDPS==NULL && m_nDimension==2 )
		{
			ppScatterDPS = new TDataPointValVal*[m_nClusterNum];
			for(dim =0;dim<m_nClusterNum;dim++)
			{
				ppScatterDPS[dim] = new TDataPointValVal[pCount[dim]];
			}
		}
		if( ppClusterDPS==NULL )
		{
			ppClusterDPS = new TDataPointValVal*[m_nClusterNum];
			for(dim =0;dim<m_nClusterNum;dim++)
			{
				ppClusterDPS[dim] = new TDataPointValVal[pCount[dim]];
			}
		}
		
		int *pPos = new int[m_nClusterNum];
		memset(pPos,0,m_nClusterNum*sizeof(int));

		nInterval = (double)m_nSampleSize/100; 
		nStep = 0;
		for( pattern=0; pattern<m_nSampleSize && !m_bAbort; pattern++ )
		{
			if( !abValid[pattern] )
				continue;

			for( knodes=0; knodes<m_nClusterNum; knodes++ )
			{
				if( bDataOverload )
				{
					GetRowData( pData, arFieldRole, pattern+1, adDataRow );
				
					///////////////////////////////////////////////////////
					// 大数据按行处理数据标准化
					//
					for( dim=0; dim<m_nDimension; dim++ )
					{
						if( aNormInfo[dim].bNorm )
						{
							adDataRow[dim] = ( adDataRow[dim]-aNormInfo[dim].dMin ) / 
										   ( aNormInfo[dim].dMax - aNormInfo[dim].dMin );
						}

						m_pKLayer->SetNodeInputValue( knodes, dim, 
										adDataRow[dim] );
					}

				}
				else
				{
					for( dim=0; dim<m_nDimension; dim++ )
					{
						m_pKLayer->SetNodeInputValue( knodes, dim, 
										adDataAll(pattern, dim) );
						//Kohonen_Design.node_in_cluster_layer[knodes].input_value[dim] = Kohonen_Train.number_of_samples[pattern].data_in_sample[dim];}
					}
				}
			}

			NodesCompeteForActivation();
			//Kohonen_Design.kluster_nodes_compete_for_activation();
			////////////////////////////////////////////////////
			// 输出分类结果
			//
			if( bOutputInfo )
			{
				string.Format( "%d", pattern+1 );
				pTable2->InsertItem( pattern, string , true );
			}

			if( bDataOverload )
			{
				if( bOutputInfo )
					pTable2->SetItemText( pattern, 2, m_nChampCluster+1 );
			}
			else
			{
				for( dim=0; dim<m_nDimension; dim++ )
				{
					if( aNormInfo[dim].bNorm )
						realValue = adDataAll( pattern,dim ) * 
										(aNormInfo[dim].dMax - aNormInfo[dim].dMin) + aNormInfo[dim].dMin;
					else
						realValue = adDataAll(pattern,dim );
					if( bOutputInfo )
						pTable2->SetItemText( pattern, dim+1, realValue );

					if( m_nDimension==2 )
					{
						if( dim%2==0 )
							ppScatterDPS[m_nChampCluster][pPos[m_nChampCluster]].fXVal = realValue;
						else
							ppScatterDPS[m_nChampCluster][pPos[m_nChampCluster]].fYVal = realValue;
					}
						
				}
				if( bOutputInfo )
					pTable2->SetItemText( pattern, m_nDimension+1, m_nChampCluster+1 );
			}

			ppClusterDPS[m_nChampCluster][pPos[m_nChampCluster]].fXVal = pattern+1;
			ppClusterDPS[m_nChampCluster][pPos[m_nChampCluster]].fYVal = m_nChampCluster+1;
			pPos[m_nChampCluster] += 1;
			string.Format( "聚类%02d", m_nChampCluster+1 );
			if( pattern>=nStep*nInterval )
			{
				strProgressText.Format("正在进行样本聚类... 完成 %d%%", int(pattern/nInterval));
				nStep++;
			}
		}


		if( !m_bAbort )
		{
			if( bOutputInfo )
			{
				CRsltElementTable *pVariable2=new CRsltElementTable( "Kohonen神经网络分类表", pTable2 );
				pResult->Add(pVariable2); 
			}

			if( nTreatedData>0 )
			{
				///////////////////////////////////
				// 输出数据预处理信息
				CTString strTitle, strTreated;
				strTitle.Format(   " 数据预处理:        总记录数   忽略行数   填充均值行数\n\n" );
				strTreated.Format( "                           %8d         %8d          %8d", 
							   pData->GetRowCount(), 
							   nTreatedData, 
							   0 );
				CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
				pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
				pResult->Add( pTreatedTextRslt );
				//
				///////////////////////////////////pClusterDPS
			}

 			pClusterplot = new CTChartClustering;
			for (dim=0;dim<m_nClusterNum;dim++)
			{
				string.Format( "聚类%02d", dim+1 );
				pClusterplot->SetScatter(ppClusterDPS[dim],pCount[dim],ValVal,string.GetData());
			}
			pClusterplot->SetTitle("聚类图");
		// 			pClusterplot->CreateObject( pClusterDPS, m_nSampleSize, 0, "聚类图", "记录号", "聚类号", 0 );
			CRsltElementChart *pClusterChart = new CRsltElementChart( "聚类图", pClusterplot );
			pResult->Add( pClusterChart );

			if( ppScatterDPS!=NULL )
			{
				pScatterplot = new CTChartScatterPlot;
				for (dim=0;dim<m_nClusterNum;dim++)
				{
					string.Format( "聚类%02d", dim+1 );
					pScatterplot->SetScatter(ppScatterDPS[dim],pCount[dim],ValVal,string.GetData());
				}
				pClusterplot->SetTitle("散点图");
				// 				pScatterplot->CreateObject( pScatterDPS, m_nSampleSize, 0, "散点图", strVar1, strVar2, 0 );
				CRsltElementChart *pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
				pResult->Add( pScatterChart );
			}
			//
			//////////////////////////////////
		}

		if( ppScatterDPS )
		{
			for (dim=0;dim<m_nClusterNum;dim++)
				delete [] ppScatterDPS[dim];
			delete [] ppScatterDPS;
			ppScatterDPS = NULL;
		}
		
		if( ppClusterDPS )
		{
			for (dim=0;dim<m_nClusterNum;dim++)
				delete [] ppClusterDPS[dim];
			delete []ppClusterDPS;
			ppClusterDPS = NULL;
		}
		
		delete [] pCount;
		delete [] pPos;
	}

	if( aNormInfo )
	{
		delete[] aNormInfo;
		aNormInfo = NULL;
	}
	if( m_bAbort ) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Testing
//输入参数：CDataInterface * pData, 
//			CPtrArray& arFieldRole, 
//			CResult * pResult, 
//			bool bOutputInfo
//函数功能：
//////////////////////////////////////////////////////////////////////////
bool CKohonenTopology::Testing( CDataInterface * pData, CTStringArray& arFieldRole, CResult * pResult, bool bOutputInfo )
{
	int i=0, nSize=0;
	CDoubleMatrix  adDataAll;
	CDoubleVector  adDataRow;
	NormInfo * aNormInfo = NULL;  // 保存样本各维是否需要标准化及最大与最小值
	CIntVector  abValid(pData->GetRowCount());

	int dim, knodes, pattern, nCol, nTreatedData=0;
	double realValue;
	bool bDataOverload = false;
	CTString string, strVar1, strVar2, strProgressText;

	aNormInfo = new NormInfo[m_nDimension];
	for( pattern=0; pattern<pData->GetRowCount(); pattern++ )
		abValid[pattern] = true;

	if( m_nDimension * m_nSampleSize < DATAOVERLOAD )
	{
		bDataOverload = false;
		if( NormalizeSampleData( pData, arFieldRole, bDataOverload, aNormInfo, abValid, adDataAll )==false )
		{
			delete[] aNormInfo; 
			return false;
		};
	}
	else
	{
		bDataOverload = true;
		if( NormalizeSampleData( pData, arFieldRole, bDataOverload, aNormInfo, abValid, adDataAll )==false )
		{
			delete[] aNormInfo;
			return false;
		}
		adDataRow.create(m_nDimension);
	}

	for( pattern=0; pattern<pData->GetRowCount(); pattern++ )
	{
		if( !abValid[pattern] )
			nTreatedData++;
	}

	if( nTreatedData==pData->GetRowCount() )
	{
// 		AfxMessageBox( IDS_ALLDATAINVALIDATE, MB_ICONSTOP );
		m_bAbort = true;
	}

	if( m_bAbort )
	{
		delete[] aNormInfo;
		delete[] abValid;
		if( adDataRow )
			delete[] adDataRow;
		return false;
	}

/*
	strProgressText = _T("测试进行中...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	///////////////////////////////////////////////////
	// 得出分类结果
	TDataPointValVal **pScatterDPS = NULL, **pClusterDPS = NULL;
	CTChartScatterPlot * pScatterplot = NULL, * pClusterplot = NULL;
	//分配内存
	pScatterDPS = new TDataPointValVal *[m_nClusterNum];
	pClusterDPS = new TDataPointValVal *[m_nClusterNum];
	int *pPointCount = new int [m_nClusterNum];
	for (i=0; i<m_nClusterNum; i++)
	{
		*(pScatterDPS+i) = new TDataPointValVal[m_nSampleSize];
		*(pClusterDPS+i) = new TDataPointValVal[m_nSampleSize];
		*(pPointCount+i) = 0;
	}

	CTLTable* pTable = NULL;
	if( bOutputInfo )
	{
		pTable = new CTLTable;
		string.Format( "%s", "分类结果" );
		pTable->SetTitle(string);
	}

	if( bDataOverload )
	{
		if( bOutputInfo )
		{
			pTable->CreateTable( m_nSampleSize+1,2);
			pTable->InsertColumn(0, "样本");
			pTable->InsertColumn(1, "分类");
		}
	}
	else
	{
		if( bOutputInfo )
		{
			pTable->CreateTable( m_nSampleSize+1,m_nDimension+1);
			pTable->InsertColumn(0, "样本");
		}
		for( nCol=0; nCol<arFieldRole.GetSize(); nCol++)
		{
			CTString strRole = arFieldRole.GetAt(nCol);
			CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strRole);
			if( bOutputInfo )
				pTable->InsertColumn( nCol+1, pField->GetFieldName());
			if( strVar1.IsEmpty() )
			{
				strVar1 = pField->GetLabel();
				if( strVar1.IsEmpty() )
					strVar1 = pField->GetFieldName();
			}
			else if( strVar2.IsEmpty() )
			{
				strVar2 = pField->GetLabel();
				if( strVar2.IsEmpty() )
					strVar2 = pField->GetFieldName();
			}
		}
		if( bOutputInfo )
			pTable->InsertColumn(nCol+1, "分类");
	}

	double nInterval = (double)m_nSampleSize/100; 
	int nStep = 0;
	for( pattern=0; pattern<m_nSampleSize && !m_bAbort; pattern++ )
	{
		if( !abValid[pattern] )
			continue;

		for( knodes=0; knodes<m_nClusterNum; knodes++ )
		{
			if( bDataOverload )
			{
				GetRowData( pData, arFieldRole, pattern+1, adDataRow );
			
				///////////////////////////////////////////////////////
				// 大数据按行处理数据标准化
				//
				for( dim=0; dim<m_nDimension; dim++ )
				{
					if( aNormInfo[dim].bNorm )
					{
						adDataRow[dim] = ( adDataRow[dim]-aNormInfo[dim].dMin ) / 
									   ( aNormInfo[dim].dMax - aNormInfo[dim].dMin );
					}

					m_pKLayer->SetNodeInputValue( knodes, dim, 
									adDataRow[dim] );
				}

			}
			else
			{
				for( dim=0; dim<m_nDimension; dim++ )
				{
					m_pKLayer->SetNodeInputValue( knodes, dim, 
									adDataAll(pattern,dim) );
					//Kohonen_Design.node_in_cluster_layer[knodes].input_value[dim] = Kohonen_Train.number_of_samples[pattern].data_in_sample[dim];}
				}
			}
		}

		NodesCompeteForActivation();
		//Kohonen_Design.kluster_nodes_compete_for_activation();
		////////////////////////////////////////////////////
		// 输出分类结果
		//
		if( bOutputInfo )
		{
			string.Format( "%d", pattern+1 );
			pTable->InsertItem( pattern, string);
		}
		if( bDataOverload )
		{
			if( bOutputInfo )
				pTable->SetItemText( pattern, 2, m_nChampCluster );
		}
		else
		{
			for( dim=0; dim<m_nDimension; dim++ )
			{
				if( aNormInfo[dim].bNorm )
					realValue = adDataAll( pattern,dim ) * 
									(aNormInfo[dim].dMax - aNormInfo[dim].dMin) + aNormInfo[dim].dMin;
				else
					realValue = adDataAll(pattern,dim);
				if( bOutputInfo )
					pTable->SetItemText( pattern, dim+1, realValue );

				if( m_nDimension==2 )
				{
					nSize = pPointCount[m_nChampCluster];
					if( dim%2==0 )
						pScatterDPS[m_nChampCluster][nSize].fXVal = realValue;
					else
						pScatterDPS[m_nChampCluster][nSize].fYVal = realValue;
				}
			}
			if( bOutputInfo )
				pTable->SetItemText( pattern, m_nDimension+1, m_nChampCluster+1 );
		}
		//聚类
		nSize = pPointCount[m_nChampCluster];
		pClusterDPS[m_nChampCluster][nSize].fXVal =  pattern+1;
		pClusterDPS[m_nChampCluster][nSize].fYVal = m_nChampCluster+1;
		pPointCount[m_nChampCluster] += 1;
		string.Format( "聚类%02d", m_nChampCluster+1 );
		printf("*****聚类号=%s*****\n",string.GetData());
		/////////////////////////////////////////////////////
		if( pattern>=nStep*nInterval )
		{
			nStep++;
		}
	}
	if( !m_bAbort )
	{
		if( bOutputInfo )
		{
			CRsltElementTable *pVariable=new CRsltElementTable( "Kohonen神经网络分类表", pTable );
			pResult->Add(pVariable);
		}
		if( nTreatedData>0 )
		{
			///////////////////////////////////
			// 输出数据预处理信息
			CTString strTitle, strTreated;
			strTitle.Format(   " 数据预处理:        总记录数   忽略行数   填充均值行数\n\n" );
			strTreated.Format( "                           %8d         %8d          %8d", 
						   pData->GetRowCount(), 
						   nTreatedData, 
						   0 );
			CRsltElementText * pTreatedTextRslt = new CRsltElementText("数据预处理");
			pTreatedTextRslt->AddString( strTitle + strTreated.GetData() );
			pResult->Add( pTreatedTextRslt );
			//
			///////////////////////////////////
		}
		//聚类图
		pClusterplot = new CTChartScatterPlot;
		for (i=0;i<m_nClusterNum; i++)
		{
			int n = pPointCount[i];
			string.Format( "聚类%02d", i+1 );
			pClusterplot->SetScatter(pClusterDPS[i],n,ValVal,string.GetData());
			delete []pClusterDPS[i];
		}
		pClusterplot->SetXAxilLabel("记录号");
		pClusterplot->SetYAxilLabel("聚类号");
		pClusterplot->SetTitle("聚类图");
		CRsltElementChart *pClusterChart = new CRsltElementChart( "聚类图", pClusterplot );
		pResult->Add( pClusterChart );
		//散点图
		if( pScatterDPS )
		{
			pScatterplot = new CTChartScatterPlot;
			for (i=0;i<m_nClusterNum; i++)
			{
				int n = pPointCount[i];
				string.Format( "聚类%02d", i+1 );
				pScatterplot->SetScatter(pScatterDPS[i],n,ValVal,string.GetData());
				delete []pScatterDPS[i];
			}
			pScatterplot->SetTitle("散点图");
			CRsltElementChart *pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
			pResult->Add( pScatterChart );
		}
		//////////////////////////////////
	}

	if( aNormInfo )
	{
		delete[] aNormInfo;
		aNormInfo = NULL;
	}
	if( pClusterDPS )
	{
		delete pClusterDPS;
		pClusterDPS = NULL;
	}
	if( pScatterDPS )
	{
		delete pScatterDPS;
		pScatterDPS = NULL;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：NormalizeSampleData
//输入参数：CDataInterface * pData, 数据接口
//			CPtrArray& arFieldRole, 输入字段
//			bool bOverload,			
//			NormInfo * aNormInfo,	
//			CIntVector & abValid, 
//			CDoubleMatrix& padDataAll
//输出结果：
//函数功能：
//////////////////////////////////////////////////////////////////////////
bool CKohonenTopology::NormalizeSampleData( CDataInterface * pData, CTStringArray& arFieldRole, bool bOverload, NormInfo * aNormInfo, CIntVector & abValid, CDoubleMatrix& padDataAll )
{
	int i, j, iMax, iMin, nColCount, nInputField;
	bool bTrigger;
	double dMax, dMin, dTemp;
	CTString strProgressText;

	CField * pField = NULL;
	CDataAccess& rDataAccess = pData->m_DataAccess;
	rDataAccess.First();
	CFieldList * pFieldList = rDataAccess.m_pFieldList;
	//nColCount = pFieldList->GetFieldCount();
	nColCount = arFieldRole.GetSize();

/*
	strProgressText = _T("标准化数据样本...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	if( bOverload )
	{
		nInputField = 0;
		for( j=0; j<nColCount; j++ )
		{
			CTString strRole = arFieldRole.GetAt(j);
			pField = pFieldList->FieldByIndex( j );
			if( pField->IsNull() )
			{
				aNormInfo[nInputField].dMax = aNormInfo[nInputField].dMin = 0;
				abValid[0] = false;
				break;
			}
			else
			{
				aNormInfo[nInputField].dMax = aNormInfo[nInputField].dMin = pField->GetAsDouble();
			}
			aNormInfo[nInputField].bNorm = false;
			nInputField++;
		}
	}
	else
	{
		padDataAll.create(m_nSampleSize,m_nDimension);
		int nRow = 0;
		while( !rDataAccess.Eof() )
		{
			pFieldList = rDataAccess.m_pFieldList;
			nInputField = 0;
			for( int nCol=0; nCol<nColCount; nCol++)
			{
				CTString strRole = arFieldRole.GetAt(nCol);
				pField = pFieldList->FieldByName(strRole);
				if( pField->IsNull() )
				{
					padDataAll(nRow,nInputField) = 0;
					abValid[nRow] = false;
					break;
				}
				else
				{
					padDataAll(nRow,nInputField) = pField->GetAsDouble();
				}
				nInputField++;
			}
			rDataAccess.Next();
			nRow++;
		}
	}

	j = 0;
	for( int k=0; k<nColCount; k++)
	{
		CTString strRole = arFieldRole.GetAt(k);
		bTrigger = true;
		if( bOverload )
			dMax = dMin = aNormInfo[j].dMax;
		else
		{
			CDoubleVector vect = padDataAll.GetVector(j);
			dMax = dMin = vect.Min();
		}

		// identify minimum and maximum values for each dimension
		for( i=0; i<m_nSampleSize; i++ )
		{
			if( !abValid[i] )
				continue;

			if( bOverload )
			{
				rDataAccess.SetCurRecNo( i+1 );
				pField = pFieldList->FieldByName(strRole);
				if( !pField->IsNull() )
					dTemp = pField->GetAsDouble();
				else
				{
					abValid[i] = false;
					continue;
				}
				if( dTemp < dMin )
					dMin = dTemp;
				if( dTemp > dMax )
					dMax = dTemp;

			}
			else
			{
				if( padDataAll.GetValue(i,j) < dMin)
					dMin = padDataAll.GetValue(i,j);
				if( padDataAll.GetValue(i,j) > dMax)
					dMax = padDataAll.GetValue(i,j);
			}
		}
		// normalize the values in each dimension of the signal
		//dMaxOutputValue[j] = dMax;
		//dMinOutputValue[j] = dMin;
		//max_output_value[j] = max;
		//min_output_value[j] = min;

		iMax = int(dMax);
		iMin = int(dMin);
	
	
		if((iMax == 1) && (iMin == 0) && (dMax <= 1.0) && (dMin <= 0.0))
			bTrigger = false;

		if((iMax == 1) && (iMin == 1) && (dMax <= 1.0) && (dMin <= 1.0))
			bTrigger = false;

		if((iMax == 0) && (iMin == 0) && (dMax <= 0.0) && (dMin <= 0.0))
			bTrigger = false;
		

		if( bTrigger )   //  do not normalize binary signals
		{
			aNormInfo[j].bNorm = true;
			aNormInfo[j].dMax = dMax;
			aNormInfo[j].dMin = dMin;
			if( !bOverload )
			{
				for( i=0; i<m_nSampleSize; i++)
					if( abValid[i] )
						padDataAll(i,j) = 
								( padDataAll(i,j) - dMin) /  ( dMax - dMin );
			}
				//number_of_samples[i].data_in_sample[j] = (number_of_samples[i].data_in_sample[j] - min)/(max - min);}
		}
		else
			aNormInfo[j].bNorm = false;

		j++;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：NodesCompeteForActivation
//函数功能：获得节点输出值的最小距离
//////////////////////////////////////////////////////////////////////////
void CKohonenTopology::NodesCompeteForActivation()
{
	double minimum_distance;

	m_nChampCluster = 0;
	m_pKLayer->CalculateNodeSumSquareEuclideanDistance( 0 );
	minimum_distance = m_pKLayer->GetNodeOutputValue( 0 );

	for( int m=1; m<m_nClusterNum; m++ )
	{
		m_pKLayer->CalculateNodeSumSquareEuclideanDistance( m );
		//node_in_cluster_layer[m].calculate_sum_square_Euclidean_distance();
		if( m_pKLayer->GetNodeOutputValue( m ) < minimum_distance )
				//node_in_cluster_layer[m].output_value[0] < minimum_distance)
		{
			m_nChampCluster = m;
			minimum_distance = m_pKLayer->GetNodeOutputValue( m );
				//node_in_cluster_layer[m].output_value[0];
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
//函数名称：UpdateNetwork
//输入参数：int ep 调节参数，可以获得合适的学习率
//函数功能：更新网络权值，使其比较合理的权重
//////////////////////////////////////////////////////////////////////////
void CKohonenTopology::UpdateNetwork(int ep)
{
	int nMaxEpoch;
	double dAdjustedRate;

	if( m_unIterateNum==1 ) 
		nMaxEpoch = 1;
	else 
		nMaxEpoch = m_unIterateNum - 1;

	dAdjustedRate = m_dMaxRate - ((( m_dMaxRate - m_dMinRate ) / nMaxEpoch ) * ep );
	//double adjusted_learning_rate = max_learning_rate - (((max_learning_rate - min_learning_rate) / maxepoch) * epoch_count);
	m_dInterimRate = dAdjustedRate * m_dInterimRate;
	//interim_learning_rate = adjusted_learning_rate * interim_learning_rate;
	m_pKLayer->UpdateNodeWeights( m_nChampCluster, m_dInterimRate );
	//node_in_cluster_layer[kluster_champ].update_the_weights(interim_learning_rate);
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SetLayerWeight
//输入参数：CDoubleVector& adWeight，权重
//函数功能：设置权重，这里的节点数一般都是相同的数目
//////////////////////////////////////////////////////////////////////////
void CKohonenTopology::SetLayerWeight( CDoubleVector& adWeight)
{
	for( int i=0; i<m_nClusterNum; i++ )
		for( int j=0; j<m_nDimension; j++ )
			m_pKLayer->SetNodeWeight( i, j, adWeight[i*m_nDimension + j] );
}

//////////////////////////////////////////////////////////////////////////
//函数名称：SaveNetwork
//函数功能：保存网络的参数和学习规则，权值
//////////////////////////////////////////////////////////////////////////
CDoubleVector CKohonenTopology::GetLayerWeight()
{
	int i=0, j=0, nIndex=0, nVlen=0;
	CDoubleVector v;
	
	nVlen = m_nClusterNum * m_nDimension;
	v.create(nVlen);

	for (i=0; i<m_nClusterNum; i++)
	{
		for (j=0; j<m_nDimension; j++)
		{
			v(nIndex) = m_pKLayer->GetNodeWeight(i,j);
			nIndex ++;
		}
	}
	return v;
}
