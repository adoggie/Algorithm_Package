// RunTest.h: interface for the CRunTest class.
//         游程检验
//      标注： 余卫军 2005年10月20日
//			修改：马良庄	2006/03/22
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlcrnt.h"
#include "RunTest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRunTest::CRunTest()
{
	m_dSpecify=0;
	m_nBttn=0;
	m_pResult=NULL;
	m_pRsltElementText=NULL;
	m_bWeight=false;
	m_nWeightCol = -1;
}

CRunTest::~CRunTest()
{
	FreeVarialbe();
	m_tVarInfo.DeleteObject();

}


int * CRunTest::Binarilize(CTArray<double,double> &DataArray,
								CTArray<double,double> &WeightArray,
								int nBinaryMethod,double &dSpecified)
{
	//pData 原始数据
	//nBinaryMethod 0,1序列化处理方法
	//为0为不处理
	//为1以均值来处理
	//为2以中位数来处理
	//为3以用户指定值来处理
	//为4以峰值来处理
	int nLen=DataArray.GetSize();

	//dSpecified 拥护指定值
	int *pIntData=new int[nLen];

	switch(nBinaryMethod)
	{
		case 0:
			{
				for(int i=0;i<nLen;i++)
					pIntData[i]=(int)(DataArray.GetAt(i));
				dSpecified=1;
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
					double dd = (double)DataArray.GetAt(nMedian);
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


BOOL CRunTest::RunTestMain(CTArray<double,double> &DataArray,
						   CTArray<double,double> &WeightArray,
						   int nBinaryMethod,double dSpecified)
{
	//pData 原始数据
	//p	取值为1的概率（检验值）
	//nBinaryMethod 0,1序列化处理方法
	//为0为不处理
	//为1以均值来处理
	//为2以中位数来处理
	//为3以用户指定值来处理
	//dSpecified 为用户指定值
	int nLen=DataArray.GetSize();
	int * pIntData=NULL;
	if(nBinaryMethod==0)
	{
		for(int j=0;j<nLen;j++)
		{
			if(!((DataArray.GetAt(j)+1==1.0)||(DataArray.GetAt(j)==1.0)))
			{
				CTString str;
				str=m_pDataInterface->GetFieldName(m_nFieldIndex);
				str+="样本数值未经过0，1二值化处理，不能进行游程检验！";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
				m_pRsltElementText->AddString(str);
				if(pIntData)
					delete []pIntData;
				//delete m_pResult;
				//m_pResult=NULL;
				return false;
			}
		}
	}
	
	pIntData=Binarilize(DataArray,WeightArray,nBinaryMethod,dSpecified);
	double Value0Count=0;
	double Value1Count=0;
	double RunCount=1;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		if(pIntData[i]==0)
			Value0Count+=WeightArray.GetAt(i);
		else
			Value1Count+=WeightArray.GetAt(i);
	}
	for(i=1;i<nLen;i++)
	{
		if(pIntData[i-1]!=pIntData[i])
		{
			RunCount++;
		}
	}
	CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
	if(Value0Count==0||Value1Count==0)
	{
		CTString str=strName;
		str+="只有一个游程，检验终止！";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		if(pIntData)
			delete []pIntData;
		return false;
	}
	double P=0,Z=0,fenzi=0,mu=0;
	double N=Value0Count+Value1Count;
	mu=1+((double)(2.0*Value1Count*Value0Count))/((double)(N));
	
	double temp=2.0*Value1Count*Value0Count;
	temp=temp*(temp-N);
	temp=temp/N;
	temp=temp/N;
	temp=temp/(N-1);///////////////////////////
	temp=sqrt(temp);
	double sigma=temp;	
	fenzi=RunCount-mu;
	if(N<50)
	{
		if(fenzi<=-0.5)
			Z=(fenzi+0.5)/sigma;
		else if(fenzi>-0.5&&fenzi<0.5)
			Z=0;
		else
			Z=(fenzi-0.5)/sigma;
	}
	else
	{
		Z=fenzi/sigma;
	}
	//Z=Z*90;
	if(Z<=0)
		P=2*normcdf(Z,0,1);
	else
		P=2*(1-normcdf(Z,0,1));
	if(P>1)
		P=1;
	if(pIntData)
		delete []pIntData;
	CTString str;
	m_NameStrArray.Add(strName);
	if(nBinaryMethod==0)
	{

		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(m_nFieldIndex);
		
		CTString labelVal;
		CTString temp;
		int valFirst=0;
		int valSecond=1;
		if(m_pDataInterface->GetShowValueLabel() && pField->GetValueVL(valFirst,labelVal))
		{
			labelVal+="\n";
		}
		else
		{
			labelVal.Format("%d\n",valFirst);
		}
		temp=labelVal;
		if(!m_pDataInterface->GetShowValueLabel() ||!pField->GetValueVL(valSecond,labelVal))
		{
			labelVal.Format("%d",valSecond);
		}	
		temp+=labelVal;
		str=temp;
		str+="\n总计";

	}
	else
		str.Format("<=%.4f\n>%.4f\n总计",dSpecified,dSpecified);
	m_GroupStrArray.Add(str);
	str.Empty();
	str.Format("%.0f\n%.0f\n%.0f",Value0Count,Value1Count,Value0Count+Value1Count);
	m_FrequencyStrArray.Add(str);
	m_RunArray.Add(RunCount);
	m_ZArray.Add(Z);
	m_PArray.Add(P);
	return true;
}

void CRunTest::CreateResult()
{
	int nRow=m_NameStrArray.GetSize();
	if(nRow>0)
	{

		//to add the output discription zkc 2003.8.12
		CRsltElementTable * pETable = NULL;
		int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
		CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
		if (nOutNote)
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,1);
			pTable->InsertColumn(0,"结果说明");
//			pTable->m_colorCell=RGB(255,0,0);
			for(int i=0;i<nRow;i++)
			{
				CTString strtemp;
				if(m_PArray.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s不服从随机分布(概率检验值=%.4f)。\n",m_NameStrArray.GetAt(i).GetData(),m_PArray.GetAt(i));
				}
				else
				{
					strtemp.Format("%s服从随机分布(概率检验值=%.4f)。\n",m_NameStrArray.GetAt(i).GetData(),m_PArray.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
		}		

		// to end the output discription.

		//CTLTable * pTable = NULL;
		//CRsltElement *pETable=NULL;
		CTLTable *pTable = new CTLTable;
		pTable->CreateTable(nRow+1,6);
//		pTable->SetCols(6);
//		pTable->SetRows(nRow+1);

		pTable->InsertColumn(0,"变量名");
		pTable->InsertColumn(1,"分组");
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"总游程数");
		pTable->InsertColumn(4,"Z");
		pTable->InsertColumn(5,"P    值");
		
		for(int i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_NameStrArray.GetAt(i));
			pTable->SetItemText(i,1,m_GroupStrArray.GetAt(i));
			pTable->SetItemText(i,2,m_FrequencyStrArray.GetAt(i));
			pTable->SetItemText(i,3,m_RunArray.GetAt(i));
			pTable->SetItemText(i,4,m_ZArray.GetAt(i));
			pTable->SetItemText(i,5,m_PArray.GetAt(i));
		}

		pETable=new CRsltElementTable("游程检验",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);		
	}
}

BOOL CRunTest::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//RUNTEST
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/WEIGHT [varname]
//	/OPTION [{NONE},{MEAN},{MEDIAN},{DEFINE({0**})}]
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
	
	return TRUE;
}

