// BinomialTest.h: interface for the CBinomialTest class.
//                  ����ֲ�����
//				��ע�������� 2005��10��21��
//			�޸ģ�����ׯ	2006/03/22
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BinomialTest.h"
#include <math.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBinomialTest::CBinomialTest()
{
	m_dSpecify=0;
	m_nBttn=0;
	m_dProportion=0.5;
	m_bWeight=false;	
	m_pRsltElementText=NULL;

}

CBinomialTest::~CBinomialTest()
{
	FreeVariable();
	m_tVarInfo.DeleteObject();

}

int * CBinomialTest::Binarilize(CTArray<double,double> &DataArray,
								CTArray<double,double> &WeightArray,
								int nBinaryMethod,double &dSpecified)
{
	//pData ԭʼ����
	//nBinaryMethod 0,1���л�������
	//Ϊ0Ϊ������
	//Ϊ1�Ծ�ֵ������
	//Ϊ2����λ��������
	//Ϊ3���û�ָ��ֵ������
	//Ϊ4�Է�ֵ������
	int nLen=DataArray.GetSize();

	//dSpecified ӵ��ָ��ֵ
	int *pIntData=new int[nLen];

	switch(nBinaryMethod)
	{
		case 0:
			{
				for(int i=0;i<nLen;i++)
					pIntData[i]=(int)(DataArray.GetAt(i));
				dSpecified=0;
				return pIntData;
			}
			break;
		case 1:
			{
				double sum=0;
				double nSize=0;
				int i=0;
				for(i=0;i<nLen;i++)
				{
					sum=sum+DataArray.GetAt(i)*WeightArray.GetAt(i);
					nSize=nSize+WeightArray.GetAt(i);
				}
				sum=sum/nSize;
				for(i=0;i<nLen;i++)
				{
					if(DataArray.GetAt(i)<=sum)
						pIntData[i]=0;
					else
						pIntData[i]=1;
				}
				dSpecified=sum;
				return pIntData;
			}
			break;
		case 2:
			{
				double nSize=0;
				int i=0;
				for(i=0;i<nLen;i++)
				{
					nSize=nSize+WeightArray.GetAt(i);
				}
				double nMedian=nSize/2;
				double nSize2=0;
				for(i=0;i<nLen;i++)
				{
					nSize2+=WeightArray.GetAt(i);
					if(nSize2>=nMedian)
						break;
				}
				nMedian=i+1;

				for(i=0;i<nLen;i++)
				{
					if(DataArray.GetAt(i)<=DataArray.GetAt(nMedian))
						pIntData[i]=0;
					else
						pIntData[i]=1;
				}
				dSpecified=DataArray.GetAt(nMedian);
				return pIntData;
			}
			break;
		case 3:
			{
				for(int i=0;i<nLen;i++)
				{
					if(DataArray.GetAt(i)<=dSpecified)
						pIntData[i]=0;
					else
						pIntData[i]=1;
				}
				return pIntData;
			}
			break;
		default:
			for(int i=0;i<nLen;i++)
				pIntData[i]=(int)(DataArray.GetAt(i));
			return pIntData;
			break;
	}


}

double CBinomialTest::Cnk(int n,int k)
{
	if(k>n)
		return -1;

	if( k>n/2 )
		k = n-k;

	if( k==0 )
		return 0;

	double s = 0;

	for( int i=0; i<k; i++ )
	{
		s += (log10(n-i)-log10(i+1));
	}
	return s;
}


