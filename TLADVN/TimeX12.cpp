// TimeX12.cpp: implementation of the CTimeX12 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "TimeX12.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destructio
//////////////////////////////////////////////////////////////////////

CTimeX12::CTimeX12()
{
	m_nMethod=0;
	m_nWeekMethod = 1;
	m_nYear = 1990;
	m_pDataInterface = NULL;
	m_pResult = NULL;
    m_pRsltVariable = NULL;
	m_nAO = 0;
	m_nLS = 0;
	m_threshold = 3.3; 
	m_AONumber = 0;
	m_LSNumber = 0;
	
}

CTimeX12::~CTimeX12()
{	
}
BOOL CTimeX12::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	//	TIMEX12
	//		/DATA=[FILE]
	//		/VARLIST [varlist]
	//		/METHOD [{MULTIP**},{ADD}] [{NONE},{WORKDAY},{WEEK**}]（增加春节一个选项）[{NOHOLIDAY},{HOLIDAY},{NEWYEAR**}]
	//		/OUTLIER [AOOUTLIER1({1990},{1}), AOOUTLIER2...] [LSOUTLIER1({1990},{1}), LSOUTLIER2...]
	//		/YEAR [BEGIN({2000**},{1**})]
	//      /OPTION [NOPREDICT] [PREDSTEP(1)] [DIFF(1)]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_nMethod = int(AnaSyntax::findstr("/METHOD", "ADD", VOstr));
	m_nYear = AnaSyntax::getvalue("/YEAR", "BEGIN", VOstr);
	m_nMonth = AnaSyntax::getvalue("/YEAR", "BEGIN", VOstr, 2);
	m_prestep = AnaSyntax::getvalue("/OPTION","PREDSTEP",VOstr);
	m_ndifference = AnaSyntax::getvalue("/OPTION","DIFF",VOstr);
	if (m_nYear < 0)
		m_nYear = 1978;
	if (m_nMonth < 0 || m_nMonth > 12)
		m_nMonth = 9;
	
	//		/METHOD [{NONE},{WORKDAY},{WEEK**}] [{NOHOLIDAY},{HOLIDAY**}]
	if (AnaSyntax::findstr("/METHOD", "WORKDAY", VOstr))
		m_nWeekMethod = 2;
	if (AnaSyntax::findstr("/METHOD", "NONE", VOstr))
		m_nWeekMethod = 0;
	m_nHoliday = int(!AnaSyntax::findstr("/METHOD", "NOHOLIDAY", VOstr));

//用户指定AO,LS读入不了？？？？？？？？？？？？？

	//		/OUTLIER [AOOUTLIER1({1990},{1}), AOOUTLIER2...] [LSOUTLIER1({1990},{1}), LSOUTLIER2...]
	if (AnaSyntax::findstr("/OUTLIER", "AOOUTLIER1", VOstr))//解析AO的设置
	{
		CIntMatrix AOMx(100, 2, 0);
		BOOL bTemp = TRUE;
		int aoNum = 0;
		CTString aoStr;
		while (bTemp)
		{
			aoNum++;
			aoStr.Format("%s%d", "AOOUTLIER", aoNum);
			if (AnaSyntax::findstr("/OUTLIER", aoStr.GetData(), VOstr))
			{
				AOMx(aoNum-1, 0) = int(AnaSyntax::getvalue("/OUTLIER", aoStr.GetData(), VOstr));
				AOMx(aoNum-1, 1) = int(AnaSyntax::getvalue("/OUTLIER", aoStr.GetData(), VOstr, 2));
				if (AOMx(aoNum-1, 0) < 0 || AOMx(aoNum-1, 1) < 0)
					return FALSE;
			}
			else
			{
				m_nAO = aoNum - 1;
				bTemp = FALSE;
			}
		}
		AOMx.rerowsize(m_nAO);
 	    m_AOMx = AOMx;    
	}
	
	if (AnaSyntax::findstr("/OUTLIER", "LSOUTLIER1", VOstr))//解析LS的设置
	{
		CIntMatrix LSMx(100, 2, 0);
		BOOL bTemp = TRUE;
		int lsNum = 0;
		CTString lsStr;
		while (bTemp)
		{
			lsNum++;
			lsStr.Format("%s%d", "LSOUTLIER", lsNum);
			if (AnaSyntax::findstr("/OUTLIER", lsStr.GetData(), VOstr))
			{
				LSMx(lsNum-1, 0) = int(AnaSyntax::getvalue("/OUTLIER", lsStr.GetData(), VOstr));
				LSMx(lsNum-1, 1) = int(AnaSyntax::getvalue("/OUTLIER", lsStr.GetData(), VOstr, 2));
				if (LSMx(lsNum-1, 0) < 0 || LSMx(lsNum-1, 1) < 0)
					return FALSE;
			}
			else
			{
				m_nLS = lsNum - 1;
				bTemp = FALSE;
			}
		}
		LSMx.rerowsize(m_nLS);
	 	m_LSMx = LSMx;    
	}
	m_bpredict =! AnaSyntax::findstr("/OPTION","NOPREDICT",VOstr);
	return TRUE;
}


CResult * CTimeX12::OnTimeX12(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pRsltVariable = NULL;
	m_pRsltElementText = NULL;
	m_pWarningTextRslt = new CRsltElementText( "错误!" );
	m_pResult = new CResult("时间序列 X12-Arima模型分析");
	if (!GetValueFromVo(pDataInterface, VO))
	{
		m_pWarningTextRslt->AddString("读取参数发生错误，请重新运行！");
		m_pResult->Add(m_pWarningTextRslt);
		return m_pResult;
	}
	CDWordArray arCol;//读取数据
	int i=0;
	for (i=0; i<m_tVarInfo.iCount; i++)
		arCol.Add(m_tVarInfo.pValue[i]);
	CDoubleMatrix Data_read;
	m_nRow = m_pDataInterface->GetColsData(arCol, Data_read, 0);
	if (m_nRow != m_pDataInterface->GetRowCount() || m_nRow <36)
	{
		m_pWarningTextRslt->AddString( "数据存在缺失值，或是少于36个，不适合X12模型！" );
		m_pResult->Add(m_pWarningTextRslt);
		return m_pResult;
	}
	if (m_nAO>0||m_nLS>0)     //定义指定异常值的相对位置
	{
		m_PreOutliers_mx.create(m_nAO+m_nLS,0);
		int nTemp = 0;
		for (i=0; i<m_nAO; i++)
		{
			nTemp = (m_AOMx(i,0)-m_nYear)*12-m_nMonth+m_AOMx(i,1);
			m_PreOutliers_mx(i) = nTemp;
		}
		for (i=m_nAO; i<m_nAO+m_nLS; i++)
		{
			nTemp = (m_LSMx(i,0)-m_nYear)*12-m_nMonth+m_LSMx(i,1);
			if (nTemp < 2 || nTemp > m_nRow-2)
			{
				m_pWarningTextRslt->AddString("异常值指定有误");
				m_pResult->Add(m_pWarningTextRslt);
				return m_pResult;
			}
			m_PreOutliers_mx(i) = nTemp;
		}
	}

	//调试 起始**************************************************************************************
	m_nHoliday = 2;
	m_nMethod  = 0; 
	//调试 终止**************************************************************************************
	
	IndependentVariables(m_nYear, m_nMonth);//定义不包含自动识别的AO,LS的自变量
	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		m_OrgVec = Data_read(i);

       //把乘法模型变换为加法*******开始*******
 		if (m_nMethod == 0)
		{
			for (int j=0; j<m_nRow; j++)
			{
				if (m_OrgVec(j) > EPS_ACCUR)
				{
					m_OrgVec(j) = log(m_OrgVec(j));
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
       //把乘法模型变换为加法*******结束*******
		
		IdentifiyOutliers(m_allvariables_mx); //自动识别异常值
		PreAdjust(m_OrgVec); //预调整序列；
		Predict(m_AdjustVec, 2, 1, m_ndifference, 12);  //将预调整后的序列前后延伸一年
		BOOL btemp = Nonx11(m_AdjustVec);//X11
		//在趋势中调回AO******开始*****
	
		if(m_nAO>0)
		{
			CDoubleMatrix Adjustallvariables_mx(m_nRow+24,1,0.0);
			CDoubleVector BetaTemp_vec(m_nAO+m_AONumber, 0.0);
			m_Beta_Vec = BetaTemp_vec.copy();
			
			int temp = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2);
			Adjustallvariables_mx(0) = m_allvariables_mx(temp); 
			BetaTemp_vec(0) = m_allBeta_vec(temp);
			WriteData(BetaTemp_vec,"aa");
			for (i=temp+1; i<temp+m_nAO; i++)
			{
				CDoubleVector tempVec = m_allvariables_mx(i);
				Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
				BetaTemp_vec(i-temp) = m_allBeta_vec(i);
			}
			if(m_AONumber>0)
			{
				int temp2 = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2)+m_nAO+m_nLS+m_LSNumber; 
				for (i=temp2; i<temp2+m_AONumber; i++)
				{
					CDoubleVector tempVec = m_allvariables_mx(i);
					Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
					BetaTemp_vec(m_nAO+i-temp2-1) = m_allBeta_vec(i);
				}
			}
			CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
			double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
			m_CT_SmoothedVec =m_CT_SmoothedVec + Adjustallvariables_mx*BetaTemp_vec;
		}
		else
		{	
			if(m_AONumber>0)
			{
				CDoubleVector BetaTemp_vec(m_AONumber, 0.0);
				CDoubleMatrix Adjustallvariables_mx(m_nRow+24,1,0.0);
				int temp = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2)+m_nAO+m_nLS+m_LSNumber;
				Adjustallvariables_mx(0) = m_allvariables_mx(temp); 
				BetaTemp_vec(0) = m_allBeta_vec(temp);
				for (i=temp+1; i<temp+m_AONumber; i++)
				{
					CDoubleVector tempVec = m_allvariables_mx(i);
					Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
					BetaTemp_vec(i-temp) = m_allBeta_vec(i);
				}
				CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
				double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
				CDoubleVector t1; //--UNIX PORT
				t1 = Adjustallvariables_mx*BetaTemp_vec;
				m_CT_SmoothedVec += t1;
			}
		}
//在趋势中调回AO*****结束*****


		m_TrendVec.create(m_nRow,0.0);
		m_SeasonVec.create(m_nRow,0.0);
		m_IrregularVec.create(m_nRow,0.0);
		m_TCI_Vec.create(m_nRow,0.0);
		if (m_nMethod == 0)//把乘法模型变换为加法后，对其进行还原
		{
			for (int j=0; j<m_nRow; j++)
			{
				m_OrgVec(j) = exp(m_OrgVec(j));
				m_TrendVec(j) = exp(m_CT_SmoothedVec(j+12));
				m_SeasonVec(j) = exp(m_S_SmoothedVec(j+12));
				m_IrregularVec(j) = exp(m_I_FinalVec(j+12));
				m_TCI_Vec(j) = m_TrendVec(j)*m_IrregularVec(j);
			}
		}


	}
	//////预测///////
   	CArima ari;	
	m_predict_Vec = ari.ArimaPred(m_TCI_Vec, 2, 1, m_ndifference, m_prestep, false);
	CDoubleVector index_Vec(12,0.0);
	CIntVector num_vec(12, 0);
	for (i=0; i<m_nRow; i++)
	{
		index_Vec((i+1)%12) += m_SeasonVec(i);
		num_vec((i+1)%12)++;
	}
	for (i=0; i<12; i++)
	{
		index_Vec(i) = index_Vec(i) / num_vec(i);
	}
	double temp;
	for (i=0; i<m_prestep; i++)
	{
		temp = (i+1)%12;
		m_predict_Vec(i)  = m_predict_Vec(i) * index_Vec(temp);
	}

	Diagnoses();//诊断
	DrawResult(); //输出
	
	return m_pResult;
}


