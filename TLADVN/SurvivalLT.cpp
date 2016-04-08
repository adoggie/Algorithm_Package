/* 
* Copyright (c) 2002,上海天律信息技术有限公司信息分析部
* All rights reserved.
*
* 文件名称：SurvivalLT.cpp
* 文件标示：见配置管理计划书
* 摘    要：实现生存表
*
* 当前版本：1.0
* 作    者：孔祥钧
* 完成日期：2002-11-27
*/
#include "StdAfx.h"
// #include "tlAdvn.h"
#include "SurvivalLT.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

/*
* 构造函数和析构函数
*/
CSurvivalLT::CSurvivalLT()
{
	m_ptLiftTbl = NULL;
	m_bBeyond = false;

	////////////////////////////////////////////////////////////
	// 添加：屈韬
	// 目的：初始化需用户设置的成员变量
	m_nTimeField		= -1;
	m_nStatusField		= -1;
	m_bSingleEventValue = true;
	m_dSingleValue		= 0;
	m_dRangeFrom		= 0;
	m_dRangeTo			= 0;
	m_dMaxTime			= 0;
	m_dStep				= 0;
	m_pDataInterface	= NULL;
	//
	////////////////////////////////////////////////////////////
}

CSurvivalLT::~CSurvivalLT()
{
}

/*
* 从对话框中传递过来的参数
*/

CResult * CSurvivalLT::OnSurvivalLT(CDataInterface * pDataInterface, string VO)
{
	assert( pDataInterface!=NULL );

	m_pDataInterface = pDataInterface;
	m_pResult = NULL; //防止第一次正确运行后,第二条件不满足时错误! zlq
	if(GetValueFromVO(pDataInterface, VO) && Initialize())
		m_pResult = Analyze();
	return m_pResult;
}

bool CSurvivalLT::GetValueFromVO(CDataInterface * pDataInterface, string VO)
{
//	SURVIVALLT
//		/DATA=[FILE]
//		/TIMEVAR [varlist]
//		/STATUSVAR [varlist]
//		/WEIGHT [varlist]
//		/CRITERIA=[MAXTIME{(**)}] [STEPLENTH{(**)}] [{EVENT(**)},{RANGE{**,**}}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/TIMEVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	CTString yname = ynames.GetAt(0);
	m_nTimeField = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();
	
	AnaSyntax::getVAR("/STATUSVAR", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return FALSE;
	yname = ynames.GetAt(0);
	m_nStatusField = pDataInterface->IndexByName(yname);
	ynames.RemoveAll();

	m_bSingleEventValue = AnaSyntax::findstr("/CRITERIA","EVENT",VOstr);
	if (m_bSingleEventValue) 
		m_dSingleValue = AnaSyntax::getvalue("/CRITERIA","EVENT",VOstr);
	else if (AnaSyntax::findstr("/CRITERIA","RANGE",VOstr))
	{
		m_dRangeFrom = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr);
		m_dRangeTo = AnaSyntax::getvalue("/CRITERIA", "RANGE", VOstr, 2);
	}
	else 
		return FALSE;

	m_dMaxTime = AnaSyntax::getvalue("/CRITERIA", "MAXTIME", VOstr);
	m_dStep = AnaSyntax::getvalue("/CRITERIA", "STEPLENTH", VOstr);

	return TRUE;
}

