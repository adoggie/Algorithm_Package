// RegressionLinear.cpp: implementation of the CRegressionLinear class.
//
//重新编写:马良庄          改编写时间：05.11.25
//文件的用途：线性回归
//////////////////////////////////////////////////////////////////////

#include "math.h"
#include "StdAfx.h"
#include "tlAdvn.h"
#include "RegressionLinear.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"
//#include "..\TLCRNT\Script.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegressionLinear::CRegressionLinear()
{
	m_nDependIndex  = -1;
	m_pDataInterface= NULL;
	m_nMethod = 0;
	m_nMissMethod  = 0;
	m_dConfidence=0.95;
	m_dTest=0.05;
	m_fEntry = 0.05;
	m_fRemoval = 0.1;
	m_bPredict=false;
	m_bResidue=false;
	m_bCoefInter=false;
	m_bChartLine=false;//拟合图
	m_bChartScatterPlot=false;// 残差散点图
	m_bChartScatterPlot2=false; // 残差正态概率图
	m_bWithConst = true;
	m_bCorr = false; //不输出相关矩阵
	m_bWithColl = false; // 不输出线性性诊断
}

CRegressionLinear::~CRegressionLinear()
{
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
}


double CRegressionLinear::GetQValue(CDoubleMatrix matrix, CDoubleMatrix matrix_Y)//计算Q值

{
	CDoubleMatrix matrix_Beta = inv( (CDoubleMatrix)( trans(matrix) * matrix )) * (trans(matrix) * matrix_Y);
	CDoubleMatrix matrix_temp = trans(matrix_Y) * matrix_Y - (trans(matrix_Y) * matrix) * matrix_Beta;
	return matrix_temp(0,0);
}


void  CRegressionLinear::GetData(CDataInterface *pDataInterface, CDoubleMatrix& Matrix_Data)//读数，输出矩阵[（weight）Xy]，自己不判断加权。
{
	//取回数据，组织输入数据矩阵
	int i,m = m_nXCol+1;
	int nColNum=0;
	CDWordArray arCol;
	if(m_bWithWeight)//添加权重变量下标
        arCol.Add(pDataInterface->GetWeightNumb());
	for(i=0;i<m_nXCol;i++)//添加自变量下标
	{
		nColNum = m_tVarInfo.pValue[i];
		arCol.Add(nColNum);
	}
	if(m_nDependIndex >= 0)//添加因变量下标
		arCol.Add(m_nDependIndex);
	int m_nXRow = pDataInterface->GetColsData(arCol,Matrix_Data,m_nMissMethod);//取回数据
	int nR = Matrix_Data.mrow();
	int nC = Matrix_Data.mcol();
}

bool CRegressionLinear::GetValueFromVo(CDataInterface * pDataInterface, string VO)
{
	//REGLINEAR
	//	/DATA=[FILE]
	//	/DEPENDENT [varlist]
	//	/INDEPENDENT [varlist]
	//	/MISSING=[{LISTWISE**},{MEANSUBSTITUTION}]
	//	/METHOD=[{ENTER**},{BACKWARD},{FORWARD},{STEPWISE}] [NOCONST]
	//	/CRITERIA=[PIN({0.05**})] [POUT({0.1**})] [CIN({.95**})]
	//	/WEIGHT [varlist]
	//	/PLOT [CHARTLINE] [CHARTSCATTER] [CHARTNORMALSCA]
	//	/PRINT [RESIDUAL] [PREDECT]
	//	/RESULT [CORRELATION] [COLINEARITY]
	//	/SAVEMODEL [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if ((ynames.GetSize())!=1) 
		return false;
	CTString yname = ynames.GetAt(0);
	m_nDependIndex = pDataInterface->IndexByName(yname);
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;
	
	m_nMissMethod = AnaSyntax::findstr("/MISSING","MEANSUBSTITUTION",VOstr);
	
	if (AnaSyntax::findstr("/METHOD", "BACKWARD",VOstr))
		m_nMethod = 1;
	else if (AnaSyntax::findstr("/METHOD", "FORWARD",VOstr))
		m_nMethod = 2;
	else if (AnaSyntax::findstr("/METHOD", "STEPWISE",VOstr))
		m_nMethod = 3;
	else 
		m_nMethod = 0;
	
	m_fEntry = AnaSyntax::getvalue("/CRITERIA","PIN",VOstr);
	m_fRemoval = AnaSyntax::getvalue("/CRITERIA","POUT",VOstr);
	m_bCoefInter = AnaSyntax::findstr("/CRITERIA","CIN",VOstr);
	m_dConfidence = AnaSyntax::getvalue("/CRITERIA","CIN",VOstr);
	if (m_fEntry >1 || m_fEntry <0)
		m_fEntry = 0.05;
	if (m_fRemoval>1 || m_fRemoval<0)
		m_fRemoval = 0.1;
	if(m_dConfidence>1 || m_dConfidence<0)
	{
		m_bCoefInter = false;
		m_dConfidence = 0.95;
	}

	m_bWithWeight = AnaSyntax::exist("/WEIGHT", VOstr);
	if (m_bWithWeight)
	{
		CTStringArray ynames;
		AnaSyntax::getVAR("/WEIGHT", VOstr, ynames);
		if ((ynames.GetSize())!=1) 
			return false;
		CTString yname = ynames.GetAt(0);
		m_nWeightID = pDataInterface->IndexByName(yname);
	}
	
	m_bWithConst = !AnaSyntax::findstr("/METHOD", "NOCONST",VOstr);
	
	m_bChartLine = AnaSyntax::findstr("/PLOT","CHARTLINE",VOstr);
	m_bChartScatterPlot = AnaSyntax::findstr("/PLOT","CHARTSCATTER",VOstr);
	m_bChartScatterPlot2 = AnaSyntax::findstr("/PLOT","CHARTNORMALSCA",VOstr);
	
	m_bPredict = AnaSyntax::findstr("/PRINT","PREDECT",VOstr);
	m_bResidue = AnaSyntax::findstr("/PRINT","RESIDUAL",VOstr);
	m_bWithColl = AnaSyntax::findstr("/RESULT","COLINEARITY",VOstr);
	m_bCorr = AnaSyntax::findstr("/RESULT","CORRELATION",VOstr);
	//	/SAVEMODEL [FILE]
	m_bOutput = true;//AnaSyntax::exist("/OUTPUTMODLE", VOstr);
	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL", VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL", VOstr);

	return true;
}

