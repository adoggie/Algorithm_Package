// PairTTest.cpp: implementation of the CPairTTest class.
//				配对样本T检验
//					马良庄    2006/03/21
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
//Function : 得到两个配对样本合法数据，排除了空值与缺失值数据，处理方法是  //
//			 只要数据或权重中有一个有空值或缺失值，则把整行删掉（或排除在外//
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
		if(m_bWeight)//假如指定了权重
		{

			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0&&pWeightArray[j].iIndex>=0&&pWeightArray[j].dValue>0)//权重与数据列均未缺失
			{
				Weight.Add(int(pWeightArray[j].dValue+0.5));
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//否则记录被忽略的行
			{
				m_RowArray.Add(pData1Set[j].iIndex);
			}
		}
		else//假如未指定权中
		{			
			if(pData1Set[j].iIndex>=0&&pData2Set[j].iIndex>=0)//权重与数据列均未缺失
			{
				Weight.Add(1);
				Data1.Add(pData1Set[j].dValue);
				Data2.Add(pData2Set[j].dValue);
			}
			else//否则记录被忽略的行
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
	//FreeAllMemember();///-------------------有待恢复
	CResult *pResult;
	pResult = new CResult("配对样本T检验");
	m_pResult = pResult;

	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
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
			str1+="有效数据太少，不能进行配对样本T检验！";
			if(m_pRsltElementText==NULL)
				m_pRsltElementText=new CRsltElementText("警告");
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
	m_NameAr.RemoveAll();			//变量名
	m_MeanSAr.RemoveAll();			//均值
	m_FrequencySAr.RemoveAll();	//频数
	m_StdDSAr.RemoveAll();			//标准离差
	m_StdEMSAr.RemoveAll();		//均值标准误

	//相关性表
	m_PairNameAr.RemoveAll();		//变量名(配对)
	m_FrequencyPAr.RemoveAll();	//频数
	m_CorrAr.RemoveAll();			//相关性
	m_PAr.RemoveAll();				//显著性
	m_PArGT.RemoveAll();
	m_PArLT.RemoveAll();

	//检验表
	m_MeanPAr.RemoveAll();			//均值
	m_StdDPAr.RemoveAll();			//标准离差
	m_StdEMPAr.RemoveAll();		//均值标准误
	m_LowerAr.RemoveAll();			//下限
	m_UpperAr.RemoveAll();			//上限
	m_tAr.RemoveAll();				//t值
	m_dfStrAr.RemoveAll();			//自由度
	m_P2Ar.RemoveAll();			//显著性(双尾)
	for(int i=0;i<m_PtrArrayIndex.GetSize();i++)
	{
		delete m_PtrArrayIndex[i];
	}
	m_PtrArrayIndex.RemoveAll();

}

