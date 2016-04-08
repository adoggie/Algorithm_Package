// PCR.cpp: implementation of the CPrinCompReg class.
//
//创建者:葛泽慧          创建时间：05.10.25
//文件的用途：主成分回归分析
//文件的最近修改时间，修改的地方：
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "PCR.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double eps=0.0000000001;
CPrinCompReg::CPrinCompReg()
{
	m_pPCRResult =		NULL;
	m_bOutputModel =	false;
	m_pResult =			NULL;
}

CPrinCompReg::~CPrinCompReg()
{
	
}

BOOL CPrinCompReg::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//PCREG
//	/DATA=[FILE]
//	/DEPENDENT [varlist]
//	/INDEPENDENT [varlist]
//	/MISSING [{DELETE**},{AVE}]
//	/OPTION [NOCONST] [NOPRINTPC]
//	/METHOD [{PCNUM({2**})},{CUM({0.8**})},{SINGLE({0.01**})},{OTHERS}]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	CTStringArray ynames;
	AnaSyntax::getVAR("/DEPENDENT", VOstr, ynames);
	if (!(pDataInterface->getVarID(ynames, m_tVarInfo_D)))
		return FALSE;
	if (m_tVarInfo_D.iCount != 1)
		return FALSE;
	CTStringArray xname;
	AnaSyntax::getVAR("/INDEPENDENT", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo))) 
		return FALSE;
	
	m_nMissingMethod = !AnaSyntax::findstr("/MISSING", "AVE", VOstr);
	m_bWithConst = !AnaSyntax::findstr("/OPTION", "NOCONST", VOstr);
	m_bWithPC = !AnaSyntax::findstr("/OPTION", "NOPRINTPC", VOstr);
	
	//读取取个数的方法	
	if (AnaSyntax::findstr("/METHOD", "PCNUM",VOstr))
	{
		m_ncMethod = 0;
		m_ncValue = AnaSyntax::getvalue("/METHOD", "PCNUM",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "CUM",VOstr))
	{
		m_ncMethod = 1;
		m_dcValueCalContr = AnaSyntax::getvalue("/METHOD", "CUM",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "SINGLE",VOstr))
	{
		m_ncMethod = 2;
		m_dcValueCorrEigVal = AnaSyntax::getvalue("/METHOD", "SINGLE",VOstr);
	}
	else if (AnaSyntax::findstr("/METHOD", "OTHERS",VOstr))
		m_ncMethod = 3;
	else
		return FALSE;
	
	return TRUE;
}

