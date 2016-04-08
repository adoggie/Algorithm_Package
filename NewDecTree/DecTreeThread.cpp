// DecTreeThread.cpp : implementation file
//

#include "StdAfx.h"
//#include "tldectree.h"
#include "DecTreeThread.h"
#include "DecTree.h"
#include "../Result/TChartDecTree.h"


/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// CDecTreeThread

//IMPLEMENT_DYNCREATE(CDecTreeThread, CWinThread)

CDecTreeThread::CDecTreeThread()
{
	this->m_pDecTree = new CDecTree;
}

CDecTreeThread::~CDecTreeThread()
{
	delete m_pDecTree;
	m_pDecTree = NULL;
}

BOOL CDecTreeThread::InitInstance(CDecTreeOutput& Out)
{
	Out.m_pDecTree = m_pDecTree;
	m_bStop=FALSE;
	return TRUE;
}

/*	-by pyx 2006-02-17
int CDecTreeThread::ExitInstance()
{
	TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}*/



/////////////////////////////////////////////////////////////////////////////
// CDecTreeThread message handlers
void PrintTree(CTreeNode * pNode,int nLevel = 0)
{
	CTString strPre;
	for (int i=0;i<nLevel ;i++)
		strPre += "\t";
	CTreeNode * pCurNode =pNode;	
	int nCurLevelPos = 1;
	do
	{
			
// 		TRACE(strPre);
		//TRACE(pCurNode->GetNodeLabel()+" Target "+pCurNode->m_pTargetField->strFieldName+"   ");//" Filter "+pCurNode->m_strFilter+
		if (pCurNode->Parent)
		{
			CTreeNode *Parent = (CTreeNode *)pCurNode->Parent;
// 			TRACE(Parent->GetNodeLabel() +" ["+ Parent->GetArcLabel(nCurLevelPos)+"]");
		}
		if (pCurNode->FirstChild)//有子节点
		{
// 			TRACE("\r\n");
			PrintTree((CTreeNode *)pCurNode->FirstChild,nLevel+1);
		}
		else
		{//已经到末尾节点了
// 			TRACE(" ==> "+pCurNode->GetNodeLabel() );
		}

		pCurNode = (CTreeNode*) pCurNode->NextSibling;
// 		TRACE("\r\n");
		nCurLevelPos++;
	}
	while (pCurNode);
}
/*
*根据树，打印出文字描述的规则放到strRuleAry  //#_S 2004-11-29 zlq
*/
void CDecTreeThread::PrintTreeIf(CTreeNode * pNode,CTStringArray &strRuleAry)
{

	CTreeNode * pCurNode =pNode;	
	int nCurLevelPos = 1;
	do
	{
		if (pCurNode->FirstChild)//有子节点
		{
			PrintTreeIf((CTreeNode *)pCurNode->FirstChild,strRuleAry);
		}
		else
		{//已经到末尾节点了
			CTStringArray strArray;
			CTreeNode * pTempNode =pCurNode;
			while (pTempNode->Parent)
			{
				int nChildNum = CTreeNode::GetChildNum(pTempNode);
				pTempNode = (CTreeNode *)pTempNode->Parent;
				CTString strTemp = pTempNode->GetNodeLabel(m_pDataInterface->GetShowValueLabel()) +"  属于 "+ 
				pTempNode->GetArcLabel(nChildNum,m_pDataInterface->GetShowValueLabel());
				CTString ssst = pTempNode->GetArcLabel(nChildNum,m_pDataInterface->GetShowValueLabel());
				strArray.Add(strTemp);
			}
			CTString strCondition;
			for (int i=0;i<strArray.GetSize();i++)
			{
				if (strCondition.GetLength())
					strCondition= ""+strCondition+"\r\n  并且: "+strArray.GetAt(strArray.GetSize()-i-1);
				else{
					strCondition= "  如果: ";
					strCondition+=strArray.GetAt(strArray.GetSize()-i-1);
				}
			}
			//TRACE("规则\r\n");
			//TRACE(strCondition);
			//TRACE("\r\n那么: "+pCurNode->m_pTargetField->strFieldName+"  ===> "+ pCurNode->GetNodeLabel() +"\r\n\r\n");
			CTString strNum;strNum.Format("规则%d",strRuleAry.GetSize()+1);
			CTString strRule = strCondition+"\r\n  那么: ";
			strRule+=pCurNode->m_pTargetField->strFieldName+"  ===> ";
			strRule+= pCurNode->GetNodeLabel(m_pDataInterface->GetShowValueLabel()) +"\r\n\r\n";
			CTString stemp;
			stemp = strNum+"\r\n";
			stemp+=strRule;
			strRuleAry.Add(stemp);
			TREERULE  * pRule = new TREERULE;
			pRule->strFilter = pCurNode->m_strFilter;
			pRule->strRule = strNum;
			m_pDecTree->m_vectTreeRule.push_back(pRule);
		}

		pCurNode = (CTreeNode*) pCurNode->NextSibling;

		nCurLevelPos++;
	}
	while (pCurNode);
}

