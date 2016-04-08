// SVMThread.cpp : implementation file
//
#include "SVMThread.h"
#include "CSVM.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"

extern "C" __declspec( dllexport ) 
int SVMTrain(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	//	支持向量机VO设置
	//	SVM
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/KERNALTYPE [{LINEAR},{LINEARNOT}]
	//	/CLASSIFY [{GAUSS},{POLYNOMIAL}]
	//	/GAUSS SIGGMA(2)
	//	/POLYNOMIAL LADDER(2)
	//	/PUNFACTOR FACTOR(2)
	//	/KKTCON KKT(0.1)
	//	/KPS VALUE(0.001)
	
	CSVMThread svm;

	(*ppRst) = svm.RunTrain(pData,pEnv,itt,VO);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	if (svm.m_ModelFile.GetLength() < 1)
		return 0;
	svm.Export(svm.m_ModelFile.GetData());
	return 0;
}

extern "C" __declspec( dllexport ) 
int SVMPred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
			 string VO, string &ppOutStruct,CResult **ppRst)
{
	CSVMThread svm;

	(*ppRst) = svm.RunPred(pData,pEnv,itt,VO,ppOutStruct);
	if (*ppRst != NULL)
		(*ppRst)->Print();
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CSVMThread

// IMPLEMENT_DYNCREATE(CSVMThread, CWinThread)

CSVMThread::CSVMThread()
{
}

CSVMThread::~CSVMThread()
{
}

void CSVMThread::SetTrainVO(string strVO)
{
	assert(m_pSVM);
	//	支持向量机VO设置
	//	SVMTRAIN
	//	/DATA = FILE
	//	/VARINPUT arglist
	//	/VARDEST arglist
	//	/MISSMETHOD DELRECORD
	//	/KERNALTYPE [{LINEAR**},{LINEARNOT}]
	//	/CLASSIFY [{GAUSS**},{POLYNOMIAL}]
	//	/CLASSIFYPAR SIGGMA(2**) LADDER(2**)
	//	/PARAM FACTOR(2**) KKT(0.1**) KPS(0.001**)
	//	/SAVEMODEL [FILE]

	
	AnaWord VOstr;
	VOstr.Import(strVO);
	int  nCassifyMethod,nKernalType,nSiggmaSquare,nLaddCount;
	double dPunishFactor,dKKTFactor,dEPS;
	
	nCassifyMethod = AnaSyntax::findstr("/KERNALTYPE","LINEARNOT",VOstr) ? 0:1;
	nKernalType = AnaSyntax::findstr("/CLASSIFY","POLYNOMIAL",VOstr) ? 0:1;
	
	nSiggmaSquare = 2;
	if (AnaSyntax::findstr("/CLASSIFYPAR","SIGGMA",VOstr))
		nSiggmaSquare = AnaSyntax::getvalue("/CLASSIFYPAR","SIGGMA",VOstr);
	nLaddCount = 2;
	if (AnaSyntax::findstr("/CLASSIFYPAR","LADDER",VOstr))
		nLaddCount = AnaSyntax::getvalue("/CLASSIFYPAR","LADDER",VOstr);
	
	dPunishFactor = 2;
	if (AnaSyntax::findstr("/PARAM","FACTOR",VOstr))
		dPunishFactor = AnaSyntax::getvalue("/PARAM","FACTOR",VOstr);
	dKKTFactor = 0.1;
	if (AnaSyntax::findstr("/PARAM","KKT",VOstr))
		dKKTFactor = AnaSyntax::getvalue("/PARAM","KKT",VOstr);
	dEPS = 0.001;
	if (AnaSyntax::findstr("/PARAM","EPS",VOstr))
		dEPS = AnaSyntax::getvalue("/PARAM","EPS",VOstr);
	
	m_pSVM->SetParam(nCassifyMethod,nKernalType,
		nSiggmaSquare,nLaddCount,dPunishFactor,
		dKKTFactor,dEPS);

	m_ModelFile = AnaSyntax::getSingleStr("/SAVEMODEL",VOstr);

	AnaSyntax::getVAR("/VARINPUT", VOstr, m_strInputField);
	AnaSyntax::getVAR("/VARDEST", VOstr, m_strDestField);
}

int CSVMThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
// 	return CWinThread::ExitInstance();
	return 0;
}

