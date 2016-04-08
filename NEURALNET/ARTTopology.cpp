//////////////////////////////////////////////////////////////////////
// ARTTopology.cpp:                                                 //
//																	//
// Define:        ART算法拓扑结构类实现                             //
// Creator:       屈韬                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ARTTopology.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
/*

const UINT WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
const UINT WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CARTTopology::CARTTopology()
{
}

CARTTopology::CARTTopology( int nType, int nSampleSize, int nDimension, int nClusterNum,
						    long lRuntime, int nIterateNum, double fVigilance ) : CTopology( nType, nSampleSize, nDimension, nClusterNum,
																		lRuntime, nIterateNum )
{
	m_pInterfaceLayer = NULL;
	m_pClusterLayer = NULL;

	m_bReset = false;
	m_nResetCount = 0;
	m_nClusterRange = 0;
	m_nClusterCount = 0;
	m_nChampCluster = 0;

	m_nClusterNum = m_nSampleSize;
	m_dVigilance = fVigilance;

	m_pInterfaceLayer = new CARTInterfaceLayer( m_nDimension, m_nClusterNum );
	m_pClusterLayer = new CARTClusterLayer( m_nDimension, m_nClusterNum );
}

CARTTopology::~CARTTopology()
{
	if( m_pInterfaceLayer )
		delete m_pInterfaceLayer;
	if( m_pClusterLayer )
		delete m_pClusterLayer;
}

bool CARTTopology::Training( CDataInterface * pData, CTStringArray& arFieldRole, CResult *& pResult, bool bOutputInfo )
{
	int i, nTreatedData=0;
	CTString strVar1, strVar2, strProgressText;
	TDataPointValVal* pScatterDPS = NULL, * pClusterDPS = NULL;
	CTChartScatterPlot* pScatterplot=NULL, * pClusterplot = NULL;
	CTLTable* pTable = NULL;

	CTString string;
	if( bOutputInfo )
	{
		pTable=new CTLTable;
		pTable->CreateTable(m_nSampleSize+1 , m_nDimension+2);
		string.Format( "%s", "分析结果" );
		pTable->SetTitle(string);

		pTable->InsertColumn(0, "样本");
	}
	for( int nCol=0; nCol<arFieldRole.GetSize(); nCol++)
	{
		CTString strFieldName = arFieldRole.GetAt(nCol);
		CField * pField = pData->m_DataAccess.m_pFieldList->FieldByName(strFieldName);
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
		pTable->InsertColumn( m_nDimension+1, "聚类" );

	////////////////////////////////////////////////////////////
	// 网络训练过程
	//
	int nAvailableNodesForClustering;
	int nGainOne;
	double dNormOfInputVector, dNormOfActivationVector;
	int nClusterActive;
	CDoubleVector adDataRow(m_nDimension);

	if( pScatterDPS==NULL && m_nDimension==2 )
		pScatterDPS = new TDataPointValVal[m_nDimension];

	double nInterval = (double)m_nSampleSize/100; 
	int nStep = 0;
	bool bIgnore = false;
	for( int pattern=0; pattern<m_nSampleSize && !m_bAbort; pattern++ )
	{
		nAvailableNodesForClustering = m_nClusterNum;

		if( !GetRowData( pData, arFieldRole, pattern+1, adDataRow ) || !ValidateInputData( adDataRow ) ) // DataAccess中的行从1开始
		{
			//////////////////////////////////////////////////
			// 处理无效输入数据
			//
			if( !bIgnore )
			{
				if( pScatterDPS )
					delete pScatterDPS;
				if( pTable )
					delete pTable;
				CRsltElementText * pText = new CRsltElementText("警告(神经网络参数设置)\n");
				pText->AddString("数据包含无效数据！\n：ART1算法只处理布尔型数据。");
				pResult = new CResult("神经网络分析");
				pResult->Add(pText);
				
				return false;
				/* Commentated by pyx 2005-12-28
				int nResult;
								CTString strMsg;
								switch( nResult )
								{
								case IDABORT:
									if( pScatterDPS )
										delete pScatterDPS;
									if( pTable )
										delete pTable;
									pResult->Clear();
				
									return false;
				
								case IDRETRY:
									pattern--;
									continue;
									break;
				
								case IDIGNORE:
									if( bOutputInfo )
									{
										string.Format( "%d", pattern+1 );
										pTable->InsertItem( pattern, string , true );
									
										for( i=0; i<m_nDimension; i++ )
											pTable->SetItemText( pattern, i+1, (int)adDataRow[i]);
										pTable->SetItemText( pattern, i+1, "无效数据" );
									}
									bIgnore = true;
									nTreatedData++;
									continue;
									break;
								}*/
				
			}
			else
			{
				nTreatedData++;
				continue;
			}
		}
		//
		//////////////////////////////////////////////////

		do
		{
			dNormOfInputVector = 0.0;
			dNormOfActivationVector = 0.0;
			nClusterActive = 0;

			m_pInterfaceLayer->SetLayerInputValue( adDataRow );
			
			for( i=0; i<m_nClusterNum; i++)
			{
				if( m_pClusterLayer->GetActivation( i ) == 1.0 ) 
					nClusterActive = 1; 
			}

			for( i=0; i<m_nDimension; i++)
				dNormOfInputVector += adDataRow[i];

			if( (nClusterActive!=1) && (dNormOfInputVector>0.0) )
				nGainOne = 1;
			else 
				nGainOne = 0;

			m_pInterfaceLayer->CalculateOutputValue( nGainOne );

			BroadcastOutputToClusterLayer();

			ClusterNodesCompeteForActivation(true);

			m_pInterfaceLayer->RecomputeActivation( m_nChampCluster );

			for( i=0; i<m_nDimension; i++)
				dNormOfActivationVector += m_pInterfaceLayer->GetActivation( i );

			UpdateNetwork( dNormOfInputVector, dNormOfActivationVector );

			if( m_bReset )
			{
				if( --nAvailableNodesForClustering<1 )
				{
					break;
				}
			}
		}while( m_bReset && !m_bAbort);

		//set cluster activation to zero
		//   for(int cnode = 0; cnode < clusterange + 1; cnode++)
		//   {node_in_cluster_layer[cnode].activation = 0.0;}

		/////////////////////////////////////////////
		// 输出分类结果
		//
		if( bOutputInfo )
		{
			string.Format( "%d", pattern+1 );
			pTable->InsertItem( pattern, string );
		}

		for( i=0; i<m_nDimension; i++ )
		{
			if( bOutputInfo )
				pTable->SetItemText( pattern, i+1, (int)adDataRow[i]);
			if( m_nDimension==2 )
			{
				if( i%2==0 )
					pScatterDPS[i].fYVal = adDataRow[i];
				else
					pScatterDPS[i].fXVal = adDataRow[i];
			}
		}
		if( bOutputInfo )
			pTable->SetItemText( pattern, i+1, m_pClusterLayer->GetNodeClusterTag( m_nChampCluster ));

		if( pClusterDPS==NULL )
			pClusterDPS = new TDataPointValVal[m_nSampleSize];

		pClusterDPS[pattern].fXVal = pattern+1;
		pClusterDPS[pattern].fYVal = m_pClusterLayer->GetNodeClusterTag( m_nChampCluster );
		string.Format( "聚类%02d", m_pClusterLayer->GetNodeClusterTag( m_nChampCluster ));