//IndependentVariables(m_nYear, m_nMonth)；//定义自变量：常数|闰年-（m_leapYear_mx）|季节因素（m_Season_mx）|指定异常值（m_AO_mx和m_LS_mx）|交易日（无/星期结构（m_weekDescrib_mx）/工作日（m_workday_mx））|春节（无/m_NewYear_mx）|节假日（无/m_Holiday_mx(春节|元旦|五一|十一)）；
void CTimeX12::IndependentVariables(int nYear, int nMonth)
{   
	int i = 0;
	int j = 0;
	int k = 0;
	int nTemp = 0;
    //定义常数变量
	m_constant_mx.create(m_nRow,1,1.0);
	WeekCalcu(nYear, nMonth);
	//定义趋势因素
	m_trend_mx.create(m_nRow,1,0.0);
	CDoubleMatrix trend_mx(m_nRow,3,0.0);
	trend_mx(0,0) = 1;
	trend_mx(0,1) = 1;
	trend_mx(0,2) = 1;
	for (i=1; i<m_nRow; i++)
	{
		trend_mx(i,0) = trend_mx(i-1,0)+1;
		trend_mx(i,1) = trend_mx(i-1,1)+trend_mx(i,0);
		trend_mx(i,2) = trend_mx(i-1,2)+trend_mx(i,1);
	}
	switch (m_ndifference)
	{
	case 2: m_trend_mx(0) = trend_mx(0); break;
	case 1: m_trend_mx(0) = trend_mx(1); break;
	case 0: m_trend_mx(0) = trend_mx(2); break;
	}
	CDoubleMatrix ForecastTrend_mx(12,1,0.0);
	CDoubleMatrix BackcastTrend_mx(12,1,0.0);
	CDoubleMatrix trend2_mx(12,3,0.0);
	CDoubleMatrix trend3_mx(12,3,0.0);
	trend2_mx(11,0) = 0;
	trend2_mx(11,1) = 0;
	trend2_mx(11,2) = 0;
	for (i=10; i>-1; i--)
	{
		trend2_mx(i,0) = trend2_mx(i+1,0)-1;
		trend2_mx(i,1) = trend2_mx(i+1,1)-trend2_mx(i+1,0);
		trend2_mx(i,2) = trend2_mx(i+1,2)-trend2_mx(i+1,1);
	}
	trend3_mx(0,0) = trend_mx(m_nRow-1,0);
	trend3_mx(0,1) = trend_mx(m_nRow-1,1);
	trend3_mx(0,2) = trend_mx(m_nRow-1,2);
	for (i=1; i<12; i++)
	{
		trend3_mx(i,0) = trend3_mx(i-1,0)+1;
		trend3_mx(i,1) = trend3_mx(i-1,1)+trend3_mx(i,0);
		trend3_mx(i,2) = trend3_mx(i-1,2)+trend3_mx(i,1);
	}
	switch (m_ndifference)
	{
	case 2: 
		{
			ForecastTrend_mx(0) = trend3_mx(0);
			BackcastTrend_mx(0) = trend2_mx(0);
			break;
		}
	case 1: 
		{
			ForecastTrend_mx(0) = trend3_mx(1);
			BackcastTrend_mx(0) = trend2_mx(1); 
			break;
		}
	case 0: 
		{
			ForecastTrend_mx(0) = trend3_mx(2);
			BackcastTrend_mx(0) = trend2_mx(2);
			break;
		}
	}

	//定义季节因素影响
	m_Season_mx.create(m_nRow,11,0.0);
	for (i=0; i<m_nRow; i++)
	{
		switch((nMonth+i)%12)
		{
		case 1:  m_Season_mx(i,0) = 1;  break;
		case 2:  m_Season_mx(i,1) = 1;  break;
		case 3:  m_Season_mx(i,2) = 1;  break;
		case 4:  m_Season_mx(i,3) = 1;  break;
		case 5:  m_Season_mx(i,4) = 1;  break;
		case 6:  m_Season_mx(i,5) = 1;  break;
		case 7:  m_Season_mx(i,6) = 1;  break;
		case 8:  m_Season_mx(i,7) = 1;  break;
		case 9:  m_Season_mx(i,8) = 1;  break;
		case 10: m_Season_mx(i,9) = 1;  break;
		case 11: m_Season_mx(i,10) = 1;  break;
		case 0: { for (j=0; j<11; j++) m_Season_mx(i,j) = -1;  break; }
		}
	}
	
	CDoubleMatrix ForecastSeason_mx(12,11,0.0);
	CDoubleMatrix BackcastSeason_mx(12,11,0.0);
	for (i=0; i<12; i++)
	{
		for(j=0; j<11; j++)
		{
			ForecastSeason_mx(i,j) = m_Season_mx(m_nRow-12+i,j);
			BackcastSeason_mx(i,j) = m_Season_mx(i,j);
		}
	}

	//定义指定异常值变量
	m_AO_mx.create(m_nRow, m_nAO, 0.0);
	m_LS_mx.create(m_nRow, m_nLS, 0.0);
	for (i=0; i<m_nAO; i++)
		m_AO_mx(m_PreOutliers_mx(i),i) = 1;
	for (i=m_nAO; i<m_nAO+m_nLS; i++)
		for (j=m_PreOutliers_mx(i); j< m_nRow; j++)  
			m_LS_mx(j,i-m_nAO) = 1;
	CDoubleMatrix CastAO_mx(12,1,0.0);
	CDoubleMatrix CastLS_mx(12,1,1.0);
	//定义春节节前的影响变量
	ChineseNewYear(nYear);
	m_NewYear_mx.create(m_nRow,1,0.0);
	CDoubleMatrix ForecastNewYear_mx(12,1,0.0);
	CDoubleMatrix BackcastNewYear_mx(12,1,0.0);
	if (m_nHoliday ==2)
	{
		int nEffectBeforeCNY=20;// 春节节前影响天数
		for (i=0; i<m_nRow/12+1; i++)
		{
			int dtemp = int(m_NewYearID_mx(i,0));
			switch(dtemp)
			{
			case 1:  
				{
					if (12*i+1-nMonth >-1 && 12*i+2-nMonth<m_nRow)  
					{
						m_NewYear_mx(12*i+1-nMonth,0) = min(m_NewYearID_mx(i,1),nEffectBeforeCNY); 
						if (12*i+2-nMonth <m_nRow)
						{
							m_NewYear_mx(12*i+2-nMonth,0) = max(nEffectBeforeCNY-m_NewYearID_mx(i,1),0);
						}
					}
					break;
				}
			case 2:
				{
					if (12*i+2-nMonth >-1 && 12*i+2-nMonth < m_nRow) 
					{
						m_NewYear_mx(12*i+2-nMonth,0) = min(m_NewYearID_mx(i,1),nEffectBeforeCNY); 
						if (12*i+1-nMonth >-1)
						{
							m_NewYear_mx(12*i+1-nMonth,0) = max(nEffectBeforeCNY-m_NewYearID_mx(i,1),0);
						}
					}
					break;
				}
			}
		}
		BackcastNewYear_mx(12-nMonth+1,0) = (m_ExtendNewYear_mx(0,0)>1.5)? max(nEffectBeforeCNY-m_ExtendNewYear_mx(0,1),0):min(m_ExtendNewYear_mx(0,1),nEffectBeforeCNY);
		BackcastNewYear_mx(12-nMonth+2,0) = (m_ExtendNewYear_mx(0,0)<1.5)? max(nEffectBeforeCNY-m_ExtendNewYear_mx(0,1),0):min(m_ExtendNewYear_mx(0,1),nEffectBeforeCNY);
		ForecastNewYear_mx((m_nRow+nMonth)%12,0) = (m_ExtendNewYear_mx(1,0)>1.5)?max(nEffectBeforeCNY-m_ExtendNewYear_mx(1,1),0):min(m_ExtendNewYear_mx(1,1),nEffectBeforeCNY);
		ForecastNewYear_mx((m_nRow+nMonth)%12+1,0) = (m_ExtendNewYear_mx(1,0)<1.5)?max(nEffectBeforeCNY-m_ExtendNewYear_mx(1,1),0):min(m_ExtendNewYear_mx(1,1),nEffectBeforeCNY);
	}
	CDoubleMatrix temp_mx = BackcastNewYear_mx.copy();
	for (i=0; i<12; i++)
	{
		 BackcastNewYear_mx(i,0) = temp_mx(11-i,0);
	}
	temp_mx = ForecastNewYear_mx.copy();
	for (i=0; i<12; i++)
	{
		 ForecastNewYear_mx(i,0) = temp_mx(11-i,0);
	}

	//定义节假日影响变量
	m_Holiday_mx.create(m_nRow,4,0.0);
	CDoubleMatrix ForecastHoliday_mx(12,4,0.0);
	CDoubleMatrix BackcastHoliday_mx(12,4,0.0);
	j=0;
	for (i=0; i<m_nRow; i++)
	{
		switch((nMonth+i)%12)
		{
		case 1: 
			{
				m_Holiday_mx(i,0) = 3;    //元旦
				if ((i-1+nMonth)/12 > -1 && (i-1+nMonth)/12 < m_nRow/12+1)
				{
					int dtemp = (i-1+nMonth)/12;
					int dtemp1 = int(m_NewYearID_mx((i-1+nMonth)/12,0));
					if (int(m_NewYearID_mx((i-1+nMonth)/12,0))==1)
					{
						m_Holiday_mx(i,1) = min(7,31-m_NewYearID_mx((i-1+nMonth)/12,1)); //春节
						m_Holiday_mx(i+1,1) = max(0,7-31+m_NewYearID_mx((i-1+nMonth)/12,1));
						double teee = m_Holiday_mx(i,1);
					}
				}
				break;	
			}
		case 2:
			{
				if ((i-2+nMonth)/12 > -1 && (i-2+nMonth)/12 < m_nRow/12+1)
				{
					int dtemp = (i-2+nMonth)/12;
					int dtemp1 = int(m_NewYearID_mx((i-2+nMonth)/12,0));
					if (int(m_NewYearID_mx((i-2+nMonth)/12,0))==2)
					{
						m_Holiday_mx(i,1) = min(7,31-m_NewYearID_mx((i-2+nMonth)/12,1)); //春节
						double teee = m_Holiday_mx(i,1);
					}
					
				}
				break;	
			}
		case 5:		
			m_Holiday_mx(i,2) = 7;   //五一
			break; 
		case 10:     
			m_Holiday_mx(i,3) = 7;   //十一
			break;
		default:
			break;
		}
	}
	BackcastHoliday_mx((12-nMonth+1)%12,0) = 3;
	BackcastHoliday_mx((12-nMonth+1)%12,1) = (m_ExtendNewYear_mx(0,0)>1.5)?0:min(7,31-m_ExtendNewYear_mx(0,1));
	BackcastHoliday_mx((12-nMonth+2)%12,1) = (m_ExtendNewYear_mx(0,0)>1.5)?7:max(0,7+m_ExtendNewYear_mx(0,1)-31);
	BackcastHoliday_mx((12-nMonth+5)%12,2) = 7;
	BackcastHoliday_mx((12-nMonth+10)%12,3) = 7;

	ForecastHoliday_mx((m_nRow+nMonth)%12,0) = 3;
	ForecastHoliday_mx((m_nRow+nMonth+1)%12,1) = (m_ExtendNewYear_mx(1,0)>1.5)?0:min(7,31-m_ExtendNewYear_mx(1,1));
	ForecastHoliday_mx((m_nRow+nMonth+1)%12,1) = (m_ExtendNewYear_mx(1,0)>1.5)?7:max(0,7+m_ExtendNewYear_mx(1,1)-31);
	ForecastHoliday_mx((m_nRow+nMonth+4)%12,2) = 7;
	ForecastHoliday_mx((m_nRow+nMonth+9)%12,3) = 7;

	temp_mx = ForecastHoliday_mx.copy();
	for (i=0; i<12; i++)
	{
		for(j=0; j<4; j++)
		{
			ForecastHoliday_mx(i,j) = temp_mx(11-i,j);
		}
	}
	temp_mx = BackcastHoliday_mx.copy();
	for (i=0; i<12; i++)
	{
		for(j=0; j<4; j++)
		{
			BackcastHoliday_mx(i,j) = temp_mx(11-i,j);
		}
	}
	
	//定义交易日变量（星期/工作日）
	m_workday_mx.create(m_nRow, 2, 0.0);
	for (i=0; i<m_nRow; i++)
	{
		m_workday_mx(i,0) = m_WeekDescrib_mx(i,0)+m_WeekDescrib_mx(i,1)+m_WeekDescrib_mx(i,2)+m_WeekDescrib_mx(i,3)+m_WeekDescrib_mx(i,4);
		m_workday_mx(i,1) = m_WeekDescrib_mx(i,5)+m_WeekDescrib_mx(i,6);  
	}
	CDoubleMatrix Forecastworkday_mx(12,2,0.0);
	CDoubleMatrix Backcastworkday_mx(12,2,0.0);
	for (i=0; i<12; i++)
	{
		Forecastworkday_mx(i,0) = m_ForecastWeekDescrib_mx(i,0)+m_ForecastWeekDescrib_mx(i,1)+m_ForecastWeekDescrib_mx(i,2)+m_ForecastWeekDescrib_mx(i,3)+m_ForecastWeekDescrib_mx(i,4);
		Forecastworkday_mx(i,1) = m_ForecastWeekDescrib_mx(i,5)+m_ForecastWeekDescrib_mx(i,6);  
		Backcastworkday_mx(i,0) = m_BackcastWeekDescrib_mx(i,0)+m_BackcastWeekDescrib_mx(i,1)+m_BackcastWeekDescrib_mx(i,2)+m_BackcastWeekDescrib_mx(i,3)+m_BackcastWeekDescrib_mx(i,4);
		Backcastworkday_mx(i,1) = m_BackcastWeekDescrib_mx(i,5)+m_BackcastWeekDescrib_mx(i,6);  
	}
	temp_mx = Forecastworkday_mx.copy();
	for (i=0; i<2; i++)
	{
		Forecastworkday_mx(i,0) = temp_mx(1-i,0);
		Forecastworkday_mx(i,1) = temp_mx(1-i,1);
	}
	temp_mx = Backcastworkday_mx.copy();
	for (i=0; i<2; i++)
	{
		Backcastworkday_mx(i,0) = temp_mx(1-i,0);
		Backcastworkday_mx(i,1) = temp_mx(1-i,1);
	}

	//定义不包含自动识别的AO,LS的自变量
	m_allvariables_mx = m_constant_mx.copy();
	m_Forecastvariables_mx.create(12,1,1.0);
	m_Backcastvariables_mx.create(12,1,1.0);
	m_allvariables_mx = m_allvariables_mx.commat(m_trend_mx,1);
	m_Forecastvariables_mx = m_Forecastvariables_mx.commat(ForecastTrend_mx,1);
	m_Backcastvariables_mx = m_Backcastvariables_mx.commat(BackcastTrend_mx,1);
	m_allvariables_mx = m_allvariables_mx.commat(m_Season_mx,1);
	m_Forecastvariables_mx = m_Forecastvariables_mx.commat(ForecastSeason_mx,1);
	m_Backcastvariables_mx = m_Backcastvariables_mx.commat(BackcastSeason_mx,1);
	if (m_nAO > 0)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_AO_mx,1);
		m_Forecastvariables_mx = m_Forecastvariables_mx.commat(CastAO_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(CastAO_mx,1);
	}
	if (m_nLS > 0)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_LS_mx,1);
		m_Forecastvariables_mx = m_Forecastvariables_mx.commat(CastAO_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(CastLS_mx,1);
	}
	if (m_nHoliday == 2)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_NewYear_mx,1);
		m_Forecastvariables_mx = m_Forecastvariables_mx.commat(ForecastNewYear_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(BackcastNewYear_mx,1);
	}
	if (m_nHoliday == 1)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_Holiday_mx,1);
		m_Forecastvariables_mx = m_Forecastvariables_mx.commat(ForecastHoliday_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(BackcastHoliday_mx,1);
	}
	if (m_nWeekMethod ==1)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_WeekDescrib_mx,1);
		m_Forecastvariables_mx = m_Forecastvariables_mx.commat(m_ForecastWeekDescrib_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(m_BackcastWeekDescrib_mx,1);
	}
	else if (m_nWeekMethod == 2)
	{
		m_allvariables_mx = m_allvariables_mx.commat(m_workday_mx,1);
		m_Forecastvariables_mx= m_Forecastvariables_mx.commat(Forecastworkday_mx,1);
		m_Backcastvariables_mx = m_Backcastvariables_mx.commat(Backcastworkday_mx,1);
	}
	int dddtemp = m_Forecastvariables_mx.mcol();
	int dddte = m_allvariables_mx.mcol();
	return;
}