/*
BEGIN_MESSAGE_MAP(CSVMThread, CWinThread)
	//{{AFX_MSG_MAP(CSVMThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// CSVMThread message handlers

CResult* CSVMThread::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO)
{
	CResult* pResult=new CResult("支持向量机");
	m_pSVM = new CSVM;
	
	m_pSVM->SetPrgWnd();
	// get fields info
	assert(m_pDataInterface!=NULL);
	m_pDataInterface = pDataInterface;
	SetTrainVO(VO);
	CDataAccess  *pTempDataAccess;

	//add by liytao 20030123
	VarIndexs tempVarIndex;
	tempVarIndex.nVarCount=GetVarIndexs(&tempVarIndex.pVarIndexs,&tempVarIndex.nTargetIndex);
	if (tempVarIndex.nVarCount<=0||tempVarIndex.nTargetIndex<0)
	{
		CTString strTitle = "警告";
		CTString strInfo = "输入变量或者目标变量没有设置！";
		CRsltElementText *pTextRslt = new CRsltElementText;
		pTextRslt->SetName(strTitle);
		pTextRslt->AddString(strInfo);
		pResult->Add(pTextRslt);
		return pResult;
	}
	
	if(m_pSVM->m_TrainVarIndex.pVarIndexs!=NULL)
		m_pSVM->m_TrainVarIndex.Empty();
	m_pSVM->m_TrainVarIndex=tempVarIndex;
	m_pSVM->m_pRecIndexs = tempVarIndex.pVarIndexs;
	if( HaveNullRec(&m_pDataInterface->m_DataAccess))
	{
//		AfxMessageBox("分析字段不能有空值");	
//		SendCalcOverMsg(WM_CALCULATE_OVER,NULL);
		CTString strTitle = "警告";
		CTString strInfo = "分析字段不能有空值！";
		CRsltElementText *pTextRslt = new CRsltElementText;
		pTextRslt->SetName(strTitle);
		pTextRslt->AddString(strInfo);
		pResult->Add(pTextRslt);
		return pResult;
	}
	//add by liytao 20021226
	if (m_pSVM->GetSourceAccess()!=NULL)
		delete m_pSVM->GetSourceAccess();
	pTempDataAccess=PretreatData(&m_pDataInterface->m_DataAccess,&(m_pSVM->m_pRecIndexs));
	if (pTempDataAccess==NULL)
	{
		CTString strTitle = "警告";
		CTString strInfo = "数据预处理失败，决策属性值不能超过2类，无法生成向量机！";
		CRsltElementText *pTextRslt = new CRsltElementText;
		pTextRslt->SetName(strTitle);
		pTextRslt->AddString(strInfo);
		pResult->Add(pTextRslt);
		return pResult;
	}
	//add end  
	m_pSVM->SetSourceAccess(pTempDataAccess);
//	m_pSVM->SetSourceAccess(&m_pDataInterface->m_DataAccess);
	m_pSVM->SetAlphaTableName("Alpha.tl");

	//svm->SetStoreMode(false);
	//6--10
	bool bOK=m_pSVM->InitData();
	if(!bOK)
	{
		CTString strTitle = "警告";
		CTString strInfo = "初始化错误！";
		CRsltElementText *pTextRslt = new CRsltElementText;
		pTextRslt->SetName(strTitle);
		pTextRslt->AddString(strInfo);
		pResult->Add(pTextRslt);
		return pResult;
	}
	//10--60
	m_pSVM->SMOCalc();
	//61--80
	double dbCorRate;
	dbCorRate=m_pSVM->Evaluate();
	//81--90
	m_pSVM->SetCorrectRate(dbCorRate);
	CTString strResult;

	//add table
	CTLTable * pTable = new CTLTable;
	CTString strHead,strInfo,strTmp;
	if (!m_pSVM->IsLinear())
		strTmp=_T("选择线形不可分\r\n");
	else
		strTmp=_T("选择线形可分\r\n");
	strHead += strTmp;
	strTmp.Format("在惩罚因子=%.4f、KKT条件=%.4f、Eps值=%.4f",
		m_pSVM->GetCeilAlpha(),m_pSVM->GetTolerance(),m_pSVM->GetEps());
	strHead += strTmp;
	if (!m_pSVM->IsLinear())
	{
		strTmp.Format("、\r\n核函数为 %s 时\r\n",m_pSVM->GetKernFunc().GetData());
	}
	else
	{
		strTmp=_T("时\r\n");
	}
	strHead += strTmp;
	strTmp.Format("分类正确率=%.4f\r\nb=%.4f\r\n",dbCorRate,m_pSVM->GetB());
	strHead += strTmp;

	strInfo.Empty();
	if (m_pSVM->IsLinear())
	{
		strTmp.Format("w=(");
		strInfo=strInfo;
		strInfo+=strTmp;
		
		
		int vLen;
		double dbV;
		vLen=m_pSVM->GetColNum()-1;
		dbV=m_pSVM->GetWeight(0);

		strTmp.Format("%.4f",dbV);
		strInfo=strInfo;
		strInfo+=strTmp;
		for (int i=1;i<vLen;i++)
		{

			dbV=m_pSVM->GetWeight(i);
			strTmp.Format(",%.4f",dbV);
			strInfo=strInfo;
			strInfo+=strTmp;
		}
		strInfo+=_T(")\n");
		strHead += strInfo;
	}
	pTable->SetTitle("支持向量机结果表");

	//find vector-sum 
	int nNext,nSeq;
	nNext=m_pSVM->FindNextSVectorIndex(-1);

	nSeq=1;
	while( nNext>=0)
	{
		nNext=m_pSVM->FindNextSVectorIndex(nNext);
		nSeq++;
	}

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(0,"支持向量");
	pTable->InsertColumn(1,"记录序号");
	pTable->InsertColumn(2,"alpha值");

	strInfo.Empty();
	nNext=m_pSVM->FindNextSVectorIndex(-1);

	nSeq=0;
	while( nNext>=0)
	{
		strTmp.Format("%d",nSeq+1);
		pTable->InsertItem(nSeq,strTmp);
		strTmp.Format("%d",nSeq+1);
		pTable->SetItemText(nSeq,1,strTmp);	
		//strTmp.Format("%d",nNext+1); modified by liytao 20021227
		strTmp.Format("%d",m_pSVM->m_pRecIndexs[nNext+1]);
		pTable->SetItemText(nSeq,2,strTmp);	
		strTmp.Format("%.4f",m_pSVM->GetAlpha(nNext));
		pTable->SetItemText(nSeq,3,strTmp);	
		
		nNext=m_pSVM->FindNextSVectorIndex(nNext);
		nSeq++;
	}
	strTmp.Format("支持向量个数=%d\r\n",nSeq);
	strHead+=strTmp;
	//add by liytao 20021230
	strTmp.Format("所处理记录个数:%d;丢弃的记录个数:%d\n",m_pSVM->GetSourceAccess()->GetRecordCount(), m_pSVM->m_nMissingRecNum);
	strHead+=strTmp;
	//add end
	
	pTable->SetHead(strHead);

	//add by liytao 20030214
	if (nSeq>0)
	{
		CRsltElementTable * pE = new CRsltElementTable("支持向量机结果表",pTable);
		pResult->Add(pE);
		m_pSVM->m_SVNum=nSeq;
	}
	else
	{
		if( pTable)
			delete pTable;
		pTable=NULL;
		CRsltElementText * pText = new CRsltElementText("警告(支持向量机结果表)\n");
		strTmp.Format("没有生成支持向量，不显示支持向量机结果表\n");
		pText->AddString(strTmp);
		pResult->Add(pText);	
		m_pSVM->m_SVNum=0;
	}
	
	return pResult;
}

bool CSVMThread::SetPredVO(string strVO)
{
//	SVMPRED
//		/DATA=[FILE]
//		/VARINPUT [varlist]
//		/MODELFROMFILE [FILE]
	AnaWord VOstr;
	VOstr.Import(strVO);
	
	m_ModelFile = AnaSyntax::getSingleStr("/MODELFROMFILE", VOstr);
	if (m_ModelFile.GetLength() < 1)
		return false;
	if (!Import(m_ModelFile.GetData())) //从文件中读取模型信息
		return false;
	CTStringArray szArrInput,szArrOutput,szArrReplace;
	AnaSyntax::getVAR("/VARINPUT", VOstr, szArrInput);
	AnaSyntax::getVAR("/REPLACEVARNAME", VOstr, szArrReplace);
	if (szArrReplace.GetSize()%2 != 0)
		return false;
	if (!AnaSyntax::Replace(m_strInputField, szArrInput, szArrReplace))
		return false;
	//目标变量-多个?
	if (AnaSyntax::exist("/PREDNAME", VOstr))
	{
		AnaSyntax::getVAR("/PREDNAME", VOstr, szArrOutput);
		if (m_strDestField.GetSize() == szArrOutput.GetSize())
		{
			m_strDestField.RemoveAll();
			m_strDestField.Copy(szArrOutput);
		}
	}
	else
	{
		for (int i=0; i<m_strDestField.GetSize(); i++)
		{
			CTString szTemp = m_strDestField.GetAt(i);
			szTemp += "_预测值";
			m_strDestField.SetAt(i,szTemp);
		}
	}
	return true;
}

CResult* CSVMThread::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,string VO, string strModel)
{
	CResult *pResult = NULL;
	//导入模型
	if (!SetPredVO(VO)) 
		return pResult;

	// get fields info
	assert(m_pDataInterface!=NULL);
	m_pDataInterface = pDataInterface;
	CDataAccess  *pTempDataAccess;

	//add by liytao 20030123
	VarIndexs tempVarIndex;
	int nTagetFieldIndex=-1;
	tempVarIndex.nVarCount=GetVarIndexs(&tempVarIndex.pVarIndexs,&tempVarIndex.nTargetIndex);
	if (tempVarIndex.nVarCount<=0)
	{
		return NULL;
	}

//	if(m_pSVM->GetSourceAccess()==NULL)
//	{
//		return NULL;
//	}

//	if( m_pSVM->m_SVNum<=0)
//	{
//		CResult* pResult=new CResult("支持向量机");
//		CRsltElementText * pText = new CRsltElementText("警告(支持向量机结果表)\n");
//		CTString strTmp;
//		strTmp.Format("没有支持向量，无法预测\n");
//		pText->AddString(strTmp);
//		pResult->Add(pText);	
//		return NULL;
//	}

	pTempDataAccess=&m_pDataInterface->m_DataAccess;
	m_pSVM->SetPredict(pTempDataAccess);

	m_pSVM->m_PredictVarIndex=tempVarIndex; //add by liytao 20030124

	int nC=pTempDataAccess->GetRecordCount();
	
	pResult = new CResult("支持向量机");
	
	CTLTable * pTable = new CTLTable;
	CTString strHead,strInfo,strTmp;
	if (!m_pSVM->IsLinear())
		strTmp=_T("选择线形不可分\r\n");
	else
		strTmp=_T("选择线形可分\r\n");
	strHead += strTmp;
	strTmp.Format("在惩罚因子=%.4f、KKT条件=%.4f、Eps值=%.4f",
	m_pSVM->GetCeilAlpha(),m_pSVM->GetTolerance(),m_pSVM->GetEps());
	strHead += strTmp;
	if (!m_pSVM->IsLinear())
	{
		strTmp.Format("、\r\n核函数为 %s 时\r\n",m_pSVM->GetKernFunc().GetData());
	}
	else
	{
		strTmp=_T("时\r\n");
	}
	strHead += strTmp;
	strTmp.Format("正确率为%.4f时\r\n"	,m_pSVM->GetCorrectRate());
	strHead += strTmp;
	strInfo.Empty();

	pTable->SetTitle("支持向量机预测值表");
	pTable->InsertColumn(0,"记录序号");
	pTable->InsertColumn(1,"预测值");
	int nPos=31;
	int nY=0,nInvalid=0;
	for (int i=0;i<nC;i++)
	{
		nY=m_pSVM->GetPredictY(i);
		if (nY==0)
		{
			nInvalid++;
			continue;
		}
		strTmp.Format("%d",i+1);
		pTable->InsertItem(i,strTmp);
//		strTmp.Format("%d",i+1);
//		pTable->SetItemText(i,1,strTmp);	
		
		if (nY==0)
		{
			strTmp=_T("无效");
		}
		else
		{

			strTmp=m_pSVM->m_RealYs.GetStrVal(nY); 
		}

		pTable->SetItemText(i,1,strTmp);	

		if(nPos<80)
			nPos++;
	}
	//add by liytao 20021230
	strTmp.Format("所处理记录个数:%d;丢弃的记录个数:%d\n",nC, nInvalid);
	strHead=strTmp;
	strHead+=strHead;
	//add end
	pTable->SetHead(strHead);
	CRsltElementTable * pE = new CRsltElementTable("支持向量机预测值表",pTable);
	pResult->Add(pE);	
	return pResult;
}

int CSVMThread::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	//获得通知消息ID
	m_pSVM->SetPrgWnd();
	// get fields info
	assert(m_pDataInterface!=NULL);
	CDataAccess  *pTempDataAccess;

	if (m_nRoleRs==ROLE_PREDICT)
	{
		//add by liytao 20030123
		VarIndexs tempVarIndex;
		int nTagetFieldIndex=-1;
// 		tempVarIndex.nVarCount=GetVarIndexs(&m_pDataInterface->m_DataAccess,m_pFields,&tempVarIndex.pVarIndexs,&tempVarIndex.nTargetIndex);
		if (tempVarIndex.nVarCount<=0||tempVarIndex.nVarCount!=m_pSVM->m_TrainVarIndex.nVarCount)
		{
			return 1;
		}

		if(m_pSVM->GetSourceAccess()==NULL)
		{
			return 1;
		}

		if( m_pSVM->m_SVNum<=0)
		{
			CResult* pResult=new CResult("支持向量机");
			CRsltElementText * pText = new CRsltElementText("警告(支持向量机结果表)\n");
			CTString strTmp;
			strTmp.Format("没有支持向量，无法预测\n");
			pText->AddString(strTmp);
			pResult->Add(pText);	
			return 1;
		}

		/*if (!CheckPredict(&m_pDataInterface->m_DataAccess))
		{
			AfxMessageBox("数据无效，无法预测");
			SendCalcOverMsg(WM_CALCULATE_OVER,NULL);
			return 1;
		} modified by liytao 20030124*/
		//add by liytao 20021223
		pTempDataAccess=&m_pDataInterface->m_DataAccess;
		m_pSVM->SetPredict(pTempDataAccess);

		m_pSVM->m_PredictVarIndex=tempVarIndex; //add by liytao 20030124

		int nC=pTempDataAccess->GetRecordCount();

		
		CResult* pResult=new CResult("支持向量机");
		
		CTLTable * pTable = new CTLTable;
		CTString strHead,strInfo,strTmp;
		if (!m_pSVM->IsLinear())
			strTmp=_T("选择线形不可分\r\n");
		else
			strTmp=_T("选择线形可分\r\n");
		strHead += strTmp;
		strTmp.Format("在惩罚因子=%.4f、KKT条件=%.4f、Eps值=%.4f",
		m_pSVM->GetCeilAlpha(),m_pSVM->GetTolerance(),m_pSVM->GetEps());
		strHead += strTmp;
		if (!m_pSVM->IsLinear())
		{
			strTmp.Format("、\r\n核函数为 %s 时\r\n",m_pSVM->GetKernFunc().GetData());
		}
		else
		{
			strTmp=_T("时\r\n");
		}
		strHead += strTmp;
		strTmp.Format("正确率为%.4f时\r\n"	,m_pSVM->GetCorrectRate());
		strHead += strTmp;
		strInfo.Empty();

		pTable->SetTitle("支持向量机预测值表");
		pTable->SetRows(nC+1);
		pTable->SetCols(2);
		pTable->InsertColumn(0,"记录序号");
		pTable->InsertColumn(1,"预测值");
		int nPos=31;
		int nY=0,nInvalid=0;
		for (int i=0;i<nC;i++)
		{
			nY=m_pSVM->GetPredictY(i);
			if (nY==0)
			{
				nInvalid++;
				continue;
			}
			strTmp.Format("%d",i+1);
			pTable->InsertItem(i,strTmp);
			strTmp.Format("%d",i+1);
			pTable->SetItemText(i,0,strTmp);	
			
			/*if (nY==0)
			{
				strTmp=_T("无效");
			}
			else
				strTmp.Format("%d",nY); modified by liytao 20030123*/
			if (nY==0)
			{
				strTmp=_T("无效");
			}
			else
			{

				strTmp=m_pSVM->m_RealYs.GetStrVal(nY); 
			}

			pTable->SetItemText(i,1,strTmp);	

			if(nPos<80)
				nPos++;

		}
		//add by liytao 20021230
		strTmp.Format("所处理记录个数:%d;丢弃的记录个数:%d\n",nC, nInvalid);
		strHead=strTmp;
		strHead+=strHead;
		//add end
		pTable->SetHead(strHead);
		CRsltElementTable * pE = new CRsltElementTable("支持向量机预测值表",pTable);
		pResult->Add(pE);	
		return 0;
	}


	//add by liytao 20030123
	VarIndexs tempVarIndex;
