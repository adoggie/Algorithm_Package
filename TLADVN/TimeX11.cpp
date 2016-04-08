// TimeX11.cpp: implementation of the CTimeX11 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tladvn.h"
#include "TimeX11.h"
#include "tlAdvn.h"
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

CTimeX11::CTimeX11()
{
	m_nMove=0;
	m_nMethod=0;
	m_nYear=2000;
	m_pDataInterface=NULL;
	m_pResult=NULL;
    m_pRsltVariable=NULL;
}

CTimeX11::~CTimeX11()
{
	m_tVarInfo.DeleteObject();
}

BOOL CTimeX11::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
//	TIMEX11
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/METHOD=[{MULTIP**},{ADD}] [{YEARAVE**},{TWICEAVE}]
//	/YEAR [BEGIN({2000**},{1**})]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;

	m_nMethod = int(AnaSyntax::findstr("/METHOD", "ADD", VOstr));
	m_nMove = int(AnaSyntax::findstr("/METHOD", "TWICEAVE", VOstr));
	m_nYear = AnaSyntax::getvalue("/YEAR", "BEGIN", VOstr);
	m_nMonth = AnaSyntax::getvalue("/YEAR", "BEGIN", VOstr, 2);
	if (m_nMonth == -1)
		m_nMonth = 1;

	return TRUE;
}

CResult * CTimeX11::OnTimeX11(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pRsltVariable=NULL;
	m_pRsltElementText=NULL;
	m_pResult = new CResult("时间序列-X11模型分析");
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}

	CDWordArray arCol;//读取数据
	int i=0;
	for (i=0; i<m_tVarInfo.iCount; i++)
		arCol.Add(m_tVarInfo.pValue[i]);
	CDoubleMatrix Data_read;
	m_nRow = m_pDataInterface->GetColsData(arCol, Data_read, 6);//Modify by xds 20060815
	if (m_nRow != m_pDataInterface->GetRowCount() || m_nRow <36)
	{
		CTString strWarning = "数据存在缺失值，或是少于36个，不适合X11模型！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	WeekCalcu(m_nYear, m_nMonth);//计算序列每个月有多少天，有多少个周一周二……，放入m_DaysinMonth_vec和m_WeekDescrib_mx
	for (i=0; i<7; i++)
	{
		CDoubleVector vecTemp = m_WeekDescrib_mx(i);
		vecTemp = vecTemp/m_DaysinMonth_vec;
		m_WeekDescrib_mx(i) = vecTemp;
	}

	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		CDoubleVector Org_vec = Data_read(i);//原始数据
		m_Org_vec = Org_vec.copy();
		Org_vec = Org_vec / m_DaysinMonth_vec;//对应到每一天，而不是每个月。
		if (m_nMethod == 0)//把乘法模型变换为加法
		{
			for (int j=0; j<m_nRow; j++)
			{
				if (Org_vec(j) > EPS_ACCUR)
				{
					Org_vec(j) = log(Org_vec(j) );
				}
				else
				{
					CTString strWarning = "数据存在负数，无法使用乘法模型！";
					CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
					pWarningTextRslt->AddString( strWarning );
					m_pResult->Add( pWarningTextRslt );
					return m_pResult;
				}
			}
		}
		BOOL bTemp = Non11(Org_vec, m_nMove, m_nMethod);//主要的计算函数
		m_Justed_vec = m_CT_SmoothedVec + m_S_SmoothedVec + m_Dr_FinalVec + m_I_FinalVec;
		m_pRsltVariable =new CRsltVariable(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		if (m_nMethod == 0)//把乘法模型变换为加法后，对其进行还原
		{
			for (int j=0; j<m_nRow; j++)
			{
				m_CT_SmoothedVec(j) = exp(m_CT_SmoothedVec(j) );
				m_S_SmoothedVec(j) = exp(m_S_SmoothedVec(j) );
				m_Dr_FinalVec(j) = exp(m_Dr_FinalVec(j) );
				m_I_FinalVec(j) = exp(m_I_FinalVec(j) );
				m_Justed_vec(j) = exp(m_Justed_vec(j) );
			}
		}
		else
		{
			m_S_SmoothedVec = m_S_SmoothedVec * m_DaysinMonth_vec;
			m_Dr_FinalVec = m_Dr_FinalVec * m_DaysinMonth_vec;
			m_I_FinalVec = m_I_FinalVec * m_DaysinMonth_vec;
		}
		m_CT_SmoothedVec = m_CT_SmoothedVec * m_DaysinMonth_vec;
		m_Justed_vec = m_Justed_vec * m_DaysinMonth_vec;

		DrawResult(bTemp, i);
		m_pResult->Add(m_pRsltVariable);
	}
	return m_pResult;
} 