//********************************************************************主函数开始
CResult	*CPrinCompReg::OnPCR(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface =	pDataInterface;
	CDoubleMatrix Matrix_X, Matrix_y;
	CResult * pResult;
	pResult	= new CResult("主成分回归分析");		
	m_pResult = pResult;
	
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	m_nXCol = m_tVarInfo.iCount;
	GetMatrixData(&Matrix_X, &Matrix_y);//取数据，放入矩阵Matrix_X&Matrix_y
	if (m_nXRow < (m_nXCol-1))
	{
		CTString strWarning = "样本数太少，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	else
	{
		CDoubleMatrix		SX;   //(X-EX)'*(X-EX)
		CDoubleMatrix		Sy;   //(X-EX)'*y
		CDoubleVector		X_mean;
		CDoubleVector		y_mean;
		X_mean	=			mean(Matrix_X);
		y_mean	=			mean(Matrix_y);
		if (m_bWithConst)
		{
			//中心化begin
			CDoubleVector ONES(m_nXRow,1);
			Matrix_y[0]-=y_mean(0)*ONES;
			for (int i=0;i<m_nXCol;i++)
				Matrix_X[i]-=X_mean[i]*ONES;	
			SX=Matrix_X.dot(Matrix_X);
			Sy=Matrix_X.dot(Matrix_y);
			//中心化end
		}
		else
		{
			SX=Matrix_X.dot(Matrix_X);//SX=X'*X
			Sy=Matrix_X.dot(Matrix_y);//SY=X'*y
		}
		//求相关系数矩阵
		if (m_bWithConst)
		{
			m_Matrix_R=SX/sqrt(Multi(diag(SX),diag(SX)));									
		}
		else
		{	
			CDoubleVector ONES(m_nXRow,1.0);
			CDoubleMatrix X_Tem(m_nXRow,m_nXCol,0.0);
			for (int i=0;i<m_nXCol;i++)
				X_Tem[i]=Matrix_X[i]-X_mean[i]*ONES;
			// 			    Matrix_X-=X_Mean;
			// 				CDoubleMatrix MatTem=X_Tem.trans();
			CDoubleMatrix MatTem2;
			MatTem2=X_Tem.dot(X_Tem);
			m_Matrix_R=MatTem2/sqrt(Multi(diag(MatTem2),diag(MatTem2)));
		}
///////////////////////////////////////////////////
////      根据cMethod选择m_ncValue      ///////////
///////////////////////////////////////////////////
//begin->
//求标准化后的相关矩阵的特征值特征向量
		m_EigVector=m_Matrix_R;	
		m_EigValue=EigValPlusVec(&m_EigVector, 1);
		// 			TraceMatrix(m_EigValue);
		switch (m_ncMethod)
		{
		case 0://自定义
			break;
		case 1://累计贡献率
			{
				int 		count=0;
				double	sum=0;
				double	calContrST=m_nXCol*m_dcValueCalContr;
				while ( (count<m_nXCol) && (sum < calContrST) )
				{
					sum+=m_EigValue(count,0); 
					count++;
				}
				m_ncValue=count;
				break;
			}
		case 2://对应特征值不太小
			{
				int count=0;
				while ( (count<m_nXCol) && (m_EigValue(count,0)>m_dcValueCorrEigVal) )
					count++;
				m_ncValue=count;	
				break;				
			}
			
		case 3:	//其他，见帮助文档
			{
				int count=0; 
				double	sum=0;
				while ( (count<m_nXCol) && ((sum+1/m_EigValue(count,0)) <= 5*(count+1)) )
				{
					sum+=1/m_EigValue(count,0);	
					count=count+1;	
				}
				m_ncValue=count; 
				break;
			}
		default: 
			m_ncValue=2;
		}
		if ( m_ncValue<0 || m_ncValue>m_nXCol)//判断，主成分数不能大于变量数
		{
			CTString strWarning = "错误：主成分数超过变量数。请重新运行！";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
			pWarningTextRslt->AddString( strWarning );
			m_pResult->Add( pWarningTextRslt );
			return pResult;
		}
		//<-end
		
		//////////////////////////////////////////////////
		////主成分回归系数估计//////////////begin/////////
		//////////////////////////////////////////////////
		//begin->
		CDoubleMatrix EigVector=SX;
		CDoubleMatrix EigValue=EigValPlusVec(&EigVector, 1);
		CDoubleMatrix EigValuePC =	EigValue.slice(m_ncValue,1);
		CDoubleMatrix EigVectorPC=	EigVector.slice(m_nXCol,m_ncValue);
		CDoubleMatrix Sy_c =	EigVectorPC.dot(Sy);
		CDoubleMatrix alpha_c =	Sy_c/EigValuePC;
		m_MatCoeff=EigVectorPC*alpha_c;
		//获取相关统计量
		
		if (m_bWithConst)
		{
			m_dINTERCEPT=y_mean(0)-(m_MatCoeff.trans()*X_mean)(0);
			CDoubleVector ONES(m_nXRow,1);
			CDoubleVector y_temp=Matrix_X*m_MatCoeff[0];
			m_dRSS=dot(y_temp,y_temp);
			y_temp=Matrix_y[0]-Matrix_X*m_MatCoeff[0];
			m_dESS=dot(y_temp,y_temp);
		}
		else
		{
			CDoubleVector ONES(m_nXRow,1);
			CDoubleVector y_temp=Matrix_X*m_MatCoeff[0] - y_mean(0)*ONES;
			m_dRSS=dot(y_temp,y_temp);
			y_temp=Matrix_y[0]-Matrix_X*m_MatCoeff[0];
			m_dESS=dot(y_temp,y_temp);
		}
		Matrix_X.destroy();
		Matrix_y.destroy();
		//<-end
		//创建结果集，输出结果
		CreateResult();
		//结构体赋值
		if (m_bOutputModel)//如果要求输出成员变量(模型结果)，则进行下述操作
		{
			m_pPCRResult->m_strVarlist.Copy(m_strVarlist);	
			m_pPCRResult->m_bWithConst =		m_bWithConst;
			m_pPCRResult->m_vectBeta =			MatrixToVector(m_MatCoeff);
			if (m_pPCRResult->m_bWithConst)
				m_pPCRResult->m_vectBeta.insert(0,m_dINTERCEPT);
			m_pPCRResult->m_ncValue =			m_ncValue;
			m_pPCRResult->m_dRSS =				m_dRSS;
			m_pPCRResult->m_dESS =				m_dESS;
			m_pPCRResult->m_Matrix_R =			m_Matrix_R;
			m_pPCRResult->m_ncMethod =			m_ncMethod;
			m_pPCRResult->m_matEigVal =			m_EigValue;
			m_pPCRResult->m_matEigVect =		m_EigVector;
		}
	}
	// 		tt.Toc("pcrtime");
	return m_pResult;
}
///*****************************************************************主函数结束


//////////////////////////////////////////////////////////////////
/////    取特征值和特征向量并排序*****矩阵形式表示          //////
/////    nIndex == 0 从小到大排序 nIndex == 1 从大到小排序  //////
/////    返回特征值m_Row×1矩阵，m_EigVec存储对应特征向量   //////
/////    特征向量矩阵的每一列对应一个特征值                 //////
//////////////////////////////////////////////////////////////////
CDoubleMatrix CPrinCompReg::EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex)
{
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
		*m_EigVec = VectorToMatrix(m_VectEigVector,m_nXCol,m_nXCol).trans();
		return EigValue;
	}
}
///////////////////////////////////////////////////////////
//////////////      创建结果集       //////////////////////
///////////////////////////////////////////////////////////
void CPrinCompReg::CreateResult()
{
	//创建k值信息表
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("主成分个数选择信息表");
	//	pTable->CreateTable(2, 2);
	pTable->SetCols(2);
	pTable->SetRows(2);
	pTable->InsertColumn(0,"方法");
	pTable->InsertColumn(0,"主成分数");
	switch (m_ncMethod)
	{
	case 0:
		pTable->InsertItem(0,"自定义");
		break;
	case 1:
		pTable->InsertItem(0,"累计贡献率判别法");
		break;
	case 2:
		pTable->InsertItem(0,"特征值不太小方法");
		break;
	case 3:		
		pTable->InsertItem(0,"其他（参见帮助）");
		break;
	default: 
		pTable->InsertItem(0,"缺省");
	}
	pTable->SetItemText(0,1,m_ncValue);
	//申请一个表对象类的对象
	CRsltElementTable * pETable= new CRsltElementTable("主成分个数选择信息表",pTable);
	m_pResult->Add(pETable);
	if (m_bWithPC)
	{
		//创建主轴信息表
		pTable = new CTLTable;
		pTable->SetTitle("主轴（对应主成分）信息表");
		pTable->InsertColumn(0,"主轴");
		pTable->InsertColumn(1,"对应特征值");
		pTable->InsertColumn(2,"贡献率");
		pTable->InsertColumn(0,"累计贡献率%");
		int i=0;
		for(i=0;i<m_nXCol;i++)
		{
			//取变量名
			pTable->InsertColumn(i+4,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		}
		double sum=0;
		CTString strTemp;
		for(i=0;i<m_ncValue;i++)
		{
			//主轴
			strTemp.Format("a(%d)",i+1);
			pTable->InsertItem(i,strTemp);
			// 			pTable->SetItemText(i,0,);
			//对应特征值，已标准化的
			pTable->SetItemText(i,1,m_EigValue(i,0));
			//贡献率
			pTable->SetItemText(i,2,(m_EigValue(i,0)/m_nXCol));
			//累计贡献率
			sum+=m_EigValue(i,0);
			pTable->SetItemText(i,3,100*(sum/m_nXCol));			
			for(int j=0;j<m_nXCol;j++)
			{
				pTable->SetItemText(i,j+4,m_EigVector(j,i));
			}
		}
		//申请一个表对象类的对象
		pETable= new CRsltElementTable("主轴（对应主成分）信息表",pTable);
		m_pResult->Add(pETable);		
	}
	
	//创建自变量相关系数表
	pTable = new CTLTable;
	pTable->SetTitle("自变量相关系数表");
	pTable->InsertColumn(0,"相关系数");
	int i=0;
	for(i=0;i<m_nXCol;i++)
	{
		pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]),FALSE);
	}
	for(i=0;i<m_nXCol;i++)
	{
		for(int j=0;j<m_nXCol;j++)
		{
			pTable->SetItemText(i,j+1,m_Matrix_R(i,j));
		}
	}
	pETable= new CRsltElementTable("自变量相关系数表",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	if (m_bWithConst)
	{
		//创建系数估计表
		pTable=new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(m_nXCol+2);
		pTable->SetTitle("回归系数表");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"常数项");
		pTable->InsertItem(0,"回归系数b");
		for (i=0;i< m_nXCol;i++)
		{
			pTable->InsertColumn(i+2,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			pTable->SetItemText(0,i+2, m_MatCoeff(i,0));
		}
		pTable->SetItemText(0,1, m_dINTERCEPT);
		pETable= new CRsltElementTable("回归系数表",pTable);
		m_pResult->Add(pETable);
		
		//创建方差分析表
		pTable = new CTLTable() ;
		pTable->CreateTable(2, 2);
		pTable->SetRows(4);
		pTable->SetCols(6);
		pTable->SetTitle("方差分析表");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"平方和");
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"均方");
		pTable->InsertColumn(4,"F值");
		pTable->InsertColumn(5,"P值");
		pTable->InsertItem(0,"回归");
		pTable->InsertItem(1,"残差");
		pTable->InsertItem(2,"总离差");
		pTable->SetItemText(0,1,(m_dRSS));
		pTable->SetItemText(1,1,(m_dESS));
		pTable->SetItemText(2,1,m_dRSS+m_dESS);
		pTable->SetItemText(0,2,m_nXCol);
		pTable->SetItemText(1,2,m_nXRow-m_nXCol-1);
		pTable->SetItemText(2,2,m_nXRow-1);
		pTable->SetItemText(0,3,m_dRSS/m_nXCol);
		pTable->SetItemText(1,3,m_dESS/(m_nXRow-m_nXCol-1));
		pTable->SetItemText(0,4,m_dRSS*(m_nXRow-m_nXCol-1)/m_dESS/m_nXCol);
		double sig=1 - fcdf(m_dRSS*(m_nXRow-m_nXCol-1)/m_dESS/m_nXCol, m_nXCol,m_nXRow-m_nXCol-1); //modified by liytao 20030813
		pTable->SetItemText(0,5,sig);
		pETable= new CRsltElementTable("方差分析表",pTable);
		m_pResult->Add(pETable);
        //创建模型分析表
		pTable = new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(2);
		pTable->SetTitle("模型分析表");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"复相关系数");
		pTable->InsertColumn(2,"修正的复相关系数");
		pTable->InsertItem(0,"",FALSE);
		pTable->SetItemText(0,1,sqrt(m_dRSS/(m_dRSS+m_dESS)));
		pTable->SetItemText(0,2,sqrt(max(0,1-m_dESS*(m_nXRow-1)/(m_dESS+m_dRSS)/(m_nXRow-m_nXCol-1))));
		pETable= new CRsltElementTable("模型分析表",pTable);
		m_pResult->Add(pETable);
	}
	else
	{	
		//创建系数估计表
		pTable=new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(m_nXCol+1);
		pTable->SetTitle("回归系数表");
		pTable->InsertColumn(0,"");
		// 		pTable->InsertColumn(1,"常数项");
		pTable->InsertItem(0,"回归系数");
		for (i=0;i<m_nXCol;i++)
		{
			pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			pTable->SetItemText(0,i+1, m_MatCoeff(i,0));
		}
		// 		pTable->SetItemText(0,1, INTERCEPT);
		pETable= new CRsltElementTable("回归系数表",pTable);
		m_pResult->Add(pETable);
		//创建方差分析表
		pTable = new CTLTable() ;
		pTable->CreateTable(2, 2);
		pTable->SetRows(4);
		pTable->SetCols(6);
		pTable->SetTitle("方差分析表");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"平方和");
		pTable->InsertColumn(2,"自由度");
		pTable->InsertColumn(3,"均方");
		pTable->InsertColumn(4,"F值");
		pTable->InsertColumn(5,"P值");
		pTable->InsertItem(0,"回归");
		pTable->InsertItem(1,"残差");
		pTable->InsertItem(2,"总离差");
		pTable->SetItemText(0,1,(m_dRSS));
		pTable->SetItemText(1,1,(m_dESS));
		pTable->SetItemText(2,1,m_dRSS+m_dESS);
		pTable->SetItemText(0,2,m_nXCol);
		pTable->SetItemText(1,2,m_nXRow-m_nXCol);
		pTable->SetItemText(2,2,m_nXRow);
		pTable->SetItemText(0,3,m_dRSS/m_nXCol);
		pTable->SetItemText(1,3,m_dESS/(m_nXRow-m_nXCol));
		pTable->SetItemText(0,4,m_dRSS*(m_nXRow-m_nXCol)/m_dESS/m_nXCol);
		double sig=1 - fcdf(m_dRSS*(m_nXRow-m_nXCol)/m_dESS/m_nXCol, m_nXCol,m_nXRow-m_nXCol); //modified by liytao 20030813
		pTable->SetItemText(0,5,sig);
		pETable= new CRsltElementTable("方差分析表",pTable);
		m_pResult->Add(pETable);
        //创建模型分析表
		pTable = new CTLTable();
		pTable->CreateTable(2, 2);
		pTable->SetRows(2);
		pTable->SetCols(2);
		pTable->SetTitle("模型分析表");
		pTable->InsertColumn(0,"");
		pTable->InsertColumn(1,"复相关系数");
		pTable->InsertColumn(2,"修正的复相关系数");
		pTable->InsertItem(0,"");
		//		pTable->InsertItem(0,sqrt(m_dRSS/TSS));
		pTable->SetItemText(0,1,sqrt(m_dRSS/(m_dRSS+m_dESS)));
		pTable->SetItemText(0,2,sqrt(max(0,1-m_dESS*(m_nXRow)/(m_dESS+m_dRSS)/(m_nXRow-m_nXCol))));
		pETable= new CRsltElementTable("模型分析表",pTable);
		m_pResult->Add(pETable);
	}
	
	//创建回归方程
	CTString strEquation=m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0])+"=";	
	CTString strTemp;	
	if(m_bWithConst)
	{
		strTemp.Format("%.4f",m_dINTERCEPT);
		strEquation+=strTemp;
		for(int i=0;i<m_nXCol;i++)
		{
			if(m_MatCoeff(i,0)>0)
			{
				strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			else
			{
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			strEquation+=strTemp;
		}
	}
    else
	{
		for(int i=0;i<m_nXCol;i++)
		{
			if (i==0)
			{
				strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
			}
			else
			{
				if(m_MatCoeff(i,0)>=0)
				{
					strTemp.Format("+%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
				}
				else
				{
					strTemp.Format("%.4f*%s",m_MatCoeff(i,0),m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]).GetData());
				}
			}
			strEquation+=strTemp;	
		}	
	}
	pTable = new CTLTable;
	pTable->CreateTable(2, 2);	
	pTable->SetTitle("回归方程");
	pTable->InsertColumn(0,strEquation);
	pETable= new CRsltElementTable("回归方程",pTable);
	m_pResult->Add(pETable);
}