//WeekCalcu(nYear, nMonth);//计算序列每个月有多少天，平均每个月多少天，每个月有多少个周一周二……，放入m_DaysinMonth_vec,m_DayaveMonth_vec和m_WeekDescrib_mx
void CTimeX12::WeekCalcu(int nYear, int nMonth)
{
	//以1900年1月0日为周0为起始标准
	
	m_DaysinMonth_vec.create(m_nRow, 0.0);  
	m_WeekDescrib_mx.create(m_nRow, 7, 4.0);
	int totalDaysInYear = (nYear - 1900) * 365 + (nYear - 1901)/4 - (nYear - 1901)/100 + ((nYear-1601)/400);
	int i = 0;
	int j = 0;    
	for (i=0; i<(nMonth-1)%12; i++)
		totalDaysInYear += DaysInMonth(nYear, i+1);
	for (i=0; i<m_nRow; i++)
	{
		m_DaysinMonth_vec(i) = DaysInMonth(nYear + i/12, i%12+1);
	}                           
	int WeekOfIMonth = (totalDaysInYear + 0 - 1)%7;//上月最后一天是周几-1，例如周二则为1；周七为6
	for (i=0; i<m_nRow; i++)
	{
		int weekdays = m_DaysinMonth_vec(i);
		for (int j=0; j<weekdays%7; j++)
			m_WeekDescrib_mx(i, (j+1+WeekOfIMonth)%7) ++;
		WeekOfIMonth = (WeekOfIMonth + weekdays)%7;	
	}
	m_ForecastWeekDescrib_mx.create(12,7,4.0);
	m_BackcastWeekDescrib_mx.create(12,7,4.0);
	m_CasetDaysinMonth_vec.create(12,0.0);

	totalDaysInYear = (nYear+ int((m_nRow+nMonth)/12) - 1900) * 365 + (nYear +int((m_nRow+nMonth)/12)- 1901)/4 - (nYear +int((m_nRow+nMonth)/12)- 1901)/100 + ((nYear+int((m_nRow+nMonth)/12)-1601)/400); 
	for (i=0; i<(m_nRow+nMonth-1)%12; i++)
		totalDaysInYear += DaysInMonth(nYear+ int((m_nRow+nMonth)/12), i+1);
	for (i=0; i<12; i++)
	{
		m_CasetDaysinMonth_vec(i) = DaysInMonth(nYear+int((m_nRow+nMonth)/12) + i/12, i%12+1);
	}                           
	WeekOfIMonth = (totalDaysInYear + 0 - 1)%7;//上月最后一天是周几-1，例如周二则为1；周七为6
	for (i=0; i<12; i++)
	{
		int weekdays = m_CasetDaysinMonth_vec(i);
		for (int j=0; j<weekdays%7; j++)
			m_ForecastWeekDescrib_mx(i, (j+1+WeekOfIMonth)%7) ++;
		WeekOfIMonth = (WeekOfIMonth + weekdays)%7;	
	}
	totalDaysInYear = (nYear-1 - 1900) * 365 + (nYear-1 - 1901)/4 - (nYear-1 - 1901)/100 + ((nYear-1-1601)/400);   
	for (i=0; i<(nMonth-1)%12; i++)
		totalDaysInYear += DaysInMonth(nYear-1, i+1);
	for (i=0; i<12; i++)
	{
		m_CasetDaysinMonth_vec(i) = DaysInMonth(nYear-1 + i/12, i%12+1);
	}                           
	WeekOfIMonth = (totalDaysInYear + 0 - 1)%7;//上月最后一天是周几-1，例如周二则为1；周七为6
	for (i=0; i<12; i++)
	{
		int weekdays = m_DaysinMonth_vec(i);
		for (int j=0; j<weekdays%7; j++)
			m_BackcastWeekDescrib_mx(i, (j+1+WeekOfIMonth)%7) ++;
		WeekOfIMonth = (WeekOfIMonth + weekdays)%7;	
	}
	return;
}

