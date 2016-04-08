// MarkovChain.cpp: implementation of the CMarkovChain class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "tlAdvn.h"
#include "MarkovChain.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMarkovChain::CMarkovChain()
{
}

CMarkovChain::~CMarkovChain()
{
}

BOOL CMarkovChain::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
//MARKOVCHAIN
//	/DATA=[FILE]
//	/USERID [varname]
//	/PATH [varname]
//	/TIMEVAR [varname]
//	/OPTION [{CLUSTERNUMBER(5**)},{AUTOCLUSTER}] [MAXITERATION(50**)] [CONVERGE(0.0001**)]
//	/SAVEMODEL [FILE]
//	/SAVEDATA [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);
	
	TIntStruct VarID;
	CTStringArray userid;
	AnaSyntax::getVAR("/USERID", VOstr, userid);
	if (!(pDataInterface->getVarID(userid, VarID)))
		return FALSE;
	if (VarID.iCount != 1)
		return FALSE;
	else
		m_nUserID = VarID.pValue[0];

	CTStringArray path;
	AnaSyntax::getVAR("/PATH", VOstr, path);
	if (!(pDataInterface->getVarID(path,VarID))) 
		return FALSE;
	if (VarID.iCount != 1)
		return FALSE;
	else
		m_nPath = VarID.pValue[0];

	if (m_nPath<0 || m_nUserID<0)
		return FALSE;
	
	if (AnaSyntax::exist("/TIMEVAR", VOstr) )
	{
		CTStringArray timev;
		AnaSyntax::getVAR("/TIMEVAR", VOstr, timev);
		if (!(pDataInterface->getVarID(path,VarID))) 
			return FALSE;
		if (VarID.iCount != 1)
			return FALSE;
		else
			m_nTime = VarID.pValue[0];
	}
	else
		m_nTime = -1;
	
	m_bAutoCluster = AnaSyntax::findstr("/OPTION", "AUTOCLUSTER", VOstr);
	if (AnaSyntax::findstr("/OPTION", "CLUSTERNUMBER", VOstr) && !m_bAutoCluster)
		m_nCluster = AnaSyntax::getvalue("/OPTION", "CLUSTERNUMBER", VOstr);
	else
		m_nCluster = 3;

	m_dMinConverge = 0.0001;
	if (AnaSyntax::findstr("/OPTION", "CONVERGE", VOstr))
		m_dMinConverge = AnaSyntax::getvalue("/OPTION", "CONVERGE", VOstr);

	m_nMaxIter = 50;
	if (AnaSyntax::findstr("/OPTION", "MAXITERATION", VOstr))
		m_nMaxIter = AnaSyntax::getvalue("/OPTION", "MAXITERATION", VOstr);

	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL", VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL", VOstr);

	m_bSaveData = AnaSyntax::exist("/SAVEDATA", VOstr);
	if (m_bSaveData)
		m_DataFile = AnaSyntax::getSingleStr("/SAVEDATA", VOstr);

	return TRUE;
}

CResult * CMarkovChain::OnCMarkovChain(CDataInterface *pDataInterface, string VO)
{
	CResult * pResult;
	pResult = new CResult("马尔可夫链");
	m_pResult = pResult;
	m_pDataInterface = pDataInterface;

	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	CDWordArray indexary;
	CDoubleMatrix Data_Matrix;
	indexary.Add(m_nUserID);
	indexary.Add(m_nPath);
//	if (m_nTime > -1)
//		indexary.Add(m_nTime);

	m_nRow = pDataInterface->GetColsData(indexary, Data_Matrix, 0);//取回界面数据
//	if (m_nTime > -1)
//	{
//		CDoubleVector tempSort_vec = Data_Matrix(2);
//		CIntVector tempSort_id(m_nRow);
//		tempSort_vec.Sort(tempSort_id);
//		m_ID_vec.create(m_nRow);
//		m_Path_vec.create(m_nRow);
//		for (int j=0; j<m_nRow; j++)
//		{
//			m_ID_vec(j) = Data_Matrix(0, tempSort_id(j)-1);
//			m_Path_vec(j) = Data_Matrix(1, tempSort_id(j)-1);
//		}
//	}
//	else
	{
		m_ID_vec = Data_Matrix(0);
		m_Path_vec = Data_Matrix(1);
	}
	
	if (Main() )
	{
		DrawResult();
	}
	else
	{
		CTString strWarning = "数据无法计算，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return pResult;
	}

//	if (m_bSaveModle)
//	{
//		...
//	}

	return pResult;
}