CResult * CRegressionLinear::OnStepWise(CDataInterface *pDataInterface, string VO)
{	
//	CTimer tTemp;
	InitParma();
	m_pDataInterface = pDataInterface;
	CResult * pResult = NULL;
	m_nMethod = 0;
	switch(m_nMethod) 
	{
	case 0:
		{
			pResult = new CResult("线性回归分析结果 (全部选入法)");
			break;
		}
	case 1:
		{
			pResult = new CResult("线性回归分析结果 (向后法)");
			break;
		}
	case 2:
		{
			pResult = new CResult("线性回归分析结果 （向前法)");
			break;
		}
	case 3:
		{
			pResult = new CResult("线性回归分析结果 (逐步法)");
			break;
		}
	}
	m_pResult = pResult;
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}

	briefrTable=NULL;
	m_strX_nameAry.RemoveAll();	
	m_stry_name="";
	
	bool WithConst = m_bWithConst;
	int i;
	m_nXCol = m_tVarInfo.iCount;
	
	if( pDataInterface->GetWeightNumb()>=0 )
		m_bWithWeight = true;
	else m_bWithWeight = false;
	
	CDoubleMatrix data_read;
	GetData(pDataInterface, data_read);//读取数据，得到[（weight）Xy]的矩阵
	CDoubleMatrix data_read_resi = NULL;
	if (m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)
		data_read_resi = copy(data_read);
	m_nXRow = data_read.mrow();
	m_stry_name = pDataInterface->GetFieldName(m_nDependIndex);
	m_strX_nameAry.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_strX_nameAry.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	m_strX_org.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_strX_org.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	CDoubleVector XyMean(m_nXCol+1);
	//开始对数据进行分析处理。
	if (m_bWithWeight)//有权重时，对X，y都乘以权重的平方根
	{
		sum_weight = sum(data_read(0));
		data_read(0) = sqrt(data_read(0));
		for (i=0;i<m_nXCol+1;i++)
		{
			data_read(i+1)= data_read(i+1)*data_read(0);
			XyMean(i) = data_read(i+1).dot(data_read(0))/sum_weight;
		}
	}
	else
		XyMean = mean(data_read);
	CDoubleMatrix m_XyTimesXy;//不包含常数的Xy‘×Xy。
	CDoubleMatrix XyTimesXy_con(m_nXCol+1+m_bWithConst,m_nXCol+1+m_bWithConst);//包括常数项（没有常数也就没有）。
	if (m_bWithWeight) 
	{
		XyTimesXy_con = dot(data_read,data_read);
		m_XyTimesXy = XyTimesXy_con.slice(1,1,m_nXCol+1,m_nXCol+1);
	}
	else
	{
		m_XyTimesXy = dot(data_read,data_read);
		if (m_bWithConst) 
		{
			for (i=0; i<m_nXCol+1; i++)
			{
				for (int j=0; j<m_nXCol+1; j++)
					XyTimesXy_con(i+1,j+1) = m_XyTimesXy(i,j);
				XyTimesXy_con(0,i+1) = XyMean(i+m_bWithWeight)*m_nXRow;
				XyTimesXy_con(i+1,0) = XyMean(i+m_bWithWeight)*m_nXRow;
			}
			XyTimesXy_con(0,0) = m_nXRow;
		}
	}
	if (!m_bWithConst) 
	{
		XyTimesXy_con = m_XyTimesXy;
	}
	
	CDoubleMatrix XyMean_matrix = VectorToMatrix(XyMean,XyMean.vlen(),1);
	CDoubleMatrix Xy_Sigma = m_XyTimesXy - ((m_bWithWeight)?sum_weight:((double)m_nXRow)) * (XyMean_matrix * XyMean_matrix.trans() );//中心化后的X'X(无常数)
	m_var_y = Xy_Sigma(m_nXCol,m_nXCol);
	m_ytimesy = m_XyTimesXy(m_nXCol,m_nXCol);
	
	if (m_bWithWeight) 
	{
		for (i=0; i<m_nXRow; i++)
			if (data_read(i,0)<0) 
			{
				CTString strWarning = "注： 权重（频数）出现负数，无法进行带权重的分析。";
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
				pWarningTextRslt->AddString( strWarning );
				m_pResult->Add( pWarningTextRslt );
				return m_pResult;
			}
	}
	
	if( Xy_Sigma(m_nXCol,m_nXCol) < EPS_ACCUR)
	{
		CTString strWarning = "注： 因变量均为常数，无法进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	m_XtimesX = XyTimesXy_con.slice(m_nXCol+m_bWithConst,m_nXCol+m_bWithConst);
	m_Xtimesy = XyTimesXy_con.slice(0,m_nXCol+m_bWithConst,m_nXCol+m_bWithConst,1);
	m_CorrMatrix = Xy_Sigma/sqrt(Multi(diag(Xy_Sigma),diag(Xy_Sigma)));
	int nColTemp = m_nXCol;
	
	for (i=0; i<nColTemp; i++) //检查X为常数的变量
	{
		if (diag(Xy_Sigma)(i) < EPS_ACCUR)
		{
			m_XtimesX = DelMatrix(m_XtimesX,i);//删除第k行和第k列
			m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),i));//删除第k行和第k列
			m_XyTimesXy = DelMatrix(m_XyTimesXy,i);//删除第k行和第k列
			m_CorrMatrix = DelMatrix(m_CorrMatrix,i);
			m_nXCol-=1;
			m_strX_nameAry.ElementAt(i);
			CTString strWarning = "注： 变量 " + m_strX_nameAry[i] + " 为常数，不参加回归分析。";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add(pWarningTextRslt);
		}
	}
	
	if((m_nXRow<=m_nXCol+1+m_bWithConst) || ((sum_weight < (m_nXCol + m_bWithConst))&&m_bWithWeight))
	{
		CTString strWarning = "注： 有效记录数太少或频数总和太小，无法进行回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	
	m_X_temp_nameAry.RemoveAll();
	for (i=0;i< m_tVarInfo.iCount;i++)
		m_X_temp_nameAry.Add(pDataInterface->GetFieldName(*(m_tVarInfo.pValue+i)));
	
	CDoubleMatrix m_EigVector=m_CorrMatrix.slice(m_nXCol,m_nXCol);
	m_EigValue = EigValPlusVec(&m_EigVector, 1);
	while (m_EigValue(m_nXCol-1,0) < EPS_ACCUR)
	{
		m_bWithColl = true;
		CDoubleVector temp_Eig = m_EigVector.slice(0,m_nXCol-1,m_nXCol,1)(0);
		int mm;//得到与其他自变量完全相关的是那个
		temp_Eig = abs(temp_Eig);
		double temp_nouse = temp_Eig.Max(mm);
		CTString strWarning = "注： 变量 " + m_strX_nameAry[mm] + " 与其他自变量之间存在完全线性关系，不参加回归分析。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		m_XtimesX = DelMatrix(m_XtimesX,mm);//删除第k行和第k列
		m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),mm));//删除第k行和第k列
		m_XyTimesXy = DelMatrix(m_XyTimesXy,mm);//删除第k行和第k列
		m_strX_nameAry.RemoveAt(mm);
		m_X_temp_nameAry.RemoveAt(mm);
		m_CorrMatrix = DelMatrix(m_CorrMatrix,mm);
		m_nXCol-=1;
	}
	
	Xmean = XyMean.copy();
	switch (m_nMethod)
	{
	case 0:	//计算变量全部进入的情况
		{
			RegLinear(m_XtimesX, m_Xtimesy, m_CorrMatrix, sum_weight, m_nXCol,m_nXRow,m_bWithConst);
			if (m_bWithColl) 
				Collinearity(m_XtimesX, m_nXCol, m_bWithConst);
			if (m_bWithColl) 
				if (m_nXCol>0) 
					m_Tolerance = mmthSelfReg(m_XtimesX, Xmean, m_bWithConst);
				DrawResult(m_nXCol, m_bWithConst);
				if (m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)
					ResiPlot(data_read_resi, m_bWithConst, m_coefficiency, m_XtimesX, m_MSE);
				break;
		}
	case 1: //向后法剔除变量
		{
			bool conti_back = true;//继续剔除
			CTString strWarning = "";
			CDoubleMatrix CorrMatrix = copy(m_CorrMatrix);
			int XCol = m_nXCol;
			int number = 0;
			m_nLine = 0;
			m_nColl = 0;
			m_bDrawTableHead = false;
			while (conti_back) 
			{
				RegLinear(m_XtimesX, m_Xtimesy, CorrMatrix, sum_weight, XCol, m_nXRow, WithConst);
				if (m_bWithColl) 
					Collinearity(m_XtimesX, XCol, WithConst);
				
				if(!m_bDrawTableHead)
				{
					DrawTableHead(m_nXCol,WithConst);
					m_bDrawTableHead = true;
				}
				if (m_bWithColl) 
					if (XCol>0) 
						m_Tolerance = mmthSelfReg(m_XtimesX, Xmean, WithConst);
					DrawTableBody(XCol, WithConst, number, strWarning);
					
					int k;//影响最小的X为第k个；
					if (((m_vector_Sig.Max(k)) > m_fRemoval) && (m_vector_Sig.vlen()>0)) 
					{
						
						if ((k == 0) && WithConst)
						{
							strWarning = "常数项 对模型影响不显著，被剔除出模型。";
							WithConst = false;
						}
						else
						{
							strWarning = "变量 \"" + m_X_temp_nameAry[k-WithConst] + "\" 对模型影响不显著，被剔除出模型。";
							XCol -= 1;
							CorrMatrix = DelMatrix(CorrMatrix,k-WithConst);
							m_X_temp_nameAry.RemoveAt(k-WithConst);
							Xmean = DelVector(Xmean,k-WithConst);
						}
						m_XtimesX = DelMatrix(m_XtimesX,k);//删除第k行和第k列
						m_Xtimesy = VectorToMatrix(DelVector(m_Xtimesy(0),k));//删除第k行和第k列
						if (XCol <= 0) //只包括常数项，没有自变量东西了！（表达式有错）
						{
							conti_back = false;	
							strWarning = "警告： 只有常数项进入回归，不输出回归分析结果。";
							CTString str;
							str.Format("%d", ++number);
							pTable1->InsertItem(number, str);
							pTable1->SetItemText(number, 1, strWarning);
						}
					}
					else
						conti_back = false;
					
			}
			if(m_bDrawTableHead) 
				DrawTable(XCol, WithConst);
			if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&(XCol>0))
				ResiPlot(data_read_resi, WithConst, m_coefficiency, m_XtimesX, m_MSE);
			break;
		}
	case 2: //向前法选入变量
		{
			CDoubleMatrix XtimesX;
			CDoubleVector coefficiency;
			double MSE;
			CDoubleMatrix temp_XX = NULL;
			CDoubleVector whichoneisin(m_nXCol+m_bWithConst,-1);
			bool conti_forew = true;//继续选入
			int total_ColXandConst = 0;
			WithConst = false;
			bool WithConst1 = false;
			CTString strWarning = "";				
			int number = 0;
			m_nLine = 0;
			m_nColl = 0;
			bool m_bDrawTableHead = false;
			while (conti_forew) 
			{
				XtimesX = copy(temp_XX);
				WithConst1 = WithConst;
				coefficiency = copy(m_coefficiency);
				MSE = m_MSE;
				total_ColXandConst++;
				if ((whichoneisin(0)>0) && (m_bWithConst)) 
					WithConst = true;
				else 
					WithConst = false;
				int mm;
				CDoubleVector temp_compare(m_nXCol+m_bWithConst,1/DoubleZero);
				for (i=0; i<m_nXCol+m_bWithConst; i++)
				{
					if (whichoneisin(i)<0) 
					{
						if ((i==0) && (m_bWithConst))
							WithConst = true;
						CDoubleVector trynewcomemin = whichoneisin.copy();
						trynewcomemin(i) = 1;
						temp_XX = SelectMatrix(trynewcomemin, m_XtimesX, total_ColXandConst);
						CDoubleVector temp_includey(m_nXCol+1);
						for (int j=0; j<m_nXCol; j++)
							temp_includey(j) = trynewcomemin(j+m_bWithConst);
						temp_includey(m_nXCol)=1;
						CDoubleMatrix temp_Xy = VectorToMatrix(SelectVector(trynewcomemin, m_Xtimesy(0), total_ColXandConst));
						CDoubleMatrix temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
						RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, (total_ColXandConst-WithConst), m_nXRow, WithConst);
						temp_compare(i) = m_MSE;
						if ((i==0) && (m_bWithConst))
							WithConst = false;
					}
				}
				double temp_nouse = temp_compare.Min(mm);
				whichoneisin(mm) = 1;
				if ((whichoneisin(0)>0) && (m_bWithConst))
					WithConst = true;
				int tt=0;
				for (i=0; i< mm; i++)
				{
					if (whichoneisin(i)>0)
						tt++;
				}
				temp_XX = SelectMatrix(whichoneisin, m_XtimesX, total_ColXandConst);
				CDoubleVector temp_includey(m_nXCol+1);
				for (int j=0; j<m_nXCol; j++)
					temp_includey(j) = whichoneisin(j+m_bWithConst);
				temp_includey(m_nXCol)=1;
				CDoubleMatrix temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
				CDoubleMatrix temp_Xy = VectorToMatrix(SelectVector(whichoneisin, m_Xtimesy(0),total_ColXandConst));
				Xmean = SelectVector(whichoneisin, XyMean,total_ColXandConst);
				RegLinear(temp_XX, temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
				if (m_bWithColl) 
					Collinearity(temp_XX, total_ColXandConst-WithConst, WithConst);
				total_ColXandConst--;
				if ((m_vector_Sig(tt)< m_fEntry) && (total_ColXandConst <(m_nXCol + m_bWithConst)))
				{
					if(!m_bDrawTableHead)
					{
						DrawTableHead(m_nXCol,WithConst);
						m_bDrawTableHead = true;
					}
					
					total_ColXandConst++;
					SelectStrAry(whichoneisin, m_strX_nameAry, m_X_temp_nameAry);
					if ((tt == 0) && WithConst) 
						strWarning = "常数项 对模型影响显著，被选入模型。";
					else
						strWarning = "变量 \""+ m_X_temp_nameAry[tt-WithConst]+"\" 对模型影响显著，被选入模型。";
					XtimesX = temp_XX;
					if (m_bWithColl) 
						if ((total_ColXandConst-WithConst)>0) 
							m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
						DrawTableBody(total_ColXandConst-WithConst, WithConst, number, strWarning);
						WithConst1 = WithConst;
				}
				else
				{
					conti_forew = false;
				}
			}
			if ((total_ColXandConst-WithConst)==0 && number) 
			{
				CTString strWarning = "警告： 没有自变量进入，无法输出回归模型分析结果。";
				strWarning = "警告： 只有常数项进入回归，不输出回归分析结果。";
				CTString str;
				str.Format("%d", ++number);
				pTable1->InsertItem(number, str);
				pTable1->SetItemText(number, 1, strWarning);
			}
			if(!m_bDrawTableHead)
			{
				CRsltElementText *pErrorText=new CRsltElementText("警告");
				CTString strError="没有自变量进入，无法输出任何回归模型分析";
				pErrorText->AddString(strError);
				m_pResult->Add(pErrorText);
			}
			if(m_bDrawTableHead)
				DrawTable(m_nXCol, WithConst);
			m_XtimesX = XtimesX;
			m_MSE = MSE;
			if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&((total_ColXandConst-WithConst)>0))
				ResiPlot(data_read_resi, WithConst1, coefficiency, XtimesX, MSE);
			break;
			}
			case 3: //逐步法选入变量
				{
					CDoubleMatrix XtimesX;
					CDoubleVector coefficiency;
					double MSE;
					CDoubleVector whichoneisin(m_nXCol+m_bWithConst,-1);
					bool conti_forew = true;//继续剔除
					int total_ColXandConst = 0;
					CTString strWarning = "";				
					WithConst = false;
					bool WithConst2 = false;
					int contoftime = 0;
					CDoubleMatrix temp_XX = NULL;
					CDoubleMatrix temp_Xy;
					CDoubleMatrix temp_corr;
					int number = 0;
					m_nLine = 0;
					m_nColl = 0;
					bool m_bDrawTableHead = false;
					while (conti_forew) 
					{
						XtimesX = copy(temp_XX);
						WithConst2 = WithConst;
						coefficiency = copy(m_coefficiency);
						MSE = m_MSE;
						total_ColXandConst++;
						if ((whichoneisin(0)>0) && (m_bWithConst)) 
							WithConst = true;
						else 
							WithConst = false;
						int mm;
						CDoubleVector temp_compare(m_nXCol+m_bWithConst,1/DoubleZero);
						for (i=0; i<m_nXCol+m_bWithConst; i++)
						{
							if (whichoneisin(i)<0) 
							{
								if ((i==0) && (m_bWithConst))
									WithConst = true;
								CDoubleVector trynewcomemin = whichoneisin.copy();
								trynewcomemin(i) = 1;
								temp_XX = SelectMatrix(trynewcomemin, m_XtimesX, total_ColXandConst);
								CDoubleVector temp_includey(m_nXCol+1);
								for (int j=0; j<m_nXCol; j++)
									temp_includey(j) = trynewcomemin(j+m_bWithConst);
								temp_includey(m_nXCol)=1;
								temp_Xy = VectorToMatrix(SelectVector(trynewcomemin, m_Xtimesy(0), total_ColXandConst));
								temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
								RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, (total_ColXandConst-WithConst), m_nXRow, WithConst);
								temp_compare(i) = m_MSE;
								if ((i==0) && (m_bWithConst))
									WithConst = false;
							}
						}
						double temp_nouse = temp_compare.Min(mm);
						whichoneisin(mm) = 1;
						if ((whichoneisin(0)>0) && (m_bWithConst))
							WithConst = true;
						int tt=0;
						for (i=0; i< mm; i++)
						{
							if (whichoneisin(i)>0)
								tt++;
						}
						temp_XX = SelectMatrix(whichoneisin, m_XtimesX, total_ColXandConst);
						CDoubleVector temp_includey(m_nXCol+1);
						for (int j=0; j<m_nXCol; j++)
							temp_includey(j) = whichoneisin(j+m_bWithConst);
						temp_includey(m_nXCol)=1;
						temp_corr = SelectMatrix(temp_includey, m_CorrMatrix, total_ColXandConst+1-WithConst);
						temp_Xy = VectorToMatrix(SelectVector(whichoneisin, m_Xtimesy(0),total_ColXandConst));
						Xmean = SelectVector(whichoneisin, XyMean,total_ColXandConst);
						RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
						if (m_bWithColl) 
							Collinearity(temp_XX, total_ColXandConst-WithConst, WithConst);
						total_ColXandConst--;
						if (m_vector_Sig(tt)< (m_fEntry-DoubleZero*pow(3,contoftime)))
						{
							if(!m_bDrawTableHead)
							{
								DrawTableHead(m_nXCol,WithConst);
								m_bDrawTableHead = true;							
							}
							total_ColXandConst++;
							SelectStrAry(whichoneisin, m_strX_nameAry, m_X_temp_nameAry);
							if ((tt == 0) && WithConst) 
								strWarning = "常数项 对模型影响显著，被选入模型。";
							else
								strWarning = "变量 \""+ m_X_temp_nameAry[tt-WithConst]+"\" 对模型影响显著，被选入模型。";
							XtimesX = temp_XX;
							if (m_bWithColl) 
								if ((total_ColXandConst-WithConst)>0) 
									m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
								DrawTableBody(total_ColXandConst-WithConst, WithConst, number, strWarning);
								WithConst2 = WithConst;
								while (m_vector_Sig.Max(mm)>m_fRemoval) //必须找出mm在原来对应的位置
								{
									if ((mm==0) && WithConst) 
										WithConst = false;
									else
										temp_corr = DelMatrix(temp_corr, mm);
									int j_cont=0;
									for (i=0; i<m_nXCol+m_bWithConst; i++)
									{
										if (whichoneisin(i)>0) 
										{
											j_cont++;
											if (j_cont==mm+1) 
												whichoneisin(i)=-1;
										}
									}
									total_ColXandConst--;
									temp_XX = DelMatrix(temp_XX, mm);
									temp_Xy = VectorToMatrix(DelVector(temp_Xy(0), mm));
									Xmean = DelVector(Xmean, mm);
									RegLinear(temp_XX,temp_Xy, temp_corr, sum_weight, total_ColXandConst-WithConst, m_nXRow, WithConst);
									if (m_bWithColl) 
										Collinearity(m_XtimesX, total_ColXandConst-WithConst, WithConst);
									if (m_bWithColl) 
										if ((total_ColXandConst-WithConst)>0) 
											m_Tolerance = mmthSelfReg(temp_XX, Xmean, WithConst);
										DrawTableBody(total_ColXandConst-WithConst, WithConst,number, strWarning);
										if ((mm == 0) && WithConst) 
											strWarning = "常数项 对模型影响不显著，被剔除模型。";
										else
											strWarning = "变量 \""+ m_X_temp_nameAry[mm-WithConst]+"\" 对模型影响不显著，被剔除模型。";
										m_X_temp_nameAry.RemoveAt(mm);
										XtimesX = temp_XX;
										WithConst2 = WithConst;
										contoftime++;
								}
								if ((total_ColXandConst) == (m_nXCol + m_bWithConst))
								{
									XtimesX = copy(temp_XX);
									coefficiency = copy(m_coefficiency);
									MSE = m_MSE;
									conti_forew = false;
								}
						}
						else
							conti_forew = false;
				}//while continouse
				if ((total_ColXandConst-WithConst)==0 && number) 
				{
					CTString strWarning = "警告： 没有自变量进入，无法输出回归模型分析结果。";
					strWarning = "警告： 只有常数项进入回归，不输出回归分析结果。";
					CTString str;
					str.Format("%d", ++number);
					pTable1->InsertItem(number, str);
					pTable1->SetItemText(number, 1, strWarning);
				}
				if(!m_bDrawTableHead)
				{
					CRsltElementText *pErrorText=new CRsltElementText("警告");
					CTString strError="没有自变量进入，无法输出任何回归模型分析!";
					pErrorText->AddString(strError);
					m_pResult->Add(pErrorText);
				}
				
				if(m_bDrawTableHead)
					DrawTable(m_nXCol, WithConst);
				m_XtimesX = XtimesX;
				m_MSE = MSE;
				if ((m_bChartLine||m_bChartScatterPlot||m_bChartScatterPlot2||m_bPredict||m_bResidue)&&((total_ColXandConst-WithConst)>0))
					ResiPlot(data_read_resi, WithConst2, coefficiency, XtimesX, MSE);
				break;
			}//case 的括号
		}//sweith 的括号
		Export();//保存模型-add by xds 20060815
		if (m_bCorr) 
			DrawCorrolation(Xy_Sigma);
		//tTemp.TotalTime();
		m_bWithColl = false;
		return pResult;
}