// 	tempVarIndex.nVarCount=GetVarIndexs(&m_pDataInterface->m_DataAccess,m_pFields,&tempVarIndex.pVarIndexs,&tempVarIndex.nTargetIndex);
	if (tempVarIndex.nVarCount<=0||tempVarIndex.nTargetIndex<0)
	{
		return 1;
	}

	
	if(m_pSVM->m_TrainVarIndex.pVarIndexs!=NULL)
		m_pSVM->m_TrainVarIndex.Empty();
	m_pSVM->m_TrainVarIndex=tempVarIndex;
	m_pSVM->m_pRecIndexs = tempVarIndex.pVarIndexs;
	/*if( HaveNullRec(&m_pDataInterface->m_DataAccess))
	{
		AfxMessageBox("分析字段不能有空值");	
		SendCalcOverMsg(WM_CALCULATE_OVER,NULL);
		return 1;
	}*/
	//add by liytao 20021226
	if (m_pSVM->GetSourceAccess()!=NULL)
		delete m_pSVM->GetSourceAccess();
	//1--5
	//add by liytao 20021227
//	if (m_pPrgWnd!=NULL)
//		m_pPrgWnd->SendMessage(WM_PROGRESS_STEP,2,0);
	pTempDataAccess=PretreatData(&m_pDataInterface->m_DataAccess,&(m_pSVM->m_pRecIndexs));
	if (pTempDataAccess==NULL)
	{
		return 1;
	}
	//add end  
	m_pSVM->SetSourceAccess(pTempDataAccess);