/*
* 生命表操作初始化：计算出两个表需要的参数
*/
bool CSurvivalLT::Initialize(void)
{
	//需要的局部变量
	int i, nSum = 0, nMaxPeriod = 0;
	int	nWeightNum = m_pDataInterface->GetWeightNumb();		//得到权重列
	int nRowCount = m_pDataInterface->GetRowCount();			//得到处理次数
	CField * pFieldTime = NULL, * pFieldWeight = NULL, *pFieldStatus = NULL;

	// 计算寿命表数组大小
	double dPeriodNum = m_dMaxTime / m_dStep;
	m_nPeriodNum = (int)dPeriodNum;
	if( dPeriodNum > m_nPeriodNum )
		m_nPeriodNum++; // 增加一个最大时间段

	m_nPeriodNum++; // 加上0时间段

//	MEMORYSTATUS ms;
//	GlobalMemoryStatus( &ms );


	m_ptLiftTbl	= new TLiftTbl[m_nPeriodNum];				//分配内存放置结果值
	for( i=0; i<m_nPeriodNum; i++ )
		m_ptLiftTbl[i].dIntStartTime = i * m_dStep;

	if( m_ptLiftTbl[m_nPeriodNum-1].dIntStartTime>m_dMaxTime )
		m_ptLiftTbl[m_nPeriodNum-1].dIntStartTime = m_dMaxTime;

	//得到一个数据库对象指针
	CDataAccess  &rDataAccess = m_pDataInterface->m_DataAccess;
	//移到表记录第一行
	rDataAccess.First();

	while ( !rDataAccess.Eof() )
	{
		double dTime, dWeight;
		pFieldTime = rDataAccess.m_pFieldList->FieldByIndex( m_nTimeField );
		pFieldStatus = rDataAccess.m_pFieldList->FieldByIndex( m_nStatusField );

		if( m_pDataInterface->IsMissingValue( pFieldTime, true ) || m_pDataInterface->IsMissingValue( pFieldStatus, true ) )
		{
			rDataAccess.Next();
			continue;
		}
		// 判断该记录属于哪个时间段
		dTime = pFieldTime->GetAsDouble();


		if( nWeightNum>=0 ) // 设置了权重
		{
			pFieldWeight = rDataAccess.m_pFieldList->FieldByIndex( nWeightNum );
			if( m_pDataInterface->IsMissingValue( pFieldWeight, true ) )
				dWeight = 0;
			else
			{
				dWeight = pFieldWeight->GetAsDouble();
				if( dWeight!=int(dWeight) )
					dWeight = 0;
			}

			nSum += (int)dWeight;
		}
		else
			nSum++;

		int nPeriod;
		if( dTime>=0 )
		{
			if( dTime>=m_dMaxTime )
			{
				if( !m_bBeyond )
					m_bBeyond = true;
			}
			int k=0; 
			for( k=0; k<m_nPeriodNum; k++ )
				if( dTime<m_ptLiftTbl[k].dIntStartTime )
					break;
			nPeriod = k-1;

			if( nPeriod>nMaxPeriod )
				nMaxPeriod = nPeriod;

			if( m_bSingleEventValue) // 使用单一状态判断值
			{
				if( pFieldStatus->GetAsDouble()==m_dSingleValue ) // 状态为真
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dTermnlEvents += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dTermnlEvents += (int)dWeight;
					}
				}
				else	// 状态为假
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dDuringIntrvl += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dDuringIntrvl += (int)dWeight;
					}
				}
			}
			else	// 使用范围状态判断值
			{
				if( pFieldStatus->GetAsDouble()>=m_dRangeFrom && 
							pFieldStatus->GetAsDouble()<=m_dRangeTo ) // 状态为真
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dTermnlEvents += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;
						
						m_ptLiftTbl[nPeriod].dTermnlEvents += (int)dWeight;
					}
				}
				else	// 状态为假
				{
					if( pFieldWeight==NULL )
						m_ptLiftTbl[nPeriod].dDuringIntrvl += 1;
					else
					{
						if( dWeight!=int(dWeight) )
							dWeight = 0;

						m_ptLiftTbl[nPeriod].dDuringIntrvl += (int)dWeight;
					}
				}
			}
		}

		rDataAccess.Next();
	} // End of While

	m_nPeriodNum = nMaxPeriod + 1;

	m_ptLiftTbl[0].dThisIntrvl = nSum;

	return true;
}	