// DaysInMonth(nYear, i+1);//计算每个月有多少天
int CTimeX12::DaysInMonth(int nYear, int nMonth)
{
	int DaysInMon = 0;
	switch(nMonth)
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
		DaysInMon = 28 + (nYear%4 == 0) - (nYear%100 == 0) + (nYear%400 == 0);
		break;
	}
	return DaysInMon;
}


// ChineseNewYear(int nYear, int nMonth);确定春节位置，放入m_NewYear_mx
void CTimeX12::ChineseNewYear(int nYear)
{
	//从1900年为起始标准
	int a[112] = {1,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,2,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,2,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2,1,2,2,1,2,1,2,2,1,2,2};
	int b[112] = {31,19,8,29,16,4,25,13,2,22,10,30,18,6,26,14,4,23,11,1,20,8,28,16,5,24,13,2,23,10,30,17,6,26,14,4,24,11,31,19,8,27,15,5,25,13,2,22,10,29,17,6,27,14,3,24,12,31,18,8,28,15,5,25,13,2,21,9,30,17,6,27,15,3,23,11,31,18,7,28,16,5,25,13,2,20,9,29,17,6,27,15,4,23,10,31,19,7,28,16,5,24,12,1,22,9,29,18,7,26,14,14};
	m_NewYearID_mx.create(m_nRow/12+1,2,0.0);
	int i = 0;
	for (i=0; i<m_nRow/12+1; i++)
	{
		m_NewYearID_mx(i,0) = a[nYear-1900+i];    
		m_NewYearID_mx(i,1) = b[nYear-1900+i];
	}
	m_ExtendNewYear_mx.create(2,2,0.0);
	m_ExtendNewYear_mx(0,0) = a[nYear-1900-1];
	m_ExtendNewYear_mx(0,1) = b[nYear-1900-1];
	m_ExtendNewYear_mx(1,0) = a[nYear-1900+m_nRow/12+1];
	m_ExtendNewYear_mx(1,1) = b[nYear-1900+m_nRow/12+1];
	return;
}

