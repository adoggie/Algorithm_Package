// BPNet.cpp: implementation of the CBPNet class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BPNet.h"
#include "BP.h"
#include <assert.h>
#include "../Public/FileReadWrite.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

CBPNet::CBPNet()
{

}


CBPNet::~CBPNet()
{

}

bool CBPNet::SetPredVO(std::string strVO)
{
//	BPPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
	if (strlen(m_ModelFile) < 1)
		return false;
	if (!Import(m_ModelFile.GetData()))
		return false;
	////////////////////////////////////////////////////
	//导入输入字段
	m_arInputField.RemoveAll();
	int i=0, nSize=m_InAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_InAry.GetAt(i);
		m_arInputField.Add(pField->strName);
	}
	//输出字段
	nSize = m_OutAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_OutAry.GetAt(i);
		m_arDestField.Add(pField->strName);
	}
	/////////////////////////////////////////////////////
	CTStringArray szArrInput,szArrOutput,szArrReplace;
	AnaSyntax::getVAR("/VARINPUT", VOstr, szArrInput);
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, szArrReplace);
	if (szArrReplace.GetSize()%2 != 0)
		return false;
	if (!AnaSyntax::Replace(m_arInputField, szArrInput, szArrReplace))
		return false;
	//目标变量-多个?
	if (AnaSyntax::exist("/PREDNAME", VOstr))
	{
		AnaSyntax::getVAR("/PREDNAME", VOstr, szArrOutput);
		if (m_arDestField.GetSize() == szArrOutput.GetSize())
		{
			m_arDestField.RemoveAll();
			m_arDestField.Copy(szArrOutput);
		}
	}
	else
	{
		for (int i=0; i<m_arDestField.GetSize(); i++)
		{
			CTString szTemp = m_arDestField.GetAt(i);
			szTemp += "_预测值";
			m_arDestField.SetAt(i,szTemp);
		}
	}
	////////////////////////////////////////////////////
	//导入输入字段
	m_arInputField.RemoveAll();
	nSize=m_InAry.GetSize();
	if (nSize == m_arInputField.GetSize())
	{
		for (i=0; i<nSize; i++)
		{
			FieldDesc *pField = m_InAry.GetAt(i);
			pField->strName = m_arInputField.GetAt(i);
		}
	}
	//输出字段
	nSize = m_OutAry.GetSize();
	if (nSize == m_arDestField.GetSize())
	{
		for (i=0; i<nSize; i++)
		{
			FieldDesc *pField = m_OutAry.GetAt(i);
			pField->strName = m_arDestField.GetAt(i);
		}
	}
	/////////////////////////////////////////////////////
	return true;
}

void	CBPNet::SetTrainVO(std::string strVO)
{
	//BP神经网络VO设置
	//	BPNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD [{DELRECORD**},{AVEFILL}]
	//	/SYSPREC = PRECISION(0.001**)
	//	/TRAINTIME = TIME(100**)
	//	/LEARNRATE = LEARNED(0.1**)
	//	/HIDENCOUNT = HIDEN(4**)
	//	/MAXRUNTIME = [{10MIN},{1HUR},{2HUR},{7HUR},{1DAY},{2DAY},{7DAY},{INF**}]
	//	/OUTTYPE [{CALCULATE**},{CLASS}]
	//	/SAVEMODEL [FILE]
	
	AnaWord VOstr;
	VOstr.Import(strVO);

	if (AnaSyntax::findstr("/SYSPREC","PRECISION",VOstr))
		m_TrainItem.dPrecision = AnaSyntax::getvalue("/SYSPREC","PRECISION",VOstr);
	if (AnaSyntax::findstr("/LEARNRATE","LEARNED",VOstr))
		m_TrainItem.dStep = AnaSyntax::getvalue("/LEARNRATE","LEARNED",VOstr);
	if (AnaSyntax::findstr("/HIDENCOUNT","HIDEN",VOstr))
		m_TrainItem.nHideNum = AnaSyntax::getvalue("/HIDENCOUNT","HIDEN",VOstr);

	m_TrainItem.m_bCalc = AnaSyntax::findstr("/OUTTYPE","CALCULATE",VOstr);

	if (AnaSyntax::findstr("/TRAINTIME","TIME",VOstr))
		m_TrainItem.nTrainTimes = AnaSyntax::getvalue("/TRAINTIME","TIME",VOstr);
	
	if (AnaSyntax::findstr("/MAXRUNTIME","10MIN",VOstr))
		m_TrainItem.dMaxRunTime = 10 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","1HUR",VOstr))
		m_TrainItem.dMaxRunTime = 1 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","2HUR",VOstr))
		m_TrainItem.dMaxRunTime = 2 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","7HUR",VOstr))
		m_TrainItem.dMaxRunTime = 7 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","1DAY",VOstr))
		m_TrainItem.dMaxRunTime = 1 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","2DAY",VOstr))
		m_TrainItem.dMaxRunTime = 2 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","7DAY",VOstr))
		m_TrainItem.dMaxRunTime = 7 * 24 * 60 * 60*1000;
	else if (AnaSyntax::findstr("/MAXRUNTIME","INF",VOstr))
		m_TrainItem.dMaxRunTime = 0;
	
	AnaSyntax::getVAR("/VARINPUT", VOstr, m_arInputField);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_arDestField);

	m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);
}

