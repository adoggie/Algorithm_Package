// OneSmplTTest.cpp: implementation of the COneSmplTTest class.
//                        单样本参数检验
//                    包括： 单样本的均值，方差，比例检验等
//                   标注： 余卫军 2005年10月21日
//									马良庄		2006/03/21
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
	
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u ≠ u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u ＞ u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u ＜ u0
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
	
	//检验类型
	//		m_nIsSTD		= pVo->m_nIsSTD;//0:方差  1:标准差
	//		m_dErrValue		= pVo->m_dErrValue; //方差或者标准差的值
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

//单样本T检验
CResult * COneSmplTTest::OnOneSmplTTest(CDataInterface *pDataInterface, string VO)//接口函数
{
	FreeMembers();
	CResult *pResult;
	pResult =new CResult("单样本均值检验");
	m_pResult = pResult;
	
	if (!GetValueFromVoT(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
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
		
		m_nFieldIndex = m_tVarInfo.pValue[i];							//取得变量的列号
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//取回数据
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=strName;
			str+="有效数据太少,不能进行单样本T检验！";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=strName;
			str+="数据源太大，不能进行单样本T检验！";
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
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//否则记录被忽略的行
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//假如未指定权重
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
			str+="没有效数据，不能进行单样本T检验！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
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
				str+="有效数据小于2个，不能进行单样本T检验！";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
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

bool COneSmplTTest::Test(CTArray<double,double> &Data,//数据
						 CTArray<double,double> &Weight,//权重
						 CTString strName,int method)//字段名 //method =0 T检验， method = 1 方差检验 method =2 比例检验
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
			str+="有效数据太少，不能进行单样本T检验。";
		if(method==1)
			str+="有效数据太少，不能进行单样本方差检验。";
		if(method==2)
			str+="有效数据太少，不能进行单样本比例检验。";
		
		if(m_pRsltElementText==NULL)
			m_pRsltElementText =new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}
	//*************** 计算参差 start **********************
	if (m_bNormalTest)
	{
		//计算权重和//#_S 2005-5-27 zlq
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
	//*************** 计算参差 end **********************
	
	double Xmean=sum/W;					//均值
	for(i=0;i<nSize;i++)
	{
		double temp=Data[i]-Xmean;
		Sx=Sx+temp*temp*Weight[i];
	}
	
	Sx=sqrt(Sx/(W-1));//标准差
	double Sxmean=Sx/sqrt(W);					//均值标准误
	//////////////////////////////////////////////////////////////////////////
	
	if(fabs(Sx)<MIN)
	{
		if(method != 2)
		{
			CTString str="警告: " + strName;
			if(method ==0)
				str+="标准离差为0，不能进行单样本T检验。";
			if(method ==1)
				str+="标准离差为0，不能进行单样本方差检验。";		
			if(m_pRsltElementText==NULL)
				m_pRsltElementText =new CRsltElementText("警告");
			m_pRsltElementText->AddString(str);
			return false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//单样本T检验
	if(method ==0)
	{
		double D=Xmean-m_dValue;
		double t=D/Sxmean;							//t
		//计算U统计量
		//m_nIsSTD;//0:方差  1:标准差
		//m_dErrValue; //方差或者标准差的值
		
		double sigma = 1.0;	
		if(m_nIsSTD == 1)
			sigma = m_dErrValue;
		else
			sigma = sqrt(m_dErrValue);
		double U = D/(sigma /sqrt(W));//U统计量
		
		double df=W-1;								//自由度
		double P=2*(1-tcdf(fabs(t),df));			//显著性
		double lower=Xmean-tinv((m_dConfidence+1)/2,df)*Sxmean;	//下限
		double upper=Xmean+tinv((m_dConfidence+1)/2,df)*Sxmean;
		
		//检验表
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
	//计算单样本方差卡方检验统计量
	if(method ==1 )
	{
		double sigma = 1.0;	
		
		if(m_nIsSTD == 1)
			sigma = m_dValue*m_dValue;
		else
			sigma = fabs(m_dValue);//一定要确保非零非负
		assert(sigma >0);
		if(sigma <= 0)
			sigma =1.0;
		double SS =Sx*Sx*(W-1);
		double Z = SS/sigma;//卡方统计量
		m_ChiSquareAr.Add(Z);
		double df=W-1;								//自由度
		double Alpha = 1-m_dConfidence;
		
		double upper=SS/chi2inv(Alpha/2.0,df);	//下限
		double lower=SS/chi2inv(1.0-Alpha/2.0,df);	//上限
		if(m_nCheckType !=0)
		{
			upper=SS/chi2inv(Alpha,df);	//下限
			lower=SS/chi2inv(1.0-Alpha,df);	//上限		
		}
		CTString str("");
		str.Format("%.0f",df);
		m_dfStrAr.Add(str);
		
		m_LowerAr.Add(lower);
		m_UpperAr.Add(upper);	
	}
	//////////////////////////////////////////////////////////////////////////
	if(method ==2) //比例检验
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
		m_UpperAr.Add(p2);//近似的区间估计	
	}
	//格式化输出
	
	//统计表
	CTString str(strName);
	m_NameStrAr.Add(str);
	
	str.Format("%.0f",W);
	m_FrequencyStrAr.Add(str);
	m_MeanAr.Add(Xmean);
	m_StdDAr.Add(Sx);
	m_StdEMAr.Add(Sxmean);
	
	return true;
	
}


void COneSmplTTest::FreeMembers()//清除变量
{
	m_NameStrAr.RemoveAll();			//变量名
	m_FrequencyStrAr.RemoveAll();		//频数
	m_MeanAr.RemoveAll();				//均值
	m_StdDAr.RemoveAll();				//标准离差
	m_StdEMAr.RemoveAll();				//均值标准误
	
	//用于检验表
	m_tAr.RemoveAll();					//t
	m_uAr.RemoveAll();                  //u
	m_dfStrAr.RemoveAll();				//自由度
	m_PAr.RemoveAll();					//显著性
	m_DFAr.RemoveAll();					//均差
	m_LowerAr.RemoveAll();				//下限
	m_UpperAr.RemoveAll();				//上限
	m_ChiSquareAr.RemoveAll();
}


void COneSmplTTest::CreateResult(int method/* =0 */)//格式化输出
{
	int nRow=m_NameStrAr.GetSize();
	if(nRow>0)
	{
		//to add the output discription zkc 2003.8.13
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
				if(m_PAr.GetAt(i)<=0.05)		
				{
					strtemp.Format("%s检验值与样本的均值有显著差别(概率检验值=%.4f)。\n",m_NameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				else
				{
					strtemp.Format("%s检验值与样本的均值无显著差别(概率检验值=%.4f)。\n",m_NameStrAr.GetAt(i).GetData(),m_PAr.GetAt(i));
				}
				pTable->InsertItem(i+1,strtemp);
			}
			pETable= new CRsltElementTable("结果说明",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
			
		}
		// to end the output discription.
		
		
		
		//统计表
		if(method != 2)//比例检验的统计表单独输出
		{
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,5);
			pTable->SetTitle("统计");
			CTString strValue;
			strValue.Format("检验值=%.2f",m_dValue);
			pTable->SetHead(strValue);
			//		pTable->SetCols(5);
			//		pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"变量名");
			pTable->InsertColumn(1,"记录数");
			pTable->InsertColumn(2,"均值");
			pTable->InsertColumn(3,"标准离差");
			pTable->InsertColumn(4,"均值标准误");
			for(int i=0;i<nRow;i++)
			{
				pTable->InsertItem(i,m_NameStrAr.GetAt(i),false);
				pTable->SetItemText(i,1,m_FrequencyStrAr.GetAt(i));
				pTable->SetItemText(i,2,m_MeanAr.GetAt(i));
				pTable->SetItemText(i,3,m_StdDAr.GetAt(i));
				pTable->SetItemText(i,4,m_StdEMAr.GetAt(i));
			}
			pETable=new CRsltElementTable("统计",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
		}
		//检验表
		if(method ==0)
		{
			CTLTable* pTable = new CTLTable;
			pTable->CreateTable(nRow+1,7);
			pTable->SetTitle("T检验结果");
			//		pTable->SetCols(7);
			//			pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"变量名");
			pTable->InsertColumn(1,"T统计量");
			pTable->InsertColumn(2,"自由度");
			pTable->InsertColumn(3,"Pr(>|T|)");
			//pTable->InsertColumn(4,"u-u0");
			CTString str,str2;
			str.Format("(%d%%置信度)", int(m_dConfidence*100));
			str2="下限"+str;
			pTable->InsertColumn(4,str2);
			str2="上限"+str;
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
				CreateTestTable(i);//输出假设检验表
			}
			
			pETable=new CRsltElementTable("检验结果",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
			//////////////////////////////////////////////////////////////////////////
		}	
		if(method ==1)//输出方差检验表
		{
			for(int i=0;i<nRow;i++)
				CreateChiTestTable(i);//输出假设检验表		
		}
		if(method ==2)//输出比例检验表
		{
			//输出统计表
			CTLTable * pTable = new CTLTable;
			pTable->CreateTable(nRow+1,6);
			pTable->SetTitle("统计表");
			CTString strValue;
			strValue.Format("检验值=%.2f",m_dValue);
			pTable->SetHead(strValue);
			//			pTable->SetCols(6);
			//			pTable->SetRows(nRow+1);
			
			pTable->InsertColumn(0,"变量名");
			pTable->InsertColumn(1,"分  组");
			pTable->InsertColumn(2,"频  数");
			pTable->InsertColumn(3,"频  率");
			pTable->InsertColumn(4,"标准离差");
			pTable->InsertColumn(5,"均值标准误");
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
			pETable=new CRsltElementTable("统计",pTable);//申请一个表对象类的对象
			m_pResult->Add(pETable);
			
			for(i=0;i<nRow;i++)
				CreatePropotionTestTable(i);//输出假设检验表		
		}
		
		CreateCharts();
		
	}
	
}
void COneSmplTTest::CreateCharts()
{
	//**************************** 残差正态概率图 START
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
			pPPPlot1->SetTitle(m_NameStrAr.GetAt(iCol) + "-正态概率图");
			pPPPlot1->SetXAxilLabel("正态分位数");
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
			CRsltElementChart*pREC=new CRsltElementChart(m_NameStrAr.GetAt(iCol) + "-正态概率图",pPPPlot1);
			m_pResult->Add(pREC);
			delete []pDPVV;
		}
		if (m_bHistogram)
		{		
			int nDV=0, nPtCnt=0;
			double fMean=0, fStdv=0, fSum=0;
			void *pDPSV1=NULL, *pDPSV2=NULL;
			CTString szTemp("");
			//排序
			CDoubleVector v = AveVect;
			v.Sort();
			//直方图
			CTChartHistogram *pHistogram= new CTChartHistogram;
			pHistogram->SetNoraml(true);
			pHistogram->CalcXYAxis(v,&pDPSV1,nDV);  //直方图
			pHistogram->CalcNormalCurve(v,&pDPSV2,nPtCnt);//曲线图
			pHistogram->GetParam(fMean,fStdv,fSum);
			szTemp.Format("均值：%.2f\n标准差：%.2f\n记录数：%.0f",fMean,fStdv,fSum);
			pHistogram->SetHistogram(pDPSV1,nDV,ValVal,szTemp);
			pHistogram->SetData(pDPSV2,nPtCnt,ELine,ValVal);
			pHistogram->SetXAxilLabel(m_NameStrAr.GetAt(iCol));
			pHistogram->SetYAxilLabel("频数");
			pHistogram->SetTitle(m_NameStrAr.GetAt(iCol) + "-直方图");
			CRsltElementChart *pChart = new CRsltElementChart(m_NameStrAr.GetAt(iCol) + "-直方图",pHistogram);
			m_pResult->Add(pChart);
			delete []pDPSV1;
			delete []pDPSV2;
		}
		delete pErrorAry ;
	}
	m_ErrorAry.RemoveAll();
	//**************************** 残差正态概率图 END  */
	
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
	sTitle.Format("原假设 : H0 : p ＝ %.4f",m_dValue);
	sTitle = m_NameStrAr.GetAt(index) +"—比例检验结果表\n"+sTitle;
	table->SetTitle(sTitle);
	//	table->SetCols(5);
	//	table->SetRows(nRow+1);
	
	
	table->InsertColumn(0,"备择假设");
	//table->InsertColumn(1,"p-p0");
	table->InsertColumn(1,"Z统计量");
	table->InsertColumn(2,"P 值");
	
	if(m_nCheckType == 0)
		str.Format("p的%d%%置信区间", int(m_dConfidence*100));
	if(m_nCheckType == 1)
		str.Format("p的%d%%置信下限", int(m_dConfidence*100));
	if(m_nCheckType == 2)
		str.Format("p的%d%%置信上限", int(m_dConfidence*100));
	table->InsertColumn(3,str);
	
	double Z =m_uAr.GetAt(index); //
	if(m_bGT)// H1 : p ＞ p0
	{
		str.Format("H1 : p ＞ %.4f",m_dValue);
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
	
	if(m_bLT)// H1 : p ＜ p0
	{
		str.Format("H1 : p ＜ %.4f",m_dValue);
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
	
	if(m_bNE)// H1 : p ≠ p0
	{
		str.Format("H1 : p ≠ %.4f",m_dValue);
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
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "检验结果",table);//申请一个表对象类的对象
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
	
	sTitle.Format("原假设 : H0 : Var ＝ %.4f,均值未知 ",dbVar);
	sTitle = m_NameStrAr.GetAt(index) +"—方差检验结果表\n"+sTitle;
	table->SetTitle(sTitle);
	//	table->SetCols(6);
	//	table->SetRows(nRow+1);
	
	
	table->InsertColumn(0,"备择假设");
	//	table->InsertColumn(1,"Var/Var0");
	table->InsertColumn(1,"卡方统计量");
	table->InsertColumn(2,"自由度");
	table->InsertColumn(3,"P 值");
	
	if(m_nCheckType == 0)
		str.Format("方差的%d%%置信区间", int(m_dConfidence*100));
	if(m_nCheckType == 1)
		str.Format("方差的%d%%置信下限", int(m_dConfidence*100));
	if(m_nCheckType == 2)
		str.Format("方差的%d%%置信上限", int(m_dConfidence*100));
	
	table->InsertColumn(4,str);
	
	double K =m_ChiSquareAr.GetAt(index); //
	if(m_bGT)// H1 : Var ＞ Var0
	{
		str.Format("H1 : Var ＞ %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//m_dfStrAr
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		table->SetItemText(iCount,3,min(1-chi2cdf(K,atoi(m_dfStrAr.GetAt(index))),1));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +∞)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		
		iCount++;
	}
	
	if(m_bLT)// H1 : Var ＜ Var0
	{
		str.Format("H1 : Var ＜ %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		
		table->SetItemText(iCount,3,chi2cdf(K,atoi(m_dfStrAr.GetAt(index))));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +∞)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		
		iCount++;
	}
	
	if(m_bNE)// H1 : Var ≠ Var0
	{
		str.Format("H1 : Var ≠ %.4f",dbVar);
		table->InsertItem(iCount,str,true);
		//	table->SetItemText(iCount,1,m_StdDAr.GetAt(index)*m_StdDAr.GetAt(index)/dbVar);
		table->SetItemText(iCount,1,K);
		table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
		
		table->SetItemText(iCount,3,min(2*(1-chi2cdf(fabs(K),atoi(m_dfStrAr.GetAt(index)))),2*chi2cdf(fabs(K),atoi(m_dfStrAr.GetAt(index)))));
		
		if(m_nCheckType ==0)
			str.Format("(%.4f , %.4f)",m_LowerAr.GetAt(index),m_UpperAr.GetAt(index));
		if(m_nCheckType ==1)
			str.Format("(%.4f , +∞)",m_LowerAr.GetAt(index));
		if(m_nCheckType ==2)
			str.Format("(0 , %.4f)",m_UpperAr.GetAt(index));
		
		table->SetItemText(iCount,4,str);
		iCount++;
	}			
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index) + "检验结果",table);//申请一个表对象类的对象
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
	if(m_nVarType == 1)//方差已知
	{
		double dbVar = 0;
		if(m_nIsSTD == 1)
			dbVar = m_dErrValue*m_dErrValue;
		else
			dbVar = m_dErrValue;
		sTitle.Format("原假设 : H0 : u ＝ %.2f， 已知方差 = %.4f",m_dValue,dbVar);
		sTitle = m_NameStrAr.GetAt(index) +"—均值检验结果表\n"+sTitle;
		table->SetTitle(sTitle);
		//		table->SetCols(4);
		//		table->SetRows(nRow+1);
		
		
		table->InsertColumn(0,"备择假设");
		//table->InsertColumn(1,"u-u0");
		table->InsertColumn(1,"Z统计量");
		table->InsertColumn(2,"P 值");
		//开始填充表格
		
		double T =m_uAr.GetAt(index); //
		
		if(m_bGT)// H1 : u ＞ u0
		{
			str.Format("H1 : u ＞ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,1-normcdf(T,0,1));
			iCount++;
		}
		
		if(m_bLT)// H1 : u ＜ u0
		{
			str.Format("H1 : u ＜ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,normcdf(T,0,1));
			
			iCount++;
		}
		
		if(m_bNE)// H1 : u ≠ u0
		{
			str.Format("H1 : u ≠ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			table->SetItemText(iCount,1,m_uAr.GetAt(index));
			table->SetItemText(iCount,2,2*(1-normcdf(fabs(T),0,1)));
			
			iCount++;
		}			
	}
	else//方差未知
	{
		sTitle.Format("原假设 : H0 : u ＝ %.2f， 方差未知",m_dValue);
		table->SetTitle(sTitle);
		table->CreateTable(nRow+1,5);
		//		table->SetCols(5);
		//		table->SetRows(nRow+1);
		
		
		table->InsertColumn(0,"备择假设");
		//table->InsertColumn(1,"u-u0");
		table->InsertColumn(1,"T统计量");
		table->InsertColumn(2,"自由度");
		
		table->InsertColumn(3,"P 值");
		
		double T =m_tAr.GetAt(index);//T统计量
		int df =atoi(m_dfStrAr.GetAt(index));
		
		if(m_bGT)// H1 : u ＞ u0
		{
			str.Format("H1 : u ＞ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//	table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,df);
			
			table->SetItemText(iCount,3,1-tcdf(T,df));
			
			iCount++;
		}
		
		if(m_bLT)// H1 : u ＜ u0
		{
			str.Format("H1 : u ＜ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
			
			table->SetItemText(iCount,3,tcdf(T,df));
			
			iCount++;
		}
		
		if(m_bNE)// H1 : u ≠ u0
		{
			str.Format("H1 : u ≠ %.4f",m_dValue);
			table->InsertItem(iCount,str,true);
			//table->SetItemText(iCount,1,m_DFAr.GetAt(index));
			table->SetItemText(iCount,1,m_tAr.GetAt(index));
			table->SetItemText(iCount,2,m_dfStrAr.GetAt(index));
			
			table->SetItemText(iCount,3,2*(1-tcdf(fabs(T),df)));
			
			iCount++;
		}
		
	}
	pETable=new CRsltElementTable(m_NameStrAr.GetAt(index)+ "检验结果",table);//申请一个表对象类的对象
	m_pResult->Add(pETable);	
}
//////////////////////////////////////////////////////////////////////////
//1、比例的单样本检验
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
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u ≠ u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u ＞ u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u ＜ u0
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

CResult* COneSmplTTest::OnOneSmplPropotionTest(CDataInterface *pDataInterface, string VO)//比例检验接口函数
{
	FreeMembers();
	
	CResult *pResult;
	pResult =new CResult("单样本比例检验");
	m_pResult = pResult;
	
	if (!GetValueFromVoPro(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
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
		
		m_nFieldIndex = m_tVarInfo.pValue[i];							//取得变量的列号
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//取回数据
		//assert( nCaseNum > 0 );
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=strName;
			str+="有效数据太少,不能进行单样本T检验！";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=strName;
			str+="数据源太大，不能进行单样本比例检验！";
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
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//否则记录被忽略的行
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//假如未指定权重
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
			str+="没有效数据，不能进行单样本T检验！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
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
				str+="有效数据小于2个，不能进行单样本比例检验！";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
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

//2、方差的单样本检验
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
	m_bNE = AnaSyntax::findstr("/MEANTEST", "EQU", VOstr);// H1 : u ≠ u0
	m_bGT = AnaSyntax::findstr("/MEANTEST", "LARGER", VOstr);// H1 : u ＞ u0
	m_bLT = AnaSyntax::findstr("/MEANTEST", "LESS", VOstr);// H1 : u ＜ u0
	
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

CResult* COneSmplTTest::OnOneSmplVarianceTest(CDataInterface *pDataInterface, string VO)//接口函数
{
	FreeMembers();
	
	CResult *pResult;
	pResult =new CResult("单样本方差检验");
	m_pResult = pResult;
	
	if (!GetValueFromVoVar(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
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
		m_nFieldIndex = m_tVarInfo.pValue[i];							//取得变量的列号
		CTString strName=m_pDataInterface->GetFieldName(m_nFieldIndex);
		int nCaseNum = pDataInterface->GetColData(m_nFieldIndex,pDataArray,5);	//取回数据
		//assert( nCaseNum > 0 );
		if(nCaseNum<1)
		{	
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str=strName;
			str+="有效数据太少,不能进行单样本卡方检验！";
			m_pRsltElementText->AddString(str);
			break;
		}
		if(nCaseNum>1e6)
		{
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
			CTString str="警告"+strName;
			str+="数据源太大，不能进行单样本卡方检验！";
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
					WeightArray.Add(pWeightArray[j].dValue);
				}
				else//否则记录被忽略的行
				{
					m_RowArray.Add(pDataArray[j].iIndex);
				}
			}
			else//假如未指定权重
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
			str+="没有效数据，不能进行单样本卡方检验！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
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
				str+="有效数据小于2个，不能进行单样本卡方检验！";
				if(m_pRsltElementText==NULL)
					m_pRsltElementText=new CRsltElementText("警告");
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





















