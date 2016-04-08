// AAnova.h: interface for the CAAnova class.
//                �����ط������
//          ��ע�� ������ 2005��10��21��
//			�޸ģ�����ׯ	2006/03/22
//////////////////////////////////////////////////////////////////////
#include <valarray>
#include "StdAfx.h"
#include "AAnova.h"
#include <vector>
//#include <new>



//////////////////////////////////////////////////////////////////////
extern double fdist(long n1, long n2, double f, double *d);
CAAnova::CAAnova()
{
	m_nRightIndex   =  -1;
	m_nFactorIndex  =  -1;
	m_bDescriptive  =  true;
    m_bMeanPlot     =  true;
	m_bVars = true;
}

CAAnova::~CAAnova()
{
	m_tVarInfo.DeleteObject();
}

BOOL CAAnova::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//	SINGLEANOVA
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/BY [varname]
//		/WEIGHT [varname]
//		/MISSING [{DELALL},{DELREL}]
//		/METHOD [BARTLETT] [{LEVENEMEAN},{LEVENEMED},{LEVENECUTEND(0.1)}]
//		/COMMEAN [{VAREQU},{VARDIF}] [{FTEST},{LEVENESTAT}]
//		/OUTPUT [TABLE] [MEANPLOT] [RESPLOT] [RESNORPLOT] [HISTOGRAM]
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
			m_nRightIndex = tVarWeight.pValue[0];
		else
			return FALSE;
	}

	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;

	if (AnaSyntax::exist("/BY", VOstr) )
	{
		CTStringArray xname;
		TIntStruct tVarWeight;
		AnaSyntax::getVAR("/BY", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarWeight)))
			return FALSE;
		if (tVarWeight.iCount == 1)
			m_nFactorIndex = tVarWeight.pValue[0];
		else
			return FALSE;
	}

	m_nMissingSelect = int(AnaSyntax::findstr("/MISSING", "DELREL", VOstr) );

	m_bBalaiteTest = AnaSyntax::findstr("/METHOD", "BARTLETT", VOstr);//�������ؼ���
	if (AnaSyntax::findstr("/METHOD", "LEVENEMEAN", VOstr) )
	{
		m_bLeveneTest = TRUE;//�ַ������ leveneTest
		m_nArithmetic = 0;//0:����ƽ��ֵ	1:��λ��  2:��β��ֵ
	}
	else if (AnaSyntax::findstr("/METHOD", "LEVENEMED", VOstr))
	{
		m_bLeveneTest = TRUE;
		m_nArithmetic = 1;
	}
	else if (AnaSyntax::findstr("/METHOD", "LEVENECUTEND", VOstr))
	{
		m_bLeveneTest = TRUE;
		m_nArithmetic = 2;
		m_dTailPer = AnaSyntax::getvalue("/METHOD", "LEVENECUTEND", VOstr);
	}
	else
		m_bLeveneTest = FALSE;
//		/COMMEAN [{VAREQU},{VARDIF}] [{FTEST},{LEVENESTAT}]
	if (AnaSyntax::exist("/COMMEAN", VOstr))
	{
		m_bMeansCompare = TRUE;
		m_bVarianceEQ = AnaSyntax::findstr("/COMMEAN", "VAREQU", VOstr);
		m_bVarianceNE = !m_bVarianceEQ;
		m_nVarianceCheck = int(AnaSyntax::findstr("/COMMEAN", "LEVENESTAT", VOstr) );
	}
	else
	{
		m_bMeansCompare = FALSE;
		m_bVarianceEQ = FALSE;
		m_bVarianceNE = FALSE;
		m_nVarianceCheck = 0;
	}

	m_bDescriptive = AnaSyntax::findstr("/OUTPUT", "TABLE", VOstr);
	m_bMeanPlot = AnaSyntax::findstr("/OUTPUT", "MEANPLOT", VOstr);
	m_bResidualScatter = AnaSyntax::findstr("/OUTPUT", "RESPLOT", VOstr);
	m_bResidualTable = AnaSyntax::findstr("/OUTPUT", "RESNORPLOT", VOstr);//�в���̬����ͼ
	m_bHistogram = AnaSyntax::findstr("/OUTPUT", "HISTOGRAM", VOstr);//ֱ��ͼ

	return TRUE;
}

CResult* CAAnova::OnAnova(CDataInterface *pDataInterface, string VO)
{
	m_pRsltElementText=NULL;
	CResult*  pResult;

	if (!GetValueFromVo(pDataInterface, VO))
	{
		pResult = new CResult("һԪ�������");
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}	

	GetData(pDataInterface);   //��ȡ����
	OnAAnova(pDataInterface);//�����һԪ�����.	
	OnDescriptive_Plot(pDataInterface, pResult);//������ͳ�Ʊ�
	if(m_pRsltElementText)
			pResult->Add(m_pRsltElementText);	
	return pResult;
}
//��ȡ��������
void CAAnova::GetData(CDataInterface* pDataInterface)
{
	int i,j,k;
	TDataSet* pxDataSet     = NULL;
	TDataSet* prightDataSet = NULL;
	CPtrArray pyarrayDataSet;
	
	m_nRightIndex=pDataInterface->GetWeightNumb();
	if(m_nRightIndex>=0)
	{
		pDataInterface->GetColData(m_nRightIndex,prightDataSet,5);
	}

	int nCaseNum = pDataInterface->GetColData(m_nFactorIndex,pxDataSet,5);
	
	for( i=0;i<m_tVarInfo.iCount;i++)
	{
		TDataSet* pyDataSetTemp=NULL;
		pDataInterface->GetColData(m_tVarInfo.GetIDAt(i),pyDataSetTemp,5);
		pyarrayDataSet.Add(pyDataSetTemp);
	}


	CTArray<int,int> arrayMissingValue;
	CPtrArray       arrayMatrix;
	bool flag = true;	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{		
		for(i=0;i<nCaseNum;i++)
		{
			if(pxDataSet[i].iIndex<0)
			{
				arrayMissingValue.Add(i);
				continue;
			}
			if( m_nRightIndex>=0 && prightDataSet[i].iIndex<0)
			{
				arrayMissingValue.Add(i);
				continue;
			}
			for(j=0;j<m_tVarInfo.iCount;j++)
			{
				if(((TDataSet*)pyarrayDataSet[j])[i].iIndex<0)
				{
					arrayMissingValue.Add(i);
					break;
				}
			}
		}			
		int nNewCaseNum = nCaseNum - arrayMissingValue.GetSize();	
		
		CDoubleVector* pRight = new CDoubleVector(nNewCaseNum, 1.0);
		CIntVector*    pX     = new CIntVector(nNewCaseNum);
		m_ptrArrayRight.Add(pRight);
		m_ptrArrayX.Add(pX);		
	
		for(j=0; j<m_tVarInfo.iCount; j++)
		{
			CDoubleVector* pY     = new CDoubleVector(nNewCaseNum);
			m_ptrArrayY.Add(pY);
		}		

		int nCnt = 0;
		for( i=0; i<nCaseNum; i++ )//�õ����.
		{
			for( j=0;j<arrayMissingValue.GetSize();j++ )
			{
				if(arrayMissingValue[j]==i)
				{
					flag = false;
					break;
				}
			}
			if(flag==true)
			{
				(*(CIntVector*)pX)(nCnt) = pxDataSet[i].dValue;
				
				if( m_nRightIndex>=0 )
					(*(CDoubleVector*)pRight)(nCnt)  = prightDataSet[i].dValue;
				
				for(j=0; j<m_tVarInfo.iCount; j++)
				{
					(*(CDoubleVector*)m_ptrArrayY[j])(nCnt) = ((TDataSet*)pyarrayDataSet[j])[i].dValue;
				}
				
				nCnt++;
			}
			flag = true;				
		}
	}
	else//��ȥ����ص���.
	{	
		for(i=0;i<m_tVarInfo.iCount;i++)
		{
			arrayMissingValue.RemoveAll();
			for(j=0;j<nCaseNum;j++)
			{
				if(pxDataSet[j].iIndex<0)
				{
					arrayMissingValue.Add(j);
					continue;
				}		
				if( m_nRightIndex>=0 && prightDataSet[j].iIndex<0)
				{
					arrayMissingValue.Add(j);
					continue;
				}
				if(((TDataSet*)pyarrayDataSet[i])[j].iIndex<0)
				{
					arrayMissingValue.Add(j);
				}
			}
			int nNewCaseNum = nCaseNum - arrayMissingValue.GetSize();
		
			CDoubleVector* pRight = new CDoubleVector(nNewCaseNum, 1.0);
			CIntVector*    pX     = new CIntVector(nNewCaseNum);
			CDoubleVector* pY     = new CDoubleVector(nNewCaseNum);
			m_ptrArrayRight.Add(pRight);
			m_ptrArrayX.Add(pX);
			m_ptrArrayY.Add(pY);
		
			int nCnt = 0;
			for( j=0; j<nCaseNum; j++ )
			{
				for( k=0;k<arrayMissingValue.GetSize();k++ )
				{
					int nnn = arrayMissingValue[k];
					if(arrayMissingValue[k]==j)
					{
						flag = false;
						break;
					}
				}
				if(flag==true)
				{
					(*(CIntVector*)pX)(nCnt)            = pxDataSet[j].dValue;
					(*(CDoubleVector*)pY)(nCnt)         = ((TDataSet*)pyarrayDataSet[i])[j].dValue;	
					if( m_nRightIndex>=0 )
						(*(CDoubleVector*)pRight)(nCnt) = prightDataSet[j].dValue;
					nCnt++;
				}
				flag = true;	
				
			}
		}
		
	}
	if(pxDataSet)
	{
	//	delete [] pxDataSet;
		pxDataSet=NULL;
	}
	if(prightDataSet)
	{
	//	delete [] prightDataSet;
		prightDataSet=NULL;
	}
	int nn = pyarrayDataSet.GetSize();
	for(i=0; i<pyarrayDataSet.GetSize(); i++)
	{
		TDataSet* p = (TDataSet*)pyarrayDataSet[i];
	//	if(p)
	//		delete[] p;
		p=NULL;
	}
	pyarrayDataSet.RemoveAll();
}