// 		pClusterDPS->MarkerArr.Add( string );

//		if( pScatterDPS )
//		{
//			pScatterDPS->MarkerArr.Add( string );
//		}

		for( i=0; i<m_nClusterRange+1; i++ )
			m_pClusterLayer->SetActivation( i, 0.0 );

		if( pattern>=nStep*nInterval )
		{
			nStep++;
		}
	}
	if( bOutputInfo )
	{
		CRsltElementTable *pVariable = new CRsltElementTable( "ART神经网络分析结果", pTable );
		pResult->Add(pVariable);
	}
	if( nTreatedData==pData->GetRowCount() )
	{
		m_bAbort = true;
	}

	if( !m_bAbort)
	{
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
		if( pClusterDPS )
		{// "记录号", "聚类号
			pClusterplot = new CTChartScatterPlot;
			pClusterplot->SetScatter( pClusterDPS, m_nSampleSize, StrArr);
			CTString strTitle = "聚类图";
			pClusterplot->SetTitle(strTitle);
			CTString strX = "记录号";
			CTString strY = "聚类号";
			pClusterplot->SetXAxilLabel(strX);
			pClusterplot->SetYAxilLabel(strY);
			CRsltElementChart *pClusterChart = new CRsltElementChart( "聚类图", pClusterplot );
			pResult->Add( pClusterChart );
		}

		if( pScatterDPS )
		{
			pScatterplot = new CTChartScatterPlot;
			pScatterplot->SetScatter( pScatterDPS, m_nSampleSize, StrArr);
			CTString strTitle = "散点图";
			pClusterplot->SetTitle(strTitle);
			pClusterplot->SetXAxilLabel(strVar1);
			pClusterplot->SetYAxilLabel(strVar2);
			CRsltElementChart *pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
			pResult->Add( pScatterChart );
		}

		////////////////////////////////////////////////
		// 输出权值表
		//
		m_pInterfaceLayer->OutputNodeWeight( pResult );
		m_pClusterLayer->OutputNodeWeight( pResult );
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

	if( m_bAbort )	return false;

	return true;
}

