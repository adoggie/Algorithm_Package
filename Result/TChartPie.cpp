// TChartPie.cpp: implementation of the CTChartPie class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartPie.h"
#include  <math.h>
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTChartPie::CTChartPie()
{
	SetChartType(CTChart::Pie);
}

CTChartPie::~CTChartPie()
{

}

void CTChartPie::SetPie(void* pDPFB,int nCount,EDataType DataType,CTString strLengend)
{
	SetData(pDPFB,nCount,EPie,DataType,strLengend);
}