CResult* CDecTreeThread::RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CDecTreeVO *pVO, CDecTreeOutput *&pOutStruc)
{
	m_bStop = false;
	//显示标签
	pDataInterface->SetShowValueLabel(pVO->bShowLabel);
	m_pDecTree->SetStopCheck(&m_bStop);
	CResult* pResult=new CResult("决策树");

	m_pDataInterface = pDataInterface;
	m_pDecTree->m_pSourceAccess=&pDataInterface->m_DataAccess;
	m_pDecTree->SetTempTableName("DecTreeTemp.tl");
	CTString str,strDec;
	BOOL bEmptyInput=TRUE;
	CField *field;
	CFieldList*pFList=pDataInterface->m_DataAccess.m_pFieldList;
	int nTarget=-1,nType;
	m_pDecTree->ClearfieldList();
	int i;
	for( i=0;i<pVO->strInputNames.GetSize();i++)
	{
		int nIndex = pDataInterface->m_DataAccess.IndexByName(pVO->strInputNames.GetAt(i));
		nType = pDataInterface->m_DataAccess.GetDCDataType(nIndex);
		switch(nType)
		{
			case DISPERSE_DATA:			//离散的
				field=m_pDecTree->m_pSourceAccess->m_pFieldList->FieldByIndex(nIndex);
				m_pDecTree->AddField(field->GetFieldName(),TRUE);
				break;
			case CONTINU_DATA:			//连续的
				field=m_pDecTree->m_pSourceAccess->m_pFieldList->FieldByIndex(nIndex);
				m_pDecTree->AddField(field->GetFieldName(),FALSE);
				break;
		}
		bEmptyInput=FALSE;
	}

	if(bEmptyInput)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "没有指定输入变量字段！不能运行决策树！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}

	nTarget= pDataInterface->m_DataAccess.IndexByName(pVO->strDecNames.GetAt(0));
	if( nTarget<0 )
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "没有指定目标变量字段！不能运行决策树！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}

	if (pFList->FieldByIndex(nTarget)->GetFieldType()==fDouble)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "目标变量不能为浮点型";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	field=m_pDecTree->m_pSourceAccess->m_pFieldList->FieldByIndex(nTarget);
	m_pDecTree->SetTargetField(field->GetFieldName());

	if (m_bStop)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}

	m_pDecTree->SetTreeParam(pVO->nMaxBranch,pVO->nDepth,
				pVO->nMinSample,pVO->dErrorRate,
				pVO->dMinInfoGain,pVO->dMinGINIReduction);
	
	m_pDecTree->m_szArrHitRange.Copy(pVO->szArrHitRange);     //模型评估值时的范围
	m_pDecTree->m_bShowLabel        = pVO->bShowLabel;        //是否显示Label
	m_pDecTree->m_nGenType          = pVO->nGenTreeMethod;
	m_pDecTree->m_nPruneType        = pVO->nPrunMethod;
	m_pDecTree->m_nMaxBranches      = pVO->nMaxBranch;
	m_pDecTree->m_nMaxDepth         = pVO->nDepth;
	m_pDecTree->m_nMinCaseLeaf      = pVO->nMinSample;
	m_pDecTree->m_fMaxErrorRate     = pVO->dErrorRate;
	m_pDecTree->m_fMinInfoGain      = pVO->dMinInfoGain;
	m_pDecTree->m_fMinGINIReduction = pVO->dMinGINIReduction;
	
	int nGenType=pVO->nGenTreeMethod;
	int nPruneType=pVO->nPrunMethod;
	int m_nMaxBranches=pVO->nMaxBranch;
	int m_nMaxDepth=pVO->nDepth;
	int m_nMinCaseLeaf=pVO->nMinSample;
	double m_fMaxErrorRate=pVO->dErrorRate;
	double m_fMinInfoGain=pVO->dMinInfoGain;
	double m_fMinGINIReduction=pVO->dMinGINIReduction;
	
	bool bRes=m_pDecTree->CreateTree(nGenType,nPruneType);
	if (m_bStop)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	if (!bRes)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "数据过于离散或无有效数据，或目标变量类值太多，无法分析";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}

	CTreeNode *pTNode = m_pDecTree->m_rootNode;
	CRsltElementTable *pDecTable= NULL;
	int nDisc = m_pDecTree->m_DiscreteFieldList.size();
	int nCons = m_pDecTree->m_ContinueFieldList.size();
	if ((nDisc+nCons > 0)&&(pTNode->m_Importic.size() > 0))
	{
		CTLTable  * pDeTable = new CTLTable;
		pDeTable->SetTitle("决策树变量分类重要性");
		pDeTable->SetRows(nDisc+nCons + 1);
		pDeTable->SetCols(2);
		pDeTable->InsertColumn(0,"变量名称");
		pDeTable->InsertColumn(1,"重要性");
		double dblMax = pTNode->m_Importic.at(0)->dblValue;
		for (i = 1; i< nDisc+nCons;i++)
		{
			if (dblMax < pTNode->m_Importic.at(i)->dblValue)
				dblMax = pTNode->m_Importic.at(i)->dblValue;
		}
		for(i=0;i<nDisc+nCons;i++)
		{
			CTString str;
			pDeTable->InsertItem(i,pTNode->m_Importic.at(i)->strName);
			str.Format("%.2f",pTNode->m_Importic.at(i)->dblValue/dblMax*100);
			pDeTable->SetItemText(i,1,str);
		}
		pDecTable=new CRsltElementTable("决策树变量分类重要性",pDeTable);//申请一个表对象类的对象
	}
	if (m_bStop)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}

	CPtrArray *pNodeArray=bridge(m_pDecTree->m_rootNode);
	//add end
	if(pNodeArray->GetSize()<=0)
	{		
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "决策树画面生成失败！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	//////////////Begin 决策树图-add by xds 20060510////////////////////
	CTChartDecTree *pTChart  = new CTChartDecTree;
	pTChart->SetDataofTree(pNodeArray);
	CRsltElementChart *pEChart = new CRsltElementChart("决策树",pTChart);
	pResult->Add(pEChart);
	//////////////End 决策树图-add by xds 20060510/////////////////////
	CTString strText,strTmp;
	strTmp=_T("(");
	strText+=strTmp;
	strTmp.Format("分裂时一个分支的最小样本数大于%d \n",m_pDecTree->m_nMinCaseLeaf);
	strText+=strTmp;
	strTmp.Format("连续值分叉结点的最大分叉数%d \n",m_pDecTree->m_nMaxBranches);
	strText+=strTmp;
	// GenerateType:
	//GENTREE_GINI	//树生成GINI系数
	//GENTREE_ENTROPY	//树生成信息增益

	//PruneType:
	//PPEPRUNE_GINI				//先剪枝GINI系数
	//PREPRUNE_ENTROPY			//先剪枝信息增益
	//PREPRUNE_DEPTH			//先剪枝深度
	//POSTPRUNE_ERRRATE			//后剪枝错误率

	switch(nGenType)
	{

	case GENTREE_GINI:
		strTmp=_T("生成树方法为GINI系数 \n");
		strText+=strTmp;
		break;
	case GENTREE_ENTROPY:
		strTmp=_T("生成树方法为信息熵 \n");
		strText+=strTmp;
		break;
	default:
		;
	}
	
	switch(nPruneType)
	{
	case PREPRUNE_NOPRUNE:
		strTmp=_T("树剪枝的方法为不剪枝)\n");
		strText+=strTmp;

		break;
	case PREPRUNE_ENTROPY:
		strTmp.Format("树剪枝的方法： 信息增益，最小信息增益为%.4f)\n",m_fMinInfoGain);
		strText+=strTmp;
		break;
	case PREPRUNE_DEPTH:
		strTmp.Format("树剪枝的方法：深度剪枝，最大深度为%d)\n",m_nMaxDepth);
		strText+=strTmp;
		break;
	case PREPRUNE_GINI	:
		strTmp.Format("树剪枝的方法：GINI系数，最小GINI增益为%4f)\n",m_fMinGINIReduction);
		strText+=strTmp;
		break;
	case POSTPRUNE_ERRRATE:
		strTmp.Format("树剪枝的方法：错误率，最大错误率为%4f)\n",m_fMaxErrorRate);
		strText+=strTmp;
		break;
	default:
		;
	}

	//add by liytao 20021218
	CRsltElementText *pText=new CRsltElementText("决策树结果信息");
	double dbCorrect=m_pDecTree->m_dbCorrect;
	
	strTmp.Format("决策树正确率:%.1f％\n",dbCorrect*100);
	strText=strTmp+strText.GetData();

	//add by liytao 20021230
	strTmp.Format("所处理记录个数:%d;丢弃的记录个数:%d\n",pDataInterface->m_DataAccess.GetRecordCount(), m_pDecTree->m_nMissingRecNum);
	strText=strTmp+strText.GetData();
	pText->AddString(strText);
	pResult->Add(pText);
	//add end
	//pResult->Add(pVariable);
	if (pDecTable != NULL)
		pResult->Add(pDecTable);
	if (m_pDecTree->m_rootNode->FirstChild)
	{//打印文字描述
		CTreeNode * pNode =(CTreeNode*) m_pDecTree->m_rootNode;
		CTStringArray strRuleAry;
		PrintTreeIf(pNode,strRuleAry);
		CRsltElementText *pTextRule=new CRsltElementText("决策树规则描述");
		pTextRule->SetName("决策树规则描述");
		for (int i=0;i<strRuleAry.GetSize();i++)
		{
			pTextRule->AddString(strRuleAry.GetAt(i));
		}
		pResult->Add(pTextRule);
	}
	//导出
	pOutStruc = new CDecTreeOutput;
	pOutStruc->m_pDecTree = m_pDecTree;

	return pResult;
}

