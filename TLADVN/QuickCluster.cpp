// QuickCluster.cpp: implementation of the CQuickCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "QuickCluster.h"
#include <math.h>
#include "../DataInterface/Define.h"
#include "../DataInterface/TCDataArray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuickCluster::CQuickCluster()
{
	m_pMNC          = NULL;	
	//m_pCluster      = NULL;
	//m_pTempCluster  = NULL;
	m_pCount        = NULL;
	m_pClusterWeight= NULL;
	m_bCountFlag	= false;
	m_nIterationNum = 10;
	m_nNC           = 2;
	m_pWeightCases  = NULL;
	m_pPrevMNC  = NULL;
	m_nValidWeightCases = 0;
	m_bOutputCaseInfo = false;
}

CQuickCluster::~CQuickCluster()
{
	Clear();
}

//////////////////////////////////////////////////////////////////
//					释放临时空间
//////////////////////////////////////////////////////////////////
void CQuickCluster::Clear()
{
	for( int i=0; i<m_DataMatrix.size(); i++ )
	{
		delete m_DataMatrix.at(i);
	}
	m_DataMatrix.clear();

	delete [] m_pMNC;
	//delete [] m_pCluster;
	//delete [] m_pTempCluster;
	delete [] m_pCount;
	delete [] m_pClusterWeight;
	delete [] m_pWeightCases;
	delete [] m_pPrevMNC;
	//m_pCluster			= NULL;
	//m_pTempCluster		= NULL;
	m_pMNC				= NULL;
	m_pCount			= NULL;
	m_pClusterWeight	= NULL;
	m_pWeightCases		= NULL;
	m_pPrevMNC			= NULL;
}
/*
///////////////////////////////////////////////////////////////////////////////////
//
//					        求类中心之间的最小的距离
//
///////////////////////////////////////////////////////////////////////////////////
double CQuickCluster::GetMinDistance(int &x1,int &x2)
{
	assert(m_pData);
	double min = 1e100;
	for(int i=0;i<m_nRow;i++)
	{
		for(int j=0;j<m_nRow;j++)
		{
			if( i == j )
				continue;
			//double ptmp = EucDistance(i,j);
			double ptmp = EucDistance(&m_pMNC[i*m_nCol],&m_pMNC[j*m_nCol],m_nCol);
			if( IsZero(ptmp) )
				continue;
			if( ptmp < min )
			{
				min = ptmp;
				x1 = i;
				x2 = j;
			}
		}
	}
	return min;
}

/////////////////////////////////////////////////////////////////////////
//							求两个向量之间的距离
/////////////////////////////////////////////////////////////////////////
double CQuickCluster::EucDistance(double * x, double * y, int nLen)
{
	assert(x&&y&&nLen>0);
	double sum(0);
	for(int i=0;i<nLen;i++)
	{
		sum += ( x[i] - y[i] ) * ( x[i] - y[i] );
	}
	return sqrt(sum);
}
*/
/////////////////////////////////////////////////////////////////////////
//							指定最初的类中心
/////////////////////////////////////////////////////////////////////////
bool CQuickCluster::InitalClusterCenter()
{
	int i, j, k, l;
	int * pInitialCenter = new int[m_nNC];
	
	// 初始化前几条有效数据为聚类中心
	i = j = 0;
	while( j<m_nRow )
	{
		j++;

		if( m_pWeightCases && m_pWeightCases[j-1]==0 )
			continue;
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);

			if( pData->GetAt(j-1).iIndex<0 )
				break;
		}
		if( k<m_nCol )
			continue;

		pInitialCenter[i] = j-1;
		m_pCount[i] = 1;
		if( m_pWeightCases )
			m_pClusterWeight[i] = m_pWeightCases[j-1];
		//m_pCluster[j-1] = i;
		m_IntCluster.SetAt(j-1,i);
		i++;
		if( i==m_nNC )
			break;
	}
	if( j==m_nRow && i<m_nNC )// 有效数据不足，返回错误
	{
		delete[] pInitialCenter;
		return false;
	}

	double dMaxDistance = 0.0, dDistanceSquare, dMinofCenter;
	int iClosest1, iClosest2;
	double * dCenterDistance = new double[m_nNC*m_nNC];
	
	// 计算聚类中心间的相互距离，并记录距离最小值
	for( i=0; i<m_nNC-1; i++ )
	{
		for( j=i+1; j<m_nNC; j++ )
		{
			dDistanceSquare = 0.0;
			for( k=0; k<m_nCol; k++ )
			{
				TDColAry * pData = m_DataMatrix.at(k);
				dDistanceSquare += ( pData->GetAt(pInitialCenter[i]).dValue - pData->GetAt(pInitialCenter[j]).dValue ) *
					( pData->GetAt(pInitialCenter[i]).dValue - pData->GetAt(pInitialCenter[j]).dValue );

			}
			dCenterDistance[i*m_nNC+j] = dDistanceSquare;
			if( i==0&&j==1 )
			{
				dMinofCenter = dDistanceSquare;
				iClosest1 = i;
				iClosest2 = j;
			}
			else if( dMinofCenter>dDistanceSquare )
			{
				dMinofCenter = dDistanceSquare;
				iClosest1 = i;
				iClosest2 = j;
			}
		}
	}
	//*********************界面控制*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("构造初始聚类中心....");	
