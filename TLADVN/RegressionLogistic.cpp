// RegressionLogistic.cpp: implementation of the CRegressionLogistic class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RegressionLogistic.h"
#include "../DataInterface/TCDataArray.h"
#include "../DataInterface/Define.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"
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

CRegressionLogistic::CRegressionLogistic()
{
	m_nDependIndex  = -1;
	m_pDataInterface= NULL;
}

CRegressionLogistic::~CRegressionLogistic()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
	if( m_tVarCategrery.iCount>0 )
		m_tVarCategrery.DeleteObject();
}

bool CRegressionLogistic::GetValueFromVO(CDataInterface *pDataInterface, string VO)
{
//	REGLOGISTIC
//		/DATA=[FILE]
//		/DEPENDENT [varlist]
//		/COVARIATE [varlist]
//		/INTERACT [paired vars list]
//		/LAYER [varlist]
//		/SAVEMODEL [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);

	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return false;
	strDependName = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(strDependName);
	ynames.RemoveAll();

	CTStringArray xname;
	AnaSyntax::getVAR("/COVARIATE", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;
	xname.RemoveAll();

	TIntStruct pairID;

	AnaSyntax::getVAR("/INTERACT", VOstr, xname);
	int pairNum = xname.GetSize()/2;
	if (!(pDataInterface->getVarID(xname,pairID)) && xname.GetSize()!=pairNum*2)
		return false;
	xname.RemoveAll();
	CTString strTemp;
	CTString strNum;
	for (int i=0; i<pairNum; i++)
	{
		strTemp.RemoveAll();
		strNum.Format("%i", pairID.GetIDAt(2*i));
		strTemp = strNum + "*";
		strNum.Format("%i", pairID.GetIDAt(2*i+1));
		strTemp += strNum;
		m_strInteractiveArray.Add(strTemp);
	}
	
	if (AnaSyntax::exist("/LAYER", VOstr))
	{
		AnaSyntax::getVAR("/LAYER", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarCategrery)))
			return false;
	}	

	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL", VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL", VOstr);
	
	return true;
}

