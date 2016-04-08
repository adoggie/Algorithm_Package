//////////////////////////////////////////////////////////////////////////
// tlcrnt.cpp : Defines the entry point for the DLL application.
//			初级统计算法调用分配，CallCrnt返回0表示成功，1表示失败
//          
//            作者：马良庄   2006年3月17日  
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlcrnt.h"
#include "../Public/Util.h"
#include "../arithInterface/EnvParam.h"

extern "C" __declspec( dllexport ) 
int CallCrnt(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{	
	AnaWord aw;
	aw.Import(VO);
	CTString StrCall = aw.GetAt(0);//调用的算法名字

	if (StrCall == "MEANANALYSE")//均值分析
	{
		CDescriptive des;
		(*ppRst) = des.OnDescriptive(pData, VO);
	}
	else if (StrCall == "FREQUENCY")//
	{
		CFrequency fre;
		(*ppRst) = fre.OnFrequency(pData, VO);
	}
	else if (StrCall == "DESCRIPTIVE")//
	{
		CExplore explo;
		(*ppRst) = explo.OnExplore(pData, VO);
	}
	else if (StrCall == "CROSSTABLE")//交出表
	{
		CCrossTab crostab;
		(*ppRst) = crostab.OnCrossTab(pData, VO);
	}
	else if (StrCall == "SINGLEANOVA")//一元方差分析
	{
		CAAnova anvo;
		(*ppRst) = anvo.OnAnova(pData, VO);
	}
	else if (StrCall == "CORRELATIONS")//相关分析，包括Peason,Spearman,Kendall
	{
		CCorrelation cor;
		(*ppRst) = cor.Correlation(pData, VO);
	}
	else if (StrCall == "PARTIALCORR")//偏相关
	{
		CPartialCorre pCorr;
		(*ppRst) = pCorr.OnPartialCorre(pData, VO);
	}
	else if (StrCall == "ONESAMPLEMEANT")//单样本均值检验
	{
		COneSmplTTest mett;
		(*ppRst) = mett.OnOneSmplTTest(pData, VO);
	}
	else if (StrCall == "ONESAMPLEPROP")//单样本比例检验
	{
		COneSmplTTest prt;
		(*ppRst) = prt.OnOneSmplPropotionTest(pData, VO);
	}
	else if (StrCall == "ONESAMPLEVAR")//单样本方差检验
	{
		COneSmplTTest vart;
		(*ppRst) = vart.OnOneSmplVarianceTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEMEAN")//双样本均值检验
	{
		CTTest ttes1;
		(*ppRst) = ttes1.OnTTest(pData, VO);
	}
	else if (StrCall == "PAIRTTEST")//配对样本检验
	{
		CPairTTest pTT;
		(*ppRst) = pTT.OnPairTTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEPROP")//双样本比例检验
	{
		CTTest ttes2;
		(*ppRst) = ttes2.OnDoublePropotionTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEVAR")//双样本方差检验
	{
		CTTest ttes3;
		(*ppRst) = ttes3.OnDoubleVarTest(pData, VO);
	}
	else if (StrCall == "CHISQUARETEST")//卡方检验
	{
		CChiSquare ch2;
		(*ppRst) = ch2.OnChiSquare(pData, VO);
	}
	else if (StrCall == "BINTEST")//二项式检验
	{
		CBinomialTest btest;
		(*ppRst) = btest.OnBinomialTest(pData, VO);
	}
	else if (StrCall == "RUNTEST")//游标检验
	{
		CRunTest runt;
		(*ppRst) = runt.OnRunTest(pData, VO);
	}
	else if (StrCall == "KSTEST")//KS检验
	{
		CCKTest kst;
		(*ppRst) = kst.OnCKTest(pData, VO);
	}
	else if (StrCall == "TWODEPTEST")//两个独立样本检验
	{
		CNonParamTest np1;
		(*ppRst) = np1.OnNonParamTest(pData, VO, 1);
	}
	else if (StrCall == "MULTIDEPTEST")//多个独立样本检验
	{
		CNonParamTest np2;
		(*ppRst) = np2.OnNonParamTest(pData, VO, 2);
	}
	else if (StrCall == "TWOCORRTEST")//两个相关样本检验
	{
		CNonParamTest np3;
		(*ppRst) = np3.OnNonParamTest(pData, VO, 3);
	}
	else if (StrCall == "MULTICORRTEST")//多个相关样本检验
	{
		CNonParamTest np4;
		(*ppRst) = np4.OnNonParamTest(pData, VO, 4);
	}
	else
	{
		CResult * pResult = new CResult("在初级统计中没有找到要调用的算法");
		CTString strWarning = "没有找到您调用的算法模块，请与供应商联系";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (*ppRst == NULL)
	{
		CResult * pResult = new CResult("初级统计算法调用发生错误");
		CTString strWarning = "您所选择的数据不适合我们的算法，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}
	else
	{
		(*ppRst)->Print();
		return 0;
	}
}