BOOL CMarkovChain::EMalgorithm(CDoubleVector& Pi_vec, CDoubleMatrix& Theta_Init_mx, 
							   CDoubleMatrix* Theta_Trans, CDoubleMatrix& responsibility,
							   CDoubleVector start_vec, CDoubleVector length_vec, 
							   CDoubleVector pairID_vec, int KClusters, int cutting)
{
	int i,j,l,k,t,L;
	double alpha_IT = 0.01 / m_nPageNum;
	

	// (Compute the log-likelihood) begin
	//
	double old = -1.0e15;
	double loglikelihood = 0;
	double constant = 10000;
	for (i=0; i<cutting; i++)
	{
		double like = 0;
		for (k=0; k<KClusters; k++)
		{
			L = length_vec(i);
			double temp = 1;
			for (t=start_vec(i); t<start_vec(i)+L-1; t++)
			{
				int st=m_Path_vec(t)-1;
				int ed=m_Path_vec(t+1)-1;
				double ttt = Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1);
				temp = temp * ttt;
			}
			like = like + temp * Pi_vec(k) * Theta_Init_mx(k, m_Path_vec(start_vec(i))-1);
		}
		loglikelihood += log(like);
	}
	// (Compute the log-likelihood) end
	//
	
	// (EM algorithm) begin
	//

	//while (abs(loglikelihood-old)>=abs(m_dMinConverge*loglikelihood))
	for (int iter=0; iter<m_nMaxIter; iter++)
	{
		if (::abs(loglikelihood-old) < ::abs(m_dMinConverge*loglikelihood))
			break;
		
		old=loglikelihood;
		// E step
		for (i=0; i<cutting; i++)
		{
			L=length_vec(i);
			for (k=0; k<KClusters; k++)
			{
				double temp = 1;
				for (t=start_vec(i); t<start_vec(i)+L-1; t++)
				{
					temp *= Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1);
				}
				responsibility(i,k) = constant * Pi_vec(k) * temp * Theta_Init_mx(k,m_Path_vec(start_vec(i))-1);
			}
		}
		for (i=0; i<cutting; i++) // Normalization of the responsibilities
		{
			double s = 0;
			for (k=0; k<KClusters; k++)
			{
				s = s + responsibility(i,k);
			}
			for (k=0; k<KClusters; k++)
			{
				responsibility(i,k) = responsibility(i,k)/s;
			}
		}
		// M step

		// Pi
		for (k=0; k<KClusters; k++)
		{
			double temp = 0;
			for (i=0; i<cutting; i++)
			{
				temp = temp + responsibility(i,k);
			}
			Pi_vec(k) = temp;
		}
		double sss = Pi_vec.sum();
		for (k=0; k<KClusters; k++)
		{
			Pi_vec(k) = Pi_vec(k)/sss;
		}
		// Theta_Init
		for (k=0; k<KClusters; k++)
		{
			for (j=0; j<m_nPageNum; j++)
			{
				double temp = 0;
				for (i=0; i<cutting; i++)
				{
					if (m_Path_vec(start_vec(i))-1==j)
					{
						temp += responsibility(i,k);
					}
				}
				Theta_Init_mx(k,j) = temp + alpha_IT;
			}
		}
		for (k=0; k<KClusters; k++) // normalization
		{
			double s = 0;
			for (j=0; j<m_nPageNum; j++)
			{
				s += Theta_Init_mx(k,j);
			}
			for (j=0; j<m_nPageNum; j++)
			{
				Theta_Init_mx(k,j) = Theta_Init_mx(k,j)/s;
			}
		}
		
		// Theta_Trans
		for (k=0; k<KClusters; k++) // reset
		{
			for (j=0; j<m_nPageNum; j++)
			{
				for (l=0; l<m_nPageNum; l++)
				{
					Theta_Trans[k](j,l) = alpha_IT;
				}
			}
		}
		for (k=0; k<KClusters; k++)
		{
			for (i=0; i<cutting; i++)
			{
				L=length_vec(i);
				for (t=start_vec(i); t<start_vec(i)+L-1; t++)
				{
					Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1) = Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1) + responsibility(i,k);
				}
			}
		}
		for (k=0; k<KClusters; k++) // normalization
		{
			for (j=0; j<m_nPageNum; j++)
			{
				double s = 0;
				for (l=0; l<m_nPageNum; l++)
				{
					s = s + Theta_Trans[k](j,l);	
				}
				for (l=0; l<m_nPageNum; l++)
				{
					Theta_Trans[k](j,l) = Theta_Trans[k](j,l)/s;
				}
			}
		}

		// recompute the log-likelihood
		loglikelihood = 0;
		for (i=0; i<cutting; i++)
		{
			double like = 0;
			for (k=0; k<KClusters; k++)
			{
				L = length_vec(i);
				double temp = 1;
				for (t=start_vec(i); t<start_vec(i)+L-1; t++)
				{
					temp *= Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1);
				}
				like += temp * Pi_vec(k) * Theta_Init_mx(k,m_Path_vec(start_vec(i))-1);
			}
			loglikelihood += log(like);
		}
				
	}
	// (EM algorithm) end
	//
	return TRUE;
}