void CAAnova::OnAAnova(CDataInterface *pDataInterface)
{
	m_pDataInterface=pDataInterface;
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		assert(m_ptrArrayX.GetSize()==1);
		assert(m_ptrArrayRight.GetSize()==1);
		
		//CDoubleVector x        = *(CDoubleVector*)m_ptrArrayX[0];
		CIntVector x        = *(CIntVector*)m_ptrArrayX[0];
		CDoubleVector right = *(CDoubleVector*)m_ptrArrayRight[0];
		for(int i=0; i<m_tVarInfo.iCount; i++)
		{
			CDoubleVector y     = *(CDoubleVector*)m_ptrArrayY[i];			
			
			if(OneAnova(y, x, right)<0)
				return;
		}
	}
	else//��ȥ����ص���.
	{	
		for(int i=0; i<m_tVarInfo.iCount; i++)
		{
			CIntVector    x     = *(CIntVector*)m_ptrArrayX[i];
			CDoubleVector y     = *(CDoubleVector*)m_ptrArrayY[i];
			CDoubleVector right = *(CDoubleVector*)m_ptrArrayRight[i];
			if(OneAnova(y, x, right)<0)
				return;
		}			
	}
}

void CAAnova::OnDescriptive_Plot(CDataInterface* pDataInterface, CResult* &pResult)
{
	briefrTable = NULL;
	int i;
	m_pDataInterface=pDataInterface;
	//add by liytao 20030812
	int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{
		briefrTable=new CTLTable();
	//	briefrTable->m_colorCell=RGB(255,0,0);
		briefrTable->InsertColumn(0,"���˵��");
	}

	CTString strFactName=pDataInterface->GetFieldName(m_nFactorIndex);
	pResult = new CResult("һԪ�������:���ӱ���--"+strFactName);//����һ����������
	//����ÿһ��Ӧ��������ڵ�ǰ���ӽ��з���			
	for( i=0; i<m_tVarInfo.iCount; i++ )
	{
		CTString strDepName=pDataInterface->GetFieldName(m_tVarInfo.GetIDAt(i));
		CRsltVariable *pDepeVariable=new CRsltVariable(strDepName);
		
		//���Ҫ������������
		if(m_bDescriptive)
		{
			CRsltElementTable* pDescriptiveTable=CreateDescriptiveTable(strDepName,strFactName,i);
			pDepeVariable->Add(pDescriptiveTable);
		}
		//pCompareMeansTable
		if(m_bMeansCompare)
		{
			CRsltElementTable* pCompareMeansTable=CreateCompareMeansTable(strDepName,strFactName,i);
			pDepeVariable->Add(pCompareMeansTable);
		}
			
		//�������������
		CreateResult(pDataInterface,pDepeVariable,i,strFactName);
		
		if(m_bMeanPlot&&(!m_pRsltElementText))
		{
			CRsltElementChart* pMeadFlot=CreateMeadPlot(strDepName,strFactName,i);
			pDepeVariable->Add(pMeadFlot);
			
		}
		//����в�ͼ
		if(m_bResidualScatter &&(!m_pRsltElementText))
		{
			CRsltElementChart* pResidualFlot=CreateResiduPlot(strDepName,strFactName,i);
			pDepeVariable->Add(pResidualFlot);
			
		}
        //���Ԥ��ֵ�Բв�ͼ
		
		if(m_bResidualTable &&(!m_pRsltElementText))
		{
			CRsltElementChart* pRPPlot=CreateRNormlot(strDepName,strFactName,i);
			pDepeVariable->Add(pRPPlot);		
		}

		//���ֱ��ͼ
		
		if(m_bHistogram &&(!m_pRsltElementText))
		{
			CRsltElementChart* pRPPlot=CreateHistogram(strDepName,strFactName,i);
			pDepeVariable->Add(pRPPlot);		
		}

		pResult->Add(pDepeVariable);
	}
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1)
	{
		if (pResult->Count()<=0)
			pResult->Add(new CRsltElementTable("���˵��",briefrTable));
		else
			pResult->InsetAt(0,new CRsltElementTable("���˵��",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete[] briefrTable;
	//add end
	//�ͷ����ݿռ�.
	for(i=0; i<m_ptrArrayX.GetSize(); i++)
	{
		CIntVector* p = (CIntVector*)m_ptrArrayX[i];
		if(p)
			delete p;
		p = NULL;
	}
	m_ptrArrayX.RemoveAll();

	for(i=0; i<m_ptrArrayY.GetSize(); i++)
	{
		CDoubleVector* p = (CDoubleVector*)m_ptrArrayY[i];
		if(p)
			delete p;
		p = NULL;
	}
	m_ptrArrayY.RemoveAll();

	for(i=0; i<m_ptrArrayRight.GetSize(); i++)
	{
		CDoubleVector* p = (CDoubleVector*)m_ptrArrayRight[i];
		if(p)
			delete p;
		p = NULL;
	}
	m_ptrArrayRight.RemoveAll();
	
	return;
}

void CAAnova::CreateResult(CDataInterface *pDataInterface,CRsltVariable* &pVariable,int nIndex,CTString strFactName)
{
	if(nIndex<0 || nIndex>=m_arrayResult.GetSize())
		return;

	CDoubleMatrix Output = *(CDoubleMatrix*)m_arrayResult.GetAt(nIndex);
	CTString strName=pDataInterface->GetFieldName(m_tVarInfo.GetIDAt(nIndex));
	//////////////////////////////////////////////////////////////////////////
	//��������������
	if(m_bBalaiteTest)
	{
		CTLTable* pTable = new CTLTable;
		pTable->CreateTable(2,5);
//		pTable->SetCols(5);
//		pTable->SetRows(2);

		pTable->SetTitle(strName+" BartLett����");
		
		pTable->InsertColumn(0,"���ɶ�");
		pTable->InsertColumn(1,"����ͳ����");
		pTable->InsertColumn(2,"95%������λ��");
		pTable->InsertColumn(3,"99%������λ��");
		pTable->InsertColumn(4,"Prb(>����)");
		
		CTString sTemp("");
		sTemp.Format("          %.0f",Output(3,0));
		pTable->InsertItem(0,sTemp);
		pTable->SetItemText(0, 1, Output(3,1));
		pTable->SetItemText(0, 2, Output(3,2));
		pTable->SetItemText(0, 3, Output(3,3));
		pTable->SetItemText(0, 4, Output(3,4));

		if(pTable)
		{
			if(!pVariable)
				pVariable=new CRsltVariable(strName);//����һ����������
			CRsltElementTable*pETable=new CRsltElementTable(strName+"BartLett����",pTable);//����һ���������Ķ���
			pVariable->Add(pETable);
		}
	}
	if(m_bLeveneTest)// Levene Test Output
	{
		CTLTable* pTable = new CTLTable;
		pTable->CreateTable(2,5);
//		pTable->SetCols(5);
//		pTable->SetRows(2);

		pTable->SetTitle(strName+" Levene����");
		
		pTable->InsertColumn(0,"���ɶ�");
		pTable->InsertColumn(1,"F ͳ����");
		pTable->InsertColumn(2,"95% F��λ��");
		pTable->InsertColumn(3,"99% F��λ��");
		pTable->InsertColumn(4,"Prb(>F)");
		
		CTString sTemp("");
		sTemp.Format(" f1= %.0f\n f2 =%.0f",Output(0,1),Output(1,1));
		pTable->InsertItem(0,sTemp);
		pTable->SetItemText(0, 1, Output(4,1));
		pTable->SetItemText(0, 2, Output(4,2));
		pTable->SetItemText(0, 3, Output(4,3));
		pTable->SetItemText(0, 4, Output(4,4));
		if(pTable)
		{
			if(!pVariable)
				pVariable=new CRsltVariable(strName);//����һ����������
			CRsltElementTable*pETable=new CRsltElementTable(strName+"Levene����",pTable);//����һ���������Ķ���
			pVariable->Add(pETable);
		}
		
	}
//////////////////////////////////////////////////////////////////////////

	CTLTable* pTable = new CTLTable;
	pTable = new CTLTable;
	pTable->CreateTable(5,6);
//	pTable->SetCols(6);
//	pTable->SetRows(5);

	pTable->SetTitle(strName+"�������");
	
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"ƽ����");
	pTable->InsertColumn(2,"���ɶ�");
	pTable->InsertColumn(3,"������");
	pTable->InsertColumn(4,"F");
	pTable->InsertColumn(5,"P    ֵ");
	
	pTable->InsertItem(0,"���");
	pTable->InsertItem(1,"����");
	pTable->InsertItem(2,"ȫ��");
	//pTable->InsertItem(3,"�в�");
	
	pTable->SetItemText(0, 1, Output(0,0));
	pTable->SetItemText(1, 1, Output(1,0));
	pTable->SetItemText(2, 1, Output(2,0));
	//pTable->SetItemText(3, 1, Output(5,1));

	pTable->SetItemText(0, 2, (int)Output(0,1));
	pTable->SetItemText(1, 2, (int)Output(1,1));
	pTable->SetItemText(2, 2, (int)Output(2,1));
	//pTable->SetItemText(3, 2, Output(5,0));

	pTable->SetItemText(0, 3, Output(0,2));
	pTable->SetItemText(1, 3, Output(1,2));
	//pTable->SetItemText(3, 3, Output(5,2));

	pTable->SetItemText(0, 4, Output(0,3));
	
	CTString strInfo = "";
	if (briefrTable)
		briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strInfo);

	pTable->SetItemText(0, 5, Output(0,4));	
		
	if(pTable)
	{
		if(!pVariable)
			pVariable=new CRsltVariable(strName);//����һ����������
		CRsltElementTable *pETable=new CRsltElementTable(strName+"�������",pTable);//����һ���������Ķ���
		pVariable->Add(pETable);
	}
}