//	m_pSVM->SetSourceAccess(&m_pDataInterface->m_DataAccess);
	m_pSVM->SetAlphaTableName("Alpha.tl");

/*
	m_pSVM->SetParam(m_pPageSVM->m_bIsLinear,m_pPageSVM->m_nKernFuncKind,
				m_pPageSVM->m_nSigma,m_pPageSVM->m_nOrder,m_pPageSVM->m_fPenalty,
				m_pPageSVM->m_dbTolerance,m_pPageSVM->m_dbEps);
*/
	//svm->SetStoreMode(false);
	//6--10
	bool bOK=m_pSVM->InitData();
	if(!bOK)
	{
		return 1;
	}
	//10--60
	m_pSVM->SMOCalc();
	//61--80
	double dbCorRate;
	dbCorRate=m_pSVM->Evaluate();
	//81--90
	m_pSVM->SetCorrectRate(dbCorRate);
	CTString strResult;

	CResult* pResult=new CResult("支持向量机");

	//add table
	CTLTable * pTable = new CTLTable;
	CTString strHead,strInfo,strTmp;
	if (!m_pSVM->IsLinear())
		strTmp=_T("选择线形不可分\r\n");
	else
		strTmp=_T("选择线形可分\r\n");
	strHead += strTmp;
	strTmp.Format("在惩罚因子=%.4f、KKT条件=%.4f、Eps值=%.4f",
		m_pSVM->GetCeilAlpha(),m_pSVM->GetTolerance(),m_pSVM->GetEps());
	strHead += strTmp;
	if (!m_pSVM->IsLinear())
	{
		strTmp.Format("、\r\n核函数为 %s 时\r\n",m_pSVM->GetKernFunc().GetData());
	}
	else
	{
		strTmp=_T("时\r\n");
	}
	strHead += strTmp;
	strTmp.Format("分类正确率=%.4f\r\nb=%.4f\r\n",dbCorRate,m_pSVM->GetB());
	strHead += strTmp;

	strInfo.Empty();
	if (m_pSVM->IsLinear())
	{
		strTmp.Format("w=(");
		strInfo=strInfo;
		strInfo+=strTmp;
		
		
		int vLen;
		double dbV;
		vLen=m_pSVM->GetColNum()-1;
		dbV=m_pSVM->GetWeight(0);

		strTmp.Format("%.4f",dbV);
		strInfo=strInfo;
		strInfo+=strTmp;
		for (int i=1;i<vLen;i++)
		{

			dbV=m_pSVM->GetWeight(i);
			strTmp.Format(",%.4f",dbV);
			strInfo=strInfo;
			strInfo+=strTmp;
		}
		strInfo+=_T(")\n");
		strHead += strInfo;
	}
	pTable->SetTitle("支持向量机结果表");

	//find vector-sum 
	int nNext,nSeq;
	nNext=m_pSVM->FindNextSVectorIndex(-1);

	nSeq=1;
	while( nNext>=0)
	{
		nNext=m_pSVM->FindNextSVectorIndex(nNext);
		nSeq++;
	}

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(0,"支持向量");
	pTable->InsertColumn(1,"记录序号");
	pTable->InsertColumn(2,"alpha值");

	strInfo.Empty();
	nNext=m_pSVM->FindNextSVectorIndex(-1);

	nSeq=0;
	while( nNext>=0)
	{
		strTmp.Format("%d",nSeq+1);
		pTable->InsertItem(nSeq,strTmp);
		strTmp.Format("%d",nSeq+1);
		pTable->SetItemText(nSeq,1,strTmp);	
		//strTmp.Format("%d",nNext+1); modified by liytao 20021227
		strTmp.Format("%d",m_pSVM->m_pRecIndexs[nNext+1]);
		pTable->SetItemText(nSeq,2,strTmp);	
		strTmp.Format("%.4f",m_pSVM->GetAlpha(nNext));
		pTable->SetItemText(nSeq,3,strTmp);	
		
		nNext=m_pSVM->FindNextSVectorIndex(nNext);
		nSeq++;
	}
	strTmp.Format("支持向量个数=%d\r\n",nSeq);
	strHead=strHead;
	strHead+=strTmp;
	//add by liytao 20021230
	strTmp.Format("所处理记录个数:%d;丢弃的记录个数:%d\n",m_pSVM->GetSourceAccess()->GetRecordCount(), m_pSVM->m_nMissingRecNum);
	strHead=strTmp;
	strHead+=strHead;
	//add end
	
	pTable->SetHead(strHead);

	//add by liytao 20030214
	if (nSeq>0)
	{
		CRsltElementTable * pE = new CRsltElementTable("支持向量机结果表",pTable);
		pResult->Add(pE);
		m_pSVM->m_SVNum=nSeq;
	}
	else
	{
		if( pTable)
			delete pTable;
		pTable=NULL;
		CRsltElementText * pText = new CRsltElementText("警告(支持向量机结果表)\n");
		strTmp.Format("没有生成支持向量，不显示支持向量机结果表\n");
		pText->AddString(strTmp);
		pResult->Add(pText);	
		m_pSVM->m_SVNum=0;
	}
	pResult->Print();
	return 0;
}