BOOL CTimeX11::Non11(CDoubleVector& Org_vec, int nMove, int Method)
{
	int i = 0;
	int nCircle = 0;
	CDoubleVector SmoothedCT = Org_vec.copy();
	CDoubleVector SmoothedS(m_nRow);
	CDoubleVector NotAdjI(m_nRow, 0.0);
	for (nCircle=0; nCircle<3; nCircle++)//做三次平滑修正
	{
		SmoothedCT = Mov_ave(SmoothedCT, m_nMove);
		SmoothedS = Org_vec - SmoothedCT;
		CDoubleVector Org_S = SmoothedS.copy();
		for (int monthly=0; monthly<12; monthly++)//1到12 月的季节
		{
			int nOfMonth = int((m_nRow - monthly - 1)/12) + 1;//第i个月出现了多少次
			CDoubleVector seasonal_vecTemp(nOfMonth);
			int seasonal=0;
			for (seasonal=0; seasonal<nOfMonth; seasonal++)
				seasonal_vecTemp(seasonal) = Org_S(seasonal*12+monthly);
			seasonal_vecTemp = Mov_ave(seasonal_vecTemp, 2);
			for (seasonal=0; seasonal<nOfMonth; seasonal++)
				Org_S(seasonal*12+monthly) = seasonal_vecTemp(seasonal);
		}
		SmoothedS = Mov_ave(Org_S, 1);
		NotAdjI = Org_vec - SmoothedCT - SmoothedS;
		if (nCircle != 2)
			AdjustI(NotAdjI);
	}
	CDoubleVector VecBeta;
	VecBeta = inv(m_WeekDescrib_mx.dot(m_WeekDescrib_mx)) * (m_WeekDescrib_mx.trans()*NotAdjI);
	CDoubleVector VecDr = m_WeekDescrib_mx * VecBeta;

	m_CT_SmoothedVec = SmoothedCT;
	m_S_SmoothedVec = SmoothedS;
	m_Dr_FinalVec = VecDr;
	m_I_FinalVec = NotAdjI - VecDr;
	return TRUE;
}

void CTimeX11::AdjustI(CDoubleVector& Ivec)
{
	return;
}