//nRowΪ���������
CRsltElementTable* CAAnova::CreateDescriptiveTable(CTString strName,CTString strFactName,int idx)
{
	int i, j;
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		assert(m_ptrArrayX.GetSize()==1);

		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}

	CIntVector idxVect, grpVect;
	grp2idx(x, idxVect, grpVect);

	int nGroup = grpVect.vlen();
	if(nGroup<=1)
		return NULL;

	CTLTable* pTable=new CTLTable;
	pTable->CreateTable(nGroup+2,11);
	pTable->SetTitle(strName+"��������");
//	pTable->SetRows(nGroup+2);
//	pTable->SetCols(11);

//	pTable->HideHoriLine(true);

	pTable->InsertColumn(0,strName);
	pTable->InsertColumn(1,"��Ч��¼");
	pTable->InsertColumn(2,"��ֵ");
	pTable->InsertColumn(3,"��׼���");
	pTable->InsertColumn(4,"��׼��");
	pTable->InsertColumn(5,"ЧӦ��ֵ");
	pTable->InsertColumn(6,"ЧӦ��׼��");

	pTable->InsertColumn(7,"��Сֵ");
	pTable->InsertColumn(8,"���ֵ");
	pTable->InsertColumn(9,"95%������������");
	pTable->InsertColumn(10,"95%������������");
	
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nFactorIndex);
	
	for(i=0; i<nGroup; i++)
	{
		CTString strTemp;
		CTString labelVal;
		if(!m_pDataInterface->GetShowValueLabel() || !pField->GetValueVL(grpVect(i),labelVal))
		{
			labelVal.Format("%d",grpVect(i));
		}
		pTable->InsertItem(i,labelVal);
	}

	pTable->InsertItem(nGroup,"ȫ��");

	CDoubleMatrix Output(nGroup+1, 8+2);//�������ЧӦ�����׼��

	for(i=0; i<nGroup; i++)
	{
		for(j=0; j<x.vlen(); j++)
		{
			if(grpVect(i)==x(j))
			{
				Output(i,6) = y(j);
				Output(i,7) = y(j);
				break;
			}
		}
	}

	double Total(0), nTotalCnt(0),TotalSE(0),dTemp;
	for(i=0; i<nGroup; i++)
	{
		double Sum(0),nCnt(0),SE(0);			
		
		for(j=0; j<x.vlen(); j++)
		{
			if(grpVect(i)==x(j))
			{
				Sum         += y(j)*right(j);      //�ܺ�,������ƽ��ֵ.
				nCnt        += right(j);           //�ܸ���,������ƽ��ֵ.
				SE          += y(j)*y(j)*right(j);				
				
				if(Output(i,6)>y(j))
					Output(i,6) = y(j);                 //��Сֵ.
				else if(Output(i,7)<y(j))
					Output(i,7) = y(j);                 //���ֵ.
			}			
		}
		Total       += Sum;
		nTotalCnt   += nCnt;
		TotalSE     += SE;	
		if(nCnt!=1)
		{
			dTemp        = ptdist(nCnt-1,0.025);
		}
		
		SE          -=  Sum/nCnt*Sum;

		Output(i,0) = nCnt;
		Output(i,1) = Sum/nCnt;                           //ƽ��ֵ.
		Output(i,2) = sqrt(SE/(nCnt-1));                  //��׼��.
		Output(i,3) = Output(i,2)/sqrt(nCnt);            //��׼��.
		Output(i,8) = Output(i,1)-Output(i,3)*dTemp;    //5%����.
		Output(i,9) = Output(i,1)+Output(i,3)*dTemp;    //5%����.
	}

	TotalSE          -=  Total/nTotalCnt*Total;
	dTemp             = ptdist(nTotalCnt-1,0.025);
	//��������ЧӦ
	CDoubleVector Vect_Temp;
	double Ymean = mean(y);
	for(i=0; i<nGroup; i++)
	{
		FindIndex(y,x,grpVect[i],Vect_Temp);
		Vect_Temp -= Ymean;
		Output(i,4) = mean(Vect_Temp); //����ЧӦ��ֵ���� 
		Output(i,5) = sqrt(var(Vect_Temp));//����ЧӦ��׼��
	}
    
	//////////////////////////////////////////////////////////////////////////
	
   //����ȫ��
	Output(nGroup,0) = nTotalCnt;
	Output(nGroup,1) = Total/nTotalCnt;
	Output(nGroup,2) = sqrt(TotalSE/(nTotalCnt-1));
	Output(nGroup,3) = Output(nGroup,2)/sqrt(nTotalCnt);
	Output(nGroup,4) = mean(y-Ymean);
	Output(nGroup,5) = Output(nGroup,2);
	
	Output(nGroup,6) = Output(0,6);
	Output(nGroup,7) = Output(0,7);	
	Output(nGroup,8) = Output(nGroup,1)-Output(nGroup,3)*dTemp;
	Output(nGroup,9) = Output(nGroup,1)+Output(nGroup,3)*dTemp;

	for(i=0; i<nGroup; i++)
	{
		if(Output(nGroup,6)>Output(i,6))
			Output(nGroup,6) = Output(i,6);                 //��Сֵ.
		else if(Output(nGroup,7)<Output(i,7))
			Output(nGroup,7) = Output(i,7);                 //���ֵ.
	}
    PrintMatrix("����ͳ�Ʒ������ݽ��",Output,2);

	for(i=0; i<nGroup+1; i++)
	{
		pTable->SetItemText(i, 1, (int)Output(i,0));
		pTable->SetItemText(i, 2, Output(i,1));		
		pTable->SetItemText(i, 3, Output(i,2));
		pTable->SetItemText(i, 4, Output(i,3));
		pTable->SetItemText(i, 5, Output(i,4));
		pTable->SetItemText(i, 6, Output(i,5));
		pTable->SetItemText(i, 9, Output(i,8));		
		pTable->SetItemText(i, 10, Output(i,9));
		pTable->SetItemText(i, 7, Output(i,6));
		pTable->SetItemText(i, 8, Output(i,7));
	}	
	CRsltElementTable*  pRsltElementTable=new CRsltElementTable(strName+"��������",pTable);
	return pRsltElementTable;
}
//���������ֵ�Ƚϵı��
CRsltElementTable* CAAnova::CreateCompareMeansTable(CTString strName,CTString strFactName,int idx)
{
	int i;
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		assert(m_ptrArrayX.GetSize()==1);

		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}

	CIntVector idxVect, grpVect;
	grp2idx(x, idxVect, grpVect);
	int nGroup = grpVect.vlen();
	int OldCount = nGroup;
	if(nGroup<=1)
		return NULL;

	CDoubleVector* DataSet = NULL;
	CDoubleVector ValidIndex;
	nGroup = GetCleanData(y,x,DataSet,ValidIndex);
	
	CDoubleMatrix Output =GetMeansTableStat(DataSet,nGroup);   //(nGroup*(nGroup-1),9);
	
	CTLTable* pTable=new CTLTable;
	pTable->CreateTable(nGroup*(nGroup-1)+1,11);
	if(m_bVarianceEQ)
		pTable->SetTitle(strName+"��ֵ�Ƚ�,���跽�����");
	else
		pTable->SetTitle(strName+"��ֵ�Ƚ�,���跽����");