BOOL CBinomialTest::BinomialTestMain(CTArray<double,double> &DataArray,
									 CTArray<double,double> &WeightArray,
									 double p,int nBinaryMethod,
									 double dSpecified)
{
	//DataArray ԭʼ����
	//WeightArray ΪȨ������
	//p	ȡֵΪ1�ĸ��ʣ�����ֵ��
	//nBinaryMethod 0,1���л�������
	//Ϊ0Ϊ������
	//Ϊ1�Ծ�ֵ������
	//Ϊ2����λ��������
	//Ϊ3���û�ָ��ֵ������
	//dSpecified Ϊ�û�ָ��ֵ
	double ff=p;
	int * pIntData=NULL;
//	CDoubleVector vector(pData,nLen);
//	WriteData(vector,"vector");
	double nLen=DataArray.GetSize();
	if(nBinaryMethod==0)
	{
		for(int j=0;j<nLen;j++)
		{
			if(!((DataArray.GetAt(j)+1==1.0)||(DataArray.GetAt(j)==1.0)))
			{
				CTString str1=m_pDataInterface->GetFieldName(m_nFieldIndex);
				str1+="������ֵδ����0��1��ֵ���������ܽ��ж���ֲ�����!";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str1);
				return false;
			}
		}
	}
	
	pIntData=Binarilize(DataArray,WeightArray,nBinaryMethod,dSpecified);
	double Value0Count=0;
	double Value1Count=0;
	for(int i=0;i<nLen;i++)
	{
		if(pIntData[i]==0)
			Value0Count+=WeightArray.GetAt(i);
		else
			Value1Count+=WeightArray.GetAt(i);
	}
	double np=(int)(nLen*p);
	double P=0;	
	double count=0;
	double count2=0;
	if(pIntData[0]==0)
	{
		count=Value0Count;
		count2=Value1Count;
	}
	else
	{
		count=Value1Count;
		count2=Value0Count;
	}

	nLen=Value1Count+Value0Count;///the lenght has been modified for the weight case

	if(nLen<=20)
	{
		if(count<np)
		{
			for(int k=0;k<=count;k++)
			{
				P=P+Cnk(nLen,k)*pow(p,k)*pow((1-p),(nLen-k));
			}
		}
		else if(count==np)
		{
			P=0.5;
		}
		else
		{
			for(int k=0;k<=count2;k++)
			{	
				P=P+Cnk(nLen,k)*pow(p,(nLen-k))*pow((1-p),k);
			}
		} 

	}
	else
	{
		double Z=fabs(nLen*p-count);
		Z=Z-0.5;
		double dividend=nLen*p*(1-p);
		dividend=sqrt(dividend);
		Z=Z/dividend;
		P=1-normcdf(Z,0,1);
	}
	if(p==0.5)
		P=2*P;
	if(P>1)
		P=1;
	CTString str="��1\n��2\n�ܼ�";
	m_GroupStrArray.Add(str);
	str.Empty();

	if(nBinaryMethod==0)
	{

		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nFieldIndex);
		
		CTString labelVal;
		CTString temp;
		int valFirst=0;
		int valSecond=1;
		if(pIntData[0]==0)	
		{
			valFirst=0;
			valSecond=1;
		}
		else
		{
			valFirst=1;
			valSecond=0;
		}
		if(m_pDataInterface->GetShowValueLabel() && 
			pField->GetValueVL(valFirst,labelVal))
		{
			labelVal+="\n";
		}
		else
		{
			labelVal.Format("%d\n",valFirst);
		}
		temp=labelVal;
		if(!m_pDataInterface->GetShowValueLabel() || !pField->GetValueVL(valSecond,labelVal))
		{
			labelVal.Format("%d",valSecond);
		}	
		temp+=labelVal;
		str=temp;
	}
	else
	{
		if(pIntData[0]==0)
			str.Format("<=%.4f\n>%.4f",dSpecified,dSpecified);
		else
			str.Format(">%.4f\n<=%.4f",dSpecified,dSpecified);
	}

	double tempCount=count<count2?count:count2;
	m_minPropArray.Add(tempCount/(count+count2));
	m_NameStrArray.Add(m_pDataInterface->GetFieldName(m_nFieldIndex));
	m_CategoryStrArray.Add(str);
	str.Empty();
	str.Format("%.0f\n%.0f\n%.0f",count,count2,Value0Count+Value1Count);
	m_FrequencyStrArray.Add(str);
	str.Empty();
	str.Format("%.4f\n%.4f\n%.4f",count/(double(nLen)),count2/(double(nLen)),1.0);
	m_ObservedStrArray.Add(str);
	m_TestPropArray.Add(p);
	m_SigArray.Add(P);
	if(pIntData)
		delete []pIntData;
	return true;
}