//	}
	// 遍历所有数据，替换构造初始聚类中心
	double * dXM = new double[m_nNC];
	for( i=0; i<m_nRow; i++ )
	{
		
//		if (m_pShow && i %1000==0)
//		{
//			m_pShow->strShowName.Format("处理聚类中心:%d....",i);	
//		}
		if(m_IntCluster.GetAt(i)>-1 )
			continue;
		if( m_pWeightCases && m_pWeightCases[i]==0 )
			continue;
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);
			if( pData->GetAt(i).iIndex<0 )
				break;
		}
		if( k<m_nCol )
			continue;

		double dMinXM1, dMinXM2;
		int nMinXM1, nMinXM2;

		// 求i到原始中心最近和次近距离
		for( j=0; j<m_nNC; j++ )
		{
			dDistanceSquare = 0.0;
			for( k=0; k<m_nCol; k++ )
			{
				TDColAry * pData = m_DataMatrix.at(k);
				double iValue = pData->GetAt(i).dValue;
				double centerValue = pData->GetAt(pInitialCenter[j]).dValue;
				dDistanceSquare += ( iValue - centerValue) * 
									( iValue - centerValue);
			}
			dXM[j] = dDistanceSquare;

			if( j==0 )
			{
				dMinXM1 = dDistanceSquare;
				nMinXM1 = j;
			}
			else if( j==1 )
			{
				if( dMinXM1>dDistanceSquare )
				{
					dMinXM2 = dMinXM1;
					nMinXM2 = nMinXM1;
					dMinXM1 = dDistanceSquare;
					nMinXM1 = j;
				}
				else
				{
					dMinXM2 = dDistanceSquare;
					nMinXM2 = j;
				}
			}
			else if( dMinXM1>dDistanceSquare )
			{
				dMinXM2 = dMinXM1;
				nMinXM2 = nMinXM1;
				dMinXM1 = dDistanceSquare;
				nMinXM1 = j;
			}
			else if( dMinXM2>dDistanceSquare )
			{
				dMinXM2 = dDistanceSquare;
				nMinXM2 = j;
			}
		}

		// 比较并替换。对照SPSS文档Step1: a、b
		bool bReplace = false;
		int nReplaceNo;
		if( dMinXM1>dMinofCenter ) // 到各中心的最小距离大于中心点间的最小距离
		{
			if( dXM[iClosest1]>dXM[iClosest2] )
			{
				m_IntCluster.SetAt( pInitialCenter[iClosest2], -1);
				pInitialCenter[iClosest2] = i;
				m_IntCluster.SetAt( i, iClosest2);
				if( m_pWeightCases )
					m_pClusterWeight[iClosest2] = m_pWeightCases[i];

			}
			else
			{
				m_IntCluster.SetAt( pInitialCenter[iClosest1], -1);
				pInitialCenter[iClosest1] = i;
				m_IntCluster.SetAt( i, iClosest1);
				if( m_pWeightCases )
					m_pClusterWeight[iClosest1] = m_pWeightCases[i];
			}

			nReplaceNo = m_IntCluster.GetAt(i);
			bReplace = true;
		}
		else 
		{
			for( k=0; k<m_nNC; k++ )
			{
				// 到次近中心的距离大于最近中心到其他中心的最小距离
				if( k>nMinXM1 && dCenterDistance[nMinXM1*m_nNC+k]<dMinXM2 || 
					k<nMinXM1 && dCenterDistance[nMinXM1+m_nNC*k]<dMinXM2 )
				{
					m_IntCluster.SetAt( pInitialCenter[nMinXM1], -1);
					//m_pCluster[pInitialCenter[nMinXM1]] = -1;
					pInitialCenter[nMinXM1] = i;
					m_IntCluster.SetAt( i, nMinXM1);
					if( m_pWeightCases )
						m_pClusterWeight[nMinXM1] = m_pWeightCases[i];

					nReplaceNo = nMinXM1;
					bReplace = true;
					break;
				}
			}
		}

		// 如果替换了聚类中心，重新计算新中心与其他中心的距离，并找出最小值
		if( bReplace )
		{
			for( l=0; l<m_nNC; l++ )
			{
				if( l==nReplaceNo )
					continue;

				dDistanceSquare = 0.0;
				for( k=0; k<m_nCol; k++ )
				{
					TDColAry * pData = m_DataMatrix.at(k);
					double RepValue = pData->GetAt(pInitialCenter[nReplaceNo]).dValue;
					double centerValue = pData->GetAt(pInitialCenter[l]).dValue ;

					dDistanceSquare += ( RepValue - centerValue ) * 
										( RepValue- centerValue );
				}
				if( l>nReplaceNo )
					dCenterDistance[nReplaceNo*m_nNC+l] = dDistanceSquare;
				else
					dCenterDistance[nReplaceNo+m_nNC*l] = dDistanceSquare;
			}

			for( l=0; l<m_nNC-1; l++ )
			{
				for( k=l+1; k<m_nNC; k++ )
				{
					if( l==0&&k==1 || dMinofCenter>dCenterDistance[l*m_nNC+k])
					{
						dMinofCenter = dCenterDistance[l*m_nNC+k];
						iClosest1 = l;
						iClosest2 = k;
					}
				}
			}
		}
	}

	delete[] dCenterDistance;
	delete[] dXM;
	//*********************界面控制*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("保存聚类中心数据....");	