//	pTable->SetRows(nGroup*(nGroup-1)+1);
//	pTable->SetCols(11);

//	pTable->HideHoriLine(true);

	pTable->InsertColumn(0,"(I)"+strName);
	pTable->InsertColumn(1,"(J)"+strName);
	pTable->InsertColumn(2,"��ֵ��(I-J)");
	pTable->InsertColumn(3,"Tͳ����");
	pTable->InsertColumn(4,"���ɶ�");
	pTable->InsertColumn(5,"Pr(>|T|)");

	if(m_nVarianceCheck == 0)
	{
		pTable->InsertColumn(6,"Fͳ����");
		pTable->InsertColumn(7,"���ɶ�");
		pTable->InsertColumn(8,"Pr(>F)");
	}
	else
	{
		pTable->InsertColumn(6,"Levene ͳ����");
		pTable->InsertColumn(7,"���ɶ�");
		pTable->InsertColumn(8,"Pr(>L)");
	}
	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nFactorIndex);
	
	
	for(i=0; i<nGroup; i++)
	{
		CTString sTemp;
		CTString labelVal,labelVal2;
		if(!m_pDataInterface->GetShowValueLabel() || !pField->GetValueVL(ValidIndex(i),labelVal))
		{
			labelVal.Format("%.0f",ValidIndex(i));
		}
		for(int j=0; j<nGroup-1; j++)
		{	
			if(j ==0)
				pTable->InsertItem(i*(nGroup-1)+j,labelVal);
			else
				pTable->InsertItem(i*(nGroup-1)+j,"");
			if(!m_pDataInterface->GetShowValueLabel() || !pField->GetValueVL(ValidIndex(Output(i*(nGroup-1)+j,1)),labelVal2))
			{
				labelVal2.Format("%.0f",ValidIndex(Output(i*(nGroup-1)+j,1)));
			}
			
		//	pTable->SetItemText(i*(nGroup-1)+j, 0, grpVect(Output(j,0)));
			pTable->SetItemText(i*(nGroup-1)+j, 1, labelVal2);		
			pTable->SetItemText(i*(nGroup-1)+j, 2, Output(i*(nGroup-1)+j,2));
			pTable->SetItemText(i*(nGroup-1)+j, 3, Output(i*(nGroup-1)+j,3));
			pTable->SetItemText(i*(nGroup-1)+j, 4, (int)Output(i*(nGroup-1)+j,5));
			pTable->SetItemText(i*(nGroup-1)+j, 5, Output(i*(nGroup-1)+j,4));

			pTable->SetItemText(i*(nGroup-1)+j, 6, Output(i*(nGroup-1)+j,6));
			sTemp.Format("f1=%.0f , f2=%.0f",Output(i*(nGroup-1)+j,8),Output(i*(nGroup-1)+j,9));
			pTable->SetItemText(i*(nGroup-1)+j, 7,sTemp);
			pTable->SetItemText(i*(nGroup-1)+j, 8, Output(i*(nGroup-1)+j,7));
		}
	}
	if(OldCount != nGroup)
	{
		pTable->SetTail("ע�����ڷ�������ĳЩ���¼����С��2��������ڣ��ʲ������ֵ�Ƚ�");
	}
	CRsltElementTable*  pRsltElementTable=new CRsltElementTable(strName+"��ֵ�����ȽϷ���",pTable);
	return pRsltElementTable;
}

CRsltElementChart*  CAAnova::CreateMeadPlot(CTString strDepName,CTString strFactName,int idx)
{
	int i, j;
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		assert(m_ptrArrayX.GetSize()==1);

		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}
	
	CIntVector idxVect, grpVect;
	grp2idx(x, idxVect, grpVect);	

	int nGroup = grpVect.vlen();
	
	CDoubleVector AveVect(nGroup);

	for(i=0; i<nGroup; i++)
	{
		double Sum(0),nCnt(0);			
		
		for(j=0; j<x.vlen(); j++)
		{
			if(grpVect(i)==x(j))
			{
				Sum         += y(j)*right(j);      //�ܺ�,������ƽ��ֵ.
				nCnt        += right(j);           //�ܸ���,������ƽ��ֵ.				
			}			
		}
		AveVect(i) = Sum/nCnt;//ƽ��ֵ.
	}
	//�ַ�������X���ϲ����������С��λ����
	TDataPointValVal* pDPVV=new TDataPointValVal[nGroup];//����Ҫ���ĵ�.

	CTString tmpStr;
	for(i=0; i<nGroup; i++)
	{
		//tmpStr.Format("%d",grpVect(i)); 
		//tmpStr.TrimLeft(); 
		pDPVV[i].fXVal =grpVect(i);
		pDPVV[i].fYVal =AveVect(i);
	}
	CTChartLine* pLine = new CTChartLine;
	pLine->SetLine(pDPVV,nGroup,ValVal);
	pLine->SetXAxilLabel(strFactName);
	pLine->SetYAxilLabel(strDepName);
	pLine->SetTitle("��ֵͼ");
	CRsltElementChart* pChart = new CRsltElementChart(strDepName+"��ֵͼ",pLine);
	//CTChartLine* pCL=new CTChartLine(pDPVV,nGroup,StrVal,"��ֵͼ",strFactName,strDepName);
	//CRsltElementChart* pREC=new CRsltElementChart(strDepName+"��ֵͼ",pCL);
	delete[] pDPVV;
	return pChart;
}
//�в�ɢ��ͼ
CRsltElementChart*  CAAnova::CreateResiduPlot(CTString strDepName,CTString strFactName,int idx)
{
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}
	int nLen = y.vlen();
	
	CDoubleVector AveVect;
		GetResidu(y,x,AveVect,0);

	TDataPointValVal* pDPVV =new TDataPointValVal[nLen];	

	for(int i=0; i<nLen; i++)
	{
		pDPVV[i].fXVal = i;
		pDPVV[i].fYVal = AveVect(i);
	}
	CTChartScatterPlot* pCL = new CTChartScatterPlot;
	pCL->SetScatter(pDPVV, nLen, ValVal, "");
	pCL->SetXAxilLabel("����");
	pCL->SetYAxilLabel(strDepName + "�в�");
	CRsltElementChart* pREC =new CRsltElementChart(strDepName+"�в�ɢ��ͼ",pCL);
	
	delete []pDPVV;
	return pREC;
}
//�в�--Ԥ��ֵɢ��ͼ
CRsltElementChart*  CAAnova::CreateRPPlot(CTString strDepName,CTString strFactName,int idx)
{
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}
	int nLen = y.vlen();
	
	CDoubleVector AveVect;
	CDoubleVector Predicts = GetResidu(y,x,AveVect,0);
	
	TDataPointValVal* pDPVV =new TDataPointValVal[nLen];	

	for(int i=0; i<nLen; i++)
	{
		pDPVV[i].fXVal = i;
		pDPVV[i].fYVal = Predicts(i);
	}
	CTChartScatterPlot* pCL = new CTChartScatterPlot;
	pCL->SetScatter(pDPVV, nLen, ValVal, "");
	pCL->SetXAxilLabel(strFactName);
	pCL->SetYAxilLabel(strDepName+"�в�");
	CRsltElementChart* pREC =new CRsltElementChart(strDepName+"ɢ��ͼ",pCL);
	
	delete []pDPVV;
	return pREC;
}
CRsltElementChart*  CAAnova::CreateRNormlot(CTString strDepName,CTString strFactName,int idx)
{
	CIntVector x;
	CDoubleVector y, right;
	
	if(m_nMissingSelect==1)//ȥ�����е���.
	{
		assert(m_ptrArrayX.GetSize()==1);
		
		x     = *(CIntVector*)m_ptrArrayX[0];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[0];
	}
	else//��ȥ����ص���.
	{	
		x     = *(CIntVector*)m_ptrArrayX[idx];
		y     = *(CDoubleVector*)m_ptrArrayY[idx];
		right = *(CDoubleVector*)m_ptrArrayRight[idx];
	}
	int nLen = y.vlen();
	
	CDoubleVector AveVect;
	GetResidu(y,x,AveVect,0);//��òв����У�
	AveVect.Sort();
	CDoubleVector Norms = GetNormMedians(nLen);
	CDoubleVector Beta = SimpleRegress(AveVect,Norms); 
	TDataPointValVal* pDPVV =new TDataPointValVal[nLen];	

	for(int i=0; i<nLen; i++)
	{
		pDPVV[i].fXVal = Norms(i);
		pDPVV[i].fYVal = AveVect(i);
	}
	//������̬����ͼ
	CTChartPP* pPPPlot = new CTChartPP(CTChart::PP);	
	pPPPlot->SetTitle(strDepName + "-��̬����ͼ");
	pPPPlot->SetXAxilLabel("��̬��λ��");
	pPPPlot->SetYAxilLabel(strDepName);
	pPPPlot->SetData(pDPVV,nLen,EScatter,ValVal);
	delete []pDPVV;
	pDPVV = new TDataPointValVal[2];
	double fxMin = 0, fxMax = 0;
	double fyMin = 0, fyMax = 0;
	Norms.MaxMin(fxMax,fxMin);
	AveVect.MaxMin(fyMax,fyMin);
	double bb = Beta(0);
	double kk = Beta(1);
	double rr = Beta(2);
	pDPVV[0].fXVal = fxMin;
	pDPVV[0].fYVal = fxMin*kk+bb;
	pDPVV[1].fXVal = fxMax;
	pDPVV[1].fYVal = fxMax*kk+bb;
	pPPPlot->SetData(pDPVV,2,ELine,ValVal);
	CRsltElementChart*pREC=new CRsltElementChart(strDepName + "-��̬����ͼ",pPPPlot);
	return pREC;
}

