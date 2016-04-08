// TChartLine.h: interface for the CTChartLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTLINE_H__A66DA0D5_9A3B_42DB_8C43_49C7341252FD__INCLUDED_)
#define AFX_TCHARTLINE_H__A66DA0D5_9A3B_42DB_8C43_49C7341252FD__INCLUDED_
#pragma  warning(disable:4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"
#include "../Public/TCLFile.h"

#include "TChart1.h"
#include "TGraphData.h"

//////////////////////////////////////////////////////////////////////
//Class name:   CTChartLine
//Author:			蒋国清
//Function:		  线型图表类（代表整个输出结果）
//Describe:		  从CTChart类继承
//Date-Time:	2002-8-15	9:08	
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CTChartLine : public CTChart  
{
public:
	CTChartLine();
	virtual ~CTChartLine();
	CTChartLine(void* pDPFB,int nCount, EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");

	void SetLine(void *pValue,int nCount, EDataType DataType, CTString strLengend="");
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTLINE_H__A66DA0D5_9A3B_42DB_8C43_49C7341252FD__INCLUDED_)
