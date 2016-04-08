#ifndef _UNIX
#include <io.h>
#endif
#include <stdio.h>
#include "RBFNet.h"

#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../Public/FileReadWrite.h"
#include "../arithInterface/EnvParam.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

extern "C" __declspec( dllexport ) 
int RBFNetTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	CRBFNet rbf;

	(*ppRst) = rbf.TrainRun(pData,pEnv,itt,VO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	//保存模型
	if (rbf.m_ModelFile.GetLength() < 1)
		return 0;
	rbf.Export(rbf.m_ModelFile.GetData());
	return 0;
}


extern "C" __declspec( dllexport ) 
int RBFNetPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			string VO, string &ppOutStruct,CResult **ppRst)
{
	CRBFNet rbf;
	
	(*ppRst) = rbf.PredRun(pData,pEnv,itt,VO,ppOutStruct);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CRBFNet Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRBFNet::CRBFNet()
{

}

CRBFNet::~CRBFNet()
{

}


CResult * CRBFNet::TrainRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO)
{
	m_pDataInterface = pDataInterface;
	SetTrainVO(strVO);

	int j;
	CDoubleMatrix mxInput,mxTarget;
	CResult* pResult=NULL;
	
	//检查数据
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
// 		AfxMessageBox("输入字段未指定！");
		return pResult;
	}
	if((nTargetCol<1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
// 		AfxMessageBox("训练或测试时的目标字段未指定！");
		return pResult;
	}
	if(nTargetCol>0 && m_strMethod=="预测")
	{
// 		AfxMessageBox("预测时不需指定目标字段！");
		return pResult;
	}
	if((nTargetCol>1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
// 		AfxMessageBox("训练或测试时的目标字段只能指定一个！");
		return pResult;
	}
	if(nRow<1)
	{
// 		AfxMessageBox("数据集为空！");
		return pResult;
	}

	
	//提取输入字段数据
	mxInput.create(nRow,nInputCol,0.0);

	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	for(j=0;j<nInputCol;j++)
	{
		int nColIdx = m_pDataInterface->IndexByName(m_strInAry.GetAt(j));
		nColAry.Add(nColIdx);
	}
 	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//提取目标字段数据,只有训练和测试有目标字段
	mxTarget.create(nRow,1,0.0);
	nColAry.RemoveAll();
	int nTargetIdx = m_pDataInterface->IndexByName(m_strTargetAry.GetAt(0));
	nColAry.Add(nTargetIdx);
	m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//开始计算
	CDoubleMatrix mxOutput;
	double dMSE,dStdMSE;		
	if(m_RBFN.Train(mxInput,mxTarget,m_nCenterNum,mxOutput,dMSE,dStdMSE))
	{	
		RBFNetResult RBFNResult;
		RBFNResult.dMSE=dMSE;
		RBFNResult.dStdMSE=dStdMSE;
		RBFNResult.mxOutput=mxOutput;
		RBFNResult.mxTarget=mxTarget;			
		RBFNResult.strRunInfo=m_RBFN.GetRunInfo();

		pResult=ShowResults("训练",RBFNResult);
	}		
	else 
	{
		//训练失败的原因：或者是未通过数据检查，或者是被用户中止
		if(m_RBFN.IsError())
			pResult=ShowResults("训练",m_RBFN.GetErrorInfo().c_str());
		else if(m_RBFN.IsAbort())
			pResult=ShowResults("训练",m_RBFN.GetAbortInfo().c_str());
		else assert(false);
	}
	return pResult;
}

bool CRBFNet::SetPredVo(string VO)
{
//		REGLINEARPRED
//		/DATA=[FILE]
//		/VARLIST [varlist]
//		/MODELFROMFILE [FILE]
	AnaWord VOstr;
	VOstr.Import(VO);

	//		/MODELFROMFILE [FILE]
	if (AnaSyntax::exist("/MODELFROMFILE", VOstr))//从文件读模型
	{
		m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
		//导入文件
		if (!Import(m_ModelFile.GetData()))
			return false;
	}
	else
	{
		return false;
	}
	CTStringArray szArrInput,szArrOutput,szArrReplace;
	AnaSyntax::getVAR("/VARINPUT", VOstr, szArrInput);
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, szArrReplace);
	if (szArrReplace.GetSize()%2 != 0)
		return false;
	if (!AnaSyntax::Replace(m_strInAry, szArrInput, szArrReplace))
		return false;
	//目标变量-多个?
	if (AnaSyntax::exist("/PREDNAME", VOstr))
	{
		AnaSyntax::getVAR("/PREDNAME", VOstr, szArrOutput);
		if (m_strTargetAry.GetSize() == szArrOutput.GetSize())
		{
			m_strTargetAry.RemoveAll();
			m_strTargetAry.Copy(szArrOutput);
		}
	}
	else
	{
		for (int i=0; i<m_strTargetAry.GetSize(); i++)
		{
			CTString szTemp = m_strTargetAry.GetAt(i);
			szTemp += "_预测值";
			m_strTargetAry.SetAt(i,szTemp);
		}
	}
	return true;
}