CResult * CBPNet::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt, std::string strVO)
{
	m_pDataInterface = pDataInterface;
	m_pEnvParam = pEnv;
	m_pInteraction = itt;
	SetTrainVO(strVO);

	assert( m_pDataInterface!=NULL );
	CResult *pResult = new CResult("神经网络分析");

	if (TrainCheck() ==false)
		return NULL;

	if (m_TrainItem.m_bCalc)
	{//计算模式  输出表，曲线图等
		DataMap(&m_InAry,&m_OutAry,m_mxSour,true,false);//归一化（输入 和 输出）
	}
	else
	{//分类模式  输出表，准确率等
		DataMap(&m_InAry,NULL,m_mxSour,true,false);//归一化（输入）
	}
	
	Train(pResult);

	//////////////////////////////////////////////////
	// 创建结果对象
	
	assert( pResult!=NULL );
	//////////////////////////////////////////////////
	// 动态生成网络拓扑
	if( m_pInteraction->bAbort || pResult  && pResult ->Count()==0 )
	{
		delete pResult;
		pResult  = NULL;
	}

	return pResult;
}

CResult * CBPNet::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,std::string strVO, std::string strModel)
{
	CResult *pResult = NULL;

	m_pDataInterface = pDataInterface;
	assert( m_pDataInterface!=NULL );
	m_pEnvParam = pEnv;
	m_pInteraction = itt;

	//读取模型参数
	if (!SetPredVO(strVO))
		return pResult;

	pResult = new CResult("神经网络分析");

	if (SimulateCheck() ==false)
		return NULL;

	DataMap(&m_InAry,NULL,m_mxSour,true,true);//归一化（输入）已经有范围

	Simulate(pResult);
	//////////////////////////////////////////////////
	// 创建结果对象
	
	assert( pResult!=NULL );
	//////////////////////////////////////////////////
	// 动态生成网络拓扑
	if( m_pInteraction->bAbort || pResult  && pResult ->Count()==0 )
	{
		delete pResult ;
		pResult  = NULL;
	}
	return pResult ;
}

void CBPNet::Abort()
{
	m_Bp.Abort();
}

class MArray
{
public:
	double *pArry;
	MArray()
	{
		pArry = NULL;
	}

	~MArray()
	{
		if (pArry)
			delete []pArry;
	}
};
bool CBPNet::Train(CResult *pResult)
{
	int nOutCount = m_OutAry.GetSize();//输出层个数
	MArray OldClassOutAry ;//原有的输出列,用于分类模式
	if (m_TrainItem.m_bCalc)
	{//计算模式  输出表，曲线图等
	}
	else
	{//分类模式  输出表，准确率等
		int nRow = m_mxSour.GetMatrixRowNumber();
		
		int nMaxLen = 256;
		double *pSortAry = new double [nMaxLen+1];
		int nDistinct = m_mxSour.GetDistinct(m_mxSour.GetMatrixColNumber() -1,
			pSortAry,nMaxLen);
		if (nDistinct==0)
			return false;
		else if (nDistinct==-1)
		{
			CTString str;str.Format("分类个数不能大于%d个！",nMaxLen);
			return false;
		}

		//扩大矩阵
		int nOldCol = m_mxSour.GetMatrixColNumber();
		OldClassOutAry.pArry = m_mxSour.GetColArray(nOldCol-1);
		m_mxSour.ReSize(m_mxSour.GetMatrixRowNumber(),nOldCol + nDistinct-1,false);
		for (int iRow=0;iRow<nRow ;iRow++)
		{
			double value = m_mxSour.m_pTMatrix[iRow][nOldCol-1] ;
			m_mxSour.m_pTMatrix[iRow][nOldCol-1]  = 0;
			int nNewPos ;
			if(CBPMatrix::FindPos(pSortAry,nDistinct,value,nNewPos)==false)
			{
				assert(0);
				return false;
			}
			m_mxSour.m_pTMatrix [iRow][nOldCol-1+nNewPos]  = 1;
		}
		//m_mxSour.Print();
		//m_mxSour.SaveDataToFile("C:\\a.txt");

		//输出到m_TrainItem中
		m_TrainItem.pSortAry = pSortAry;
		m_TrainItem.nClassCount = nDistinct;
		nOutCount = nDistinct;//输出层个数
		//delete []pSortAry;
	}


	m_Bp.Reset();
	m_Bp.SetWnd();
	//*****************赋值开始********************************
	m_Bp.m_nInputLayerNumber = m_InAry.GetSize();//输入层数目:
	m_Bp.m_nHideLayerNumber = m_TrainItem.nHideNum;//隐含层数目:
	m_Bp.m_nOutputLayerNumber = nOutCount ;//输出层数目:

	///////////////////////////
	m_Bp.m_nComboArithmetic = 1;//用来训练的算法 0:LM  1:BP
	
	//m_pBp->m_bSimulateDataFlag = (m_TrainItem.m_nType==3);// 是样本训练(0)还是数据仿真(1)
	m_Bp.m_nComboFunc = 0;//用来训练的激活函数 0:Sigmoid 1:tanh 2:Tansig
	m_Bp.m_nSystemErrorLevel = m_TrainItem.dPrecision;//系统精度:

	m_Bp.m_nMaxTrainTimes = m_TrainItem.nTrainTimes;//最大训练次数:
	m_Bp.m_dStep = m_TrainItem.dStep;//步长

	//bp.m_nSystemError = m_nSystemError;//系统总误差:(结果)
	//bp.m_nTrainTimes = m_nTrainTimes;//训练次数:(结果)

//	CTString strProgressText = _T("初始化数据...");
//	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );

	if (m_Bp.TrainingDemo(m_mxSour)==false)
	{//训练失败
// 		AfxMessageBox("训练失败!", MB_ICONEXCLAMATION );
		return false;
	}
	
	if (pResult)
	{//输出系统训练精度
		CRsltElementText*pRsltElementText=new CRsltElementText("系统训练精度");
		CTString str;str.Format("系统训练实际精度%f",m_Bp.m_SystemErrorOld);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);
	}

	if (m_TrainItem.m_bCalc)
	{//计算模式  输出表，曲线图等
		DataMap(NULL,&m_OutAry,m_Bp.m_mxTrainOut,false,0);//反归一化
		DataMap(NULL,&m_OutAry,m_mxSour,false,0);//反归一化
		for(int nCol=0; nCol< m_Bp.m_mxTrainOut.GetMatrixColNumber(); nCol++)
		{//对多个目标变量进行遍历
			double *pdPredict = m_Bp.m_mxTrainOut.GetColArray(nCol);//预测值
			double *pdActual =  m_mxSour.GetColArray(m_InAry.GetSize()+nCol);//实际值

			CreateOut(pResult,pdActual,pdPredict,m_Bp.m_mxTrainOut.GetMatrixRowNumber(),
				"变量("+m_OutAry.GetAt(nCol)->strName+")训练");
			delete []pdPredict;
			delete []pdActual;
		}		
	}
	else
	{//分类模式  输出表，准确率等
		//===uc  无需反归一化
		//m_pBp->m_mxTrainOut;
		MArray newOut;//训练结果
		newOut.pArry = new double [m_Bp.m_mxTrainOut.GetMatrixRowNumber()];
		for (int i=0;i< m_Bp.m_mxTrainOut.GetMatrixRowNumber() ;i++)
		{
			int nIndex = m_Bp.m_mxTrainOut.GetMaxValueCol(i);//得到数值最大的列号，来从数组中反查得到对应的原来值
			newOut.pArry[i] = m_TrainItem.pSortAry[nIndex];//新值
			//OldClassOutAry.pArry[i];//旧值
		}
		//m_pBp->m_mxTrainOut.Print();
		//m_pBp->m_mxTrainOut.SaveDataToFile("C:\\b.txt");
		CreateOut(pResult , OldClassOutAry.pArry , newOut.pArry, m_Bp.m_mxTrainOut.GetMatrixRowNumber(),
				"变量("+m_OutAry.GetAt(0)->strName+")训练",true);		

	}



	
	return true;
}