//	}

	// 保存聚类中心数据
	for( i=0; i<m_nNC; i++ )
	{
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);
			m_pMNC[i*m_nCol+k] = pData->GetAt(pInitialCenter[i]).dValue;
		}
	}
	//*********************界面控制*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("创建结果....");	
//	}
	CreateInitResult();

	if( !UpdateInitialClusterCenter( pInitialCenter ) )
	{
		delete[] pInitialCenter;
		return false;
	}

	delete[] pInitialCenter;
////////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//		 将余下的样品归入最近的那一类,然后更新那一类的重心
//
//////////////////////////////////////////////////////////////////////////
bool CQuickCluster::UpdateInitialClusterCenter( int pInitialCenter[] )
{
	assert(m_pMNC);
	//遍历余下的样品
	int i=0;
	for(i=0;i<m_nRow;i++)
	{
		if( m_IntCluster.GetAt(i)>=0 ) 
			continue;
		if( m_pWeightCases && m_pWeightCases[i]==0 )
			continue;
//		if (m_pShow && i %1000==0)
//		{
//			m_pShow->strShowName.Format("归类:%d....",i);	
//		}

		int k=0;
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);
			if( pData->GetAt(i).iIndex<0 )
				break;
		}
		if( k<m_nCol )
			continue;

		double min;
		int    minIndex;
		int j=0;
		for( j=0; j<m_nNC; j++ )
		{
			double dDistanceSquare = 0.0;
			for( k=0; k<m_nCol; k++ )
			{
				TDColAry * pData = m_DataMatrix.at(k);

				dDistanceSquare += ( pData->GetAt(i).dValue - m_pMNC[j*m_nCol+k] ) * 
									( pData->GetAt(i).dValue - m_pMNC[j*m_nCol+k] );
			}

			if( j==0 || dDistanceSquare<min )
			{
				min = dDistanceSquare;
				minIndex = j;
			}
		}

		m_IntCluster.SetAt( i, minIndex);
		//m_pCluster[i] = minIndex;
		m_pCount[minIndex]  += 1;
		if( m_pWeightCases )
			m_pClusterWeight[minIndex] += m_pWeightCases[i];

	}
	//更新重心
	if( !UpdateMean(&m_IntCluster) )
		return false;	
	memcpy( m_pPrevMNC, m_pMNC, sizeof(double)*m_nNC*m_nCol );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//							将所有样品从头到尾处理一次