CRsltElementChart*  CAAnova::CreateHistogram(CTString strDepName,CTString strFactName,int idx)
{
	int nDV=0, nPtCnt=0;
	double fMean=0, fStdv=0, fSum=0;
	void *pDPSV1=NULL, *pDPSV2=NULL;
	CTString szTemp("");

   	CDoubleVector v=*(CDoubleVector*)m_ptrArrayY[idx];
	//����
	v.Sort();
	//ֱ��ͼ
	CTChartHistogram *pHistogram= new CTChartHistogram;
	pHistogram->SetNoraml(true);
	pHistogram->CalcXYAxis(v,&pDPSV1,nDV);  //ֱ��ͼ
	pHistogram->CalcNormalCurve(v,&pDPSV2,nPtCnt);//����ͼ
	pHistogram->GetParam(fMean,fStdv,fSum);
	szTemp.Format("��ֵ��%.2f\n��׼�%.2f\n��¼����%.0f",fMean,fStdv,fSum);
	pHistogram->SetHistogram(pDPSV1,nDV,ValVal,szTemp);
	pHistogram->SetData(pDPSV2,nPtCnt,ELine,ValVal);
	pHistogram->SetXAxilLabel(strDepName);
	pHistogram->SetYAxilLabel("Ƶ��");
	pHistogram->SetTitle("ֱ��ͼ");
	CRsltElementChart *pChart = new CRsltElementChart("ֱ��ͼ",pHistogram);
	delete []pDPSV1;
	delete []pDPSV2;
	return pChart;
}

//Add By ytli @2003/01/14
//xΪ�������.
int CAAnova::OneAnova(CDoubleVector& y, CIntVector& x, CDoubleVector& right)
{
	int i,j;	
	
	int nx   = x.vlen();
	int ny   = y.vlen();

	//////////////////////////////////////���Է������ļ�����///////////////////////////////////////////
	Testify(y,x);
	////////////////////////////////////////���Խ����//////////////////////////////////////////
	
	CTArray<int, int>       array1;//�洢����.    
	CTArray<int, int>       array2;//�洢ÿ������Ԫ�صĸ���.
	CTArray<double, double> array3;//�洢ÿ����ܺ�.

	bool flag = false;
	for( i=0; i<ny; i++ )
	{
		for( j=0; j<array1.GetSize(); j++ )
		{
			if( x(i)==array1[j] )
			{
				flag = true;                                
				break;
			}
		}
		if(flag == false)
		{
			array1.Add(x(i));  
			array2.Add(right(i));
			array3.Add(y(i)*right(i));
		}
		else
		{
			array2[j] += right(i);
			array3[j] += y(i)*right(i);
			flag       = false;
		}
	}
	
	if(array2.GetSize()<=1)
	{
		CTString str="����: �������¼������2��,���ܽ��м���!";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return -1;
	}

	long double Total(0),nCnt(0),SE(0),SA(0),ST(0);
	
	for(i=0; i<ny; i++)
	{
		ST       += y(i)*y(i)*right(i);
		Total    += y(i)*right(i);
		nCnt     += right(i);
	}
	
	ST -=  Total/nCnt*Total;

	
	for(i=0; i<array1.GetSize(); i++)
	{
		SA += array3[i]/array2[i]*array3[i];
	}

	SA -=  Total/nCnt*Total;
	
	double AveSA = SA/(array2.GetSize()-1);
	double AveSE = ST/(nCnt-array2.GetSize());

	//========================
	CIntVector idx,grp;
	grp2idx(x,idx,grp);

	//��ƽ��ֵ
	CDoubleVector Mean(grp.vlen(), 0);
	for(i=0;i<grp.vlen();i++)
	{
		int nCnt = 0;
		for(j=0;j<x.vlen();j++)
		{
			if(x(j)==grp(i))
			{
				Mean(i) += y(j)*right(i);
				nCnt += right(i);			
			} 
		}
		Mean(i) /= nCnt;
	}
	for(i=0;i<x.vlen();i++)
	{
		for(j=0;j<grp.vlen();j++)
		{
			if(x(i)==grp(j))
			{
				SE += (y(i)-Mean(j))*(y(i)-Mean(j));
				break;
			}
		}
	}
	//========================
	//�������������:
	// 0-2 ���������, 3- Bartlett ,4-Levene ,5-�в� ,6-�������ƣ�7-�в������ 
	CDoubleMatrix* pResult = new CDoubleMatrix(8, 5);
	(*pResult)(0,0) = SA;	
	(*pResult)(2,0) = ST;
	(*pResult)(1,0) = SE;

	(*pResult)(0,1) = array1.GetSize()-1;	
	(*pResult)(2,1) = nCnt-1;
	(*pResult) (1,1) = nCnt-array1.GetSize();

	(*pResult) (0,2) = (*pResult) (0,0)/(*pResult) (0,1);	
	(*pResult) (1,2) = (*pResult) (1,0)/(*pResult) (1,1);

	(*pResult) (0,3) = (*pResult) (0,2)/(*pResult) (1,2);	

	(*pResult) (0,4) =1- fcdf((*pResult) (0,3),array1.GetSize()-1,nCnt-array1.GetSize());
	
	double T =0.0; //BartLett Test Statistics
	double W =0.0; //Levene Test Statistics
	int f = 0; //freedom of test
	CDoubleVector* DataSet =NULL;
	CDoubleVector ValidIndex;
	int level =GetCleanData(y,x,DataSet,ValidIndex);
	if(level <=1)
	{
		CTString str="����: �ṩ�ķ���������ʺ��������������!";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		
		m_bBalaiteTest =false;
		m_bLeveneTest = false;
		return -1;
	}
	//GetLStat(CDoubleVector* DataSet , int Count,int& f, int index, double percent)
	if(m_bBalaiteTest)// ���������
	{
		T = GetBStat(DataSet,level,f);//���ɶ� ,ͳ���� ,  95% ,99%, Pr;
		(*pResult)(3,0) = f;
		(*pResult)(3,2) = chi2inv(0.95,f);
		(*pResult)(3,3) = chi2inv(0.99,f);
		if (m_bVars)
		{
			(*pResult)(3,1) = T;
			(*pResult)(3,4) = 1-chi2cdf(T,f);
		}
	}
	if(m_bLeveneTest)
	{
		W = GetLStat(DataSet,level,f,m_nArithmetic,m_dTailPer);
		(*pResult)(4,0) = f;
		(*pResult)(4,1) = W;
		(*pResult)(4,2) = finv(0.95,f,y.vlen()-f-1);
		(*pResult)(4,3) = finv(0.99,f,y.vlen()-f-1);
		(*pResult)(4,4) = 1-fcdf(W,f,y.vlen()-f-1);
	}

	m_arrayResult.Add(pResult);
	return 0;
}
/*******************************************************************
             ��ӷ���ͬһ�Լ����ͳ����  
		������ Levene test , Bartlett test
		���ߣ�������
		ʱ�䣺2005��5��19��
********************************************************************/
/*
//��һ���ְ�BartLett test    ����Ӽ��������ԵĹ��ܺ���������,Ҫ�����ݽ���������,����
����ԭ���Ǻ���ĺܶ���̶��ǻ������������ݽ��е�,���Ĭ�����ݰ���y���������У���Ӧ��xҲ
��ͬ���ı䶯��
*/
//�����±�������ȡ������
CDoubleVector CAAnova::GetSubVect(CDoubleVector Y ,CDoubleVector IndexVect)
{
	int n = IndexVect.vlen();
	CDoubleVector RetVect(n);
	for(int i =0 ;i< n ; i++)
		RetVect[i] = Y[IndexVect[i]];
	return RetVect;
}
//���Һ�����
//���������һ������X , һ����ֵ y 
//���ؽ�������X�д���y�򷵻�X������Ӧ���±꣬���һ������������ء�
int CAAnova::FindIndex(CDoubleVector Y ,CIntVector X , double Xval,CDoubleVector& Result)
{
	//��ȡ����ֵ�ĸ���
	int count = 0;
	int nLen = X.vlen();
	int i =0;
	for(i=0; i< nLen; i++)
		if(abs(X[i] - Xval) < DOUBLE_ZERO)
			count++;
	Result.create(count);
	int iCount = 0;
	for(i =0; i< nLen; i++)
		if(abs(X[i] - Xval) < DOUBLE_ZERO)
			Result[iCount++] = Y[i];
	return count;
}
double CAAnova::GetpercentVal(CDoubleVector Y ,double alpha)
{
	assert(alpha >0 && alpha <100);//ȡֵ 0-100��
	CDoubleVector Sorted_Vect = Y;
	int low = (alpha * Y.vlen())/100.0;
    assert(low >=0 && low<Y.vlen());
	if (low == Y.vlen() -1 || low == 0)
		return Sorted_Vect[low];
	double rate = (low*100.0)/(double)Y.vlen();

	if(rate >= alpha )
		return (Sorted_Vect[low-1]+Sorted_Vect[low])/2.0;
	
  	while(rate <alpha && low < Y.vlen()-1)
	{
		low++;
		rate = (low*100.0)/(double)Y.vlen();
	}
		
	if (low == Y.vlen() -1)
		return Sorted_Vect[low];
	else
		return (Sorted_Vect[low-1]+Sorted_Vect[low])/2.0;	

return 0;
}	

