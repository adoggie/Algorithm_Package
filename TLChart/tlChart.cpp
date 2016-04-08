// tlChart.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include <assert.h>
#include "Line.h"
#include "Bar.h"
#include "Pie.h"
#include "Box.h"
#include "Area.h"
#include "Scatter.h"
#include "Histogram.h"
#include "SelfCorrelation.h"
#include "CrossCorrelation.h"
#include "Emp.h"

extern "C" __declspec( dllexport ) 

int DrawChart(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{	
	AnaWord aw;
	aw.Import(VO);
	CTString szChartType = aw.GetAt(0);//���õ��㷨����
	
	if (szChartType == CHART_LINE)          //ֱ��ͼ
	{
		CLine line;
		(*ppRst) = line.OnChart(pData, VO);  
	}
	else if (szChartType == CHART_BAR)      //��״ͼ
	{
		CBar Bar;
		(*ppRst) = Bar.OnChart(pData, VO);
	}
	else if (szChartType == CHART_PIE)      //��ͼ
	{
		CPie Pie;
		(*ppRst) = Pie.OnChart(pData, VO);
	}
	else if (szChartType == CHART_AREA)     //���ͼ
	{
		CArea Area;
		(*ppRst) = Area.OnChart(pData, VO);
	}
	else if (szChartType == CHART_BOXPLOT)  //��״ͼ
	{
		CBox Box;
		(*ppRst) = Box.OnChart(pData, VO);
	}
	else if (szChartType == CHART_HISTOGRAM)//ֱ��ͼ
	{
		CHistogram Histogram;
		(*ppRst) = Histogram.OnChart(pData, VO);
	}
	else if (szChartType == CHART_SCATTER)  //ɢ��ͼ
	{
		CScatter Scatter;
		(*ppRst) = Scatter.OnChart(pData, VO);
	}
	else if (szChartType == CHART_SELFCORRELATIONS) //�����
	{
		CSelfCorrelation SelfCorr;
		(*ppRst) = SelfCorr.OnChart(pData,VO);
	}
	else if (szChartType == CHART_CROSSCORRELATIONS) //�����
	{
		CCrossCorrelation CrossCorr;
		(*ppRst) = CrossCorr.OnChart(pData,VO);
	}
	else if (szChartType == CHART_EMP)
	{
		CEmp Emp;
		(*ppRst) = Emp.OnChart(pData,VO);
	}
	else
	{
		CResult *pResult = new CResult("ͼ�νӿ��㷨���ô���");
		CTString szWarning = "û����Ӧ��ͼ�νӿڣ����빩Ӧ����ϵ��";
		CRsltElementText *pWarningTextRslt = new CRsltElementText("����");
		pWarningTextRslt->AddString(szWarning);
		pResult->Add(pWarningTextRslt);
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (ppRst == NULL)
	{
		CResult *pResult = new CResult("ͼ�νӿ��㷨���ô���");
		CTString szWarning = "����ѡ������ݲ��ʺ���Ӧ��ͼ�νӿ��㷨�����������У�";
		CRsltElementText *pWarningTextRslt = new CRsltElementText( "����" );
		pWarningTextRslt->AddString(szWarning);
		pResult->Add(pWarningTextRslt);
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