BOOL CPairTTest::PairTTest(CTArray<double,double> &Data1,//配对变量1数据
						   CTArray<double,double> &Data2,//配对变量2数据
						   CTArray<double,double> &Weight,//对应的权重
						   int column1,int column2,//对应的列索引
						   CTString strName)//配对变量名
{
	int nSize=Data1.GetSize();
	double Xsum=0,Ysum=0;		//Data1,Data2的加权求和
	double Sx=0,Sy=0;			//Data1均值的标准误,Data2均值的标准误
	double W=0;
	//*************** 计算残差 start **********************
	if (m_bNormalTest)
	{
		//计算权重和//#_S 2005-5-27 zlq
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
	//*************** 计算残差 end **********************
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
		str+="有效样本数为1，不能进行配对样本T检验。";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}

	//统计表
	double Xmean=Xsum/W;				//Data1的均值
	double Ymean=Ysum/W;				//Data2的均值
	Sx=(Sx-Xsum*Xsum/W)/(W-1);			//Data1的标准离差
	Sy=(Sy-Ysum*Ysum/W)/(W-1);			//Data2的标准离差
	Sx=sqrt(Sx);
	Sy=sqrt(Sy);
	double SXmean=Sx/sqrt(W);			//Data1均值的标准误
	double SYmean=Sy/sqrt(W);			//Data2均值的标准误

	//检验结果表
	double Sxy=0;//协方差
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
	Sd=sqrt(Sd);							//标准离差
	if(fabs(Sd)<MIN)
	{
		CTString str=strName;
		str+="差值的标准误为0，不能进行配对样本T检验。";
		if(m_pRsltElementText==NULL)
			m_pRsltElementText=new CRsltElementText("警告");
		m_pRsltElementText->AddString(str);
		return false;
	}

	double Sdmean=Sd/sqrt(W);					//差值的标准误			
	double Dmean=Xmean-Ymean;					//均值的差值
	double D=Xsum-Ysum;
	double t=Dmean/Sdmean;						//t值
	double df=W-1;								//自由度

	double P=2*(1-tcdf(fabs(t),df));			//显著性
	double pGT = 1-tcdf(t,df);
	double pLT = tcdf(t,df);

	Sxy=Sxy/(W-1);
	double lower=Dmean-tinv((m_dConfidence+1)/2,df)*Sdmean;	//下限
	double upper=Dmean+tinv((m_dConfidence+1)/2,df)*Sdmean;	//上限


	//相关性表
	double r=0;									//相关系数
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
	double P2=0;								//显著性
	if(fabs(Sx*Sy)>MIN && fabs(r*r-1)>MIN)		//r存在且不为1
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

	////格式化输出

	//统计表
	CTString str;
	str.Format("%s%d   ","配对组",m_nPairs);
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
	

	//检验结果表
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
		//统计表
		//CTLTable * pTable = NULL;
		//CRsltElement *pETable=NULL;
		CTLTable * pTable = new CTLTable;
		pTable->CreateTable(2, 2);
		pTable->SetTitle("统计");

		pTable->InsertColumn(0,"变量名");
		pTable->InsertColumn(1,"均值");
		pTable->InsertColumn(2,"频数");
		pTable->InsertColumn(3,"标准离差");
		pTable->InsertColumn(4,"均值标准误");
		int i=0;
		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_NameAr.GetAt(i),false);
			pTable->SetItemText(i,1,m_MeanSAr.GetAt(i));
			pTable->SetItemText(i,2,m_FrequencySAr.GetAt(i));
			pTable->SetItemText(i,3,m_StdDSAr.GetAt(i));
			pTable->SetItemText(i,4,m_StdEMSAr.GetAt(i));
		}
		pETable=new CRsltElementTable("统计",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);
		
		//相关性表
		nRow=m_PairNameAr.GetSize();
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,4);
		pTable->SetTitle("相关性");

		pTable->InsertColumn(0,"变量名");
		pTable->InsertColumn(1,"频数");
		pTable->InsertColumn(2,"相关系数");
		pTable->InsertColumn(3,"P 值"); 

		for(i=0;i<nRow;i++)
		{
			pTable->InsertItem(i,m_PairNameAr.GetAt(i),false);
			pTable->SetItemText(i,1,m_FrequencyPAr.GetAt(i));
			pTable->SetItemText(i,2,m_CorrAr.GetAt(i));
			pTable->SetItemText(i,3,m_P2Ar.GetAt(i));
		}
		pETable=new CRsltElementTable("相关性",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);

		//检验结果
		pTable = new CTLTable;
		pTable->CreateTable(nRow+1,6);
		pTable->SetTitle("配对检验统计量");

		pTable->InsertColumn(0,"变量名");
		pTable->InsertColumn(1,"差的均值");
		pTable->InsertColumn(2,"差的标准离差");
		pTable->InsertColumn(3,"差的均值标准误");
		CTString str,str2;
		str.Format("(%d%%置信度)", int(m_dConfidence*100));
		str2="下限"+str;
		pTable->InsertColumn(4,str2);
		str2="上限"+str;
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

		pETable=new CRsltElementTable("配对检验统计量",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);

		pTable = new CTLTable;
		int ncol = m_bNEQ + m_bGT + m_bLT;;
		pTable->CreateTable(nRow+1,3+ncol);
		pTable->SetTitle("检验结果");
		
		pTable->InsertColumn(0,"变量名");
		pTable->InsertColumn(1,"检验的T值");
		pTable->InsertColumn(2,"自由度");

		CTString strtemp1 ;
		int jtemp = 3;
		if (m_bNEQ) 
		{
			pTable->InsertColumn(jtemp,"P值(u1≠u2)");
			jtemp++;
		}
		if (m_bGT) 
		{
			pTable->InsertColumn(jtemp,"P值(u1>u2)");
			jtemp++;
		}
		if (m_bLT) 
		{
			pTable->InsertColumn(jtemp,"P值(u1<u2)");
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
		
		pETable=new CRsltElementTable("检验结果",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);

		//**************************** 残差正态概率图 START
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
				pPPPlot1->SetTitle(szTitle + "-正态概率图");
				pPPPlot1->SetXAxilLabel("正态分位数");
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
				CRsltElementChart*pREC=new CRsltElementChart(szTemp + "-正态概率图",pPPPlot1);
				m_pResult->Add(pREC);
				delete []pDPVV;
			}
			if (m_bHistogram)
			{
				CTChartHistogram* pTChartHistogram= new CTChartHistogram;
				pTChartHistogram->SetTitle(szTitle + "-直方图");
				pTChartHistogram->SetXAxilLabel(szTitle);
				int nDV = 0;
				void *pDPSV = NULL;
				pTChartHistogram->CalcXYAxis(AveVect,&pDPSV,nDV);
				pTChartHistogram->SetHistogram(pDPSV,nDV,ValVal);
				CRsltElementChart* pREC=new CRsltElementChart(szTemp + "-直方图",pTChartHistogram);
				m_pResult->Add(pREC);
				delete []pDPSV;
            }
			delete pErrorAry ;
		}
		m_ErrorAry.RemoveAll();
		//**************************** 残差正态概率图 END  */
	}
}