/*
*数据仿真
*pSimulate：原预测矩阵，用于和结果比较 （为空，则不是训练及预测模式）
*pResult：结果集
*/
void CBPNet::Simulate(CResult *pResult,CBPMatrix*pSimulate)
{

/*
	m_Bp.SetWnd(m_pProgressWnd->m_hWnd);
	CTString strProgressText = _T("开始预测数据...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/

	CBPMatrix mxResult;
	if (m_Bp.SimulateData(m_mxSour,mxResult)==false)
		return ;

	if (m_TrainItem.m_bCalc)
	{//1.计算模式  输出表，曲线图等
		DataMap(NULL,&m_OutAry,mxResult,false,0);//反归一化
		
		if (pSimulate==NULL)//原预测矩阵，用于和结果比较 
		{//1.1仅输出预测结果
			CTLTable* pTableMin=new CTLTable;
			pTableMin->SetCols( mxResult.GetMatrixColNumber() );
			pTableMin->SetRows( mxResult.GetMatrixRowNumber() +1 );
			
			pTableMin->SetTitle("预测结果");
			
			pTableMin->InsertColumn(0, "");
			for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++)
				pTableMin->InsertColumn( nCol+1, m_OutAry.GetAt(nCol)->strName);
			
			for(int nRow=0; nRow< mxResult.GetMatrixRowNumber(); nRow++ )
			{
				CTString string;string.Format( "行号:%02d", nRow+1 );
				pTableMin->InsertItem( nRow, string , true );
				
				for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++ )
					pTableMin->SetItemText( nRow, nCol+1, mxResult.m_pTMatrix[nRow][nCol]);
			}
			
			CRsltElementTable *pVariableMin = new CRsltElementTable( "预测表", pTableMin );
			pResult->Add(pVariableMin);
		}
		else//是训练及预测模式
		{//1.2输出原有值 及 预测结果
			//反归一化操作 start
			DataMap(NULL,&m_OutAry,*pSimulate,false,0);
			//反归一化操作 end
			
			for(int nCol=0; nCol<mxResult.GetMatrixColNumber(); nCol++)
			{//1.2.1对多个目标变量进行遍历
				double *pdPredict = mxResult.GetColArray(nCol);//预测值
				double *pdActual =  pSimulate->GetColArray(m_InAry.GetSize()+nCol);//实际值
				
				CreateOut(pResult,pdActual,pdPredict,mxResult.GetMatrixRowNumber(),
					"变量("+m_OutAry.GetAt(nCol)->strName+")预测");
				delete []pdPredict;
				delete []pdActual;
				
				
			}
			
		}
	}
	else
	{//2.分类模式  输出表，准确率等
		//m_pBp->m_mxTrainOut;
		MArray newOut;//训练结果
		newOut.pArry = new double [mxResult.GetMatrixRowNumber()];
		for (int i=0;i<mxResult.GetMatrixRowNumber() ;i++)
		{
			int nIndex = mxResult.GetMaxValueCol(i);//得到数值最大的列号，来从数组中反查得到对应的原来值
			newOut.pArry[i] = m_TrainItem.pSortAry[nIndex];//新值
			//OldClassOutAry.pArry[i];//旧值
		}

		if (pSimulate==NULL)//原预测矩阵，用于和结果比较 
		{//2.1仅输出预测结果
			CTLTable* pTableMin=new CTLTable;
			pTableMin->SetCols( 1 );
			pTableMin->SetRows( mxResult.GetMatrixRowNumber() +1 );
			
			pTableMin->SetTitle("预测结果");
			
			pTableMin->InsertColumn(0, "");
			int aa = m_OutAry.GetSize();//===temp
			pTableMin->InsertColumn( 1, m_OutAry.GetAt(0)->strName);
			
			for(int nRow=0; nRow< mxResult.GetMatrixRowNumber(); nRow++ )
			{//2.1.1
				CTString string;string.Format( "行号:%02d", nRow+1 );
				pTableMin->InsertItem( nRow, string , true );
				
				pTableMin->SetItemText( nRow, 0+1, newOut.pArry[nRow]);
			}
			
			CRsltElementTable *pVariableMin = new CRsltElementTable( "预测表", pTableMin );
			pResult->Add(pVariableMin);
		}
		else//是训练及预测模式
		{//2.2输出原有值 及 预测结果	
			double *pdPredict = newOut.pArry;//预测值
			double *pdActual =  pSimulate->GetColArray(m_InAry.GetSize()+0);//实际值
			
			CreateOut(pResult,pdActual,pdPredict,mxResult.GetMatrixRowNumber(),
				"变量("+m_OutAry.GetAt(0)->strName+")预测",true);
			//delete []pdPredict;
			delete []pdActual;
	
		}
	}
}

bool CBPNet::TrainCheck()
{
	int i;
	m_InAry.RemoveAll();
	m_OutAry.RemoveAll();
	m_NetWork.Reset();

	int nFieldCount = m_arInputField.GetSize();
	CFieldType cType;
	for(i=0; i<nFieldCount; i++ )
	{
 		CTString strField = m_arInputField.GetAt(i);
        CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strField); 
		cType = pField->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			return false;
		m_InAry.Add(pField->GetFieldName(),cType);
	}
	nFieldCount = m_arDestField.GetSize();
	for(i=0; i<nFieldCount; i++ )
	{
		CTString strField = m_arDestField.GetAt(i);
        CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(strField); 
		cType = pField->GetFieldType();
		if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			return false;
		m_OutAry.Add(pField->GetFieldName(),cType);
	}
	
	if (m_InAry.GetSize()==0)
	{
		return false;
	}
	
	CTStringArray sourInpu;
	if (m_OutAry.GetSize()==0)
	{
		return NULL;
	}
	if (m_OutAry.GetSize()!=1)
	{
		return NULL;
	}
	for (i=0;i<m_InAry.GetSize();i++)
		sourInpu.Add(m_InAry.GetAt(i)->strName);
	for (i=0;i<m_OutAry.GetSize();i++)
		sourInpu.Add(m_OutAry.GetAt(i)->strName);

	//***********  将输入存入矩阵  start ***********
	int nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	if (nSampleSize ==0)
		return false;

	m_mxSour.ReSize(nSampleSize,sourInpu.GetSize());//重新设定矩阵大小
	CDataAccess *pDA = &m_pDataInterface->m_DataAccess;
	pDA->First();
	int nCurRecNo = 0;
	while (!pDA->Eof())
	{
		for (int i=0;i<sourInpu.GetSize();i++)
		{
			CField *pField = pDA->m_pFieldList->FieldByName(sourInpu.GetAt(i));
			if (pField->IsNull())
			{
				nCurRecNo--;
				break;
			}
			CFieldType ft = pField->GetFieldType();
			m_mxSour.m_pTMatrix[nCurRecNo][i] = pField->GetAsDouble();
		}
		pDA->Next();
		nCurRecNo++;
	}
	m_mxSour.ReSize(nCurRecNo,sourInpu.GetSize(),false);

	//***********  将输入存入矩阵  end   ***********
	return true;
}

bool CBPNet::SimulateCheck(bool bLoadData)
{
	int nFieldCount = m_pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	for (int i=0;i<m_InAry.GetSize() ; i++)
	{
		CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(m_InAry.GetAt(i)->strName);
		if (pField == NULL)
		{
// 			AfxMessageBox("网络中字段名：'"+ m_InAry.GetAt(i)->strName+"' 在新数据源中没有找到!", MB_ICONEXCLAMATION );
			return false;
		}
	}
	if (!bLoadData)//不需要装载数据，则返回
		return true;
	//***********  将输入存入矩阵  start ***********
	int nSampleSize = m_pDataInterface->m_DataAccess.GetRecordCount();
	if (nSampleSize ==0)
	{
// 		AfxMessageBox("样本数据为空！");
		return false;
	}
	
	m_mxSour.ReSize(nSampleSize,m_InAry.GetSize());//重新设定矩阵大小
	
	CDataAccess *pDA = &m_pDataInterface->m_DataAccess;
	pDA->First();
	int nCurRecNo = 0;
	while (!pDA->Eof())
	{
		for (int i=0;i<m_InAry.GetSize();i++)
		{
			CField *pField = pDA->m_pFieldList->FieldByName(m_InAry.GetAt(i)->strName);
			
			if (pField->IsNull())
			{
// 				AfxMessageBox("指定的列中含有空值，请先进行数据预处理！");
				return false;
			}
			

			m_mxSour.m_pTMatrix[nCurRecNo][i] = pField->GetAsDouble();
		}
		pDA->Next();
		nCurRecNo++;
	}
	//***********  将输入存入矩阵  end   ***********
	return true;
}

/*
*检查是否已经有训练好的网络结构
*/
bool CBPNet::HasNetWork()
{
	return m_NetWork.GetLength()>0 ? true : false;
}

/*
*返回每一个元素第一个字符表示所在层次关系。从零开始
* //#_S 2005-1-7 zlq
*/
bool CBPNet::GetNetMsg(CTStringArray& msg)
{
	if (HasNetWork())
	{//m_OutAry
		m_NetWork.Seek(0);//复位位置
		Serialize(false);
		msg.Add("0输入变量");
		int i;
		for (i=0;i<m_InAry.GetSize();i++)
		{
			msg.Add("1"+m_InAry.GetAt(i)->strName);
		}
		msg.Add("0输出变量");
		for ( i=0;i<m_OutAry.GetSize();i++)
		{
			msg.Add("1"+m_OutAry.GetAt(i)->strName);
		}
		CTString strTemp;
		strTemp.Format("0训练次数：%d",m_TrainItem.nTrainTimes);
		msg.Add(strTemp);
		strTemp.Format("0系统设定精度：%f",m_TrainItem.dPrecision);
		msg.Add(strTemp);
		strTemp.Format("0网络训练实际精度：%f",m_Bp.m_SystemErrorOld);
		msg.Add(strTemp);
		strTemp.Format("0学习率：%f",m_TrainItem.dStep);
		msg.Add(strTemp);
		strTemp.Format("0隐含层神经元数目：%d",m_TrainItem.nHideNum);
		msg.Add(strTemp);
		msg.Add("0网络训练模式");
		if (m_TrainItem.m_bCalc == 0 )
		{
			strTemp.Format("1分类模式(个数:%d)",m_TrainItem.nClassCount);
			msg.Add(strTemp);
		}
		else
		{
			strTemp.Format("1计算模式");
			msg.Add(strTemp);
		}
		return true;
	}
	msg.Add("0没有网络");
	return false;
}

bool CBPNet::Serialize(bool IsStoring)
{
	if (IsStoring)
	{//保存
		m_NetWork.Reset();//复位内存流
		m_NetWork.WriteString("CBPNet IS BEGIN");
		m_NetWork.Write(&m_TrainItem,sizeof(TrainItem));
		if (m_TrainItem.pSortAry)
		{
			m_NetWork.Write(m_TrainItem.pSortAry,sizeof(double)*m_TrainItem.nClassCount);
		}
		
		m_InAry.Serialize(&m_NetWork,IsStoring);
		m_OutAry.Serialize(&m_NetWork,IsStoring);
		m_Bp.Serialize(&m_NetWork,IsStoring);
		m_NetWork.WriteString("CBPNet IS End");
	}
	else
	{//装载
		m_NetWork.Seek(0);//复位位置
		//清空原有字段信息
		m_InAry.RemoveAll();
		m_OutAry.RemoveAll();
		CTString strID ;
		m_NetWork.ReadString(strID);
		if (strID !="CBPNet IS BEGIN")
		{
			//AfxMessageBox("序列化错误!");
			return false;
		}
		m_TrainItem.Clear();
		m_NetWork.Read(&m_TrainItem,sizeof(TrainItem));
		if (m_TrainItem.pSortAry)
		{
			m_TrainItem.pSortAry = new double[m_TrainItem.nClassCount];
			m_NetWork.Read(m_TrainItem.pSortAry,sizeof(double)*m_TrainItem.nClassCount);
		}
		
		m_InAry.Serialize(&m_NetWork,IsStoring);
		m_OutAry.Serialize(&m_NetWork,IsStoring);
		m_Bp.Serialize(&m_NetWork,IsStoring);
		
		m_NetWork.ReadString(strID);
		if (strID !="CBPNet IS End")
		{
			//AfxMessageBox("序列化错误!");
			return false;
		}
	}
	return true;
}

bool CBPNet::SaveNetWork(CTString strFileName)
{
	Serialize(true);
	return m_NetWork.SaveFile(strFileName);
}

bool CBPNet::LoadNetWork(CTString strFileName)
{
	bool rst = m_NetWork.LoadFile(strFileName);
	if (rst == false)
		return false;
	if (Serialize(false)==false)//先读字段名信息，再读网络结构
	{
		m_NetWork.Reset();
		return false;
	}
	return rst;
}
/*
 *	实现数据的归一化，和还原
 *  pInFeildList 从前到后一一对应
 *  pOutFeildList 从后到前一一对应，如果两个都不为NULL则将起来应该与mx的列总和相同
 *  mx 矩阵
 *  bIsMapToOne 为true则为归一化，false 反归一化
 *  bHasRange   在归一化前提下，是否为已经有上下限范围（在训练模式时，不需要重新得到范围）
                反归一化时无效
 */
bool CBPNet::DataMap(FieldDescList *pInFeildList, FieldDescList *pOutFeildList, CBPMatrix &mx, bool bIsMapToOne,bool bHasRange)
{
	if (pInFeildList && pOutFeildList)
	{
		if (pInFeildList->GetSize()+pOutFeildList->GetSize() != 
			mx.GetMatrixColNumber())
		{
			assert(0);
			return false;//矩阵总和
		}
	}
	
	if (pInFeildList)
	{//有输入矩阵
		CDWordArray FeildIndexAry;
		for (int i=0;i<pInFeildList->GetSize();i++)
			FeildIndexAry.Add(i); 
		DataMapS(*pInFeildList,mx,bIsMapToOne,bHasRange,&FeildIndexAry,&FeildIndexAry);//归一化（输入）
	}
	
	if (pOutFeildList)
	{//有输出矩阵
		int nStart = mx.GetMatrixColNumber() - pOutFeildList->GetSize();
		CDWordArray MxIndexAry;CDWordArray FeildIndexAry;
		for (int i=0;i<pOutFeildList->GetSize();i++)
		{
			FeildIndexAry.Add(i);
			MxIndexAry.Add(nStart+i);
		}
		DataMapS(*pOutFeildList,mx,bIsMapToOne,bHasRange,&FeildIndexAry,&MxIndexAry);//归一化（输入）
	}

	return true;
}

/*
 *	实现数据的归一化，和还原
 *  pFieldIndex，pmxIndex 都为空时，所有的列都操作，按序号对接
 *  不为空时，两个应该相同大小
 *  pFieldIndex 字段对应的索引号
 *  pmxIndex 矩阵对应的索引号
 *  bIsMapToOne 为true则为归一化，false 反归一化
 *  bHasRange   在归一化前提下，是否为已经有上下限范围（在训练模式时，不需要重新得到范围）
                反归一化时无效
 */
bool CBPNet::DataMapS(FieldDescList &FeildList, CBPMatrix &mx, bool bIsMapToOne,bool bHasRange,
		 CDWordArray *pFieldIndex /*= NULL*/, CDWordArray *pMxIndex/*= NULL*/)
{
	CDWordArray tempAry;
	//检查
	if (pFieldIndex || pMxIndex)
	{
		if (pFieldIndex && pMxIndex)
		{
			if (pFieldIndex->GetSize() != pMxIndex->GetSize())
			{
				assert(0);//矩阵列索引大小不一致！
				//AfxMessageBox("矩阵列索引大小不一致！");
				return false;
			}
		}
		else
		{
			assert(0);//两个数组应该同时有
			//AfxMessageBox();
			return false;
		}
	}
	else
	{//如果为空则构造一个
		pFieldIndex = & tempAry;
		pMxIndex    = & tempAry;
		for (int i=0; i<mx.GetMatrixColNumber();i++)
			tempAry.Add(i);
		assert(mx.GetMatrixColNumber() == FeildList.GetSize());
	}


	int nColNum = pFieldIndex->GetSize();//mx.GetMatrixColNumber();
	int nRowNum = mx.GetMatrixRowNumber();

	if (bIsMapToOne)
	{//归一化
		for (int i=0;i<nColNum;i++)
		{//对每一个字段进行归一化
			//************ 找到最大值和最小值 **************
			int nMxCol =  pMxIndex->GetAt(i);//从矩阵中得到列
			
			double dMax,dMin;
			if (bHasRange)
			{//字段描述里已经有上下限信息
				int nFieldCol =  pFieldIndex->GetAt(i);//从字段中得到列
				FieldDesc *pDesc = FeildList.GetAt(nFieldCol);
				if (pDesc ==NULL)
				{
// 					AfxMessageBox("归一化时字段没有找到!");
					return false;
				}
				dMax = pDesc->dMax;
				dMin = pDesc->dMin;
			}
			else
			{//字段描述里 没有上下限信息
				dMax = dMin = mx.m_pTMatrix[0][nMxCol];
				for (int nRow=0; nRow < nRowNum; nRow++)
				{
					double value = mx.m_pTMatrix[nRow][nMxCol];
					if ( value>dMax)
						dMax = value;
					if (value<dMin)
						dMin = value;
				}
				int nFieldCol =  pFieldIndex->GetAt(i);//从字段中得到列
				if(!FeildList.SetAt(nFieldCol,dMin,dMax))
				{
// 					AfxMessageBox("归一化时出现异常!");
					return false;
				}
			}
		
			//************ 开始转化数据 **************
			double dInterval = dMax-dMin;//得到间隔
			if (fabs(dInterval)<1E-30 )
			{//出现为零情况,即全部相等
				dInterval = 1;//防止除零
			}
			for (int nRow=0; nRow < nRowNum; nRow++)
			{
				double value = mx.m_pTMatrix[nRow][nMxCol];
				value = (value-dMin)/dInterval;
				//限制峰值，个别仿真数据异常时
				if (value >1)
					value = 1;
				if (value < 0 )
					value = 0;
				mx.m_pTMatrix[nRow][nMxCol] = value;
			}
		}

	}
	else
	{//反归一化
		for (int i=0;i<nColNum;i++)
		{//对每一个字段进行反归一化
			//************ 得到最大值和最小值 **************
			
			int nFieldCol =  pFieldIndex->GetAt(i);//从字段中得到列
			double dMax,dMin;

			FieldDesc *p = FeildList.GetAt(nFieldCol);
			if(!p )
			{
// 				AfxMessageBox("反归一化时出现异常!");
				return false;
			}
			dMax = p->dMax;
			dMin = p->dMin;
			//************ 开始转化数据 **************
			int nMxCol =  pMxIndex->GetAt(i);//从矩阵中得到列

			double dInterval = dMax-dMin;//得到间隔
			for (int nRow=0; nRow < nRowNum; nRow++)
			{
				double value = mx.m_pTMatrix[nRow][nMxCol];
				mx.m_pTMatrix[nRow][nMxCol] = value*dInterval+ dMin;			
			}
		}
	}
	return true;
}

void FieldDescList::Serialize(CMemoStream *pStream, bool IsStoring)
{
	if (IsStoring)
	{
		pStream->WriteString("Field IS BEGIN");
		int nSize = GetSize();
		pStream->WriteInt(nSize);
		for (int i=0;i<GetSize(); i++)
		{
			FieldDesc *pDesc = GetAt(i);
			pStream->WriteString(pDesc->strName);
			pStream->Write(&pDesc->nType,sizeof(pDesc->nType));
			pStream->WriteDouble(pDesc->dMax);
			pStream->WriteDouble(pDesc->dMin);
			
		}
		pStream->WriteString("Field IS END");
	}
	else
	{
		CTString strTemp;
		pStream->ReadString(strTemp);//"IS BEGIN"
//		if (strTemp !="Field IS BEGIN")
//			AfxMessageBox("FieldDescList Begin序列化错误！");
		int nSize = 0;
		pStream->ReadInt(nSize);
		for (int i=0;i<nSize; i++)
		{
			FieldDesc *pDesc = new FieldDesc();
			pStream->ReadString(pDesc->strName);
			pStream->Read(&pDesc->nType,sizeof(pDesc->nType));
			pStream->ReadDouble(pDesc->dMax);
			pStream->ReadDouble(pDesc->dMin);

			m_Array.push_back(pDesc);
			
		}
		pStream->ReadString(strTemp);
	}
}
FieldDescList::~FieldDescList()
{
	RemoveAll();
}

/*
 *设置最大值和最小值,前提是已经有该项,如果索引号超出范围,则返回false	
 */
bool FieldDescList::SetAt(int nIndex,double dMin,double dMax)
{
	FieldDesc* pFieldDesc = GetAt(nIndex);
	if (pFieldDesc==NULL)
		return false;
	pFieldDesc->dMax = dMax;
	pFieldDesc->dMin = dMin;
	return true;
}

/*
 *添加一项目
 */
void FieldDescList::Add(CTString strName,CFieldType nType,double dMin,double dMax)
{
	FieldDesc *p = new FieldDesc();
	p->strName = strName;
	p->nType = nType;
	p->dMin = dMin;
	p->dMax = dMax;
	m_Array.push_back(p);
}

FieldDesc* FieldDescList::GetAt(int nIndex)
{
	if (nIndex >=m_Array.size() || nIndex<0)
		return NULL;
	return m_Array.at(nIndex);
}

void FieldDescList::RemoveAll()
{
	for (int i=0;i<m_Array.size();i++)
		delete m_Array.at(i);
	m_Array.clear();
}

bool CBPNet::Export(std::string szFileName)
{
	int i=0, nSize=0;
	CTString szText(""), szTemp("");
	TCLFile theFile;
	
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += BPNET;
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" + m_pDataInterface->m_DataAccess.GetFileName() + "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = m_arInputField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_arInputField.GetAt(i);
	}
	//目标变量
	szText += " ";
	szText += VAROUTPUT;
	nSize = m_arDestField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " + m_arDestField.GetAt(i);
	}
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
	//	FieldDescList m_InAry;
	nSize = m_InAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_InAry.GetAt(i);
		theFile << pField->nType;
		theFile << pField->dMax;
		theFile << pField->dMin;
		theFile << pField->strName;
	}
	//	FieldDescList m_OutAry;
	nSize = m_OutAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		FieldDesc *pField = m_OutAry.GetAt(i);
		theFile << pField->nType;
		theFile << pField->dMax;
		theFile << pField->dMin;
		theFile << pField->strName;
	}
	//	TrainItem m_TrainItem;
	theFile << m_TrainItem.m_bCalc;
	theFile << m_TrainItem.nClassCount;
	theFile << m_TrainItem.nHideNum;
	theFile << m_TrainItem.nTrainTimes;
	theFile << m_TrainItem.dPrecision;
	theFile << m_TrainItem.dStep;
	for (i=0; i<m_TrainItem.nClassCount; i++)
	{
		theFile << m_TrainItem.pSortAry[i];
	}
	m_Bp.Export(theFile);
	theFile.Close();
	return true;
}

