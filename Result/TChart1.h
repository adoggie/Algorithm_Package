#if !defined(AFX_TCHART1_H__0E136B49_15DE_4740_9DF8_0C54C2206F61__INCLUDED_)
#define AFX_TCHART1_H__0E136B49_15DE_4740_9DF8_0C54C2206F61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma  warning(disable:4251)

#define MIN_ZOOM_PIXELS  15

#define UWM_YAXISCLICK	 WM_USER + 1
#define UWM_XAXISCLICK	 WM_USER + 2
#define UWM_CURVECLICK	 WM_USER + 3
#define UWM_YAXISDBLCLICK WM_USER + 4
#define UWM_XAXISDBLCLICK WM_USER + 5
#define UWM_CURVEDBLCLICK WM_USER + 6
#define UWM_GRAPHDBLCLICK WM_USER + 7
#define UWM_RBUTTONUP     WM_USER + 8
#define UWM_CURSORMOVED   WM_USER + 9

#define IDM_SELECT								 40000
#define IDM_INSERTLABEL                   40001
#define IDM_ZOOM								  40002
#define IDM_RESET								  40003
#define IDM_MOVE								  40004
#define IDM_PROPERTIES                    40005
#define IDM_PRINT                                40007
#define IDM_PANEL				         		 40008
#define IDM_TOP							           40009
#define IDM_PREV						          40010
#define IDM_NEXT				                  40011
#define IDM_BOTTOM					          40012
#define IDM_DELETE					            40013		
#define IDM_UPDATE					           40014
#define IDM_OPENCHART						         40015
#define IDM_SAVECHART						          40016
#define IDM_DATA_MANAGER	        40017
#define IDM_OBJECT_MANAGER        40018
#define IDM_CLEAR			          			40019
#define IDM_ABOUTCHART								40020
#define IDM_ORIGINALITY_SIZE	  40021

#define	SCATTERCOLOR_COUNT	70
#define COLOR_COUNT 14
#include "../Public/TString.h"
#include "../Public/TCLFile.h"
#include "TGraphData.h"

typedef struct tagITEMSORTGROUPDATA
{
	int nCount;//本组个数
	CTArray<int,int> arrIndex;//项目在原数据中的索引
}TITEMSORTGROUPDATA;
#define TCHART_WND_WIDTH 560
#define TCHART_WND_HEIGHT 400

typedef struct tagGLPoint{
	tagGLPoint()
	{
		x = 0;
		y = 0;
		z = 0;
	};
	double x;
	double y;
	double z;
}GLPOINT;          //WM 2002/01/21 ADD 3D

typedef struct tagGroupData{
	tagGroupData()
	{
		pGLPointSet = NULL;
		size = 0;
	};
	~tagGroupData()
	{
		if (pGLPointSet)
		{
			delete[] pGLPointSet;
			pGLPointSet = NULL;
			size = 0;
		}
	};

	GLPOINT *pGLPointSet;
	int size;
}GROUPDATA;//WM 2002/02/24 ADD 3D

enum EGraphType
{
	ELine = 0,
	EPie,
	EScatter,
	EBoxPlot,
	EHistogram,
	EBar,
	EArea,
	EPedigree,
	EDecTree
};

typedef struct tagTGraph
{
	EGraphType  eGraphType;
	CTString	strLengend;
	CTGraphData grphData;
} TGraph;

class CPropSheetTable;

class AFX_EXT_CLASS CTChart
{
public:
	enum EChartKind
	{
		Line,
		Bar,
		Pie,
		Area,
		Boxplot,
		Histogram,
		Pareno,
		ErrorBar,
		Sequence,
		Scatter,
		Autocorrelations,
		Crosscorrelations,
		Control,
		ROC,
		Clustering,
		HighLow,	
		PP,
		Curve,
		Pedigree,
		DecTree
	};

	CTString	    m_strTitle;//图表标题
	CTString		m_strXAxisLabel;
	CTString		m_strYAxisLabel;
	EChartKind	    m_ChartKind;//图表类型(Line\Bar\Pie)
	CTStringArray	m_arChartInfo;
	CTArray<TGraph*,TGraph*>  m_arData;   //数据（画图用）
// Construction
public:
	CTChart();
public:
	void SetTitle(CTString strTitle=""){m_strTitle = strTitle;}
	CTString GetTitle(){return m_strTitle;}
	void SetChartType(EChartKind ChartKind){m_ChartKind = ChartKind;}
	EChartKind GetChartType(){return m_ChartKind;}
	void SetXAxilLabel(CTString strXLabel){m_strXAxisLabel = strXLabel;}
	void SetYAxilLabel(CTString strYLabel){m_strYAxisLabel = strYLabel;}
	CTString GetXAxilLabel(){return m_strXAxisLabel;}
	CTString GetYAxilLabel(){return m_strYAxisLabel;}
	
	void SetData(void *pValues, int nLen, EGraphType eGraphType,EDataType eDataType,CTString strLengend="");
	void OnClear();
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	virtual void	Print();
	virtual ~CTChart();
};

/////////////////////////////////////////////////////////////////////////////
#pragma  warning(default : 4251)
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCHART1_H__0E136B49_15DE_4740_9DF8_0C54C2206F61__INCLUDED_)































