///********自动识别异常值
void CTimeX12::IdentifiyOutliers(CDoubleMatrix variables_mx)
{	
	int i;
	int j;
	BOOL bTemp = TRUE;
	int nRow = variables_mx.mrow();
	int nCol = variables_mx.mcol();
	CDoubleMatrix FCastConstant_mx(12,1,1.0);
	CDoubleMatrix BCastConstant_mx(12,1,0.0);
	CDoubleMatrix Forecast_mx;
	CDoubleMatrix Backcast_mx;

//添加LS
	CDoubleVector location_vec(m_nRow,0.0);
	for (i=2; i<m_nRow-1; i++)
	{
		double Beaverage = 0;
		double Afaverage = 0; 
		for(j=0; j<i; j++)
		{
			Beaverage += m_OrgVec(j)/i;
		}
		for(j=i; j<m_nRow; j++)
		{
			Afaverage += m_OrgVec(j)/(m_nRow-i);
		}
		location_vec(i) = Afaverage-Beaverage;
	}
	CDoubleVector locationabs_vec = location_vec.abs();//abs函数可能有错
	if (m_nLS>0)
	{
		for (i=m_nAO; i<m_nAO+m_nLS; i++)
		{
			locationabs_vec(m_PreOutliers_mx(i)) = 0;
		}
	}
	while(bTemp)
	{
		int nLSId;
		CDoubleVector NewLS_vec(m_nRow, 0.0); 
		double dLocation = locationabs_vec.Max(nLSId);
		for (i=nLSId; i<m_nRow; i++)	
		{
			NewLS_vec(i) = 1;
		}
		locationabs_vec(nLSId) = 0;
		variables_mx = variables_mx|NewLS_vec;
		if (Forecast_mx.mcol()>0) 
		{
			Forecast_mx = Forecast_mx.commat(FCastConstant_mx,1);
			Backcast_mx = Backcast_mx.commat(BCastConstant_mx,1);
		}
		else
		{
			Forecast_mx = FCastConstant_mx.copy();
			Backcast_mx = BCastConstant_mx.copy();
		}
		CDoubleMatrix invXtimeX = inv(dot(variables_mx, variables_mx));
		CDoubleVector beta_vec_newLS = invXtimeX*dot(variables_mx,m_OrgVec);
		CDoubleVector residual_vec = m_OrgVec - variables_mx * beta_vec_newLS;
		double dSigma = sqrt(residual_vec.var());
		int nLSCol = variables_mx.mcol();	
		double tVauleLS = beta_vec_newLS(nLSCol-1) / (invXtimeX(nLSCol-1, nLSCol-1)*dSigma);
		if (fabs(tVauleLS) < m_threshold)
		{
			bTemp = FALSE;
			variables_mx.delt(nLSCol-1);
			int kk = Forecast_mx.mcol();
			Forecast_mx.delt(kk-1);
			Backcast_mx.delt(kk-1);
	
		}
	}
	int nNewCol= variables_mx.mcol();
	
//添加AO
	bTemp = TRUE;
	while(bTemp)
	{
		CDoubleVector beta_vec = inv(dot(variables_mx, variables_mx))*(dot(variables_mx, m_OrgVec));
		CDoubleVector residual_vec = m_OrgVec - variables_mx * beta_vec;
		CDoubleVector residualabs_vec = residual_vec.abs();  //abs函数可能有错
		if (m_nAO>0)
		{
			for (i=0; i<m_nAO; i++)
				residualabs_vec(m_PreOutliers_mx(i)) = 0;
		}
		int nAoId;
		double dResValue = residualabs_vec.Max(nAoId);
		CDoubleVector NewAo_vec(m_nRow, 0.0);
		NewAo_vec(nAoId) = 1;  
		variables_mx = variables_mx|NewAo_vec;
		if (Forecast_mx.mcol()>0) 
		{
			Forecast_mx = Forecast_mx.commat(BCastConstant_mx,1);
			Backcast_mx = Backcast_mx.commat(BCastConstant_mx,1);
		}
		else 
		{
			Forecast_mx = BCastConstant_mx.copy();
			Backcast_mx = BCastConstant_mx.copy();
		}
		CDoubleMatrix invXtimeX = inv(dot(variables_mx, variables_mx));
		CDoubleVector beta_vec_newAO = invXtimeX*(dot(variables_mx,m_OrgVec));
		residual_vec = m_OrgVec - variables_mx * beta_vec_newAO;
		double dSigma = sqrt(residual_vec.var());
		int nAOCol = variables_mx.mcol();	
		double tVauleAO = beta_vec_newAO(nAOCol-1) / invXtimeX(nAOCol-1, nAOCol-1)/dSigma;
		if (fabs(tVauleAO)<m_threshold)
		{
			bTemp = FALSE;
			variables_mx.delt(nAOCol-1);
			int kk = Forecast_mx.mcol();
			Forecast_mx.delt(kk-1);
			Backcast_mx.delt(kk-1);
		}	
	}

// 检验剔除新增的AO,LS
	CDoubleVector beta_vec = inv(dot(variables_mx, variables_mx))*dot(variables_mx,m_OrgVec);
	int nTemp = variables_mx.mcol();
	double tVauleAO;
	double tVauleLS;
	CDoubleMatrix invXtimeX = inv(dot(variables_mx, variables_mx));
	CDoubleVector beta_vec_new = invXtimeX * dot(variables_mx,m_OrgVec);
	CDoubleVector residual_vec = m_OrgVec - variables_mx * beta_vec;
	double dSigma = sqrt(residual_vec.var());
	 m_LSNumber = nNewCol-nCol;
	 m_AONumber = nTemp-nNewCol;
	for(i=nCol; i<nNewCol; i++)
	{
		tVauleLS = beta_vec_new(i) / (invXtimeX(i, i)*dSigma);
		if (fabs(tVauleLS) < m_threshold)
		{
			variables_mx.delt(i);
			Forecast_mx.delt(i-nCol);
			Backcast_mx.delt(i-nCol);
			m_LSNumber = m_LSNumber-1;
		}
	}
	for(i=nNewCol; i<nTemp; i++)
	{
		tVauleAO = beta_vec_new(i) / (invXtimeX(i, i)*dSigma);
		if (fabs(tVauleAO) < m_threshold)
		{
			variables_mx.delt(i);
			Forecast_mx.delt(i-nNewCol+nCol);
			Backcast_mx.delt(i-nNewCol+nCol);
			m_AONumber = m_AONumber-1;
		}
	}
	m_allvariables_mx = variables_mx.copy();
	int t1 = m_allvariables_mx.mcol();
	int t2 = Forecast_mx.mcol()+ m_Forecastvariables_mx.mcol();
	int t3 = Backcast_mx.mcol()+ m_Backcastvariables_mx.mcol();
	return;
}


//预调整
void CTimeX12::PreAdjust(CDoubleVector OrgVec)
{
	m_allBeta_vec = inv(dot(m_allvariables_mx, m_allvariables_mx))*dot(m_allvariables_mx,OrgVec);
	int i = 0;
	CDoubleMatrix Adjustallvariables_mx(m_nRow,1,0.0);
	CDoubleVector BetaTemp_vec(m_allBeta_vec.vlen()-2, 0.0);
	Adjustallvariables_mx(0) = m_allvariables_mx(2); 
	BetaTemp_vec(0) = m_allBeta_vec(2);
	for (i=3; i<m_allvariables_mx.mcol(); i++)
	{
		CDoubleVector tempVec = m_allvariables_mx(i);
		Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
		BetaTemp_vec(i-2) = m_allBeta_vec(i);
	}
	CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
	double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
	m_AdjustVec = OrgVec - Adjustallvariables_mx*BetaTemp_vec+dTemp;
	m_Coff_mx = Adjustallvariables_mx.copy();
	m_Beta_Vec= BetaTemp_vec.copy();
	return;
}

void CTimeX12::Predict(CDoubleVector OrgVec, int mp, int mq, int md, int length)
{
		CArima ari;
		int i = 0;
		CDoubleVector tempVec = OrgVec.copy();
		CDoubleVector Forecast_Vec = ari.ArimaPred(tempVec, mp, mq, md, length, false);
		for (i=0; i<m_nRow; i++)
			{
				tempVec(i) = OrgVec(m_nRow-i-1);
			}
		CDoubleVector Backcast_Vec = ari.ArimaPred(tempVec, mp, mq, md, length,false);
		CDoubleVector addVec(m_nRow+24,0.0);
		for (i=0; i<12; i++)
		{
			addVec(i) = Backcast_Vec(11-i);
			addVec(i+m_nRow+12) = Forecast_Vec(i);
		}
		for (i=0; i<m_nRow; i++)
		{
			addVec(i+12) = m_AdjustVec(i);
		}
		m_AdjustVec = addVec.copy();
		return;
}


