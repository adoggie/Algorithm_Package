// TChartArea.cpp: implementation of the CTChartArea class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartArea.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/
#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartArea::CTChartArea()
{
	SetChartType(CTChart::Area);
}

CTChartArea::~CTChartArea()
{

}

CTChartArea::CTChartArea(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel)
{
	m_strTitle = strTitle;
	m_strXAxisLabel = strXAxisLabel;
	m_strYAxisLabel = strYAxisLabel;
	SetData(pDPFB,nCount,EArea,DataType);
}

CTChartArea::CTChartArea(void* pDPFB,int nCount,EDataType DataType,CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel)
{
	m_strTitle = strTitle;
	m_strXAxisLabel = strXAxisLabel;
	m_strYAxisLabel = strYAxisLabel;
	SetData(pDPFB,nCount,EArea,DataType);
}

void CTChartArea::SetArea(void* pDPFB,int nCount,EDataType DataType,CTString strLengend)
{
	SetData(pDPFB,nCount,EArea,DataType,strLengend);
}