//
//////////////////////////////////////////////////////////////////////////////////////
bool CQuickCluster::AssignCasesToNearestCluster()
{

	int i=0;
	for (i=0;i<m_nRow ;i++)
		m_IntTempCluster.SetAt(i,-1);
	
	//遍历所有样品
	for( i=0;i<m_nRow;i++)
	{
		if( m_pWeightCases && m_pWeightCases[i]==0 )
			continue;
		int k=0; 
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);
			if( pData->GetAt(i).iIndex<0 )
				break;
		}
		if( k<m_nCol )
			continue;

		//寻找距离最小的分类
		double min = 1e100;
//		double distance(0);
		int    minIndex(0);
		int j=0; 
		for( j=0; j<m_nNC; j++ )
		{
			double dDistanceSquare = 0.0;
			for( k=0; k<m_nCol; k++ )
			{
				TDColAry * pData = m_DataMatrix.at(k);

				dDistanceSquare += ( pData->GetAt(i).dValue - m_pMNC[j*m_nCol+k] ) * 
									( pData->GetAt(i).dValue - m_pMNC[j*m_nCol+k] );
			}

			if( j==0 || dDistanceSquare<min )
			{
				min = dDistanceSquare;
				minIndex = j;
			}
		}
		
		m_IntTempCluster.SetAt(i,minIndex);
		//m_pTempCluster[i] = minIndex;

		if( m_IntTempCluster.GetAt(i) == m_IntCluster.GetAt(i) )
			continue;


		m_pCount[minIndex] += 1;
		m_pCount[m_IntCluster.GetAt(i)] -= 1;
		if( m_pWeightCases )
		{
			m_pClusterWeight[minIndex] += m_pWeightCases[i];
			m_pClusterWeight[m_IntCluster.GetAt(i)] -= m_pWeightCases[i];
		}
	}

	//更新重心
	UpdateMean( &m_IntTempCluster );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//				迭代过程,直到次数到达指定次数，或者聚类的结果不在改变
//
////////////////////////////////////////////////////////////////////////////////////////////
void CQuickCluster::Iteration()
{
	int ncount(0);
	while( (ncount==0) || (IsClusterChanged()) && ((m_bCountFlag&&ncount<m_nIterationNum)||(!m_bCountFlag)) )
	{
		if( ncount>0 )
		{
			CopyClusterIndex(&m_IntCluster,&m_IntTempCluster,m_nRow);
			memcpy( m_pPrevMNC, m_pMNC, sizeof(double)*m_nNC*m_nCol );
		}
		AssignCasesToNearestCluster();	
		ncount++;
	}
}