BOOL CMarkovChain::Main()
{
	int id = 0;
	int L = 0;
	int i = 0;
	int j,k,l,t;
	m_nPageNum = m_Path_vec.Max();
	CDoubleVector start_vec(m_nRow);
	CDoubleVector pairID_vec(m_nRow);
	CDoubleVector length_vec(m_nRow, 0);
	
// (Preprocessing) begin
//
	while (i < m_nRow) 
	{
		L=0;
		start_vec(id) = i;
		pairID_vec(id) = m_ID_vec(i);
		while (pairID_vec(id) == m_ID_vec(i)) 
		{
			length_vec(id)++;
			i++;
			if (i >= m_nRow) 
			{
				break;
			}
		}
		id++;
	}
	m_nTotalID = id;
    start_vec.resize(m_nTotalID);
	pairID_vec.resize(m_nTotalID);
	length_vec.resize(m_nTotalID);
	CDoubleVector score(28);
// (Preprocessing) end
//

// (Model selection) begin
//
	if (m_bAutoCluster)//如果自动计算KClusters
	{
		int cutting  = min(0.4 * m_nTotalID, 5000);
		int KClusters;
		
		for (KClusters=3; KClusters<15; KClusters++)
		{
			CDoubleMatrix responsibility(cutting, KClusters);
			// (Initialization) begin
			CDoubleVector Pi_vec(KClusters, 1.0/KClusters);
			
			CDoubleMatrix Theta_Init_mx(KClusters, m_nPageNum);
			srand(GetTickCount());
			for (k=0; k<KClusters; k++)
			{
				for (j=0; j<m_nPageNum; j++)
				{
					Theta_Init_mx(k,j) = rand();
				}
			}
			for (k=0; k<KClusters; k++) 
			{
				double s = 0;
				for (j=0; j<m_nPageNum; j++)
				{
					s += Theta_Init_mx(k,j);
				}
				for (j=0; j<m_nPageNum; j++)
				{
					Theta_Init_mx(k,j) = Theta_Init_mx(k,j)/s;
				}
			}
			
			CDoubleMatrix * Theta_Trans= new CDoubleMatrix[KClusters];
			
			for (k=0; k<KClusters; k++)
			{		
				CDoubleMatrix Theta_Trans_mx(m_nPageNum, m_nPageNum);
				for (i=0; i<m_nPageNum; i++)
				{
					for (j=0; j<m_nPageNum; j++)
					{
						Theta_Trans_mx(i, j) = rand();
					}
				}
				Theta_Trans[k] = Theta_Trans_mx;
			}
			for (k=0; k<KClusters; k++) 
			{
				for (j=0; j<m_nPageNum; j++)
				{
					double s = 0;
					for (l=0; l<m_nPageNum; l++)
					{
						s += Theta_Trans[k](j,l);
					}
					for (l=0; l<m_nPageNum; l++)
					{
						Theta_Trans[k](j,l) = Theta_Trans[k](j,l)/s;
					}
				}
			}
			// (Initialization) end
			EMalgorithm(Pi_vec, Theta_Init_mx, Theta_Trans, responsibility,
				start_vec, length_vec, pairID_vec, KClusters, cutting);
			// (Compute Score) begin
			double LS2 = 0;
			double total_length = 0;
			
			for (i=cutting; i<m_nTotalID; i++)
			{
				double like = 0;
				for (k=0; k<KClusters; k++)
				{
					L = length_vec(i);
					double temp = 1;
					for (t=start_vec(i); t<start_vec(i)+L-1; t++)
					{
						temp *= Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1);
					}
					like += temp * Pi_vec(k) * Theta_Init_mx(k,m_Path_vec(start_vec(i))-1);
				}
				LS2 += log(like)/log(2);
			}
			for (i=0; i<m_nTotalID; i++)
			{
				total_length += length_vec(i);
			}
			score(KClusters-3) = -(LS2/total_length)+(0.01*KClusters);
			
			// (Compute Score) end
		}
	}

	for (i=0; i<28; i++)
	{
		if (score(i) < score(m_nCluster-3))
		{
        	m_nCluster = i+3;
		}
	}