bool CDecTreeThread::UpdateField(TPredVO *pVO, CDecTreeOutput *pOutStruc)
{
	int i=0, nSize=0;
	CTString szTemp("");
	CTStringArray arrTemp;

	if (pVO == NULL || pOutStruc == NULL)
		return false;
	nSize = m_pDecTree->m_ContinueFieldList.size();
	if (nSize < 1)
		return false;
	for (i=0; i<nSize; i++)
	{
		szTemp = m_pDecTree->m_ContinueFieldList.at(i)->strFieldName;
		arrTemp.Add(szTemp);
	}
	if (!AnaSyntax::Replace(arrTemp,pVO->arFieldInfo,pVO->szArrReplace))
		return false;
	for (i=0; i<nSize; i++)
	{
		FieldClass *pFldClass = m_pDecTree->m_ContinueFieldList.at(i);
		pFldClass->strFieldName = arrTemp.GetAt(i);
	}
	/*
	if (pVO->szOutput.IsEmpty())
		m_pDecTree->m_TargetField.strFieldName += "_预测值";
	else
		m_pDecTree->m_TargetField.strFieldName = pVO->szOutput;
	*/
	return true;
}

CResult*  CDecTreeThread::RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,TPredVO *pVO, CDecTreeOutput *pOutStruc)
{
	CTString  strTargetName;

	//更新字段...
	if (!UpdateField(pVO, pOutStruc))
		return false;
	//是否为评估
	m_pDecTree->m_bOutput = !pVO->bEvaluate; //若为评估，则不输出到文件
	//显示标签
	pDataInterface->SetShowValueLabel(m_pDecTree->m_bShowLabel);
	CResult* pResult=new CResult("决策树预测结果");
	m_pDecTree->ClearAccess();
	m_pDataInterface = pDataInterface;
	m_pDecTree->m_pSourceAccess=&m_pDataInterface->m_DataAccess;
	m_pDecTree->SetTempTableName("DecTreeTemp.tl");

	CTString str,strDec;
	bool bEmptyInput=TRUE;
	CFieldList*pFList=m_pDataInterface->m_DataAccess.m_pFieldList;
	int i;
	for ( i=0; i <m_pDecTree->m_DiscreteFieldList.size();i++)
	{
		CTString strField = m_pDecTree->m_DiscreteFieldList.at(i)->strFieldName;
		CField *pField = pFList->FieldByName(strField);
		if (pField == NULL)
		{
			CRsltElementText *pText = new CRsltElementText;
			CTString strTitle = "警告";
			CTString strInfo = "输入字段不存在，请重新选择！";
			pText->SetName(strTitle);
			pText->AddString(strInfo);
			pResult->Add(pText);
			return pResult;
		}
	}
	if (m_bStop)
	{
		//SendCalcOverMsg(WM_CALCULATE_OVER,NULL);	-by pyx 2006-02-17
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	
	for (i=0; i <m_pDecTree->m_ContinueFieldList.size();i++)
	{
		CTString strField = m_pDecTree->m_ContinueFieldList.at(i)->strFieldName;
		CField *pField = pFList->FieldByName(strField);
		if (pField == NULL)
		{
			CRsltElementText *pText = new CRsltElementText;
			CTString strTitle = "警告";
			CTString strInfo = "输入字段不存在，请重新选择！";
			pText->SetName(strTitle);
			pText->AddString(strInfo);
			pResult->Add(pText);
			return pResult;
		}
	}
	if (m_bStop)
	{
		//SendCalcOverMsg(WM_CALCULATE_OVER,NULL);	-by pyx 2006-02-17
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	//评估及预测
	bool bRes = m_pDecTree->PreDecTree(pResult);
	if (m_bStop)
	{
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "用户强制停止！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	if (!bRes)
	{		
		CRsltElementText *pText = new CRsltElementText;
		CTString strTitle = "警告";
		CTString strInfo = "无法进行预测结果！";
		pText->SetName(strTitle);
		pText->AddString(strInfo);
		pResult->Add(pText);
		return pResult;
	}
	if (m_pDecTree->m_bOutput) //预测不评估-输出到文件
	{
		CRsltElementText *pText = new CRsltElementText();
		CTString strTitle = "运行结果信息";
		pText->SetName(strTitle);
		pText->AddString(m_pDecTree->m_SaveFileName);
		pResult->Add(pText);
	}
	return pResult;
}



CPtrArray* CDecTreeThread::bridge(CTreeNode * m_node)
{
	DecisionTreeNode * myNode;
	DecisionTreeNode * tempNode;
//	CTreeNode * otherNode;
	CPtrArray *pNodeList;
	pNodeList=new CPtrArray();
	CTString nodeLabel,str,strNodeID,strRate;
	int childNum,i;
	tempNode= new DecisionTreeNode();
	myNode= new DecisionTreeNode();

	//nodeLabel赋值
	nodeLabel=m_node->GetNodeLabel(m_pDataInterface->GetShowValueLabel());
	childNum=CTreeNode::GetChildrenCount(m_node);
	myNode->setNodeLabel (nodeLabel);
	myNode->isRoot =true;
	strNodeID="root ";
	//myNode->nodeID = strNodeID;	-by pyx 2006-02-17
	myNode->setArcNum(childNum);
	myNode->setChildNum (childNum);
	//此处需要加arcLabels

	CTreeNode * subNode=NULL;

	for(i=0;i<childNum;i++)
	{
		// 增加百分比  mzh
		subNode=(CTreeNode *)CTreeNode::GetChild(m_node,i+1);
		double dRate = 0;
		if (m_node->m_TotalNum != 0)
			dRate = (double)subNode->m_TotalNum/(double)m_node->m_TotalNum * 100;
		strRate.Format("%.2f",dRate); 
		str=m_node->GetArcLabel(i+1,m_pDataInterface->GetShowValueLabel());
		str = str + "\r\n" + strRate.GetData() + "%"; 
		if (str.GetLength ()<1 )
			str.Format ("%d",i);
		myNode->setArcLabel (str,i);
	}

	pNodeList->Add(myNode);



	for (i=1;i<=childNum;i++)
	{
		subNode=(CTreeNode *)CTreeNode::GetChild(m_node,i);
		bridgeNode(subNode,myNode,i,pNodeList);
	}

	if (tempNode !=NULL) 
	{
		delete tempNode;
		tempNode=NULL;
	}

	return pNodeList;
}

///对所有的子结点进行遍历
void CDecTreeThread::bridgeNode(CTreeNode * m_node,DecisionTreeNode * parentNode,int num,CPtrArray* pNodeList)
{
	DecisionTreeNode * myNode;
	DecisionTreeNode * tempNode;
	CTString nodeLabel,str,temp,strNodeID,strRate;
	int i,childnum;

//	CTreeNode * otherNode;
	tempNode= new DecisionTreeNode();
	myNode= new DecisionTreeNode();
	
	nodeLabel=m_node->GetNodeLabel(m_pDataInterface->GetShowValueLabel());
	childnum=CTreeNode::GetChildrenCount(m_node);
	if (nodeLabel.GetLength ()<1 )
		nodeLabel="空";
	myNode->setNodeLabel (nodeLabel);
	myNode->setArcNum(childnum);
	myNode->setChildNum (childnum);
	CTreeNode * subNode=NULL;
	//此处需要加arcLabels
	for(i=0;i<childnum;i++)
	{
		// 增加百分比  mzh
		subNode=(CTreeNode *)CTreeNode::GetChild(m_node,i+1);
		double dRate = 0;
		if (m_node->m_TotalNum != 0)
			dRate = (double)subNode->m_TotalNum/(double)m_node->m_TotalNum * 100;
		strRate.Format("%.2f",dRate); 
		str=m_node->GetArcLabel(i+1,m_pDataInterface->GetShowValueLabel());
		str = str + "\r\n" + strRate.GetData() + "%"; 
		if (str.GetLength ()<1 )
			str.Format ("%d",i);
		myNode->setArcLabel (str,i);
	}

	//strNodeID=parentNode->nodeID;	-by pyx 2006-02-17
	temp.Format ("  %d",i);
	strNodeID=strNodeID+nodeLabel.GetData();
	strNodeID+=temp;
	myNode->setParent (parentNode);
// 	myNode->nodeID =strNodeID;	-by pyx 2006-02-17
	myNode->getParent ()->setChild(myNode,num -1);

	pNodeList->Add(myNode);
	for (i=1;i<=childnum;i++)
	{
		subNode=(CTreeNode *)CTreeNode::GetChild(m_node,i);
		bridgeNode(subNode,myNode,i,pNodeList);
	}


	if (tempNode !=NULL) 
	{
		delete tempNode;
		tempNode=NULL;
	}
}

int	CDecTreeThread::GetTotalNodeModel(CPtrArray& arNode)
{
	arNode.RemoveAll();
	int i,nChild,nCount;
	TreeNode *pTreeNode = new TreeNode;
	m_pDecTree->m_rootNode->nID = arNode.GetSize();
	arNode.Add(pTreeNode);
	pTreeNode->nTreeID = m_pDecTree->m_rootNode->nID;
	pTreeNode->nParentID = -1;
	pTreeNode->bDiscrete = m_pDecTree->m_rootNode->m_bDiscrete;
	pTreeNode->nLevel = m_pDecTree->m_rootNode->m_nLevel;
	pTreeNode->nNumChild = m_pDecTree->m_rootNode->m_NumChild;
	pTreeNode->nNumLeaf = m_pDecTree->m_rootNode->m_NumLeaf;
	pTreeNode->nValueType = m_pDecTree->m_rootNode->nVal;
	pTreeNode->pClassCount = &m_pDecTree->m_rootNode->m_ClassCounts;
	pTreeNode->pSplitField = &m_pDecTree->m_rootNode->m_SplitField;
	pTreeNode->pImportic = &m_pDecTree->m_rootNode->m_Importic;
	pTreeNode->strLabel = m_pDecTree->m_rootNode->m_strNodeLabel;
	
	nChild = CTreeNode::GetChildrenCount(m_pDecTree->m_rootNode);
	if (nChild == 0)
		return 0;
	for (i=0;i<nChild;i++)
	{
		CTreeNode *pSubNode=(CTreeNode *)CTreeNode::GetChild(m_pDecTree->m_rootNode,i+1);
		TreeNode *pTreeNode = new TreeNode;
		pSubNode->nID = arNode.GetSize();
		arNode.Add(pTreeNode);
		pTreeNode->nTreeID = pSubNode->nID;
		pTreeNode->nParentID = m_pDecTree->m_rootNode->nID;
		pTreeNode->bDiscrete = pSubNode->m_bDiscrete;
		pTreeNode->nLevel = pSubNode->m_nLevel;
		pTreeNode->nNumChild = pSubNode->m_NumChild;
		pTreeNode->nNumLeaf = pSubNode->m_NumLeaf;
		pTreeNode->nValueType = pSubNode->nVal;
		pTreeNode->pClassCount = &pSubNode->m_ClassCounts;
		pTreeNode->pSplitField = &pSubNode->m_SplitField;
		pTreeNode->pImportic = &pSubNode->m_Importic;
		pTreeNode->strLabel = pSubNode->m_strNodeLabel;
		GetNodeModel(pSubNode,arNode);
	}

	nCount = arNode.GetSize();

	return nCount;
}

void CDecTreeThread::GetNodeModel(CTreeNode *pNode,CPtrArray &arNode)
{
	int i, nChild = CTreeNode::GetChildrenCount(pNode);
	if (nChild == 0)
		return;
	
	for (i=0;i<nChild;i++)
	{
		CTreeNode *pSubNode=(CTreeNode *)CTreeNode::GetChild(pNode,i+1);
		TreeNode *pTreeNode = new TreeNode;
		pSubNode->nID = arNode.GetSize();
		arNode.Add(pTreeNode);
		pTreeNode->nTreeID = pSubNode->nID;
		pTreeNode->nParentID = pNode->nID;
		pTreeNode->bDiscrete = pSubNode->m_bDiscrete;
		pTreeNode->nLevel = pSubNode->m_nLevel;
		pTreeNode->nNumChild = pSubNode->m_NumChild;
		pTreeNode->nNumLeaf = pSubNode->m_NumLeaf;
		pTreeNode->nValueType = pSubNode->nVal;
		pTreeNode->pClassCount = &pSubNode->m_ClassCounts;
		pTreeNode->pSplitField = &pSubNode->m_SplitField;
		pTreeNode->pImportic = &pSubNode->m_Importic;
		pTreeNode->strLabel = pSubNode->m_strNodeLabel;
		GetNodeModel(pSubNode,arNode);
	}
}

void CDecTreeThread::Stop()
{
	m_bStop=TRUE;
}































































































