bool CARTTopology::Testing( CDataInterface * pData, CTStringArray& arFieldRole, CResult * pResult, bool bOutputInfo )
{
	int i, nCol, nTreatedData=0;
	CTString string, strVar1, strVar2, strProgressText;
	TDataPointValVal* pClusterDPS = NULL, * pScatterDPS = NULL;
	CTChartScatterPlot * pScatterplot = NULL, * pClusterplot = NULL;
	CDoubleVector adDataRow(m_nDimension);
	CTLTable* pTable = NULL;

/*
	strProgressText = _T("测试进行中...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	if( bOutputInfo )
	{
		pTable = new CTLTable;
		pTable->CreateTable(m_nSampleSize+1,m_nDimension+2);
		string.Format( "%s", "测试结果" );
		pTable->SetTitle(string);

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
		pTable->InsertColumn( m_nDimension+1, "聚类" );

	if( m_nDimension==2 && pScatterDPS==NULL )
		pScatterDPS = new TDataPointValVal[m_nDimension];

	double nInterval = (double)m_nSampleSize/100; 
	int nStep = 0;
	bool bIgnore = false;
	for( int pattern=0; pattern<m_nSampleSize && !m_bAbort; pattern++ )
	{
		if( !GetRowData( pData, arFieldRole, pattern+1, adDataRow ) || !ValidateInputData( adDataRow ) ) // DataAccess中的行从1开始
		{
			//////////////////////////////////////////////////
			// 处理无效输入数据
			//
/*
			if( !bIgnore )
			{
				int nResult;
				CTString strMsg;
				strMsg.Format( IDS_INVALIDATEDATA, pattern+1 );
				nResult = AfxMessageBox( strMsg, MB_ABORTRETRYIGNORE | MB_ICONQUESTION );
				switch( nResult )
				{
				case IDABORT:
					if( pScatterDPS )
						delete pScatterDPS;
					if( pTable )
						delete pTable;
					pResult->Clear();

					return true;

				case IDRETRY:
					pattern--;
					continue;
					break;

				case IDIGNORE:
					if( bOutputInfo )
					{
						string.Format( "%d", pattern+1 );
						pTable->InsertItem( pattern, string , true );
						
						for( i=0; i<m_nDimension; i++ )
							pTable->SetItemText( pattern, i+1, (int)adDataRow[i]);
						pTable->SetItemText( pattern, i+1, "无效数据" );
					}
					bIgnore = true;
					nTreatedData++;
					continue;

					break;

				}
			}
			else
			{
				nTreatedData++;
				continue;
			}
*/
		}

		//
		//////////////////////////////////////////////////////

		m_pClusterLayer->SetLayerInputValue( adDataRow );

		//transmit_pattern_to_cluster()

		ClusterNodesCompeteForActivation(false);

		/////////////////////////////////////////////
		// 输出分类结果
		//
		if( bOutputInfo )
		{
			string.Format( "%d", pattern+1 );
			pTable->InsertItem( pattern, string , true );
		}
		for( i=0; i<m_nDimension; i++ )
		{
			if( bOutputInfo )
				pTable->SetItemText( pattern, i+1, (int)adDataRow[i]);
			if( m_nDimension==2 )
			{
				if( i%2==0 )
					pScatterDPS[i].fXVal = adDataRow[i];
				else
					pScatterDPS[i].fYVal = adDataRow[i];
			}
		}

		if( bOutputInfo )
			pTable->SetItemText( pattern, i+1, m_pClusterLayer->GetNodeClusterTag( m_nChampCluster ));

		if( pClusterDPS==NULL )
			pClusterDPS = new TDataPointValVal[m_nSampleSize];

		pClusterDPS[pattern].fXVal = pattern+1;
		pClusterDPS[pattern].fYVal = m_pClusterLayer->GetNodeClusterTag( m_nChampCluster );
		string.Format( "聚类%02d", m_pClusterLayer->GetNodeClusterTag( m_nChampCluster ));