// (Model selection) end
//

// (Training) begin 
//

	CDoubleMatrix responsibility(m_nTotalID, m_nCluster);

    // (Initialization)

	CDoubleVector Pi_vec(m_nCluster, 1.0/m_nCluster); // Pi
	
	CDoubleMatrix Theta_Init_mx(m_nCluster, m_nPageNum); // Theta_Init
	srand(GetTickCount());
	for (k=0; k<m_nCluster; k++)
	{
		for (j=0; j<m_nPageNum; j++)
		{
			Theta_Init_mx(k,j) = rand();
		}
	}
	for (k=0; k<m_nCluster; k++) // normalization
	{
		double s = 0;
		for (j=0; j<m_nPageNum; j++)
		{
			s += Theta_Init_mx(k,j);
		}
		for (j=0; j<m_nPageNum; j++)
		{
			Theta_Init_mx(k,j) = Theta_Init_mx(k,j)/s;
		}
	}

	CDoubleMatrix * Theta_Trans= new CDoubleMatrix[m_nCluster]; // Theta_Trans
	for (k=0; k<m_nCluster; k++)
	{		
		CDoubleMatrix Theta_Trans_mx(m_nPageNum, m_nPageNum);
		for (i=0; i<m_nPageNum; i++)
		{
			for (j=0; j<m_nPageNum; j++)
			{
				Theta_Trans_mx(i, j) = rand();
			}
		}
		Theta_Trans[k] = Theta_Trans_mx;
	}
	for (k=0; k<m_nCluster; k++) // normalization
	{
		for (j=0; j<m_nPageNum; j++)
		{
			double s = 0;
			for (l=0; l<m_nPageNum; l++)
			{
				s += Theta_Trans[k](j,l);
			}
			for (l=0; l<m_nPageNum; l++)
			{
				Theta_Trans[k](j,l) = Theta_Trans[k](j,l)/s;
			}
		}
	}
	// (hyperparameter) 
    double alpha_IT = 0.01 / m_nPageNum;
	
	// EM training
	EMalgorithm(Pi_vec, Theta_Init_mx, Theta_Trans, responsibility,
					start_vec, length_vec, pairID_vec, m_nCluster, m_nTotalID);
// (Training) end
//

// (Hard assignment) begin
//
	m_VecClus.create(m_nTotalID);
	for (i=0; i<m_nTotalID; i++)
	{
		int which = 0;
		for (k=0; k<m_nCluster; k++)
		{
			if (responsibility(i, which) > responsibility(i,k))
			{
				which = k;
			}
		}
		m_VecClus(i) = which;
	}

	//输出模型信息到 m_strModel
	CTString strTemp;
	m_strModel = "";
	strTemp.Format("%d", m_nCluster);
	m_strModel += strTemp;
	m_strModel += " ";
	strTemp.Format("%d", m_nPageNum);
	m_strModel += strTemp;
	m_strModel += " ";
	for (k=0; k<m_nCluster; k++)
	{
		for (j=0; j<m_nPageNum; j++)
		{
			for (i=0; i<m_nPageNum; i++)
			{
				strTemp.Format("%.9f", Theta_Trans[k](i,j));
				m_strModel += strTemp;
				m_strModel += " ";
			}
		}
	}	//输出模型信息到 m_strModel 完成

	if (m_bSaveModel)//保存模型
		CFileReadWrite::WriteStrToFile(m_ModelFile, m_strModel);