CResult * CRegressionLogistic::OnLogistics(CDataInterface *pDataInterface, string VO)
{
	briefrTable=NULL;
	InitParma();
	m_pResult = NULL;
	m_pResult = new CResult("��ֵ�߼��ع�");
	if (!GetValueFromVO(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	m_pDataInterface = pDataInterface;
	int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{					
		briefrTable=new CTLTable();
		briefrTable->InsertColumn(0,"���˵��");
	}
	CDoubleVector pWeight;
	int nCaseNum = 0;
	CDoubleMatrix matrix = GetDataForLogistics( pDataInterface, &m_strInteractiveArray, pWeight, nCaseNum );
	if( matrix.mcol()<1 )
		return m_pResult;

	if( !m_strMissField.IsEmpty() )
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( "ע�� ���� " + m_strMissField + " Ϊ���������μӻع�������㡣" );
		m_pResult->Add( pWarningTextRslt );
	}

	if( matrix.mrow()<3 )
	{
		CTString strWarning = "ע�� ��Ч������̫�٣��޷����лع������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	if(!Logistics(matrix,pWeight,m_pResult))
	{
		CTString strWarning = "ע�� ��ѡ������ݲ��ʺ�����ֵ�߼��ع���������μ���û�н����";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}
	
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (m_pResult->Count()<=0)
				m_pResult->Add(new CRsltElementTable("���˵��",briefrTable));
			else
				m_pResult->InsetAt(0,new CRsltElementTable("���˵��",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;
	//ģ�ͱ���
	Export();//add by xds 20060817.
	return m_pResult;
}

CDoubleMatrix CRegressionLogistic::GetDataForLogistics( CDataInterface * pDataInterface, CTStringArray * pStrArray, CDoubleVector& Weight, int& nCaseNum )
{
	CDoubleMatrix matrix;
	int i, j, k;
	CIntVector iIndex(pDataInterface->GetColCount(),-1); // �����������
	CTArray<int, int> arInteractive;		// ���潻�������б����
	m_strArrayFieldName.RemoveAll();
	bool bDataError = false;

	//for( i=0; i<pDataInterface->GetColCount(); i++ )  
	//		iIndex[i] = -1;
//////////////////////////////////////////////////////////////////////////
	
	// �����ݿ��ȡ����
	TCDataMatrix  ColArray;
	TDColAry * tColData ;
	int nColCount = 0;
	for( i=0;i<m_tVarInfo.iCount;i++ )
	{
		int nColNum = m_tVarInfo.pValue[i];
        tColData = new TDColAry;
		pDataInterface->GetColData( nColNum, *tColData, 5 );
		iIndex[nColNum] = nColCount++;
	//	rColArray.Add( pColData );
		ColArray.push_back(tColData);
	}

	//////////////////////////////////////////////////////////////////////////
	
	for(i =0; i<ColArray.size();i++ )
	{
		TDColAry* temp =  ColArray.at(i);
		//TRACE("%d : " ,i);
		for(int k =0; k<temp->GetLen();k++ )
			;//TRACE("%.2f ",temp->GetAt(k).dValue);
		//TRACE("\n");
	}

	//////////////////////////////////////////////////////////////////////////
	
	for( i=0; i<pStrArray->GetSize(); i++ )
	{
		CTString str = pStrArray->GetAt(i);
		CTString strField1 = str.Left( str.Find("*") );
		CTString strField2 = str.Right( str.GetLength() - str.Find("*") - 1 );
		int nt1 = atoi(strField1);	
		int nt2 = atoi(strField2);
		arInteractive.Add( nt1 );
		arInteractive.Add( nt2 );

		if( iIndex[nt1]<0 )
		{
			tColData = new TDColAry;
			pDataInterface->GetColData( nt1, *tColData, 5 );
			iIndex[nt1] = nColCount++;
			ColArray.push_back( tColData );
		}
		if( iIndex[nt2]<0 )
		{
			tColData = new TDColAry;
			pDataInterface->GetColData( nt2, *tColData, 5 );
			iIndex[nt2] = nColCount++;
			ColArray.push_back( tColData);
		}
	}
	tColData = new TDColAry;
	pDataInterface->GetColData( m_nDependIndex, *tColData, 5 );
	iIndex[m_nDependIndex] = nColCount++;
	ColArray.push_back(tColData );

	// ��ȡȨ��
	CDoubleVector Weight_Org(pDataInterface->GetRowCount(), 0.0);
	if( pDataInterface->GetWeightNumb()>=0 )
	{
		Weight_Org = GetWeightVector(pDataInterface);
	}
	else
	{
		for( i=0; i<pDataInterface->GetRowCount(); i++ )
			Weight_Org[i] = 1;
	}

	//����ȱʧֵ
	//����ʵ��������
	nCaseNum = 0;
	for( i=0; i<pDataInterface->GetRowCount(); i++ )
	{
		if( Weight_Org[i]==0 )
			continue;

		for( j=0; j<ColArray.size(); j++ )
		{
			TDColAry * pDataSet = ColArray.at(j);

			if( pDataSet->GetAt(i).iIndex<0 )
				break;
		}

		if( j<ColArray.size() )
			Weight_Org[i] = 0;
		else
			nCaseNum++;
	}

	if( nCaseNum==0 )
	{
		CTString strWarning = "ע�� ��Ч������Ϊ0���޷����лع������";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		bDataError = true;
	}

	if( !bDataError )
	{
		// ��������
		for( i=ColArray.size()-1; i>=0; i-- )
		{
			TDColAry * pDataSet =ColArray.at(i);
			bool bFirst=true, bEqual=true;
			double dFirst;
			for( j=0; j<pDataInterface->GetRowCount(); j++ )
			{
				if( Weight_Org[j]>0 )
				{
					if( bFirst )
					{
						dFirst = pDataSet->GetAt(j).dValue;
						bFirst = false;
					}
					else
					{
						if( pDataSet->GetAt(j).dValue!=dFirst )
						{
							bEqual = false;
							break;
						}
					}
				}
			}
			if( bEqual ) // ɾ��Ϊ��������
			{
				for( j=0; j<pDataInterface->GetColCount(); j++ )
				{
					if( iIndex[j]==i )
						break;
				}

				assert( j<pDataInterface->GetColCount() ); 
				
				if( !m_strMissField.IsEmpty())
					m_strMissField += ",";
   				m_strMissField += pDataInterface->GetFieldName(j);

				delete ColArray.at(i);
				ColArray.erase(ColArray.begin()+i);

				iIndex[j]=-1;
				for( k=0; k<pDataInterface->GetColCount(); k++ )
					if( iIndex[k]>i )
						iIndex[k]--;
			}
		}

		// ���������Ķ�ֵ��
		if( iIndex[m_nDependIndex]==-1 || !IsBinary(ColArray.at(iIndex[m_nDependIndex]), 
			Weight_Org, pDataInterface->GetRowCount()) )
		{
			CTString strWarning = "ע�� ���������Ϊ��ֵ���ݡ���������ݲ��������������ܽ��ж�ֵ�߼��ع������";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			bDataError = true;
		}

		if( !bDataError )
		{
			// �ӱ����б���ɾ����������
			// Э����
			for( i=m_tVarInfo.iCount-1; i>=0; i-- )
			{
				if( iIndex[m_tVarInfo.pValue[i]]==-1 )
					m_tVarInfo.RemoveAt(i);
			}
			// ��������
			for( i=pStrArray->GetSize()-1; i>=0; i-- )
			{
				if( iIndex[arInteractive[i*2]]==-1 || iIndex[arInteractive[i*2+1]]==-1 )
				{
					pStrArray->RemoveAt(i);
					arInteractive.RemoveAt(i*2+1);
					arInteractive.RemoveAt(i*2);
				}
			}
			// �ֲ����
			for( i=m_tVarCategrery.iCount-1; i>=0; i-- )
			{
				if( iIndex[m_tVarCategrery.pValue[i]]==-1 )
					m_tVarCategrery.RemoveAt(i);
			}

			// ������������
			CTArray< CDoubleVector *, CDoubleVector *> NewColArray;
			int nCount=0;
			
			for( i=0; i<ColArray.size(); i++ )
			{
				TDColAry* DataSet = ColArray.at(i);
				CDoubleVector *pNewData = new CDoubleVector(nCaseNum,0.0);
				nCount = 0;
				for( j=0; j<pDataInterface->GetRowCount(); j++ )
				{
					if( Weight_Org[j]>0 )
					{
						(*pNewData)(nCount++) = DataSet->GetAt(j).dValue;
					}
				}
				
				NewColArray.Add( pNewData );
			}
			Weight.create(nCaseNum,0.0);
			nCount = 0;
			for( j=0; j<pDataInterface->GetRowCount(); j++ )
			{
				if( Weight_Org[j]>0 )
					Weight[nCount++] = Weight_Org[j];
			}

			// ����ֲ����
			CTArray< CDoubleMatrix*, CDoubleMatrix* > arCategory;
			CRsltElementText * pWarningTextRslt=NULL;
			CDoubleMatrix * tempMatrix = NULL;
			for( i=0; i<m_tVarCategrery.iCount; i++ )
			{
			
				if( GetCategoryMatrix( *(NewColArray.GetAt(iIndex[m_tVarCategrery.pValue[i]])), tempMatrix, nCaseNum ))
					arCategory.Add( tempMatrix );
				else
				{
					for( int j=0; j<arCategory.GetSize(); j++ )
						delete arCategory.GetAt(j);
					arCategory.RemoveAll();

					CTString strWarning = "ע�⣺ �ֲ���Ŀ̫�ࡣû���㹻���ڴ�ռ���ж�ֵ�߼��ع������";
					pWarningTextRslt = new CRsltElementText( "ע��" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );

					bDataError = true;
				}
			}
			if( !bDataError )
			{
				// �����������
				GenerateMatrix( NewColArray, arCategory, arInteractive, iIndex, matrix, pDataInterface, nCaseNum );
			}

			// ɾ����ʱ����
			for( i=0; i<NewColArray.GetSize(); i++ )
			{
				 NewColArray[i]->destruction();
			}
			NewColArray.RemoveAll();

			for( i=0; i<arCategory.GetSize(); i++ )
			{
				delete arCategory.GetAt(i);
			}
			arCategory.RemoveAll();
		}			
	}

	for( i=0; i<ColArray.size(); i++ )
		delete ColArray.at(i);

	return matrix;
}



int CRegressionLogistic::GetCategoryNum( int nCol )
{
	int i;
	
	for( i=0; i<m_tVarCategrery.iCount; i++ )
		if( m_tVarCategrery.pValue[i]==nCol )
			return i;

	return -1;
}

///////////////////////////New One ?????????????????
void CRegressionLogistic::GenerateMatrix( CTArray< CDoubleVector *,CDoubleVector *>& NewColArry, CTArray< CDoubleMatrix*, CDoubleMatrix* >& arCategory, CTArray<int, int>& arInteractive, CIntVector iIndex, CDoubleMatrix& matrix, CDataInterface * pDataInterface, int nCaseNum )
{
	int i, j, k;
	// ����Э��������
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{
		int nColNum = m_tVarInfo.pValue[i];
		int nCategoryNum = GetCategoryNum( nColNum );

		if( nCategoryNum<0 )
		{
			CDoubleVector vectColData( *NewColArry[iIndex(nColNum)],0, nCaseNum-1 );
			matrix = matrix | vectColData;
			m_strArrayFieldName.Add( pDataInterface->GetFieldName(nColNum) );
		}
		else
		{
			CDoubleMatrix * matrix_temp = arCategory[nCategoryNum];
			for( k=0; k<matrix_temp->mcol(); k++ )
			{
				matrix = matrix | matrix_temp->col( k );
				CTString str = pDataInterface->GetFieldName( nColNum );
				CTString strItem;
				strItem.Format( "(%d)", k+1 );
				str += strItem;
				m_strArrayFieldName.Add( str );					
			}				
		}
	}
	// ���뽻�����þ���
	CDoubleVector vect1;
	CDoubleVector vect2;
	CDoubleMatrix * matrix_temp1 = NULL;
	CDoubleMatrix * matrix_temp2 = NULL;
	//CDoubleVector vect_new;
	for( i=0; i<arInteractive.GetSize()/2; i++ )
	{
		int nColNum1 = arInteractive[i*2];
		int nCategoryNum1 = GetCategoryNum( nColNum1 );
		int nColNum2 = arInteractive[i*2+1];
		int nCategoryNum2 = GetCategoryNum( nColNum2 );
		CTString strField1 = pDataInterface->GetFieldName( nColNum1 );
		CTString strField2 = pDataInterface->GetFieldName( nColNum2 );

		if( nCategoryNum1>=0 && nCategoryNum2>=0 )
		{
			matrix_temp1 = arCategory[nCategoryNum1];
			matrix_temp2 = arCategory[nCategoryNum2];

			for( j=0; j<matrix_temp1->mcol(); j++ )
			{	
				CTString str1;
				str1.Format( "(%d)", j+1 );
				CTString str2 = strField1;
				str2 += str1;
				vect1 = matrix_temp1->col(j);

				for( k=0; k<matrix_temp2->mcol(); k++)
				{
					CTString str11;
					str11.Format( "(%d)", k+1);
					CTString str22 = strField2;
					str22 += str11;

					vect2 = matrix_temp2->col(k);
					//vect_new = vect1 * vect2;

					matrix = matrix | (vect1 * vect2);		
					//vect_new.destroy();

					m_strArrayFieldName.Add( str2 + "*" + str22.GetData() );
				}
			}
		}
		else if ( nCategoryNum1>=0 && nCategoryNum2<0 )
		{
			matrix_temp1 = arCategory[nCategoryNum1];

			for( j=0; j<matrix_temp1->mcol(); j++ )
			{	
				CTString str1;
				str1.Format( "(%d)", j+1 );
				CTString str2 = strField1;
				str2 += str1;
				vect1 = matrix_temp1->col(j);
				vect2 = *NewColArry[iIndex[nColNum2]];

				matrix = matrix | (vect1 * vect2);

				m_strArrayFieldName.Add( str2 + "*" + strField2.GetData() );
			}	
		}
		else if ( nCategoryNum1<0 && nCategoryNum2>=0 )
		{
			CDoubleMatrix * matrix_temp2 = arCategory[nCategoryNum2];

			for( j=0; j<matrix_temp2->mcol(); j++ )
			{	
				CTString str1;
				str1.Format( "(%d)", j+1 );
				CTString str2 = strField2;
				str2 += str1;
				vect1 = *NewColArry[iIndex[nColNum1]];
				vect2 = matrix_temp2->col(j);

				matrix = matrix | (vect1 * vect2);

				m_strArrayFieldName.Add( strField1 + "*" + str2.GetData() );
			}	
		}
		else
		{
			vect1 = *NewColArry[iIndex[nColNum1]];
			vect2 = *NewColArry[iIndex[nColNum2]];

			matrix = matrix | (vect1 * vect2 );
			
			m_strArrayFieldName.Add( strField1 + "*" + strField2.GetData() );
		}
	}
	// ���������
	matrix = matrix | (*NewColArry[iIndex[m_nDependIndex]]);
};


bool CRegressionLogistic::GetCategoryMatrix( CDoubleVector pData, CDoubleMatrix*& pMatrix, int nRow )
{
	CIntVector vect(nRow);
	int i=0;
	for(i=0;i<nRow;i++)
	{
		vect[i] = (int)pData[i];
	}

	//find the number of the different number in the vector;
	CIntVector vect_temp = copy(vect);
	vect.Sort();
	int min = vect[0];
	int count = 1;
	for(i=0;i<nRow-1;i++)
	{
		if( vect[i] != vect[i+1] )
		{
			count++;
		}		
	}

	if( count>50 )
		return false;

	assert( count > 1 );
	//if min is not equal to zero then the every item of the vector be substract with (min)
	if( min != 0 )
	{
		for(int i=0;i<nRow;i++)
		{
			vect_temp[i] -= min;
			vect[i] -= min;
		}
	}

	//translate the vector from zero , 
	for(i=1;i<nRow;i++)
	{
		bool bExit(false);
		int  nchange(0);
		int j=0;
		for(j=0;j<nRow;j++)
		{
			if( vect[j] == i )
			{
				bExit = true;
				break;
			}
			if( vect[j] > i )
			{
				nchange = vect[j] - i;
				break;
			}
		}
		if( bExit )
		{
			continue;
		}
		else
		{
			//change the item of the vector which is bigger than i
			for(j =0;j<nRow;j++)
			{
				if( vect_temp[j] > i )
					vect_temp[j] -= nchange;
				if( vect[j]>i )
					vect[j] -= nchange;
			}
		}
	}

	pMatrix = new CDoubleMatrix( nRow, count-1 );
	for(i=0;i<nRow;i++)
	{
		int nt = vect_temp[i];
		if( nt != count-1)
			(*pMatrix)(i,nt) = 1;
	}	

	return true;
}


bool CRegressionLogistic::IsBinary( TDColAry * ColData, CDoubleVector pWeight, int nCaseNum ) 
{
	bool bBinary = false;
	
	CTArray< double, double > arData;
	int i=0; 
	for( i=0; i<nCaseNum; i++ )
	{
		if( pWeight[i]>0 )
		{
			if( arData.GetSize()==0 )
				arData.Add( ColData->GetAt(i).dValue );
			else if( arData.GetSize()==1 && ColData->GetAt(i).dValue!=arData[0] )
			{
				arData.Add(ColData->GetAt(i).dValue );
				bBinary = true;
			}
			else if( arData.GetSize()==2 && ColData->GetAt(i).dValue!=arData[0] && ColData->GetAt(i).dValue!=arData[1])
			{
				bBinary = false;
				break;
			}
		}
	}
	// 
	if( bBinary )
	{
		for( i=0; i<nCaseNum; i++ )
		{
			if( pWeight[i]>0 )
			{
				TDataCol dc;
				if( arData[0]>arData[1] )
					dc.dValue = (ColData->GetAt(i).dValue==arData[0]) ? 1 : 0;
				else
					dc.dValue= ((ColData->GetAt(i).dValue==arData[0]) ? 0 : 1);
				ColData->SetAt(i,dc);
			}
		}
	}
	
	return bBinary;
}


////////////////////////////////////////////////////////////////////////////
//
//		  ��ֵ�߼��ع鷨����������Ա�����Ӧ������ϵľ���
//         �޸� ������  2005-04-27
////////////////////////////////////////////////////////////////////////////
bool CRegressionLogistic::Logistics(CDoubleMatrix matrix,CDoubleVector vector_weight,CResult * pResult)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();

//	if( nCol > 50 )
//		return false;

	//����X����n*(p+1)����Y����(n*1)
	CDoubleMatrix matrix_X(nRow,nCol);
	CDoubleMatrix matrix_Y(nRow,1);
//	CDoubleVector matrix_Y(nRow,)

	//��X,Yֵ
	int i=0;
	for(i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			if(j==0)
			{                                                                                                                                                                                                                                                                                                                                                                                                      
				matrix_X(i,j) = 1;
				matrix_Y(i,0) = matrix(i,nCol-1);
			}
			else
			{
				matrix_X(i,j) = matrix(i,j-1);
			}
		}
	}
	int p = nCol - 1;
	CDoubleVector vector_n(nRow,1);
//	for(i=0;i<nRow;i++)
//	{
//		vector_n[i] = 1;
//	}

	//���濪ʼ����������ȡbetaEstimate

	//����ʼ��betaֵ��initialBeta
	CDoubleVector vector_preBeta = InitialBeta(matrix_X,matrix_Y,vector_n);
	//�������epsilon�ͳ�ʼ���error���Լ�ѭ��������
	double error   = 1e100;
	double errorpre= 100;
	double epsilon = 1e-7;
	long   cycleNum= 1;
	CDoubleVector vector_passBeta;
	double nuerror = 0;
	double FuncOld = 0;
	CDoubleVector nu;
	CDoubleVector vector_temp;
	CDoubleVector vector_probit;

	CDoubleVector qVector;	
	CDoubleMatrix Hessian;
	
	while( error > epsilon )
	{
		//Newton-Raphson����ʽ
		vector_temp   = exp( matrix_X * vector_preBeta );

		vector_probit = vector_temp / (1.0 + vector_temp);
		//����  Ywj 2005-04-27
		int i=0;
		for(i=0;i<vector_probit.vlen();i++)
		{
			if( fabs( vector_probit[i] - 1 ) < 0.00001 )
			{ 
				vector_probit[i] -= 0.0000001;
			}
		}


		GetHessian(vector_probit,matrix_X,matrix_Y,vector_n,vector_weight,qVector,Hessian);

		nu =  MatrixToVector((CDoubleMatrix)(pinv( Hessian ) * VectorToMatrix(qVector)));

		double t = GetT(matrix_X,matrix_Y.col(0),vector_n,vector_weight,vector_preBeta,nu);
		
		vector_passBeta = vector_preBeta + t * nu;

		double func = GetLikeHood(matrix_X,matrix_Y.col(0),vector_n,vector_weight,vector_preBeta);

		error = fabs( ( func - FuncOld ) / func );
		
		FuncOld = func;

		vector_preBeta = vector_passBeta;

		cycleNum++;
	}

	nuerror = fabs( Max((CDoubleVector)( nu /vector_passBeta)));

	if( nuerror > 0.01 )
	{
		m_strErrorInfo += _T("���棺������Ȼ����ֵ���ܲ����ڣ�������������һ�����������ϸ�����ģ����ϵ���Ч��������.\n");
	}
	//CDoubleVector vector_lastBeta = vector_passBeta;
	m_CoefVec = vector_passBeta;//modify by xds 20060817
	//beta��Э�������Ĺ���
	 vector_temp  = exp( matrix_X * m_CoefVec );
	 vector_probit= vector_temp / (1.0+vector_temp);

	CDiagMatrix<double> matrix_informA(vector_n * vector_probit * (1.0-vector_probit));
	CDoubleMatrix temp ;
	temp = trans(matrix_X) *matrix_informA;
	CDoubleMatrix matrix_covBeta= pinv( (CDoubleMatrix)(temp * matrix_X) );
	StatisForLogistic(matrix_covBeta,m_CoefVec,nCol);

	return true;
}

bool CRegressionLogistic::Export()
{	
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	
	if (!m_bSaveModel)
		return true;
	bool bOpen = theFile.Open(m_ModelFile.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Javaʹ��
	//�㷨����
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += REGRESSIONLOGISTIC;
	//����Դ
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + m_pDataInterface->m_DataAccess.GetFileName() + "\"";
	//������� 
	szText += " ";
	szText += VARINPUT;
	nSize = m_strArrayFieldName.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_strArrayFieldName.GetAt(i);
	}
	//Ŀ�����
	szText += " ";
	szText += VAROUTPUT;
	szText += " " + strDependName;
	//����ʱ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	szTemp.Format("%d��%d��%d��%dʱ%d��%d��",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	szText += " ";
	szText += DATETIME;
	szText += " " + szTemp + "\r\n";
	//����
	nLen = szText.GetLength() + sizeof(int);//�ܵĳ���
	theFile << nLen;
	nLen = theFile.WriteString(szText.GetData());
	//��ʼдģ�Ͳ���
	theFile.SeekToEnd();
	bool bWithConst = true;
	theFile << bWithConst;
	//�������������
	nSize = m_strArrayFieldName.GetSize();
	theFile << nSize;
	//����
	for (i=0; i<nSize; i++)
	{
		theFile << m_strArrayFieldName.GetAt(i);
	}
	//����
	nSize += bWithConst ? 1 : 0;
	for (i=0; i<nSize; i++)
	{
		theFile << m_CoefVec(i);
	}
	//Ŀ�����
	theFile << strDependName;
	theFile.Close();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			                        ��ʼ��beta����
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CDoubleVector CRegressionLogistic::InitialBeta(CDoubleMatrix matrix_X, CDoubleMatrix matrix_Y, CDoubleVector vector_n)
{
	CDoubleVector vector_y = matrix_Y.col(0);
	CDoubleVector vector_temp = (vector_y + 0.5) / ( vector_n - vector_y + 0.5);
	CDoubleVector vector_z = log( vector_temp );
	CDoubleVector vector_v = ((vector_n +1.0)*(vector_n+2.0))/(vector_n*(vector_y+1.0)*(vector_n-vector_y+1.0));
	CDiagMatrix<double>  thinMatrix(1.0/ vector_v );
	TraceVector(vector_v);
//	TraceMatrix(thinMatrix);
	CDoubleMatrix temp = trans(matrix_X) *thinMatrix;
	CDoubleMatrix matrix = temp *matrix_X;
	//CDoubleMatrix matrix = (CDoubleMatrix)(trans(matrix_X) * diag(1.0/ vector_v ) * matrix_X );
//	::WriteData(matrix,"zfm"); //zfm
	//CDoubleMatrix matrix_inv = pinv(matrix);  
	
	temp = pinv( matrix ) * trans(matrix_X) ;
	CDoubleMatrix temp2 = temp* thinMatrix;
	CDoubleVector vector_beta = temp2* vector_z;
	//CDoubleVector vector_beta = pinv( matrix ) * trans(matrix_X) * diag(1.0 / vector_v ) * vector_z;
    return vector_beta;
}

double CRegressionLogistic::GetT(CDoubleMatrix xMatrix, CDoubleVector yVector,CDoubleVector nVector, CDoubleVector weight, CDoubleVector preBeta, CDoubleVector S)
{
	double Ep=0.01;
	double A=0;
	double B=1;
	double ts=B-A;

	while(ts>=Ep)
	{
		double TA=B-0.618*ts;
		double TB=A+0.618*ts;
		CDoubleVector Beta1=preBeta+TA*S;
		double FA=GetLikeHood(xMatrix,yVector,nVector,weight,Beta1);
		CDoubleVector Beta2=preBeta+TB*S;
		double FB=GetLikeHood(xMatrix,yVector,nVector,weight,Beta2);
		if(FA<FB)
			A=TA;
		else
			B=TB;
		ts=B-A;
	}
	return (A+B)/2;
}

double CRegressionLogistic::GetLikeHood(CDoubleMatrix xMatrix, CDoubleVector yVector, CDoubleVector nVector, CDoubleVector weight, CDoubleVector preBetaVector)
{
	int row = xMatrix.mrow();
	int col = xMatrix.mcol();
	double Func=0;
	CDoubleVector prob=MatrixToVector( (CDoubleMatrix)(xMatrix * VectorToMatrix(preBetaVector)));
	for(int i=0;i<row;i++)
	{
		Func += weight[i] * ( 1.0 - nVector[i] * log(1.0+exp(prob[i]) ) + yVector[i] * prob[i]  );
	}	
	return Func;
}

void CRegressionLogistic::GetHessian(CDoubleVector probVector,CDoubleMatrix xMatrix,CDoubleMatrix yMatrix,CDoubleVector nVector,CDoubleVector weight,CDoubleVector &vect_q,CDoubleMatrix &hessen)
{
	int r = xMatrix.mrow();
	int c = xMatrix.mcol();

	CDoubleVector qVector(c);

	int i=0;
	for(i=0;i<c;i++)
	{
		 for(int j=0;j<r;j++)
		 {
			  qVector(i)=qVector(i) + weight(j) * (  yMatrix(j,0) - nVector(j)*probVector(j)  ) * xMatrix(j,i)  ;
		 }
	}

	CDoubleMatrix Hessian(c,c);
	for(i=0;i<c;i++)
	{
      for(int j=0;j<c;j++)
	  {
            for(int k=0;k<r;k++)
			{
                  Hessian(i,j)=Hessian(i,j)+weight(k) * (  xMatrix(k,i) * xMatrix(k,j) * nVector(k) * probVector(k) * (1-probVector(k))  );
			}
	  }
	}
	vect_q = copy(qVector);
	hessen = copy(Hessian);
}

void CRegressionLogistic::InitParma()
{
	m_bStopCalculate = false;
	m_strErrorInfo = _T("");
	m_strMissField = _T("");
}

bool CRegressionLogistic::IsCategreryVar(CTString strName,int &ntbegin)
{
	strName += "(1)";
	for(int j=0;j<m_strArrayFieldName.GetSize();j++)
	{
		CTString str = m_strArrayFieldName.GetAt(j);
		if( str == strName )
		{
			ntbegin = j;
			return true;
		}
	}	
	return false;
}

CDoubleMatrix CRegressionLogistic::GetCategrery(TDColAry *pDataArray,int nRow)
{
	CIntVector vect(nRow);
	int i=0;
	for(i=0;i<nRow;i++)
	{
		//vect[i] = (int)pDataArray[i].dValue;
		vect[i] = (int)pDataArray->GetAt(i).dValue;
	}

	WriteData(vect,"vect");
	
	//find the number of the different number in the vector;
	CIntVector vect_temp = copy(vect);
	vect.Sort();
	int min = vect[0];
	int count = 1;
	for(i=0;i<nRow-1;i++)
	{
		if( vect[i] != vect[i+1] )
		{
			count++;
		}		
	}

	assert( count > 1 );
	//if min is not equal to zero then the every item of the vector be substract with (min)
	if( min != 0 )
	{
		for(int i=0;i<nRow;i++)
		{
			vect_temp[i] -= min;
			vect[i] -= min;
		}
	}

	//translate the vector from zero , 
	for(i=1;i<nRow;i++)
	{
		bool bExit(false);
		int  nchange(0);
		int j=0;
		for(j=0;j<nRow;j++)
		{
			if( vect[j] == i )
			{
				bExit = true;
				break;
			}
			if( vect[j] > i )
			{
				nchange = vect[j] - i;
				break;
			}
		}
		if( bExit )
		{
			continue;
		}
		else
		{
			//change the item of the vector which is bigger than i
			for(j =0;j<nRow;j++)
			{
				if( vect_temp[j] > i )
				{
					vect_temp[j] -= nchange;
				}
			}
		}
	}

	WriteData(vect_temp,"temp");

	CDoubleMatrix matrix(nRow,count - 1);
	for(i=0;i<nRow;i++)
	{
		int nt = vect_temp[i];
		if( nt != count-1)
			matrix(i,nt) = 1;
	}	
	return matrix;
}

void CRegressionLogistic::StatisForLogistic(CDoubleMatrix matrix_covBeta, CDoubleVector vector_beta, int nColNum)
{
	int i=0;

	CDoubleVector vector_se = sqrt( diag( matrix_covBeta ) );
	CDoubleVector vector_w  = pow((CDoubleVector)( vector_beta / vector_se ),2.0 );
	CDoubleVector vector_p  = 1.0 - chi2cdf(vector_w,1);	
	//����������
	if( !m_strErrorInfo.IsEmpty()  )
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( m_strErrorInfo );
		m_pResult->Add( pWarningTextRslt );
	}
	if( m_bStopCalculate )
		return;

	CTString strEquation = " Ln(Prob{1}/Prob{0}) = ";
	CTString strTemp;

	int temp_cont=1;
	strTemp.Format("%f",vector_beta(0));
	strEquation+=strTemp;
	for (i=1;i<vector_beta.vlen();i++)
	{
		strTemp.Format("%.4f*%s",vector_beta(i),m_strArrayFieldName.GetAt(i-1).GetData());
		if ((vector_beta(i)>0))
			strEquation+="+";
		if (strEquation.GetLength()+strTemp.GetLength() > temp_cont * 100)
		{
			strEquation =strEquation + "\n";
			temp_cont++;
		}
		strEquation+=strTemp;
	}
	CRsltElementTable * pETableFunc = NULL;
	CTLTable * pTableFunc = new CTLTable;
	pTableFunc->CreateTable(2, 2);	
	pTableFunc->SetTitle("�ع鷽��");
	pTableFunc->InsertColumn(0,strEquation);
	pETableFunc= new CRsltElementTable("�ع鷽��", pTableFunc);//����һ���������Ķ���
	m_pResult->Add(pETableFunc);

	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("�ع�ϵ������");
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"ϵ��");
	pTable->InsertColumn(2,"��׼���");
	pTable->InsertColumn(3,"�ֶ���ͳ����");
	pTable->InsertColumn(4,"���ɶ�");
	pTable->InsertColumn(5,"������");
	pTable->InsertColumn(6,"���Ʊ�");

	CDoubleVector expb = exp(vector_beta);
	for(i=0;i<vector_se.vlen();i++)
	{
		CTString strVar,strInfl,strVal;
		if (i==0)
			strVar.Format("�������%s",strDependName.GetData());
		else if(i-1 < m_strArrayFieldName.GetSize())
			strVar.Format("%s��%s",m_strArrayFieldName.GetAt(i-1).GetData(), strDependName.GetData());
		strVal.Format("(���ʼ���ֵ=%f)",vector_p[i]);
		if (vector_p[i]<=0.05)
			strInfl="��Ӱ��ϴ�";
		else
			strInfl="��Ӱ���С";
		if(briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strVar+strInfl.GetData()+strVal.GetData());
		if( i == 0 )
			pTable->InsertItem(i,"������");
		else if(i-1 < m_strArrayFieldName.GetSize())
		{
			pTable->InsertItem(i,m_strArrayFieldName.GetAt(i-1));
		}	
		pTable->SetItemText(i,1,vector_beta[i]);
		pTable->SetItemText(i,2,vector_se[i]);
		pTable->SetItemText(i,3,vector_w[i]);
		pTable->SetItemText(i,4,1);
		pTable->SetItemText(i,5,vector_p[i]);
		if (i!=0)
			pTable->SetItemText(i,6,expb[i]);
	}

	CRsltVariable * pETable= new CRsltElementTable("�ع�ϵ������",pTable);
	m_pResult->Add(pETable);
}


//���²�������Ԥ��ġ�
CResult* CRegressionLogistic::OnRegPred(CDataInterface * pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pResult = new CResult("��ֵ�߼��ع�ģ��Ԥ����");
	if (!GetPredValueVo(pDataInterface, VO))//��������ģ��
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( "ģ�Ͳ��������޷����С�" );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	CDWordArray indexArr;
	int nCol = m_tVarPred.iCount;
	int i=0;
	for (i=0; i<m_tVarPred.iCount; i++)
		indexArr.Add(m_tVarPred.pValue[i]);
	CDoubleMatrix Pred_Mx;
	int nRow = pDataInterface->GetColsData(indexArr, Pred_Mx, 1);//������
	CDoubleVector OneTemp(nRow, 1.0);
	Pred_Mx = OneTemp|Pred_Mx;
	CDoubleVector yPred = Pred_Mx * m_CoefVec;//Ԥ��ֵ
	yPred = exp(yPred);
	for (i=0; i<yPred.vlen(); i++)
		yPred(i) = yPred(i)/(1.0+yPred(i));
	
//	������������ӣ�ʼ
	if (m_pDataInterface->m_DataAccess.FieldByName(m_strDepPred) != NULL)
	{
		int nTemp = m_pDataInterface->m_DataAccess.IndexByName(m_strDepPred);
		m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
	}
	CField *PredField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
	PredField->SetFieldName(m_strDepPred);
	pDataInterface->m_DataAccess.m_pFieldList->Add(PredField);
	pDataInterface->m_DataAccess.First();
	for (i=0; i<nRow; i++)
	{
		//pDataInterface->m_DataAccess.Edit();
		PredField->SetAsDouble(yPred(i));
		//pDataInterface->m_DataAccess.Post();
		pDataInterface->m_DataAccess.Next();
	}
	if (m_bEvaluation)
	{
		GainLeftPlot(pDataInterface);
	}
	//TEST �ã�����
	CTString szNowFile("");
	CTString szNewFile("");
	szNowFile = pDataInterface->m_DataAccess.GetFileName();
	bool bOld = pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
	//��ʱ:ΪNewFile
	//��ʱ:ΪNowFile.
	CTString SaveFileName;
	SaveFileName.Format("��ֵ�߼��ع�Ԥ��������ɣ�������ļ���Ϊ��%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
	CRsltElementText *pText = new CRsltElementText("��ֵ�߼��ع�Ԥ��");//����һ���������Ķ���
	pText->AddString(SaveFileName);
	m_pResult->Add(pText);
	return m_pResult;
}

bool CRegressionLogistic::GainLeftPlot(CDataInterface *pDataInterface)
{
	int i=0, j=0;
	CDWordArray dataArr;
	int nDepOrg = pDataInterface->IndexByName(strDependName);
	if (nDepOrg == -1)
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��");
		pWarningTextRslt->AddString( "û���ҵ�Ԥ�������ԭʼֵ���޷�����Gain Leftͼ��������" );
		m_pResult->Add(pWarningTextRslt);
		return false;
	}
	dataArr.Add(nDepOrg);
	int nDepPred = pDataInterface->IndexByName(m_strDepPred);
	dataArr.Add(nDepPred);
	CDoubleMatrix	PairedMx;
	int nRow = pDataInterface->GetColsData(dataArr, PairedMx, 0);
	CDoubleVector ProbVec = PairedMx(1);
	CIntVector	indexVec;
	ProbVec.Sort(indexVec, 1);
	CDoubleVector	OregVec(nRow);
	for (i=0; i<nRow; i++)
		OregVec(i) = PairedMx(indexVec(i)-1, 0);

	double TValue = nRow - OregVec.sum();//1Ϊ��ʧ
	if (TValue<1)
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��");
		pWarningTextRslt->AddString( "ģ���޷�����" );
		m_pResult->Add(pWarningTextRslt);
		return false;
	}

	CDoubleMatrix FTPN(2, 2, 0.1);
	double SepValue = TValue/nRow;//����ʧ�ı���
	for (i=0; i<nRow; i++)
	{
		FTPN(int(OregVec(i)+0.01), (ProbVec(i)>SepValue)) += 1;
	}

	CRsltElementTable * pETableFTPN = NULL;
	CTLTable * pTableFTPN = new CTLTable;
	pTableFTPN->CreateTable(2, 2);
	pTableFTPN->SetTitle("��������");

	pTableFTPN->SetCols(4);
	pTableFTPN->SetRows(4);
	pTableFTPN->InsertColumn(0,"");
	pTableFTPN->InsertColumn(1,"Ԥ��ֵΪ1");
	pTableFTPN->InsertColumn(2,"Ԥ��ֵΪ0");
	pTableFTPN->InsertColumn(3,"�ܼ�");

	pTableFTPN->InsertItem(1,"ʵ��ֵΪ1");
	pTableFTPN->InsertItem(2,"ʵ��ֵΪ0");
	pTableFTPN->InsertItem(3,"�ܼ�");
	pTableFTPN->SetItemText(0, 1, int(FTPN(1,1)));
	pTableFTPN->SetItemText(0, 2, int(FTPN(1,0)));
	pTableFTPN->SetItemText(1, 1, int(FTPN(0,1)));
	pTableFTPN->SetItemText(1, 2, int(FTPN(0,0)));
	pTableFTPN->SetItemText(0, 3, int(FTPN(1,1) + FTPN(1,0)));
	pTableFTPN->SetItemText(2, 1, int(FTPN(1,1) + FTPN(0,1)));
	pTableFTPN->SetItemText(1, 3, int(FTPN(0,0) + FTPN(0,1)));
	pTableFTPN->SetItemText(2, 2, int(FTPN(0,0) + FTPN(1,0)));
	pTableFTPN->SetItemText(2, 3, nRow);
	pETableFTPN = new CRsltElementTable("��������",pTableFTPN);//����һ���������Ķ���
	m_pResult->Add(pETableFTPN);


	int step = (nRow-1)/500+1;
	int Case = nRow/step;

	TDataPointValVal* pGainValue;
	pGainValue = new TDataPointValVal[Case];

	TDataPointValVal* pLiftValue;
	pLiftValue = new TDataPointValVal[Case];
	int nYpoint= 0;
	int nXpoint = 0;
	int iCount = 0;
	
	pGainValue[0].fXVal = 0;
	pLiftValue[0].fXVal = 0;
	pGainValue[0].fYVal = 0;
	for (i=1; i<Case; i++)
	{
		pGainValue[i].fXVal = (i+1.0)/Case;
		pLiftValue[i].fXVal = (i+1.0)/Case;
		for (j=0; j<step; j++)
		{
			nYpoint += (int(OregVec(j+(i-1)*step) + EPS_ACCUR));
		}
		pGainValue[i].fYVal = nYpoint/(nRow - TValue);
		if (pLiftValue[i].fXVal > 0.005)
		{
			pLiftValue[i].fYVal = pGainValue[i].fYVal/pGainValue[i].fXVal;
		}
		else
			iCount = i;
	}

	for (i=0; i<iCount+1; i++)
	{
		pLiftValue[i].fYVal = pLiftValue[iCount+1].fYVal;
	}

	TDataPointValVal* pBaseLine;
	pBaseLine = new TDataPointValVal[2];
	pBaseLine[0].fXVal = 0;
	pBaseLine[0].fYVal = 0;
	pBaseLine[1].fXVal = 1;
	pBaseLine[1].fYVal = 1;

	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle("����ͼ����");

	pTable->SetCols(3);
	pTable->SetRows(Case+1);
	pTable->InsertColumn(0,"�����ٷ�λ");
	pTable->InsertColumn(1,"Gainֵ");
	pTable->InsertColumn(2,"Liftֵ");
	CTString str;
	for(i=0;i<Case;i++)
	{
		str.Format("%.2f %", pLiftValue[i].fXVal*100.0);
		pTable->InsertItem(i, str);
		pTable->SetItemText(i, 1, pGainValue[i].fYVal);
		pTable->SetItemText(i, 2, pLiftValue[i].fYVal);
	}
	pETable= new CRsltElementTable("����ͼ����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);

	CTChartLine *pTChartLine = new CTChartLine();
	pTChartLine->SetLine(pBaseLine, 2, ValVal, "��׼��"); 
	pTChartLine->SetLine(pGainValue, Case, ValVal, "Gainֵ"); 
	CRsltElementChart *pREC = new CRsltElementChart(strDependName + " ģ��Ч�� Gainͼ",  pTChartLine);
	m_pResult->Add(pREC);    
	
	pBaseLine[0].fYVal = 1;
	CTChartLine *pTChartLineLift = new CTChartLine();
	pTChartLineLift->SetLine(pBaseLine, 2, ValVal, "��׼��"); 
	pTChartLineLift->SetLine(pLiftValue, Case, ValVal, "Liftֵ"); 
	pREC = new CRsltElementChart(strDependName + " ģ��Ч�� Liftͼ",  pTChartLineLift);
	m_pResult->Add(pREC);    
	return true;
}

bool CRegressionLogistic::GetPredValueVo(CDataInterface * pDataInterface, string VO)
{
//	REGBLOGPRED
//		/DATA=[FILE]
//		/MODELFROMFILE [FILE]
//		/VARLIST [varlist]
//		/PREDNAME [strname]
// 		/REPLACEVARNAME [nameorginal namepred]...
//		/OUTPUT EVALUATION
	
	AnaWord VOstr;
	VOstr.Import(VO);

//		/MODELFROMFILE [FILE]
	bool bOK = false;
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//���ļ���ģ��
	{
		CTString szFileName = AnaSyntax::getSingleStr("/MODELFROMFILE",VOstr);
		if(GetValueFromModel(pDataInterface, szFileName.GetData()))
			bOK = true;
	}
	if (!bOK)
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( "���õ�ģ���д���" );
		m_pResult->Add( pWarningTextRslt );
	}
//		/VARLIST [varlist]
	CTStringArray ynames;
	AnaSyntax::getVAR("/VARLIST", VOstr, ynames);
// 		/REPLACEVARNAME [nameorgi namepred]...
	CTStringArray replacestr;
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, replacestr);
	if (replacestr.GetSize() != 2*int(replacestr.GetSize()/2))
		return false;
	//���������򣬴��µõ�����ֵ��
	if (!AnaSyntax::Replace(m_strArrayFieldName, ynames, replacestr))
		return false;
	pDataInterface->getVarID(m_strArrayFieldName, m_tVarPred);