CResult * CRBFNet::PredRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO, string strModel)
{
	CResult* pResult = NULL;
	m_pDataInterface = pDataInterface;
	if (!SetPredVo(strVO))//读参数及模型
	{
		pResult = new CResult("RBF神经网络预测");
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( "模型参数有误，无法运行。" );
		pResult->Add( pWarningTextRslt );
		return pResult;
	}
	
	int j;
	CDoubleMatrix mxInput,mxTarget;
	
	//检查数据
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
		// 		AfxMessageBox("输入字段未指定！");
		return pResult;
	}
	if((nTargetCol<1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
		// 		AfxMessageBox("训练或测试时的目标字段未指定！");
		return pResult;
	}
	if(nTargetCol>0 && m_strMethod=="预测")
	{
		// 		AfxMessageBox("预测时不需指定目标字段！");
		return pResult;
	}
	if((nTargetCol>1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
		// 		AfxMessageBox("训练或测试时的目标字段只能指定一个！");
		return pResult;
	}
	if(nRow<1)
	{
		// 		AfxMessageBox("数据集为空！");
		return pResult;
	}
	
	//提取输入字段数据
	mxInput.create(nRow,nInputCol,0.0);
	
	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	for(j=0;j<nInputCol;j++)
	{
		int nColIdx = m_pDataInterface->IndexByName(m_strInAry.GetAt(j));
		nColAry.Add(nColIdx);
	}
	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//提取目标字段数据,只有训练和测试有目标字段
	//mxTarget.create(nRow,1,0.0);
	//nColAry.RemoveAll();
	//int nTargetIdx = m_pDataInterface->IndexByName(m_strTargetAry.GetAt(0));
	//nColAry.Add(nTargetIdx);
	//m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//开始计算
	CDoubleMatrix mxOutput;
	if(m_RBFN.Predict(mxInput,mxOutput))
	{
		RBFNetResult RBFNResult;
		RBFNResult.mxOutput=mxOutput;
		RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
		
		pResult=ShowResults("预测",RBFNResult);
	}
	else
	{
		if(m_RBFN.IsError())
			pResult=ShowResults("预测",m_RBFN.GetErrorInfo().c_str());
		else
			assert(false);
	}
	return pResult;
}


bool CRBFNet::SetDataSource(CDataInterface *pDataInterface, const CPtrArray &arFieldRole)
{
	m_pDataInterface = pDataInterface;
	m_arFieldRole.RemoveAll();
	m_arFieldRole.Copy( arFieldRole );	
	m_strInAry.RemoveAll();
	m_strTargetAry.RemoveAll();
	
	//获取输入和目标字段名。入选的已全部是数值型字段
	int nFieldCount = m_arFieldRole.GetSize(); 
	for(int i=0; i<nFieldCount; i++ )
	{
		TFieldRole * pRole = (TFieldRole*)(m_arFieldRole.GetAt(i));
		if( pRole->strRole==CTString("输入变量") || pRole->strRole==CTString("目标变量"))
		{
            CField *pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByName(pRole->strFieldName); 
			CFieldType cType = pField->GetFieldType();
			if( cType!=fDouble && cType!=fInt && cType!=fBoolean && cType!=fCurrency )
			{
				// 				AfxMessageBox( "无效的数据类型", MB_ICONEXCLAMATION );				
				return false;
			}
			if (pRole->strRole==CTString("输入变量"))
			{//输入变量
				m_strInAry.Add(pField->GetFieldName());
			}
			else
			{//目标变量
				m_strTargetAry.Add(pField->GetFieldName());
			}
		}
	}
	
	return true;
}

void CRBFNet::SetTrainVO(string strVO)
{
	//	RBF神经网络VO设置
	//	RBFNETTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/HIDENCENTER = CENTER(10**)
	//	/SAVEMODEL [FILE]
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	if (AnaSyntax::findstr("/HIDENCENTER","CENTER",VOstr))
		m_nCenterNum = AnaSyntax::getvalue("/HIDENCENTER","CENTER",VOstr);
	else 
		m_nCenterNum = 10;
	
	AnaSyntax::getVAR("/VARINPUT", VOstr, m_strInAry);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_strTargetAry);
	m_bSaveModel = AnaSyntax::exist("/SAVEMODEL",VOstr);
	if (m_bSaveModel)
		m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);

	return;	
}
//add by xds 20060712
bool CRBFNet::Export(string szFileName)
{
	int i=0, nSize=0;
	CTString szText(""),szTemp("");
	TCLFile theFile;

	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += RBFNET;
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"" ;
	szText +=m_pDataInterface->m_DataAccess.GetFileName() ;
	szText += "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = m_strInAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText += m_strInAry.GetAt(i);
	}
	//目标变量
	szText += " ";
	szText += VAROUTPUT;
	nSize = m_strTargetAry.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText += m_strTargetAry.GetAt(i);
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
	nSize = m_strInAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strInAry.GetAt(i);
	}
	nSize = m_strTargetAry.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strTargetAry.GetAt(i);
	}	
	//导出
	m_RBFN.Export(theFile);
	theFile.Close();
	return true;
}

