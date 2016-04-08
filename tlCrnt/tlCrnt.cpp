//////////////////////////////////////////////////////////////////////////
// tlcrnt.cpp : Defines the entry point for the DLL application.
//			����ͳ���㷨���÷��䣬CallCrnt����0��ʾ�ɹ���1��ʾʧ��
//          
//            ���ߣ�����ׯ   2006��3��17��  
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
	CTString StrCall = aw.GetAt(0);//���õ��㷨����

	if (StrCall == "MEANANALYSE")//��ֵ����
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
	else if (StrCall == "CROSSTABLE")//������
	{
		CCrossTab crostab;
		(*ppRst) = crostab.OnCrossTab(pData, VO);
	}
	else if (StrCall == "SINGLEANOVA")//һԪ�������
	{
		CAAnova anvo;
		(*ppRst) = anvo.OnAnova(pData, VO);
	}
	else if (StrCall == "CORRELATIONS")//��ط���������Peason,Spearman,Kendall
	{
		CCorrelation cor;
		(*ppRst) = cor.Correlation(pData, VO);
	}
	else if (StrCall == "PARTIALCORR")//ƫ���
	{
		CPartialCorre pCorr;
		(*ppRst) = pCorr.OnPartialCorre(pData, VO);
	}
	else if (StrCall == "ONESAMPLEMEANT")//��������ֵ����
	{
		COneSmplTTest mett;
		(*ppRst) = mett.OnOneSmplTTest(pData, VO);
	}
	else if (StrCall == "ONESAMPLEPROP")//��������������
	{
		COneSmplTTest prt;
		(*ppRst) = prt.OnOneSmplPropotionTest(pData, VO);
	}
	else if (StrCall == "ONESAMPLEVAR")//�������������
	{
		COneSmplTTest vart;
		(*ppRst) = vart.OnOneSmplVarianceTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEMEAN")//˫������ֵ����
	{
		CTTest ttes1;
		(*ppRst) = ttes1.OnTTest(pData, VO);
	}
	else if (StrCall == "PAIRTTEST")//�����������
	{
		CPairTTest pTT;
		(*ppRst) = pTT.OnPairTTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEPROP")//˫������������
	{
		CTTest ttes2;
		(*ppRst) = ttes2.OnDoublePropotionTest(pData, VO);
	}
	else if (StrCall == "TWOSAMPLEVAR")//˫�����������
	{
		CTTest ttes3;
		(*ppRst) = ttes3.OnDoubleVarTest(pData, VO);
	}
	else if (StrCall == "CHISQUARETEST")//��������
	{
		CChiSquare ch2;
		(*ppRst) = ch2.OnChiSquare(pData, VO);
	}
	else if (StrCall == "BINTEST")//����ʽ����
	{
		CBinomialTest btest;
		(*ppRst) = btest.OnBinomialTest(pData, VO);
	}
	else if (StrCall == "RUNTEST")//�α����
	{
		CRunTest runt;
		(*ppRst) = runt.OnRunTest(pData, VO);
	}
	else if (StrCall == "KSTEST")//KS����
	{
		CCKTest kst;
		(*ppRst) = kst.OnCKTest(pData, VO);
	}
	else if (StrCall == "TWODEPTEST")//����������������
	{
		CNonParamTest np1;
		(*ppRst) = np1.OnNonParamTest(pData, VO, 1);
	}
	else if (StrCall == "MULTIDEPTEST")//���������������
	{
		CNonParamTest np2;
		(*ppRst) = np2.OnNonParamTest(pData, VO, 2);
	}
	else if (StrCall == "TWOCORRTEST")//���������������
	{
		CNonParamTest np3;
		(*ppRst) = np3.OnNonParamTest(pData, VO, 3);
	}
	else if (StrCall == "MULTICORRTEST")//��������������
	{
		CNonParamTest np4;
		(*ppRst) = np4.OnNonParamTest(pData, VO, 4);
	}
	else
	{
		CResult * pResult = new CResult("�ڳ���ͳ����û���ҵ�Ҫ���õ��㷨");
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
		CResult * pResult = new CResult("����ͳ���㷨���÷�������");
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