/*
* 生成生命表：以表形式输出两个表
*/
CResult * CSurvivalLT::Analyze(void)
{
	int i, j;
	double dTemp;

	m_ptLiftTbl[0].dToRisk = m_ptLiftTbl[0].dThisIntrvl - (m_ptLiftTbl[0].dDuringIntrvl / 2);
	m_ptLiftTbl[0].dTerminating	= m_ptLiftTbl[0].dTermnlEvents / m_ptLiftTbl[0].dToRisk; 
	m_ptLiftTbl[0].dSurviving = 1 - m_ptLiftTbl[0].dTerminating;
	m_ptLiftTbl[0].dSurvAtEnd = m_ptLiftTbl[0].dSurviving;
	m_ptLiftTbl[0].dDensty = (1 - m_ptLiftTbl[0].dSurvAtEnd) / m_dStep;
	m_ptLiftTbl[0].dRate = (2 * m_ptLiftTbl[0].dTerminating) / (m_dStep * (1 + m_ptLiftTbl[0].dSurviving));

	for ( i=1; i<m_nPeriodNum; i++)
	{
		m_ptLiftTbl[i].dThisIntrvl = m_ptLiftTbl[i-1].dThisIntrvl - m_ptLiftTbl[i-1].dDuringIntrvl - m_ptLiftTbl[i-1].dTermnlEvents;
		m_ptLiftTbl[i].dToRisk = m_ptLiftTbl[i].dThisIntrvl - (m_ptLiftTbl[i].dDuringIntrvl / 2);
		m_ptLiftTbl[i].dTerminating	= m_ptLiftTbl[i].dTermnlEvents / m_ptLiftTbl[i].dToRisk; 
		m_ptLiftTbl[i].dSurviving = 1 - m_ptLiftTbl[i].dTerminating;
		m_ptLiftTbl[i].dSurvAtEnd = m_ptLiftTbl[i-1].dSurvAtEnd * m_ptLiftTbl[i].dSurviving;
		m_ptLiftTbl[i].dDensty = (m_ptLiftTbl[i-1].dSurvAtEnd - m_ptLiftTbl[i].dSurvAtEnd) / m_dStep;
		m_ptLiftTbl[i].dRate = (2 * m_ptLiftTbl[i].dTerminating) / (m_dStep * (1 + m_ptLiftTbl[i].dSurviving));
	}

	// 计算 SECS、SEPD、SEHR
	for( i=0; i<m_nPeriodNum; i++ )
	{
		dTemp = 0;
		for( j=0; j<i+1; j++ )
		{
			if( fabs(m_ptLiftTbl[j].dToRisk)<1e-10 || fabs(m_ptLiftTbl[j].dSurviving)<1e-10 )
			{
				dTemp = 0;
				break;
			}
			dTemp += m_ptLiftTbl[j].dTerminating / ( m_ptLiftTbl[j].dToRisk * m_ptLiftTbl[j].dSurviving );
		}
		m_ptLiftTbl[i].dSecs = m_ptLiftTbl[i].dSurvAtEnd * sqrt( dTemp );

		if (fabs(m_ptLiftTbl[i].dTerminating)<1e-10)
			m_ptLiftTbl[i].dSehr =0.0;
		else
		{
			double lamda = 2 * m_ptLiftTbl[i].dTerminating / (m_dStep * (1 + m_ptLiftTbl[i].dSurviving));
			m_ptLiftTbl[i].dSehr = sqrt(lamda * lamda * (1 - ((lamda * m_dStep) / 2)) *
								(1 + (lamda * m_dStep)/2) / (m_ptLiftTbl[i].dToRisk * 
								m_ptLiftTbl[i].dTerminating));
		}
	}

	if( fabs(m_ptLiftTbl[0].dTerminating)<1e-10 )
		m_ptLiftTbl[0].dSepd = 0;
	else
		m_ptLiftTbl[0].dSepd = m_ptLiftTbl[0].dTerminating / m_dStep *
							sqrt(m_ptLiftTbl[0].dSurviving / (m_ptLiftTbl[0].dToRisk * m_ptLiftTbl[0].dTerminating));
	for( i=1; i<m_nPeriodNum; i++ )
	{
		dTemp = 0;
		int j=0; 
		for (j=0; j<i; j++)
			dTemp += m_ptLiftTbl[j].dTerminating / ( m_ptLiftTbl[j].dToRisk * m_ptLiftTbl[j].dSurviving );

		if( fabs(m_ptLiftTbl[i].dTerminating)<1e-10 )
			m_ptLiftTbl[i].dSepd = 0;
		else
		{
			dTemp += m_ptLiftTbl[i].dSurviving / (m_ptLiftTbl[i].dToRisk * m_ptLiftTbl[i].dTerminating);

			m_ptLiftTbl[i].dSepd = (m_ptLiftTbl[i-1].dSurvAtEnd * m_ptLiftTbl[i].dTerminating * 
									 sqrt(dTemp)) / m_dStep;
		}
	}

	return OutputTable();
}