CResult* CRunTest::OnRunTest(CDataInterface *pDataInterface, string VO)
{
	FreeVarialbe();
	m_pResult =new CResult("游程检验");
	m_pRsltElementText=NULL;
	m_pDataInterface = pDataInterface;
	m_bWeight=false;
	TDataSet * pWeightArray=NULL;
	TDataSet *pDataArray=NULL;
	CTArray<double,double> WeightArray;
	CTArray<double,double> DataArray;
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}	

	if(m_nWeightCol>=0)
	{
		m_bWeight=true;
		int nRow=m_pDataInterface->GetColData(m_nWeightCol,pWeightArray,5);
	}
	
	
	for(int i=0;i<m_tVarInfo.iCount;i++)
	{			
		m_nFieldIndex = m_tVarInfo.pValue[i];							//取得变量的列号
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//取回数据
		if(nCaseNum<2)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=pDataInterface->GetFieldName(m_nFieldIndex);
			str+="有效数据太少，不能进行游程检验！";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=pDataInterface->GetFieldName(m_nFieldIndex);
			str+="数据源太大，不能进行游程检验！";
			m_pRsltElementText->AddString(str);
			break;
		}
		for(int j=0;j<nCaseNum;j++)
		{		
			if(m_bWeight)//假如指定了权重
			{
				
				if(pWeightArray[j].iIndex>=0&&pDataArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//权重与数据列均未缺失
				{
					DataArray.Add(pDataArray[j].dValue);
					WeightArray.Add(floor(pWeightArray[j].dValue+0.5));
				}
				else//否则记录被忽略的行
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//假如未指定权中
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
		if(DataArray.GetSize()<2)
		{ 
			DataArray.RemoveAll();
			WeightArray.RemoveAll();
			CTString str=m_pDataInterface->GetFieldName(m_nFieldIndex);
			str+="有效数据太少，不能进行游程检验！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			continue;
		}			
		RunTestMain(DataArray,WeightArray,m_nBttn,m_dSpecify);
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

 void CRunTest::FreeVarialbe()
 {
	m_NameStrArray.RemoveAll();
	m_GroupStrArray.RemoveAll();
	m_FrequencyStrArray.RemoveAll();
	m_RowArray.RemoveAll();
	m_RunArray.RemoveAll();
	m_ZArray.RemoveAll();
	m_PArray.RemoveAll();
 }