CDoubleVector CTimeX11::Mov_ave(CDoubleVector& SmoothVec, int nMove)
{
	CDoubleVector retVec(m_nRow, 0.0);
	switch(nMove) 
	{
	case 0:
		{
			int i = 0;
			for (i=0; i<m_nRow; i++)
			{
				double dTemp1 = (i-6>-1)?SmoothVec(i-6):SmoothVec(i-6+12);
				dTemp1 += (i-5>-1)?SmoothVec(i-5)*2:SmoothVec(i-5+12)*2;
				dTemp1 += (i-4>-1)?SmoothVec(i-4)*2:SmoothVec(i-4+12)*2;
				dTemp1 += (i-3>-1)?SmoothVec(i-3)*2:SmoothVec(i-3+12)*2;
				dTemp1 += (i-2>-1)?SmoothVec(i-2)*2:SmoothVec(i-2+12)*2;
				dTemp1 += (i-1>-1)?SmoothVec(i-1)*2:SmoothVec(i-1+12)*2;
				double dTemp2 = SmoothVec(i)*2;
				double dTemp3 = (i+6<m_nRow)?SmoothVec(i+6):SmoothVec(i+6-12);
				dTemp3 += (i+5<m_nRow)?SmoothVec(i+5)*2:SmoothVec(i+5-12)*2;
				dTemp3 += (i+4<m_nRow)?SmoothVec(i+4)*2:SmoothVec(i+4-12)*2;
				dTemp3 += (i+3<m_nRow)?SmoothVec(i+3)*2:SmoothVec(i+3-12)*2;
				dTemp3 += (i+2<m_nRow)?SmoothVec(i+2)*2:SmoothVec(i+2-12)*2;
				dTemp3 += (i+1<m_nRow)?SmoothVec(i+1)*2:SmoothVec(i+1-12)*2;
				double dTotalTemp = dTemp1 + dTemp2 + dTemp3;
				dTotalTemp = dTotalTemp / 24;
//					(
//					(i-6>-1) + (i-5>-1)*2 + (i-4>-1)*2 + (i-3>-1)*2 + (i-2>-1)*2 + (i-1>-1)*2 
//					+ 2.0 
//					+ (i+6<m_nRow) + (i+5<m_nRow)*2 + (i+4<m_nRow)*2 + (i+3<m_nRow)*2 + (i+2<m_nRow)*2 + (i+1<m_nRow)*2
//					);
				retVec(i) = dTotalTemp;
			}
		}
		break;
	case 1:
		{
			int i = 0;
			for (i=0; i<m_nRow; i++)
			{
				double dTemp1 = (i>1)?SmoothVec(i-2):SmoothVec(i-2+12);
				double dTemp2 = (i>0)?SmoothVec(i-1)*2:SmoothVec(i-1+12)*2;
				double dTemp3 = SmoothVec(i)*3;
				double dTemp4 = (i+1<m_nRow)?SmoothVec(i+1)*2:SmoothVec(i+1-12)*2;
				double dTemp5 = (i+2<m_nRow)?SmoothVec(i+2):SmoothVec(i+2-12);
				double dTotalTemp = dTemp1 + dTemp2 + dTemp3 + dTemp4 + dTemp5;
				dTotalTemp = dTotalTemp / 9;
					//((i>1) + (i>0)*2 + 3.0 + (i<m_nRow-1)*2 + (i<m_nRow-2)*1);
				retVec(i) = dTotalTemp;
			}
		}
		break;
	case 2:
		{
			int i = 0;
			int nRow = SmoothVec.vlen();
			for (i=0; i<nRow; i++)
			{
				double dTemp1 = (i>1)?SmoothVec(i-2):0;
				double dTemp2 = (i>0)?SmoothVec(i-1):0;
				double dTemp3 = SmoothVec(i);
				double dTemp4 = (i+1<nRow)?SmoothVec(i+1):0;
				double dTemp5 = (i+2<nRow)?SmoothVec(i+2):0;
				double dTotalTemp = dTemp1 + dTemp2 + dTemp3 + dTemp4 + dTemp5;
				dTotalTemp = dTotalTemp / ((i>1) + (i>0) + 1.0 + (i<nRow-1) + (i<nRow-2));
				retVec(i) = dTotalTemp;
			}
		}
		break;
	default:
		break;
	}

	return retVec;
}