// 		pClusterDPS->MarkerArr.Add( string );
//		if( pScatterDPS )
//		{
//			pScatterDPS->MarkerArr.Add( string );
//		}

		if( pattern>=nStep*nInterval )
		{
/*
			strProgressText.Format("正在测试ART1网络结构... 完成 %d%%", int(pattern/nInterval));
			m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
			m_pProgressWnd->SendMessage( WM_PROGRESS_STEP, 0, 0 );
*/

			nStep++;
		}

	}
//    for(pattern = 0; pattern < ART_Test[Atest].sample_number; pattern++)
//    {
//      for(dim = 0; dim < ART_Design.dimensions_of_signal; dim++)
//      {ART_Design.node_in_input_layer[dim].signal_value = ART_Test[Atest].number_of_samples[pattern].data_in_sample[dim];}

//      ART_Design.transmit_pattern_to_cluster();
//      ART_Design.cluster_nodes_compete_for_activation(2);

//      ART_savefile_ptr <<pattern + 1<<" ";
//      for(dim = 0; dim < ART_Design.dimensions_of_signal; dim++)
//      {ART_savefile_ptr << int(ART_Design.node_in_input_layer[dim].signal_value);}

//      ART_savefile_ptr << " " << ART_Design.node_in_cluster_layer[ART_Design.cluster_champ].cluster_tag << "\n";
//    }
	if( nTreatedData==pData->GetRowCount() )
	{
// 		AfxMessageBox( IDS_ALLDATAINVALIDATE, MB_ICONSTOP );
		m_bAbort = true;
	}

	if( !m_bAbort)
	{
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

/*
		strProgressText = _T("输出测试结果...");
		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

		if( bOutputInfo )
		{
			CRsltElementTable *pVariable=new CRsltElementTable( "ART神经网络测试结果", pTable );
			pResult->Add(pVariable);
		}

		pClusterplot = new CTChartClustering;
		pClusterplot->SetScatter(pClusterDPS,m_nSampleSize,ValVal);
		pClusterplot->SetTitle("聚类图");
		// 		pClusterplot->CreateObject( pClusterDPS, m_nSampleSize, 0, "聚类图", "记录号", "聚类号", 0 );
		CRsltElementChart *pClusterChart = new CRsltElementChart( "聚类图", pClusterplot );
		pResult->Add( pClusterChart );

		if( pScatterDPS )
		{
			pScatterplot = new CTChartScatterPlot;
			pScatterplot->SetScatter(pClusterDPS,m_nSampleSize,ValVal);
			pScatterplot->SetTitle("散点图");
			// 			pScatterplot->CreateObject( pScatterDPS, m_nSampleSize, 0, "散点图", strVar1, strVar2, 0 );
			CRsltElementChart *pScatterChart = new CRsltElementChart( "散点图", pScatterplot );
			pResult->Add( pScatterChart );
		}
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

	if( m_bAbort ) return false;

	return true;
}

void CARTTopology::BroadcastOutputToClusterLayer()
{
	for( int d=0; d<m_nDimension; d++)
		for( int c=0; c<m_nClusterNum; c++)
			m_pClusterLayer->SetNodeInputValue( c, d, 
									m_pInterfaceLayer->GetNodeOutputValue( d, c ) );
}

void CARTTopology::ClusterNodesCompeteForActivation( bool bTraining )
{
	double dChampion = 0.0, dNetInput;
	int nCluster, nDimension;

	for( nCluster=0; nCluster<m_nClusterRange+1; nCluster++)
	{
		if( m_pClusterLayer->GetActivation( nCluster ) != -1.0)
		{
			dNetInput = m_pClusterLayer->CalculateNetInput( nCluster );
			if( dNetInput>dChampion )
			{
				dChampion = dNetInput;
				m_nChampCluster = nCluster;
			}
		}
	}	

    
	if( bTraining )
	{
		if( (m_pClusterLayer->GetNodeClusterTag( m_nChampCluster ) == 0))
		{
				m_pClusterLayer->SetNodeClusterTag( m_nChampCluster, m_nClusterCount+1 );
				m_nClusterCount++;
		}

		for( nCluster=0; nCluster<m_nClusterRange+1; nCluster++)
		{
			if( nCluster==m_nChampCluster )
				m_pClusterLayer->SetActivation( nCluster, 1.0 );
			else
			{
				if( m_pClusterLayer->GetActivation( nCluster ) != -1.0)
					m_pClusterLayer->SetActivation( nCluster, 0.0 );
			}

			m_pClusterLayer->EstablishNodeOutputValue( nCluster);

		  // send output signals to Interface layer
			for( nDimension=0; nDimension<m_nDimension; nDimension++)
				m_pInterfaceLayer->SetNodeInputValue( nDimension, nCluster+1, 
									m_pClusterLayer->GetNodeOutputValue( nCluster, nDimension ) );
	
		}
	}
}

void CARTTopology::UpdateNetwork( double dNormInputVector, double dNormActivationVector )
{
	int dim;
	
	if( (dNormActivationVector/dNormInputVector) < m_dVigilance )
	{
		m_pClusterLayer->SetActivation( m_nChampCluster, -1.0 );
		m_bReset = true;
		m_nResetCount++;
		if( m_nResetCount==m_nClusterNum-1 )
		{
			m_nClusterRange++;
			if( m_nClusterRange>m_nClusterNum )
				m_nClusterRange = m_nClusterNum;
		}
	}
	else
	{
		// update the weights of the champion cluster unit
//    for(int u = 0; u < node_in_cluster_layer[cluster_champ].number_of_inputs; u++)
		for( dim=0; dim<m_nDimension; dim++)
		{
 //   {node_in_cluster_layer[cluster_champ].input_weight_vector[u] = (weight_update_parameter * node_in_interface_layer[u].activation * node_in_cluster_layer[cluster_champ].input_weight_vector[u]) / ((weight_update_parameter - 1.0) + norm_of_activation_vector);}
			m_pClusterLayer->SetNodeWeight( m_nChampCluster, dim, 
					(WEIGHT_UPDATE_PARAMETER*m_pInterfaceLayer->GetActivation(dim) * 
					m_pClusterLayer->GetNodeWeight(m_nChampCluster, dim)) / 
					((WEIGHT_UPDATE_PARAMETER-1.0)+dNormActivationVector));

 //   for(int n = 0; n < dimensions_of_signal; n++)
 //   {node_in_interface_layer[n].input_weight_vector[cluster_champ] = node_in_interface_layer[n].input_weight_vector[cluster_champ] * node_in_interface_layer[n].activation;}
			m_pInterfaceLayer->SetNodeWeight( dim, m_nChampCluster, 
					m_pInterfaceLayer->GetNodeWeight(dim, m_nChampCluster) *
					m_pInterfaceLayer->GetActivation(dim));					
		}

		m_bReset = false;
		m_nResetCount = 0;
	}
}

void CARTTopology::SetInterfaceLayerWeight( CDoubleVector& adWeight) 
{
	for( int i=0; i<m_nClusterNum; i++ )
		for( int j=0; j<m_nDimension; j++ )
			m_pInterfaceLayer->SetNodeWeight( j, i, adWeight[i*m_nDimension + j] );

}

void CARTTopology::SetClusterLayerWeight( CDoubleVector& adWeight) 
{
	for( int i=0; i<m_nClusterNum; i++ )
		for( int j=0; j<m_nDimension; j++ )
			m_pClusterLayer->SetNodeWeight( i, j, adWeight[i*m_nDimension + j] );
}

void CARTTopology::SaveNetwork()
{
	TCLFile theFile;
	char strFileName[512];
	int i, j;

	sprintf(strFileName,"04d%d.tpl", (int)m_nTopologyType );
	//theFile.Open( strFileName, _O_CREAT| _O_RDWR);
	theFile.Open( strFileName, TCLFile::modeCreate|TCLFile::modeReadWrite);
	int v;
	v= ((int)m_nTopologyType);
	theFile<<v;
//	theFile<<((int)m_nTopologyType);
//	theFile<< ;
	theFile<< m_nDimension << m_nClusterNum << m_dVigilance 
			<< m_nClusterCount << m_nClusterRange;

	for( i=0; i<m_nClusterNum; i++ )
		for( j=0; j<m_nDimension; j++ )
		{
			double dValue = m_pInterfaceLayer->GetNodeWeight( j, i );
			theFile << dValue;
		}
	for( i=0; i<m_nClusterNum; i++ )
	{
		int nClustTag = m_pClusterLayer->GetNodeClusterTag( i );
		theFile << nClustTag;
		for( j=0; j<m_nDimension; j++ )
		{
			double dValue = m_pClusterLayer->GetNodeWeight( i, j );
			theFile << dValue;
		}
	}
	theFile.Close();
}

void CARTTopology::SetClusterTag(CIntVector& anTag)
{
	for( int i=0; i<m_nClusterNum; i++ )
		m_pClusterLayer->SetNodeClusterTag( i, anTag[i] );
}

bool CARTTopology::ValidateInputData(CDoubleVector& adDataRow)
{
	for( int nCol=0; nCol<m_nDimension; nCol++)
	{
		if(	(adDataRow[nCol]!=0) && (adDataRow[nCol]!=1) )
			return false;
	}

	return true;
}










