void CBinomialTest::CreateResult()
{
	int nRow=m_NameStrArray.GetSize();

	if(nRow>0)
	{

		///*to add the discription
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//�Ƿ���ʾע��,0 ����ʾ,������ʾ #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);
	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->InsertColumn(0,"���˵��");
			//pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<nRow;i++)
			{	
				CTString strtemp;
				if(m_SigArray.GetAt(i)<0.05)
				{
					strtemp.Format("%s�۲����%.4f��������%.4f�������Բ���(���ʼ���ֵ=%.4f)��\n",m_NameStrArray.GetAt(i).GetData(),m_minPropArray.GetAt(i),m_TestPropArray.GetAt(i),m_SigArray.GetAt(i));
				}
				else
				{
					strtemp.Format("%s�۲����%.4f��������%.4f�������Բ���(���ʼ���ֵ=%.4f)��\n",m_NameStrArray.GetAt(i).GetData(),m_minPropArray.GetAt(i),m_TestPropArray.GetAt(i),m_SigArray.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("���˵��",pTable);//����һ���������Ķ���
			m_pResult->Add(pETable);
		}
		//*/to end the discription

	
		CTLTable * pTable = new CTLTable;
		pTable->CreateTable(nRow+1,7);

		CTString str="ɾ����¼�У�";
		int nDefault=m_RowArray.GetSize();

//		pTable->SetCols(7);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"������");
		pTable->InsertColumn(1,"����");
		pTable->InsertColumn(2,"��");
		pTable->InsertColumn(3,"Ƶ��");
		pTable->InsertColumn(4,"�۲����");
		pTable->InsertColumn(5,"�������");
		pTable->InsertColumn(6,"P    ֵ");

		for(int i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_NameStrArray.GetAt(i));
			pTable->SetItemText(i,1,m_GroupStrArray.GetAt(i));
			pTable->SetItemText(i,2,m_CategoryStrArray.GetAt(i));
			pTable->SetItemText(i,3,m_FrequencyStrArray.GetAt(i));
			pTable->SetItemText(i,4,m_ObservedStrArray.GetAt(i));
			pTable->SetItemText(i,5,m_TestPropArray.GetAt(i));
			pTable->SetItemText(i,6,m_SigArray.GetAt(i));
		}

		pETable=new CRsltElementTable("����ֲ�����",pTable);//����һ���������Ķ���
		m_pResult->Add(pETable);	
	}

}


BOOL CBinomialTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{

//BINTEST
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/WEIGHT [varname]
//	/OPTION [PROP({0.5**})] [{NONE},{MEAN},{MEDIAN},{DEFINE({0**})}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		CTStringArray xname;
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
			return FALSE;
		if (m_tVarInfo.iCount == 1)
			m_nWeightCol = m_tVarInfo.pValue[0];
		else
			return FALSE;
	}

	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	if (AnaSyntax::findstr("/OPTION", "MEAN", VOstr))
		m_nBttn = 1;
	else if (AnaSyntax::findstr("/OPTION", "MEDIAN", VOstr) )
		m_nBttn = 2;
	else if (AnaSyntax::findstr("/OPTION", "DEFINE", VOstr) )
	{
		m_nBttn = 3;
		m_dSpecify = AnaSyntax::getvalue("/OPTION", "DEFINE", VOstr);
	}
	else
		m_nBttn = 0;

	m_dProportion = 0.5;
	if (AnaSyntax::findstr("/OPTION", "PROP", VOstr) )
		m_dProportion = AnaSyntax::getvalue("/OPTION", "PROP", VOstr);

	return TRUE;
}

CResult* CBinomialTest::OnBinomialTest(CDataInterface *pDataInterface, string VO)
{
	FreeVariable();
	m_pResult =new CResult("����ֲ�����");

	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	m_bWeight=false;
	TDataSet * pWeightArray=NULL;
	TDataSet *pDataArray=NULL;
	CTArray<double,double> WeightArray;
	CTArray<double,double> DataArray;
	m_NameStrArray.RemoveAll();
	m_minPropArray.RemoveAll();
	m_TestPropArray.RemoveAll();
	m_SigArray.RemoveAll();
	int m_nWeightCol=m_pDataInterface->GetWeightNumb();
	if(m_nWeightCol>=0)
	{
		m_bWeight=true;
		int nRow=m_pDataInterface->GetColData(m_nWeightCol,pWeightArray,5);
	}
	for(int i=0;i<m_tVarInfo.iCount;i++)
	{
		
		//	m_pRsltElementText=NULL;
		m_nFieldIndex = m_tVarInfo.GetIDAt(i);							//ȡ�ñ������к�
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//ȡ������
		//ASSERT( nCaseNum > 0 );
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=pDataInterface->GetFieldName(m_nFieldIndex);
			str+="��Ч����̫��,���ܽ��ж���ֲ����飡";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("����");
			CTString str=pDataInterface->GetFieldName(m_nFieldIndex);
			str+="����Դ̫�󣬲��ܽ��ж���ֲ����飡";
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
					WeightArray.Add(floor(pWeightArray[j].dValue+0.5));
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
			CTString str=m_pDataInterface->GetFieldName(m_nFieldIndex);
			str+="û��Ч���ݣ����ܽ��ж���ֲ����飡";
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
				CTString str=m_pDataInterface->GetFieldName(m_nFieldIndex);
				str+="��Ч����С��2�������ܽ��ж���ֲ����飡";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("����");
				m_pRsltElementText->AddString(str);
				continue;
			}
		}
		
		BinomialTestMain(DataArray,WeightArray,m_dProportion,m_nBttn,m_dSpecify);
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

void CBinomialTest::FreeVariable()
{
	m_RowArray.RemoveAll();
	m_TestPropArray.RemoveAll();
	m_SigArray.RemoveAll();
	m_NameStrArray.RemoveAll();
	m_GroupStrArray.RemoveAll();
	m_CategoryStrArray.RemoveAll();
	m_FrequencyStrArray.RemoveAll();
	m_ObservedStrArray.RemoveAll();
}