BOOL CTimeX12::Nonx11(CDoubleVector Org_vec)
{
	int i = 0;
	int j = 0;
	m_allvariables_mx = m_Backcastvariables_mx.commat(m_allvariables_mx);
	m_allvariables_mx = m_allvariables_mx.commat(m_Forecastvariables_mx);

//调整延伸的序列的*****开始*******
	int temp = 2;
	if (temp<m_allvariables_mx.mcol())
	{
		CDoubleVector BetaTemp_vec(m_allBeta_vec.vlen()-temp, 0.0);
		CDoubleMatrix Adjustallvariables_mx(m_nRow+24,1,0.0);
		Adjustallvariables_mx(0) = m_allvariables_mx(temp); 
		BetaTemp_vec(0) = m_allBeta_vec(temp);
		for (i=temp+1; i<m_allvariables_mx.mcol(); i++)
		{
			CDoubleVector tempVec = m_allvariables_mx(i);
			Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
			BetaTemp_vec(i-temp) = m_allBeta_vec(i);
		}
		CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
		double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
		CDoubleVector t1;
		t1 = Adjustallvariables_mx*BetaTemp_vec;
		CDoubleVector t2;
		t2 = t1-dTemp;
		Org_vec += t2;		 //--UNIX PORT
		CDoubleVector ttemp_vec = Adjustallvariables_mx*BetaTemp_vec;
	}
//调整延伸的序列*****结束*******

//调回序列AO*****开始*******
	if(m_nAO>0)
	{
		CDoubleVector BetaTemp_vec(m_nAO+m_AONumber, 0.0);
		CDoubleMatrix Adjustallvariables_mx(m_nRow+24,1,0.0);
		int temp = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2);
		Adjustallvariables_mx(0) = m_allvariables_mx(temp); 
		BetaTemp_vec(0) = m_allBeta_vec(temp);
		for (i=temp+1; i<temp+m_nAO; i++)
		{
			CDoubleVector tempVec = m_allvariables_mx(i);
			Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
			BetaTemp_vec(i-temp) = m_allBeta_vec(i);
		}
		if(m_AONumber>0)
		{
			int temp2 = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2)+m_nAO+m_nLS+m_LSNumber; 
			for (i=temp2; i<temp2+m_AONumber; i++)
			{
				CDoubleVector tempVec = m_allvariables_mx(i);
				Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
				BetaTemp_vec(m_nAO+i-temp2-1) = m_allBeta_vec(i);
			}
		}
		CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
		double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
		Org_vec -= Adjustallvariables_mx*BetaTemp_vec+dTemp;
	}
	else
	{	
		if(m_AONumber>0)
		{
			CDoubleVector BetaTemp_vec(m_AONumber, 0.0);
			CDoubleMatrix Adjustallvariables_mx(m_nRow+24,1,0.0);
			int temp = 13+(m_nHoliday==2)+4*(m_nHoliday==1)+7*(m_nWeekMethod==1)+2*(m_nWeekMethod==2)+m_nAO+m_nLS+m_LSNumber;
			Adjustallvariables_mx(0) = m_allvariables_mx(temp); 
			BetaTemp_vec(0) = m_allBeta_vec(temp);
			for (i=temp+1; i<temp+m_AONumber; i++)
			{
				CDoubleVector tempVec = m_allvariables_mx(i);
				Adjustallvariables_mx = Adjustallvariables_mx|tempVec;
				BetaTemp_vec(i-temp) = m_allBeta_vec(i);
			}
			CDoubleVector VariablesAve_vec = mean(Adjustallvariables_mx);
			double dTemp = VariablesAve_vec.dot(BetaTemp_vec);
			Org_vec -= Adjustallvariables_mx*BetaTemp_vec+dTemp;
		}
   		}
//调回序列AO*****结束*******

	CDoubleVector Trend_vec(m_nRow+24,0.0);//初步趋势估计(12*2)
	for (i=0; i<m_nRow+24; i++)
	{
		double dTemp1 = (i-5>0)?Org_vec(i-6):Org_vec(i+6);
		dTemp1 += (i-4>0)?Org_vec(i-5)*2:Org_vec(i+7)*2;
		dTemp1 += (i-3>0)?Org_vec(i-4)*2:Org_vec(i+8)*2;
		dTemp1 += (i-2>0)?Org_vec(i-3)*2:Org_vec(i+9)*2;
		dTemp1 += (i-1>0)?Org_vec(i-2)*2:Org_vec(i+10)*2;
		dTemp1 += (i>0)?Org_vec(i-1)*2:Org_vec(i+11)*2;
		double dTemp2 = Org_vec(i)*2;
		double dTemp3 = (i+1<m_nRow+24)?Org_vec(i+1)*2:Org_vec(i-11)*2;
		dTemp3 += (i+2<m_nRow+24)?Org_vec(i+2)*2:Org_vec(i-10)*2;
		dTemp3 += (i+3<m_nRow+24)?Org_vec(i+3)*2:Org_vec(i-9)*2;
		dTemp3 += (i+4<m_nRow+24)?Org_vec(i+4)*2:Org_vec(i-8)*2;
		dTemp3 += (i+5<m_nRow+24)?Org_vec(i+5)*2:Org_vec(i-7)*2;
		dTemp3 += (i+6<m_nRow+24)?Org_vec(i+6):Org_vec(i-6);
		Trend_vec(i) = (dTemp1+dTemp2+dTemp3)/24;
	} 
	CDoubleVector SIVec(m_nRow+24,0.0);  //初步SI比率估计
	for (i=0; i<m_nRow+24; i++)
	{
		SIVec(i) = Org_vec(i)-Trend_vec(i);
	}
	CDoubleVector SeasonVec = SeasonMove(SIVec,0); //初步季节估计(3*3)
	CDoubleVector AdjustSeason_vec(m_nRow+24,0.0);//初步季节调整
	for (i=0; i<m_nRow+24; i++)   
	{
		AdjustSeason_vec(i) = Org_vec(i)-SeasonVec(i);
	}

	int TrendID = TrendMethod(AdjustSeason_vec);// 中间选择趋势估计方法
	Trend_vec = TrendMove(AdjustSeason_vec, TrendID); //中间趋势估计
	for (i=0; i<m_nRow+24; i++)//中间SI比率估计
	{
		SIVec(i) = Org_vec(i)-Trend_vec(i);
	}
	SeasonVec = SeasonMove(SIVec,1);    //中间季节估计(3*5)
	for (i=0; i<m_nRow+24; i++)   //中间季节调整
	{
		AdjustSeason_vec(i) =Org_vec(i)-SeasonVec(i);
	}

	TrendID = TrendMethod(AdjustSeason_vec);// 选择趋势估计方法
	Trend_vec = TrendMove(AdjustSeason_vec,TrendID); //最终趋势估计
	m_IrregularVec.create(m_nRow+24,0.0);
	for (i=0; i<m_nRow+24; i++)
	{
		m_IrregularVec(i) = AdjustSeason_vec(i)-Trend_vec(i);
	}
	m_CT_SmoothedVec = Trend_vec;
	m_I_FinalVec= m_IrregularVec;
	m_S_SmoothedVec = SeasonVec;
	return  TRUE;
}


CDoubleVector CTimeX12::TrendMove(CDoubleVector SmoothedCT, int nMethod)
{
		WeightCalcu(nMethod);
		CDoubleVector Trend_Vec(m_nRow+24,0.0);
		switch (nMethod)
		{
		case 9:
			{
				for (int i=0; i<m_nRow+24; i++)
				{
					double dTemp1 = (i-3>0)?m_Weight_Vec(0)*SmoothedCT(i-4):m_Weight_Vec(0)*SmoothedCT(i+8);
					dTemp1 += (i-2>0)?m_Weight_Vec(1)*SmoothedCT(i-3):m_Weight_Vec(1)*SmoothedCT(i+9);
					dTemp1 += (i-1>0)?m_Weight_Vec(2)*SmoothedCT(i-2):m_Weight_Vec(2)*SmoothedCT(i+10);
					dTemp1 += (i>0)?m_Weight_Vec(3)*SmoothedCT(i-1):m_Weight_Vec(3)*SmoothedCT(i+11);
					double dTemp2 = m_Weight_Vec(4)*SmoothedCT(i);
					double dTemp3 = (i+1<m_nRow+24)?m_Weight_Vec(5)*SmoothedCT(i+1):m_Weight_Vec(5)*SmoothedCT(i-11);
					dTemp3 += (i+2<m_nRow+24)?m_Weight_Vec(6)*SmoothedCT(i+2):m_Weight_Vec(6)*SmoothedCT(i-10);
					dTemp3 += (i+3<m_nRow+24)?m_Weight_Vec(7)*SmoothedCT(i+3):m_Weight_Vec(7)*SmoothedCT(i-9);
					dTemp3 += (i+4<m_nRow+24)?m_Weight_Vec(8)*SmoothedCT(i+4):m_Weight_Vec(8)*SmoothedCT(i-8);
					Trend_Vec(i) = dTemp1+dTemp2+dTemp3;
				}
				break;
			}
		case 13:
			{
				for (int i=0; i<m_nRow+24; i++)
				{
					double dTemp1 = (i-5>0)?m_Weight_Vec(0)*SmoothedCT(i-6):m_Weight_Vec(0)*SmoothedCT(i+6);
					dTemp1 += (i-4>0)?m_Weight_Vec(1)*SmoothedCT(i-5):m_Weight_Vec(1)*SmoothedCT(i+7);
					dTemp1 += (i-3>0)?m_Weight_Vec(2)*SmoothedCT(i-4):m_Weight_Vec(2)*SmoothedCT(i+8);
					dTemp1 += (i-2>0)?m_Weight_Vec(3)*SmoothedCT(i-3):m_Weight_Vec(3)*SmoothedCT(i+9);
					dTemp1 += (i-1>0)?m_Weight_Vec(4)*SmoothedCT(i-2):m_Weight_Vec(4)*SmoothedCT(i+10);
					dTemp1 += (i>0)?m_Weight_Vec(5)*SmoothedCT(i-1):m_Weight_Vec(5)*SmoothedCT(i+11);
					double dTemp2 = m_Weight_Vec(6)*SmoothedCT(i);
					double dTemp3 = (i+1<m_nRow+24)?m_Weight_Vec(7)*SmoothedCT(i+1):m_Weight_Vec(7)*SmoothedCT(i-11);
					dTemp3 += (i+2<m_nRow+24)?m_Weight_Vec(8)*SmoothedCT(i+2):m_Weight_Vec(8)*SmoothedCT(i-10);
					dTemp3 += (i+3<m_nRow+24)?m_Weight_Vec(9)*SmoothedCT(i+3):m_Weight_Vec(9)*SmoothedCT(i-9);
					dTemp3 += (i+4<m_nRow+24)?m_Weight_Vec(10)*SmoothedCT(i+4):m_Weight_Vec(10)*SmoothedCT(i-8);
					dTemp3 += (i+5<m_nRow+24)?m_Weight_Vec(11)*SmoothedCT(i+5):m_Weight_Vec(11)*SmoothedCT(i-7);
					dTemp3 += (i+6<m_nRow+24)?m_Weight_Vec(12)*SmoothedCT(i+6):m_Weight_Vec(12)*SmoothedCT(i-6);
					Trend_Vec(i) = dTemp1+dTemp2+dTemp3;
				}
				break;
			}
		}	
	return Trend_Vec;
}