double  CAAnova::TrimMean(CDoubleVector Y, double percent)//��ȡ��β�ľ�ֵ
{

	double alpha = percent/2.0;	
	double low = GetpercentVal(Y,alpha);
	double Up = GetpercentVal(Y,100 - alpha); 
	assert(low <= Up);
	CIntVector(Y.vlen(),0);
	double sum =0.0;
	int count =0;
	for(int i =0; i< Y.vlen(); i++)
		if(Y[i] >= low && Y[i] <= Up)
		{
			sum += Y[i];
			count++;
		}
	return sum/count;
}

//��ȡ��ֵ��Levene test��������Ҫ�õ���
//index = 0 ����Y�� ����ƽ��ֵ
//index = 1 ����Y�� ��λ��
//index = 2 ����Y�� ���½�βpercent/2%��������ľ�ֵ
double  CAAnova::GetMean(CDoubleVector Y, int index , double percent)
{
	double Result = 0.0;
	switch(index) {
	case 0:
		Result = mean(Y);
		break;
	case 1:
		Result = medium(Y);
		break;
	case 2:
		Result = TrimMean(Y,percent);
		break;
	default:
		Result = medium(Y);
		break;
	}
//	TRACE("�õ�����ֵ ��%.6f\n",Result);
	return Result;
}
double CAAnova::medium(CDoubleVector Y)//������λ��
{
	int num = Y.vlen();
	CDoubleVector X = Y;
	if(num%2 != 0)//���������������м���Ǹ�ֵ
		return X[(num-1)/2];
	else
		return (X[num/2]+X[num/2-1])/2.0;
}
CDoubleVector CAAnova::GetIndexVal(CIntVector X)
{
	int L = X.vlen();
	int N = 0;
	BOOL flag = true;
	double temp = X[0];
	int count =1;
	std::vector<double> Vect_Index;
	Vect_Index.push_back(temp);
	int i =1;
	for(i =1; i< L ;i++)
	{
		temp = X[i];
       for(int j =0 ;j < Vect_Index.size(); j++)
		{
			if (abs(Vect_Index.at(j) - temp) < DOUBLE_ZERO )
			{
				flag = false;
				if(count >1)
				break;
			}
		}
		if(flag)
			Vect_Index.push_back(temp);
		flag = true;
	}
	CDoubleVector Ret(Vect_Index.size());
	for(i =0 ;i< Vect_Index.size(); i++)
		Ret[i] = Vect_Index.at(i);
	Vect_Index.clear();
	return Ret;
}
//����BartLett ����ͳ����
double  CAAnova::GetBStat(CDoubleVector Y,CIntVector X, int&f)//����BartLett test ͳ����
{
assert(Y.vlen() == X.vlen());
	CDoubleVector Vect_Grp=GetIndexVal(X);
	int K = Vect_Grp.vlen();
	int N = X.vlen();
	CDoubleVector Vars(K);
	CDoubleVector Ns(K);
	CDoubleVector Vect_Temp;
	m_bVars = true;
	
	for(int i =0; i< K; i++)
	{
		Ns[i] = FindIndex( Y,X,Vect_Grp[i],Vect_Temp)-1.0;
		assert(Ns[i]>0);
		Vars[i] = var(Vect_Temp);
		if (Vars[i] < DOUBLE_ZERO)
			m_bVars = false;
	}
	double T =	0;
	if (m_bVars)
	{
		double dd = dot(Ns,Vars);
		double Sp = dot(Ns,Vars)/(N -K);
		double Frac_Up = (N-K)*log(Sp) - dot(Ns,(CDoubleVector)log(Vars));
		double Frac_Down = 1+(1.0/(3*(K-1)))*(sum(1.0/(Ns)) - 1.0/(N-K));
		T =	Frac_Up/Frac_Down;
//		TRACE("��BartLett �����У�������м����������£�\n");
//		TRACE("Sp = %.6f   Frac_Up = %.6f   Frac_Down = %.6f \n", Sp,Frac_Up,Frac_Down);
//		TRACE("���ļ���õ�BartLett ͳ���� T = %.6f \n", T);
	}
	f = K-1;
	return T;
}
double  CAAnova::GetBStat(CDoubleVector* DataSet,int Count, int&f)//����BartLett test ͳ����
{
	assert(DataSet);
//	CDoubleVector Vect_Grp=GetIndexVal(X);
	int K = Count;
//	int N = X.vlen();
	CDoubleVector Vars(K);
	CDoubleVector Ns(K);
	CDoubleVector Vect_Temp;
	m_bVars = true;
	
	for(int i =0; i< K; i++)
	{
		Vect_Temp = DataSet[i];
		for (int kk=0;kk<Vect_Temp.vlen();kk++)
//			TRACE("%.2f   ",Vect_Temp(kk));
//		TRACE("\nVector Length %d\n",Vect_Temp.vlen());
		
		Ns[i] = Vect_Temp.vlen();//FindIndex( Y,X,Vect_Grp[i],Vect_Temp)-1.0;
		assert(Ns[i]>1);
		Vars[i] = var(Vect_Temp);
		if (Vars[i] < DOUBLE_ZERO)
			m_bVars = false;
	}
	double T =	0;
	if (m_bVars)
	{
		int N = Ns.sum();
		CDoubleVector Ns1 = Ns -1.0;
		double dd = dot(Ns1,Vars);
		double Sp = dot(Ns1,Vars)/(N -K);
		double Frac_Up = (N-K)*log(Sp) - dot(Ns1,(CDoubleVector)log(Vars));
		double Frac_Down = 1+(1.0/(3*(K-1)))*(sum(1.0/Ns1) - 1.0/(N-K));
		T =	Frac_Up/Frac_Down;
//		TRACE("��BartLett �����У�������м����������£�\n");
//		TRACE("Sp = %.6f   Frac_Up = %.6f   Frac_Down = %.6f \n", Sp,Frac_Up,Frac_Down);
//		TRACE("���ļ���õ�BartLett ͳ���� T = %.6f \n", T);
	}
	f = K-1;
	return T;
}
//����Levene test ͳ������Ĭ��ʹ����λ��������β�Ļ���Ĭ��Ϊ10%
double  CAAnova::GetLStat(CDoubleVector Y,CIntVector X,int& f, int index, double percent)
{
	assert(Y.vlen() == X.vlen());
	CDoubleVector Vect_Grp=GetIndexVal(X);
	int K = Vect_Grp.vlen();
	int N = X.vlen();
	CDoubleVector Ns(K);
	CDoubleVector Vect_Temp;
	CDoubleVector Vect_Temp2;
	CDoubleVector Vect_Means(K);
	CDoubleVector Vect_Sums(K);
	double mean = 0;
	double temp =0.0;
	double Zsum = 0.0;
    for(int i = 0; i< K; i++)
	{
		Ns[i] = FindIndex( Y,X,Vect_Grp[i],Vect_Temp);
		assert(Ns[i]>1);
		Vect_Temp.Sort();
		mean = GetMean(Vect_Temp,index,percent);
		Vect_Temp2 = abs((CDoubleVector)(Vect_Temp - mean));
		Vect_Means[i] = Vect_Temp2.mean();
		Zsum += Vect_Temp2.sum();
		temp = var(Vect_Temp2);
		Vect_Sums[i] =temp * (Ns[i]-1.0);		
	}
	double Zmean = Zsum/N;
	
	Vect_Temp = Vect_Means - Zmean;
	Vect_Temp2 = Vect_Temp * Vect_Temp;
	Vect_Temp = Vect_Temp2 * Ns;
	double Frac_Up = (N-K)*Vect_Temp.sum();
	//double Frac_Up = (N-K)*sum(Ns*(Vect_Means - Zmean).^2);
	double Frac_Down = (K-1.0)*Vect_Sums.sum();
	double W = Frac_Up/Frac_Down;
	f = K-1;
	return W;	
}
double  CAAnova::GetLStat(CDoubleVector* DataSet , int Count,int& f, int index, double percent)
{
	assert(DataSet);
	int K = Count;
//	int N = 0;
	CIntVector Ns(K);
	CDoubleVector Vect_Temp;
	CDoubleVector Vect_Temp2;
	CDoubleVector Vect_Means(K);
	CDoubleVector Vect_Sums(K);
	double mean = 0;
	double temp =0.0;
	double Zsum = 0.0;
	int i;
    for( i = 0; i< K; i++)
	{
		Vect_Temp = DataSet[i]; 
		Ns[i] = Vect_Temp.vlen();//FindIndex( Y,X,Vect_Grp[i],Vect_Temp);
		assert(Ns[i]>1);
		Vect_Temp.Sort();
		mean = GetMean(Vect_Temp,index,percent);
		Vect_Temp2 = abs((CDoubleVector)(Vect_Temp - mean));
		Vect_Means[i] = Vect_Temp2.mean();
		Zsum += Vect_Temp2.sum();
		temp = var(Vect_Temp2);
		Vect_Sums[i] =temp * (Ns[i]-1);		
	}

	int N =Ns.sum();
	double Zmean = Zsum/N;
	Vect_Temp = Vect_Means - Zmean;
	Vect_Temp2 = Vect_Temp * Vect_Temp;
	CDoubleVector VsDouble (Ns.vlen());
	for (i=0; i<Ns.vlen(); i++)
		VsDouble(i) = Ns(i)*1.0;
	Vect_Temp = Vect_Temp2 * VsDouble;
	double Frac_Up = (N-K)*Vect_Temp.sum();
	//double Frac_Up = (N-K)*sum(Ns*(Vect_Means - Zmean).^2);
	double Frac_Down = (K-1.0)*Vect_Sums.sum();
	double W = Frac_Up/Frac_Down;
	f = K-1;
	return W;	
}
//���Լ������ȷ��
void CAAnova::Testify(CDoubleVector& Y, CIntVector& X)
{
	//	int f1 ,f2;
	
	//double f  = TrimMean(Y,10);
	//double T =GetBStat(Y,X,f1);
	//double W =GetLStat(Y,X,f2,2);
	GetNormMedians(100);
	CDoubleVector Z ;
	GetResidu(Y,X,Z,0);

	double f = mean(Z);
	double d = sqrt(var(Z));
	double e = var(Z)*(Z.vlen()-1);
}
//������̬��λ��
CDoubleVector CAAnova::GetNormMedians(int n)
{
	CDoubleVector Vect_Ret(n);
	Vect_Ret[n-1] = norminv(pow(0.5,1.0/n),0,1);
	Vect_Ret[0] = norminv(1-pow(0.5,1.0/n),0,1);
	for(int i =1;i< n-1 ;i++)
	{
		Vect_Ret[i] = norminv((i- 0.3175)/(n+0.365),0,1);
	}
	return Vect_Ret;
}
CDoubleVector CAAnova::GetNormMedians(double mu, double sigma/*std */, int n)
{
	CDoubleVector Vect_Ret(n);
	Vect_Ret[n-1] = norminv(pow(0.5,1.0/n),mu,sigma);
	Vect_Ret[0] = norminv(1-pow(0.5,1.0/n),mu,sigma);
	for(int i =1;i< n-1 ;i++)
	{
		Vect_Ret[i] = norminv((i- 0.3175)/(n+0.365),mu,sigma);
	}
	return Vect_Ret;
}
//����в�
CDoubleVector CAAnova::GetResidu(CDoubleVector Y , CIntVector X,CDoubleVector& Residual ,int index)
{
	assert(index ==1 || index == 0);
	CDoubleVector Z(Y.vlen());
	Residual.create(Y.vlen());
	CDoubleVector Vect_Temp;
	CDoubleVector* DataSet = NULL;
	int k = 0;
	int j =0;
	int iCount = 0;
	CDoubleVector ValidIndex;
	int level =GetCleanData(Y,X,DataSet,ValidIndex);
	if( level > 1) //����������Ч������
	{
		if(index == 0) //����ģ��
		{
			for(k=0;k<level;k++ )
			{
				Vect_Temp = DataSet[k];
				double dMean = mean(Vect_Temp);
				for(j =0; j<Vect_Temp.vlen();j++)
				{
					Residual[iCount] = Vect_Temp[j] - dMean; //�õ�����ģ�Ͳв�
					Z[iCount] = dMean; //�õ�����ģ��Ԥ��ֵ 
					iCount++;
				}
			}
		}
	
		else //������ЧӦģ��
		{
			double YMean = mean(Y);
			for(k=0;k<level;k++ )
			{
				Vect_Temp = DataSet[k] - YMean;
				double dMean = mean(Vect_Temp);
				for(j =0; j<Vect_Temp.vlen();j++)
				{
					Residual[iCount++] = Vect_Temp[j] - dMean; //�õ�����ЧӦ�в�					
					Z[iCount++] = YMean + dMean; //�õ�����ЧӦģ�Ͳв�
				}
			}
		}
	}
	delete [] DataSet;
	DataSet =NULL;
	return Z;
}
int CAAnova::GetCleanData(CDoubleVector Y, CIntVector X,CDoubleVector*& pValidData,CDoubleVector& ValidIndex)
{
	assert(Y.vlen() == X.vlen());
	CDoubleVector Vect_Grp = GetIndexVal(X);
	int K = Vect_Grp.vlen();
	int N = X.vlen();
	int Ni = 0;
	int iCount = 0;
	CDoubleVector  VectTemp;
	pValidData = new CDoubleVector[K];
	ValidIndex.create(K);
	for(int i = 0; i< K; i++)
	{
		Ni = FindIndex( Y,X,Vect_Grp[i],VectTemp);
		if(Ni <= 1) //����С��2��Ϊ��Ч���������������������
			continue;
		else
		{
			pValidData[iCount] = VectTemp;
			ValidIndex[iCount] = Vect_Grp[i];
			VectTemp.destruction();
			iCount++;			
		}		
	}
  return iCount;
}
//����Fͳ����,�������������ķ�������
double CAAnova::GetFStat(CDoubleVector X1,CDoubleVector X2 )
{
	int n1 = X1.vlen();
	int n2 = X2.vlen();
	assert(n1>1 && n2>1);//����Ҫ�������ϵ�������
    double s1 = var(X1);
	double s2 = var(X2);
	if(s1==0 && s2 == 0)
        return 0; //���Ϊ��ʱ
	if(s1*s2 ==0 && fabs(s1)+ fabs(s2) >0)
		return -1; //��һ��Ϊ�㡣
	double F = s1/s2; //���� F(n1-1,n2-1); 
	return  2.0*(1-fcdf(F,n1-1,n2-1));//������
}
//����Tͳ����
double CAAnova::GetTStat(CDoubleVector X1,CDoubleVector X2,BOOL bEquel)//����Tͳ�������Ƚ����������ľ�ֵ��bEqual = 0���������
{
	int n1 = X1.vlen();
	int n2 = X2.vlen();
	assert(n1>1 && n2>1);//����Ҫ�������ϵ�������
    double s1 = var(X1);
	double s2 = var(X2);
	if(s1==0 && s2 == 0)
        return 0; //���Ϊ��ʱ
	if(s1*s2 ==0 && fabs(s1)+ fabs(s2) >0)
		return -1; //��һ��Ϊ�㡣
	double S = ((n1-1)*s1 + (n2-1)*s2) / (n1+ n2 - 2);
	double T = 0.0;
    if(bEquel)
		T = fabs(mean(X1) - mean(X2))/sqrt(S*(1.0/n1+1.0/n2));//����t(n1+n2-2)
	else
	{
		double a = sqrt(s1)/n1;
		double b = sqrt(s2)/n2;

		double z1 = (a/(a+b))*(a/(a+b))/(n1-1);
		double z2 = (b/(a+b))*(b/(a+b))/(n2-1);
		int	   df = 1.0/(z1+z2);
		double Sd =sqrt(s1/n1+s2/n2);
		T = fabs(mean(X1) - mean(X2))/Sd;
	}
	return 2.0*(1.0-tcdf(T,n1+n2-2));//������
}
//��������������leveleͳ����
double CAAnova::GetLeveneStat(CDoubleVector X1,CDoubleVector X2)
{
	int n1 = X1.vlen();
	int n2 = X2.vlen();
	assert(n1>1 && n2>1);//����Ҫ�������ϵ�������
	X1 =X1 - GetMean(X1,1,10);
	X1 =X1.abs();
	X2 =X2 - GetMean(X2,1,10);
	X2 =X2.abs();
	double s1 = var(X1);
	double s2 = var(X2);
	if(s1==0 && s2 == 0)
	   return 0; //���Ϊ��ʱ
	if(s1*s2 ==0 && fabs(s1)+ fabs(s2) >0)
	   return -1; //��һ��Ϊ�㡣
	double Zmean = (X1.sum() + X2.sum())/(n1+n2);
	double mean1 = X1.Mean();
	double mean2 = X2.Mean();
	double Frac_up = n1*(mean1-Zmean)*(mean1-Zmean) + n2*(mean2-Zmean)*(mean2-Zmean) ;
	Frac_up *= (n1 + n2 - 2);
	double Frac_Down = (n1-1)*s1 + (n2-1)*s2;
	double F = Frac_up/Frac_Down;
	return 1- fcdf(F,1,n1+n2 - 2); //������
}
CDoubleMatrix CAAnova::GetMeansTableStat(CDoubleVector* DataSet , int Count)
{
  assert(DataSet !=NULL);
  CDoubleMatrix OutPut(Count*(Count-1),12);
  CDoubleVector Temp1;
  CDoubleVector Temp2;
  CDoubleVector StatList(8);
  int Row = 0;
  int iValid = 0;
  for(int i =0;i<Count; i++)
  {
	  Temp1 = DataSet[i];
 	  for(int j =0;j< Count; j++)
	  {
		  if(i ==j)
			  continue;
		  Temp2 = DataSet[j];
//    yzmModify ���Ϸ��سɹ������ж�
		  if (GetStatList(StatList,Temp1,Temp2,m_bVarianceEQ)<=0)
			  continue;
		  OutPut(Row,0) = i;
		  OutPut(Row,1) = j;
		  OutPut(Row,2) = StatList[0];//��ֵ��
		  OutPut(Row,3) = StatList[1];//T����ͳ����
		  OutPut(Row,4) = StatList[2];//������
		  OutPut(Row,5) = StatList[3];//���ɶ�

		  OutPut(Row,6) = StatList[4];//F/L����ͳ����
		  OutPut(Row,7) = StatList[5];//������
		  
		  OutPut(Row,8) = StatList[6];//���ɶ�
		  OutPut(Row,9) = StatList[7];//���ɶ�
		  Row++;	  
	  }
  }
  delete [] DataSet;	  
  return OutPut;
}