bool CRegressionLinear::Export()//输出模型
{
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	
	if (!m_bSaveModel)
		return true;
	bool bOpen = theFile.Open(m_ModelFile.GetData(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += REGRESSIONLINEAR;
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + m_pDataInterface->m_DataAccess.GetFileName() + "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = m_X_temp_nameAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_X_temp_nameAry.GetAt(i);
	}
	//目标变量
	szText += " ";
	szText += VAROUTPUT;
	szText += " " + m_stry_name;
	//日期时间
	SYSTEMTIME st;
	GetLocalTime(&st);
	szTemp.Format("%d年%d月%d日%d时%d分%d秒",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	szText += " ";
	szText += DATETIME;
	szText += " " + szTemp + "\r\n";
	//长度
	nLen = szText.GetLength() + sizeof(int);//总的长度
	theFile << nLen;
	nLen = theFile.WriteString(szText.GetData());
	//开始写模型参数
	theFile.SeekToEnd();
	theFile << m_bWithConst;
	//自变量
	nSize = m_coefficiency.vlen();
	theFile << nSize;
	//系数
	for (i=0; i<nSize; i++)
	{
		theFile << m_coefficiency(i);
	}
	//输入变量
	nSize = m_X_temp_nameAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_X_temp_nameAry.GetAt(i);
	}
	//矩阵
	int nRow = m_XtimesX.mrow();
	int nCol = m_XtimesX.mcol();
	theFile << nRow;
	theFile << nCol;
	for (i=0; i<nRow; i++)
	{
		for (int j=0; j<nCol; j++)
		{
			theFile << m_XtimesX(i, j);
		}
	}
	//目标变量
	theFile << m_stry_name;
	//残差
	theFile << m_MSE;
	theFile.Close();
	return true;
}

void CRegressionLinear::ResiPlot(CDoubleMatrix data_read_resi, bool WithConst, CDoubleVector coefficiency, CDoubleMatrix XtimesX, double MSE)
{
	if ((XtimesX.mrow() <= 0)||(coefficiency.vlen()<=0))
		return;
	double sum_weight;
	if (m_bWithWeight) 
		sum_weight = (data_read_resi(0)).sum();
	else
		sum_weight = m_nXRow;
	double t_value = 0.0;
	if (m_dConfidence > EPS_ACCUR)
		t_value = tinv( (1+m_dConfidence)/2.0, sum_weight -m_X_temp_nameAry.GetSize()-WithConst);
	CDoubleMatrix X_matrix = NULL;
	int j = 0;
	if (WithConst)
	{
		CDoubleVector Ones_v(m_nXRow, 1);
		X_matrix = X_matrix | Ones_v;
	}
	int i;
	for (i=0; i<m_nXCol; i++)
	{
		for (j=0; j<(m_X_temp_nameAry.GetSize()); j++)
		{	
			if (m_strX_nameAry[i] == m_X_temp_nameAry[j]) 
				X_matrix = X_matrix | data_read_resi(i+m_bWithWeight);
		}
	}
	CDoubleVector y_hat = X_matrix * coefficiency;//预测值
	CDoubleVector vect_y = data_read_resi(data_read_resi.mcol()-1);
	CDoubleVector y_resi = vect_y - y_hat;//残差
	CDoubleVector y_range(m_nXRow);
	CDoubleVector y_upper;//预测值的上限
	CDoubleVector y_lower;//预测值的下限
	if (m_bPredict)
	{
		CDoubleMatrix X_trans = X_matrix.trans();
		CDoubleMatrix X_inv = inv(XtimesX);
		for (i=0; i<m_nXRow; i++)
		{
			CDoubleVector vi = X_trans(i);
			y_range(i) = t_value*sqrt(1+((VectorToMatrix(vi).trans())*X_inv*VectorToMatrix(vi))(0,0))*sqrt(MSE);
		}
		y_upper = y_hat + y_range;
		y_lower = y_hat - y_range;
	}
	//在主界面添加残差输出
	if(m_bResidue)
	{
		CTString strFieldName="残差";
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		CField *pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		pField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess .First();
		for(int i = 0;i<m_nXRow;i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			pField->SetAsDouble(y_resi(i));
			//m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}	
	}
	
	if(m_bPredict)////在主界面添加预测值输出
	{
		CTString strFieldName = "预测值", strUpName = "上限预测值", strDownName = "下限预测值";
		
		if (m_pDataInterface->m_DataAccess.FieldByName(strFieldName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strFieldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		if (m_pDataInterface->m_DataAccess.FieldByName(strUpName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strUpName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}
		if (m_pDataInterface->m_DataAccess.FieldByName(strDownName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(strDownName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}
		CField *PredField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble),
			*UpField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble),
			*DownField = m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
	
		PredField->SetFieldName(strFieldName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(PredField);
		
		UpField->SetFieldName(strUpName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(UpField);
		
		DownField->SetFieldName(strDownName);
		m_pDataInterface->m_DataAccess.m_pFieldList->Add(DownField);
		
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess.First();
		for(int i=0; i<m_nXRow; i++)
		{
			m_pDataInterface->m_DataAccess.Edit();			
			PredField->SetAsDouble(y_hat(i));
			UpField->SetAsDouble(y_upper(i));
			DownField->SetAsDouble(y_lower(i));
			//m_pDataInterface->m_DataAccess.Post();			
			m_pDataInterface->m_DataAccess.Next();
		}			
	}
	if (m_bResidue) 
	{
		//残差统计表
		//----------------------------------
		CTLTable *pTable = new CTLTable();
		pTable->SetTitle("残差统计表");
		pTable->SetRows(5);
		pTable->SetCols(7);
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(0,"最小值");
		pTable->InsertColumn(0,"最大值");
		pTable->InsertColumn(0,"均值");
		pTable->InsertColumn(0,"标准离差");
		pTable->InsertColumn(0,"样本数");
		pTable->InsertColumn(0,"D.W.");
		
		pTable->InsertItem(0,"预测值");
		pTable->InsertItem(1,"残差");
		pTable->InsertItem(2,"标准化的预测值");
		pTable->InsertItem(3,"标准化的残差");
		
		
		double prey_mean;
		double prey_dev;
		CDoubleVector weight_v = data_read_resi(0);
		if (m_bWithWeight) 
		{
			prey_mean = sum(y_hat * weight_v)/ sum_weight;
			CDoubleVector temp_y0 = (y_hat - prey_mean);
			CDoubleVector temp_y1 = temp_y0*weight_v;
			prey_dev = sqrt((dot(temp_y0,temp_y1))/(sum_weight-1));
		}
		else
		{
			prey_mean = y_hat.Mean();
			prey_dev = sqrt(var(y_hat));
		}
		
		double resi_mean;
		double resi_dev;
		if (m_bWithWeight) 
		{
			resi_mean = sum(y_resi * weight_v) / sum_weight;
			CDoubleVector temp_y0 = (y_resi- resi_mean);
			CDoubleVector temp_y1 = temp_y0 * weight_v;
			resi_dev = sqrt((dot(temp_y0,temp_y1))/(sum_weight-1));
		}
		else
		{
			resi_mean = y_resi.mean();
			resi_dev = sqrt(var(y_resi));
		}
		
		double DW1 = 0;
		for (i=0; i<m_nXRow-1; i++)
			DW1+=pow((y_resi(i)-y_resi(i+1)),2);
		double DW = DW1 / y_resi.dot(y_resi);
		
		pTable->SetItemText(0,1,y_hat.Min());//prey.Min()
		pTable->SetItemText(0,2,y_hat.Max());//prey.Min()
		pTable->SetItemText(0,3,prey_mean);//sum(prey) / prey.vlen());
		pTable->SetItemText(0,4,prey_dev);//Dev(prey)
		pTable->SetItemText(0,5,int(sum_weight));//prey.vlen()
		pTable->SetItemText(0,6,DW);//Dw统计两
		
		pTable->SetItemText(1,1,y_resi.Min());//vector_e.Min());
		pTable->SetItemText(1,2,y_resi.Max());//vector_e.Max());
		pTable->SetItemText(1,3,resi_mean);//sum(vector_e) / vector_e.vlen());
		pTable->SetItemText(1,4,resi_dev);//Dev(vector_e));
		pTable->SetItemText(1,5,int(sum_weight));//vector_e.vlen()
		
		CDoubleVector zpred = (y_hat - prey_mean)/prey_dev;
		
		pTable->SetItemText(2,1,zpred.Min());
		pTable->SetItemText(2,2,zpred.Max());
		pTable->SetItemText(2,3,0.0);//sum(zpred) / zpred.vlen());
		pTable->SetItemText(2,4,1.0);//Dev(zpred)
		pTable->SetItemText(2,5,int(sum_weight));//zpred.vlen());
		
		double zresi_dev = sqrt((sum_weight-WithConst-m_X_temp_nameAry.GetSize())/(sum_weight-WithConst));
		CDoubleVector zresi = y_resi*(zresi_dev/resi_dev);
		double zresi_mean;
		if (m_bWithWeight) 
			zresi_mean = sum(zresi * weight_v) / sum_weight;
		else
			zresi_mean = y_resi.mean();
		
		pTable->SetItemText(3,1,zresi.Min());//zresid.Min());
		pTable->SetItemText(3,2,zresi.Max());//zresid.Max());
		pTable->SetItemText(3,3,zresi_mean);//sum(zresid) / zresid.vlen());
		pTable->SetItemText(3,4,zresi_dev);//Dev(zresid)
		pTable->SetItemText(3,5,int(sum_weight));//zresid.vlen());
		
		CRsltElementTable * pETable= new CRsltElementTable("残差统计表",pTable);
		m_pResult->Add(pETable);
	}
	

	//准备作图
	int stepofj = 1;
	const int Max = 3000;
	if (m_nXRow > Max)
		stepofj = int(m_nXRow / Max);
	int nXRow = m_nXRow / stepofj;
	TDataPointValVal* pValueY;
	TDataPointValVal* pYHat;
	pValueY = new TDataPointValVal[nXRow];
	pYHat = new TDataPointValVal[nXRow];
	for(j=0; j<nXRow; j++)
	{
		pValueY[j].fXVal = j * stepofj + 1;
		pYHat[j].fXVal = j * stepofj + 1;
		pValueY[j].fYVal = vect_y(j * stepofj);
		pYHat[j].fYVal = y_hat(j * stepofj);
	}
	
	if (m_bChartLine && m_tVarInfo.iCount==1)//只有一个变量（一元）时，画拟和图。
	{
		TDataPointValVal *pDPVV =new TDataPointValVal;        
		CDoubleVector XValue = data_read_resi(0+m_bWithWeight);
		for(int i=0; i<nXRow; i++)
		{
			pDPVV->fXVal = XValue(i * stepofj);
			pDPVV->fYVal = vect_y(i * stepofj);
		}
		CTChartPP* pPPPlot = new CTChartPP(CTChart::PP);
		pPPPlot->SetTitle(m_stry_name + "-拟合图");
		pPPPlot->SetXAxilLabel("自变量");
		pPPPlot->SetYAxilLabel("因变量");
		pPPPlot->SetData(pDPVV,nXRow,EScatter,ValVal);
		delete []pDPVV;
		double XMin = XValue.Min();
		double XMax = XValue.Max();
		double YMinValue = XMin*coefficiency(m_bWithConst) + m_bWithConst*coefficiency(0);
		double YMaxValue = XMax*coefficiency(m_bWithConst) + m_bWithConst*coefficiency(0);

		pDPVV = new TDataPointValVal[2];
		pDPVV[0].fXVal = XMin;
		pDPVV[0].fYVal = YMinValue;
		pDPVV[1].fXVal = XMax;
		pDPVV[1].fYVal = YMaxValue;
		pPPPlot->SetData(pDPVV,2,ELine,ValVal);
		CRsltElementChart *pREC = new CRsltElementChart(m_stry_name + "- 拟和图", pPPPlot);
		m_pResult->Add(pREC);   
		delete []pDPVV;
	}
	
	if ((m_bChartLine) && m_tVarInfo.iCount >1)//回归效果图
	{		
		CTChartLine *pTChartLine = new CTChartLine();
		pTChartLine->SetLine(pValueY, nXRow, ValVal,"原始数据"); 
		pTChartLine->SetLine(pYHat, nXRow, ValVal,"拟合数据"); 
		CRsltElementChart *pREC = new CRsltElementChart(m_stry_name + "-回归效果图",  pTChartLine);
		m_pResult->Add(pREC);    
	}
	/*
	if (m_bChartLine)//预测拟合图
	{
		CTChartLine* pTChartLine = new CTChartLine;
		pTChartLine->SetXAxilLabel("样本");
		pTChartLine->SetYAxilLabel("期望");
		CTString strTitle = "回归效果图";
		CRsltElementChart *pRegFit = new CRsltElementChart(strTitle, pTChartLine);
		m_pResult->Add(pRegFit);

		pTChartLine->SetLine(pValueY, nXRow, ValVal, "原始数据");
		pTChartLine->SetLine(pYHat, nXRow, ValVal, "拟和数据");
	}//******************************* 预测拟合图 END
	*/
	if (m_bChartScatterPlot)//残差散点图
	{
		CTChartScatterPlot* pResScat = new CTChartScatterPlot;
		CRsltElementChart* pResidScat = new CRsltElementChart("残差散点图", pResScat);
		
		TDataPointValVal* pResi;
		pResi = new TDataPointValVal[nXRow];
		for(int i=0; i<nXRow; i++)
		{
			pResi[i].fXVal = vect_y(i * stepofj);//预测);
			pResi[i].fYVal = vect_y(i * stepofj) - y_hat(i * stepofj);//实际-预测);
		}
		m_pResult->Add(pResidScat);
		pResScat->SetScatter(pResi, nXRow, ValVal, "");
		delete []pResi;
	}
	//******************************* 残差散点图 END
	if (m_bChartScatterPlot2)// 残差正态概率图
	{
		CDoubleVector AveVect;
		AveVect.create(nXRow);
		int i;
		for(i=0; i<nXRow; i++)
		{
			AveVect(i) = vect_y(i*stepofj) - y_hat(i*stepofj);
		}
		AveVect.Sort();
		CDoubleVector Norms = GetNormMedians(nXRow);
		CDoubleVector Beta = SimpleRegress(AveVect,Norms); 
		TDataPointValVal *pDPVV = new TDataPointValVal[nXRow];
			
		for(i=0; i<nXRow; i++)
		{
			pDPVV[i].fXVal = Norms(i);
			pDPVV[i].fYVal = AveVect(i);
		}
		CTChartPP* pPPPlot = new CTChartPP(CTChart::PP);	
		pPPPlot->SetTitle(m_stry_name + "-正态概率图");
		pPPPlot->SetXAxilLabel("正态分位数");
		pPPPlot->SetYAxilLabel(m_stry_name);
		pPPPlot->SetData(pDPVV,nXRow,EScatter,ValVal);
		delete []pDPVV;
		pDPVV = new TDataPointValVal[2];
		double fxMin = 0, fxMax = 0;
		double fyMin = 0, fyMax = 0;
		Norms.MaxMin(fxMax,fxMin);
		AveVect.MaxMin(fyMax,fyMin);
		double bb = Beta(0);
		double kk = Beta(1);
		double rr = Beta(2);
		pDPVV[0].fXVal = fxMin;
		pDPVV[0].fYVal = fxMin*kk+bb;
		pDPVV[1].fXVal = fxMax;
		pDPVV[1].fYVal = fxMax*kk+bb;
		pPPPlot->SetData(pDPVV,2,ELine,ValVal);
		CRsltElementChart*pREC=new CRsltElementChart(m_stry_name + "-正态概率图",pPPPlot);
		m_pResult->Add(pREC);
		delete []pDPVV;
	}
	//**************************** 残差正态概率图 END	
	delete[] pValueY;//移动到外面，防止非预测时的内存泄漏 //#_S 2004-12-30 zlq
	delete[] pYHat;
	pValueY = pYHat = NULL;	
}


CDoubleMatrix CRegressionLinear::SelectMatrix(CDoubleVector SingVector,CDoubleMatrix SMatrix,int tot)//按照SingVector来选择SMatrix中的行和列，SingVector的第i列大于0，则选矩阵的i行和列与其他j的交叉项。
{
	CDoubleMatrix SNMatrix(tot, tot);
	int tt1 = 0;
	for (int i=0; i<SingVector.vlen(); i++)
	{
		if (SingVector(i)>0) 
		{
			int tt2 = 0;
			for (int j=0; j<=i; j++)
			{
				if (SingVector(j)>0) 
				{
					SNMatrix(tt1,tt2) = SMatrix(i,j);
					SNMatrix(tt2,tt1) = SMatrix(i,j);
					tt2++;
				}
			}
			tt1++;
		}
	}
	return SNMatrix;
}

CDoubleVector CRegressionLinear::SelectVector(CDoubleVector SingVector,CDoubleVector SVector, int tot)
{
	int tt=0;
	CDoubleVector SNVector(tot);
	for (int i=0; i<SingVector.vlen(); i++)
	{
		if (SingVector(i)>0) 
		{
			SNVector(tt)=SVector(i);
			tt++;
		}
	}
	return SNVector;
}

void CRegressionLinear::SelectStrAry(CDoubleVector SingVector,CTStringArray & m_strX_nameAry, CTStringArray &X_temp_nameAry)
{
	X_temp_nameAry.RemoveAll();
	for (int i=0; i<SingVector.vlen()-m_bWithConst; i++)
	{
		if (SingVector(i+m_bWithConst)>0) 
		{
			X_temp_nameAry.Add(m_strX_nameAry.GetAt(i));
		}
	}
}

void CRegressionLinear::RegLinear(CDoubleMatrix XX, CDoubleMatrix Xy, CDoubleMatrix CorrMatrix,double sum_weight, int XCol, int XRow, bool WithConst)
{
	m_df_residual = (((m_bWithWeight)?sum_weight:XRow) - XCol - WithConst);
	m_coefficiency = inv(XX) * Xy(0);
	m_SST = (WithConst)?m_var_y:m_ytimesy;
	m_SSE = fabs(m_ytimesy - dot(Xy,(VectorToMatrix(m_coefficiency)))(0,0));
	if (m_SSE<EPS_ACCUR)
		m_SSE = EPS_ACCUR;
	CDoubleMatrix temp_xx = CorrMatrix.slice(XCol,XCol);
	m_MSE = m_SSE/(m_df_residual);
	m_vector_sbeta = sqrt( m_MSE * diag( inv(XX)));
	if (XCol>0)
	{	
		m_vector_standbeta = ((inv(temp_xx)) * CorrMatrix.slice(0,XCol,XCol,1))(0);
		m_vector_T     = m_coefficiency / m_vector_sbeta;
		m_vector_Sig   = 2.0 * ( 1.0 - tcdf(abs(m_vector_T),m_df_residual));
		if (m_bCoefInter) 
		{
			double t_value = tinv((1+m_dConfidence)/2.0, m_df_residual);
			m_CoefLower = m_coefficiency - t_value*m_vector_sbeta;
			m_CoefUpper = m_coefficiency + t_value*m_vector_sbeta;
		}
	}
	m_R_square = 1- m_SSE/m_SST;
	m_R_adjust = 1 - m_SSE*(m_df_residual +m_nXCol)/(m_SST*(m_df_residual));
}

CDoubleMatrix CRegressionLinear::DelMatrix(CDoubleMatrix matrix_comein, int mm)
{
	if ((mm < matrix_comein.mrow())&&(mm<matrix_comein.mcol()) &&(mm>=0))
	{
		int ii,jj;
		CDoubleMatrix matrix(matrix_comein.mrow()-1,matrix_comein.mcol()-1);
		for (int i=0;i<matrix_comein.mcol();i++)
			for (int j=0;j<matrix_comein.mrow();j++)
			{
				if (i>mm)
					ii = i-1;
				else if (i<mm)
					ii = i;
				else
					continue;
				if (j>mm)
					jj = j-1;
				else if (j< mm)
					jj = j;
				else
					continue;
				matrix(jj,ii) = matrix_comein(j,i);
			}
			return matrix;
	}
	return matrix_comein;
}

CDoubleVector CRegressionLinear::mmthSelfReg(CDoubleMatrix matrix_comein, CDoubleVector Xmean, bool WithConst)//求Tolerance
{
	CDoubleVector Tolerance(matrix_comein.mcol()-WithConst);
	int num = matrix_comein.mrow();
	if ((matrix_comein.mcol()-WithConst) == 1) 
	{
		Tolerance(0)=1.0;
		return Tolerance;
	}
	int i;
	for (int mm=WithConst; mm<matrix_comein.mcol(); mm++)
	{
		CDoubleMatrix temp_ma1 = copy(matrix_comein);
		CDoubleMatrix matrix(matrix_comein.mrow()-1,matrix_comein.mcol());
		
		for (i=mm;i<num-1;i++)
			temp_ma1(i) = temp_ma1(i+1);
		temp_ma1 = temp_ma1.slice(num,num-1);
		CDoubleMatrix temp_ma = temp_ma1.trans();
		for (i=0; i<mm; i++)
			matrix(i) = temp_ma(i);
		for (i=mm; i<num-1; i++)
			matrix(i)=temp_ma(i+1);
		CDoubleVector te_xy = temp_ma(mm);
		CDoubleMatrix te_xx = matrix.slice(num-1, num-1);
		CDoubleVector coef = inv(te_xx) * te_xy;
		double sst_t = matrix_comein(mm,mm) - ((m_bWithWeight)?sum_weight:m_nXRow)* (WithConst?(Xmean(mm-WithConst)*Xmean(mm-WithConst)):0);
		double sse_t = matrix_comein(mm,mm) - te_xy.dot(coef);
		Tolerance(mm-WithConst) = sse_t/sst_t;
	}
	return Tolerance;
}


CDoubleVector CRegressionLinear::DelVector(CDoubleVector vector_comein, int mm)
{
	if (mm < vector_comein.vlen())
	{
		int ii;
		CDoubleVector vect(vector_comein.vlen()-1);
		for (int i=0;i<vector_comein.vlen();i++)
		{
			if (i>mm)
				ii = i-1;
			else if (i<mm)
				ii = i;
			else
				continue;
			vect(ii) = vector_comein(i);
		}
		return vect;
	}
	return vector_comein;
}


//求特征根
CDoubleMatrix CRegressionLinear::EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex)
{
	const double eps=0.0000000001;
	int m_Row=(*m_EigVec).mrow();
	if (m_Row!=(*m_EigVec).mcol())
	{
		return NULL;
	}
	else
	{
		CDoubleVector	Vect_Data = MatrixToVector(*m_EigVec);
		CDoubleVector	m_VectEigVector(m_Row*m_Row);
		cjcbj(Vect_Data, m_Row, m_VectEigVector, eps);
		SortEigenVect(Vect_Data, m_VectEigVector, m_Row, m_Row, nIndex);
		CDoubleMatrix	EigValue(m_Row,1);
		for( int i= 0; i < m_Row; i++)
		{
			EigValue(i,0) = Vect_Data(i+i*m_Row);
		}
		*m_EigVec = VectorToMatrix(m_VectEigVector,m_Row,m_Row).trans();
		return EigValue;
	}
}

void CRegressionLinear::InitParma()
{
	m_bStopCalculate = false;
	m_strErrorInfo = _T("");
}


void CRegressionLinear::DrawCorrolation(CDoubleMatrix Xy_Sigma)
{
	int i,j;
	double df;
	CDoubleMatrix CorrMatrix_org = Xy_Sigma/sqrt(Multi(diag(Xy_Sigma),diag(Xy_Sigma)));
	int XCol_org = Xy_Sigma.mrow()-1;
	if (m_bWithWeight) 
		df = sum_weight - 2;
	else
		df = m_nXRow - 2;
	CDoubleMatrix matrix_sig(XCol_org+1,XCol_org+1);
	for(i=0;i<XCol_org+1;i++)
	{
		for(j=0;j<XCol_org+1;j++)
		{
			if(( i == j)||(CorrMatrix_org(i,j) == 1) )
				matrix_sig(i,j) = 0;
			else
			{
				double t = CorrMatrix_org(i,j) * sqrt( ( df)/(1.0-CorrMatrix_org(i,j)*CorrMatrix_org(i,j)));
				matrix_sig(i,j) = 1.0 - tcdf(fabs(t),df);
			}
		}
	}
	CTLTable *pTable = new CTLTable;
	pTable->SetTitle("相关系数表");
	pTable->CreateTable(2, 2);
	pTable->SetCols(Xy_Sigma.mcol()+2);
	pTable->SetRows(Xy_Sigma.mrow()*3+1);
	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"");
	for(i=0;i<XCol_org;i++)
	{
		pTable->InsertColumn(i,m_strX_org[i]);
		pTable->InsertItem(i*3,m_strX_org[i],false);
		pTable->SetItemText( i*3, 1, "相关系数" );
		pTable->InsertItem(i*3+1,"",false);
		//pTable->SetItemText( i*3+1, 1, "显著性" );//根据计算结果可知此处应为单边显著性
		pTable->SetItemText( i*3+1, 1, "单边显著性" );
		pTable->InsertItem(i*3+2,"");
		pTable->SetItemText( i*3+2, 1, "协方差" );
	}
	pTable->InsertColumn(i,m_stry_name);
	pTable->InsertItem(i*3,m_stry_name, false);
	pTable->SetItemText( i*3, 1, "相关系数" );
	pTable->InsertItem(i*3+1,"",false);
	//pTable->SetItemText( i*3+1, 1, "显著性" );//根据计算结果可知此处应为单边显著性
	pTable->SetItemText( i*3+1, 1, "单边显著性" );
	pTable->InsertItem(i*3+2,"");
	pTable->SetItemText( i*3+2, 1, "协方差" );
	
	for(i=0;i<Xy_Sigma.mrow();i++)
	{
		for(j=0;j<Xy_Sigma.mcol();j++)
		{
			pTable->SetItemText(i*3,j+2,CorrMatrix_org(i,j));
			if( i!=j )
				pTable->SetItemText(i*3+1,j+2,matrix_sig(i,j));
			pTable->SetItemText(i*3+2,j+2,Xy_Sigma(i,j)/(df+2));
		}
	}
	CRsltElementTable * pETable= new CRsltElementTable("相关系数表",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
}

void CRegressionLinear::DrawResult(int XCol, int WithConst)
{
	m_strArr_FinalXname.Copy(m_X_temp_nameAry);
	if (XCol >0) 
	{
		//回归方程
		//-------------------------------
		CTString strEquation=m_stry_name+" = ";	
		CTString strTemp;
		int temp_cont=1;
		
		if (WithConst) 
		{
			strTemp.Format("%f",m_coefficiency(0));
			strEquation+=strTemp;
		}
		for (int i=0; i<XCol; i++)
		{    		
			strTemp.Format("%.4f*%s",m_coefficiency(i+WithConst),m_X_temp_nameAry.GetAt(i).GetData());
			if ((m_coefficiency(i+WithConst)>0) && (WithConst || (i>0)))
				strEquation+="+";
			if (strEquation.GetLength()+strTemp.GetLength() > temp_cont * 100)
			{
				strEquation =strEquation + "\n";
				temp_cont++;
			}
			
			strEquation+=strTemp;
		}
		CRsltElementTable * pETable = NULL;
		CTLTable * pTable1 = new CTLTable;
		pTable1->CreateTable(2, 2);	
		pTable1->SetTitle("回归方程");
		pTable1->InsertColumn(0,strEquation);
		pETable= new CRsltElementTable("回归方程",pTable1);//申请一个表对象类的对象
		m_pResult->Add(pETable);
		//模型分析
		//-------------------------------
		
		CTLTable * pTable2 = new CTLTable;
		pTable2->CreateTable(2, 2);
		pTable2->SetTitle("模型分析");
		pTable2->SetRows(2);
		pTable2->SetCols(2);
		pTable2->InsertColumn(0,"");
		pTable2->InsertColumn(0,"R");
		pTable2->InsertColumn(0,"R 平方");
		pTable2->InsertColumn(0,"修正的R 平方");
		pTable2->InsertColumn(0,"估计的标准误");
		CTString str;
		str.Format("%f/4",sqrt(m_R_square));
		pTable2->InsertItem(0,"");
		pTable2->SetItemText(0,1,sqrt(m_R_square));
		pTable2->SetItemText(0,2,m_R_square);
		pTable2->SetItemText(0,3,m_R_adjust);
		pTable2->SetItemText(0,4,sqrt(m_MSE));
		CRsltElementTable * pETable2 = NULL;
		pETable2 = new CRsltElementTable("模型分析",pTable2);//申请一个表对象类的对象
		m_pResult->Add(pETable2);
		CTString strInfl="",strVars="";//"自变量";
		strVars+=m_X_temp_nameAry.GetAt(0);
		for (int j=1;j<m_X_temp_nameAry.GetSize();j++)
			strVars+=","+m_X_temp_nameAry.GetAt(j);
		strVars+="对"+m_stry_name;
		strInfl.Format("变异的影响为%f",m_R_square*100);
		
		if (briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型中"+strVars+strInfl+"%");	
		//-------------------------------
		//方差分析表
		//--------------------------------
		CTLTable * pTable3 = new CTLTable ;
		pTable3->CreateTable(2, 2);
		pTable3->SetRows(4);
		pTable3->SetCols(6);
		pTable3->SetTitle("方差分析表");
		pTable3->InsertColumn(0,"");
		pTable3->InsertColumn(0,"平方和");
		pTable3->InsertColumn(0,"自由度");
		pTable3->InsertColumn(0,"均方");
		pTable3->InsertColumn(0,"F值");
		pTable3->InsertColumn(0,"显著性");
		
		pTable3->InsertItem(0,"回归");
		pTable3->InsertItem(1,"残差");
		pTable3->InsertItem(2,"总和");
		pTable3->SetItemText(0,1,(m_SST-m_SSE));
		pTable3->SetItemText(1,1,(m_SSE));
		pTable3->SetItemText(2,1,m_SST);
		
		pTable3->SetItemText(0,2,XCol);
		pTable3->SetItemText(1,2,int(m_df_residual));
		pTable3->SetItemText(2,2,int(XCol + m_df_residual));
		pTable3->SetItemText(0,3,((m_SST-m_SSE)/XCol));
		pTable3->SetItemText(1,3,m_MSE);
		pTable3->SetItemText(0,4,(m_SST-m_SSE)/XCol/m_MSE);
		double pofFvalue;
		pofFvalue=1 - fcdf((m_SST-m_SSE)/XCol/m_MSE, XCol, m_df_residual);
		pTable3->SetItemText(0,5,pofFvalue);
		CRsltElementTable * pETable3= new CRsltElementTable("方差分析表",pTable3);
		m_pResult->Add(pETable3);
		//--------------------------------
		//回归系数分析
		//---------------------------------
		CTLTable * pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);
		pTable4->SetTitle("回归系数分析");
		pTable4->SetRows(2);
		pTable4->SetCols(10);
		pTable4->InsertColumn(0,"");
		pTable4->InsertColumn(1,"回归系数");
		pTable4->InsertColumn(2,"标准误");
		pTable4->InsertColumn(3,"标准化的\nbeta");
		pTable4->InsertColumn(4,"t");
		pTable4->InsertColumn(5,"显著性");
		CTString strInterval1;
		CTString strInterval2;	
		if (m_bCoefInter)
		{
			strInterval1.Format("%.f%%置信区间\n上界",m_dConfidence*100);
			strInterval2.Format("%.f%%置信区间\n下界",m_dConfidence*100);
			pTable4->InsertColumn(6,strInterval1);
			pTable4->InsertColumn(7,strInterval2);
		}
		if (m_bWithColl) 
		{
			pTable4->InsertColumn(6+m_bCoefInter*2,"方差扩大因子");
			pTable4->InsertColumn(7+m_bCoefInter*2,"容忍度");	
		}
		
		if (WithConst)
		{
			pTable4->InsertItem(0,"常数项");
			pTable4->SetItemText(0,1,m_coefficiency(0));
			pTable4->SetItemText(0,2,m_vector_sbeta(0));
			//pTable4->SetItemText(k,3,m_vector_standbeta(k));
			pTable4->SetItemText(0,4,m_vector_T(0));
			pTable4->SetItemText(0,5,m_vector_Sig(0));
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(0,6,m_CoefUpper(0));
				pTable4->SetItemText(0,7,m_CoefLower(0));
			}
		}
		for (int k = WithConst ; k<XCol+WithConst;k++)
		{
			pTable4->InsertItem(k,m_X_temp_nameAry[k-WithConst]);
			pTable4->SetItemText(k,1,m_coefficiency(k));
			pTable4->SetItemText(k,2,m_vector_sbeta(k));
			pTable4->SetItemText(k,3,m_vector_standbeta(k-WithConst));
			pTable4->SetItemText(k,4,m_vector_T(k));
			pTable4->SetItemText(k,5,m_vector_Sig(k));	
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(k,6,m_CoefUpper(k));
				pTable4->SetItemText(k,7,m_CoefLower(k));
			}
			if (m_bWithColl) 
			{			
				pTable4->SetItemText(k,6+m_bCoefInter*2,1/m_Tolerance(k-WithConst));
				pTable4->SetItemText(k,7+m_bCoefInter*2,m_Tolerance(k-WithConst));
			}
		}
		CRsltElementTable * pETable4= new CRsltElementTable("回归系数分析",pTable4);
		m_pResult->Add(pETable4);
		//---------------------------------
		
		
		//共线性诊断
		//----------------------------------
		if (m_bWithColl) 
		{
			CTLTable * pTable6 = new CTLTable;	
			pTable6->CreateTable(2, 2);
			pTable6->SetTitle("共线性诊断");
			pTable6->InsertColumn(0,"");
			pTable6->InsertColumn(1,"特征值");
			pTable6->InsertColumn(2,"条件指数");
			pTable6->SetRows(4);
			pTable6->SetCols(3);
			//		if (WithConst)
			//		{
			//			pTable6->InsertColumn(0,"方差比例\n(常数项)");
			//		}
			//		for (int t=WithConst; t<XCol+WithConst;t++)
			//		{
			//			pTable6->InsertColumn(0,(_T("方差比例\n(") + m_X_temp_nameAry[t-WithConst] +_T(")") ));
			//		}
			
			for (int k = 0; k<XCol+WithConst;k++)
			{
				strTemp.Format("%d",(k+1));
				pTable6->InsertItem(k,strTemp);
				pTable6->SetItemText(k,1,m_useforcoll_e(k));
				pTable6->SetItemText(k,2,m_vector_condindex(k));
			}
			
			CRsltElementTable * pETable6= new CRsltElementTable("共线性诊断",pTable6);//申请一个表对象类的对象
			m_pResult->Add(pETable6);
		}
	}
	else
	{
		CTString strWarning = "注： 只有常数项进入回归，暂不输出回归分析结果。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}			
}

bool CRegressionLinear::IsExisted(CDataInterface *p,CTString str)
{
	for(int i=0;i<p->GetColCount();i++)
	{
		if(p->GetFieldName(i)==str)
		{
			return true;
		}
	}
	return false;
}

void CRegressionLinear::Collinearity(CDoubleMatrix m_XtimesX, int XCol, bool WithConst)
{		//共线性诊断
	CDoubleMatrix useforcoll_matrix = m_XtimesX.copy();
	for (int i=0; i<XCol+WithConst; i++)
		useforcoll_matrix(i) = useforcoll_matrix(i) / useforcoll_matrix(i,i);
	CComplexMatrix useforcoll_m;//（特征向量矩阵复）
	CComplexMatrix useforcoll_v;//特征根向量(复)
	useforcoll_v = eig(useforcoll_matrix, useforcoll_m);
	CComplexVector useforcoll_eig = diag(useforcoll_v);
	m_useforcoll_e = (useforcoll_eig.real()).Sort(1);
	m_vector_condindex = sqrt( m_useforcoll_e.Max() / m_useforcoll_e);//条件数
}

void CRegressionLinear::DrawTableHead(int XCol, int WithConst)
{
	if (XCol >0) 
	{
		//-------------------------------
		//回归方程
		//-------------------------------
		pTable1 = new CTLTable;
		pTable1->CreateTable(2, 2);	
		pTable1->SetTitle("回归方程");
		pTable1->InsertColumn(0, "");
		pTable1->InsertColumn(0, "回归方程");
		//-------------------------------
		//模型分析
		//-------------------------------		
		pTable2 = new CTLTable;
		pTable2->CreateTable(2, 2);
		pTable2->SetTitle("模型分析");
		pTable2->InsertColumn(0, "");
		pTable2->InsertColumn(0, "R");
		pTable2->InsertColumn(0, "R 平方");
		pTable2->InsertColumn(0, "修正的R 平方");
		pTable2->InsertColumn(0, "估计的标准误");
		//-------------------------------
		//方差分析表
		//--------------------------------
		pTable3 = new CTLTable ;
		pTable3->CreateTable(2, 2);
		pTable3->SetTitle("方差分析表");
		pTable3->InsertColumn(0, "");
		pTable3->InsertColumn(0, "");
		pTable3->InsertColumn(0, "平方和");
		pTable3->InsertColumn(0, "自由度");
		pTable3->InsertColumn(0, "均方");
		pTable3->InsertColumn(0, "F值");
		pTable3->InsertColumn(0, "显著性");
		//--------------------------------
		//回归系数分析
		//---------------------------------
		pTable4 = new CTLTable;
		pTable4->CreateTable(2, 2);
		pTable4->SetTitle("回归系数分析");
		pTable4->InsertColumn(0, "");
		pTable4->InsertColumn(0, "");
		pTable4->InsertColumn(0, "回归系数");
		pTable4->InsertColumn(0, "标准误");
		pTable4->InsertColumn(0, "标准化的\nbeta");
		pTable4->InsertColumn(0, "t");
		pTable4->InsertColumn(0, "显著性");
		CTString strInterval1;
		CTString strInterval2;	
		if (m_bCoefInter)
		{
			strInterval1.Format("%.f%%置信区间\n上界", m_dConfidence * 100);
			strInterval2.Format("%.f%%置信区间\n下界", m_dConfidence * 100);
			pTable4->InsertColumn(0, strInterval1);
			pTable4->InsertColumn(0, strInterval2);
		}
		if (m_bWithColl) 
		{
			pTable4->InsertColumn(0 /*+ m_bCoefInter * 2*/, "方差扩大因子");
			pTable4->InsertColumn(0 /*+ m_bCoefInter * 2*/, "容忍度");	
		}
		//---------------------------------	
		//共线性诊断
		//----------------------------------
		if (m_bWithColl) 
		{
			pTable6 = new CTLTable;	
			pTable6->CreateTable(2, 2);
			pTable6->SetTitle("共线性诊断");			
			pTable6->InsertColumn(0,"");
			pTable6->InsertColumn(0,"");			
			pTable6->InsertColumn(0,"特征值");
			pTable6->InsertColumn(0,"条件指数");
		}
	}
	else
	{
		CTString strWarning = "注： 只有常数项进入回归，暂不输出回归分析结果。";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
	}
}

void CRegressionLinear::DrawTableBody(int XCol, int WithConst, int &number, CTString strWarning)
{
	m_strArr_FinalXname.Copy(m_X_temp_nameAry);
	if (XCol >0) 
	{
		//回归方程
		//-------------------------------
		CTString strEquation=m_stry_name+" = ";	
		CTString strTemp;
		int temp_cont=1;
		
		CTString strstr;
		strstr.Format("%d", number+1);
		
		if (WithConst) 
		{
			strTemp.Format("%f",m_coefficiency(0));
			strEquation+=strTemp;
		}
		for (int i=0; i<XCol; i++)
		{    		
			strTemp.Format("%.4f*%s",m_coefficiency(i+WithConst),m_X_temp_nameAry.GetAt(i).GetData());
			if ((m_coefficiency(i+WithConst)>0) && (WithConst || (i>0)))
				strEquation+="+";
			if (strEquation.GetLength()+strTemp.GetLength() > temp_cont * 100)
			{
				strEquation =strEquation + "\n";
				temp_cont++;
			}
			
			strEquation+=strTemp;
		}
		if(strWarning != "")
			strEquation = "( " + strWarning + ")" + "\n" +strEquation;
		pTable1->InsertItem(number, strstr, true);
		pTable1->SetItemText(number, 1, strEquation);
		
		//-------------------------------
		//模型分析
		//-------------------------------
		CTString str;
		str.Format("%f/4",sqrt(m_R_square));
		pTable2->InsertItem(number,strstr,true);
		pTable2->SetItemText(number,1,sqrt(m_R_square));
		pTable2->SetItemText(number,2,m_R_square);
		pTable2->SetItemText(number,3,m_R_adjust);
		pTable2->SetItemText(number,4,sqrt(m_MSE));
		CTString strInfl="",strVars="";//"自变量";
		strVars+=m_X_temp_nameAry.GetAt(0);
		for (int j=1;j<m_X_temp_nameAry.GetSize();j++)
			strVars+=","+m_X_temp_nameAry.GetAt(j);
		strVars+="对"+m_stry_name;
		strInfl.Format("变异的影响为%f",m_R_square*100);
		
		if (briefrTable)
			briefrTable->InsertItem(briefrTable->GetTableRowS()+1,"模型中"+strVars+strInfl+"%",false);	
		//-------------------------------
		//方差分析表
		//--------------------------------
		pTable3->InsertItem(3*number,strstr,false);
		pTable3->InsertItem(3*number+1,"",false);
		pTable3->InsertItem(3*number+2,"",true);
		pTable3->SetItemText(3*number,1,"回归");
		pTable3->SetItemText(3*number+1,1,"残差");
		pTable3->SetItemText(3*number+2,1,"总和");
		pTable3->SetItemText(3*number,2,(m_SST-m_SSE));
		pTable3->SetItemText(3*number+1,2,(m_SSE));
		pTable3->SetItemText(3*number+2,2,m_SST);		
		pTable3->SetItemText(3*number,3,XCol);
		pTable3->SetItemText(3*number+1,3,int(m_df_residual));
		pTable3->SetItemText(3*number+2,3,int(XCol + m_df_residual));
		pTable3->SetItemText(3*number,4,((m_SST-m_SSE)/XCol));
		pTable3->SetItemText(3*number+1,4,m_MSE);
		pTable3->SetItemText(3*number,5,(m_SST-m_SSE)/XCol/m_MSE);
		double pofFvalue;
		pofFvalue=1 - fcdf((m_SST-m_SSE)/XCol/m_MSE, XCol, m_df_residual);
		pTable3->SetItemText(3*number,6,pofFvalue);
		//--------------------------------
		//回归系数分析
		//---------------------------------
		if(XCol == 1 && !WithConst)//2006.01.17  add && !WithConst
			pTable4->InsertItem(m_nLine,strstr);
		else
			pTable4->InsertItem(m_nLine,strstr,false);
		if (WithConst)//如果有常数项
		{
			pTable4->SetItemText(m_nLine,1,"常数项");
			pTable4->SetItemText(m_nLine,2,m_coefficiency(0));
			pTable4->SetItemText(m_nLine,3,m_vector_sbeta(0));
			pTable4->SetItemText(m_nLine,5,m_vector_T(0));
			pTable4->SetItemText(m_nLine,6,m_vector_Sig(0));
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(m_nLine,7,m_CoefUpper(0));
				pTable4->SetItemText(m_nLine,8,m_CoefLower(0));
			}
			if(XCol == 1)//2006.01.17  add
				pTable4->InsertItem(++m_nLine,"",true);	//2006.01.17  add
			else//2006.01.17  add
				pTable4->InsertItem(++m_nLine,"",false);	
		}
		for (int k = WithConst ; k<XCol+WithConst;k++)
		{
			pTable4->SetItemText(m_nLine,1,m_X_temp_nameAry[k-WithConst]);
			pTable4->SetItemText(m_nLine,2,m_coefficiency(k));
			pTable4->SetItemText(m_nLine,3,m_vector_sbeta(k));
			pTable4->SetItemText(m_nLine,4,m_vector_standbeta(k-WithConst));
			pTable4->SetItemText(m_nLine,5,m_vector_T(k));
			pTable4->SetItemText(m_nLine,6,m_vector_Sig(k));	
			if (m_bCoefInter) 
			{
				pTable4->SetItemText(m_nLine,7,m_CoefUpper(k));
				pTable4->SetItemText(m_nLine,8,m_CoefLower(k));
			}
			if (m_bWithColl) 
			{			
				pTable4->SetItemText(m_nLine,7+m_bCoefInter*2,1/m_Tolerance(k-WithConst));
				pTable4->SetItemText(m_nLine,8+m_bCoefInter*2,m_Tolerance(k-WithConst));
			}
			m_nLine++;
			if(k+2<XCol+WithConst)
			{
				pTable4->InsertItem(m_nLine,"",false);				
			}
			else if(k+1<XCol+WithConst)
			{
				pTable4->InsertItem(m_nLine,"");
			}
		}
		//---------------------------------
		//共线性诊断
		//----------------------------------
		if (m_bWithColl) 
		{
			if(XCol == 1)
				pTable6->InsertItem(m_nColl,strstr);
			else
				pTable6->InsertItem(m_nColl,strstr,false);
			for (int k = 0; k<XCol+WithConst;k++)
			{
				strTemp.Format("%d",(k+1));
				pTable6->SetItemText(m_nColl,1,strTemp);
				pTable6->SetItemText(m_nColl,2,m_useforcoll_e(k));
				pTable6->SetItemText(m_nColl,3,m_vector_condindex(k));
				m_nColl++;
				if(k+2<XCol+WithConst)
				{
					pTable6->InsertItem(m_nColl,"",false);				
				}
				else if(k+1<XCol+WithConst)
				{
					pTable6->InsertItem(m_nColl,"");				
				}
			}			
		}
		number++;
	}
}

void CRegressionLinear::DrawTable(int XCol, int WithConst)
{
	//	if(XCol > 0)
	{
		CRsltElementTable *  pETable1= new CRsltElementTable("回归方程",pTable1);//申请一个表对象类的对象
		m_pResult->Add(pETable1);
		CRsltElementTable *  pETable2 = new CRsltElementTable("模型分析",pTable2);//申请一个表对象类的对象
		m_pResult->Add(pETable2);
		CRsltElementTable *  pETable3= new CRsltElementTable("方差分析表",pTable3);
		m_pResult->Add(pETable3);
		CRsltElementTable * pETable4= new CRsltElementTable("回归系数分析",pTable4);
		m_pResult->Add(pETable4);	
		if (m_bWithColl) 
		{		
			CRsltElementTable * pETable6= new CRsltElementTable("共线性诊断",pTable6);//申请一个表对象类的对象
			m_pResult->Add(pETable6);
		}
	}
}

//以下部分是做预测的。
CResult* CRegressionLinear::OnRegPred(CDataInterface * pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	m_pResult = new CResult("线性回归模型预测结果");
	if (!GetPredValueVo(pDataInterface, VO))//读参数及模型
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( "模型参数有误，无法运行。" );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	CDWordArray indexArr;
	int nCol = m_tVarPred.iCount;
	int i=0;
	for (i=0; i<m_tVarPred.iCount; i++)
		indexArr.Add(m_tVarPred.pValue[i]);
	CDoubleMatrix Pred_Mx;
	int nRow = pDataInterface->GetColsData(indexArr, Pred_Mx, 1);//读数据
	if (m_bWithConst && (Pred_Mx.mcol() < m_coefficiency.vlen()) )
	{
		CDoubleVector ConstOne(nRow, 1.0);
		Pred_Mx = ConstOne | Pred_Mx;
	}
	CDoubleVector yPred = Pred_Mx * m_coefficiency;//预测值
	CDoubleVector yRange;
	CDoubleVector yRangeUp;
	CDoubleVector yRangeLow;
	if (m_bPredRange)//如果要输出预测值的置信区间
	{	
		CDoubleMatrix X_inv = inv(m_XtimesX);
		double tValue = tinv( (1+m_dConfidence)/2.0, sum_weight -m_X_temp_nameAry.GetSize()-m_bWithConst);

		yRange.create(nRow);
		CDoubleVector vecTemp(m_tVarPred.iCount+m_bWithConst, 1.0);
		for (i=0; i<nRow; i++)//计算预测值
		{
			for (int j=0; j<nCol; j++)
				vecTemp(j+m_bWithConst) = Pred_Mx(i,j);
			yRange(i) = tValue*sqrt(1+((VectorToMatrix(vecTemp).trans())*X_inv*VectorToMatrix(vecTemp))(0,0))*sqrt(m_MSE);
		}
		yRangeUp = yPred + yRange;
		yRangeLow = yPred - yRange;
	}
//	在主界面中添加：始
	CTString PredUpName = m_strDepPred + "_上界";
	CTString PredLowName = m_strDepPred + "_下界";
	//预测变量名称
	if (m_pDataInterface->m_DataAccess.FieldByName(m_strDepPred) != NULL)
	{
		int nTemp = m_pDataInterface->m_DataAccess.IndexByName(m_strDepPred);
		m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
	}	
	CField *PredField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
	PredField->SetFieldName(m_strDepPred);
	pDataInterface->m_DataAccess.m_pFieldList->Add(PredField);
	CField *PredUpField;
	CField *PredLowField;
	if (m_bPredRange)
	{
		if (m_pDataInterface->m_DataAccess.FieldByName(PredUpName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(PredUpName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		if (m_pDataInterface->m_DataAccess.FieldByName(PredLowName) != NULL)
		{
			int nTemp = m_pDataInterface->m_DataAccess.IndexByName(PredLowName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
		}	
		PredUpField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		PredLowField = pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		PredUpField->SetFieldName(PredUpName);
		PredLowField->SetFieldName(PredLowName);
	}
	pDataInterface->m_DataAccess.First();
	for (i=0; i<nRow; i++)
	{
		pDataInterface->m_DataAccess.Edit();
		PredField->SetAsDouble(yPred(i));
		if (m_bPredRange)
		{
			PredUpField->SetAsDouble(yRangeUp(i));
			PredLowField->SetAsDouble(yRangeLow(i));
		}
		//pDataInterface->m_DataAccess.Post();			
		pDataInterface->m_DataAccess.Next();
	}
	//TEST 用，保存
	CTString szNowFile("");
	CTString szNewFile("");
	szNowFile = pDataInterface->m_DataAccess.GetFileName();
	bool bOld = pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
	//真时:为NewFile
	//假时:为NowFile.
	CTString SaveFileName;
	SaveFileName.Format("线性回归预测运行完成，保存的文件名为：%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
	CRsltElementText *pText = new CRsltElementText("线性回归预测");//申请一个表对象类的对象
	pText->AddString(SaveFileName);
	m_pResult->Add(pText);
	return m_pResult;
}

bool CRegressionLinear::GetPredValueVo(CDataInterface * pDataInterface, string VO)//m_strModel作为模型已经在算法中存在了
{
//	REGLINEARPRED
//		/DATA=[FILE]
//		/MODELFROMFILE [FILE]
//		/VARLIST [varlist]
//		/PREDNAME [strname]
// 		/REPLACEVARNAME [nameorginal namepred]...
//		/PREDRANGE CIN(0.95)
	
	bool bOK = false;
	AnaWord VOstr;
	VOstr.Import(VO);
	
//		/MODELFROMFILE [FILE]
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//从文件读模型
	{
		CTString szFileName = AnaSyntax::getSingleStr("/MODELFROMFILE",VOstr);
		if(GetValueFromModel(pDataInterface, szFileName.GetData()))
			bOK = true;
	}
	if (!bOK)
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( "调用的模型有错误。" );
		m_pResult->Add( pWarningTextRslt );
	}
//		/VARLIST [varlist]
	CTStringArray ynames;
	AnaSyntax::getVAR("/VARLIST", VOstr, ynames);
// 		/REPLACEVARNAME [nameorgi namepred]...
	CTStringArray replacestr;
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, replacestr);
	if (replacestr.GetSize()%2 != 0)
		return false;
//按名字排序，从新得到参数值。
	if (!AnaSyntax::Replace(m_strX_nameAry, ynames, replacestr))
		return false;
	pDataInterface->getVarID(m_strX_nameAry, m_tVarPred);
//		/PREDNAME [strname]
	if (AnaSyntax::exist("/PREDNAME", VOstr))
		m_strDepPred = AnaSyntax::getSingleStr("/PREDNAME", VOstr);
	else
		m_strDepPred = m_stry_name+"预测值";
	if (m_strDepPred == "")
		return false;
//		/PREDRANGE CIN(0.95)
	if (AnaSyntax::exist("/PREDRANGE", VOstr))
	{
		m_bPredRange = true;
		m_dConfidence = AnaSyntax::getvalue("/PREDRANGE", "CIN",VOstr);
		if (m_dConfidence>=1 || m_dConfidence<EPS_ACCUR)
			m_dConfidence = 0.95;
	}
	else
		m_bPredRange = false;
	if (ynames.GetSize() != m_strX_nameAry.GetSize())
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "警告!" );
		pWarningTextRslt->AddString("提供的自变量个数多于模型中自变量的个数。" );
		m_pResult->Add( pWarningTextRslt );
	}

	return true;
}

bool CRegressionLinear::GetValueFromModel(CDataInterface* pDataInterface, string Model)
{
	int i=0, nSize=0;
	CTString szText("");
	string szTemp;
	TCLFile theFile;
	
	bool bOpen = theFile.Open(Model.c_str(), TCLFile::modeRead);
	if (!bOpen)
		return false;
	//截尾长度
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//开始写模型参数
	theFile >> m_bWithConst;
	//自变量
	theFile >> nSize;
	m_coefficiency.create(nSize);
	//系数
	for (i=0; i<nSize; i++)
		theFile >> m_coefficiency(i);
	//输入变量
	m_strX_nameAry.RemoveAll();
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szText;
		m_strX_nameAry.Add(szText);
	}
	//矩阵
	int nRow = 0, nCol = 0;
	theFile >> nRow;
	theFile >> nCol;
	m_XtimesX.create(nRow,nCol);
	for (i=0; i<nRow; i++)
	{
		for (int j=0; j<nCol; j++)
		{
			theFile >> m_XtimesX(i, j);
		}
	}
	//目标变量
	theFile >> m_stry_name;
	//残差
	theFile >> m_MSE;
	theFile.Close();
	return true;
}