//////////////////////////////////////////////////////////////////////////////////
///////     取数据，并处理缺失值，将数据放入矩阵Matrix_X&Matrix_y
//////      规划，将缺失值处理封装，放入一个public类库，以后每个算法可以直接调用
//////////////////////////////////////////////////////////////////////////////////
void CPrinCompReg::GetMatrixData(CDoubleMatrix* Matrix_X, CDoubleMatrix* Matrix_y)
{
	if (m_nMissingMethod==0)
	{
		//处理缺失值，填均值
		TCDataMatrix  pDataSet;
		if( m_nXCol >0 )
		{
			CDWordArray indexary;
			//TDataSet * pDataArray = NULL;
			indexary.Add(m_tVarInfo_D.pValue[0]);
			m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0]));				
			for( int i=0; i<m_nXCol; i++ )
			{			
				indexary.Add(m_tVarInfo.pValue[i]);
				m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			}
			m_pDataInterface->GetColsData( indexary, pDataSet, 3 );//取回数据
		}
		//转化成矩阵，存入Matrix_X,Matrix_y
		m_nXRow =pDataSet[0]->GetLen();
		(*Matrix_y)=CDoubleMatrix(m_nXRow,1,0.0);
		(*Matrix_X)=CDoubleMatrix(m_nXRow, m_nXCol,0.0);
		int i=0; 
		for( i=0; i<m_nXRow; i++ )
		{
			//取因变量数据放入矩阵Matrix_y
			(*Matrix_y)(i,0) = pDataSet[0]->GetAt(i).dValue;
			for( int j=0;j<m_nXCol;j++)
			{
				//取自变量数据放入矩阵Matrix_X
				(*Matrix_X)(i,j) = pDataSet[j+1]->GetAt(i).dValue;
			}
			
		}
		//释放内存
		for( i =0;i<pDataSet.size();i++ )
		{
			delete pDataSet[i];
		}		
	}
	else
	{
		//处理缺失值，整行删除
		TDColAry* pyDataSet=new TDColAry();
		//CPtrArray* pxarrayDataSet;
		TCDataMatrix  pxarrayDataSet;
		int nCaseNum = m_pDataInterface->GetColData(m_tVarInfo_D.pValue[0],*pyDataSet,5);
		m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo_D.pValue[0]));				
		if( m_nXCol>0 )
		{
			CDWordArray indexary;
			for( int i=0; i<m_nXCol; i++ )
			{			
				indexary.Add(m_tVarInfo.pValue[i]);
				m_strVarlist.Add(m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));					
			}
			m_pDataInterface->GetColsData( indexary, pxarrayDataSet, 5 );//取回数据
		}
		vector<int> arrayMissingValue;

		//CArray<int,int> arrayMissingValue;
		int i=0;
		for(i=0;i<nCaseNum;i++)
		{
			if(pyDataSet->GetAt(i).iIndex<0)
			{
				arrayMissingValue.push_back(i);
				continue;
			}
			for(int j=0;j<m_nXCol;j++)
			{
				if(pxarrayDataSet[j]->GetAt(i).iIndex<0)
				{
					arrayMissingValue.push_back(i);
					break;
				}
			}
		}
		//转化成矩阵，存入Matrix_X,Matrix_y
		m_nXRow= nCaseNum - arrayMissingValue.size();
		(*Matrix_y)=CDoubleMatrix(m_nXRow,1,0.0);
		(*Matrix_X)=CDoubleMatrix(m_nXRow, m_nXCol,0.0);
		BOOL Flag = TRUE;
		int  nCnt = 0;
		for(i=0; i<nCaseNum; i++ )
		{
			int j=0;
			for( j=0;j<arrayMissingValue.size();j++ )
			{
				if(arrayMissingValue[j]==i)
					Flag = FALSE;
			}
			if(Flag==TRUE)
			{
				//取因变量数据放入矩阵Matrix_y
				(*Matrix_y)(nCnt,0) = pyDataSet->GetAt(i).dValue;
				for( j=0;j<m_nXCol;j++)
				{
					//取自变量数据放入矩阵Matrix_X
					(*Matrix_X)(nCnt,j) = pxarrayDataSet[j]->GetAt(i).dValue;
				}
				nCnt++;
			}
			Flag = TRUE;
		}
		//释放内存
		for(i =0;i<pxarrayDataSet.size();i++ )
		{
			delete pxarrayDataSet[i];
		}		    
		pxarrayDataSet .clear();
		delete pyDataSet;
	}
}