bool CBPNet::Import(std::string szFileName)
{
	int i=0, nSize=0;
	CFieldType ft;
	double dMax=0, dMin=0;
	CTString szName("");

	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeRead);
	if (!bOpen)
		return false;
	//截尾长度
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//开始写模型参数
	//	FieldDescList m_InAry;
	theFile >> nSize;
	m_InAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> ft;
		theFile >> dMax;
		theFile >> dMin;
		theFile >> szName;
		m_InAry.Add(szName,ft,dMax,dMin);
	}
	//	FieldDescList m_OutAry;
	theFile >> nSize;
	m_OutAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> ft;
		theFile >> dMax;
		theFile >> dMin;
		theFile >> szName;
		m_OutAry.Add(szName,ft,dMax,dMin);
	}
	//	TrainItem m_TrainItem;
	theFile >> m_TrainItem.m_bCalc;
	theFile >> m_TrainItem.nClassCount;
	theFile >> m_TrainItem.nHideNum;
	theFile >> m_TrainItem.nTrainTimes;
	theFile >> m_TrainItem.dPrecision;
	theFile >> m_TrainItem.dStep;
	m_TrainItem.pSortAry = new double[m_TrainItem.nClassCount];
	for (i=0; i<m_TrainItem.nClassCount; i++)
	{
		theFile >> m_TrainItem.pSortAry[i];
	}
	m_Bp.Import(theFile);
	theFile.Close();

	return true;
}
/*
*创建输出结果
*bClassMode 是否为分类模式
*/
void CBPNet::CreateOut(CResult *pResult, double *pActualAry, double *pOutAry, int nLen, CTString strTitle,bool bClassMode /*=false*/)
{
	CTLTable* pTableMin=new CTLTable;
	//pTableMin->SetCols(m_pBp->m_mxTrainOut.GetMatrixColNumber());
	pTableMin->SetRows( nLen +1 );
	
	pTableMin->SetTitle(strTitle+ "结果");
	
	pTableMin->InsertColumn(0, "");
	pTableMin->InsertColumn( 1,"预测值");
	pTableMin->InsertColumn( 2, "实际值");
	if (bClassMode==false)
	{//计算模式
		pTableMin->InsertColumn( 3, "绝对误差");
		pTableMin->InsertColumn( 4, "相对误差");
	}
	int nCorrect = 0;//正确率 分类模式
	for(int nRow=0; nRow< nLen; nRow++ )
	{
		CTString string;string.Format( "行号:%02d", nRow+1 );
		pTableMin->InsertItem( nRow, string , true );
		
		double dPredict =  *(pOutAry+nRow);//预测值
		double dActual = *(pActualAry+nRow);//实际值
		pTableMin->SetItemText( nRow, 1, dPredict);
		pTableMin->SetItemText( nRow, 2, dActual);
		if (bClassMode==false)
		{//计算模式
			pTableMin->SetItemText( nRow, 3, fabs(dPredict- dActual));
			pTableMin->SetItemText( nRow, 4, fabs(dPredict- dActual)/fabs(dActual));
		}
		else
		{//分类模式
			if (dPredict == dActual)
			{
				nCorrect++;
			}
		}
		
	}
	if (bClassMode==false)
	{//计算模式
	}
	else
	{//分类模式
		//输出准确率
		CRsltElementText*pRsltElementText=new CRsltElementText("分类准确率");
		CTString str;str.Format("分类准确率%2.2f%%",(double)nCorrect*100.0/nLen);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);
	}
	CRsltElementTable *pVariableMin = new CRsltElementTable(strTitle+  "表", pTableMin );
	pResult->Add(pVariableMin);
	
	//********************  制图  start********************
	{
		int nTotalRows = nLen;
		CTChartLine*pTChartLine=new CTChartLine;
// 		CTChartLine*pTChartLine=new CTChartLine(nTotalRows,0,ValVal,"","","值");
		
		//得到Variable数据//////////////////////////////////////////////////////////
		TDataPointValVal* pDPSV1=new TDataPointValVal[nTotalRows];
		TDataPointValVal* pDPSV2=new TDataPointValVal[nTotalRows];
	    double Max=*pActualAry,Min=*pActualAry;
		int nRow;
		for(nRow=0;nRow<nTotalRows;nRow++)
		{
			pDPSV1[nRow].fXVal =nRow+1;
			pDPSV2[nRow].fXVal =nRow+1;
			double dPredict = *(pOutAry+nRow);//预测值
			double dActual = *(pActualAry+nRow);//实际值
			pDPSV1[nRow].fYVal =dPredict;
			pDPSV2[nRow].fYVal =dActual;
		    if (dActual>Max)
				 Max=dActual;
			if(dActual<Min)
				 Min=dActual;
		} 
		for( nRow=0;nRow<nTotalRows;nRow++)
			pDPSV1[nRow].fYVal=pDPSV1[nRow].fYVal+(Max-Min)*0.8;
		pTChartLine->SetLine(pDPSV1,nTotalRows,ValVal);
		pTChartLine->SetLine(pDPSV2,nTotalRows,ValVal); 
		delete []pDPSV1;
		delete []pDPSV2;
		pDPSV1 = pDPSV2 = NULL;
		
		CRsltElementChart*pREC=new CRsltElementChart(strTitle+ "拟合图",pTChartLine);
		pResult->Add(pREC);   
	}
	//********************  制图  end********************
	
}





