// (Hard assignment) end
//
	return TRUE;
}

void CMarkovChain::DrawResult()
{
	if (m_nTotalID > 1000 || m_bSaveData)
	{
		//在主界面输出聚类结果
		int i;
		CDataAccess DataAcc;
		CField *pUserIdNum = DataAcc.m_pFieldList->CreateField(fInt);
		CField *pCluster = DataAcc.m_pFieldList->CreateField(fInt);
		pUserIdNum->SetFieldName("用户序号");
		pCluster->SetFieldName("分类结果");
		DataAcc.m_pFieldList->Add(pUserIdNum);
		DataAcc.m_pFieldList->Add(pCluster);
		for ( i=0; i<m_nTotalID; i++)
		{
			DataAcc.Append();
			pUserIdNum->SetAsInteger(i+1);
			pCluster->SetAsInteger(m_VecClus(i) );
		}
		BOOL bSucSave;//保存文件
		CTString NoUse, FileName;
		if (m_bSaveData)
		{
			bSucSave = DataAcc.SaveFile(m_DataFile, &NoUse);
			FileName = m_DataFile;
		}
		else
		{
			CTString strOutNameTemp = "_马尔可夫链聚类结果";
			CTString strOrgNameTemp = m_pDataInterface->m_DataAccess.GetFileName();
			FileName = CFileReadWrite::GetSaveFileName(strOrgNameTemp, strOutNameTemp);
			bSucSave = DataAcc.SaveFile(FileName, &NoUse);
		}

		CRsltElementText * pWarningTextRslt = new CRsltElementText( "马尔可夫链分类结果" );
		CTString strWarn;
		if (bSucSave)
			strWarn = "模型运行结束，结果已经成功输出到"+FileName+"文件里。";
		else
			strWarn = "模型运行结束，结果已经成功输出到"+NoUse+"文件里。";
		pWarningTextRslt->AddString(strWarn);
		m_pResult->Add( pWarningTextRslt );
		return;
	}
	else
	{
		CTLTable * pTable = new CTLTable;
		CRsltElementTable * pETable = new CRsltElementTable("马尔可夫链分类结果",pTable);//申请一个表对象类的对象
		pTable->CreateTable(2, 2);
		pTable->SetTitle("马尔可夫链分类情况");

		pTable->SetCols(2);
		pTable->SetRows(m_nTotalID+1);
		pTable->InsertColumn(0,"用户序号");
		pTable->InsertColumn(1,"分类结果");
		CTString str;
		for(int i=0;i<m_nTotalID;i++)
		{
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			pTable->SetItemText(i, 1, m_VecClus(i) );
		}
		m_pResult->Add(pETable);
		return;
	}
}