//
//////////////////////////////////////////////////////////////////////////////
////创建者:葛泽慧          创建时间：05.10.25
////方法的用途：专用于求矩阵转秩后相乘，Z=X'*Y
////文件的最近修改时间，修改的地方：
//////////////////////////////////////////////////////////////////////////////
//CDoubleMatrix CPrinCompReg::TranProd(CDoubleMatrix &X, CDoubleMatrix &Y)
//{
//	//int m_nRowX=X.mrow();
//	int m_nColX=X.mcol();
//	int m_nColY=Y.mcol();
//	CDoubleMatrix Z(m_nColX,m_nColY);
//	if (X.mrow()!=Y.mrow())
//		AfxMessageBox("矩阵维数不匹配！");
//	else
//		for (int i=0;i<m_nColX;i++)
//			for (int j=0;j<m_nColY;j++)
//				Z(i,j)=X[i].dot(Y[j]);
//			return Z;
//}

//int CPrinCompReg::GetCurSec()//取得运行时间——秒
//{  
//	time_t time0;
//	// 		struct _timeb tstruct;
//	struct tm *pNow;
//	_tzset();
//	time( &time0 );
//	
//	//		_ftime( &tstruct );
//	pNow = localtime( &time0 );
//	// 		TRACE( "Current Time:\t\t\t%.8s", 	asctime( pNow )+11 );
//	return pNow->tm_sec+pNow->tm_min*60+pNow->tm_hour*3600;
//	//	    TRACE( ":%u\n", tstruct.millitm );
//}








