// TTest.h: interface for the CTTest class.
//               ����˫�����ļ������
//                  ��ע�������� 2005��10��20��
//					�޸ģ�����ׯ 2006/03/28
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TTest.h"

const double MIN=1e-15;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTTest::CTTest()
{
	m_nGroupIndex=-1;
	m_nGroupMethod=0;
	
	m_dValue1=0;
	m_dValue2=1;
	m_dConfidence=0.95;
}

CTTest::~CTTest()
{

}

//******************************************************************************//
//Original Orthor:zkc														    //
//Date:2003.3.13															    //
//Function:�ӿں�����������������ã������ݽ���Ԥ����						    //
//Points:											    //
//******************************************************************************//

BOOL CTTest::GetValueFromVoT(CDataInterface *pDataInterface, string VO)
{
//	TWOSAMPLEMEAN
//		/DATA=[FILE]
//		/PAIREDVAR [varname varname] [varname varname] ...
//		/SELFVAR [varlist]
//		/BYVAR [varname]
//		/GROUP [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
//		/WEIGHT [varname]
//		/MEANTEST [SIGN({0.95**})] [DIF({0**})] [{EQU},{LARGER},{LESS}]
//		/METHOD [LEVENE] [FTEST] [BARTLETT]
//		/VARUNNO [VAREQU] [VARNOTEQU]
//		/VARKNOWN [STDDEV({1} {1})] [VAR({1} {1})]
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
	if (AnaSyntax::exist("/PAIREDVAR", VOstr) && 
		!AnaSyntax::exist("/SELFVAR", VOstr) && 
		!AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/PAIREDVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (int(m_tVarInfo.iCount/2)*2 != m_tVarInfo.iCount)
			return FALSE;
		m_bGroup = FALSE;
	}
	else if (AnaSyntax::exist("/SELFVAR", VOstr) && AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/BYVAR", VOstr, xname);
//		/GROUP [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (m_tVarInfo.iCount == 1)
			m_nGroupIndex = m_tVarInfo.pValue[0];
		else
			return FALSE;
		m_nGroupMethod = int(AnaSyntax::findstr("/GROUP", "SEPVALUE", VOstr));
		if (m_nGroupMethod == 0)
		{
			m_dValue1 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr);
			m_dValue2 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr, 2);
		}
		else
			m_dBoundary = AnaSyntax::getvalue("/GROUP", "SEPVALUE", VOstr);
		
		xname.RemoveAll();
		AnaSyntax::getVAR("/SELFVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		m_bGroup = TRUE;
	}
	else
		return FALSE;