CResult * CMarkovChain::OnMorkovChainPred(CDataInterface *pDataInterface, string VO)//外部调用预测函数
{
	m_pResult = new CResult("线性回归模型预测结果");
	m_pDataInterface = pDataInterface;
	if (!GetPredValueVo(pDataInterface, VO))//读参数及模型
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( "模型参数有误，无法运行。" );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
//读取数据
	CDWordArray indexary;
	CDoubleMatrix Data_Matrix;
	indexary.Add(m_nUserID);
	indexary.Add(m_nPath);
	m_nRow = pDataInterface->GetColsData(indexary, Data_Matrix, 0);//取回界面数据
	m_ID_vec = Data_Matrix(0);
	m_Path_vec = Data_Matrix(1);
	CDoubleVector start_vec(m_nRow);
	CDoubleVector pairID_vec(m_nRow);
	CDoubleVector length_vec(m_nRow, 0);
	int i=0, L, id=0;
// 数据准备(Preprocessing) begin
//
	while (i < m_nRow) 
	{
		L=0;
		start_vec(id) = i;
		pairID_vec(id) = m_ID_vec(i);
		while (pairID_vec(id) == m_ID_vec(i)) 
		{
			length_vec(id)++;
			i++;
			if (i >= m_nRow) 
			{
				break;
			}
		}
		id++;
	}
	m_nTotalID = id;
    start_vec.resize(m_nTotalID);
	pairID_vec.resize(m_nTotalID);
	length_vec.resize(m_nTotalID);
//数据准备(Preprocessing) end

//开始计算
	int k, t;
	CDoubleMatrix responsibility(m_nTotalID, m_nCluster);
	for (i=0; i<m_nTotalID; i++)
	{
		L=length_vec(i);
		for (k=0; k<m_nCluster; k++)
		{
			double temp = 1;
			for (t=start_vec(i); t<start_vec(i)+L-1; t++)
			{
				temp *= m_Theta_Trans[k](m_Path_vec(t)-1,m_Path_vec(t+1)-1);
			}
			responsibility(i,k) = temp;
		}
	}
	for (i=0; i<m_nCluster; i++) // Normalization of the responsibilities
	{
		double s = 0;
		for (k=0; k<m_nCluster; k++)
		{
			s = s + responsibility(i,k);
		}
		for (k=0; k<m_nCluster; k++)
		{
			responsibility(i,k) = responsibility(i,k)/s;
		}
	}

	m_VecClus.create(m_nTotalID);
	for (i=0; i<m_nTotalID; i++)
	{
		int which = 0;
		for (k=0; k<m_nCluster; k++)
		{
			if (responsibility(i, which) > responsibility(i,k))
			{
				which = k;
			}
		}
		m_VecClus(i) = which;
	}
//完成计算
	DrawResult();
	return m_pResult;
}

BOOL CMarkovChain::GetPredValueVo(CDataInterface *pDataInterface, string VO)
{
//	MARKOVCHPRED
//		/DATA=[FILE]
//		/USERID [varname]
//		/PATH [varname]
//		/MODELFROMFILE [FILE]
//		/SAVEDATA [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);

	CTString IDName = AnaSyntax::getSingleStr("/USERID", VOstr);
	CTString PathName = AnaSyntax::getSingleStr("/PATH", VOstr);
	m_nUserID = pDataInterface->IndexByName(IDName);
	m_nPath = pDataInterface->IndexByName(PathName);
	if (m_nUserID<0 || m_nPath<0)
		return FALSE;

//		/MODELFROMFILE [FILE]
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//从文件读模型
	{
		CTString FileNameTemp = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
		if (!CFileReadWrite::ReadFileToStr(FileNameTemp, m_strModel))
			return FALSE;
	}
	else if (m_strModel.length() < 5)
		return FALSE;

	if(!GetValueFromModel(pDataInterface, m_strModel) )
	{
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( "调用的模型有错误，无法进行预测。" );
		m_pResult->Add( pWarningTextRslt );
		return FALSE;
	}

	m_bSaveData = AnaSyntax::exist("/SAVEDATA", VOstr);
	if (m_bSaveData)
		m_DataFile = AnaSyntax::getSingleStr("/SAVEDATA", VOstr);

	return TRUE;
}

BOOL CMarkovChain::GetValueFromModel(CDataInterface *pDataInterface, string strModel)
{
	AnaWord AnaWordModel;
	AnaWordModel.Import(strModel);

	if (AnaWordModel.GetWordCount() < 5)
		return FALSE;
	m_nCluster = atoi(AnaWordModel.GetAt(0).c_str());
	m_nPageNum = atoi(AnaWordModel.GetAt(1).c_str());
	if (AnaWordModel.GetWordCount() < m_nCluster*m_nPageNum*m_nPageNum+2 
		|| m_nPageNum < 0)
		return FALSE;

	m_Theta_Trans= new CDoubleMatrix[m_nCluster];

	for (int k=0; k<m_nCluster; k++)
	{
		CDoubleMatrix Theta_Trans_mx(m_nPageNum, m_nPageNum);
		for (int i=0; i<m_nPageNum; i++)
		{
			for (int j=0; j<m_nPageNum; j++)
			{
				Theta_Trans_mx(i, j) = atof(AnaWordModel.GetAt(2+k*m_nPageNum*m_nPageNum+i*m_nPageNum+j).c_str());
				if (Theta_Trans_mx(i, j) < 0 || Theta_Trans_mx(i, j) > 1)
					return FALSE;
			}
		}
		m_Theta_Trans[k] = Theta_Trans_mx.copy();
	}

	return TRUE;
}