bool CRBFNet::Import(string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp("");
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
	theFile >> nSize;
	m_strInAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strInAry.Add(szTemp);
	}
	theFile >> nSize;
	m_strTargetAry.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strTargetAry.Add(szTemp);
	}
	m_RBFN.Import(theFile);
	theFile.Close();
	return true;
}


CResult * CRBFNet::Run(/*CWnd *pProgressWnd*/)
{
//	ASSERT( m_pDataInterface!=NULL );
//	m_pProgressWnd = pProgressWnd;
//	
//	CTString strProgressText = _T("读取数据...");
//	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
	
	////////////////////////////////////////////////////////////////////////
	//计算并显示结果
	int j;
// 	CTString str;
	CDoubleMatrix mxInput,mxTarget;
//	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;
	CResult* pResult;
	
	//检查数据
	int nRow=m_pDataInterface->GetRowCount();
	int nInputCol=m_strInAry.GetSize();
	int nTargetCol=m_strTargetAry.GetSize();
	if(nInputCol<1)
	{
// 		AfxMessageBox("输入字段未指定！");
		return NULL;
	}
	if((nTargetCol<1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
// 		AfxMessageBox("训练或测试时的目标字段未指定！");
		return NULL;
	}
	if(nTargetCol>0 && m_strMethod=="预测")
	{
// 		AfxMessageBox("预测时不需指定目标字段！");
		return NULL;
	}
	if((nTargetCol>1) && (m_strMethod=="训练"||m_strMethod=="测试"))
	{
// 		AfxMessageBox("训练或测试时的目标字段只能指定一个！");
		return NULL;
	}
	if(nRow<1)
	{
// 		AfxMessageBox("数据集为空！");
		return NULL;
	}

	
	//提取输入字段数据
	mxInput.create(nRow,nInputCol,0.0);

	CDWordArray nColAry;
	int nn = m_pDataInterface->GetRowCount();
	CTString strInputName;
	for(j=0;j<nInputCol;j++)
	{
		strInputName = m_strInAry.GetAt(j);
		int nColIdx = m_pDataInterface->IndexByName(strInputName);
		nColAry.Add(nColIdx);
	}
 	m_pDataInterface->GetColsData(nColAry,mxInput,0);
	
	//提取目标字段数据,只有训练和测试有目标字段
	mxTarget.create(nRow,1,0.0);
	nColAry.RemoveAll();
	CTString strTarget = m_strTargetAry.GetAt(0);
	int nTargetIdx = m_pDataInterface->IndexByName(strTarget);
	nColAry.Add(nTargetIdx);
	m_pDataInterface->GetColsData(nColAry,mxTarget,0);
	
	//开始计算
	CDoubleMatrix mxOutput;
	double dMSE,dStdMSE;		
	if(m_strMethod=="训练")
	{
//		strProgressText = _T("开始训练...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );		
		if(m_RBFN.Train(mxInput,mxTarget,m_nCenterNum,mxOutput,dMSE,dStdMSE))
		{	
			//AfxMessageBox("stop");
			
			RBFNetResult RBFNResult;
			RBFNResult.dMSE=dMSE;
			RBFNResult.dStdMSE=dStdMSE;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.mxTarget=mxTarget;			
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();

			pResult=ShowResults("训练",RBFNResult);
		}		
		else 
		{
			//训练失败的原因：或者是未通过数据检查，或者是被用户中止
			if(m_RBFN.IsError())
				pResult=ShowResults("训练",m_RBFN.GetErrorInfo().c_str());
			else if(m_RBFN.IsAbort())
				pResult=ShowResults("训练",m_RBFN.GetAbortInfo().c_str());
			else assert(false);
		}
	}
	else if(m_strMethod=="测试")
	{
//		strProgressText = _T("开始测试...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		if(m_RBFN.Test(mxInput,mxTarget,mxOutput,dMSE,dStdMSE))
		{			
			RBFNetResult RBFNResult;
			RBFNResult.dMSE=dMSE;
			RBFNResult.dStdMSE=dStdMSE;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.mxTarget=mxTarget;
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
			
			pResult=ShowResults("测试",RBFNResult);
		}
		else
		{
			if(m_RBFN.IsError())
				pResult=ShowResults("测试",m_RBFN.GetErrorInfo().c_str());
			else
				assert(false);
		}
	}	
	else if(m_strMethod=="预测")
	{
//		strProgressText = _T("开始预测...");
//		m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		if(m_RBFN.Predict(mxInput,mxOutput))
		{
			RBFNetResult RBFNResult;
			RBFNResult.mxOutput=mxOutput;
			RBFNResult.strRunInfo=m_RBFN.GetRunInfo();
			
			pResult=ShowResults("预测",RBFNResult);
		}
		else
		{
			if(m_RBFN.IsError())
				pResult=ShowResults("预测",m_RBFN.GetErrorInfo().c_str());
			else
				assert(false);
		}
	}
	else
		assert(false);
	
	return pResult;
}

void CRBFNet::Abort()
{
	m_RBFN.Abort();
}

CResult* CRBFNet::ShowResults(CTString strModel, CTString strInfo)
{
	CResult *pResult = new CResult("RBF神经网络"+strModel);
	CRsltElementText *pRsltElementText=new CRsltElementText("系统"+strModel+"信息");
	pRsltElementText->AddString(strInfo);
	pResult->Add(pRsltElementText);
	return pResult;	
}

CResult* CRBFNet::ShowResults(CTString strModel, RBFNetResult& RBFNResult)
{

	int nRow=RBFNResult.mxOutput.mrow();
	const int nMaxShowRows=10000;//若记录数大于nMaxShowRows，则表格输出到文件，图形仅显示nMaxShowRows条记录
	CResult *pResult;
	pResult = new CResult("RBF神经网络"+strModel);
	CTString stemp;
	//输出运行信息
	CRsltElementText *pRsltRunInfo=new CRsltElementText("运行信息");
	pRsltRunInfo->AddString(RBFNResult.strRunInfo.c_str());
	pResult->Add(pRsltRunInfo);

	if(strModel=="训练"||strModel=="测试")
	{
		CTString str,strTargetName=m_strTargetAry[0];
		
		//输出系统精度
		CRsltElementText *pRsltElementText=new CRsltElementText("系统"+strModel+"精度");
		strModel+="样本目标值和RBF神经网络输出值的均方误差（标准化值）=%f\n";
		strModel+="样本目标值和RBF神经网络输出值的均方误差（实际值）=%f";
		str.Format(strModel,RBFNResult.dStdMSE,RBFNResult.dMSE);
		pRsltElementText->AddString(str);
		pResult->Add(pRsltElementText);

		//输出目标变量结果
		//********************  制表  start********************
		//导出到数据界面
		if(nRow>nMaxShowRows)//若记录数太多则输出到文件
		{
			CTString szFldName("");
			CField *pField1=NULL;	
			CField *pField2=NULL;
			CField *pField3=NULL;
			CField *pField4=NULL;	

			szFldName = "预测值";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField1=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField1->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField1);	
			
			szFldName = "实际值";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField2=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField2->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField2);	
			
			szFldName = "绝对误差";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField3=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField3->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField3);

			szFldName = "相对误差";
			if (m_pDataInterface->m_DataAccess.FieldByName(szFldName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szFldName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField4=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField4->SetFieldName(szFldName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField4);
			int nRecordCount = m_pDataInterface->m_DataAccess.GetRecordCount();
			m_pDataInterface->m_DataAccess.First();
			for(int i=0;i<nRow;i++)
			{
				if (i<nRecordCount)
					m_pDataInterface->m_DataAccess.Edit();
				else
					m_pDataInterface->m_DataAccess.Append();
				double dPredict=RBFNResult.mxOutput(i,0);
				double dActual=RBFNResult.mxTarget(i,0);
				pField1->SetAsDouble(dPredict);
				pField2->SetAsDouble(dActual);
				pField3->SetAsDouble(fabs(dPredict- dActual));
				pField4->SetAsDouble(fabs(dPredict- dActual)/fabs(dActual));	
				if (i<nRecordCount)
				{
					m_pDataInterface->m_DataAccess.Post();
					m_pDataInterface->m_DataAccess.Next();
				}
			}
			//保存文件			
			CTString szNowFile("");
			CTString szNewFile("");
			szNowFile = m_pDataInterface->m_DataAccess.GetFileName();
			bool bOld = m_pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
			//真时:为NewFile
			//假时:为NowFile.		
			char buffer[20];
			stemp="目标变量("+strTargetName;
			stemp+=")";
			stemp+=strModel;
			stemp+="表";
			CRsltElementText *pShowTableText=new CRsltElementText(stemp);
			stemp ="目标变量("+strTargetName;
			stemp+=")";
			stemp+=strModel;
			stemp+="结果\n";
			stemp+="数据量大于";
			sprintf(buffer,"%d",nMaxShowRows);
			stemp+=buffer;//_itoa(nMaxShowRows,buffer,10);
			stemp+="条，已保存至";
			stemp+=(bOld ? szNowFile.GetData() : szNewFile.GetData()) ;
			pShowTableText->AddString(stemp);
			pResult->Add(pShowTableText);			
		}
		else
		{				
			CTLTable* pTableMin=new CTLTable;
			nRow=RBFNResult.mxOutput.mrow();
			pTableMin->SetRows( nRow +1 );
			stemp="目标变量("+strTargetName;
			stemp+=")"+strModel;
			stemp+="结果";
			pTableMin->SetTitle(stemp);
			pTableMin->InsertColumn(0, "记录号");
			pTableMin->InsertColumn( 1,"预测值");
			pTableMin->InsertColumn( 2, "实际值");
			pTableMin->InsertColumn( 3, "绝对误差");
			pTableMin->InsertColumn( 4, "相对误差");
			
			for(int i=0;i<nRow;i++)
			{
				str.Format( "%d", i+1 );
				pTableMin->InsertItem( i, str , true );
				double dPredict=RBFNResult.mxOutput(i,0);
				double dActual=RBFNResult.mxTarget(i,0);

				pTableMin->SetItemText( i, 1, dPredict);
				pTableMin->SetItemText( i, 2, dActual);
				pTableMin->SetItemText( i, 3, fabs(dPredict- dActual));
				pTableMin->SetItemText( i, 4, fabs(dPredict- dActual)/fabs(dActual));		
			}
			stemp = "目标变量("+strTargetName;
			stemp+=")"+strModel;
			stemp+="表";
			CRsltVariable *pVariableMin = new CRsltElementTable(stemp, pTableMin );
			pResult->Add(pVariableMin);
		}
		//********************  制表  end********************

		
		//********************  制图  start********************
		//若数据量大于nMaxShowRows，则只绘制其中均匀分布的nMaxShowRows个点
		double dInterval=1.0;
		if(nRow>nMaxShowRows)
		{
			dInterval=double(nRow)/nMaxShowRows;
		}
		int nNewRow=nRow/dInterval+0.5; //若nRow>nMaxShowRows,则nNewRow与nMaxShowRows相等
		
		CTString strTitle="";
		if(nRow>nMaxShowRows)
		{
			char buffer[120];
			sprintf(buffer,"(数据量大于%d条，仅均匀抽取%d条制图)",nMaxShowRows,nMaxShowRows);
//			strTitle=CTString("")+_itoa(nMaxShowRows,buffer,10)+""
//				+_itoa(nMaxShowRows,buffer,10)+"";
			strTitle = buffer;
		}
		strTitle="目标变量("+strTargetName;
		strTitle+=")"+strModel;
		strTitle+="拟合图"+strTitle;
		CTChartLine *pTChartLine=new CTChartLine;
		pTChartLine->SetTitle(strTitle);
		CTString strX = "记录号";
        CTString strY = "目标值";
		pTChartLine->SetXAxilLabel(strX);
		pTChartLine->SetYAxilLabel(strY);
		
		//获取数据
		TDataPointValVal* pDPSV1=new TDataPointValVal[nNewRow];
		TDataPointValVal* pDPSV2=new TDataPointValVal[nNewRow];
		double dPredict,dActual;
		for(int i=0;i<nNewRow;i++)
		{
			pDPSV1[i].fXVal =int(i*dInterval+0.5)+1;
			pDPSV2[i].fXVal =int(i*dInterval+0.5)+1;
			dPredict = RBFNResult.mxOutput(int(i*dInterval+0.5),0);
			dActual = RBFNResult.mxTarget(int(i*dInterval+0.5),0);					
			pDPSV1[i].fYVal =dPredict;
			pDPSV2[i].fYVal =dActual;
		} 			
		CTString pStr[2];
		pStr[0]="预测数据";
		pStr[1]="目标数据";		
//		pTChartLine->AddData(0,0,pDPSV1,nNewRow,ValVal); 
		pTChartLine->SetLine(pDPSV1,nNewRow,ValVal,pStr[0]);
//		pTChartLine->AddData(0,1,pDPSV2,nNewRow,ValVal); 
		pTChartLine->SetLine(pDPSV2,nNewRow,ValVal,pStr[1]);
		delete []pDPSV1;
		delete []pDPSV2;
					
//		double fxMin = DBL_MAX;
//		double fxMax =-DBL_MAX;
//		double fyMin =DBL_MAX;
//		double fyMax =-DBL_MAX;
//		pTChartLine->GetMinMaxForData(0,fxMin,fxMax,fyMin,fyMax);
//		pTChartLine->SetAxisRange(0,fxMin,fxMax,fyMin,fyMax);
//		pTChartLine->AddLegend(0,pStr,2);
				
		
		//////一开始显示的图形标题其实是左侧导航的标题
		//////在输出图形上点击鼠标右键，选择属性再取消，图形标题才会变为真正的标题。是图形类的bug
		//////因此只好将左侧导航标题设为和图形标题一样
		CRsltElementChart *pREC=new CRsltElementChart(strTitle,pTChartLine);
		//////////////////////////////////////////////////////////////////////////
		
		pResult->Add(pREC);   
		//********************  制图  end********************
	}
	else if(strModel=="预测")
	{
		CTString szPredName("");
		//目标变量只有一个.
		if (m_strTargetAry.GetSize() > 0)
			szPredName = m_strTargetAry.GetAt(0);
		else
			szPredName = "预测值";
		//输出
		if(nRow>nMaxShowRows)
		{
			CField *pField=NULL;			
			if (m_pDataInterface->m_DataAccess.FieldByName(szPredName) != NULL)
			{
				int nTemp = m_pDataInterface->m_DataAccess.IndexByName(szPredName);
				m_pDataInterface->m_DataAccess.m_pFieldList->Delete(nTemp);
			}
			pField=m_pDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			pField->SetFieldName(szPredName);
			m_pDataInterface->m_DataAccess.m_pFieldList->Add(pField);
			int nRecordCount = m_pDataInterface->m_DataAccess.GetRecordCount();
			m_pDataInterface->m_DataAccess.First();
			for(int i=0;i<nRow;i++)
			{
				if (i<nRecordCount)
					m_pDataInterface->m_DataAccess.Edit();
				else
					m_pDataInterface->m_DataAccess.Append();
				pField->SetAsDouble(RBFNResult.mxOutput(i,0));
				if (i<nRecordCount)
				{
					m_pDataInterface->m_DataAccess.Post();
					m_pDataInterface->m_DataAccess.Next();
				}
			}
			
			//保存文件			
			CTString szNowFile("");
			CTString szNewFile("");
			szNowFile = m_pDataInterface->m_DataAccess.GetFileName();
			bool bOld = m_pDataInterface->m_DataAccess.SaveFile(szNowFile,&szNewFile);
			//真时:为NewFile
			//假时:为NowFile.			
			char buffer[20];
			stemp = "目标变量"+strModel;
			stemp+="表";
			CRsltElementText *pShowTableText=new CRsltElementText(stemp);
			stemp = "目标变量"+strModel;
			stemp+="结果\n";
			stemp+="数据量大于";
			sprintf(buffer,"%d",nMaxShowRows);
			stemp+=buffer;
			stemp+="条，已保存至";
			stemp+=(bOld ? szNowFile.GetData() : szNewFile.GetData());
			pShowTableText->AddString(stemp);
			pResult->Add(pShowTableText);
		}		
		else
		{		
			//输出预测结果
			CTLTable* pTableMin=new CTLTable;
			nRow=RBFNResult.mxOutput.mrow();
			pTableMin->SetRows( nRow +1 );
			pTableMin->SetTitle("目标变量"+strModel+"结果");
			pTableMin->InsertColumn(0, "记录号");
			pTableMin->InsertColumn( 1,szPredName);
		
			CTString str;
			for(int i=0;i<nRow;i++)
			{
				str.Format( "%d", i+1 );
				pTableMin->InsertItem( i, str , true );
				double dPredict=RBFNResult.mxOutput(i,0);			
				pTableMin->SetItemText( i, 1, dPredict);			
			}
			CRsltVariable *pVariableMin = new CRsltElementTable("目标变量"+strModel+"表", pTableMin );
			pResult->Add(pVariableMin);
		}
	}
	
	return pResult;
}