//		/MEANTEST [SIGN({0.95**})] [DIF({0**})]
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);
	if(AnaSyntax::findstr("/MEANTEST", "DIF", VOstr))
		m_dValue = AnaSyntax::getvalue("/MEANTEST", "DIF", VOstr);
	else
		m_dValue = 0;
	if (AnaSyntax::findstr("/MEANTEST", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/MEANTEST", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
//		/METHOD [LEVENE] [FTEST] [BARTLETT]
	m_bLevene = AnaSyntax::findstr("/METHOD", "LEVENE", VOstr);
	m_bF = AnaSyntax::findstr("/METHOD", "FTEST", VOstr);
	m_bBartlett = AnaSyntax::findstr("/METHOD", "BARTLETT", VOstr);
//		/VARUNNO [{VAREQU},{VARNOTEQU}]
	if (AnaSyntax::exist("/VARUNNO", VOstr) && !AnaSyntax::exist("/VARKNOWN", VOstr))
	{
		m_nCheckType = 0;
		m_nIsVarNE = int(AnaSyntax::findstr("/VARUNNO", "VARNOTEQU", VOstr));
	}
//		/VARKNOWN [STDDEV({1} {1})] [VAR({1} {1})]
	else if (AnaSyntax::exist("/VARKNOWN", VOstr))
	{
		m_nCheckType = 1;
		m_nIsStdType = int(AnaSyntax::findstr("/VARKNOWN", "STDDEV", VOstr));
		if (m_nIsStdType == 1)
		{
			m_dSigma1 = AnaSyntax::getvalue("/VARKNOWN", "STDDEV", VOstr);
			m_dSigma2 = AnaSyntax::getvalue("/VARKNOWN", "STDDEV", VOstr, 2);
		}
		else
		{
			m_dSigma1 = AnaSyntax::getvalue("/VARKNOWN", "VAR", VOstr);
			m_dSigma2 = AnaSyntax::getvalue("/VARKNOWN", "VAR", VOstr, 2);
		}
	}
	else
		return FALSE;

	return TRUE;
}

CResult *CTTest::OnTTest(CDataInterface *pDataInterface, string VO)
{
	m_pResult=new CResult("��ֵ��˫��������");
	if (!GetValueFromVoT(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet *pWeightArray=NULL;
	if(m_bWeight)
		m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);

	TDataSet *pDataSet=NULL;
	TDataSet *pGroupSet=NULL;
	CTArray<double,double> V1,V2;
	CTArray<double,double> Weight1,Weight2;
	if (m_bGroup)
	{
		int nRow=m_pDataInterface->GetColData(m_nGroupIndex,pGroupSet,5);
		FreeMemebers();

		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nIndex=m_tVarInfo.pValue[i];
			GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
			CTString str=pDataInterface->GetFieldName(nIndex);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{
				CTString str1="����: "+ str;
				str1+="�����Ժ���Ч����̫��,���ܽ��ж�������T���飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			
			TestMain(V1,V2,Weight1,Weight2,nIndex,0);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}
	else
	{
		FreeMemebers();
		for(int i=0;i<m_tVarInfo.iCount/2;i++)
		{
			int nIndex1 = m_tVarInfo.pValue[2*i];
			int nIndex2 = m_tVarInfo.pValue[2*i+1];
			int nRow = m_pDataInterface->GetColData(nIndex1,pGroupSet,5);
			CTString sName1,sName2;
			sName1 = m_pDataInterface->GetFieldName(nIndex1);
			int kk;
			for (kk = 0; kk< nRow;kk++)
			{
				if (pGroupSet[kk].iIndex >= 0)
				{
					Weight1.Add(1);
					V1.Add(pGroupSet[kk].dValue);
				}
			}
			nRow = m_pDataInterface->GetColData(nIndex2,pWeightArray,5);
			sName2 = m_pDataInterface->GetFieldName(nIndex2);
			if(!m_bGroup) //������
			{
			  CTString sTemp;
			  sTemp.Format(sName1 + "\n" + sName2);
		      m_GroupStrAr.Add(sTemp);
			}
			for (kk = 0; kk< nRow;kk++)
			{
				if (pWeightArray[kk].iIndex >= 0)
				{
					Weight2.Add(1);
					V2.Add(pWeightArray[kk].dValue);
				}
			}
			CTString str=pDataInterface->GetFieldName(nIndex1);
			str += "--";
			str += pDataInterface->GetFieldName(nIndex2);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{
				CTString str1="����: "+ str;
				str1+="�����Ժ���Ч����̫��,���ܽ��о�ֵ��˫����T���飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			m_strPair = str;
			TestMain(V1,V2,Weight1,Weight2,nIndex1,0);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}
	CreateResult(0);
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
}

void CTTest::FreeMemebers()
{
	 m_NameStrAr.RemoveAll();		//������
	 m_GroupStrAr.RemoveAll();		//�����־
	 m_FrequencyStrAr.RemoveAll();	//Ƶ��
	 m_MeanStrAr.RemoveAll();		//��ֵ
	 m_StdDStrAr.RemoveAll();		//��׼���
	 m_StdMStrAr.RemoveAll();		//��ֵ��׼��

	//������ͳ�����ݱ����ݵĴ洢
	 m_CommentStrAr.RemoveAll();	//��ڶ���˵��

	 m_LStrAr.RemoveAll();	//Leveneֵ
	 m_LdfStrAr.RemoveAll();	//Leveneֵ ���ɶ�

	 m_FStrAr.RemoveAll();//Fֵ
     m_FdfStrAr.RemoveAll();//Fֵ���ɶ�
	 m_FPStrAr.RemoveAll();//������
	 
	 m_BStrAr.RemoveAll();	//Bartlettֵ
	 m_BdfStrAr.RemoveAll(); //bartlett ���ɶ� 
	 m_BPStrAr.RemoveAll();	//Bartlettֵ������
	 
	 m_ZStrAr.RemoveAll();	//U����ͳ����ֵ
	 m_EtStrAr.RemoveAll();	//T����ͳ����ֵ//�������ʱ
	 m_EtdefStrAr.RemoveAll();	//�������ʱ���ɶ�
	 
	 m_UtStrAr.RemoveAll();	//T����ͳ����ֵ//�����
	 m_UtdefStrAr.RemoveAll();//�������ʱ���ɶ�
	 
	 m_PStrAr.RemoveAll();	//������
	 m_tStrAr.RemoveAll();			//tֵ
	 m_dfStrAr.RemoveAll();			//���ɶ�
	 m_P2StrAr.RemoveAll();			//�����ԣ�˫β��
	 m_MeanDStrAr.RemoveAll();		//��ֵ��
	 m_StdEDStrAr.RemoveAll();		//��ı�׼��
	 m_LowerStrAr.RemoveAll();		//��������
	 m_UpperStrAr.RemoveAll();		//��������

}



//******************************************************************************//
//Original Orthor:zkc														    //
//Date:2003.3.14															    //
//Function:������Ҫ����ĸ���ͳ����������ʽ����Ҫ����ı���					    //
//Points:																	    //
//******************************************************************************//

BOOL CTTest::TestMain(CTArray<double,double> &Data1,		//��1����
					  CTArray<double,double> &Data2,		//��2����
					  CTArray<double,double> &Weight1,	//��1���ݶ�Ӧ��Ȩ��
					  CTArray<double,double> &Weight2,	//��2���ݶ�Ӧ��Ȩ��
					  int nIndex,int method)						//��ǰ�����ֶε��к�
{
	CTString strName=m_pDataInterface->GetFieldName(nIndex);
	double w1=0,w2=0;					//�ֱ��ʾ��1����2���ݵ�������
	double sum1=0,sum2=0;				//�ֱ��ʾ��1����2���ݼ�Ȩ���
	int nSize1=Data1.GetSize();			//δ����Ȩ��ʱ��1���ݳ���
	int nSize2=Data2.GetSize();			//δ����Ȩ������2���ݳ���
	double S1Sqr=0;						//��1��׼���ƽ��
	double S2Sqr=0;						//��2��׼���ƽ��
	int i=0;
	for(i=0;i<nSize1;i++)
	{
		w1=w1+Weight1[i];
		sum1=sum1+Data1[i]*Weight1[i];
		S1Sqr=S1Sqr+Data1[i]*Data1[i]*Weight1[i];
	}
	double S1=0;
	double SEM1=0;
	if(w1<=1)
	{
		CTString str="����: "+ m_pDataInterface->GetFieldName(nIndex);
		str+="�����Ժ�ĳһ�����Ч���ݹ۲���̫�٣�<=1����������˫�����ļ���";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText = new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return false;
	}
	else
	{
		S1Sqr=(S1Sqr-sum1*sum1/w1)/(w1-1);
		S1=sqrt(S1Sqr);
		SEM1=S1/sqrt(w1);			//��1�ľ�ֵ�����
	}
	double XMean1=sum1/w1;				//��1���ݼ�Ȩƽ��ֵ	

	for(i=0;i<nSize2;i++)
	{
		w2=w2+Weight2[i];
		sum2=sum2+Data2[i]*Weight2[i];
		S2Sqr=S2Sqr+Data2[i]*Data2[i]*Weight2[i];
	}
	double S2=0;
	double SEM2=0;
	if(w2<=1)
	{
		CTString str="����: "+ strName;
		str+="�����Ժ�ĳһ�����Ч���ݹ۲���̫�٣�<=1����������˫�����ļ���";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText = new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		return false;
	}
	else
	{
		S2Sqr=(S2Sqr-sum2*sum2/w2)/(w2-1);
		S2=sqrt(S2Sqr);				//��2��׼���
		SEM2=S2/sqrt(w2);			//��2�ľ�ֵ��׼��
	}
	//�жϷ����Ƿ�Ϊ��
	BOOL bVarTest = true;
	if(S1Sqr*S2Sqr<=0)
	{
		if(method == 0 ||method == 1 )
		{
			CTString str("");
			if(method == 0)
			{
				str="����: "+ strName;
				str+="�����Ժ�����һ�鷽��Ϊ0,���ܽ��о�ֵT����";
			}
			if(method == 1)
			{
				str="����: "+ strName;
				str+="�����Ժ�����һ�鷽��Ϊ0,���ܽ��з��������";
			}
			if(m_pRsltElementText==NULL)
				m_pRsltElementText = new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
            
			return false;
		}	
		else 
			bVarTest = false;
	}
	//F ����
	if(m_bF && bVarTest)
	{
		//F ���飬����s1/s2
		double R =  S1Sqr/S2Sqr ; // R ~ F(w1-1,w2-1)  
		double Prob=  min(2*(1- fcdf(R,w1-1,w2-1)),2*fcdf(R,w1-1,w2-1)); 
		CTString sDef("");
		sDef.Format("%.0f\n%.0f",w1-1,w2-1);
		m_FdfStrAr.Add(sDef);

		sDef.Empty();
		sDef.Format("%.4f",R);
		m_FStrAr.Add(sDef);

		sDef.Empty();
		sDef.Format("%.4f",Prob);
		m_FPStrAr.Add(sDef);
	}
	//Fͳ��������洢���
	//���㷽��ȵ���������
	if(method == 1)
	{
		double dUpper = 0.0;
		double dLower =0.0;
		double Alpha = 1-m_dConfidence;
		double Ualpha1 = fabs(finv(Alpha/2.0,w1-1,w2-1));
		if(fabs(Ualpha1)< DoubleZero)
			Ualpha1 = 1e-16;
		
		double Ualpha2 = fabs(finv(1-Alpha/2.0,w1-1,w2-1));
		if(fabs(Ualpha2)< DoubleZero)
			Ualpha2 = 1e-16;
		
		double R =  S1Sqr/S2Sqr ; // R ~ F(w1-1,w2-1) 
			 dLower=  R /Ualpha2 ;
			 dUpper=  R /Ualpha1;
		
		if(m_nCheckType !=0)//�����������������0-���� ,1-���� ,2-����
		{
			Ualpha1 = fabs(finv(Alpha,w1-1,w2-1));
			if(fabs(Ualpha1)< DoubleZero)
				Ualpha1 = 1e-16;
			
			Ualpha2 = fabs(finv(1-Alpha,w1-1,w2-1));
			if(fabs(Ualpha2)< DoubleZero)
				Ualpha2 = 1e-16;
			
			dLower= R /Ualpha2;
			dUpper = R /Ualpha1;
		}
		CTString str("");		
		str.Format("%.4f",dUpper);			
		m_UpperStrAr.Add(str);
		
		str.Format("%.4f",dLower);
		m_LowerStrAr.Add(str);
	}
//����Bartlett ͳ����
	if(m_bBartlett && bVarTest)
	{
		double Sp = ((w1-1)*S1Sqr + (w2-1)*S2Sqr)/(w1+w2-2);
		double Frac_Up = (w1+w2-2)*log(Sp) - ((w1-1)*log(S1Sqr) + (w2-1)*log(S2Sqr));
		double Frac_Down = 1+(1.0/3)*(1.0/(w1-1) +1.0/(w2-1) - 1.0/(w1+w2-2));
		double B =	Frac_Up/Frac_Down; //B ~ X^2(1)
		double Prob = 1- chi2cdf(B,1);
		CTString sDef;
		sDef.Empty();
		sDef.Format("%d",1);
		m_BdfStrAr.Add(sDef);
	
		sDef.Empty();
		sDef.Format("%.4f",B);
		m_BStrAr.Add(sDef);

		sDef.Empty();
		sDef.Format("%.4f",Prob);
		m_BPStrAr.Add(sDef);
	}
// Barttlett ͳ�����������
	
	
	double XMean2=sum2/w2;				//��2���ݵļ�Ȩƽ��ֵ
	double w=w1+w2;
	double D=XMean1-XMean2 - m_dValue; // �� ��Ĭ��Ϊ��
	sum1=0;sum2=0;
	for(i=0;i<nSize1;i++)
	{
		Data1[i]=fabs(Data1[i]-XMean1);//��Data1���洢Z1����ֵ����Ϊ���� ����һ�£���Data1������Ҫ
		sum1=sum1+Data1[i]*Weight1[i];
	}
	double ZMean1=sum1/w1;
	for(i=0;i<nSize2;i++)
	{
		Data2[i]=fabs(Data2[i]-XMean2);//��Data2���洢Z2����ֵ����Ϊ���� ����һ�£���Data1������Ҫ
		sum2=sum2+Data2[i]*Weight2[i];
	}
	double ZMean2=sum2/w2;
	double ZMean=(sum1+sum2)/(w1+w2);
	//////////////////////////////////////////////////////////////////////////
	//��������
	if(method == 2)
	{
		double p1 = XMean1;
		double p2 = XMean2;
		double pp = (XMean1*w1+XMean2*w2)/(w1+w2);			
		
		double Rate = 0.0;
		double dUpper = 0.0;
		double dLower =0.0;
		double Alpha = 1-m_dConfidence;
		double Ualpha = fabs(norminv(Alpha/2.0,0,1));
		CTString str("");
		if(m_bPooledData == false)//SASʹ�����ַ���
		{
			Rate = (p1-p2)/sqrt(p1*(1-p1)/w1 + p2*(1-p2)/w2);
			dUpper = (p1-p2) + Ualpha * sqrt(p1*(1-p1)/w1 + p2*(1-p2)/w2);
			dLower = (p1-p2) - Ualpha * sqrt(p1*(1-p1)/w1 + p2*(1-p2)/w2);

		}
		else
		{	//��Ϲ��� 
			Rate = (p1-p2)/sqrt(pp*(1-pp)*(1.0/w1 +1.0/w2));
			dUpper = (p1-p2) + Ualpha * sqrt(pp*(1-pp)*(1.0/w1 +1.0/w2));
			dLower = (p1-p2) - Ualpha * sqrt(pp*(1-pp)*(1.0/w1 +1.0/w2));
		}
		//Rate = (p1-p2)/sqrt(pp*(1-pp)*(1.0/w1 +1.0/w2));
		
		if(m_nCheckType !=0)//�����������������0-���� ,1-���� ,2-����
		{
			Ualpha = fabs(norminv(Alpha,0,1));
			dUpper = (p1-p2) + Ualpha * sqrt(p1*(1-p1)/w1 + p2*(1-p2)/w2);
			dLower = (p1-p2) - Ualpha * sqrt(p1*(1-p1)/w1 + p2*(1-p2)/w2);
		}
		str.Format("%.4f",dUpper);			
		m_UpperStrAr.Add(str);
		
		str.Format("%.4f",dLower);
		m_LowerStrAr.Add(str);
		
		str.Format("%.4f",Rate);
		m_ZStrAr.Add(str);		
	}    

	//����Lֵ���㹫ʽ�ķ�ĸֵ
	double temp=0;
	for(i=0;i<nSize1;i++)
	{
		double temp2=Data1[i]-ZMean1;
		temp=temp+temp2*temp2*Weight1[i];
	}
	for(i=0;i<nSize2;i++)
	{
		double temp2=Data2[i]-ZMean2;
		temp=temp+temp2*temp2*Weight2[i];
	}
	if(fabs(temp)<1e-15)
	{
		if( method == 0)
		{
		CTString str=strName;
		str+="�����Ժ�����ı�׼����Ϊ0������ʹ��Levene�������з������Լ��顣";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText = new CRsltElementText("����");
		m_pRsltElementText->AddString(str);
		}
		else  if(method == 1)
		{
			CTString str=strName;
			str+="�����Ժ�����ı�׼����Ϊ0������Levene�������з������Լ��顣";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText = new CRsltElementText("����");
			m_pRsltElementText->AddString(str);
			
			 m_bLevene = false;
		}
		      else
				  bVarTest = false;			  
	}

	//����Lֵ���㹫ʽ�ķ���ֵ	
	//����Levene ͳ������ ��F��ʾ
	if(bVarTest)
	{
		double numerator=(w-2)*(w1*(ZMean1-ZMean)*(ZMean1-ZMean)+w2*(ZMean2-ZMean)*(ZMean2-ZMean));
		double L=numerator/temp; //L~ F(1,w-2)
		double P=1-fcdf(L,1,w-2);
		CTString sL("");
		sL.Format("%.0f",w-2);
		m_LdfStrAr.Add(sL);
		sL.Empty();
		sL.Format("%.4f",L);
		m_LStrAr.Add(sL);
		sL.Empty();
		sL.Format("%.4f",P);
		m_PStrAr.Add(sL);	
	}
	
	CTString df2Str,P2Str,Sd2Str,t2Str,lower2Str,upper2Str;
	CTString df1Str,P1Str,Sd1Str,t1Str,lower1Str,upper1Str;
	
	//�������ʱ��ֵ���顣
	if(method ==0)
	{
		double df1=w-2;
		double Sp=0;
		double Sdp=0;//Sd1
		double t1=0;
		double P1=0;
		double lower1=0;
		double upper1=0;
		df1Str.Format("%.0f\n",df1);
		if((w-2)!=0)
		{
			Sp=sqrt(((w1-1)*S1*S1+(w2-1)*S2*S2)/(w-2));
			Sdp=Sp*sqrt(1/w1+1/w2);
			t1=D/Sdp;
			P1=2*(1-tcdf(fabs(t1),w-2));
			lower1=D-tinv((m_dConfidence+1)/2,df1)*Sdp;
			upper1=D+tinv((m_dConfidence+1)/2,df1)*Sdp;
			P1Str.Format("%.4f\n",P1);
			Sd1Str.Format("%.4f\n",Sdp);
			t1Str.Format("%.4f\n",t1);
			lower1Str.Format("%.4f\n",lower1);
			upper1Str.Format("%.4f\n",upper1);
		}
		else
		{
			P1Str=" \n";
			Sd1Str=" \n";
			t1Str=" \n";
			lower1Str=" \n";
			upper1Str=" \n";
		}

		//������ʱ��ֵ����
		double df2=w-2;
		double Sd=0;//Sd2
		double t2=0;
		double P2=0;
		double lower2=0;
		double upper2=0;
		double Z1=0;
		double Z2=0;
		if(w1!=1&&w2!=1&&(S1/w1+S2/w2)!=0)
		{
			double temp=S1*S1/w1/(S1*S1/w1+S2*S2/w2);//��ĸΪ0��
			Z1=temp*temp/(w1-1);
			temp=S2*S2/w2/(S1*S1/w1+S2*S2/w2);//��ĸΪ0��
			Z2=temp*temp/(w2-1);
			df2=1/(Z1+Z2);
			Sd=sqrt(S1*S1/w1+S2*S2/w2);
			t2=D/Sd;
			P2=2*(1-tcdf(fabs(t2),df2));
			lower2=D-tinv((m_dConfidence+1)/2,df2)*Sd;
			upper2=D+tinv((m_dConfidence+1)/2,df2)*Sd;
			df2Str.Format("%.0f",df2);
			P2Str.Format("%.4f",P2);
			Sd2Str.Format("%.4f",Sd);
			t2Str.Format("%.4f",t2);
			lower2Str.Format("%.4f",lower2);
			upper2Str.Format("%.4f",upper2);
		}
		else
		{
			df2Str=" ";
			P2Str=" ";
			Sd2Str=" ";
			t2Str=" ";
			lower2Str=" ";
			upper2Str=" ";
		}
		lower1Str+=lower2Str;
		m_LowerStrAr.Add(lower1Str);
		
		upper1Str+=upper2Str;
		m_UpperStrAr.Add(upper1Str);
		
		//���㷽����֪��ʱ���Uͳ����	//////////////////////////////////////////////////////////////////////////
		if(m_nCheckType ==1)
		{
			double sigma1,sigma2;
			if(m_nIsStdType == 0)
			{
				sigma1 = m_dSigma1;
				sigma2 = m_dSigma2;
			}
			else
			{
				sigma1 = m_dSigma1*m_dSigma1;
				sigma2 = m_dSigma2*m_dSigma2;
			}

			double Z = D/sqrt(sigma1/w1 + sigma2/w2);//  Z ~ N(0,1);
			CTString sZ("");
			sZ.Format("%.4f",Z);
			m_ZStrAr.Add(sZ);
		}
		//////////////////////////////////////////////////////////////////////////
		//����δ֪������
		if(m_nCheckType == 0 )
		{

			CTString sEt("");
			if( m_nIsVarNE == 0)
			{
				sEt.Format("%.4f", t1);
				m_EtStrAr.Add(sEt);
				
				sEt.Empty();
				sEt.Format("%.0f",w-2);
				m_EtdefStrAr.Add(sEt);
			}
			else if( m_nIsVarNE == 1)
			{
				sEt.Empty();
				sEt.Format("%.4f", t2);
				m_UtStrAr.Add(sEt);
				
				sEt.Empty();
				sEt.Format("%.0f",df2);
				m_UtdefStrAr.Add(sEt);				
			}		
		}
	}
	///��ʽ�����
	if (!m_bGroup)
		strName = m_strPair;
	m_NameStrAr.Add(strName);
	
	CTString str;

	CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nGroupIndex);
	

	if(m_nGroupMethod==0)//��һ�ַ��鷽ʽ
	{

		CTString labelVal;
		if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(m_dValue1,labelVal))
		{
			labelVal+="\n";
		}
		else
		{
			if(floor(m_dValue1)==m_dValue1)
				labelVal.Format("%.0f\n",m_dValue1);
			else
				labelVal.Format("%.2f\n",m_dValue1);

		}
		str=labelVal;


		CTString strTemp;
		if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(m_dValue2,labelVal))
		{
			if(floor(m_dValue2)==m_dValue2)
				labelVal.Format("%.0f\n",m_dValue2);
			else
				labelVal.Format("%.2f\n",m_dValue2);
		}
		strTemp=labelVal;
		str+=strTemp;
	}
	else//�ڶ�����鷽ʽ
	{
		CTString strTemp;
		if(floor(m_dBoundary)==m_dBoundary)
			strTemp.Format("%.0f",m_dBoundary);
		else
			strTemp.Format("%.2f",m_dBoundary);
		str=">=";
		str+=strTemp;
		str+="\n<";
		str+=strTemp;
	}
	if(m_bGroup)
		m_GroupStrAr.Add(str);

	str.Empty();
	str.Format("%.0f\n%.0f",floor(w1+0.5),floor(w2+0.5));
	m_FrequencyStrAr.Add(str);

	str.Empty();
	str.Format("%.4f\n%.4f",XMean1,XMean2);
	m_MeanStrAr.Add(str);

	str.Empty();
	CTString str1,strTemp,strTemp1;
	if(w1>1)
	{
		str.Format("%.4f\n",S1);
		str1.Format("%.4f\n",SEM1);
	}
	else
	{
		str="\n";
		str1="\n";
	}
	if(w2>1)
	{
		strTemp.Format("%.4f",S2);
		strTemp1.Format("%.4f",SEM2);
	}
	else
	{
		strTemp="\n";
		strTemp1="\n";
	}
	str+=strTemp;
	str1+=strTemp1;
	m_StdDStrAr.Add(str);
	m_StdMStrAr.Add(str1);

	//test result table
	str.Empty();
	str="�������\n������";
	m_CommentStrAr.Add(str);



	t1Str+=t2Str;
	m_tStrAr.Add(t1Str);

	df1Str+=df2Str;
	m_dfStrAr.Add(df1Str);

	P1Str+=P2Str;
	m_P2StrAr.Add(P1Str);

	CTString DStr;
	DStr.Format("%.4f\n%.4f",D,D);
	m_MeanDStrAr.Add(DStr);

	Sd1Str+=Sd2Str;
	m_StdEDStrAr.Add(Sd1Str);

	return true;
}


//******************************************************************************//
//Original Orthor:zkc														    //
//Date:2003.3.14															    //
//Function:���ݷ�������Է����������з��飬ȡ�������������ݡ�				    //
//Points:����Ȩ�أ�ȱʧֵ�������											    //
//******************************************************************************//
void CTTest::GroupIntoTwo(CTArray<double,double> &ar1,				//��1����
								 CTArray<double,double> &ar2,		//��2����
								 CTArray<double,double> &Weight1,	//��1���ݶ�Ӧ��Ȩ��
								 CTArray<double,double> &Weight2,	//��2���ݶ�Ӧ��Ȩ��
								 double dValue1,double dValue2,		//����ֵ1������ֵ2
								 int nVarCol,						//��ǰ��Ӧ�ķ����к�
								 TDataSet *pWeightArray,			//�洢��Ȩ������
								 TDataSet *pGroupArray)				//�洢�˷����������

{
	ar1.RemoveAll();
	ar2.RemoveAll();
	Weight1.RemoveAll();
	Weight2.RemoveAll();

	TDataSet *pDataArray=NULL;
	int nRow=m_pDataInterface->GetColData(nVarCol,pDataArray,5);
	
	for(int j=0;j<nRow;j++)
	{
			
		if(m_bWeight)						//����ָ����Ȩ��
		{

			if(pGroupArray[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//Ȩ���������о�δȱʧ
			{
				if(m_nGroupMethod==0)		//��һ�ַ��鷽ʽ
				{
					if(pGroupArray[j].dValue==dValue1)
					{
						Weight1.Add(pWeightArray[j].dValue);
						ar1.Add(pDataArray[j].dValue);
					}
					if(pGroupArray[j].dValue==dValue2)
					{
						Weight2.Add(pWeightArray[j].dValue);
						ar2.Add(pDataArray[j].dValue);
					}
				}
				else						//�ڶ��ַ��鷽ʽ
				{
					if(pGroupArray[j].dValue>=m_dBoundary)
					{
						Weight1.Add(pWeightArray[j].dValue);
						ar1.Add(pDataArray[j].dValue);
					}
					else
					{
						Weight2.Add(pWeightArray[j].dValue);
						ar2.Add(pDataArray[j].dValue);
					}
				}
			}
			else							//�����¼�����Ե���
			{
				m_RowArray.Add(pDataArray[j].iIndex);
			}
		}
		else								//����δָ��Ȩ��
		{
			if(pGroupArray[j].iIndex>=0&&pDataArray[j].iIndex>=0)
			{
				if(m_nGroupMethod==0)		//��һ�ַ��鷽ʽ
				{
					if(pGroupArray[j].dValue==dValue1)
					{
						Weight1.Add(1);
						ar1.Add(pDataArray[j].dValue);
					}
					if(pGroupArray[j].dValue==dValue2)
					{
						Weight2.Add(1);
						ar2.Add(pDataArray[j].dValue);
					}
				}
				else						//�ڶ��ַ��鷽ʽ
				{
					if(pGroupArray[j].dValue>=m_dBoundary)
					{
						Weight1.Add(1);
						ar1.Add(pDataArray[j].dValue);
					}
					else
					{
						Weight2.Add(1);
						ar2.Add(pDataArray[j].dValue);
					}

				}
			}
			else						//�洢��ɾ���еļ�¼��
			{
				m_RowArray.Add(pDataArray[j].iIndex);
			}
		}
	}
	if(pDataArray)						//free the momory
	{
		delete []pDataArray;
		pDataArray=NULL;
	}	
}


void CTTest::CreateResult(int method /* = 0 */)
{
	int nRow=m_NameStrAr.GetSize();
	if(nRow>0)
	{
		CTLTable * pTable = NULL;
		CRsltElementTable *pETable=NULL;
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetTitle("����ͳ��");		
		
		if(!m_bGroup)
		{
			pTable->InsertColumn(0,"������");
			pTable->InsertColumn(1,"������");
			pTable->InsertColumn(2,"��¼��");
			pTable->InsertColumn(3,"��ֵ");
			pTable->InsertColumn(4,"��׼���");
			pTable->InsertColumn(5,"��ֵ��׼��");
			for(int i=0;i<nRow;i++)
			{       
				pTable->InsertItem(i,m_NameStrAr.GetAt(i));
				pTable->SetItemText(i,1,m_GroupStrAr.GetAt(i));
				pTable->SetItemText(i,2,m_FrequencyStrAr.GetAt(i));
				pTable->SetItemText(i,3,m_MeanStrAr.GetAt(i));
				pTable->SetItemText(i,4,m_StdDStrAr.GetAt(i));
				pTable->SetItemText(i,5,m_StdMStrAr.GetAt(i));
			}
		}
		else
		{
			pTable->InsertColumn(0,"������");
			CTString groupName=m_pDataInterface->GetFieldName(m_nGroupIndex);
			pTable->InsertColumn(1,groupName);
			pTable->InsertColumn(2,"��¼��");
			pTable->InsertColumn(3,"��ֵ");
			pTable->InsertColumn(4,"��׼���");
			pTable->InsertColumn(5,"��ֵ��׼��");
			for(int i=0;i<nRow;i++)
			{       
				pTable->InsertItem(i,m_NameStrAr.GetAt(i));
				pTable->SetItemText(i,1,m_GroupStrAr.GetAt(i));
				pTable->SetItemText(i,2,m_FrequencyStrAr.GetAt(i));
				pTable->SetItemText(i,3,m_MeanStrAr.GetAt(i));
				pTable->SetItemText(i,4,m_StdDStrAr.GetAt(i));
				pTable->SetItemText(i,5,m_StdMStrAr.GetAt(i));
			}
		}
		pETable=new CRsltElementTable("����ͳ��",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);
		
		if(method == 0)
		{
			CRsltElementTable * pETable = NULL;
			CTLTable *	pTable = new CTLTable;
			pTable->CreateTable(nRow+1,11);
			pTable->SetTitle("������");

			pTable->InsertColumn(0,"������");
			pTable->InsertColumn(1,"��������");
			pTable->InsertColumn(2,"Levene ͳ����");
			pTable->InsertColumn(3,"P ֵ");
			pTable->InsertColumn(4,"Tͳ����");
			pTable->InsertColumn(5,"���ɶ�");
			pTable->InsertColumn(6,"P ֵ(˫β)");
			pTable->InsertColumn(7,"��ֵ�ľ�ֵ");
			pTable->InsertColumn(8,"��ֵ��׼��");
			CTString str,str2;
			str.Format("(%d%%���Ŷ�)", int(m_dConfidence*100));
			str2="����"+str;
			pTable->InsertColumn(9,str2);
			str2="����"+str;
			pTable->InsertColumn(10,str2);
			int i=0;
			for(i=0;i<nRow;i++)
			{
				pTable->InsertItem(i,m_NameStrAr.GetAt(i));
				pTable->SetItemText(i,1,m_CommentStrAr.GetAt(i));
				pTable->SetItemText(i,2,m_LStrAr.GetAt(i));
				pTable->SetItemText(i,3,m_PStrAr.GetAt(i));
				pTable->SetItemText(i,4,m_tStrAr.GetAt(i));
				pTable->SetItemText(i,5,m_dfStrAr.GetAt(i));
				pTable->SetItemText(i,6,m_P2StrAr.GetAt(i));
				pTable->SetItemText(i,7,m_MeanDStrAr.GetAt(i));
				pTable->SetItemText(i,8,m_StdEDStrAr.GetAt(i));
				pTable->SetItemText(i,9,m_LowerStrAr.GetAt(i));
				pTable->SetItemText(i,10,m_UpperStrAr.GetAt(i));
			}

			pETable=new CRsltElementTable("������",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
	//////////////////////////////////////////////////////////////////////////

				for(i=0;i<nRow;i++)
				{
					CreatHomoTestTable(i);
					CreatHypoTestTable(i);
				}
		}
//////////////////////////////////////////////////////////////////////////
		if(method == 1) //�������ļ���
		{
			for(int i=0;i<nRow;i++)
				CreatVarTestTable(i);
		}
		if(method == 2) //���������������
		{
			for(int i=0;i<nRow;i++)				
				CreatPropotionTestTable(i);
		}
	}
}
//���������μ���Ľ��
void CTTest::CreatHomoTestTable(int index)//���������μ���
{
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	int nRow = m_bLevene + m_bBartlett + m_bF;
	if (nRow <= 0 )
		return ;
	pTable->CreateTable(nRow+1,4);
	pTable->SetTitle(m_NameStrAr.GetAt(index)+ "�������Լ�����");

	pTable->InsertColumn(0,"���鷽��");
	pTable->InsertColumn(1,"ͳ����ֵ");
	pTable->InsertColumn(2,"���ɶ�");
	pTable->InsertColumn(3,"P ֵ");

	CTString str,str2;
	int count =0;
		if(m_bF)
		{
		pTable->InsertItem(count,"F ����");
		pTable->SetItemText(count,1,m_FStrAr.GetAt(index));
		pTable->SetItemText(count,2,m_FdfStrAr.GetAt(index));
		pTable->SetItemText(count,3,m_FPStrAr.GetAt(index));
		count++;
		}
		if(m_bBartlett)
		{
			pTable->InsertItem(count,"Barttlet ����");
			pTable->SetItemText(count,1,m_BStrAr.GetAt(index));
			pTable->SetItemText(count,2,m_BdfStrAr.GetAt(index));
			pTable->SetItemText(count,3,m_BPStrAr.GetAt(index));
			count++;
		}
		if(m_bLevene)
		{
			pTable->InsertItem(count,"Levene ����");
			pTable->SetItemText(count,1,m_LStrAr.GetAt(index));
			pTable->SetItemText(count,2,m_LdfStrAr.GetAt(index));
			pTable->SetItemText(count,3,m_PStrAr.GetAt(index));
			count++;
		}
	
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "�������Լ�����",pTable);//����һ���������Ķ���
	m_pResult->Add(pETable);	

}
//ͳ�Ƽ�����顣
void CTTest::CreatHypoTestTable(int index)
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
	CTString sMean = m_MeanDStrAr.GetAt(index);//�洢��ֵ��
	sMean = sMean.Left(sMean.GetLength()/2);
	
	int iCount =0;
	if(m_nCheckType == 1)//������֪
	{
		double dbVar1 = 0;
		double dbVar2 = 0;
		if(m_nIsStdType == 1)
		{
			dbVar1 = m_dSigma1*m_dSigma1;
			dbVar2 = m_dSigma2 * m_dSigma2;
		}
		else
		{
			dbVar1 = m_dSigma1;
			dbVar2 = m_dSigma2;
		}
		sTitle.Format(m_NameStrAr.GetAt(index)+"�����������\nԭ���� : H0 : u1 �� u2  �� �ģ� �� = %.4f,\n��֪���� Var1 = %.4f ,Var2= %.4f",m_dValue,dbVar1,dbVar2);		
		table->SetTitle(sTitle);
		
		table->InsertColumn(0,"�������");
//		table->InsertColumn(1,"u1 �� u2 -��");
		table->InsertColumn(1,"Zͳ����");
		table->InsertColumn(2,"P ֵ");
	
		double T =atof(m_ZStrAr.GetAt(index)); //
		if(m_bGT)// H1 : u �� u0
		{
			str.Format("H1 : u1��u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
	//		table->SetItemText(iCount,1,sMean);
			table->SetItemText(iCount,1,T);
			table->SetItemText(iCount,2,1-normcdf(T,0,1));
 			iCount++;
		}

		if(m_bLT)// H1 : u �� u0
		{
			str.Format("H1 : u1��u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			if(iCount ==0)
			{
		//		table->SetItemText(iCount,1,sMean);
				table->SetItemText(iCount,1,T);
			}		
			table->SetItemText(iCount,2,normcdf(T,0,1));		
			iCount++;
		}

		if(m_bNE)// H1 : u �� u0
		{
			str.Format("H1 : u1��u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			if(iCount ==0)
			{
		//		table->SetItemText(iCount,1,sMean);
				table->SetItemText(iCount,1,T);
			}			
			table->SetItemText(iCount,2,2*(1-normcdf(fabs(T),0,1)));
			iCount++;
		}			
	}
	else//����δ֪
	{
		iCount =0;
		if(m_nIsVarNE == 0 ) //�������
			sTitle.Format(m_NameStrAr.GetAt(index)+"�����������\nԭ���� : H0 : u1��u2 ����,��= %.4f��\n����Var1 = Var2 δ֪",m_dValue);
		else
			sTitle.Format(m_NameStrAr.GetAt(index)+"�����������\nԭ���� : H0 : u1��u2 ����,��= %.4f��\n����Var1 ��Var2 δ֪",m_dValue);

		table->SetTitle(sTitle);
		table->CreateTable(nRow+1,5);		
		table->InsertColumn(0,"�������");
	//	table->InsertColumn(1,"u1��u2-��");
		table->InsertColumn(1,"Tͳ����");
		table->InsertColumn(2,"���ɶ�");

		table->InsertColumn(3,"P ֵ");
		
		double T =0.0;
		int df =0;
		if(m_nIsVarNE == 0 ) //�������
		{
			T =atof(m_EtStrAr.GetAt(index));//Tͳ����
			df =atoi(m_EtdefStrAr.GetAt(index));
		}
		else
		{
			T =atof(m_UtStrAr.GetAt(index));//Tͳ����
			df =atoi(m_UtdefStrAr.GetAt(index));
		}
		
		if(m_bGT)// H1 : u �� u0
		{
			str.Format("H1 : u1��u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
	//		table->SetItemText(iCount,1,sMean);
			table->SetItemText(iCount,1,T);
			table->SetItemText(iCount,2,df);
			table->SetItemText(iCount,3,1-tcdf(T,df));
 			iCount++;
		}

		if(m_bLT)// H1 : u �� u0
		{
			str.Format("H1 : u1��u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			if(iCount ==0)
			{
		//		table->SetItemText(iCount,1,sMean);
				table->SetItemText(iCount,1,T);
				table->SetItemText(iCount,2,df);
			}
			table->SetItemText(iCount,3,tcdf(T,df));
			iCount++;
		}

		if(m_bNE)// H1 : u �� u0
		{
			str.Format("H1 : u1-u2 �� %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			if(iCount ==0)
			{
	//			table->SetItemText(iCount,1,sMean);
				table->SetItemText(iCount,1,T);
				table->SetItemText(iCount,2,df);
			}
			table->SetItemText(iCount,3,2*(1-tcdf(fabs(T),df)));
			iCount++;
		}
		
	}
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index)+ "������������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);		
}
//����ļ������

BOOL CTTest::GetValueFromVoVar(CDataInterface *pDataInterface, string VO)
{
//	TWOSAMPLEVAR
//		/DATA=[FILE]
//		/PAIREDVAR [varname varname] [varname varname] ...
//		/SELFVAR [varlist]
//		/BYVAR [varname] [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
//		/VARTEST [{EQU},{LARGER},{LESS}]
//		/SIGNRANGE [SIGN({0.95**})] [{DOUBLE},{UPPER},{LOWER}]
//		/METHOD [LEVENE] [FTEST] [BARTLETT]
	AnaWord VOstr;
	VOstr.Import(VO);

	if (AnaSyntax::exist("/WEIGHT", VOstr) )
	{
		TIntStruct tVarWeight;
		CTStringArray xname;
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
	if (AnaSyntax::exist("/PAIREDVAR", VOstr) && 
		!AnaSyntax::exist("/SELFVAR", VOstr) && 
		!AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/PAIREDVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (int(m_tVarInfo.iCount/2)*2 != m_tVarInfo.iCount)
			return FALSE;
		m_bGroup = FALSE;
	}
	else if (AnaSyntax::exist("/SELFVAR", VOstr) && AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/BYVAR", VOstr, xname);
//		/GROUP [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (m_tVarInfo.iCount == 1)
			m_nGroupIndex = m_tVarInfo.pValue[0];
		else
			return FALSE;
		m_nGroupMethod = int(AnaSyntax::findstr("/GROUP", "SEPVALUE", VOstr));
		if (m_nGroupMethod == 0)
		{
			m_dValue1 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr);
			m_dValue2 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr, 2);
		}
		else
			m_dBoundary = AnaSyntax::getvalue("/GROUP", "SEPVALUE", VOstr);
		
		xname.RemoveAll();
		AnaSyntax::getVAR("/SELFVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		m_bGroup = TRUE;
	}
	else
		return FALSE;
//		/VARTEST [{EQU},{LARGER},{LESS}]
	m_bGT = AnaSyntax::findstr("/VARTEST", "LARGER", VOstr);
	m_bLT = AnaSyntax::findstr("/VARTEST", "LESS", VOstr);
	m_bNE = AnaSyntax::findstr("/VARTEST", "EQU", VOstr);
//		/SIGNRANGE [SIGN({0.95**})] [{DOUBLE},{UPPER},{LOWER}]
	if (AnaSyntax::findstr("/SIGNRANGE", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/SIGNRANGE", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
	if (AnaSyntax::findstr("/SIGNRANGE", "LOWER", VOstr) )
		m_nCheckType = 1;
	if (AnaSyntax::findstr("/SIGNRANGE", "UPPER", VOstr) )
		m_nCheckType = 2;
	else
		m_nCheckType = 0;
//		/METHOD [LEVENE] [FTEST] [BARTLETT]
	m_bLevene = AnaSyntax::findstr("/METHOD", "LEVENE", VOstr);
	m_bF = AnaSyntax::findstr("/METHOD", "FTEST", VOstr);
	m_bBartlett = AnaSyntax::findstr("/METHOD", "BARTLETT", VOstr);

	return TRUE;
}

CResult* CTTest::OnDoubleVarTest(CDataInterface *pDataInterface, string VO)
{
	m_pResult=new CResult("˫�����������Լ���");
	if (!GetValueFromVoVar(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet *pWeightArray=NULL;
	if(m_bWeight)
		m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);

	TDataSet *pDataSet=NULL;
	TDataSet *pGroupSet=NULL;
	CTArray<double,double> V1,V2;
	CTArray<double,double> Weight1,Weight2;
	if (m_bGroup)	
	{
		int nRow=m_pDataInterface->GetColData(m_nGroupIndex,pGroupSet,5);
		FreeMemebers();
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nIndex=m_tVarInfo.pValue[i];
			GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
			CTString str=pDataInterface->GetFieldName(nIndex);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{	
				CTString str1=str;
				str1+="�����Ժ���Ч����̫��,���ܽ���˫����������飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			TestMain(V1,V2,Weight1,Weight2,nIndex,1);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}
	else
	{
		FreeMemebers();
		for(int i=0;i<m_tVarInfo.iCount/2;i++)
		{
			int nIndex1 = m_tVarInfo.pValue[2*i];
			int nIndex2 = m_tVarInfo.pValue[2*i+1];
			int nRow = m_pDataInterface->GetColData(nIndex1,pGroupSet,5);
			CTString sName1,sName2;				
			sName1 = m_pDataInterface->GetFieldName(nIndex1);
			int kk;
			for (kk = 0; kk< nRow;kk++)
			{
				if (pGroupSet[kk].iIndex >= 0)
				{
					Weight1.Add(1);
					V1.Add(pGroupSet[kk].dValue);
				}
			}
			nRow = m_pDataInterface->GetColData(nIndex2,pWeightArray,5);
			sName2 = m_pDataInterface->GetFieldName(nIndex2);
			if(!m_bGroup) //������
			{
				CTString sTemp;
				sTemp.Format(sName1 + "\n" + sName2);
				m_GroupStrAr.Add(sTemp);
			}
			for (kk = 0; kk< nRow;kk++)
			{
				if (pWeightArray[kk].iIndex >= 0)
				{
					Weight2.Add(1);
					V2.Add(pWeightArray[kk].dValue);
				}
			}
			CTString str=pDataInterface->GetFieldName(nIndex1);
			str += "--";
			str += pDataInterface->GetFieldName(nIndex2);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{
				CTString str1="����: "+ str;
				str1+="�����Ժ���Ч����̫��,���ܽ��ж�������T���飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			m_strPair = str;
			TestMain(V1,V2,Weight1,Weight2,nIndex1,1);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}

	CreateResult(1);
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
}

double CTTest::GetVar(CTArray<double,double> &Data)
{
	int n = Data.GetSize();
	if(n<=1)
		return 0.0;
	double sum =0.0;
	double sum2 =0.0;
	for(int i =0; i<n;i++)
	{
		sum+=Data[i];
		sum2+=Data[i]*Data[i];
	}
	double Mean= sum/n;
	double S = (sum2 - n*Mean*Mean)/(n-1);
	return S;
}

BOOL CTTest::GetValueFromVoPro(CDataInterface *pDataInterface, string VO)
{
//	TWOSAMPLEPROP
//		/DATA=[FILE]
//		/PAIREDVAR [varname varname] [varname varname] ...
//		/WEIGHT [varname]
//		/SELFVAR [varlist]
//		/BYVAR [varname] 
//		/GROUP [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
//		/PROPTEST [{EQU},{LARGER},{LESS}] [UNITE]
//		/SIGNRANGE [SIGN({0.95**})] [{DOUBLE},{UPPER},{LOWER}]
	AnaWord VOstr;
	VOstr.Import(VO);

	if (AnaSyntax::exist("/WEIGHT", VOstr) )
	{
		TIntStruct tVarWeight;
		CTStringArray xname;
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
	if (AnaSyntax::exist("/PAIREDVAR", VOstr) && 
		!AnaSyntax::exist("/SELFVAR", VOstr) && 
		!AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/PAIREDVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (int(m_tVarInfo.iCount/2)*2 != m_tVarInfo.iCount)
			return FALSE;
		m_bGroup = FALSE;
	}
	else if (AnaSyntax::exist("/SELFVAR", VOstr) && AnaSyntax::exist("/BYVAR", VOstr))
	{
		AnaSyntax::getVAR("/BYVAR", VOstr, xname);
//		/GROUP [{POINTVALUE({0**} {1**})},{SEPVALUE({0**})}]
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (m_tVarInfo.iCount == 1)
			m_nGroupIndex = m_tVarInfo.pValue[0];
		else 
			return FALSE;
		m_nGroupMethod = int(AnaSyntax::findstr("/GROUP", "SEPVALUE", VOstr));
		if (m_nGroupMethod == 0)
		{
			m_dValue1 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr);
			m_dValue2 = AnaSyntax::getvalue("/GROUP", "POINTVALUE", VOstr, 2);
		}
		else
			m_dBoundary = AnaSyntax::getvalue("/GROUP", "SEPVALUE", VOstr);
		
		xname.RemoveAll();
		AnaSyntax::getVAR("/SELFVAR", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		m_bGroup = TRUE;
	}
	else
		return FALSE;
//		/PROPTEST [{EQU},{LARGER},{LESS}] [UNITE]
	m_bGT = AnaSyntax::findstr("/PROPTEST", "LARGER", VOstr);
	m_bLT = AnaSyntax::findstr("/PROPTEST", "LESS", VOstr);
	m_bNE = AnaSyntax::findstr("/PROPTEST", "EQU", VOstr);
	m_bPooledData = AnaSyntax::findstr("/PROPTEST", "UNITE", VOstr);
//		/SIGNRANGE [SIGN({0.95**})] [{DOUBLE},{UPPER},{LOWER}]
	if (AnaSyntax::findstr("/SIGNRANGE", "SIGN", VOstr))
		m_dConfidence = AnaSyntax::getvalue("/SIGNRANGE", "SIGN", VOstr);
	else
		m_dConfidence = 0.95;
	if (AnaSyntax::findstr("/SIGNRANGE", "LOWER", VOstr) )
		m_nCheckType = 1;
	if (AnaSyntax::findstr("/SIGNRANGE", "UPPER", VOstr) )
		m_nCheckType = 2;
	else
		m_nCheckType = 0;

	return TRUE;
}

CResult *CTTest::OnDoublePropotionTest(CDataInterface *pDataInterface, string VO)
{
	m_pResult=new CResult("˫������������");
	if (!GetValueFromVoPro(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	TDataSet *pWeightArray=NULL;
	if(m_bWeight)
		m_pDataInterface->GetColData(m_nWeight,pWeightArray,5);

	TDataSet *pDataSet=NULL;
	TDataSet *pGroupSet=NULL;
	CTArray<double,double> V1,V2;
	CTArray<double,double> Weight1,Weight2;
	
	if (m_bGroup)
	{
		int nRow=m_pDataInterface->GetColData(m_nGroupIndex,pGroupSet,5);
		FreeMemebers();
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nIndex=m_tVarInfo.pValue[i];
			GroupIntoTwo(V1,V2,Weight1,Weight2,m_dValue1,m_dValue2,nIndex,pWeightArray,pGroupSet);
			CTString str=pDataInterface->GetFieldName(nIndex);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{
				CTString str1=str;
				str1+="�����Ժ���Ч����̫��,���ܽ��б������飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			TestMain(V1,V2,Weight1,Weight2,nIndex,2);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}
	else
	{
		FreeMemebers();
		for(int i=0;i<m_tVarInfo.iCount/2;i++)
		{
			int nIndex1 = m_tVarInfo.pValue[2*i];
			int nIndex2 = m_tVarInfo.pValue[2*i+1];
			int nRow = m_pDataInterface->GetColData(nIndex1,pGroupSet,5);
			CTString sName1,sName2;
			
			sName1 = m_pDataInterface->GetFieldName(nIndex1);
			int kk;
			for (kk = 0; kk< nRow;kk++)
			{
				if (pGroupSet[kk].iIndex >= 0)
				{
					Weight1.Add(1);
					V1.Add(pGroupSet[kk].dValue);
				}
			}
			nRow = m_pDataInterface->GetColData(nIndex2,pWeightArray,5);
			sName2 = m_pDataInterface->GetFieldName(nIndex2);
			if(!m_bGroup) //������
			{
				CTString sTemp;
				sTemp.Format(sName1 + "\n" + sName2);
				m_GroupStrAr.Add(sTemp);
			}
			for (kk = 0; kk< nRow;kk++)
			{
				if (pWeightArray[kk].iIndex >= 0)
				{
					Weight2.Add(1);
					V2.Add(pWeightArray[kk].dValue);
				}
			}
			CTString str=pDataInterface->GetFieldName(nIndex1);
			str += "--";
			str += pDataInterface->GetFieldName(nIndex2);
			if(V1.GetSize()<1||V2.GetSize()<1)
			{
				CTString str1="����: "+ str;
				str1+="�����Ժ���Ч����̫��,���ܽ��б������飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				continue;
			}
			m_strPair = str;
			TestMain(V1,V2,Weight1,Weight2,nIndex1,2);
			V1.RemoveAll();
			V2.RemoveAll();
			Weight1.RemoveAll();
			Weight2.RemoveAll();
		}
	}
	CreateResult(2);
	if(m_pRsltElementText)
		m_pResult->Add(m_pRsltElementText);
	return m_pResult;
}

void CTTest::CreatPropotionTestTable(int index)
{
	CRsltElementTable * pETable = NULL;
	CTLTable * table = new CTLTable;
	
	CTString sTitle("");
	int nRow = m_bGT + m_bLT + m_bNE;
	if(nRow ==0)
		return ;
    CTString str,str2;
	int iCount =0;
	double dbVar = 0;
	sTitle.Format("ԭ���� : H0 : p1 �� p2");
	sTitle = m_NameStrAr.GetAt(index) +"��������������\n"+sTitle;
	table->SetTitle(sTitle);
	table->CreateTable(nRow+1,5);
	
	table->InsertColumn(0,"�������");
	table->InsertColumn(1,"Zͳ����");
	table->InsertColumn(2,"P ֵ");
	
	if(m_nCheckType ==0)
		str.Format("�������%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType ==1)
		str.Format("�������%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType ==2)
		str.Format("�������%d%%��������", int(m_dConfidence*100));
	
	table->InsertColumn(3,str);
	
	str =m_MeanStrAr.GetAt(index);
	int n = str.Find("\n");
	double p1= atof(str.Left(n));
	double p2 =atof(str.Mid(n+1));
	str2.Format("%.4f",p1 -p2); //��ñ����Ĳ�
	

	double Z =atof(m_ZStrAr.GetAt(index)); //
	if(m_bGT)// H1 : p �� p0
	{
		str.Format("H1 : p1 �� p2");
		table->InsertItem(iCount,str,true);
		//m_dfStrAr
	//	table->SetItemText(iCount,1, str2);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,1-normcdf(Z,0,1));
		
		if(m_nCheckType ==0)
			str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
		if(m_nCheckType ==1)
			str.Format("(%s , 1)",m_LowerStrAr.GetAt(index).GetData());
		if(m_nCheckType ==2)
			str.Format("(-1 , %s)",m_UpperStrAr.GetAt(index).GetData());
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}
	
	if(m_bLT)// H1 : p �� p0
	{
		str.Format("H1 : p1 �� p2");
		table->InsertItem(iCount,str,true);
	//	table->SetItemText(iCount,1,str2);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,normcdf(Z,0,1));
		
		if(m_nCheckType ==0)
			str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
		if(m_nCheckType ==1)
			str.Format("(%s , 1)",m_LowerStrAr.GetAt(index).GetData());
		if(m_nCheckType ==2)
			str.Format("(-1 , %s)",m_UpperStrAr.GetAt(index).GetData());
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}
	
	if(m_bNE)// H1 : p �� p0
	{
		str.Format("H1 : p1 �� p2",m_dValue);
		table->InsertItem(iCount,str,true);
	//	table->SetItemText(iCount,1,str2);
		table->SetItemText(iCount,1,Z);
		table->SetItemText(iCount,2,2*(1-normcdf(fabs(Z),0,1)));
		
		if(m_nCheckType ==0)
			str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
		if(m_nCheckType ==1)
			str.Format("(%s , 1)",m_LowerStrAr.GetAt(index).GetData());
		if(m_nCheckType ==2)
			str.Format("(-1 , %s)",m_UpperStrAr.GetAt(index).GetData());
		
		table->SetItemText(iCount,3,str);
		
		iCount++;
	}			
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);
}
void CTTest::CreatVarTestTable(int index)
{
	BOOL bF = m_bF;
	BOOL bLevene = m_bLevene;
	BOOL bBartlett = m_bBartlett;
	
	int nMethod = m_bF+ m_bLevene + m_bBartlett;
    if(nMethod ==0)
		return;

	int nRow = m_bGT + m_bLT + m_bNE;
	if(nRow ==0)
		return ;
	
	CRsltElementTable * pETable = NULL;
	CTLTable * table = new CTLTable;
	assert(table != NULL);	
	table->CreateTable(nMethod*nRow+1,7);

	CTString sTitle("");
    CTString str,str2;
	CTString sMean = m_MeanDStrAr.GetAt(index);//�洢��ֵ��
	sMean = sMean.Left(sMean.GetLength()/2);
	
	int iCount =0;

		sTitle.Format(m_NameStrAr.GetAt(index)+"��������������Var1 ��Var2 �������\nԭ���� : H0 : Var1 �� Var2 ");
	//	sTitle.Format(m_NameStrAr.GetAt(index)+"��������������\nԭ���� : H0 : u1��u2 ����,��= %.4f��\n����Var1 ��Var2 δ֪",m_dValue);

	table->SetTitle(sTitle);//Var
//	table->SetCols(7);
//	table->SetRows(nMethod*nRow+1);


	table->InsertColumn(0,"���鷽��");
	table->InsertColumn(1,"�������");
//	table->InsertColumn(2,"Var1/Var2");
	table->InsertColumn(2,"ͳ����");
	table->InsertColumn(3,"���ɶ�");
	table->InsertColumn(4,"P ֵ");

	if(m_nCheckType ==0)
		str.Format("Var1/Var2��%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType ==1)
		str.Format("Var1/Var2��%d%%��������", int(m_dConfidence*100));
	if(m_nCheckType ==2)
		str.Format("Var1/Var2��%d%%��������", int(m_dConfidence*100));
	
	table->InsertColumn(5,str);
	
	double T =0.0;
	int df =0;
	int nType = 0; 
	int j =0;
	
	str = m_StdDStrAr.GetAt(index);//��ȡ��׼��
	int n = str.Find("\n");
	double std1= atof(str.Left(n));
	double std2 =atof(str.Mid(n+1));
	sMean.Format("%.4f",(std1/std2)*(std1/std2)); //��÷����
				
	for(int i =0 ;i< nMethod; i++ )
	{
		if(m_bF) //����
		{
			iCount =0;
			
			str = "F ����";
			table->InsertItem(nRow*nType,str,false);

			for(j =1;j<nRow;j++ )
				table->InsertItem(nRow*nType+j,"",j==(nRow-1));
			
			T = atof(m_FStrAr.GetAt(index));//�õ�Fͳ����
			
			str = m_FdfStrAr.GetAt(index);//��ȡ�����ɶ�
			n = str.Find("\n");
			int df1= atoi(str.Left(n));
			int df2 =atoi(str.Mid(n+1));
			str.Empty();
			
			if(m_bGT)// H1 : u �� u0
			{
			str.Format("H1: Var1 �� Var2");
			table->SetItemText(nRow*nType+iCount,1,str);
			//table->SetItemText(nRow*nType+iCount,2,sMean);
			table->SetItemText(nRow*nType+iCount,2,T);

			str = m_FdfStrAr.GetAt(index);//��ȡ�����ɶ�
			table->SetItemText(nRow*nType+iCount,3,str);

			table->SetItemText(nRow*nType+iCount,4,1-fcdf(T,df1,df2));
			
			if(iCount == 0)
			{
			if(m_nCheckType ==0)
				str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
			if(m_nCheckType ==1)
				str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
			if(m_nCheckType ==2)
				str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
			
			table->SetItemText(nRow*nType+iCount,5,str);
			}
			iCount++;
			}
			str.Empty();
			if(m_bLT)// H1 : u �� u0
			{
			
			str.Format("H1: Var1 �� Var2");
			table->SetItemText(nRow*nType+iCount,1,str);
		//	table->SetItemText(nRow*nType+iCount,2,sMean);
			table->SetItemText(nRow*nType+iCount,2,T);
			str = m_FdfStrAr.GetAt(index);//��ȡ�����ɶ�
			table->SetItemText(nRow*nType+iCount,3,str);
			
			table->SetItemText(nRow*nType+iCount,4,fcdf(T,df1,df2));
			if(iCount == 0)
			{
				if(m_nCheckType ==0)
					str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
				if(m_nCheckType ==1)
					str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
				if(m_nCheckType ==2)
					str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
				
				table->SetItemText(nRow*nType+iCount,5,str);
			}			
			iCount++;
			}

			str.Empty();			
			if(m_bNE)// H1 : u �� u0
			{
			str.Format("H1: Var1 �� Var2");
			table->SetItemText(nRow*nType+iCount,1,str);
		//	table->SetItemText(nRow*nType+iCount,2,sMean);
			table->SetItemText(nRow*nType+iCount,2,T);
			str = m_FdfStrAr.GetAt(index);//��ȡ�����ɶ�
			table->SetItemText(nRow*nType+iCount,3,str);
			table->SetItemText(nRow*nType+iCount,4,min(2*fcdf(fabs(T),df1,df2),2*(1-fcdf(fabs(T),df1,df2))));

			if(iCount == 0)
			{
				if(m_nCheckType ==0)
					str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
				if(m_nCheckType ==1)
					str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
				if(m_nCheckType ==2)
					str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
				
				table->SetItemText(nRow*nType+iCount,5,str);
			}			
			iCount++;
			}
			nType++;
			m_bF = false;			
		}
		//
		if(m_bLevene)
		{
			iCount =0;
			
			if (m_bNE)
			{
				str = "Levene ����";
				table->InsertItem(nRow*nType,str,false);
			}			
			for(j =1;j<nRow;j++ )
				table->InsertItem(nRow*nType+j,"",j==(nRow-1));
			
			T = atof(m_LStrAr.GetAt(index));
			df =atof(m_LdfStrAr.GetAt(index));
			
			str.Empty();
			
			if(m_bNE && m_bGT)// H1 : u �� u0
			{
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
			//	table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				str.Format("1\n%d",df);
				table->SetItemText(nRow*nType+iCount,3,str);
				table->SetItemText(nRow*nType+iCount,4,1-fcdf(T,1,df));
				if(iCount == 0)
				{
					if(m_nCheckType ==0)
						str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
					if(m_nCheckType ==1)
						str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
					if(m_nCheckType ==2)
						str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
					
					table->SetItemText(nRow*nType+iCount,5,str);
				}
				
				iCount++;
			}
			
			str.Empty();
			
			if(m_bNE && m_bLT)// H1 : u �� u0
			{
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
				//table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				str.Format("1\n%d",df);
				table->SetItemText(nRow*nType+iCount,3,str);
				
				table->SetItemText(nRow*nType+iCount,4,fcdf(T,1,df));
				if(iCount == 0)
				{
				if(m_nCheckType ==0)
					str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
				if(m_nCheckType ==1)
					str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
				if(m_nCheckType ==2)
					str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
				
				table->SetItemText(nRow*nType+iCount,5,str);
				}
				iCount++;
			}
			str.Empty();
			
			if(m_bNE)// H1 : u �� u0
			{
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
				//table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				str.Format("1\n%d",df);
				table->SetItemText(nRow*nType+iCount,3,str);
				
				table->SetItemText(nRow*nType+iCount,4,min(1-fcdf(fabs(T),1,df),fcdf(fabs(T),1,df)));
				if(iCount == 0)
				{
					if(m_nCheckType ==0)
						str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
					if(m_nCheckType ==1)
						str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
					if(m_nCheckType ==2)
						str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
					
					table->SetItemText(nRow*nType+iCount,5,str);
				}
				iCount++;
			}
			nType++;	
			m_bLevene =false;
		}
		if(m_bBartlett)
		{
			iCount =0;		
			if (m_bNE)
			{
				str = "Bartlett ����";
				table->InsertItem(nRow*nType,str,false);
			}
			for(j =1;j<nRow;j++ )
				table->InsertItem(nRow*nType+j,"",j==(nRow-1));
			
			T = atof(m_BStrAr.GetAt(index));
			df = 1;
			if(m_bNE && m_bGT)// H1 : u �� u0
			{
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
			//	table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				table->SetItemText(nRow*nType+iCount,3,df);
				table->SetItemText(nRow*nType+iCount,4,1-chi2cdf(T,df));
				if(iCount == 0)
				{
					if(m_nCheckType ==0)
						str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
					if(m_nCheckType ==1)
						str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
					if(m_nCheckType ==2)
						str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
					
					table->SetItemText(nRow*nType+iCount,5,str);
				}
				iCount++;
			}
			str.Empty();
			
			if(m_bNE && m_bLT)// H1 : u �� u0
			{
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
			//	table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				table->SetItemText(nRow*nType+iCount,3,df);
				
				table->SetItemText(nRow*nType+iCount,4,chi2cdf(T,df));
				if(iCount == 0)
				{
					if(m_nCheckType ==0)
						str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
					if(m_nCheckType ==1)
						str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
					if(m_nCheckType ==2)
						str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
					
					table->SetItemText(nRow*nType+iCount,5,str);
				}
				iCount++;
			}
			
			if(m_bNE)// H1 : u �� u0
			{
				str.Empty();
				
				str.Format("H1: Var1 �� Var2");
				table->SetItemText(nRow*nType+iCount,1,str);
			//	table->SetItemText(nRow*nType+iCount,2,sMean);
				table->SetItemText(nRow*nType+iCount,2,T);
				table->SetItemText(nRow*nType+iCount,3,df);
				
				table->SetItemText(nRow*nType+iCount,4,min(1-chi2cdf(fabs(T),df),chi2cdf(fabs(T),df)));
				if(iCount == 0)
				{
					if(m_nCheckType ==0)
						str.Format("(%s , %s)",m_LowerStrAr.GetAt(index).GetData(),m_UpperStrAr.GetAt(index).GetData());
					if(m_nCheckType ==1)
						str.Format("(%s , +��)",m_LowerStrAr.GetAt(index).GetData());
					if(m_nCheckType ==2)
						str.Format("(0 , %s)",m_UpperStrAr.GetAt(index).GetData());
					
					table->SetItemText(nRow*nType+iCount,5,str);
				}
				iCount++;
			}
			nType++;
			m_bBartlett = false;
		}
	}
	m_bF = bF;
	m_bLevene = bLevene;
	m_bBartlett = bBartlett;
	
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index)+ "������������",table);//����һ���������Ķ���
	m_pResult->Add(pETable);		

}























