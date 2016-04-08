// PairTTest.cpp: implementation of the CPairTTest class.
//				�������T����
//					����ׯ    2006/03/21
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlcrnt.h"
#include "PairTTest.h"
const double MIN=1e-15;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPairTTest::CPairTTest()
{
	m_nWeight = -1;
	m_dConfidence=0.95;

}

CPairTTest::~CPairTTest()
{
	for(int i=0;i<m_PtrArrayIndex.GetSize();i++)
	{
		delete m_PtrArrayIndex[i];
	}
	m_PtrArrayIndex.RemoveAll();

}
//*************************************************************************//
//Original author:ZKC													   //
//Date: 2003.3.18														   //
//Function : �õ�������������Ϸ����ݣ��ų��˿�ֵ��ȱʧֵ���ݣ���������  //
//			 ֻҪ���ݻ�Ȩ������һ���п�ֵ��ȱʧֵ���������ɾ�������ų�����//
//*************************************************************************//
void CPairTTest::GetTwoRelatedData(CTArray<double,double> &Data1,
									  CTArray<double,double> &Data2,
									  CTArray<double,double> &Weight,
									  TDataSet *pWeightArray,
									  int nCol1,int nCol2)
{
	TDataSet *pData1Set=NULL;
	TDataSet *pData2Set=NULL;
	int nRow=m_pDataInterface->GetColData(nCol1,pData1Set,5);
	m_pDataInterface->GetColData(nCol2,pData2Set,5);
	for(int j=0;j<nRow;j++)
	{		
		if(m_bWeight)//����ָ����Ȩ��
		{

			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//Ȩ���������о�δȱʧ
			{
				Weight.Add(int(pWeightArray[j].dValue+0.5));
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//�����¼�����Ե���
			{
				m_RowArray.Add(pData1Set[j].iIndex);
			}
		}
		else//����δָ��Ȩ��
		{			
			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0)//Ȩ���������о�δȱʧ
			{
				Weight.Add(1);
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//�����¼�����Ե���
			{
				m_RowArray.Add(pData1Set[j].iIndex);
			}
		}
	}
	if(pData1Set)
	{
		delete []pData1Set;
		pData1Set=NULL;
	}
	if(pData2Set)
	{
		delete []pData2Set;
		pData2Set=NULL;
	}
}

BOOL CPairTTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//PAIRTTEST
//	/DATA=[FILE]
//	/PAIREDVAR [varname varname] [varname varname] ...

//	/WEIGHT [varname]
//	/NORMALTEST [HISTOGRAM({0**})] [NOAMALDIS]
// 	/OPTION [SIGN({0.95**})] 
//  /TestType [NEQ**] [GT] [LT]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	if (AnaSyntax::exist("/TestType",VOstr))
	{
//		CTString str_type;
		//		AnaSyntax::getvalue("/TestType",VOstr,str_type);	
		m_bNEQ = AnaSyntax::findstr("/TestType", "NEQ", VOstr);
		m_bLT = AnaSyntax::findstr("/TestType", "LT", VOstr);
		m_bGT = AnaSyntax::findstr("/TestType", "GT", VOstr);
		if (!m_bNEQ && !m_bLT && m_bGT)
		{
			return FALSE;
		}
	}
	else
	{
		m_bNEQ = TRUE;
	}
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
		m_bWeight = TRUE;
	}
	else
		m_bWeight = FALSE;

	CTStringArray xname;
	AnaSyntax::getVAR("/PAIREDVAR", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	if (m_tVarInfo.iCount/2.0 != m_tVarInfo.iCount/2)
		return FALSE;

	if (AnaSyntax::exist("/NORMALTEST", VOstr) )
	{
		m_bNormalTest = TRUE;
		m_bHistogram = AnaSyntax::findstr("/NORMALTEST", "HISTOGRAM", VOstr);
		if (m_bHistogram)
			m_nDivi = AnaSyntax::getvalue("/NORMALTEST", "HISTOGRAM", VOstr);
		m_bNormalLine = AnaSyntax::findstr("/NORMALTEST", "NOAMALDIS", VOstr);
	}
	else
	{
		m_bNormalTest = FALSE;
		m_bHistogram = FALSE;
		m_bNormalLine = FALSE;
		m_nDivi = 0;
	}

	if (AnaSyntax::findstr("/OPTION", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/OPTION", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;

	return TRUE;
}

CResult *CPairTTest::OnPairTTest(CDataInterface *pDataInterface, string VO)
{
	m_nPairs=1;
	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	m_bWeight=false;
	TDataSet *pWeightArray=NULL;
	//FreeAllMemember();///-------------------�д��ָ�
	CResult *pResult;
	pResult = new CResult("�������T����");
	m_pResult = pResult;

	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}	

	if(m_nWeight>=0)
	{
		m_bWeight=true;	
		m_pDataInterface->SetWeightNumb(m_nWeight);
		m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);
	}
	int nLen=pDataInterface->GetRowCount();
	CTArray<double,double> Data1,Data2;
	CTArray<double,double> Weight;
	for(int i=0;i<m_tVarInfo.iCount/2;i++)
	{
		int column1 = m_tVarInfo.pValue[i*2];
		int column2 = m_tVarInfo.pValue[i*2+1];
		CTString szFldName = m_pDataInterface->GetFieldName(column1);
		szFldName += "-";
		szFldName += m_pDataInterface->GetFieldName(column2);
		GetTwoRelatedData(Data1,Data2,Weight,pWeightArray,column1,column2);
		if(Data1.GetSize()<1)
		{
			Data1.RemoveAll();
			Data2.RemoveAll();
			Weight.RemoveAll();
			CTString str1 = "";
			str1+="��Ч����̫�٣����ܽ����������T���飡";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			m_pRsltElementText->AddString(str1);						
			continue;
		}
		PairTTest(Data1,Data2,Weight,column1,column2,szFldName);
		Data1.RemoveAll();
		Data2.RemoveAll();
		Weight.RemoveAll();
	}
	
	CreateResult();
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
}