void CTimeX11::DrawResult(BOOL TOut, int VarInfo)
{
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle("X11模型");

	pTable->SetCols(6);
	pTable->SetRows(m_nRow+1);
	pTable->InsertColumn(0,"序号");
	pTable->InsertColumn(1,"趋势因素");
	pTable->InsertColumn(2,"不规则因素");
	pTable->InsertColumn(3,"季节因素");
	pTable->InsertColumn(4,"星期构成因素");
//	pTable->InsertColumn(5,"调整后的序列");
	CTString str;
	int i;
	for(i=0;i<m_nRow;i++)
	{
		str.Format("%d",i+1);
		pTable->InsertItem(i,str);
		pTable->SetItemText(i, 1, m_CT_SmoothedVec(i) );
		pTable->SetItemText(i, 2, m_I_FinalVec(i) );
		pTable->SetItemText(i, 3, m_S_SmoothedVec(i) );
		pTable->SetItemText(i, 4, m_Dr_FinalVec(i) );
//		pTable->SetItemText(i+1, 5, m_Justed_vec(i) );
	}
	
	pETable= new CRsltElementTable("X11",pTable);//申请一个表对象类的对象
	m_pRsltVariable->Add(pETable);

	///////////////////////////////趋势因素图形输出描述/////////////////////////////////////
	TDataPointStrVal* pDPVV=new TDataPointStrVal[m_nRow];
	CTString tmpStr;
	int StepTemp = m_nRow/13;
	for(i=0;i<m_nRow;i++)
	{	
		tmpStr.Format("%d",i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal =m_CT_SmoothedVec(i);
	}
	
	CTString strTitle = "趋势因素";
	CTChartLine* pCL=new CTChartLine(pDPVV,m_nRow,StrVal,"趋势因素","序列","趋势因素");
	CRsltElementChart* pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
	m_pRsltVariable->Add(pREC);

	////////////////////////不规则因素图形输出/////////////////////////
    pDPVV=new TDataPointStrVal[m_nRow];
	for(i=0;i<m_nRow;i++)
	{	
		tmpStr.Format("%d",i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal = m_I_FinalVec(i);
	}
	strTitle = "不规则因素";
	pCL=new CTChartLine(pDPVV,m_nRow,StrVal,"不规则因素","序列","不规则因素");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
	m_pRsltVariable->Add(pREC);

	////////////////////////季节因素图形输出/////////////////////////
    pDPVV=new TDataPointStrVal[m_nRow];
	for(i=0;i<m_nRow;i++)
	{	
		tmpStr.Format("%d",i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal = m_S_SmoothedVec(i);
	}
	strTitle = "季节因素";
	pCL=new CTChartLine(pDPVV,m_nRow,StrVal,"季节因素","序列","季节因素");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
	m_pRsltVariable->Add(pREC);

	////////////////////////星期构成因素图形输出/////////////////////////
    pDPVV=new TDataPointStrVal[m_nRow];
	for(i=0;i<m_nRow;i++)
	{	
		tmpStr.Format("%d",i+1);
		pDPVV[i].fXVal =tmpStr;
		pDPVV[i].fYVal = m_Dr_FinalVec(i);
	}
	strTitle = "星期构成因素";
	pCL=new CTChartLine(pDPVV,m_nRow,StrVal,"星期构成因素","序列","星期的影响");
	pREC=new CRsltElementChart(strTitle,pCL);
	delete[] pDPVV;
	m_pRsltVariable->Add(pREC);

	return;
}

void CTimeX11::WeekCalcu(int year, int month)
{
	//以1900年1月0日为周0为起始标准
	m_DaysinMonth_vec.create(m_nRow, 0.0);
	m_WeekDescrib_mx.create(m_nRow, 7, 4.0);
	
	int totalDaysInYear = (year - 1900) * 365 + (year - 1901)/4 - (year - 1901)/100 + ((year-1601)/400);
	int i = 0;
	for (i=0; i<(month-1)%12; i++)
		totalDaysInYear += DaysInMonth(year, i+1);
	for (i=0; i<m_nRow; i++)
		m_DaysinMonth_vec(i) = DaysInMonth(year + i/12, i%12+1);
	int WeekOfIMonth = (totalDaysInYear + 0 - 1)%7;//上月最后一天是周几-1，例如周二则为1；周七为6
	for (i=0; i<m_nRow; i++)
	{
		int weekdays = m_DaysinMonth_vec(i);
		for (int j=0; j<weekdays%7; j++)
			m_WeekDescrib_mx(i, (j+1+WeekOfIMonth)%7) ++;
		WeekOfIMonth = (WeekOfIMonth + weekdays)%7;	
	}
	return;
}


int CTimeX11::DaysInMonth(int year, int month)
{
	int DaysInMon = 0;
	switch(month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		DaysInMon = 31;
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		DaysInMon = 30;
		break;
    case 2 :
		DaysInMon = 28 + (year%4 == 0) - (year%100 == 0) + (year%400 == 0);
		break;
	}
	return DaysInMon;
}