void	CSVMThread::SetDataParam(CSVMTrainVO *pVO)
{
}

CDataAccess* CSVMThread::DelInvalidRec(CDataAccess *pDataAccess)
{
	CDataAccess  *pTempDataAccess=new CDataAccess(pDataAccess);
	CFieldList *pSFList,*pTFList;
	CField *field;
	pSFList=pDataAccess->m_pFieldList;
	pTFList=pTempDataAccess->m_pFieldList;
	int i,nFC=pSFList->GetFieldCount();
	bool bVal;
	int nVal;
	double dblVal;
	CTString strVal;


	pDataAccess->First();
	bool bIgnore;
	while ( !pDataAccess->Eof()) 
	{
		bIgnore=false;
		for ( i=0;i<nFC&& !bIgnore;i++)
		{
			field=pSFList->FieldByIndex(i);
			if (field->IsNull())
				bIgnore=true;
		}
		if (bIgnore)
		{
			pDataAccess->Next();
			continue;
		}
		pTempDataAccess->Append();
		for ( i=0;i<nFC&& !bIgnore;i++)
		{
			field=pSFList->FieldByIndex(i);
			switch(field->GetFieldType()){
				case fBoolean:
					bVal=pSFList->FieldByIndex(i)->GetAsBool();
					pTFList->FieldByIndex(i)->SetAsBool(bVal);					
					break;
				case fInt:
					nVal=pSFList->FieldByIndex(i)->GetAsInteger();
					pTFList->FieldByIndex(i)->SetAsInteger(nVal);					
					break;
				case fDouble:
				case fCurrency:
				case fDate:
					dblVal=pSFList->FieldByIndex(i)->GetAsDouble();
					pTFList->FieldByIndex(i)->SetAsDouble(nVal);					
					break;
				case fString:
					strVal=pSFList->FieldByIndex(i)->GetAsString();
					pTFList->FieldByIndex(i)->SetAsString(strVal);					
					break;	
			default:
			break;
			}
		}
		pTempDataAccess->Post();
		pDataAccess->Next();
	}




	return pTempDataAccess;

}

