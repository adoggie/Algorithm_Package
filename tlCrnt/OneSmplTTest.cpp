// OneSmplTTest.cpp: implementation of the COneSmplTTest class.
//                        ��������������
//                    ������ �������ľ�ֵ��������������
//                   ��ע�� ������ 2005��10��21��
//									����ׯ		2006/03/21
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlcrnt.h"
#include "OneSmplTTest.h"


const double MIN=1e-15;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COneSmplTTest::COneSmplTTest()
{
	m_dValue=0.0;
	m_dConfidence=0.95;
}

COneSmplTTest::~COneSmplTTest()
{
	m_tVarInfo.DeleteObject();
}

BOOL COneSmplTTest::GetValueFromVoT(CDataInterface *pDataInterface, string VO)
{
	//ONESAMPLEMEANT
	//	/DATA=[FILE]
	//	/VARLIST [varlist]
	//	/WEIGHT [varname]
	//	/MEANTEST [SIGN({0.95**})] [MEAN({0**})] [{EQU},{LARGER},{LESS}]
	//	/NORMALTEST [HISTOGRAM({0**})] [NOAMALDIS]
	//	/TESTTYPE [{VARUNKNOW**},{VARGIVEN({1})},{STDGIVEN({1})}]
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
		m_bWeight = TRUE;
	}
	else
		m_bWeight = FALSE;
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u �� u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u �� u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u �� u0
	m_dValue = 0.5;
	if (AnaSyntax::findstr("/MEANTEST", "MEAN", VOstr))
		m_dValue = AnaSyntax::getvalue("/MEANTEST", "MEAN", VOstr);
	
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
	if (AnaSyntax::findstr("/MEANTEST", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/MEANTEST", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
	//	/TESTTYPE [{VARUNKNOW**},{VARGIVEN({1})},{STDGIVEN({1})}]
	
	//��������
	//		m_nIsSTD		= pVo->m_nIsSTD;//0:����  1:��׼��
	//		m_dErrValue		= pVo->m_dErrValue; //������߱�׼���ֵ
	if (AnaSyntax::exist("/TESTTYPE", VOstr))
	{
		m_nVarType = int(!AnaSyntax::findstr("/TESTTYPE", "VARUNKNOW", VOstr));
		if (m_nVarType == 1)
		{
			m_nIsSTD = AnaSyntax::findstr("/TESTTYPE", "STDGIVEN", VOstr);
			if (m_nIsSTD ==1)
				m_dErrValue = AnaSyntax::getvalue("/TESTTYPE", "STDGIVEN", VOstr);
			else
				m_dErrValue = AnaSyntax::getvalue("/TESTTYPE", "VARGIVEN", VOstr);
			if (m_dErrValue < 0)
				return FALSE;
		}
	}
	else
	{
		m_nVarType = 0;
	}
	
	return TRUE;
}

//������T����
CResult * COneSmplTTest::OnOneSmplTTest(CDataInterface *pDataInterface, string VO)//�ӿں���
{
	FreeMembers();
	CResult *pResult;
	pResult =new CResult("��������ֵ����");
	m_pResult = pResult;
	
	if (!GetValueFromVoT(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}	
	
	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet * pWeightArray=NULL;
	TDataSet *pDataArray=NULL;
	CTArray<double,double> WeightArray;
	CTArray<double,double> DataArray;
	
	if(m_bWeight)
	{
		m_pDataInterface->SetWeightNumb(m_nWeight); //add by xds ...
		int nRow=m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);
	}
	for(int i=0;i<m_tVarInfo.iCount;i++)
	{
		
		m_nFieldIndex = m_tVarInfo.pValue[i];							//ȡ�ñ������к�
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//ȡ������
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=strName;
			str+="��Ч����̫��,���ܽ��е�����T���飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=strName;
			str+="����Դ̫�󣬲��ܽ��е�����T���飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		for(int j=0;j<nCaseNum;j++)
		{	
			if(m_bWeight)//����ָ����Ȩ��
			{
				if(pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//Ȩ���������о�δȱʧ
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//�����¼�����Ե���
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//����δָ��Ȩ��
			{
				if(pDataArray[j].iIndex>=0)
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(1);
				}
				else
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
		}
		if( pDataArray )//free the memory
		{
			delete [] pDataArray;
			pDataArray=NULL;
		}
		if(DataArray.GetSize()<1)
		{
			DataArray.RemoveAll();
			WeightArray.RemoveAll();
			CTString str=strName;
			str+="û��Ч���ݣ����ܽ��е�����T���飡";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
			continue;
		}
		if(DataArray.GetSize()==1)
		{
			if(WeightArray.GetAt(0)<2)
			{
				DataArray.RemoveAll();
				WeightArray.RemoveAll();
				CTString str=strName;
				str+="��Ч����С��2�������ܽ��е�����T���飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str);
				continue;
			}
		}
		
		Test(DataArray,WeightArray,strName);
		DataArray.RemoveAll();
		WeightArray.RemoveAll();
	}
	if(pWeightArray)//free the memory
	{
		delete [] pWeightArray;
		pWeightArray=NULL;
	}
	CreateResult();
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	WeightArray.RemoveAll();
	return m_pResult;
}

bool COneSmplTTest::Test(CTArray<double,double> &Data,//����
						 CTArray<double,double> &Weight,//Ȩ��
						 CTString strName,int method)//�ֶ��� //method =0 T���飬 method = 1 ������� method =2 ��������
{
	int nSize=Data.GetSize();
	double sum=0;
	double Sx=0;
	double W=0;
	int i=0;
	for(i=0;i<nSize;i++)
	{
		sum=sum+Data[i]*Weight[i];
		W=W+Weight[i];
	}
	if(W<=1)
	{
		CTString str=strName;
		if(method==0)
			str+="��Ч����̫�٣����ܽ��е�����T���顣";
		if(method==1)
			str+="��Ч����̫�٣����ܽ��е�����������顣";
		if(method==2)
			str+="��Ч����̫�٣����ܽ��е������������顣";
		
		if(m_pRsltElementText==NULL)
			m_pRsltElementText =new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return false;
	}
	//*************** ����β� start **********************
	if (m_bNormalTest)
	{
		//����Ȩ�غ�//#_S 2005-5-27 zlq
		int nWeightNum =0;
		for (int i=0;i<nSize ;i++)
			nWeightNum+= Weight[i];
		CTArray<double,double> *pErrorAry = new CTArray<double,double>;
		pErrorAry->SetSize(nWeightNum);
		int nCur = 0;
		for (i =0;i<nSize ;i++)
		{
			int nW = Weight[i];
			double value = Data[i];
			for (int j=0;j<nW ;j++)
				pErrorAry->SetAt(nCur++,value);
		}
		m_ErrorAry.Add(pErrorAry);
	}
	//*************** ����β� end **********************
	
	double Xmean=sum/W;					//��ֵ
	for(i=0;i<nSize;i++)
	{
		double temp=Data[i]-Xmean;
		Sx=Sx+temp*temp*Weight[i];
	}
	
	Sx=sqrt(Sx/(W-1));//��׼��
	double Sxmean=Sx/sqrt(W);					//��ֵ��׼��
	//////////////////////////////////////////////////////////////////////////
	
	if(fabs(Sx)<MIN)
	{
		if(method != 2)
		{
			CTString str="����: " + strName;
			if(method ==0)
				str+="��׼���Ϊ0�����ܽ��е�����T���顣";
			if(method ==1)
				str+="��׼���Ϊ0�����ܽ��е�����������顣";		
			if(m_pRsltElementText==NULL)
				m_pRsltElementText =new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//������T����
	if(method ==0)
	{
		double D=Xmean-m_dValue;
		double t=D/Sxmean;							//t
		//����Uͳ����
		//m_nIsSTD;//0:����  1:��׼��
		//m_dErrValue; //������߱�׼���ֵ
		
		double sigma = 1.0;	
		if(m_nIsSTD == 1)
			sigma = m_dErrValue;
		else
			sigma = sqrt(m_dErrValue);
		double U = D/(sigma /sqrt(W));//Uͳ����
		
		double df=W-1;								//���ɶ�
		double P=2*(1-tcdf(fabs(t),df));			//������
		double lower=Xmean-tinv((m_dConfidence+1)/2,df)*Sxmean;	//����
		double upper=Xmean+tinv((m_dConfidence+1)/2,df)*Sxmean;
		
		//�����
		m_tAr.Add(t);
		m_uAr.Add(U);
		CTString str(""); 
		
		str.Format("%.0f",df);
		m_dfStrAr.Add(str);
		
		m_PAr.Add(P);
		m_DFAr.Add(D);
		m_LowerAr.Add(lower);
		m_UpperAr.Add(upper);
	}
	//////////////////////////////////////////////////////////////////////////
	//���㵥�������������ͳ����
	if(method ==1 )
	{
		double sigma = 1.0;	
		
		if(m_nIsSTD == 1)
			sigma = m_dValue*m_dValue;
		else
			sigma = fabs(m_dValue);//һ��Ҫȷ������Ǹ�
		assert(sigma >0);
		if(sigma <= 0)
			sigma =1.0;
		double SS =Sx*Sx*(W-1);
		double Z = SS/sigma;//����ͳ����
		m_ChiSquareAr.Add(Z);
		double df=W-1;								//���ɶ�
		double Alpha = 1-m_dConfidence;
		
		double upper=SS/chi2inv(Alpha/2.0,df);	//����
		double lower=SS/chi2inv(1.0-Alpha/2.0,df);	//����
		if(m_nCheckType !=0)
		{
			upper=SS/chi2inv(Alpha,df);	//����
			lower=SS/chi2inv(1.0-Alpha,df);	//����		
		}
		CTString str("");
		str.Format("%.0f",df);
		m_dfStrAr.Add(str);
		
		m_LowerAr.Add(lower);
		m_UpperAr.Add(upper);	
	}
	//////////////////////////////////////////////////////////////////////////
	if(method ==2) //��������
	{
		assert(m_dValue > 0 && m_dValue <1);
		double p = sum/W;
		double p0 = m_dValue;
		double Z = (p-p0)/sqrt(p0*(1-p0)/W);
		double Alpha = 1-m_dConfidence;
		double Ualpha = fabs(norminv(Alpha/2.0,0,1));
		double p1=0.0;//p-Ualpha*sqrt(p*(1-p)/W);
		double p2=0.0;//p+Ualpha*sqrt(p*(1-p)/W);
		if(m_nCheckType ==0)
		{
			if(	m_bEquation)
			{
				double a = W + Ualpha*Ualpha;
				double b = 2*sum + Ualpha*Ualpha;
				double c = sum*sum/W;
				double delta = sqrt(fabs(b*b - 4*a*c));
				p1 = (b - delta)/(2.0*a);
				p2 = (b + delta)/(2.0*a);
			}
			else
			{
				p1=p-Ualpha*sqrt(p*(1-p)/W);
				p2=p+Ualpha*sqrt(p*(1-p)/W);
				
			}
		}
		else
		{
			Ualpha = fabs(norminv(Alpha,0,1));
			p1=p-Ualpha*sqrt(p*(1-p)/W);
			p2=p+Ualpha*sqrt(p*(1-p)/W);			
		}
		m_uAr.Add(Z);
		m_LowerAr.Add(p1);
		m_UpperAr.Add(p2);//���Ƶ��������	
	}
	//��ʽ�����
	
	//ͳ�Ʊ�
	CTString str(strName);
	m_NameStrAr.Add(str);
	
	str.Format("%.0f",W);
	m_FrequencyStrAr.Add(str);
	m_MeanAr.Add(Xmean);
	m_StdDAr.Add(Sx);
	m_StdEMAr.Add(Sxmean);
	
	return true;
	
}


void COneSmplTTest::FreeMembers()//�������
{
	m_NameStrAr.RemoveAll();			//������
	m_FrequencyStrAr.RemoveAll();		//Ƶ��
	m_MeanAr.RemoveAll();				//��ֵ
	m_StdDAr.RemoveAll();				//��׼���
	m_StdEMAr.RemoveAll();				//��ֵ��׼��
	
	//���ڼ����
	m_tAr.RemoveAll();					//t
	m_uAr.RemoveAll();                  //u
	m_dfStrAr.RemoveAll();				//���ɶ�
	m_PAr.RemoveAll();					//������
	m_DFAr.RemoveAll();					//����
	m_LowerAr.RemoveAll();				//����
	m_UpperAr.RemoveAll();				//����
	m_ChiSquareAr.RemoveAll();
}


void COneSmplTTest::CreateResult(int method/* =0 */)//��ʽ�����
{
	int nRow=m_NameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
		CRsltElementTable * pETable = NULL;
		
		int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->InsertColumn(0,"���˵��");
			//			pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s����ֵ�������ľ�ֵ���������(���ʼ���ֵ=%.4f)��\n",m_NameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				else
				{
					strtemp.Format("%s����ֵ�������ľ�ֵ���������(���ʼ���ֵ=%.4f)��\n",m_NameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("���˵��",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
			
		}
		// to end the output discription.
		
		
		
		//ͳ�Ʊ�
		if(method != 2)//���������ͳ�Ʊ������
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,5);
			pTable->SetTitle("ͳ��");
			CTString strValue;
			strValue.Format("����ֵ=%.2f",m_dValue);
			pTable->SetHead(strValue);
			//		pTable->SetCols(5);
			//		pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"������");
			pTable->InsertColumn(1,"��¼��");
			pTable->InsertColumn(2,"��ֵ");
			pTable->InsertColumn(3,"��׼���");
			pTable->InsertColumn(4,"��ֵ��׼��");
			for(int i=0;i<nRow;i++)
			{
				pTable->InsertItem(i,m_NameStrAr.GetAt(i),false);
				pTable->SetItemText(i,1,m_FrequencyStrAr.GetAt(i));
				pTable->SetItemText(i,2,m_MeanAr.GetAt(i));
				pTable->SetItemText(i,3,m_StdDAr.GetAt(i));
				pTable->SetItemText(i,4,m_StdEMAr.GetAt(i));
			}
			pETable=new CRsltElementTable("ͳ��",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
		}
		//�����
		if(method ==0)
		{
			CTLTable* pTable = new CTLTable;
			pTable->CreateTable(nRow+1,7);
			pTable->SetTitle("T������");
			//		pTable->SetCols(7);
			//			pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"������");
			pTable->InsertColumn(1,"Tͳ����");
			pTable->InsertColumn(2,"���ɶ�");
			pTable->InsertColumn(3,"Pr(>|T|)");
			//pTable->InsertColumn(4,"u-u0");
			CTString str,str2;
			str.Format("(%d%%���Ŷ�)", int(m_dConfidence*100));
			str2="����"+str;
			pTable->InsertColumn(4,str2);
			str2="����"+str;
			pTable->InsertColumn(5,str2);
			for(int i=0;i<nRow;i++)
			{
				pTable->InsertItem(i,m_NameStrAr.GetAt(i),false);
				pTable->SetItemText(i,1,m_tAr.GetAt(i));
				pTable->SetItemText(i,2,m_dfStrAr.GetAt(i));
				pTable->SetItemText(i,3,m_PAr.GetAt(i));
				//	pTable->SetItemText(i,4,m_DFAr.GetAt(i));
				pTable->SetItemText(i,4,m_LowerAr.GetAt(i));
				pTable->SetItemText(i,5,m_UpperAr.GetAt(i));
				CreateTestTable(i);//�����������
			}
			
			pETable=new CRsltElementTable("������",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
			//////////////////////////////////////////////////////////////////////////
		}	
		if(method ==1)//�����������
		{
			for(int i=0;i<nRow;i++)
				CreateChiTestTable(i);//�����������		
		}
		if(method ==2)//������������
		{
			//���ͳ�Ʊ�
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,6);
			pTable->SetTitle("ͳ�Ʊ�");
			CTString strValue;
			strValue.Format("����ֵ=%.2f",m_dValue);
			pTable->SetHead(strValue);
			//			pTable->SetCols(6);
			//			pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"������");
			pTable->InsertColumn(1,"��  ��");
			pTable->InsertColumn(2,"Ƶ  ��");
			pTable->InsertColumn(3,"Ƶ  ��");
			pTable->InsertColumn(4,"��׼���");
			pTable->InsertColumn(5,"��ֵ��׼��");
			int i;
			for(i=0;i<nRow;i++)
			{
				pTable->InsertItem(i,m_NameStrAr.GetAt(i),false);
                int iCount = atoi(m_FrequencyStrAr.GetAt(i));
				strValue.Format("0 \n 1 ");
				pTable->SetItemText(i,1,strValue);
				strValue.Format("%d \n %d ",(int)(iCount-m_MeanAr.GetAt(i)*iCount),(int)(m_MeanAr.GetAt(i)*iCount));
				pTable->SetItemText(i,2,strValue);
				strValue.Format("%.4f \n %.4f ",(1-m_MeanAr.GetAt(i)),m_MeanAr.GetAt(i));
				pTable->SetItemText(i,3,strValue);
				pTable->SetItemText(i,4,m_StdDAr.GetAt(i));
				pTable->SetItemText(i,5,m_StdEMAr.GetAt(i));
			}
			pETable=new CRsltElementTable("ͳ��",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
			
			for(i=0;i<nRow;i++)
				CreatePropotionTestTable(i);//�����������		
		}
		
		CreateCharts();
		
	}
	
}
void COneSmplTTest::CreateCharts()
{
	//**************************** �в���̬����ͼ START
	int iCol;
	for (iCol=0;iCol<m_ErrorAry.GetSize() ;iCol++)
	{
		CTArray<double,double> *pErrorAry = (CTArray<double,double> *)m_ErrorAry.GetAt(iCol);
		int nRow = pErrorAry->GetSize();
		CDoubleVector AveVect;
		AveVect.create(nRow);
		int i;
		for(i=0; i<nRow; i++)
		{
			AveVect[i] =pErrorAry->GetAt(i);
		}
		AveVect = Sort(AveVect);
		
		CDoubleVector Norms = GetNormMedians(nRow);
		CDoubleVector Beta = SimpleRegress(AveVect,Norms); 
		
		if (m_bNormalLine)
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[nRow];
			
			for(i=0; i<nRow; i++)
			{
				pDPVV[i].fXVal = Norms(i);
				pDPVV[i].fYVal = AveVect(i);
			}
			CTChartPP* pPPPlot1 = new CTChartPP(CTChart::PP);	
			pPPPlot1->SetTitle(m_NameStrAr.GetAt(iCol) + "-��̬����ͼ");
			pPPPlot1->SetXAxilLabel("��̬��λ��");
			pPPPlot1->SetYAxilLabel(m_NameStrAr.GetAt(iCol));
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
			CRsltElementChart*pREC=new CRsltElementChart(m_NameStrAr.GetAt(iCol) + "-��̬����ͼ",pPPPlot1);
			m_pResult->Add(pREC);
			delete []pDPVV;
		}
		if (m_bHistogram)
		{		
			int nDV=0, nPtCnt=0;
			double fMean=0, fStdv=0, fSum=0;
			void *pDPSV1=NULL, *pDPSV2=NULL;
			CTString szTemp("");
			//����
			CDoubleVector v = AveVect;
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
			pHistogram->SetXAxilLabel(m_NameStrAr.GetAt(iCol));
			pHistogram->SetYAxilLabel("Ƶ��");
			pHistogram->SetTitle(m_NameStrAr.GetAt(iCol) + "-ֱ��ͼ");
			CRsltElementChart *pChart = new CRsltElementChart(m_NameStrAr.GetAt(iCol) + "-ֱ��ͼ",pHistogram);
			m_pResult->Add(pChart);
			delete []pDPSV1;
			delete []pDPSV2;
		}
		delete pErrorAry ;
	}
	m_ErrorAry.RemoveAll();
	//**************************** �в���̬����ͼ END  */
	
}
void COneSmplTTest::CreatePropotionTestTable(int index)
{
	CRsltElementTable * pETable = NULL;
	CTLTable * table = new CTLTable;
	assert(table != NULL);	
	
	CTString sTitle("");
	int nRow = m_bGT + m_bLT + m_bNE;
	if(nRow ==0)
		return ;
	table->CreateTable(nRow+1,5);
	
    CTString str,str2;
	int iCount =0;
	double dbVar = 0;
	sTitle.Format("ԭ���� : H0 : p �� %.4f",m_dValue);
	sTitle = m_NameStrAr.GetAt(index) +"��������������\n"+sTitle;
	table->SetTitle(sTitle);
	//	table->SetCols(5);
	//	table->SetRows(nRow+1);
	
	
	table->InsertColumn(0,"�������");
	//table->InsertColumn(1,"p-p0");
	table->InsertColumn(1,"Zͳ����");
	table->InsertColumn(2,"P ֵ");
	
	if(m_nCheckType == 0)
		str.Format("p��%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType == 1)
		str.Format("p��%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType == 2)
		str.Format("p��%d%%��������", int(m_dConfidence*100));
	table->InsertColumn(3,str);
	
	double Z =m_uAr.GetAt(index); //
	if(m_bGT)// H1 : p �� p0
	{
		str.Format("H1 : p �� %.4f",m_dValue);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_MeanAr.GetAt(index) - m_dValue);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,1-normcdf(Z,0,1));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , 1)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}
	
	if(m_bLT)// H1 : p �� p0
	{
		str.Format("H1 : p �� %.4f",m_dValue);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_MeanAr.GetAt(index) - m_dValue);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,normcdf(Z,0,1));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , 1)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}
	
	if(m_bNE)// H1 : p �� p0
	{
		str.Format("H1 : p �� %.4f",m_dValue);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_MeanAr.GetAt(index) - m_dValue);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,2*(1-normcdf(fabs(Z),0,1)));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , 1)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}			
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);
}
void COneSmplTTest::CreateChiTestTable(int index)
{
	CRsltElementTable * pETable = NULL;
	CTLTable * table = new CTLTable;
	assert(table != NULL);	
	
	CTString sTitle("");
	int nRow = m_bGT + m_bLT + m_bNE;
	if(nRow ==0)
		return ;
	table->CreateTable(nRow+1,6);
	
    CTString str,str2;
	int iCount =0;
	double dbVar = 0;
	if(m_nIsSTD == 1)
		dbVar = m_dValue*m_dValue;
	else
		dbVar = m_dValue;
	
	sTitle.Format("ԭ���� : H0 : Var �� %.4f,��ֵδ֪ ",dbVar);
	sTitle = m_NameStrAr.GetAt(index) +"�������������\n"+sTitle;
	table->SetTitle(sTitle);
	//	table->SetCols(6);
	//	table->SetRows(nRow+1);
	
	
	table->InsertColumn(0,"�������");
	//	table->InsertColumn(1,"Var/Var0");
	table->InsertColumn(1,"����ͳ����");
	table->InsertColumn(2,"���ɶ�");
	table->InsertColumn(3,"P ֵ");
	
	if(m_nCheckType == 0)
		str.Format("�����%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType == 1)
		str.Format("�����%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType == 2)
		str.Format("�����%d%%��������", int(m_dConfidence*100));
	
	table->InsertColumn(4,str);
	
	double K =m_ChiSquareAr.GetAt(index); //
	if(m_bGT)// H1 : Var �� Var0
	{
		str.Format("H1 : Var �� %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//m_dfStrAr
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		table->SetItemText(iCount,3,min(1-chi2cdf(K,atoi(m_dfStrAr.GetAt(index))),1));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +��)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		
		iCount++;
	}
	
	if(m_bLT)// H1 : Var �� Var0
	{
		str.Format("H1 : Var �� %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		
		table->SetItemText(iCount,3,chi2cdf(K,atoi(m_dfStrAr.GetAt(index))));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +��)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		
		iCount++;
	}
	
	if(m_bNE)// H1 : Var �� Var0
	{
		str.Format("H1 : Var �� %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		
		table->SetItemText(iCount,3,min(2*(1-chi2cdf(fabs(K),atoi(m_dfStrAr.GetAt(index)))),2*chi2cdf(fabs(K),atoi(m_dfStrAr.GetAt(index)))));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +��)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		iCount++;
	}			
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);
	
	
	
}
void COneSmplTTest::CreateTestTable(int index)
{
	CRsltElementTable * pETable = NULL;
	CTLTable * table = new CTLTable;
	assert(table != NULL);	
	
	CTString sTitle("");
	int nRow = m_bGT + m_bLT + m_bNE;
	if(nRow ==0)
		return ;
	table->CreateTable(nRow+1,4);
	
    CTString str,str2;
	int iCount =0;
	if(m_nVarType == 1)//������֪
	{
		double dbVar = 0;
		if(m_nIsSTD == 1)
			dbVar = m_dErrValue*m_dErrValue;
		else
			dbVar = m_dErrValue;
		sTitle.Format("ԭ���� : H0 : u �� %.2f�� ��֪���� = %.4f",m_dValue,dbVar);
		sTitle = m_NameStrAr.GetAt(index) +"����ֵ��������\n"+sTitle;
		table->SetTitle(sTitle);
		//		table->SetCols(4);
		//		table->SetRows(nRow+1);
		
		
		table->InsertColumn(0,"�������");
		//table->InsertColumn(1,"u-u0");
		table->InsertColumn(1,"Zͳ����");
		table->InsertColumn(2,"P ֵ");
		//��ʼ�����
		
		double T =m_uAr.GetAt(index); //
		
		if(m_bGT)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,1-normcdf(T,0,1));
			iCount++;
		}
		
		if(m_bLT)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,normcdf(T,0,1));
			
			iCount++;
		}
		
		if(m_bNE)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,2*(1-normcdf(fabs(T),0,1)));
			
			iCount++;
		}			
	}
	else//����δ֪
	{
		sTitle.Format("ԭ���� : H0 : u �� %.2f�� ����δ֪",m_dValue);
		table->SetTitle(sTitle);
		table->CreateTable(nRow+1,5);
		//		table->SetCols(5);
		//		table->SetRows(nRow+1);
		
		
		table->InsertColumn(0,"�������");
		//table->InsertColumn(1,"u-u0");
		table->InsertColumn(1,"Tͳ����");
		table->InsertColumn(2,"���ɶ�");
		
		table->InsertColumn(3,"P ֵ");
		
		double T =m_tAr.GetAt(index);//Tͳ����
		int df =atoi(m_dfStrAr.GetAt(index));
		
		if(m_bGT)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,df);
			
			table->SetItemText(iCount,3,1-tcdf(T,df));
			
			iCount++;
		}
		
		if(m_bLT)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
			
			table->SetItemText(iCount,3,tcdf(T,df));
			
			iCount++;
		}
		
		if(m_bNE)// H1 : u �� u0
		{
			str.Format("H1 : u �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
			
			table->SetItemText(iCount,3,2*(1-tcdf(fabs(T),df)));
			
			iCount++;
		}
		
	}
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index)+ "������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);	
}
//////////////////////////////////////////////////////////////////////////
//1�������ĵ���������
BOOL COneSmplTTest::GetValueFromVoPro(CDataInterface *pDataInterface, string VO)
{
	//	ONESAMPLEPROP
	//		/DATA=[FILE]
	//		/VARLIST [varlist]
	//		/WEIGHT [varname]
	//		/MEANTEST [SIGN({0.95**})] [PROP({0.5**})] [{EQU},{LARGER},{LESS}]
	// 		/SIGNRANGE [{DOUBLER},{DFUNCTION},{UPPER},{LOWER}]
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
		m_bWeight = TRUE;
	}
	else
		m_bWeight = FALSE;
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u �� u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u �� u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u �� u0
	if (AnaSyntax::findstr("/MEANTEST", "PROP", VOstr))
		m_dValue = AnaSyntax::getvalue("/MEANTEST", "PROP", VOstr);
	else
		m_dValue = 0.5;
	if (AnaSyntax::findstr("/MEANTEST", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/MEANTEST", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
	
	if (AnaSyntax::findstr("/SIGNRANGE", "UPPER", VOstr))
		m_nCheckType = 2;
	else if (AnaSyntax::findstr("/SIGNRANGE", "LOWER", VOstr))
		m_nCheckType = 1;
	else
	{
		m_nCheckType = 0;
		m_bEquation = AnaSyntax::findstr("/SIGNRANGE", "DFUNCTION", VOstr);
	}
	m_bNormalTest = FALSE;
	m_bHistogram = FALSE;
	m_bNormalLine = FALSE;
	m_nDivi = 0;
	
	return TRUE;
}

CResult* COneSmplTTest::OnOneSmplPropotionTest(CDataInterface *pDataInterface, string VO)//��������ӿں���
{
	FreeMembers();
	
	CResult *pResult;
	pResult =new CResult("��������������");
	m_pResult = pResult;
	
	if (!GetValueFromVoPro(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet * pWeightArray=NULL;
	TDataSet *pDataArray=NULL;
	CTArray<double,double> WeightArray;
	CTArray<double,double> DataArray;
	
	if(m_bWeight)
	{
		m_pDataInterface->SetWeightNumb(m_nWeight); //add by xds 20060628
		int nRow=m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);
	}
	for(int i=0;i<m_tVarInfo.iCount;i++)
	{
		
		m_nFieldIndex = m_tVarInfo.pValue[i];							//ȡ�ñ������к�
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//ȡ������
		//assert( nCaseNum > 0 );
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=strName;
			str+="��Ч����̫��,���ܽ��е�����T���飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=strName;
			str+="����Դ̫�󣬲��ܽ��е������������飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		for(int j=0;j<nCaseNum;j++)
		{	
			if(m_bWeight)//����ָ����Ȩ��
			{
				
				if(pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//Ȩ���������о�δȱʧ
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//�����¼�����Ե���
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//����δָ��Ȩ��
			{
				if(pDataArray[j].iIndex>=0)
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(1);
				}
				else
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
		}
		if( pDataArray )//free the memory
		{
			delete [] pDataArray;
			pDataArray=NULL;
		}
		if(DataArray.GetSize()<1)
		{
			DataArray.RemoveAll();
			WeightArray.RemoveAll();
			CTString str=strName;
			str+="û��Ч���ݣ����ܽ��е�����T���飡";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
			continue;
		}
		if(DataArray.GetSize()==1)
		{
			if(WeightArray.GetAt(0)<2)
			{
				DataArray.RemoveAll();
				WeightArray.RemoveAll();
				CTString str=strName;
				str+="��Ч����С��2�������ܽ��е������������飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str);
				continue;
			}
		}
		
		Test(DataArray,WeightArray,strName,2);
		DataArray.RemoveAll();
		WeightArray.RemoveAll();
	}
	if(pWeightArray)//free the memory
	{
		delete [] pWeightArray;
		pWeightArray=NULL;
	}
	CreateResult(2);
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	WeightArray.RemoveAll();
	return m_pResult;
}

//2������ĵ���������
BOOL COneSmplTTest::GetValueFromVoVar(CDataInterface *pDataInterface, string VO)
{
	//	ONESAMPLEVAR
	//		/DATA=[FILE]
	//		/VARLIST [varlist]
	//		/WEIGHT [varname]
	//		/MEANTEST [SIGN({0.95**})] [VAR({0**})] [{EQU},{LARGER},{LESS}]
	//		/NORMALTEST [HISTOGRAM({0**})] [NOAMALDIS]
	// 		/SIGNRANGE [{DOUBLER},{UPPER},{LOWER}]
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
		m_bWeight = TRUE;
	}
	else
		m_bWeight = FALSE;
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u �� u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u �� u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u �� u0
	
	if (AnaSyntax::findstr("/MEANTEST", "VAR", VOstr))
		m_dValue = AnaSyntax::getvalue("/MEANTEST", "VAR", VOstr);
	else
		m_dValue = 0.5;
	if (AnaSyntax::findstr("/MEANTEST", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/MEANTEST", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
	
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
	if (AnaSyntax::findstr("/SIGNRANGE", "UPPER", VOstr))
		m_nCheckType = 2;
	else if (AnaSyntax::findstr("/SIGNRANGE", "UPPER", VOstr))
		m_nCheckType = 1;
	else
		m_nCheckType = 0;
	
	return TRUE;
}

CResult* COneSmplTTest::OnOneSmplVarianceTest(CDataInterface *pDataInterface, string VO)//�ӿں���
{
	FreeMembers();
	
	CResult *pResult;
	pResult =new CResult("�������������");
	m_pResult = pResult;
	
	if (!GetValueFromVoVar(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet * pWeightArray=NULL;
	TDataSet *pDataArray=NULL;
	CTArray<double,double> WeightArray;
	CTArray<double,double> DataArray;
	if(m_bWeight)
	{
		m_pDataInterface->SetWeightNumb(m_nWeight);//add by xds 20060628
		int nRow=m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);
	}
	for(int i=0;i<m_tVarInfo.iCount;i++)
	{
		m_nFieldIndex = m_tVarInfo.pValue[i];							//ȡ�ñ������к�
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//ȡ������
		//assert( nCaseNum > 0 );
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=strName;
			str+="��Ч����̫��,���ܽ��е������������飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str="����"+strName;
			str+="����Դ̫�󣬲��ܽ��е������������飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		for(int j=0;j<nCaseNum;j++)
		{	
			if(m_bWeight)//����ָ����Ȩ��
			{
				
				if(pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//Ȩ���������о�δȱʧ
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//�����¼�����Ե���
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//����δָ��Ȩ��
			{
				if(pDataArray[j].iIndex>=0)
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(1);
				}
				else
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
		}
		if( pDataArray )//free the memory
		{
			delete [] pDataArray;
			pDataArray=NULL;
		}
		if(DataArray.GetSize()<1)
		{
			DataArray.RemoveAll();
			WeightArray.RemoveAll();
			CTString str=strName;
			str+="û��Ч���ݣ����ܽ��е������������飡";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
			continue;
		}
		if(DataArray.GetSize()==1)
		{
			if(WeightArray.GetAt(0)<2)
			{
				DataArray.RemoveAll();
				WeightArray.RemoveAll();
				CTString str=strName;
				str+="��Ч����С��2�������ܽ��е������������飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str);
				continue;
			}
		}
		
		Test(DataArray,WeightArray,strName,1);
		DataArray.RemoveAll();
		WeightArray.RemoveAll();
	}
	if(pWeightArray)//free the memory
	{
		delete [] pWeightArray;
		pWeightArray=NULL;
	}
	CreateResult(1);
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	WeightArray.RemoveAll();
	return m_pResult;
}
//////////////////////////////////////////////////////////////////////////





















