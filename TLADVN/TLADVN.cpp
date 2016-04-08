// TLADVN.cpp ) Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "../Public/Util.h"
#include "../arithInterface/EnvParam.h"
#include "tlAdvn.h"

extern "C" __declspec( dllexport ) 
int CallAdvn(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)//0Ϊ������ 1Ϊ�������⣬û�еõ��κ�result��Ϣ
{			 
	AnaWord aw;
	aw.Import(VO);
	CTString StrCall = aw.GetAt(0);//���õ��㷨����

	if (StrCall == "DISCRIMINANT")//�б����
	{
		CDiscriminant discri;
		(*ppRst) = discri.OnDiscriminant(pData, VO);
	}
	else if (StrCall == "CLUSTER")//�ֲ����
	{
		CClassify clust;
		(*ppRst) = clust.OnClassify(pData, VO);
	}
	else if (StrCall == "ARIMA")//ARIMAģ��
	{
		CArima arim;
		(*ppRst) = arim.OnArima(pData, VO);
	}
	else if (StrCall == "TIMEX12")//X12ģ��
	{
		CTimeX12 x12;
		(*ppRst) = x12.OnTimeX12(pData, VO);
	}
	else if (StrCall == "CURVEREG")//���߻ع�
	{
		CCurveReg curvreg;
		(*ppRst) = curvreg.OnCurveReg(pData, VO);
	}
	else if (StrCall == "EXPSMOOTH")//ָ��ƽ��
	{
		CExponSmoothing exsmth;
		(*ppRst) = exsmth.OnExponSmoothing(pData, VO);
	}
	else if (StrCall == "PCOMPONENT")//���ɷ�
	{
		CFactor prcp;
		(*ppRst) = prcp.OnFactor2(pData, VO);
	}
	else if (StrCall == "FACTOR")//���ӷ���
	{
		CFactor fact;
		(*ppRst) = fact.OnFactor(pData, VO);
	}
	else if (StrCall == "MULTIANOVA")//��Ԫ����
	{
		CMultiAnova muan;
		(*ppRst) = muan.OnMultAnova(pData, VO);
	}
	else if (StrCall == "MARKOVCHAIN")//��Ԫ����
	{
		CMarkovChain markov;
		(*ppRst) = markov.OnCMarkovChain(pData, VO);
	}
	else if (StrCall == "QUICKCLUSTER")//���پ���
	{
		CQuickCluster qucl;
		(*ppRst) = qucl.OnQuickCluster(pData, VO);
	}
	else if (StrCall == "PCREG")//���ɷֻع�
	{
		CPrinCompReg pcr;
		(*ppRst) = pcr.OnPCR(pData, VO);
	}
	else if (StrCall == "RIDGEREG")//��ع�
	{
		CRidgeAnalysize rian;
		(*ppRst) = rian.OnRidgeAnalysize(pData, VO);
	}
	else if (StrCall == "REGLINEAR")//���Իع�
	{
		CRegressionLinear reli;
		(*ppRst) = reli.OnStepWise(pData, VO);
	}
	else if (StrCall == "REGLOGISTIC")//�߼��ع�
	{
		CRegressionLogistic relo;
		(*ppRst) = relo.OnLogistics(pData, VO);
	}
	else if (StrCall == "REGORDINAL")//����ع�
	{
		CRegressionOrdinal reor;
		(*ppRst) = reor.OnOrdinal(pData, VO);
	}
	else if (StrCall == "REGPROBIT")//���ʵ�λ�ع�
	{
		CRegressionProbit repr;
		(*ppRst) = repr.OnProbit(pData, VO);
	}
	else if (StrCall == "SURVIVALCOX")//COX����
	{
		CSurvivalCox scox;
		(*ppRst) = scox.OnSurvivalCox(pData, VO);
	}
	else if (StrCall == "SURVIVALKM")//KM����
	{
		CSurvivalKM suKM;
		(*ppRst) = suKM.OnSurvivalKM(pData, VO);
	}
	else if (StrCall == "SURVIVALLT")//LT����
	{
		CSurvivalLT suLT;
		(*ppRst) = suLT.OnSurvivalLT(pData, VO);
	}
	else if (StrCall == "SEASONAL")//����ģ��
	{
		CTimeSeansonal tise;
		(*ppRst) = tise.OnTimeSeasonal(pData, VO);
	}
	else if (StrCall == "TIMESERIESAR")//ARģ��
	{
		CTimeSeries tAR;
		(*ppRst) = tAR.OnTimeSeriesAR(pData, VO);
	}
	else if (StrCall == "TIMESERIESMA")//MAģ��
	{
		CTimeSeries tMA;
		(*ppRst) = tMA.OnTimeSeriesMA(pData, VO);
	}
	else if (StrCall == "TIMESERIESARIMA")//ARIMAģ��
	{
		CTimeSeries tAM;
		(*ppRst) = tAM.OnTimeSeriesARIMA(pData, VO);
	}
	else if (StrCall == "TIMEX11")//X11ģ��
	{
		CTimeX11 x11;
		(*ppRst) = x11.OnTimeX11(pData, VO);
	}
	else if (StrCall == "VARMODEL")//�����Իع�
	{
		CVar varm;
		(*ppRst) = varm.OnVAR(pData, VO);
	}
	else if (StrCall == "ROOTTEST")//��λ������
	{
		CRootTest rott;
		(*ppRst) = rott.OnRun(pData, VO);
	}
	else if (StrCall == "COINTEGRATION")//Э������
	{
		CCointegrationTest cott;
		(*ppRst) = cott.OnRun(pData, VO);
	}
	else if (StrCall == "VECTEST")//�������ģ��
	{
		CVEC vtst;
		(*ppRst) = vtst.OnRun(pData, VO);
	}
	else if (StrCall == "SIMUEQUATION")//��������
	{
		CSimuequation seq;
		(*ppRst) = seq.OnSimEqu(pData, VO);
	}
	else
	{
		CResult * pResult = new CResult("�ڸ߼�ͳ����û���ҵ�Ҫ���õ��㷨");
		CTString strWarning = "û���ҵ������õ��㷨ģ�飬���빩Ӧ����ϵ";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (*ppRst == NULL)
	{
		CResult * pResult = new CResult("�߼�ͳ���㷨���÷�������");
		CTString strWarning = "����ѡ������ݲ��ʺ����ǵ��㷨�����������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
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
			 string VO, string &ppOutStruct,CResult **ppRst)//0Ϊ������ 1Ϊ�������⣬û�еõ��κ�result��Ϣ
{
	AnaWord aw;
	aw.Import(VO);
	CTString StrCall = aw.GetAt(0);//���õ��㷨����

	if (StrCall == "REGLINEARPRED")//���Իع�Ԥ��
	{
		CRegressionLinear RLPred;
		(*ppRst) = RLPred.OnRegPred(pData, VO);
	}
	else if (StrCall == "MARKOVCHPRED")//����ɷ���Ԥ��
	{
		CMarkovChain MkChPred;
		(*ppRst) = MkChPred.OnMorkovChainPred(pData, VO);
	}
	else if (StrCall == "REGBLOGPRED")//��ֵ�߼��ع�Ԥ��
	{
		CRegressionLogistic RBLogPred;
		(*ppRst) = RBLogPred.OnRegPred(pData, VO);
	}
	else
	{
		CResult * pResult = new CResult("�ڸ߼�ͳ��Ԥ��ģ����û���ҵ�Ҫ���õ��㷨");
		CTString strWarning = "û���ҵ������õķ�ģ���Ԥ�⹦�ܣ����빩Ӧ����ϵ";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (*ppRst == NULL)
	{
		CResult * pResult = new CResult("�߼�ͳ���㷨���÷�������");
		CTString strWarning = "����ѡ������ݲ��ʺ����ǵ��㷨�����������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
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