// yzmModify ԭ���ķ���ֵΪCDoubleVector ��ȴ������0,-1���ֳ����룬��������
int CAAnova::GetStatList(CDoubleVector &Vect_Ret, CDoubleVector X1,CDoubleVector X2,BOOL bEqual)
{
	int n1 = X1.vlen();
	int n2 = X2.vlen();
//	assert(n1>1 && n2>1);//����Ҫ�������ϵ�������
    double s1 = var(X1);
	double s2 = var(X2);
	if(s1==0 && s2 == 0)
        return 0; //���Ϊ��ʱ
	if(s1*s2 ==0 && fabs(s1)+ fabs(s2) >0)
		return -1; //��һ��Ϊ�㡣
	double S = ((n1-1)*s1 + (n2-1)*s2) / (n1+ n2 - 2);
	double T = 0.0; //T ����ͳ����
	double F = 0.0; //F ����ͳ����
	double L = 0.0;  //Levene ����ͳ����
	double Zmean = (X1.sum() + X2.sum())/(n1+n2);	
	double mean1 = X1.Mean();
	double mean2 = X2.Mean();
	int	   df = 0;
    if(bEqual)
		T = (mean1 - mean2)/sqrt(S*(1.0/n1+1.0/n2));//����t(n1+n2-2)
	else
	{
		double a = sqrt(s1)/n1;
		double b = sqrt(s2)/n2;
		
		double z1 = (a/(a+b))*(a/(a+b))/(n1-1);
		double z2 = (b/(a+b))*(b/(a+b))/(n2-1);
		df= 1.0/(z1+z2);
		double Sd =sqrt(s1/n1+s2/n2);
		T = (mean1 - mean2)/Sd;
	}
//	CDoubleVector Vect_Ret(8);

	//��ֵ�Ƚϼ���
	Vect_Ret[0] = mean1 - mean2;
	Vect_Ret[1] = T;
	if(bEqual)
	{
		Vect_Ret[2] = 2.0*(1.0-tcdf(fabs(T),n1+n2-2));
		Vect_Ret[3] = n1+n2-2;
	}
	else
	{
		Vect_Ret[2] = 2.0*(1.0-tcdf(fabs(T),df));
		Vect_Ret[3] = df;
	}
    //������μ���
	if(m_nVarianceCheck == 0)
	{
		F = s1/s2;
		Vect_Ret[4] = F; //F����ͳ����
		Vect_Ret[5] = 1 - fcdf(F,n1-1,n2-1);//������;
		Vect_Ret[6] = n1-1;
		Vect_Ret[7] = n2-1;
	}
	
	//Levene ͳ�����ļ���
	if(m_nVarianceCheck == 1)
	{
		double Frac_up = n1*(mean1-Zmean)*(mean1-Zmean) + n2*(mean2-Zmean)*(mean2-Zmean) ;
		Frac_up *= (n1 + n2 - 2);
		double Frac_Down = (n1-1)*s1 + (n2-1)*s2;
		L = Frac_up/Frac_Down;
		Vect_Ret[4] = L; //F����ͳ����
		Vect_Ret[5] = 1- fcdf(L,1,n1+n2 - 2); //������
		Vect_Ret[6] = 1;
		Vect_Ret[7] = n1 + n2 - 2;
	}
//  yzmModify �ɹ��򷵻�1
	 return 1;
}


































































