void CPairTTest::FreeAllMemember()
{
	m_NameAr.RemoveAll();			//������
	m_MeanSAr.RemoveAll();			//��ֵ
	m_FrequencySAr.RemoveAll();	//Ƶ��
	m_StdDSAr.RemoveAll();			//��׼���
	m_StdEMSAr.RemoveAll();		//��ֵ��׼��

	//����Ա�
	m_PairNameAr.RemoveAll();		//������(���)
	m_FrequencyPAr.RemoveAll();	//Ƶ��
	m_CorrAr.RemoveAll();			//�����
	m_PAr.RemoveAll();				//������
	m_PArGT.RemoveAll();
	m_PArLT.RemoveAll();

	//�����
	m_MeanPAr.RemoveAll();			//��ֵ
	m_StdDPAr.RemoveAll();			//��׼���
	m_StdEMPAr.RemoveAll();		//��ֵ��׼��
	m_LowerAr.RemoveAll();			//����
	m_UpperAr.RemoveAll();			//����
	m_tAr.RemoveAll();				//tֵ
	m_dfStrAr.RemoveAll();			//���ɶ�
	m_P2Ar.RemoveAll();			//������(˫β)
	for(int i=0;i<m_PtrArrayIndex.GetSize();i++)
	{
		delete m_PtrArrayIndex[i];
	}
	m_PtrArrayIndex.RemoveAll();

}