//		/PREDNAME [strname]
	if (AnaSyntax::exist("/PREDNAME", VOstr))
		m_strDepPred = AnaSyntax::getSingleStr("/PREDNAME", VOstr);
	else
		m_strDepPred = "Ԥ��ֵ";
	if (m_strDepPred == "")
		return false;

	if (ynames.GetSize() != m_strArrayFieldName.GetSize())
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString("�ṩ���Ա�����������ģ�����Ա����ĸ�����" );
		m_pResult->Add( pWarningTextRslt );
	}
//		/OUTPUT EVALUATION
	m_bEvaluation = AnaSyntax::findstr("/OUTPUT", "EVALUATION", VOstr);

	return true;
}

bool CRegressionLogistic::GetValueFromModel(CDataInterface* pDataInterface, string Model)
{
	int i=0, nSize=0;
	CTString szText("");
	string szTemp;
	TCLFile theFile;
	
	bool bOpen = theFile.Open(Model.c_str(), TCLFile::modeRead);
	if (!bOpen)
		return false;
	//��β����
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//��ʼдģ�Ͳ���
	bool bWithConst = true;
	theFile >> bWithConst;
	//�������������
	theFile >> nSize;
	m_strArrayFieldName.RemoveAll();
	//����
	for (i=0; i<nSize; i++)
	{
		theFile >> szText;
		m_strArrayFieldName.Add(szText);
	}
	//����
	nSize += bWithConst ? 1 : 0;
	m_CoefVec.create(nSize);
	for (i=0; i<nSize; i++)
	{
		theFile >> m_CoefVec(i);
	}
	//Ŀ�����
	theFile >> strDependName;
	theFile.Close();
	return true;
}