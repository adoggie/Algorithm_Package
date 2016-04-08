// TLADVN.cpp ) Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "../Public/Util.h"
#include "../arithInterface/EnvParam.h"
#include "tlAdvn.h"

extern "C" __declspec( dllexport ) 
int CallAdvn(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)//0为正常， 1为发生意外，没有得到任何result信息
{			 
	AnaWord aw;
	aw.Import(VO);
	CTString StrCall = aw.GetAt(0);//调用的算法名字

	if (StrCall == "DISCRIMINANT")//判别分析
	{
		CDiscriminant discri;
		(*ppRst) = discri.OnDiscriminant(pData, VO);
	}
	else if (StrCall == "CLUSTER")//分层聚类
	{
		CClassify clust;
		(*ppRst) = clust.OnClassify(pData, VO);
	}
	else if (StrCall == "ARIMA")//ARIMA模型
	{
		CArima arim;
		(*ppRst) = arim.OnArima(pData, VO);
	}
	else if (StrCall == "TIMEX12")//X12模型
	{
		CTimeX12 x12;
		(*ppRst) = x12.OnTimeX12(pData, VO);
	}
	else if (StrCall == "CURVEREG")//曲线回归
	{
		CCurveReg curvreg;
		(*ppRst) = curvreg.OnCurveReg(pData, VO);
	}
	else if (StrCall == "EXPSMOOTH")//指数平滑
	{
		CExponSmoothing exsmth;
		(*ppRst) = exsmth.OnExponSmoothing(pData, VO);
	}
	else if (StrCall == "PCOMPONENT")//主成分
	{
		CFactor prcp;
		(*ppRst) = prcp.OnFactor2(pData, VO);
	}
	else if (StrCall == "FACTOR")//因子分析
	{
		CFactor fact;
		(*ppRst) = fact.OnFactor(pData, VO);
	}
	else if (StrCall == "MULTIANOVA")//多元方差
	{
		CMultiAnova muan;
		(*ppRst) = muan.OnMultAnova(pData, VO);
	}
	else if (StrCall == "MARKOVCHAIN")//多元方差
	{
		CMarkovChain markov;
		(*ppRst) = markov.OnCMarkovChain(pData, VO);
	}
	else if (StrCall == "QUICKCLUSTER")//快速聚类
	{
		CQuickCluster qucl;
		(*ppRst) = qucl.OnQuickCluster(pData, VO);
	}
	else if (StrCall == "PCREG")//主成分回归
	{
		CPrinCompReg pcr;
		(*ppRst) = pcr.OnPCR(pData, VO);
	}
	else if (StrCall == "RIDGEREG")//岭回归
	{
		CRidgeAnalysize rian;
		(*ppRst) = rian.OnRidgeAnalysize(pData, VO);
	}
	else if (StrCall == "REGLINEAR")//线性回归
	{
		CRegressionLinear reli;
		(*ppRst) = reli.OnStepWise(pData, VO);
	}
	else if (StrCall == "REGLOGISTIC")//逻辑回归
	{
		CRegressionLogistic relo;
		(*ppRst) = relo.OnLogistics(pData, VO);
	}
	else if (StrCall == "REGORDINAL")//有序回归
	{
		CRegressionOrdinal reor;
		(*ppRst) = reor.OnOrdinal(pData, VO);
	}
	else if (StrCall == "REGPROBIT")//概率单位回归
	{
		CRegressionProbit repr;
		(*ppRst) = repr.OnProbit(pData, VO);
	}
	else if (StrCall == "SURVIVALCOX")//COX生存
	{
		CSurvivalCox scox;
		(*ppRst) = scox.OnSurvivalCox(pData, VO);
	}
	else if (StrCall == "SURVIVALKM")//KM生存
	{
		CSurvivalKM suKM;
		(*ppRst) = suKM.OnSurvivalKM(pData, VO);
	}
	else if (StrCall == "SURVIVALLT")//LT生存
	{
		CSurvivalLT suLT;
		(*ppRst) = suLT.OnSurvivalLT(pData, VO);
	}
	else if (StrCall == "SEASONAL")//季节模型
	{
		CTimeSeansonal tise;
		(*ppRst) = tise.OnTimeSeasonal(pData, VO);
	}
	else if (StrCall == "TIMESERIESAR")//AR模型
	{
		CTimeSeries tAR;
		(*ppRst) = tAR.OnTimeSeriesAR(pData, VO);
	}
	else if (StrCall == "TIMESERIESMA")//MA模型
	{
		CTimeSeries tMA;
		(*ppRst) = tMA.OnTimeSeriesMA(pData, VO);
	}
	else if (StrCall == "TIMESERIESARIMA")//ARIMA模型
	{
		CTimeSeries tAM;
		(*ppRst) = tAM.OnTimeSeriesARIMA(pData, VO);
	}
	else if (StrCall == "TIMEX11")//X11模型
	{
		CTimeX11 x11;
		(*ppRst) = x11.OnTimeX11(pData, VO);
	}
	else if (StrCall == "VARMODEL")//向量自回归
	{
		CVar varm;
		(*ppRst) = varm.OnVAR(pData, VO);
	}
	else if (StrCall == "ROOTTEST")//单位根检验
	{
		CRootTest rott;
		(*ppRst) = rott.OnRun(pData, VO);
	}
	else if (StrCall == "COINTEGRATION")//协整检验
	{
		CCointegrationTest cott;
		(*ppRst) = cott.OnRun(pData, VO);
	}
	else if (StrCall == "VECTEST")//误差修正模型
	{
		CVEC vtst;
		(*ppRst) = vtst.OnRun(pData, VO);
	}
	else if (StrCall == "SIMUEQUATION")//联立方程
	{
		CSimuequation seq;
		(*ppRst) = seq.OnSimEqu(pData, VO);
	}
	else
	{
		CResult * pResult = new CResult("在高级统计中没有找到要调用的算法");
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
		CResult * pResult = new CResult("高级统计算法调用发生错误");
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

extern "C" __declspec( dllexport ) 
int AdvnPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)//0为正常， 1为发生意外，没有得到任何result信息
{
	AnaWord aw;
	aw.Import(VO);
	CTString StrCall = aw.GetAt(0);//调用的算法名字

	if (StrCall == "REGLINEARPRED")//线性回归预测
	{
		CRegressionLinear RLPred;
		(*ppRst) = RLPred.OnRegPred(pData, VO);
	}
	else if (StrCall == "MARKOVCHPRED")//马尔可夫链预测
	{
		CMarkovChain MkChPred;
		(*ppRst) = MkChPred.OnMorkovChainPred(pData, VO);
	}
	else if (StrCall == "REGBLOGPRED")//二值逻辑回归预测
	{
		CRegressionLogistic RBLogPred;
		(*ppRst) = RBLogPred.OnRegPred(pData, VO);
	}
	else
	{
		CResult * pResult = new CResult("在高级统计预测模块中没有找到要调用的算法");
		CTString strWarning = "没有找到您调用的法模块的预测功能，请与供应商联系";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (*ppRst == NULL)
	{
		CResult * pResult = new CResult("高级统计算法调用发生错误");
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