BOOL CPairTTest::PairTTest(CTArray<double,double> &Data1,//��Ա���1����
						   CTArray<double,double> &Data2,//��Ա���2����
						   CTArray<double,double> &Weight,//��Ӧ��Ȩ��
						   int column1,int column2,//��Ӧ��������
						   CTString strName)//��Ա�����
{
	int nSize=Data1.GetSize();
	double Xsum=0,Ysum=0;		//Data1,Data2�ļ�Ȩ���
	double Sx=0,Sy=0;			//Data1��ֵ�ı�׼��,Data2��ֵ�ı�׼��
	double W=0;
	//*************** ����в� start **********************
	if (m_bNormalTest)
	{
		//����Ȩ�غ�//#_S 2005-5-27 zlq
		int nWeightNum =0;
		int i;
		for (i=0;i<nSize ;i++)
			nWeightNum+= Weight[i];
		CTArray<double,double> *pErrorAry = new CTArray<double,double>;
		pErrorAry->SetSize(nWeightNum);
		int nCur = 0;
		for (i =0;i<nSize ;i++)
		{
			int nW = Weight[i];
			double value = Data1[i]-Data2[i];
			for (int j=0;j<nW ;j++)
				pErrorAry->SetAt(nCur++,value);
		}
		m_ErrorAry.Add(pErrorAry);
	}
	//*************** ����в� end **********************
	int i=0;
	for(i=0;i<nSize;i++)
	{
		double Xtemp=Data1[i]*Weight[i];
		Xsum=Xsum+Xtemp;
		Sx=Sx+Data1[i]*Data1[i]*Weight[i];
		double Ytemp=Data2[i]*Weight[i];
		Ysum=Ysum+Ytemp;
		Sy=Sy+Data2[i]*Data2[i]*Weight[i];
		W=W+Weight[i];
	}
	if(W<=1)
	{
		CTString str=strName;
		str+="��Ч������Ϊ1�����ܽ����������T���顣";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return false;
	}

	//ͳ�Ʊ�
	double Xmean=Xsum/W;				//Data1�ľ�ֵ
	double Ymean=Ysum/W;				//Data2�ľ�ֵ
	Sx=(Sx-Xsum*Xsum/W)/(W-1);			//Data1�ı�׼���
	Sy=(Sy-Ysum*Ysum/W)/(W-1);			//Data2�ı�׼���
	Sx=sqrt(Sx);
	Sy=sqrt(Sy);
	double SXmean=Sx/sqrt(W);			//Data1��ֵ�ı�׼��
	double SYmean=Sy/sqrt(W);			//Data2��ֵ�ı�׼��

	//��������
	double Sxy=0;//Э����
	double Sd=0;
	double Sdtemp=0;
	for(i=0;i<nSize;i++)
	{
		double Dtemp=Data1[i]-Data2[i];
		Sd=Sd+Dtemp*Dtemp*Weight[i];
		Sdtemp=Sdtemp+Dtemp*Weight[i];
		Sxy=Sxy+(Data1[i]-Xmean)*(Data2[i]-Ymean)*Weight[i];
	}
	Sd=(Sd-Sdtemp*Sdtemp/W)/(W-1);
	Sd=sqrt(Sd);							//��׼���
	if(fabs(Sd)<MIN)
	{
		CTString str=strName;
		str+="��ֵ�ı�׼��Ϊ0�����ܽ����������T���顣";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return false;
	}

	double Sdmean=Sd/sqrt(W);					//��ֵ�ı�׼��			
	double Dmean=Xmean-Ymean;					//��ֵ�Ĳ�ֵ
	double D=Xsum-Ysum;
	double t=Dmean/Sdmean;						//tֵ
	double df=W-1;								//���ɶ�

	double P=2*(1-tcdf(fabs(t),df));			//������
	double pGT = 1-tcdf(t,df);
	double pLT = tcdf(t,df);

	Sxy=Sxy/(W-1);
	double lower=Dmean-tinv((m_dConfidence+1)/2,df)*Sdmean;	//����
	double upper=Dmean+tinv((m_dConfidence+1)/2,df)*Sdmean;	//����


	//����Ա�
	double r=0;									//���ϵ��
	if(fabs(Sx*Sy)>MIN)
	{
		r=Sxy/(Sx*Sy);
		CTString str;
		str.Format("%.4f",r);
		m_CorrAr.Add(str);
	}
	else
	{
		m_CorrAr.Add(" ");
	}
	double t2=0;
	double P2=0;								//������
	if(fabs(Sx*Sy)>MIN && fabs(r*r-1)>MIN)		//r�����Ҳ�Ϊ1
	{
		t2=r*sqrt((W-2)/(1-r*r));
		P2=2*(1-tcdf(fabs(t2),W-2));
		CTString str;
		str.Format("%.4f",P2);
		m_P2Ar.Add(str);
	}
	else if(fabs(Sx*Sy)>MIN && fabs(r*r-1)<=MIN)
	{
		m_P2Ar.Add("0.0000 ");
	}
	else
	{
		m_P2Ar.Add(" ");
	}
	CTString strW;
	strW.Format("%.0f",W);
	m_FrequencyPAr.Add(strW);

	////��ʽ�����

	//ͳ�Ʊ�
	CTString str;
	str.Format("%s%d   ","�����",m_nPairs);
	int nSpace=str.GetLength();
	CTString strSpace;
	for(i=0;i<nSpace;i++)
		strSpace+=" ";
	strSpace+="       ";
	CTString str1=str;
	str1+=m_pDataInterface->GetFieldName(column1);
	m_NameAr.Add(str1);
	strSpace+=m_pDataInterface->GetFieldName(column2);
	m_NameAr.Add(strSpace);
	m_MeanSAr.Add(Xmean);
	m_MeanSAr.Add(Ymean);
	m_StdDSAr.Add(Sx);
	m_StdDSAr.Add(Sy);
	m_StdEMSAr.Add(SXmean);
	m_StdEMSAr.Add(SYmean);
	CTString strFreq;
	strFreq.Format("%.0f",W);
	m_FrequencySAr.Add(strFreq);
	m_FrequencySAr.Add(strFreq);
	

	//��������
	str+=strName;
	m_PairNameAr.Add(str);
	m_MeanPAr.Add(Dmean);
	m_StdDPAr.Add(Sd);
	m_StdEMPAr.Add(Sdmean);
	m_LowerAr.Add(lower);
	m_UpperAr.Add(upper);
	m_tAr.Add(t);
	CTString strDf;
	strDf.Format("%.0f",df);
	m_dfStrAr.Add(strDf);
	m_PAr.Add(P);
	m_PArGT.Add(pGT);
	m_PArLT.Add(pLT);

	m_nPairs++;
	return true;

}