CDoubleVector CTimeX12::SeasonMove(CDoubleVector SI_vec, int method)
{
	CDoubleVector retvec(m_nRow+24,0.0);
	switch (method)
	{
	case 0:   /* 季节滑动(3*3) */
		{
			for (int i=0; i<m_nRow+24; i++)
			{
				double dtemp1 = (i-24>0)?SI_vec(i-24):SI_vec(i+24);//
				dtemp1 += (i-12>0)?SI_vec(i-12)*2:SI_vec(i+12)*2;
				double dtemp2 = SI_vec(i)*3;
				double dtemp3 = (i+24<m_nRow+24)?SI_vec(i+24)*2:SI_vec(i-24)*2;
				dtemp3 += (i+12<m_nRow+24)?SI_vec(i+12):SI_vec(i-12);
				retvec(i) = (dtemp1+dtemp2+dtemp3)/9;
			}
			break;
		}
	case 1:   /*  季节滑动(3*5) */
		{
			for (int i=0; i<m_nRow+24; i++)
			{
				double dtemp1 = (i-36>0)?SI_vec(i-36):SI_vec(i+36);
				dtemp1 += (i-24>0)?SI_vec(i-24)*2:SI_vec(i+24)*2;
				dtemp1 += (i-12>0)?SI_vec(i-12)*3:SI_vec(i+12)*3;
				double dtemp2 = SI_vec(i)*3;
				double dtemp3 = (i+12<m_nRow+24)?SI_vec(i+12)*3:SI_vec(i-12)*3;
				dtemp3 += (i+24<m_nRow+24)?SI_vec(i+24)*2:SI_vec(i-24)*2;
				dtemp3 += (i+36<m_nRow+24)?SI_vec(i+36):SI_vec(i-36);
				retvec(i) = (dtemp1+dtemp2+dtemp3)/15;
			}
			break;
		}
	}
	return retvec;
}

int CTimeX12::TrendMethod(CDoubleVector TCI_vec)
{
	int i = 0;
	CDoubleVector CTrend_vec = TrendMove(TCI_vec,13);
	CDoubleVector Irregular_Vec(m_nRow+24,0.0);
	for (i=0; i<m_nRow+24; i++)
	{
		Irregular_Vec(i) = TCI_vec(i)-CTrend_vec(i);
	}
	double TrendAve = 0;
	double IrregularAve = 0;
	for (i=1; i<m_nRow+24; i++)   
	{		
		TrendAve += (CTrend_vec(i)/CTrend_vec(i-1)>0)?(CTrend_vec(i)/CTrend_vec(i-1))/(m_nRow-1):-(CTrend_vec(i)/CTrend_vec(i-1))/(m_nRow-1);
		IrregularAve += (Irregular_Vec(i)/Irregular_Vec(i-1)>0)?(Irregular_Vec(i)/Irregular_Vec(i-1))/(m_nRow-1):-(Irregular_Vec(i)/Irregular_Vec(i-1))/(m_nRow-1);
	}
	int Temp = (IrregularAve/TrendAve<1.0)?9:13;
	return Temp;
}


void CTimeX12::WeightCalcu(int nSpan)
{
	int i = 0;
	CDoubleVector Q_Vec(nSpan,0.0);
	nSpan = int((nSpan-1)/2);
	for (i= -nSpan; i<nSpan+1; i++)
	{
		Q_Vec(i+nSpan) = ((nSpan+1)*(nSpan+1)-i*i)*((nSpan+2)*(nSpan+2)-i*i)*((nSpan+3)*(nSpan+3)-i*i);
	}
	double sum1 = 0;
	double sum2 = 0;
	double sum3 = 0;
	for (i=-nSpan; i<nSpan+1; i++)
	{
		sum1 += Q_Vec(i+nSpan);
		sum2 += Q_Vec(i+nSpan)*i*i;
		sum3 += Q_Vec(i+nSpan)*i*i*i*i;
	}
	double Beta2 = sum2/(sum2*sum2-sum1*sum3);
	double Beta1 = -sum3*Beta2/sum2;
	CDoubleVector  Weight_Vec = Q_Vec.copy();
	for (i=-nSpan; i<nSpan+1; i++)
	{
		Weight_Vec(i+nSpan) = Q_Vec(i+nSpan)*(Beta1+Beta2*i*i);
 	}
	m_Weight_Vec = Weight_Vec.copy();
	return;
}

void CTimeX12::Diagnoses()
{
	m_RatioOrg_Vec.create(m_nRow-1,0.0) ;
	m_RatioTCI_Vec.create(m_nRow-1,0.0);
	int i = 0;
	for (i=0; i<m_nRow-1; i++)
	{
		if (m_nMethod == 0)
		{
			m_RatioOrg_Vec(i) = (m_OrgVec(i+1)-m_OrgVec(i))/m_OrgVec(i);
			m_RatioTCI_Vec(i) = (m_TCI_Vec(i+1)-m_TCI_Vec(i))/m_TCI_Vec(i);
		}
		else
		{
			m_RatioOrg_Vec(i) = m_OrgVec(i+1)-m_OrgVec(i);
			m_RatioTCI_Vec(i) = m_TCI_Vec(i+1)-m_TCI_Vec(i);
		}
	}
	m_AveOrg_Vec.create(12,0.0);
	m_AveTCI_Vec.create(12,0.0);
	m_AveTrend_Vec.create(12,0.0);
	m_AveSeason_Vec.create(12,0.0);
	m_AveIrregular_Vec.create(12,0.0);

	for (int j=0; j<12; j++)
	{
		if (m_nMethod == 0) 
		{
			for (i=j+1; i<m_nRow; i++)
			{
				m_AveOrg_Vec(j) += fabs((m_OrgVec(i)-m_OrgVec(i-j-1))/m_OrgVec(i-j-1));
				m_AveTCI_Vec(j) += fabs((m_TCI_Vec(i)-m_TCI_Vec(i-j-1))/m_TCI_Vec(i-j-1));
				m_AveTrend_Vec(j) += fabs((m_TrendVec(i)-m_TrendVec(i-j-1))/m_TrendVec(i-j-1));
				m_AveSeason_Vec(j) += fabs((m_SeasonVec(i)-m_SeasonVec(i-j-1))/m_SeasonVec(i-j-1));
				m_AveIrregular_Vec(j) += fabs((m_IrregularVec(i)-m_IrregularVec(i-j-1))/m_IrregularVec(i-j-1));
			}
		}
		else
		{
			for (i=j+1; i<m_nRow; i++)
			{
				m_AveOrg_Vec(j) += fabs(m_OrgVec(i)-m_OrgVec(i-j-1));
				m_AveTCI_Vec(j) += fabs(m_TCI_Vec(i)-m_TCI_Vec(i-j-1));
				m_AveTrend_Vec(j) += fabs(m_TrendVec(i)-m_TrendVec(i-j-1));
				m_AveSeason_Vec(j) += fabs(m_SeasonVec(i)-m_SeasonVec(i-j-1));
				m_AveIrregular_Vec(j) += fabs(m_IrregularVec(i)-m_IrregularVec(i-j-1));
			}
		}
		m_AveOrg_Vec(j) = m_AveOrg_Vec(j)*100.0/(m_nRow-j-1);
		m_AveTCI_Vec(j) = m_AveTCI_Vec(j)*100.0/(m_nRow-j-1);
		m_AveTrend_Vec(j) = m_AveTrend_Vec(j)*100.0/(m_nRow-j-1);
		m_AveSeason_Vec(j)= m_AveSeason_Vec(j)*100.0/(m_nRow-j-1);
		m_AveIrregular_Vec(j) = m_AveIrregular_Vec(j)*100.0/(m_nRow-j-1);
	}
	return;
}