CResult * CSurvivalLT::OutputTable()
{
	int i, nRow;

	m_pResult = new CResult("生存分析");

	for( i=0; i<m_nPeriodNum; i++ )
	{
		if( m_ptLiftTbl[i].dThisIntrvl==0 )
			break;
	}
	
	if( i<2 )
	{
		CTString	strWarning;
		strWarning.Format( " 注：无有效的合法的数据输入，无法输出寿命表！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	else
		nRow = i;
	///////////////输出第一个表///////////////
	CRsltVariable * pETable = NULL;
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("寿命表");
// 	pTable->CreateTable(nRow+1,13);
//	pTable->SetCols(13);
//	pTable->SetRows(nRow + 1);

		
	pTable->InsertColumn(0, "时段下限");
	pTable->InsertColumn(1, "时段观察\n例数");	
	pTable->InsertColumn(2, "时段删失\n例数");	
	pTable->InsertColumn(3, "有效观察\n例数");	
	pTable->InsertColumn(4, "终结事件\n例数");	
	pTable->InsertColumn(5, "终结事件\n比例");	
	pTable->InsertColumn(6, "生存事件\n比例");	
	pTable->InsertColumn(7, "累积\n生存率");	
	pTable->InsertColumn(8, "概率密度");	
	pTable->InsertColumn(9, "风险率");	
	pTable->InsertColumn(10, "累积\n生存率\n的标准差");	
	pTable->InsertColumn(11, "概率密度\n的标准差");	
	pTable->InsertColumn(12, "风险率\n的标准差");
	
	CTString str;
	for( i=0; i<nRow; i++ )
	{
		if( i==nRow-1 && m_bBeyond )
			str.Format( "%g +", m_ptLiftTbl[i].dIntStartTime );
		else
			str.Format( "%g", m_ptLiftTbl[i].dIntStartTime );
		pTable->InsertItem( i, str );

		pTable->SetItemText( i, 1, m_ptLiftTbl[i].dThisIntrvl );
		pTable->SetItemText( i, 2, m_ptLiftTbl[i].dDuringIntrvl );
		pTable->SetItemText( i, 3, m_ptLiftTbl[i].dToRisk );
		pTable->SetItemText( i, 4, m_ptLiftTbl[i].dTermnlEvents );
		pTable->SetItemText( i, 5, m_ptLiftTbl[i].dTerminating );
		pTable->SetItemText( i, 6, m_ptLiftTbl[i].dSurviving );
		pTable->SetItemText( i, 7, m_ptLiftTbl[i].dSurvAtEnd );
		pTable->SetItemText( i, 8, m_ptLiftTbl[i].dDensty );
		pTable->SetItemText( i, 9, m_ptLiftTbl[i].dRate );
		pTable->SetItemText( i, 10, m_ptLiftTbl[i].dSecs );
		pTable->SetItemText( i, 11, m_ptLiftTbl[i].dSepd );
		pTable->SetItemText( i, 12, m_ptLiftTbl[i].dSehr );
	}

	if( m_bBeyond )
	{
		str.Format( "**" );
		pTable->SetItemText( m_nPeriodNum-1, 8, str );
		pTable->SetItemText( m_nPeriodNum-1, 9, str );
		pTable->SetItemText( m_nPeriodNum-1, 11, str );
		pTable->SetItemText( m_nPeriodNum-1, 12, str );
		str.Format( " **  最后时间段的计算无意义" );
		pTable->SetTail( str );
	}

	pETable= new CRsltElementTable("寿命表",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

	delete[] m_ptLiftTbl;
	m_ptLiftTbl = NULL;

	return m_pResult;
}