bool CSVMThread::HaveNullRec(CDataAccess *pAccess)
{
	CFieldList *pSFList;
	CField *field;
	pSFList=pAccess->m_pFieldList;
	int nFC=pSFList->GetFieldCount();
	pAccess->First();
	int nAvailCount=0;
	int i;
	while ( !pAccess->Eof()) 
	{
		bool bHave=false;
		for ( i=0;i<nFC&& !bHave;i++)
		{
			field=pSFList->FieldByIndex(i);
			if (field->IsNull())
				bHave=true;
		}
		if (!bHave)
			nAvailCount++;
		pAccess->Next();
	}
	if (nAvailCount > 0)
		return false;
	else return true;




}

bool CSVMThread::HaveFieldInvalid(CDataAccess *pAccess)
{
	CFieldList *pSFList=pAccess->m_pFieldList;
	int nFC=pSFList->GetFieldCount();
	if (nFC!=m_pSVM->GetColNum()-1)
		return true;
	CField* field;
	for (int i =0;i<nFC;i++)
	{
		field=pSFList->FieldByIndex(i);
		if(field->GetFieldType()==fString)
			return true;
	}
	return false;
}

bool CSVMThread::CheckPredict(CDataAccess *pAccess)
{
	if (HaveFieldInvalid(pAccess))
		return false;
	//if (HaveNullRec(pAccess)) modified by liytao 20021230
	//	return false;
	return true;
}