void CTimeX12::DrawResult()
{
	
	//////////////**********X-12-ARIMA模型的各因素输出**********//////////
	int i=0;
	CRsltElementTable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle("X-12-ARIMA模型");
	pTable->InsertColumn(0,"日期");
	pTable->InsertColumn(1,"原序列");
	pTable->InsertColumn(2,"调整后序列");
	pTable->InsertColumn(3,"趋势因素");
	pTable->InsertColumn(4,"季节因素");
	pTable->InsertColumn(5,"不规则因素");
	CTStringArray NameArr;
	CTString strTemp;
	for (i=0; i<m_nRow; i++)
	{
		strTemp.Format("%d", i+1);
		NameArr.Add(strTemp);
	}
	for(i=0;i<m_nRow;i++)
	{
		pTable->InsertItem(i, NameArr.GetAt(i));
		pTable->SetItemText(i, 1, m_OrgVec(i) );
		pTable->SetItemText(i, 2, m_TCI_Vec(i));
		pTable->SetItemText(i, 3, m_TrendVec(i));
		pTable->SetItemText(i, 4, m_SeasonVec (i)*100);
		pTable->SetItemText(i, 5, m_IrregularVec(i)*100);
	}	
	pETable= new CRsltElementTable("X-12-ARIMA模型序列",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);



//////////////**********原序列和季节调整序列对比图**********//////////
	int nVLen = m_OrgVec.vlen();
	TDataPointValVal  *pVV    = NULL;
	CTChartLine       *pLine  = NULL;
	CRsltElementChart *pChart = NULL;
	
	pVV = new TDataPointValVal[nVLen];
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_OrgVec[i];
	}
	pLine = new CTChartLine;
	pLine->SetLine(pVV, nVLen, ValVal, "原序列");  
	delete []pVV;
	nVLen = m_TCI_Vec.vlen();
	pVV = new TDataPointValVal[nVLen];
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_TCI_Vec[i];
	}
	pLine->SetLine(pVV, nVLen, ValVal, "季节调整序列");
	pChart = new CRsltElementChart("-原序列和季节调整序列对比图", pLine);
	m_pResult->Add(pChart);  

//////////////**********趋势因素图形输出描述**********//////////
	nVLen = m_TrendVec.vlen();
	pVV = new TDataPointValVal[nVLen];  
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_TrendVec[i];
	}
	pLine = new CTChartLine;
	//点图	
	pLine->SetData(pVV, nVLen, EScatter, ValVal, "趋势因素");
	pLine->SetLine(pVV, nVLen, ValVal,"趋势因素");
	pChart = new CRsltElementChart("-趋势因素图", pLine);
	m_pResult->Add(pChart); 
	delete []pVV;
	pVV = NULL;

//////////////**********季节因素图形输出描述**********//////////
	nVLen = m_SeasonVec.vlen();
	pVV = new TDataPointValVal[nVLen];  
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_SeasonVec[i];
	}
	pLine = new CTChartLine;
	pLine->SetLine(pVV, nVLen, ValVal,"季节因素");
	//点图	
	pLine->SetData(pVV, nVLen, EScatter, ValVal, "季节因素");
	pChart = new CRsltElementChart("-季节因素图", pLine);
	m_pResult->Add(pChart); 
	delete []pVV;
	pVV = NULL;

//////////////**********不规则因素图形输出描述**********//////////
	nVLen = m_IrregularVec.vlen();
	pVV = new TDataPointValVal[nVLen];  
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_IrregularVec[i];
	}
	pLine = new CTChartLine;
	pLine->SetLine(pVV, nVLen, ValVal,"不规则因素");
	//点图	
	pLine->SetData(pVV, nVLen, EScatter, ValVal, "不规则因素");
	pChart = new CRsltElementChart("-不规则因素图", pLine);
	m_pResult->Add(pChart); 
	delete []pVV;
	pVV = NULL;

//////////////**********对比图**********//////////
	nVLen = m_RatioOrg_Vec.vlen();
	pVV = new TDataPointValVal[nVLen];
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_RatioOrg_Vec[i];
	}
	pLine = new CTChartLine;
	pLine->SetLine(pVV, nVLen, ValVal, "原序列变化率");  
	delete []pVV;
	nVLen =m_RatioTCI_Vec.vlen();
	pVV = new TDataPointValVal[nVLen];
	for(i=0; i<nVLen; i++)
	{
		pVV[i].fXVal = i+1;
		pVV[i].fYVal = m_RatioTCI_Vec[i];
	}
	pLine->SetData(pVV, nVLen, EScatter, ValVal, "季节调整后序列的变化率");
	pChart = new CRsltElementChart("-原序列和季节调整后序列的变化率对比图", pLine);
	m_pResult->Add(pChart);  
	delete []pVV;
	pVV = NULL;
	
//////////////**********各要素的特性值**********//////////
	pETable = NULL;
	pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	pTable->SetTitle("各要素的特性值");
	pTable->InsertColumn(0,"间隔月份");
	pTable->InsertColumn(1,"原序列特性值");
	pTable->InsertColumn(2,"调整序列特性值");
	pTable->InsertColumn(3,"趋势因素特性值");
	pTable->InsertColumn(4,"季节因素特性值");
	pTable->InsertColumn(5,"不规则因素特性值");
	for (i=0; i<m_nRow; i++)
	{
		strTemp.Format("%d", i+1);
		NameArr.Add(strTemp);
	}
	for(i=0; i<m_AveOrg_Vec.vlen(); i++)
	{
		pTable->InsertItem(i, NameArr.GetAt(i));
		pTable->SetItemText(i, 1, m_AveOrg_Vec(i) );
		pTable->SetItemText(i, 2, m_AveTCI_Vec(i));
		pTable->SetItemText(i, 3, m_AveTrend_Vec(i));
		pTable->SetItemText(i, 4, m_AveSeason_Vec(i));
		pTable->SetItemText(i, 5, m_AveIrregular_Vec(i));
	}	
	pETable= new CRsltElementTable("各要素的特性值",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

	//////////////**********X-12-ARIMA模型预测结果表**********//////////
	if (m_bpredict)
	{
		pETable = NULL;
		pTable = new CTLTable;
		pTable->CreateTable(2, 2);
		pTable->SetTitle("X-12-ARIMA模型预测结果");
		pTable->InsertColumn(0,"日期");
		pTable->InsertColumn(1,"预测值");
		
		
		for (i=0; i<m_prestep; i++)
		{
			strTemp.Format("%d", i+1+m_nRow);
			NameArr.Add(strTemp);
		}
		for(i=0;i<m_prestep;i++)
		{
			pTable->InsertItem(i, NameArr.GetAt(i));
			pTable->SetItemText(i, 1, m_predict_Vec(i) );
			
		}	
		pETable= new CRsltElementTable("X-12-ARIMA模型预测结果",pTable);//申请一个表对象类的对象
		m_pResult->Add(pETable);
	}
	
///模型预测图形////
	if (m_prestep > 0)
	{
		TDataPointValVal* pOrgVV;
		TDataPointValVal* pPredVV;
		pOrgVV = new TDataPointValVal[m_OrgVec.vlen()];
		pPredVV = new TDataPointValVal[m_prestep+1];
		int j;
		for(j=0; j<m_OrgVec.vlen(); j++)
		{
			pOrgVV[j].fXVal = j+1;
			pOrgVV[j].fYVal = m_OrgVec[j];
		}  
		pPredVV[0].fXVal = j;
		pPredVV[0].fYVal = m_OrgVec[j-1];
		for(int i=0;i<m_prestep;i++)
		{
			pPredVV[i+1].fXVal =j+i+1;
			pPredVV[i+1].fYVal =m_predict_Vec[i];
		}
		CTChartLine *pTChartLine = new CTChartLine();
		pTChartLine->SetLine(pOrgVV, m_OrgVec.vlen(), ValVal,"原始数据"); 
		pTChartLine->SetLine(pPredVV, m_prestep, ValVal,"预测数据"); 
		CRsltElementChart *pREC = new CRsltElementChart("-预测效果图",  pTChartLine);
		m_pResult->Add(pREC);    
	}

	return;
}


