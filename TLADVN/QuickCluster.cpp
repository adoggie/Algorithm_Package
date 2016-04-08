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
//					�ͷ���ʱ�ռ�
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
//					        ��������֮�����С�ľ���
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
//							����������֮��ľ���
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
//							ָ�������������
/////////////////////////////////////////////////////////////////////////
bool CQuickCluster::InitalClusterCenter()
{
	int i, j, k, l;
	int * pInitialCenter = new int[m_nNC];
	
	// ��ʼ��ǰ������Ч����Ϊ��������
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
	if( j==m_nRow && i<m_nNC )// ��Ч���ݲ��㣬���ش���
	{
		delete[] pInitialCenter;
		return false;
	}

	double dMaxDistance = 0.0, dDistanceSquare, dMinofCenter;
	int iClosest1, iClosest2;
	double * dCenterDistance = new double[m_nNC*m_nNC];
	
	// ����������ļ���໥���룬����¼������Сֵ
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
	//*********************�������*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("�����ʼ��������....");	
//	}
	// �����������ݣ��滻�����ʼ��������
	double * dXM = new double[m_nNC];
	for( i=0; i<m_nRow; i++ )
	{
		
//		if (m_pShow && i %1000==0)
//		{
//			m_pShow->strShowName.Format("�����������:%d....",i);	
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

		// ��i��ԭʼ��������ʹν�����
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

		// �Ƚϲ��滻������SPSS�ĵ�Step1: a��b
		bool bReplace = false;
		int nReplaceNo;
		if( dMinXM1>dMinofCenter ) // �������ĵ���С����������ĵ�����С����
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
				// ���ν����ĵľ������������ĵ��������ĵ���С����
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

		// ����滻�˾������ģ����¼������������������ĵľ��룬���ҳ���Сֵ
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
	//*********************�������*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("���������������....");	
//	}

	// ���������������
	for( i=0; i<m_nNC; i++ )
	{
		for( k=0; k<m_nCol; k++ )
		{
			TDColAry * pData = m_DataMatrix.at(k);
			m_pMNC[i*m_nCol+k] = pData->GetAt(pInitialCenter[i]).dValue;
		}
	}
	//*********************�������*****************************
//	if (m_pShow)
//	{
//		m_pShow->strShowName.Format("�������....");	
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
//		 �����µ���Ʒ�����������һ��,Ȼ�������һ�������
//
//////////////////////////////////////////////////////////////////////////
bool CQuickCluster::UpdateInitialClusterCenter( int pInitialCenter[] )
{
	assert(m_pMNC);
	//�������µ���Ʒ
	int i=0;
	for(i=0;i<m_nRow;i++)
	{
		if( m_IntCluster.GetAt(i)>=0 ) 
			continue;
		if( m_pWeightCases && m_pWeightCases[i]==0 )
			continue;
//		if (m_pShow && i %1000==0)
//		{
//			m_pShow->strShowName.Format("����:%d....",i);	
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
	//��������
	if( !UpdateMean(&m_IntCluster) )
		return false;	
	memcpy( m_pPrevMNC, m_pMNC, sizeof(double)*m_nNC*m_nCol );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//
//							��������Ʒ��ͷ��β����һ��
//
//////////////////////////////////////////////////////////////////////////////////////
bool CQuickCluster::AssignCasesToNearestCluster()
{

	int i=0;
	for (i=0;i<m_nRow ;i++)
		m_IntTempCluster.SetAt(i,-1);
	
	//����������Ʒ
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

		//Ѱ�Ҿ�����С�ķ���
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

	//��������
	UpdateMean( &m_IntTempCluster );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//				��������,ֱ����������ָ�����������߾���Ľ�����ڸı�
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
//			�Ƚ������������Ƿ���ͬ����ͬ����true,����ͬ����false
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
//			��������Ľ����x �� Ŀ�ĵأ� y������Դ�ص㣬nLen :  ����
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
//				���������ģ�
//				pCluster��
//
/////////////////////////////////////////////////////////

bool CQuickCluster::UpdateMean(TIntAry* pCluster)
{
	assert(pCluster);
	//���
	memset(m_pMNC,0,sizeof(double)*m_nCol*m_nNC);
	for(int i=0;i<m_nNC;i++)
	{
		//ͳ�Ƹ÷����е�Ԫ�ظ���,���
		double dWeight=1, dWeightSum=0;
		for(int j=0;j<m_nRow;j++)
		{
//			if (m_pShow && j %1000==0)
//			{
//				m_pShow->strShowName.Format("����%d������:%d....",i,j);	
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

		//������
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
	//WriteData(m_pMNC,m_nNC,m_nCol,"����");
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
	m_pResult = new CResult("���پ���");
	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
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
	if( m_pMNC==NULL ) // �޺Ϸ�����������ʾ
	{
		CTString	strWarning;
		strWarning.Format( " ע����Ч����̫�٣��޷�ִ�п��پ��������" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return;
	}

	if( m_pWeightCases && m_nValidWeightCases < m_nRow )
	{
		CTString	strWarning;
		strWarning.Format( " Ȩ�ش�����һ�����ϵ�����������Ȩ��Ϊ�㡢������ȱʧ��ɾ����" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "Ȩ�ش���" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->InsetAt( 0, pWarningTextRslt );
	}

	CTLTable * pTable = new CTLTable;
	CRsltVariable * pETable= new CRsltElementTable("���վ�������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	pTable->SetTitle("���վ�������");
	pTable->InsertColumn(0,"");

	int i=0;
	for(i=0;i<m_nNC;i++)
	{
		CTString str;
		str.Format("��%d",i+1);
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

	m_DistanceToshortestCluster.create(m_nNC);//�������ĵ�֮�����̾���
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

	CTString strFieldName;//����������������ࣺʼ
	CField* pField_cluster;
	CField* pField_dis;
	if( m_bOutputCaseInfo )//�����������������
	{
		strFieldName = "������_���";
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}		
		pField_cluster=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fInt);
		pField_cluster->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField_cluster);
		strFieldName = "������_����";
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
			if (m_bOutputCaseInfo) //��Ӧ����ֵ-���.
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
		//��ʱ:ΪNewFile
		//��ʱ:ΪNowFile.
		CTString SaveFileName;
		SaveFileName.Format("���پ�������������Ϣ������ļ���Ϊ��%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
		CRsltElementText * OutputInforStr= new CRsltElementText("����������Ԥ�������Ϣ");//����һ���������Ķ���
		OutputInforStr->AddString(SaveFileName);
		m_pResult->Add(OutputInforStr);
	}//����������������ֹࣺ

	pTable = new CTLTable;
	pETable= new CRsltElementTable("������������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	pTable->SetTitle("������������");

	pTable->InsertColumn(0,"����");
	bool bWithWight = false;
	if (m_pWeightCases)
		bWithWight = true;
	if( m_pWeightCases )
	{
		pTable->InsertColumn(1,"δ��Ȩ������");
		pTable->InsertColumn(2,"��Ȩ������");
	}
	else
		pTable->InsertColumn(1,"������");
	pTable->InsertColumn(2+bWithWight, "��Ч��������ռ��");

	for( i=0; i<m_nNC; i++ )
	{
		CTString str;
		str.Format("����%d",i+1);
		pTable->InsertItem(i, str);  

		pTable->SetItemText( i, 1, m_pCount[i] );
		if( m_pWeightCases )
			pTable->SetItemText( i, 2, m_pClusterWeight[i] );
		pTable->SetItemText(i, 2+bWithWight, m_pCount[i]*1.0/nValidCases);
	}
	CDoubleVector	ModelFit_vec(m_nNC);
	for (i=0; i<m_nNC; i++)
		ModelFit_vec(i) = m_DisPointToCenter(i) / m_pCount[i] / m_DistanceToshortestCluster(i);

	pTable->InsertItem( m_nNC, "��Ч����" );
	pTable->SetItemText( m_nNC, 1, nValidCases );
	if( m_pWeightCases )
		pTable->SetItemText( m_nNC, 2, dValidWeightSum );

	pTable->InsertItem( m_nNC+1, "ȱʧ����" );
	if( m_pWeightCases )
	{
		pTable->SetItemText( m_nNC+1, 1, m_nValidWeightCases - nValidCases );
		pTable->SetItemText( m_nNC+1, 2, dWeightSum - dValidWeightSum );
	}
	else
		pTable->SetItemText( m_nNC+1, 1, m_nRow - nValidCases );

	pTable = new CTLTable;
	pETable= new CRsltElementTable("������Ч��",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	pTable->SetTitle("������Ч��");

	pTable->InsertColumn(0, "����");
	pTable->InsertColumn(1, "��Ч��");
	for( i=0; i<m_nNC; i++ )
	{
		CTString str;
		str.Format("����%d",i+1);
		pTable->InsertItem( i, str );  
		pTable->SetItemText( i, 1, ModelFit_vec(i));
	}
	pTable->InsertItem(m_nNC, "�ܺ�");
	pTable->SetItemText(m_nNC, 1, ModelFit_vec.sum());	
}

void CQuickCluster::CreateInitResult()
{
	CTLTable * pTable = new CTLTable;
	CRsltVariable * pETable= new CRsltElementTable("��ʼ��������",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);
	pTable->SetTitle("��ʼ��������");
	pTable->InsertColumn(0,"");
	int i=0;
	for(i=0;i<m_nNC;i++)
	{
		CTString str;
		str.Format("��%d",i+1);
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
	// �����ݿ��ȡ�û�ѡ���������ֶ�����
	if( m_tVarInfo.iCount>0 )
	{
		//TDataSet * pDataArray = NULL;
		CDWordArray indexary;
		for( int i=0; i<m_tVarInfo.iCount; i++ )
		{			
			indexary.Add(m_tVarInfo.pValue[i]);
		}
		pDataInterface->GetColsData( indexary, rArray, nGetType );//ȡ������

	}	
}

bool CQuickCluster::Do()
{
	GetData( m_pDataInterface, m_DataMatrix, 5 );
	if (m_nNC >5&& m_nNC<200)//����5����������ʱ
	{//���ݾ������ĸ���,�Զ�����ҳ����
		for (int i=0;i<m_DataMatrix.size();i++)
		{
			TDColAry * pData = m_DataMatrix.at(i);
			pData->SetMaxPages(m_nNC+1);
		}
	}
	// ��ȡȨ��ֵ
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
		{//���ݾ������ĸ���,�Զ�����ҳ����
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