CDataAccess* CSVMThread::PretreatData(CDataAccess *pDataAccess, int **pIndexs)
{
	CDataAccess *pTempDataAccess;
	if (m_pSVM->m_TrainVarIndex.nTargetIndex<0)
		return NULL;
//	int nFC=CreateAccessStruct(pDataAccess,&pTempDataAccess,);
	int nFC=CreateAccessStruct(pDataAccess,&pTempDataAccess,&m_pSVM->m_TrainVarIndex);
	if (nFC<=1)
		return NULL; 
	CFieldList *pSFList=pDataAccess->m_pFieldList,*pTFList=pTempDataAccess->m_pFieldList;
	int nRec,nValid=0;
	//nFC=pSFList->GetFieldCount();
	nRec=pDataAccess->GetRecordCount();
	int *RecIndexs=new int [nRec+1];
	memset(RecIndexs,0x00,(nRec+1)*sizeof(int));
	// add	by liytao 20030122
	char nYFType=0x00;
	CField * pYField = pTFList->FieldByIndex(nFC-1);
	CField * pSourceField;
	CField * pDestField ;
	CFieldType ft=pYField->GetFieldType();
	//add end
	bool bStop =false;
	pDataAccess->First();
	while (!pDataAccess->Eof()&&!bStop)
	{

		CField* field;
		bool bNull=false;
		for (int i =0;i<nFC-1&&!bNull;i++)
		{
			field=pSFList->FieldByIndex(m_pSVM->m_TrainVarIndex.pVarIndexs[i]);
			if(field->IsNull())
				bNull=true;
		}
		field=pSFList->FieldByIndex(m_pSVM->m_TrainVarIndex.nTargetIndex);
		if(field->IsNull())
			bNull=true;
		if (bNull)
		{
			pDataAccess->Next();
			continue;
		}
		pTempDataAccess->Append();		
		//add by liytao 20030122
		pSourceField = pSFList->FieldByIndex(m_pSVM->m_TrainVarIndex.nTargetIndex);
		pDestField = pTFList->FieldByIndex(nFC-1);
		int nIndex;
		switch(pSourceField->GetFieldType())
		{
			case fDate :
			case fString :
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsString());
				if (nIndex<0)
					m_pSVM->m_RealYs.Add(pSourceField->GetAsString());
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsString());
				if (nIndex==0)
						pDestField->SetAsInteger(1);
				if (nIndex==1)
						pDestField->SetAsInteger(-1);
				if( nIndex>1)
				{
					bStop=true;
					pDataAccess->Next();
					continue;
				}
				break;
			case fInt :
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsInteger());
				if (nIndex<0)
					m_pSVM->m_RealYs.Add(pSourceField->GetAsInteger());
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsInteger());
				if (nIndex==0)
						pDestField->SetAsInteger(1);
				if (nIndex==1)
						pDestField->SetAsInteger(-1);
				if( nIndex>1)
				{
					bStop=true;
					pDataAccess->Next();
					continue;
				}
				break;
			case fBoolean :
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsBool());
				if (nIndex<0)
					m_pSVM->m_RealYs.Add(pSourceField->GetAsBool());
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsBool());
				if (nIndex==0)
						pDestField->SetAsInteger(1);
				if (nIndex==1)
						pDestField->SetAsInteger(-1);
				if( nIndex>1)
				{
					bStop=true;
					pDataAccess->Next();
					continue;
				}
				break;
				//pDestField->SetAsDateTime(&pSourceField->GetAsDateTime());
			case fBlob :
			case fCurrency :
			case fDouble:
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsDouble());
				if (nIndex<0)
					m_pSVM->m_RealYs.Add(pSourceField->GetAsDouble());
				nIndex=m_pSVM->m_RealYs.Find(pSourceField->GetAsDouble());
				if (nIndex==0)
						pDestField->SetAsDouble(1);
				if (nIndex==1)
						pDestField->SetAsDouble(-1);
				if( nIndex>1)
				{
					bStop=true;
					pDataAccess->Next();
					continue;
				}

		}
		//add end
		//for(int j = 0;j < pSFList->GetFieldCount();j++) modified by liytao 20030122
		for(int j = 0;j < nFC-1;j++)
		{
			CField * pSourceField = pSFList->FieldByIndex(m_pSVM->m_TrainVarIndex.pVarIndexs[j]);
			CField * pDestField = pTFList->FieldByIndex(j);
			switch(pSourceField->GetFieldType())
			{
				case fDate :
					//pDestField->SetAsDateTime(&pSourceField->GetAsDateTime());
				case fString :
					pDestField->SetAsString(pSourceField->GetAsString());
					break;
				case fInt :
					pDestField->SetAsInteger(pSourceField->GetAsInteger());
					break;
				case fBoolean :
					pDestField->SetAsBool(pSourceField->GetAsBool());
					break;
					break;
				case fBlob :
					break;
				case fCurrency :
					//pDestField->SetAsCurrency(pSourceField->GetAsCurrency());
					break;
				case fDouble:
					pDestField->SetAsDouble(pSourceField->GetAsDouble());
					break;
			}
		}

		nValid++;
		RecIndexs[nValid]=pDataAccess->GetRecNo();
		pTempDataAccess->Post();
		pDataAccess->Next();
	}
	//if(nValid==0) modified by liytao 20030123
	if(nValid==0||bStop)
	{
		if( *pIndexs!=NULL)
			delete [](*pIndexs);
		delete []RecIndexs;
		delete pTempDataAccess;
		
		return NULL;
	}
	else
	{
		if( *pIndexs!=NULL)
			delete [](*pIndexs);
		*pIndexs=RecIndexs;
		m_pSVM->m_nMissingRecNum=pDataAccess->GetRecordCount()-nValid	;

	}
	return pTempDataAccess;

}
// add by liytao 20030123
int CSVMThread::GetVarIndexs(int **pVarFields, int *pTargetIndex)
{
	if (m_strInputField.GetSize()<=0 || m_strDestField.GetSize() <=0)
	{
		*pVarFields=NULL;
		return -1;
	}

	
	CTString str,strDec;
	bool bEmptyInput=true;
	CField *field;
	CFieldList*pFList= m_pDataInterface->m_DataAccess.m_pFieldList;
	int nTarget=-1;
	int i,j,nFC=m_strInputField.GetSize();
	int *pIndex=new int [nFC];
	for (i=0;i<nFC;i++)
		pIndex[i]=-1;

	j=0;
	for( i=0;i<nFC;i++)
	{

		CTString strField = m_strInputField.GetAt(i);
		field = pFList->FieldByName(strField);
		CFieldType ft=field->GetFieldType();
		if (ft!=fDouble &&ft!=fInt&&ft!=fCurrency)
		{
			delete[] pIndex;
			if (pTargetIndex!=NULL)
				*pTargetIndex=-1;
			*pVarFields=NULL;
			return -1;
		}
		else
			pIndex[j]= m_pDataInterface->IndexByName(strField);
		j++;
		
		bEmptyInput=false;
	}

	CTString strTarget = m_strDestField.GetAt(0);
	nTarget = m_pDataInterface->IndexByName(strTarget);
	if (pTargetIndex!=NULL)
		*pTargetIndex=nTarget;
	if (!bEmptyInput)
	{
		if (pTargetIndex!=NULL)
			*pTargetIndex=nTarget;
		*pVarFields=pIndex;
		return j;

	}
	return -1;	
}