//自动产生将要保存的文件名
CTString CRBFNet::GetSaveFileName()
{
	CTString strDataFile,strSaveFile,strTemp;
	strDataFile=m_pDataInterface->m_DataAccess.GetFileName();
	strTemp=strDataFile;
	
	int nIndex=strDataFile.ReverseFind('.');
	if(nIndex<0)  //若扩展名不存在
	{
		strTemp=strTemp+"_$out$.mkw";
		if(strTemp.ReverseFind(CHAR_SLASH)<0) 
		{//若路径不存在则保存至当前路径，用于分析数据在马克威新建的未保存文件中
			strTemp="./"+strTemp;
		}
	}
	else
	{
		strTemp.Insert(nIndex,"_$out$");
	}
	
	strSaveFile=strTemp;
// 	CFileFind FF;
	int nSuffix=1;
	char buffer[20];

//	struct _finddata_t c_file;
//	long hFile;
	while(1)
	{
		//if( (hFile = _findfirst( strSaveFile.GetData(), &c_file )) == -1L )
		if(_access( strSaveFile.GetData(),0))	//--file existed!
			break;
		else
		{	
			strSaveFile=strTemp;
			sprintf(buffer,"%d",nSuffix);
			//_itoa(nSuffix,buffer,10);
			strSaveFile.Insert(strSaveFile.ReverseFind('.')-1,buffer);
			nSuffix++;
		}
	}							
	
	return strSaveFile;
}