bool CQuickCluster::IsClusterChanged()
{
	for( int i=0; i<m_nNC; i++ )
	{
		for( int k=0; k<m_nCol; k++ )
		{
			if( fabs(m_pMNC[i*m_nCol+k]-m_pPrevMNC[i*m_nCol+k])>1e-20 )
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
//
//			比较两个聚类结果是否相同，相同返回true,不相同返回false
//
//////////////////////////////////////////////////////////////////////////////////
bool CQuickCluster::CompareCluster(int *x, int *y,int nLen)
{
	assert( x && y );
	if( memcmp(x,y,sizeof(int)*nLen) == 0 )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////////
//
//			拷贝聚类的结果，x ： 目的地， y：源地点，nLen :  长度
//
/////////////////////////////////////////////////////////////////////////////////
void CQuickCluster::CopyClusterIndex(TIntAry *x, TIntAry *y, int nLen)
{
	assert( x&&y );
	for (int i=0;i<nLen;i++)
	{
		x->SetAt(i,y->GetAt(i));
	}
	//memcpy(x,y,sizeof(int)*nLen);
}

/////////////////////////////////////////////////////////
//
//				更新类中心，
//				pCluster：
//
/////////////////////////////////////////////////////////

bool CQuickCluster::UpdateMean(TIntAry* pCluster)
{
	assert(pCluster);
	//清空
	memset(m_pMNC,0,sizeof(double)*m_nCol*m_nNC);
	for(int i=0;i<m_nNC;i++)
	{
		//统计该分类中的元素个数,求和
		double dWeight=1, dWeightSum=0;
		for(int j=0;j<m_nRow;j++)
		{
//			if (m_pShow && j %1000==0)
//			{
//				m_pShow->strShowName.Format("更新%d类中心:%d....",i,j);	
//			}
			if( pCluster->GetAt(j)  == i )
			{
				if( m_pWeightCases )
					dWeight = m_pWeightCases[j];
				for(int k=0;k<m_nCol;k++)
				{
					TDColAry * pData = m_DataMatrix.at(k);
					m_pMNC[i*m_nCol+k] += pData->GetAt(j).dValue * dWeight;
				}
				dWeightSum += dWeight;
			}
		}

		//求重心
		for(int k=0;k<m_nCol;k++)
		{
			if( dWeightSum>0 )
			{
				m_pMNC[i*m_nCol+k] /= dWeightSum;
			}
			else
			{
				return false;
			}
		}
	}
	//WriteData(m_pMNC,m_nNC,m_nCol,"重心");
	return true;
}
bool ConnectThreadSum(void *pDlga,CShowData*pDialogData)
{
	//**************
	CShowData*pSD = (CShowData*)pDialogData;
	pSD->bIsUse = true;
	pSD->dwStyle = 1;
	//**************
	CQuickCluster*pDlg = (CQuickCluster*)pDlga;
	pDlg ->m_pShow = pDialogData;
	return  pDlg->Do();
}

bool CQuickCluster::GetValueFromVO(CDataInterface *pDataInterface, std::string VO)
{
//	QUICKCLUSTER
//		/DATA=[FILE]
//		/VARLIST=[varlist]
//		/CRITERIA=[CLUSTER({2**})] [MAX[{10**}]]
//		/RESULT [CLASSIFY]
	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;
	
	m_nIterationNum = 10;
	if (AnaSyntax::findstr("/CRITERIA","MAX",VOstr))
		m_nIterationNum = int(AnaSyntax::getvalue("/CRITERIA","MAX",VOstr));
	m_nNC = 2;
	if (AnaSyntax::findstr("/CRITERIA","CLUSTER",VOstr))
		m_nNC = int(AnaSyntax::getvalue("/CRITERIA","CLUSTER",VOstr));
	m_bOutputCaseInfo = AnaSyntax::findstr("/RESULT","CLASSIFY",VOstr);
}

CResult * CQuickCluster::OnQuickCluster(CDataInterface *pDataInterface, std::string VO)
{
	m_pDataInterface = pDataInterface;
	m_pResult = NULL;
	m_pResult = new CResult("快速聚类");
	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	m_nRow = pDataInterface->GetRowCount();
	m_nCol = m_tVarInfo.iCount;
	Do();
	return m_pResult;
}
void CQuickCluster::CreateResult()
{
	if( m_pMNC==NULL ) // 无合法数据输入提示
	{
		CTString	strWarning;
		strWarning.Format( " 注：有效数据太少，无法执行快速聚类分析！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return;
	}

	if( m_pWeightCases && m_nValidWeightCases < m_nRow )
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：有一个以上的样本数据因权重为零、负数或缺失而删除！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->InsetAt( 0, pWarningTextRslt );
	}

	CTLTable * pTable = new CTLTable;
	CRsltVariable * pETable= new CRsltElementTable("最终聚类中心",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	pTable->SetTitle("最终聚类中心");
	pTable->InsertColumn(0,"");

	int i=0;
	for(i=0;i<m_nNC;i++)
	{
		CTString str;
		str.Format("类%d",i+1);
		pTable->InsertColumn(1,str);
	}
	m_CenterOFCluster.create(m_nCol, m_nNC);
	for(i=0;i<m_nCol;i++)
	{
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		for(int j=0;j<m_nNC;j++)
		{
			pTable->SetItemText(i,j+1,m_pMNC[j*m_nCol+i]);
			m_CenterOFCluster(i, j) = m_pMNC[j*m_nCol+i];
		}
	}

	m_DistanceToshortestCluster.create(m_nNC);//两两中心点之间得最短距离
	m_DisPointToCenter.create(m_nNC, 0.0);
	for (i=0; i<m_nNC; i++)
	{
		CDoubleVector DistanceTempVec(m_nNC, 1.0);
		for (int j=0; j<m_nNC; j++)
		{
			for (int k=0; k<m_nCol; k++)
			{
				DistanceTempVec(j) += pow(m_pMNC[i*m_nCol+k] - m_pMNC[j*m_nCol+k], 2);
			}
			DistanceTempVec(j) = sqrt(DistanceTempVec(j));
		}
		double dMax=0, dMin=0;
		DistanceTempVec.MaxMin(dMax, dMin);
		DistanceTempVec(i) = dMax;//DistanceTempVec.Max();
		m_DistanceToshortestCluster(i) = dMin;//DistanceTempVec.Min();
		assert(m_DistanceToshortestCluster(i)>0);
	}
	
	double dWeightSum = 0.0, dValidWeightSum = 0.0;
	int nValidCases = 0;

	CTString strFieldName;//在主界面中添加种类：始
	CField* pField_cluster;
	CField* pField_dis;
	if( m_bOutputCaseInfo )//在主界面添加输出类别
	{
		strFieldName = "聚类结果_类别";
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}		
		pField_cluster=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fInt);
		pField_cluster->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField_cluster);
		strFieldName = "聚类结果_距离";
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}
		pField_dis = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		pField_dis->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField_dis);
		
		m_pDataInterface->m_DataAccess.First();
	}	
	for(i=0;i<m_nRow;i++)
	{
		if( m_pWeightCases )
		{
			if( m_pWeightCases[i]==0 )
			{
				if (m_bOutputCaseInfo)
				{
					pField_cluster->SetNull();
					pField_dis->SetNull();
					m_pDataInterface->m_DataAccess.Next();
				}
				continue;
			}
			else
			{
				dWeightSum += m_pWeightCases[i];
			}
		}
		
		if( m_IntCluster.GetAt(i)>=0 )
		{
			nValidCases++;
			if( m_pWeightCases )
				dValidWeightSum += m_pWeightCases[i];
			double dDistance, dDistanceSquare = 0;
			for( int j=0; j<m_nCol; j++ )
			{	
				TDColAry * pData = m_DataMatrix.at(j);
				dDistanceSquare += ( pData->GetAt(i).dValue - m_pMNC[m_IntCluster.GetAt(i)*m_nCol+j] ) *
					( pData->GetAt(i).dValue - m_pMNC[m_IntCluster.GetAt(i)*m_nCol+j] );
			}
			dDistance = sqrt( dDistanceSquare );
			m_DisPointToCenter(m_IntCluster.GetAt(i)) += dDistance;
			if (m_bOutputCaseInfo) 
			{
				m_pDataInterface->m_DataAccess.Edit();			
				pField_cluster->SetAsInteger(m_IntCluster.GetAt(i)+1);
				pField_dis->SetAsDouble(dDistance);
				//m_pDataInterface->m_DataAccess.Post();	
			}
		}
		else
		{
			if (m_bOutputCaseInfo) //对应的数值-填空.
			{
				m_pDataInterface->m_DataAccess.Edit();
				pField_cluster->SetNull();
				pField_dis->SetNull();
			}
		}
		if (m_bOutputCaseInfo)
		{		
			m_pDataInterface->m_DataAccess.Next();
		}
	}
	if (m_bOutputCaseInfo)
	{
		CTString szNewFile("");
		CTString szNowFile = m_pDataInterface->m_DataAccess.GetFileName();
		bool bOld = m_pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
		//真时:为NewFile
		//假时:为NowFile.
		CTString SaveFileName;
		SaveFileName.Format("快速聚类样本聚类信息保存的文件名为：%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
		CRsltElementText * OutputInforStr= new CRsltElementText("各聚类样本预测输出信息");//申请一个表对象类的对象
		OutputInforStr->AddString(SaveFileName);
		m_pResult->Add(OutputInforStr);
	}//在主界面中添加种类：止

	pTable = new CTLTable;
	pETable= new CRsltElementTable("各聚类样本数",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	pTable->SetTitle("各聚类样本数");

	pTable->InsertColumn(0,"聚类");
	bool bWithWight = false;
	if (m_pWeightCases)
		bWithWight = true;
	if( m_pWeightCases )
	{
		pTable->InsertColumn(1,"未加权样本数");
		pTable->InsertColumn(2,"加权样本数");
	}
	else
		pTable->InsertColumn(1,"样本数");
	pTable->InsertColumn(2+bWithWight, "有效样本数量占比");

	for( i=0; i<m_nNC; i++ )
	{
		CTString str;
		str.Format("聚类%d",i+1);
		pTable->InsertItem(i, str);  

		pTable->SetItemText( i, 1, m_pCount[i] );
		if( m_pWeightCases )
			pTable->SetItemText( i, 2, m_pClusterWeight[i] );
		pTable->SetItemText(i, 2+bWithWight, m_pCount[i]*1.0/nValidCases);
	}
	CDoubleVector	ModelFit_vec(m_nNC);
	for (i=0; i<m_nNC; i++)
		ModelFit_vec(i) = m_DisPointToCenter(i) / m_pCount[i] / m_DistanceToshortestCluster(i);

	pTable->InsertItem( m_nNC, "有效样本" );
	pTable->SetItemText( m_nNC, 1, nValidCases );
	if( m_pWeightCases )
		pTable->SetItemText( m_nNC, 2, dValidWeightSum );

	pTable->InsertItem( m_nNC+1, "缺失样本" );
	if( m_pWeightCases )
	{
		pTable->SetItemText( m_nNC+1, 1, m_nValidWeightCases - nValidCases );
		pTable->SetItemText( m_nNC+1, 2, dWeightSum - dValidWeightSum );
	}
	else
		pTable->SetItemText( m_nNC+1, 1, m_nRow - nValidCases );

	pTable = new CTLTable;
	pETable= new CRsltElementTable("聚类有效性",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	pTable->SetTitle("聚类有效性");

	pTable->InsertColumn(0, "聚类");
	pTable->InsertColumn(1, "有效性");
	for( i=0; i<m_nNC; i++ )
	{
		CTString str;
		str.Format("聚类%d",i+1);
		pTable->InsertItem( i, str );  
		pTable->SetItemText( i, 1, ModelFit_vec(i));
	}
	pTable->InsertItem(m_nNC, "总和");
	pTable->SetItemText(m_nNC, 1, ModelFit_vec.sum());	
}

void CQuickCluster::CreateInitResult()
{
	CTLTable * pTable = new CTLTable;
	CRsltVariable * pETable= new CRsltElementTable("初始聚类中心",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	pTable->SetTitle("初始聚类中心");
	pTable->InsertColumn(0,"");
	int i=0;
	for(i=0;i<m_nNC;i++)
	{
		CTString str;
		str.Format("类%d",i+1);
		pTable->InsertColumn(1,str);
	}
	for(i=0;i<m_nCol;i++)
	{
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		for(int j=0;j<m_nNC;j++)
		{
			pTable->SetItemText(i,j+1,m_pMNC[j*m_nCol+i]);
		}
	}
}

void CQuickCluster::GetData(CDataInterface *pDataInterface, TCDataMatrix &rArray, int nGetType )
{
	// 从数据库读取用户选定的所有字段数据
	if( m_tVarInfo.iCount>0 )
	{
		//TDataSet * pDataArray = NULL;
		CDWordArray indexary;
		for( int i=0; i<m_tVarInfo.iCount; i++ )
		{			
			indexary.Add(m_tVarInfo.pValue[i]);
		}
		pDataInterface->GetColsData( indexary, rArray, nGetType );//取回数据

	}	
}

bool CQuickCluster::Do()
{
	GetData( m_pDataInterface, m_DataMatrix, 5 );
	if (m_nNC >5&& m_nNC<200)//大于5个聚类中心时
	{//根据聚类中心个数,自动调整页面数
		for (int i=0;i<m_DataMatrix.size();i++)
		{
			TDColAry * pData = m_DataMatrix.at(i);
			pData->SetMaxPages(m_nNC+1);
		}
	}
	// 读取权重值
	int nWeightNo = -1;
	bool bContinue = true;
	
	if( (nWeightNo=m_pDataInterface->GetWeightNumb())!=-1 )
	{
		m_pWeightCases = new double[m_nRow];
		m_nValidWeightCases = m_pDataInterface->GetWeightData( m_pWeightCases );
		if( m_nValidWeightCases<1 )
		{
			delete m_pWeightCases;
			m_pWeightCases = NULL;
			bContinue = false;
		}
	}
	
	if( bContinue )
	{
		m_pMNC         = new double [ m_nNC * m_nCol ];
		m_pPrevMNC	   = new double [ m_nNC * m_nCol ];
		m_IntCluster.SetSize(m_nRow);
		m_IntTempCluster.SetSize(m_nRow);
		if (m_nNC >5 && m_nNC<200)
		{//根据聚类中心个数,自动调整页面数
			m_IntCluster.SetMaxPages(m_nNC+1);
			m_IntTempCluster.SetMaxPages(m_nNC+1);
		}
		m_pCount       = new int [m_nNC];
		if( m_pWeightCases )
			m_pClusterWeight = new double[m_nNC];
		int i=0;
		for (i=0;i<m_nRow ;i++)
			m_IntCluster.SetAt(i,-1);
		
		if( m_nIterationNum <= 0)
			m_bCountFlag = false;
		else
		{
			m_bCountFlag = true;		
		}
		
		if( InitalClusterCenter() )
		{
			Iteration();
		}
		else
		{
			delete[] m_pMNC;
			m_pMNC = NULL;
		}
	}
	
	CreateResult();
	Clear();
	return true;
}