int CSVMThread::CreateAccessStruct(CDataAccess *pSrcAccess, CDataAccess **pTargetAccess, VarIndexs* pVarIndexs)
{
	if ( pSrcAccess==NULL||pVarIndexs==NULL)
	{
		*pTargetAccess=NULL;
		return -1;
	}
	if (pVarIndexs->nVarCount<=0)
	{
		*pTargetAccess=NULL;
		return -1;
	}

	CDataAccess *pTempDataAccess=new CDataAccess(pSrcAccess);
	CFieldList *pSFList=pSrcAccess->m_pFieldList,*pTFList=pTempDataAccess->m_pFieldList;
	int nFC=0,nValid=0;
	nFC=pSFList->GetFieldCount();
	if (nFC<=0)
	{
		*pTargetAccess=NULL;
		return -1;
	}

	m_pSVM->m_RealYs.Empty();//add by liytao 20030123
	// add	by liytao 20030122
	char nYFType=0x00;
	CField * pYField = pTFList->FieldByIndex(pVarIndexs->nTargetIndex);
	
	for (int i=nFC-1;i>=0;i--)
	{
		//delete invalid field
		if (pVarIndexs->Find(i)<0)
			pTFList->Delete(i);
		pTempDataAccess->Post();
	}
	
	if( pVarIndexs->nTargetIndex<0)
	{
		*pTargetAccess=pTempDataAccess;
		return pTFList->GetFieldCount();
	}


	CFieldType ft=pYField->GetFieldType();
	if (ft==fString||ft==fDate ||ft==fBoolean)
	{
		pYField=pTFList->CreateField(fInt);
		pTFList->Add(pYField);
		pTempDataAccess->Post();
		m_pSVM->m_RealYs.m_cType=ft;

	}
	else
	{
		pYField=pTFList->CreateField(ft);
		pTFList->Add(pYField);
		pTempDataAccess->Post();
		m_pSVM->m_RealYs.m_cType=ft;
	}
	*pTargetAccess=pTempDataAccess;
	return pTFList->GetFieldCount();
}

bool CSVMThread::Export(string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp(""), szText("");
	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeCreate|TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += SVM;
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText+= "\"" ;
	szText+= m_pDataInterface->m_DataAccess.GetFileName() ;
	szText+= "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = m_strInputField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText+= m_strInputField.GetAt(i);
	}
	//目标变量
	szText += " ";
	szText += VAROUTPUT;
	nSize = m_strDestField.GetSize();
	for (i=0; i<nSize; i++)
	{
		szText += " " ;
		szText+= m_strDestField.GetAt(i);
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
	nSize = m_strInputField.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strInputField.GetAt(i);
	}
	nSize = m_strDestField.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_strDestField.GetAt(i);
	}
	m_pSVM->Export(theFile);
	theFile.Close();
	
	return true;
}

bool CSVMThread::Import(string szFileName)
{
	int i=0, nSize=0;
	CTString szTemp("");

	TCLFile theFile;
	bool bOpen = theFile.Open(szFileName.c_str(), TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//截尾长度
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//开始写模型参数
	m_strInputField.RemoveAll();
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strInputField.Add(szTemp);
	}
	theFile >> nSize;
	m_strDestField.RemoveAll();
	for (i=0; i<nSize; i++)
	{
		theFile >> szTemp;
		m_strDestField.Add(szTemp);
	}
	m_pSVM = new CSVM;
	m_pSVM->Import(theFile);
	theFile.Close();
	
	return true;
}