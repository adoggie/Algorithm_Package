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
	CTString szChartType = aw.GetAt(0);//调用的算法名字
	
	if (szChartType == CHART_LINE)          //直线图
	{
		CLine line;
		(*ppRst) = line.OnChart(pData, VO);  
	}
	else if (szChartType == CHART_BAR)      //条状图
	{
		CBar Bar;
		(*ppRst) = Bar.OnChart(pData, VO);
	}
	else if (szChartType == CHART_PIE)      //饼图
	{
		CPie Pie;
		(*ppRst) = Pie.OnChart(pData, VO);
	}
	else if (szChartType == CHART_AREA)     //面积图
	{
		CArea Area;
		(*ppRst) = Area.OnChart(pData, VO);
	}
	else if (szChartType == CHART_BOXPLOT)  //盒状图
	{
		CBox Box;
		(*ppRst) = Box.OnChart(pData, VO);
	}
	else if (szChartType == CHART_HISTOGRAM)//直方图
	{
		CHistogram Histogram;
		(*ppRst) = Histogram.OnChart(pData, VO);
	}
	else if (szChartType == CHART_SCATTER)  //散点图
	{
		CScatter Scatter;
		(*ppRst) = Scatter.OnChart(pData, VO);
	}
	else if (szChartType == CHART_SELFCORRELATIONS) //自相关
	{
		CSelfCorrelation SelfCorr;
		(*ppRst) = SelfCorr.OnChart(pData,VO);
	}
	else if (szChartType == CHART_CROSSCORRELATIONS) //互相关
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
		CResult *pResult = new CResult("图形接口算法调用错误");
		CTString szWarning = "没有相应的图形接口，请与供应商联系！";
		CRsltElementText *pWarningTextRslt = new CRsltElementText("错误！");
		pWarningTextRslt->AddString(szWarning);
		pResult->Add(pWarningTextRslt);
		(*ppRst) = pResult; 
		(*ppRst)->Print();
		return 1;
	}

	if (ppRst == NULL)
	{
		CResult *pResult = new CResult("图形接口算法调用错误");
		CTString szWarning = "您所选择的数据不适合相应的图形接口算法，请重新运行！";
		CRsltElementText *pWarningTextRslt = new CRsltElementText( "错误！" );
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