void CPairTTest::CreateResult()
{
	int nRow=m_NameAr.GetSize();
	if(nRow>0)
	{
		CRsltElementTable * pETable = NULL;
		//ͳ�Ʊ�
		//CTLTable * pTable = NULL;
		//CRsltElement *pETable=NULL;
		CTLTable * pTable = new CTLTable;
		pTable->CreateTable(2, 2);
		pTable->SetTitle("ͳ��");

		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(1,"��ֵ");
		pTable->InsertColumn(2,"Ƶ��");
		pTable->InsertColumn(3,"��׼���");
		pTable->InsertColumn(4,"��ֵ��׼��");
		int i=0;
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_NameAr.GetAt(i),false);
			pTable->SetItemText(i,1,m_MeanSAr.GetAt(i));
			pTable->SetItemText(i,2,m_FrequencySAr.GetAt(i));
			pTable->SetItemText(i,3,m_StdDSAr.GetAt(i));
			pTable->SetItemText(i,4,m_StdEMSAr.GetAt(i));
		}
		pETable=new CRsltElementTable("ͳ��",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);
		
		//����Ա�
		nRow=m_PairNameAr.GetSize();
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,4);
		pTable->SetTitle("�����");

		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(1,"Ƶ��");
		pTable->InsertColumn(2,"���ϵ��");
		pTable->InsertColumn(3,"P ֵ"); 

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_PairNameAr.GetAt(i),false);
			pTable->SetItemText(i,1,m_FrequencyPAr.GetAt(i));
			pTable->SetItemText(i,2,m_CorrAr.GetAt(i));
			pTable->SetItemText(i,3,m_P2Ar.GetAt(i));
		}
		pETable=new CRsltElementTable("�����",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);

		//������
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetTitle("��Լ���ͳ����");

		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(1,"��ľ�ֵ");
		pTable->InsertColumn(2,"��ı�׼���");
		pTable->InsertColumn(3,"��ľ�ֵ��׼��");
		CTString str,str2;
		str.Format("(%d%%���Ŷ�)", int(m_dConfidence*100));
		str2="����"+str;
		pTable->InsertColumn(4,str2);
		str2="����"+str;
		pTable->InsertColumn(5,str2);
		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_PairNameAr.GetAt(i),false);
			pTable->SetItemText(i,1,m_MeanPAr.GetAt(i));
			pTable->SetItemText(i,2,m_StdDPAr.GetAt(i));
			pTable->SetItemText(i,3,m_StdEMPAr.GetAt(i));
			pTable->SetItemText(i,4,m_LowerAr.GetAt(i));
			pTable->SetItemText(i,5,m_UpperAr.GetAt(i));				
		}

		pETable=new CRsltElementTable("��Լ���ͳ����",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);

		pTable = new CTLTable;
		int ncol = m_bNEQ + m_bGT + m_bLT;;
		pTable->CreateTable(nRow+1,3+ncol);
		pTable->SetTitle("������");
		
		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(1,"�����Tֵ");
		pTable->InsertColumn(2,"���ɶ�");

		CTString strtemp1 ;
		int jtemp = 3;
		if (m_bNEQ) 
		{
			pTable->InsertColumn(jtemp,"Pֵ(u1��u2)");
			jtemp++;
		}
		if (m_bGT) 
		{
			pTable->InsertColumn(jtemp,"Pֵ(u1>u2)");
			jtemp++;
		}
		if (m_bLT) 
		{
			pTable->InsertColumn(jtemp,"Pֵ(u1<u2)");
			jtemp++;
		}
		
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_PairNameAr.GetAt(i),FALSE);
			pTable->SetItemText(i,1,m_tAr.GetAt(i));
			pTable->SetItemText(i,2,m_dfStrAr.GetAt(i));
			jtemp = 3;
			if (m_bNEQ) 
			{
				pTable->SetItemText(i,jtemp,m_PAr.GetAt(i));
				jtemp++;
			}
			if (m_bGT)
			{
				pTable->SetItemText(i,jtemp,m_PArGT.GetAt(i));
				jtemp++;
			}
			if (m_bLT)
			{
				pTable->SetItemText(i,jtemp,m_PArLT.GetAt(i));
				jtemp++;
			}
		}
		
		pETable=new CRsltElementTable("������",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);

		//**************************** �в���̬����ͼ START
		for (int iCol=0;iCol<m_ErrorAry.GetSize() ;iCol++)
		{
			////////////////////////////////////////////////////
			CTString szTemp = m_PairNameAr.GetAt(iCol);
			CTString szTitle = szTemp;
			szTitle = szTitle.Mid(szTitle.Find(" "));
			szTitle.TrimLeft();
			////////////////////////////////////////////////////


			CTArray<double,double> *pErrorAry = (CTArray<double,double> *)m_ErrorAry.GetAt(iCol);
			int nRow = pErrorAry->GetSize();
			CDoubleVector AveVect;
			AveVect.create(nRow);
			for( i=0; i<nRow; i++)
			{
				AveVect[i] =pErrorAry->GetAt(i);
			}
			AveVect = Sort(AveVect);
			
			CDoubleVector Norms = GetNormMedians(nRow);
			CDoubleVector Beta = SimpleRegress(AveVect,Norms); 
			if (m_bNormalLine)
			{
				TDataPointValVal* pDPVV = new TDataPointValVal[nRow];
				
				for(i=0; i<nRow; i++)
				{
					pDPVV[i].fXVal = Norms(i);
					pDPVV[i].fYVal = AveVect(i);
				}
				CTChartPP* pPPPlot1 = new CTChartPP(CTChart::PP);	
				pPPPlot1->SetTitle(szTitle + "-��̬����ͼ");
				pPPPlot1->SetXAxilLabel("��̬��λ��");
				pPPPlot1->SetYAxilLabel(szTemp);
				pPPPlot1->SetData(pDPVV,nRow,EScatter,ValVal);
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
				pPPPlot1->SetData(pDPVV,2,ELine,ValVal);
				CRsltElementChart*pREC=new CRsltElementChart(szTemp + "-��̬����ͼ",pPPPlot1);
				m_pResult->Add(pREC);
				delete []pDPVV;
			}
			if (m_bHistogram)
			{
				CTChartHistogram* pTChartHistogram= new CTChartHistogram;
				pTChartHistogram->SetTitle(szTitle + "-ֱ��ͼ");
				pTChartHistogram->SetXAxilLabel(szTitle);
				int nDV = 0;
				void *pDPSV = NULL;
				pTChartHistogram->CalcXYAxis(AveVect,&pDPSV,nDV);
				pTChartHistogram->SetHistogram(pDPSV,nDV,ValVal);
				CRsltElementChart* pREC=new CRsltElementChart(szTemp + "-ֱ��ͼ",pTChartHistogram);
				m_pResult->Add(pREC);
				delete []pDPSV;
            }
			delete pErrorAry ;
		}
		m_ErrorAry.RemoveAll();
		//**************************** �в���̬����ͼ END  */
	}
}
