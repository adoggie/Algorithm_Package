// DecTree.cpp : implementation file
/********************************************************************************/
/******************      决策树主程序                          ******************/
/******************      创建者：  刘堂友                 ***********************/
/******************      创建日期：Oct.25 2002             ***********************/
/******************      修改者：                         ***********************/
/********************************************************************************/ 
#include "../Public/TString.h"
#include "DecTree.h"
#include "Evaluate.h"
#include "../Public/JavaRead.h"
#include "../Public/Util.h"
#include "../Public/System.h"
#define MAX_DISCRETE_VALNUM 100
#define DOUBLE_ZERO 1e-20
#ifdef _UNIX
	#include <string.h>
#else
	#include <stdio.h>
	#include <memory.h>
	#include <direct.h>
	#include <stdlib.h>
#endif
// Tree.cpp: implementation of the Tree class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CDecTreeOutput
CDecTreeOutput::CDecTreeOutput()
{
	m_pDecTree = NULL;
}

string CDecTreeOutput::Export()
{
	string str = "";
	return str;
}

bool CDecTreeOutput::Export(string str)
{
	if (m_pDecTree == NULL)
		return false;
	m_pDecTree->SaveTree(str);
	return true;
}

bool CDecTreeOutput::Import(string str)
{
	if (m_pDecTree == NULL || str.length() < 1)
		return false;
	m_pDecTree->LoadTree(str);
	return true;
}

Tree::Tree()
{
	Parent = NULL;
	FirstChild = NULL;
	NextSibling = NULL;
	PrevSibling = NULL;
	nVal = 0;
}

Tree::~Tree()
{
	
}

//DEL Tree* Tree::Width()
//DEL {
//DEL 	
//DEL 
//DEL 
//DEL 	return NULL;
//DEL }

//DEL void Tree::Print(CTString strMsg)
//DEL {
//DEL 	(CTreeTestDlg*)pParent->Print(strMsg);
//DEL }

//DEL void Tree::SetDispParent(CWnd *pWnd)
//DEL {
//DEL 	pDispParent=pWnd;
//DEL }

void Tree::Delete(Tree *tree)
{
	CTString strVal;
	Tree *pCur,*pPrev;
	if (tree==NULL)
		return;
	
	pCur=tree->FirstChild;
	if (pCur!=NULL)
	{
		while (pCur->NextSibling!=NULL)
			pCur=pCur->NextSibling;
		pPrev=pCur->PrevSibling;
	}
	else
	{
		pPrev=NULL;
	}
	while(pPrev!=NULL)
	{
		Delete(pCur);
		pPrev->NextSibling=NULL;
		pCur=pPrev;
		pPrev=pPrev->PrevSibling;
	}
	Delete(pCur);
		
	delete tree;
	return;


}

void Tree::AddChild(Tree *parent, Tree *child)
{
	if (parent==NULL)
		return;
	if (child==NULL)
		return;
	Tree *pCur;
	
	
	// add first child node
	if (parent->FirstChild==NULL)
	{
		child->Parent=parent;
		parent->FirstChild=child;
	}
	else
		// normal
	{
		pCur=parent->FirstChild;
		while (pCur->NextSibling!=NULL)
			pCur=pCur->NextSibling;
		child->Parent=parent;
		pCur->NextSibling=child;
		child->PrevSibling=pCur;
	}
	
}

void Tree::DeleteAllChildren(Tree *parent)
{
	Tree *pCur,*pNext=NULL;
	// have no child
	if(parent->FirstChild=NULL)
		return;
	
	pCur=parent->FirstChild;
	if (pCur!=NULL)
		pNext=pCur->NextSibling;
	while(pCur!=NULL)
	{
		Prune(pCur);
		pCur=pNext;
		pNext=pNext->NextSibling;
	}
	parent->FirstChild=NULL;
}

Tree * Tree::GetChild(Tree *parent, int nChildNo)
{
	if (parent==NULL||parent->FirstChild==NULL)
		return NULL;
	int nCount;
	nCount=GetChildrenCount(parent);
	// invalid childno
	if (nChildNo>nCount)
		return NULL;
	
	int nTmp;
	Tree *pCur;
	pCur=parent->FirstChild;
	nTmp=1;
	while (nTmp<nChildNo&&pCur!=NULL )
	{
		pCur=pCur->NextSibling;
		nTmp++;
	}
	if (pCur==NULL)
		return NULL;
	return pCur;
	
}

int Tree::GetChildrenCount(Tree *parent)
{
	int nCount=0;
	if (parent==NULL||parent->FirstChild==NULL)
		return nCount;
	Tree *pCur;
	// at least have first child
	pCur=parent->FirstChild;
	while (pCur!=NULL)
	{
		nCount++;
		pCur=pCur->NextSibling;
	}
	return nCount;
}

bool Tree::InsertChild(Tree *parent, Tree *child, int nChildNo)
{
	if (parent==NULL)
		return FALSE;
	if (child==NULL)
		return FALSE;
	int nChildCount;
	nChildCount=GetChildrenCount(parent);
	// in case of invalid nItemNo  or zero-child parent  
	if (nChildNo>nChildCount||nChildCount<=0)
		return FALSE;
	Tree	*pCur;
	// insert before first child
	if (nChildNo==1)
	{
		pCur=parent->FirstChild;
		parent->FirstChild=child;
		child->Parent=parent;
		child->NextSibling=pCur;
		pCur->PrevSibling=child;
		return TRUE;
	}
	else
		// others
	{
		pCur=Tree::GetChild(parent,nChildNo);
		if (pCur==NULL)
			return FALSE;
		child->Parent=parent;
		pCur->PrevSibling->NextSibling=child;
		child->PrevSibling=pCur->PrevSibling;
		
		child->NextSibling=pCur;
		pCur->PrevSibling=child;
		return TRUE;
	}
	
}

void Tree::Prune(Tree *tree)
{
	Tree *pPrev,*pNext,*pParent;
	pParent=tree->Parent;
	pNext=tree->NextSibling;
	pPrev=tree->PrevSibling;
	
	if (pPrev==NULL)
		// is first child
	{
		pParent->FirstChild=pNext;
		if (pNext!=NULL)
			pNext->PrevSibling=NULL;
		tree->NextSibling=NULL;
		Delete(tree);
	}
	else if (pNext==NULL)
		// is last child
	{
		
		pPrev->NextSibling=NULL;
		tree->PrevSibling=NULL;
		Delete(tree);
		
	}
	else if ((pPrev!=NULL)&&(pNext!=NULL))
		// others
	{
		
		pPrev->NextSibling=pNext;
		pNext->PrevSibling=pPrev;
		tree->PrevSibling=NULL;
		tree->NextSibling=NULL;
		Delete (tree);
	}
	
}

int Tree::GetChildNum(Tree *child)
{
	if(child->Parent==NULL||child ==NULL)
		return -1;
	Tree *pCur;
	pCur= child->Parent->FirstChild;
	// is first child
	if (pCur==child)
		return 1;
	int nNo=1;
	while (pCur!=child&& pCur!=NULL)
	{
		pCur=pCur->NextSibling;
		nNo++;
	}
	if (pCur==NULL)
		return -1;
	return nNo;
	
}

bool Tree::AreAllChildrenLeaves(Tree *tree)
{
	if (tree->FirstChild==NULL||tree==NULL)
		return FALSE;
	bool bOK=TRUE;
	Tree *child;
	child=tree->FirstChild;
	while (child!=NULL)
	{
		if (child->FirstChild!=NULL)
		{
			bOK=FALSE;
			break;
		}
		child =child->NextSibling;
	}
	return bOK;
}




////////////////////////////////////////////////////////////////////////////////////
////
// CTreeNode.cpp: implementation of the CTreeNode class.
//
//////////////////////////////////////////////////////////////////////
int CTreeNode::nPrgPos=0;
CTreeNode::CTreeNode()
{
	m_pDecTree = NULL;
	m_strNodeLabel="";
	m_TotalNum = 0;
	m_nLevel=0;
	m_nNodeType=LEAF;
	m_pbCheck=NULL;
	m_nIndex = -1;
	m_NumChild = 0;
	m_NumLeaf = 0;
	m_nNodeType = 0;
	m_pBranchNum = NULL;
	m_DiscreteFieldList.clear();
	m_ContinueFieldList.clear();
}

CTreeNode::CTreeNode(CDecTree *pDecTree)
{
	m_pDecTree = pDecTree;
	m_pDataAccess=NULL;
	m_TotalNum = 0;
	m_strNodeLabel="";
	m_pTargetField=NULL;
	m_nLevel=0;
	m_nNodeType=LEAF;
	m_pbCheck=NULL;
	m_nIndex = -1;
	m_NumChild = 0;
	m_NumLeaf = 0;
	m_nNodeType = 0;
	m_pBranchNum = NULL;
	m_DiscreteFieldList.clear();
	m_ContinueFieldList.clear();
}

CTreeNode::CTreeNode(CDataAccess *pDataAccess)
{
	m_pDecTree = NULL;
	m_pDataAccess=pDataAccess;
	m_TotalNum = 0;
	m_strNodeLabel="";
	m_pTargetField=NULL;
	m_nLevel=0;
	m_nNodeType=LEAF;
	m_pbCheck=NULL;
	m_nIndex = -1;
	m_NumChild = 0;
	m_NumLeaf = 0;
	m_nNodeType = 0;
	m_pBranchNum = NULL;
	m_DiscreteFieldList.clear();
	m_ContinueFieldList.clear();
}
CTreeNode::~CTreeNode()
{
	EmptyLists();
}

/*
void    CTreeNode::CheckData()
{
	if (Parent == NULL)
		return;

	double condMax = DBL_MAX;
	FieldClass  *field;
	field=&((CTreeNode*)Parent)->m_SplitField;
	int nChildNo=CTreeNode::GetChildNum(this);
	int nChildCount = ((CTreeNode*)Parent)->m_NumChild;
	FieldSourceData *pData;
	if (((CTreeNode*)Parent)->m_bDiscrete)
	{
		for (int i=0;i<m_pDecTree->m_arSourceData.GetSize();i++)
		{
			pData = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(i);
			if (pData->strFieldName == field->strFieldName)
				break;
		}
		for (i =0; i<m_pDecTree->m_nTotalCase;i++)
		{
			if ((pData->nType == fInt)||(pData->nType == fBoolean))
			{
				if (field->VarValue.at(nChildNo-1).nVal != pData->vectIntData(i))
					m_vtCheckData(i) = 0;
			}
			else if (pData->nType == fString)
			{
				if (pData->arStringData.GetAt(i).GetLength() == 0)
				{
					m_vtCheckData(i) = 0;
					continue;
				}
				
				if (pData->arStringData.GetAt(i).Compare(field->VarValue.at(nChildNo-1).strVal) != SC_EQUAL)
					m_vtCheckData(i) = 0;
			}
			else 
			{
				if (field->VarValue.at(nChildNo-1).nVal == pData->vectDoubleData(i))
					m_vtCheckData(i) = 0;
			}
		}
	}
	else 
	{
		for (int i=0;i<m_pDecTree->m_arSourceData.GetSize();i++)
		{
			pData = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(i);
			if (pData->strFieldName == field->strFieldName)
				break;
		}
		for (i =0; i<m_pDecTree->m_nTotalCase;i++)
		{
			double dVal = 0;
			if ((pData->nType == fBoolean)||(pData->nType == fInt))
			{
				dVal = pData->vectIntData(i);
			}
			else
			{
				dVal = pData->vectDoubleData(i);
			}
			for (int k=0;k<field->VarValue.size();k++)
			{
				if (nChildNo-1 != k)
				{
					if ((dVal >= field->VarValue.at(nChildNo-1).dblVal)||(dVal < field->VarValue.at(nChildNo).dblVal))
						m_vtCheckData(i) = 0;
					if (dVal >= field->VarValue.at(nChildNo-1).dblVal)
						m_vtCheckData(i) = 0;
				}
			}
		}
	}
}
*/

void    CTreeNode::FieldCheckData(CIntVector &Check_Vect)
{
	CTreeNode *parent;
	parent=(CTreeNode*) this->Parent;
	if (parent !=NULL)
		parent->FieldCheckData(Check_Vect);
	else
		return;

	double condMax = DBL_MAX;
	FieldClass  *field;
	field=&parent->m_SplitField;
	int nChildNo=CTreeNode::GetChildNum(this);
	int nChildCount = parent->m_NumChild;
	FieldSourceData *pData;
	int i;
	if (parent->m_bDiscrete)
	{
		for (i=0;i<m_pDecTree->m_nTotalCase;i++)
		{
			pData = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(i);
			if (pData->strFieldName == field->strFieldName)
				break;
		}
		for (i =0; i<m_pDecTree->m_nTotalCase;i++)
		{
			if ((pData->nType == fInt)||(pData->nType == fBoolean))
			{
				if (field->VarValue.at(nChildNo-1).nVal != pData->vectIntData(i))
					Check_Vect(i) = 0;
			}
			else if (pData->nType == fString)
			{
				if (pData->arStringData.GetAt(i).GetLength() == 0)
				{
					Check_Vect(i) = 0;
					continue;
				}

				if (pData->arStringData.GetAt(i).Compare(field->VarValue.at(nChildNo-1).strVal) != SC_EQUAL)
					Check_Vect(i) = 0;
			}
			else 
			{
				if (field->VarValue.at(nChildNo-1).nVal == pData->vectDoubleData(i))
					Check_Vect(i) = 0;
			}
		}
	}
	else 
	{
		for (int i=0;i<m_pDecTree->m_arSourceData.GetSize();i++)
		{
			pData = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(i);
			if (pData->strFieldName == field->strFieldName)
				break;
		}
		for (i =0; i<m_pDecTree->m_nTotalCase;i++)
		{
			double dVal = 0;
			if ((pData->nType == fBoolean)||(pData->nType == fInt))
			{
				dVal = pData->vectIntData(i);
			}
			else
			{
				dVal = pData->vectDoubleData(i);
			}
			if (nChildNo < nChildCount)
			{
				if ((field->VarValue.at(nChildNo-1).dblVal > dVal)||(field->VarValue.at(nChildNo).dblVal <= dVal))
					Check_Vect(i) = 0;
			}
			else
			{
				if (field->VarValue.at(nChildNo-1).dblVal > dVal)
					Check_Vect(i) = 0;
			}
		}
	}
}

void CTreeNode::ClearTempAccess()
{
}

//DEL void CTreeNode::SetLevel(int num)
//DEL {
//DEL 	m_nLevel=num;
//DEL }

//DEL int CTreeNode::GetLevel()
//DEL {
//DEL 	return m_nLevel;
//DEL }



void CTreeNode::SetNodeLabel(CTString label)
{
	if (label.GetLength  ( )<1){
		m_strNodeLabel=" ";
	}
	else{
		m_strNodeLabel= label;
	}
}

CTString CTreeNode::GetNodeLabel(bool bLabel)
{
	CTString strLabel,strVal;
	if(this->FirstChild==NULL)
	{
		int nValIndex,nVal;
		double dbAvail;
		nValIndex= this->GetMaxClass(dbAvail);
		switch(m_pTargetField->cFieldType){
		case fBoolean:
			
			if (this->m_pTargetField->VarValue[nValIndex].bVal)
				strVal="true";
			else
				strVal="false";
			break;
		case fInt:
			nVal=this->m_pTargetField->VarValue[nValIndex].nVal;

			strVal.Format("%d",nVal);
			break;
		case fString:
			strVal = this->m_pTargetField->VarValue[nValIndex].strVal;
			break;	
		default:
			break;
		}

		CTString labelVal;
		if ((bLabel)&&(m_pTargetField->cFieldType == fInt))
		{
			for (int i=0;i<m_pTargetField->FieldLabel.size();i++)
			{
				if (m_pTargetField->FieldLabel.at(i)->varValue.nVal == nVal)
					strVal = m_pTargetField->FieldLabel.at(i)->strLabelName;
			}
		}
		CTString str;
		str.Format("(可信度%.2f%%)",dbAvail*100);
		strVal += str;
	}
	else
	{
		strVal=this->m_SplitField.strFieldName;
	}
//	CTString str;
//	str.Format("(%d)",m_TotalNum);
//	strVal += str;
	return strVal;
}


void CTreeNode::SetTargetField(FieldClass* pFieldClass)
{
	m_pTargetField=pFieldClass;
}
void CTreeNode::SetTreeParam(int MaxBranches,int MaxDepth,int MinCaseLeaf,
							 float fErrorRatio,float fMinInfoGain,float fMinGINIReduction)
{
	m_nMaxBranches=MaxBranches;
	m_nMaxDepth=MaxDepth;
	m_nMinCaseLeaf=MinCaseLeaf;
	m_fMaxErrorRate=fErrorRatio;
	m_fMinInfoGain=fMinInfoGain;
	m_fMinGINIReduction=fMinGINIReduction;
}


bool CTreeNode::CalcEntropyGain(int nPruneType)
{	
	int Num;
	int nTemp=0;
	bool bBranches=TRUE;
	int  i,j,k,NumTotalCase;
	int  NumCont,NumDisc;
	int  NumHistgram=0;
	int *pClassCount=NULL;
	int *pTargetCount=NULL;
	int  NumTargetBranch;
	int *NumCasePerClass;
	double *dblMax=NULL,*dblMin=NULL;
	double *dblHistBorder=NULL;
	FIELDVALUEFREQUENCY *ContValFreqTemp=NULL;
	FIELDVALUEFREQUENCY *ContValFreq=NULL;
	FIELDVALUEFREQUENCY *DiscValFreq=NULL;
	FieldSourceData *pSource = NULL;
	NumTargetBranch=m_pTargetField->NumBranch;	
	NumCasePerClass=new int[NumTargetBranch];
	//::FillMemory(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	memset(NumCasePerClass,NumTargetBranch*sizeof(int),0); //--UNIX PORT
	NumCont=m_ContinueFieldList.size();
	NumDisc=m_DiscreteFieldList.size();

	if (NumCont==0&&NumDisc==0)
	{
			NumTotalCase = 0;
			int nCur = 0;
			for (i=0;i<m_pDecTree->m_arSourceData.GetSize();i++)
			{
				pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(i);
				if (pSource->strFieldName == m_pTargetField->strFieldName)
					break;
			}
			while(nCur<m_pDecTree->m_nTotalCase){
				if (m_vtCheckData(nCur) == 1)
				{
					if (m_pbCheck!=NULL)
					{
						if (*m_pbCheck)
						{
							delete []NumCasePerClass;			
							return false; 
						}
					}
					for (i=0;i<m_pTargetField->VarValue.size();i++)
					{
						if (m_pTargetField->cFieldType == fString)
						{
							CTString strTemp = pSource->arStringData.GetAt(nCur);
							if (strTemp.Compare(m_pTargetField->VarValue.at(i).strVal)==SC_EQUAL)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
						else if ((m_pTargetField->cFieldType == fBoolean)||(m_pTargetField->cFieldType == fInt))
						{
							nTemp= pSource->vectIntData(nCur);
							if (nTemp == m_pTargetField->VarValue.at(i).nVal)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
						else
						{
							 double dTemp= pSource->vectDoubleData(nCur);
							if (dTemp == m_pTargetField->VarValue.at(i).dblVal)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
					}
					NumTotalCase++;
				}
				nCur++;
			}
			for(j=0;j<NumTargetBranch;j++){
				if (m_pbCheck!=NULL)
				{
					if (*m_pbCheck)
					{
						delete []NumCasePerClass;			
						return false; 
					}
				}
				m_ClassCounts.push_back(NumCasePerClass[j]);
			}
		return FALSE;
	}

	if(NumCont>0){
		double dblTemp=0;
		dblMax=new double[NumCont];
		dblMin=new double[NumCont];
		/***求出个连续字段的最大最小值*******************************************************************/

		for(i=0;i<NumCont;i++){
			CTString strField = m_ContinueFieldList[i]->strFieldName;
			for (int j=0;j<m_pDecTree->m_arSourceData.GetSize();j++)
			{
				pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
				if (strField.Compare(pSource->strFieldName) == SC_EQUAL)
				{
					if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					{
						int nMax,nMin;
						pSource->vectIntData.MaxMin(m_vtCheckData,nMax,nMin);
						dblMax[i] = nMax;
						dblMin[i] = nMin;
					}
					else
					{
						pSource->vectDoubleData.MaxMin(m_vtCheckData,dblMax[i],dblMin[i]);
					}
					break;
				}
			}
		}
		NumTotalCase=0;
			
		pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(m_pDecTree->m_arSourceData.GetSize()-1);
		for (i=0;i<m_pDecTree->m_nTotalCase;i++)
		{
			if (m_vtCheckData(i))
			{
				if (pSource->nType == fString)
				{
					CTString strTemp = pSource->arStringData.GetAt(i);
					for (j=0;j<m_pTargetField->VarValue.size();j++)
					{
						if (strTemp.Compare(m_pTargetField->VarValue.at(j).strVal)==SC_EQUAL)
						{
							NumCasePerClass[j]++;
							break;
						}
					}
				}
				else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					nTemp = pSource->vectIntData(i);
					for (j=0;j<m_pTargetField->VarValue.size();j++)
					{
						if (nTemp == m_pTargetField->VarValue.at(j).nVal)
						{
							NumCasePerClass[j]++;
							break;
						}
					}
				}
				else
				{
					double dTemp = pSource->vectDoubleData(i);
					for (j=0;j<m_pTargetField->VarValue.size();j++)
					{
						if (dTemp == m_pTargetField->VarValue.at(j).dblVal)
						{
							NumCasePerClass[j]++;
							break;
						}
					}
				}
				
				NumTotalCase++;
			}
		}

		for(j=0;j<NumTargetBranch;j++){
			if (m_pbCheck!=NULL)
			{
				if (*m_pbCheck)
				{
					delete []NumCasePerClass;			
					delete []dblMax;
					delete []dblMin;

					return false; 
				}
			}
			m_ClassCounts.push_back(NumCasePerClass[j]);
		}

		/*******判断本节点是否是叶子**************************************************************/
		j=0;
		for(i=0;i<NumTargetBranch;i++){
			if (m_pbCheck!=NULL)
			{
				if (*m_pbCheck)
				{
					delete []NumCasePerClass;			
					delete []dblMax;
					delete []dblMin;

					return false; 
				}
			}
			if(NumCasePerClass[i]==0)
				j++;
		}
		if(j==NumTargetBranch-1){
			m_NumChild=0;			
			m_nNodeType=LEAF;
			delete []NumCasePerClass;			
			delete []dblMax;
			delete []dblMin;
			return true;
		}
		if(NumTotalCase<m_nMaxBranches*m_nMinCaseLeaf){
			m_NumChild=NumTotalCase/m_nMinCaseLeaf;
			if(m_NumChild<2){
				m_NumChild=0;
				m_nNodeType=LEAF;
				delete []NumCasePerClass;				
				delete []dblMax;
				delete []dblMin;
				return false;
			}
		}
		else
			m_NumChild=m_nMaxBranches;
		/***********************************************************************************************/


		/***为连续字段值的直方图分配内存、初始化********************************************************/		
		NumHistgram=m_NumChild*5;		
		ContValFreqTemp =new FIELDVALUEFREQUENCY[NumCont];
		for(i=0;i<NumCont;i++){
			ContValFreqTemp[i].ValHistBorder.push_back(dblMin[i]-0.01);
			for(j=0;j<NumHistgram;j++){					
				ContValFreqTemp[i].ValFreq.push_back(0);
		    	pClassCount=new int[m_pTargetField->NumBranch];
			    //::FillMemory(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
			    memset(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
			    ContValFreqTemp[i].ValClassFreq.push_back(pClassCount);
				if(j==NumHistgram-1)
					ContValFreqTemp[i].ValHistBorder.push_back(dblMax[i]+0.01);
				else{
					dblTemp=(j+1)*(dblMax[i]-dblMin[i])/NumHistgram+dblMin[i];
					ContValFreqTemp[i].ValHistBorder.push_back(dblTemp);
				}
			}
		}
	}


/***为离散字段值的直方图分配内存、初始化************************************************************/
	if(NumDisc>0){
		int  Num=0;
		//int *pClassCount=NULL;
		DiscValFreq =new FIELDVALUEFREQUENCY[NumDisc];
		for(i=0;i<NumDisc;i++){
			Num=m_DiscreteFieldList[i]->NumBranch;
			for(j=0;j<Num;j++){
				DiscValFreq[i].ValFreq.push_back(0);
				pClassCount=new int[m_pTargetField->NumBranch];
				//::FillMemory(pClassCount,NumTargetBranch*sizeof(int),0);
				memset(pClassCount,NumTargetBranch*sizeof(int),0);
				DiscValFreq[i].ValClassFreq.push_back(pClassCount);
			}
		}
	}
/**************************************************************************************************/


/*******遍历数据表，获得直方图数据*****************************************************************/
	int nValTarget;
	double dblValCont;
	NumTotalCase=0;
//	::FillMemory(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	memset(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	int nCur = 0;
	while(nCur < m_pDecTree->m_nTotalCase){
		if (m_vtCheckData(nCur)==1)
		{
			int jj = m_pDecTree->m_arSourceData.GetSize()-1;
			pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(jj);
			if (pSource->nType == fString)
			{
				CTString strTemp = pSource->arStringData.GetAt(nCur);
				for (i=0;i<m_pTargetField->VarValue.size();i++)
				{
					if (strTemp.Compare(m_pTargetField->VarValue.at(i).strVal) == SC_EQUAL)
					{
						NumCasePerClass[i]++;
						break;
					}
				}
			}
			else if ((pSource->nType==fBoolean)||(pSource->nType==fInt))
			{
				nTemp = pSource->vectIntData(nCur);
				for (i=0;i<m_pTargetField->VarValue.size();i++)
				{
					if (m_pTargetField->VarValue.at(i).nVal == nTemp)
					{
						NumCasePerClass[i]++;
						break;
					}
				}
			}
			else
			{
				double dTemp = pSource->vectDoubleData(nCur);
				for (i=0;i<m_pTargetField->VarValue.size();i++)
				{
					if (m_pTargetField->VarValue.at(i).dblVal == dTemp)
					{
						NumCasePerClass[i]++;
						break;
					}
				}
			}
			nValTarget = i;
			if (*m_pbCheck)
			{
				delete []NumCasePerClass;
				if (dblMax)
					delete []dblMax;
				if( dblMin)
					delete []dblMin;
				dblMax=dblMin=NULL;
				for(i=0;i<NumCont;i++){			
					for(j=0;j<NumHistgram;j++){				
						pClassCount=ContValFreqTemp[i].ValClassFreq[j];
						delete []pClassCount;				
					}
					ContValFreqTemp[i].ValFreq.empty();
					ContValFreqTemp[i].ValHistBorder.empty();
					ContValFreqTemp[i].ValClassFreq.empty();
				}
				for(i=0;i<NumDisc;i++){
					Num=m_DiscreteFieldList[i]->NumBranch;
					for(j=0;j<Num;j++){
						pClassCount=DiscValFreq[i].ValClassFreq[j];
						delete []pClassCount;
					}
					DiscValFreq[i].ValFreq.empty();
					DiscValFreq[i].ValClassFreq.empty();
				}
				if(ContValFreqTemp!=NULL)
					delete []ContValFreqTemp;
				if(DiscValFreq!=NULL)
					delete []DiscValFreq;
				
				return false; 
			}
			for(i=0;i<NumDisc;i++){
				FieldClass *pField = m_DiscreteFieldList.at(i);
				for (j=0;j<m_pDecTree->m_arSourceData.GetSize();j++)
				{
					pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
					if (pSource->strFieldName == pField->strFieldName)
						break;
				}
				if (pSource->nType == fString)
				{
					CTString strTemp = pSource->arStringData.GetAt(nCur);
					for (j=0;j<pField->VarValue.size();j++)
						if (strTemp.Compare(pField->VarValue.at(j).strVal)==SC_EQUAL)
						{
							DiscValFreq[i].ValFreq[j]=DiscValFreq[i].ValFreq[j]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[j];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;				
							break;
						}
				}
				else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					nTemp = pSource->vectIntData(nCur);
					for (j=0;j<pField->VarValue.size();j++)
					{
						if (nTemp == pField->VarValue.at(j).nVal)
						{
							DiscValFreq[i].ValFreq[j]=DiscValFreq[i].ValFreq[j]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[j];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;
							break;
						}
					}
				}
				else
				{
					dblValCont = pSource->vectDoubleData(nCur);
					for (j=0;j<pField->VarValue.size();j++)
					{
						if ( fabs(dblValCont - pField->VarValue.at(j).dblVal) < DBL_EPSILON)
						{
							DiscValFreq[i].ValFreq[j]=DiscValFreq[i].ValFreq[j]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[j];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;
							break;
						}
					}
				}
			}
			for(i=0;i<NumCont;i++){
				FieldClass *pField = m_ContinueFieldList.at(i);
				for (j=0;j< m_pDecTree->m_arSourceData.GetSize();j++)
				{
					pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
					if (pSource->strFieldName == pField->strFieldName)
						break;
				}
				if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					dblValCont = pSource->vectIntData(nCur);
				else
					dblValCont = pSource->vectDoubleData(nCur);
				for(j=0;j<NumHistgram;j++){
					if(dblValCont>=ContValFreqTemp[i].ValHistBorder[j] && dblValCont<ContValFreqTemp[i].ValHistBorder[j+1]){
						ContValFreqTemp[i].ValFreq[j]=ContValFreqTemp[i].ValFreq[j]+1;
						pTargetCount=ContValFreqTemp[i].ValClassFreq[j];
						pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;
						break;
					}
				}
			}
			NumTotalCase++;
		}
		nCur++;
	}

	this->m_TotalNum = NumTotalCase;

	if(m_ClassCounts.size()==0){
		for(j=0;j<NumTargetBranch;j++){
			m_ClassCounts.push_back(NumCasePerClass[j]);
		}
	}
		
	j=0;
	for(i=0;i<NumTargetBranch;i++){
		if(NumCasePerClass[i]==0)
			j++;
	}
	if(j==NumTargetBranch-1){//||(NumCont+NumDisc)==1
		m_NumChild=0;			
		m_nNodeType=LEAF;
		delete []NumCasePerClass;			
		delete []dblMax;
		delete []dblMin;		
		for(i=0;i<NumCont;i++){			
			for(j=0;j<NumHistgram;j++){				
				pClassCount=ContValFreqTemp[i].ValClassFreq[j];
				delete []pClassCount;				
			}
			ContValFreqTemp[i].ValFreq.empty();
			ContValFreqTemp[i].ValHistBorder.empty();
			ContValFreqTemp[i].ValClassFreq.empty();
		}
		
		for(i=0;i<NumDisc;i++){
			Num=m_DiscreteFieldList[i]->NumBranch;
			for(j=0;j<Num;j++){
				pClassCount=DiscValFreq[i].ValClassFreq[j];
				delete []pClassCount;
			}
			DiscValFreq[i].ValFreq.empty();
			DiscValFreq[i].ValClassFreq.empty();
		}
		if(ContValFreqTemp!=NULL)
			delete []ContValFreqTemp;
		if(DiscValFreq!=NULL)
			delete []DiscValFreq;
		
		return true;
	}


/***获得连续测试属性的分类区间和目标分类个数****************************************************************************/
	int NumCasesPerRegion=(int)ceil((double)NumTotalCase/m_NumChild);
	int Number=0;
	if(NumCont>0){
	/***重新构造连续的测试属性的分类信息的内存分配**********************************************************************/
		ContValFreq =new FIELDVALUEFREQUENCY[NumCont];
		for(i=0;i<NumCont;i++){	
			ContValFreq[i].ValHistBorder.push_back(0);
			for(j=0;j<m_NumChild;j++){					
				ContValFreq[i].ValFreq.push_back(0);					
				pClassCount=new int[NumTargetBranch];
			    //::FillMemory(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
			    memset(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
			    ContValFreq[i].ValClassFreq.push_back(pClassCount);
				ContValFreq[i].ValHistBorder.push_back(0);
			}
		}
	/*******************************************************************************************************************/

	/***重新构造连续的测试属性的分类信息********************************************************************************/
		int k;
		int nTempFreq;
		for(i=0;i<NumCont;i++){
			Number=0;
			nTempFreq=0;
			ContValFreq[i].ValHistBorder[0]=ContValFreqTemp[i].ValHistBorder[0];
			for(j=0;j<NumHistgram;j++){
				ContValFreq[i].ValFreq[Number]+=ContValFreqTemp[i].ValFreq[j];				
				pClassCount=ContValFreq[i].ValClassFreq[Number];
				pTargetCount=ContValFreqTemp[i].ValClassFreq[j];
				for(k=0;k<NumTargetBranch;k++)
					pClassCount[k]+=pTargetCount[k];
				ContValFreq[i].ValHistBorder[Number+1]=ContValFreqTemp[i].ValHistBorder[j+1];
				if(ContValFreq[i].ValFreq[Number]>=NumCasesPerRegion){					
					Number++;
				if (Number >= m_NumChild)
					Number--;
				}
			}
		}

    /*******************************************************************************************************************/

	/***删除原先连续的的测试属性分类信息的直方图******************************************************************************/
		for(i=0;i<NumCont;i++){
			Num=ContValFreqTemp[i].ValFreq.size();
			for(j=0;j<Num;j++){				
				pClassCount=ContValFreqTemp[i].ValClassFreq[j];
				delete []pClassCount;				
			}
			ContValFreqTemp[i].ValHistBorder.empty();
			ContValFreqTemp[i].ValFreq.empty();
			ContValFreqTemp[i].ValClassFreq.empty();
		}
		delete []ContValFreqTemp;
	}
	/****求熵*****************************************************************************************************/
	double dblTargetEntropy=0;
	double *dblDiscEntropy=new double[NumDisc];
	double *dblContEntropy=new double[NumCont];
	//::FillMemory(dblDiscEntropy,NumDisc*sizeof(double),0);
	memset(dblDiscEntropy,NumDisc*sizeof(double),0);
	//::FillMemory(dblContEntropy,NumCont*sizeof(double),0);
	memset(dblContEntropy,NumCont*sizeof(double),0);


	/***求目标字段的熵****************************************************************************************/
	double dblQuot=0;
	for(i=0;i<NumTargetBranch;i++){
		if(NumCasePerClass[i]>0){
			dblQuot= ( (double)NumCasePerClass[i])/NumTotalCase;
			dblTargetEntropy+=-(dblQuot)*log(dblQuot)/log(2);
		}
	}
	/********************************************************************************************************/

	double dblQt=0,dblPt;
	double dblTest=0;
	/***求离散字段的条件熵****************************************************************************************/
	for(i=0;i<NumDisc;i++){
		for(j=0;j<m_DiscreteFieldList[i]->NumBranch;j++){
			dblTest=(double)DiscValFreq[i].ValFreq[j];
			dblQuot=(dblTest)/NumTotalCase;
			pClassCount=DiscValFreq[i].ValClassFreq[j];
			dblPt=0;
			for(k=0;k<NumTargetBranch;k++){
				if(pClassCount[k]>0){
					dblQt= ( (double)pClassCount[k])/dblTest;
					dblPt+=-(dblQt)*log(dblQt)/log(2);
				}
			}
			dblDiscEntropy[i]+=dblPt*dblQuot;
		}
	}
	/***求连续字段的条件熵****************************************************************************************/
	for(i=0;i<NumCont;i++){
		Num=ContValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			dblTest=(double)ContValFreq[i].ValFreq[j];
			dblQuot=(dblTest)/NumTotalCase;
			pClassCount=ContValFreq[i].ValClassFreq[j];
			dblPt=0;
			for(k=0;k<NumTargetBranch;k++){
				if(pClassCount[k]>0){
					dblQt= ( (double)pClassCount[k])/dblTest;
					dblPt+=-(dblQt)*log(dblQt)/log(2);
				}
			}
			dblContEntropy[i]+=dblPt*dblQuot;
		}
	}
	/*************************************************************************************************************/

	/***求熵的最大增益，确定分类字段******************************************************************************/
	int    nIndexMinEntropy=-1;
	double dblMinEntropy=9.9999e60;
	double dblMaxEntropyGain;
	m_bDiscrete=FALSE;
	VARVAL * pFC = NULL;
	for (i=0;i<m_Importic.size();i++)
		delete m_Importic.at(i);
	m_Importic.clear();
	for(i=0;i<NumDisc;i++){
		if(dblDiscEntropy[i]<dblMinEntropy){
			dblMinEntropy=dblDiscEntropy[i];
			m_SplitField.strFieldName=m_DiscreteFieldList[i]->strFieldName; 
			m_SplitField.cFieldType=m_DiscreteFieldList[i]->cFieldType;		
			nIndexMinEntropy=i;
			m_bDiscrete=TRUE;
			dblMaxEntropyGain=dblTargetEntropy-dblDiscEntropy[i];
		}
		pFC = new VARVAL;
		pFC->strName=m_DiscreteFieldList[i]->strFieldName; 
		pFC->dblValue = dblTargetEntropy-dblDiscEntropy[i];
		m_Importic.push_back(pFC);
	}
	for(i=0;i<NumCont;i++){
		if(dblContEntropy[i]<dblMinEntropy){
			dblMinEntropy=dblContEntropy[i];
			m_SplitField.strFieldName=m_ContinueFieldList[i]->strFieldName; 
			m_SplitField.cFieldType=m_ContinueFieldList[i]->cFieldType; 
			nIndexMinEntropy=i;
			m_bDiscrete=FALSE;
			dblMaxEntropyGain=dblTargetEntropy-dblContEntropy[i];
		}
		pFC = new VARVAL;
		pFC->strName=m_ContinueFieldList[i]->strFieldName; 
		pFC->dblValue = dblTargetEntropy-dblContEntropy[i];
		m_Importic.push_back(pFC);
	}
	delete []dblDiscEntropy;
	delete []dblContEntropy;

/***设置节点信息********************************************************************************************/
	
	m_nIndex = nIndexMinEntropy;
	if(nPruneType==PREPRUNE_ENTROPY&&dblMaxEntropyGain<m_fMinInfoGain){
		m_NumChild=0;
		m_nNodeType=LEAF;
		bBranches=FALSE;
	}
	else{
		int nNumChild=0,nNum;
		if(m_bDiscrete){
			nNum=DiscValFreq[nIndexMinEntropy].ValFreq.size();
			
			for(j=0;j<nNum;j++){
				if(DiscValFreq[nIndexMinEntropy].ValFreq[j]>0){ 
					m_SplitField.VarValue.push_back(m_DiscreteFieldList[nIndexMinEntropy]->VarValue[j]);
					m_SplitField.ChildIndex.push_back(j+1); 
					nNumChild++;
				}
			}
			m_SplitField.NumBranch=nNumChild;
			m_NumChild=nNumChild;
		}
		else{
			nNum=ContValFreq[nIndexMinEntropy].ValClassFreq.size();
		
			DBVariant DBVarTemp;
			for(j=0;j<nNum;j++){
				if(ContValFreq[nIndexMinEntropy].ValFreq[j]>0){ 
				DBVarTemp.dblVal=ContValFreq[nIndexMinEntropy].ValHistBorder[j];
				m_SplitField.VarValue.push_back(DBVarTemp);	
				m_SplitField.ChildIndex.push_back(j+1);
				nNumChild++;
				}
			}
			DBVarTemp.dblVal=ContValFreq[nIndexMinEntropy].ValHistBorder[nNum];
			m_SplitField.VarValue.push_back(DBVarTemp);
			m_SplitField.NumBranch=nNumChild;
			m_NumChild=nNumChild;
		}
		
		m_nNodeType=NODE;
		bBranches=TRUE;
	}
	/************************************************************************************************************/

	/***释放本函数分配的临时内存*********************************************************************************/
	delete []NumCasePerClass;				
	for(i=0;i<NumCont;i++){	
		Num=ContValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			pClassCount=ContValFreq[i].ValClassFreq[j];
			delete []pClassCount;
		}
		ContValFreq[i].ValFreq.empty();
		ContValFreq[i].ValHistBorder.empty();
		ContValFreq[i].ValClassFreq.empty();
	}
	if(NumCont>0)
		delete []ContValFreq;
	for(i=0;i<NumDisc;i++){
		Num=DiscValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			pClassCount=DiscValFreq[i].ValClassFreq[j];
			delete []pClassCount;
		}
		DiscValFreq[i].ValFreq.empty();
		DiscValFreq[i].ValClassFreq.empty();
	}
	if(NumDisc>0)
		delete []DiscValFreq;
	return bBranches;
}

bool CTreeNode::CopyLists(CTreeNode *pSrcNode, CTreeNode *pDesNode)
{
	
	if (pSrcNode==NULL||pDesNode==NULL)
		return FALSE;
	//Copy 
	int nDiscretSize= pSrcNode->m_DiscreteFieldList.size();
	int nCur,nContSize= pSrcNode->m_ContinueFieldList.size();
	// copy discreteField List
	if ( nDiscretSize>0)
	{
		//copy fields
		for (nCur=0;nCur<nDiscretSize;nCur++)
		{
			pDesNode->m_DiscreteFieldList.push_back( pSrcNode->m_DiscreteFieldList[nCur]);
		}
	}
	// copy continueField List
	if (nContSize>0)
	{
		//copy fields
		for (nCur=0;nCur<nContSize;nCur++)
		{
			pDesNode->m_ContinueFieldList.push_back(pSrcNode->m_ContinueFieldList[nCur]);
		}
	}
	
	//copy tartet field
	
	pDesNode->m_pTargetField=pSrcNode->m_pTargetField;
	return TRUE;
}

bool CTreeNode::CopyLists(CTreeNode *parent)
{
	if (parent==NULL)
		return FALSE;
	//Copy 
	int nDiscretSize= parent->m_DiscreteFieldList.size();
	int nCur,nContSize= parent->m_ContinueFieldList.size();
	// copy discreteField List
	if ( nDiscretSize>0)
	{
		//copy fields
		for (nCur=0;nCur<nDiscretSize;nCur++)
		{
			this->m_DiscreteFieldList.push_back( parent->m_DiscreteFieldList[nCur]);
		}
	}
	// copy continueField List
	if (nContSize>0)
	{
		//copy fields
		for (nCur=0;nCur<nContSize;nCur++)
		{
			this->m_ContinueFieldList.push_back(parent->m_ContinueFieldList[nCur]);
		}
	}
	//copy tartet field
	
	this->m_pTargetField=parent->m_pTargetField;
	
	return TRUE;
}

bool CTreeNode::CalFilter()
{
	CTreeNode *parent;
	CTString   strTmpFilter;
	parent=(CTreeNode*) this->Parent;
	if (parent==NULL)
		return FALSE;
	int nChildNo,nChildCount;
	FieldClass  *field;
	field=&parent->m_SplitField;
	nChildNo=CTreeNode::GetChildNum(this);
	nChildCount = parent->m_NumChild;

	CTString str;
	str=field->strFieldName;
	str.Remove(0x20);
	if (str.IsEmpty())
		return FALSE;
	
	if (parent->m_bDiscrete)
	{
		if (field->ChildIndex.size()>=nChildNo)
			strTmpFilter.Format("(%s=%d)", field->strFieldName.GetData(), field->ChildIndex.at(nChildNo-1)-1);
		else
			return FALSE;
		if (parent->m_strFilter.IsEmpty())
			this->m_strFilter=strTmpFilter;
		else{
			this->m_strFilter=parent->m_strFilter+"&";
			this->m_strFilter+=strTmpFilter;
		}
	}
	else 
	{
		if (nChildNo==1)
			strTmpFilter.Format("(%s>=%f)&(%s<%f)", field->strFieldName.GetData(), field->VarValue[0].dblVal,field->strFieldName,field->VarValue[1].dblVal);
		else
		{
			if( nChildNo<nChildCount)
				strTmpFilter.Format("(%s>=%f)&(%s<%f)", field->strFieldName.GetData(),field->VarValue[nChildNo-1].dblVal,field->strFieldName, field->VarValue[nChildNo].dblVal);
			else
				strTmpFilter.Format("(%s>=%f)", field->strFieldName.GetData(),field->VarValue[nChildNo-1].dblVal);//add by liytao 20030227
		}
		if (parent->m_strFilter.IsEmpty())
			this->m_strFilter=strTmpFilter;
		else{
			this->m_strFilter=parent->m_strFilter+"&";
			this->m_strFilter+=strTmpFilter;
		}
	}
	return TRUE;
}

int CTreeNode::GenerateTree(int nGenType,int nPruneType)
{
	if (this->Parent==NULL&&this->m_ClassCounts.size()==1) //is root	
		return 1;
	
	
	// GenerateType:
	//GENTREE_GINI	//树生成GINI系数
	//GENTREE_ENTROPY	//树生成信息增益
	
	//PruneType:
	//PPEPRUNE_GINI				//先剪枝GINI系数
	//PREPRUNE_ENTROPY			//先剪枝信息增益
	//PREPRUNE_DEPTH			//先剪枝深度
	//POSTPRUNE_ERRRATE			//后剪枝错误率
	
	//Mincase Leaf
	int nCaseLeaf;
	nCaseLeaf=this->GetCaseCount();
	this->m_TotalNum = nCaseLeaf;   // add by mzh 2005-5-25
	if (nCaseLeaf<=this->m_nMinCaseLeaf&&nCaseLeaf!=0)
		return 1;
	
	if (nPruneType==PREPRUNE_DEPTH)
	{
		if(this->m_nLevel==this->m_nMaxDepth)
		{
			return 1; 
		}
	}
	
	CTreeNode	*pNewChild;
	int nBranch,nRealBranch=0,nInvalidChild=0;
	bool bGen;
	//nBranch= this->m_SplitField.NumBranch;
	nBranch= this->m_NumChild;
	// 统计结点不同分支的个数
	if (!m_bDiscrete&&nBranch>this->m_nMaxBranches)	
		return 1; 

	for (int nIndex=0;nIndex<nBranch;nIndex++)
	{
		pNewChild=new CTreeNode(m_pDecTree);
//		pNewChild=new CTreeNode(m_pDataAccess);
		pNewChild->NextSibling=NULL;
		pNewChild->FirstChild=NULL;
		pNewChild->Parent=NULL;
		pNewChild->PrevSibling=NULL;
		pNewChild->m_nLevel =this->m_nLevel+1;
		if(m_pbCheck!=NULL)
		{
			if(*m_pbCheck)
			{
				delete pNewChild;
				return -1;
			}

		}
		
		//Step 1: copy lists got by initiating data
		CTreeNode::AddChild(this,pNewChild);
		CTreeNode::CopyLists(this,pNewChild);	
		DeleteField(&(pNewChild->m_ContinueFieldList),this->m_SplitField);
		DeleteField(&(pNewChild->m_DiscreteFieldList),this->m_SplitField);
		pNewChild->SetTargetField(m_pTargetField);
		pNewChild->SetStopCheck(m_pbCheck);
		//pNewChild->SetPrgWnd(m_pPrgWnd,m_prgMsg); -by pyx 2006-02-15
		pNewChild->SetTreeParam(m_nMaxBranches,
			m_nMaxDepth,
			m_nMinCaseLeaf,
			m_fMaxErrorRate,
			m_fMinInfoGain,
			m_fMinGINIReduction);
		
		pNewChild->m_nNodeType=NODE;
		pNewChild->m_vtCheckData.create(m_pDecTree->m_nTotalCase,1);
		pNewChild->FieldCheckData(pNewChild->m_vtCheckData);
 		int nSum = pNewChild->m_vtCheckData.sum();
		if( pNewChild->m_vtCheckData.sum()==0)
		{
			CTreeNode::Prune(pNewChild);
			continue;
		}
		//add end
		if (nGenType==GENTREE_ENTROPY)
			bGen=pNewChild->CalcEntropyGain(nPruneType);
		if (nGenType==GENTREE_GINI)
			bGen=pNewChild->CalcGINIReduction(nPruneType);
		int nSum2 = pNewChild->m_vtCheckData.sum();
		m_vtCheckData.destruction();
		pNewChild->ClearTempAccess();
		if(bGen){
			//Step 2:evaluate all fields and get best splitting field
			// assgin m_SplitField, ClassCounts
			// Compare and decide whether to extract:
			// stop if sample Count is too small to split
			//
			int nOK=pNewChild->GenerateTree(nGenType,nPruneType);
			if(nOK==-1)
				return -1;
			if (nOK==0)
				continue;

			nCaseLeaf=pNewChild->GetCaseCount();
			if (nCaseLeaf<this->m_nMinCaseLeaf)
				nInvalidChild++;
		}
		//add by liytao 20030121
		//add end
		//add by liytao 20030106
		pNewChild->m_TotalNum = pNewChild->GetCaseCount();
		pNewChild->m_nLevel =this->m_nLevel+1;
		nRealBranch++;
		
		// Min Case Leaf
		//add by liytao 20021231
		/*if(nRealBranch>10)
		{
			m_NumChild=nRealBranch;
			m_SplitField.NumBranch=nRealBranch;
			return -1;
		}*/

	}
	
	
	
	
	// invalid split if all children are invalid because of case 
	//if (nInvalidChild==nRealBranch) modified by liytao 20021217
	if (nInvalidChild==nRealBranch||nRealBranch==1)
		DeleteAllChildren(this);
	else
	//add by liytao 20021218
	{
		m_NumChild=nRealBranch;
		m_SplitField.NumBranch=nRealBranch;
		//m_SplitField.VarValue;
	}
	//add end
/*		// -by pyx 2006-02-15
		if (m_pPrgWnd!=NULL)
		{
			int nFC=nPrgPos;//m_ContinueFieldList.size()+m_DiscreteFieldList.size();
			if((15+nFC)<40) 
			m_pPrgWnd->SendMessage(m_prgMsg,15+nFC,0); -by pyx 2006-02-15
			CTreeNode::nPrgPos++;
		}*/
	
	return 1;
	//add end	
	
}

void CTreeNode::DeleteField(std::vector<FieldClass*> * pFieldList, FieldClass FClass)
{
	//add by liytao
	for (int i=0;i<pFieldList->size();i++)
	{
		if (pFieldList->at(i)->strFieldName == FClass.strFieldName)
		{
			pFieldList->erase(pFieldList->begin()+i);
			break;
		}
	}
//	for (i=0;i<pFieldList->size();i++)
//	{
//		TRACE("%s\n",pFieldList->at(i)->strFieldName);
//	}
}

void CTreeNode::PostPrune( int nPruneType)
{
	//PruneType:
	
	//POSTPRUNE_ERRRATE			//后剪枝错误率
	
	// add
	CTreeNode *child,*pNext;
	bool bOK;
	float fErrRate;
	if (this->FirstChild==NULL)
		return;
	child=(CTreeNode*)this->FirstChild;
	while (child!=NULL)
	{
		pNext=(CTreeNode*)child->NextSibling;
		child->PostPrune(nPruneType);
		child=pNext;
	}
	bOK=CTreeNode::AreAllChildrenLeaves(this);
	if (bOK)
	{
		fErrRate=this->GetErrRate();
		if (fErrRate<this->m_fMaxErrorRate)
		{
			// delete children
			CTreeNode::DeleteAllChildren(this);
			this->FirstChild=NULL;
		}
	}
	return;	
	
	//add end	
	
}



float CTreeNode::GetErrRate()
{
	int nMaxClass=0;
	int * iterator_,nTotal=0;
	ClassCounts::iterator iterator;
	for (iterator=this->m_ClassCounts.begin();iterator!=this->m_ClassCounts.end();iterator++)
	{
		if(nMaxClass<*(iterator))
			nMaxClass=*(iterator);
		nTotal=nTotal+*(iterator);
	}
	return (float)((float)(nTotal-nMaxClass)/(float)nTotal);
	
}

bool CTreeNode::EmptyLists()
{
	// add by liytao
	// empty lists
	//CTreeNode *pCur;
	CTString strVal;
	
	this->m_ContinueFieldList.clear();
	this->m_DiscreteFieldList.clear();
	
	// modified by mzh
	//m_SplitField.VarValue.clear();
	// end mzh
	m_ClassCounts.clear();
	
	for (int i=0; i< m_Importic.size();i++)
	{
		delete m_Importic.at(i);
	}
	m_Importic.clear();
	return TRUE;
}


//对当前记录集进行检测
bool CTreeNode::Evaluate(CPtrArray &arFieldSource,int nPos)
{
	int    i;
	CTreeNode *pCur;
	bool bFind = TRUE;
	pCur=this;
	if (pCur == NULL)
		return FALSE;
	int nCldNum = 1;
	for (int k=0;k<arFieldSource.GetSize();k++)
	{
		if ((!bFind)||(pCur==NULL))
			break;
		FieldSourceData *pSource = (FieldSourceData*)arFieldSource.GetAt(k);
		if (pSource->strFieldName == pCur->m_SplitField.strFieldName)
		{
			bFind = FALSE;
			if (pCur->m_bDiscrete)
			{
				if (pSource->nType == fString)
				{
					CTString strVal = pSource->arStringData.GetAt(nPos);
					for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
					{
						if (strVal.Compare(pCur->m_SplitField.VarValue.at(i).strVal)==SC_EQUAL)
						{
							if (i < pCur->m_SplitField.ChildIndex.size())
							{
								nCldNum = pCur->m_SplitField.ChildIndex.at(i);
								pCur = (CTreeNode*)Tree::GetChild(pCur,nCldNum);
								bFind = TRUE;
							}
							break;
						}
					}
				}
				else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					int nVal = pSource->vectIntData(nPos);
					for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
					{
						if (nVal == pCur->m_SplitField.VarValue.at(i).nVal)
						{
							if (i < pCur->m_SplitField.ChildIndex.size())
							{
								nCldNum = pCur->m_SplitField.ChildIndex.at(i);
								pCur = (CTreeNode*)Tree::GetChild(pCur,nCldNum);
								bFind = TRUE;
							}
							break;
						}
					}
				}
				else
				{
					double dVal = pSource->vectDoubleData(nPos);
					for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
					{
						if (pCur->m_SplitField.VarValue.at(i).dblVal == dVal)
						{
							if (i < pCur->m_SplitField.ChildIndex.size())
							{
								nCldNum = pCur->m_SplitField.ChildIndex.at(i);
								pCur = (CTreeNode*)Tree::GetChild(pCur,nCldNum);
								bFind = TRUE;
							}
							break;
						}
					}
				}
			}
			else
			{
				double dVal = 0;
				if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					dVal = pSource->vectIntData(nPos);
				else
					dVal = pSource->vectDoubleData(nPos);
				int nSS = pCur->m_SplitField.VarValue.size();
				if (nSS > 0)
				for (i=0;i<pCur->m_SplitField.VarValue.size()-1;i++)
				{
					if ((dVal >= pCur->m_SplitField.VarValue.at(i).dblVal)&&((dVal < pCur->m_SplitField.VarValue.at(i+1).dblVal)))
					{
						if (i < pCur->m_SplitField.ChildIndex.size())
						{
							nCldNum = pCur->m_SplitField.ChildIndex.at(i);
							pCur = (CTreeNode*)Tree::GetChild(pCur,nCldNum);
							bFind = TRUE;
						}
						break;
					}
				}
			}
		}
	}

	return bFind;
}

int CTreeNode::GetMaxClass(double &dbAvail)
{
	int *iterator_,i,nIndex=0;
	int nMaxClass=0;
	int nTotal=0;
	i=0;
	ClassCounts::iterator iterator;
	for (iterator=m_ClassCounts.begin();iterator!=m_ClassCounts.end();iterator++)
	{
		nTotal += *(iterator);
		if(nMaxClass<*(iterator))
		{
			nMaxClass=*(iterator);
			nIndex=i;
		}
		i++;
	}
	if (nTotal>0)
		dbAvail = 1.0*nMaxClass/nTotal;
	else
		dbAvail = 0;
	return nIndex;
}


bool CTreeNode::MergeSameChildren()
{
	CTreeNode *child,*pNext;
	bool bOK;
//	float fErrRate;
	if (this->FirstChild==NULL)
		return TRUE;
	child=(CTreeNode*)this->FirstChild;
	while (child!=NULL)
	{
		pNext=(CTreeNode*)child->NextSibling;
		child->MergeSameChildren();
		child=pNext;
	}
	bOK=CTreeNode::AreAllChildrenLeaves(this);
	if (bOK)
	{
		bOK=this->HaveAllChildrenSameClass();
		if (bOK)
		{
			// delete children
			CTreeNode::DeleteAllChildren(this);
			this->FirstChild=NULL;
		}
	}
	return TRUE;	
}

bool CTreeNode::HaveAllChildrenSameClass()
{
	if (this->FirstChild==NULL||this==NULL)
		return FALSE;
	bool bOK=TRUE;
	CTreeNode *child;
	child=(CTreeNode*)this->FirstChild;
	int nMaxClass=0,nPrev=0;
	double dbAvail;
	nPrev=child->GetMaxClass(dbAvail);
	while (child->NextSibling!=NULL)
	{
		
		child =(CTreeNode*)child->NextSibling;
		nMaxClass=child->GetMaxClass(dbAvail);
		
		if (nPrev!=nMaxClass)
		{
			bOK=FALSE;
			break;
		}
		nPrev=nMaxClass;
	}
	return bOK;
	
}

bool CTreeNode::CalcGINIReduction(int nPruneType)
{
	int Num;
	int nTemp=0;
	bool bBranches=TRUE;
	int  i,j,k,NumTotalCase;
	int  NumCont,NumDisc;
	int  NumHistgram=0;
	int *pClassCount=NULL;
	int *pTargetCount=NULL;
	int  NumTargetBranch;
	int *NumCasePerClass;
	double *dblMax=NULL,*dblMin=NULL;
	double *dblHistBorder=NULL;
	FIELDVALUEFREQUENCY *ContValFreqTemp=NULL;
	FIELDVALUEFREQUENCY *ContValFreq=NULL;
	FIELDVALUEFREQUENCY *DiscValFreq=NULL;
	FieldSourceData	*pSource = NULL;

	NumTargetBranch=m_pTargetField->NumBranch;	
	NumCasePerClass=new int[NumTargetBranch];
//	::FillMemory(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	memset(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	NumCont=m_ContinueFieldList.size();
	NumDisc=m_DiscreteFieldList.size();

	int nCur = 0;
	
	if (NumCont==0&&NumDisc==0)
	{
			NumTotalCase = 0;
			nCur = 0;
			while(nCur < m_pDecTree->m_nTotalCase){
				if (m_pbCheck!=NULL)
				{
					if (*m_pbCheck)
					{
						delete []NumCasePerClass;			

						return false; 
					}
				}
				if (m_vtCheckData(nCur)==1)
				{
					j = m_pDecTree->m_arSourceData.GetSize()-1;
					pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
					if (pSource->nType == fString)
					{
						CTString strTemp = pSource->arStringData.GetAt(nCur);
						for (i=0;i<m_pTargetField->VarValue.size();i++)
						{
							if (strTemp.Compare(m_pTargetField->VarValue.at(i).strVal)==SC_EQUAL)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
					}
					else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					{
						nTemp = pSource->vectIntData(nCur);
						for (i=0;i<m_pTargetField->VarValue.size();i++)
						{
							if (nTemp == m_pTargetField->VarValue.at(i).nVal)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
					}
					else
					{
						double dTemp = pSource->vectDoubleData(nCur);
						for (i=0;i<m_pTargetField->VarValue.size();i++)
						{
							if (dTemp == m_pTargetField->VarValue.at(i).dblVal)
							{
								NumCasePerClass[i]++;
								break;
							}
						}
					}
					NumTotalCase++;
				}
				nCur ++;//add by xds 20060618
			}
			for(j=0;j<NumTargetBranch;j++){
				if (m_pbCheck!=NULL)
				{
					if (*m_pbCheck)
					{
						delete []NumCasePerClass;			
						return false; 
					}
				}
				m_ClassCounts.push_back(NumCasePerClass[j]);
			}
		return FALSE;
	}
	//add end 
	if(NumCont>0){
		double dblTemp=0;
		dblMax=new double[NumCont];
		dblMin=new double[NumCont];
		/***求出个连续字段的最大最小值*******************************************************************/
		for(i=0;i<NumCont;i++){
			FieldClass *pField = m_ContinueFieldList.at(i);
			for (j=0;j<m_pDecTree->m_arSourceData.GetSize();j++)
			{
				pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
				if (pField->strFieldName == pSource->strFieldName)
				{
					if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					{
						int nMax,nMin;
						pSource->vectIntData.MaxMin(m_vtCheckData,nMax,nMin);
						dblMax[i] = nMax;
						dblMin[i] = nMin;
					}
					else
					{
						pSource->vectDoubleData.MaxMin(m_vtCheckData,dblMax[i],dblMin[i]);
					}
					break;
				}
			}
		}
		NumTotalCase=0;
		
		int nTemp=0;
		nCur = 0;
		while(nCur < m_pDecTree->m_nTotalCase){
			if (m_pbCheck!=NULL)
			{
				if (*m_pbCheck)
				{
					delete []NumCasePerClass;			
					delete []dblMax;
					delete []dblMin;

					return false; 
				}
			}
			if (m_vtCheckData(nCur)==1)
			{
				j = m_pDecTree->m_arSourceData.GetSize()-1;
				pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
				if (pSource->nType == fString)
				{
					CTString strTemp = pSource->arStringData.GetAt(nCur);
					for (i=0;i<m_pTargetField->VarValue.size();i++)
					{
						if (strTemp.Compare(m_pTargetField->VarValue.at(i).strVal)==SC_EQUAL)
						{
							NumCasePerClass[i]++;
							break;
						}
					}
				}
				else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					nTemp = pSource->vectIntData(nCur);
					for (i=0;i<m_pTargetField->VarValue.size();i++)
					{
						if (nTemp == m_pTargetField->VarValue.at(i).nVal)
						{
							NumCasePerClass[i]++;
							break;
						}
					}
				}
				else
				{
					double dTemp = pSource->vectDoubleData(nCur);
					for (i=0;i<m_pTargetField->VarValue.size();i++)
					{
						if (dTemp == m_pTargetField->VarValue.at(i).dblVal)
						{
							NumCasePerClass[i]++;
							break;
						}
					}
				}
				NumTotalCase++;
			}
			nCur++;
		}

		//this->m_TotalNum = NumTotalCase;  // add by mzh 
		for(j=0;j<NumTargetBranch;j++){
			if (m_pbCheck!=NULL)
			{
				if (*m_pbCheck)
				{
					delete []NumCasePerClass;			
					delete []dblMax;
					delete []dblMin;

					return false; 
				}
			}
			m_ClassCounts.push_back(NumCasePerClass[j]);
		}
		
		/*******判断本节点是否是叶子**************************************************************/
		j=0;
		for(i=0;i<NumTargetBranch;i++){
			if (m_pbCheck!=NULL)
			{
				if (*m_pbCheck)
				{
					delete []NumCasePerClass;			
					delete []dblMax;
					delete []dblMin;

					return false; 
				}
			}
			if(NumCasePerClass[i]==0)
				j++;
		}
		if(j==NumTargetBranch-1){//||(NumCont+NumDisc)==1
			m_NumChild=0;			
			m_nNodeType=LEAF;
			//delete pDataAccess;
			delete []NumCasePerClass;			
			delete []dblMax;
			delete []dblMin;
			return true;
		}
		if(NumTotalCase<m_nMaxBranches*m_nMinCaseLeaf){
			m_NumChild=NumTotalCase/m_nMinCaseLeaf;
			if(m_NumChild<2){
				m_NumChild=0;
				m_nNodeType=LEAF;
				//delete pDataAccess;
				delete []NumCasePerClass;				
				delete []dblMax;
				delete []dblMin;
				return false;
			}
		}
		else
			m_NumChild=m_nMaxBranches;
		/***********************************************************************************************/
		
		
		/***为连续字段值的直方图分配内存、初始化********************************************************/		
		NumHistgram=m_NumChild*5;		
		ContValFreqTemp =new FIELDVALUEFREQUENCY[NumCont];
		for(i=0;i<NumCont;i++){
			ContValFreqTemp[i].ValHistBorder.push_back(dblMin[i]-0.01);
			for(j=0;j<NumHistgram;j++){					
				ContValFreqTemp[i].ValFreq.push_back(0);
				pClassCount=new int[m_pTargetField->NumBranch];
				//::FillMemory(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
				memset(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
				ContValFreqTemp[i].ValClassFreq.push_back(pClassCount);
				if(j==NumHistgram-1)
					ContValFreqTemp[i].ValHistBorder.push_back(dblMax[i]+0.01);
				else{
					dblTemp=(j+1)*(dblMax[i]-dblMin[i])/NumHistgram+dblMin[i];
					ContValFreqTemp[i].ValHistBorder.push_back(dblTemp);
				}
			}
		}
		/***********************************************************************************************/	
		delete []dblMax;
		delete []dblMin;
	}
	
	
	/***为离散字段值的直方图分配内存、初始化************************************************************/
	if(NumDisc>0){
		int  Num=0;
		//int *pClassCount=NULL;
		DiscValFreq =new FIELDVALUEFREQUENCY[NumDisc];
		for(i=0;i<NumDisc;i++){
			Num=m_DiscreteFieldList[i]->NumBranch;
			for(j=0;j<Num;j++){
				DiscValFreq[i].ValFreq.push_back(0);
				pClassCount=new int[m_pTargetField->NumBranch];
				//::FillMemory(pClassCount,NumTargetBranch*sizeof(int),0);
				memset(pClassCount,NumTargetBranch*sizeof(int),0);
				DiscValFreq[i].ValClassFreq.push_back(pClassCount);
			}
		}
	}
	/**************************************************************************************************/
	
	
	/*******遍历数据表，获得直方图数据*****************************************************************/
	int nValTarget,nValDisc;
	double dblValCont;
	
	//add by liytao 20021217
	NumTotalCase=0;
	//add end
	//::FillMemory(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	memset(NumCasePerClass,NumTargetBranch*sizeof(int),0);
	nCur = 0;
	while(nCur < m_pDecTree->m_nTotalCase){
		if (m_vtCheckData(nCur)==1)
		{
			j = m_pDecTree->m_arSourceData.GetSize()-1;
			pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
			for (i=0;i<m_pTargetField->VarValue.size();i++)
			{
				if (pSource->nType == fString)
				{
					CTString strTemp = pSource->arStringData.GetAt(nCur);
					if (strTemp.Compare(m_pTargetField->VarValue.at(i).strVal)==SC_EQUAL)
					{
						nValTarget = i;
						NumCasePerClass[nValTarget]++;
					}
				}
				else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					int nTemp = pSource->vectIntData(nCur);
					if (nTemp == m_pTargetField->VarValue.at(i).nVal)
					{
						nValTarget = i;
						NumCasePerClass[nValTarget]++;
					}
				}
				else
				{
					double dTemp = pSource->vectIntData(nCur);
					if (dTemp == m_pTargetField->VarValue.at(i).dblVal)
					{
						nValTarget = i;
						NumCasePerClass[nValTarget]++;
					}
				}
			}
			if (*m_pbCheck)
			{
				delete []NumCasePerClass;
				if (dblMax)
					delete []dblMax;
				if( dblMin)
					delete []dblMin;
				dblMax=dblMin=NULL;
				for(i=0;i<NumCont;i++){			
					for(j=0;j<NumHistgram;j++){				
						pClassCount=ContValFreqTemp[i].ValClassFreq[j];
						delete []pClassCount;				
					}
					ContValFreqTemp[i].ValFreq.empty();
					ContValFreqTemp[i].ValHistBorder.empty();
					ContValFreqTemp[i].ValClassFreq.empty();
				}
				for(i=0;i<NumDisc;i++){
					Num=m_DiscreteFieldList[i]->NumBranch;
					for(j=0;j<Num;j++){
						pClassCount=DiscValFreq[i].ValClassFreq[j];
						delete []pClassCount;
					}
					DiscValFreq[i].ValFreq.empty();
					DiscValFreq[i].ValClassFreq.empty();
				}
				if(ContValFreqTemp!=NULL)
					delete []ContValFreqTemp;
				if(DiscValFreq!=NULL)
					delete []DiscValFreq;
				
				return false; 
			}
			for(i=0;i<NumDisc;i++){
				FieldClass *pField = m_DiscreteFieldList.at(i);
				for (j=0;j<m_pDecTree->m_arSourceData.GetSize();j++)
				{
					pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
					if (pField->strFieldName == pSource->strFieldName)
						break;
				}
				for (j=0;j<pField->VarValue.size();j++)
				{
					if (pSource->nType == fString)
					{
						CTString strTemp = pSource->arStringData.GetAt(nCur);
						if (strTemp.Compare(pField->VarValue.at(j).strVal)==SC_EQUAL)
						{
							nValDisc= j;
							DiscValFreq[i].ValFreq[nValDisc]=DiscValFreq[i].ValFreq[nValDisc]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[nValDisc];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;				
						}
					}
					else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					{
						int nTemp = pSource->vectIntData(nCur);
						if (nTemp == pField->VarValue.at(j).nVal)
						{
							nValDisc= j;
							DiscValFreq[i].ValFreq[nValDisc]=DiscValFreq[i].ValFreq[nValDisc]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[nValDisc];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;				
						}
					}
					else
					{
						double dTemp =  pSource->vectDoubleData(nCur);
						if (fabs(dTemp-pField->VarValue.at(j).dblVal) > DBL_EPSILON)
						{
							nValDisc= j;
							DiscValFreq[i].ValFreq[nValDisc]=DiscValFreq[i].ValFreq[nValDisc]+1;
							pTargetCount=DiscValFreq[i].ValClassFreq[nValDisc];
							pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;				
						}
					}
				}
			}
			for(i=0;i<NumCont;i++){
				FieldClass *pField = m_ContinueFieldList.at(i);
				for (j=0;j<m_pDecTree->m_arSourceData.GetSize();j++)
				{
					pSource = (FieldSourceData*)m_pDecTree->m_arSourceData.GetAt(j);
					if (pField->strFieldName == pSource->strFieldName)
						break;
				}
				if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
					dblValCont = pSource->vectIntData(nCur);
				else
					dblValCont= pSource->vectDoubleData(nCur);
				for(j=0;j<NumHistgram;j++){
					if (*m_pbCheck)
					{
						delete []NumCasePerClass;			
						for(i=0;i<NumCont;i++){			
							for(j=0;j<NumHistgram;j++){				
								pClassCount=ContValFreqTemp[i].ValClassFreq[j];
								delete []pClassCount;				
							}
							ContValFreqTemp[i].ValFreq.empty();
							ContValFreqTemp[i].ValHistBorder.empty();
							ContValFreqTemp[i].ValClassFreq.empty();
						}
						for(i=0;i<NumDisc;i++){
							Num=m_DiscreteFieldList[i]->NumBranch;
							for(j=0;j<Num;j++){
								pClassCount=DiscValFreq[i].ValClassFreq[j];
								delete []pClassCount;
							}
							DiscValFreq[i].ValFreq.empty();
							DiscValFreq[i].ValClassFreq.empty();
						}
						if(ContValFreqTemp!=NULL)
							delete []ContValFreqTemp;
						if(DiscValFreq!=NULL)
							delete []DiscValFreq;
						
						return false; 
					}
					if(dblValCont>=ContValFreqTemp[i].ValHistBorder[j] && dblValCont<ContValFreqTemp[i].ValHistBorder[j+1]){
						ContValFreqTemp[i].ValFreq[j]=ContValFreqTemp[i].ValFreq[j]+1;
						pTargetCount=ContValFreqTemp[i].ValClassFreq[j];
						pTargetCount[nValTarget]=pTargetCount[nValTarget]+1;
						break;
					}
				}
			}
			NumTotalCase++;
		}
		nCur++;
	}

	if(m_ClassCounts.size()==0){
		for(j=0;j<NumTargetBranch;j++){
			m_ClassCounts.push_back(NumCasePerClass[j]);
		}
	}
	
	j=0;
	for(i=0;i<NumTargetBranch;i++){
		if(NumCasePerClass[i]==0)
			j++;
	}
	if(j==NumTargetBranch-1){//||(NumCont+NumDisc)==1
		m_NumChild=0;			
		m_nNodeType=LEAF;
		delete []NumCasePerClass;			
		delete []dblMax;
		delete []dblMin;		
		for(i=0;i<NumCont;i++){			
			for(j=0;j<NumHistgram;j++){				
				pClassCount=ContValFreqTemp[i].ValClassFreq[j];
				delete []pClassCount;				
			}
			ContValFreqTemp[i].ValFreq.empty();
			ContValFreqTemp[i].ValHistBorder.empty();
			ContValFreqTemp[i].ValClassFreq.empty();
		}
		
		for(i=0;i<NumDisc;i++){
			Num=m_DiscreteFieldList[i]->NumBranch;
			for(j=0;j<Num;j++){
				pClassCount=DiscValFreq[i].ValClassFreq[j];
				delete []pClassCount;
			}
			DiscValFreq[i].ValFreq.empty();
			DiscValFreq[i].ValClassFreq.empty();
		}
		if(ContValFreqTemp!=NULL)
			delete []ContValFreqTemp;
		if(DiscValFreq!=NULL)
			delete []DiscValFreq;
		
		return true;
	}
	
	
	/***获得连续测试属性的分类区间和目标分类个数****************************************************************************/
	int NumCasesPerRegion=(int)ceil((double)NumTotalCase/m_NumChild);
	int Number=0;
	if(NumCont>0){
		/***重新构造连续的测试属性的分类信息的内存分配**********************************************************************/
		ContValFreq =new FIELDVALUEFREQUENCY[NumCont];
		for(i=0;i<NumCont;i++){	
			ContValFreq[i].ValHistBorder.push_back(0);
			for(j=0;j<m_NumChild;j++){					
				ContValFreq[i].ValFreq.push_back(0);					
				pClassCount=new int[NumTargetBranch];
				//::FillMemory(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
				memset(pClassCount,m_pTargetField->NumBranch*sizeof(int),0);
				ContValFreq[i].ValClassFreq.push_back(pClassCount);
				ContValFreq[i].ValHistBorder.push_back(0);
			}
		}
		/*******************************************************************************************************************/
		
		/***重新构造连续的测试属性的分类信息********************************************************************************/
		int k;//,nSize;
		int nTempFreq;
		for(i=0;i<NumCont;i++){
			Number=0;
			nTempFreq=0;
			ContValFreq[i].ValHistBorder[0]=ContValFreqTemp[i].ValHistBorder[0];
			
			for(j=0;j<NumHistgram;j++){
				ContValFreq[i].ValFreq[Number]+=ContValFreqTemp[i].ValFreq[j];				
				pClassCount=ContValFreq[i].ValClassFreq[Number];
				pTargetCount=ContValFreqTemp[i].ValClassFreq[j];
				for(k=0;k<NumTargetBranch;k++)
					pClassCount[k]+=pTargetCount[k];
				ContValFreq[i].ValHistBorder[Number+1]=ContValFreqTemp[i].ValHistBorder[j+1];
				if(ContValFreq[i].ValFreq[Number]>=NumCasesPerRegion){
					Number++;
				if (Number >= m_NumChild)
					Number--;
				}				
			}			
		}
		
		/*******************************************************************************************************************/
		
		/***删除原先连续的的测试属性分类信息的直方图******************************************************************************/
		for(i=0;i<NumCont;i++){
			Num=ContValFreqTemp[i].ValFreq.size();
			for(j=0;j<Num;j++){				
				pClassCount=ContValFreqTemp[i].ValClassFreq[j];
				delete []pClassCount;				
			}
			ContValFreqTemp[i].ValHistBorder.empty();
			ContValFreqTemp[i].ValFreq.empty();
			ContValFreqTemp[i].ValClassFreq.empty();
		}
		delete []ContValFreqTemp;
	}
	/****求GINI系数*****************************************************************************************************/
	double dblTargetGINI=0;
	double *dblDiscGINI=new double[NumDisc];
	double *dblContGINI=new double[NumCont];
	//::FillMemory(dblDiscGINI,NumDisc*sizeof(double),0);
	//::FillMemory(dblContGINI,NumCont*sizeof(double),0);
	memset(dblDiscGINI,NumDisc*sizeof(double),0);
	memset(dblContGINI,NumCont*sizeof(double),0);
	/***求目标字段的GINI****************************************************************************************/
	double dblQuot=0;
	
	for(i=0;i<NumTargetBranch;i++){
		if(NumCasePerClass[i]>0){
			dblQuot= ( (double)NumCasePerClass[i])/NumTotalCase;
			dblTargetGINI+=-(dblQuot*dblQuot );
		}
	}
	dblTargetGINI+=1;
	
	/********************************************************************************************************/
	
	double dblQt=0,dblPt;
	double dblTest=0;
	/***求离散字段的GINI****************************************************************************************/
	for(i=0;i<NumDisc;i++){
		for(j=0;j<m_DiscreteFieldList[i]->NumBranch;j++){
			dblTest=(double)DiscValFreq[i].ValFreq[j];
			dblQuot=(dblTest)/NumTotalCase;
			pClassCount=DiscValFreq[i].ValClassFreq[j];
			dblPt=0;
			for(k=0;k<NumTargetBranch;k++){
				if(pClassCount[k]>0){
					dblQt= ( (double)pClassCount[k])/dblTest;
					dblPt+=(dblQt*dblQt);
				}
			}
			dblDiscGINI[i]+=dblQuot*(1-dblPt);
		}
		
	}
	/***求连续字段的GINI****************************************************************************************/
	for(i=0;i<NumCont;i++){
		Num=ContValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			dblTest=(double)ContValFreq[i].ValFreq[j];
			dblQuot=(dblTest)/NumTotalCase;
			pClassCount=ContValFreq[i].ValClassFreq[j];
			dblPt=0;
			for(k=0;k<NumTargetBranch;k++){
				if(pClassCount[k]>0){
					dblQt= ((double)pClassCount[k])/dblTest;
					dblPt+=(dblQt*dblQt);
				}
			}
			dblContGINI[i]+=dblQuot*(1-dblPt);
		}
	}
	/*************************************************************************************************************/
	
	/***求GINI的最大差，确定分类字段******************************************************************************/
	int    nIndexMinGINI=-1;
	double dblMinGINI=9.9999e60;
	double dblMaxGINIGain=0;
	m_bDiscrete=FALSE;
	for (i=0;i<m_Importic.size();i++)
		delete m_Importic.at(i);
	m_Importic.clear();
	VARVAL * pFC = NULL;
	for(i=0;i<NumDisc;i++){
		if(dblDiscGINI[i]<dblMinGINI){
			dblMinGINI=dblDiscGINI[i];
			m_SplitField.strFieldName=m_DiscreteFieldList[i]->strFieldName; 
			m_SplitField.cFieldType=m_DiscreteFieldList[i]->cFieldType;		
			nIndexMinGINI=i;
			dblMaxGINIGain=dblTargetGINI-dblDiscGINI[i];
			
			m_bDiscrete=TRUE;
		}
		pFC = new VARVAL;
		pFC->dblValue = dblTargetGINI-dblDiscGINI[i];
		pFC->strName = m_DiscreteFieldList[i]->strFieldName;
		m_Importic.push_back(pFC);
	}
	for(i=0;i<NumCont;i++){
		if(dblContGINI[i]<dblMinGINI){
			dblMinGINI=dblContGINI[i];
			m_SplitField.strFieldName=m_ContinueFieldList[i]->strFieldName; 
			m_SplitField.cFieldType=m_ContinueFieldList[i]->cFieldType; 
			nIndexMinGINI=i;
			dblMaxGINIGain=dblTargetGINI-dblContGINI[i];
			
			m_bDiscrete=FALSE;
		}
		pFC = new VARVAL; 
		pFC->dblValue = dblTargetGINI-dblContGINI[i];
		pFC->strName = m_ContinueFieldList[i]->strFieldName;
		m_Importic.push_back(pFC);
	}
	delete []dblDiscGINI;
	delete []dblContGINI;
	
	m_nIndex = nIndexMinGINI;
	/***设置节点信息********************************************************************************************/
	if(nPruneType==PREPRUNE_GINI&&dblMaxGINIGain<this->m_fMinGINIReduction){
		m_NumChild=0;
		m_nNodeType=LEAF;
		bBranches=FALSE;
	}
	else{
		int nNumChild=0,nNum;
		if(m_bDiscrete){
			nNum=DiscValFreq[nIndexMinGINI].ValFreq.size();
			for(j=0;j<nNum;j++){
				if(DiscValFreq[nIndexMinGINI].ValFreq[j]>0){//modified by liytao 20030121
					m_SplitField.VarValue.push_back(m_DiscreteFieldList[nIndexMinGINI]->VarValue[j]);
					m_SplitField.ChildIndex.push_back(j+1); //modified by liytao 20030121

					nNumChild++;
				}
			}
			m_SplitField.NumBranch=nNumChild;
			m_NumChild=nNumChild;
		}
		else{
			m_NumChild=ContValFreq[nIndexMinGINI].ValClassFreq.size();
			m_SplitField.NumBranch=m_NumChild;
			DBVariant DBVarTemp;
			for(j=0;j<m_NumChild;j++){
				DBVarTemp.dblVal=ContValFreq[nIndexMinGINI].ValHistBorder[j];
				m_SplitField.VarValue.push_back(DBVarTemp);			
				m_SplitField.ChildIndex.push_back(j+1); 
			}
			DBVarTemp.dblVal=ContValFreq[nIndexMinGINI].ValHistBorder[m_NumChild];
			m_SplitField.VarValue.push_back(DBVarTemp);
		}
		
		m_nNodeType=NODE;
		bBranches=TRUE;
	}
	/************************************************************************************************************/
	
	/***释放本函数分配的临时内存*********************************************************************************/
	delete []NumCasePerClass;				
	for(i=0;i<NumCont;i++){	
		Num=ContValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			pClassCount=ContValFreq[i].ValClassFreq[j];
			delete []pClassCount;
		}
		ContValFreq[i].ValFreq.empty();
		ContValFreq[i].ValHistBorder.empty();
		ContValFreq[i].ValClassFreq.empty();
	}
	if(NumCont>0)
		delete []ContValFreq;
	for(i=0;i<NumDisc;i++){
		Num=DiscValFreq[i].ValFreq.size();
		for(j=0;j<Num;j++){
			pClassCount=DiscValFreq[i].ValClassFreq[j];
			delete []pClassCount;
		}
		DiscValFreq[i].ValFreq.empty();
		DiscValFreq[i].ValClassFreq.empty();
	}
	if(NumDisc>0)
		delete []DiscValFreq;
	
	return bBranches;
}

int CTreeNode::GetCaseCount()
{
	int nCount=0;
    int *iterator_;
	ClassCounts::iterator iterator;
    for (iterator=m_ClassCounts.begin();iterator!=m_ClassCounts.end();iterator++)
		nCount+=*(iterator);  
	return nCount;
}

CTString CTreeNode::GetArcLabel(int nChildNum,bool bLabel)
{
	if (this->FirstChild==NULL)
		return _T("");
	int nChildCount;
	double dbFirst,dbVal;
	CTString strArc,strVal;
	FieldClass  *field;
	field=&this->m_SplitField;
	nChildCount=CTreeNode::GetChildrenCount(this);
	//CTString temp = "aa";
	
	CTString str;

	if (m_bDiscrete)
	{
		switch(field->cFieldType){
		case fBoolean:
			//strVal=field->VarValue[nChildNum-1].strVal;	
			if (field->VarValue[nChildNum-1].bVal)
				strVal="是";
			else
				strVal="否";
			break;
		case fInt:
			nVal=field->VarValue[nChildNum-1].nVal;
			strVal.Format("%d",nVal);
			break;
		// add by liytao 20021213
		case fDouble:
		case fCurrency:
		case fDate:

			dbVal=field->VarValue[nChildNum-1].dblVal;
			strVal.Format("%.2f",dbVal);
			//strVal=field->VarValue[nChildNum-1].strVal;	
			break;
			//add end
		case fString:
			strVal  = field->VarValue[nChildNum-1].strVal;
		//str=_T("CTString");
		//AfxMessageBox(str);
			break;	
		default:
			break;
		}
		
		CTString labelVal;
		strArc.Format("%s",strVal.GetData());
		int i;
		if ((bLabel)&&(m_pTargetField->cFieldType == fInt))
		{
			for ( i=0;i<m_pDecTree->m_DiscreteFieldList.size();i++)
			{
				CTString strField = m_pDecTree->m_DiscreteFieldList.at(i)->strFieldName;
				if (strField.Compare(field->strFieldName)== SC_EQUAL)
				{
					FieldClass *pFieldClass = m_pDecTree->m_DiscreteFieldList.at(i);
					for (int j=0;j<pFieldClass->FieldLabel.size();j++)
					{
						if (pFieldClass->FieldLabel.at(j)->varValue.nVal == nVal)
							return pFieldClass->FieldLabel.at(j)->strLabelName;
					}
				}
			}
			for (i=0;i<m_pDecTree->m_ContinueFieldList.size();i++)
			{
				CTString strField = m_pDecTree->m_ContinueFieldList.at(i)->strFieldName;
				if (strField.Compare(field->strFieldName)== SC_EQUAL)
				{
					FieldClass *pFieldClass = m_pDecTree->m_ContinueFieldList.at(i);
					for (int j=0;j<pFieldClass->FieldLabel.size();j++)
					{
						//if (pFieldClass->FieldLabel.at(j)->varValue.strVal == strArc)
						if (pFieldClass->FieldLabel.at(j)->varValue.nVal == nVal)
							return pFieldClass->FieldLabel.at(j)->strLabelName;
					}
				}
			}
		}
		return strArc;
	}
	else
	{
		dbFirst=field->VarValue[0].dblVal;
		if ( nChildNum  >= field->VarValue.size()-1 )
		{
			strArc.Format(">=%.2f", field->VarValue[nChildNum-1].dblVal);
		}
		else
		{
			double dFir = field->VarValue[nChildNum-1].dblVal;
			double dLas = field->VarValue[nChildNum].dblVal;
			strArc.Format("[%.2f ,%.2f)", field->VarValue[nChildNum-1].dblVal,field->VarValue[nChildNum].dblVal);
		}
		return strArc;
	}
}

void  CTreeNode::DeleteMissingRecord()
{
	int NumTotalCase=0;
	CFieldList *pFieldList=m_pDataAccess->m_pFieldList;

	m_pDataAccess->First();
	while(!(m_pDataAccess->Eof()))
	{
		if (pFieldList->FieldByName(m_pTargetField->strFieldName)->IsNull())
		{
			m_pDataAccess->Delete();
			continue;
		}
		m_pDataAccess->Next();
	}

	if (m_DiscreteFieldList.size() >0)
	{
		m_pDataAccess->First();
		while(!(m_pDataAccess->Eof()))
		{
			bool bDelete = FALSE;
			for (int i=0;i<m_DiscreteFieldList.size();i++)
			{
				if (pFieldList->FieldByName(m_DiscreteFieldList.at(i)->strFieldName)->IsNull())
				{
					m_pDataAccess->Delete();
					bDelete = TRUE;
					break;
				}
			}
			if (bDelete)
				continue;
			m_pDataAccess->Next();
		}
	}

	if (m_ContinueFieldList.size()>0)
	{
		m_pDataAccess->First();
		while(!(m_pDataAccess->Eof()))
		{
			bool bDelete = FALSE;
			for (int i=0;i<m_ContinueFieldList.size();i++)
			{
				if (pFieldList->FieldByName(m_ContinueFieldList.at(i)->strFieldName)->IsNull())
				{
					m_pDataAccess->Delete();
					bDelete = TRUE;
					break;
				}
			}
			if (bDelete)
				continue;
			m_pDataAccess->Next();
		}
	}
}

int		CTreeNode::GetCount()
{
	return m_vtCheckData.sum();
}
void  CTreeNode::ClearNode()
{
	int i;
	for (i=0;i<m_DiscreteFieldList.size();i++)
	{
		FieldClass *pField = m_DiscreteFieldList.at(i);
		delete pField;
	}
	for ( i=0;i<m_ContinueFieldList.size();i++)
	{
		FieldClass *pField = m_ContinueFieldList.at(i);
		delete pField;
	}
	for (i=0; i< m_ContinueFieldList.size();i++)
	{
		FieldClass *field=   m_ContinueFieldList.at(i);
		delete field;
	}
	this->m_ContinueFieldList.clear();
	for (i=0; i< m_DiscreteFieldList.size();i++)
	{
		FieldClass *field=  m_DiscreteFieldList.at(i);
		delete field;
	}
	this->m_DiscreteFieldList.clear();
	
	m_SplitField.VarValue.clear();
	m_SplitField.ChildIndex.clear();
	
	m_ClassCounts.clear();
}

bool	CTreeNode::SearchTreeLeaf(int  nGenType)
{

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// DecTree


CDecTree::CDecTree()
{
	m_pSourceAccess=NULL;
	m_pTargetAccess=NULL;
	m_rootNode=NULL;
	m_pbCheck=NULL;
	m_nMissingRecNum=0;
	m_nSelect = -1;
	m_bOutput = false;
}	
CDecTree::~CDecTree()
{
	EmptyData();
	CTreeNode::Delete(m_rootNode);
	ClearAccess();
}
bool CDecTree::ConnectDatabase(LPCTSTR lpszFileName)
{
	bool bConnected=FALSE;
	/*  -by pyx 2006-02-15
	try{
		bConnected=TRUE;
	}
		catch(CMemoryException a){
			bConnected=FALSE;
		}*/
		
	return bConnected;
}
void CDecTree::SetSourceTableName(CTString TableName)
{
	m_strSourceTableName=TableName;
}

void CDecTree::SetTempTableName(CTString		strTmpTableName)
{
	char *strFieldName=new char[MAX_PATH];
	int nBufferLength=MAX_PATH;
	_getcwd(strFieldName,nBufferLength-1);
	//::GetCurrentDirectory(nBufferLength,strFieldName);
	CTString temp(strFieldName);
	temp = TCSystem::ConvertPath(temp.GetData());
	m_strTempTableName=temp+_T("/");
	m_strTempTableName+=strTmpTableName;
	delete []strFieldName;
	
}
void CDecTree::SetTargetField(CTString Target)
{
	m_TargetField.strFieldName=Target;
	m_TargetField.NumBranch=0;
}

void CDecTree::AddField(CTString FieldName,bool bDiscrete)
{
	FieldClass *field=new FieldClass;
	field->NumBranch=0;
	field->strFieldName=FieldName;
	if(bDiscrete)		
		m_DiscreteFieldList.push_back(field);
	else
		m_ContinueFieldList.push_back(field);
}

bool CDecTree::CheckFieldName(CTString FieldName,bool bDiscrete)
{
	FieldClass *field = NULL;
	if(bDiscrete)		
		for (int i= 0; i< m_DiscreteFieldList.size();i++)
		{
			field = m_DiscreteFieldList.at(i);
			if (FieldName.Compare(field->strFieldName) == SC_EQUAL)
				return true;
		}
	else
		for (int i= 0; i< m_ContinueFieldList.size();i++)
		{
			field = m_ContinueFieldList.at(i);
			if (FieldName.Compare(field->strFieldName) == SC_EQUAL)
				return true;
		}
	return false;
}
bool CDecTree::CheckTargetName(CTString FieldName)
{
	if (m_TargetField.strFieldName.Compare(FieldName) == SC_EQUAL)
		return true;
	else
		return false;
}

void CDecTree::ClearAccess()
{
	FieldClass *field = NULL;
	int i;
	for ( i=0;i<m_vectRedDecTree.size();i++)
		delete m_vectRedDecTree.at(i);
	m_vectRedDecTree.clear();

	for (i=0;i<m_vectTreeRule.size();i++)
		delete m_vectTreeRule.at(i);
	m_vectTreeRule.clear();
}
void  CDecTree::ClearfieldList()
{
	int i;
	for ( i=0; i< m_DiscreteFieldList.size();i++)
		delete m_DiscreteFieldList.at(i);
	m_DiscreteFieldList.clear();

	for (i=0; i< m_ContinueFieldList.size();i++)
		delete m_ContinueFieldList.at(i);
	m_ContinueFieldList.clear();

	m_TargetField.VarValue.clear();
}

void CDecTree::SetTreeParam(int MaxBranches,int MaxDepth,int MinCaseLeaf,
							 float fErrorRatio,float fMinInfoGain,float fMinGINIReduction)
{
	m_nMaxBranches=MaxBranches;
	m_nMaxDepth=MaxDepth;
	m_nMinCaseLeaf=MinCaseLeaf;
	m_fMaxErrorRate=fErrorRatio;
	m_fMinInfoGain=fMinInfoGain;
	m_fMinGINIReduction=fMinGINIReduction;
}

bool CDecTree::CreateTargetAccess()
{
	if(m_pTargetAccess!=NULL)
		delete m_pTargetAccess;
	m_pTargetAccess=new CDataAccess();
	m_pTargetAccess->NewFile();
	CFieldList* pFieldList = m_pTargetAccess->m_pFieldList;
	CField* pTempField;
	int i=0;
	int NumDisc,NumCont;
	NumDisc=m_DiscreteFieldList.size();
	NumCont=m_ContinueFieldList.size();
	for(i = 0;i<NumDisc;i++){
		pTempField = pFieldList->CreateField(fInt);
		pTempField->SetFieldName(m_DiscreteFieldList[i]->strFieldName);
		pFieldList->Add(pTempField);
	}
	for(i = 0;i<NumCont;i++){
		pTempField = pFieldList->CreateField(m_ContinueFieldList[i]->cFieldType);
		pTempField->SetFieldName(m_ContinueFieldList[i]->strFieldName);
		pFieldList->Add(pTempField);
	}
	pTempField = pFieldList->CreateField(fInt);
	pTempField->SetFieldName(m_TargetField.strFieldName);
	pFieldList->Add(pTempField);
	m_strTempTableName="C:/DeciTreeTempTable.tl";;
	return TRUE;
}

//有序型，名义型都是离散的，标量型是连续的
bool CDecTree::InitData()
{
	//初始化，一次性把数据读入到内存中
	int i;
	CFieldList *pListField = m_pSourceAccess->m_pFieldList;
	FieldSourceData *pSource=NULL;
	CField * pField=NULL;
	int nTotalCase = m_pSourceAccess->GetRecordCount();

	for (i=0;i<m_arSourceData.GetSize();i++)
	{
		FieldSourceData *pSource = (FieldSourceData*)m_arSourceData.GetAt(i);
		pSource->arStringData.RemoveAll();
		pSource->vectDoubleData.destruction();
		pSource->vectIntData.destruction();
	}
	m_arSourceData.RemoveAll();
	for (i=0;i<m_DiscreteFieldList.size();i++)//离散变量
	{
		pSource = new FieldSourceData;
		pSource->strFieldName = m_DiscreteFieldList.at(i)->strFieldName;
		pField = pListField->FieldByName(pSource->strFieldName);
		pSource->nType = pField->GetFieldType();
		m_DiscreteFieldList.at(i)->cFieldType = pSource->nType;
		if (pSource->nType == fString)
			pSource->arStringData.SetSize(nTotalCase);
		else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
			pSource->vectIntData.create(nTotalCase);
		else
			pSource->vectDoubleData.create(nTotalCase);
		m_arSourceData.Add(pSource);
	}
	for (i=0;i<m_ContinueFieldList.size();i++)//连续变量
	{
		pSource = new FieldSourceData;
		pSource->strFieldName = m_ContinueFieldList.at(i)->strFieldName;
		pField = pListField->FieldByName(pSource->strFieldName);
		pSource->nType = pField->GetFieldType();
		if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
			pSource->vectIntData.create(nTotalCase);
		else
			pSource->vectDoubleData.create(nTotalCase);
		m_ContinueFieldList.at(i)->cFieldType = pSource->nType;
		m_arSourceData.Add(pSource);
	}
	pSource = new FieldSourceData;//目标变量的数值
	pSource->strFieldName = m_TargetField.strFieldName;
	pField = pListField->FieldByName(pSource->strFieldName);
	pSource->nType = pField->GetFieldType();
	m_TargetField.cFieldType = pSource->nType;
	if (pSource->nType == fString)
		pSource->arStringData.SetSize(nTotalCase);
	else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
		pSource->vectIntData.create(nTotalCase);
	else
		pSource->vectDoubleData.create(nTotalCase);
	m_arSourceData.Add(pSource);

	int nCur = 0;
	bool bMissing = FALSE;
	int NumTotalCase = 0;
	int nDisNum = m_DiscreteFieldList.size();
	DBVariant	valDB;
	m_pSourceAccess->First();
	while(!(m_pSourceAccess->Eof())){
		bMissing = FALSE;
		for (i=0;i<m_DiscreteFieldList.size();i++)	
		{
			pField = pListField->FieldByName(m_DiscreteFieldList.at(i)->strFieldName);
			bMissing = pField->IsNull();
			if (bMissing)
				break;
			pSource = (FieldSourceData*)m_arSourceData.GetAt(i);
			if (pSource->nType == fString)
			{
				CTString strVal;//为空作为缺失值，
				strVal = pField->GetAsString();
				pSource->arStringData.SetAt(nCur,strVal);
				CTString strTest = pSource->arStringData.GetAt(nCur);
				valDB.strVal = new char[strVal.GetLength()+1];
				memcpy(valDB.strVal,strVal,strVal.GetLength()+1);
 				CompareAndSave(m_DiscreteFieldList.at(i),valDB,pSource->nType,TRUE);
			}
			else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
			{
				int nVal = 0;
				nVal = pField->GetAsInteger();
				pSource->vectIntData(nCur) = nVal;
				valDB.nVal = nVal;
 				CompareAndSave(m_DiscreteFieldList.at(i),valDB,pSource->nType,TRUE);		
			}
			else
			{
				double dbVal = 0;
				dbVal = pField->GetAsDouble();
				pSource->vectDoubleData(nCur) = dbVal;
 				valDB.dblVal = dbVal;
 				CompareAndSave(m_DiscreteFieldList.at(i),valDB,pSource->nType,TRUE);		
			}
		}
		if (!bMissing)
		{
			for (i=0;i<m_ContinueFieldList.size();i++)
			{
				pField = pListField->FieldByName(m_ContinueFieldList.at(i)->strFieldName);
				bMissing = pField->IsNull();
				if (bMissing)
					break;
				pSource = (FieldSourceData*)m_arSourceData.GetAt(nDisNum+i);
				if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
				{
					int nVal = 0;//如果不存在，最大浮点数为缺失值
					nVal = pField->GetAsInteger();
					pSource->vectIntData(nCur) = nVal;
				}
				else
				{
					double dbVal = 0;//如果不存在，最大浮点数为缺失值
					dbVal = pField->GetAsDouble();
					pSource->vectDoubleData(nCur) = dbVal;
				}
			}
		}

		if (!bMissing)
		{
			pField = pListField->FieldByName(m_TargetField.strFieldName);
			bMissing = pField->IsNull();
			if (bMissing)
				break;
			int nOff =m_arSourceData.GetSize()-1;
			pSource = (FieldSourceData*)m_arSourceData.GetAt(nOff);
			if (pSource->nType == fString)
			{
				CTString strVal;
				strVal = pField->GetAsString();
				pSource->arStringData.SetAt(nCur,strVal);
				valDB.strVal = new char[strVal.GetLength()+1];
				memcpy(valDB.strVal,(LPCTSTR)strVal,strVal.GetLength()+1);
				CompareAndSave(&m_TargetField,valDB,pSource->nType,TRUE);
			}
			else if ((pSource->nType == fBoolean)||(pSource->nType == fInt))
			{
				int nVal = 0; //如果不存在，整数最大值为缺失值
				nVal = pField->GetAsInteger();
				pSource->vectIntData(nCur) = nVal;
				valDB.nVal = nVal;
				CompareAndSave(&m_TargetField,valDB,pSource->nType,TRUE);
			}
			else
			{
				double dVal = 0; //如果不存在，整数最大值为缺失值
				dVal = pField->GetAsDouble();
				pSource->vectDoubleData(nCur) = dVal;
				valDB.dblVal = dVal;
				CompareAndSave(&m_TargetField,valDB,pSource->nType,TRUE);
			}
		}
		if (!bMissing)
		{
			nCur++;
			NumTotalCase++;
		}
		m_pSourceAccess->Next();
	}
	m_nMissingRecNum = nTotalCase - NumTotalCase;
	m_nTotalCase = NumTotalCase;
				
	pField = pListField->FieldByName(m_TargetField.strFieldName);
	if (pField->bHaveValueLabel())
	{
		for (i=0;i<pField->m_ValueLabel.GetCount();i++)
		{
			LabelData  *pLabel = new LabelData;
			pField->m_ValueLabel.SetCurPos(i);
			if (pField->GetFieldType() == fString)
			{
				CTString str = pField->m_ValueLabel.GetAsString();
				pLabel->varValue.strVal = new char[str.GetLength()+1];
				memcpy(pLabel->varValue.strVal,(LPCTSTR)str,str.GetLength()+1);
				str.ReleaseBuffer();
			}
			else if ((pField->GetFieldType() == fBoolean)||((pField->GetFieldType() == fInt)))
				pLabel->varValue.nVal = pField->m_ValueLabel.GetAsInteger();
			else
				pLabel->varValue.dblVal = pField->m_ValueLabel.GetAsDouble();
			pLabel->strLabelName = pField->m_ValueLabel.GetExplainString();
			m_TargetField.FieldLabel.push_back(pLabel);
		}
	}
	//add by xds 20060525
	for (int j=0;j<m_DiscreteFieldList.size();j++)
	{
		FieldClass* pClass = m_DiscreteFieldList.at(j);
		pField = pListField->FieldByName(pClass->strFieldName);
		if (pField->bHaveValueLabel())
		{
			for (i=0;i<pField->m_ValueLabel.GetCount();i++)
			{
				LabelData  *pLabel = new LabelData;
				pField->m_ValueLabel.SetCurPos(i);
				if (pField->GetFieldType() == fString)
				{
					CTString str = pField->m_ValueLabel.GetAsString();
					pLabel->varValue.strVal = new char[str.GetLength()+1];
					memcpy(pLabel->varValue.strVal,(LPCTSTR)str,str.GetLength()+1);
					str.ReleaseBuffer();
				}
				else if ((pField->GetFieldType() == fBoolean)||((pField->GetFieldType() == fInt)))
					pLabel->varValue.nVal = pField->m_ValueLabel.GetAsInteger();
				else
					pLabel->varValue.dblVal = pField->m_ValueLabel.GetAsDouble();
				pLabel->strLabelName = pField->m_ValueLabel.GetExplainString();
				pClass->FieldLabel.push_back(pLabel);
			}
		}
	}
	if (NumTotalCase==0)
		return false;
	return true;
}

void CDecTree::GetFieldInfo()
{

	
	CFieldList* pFieldList = m_pSourceAccess->m_pFieldList;
	CField* pTempField;
	FieldClass *field;
	int i;
	int NumDisc=m_DiscreteFieldList.size();
	int NumCont=m_ContinueFieldList.size();
	for(i=0;i<NumDisc;i++){
		field=m_DiscreteFieldList[i];
		pTempField=pFieldList->FieldByName(field->strFieldName);
		field->cFieldType=pTempField->GetFieldType();
		field->NumBranch=0;
	}
	for(i=0;i<NumCont;i++){
		field=m_ContinueFieldList[i];
		pTempField=pFieldList->FieldByName(field->strFieldName);
		field->cFieldType=pTempField->GetFieldType();
		field->NumBranch=0;
	}
	pTempField=pFieldList->FieldByName(m_TargetField.strFieldName);
	m_TargetField.cFieldType=pTempField->GetFieldType();
	m_TargetField.NumBranch=0;
}

CTString	 CDecTree::GetTreeRule(CTString strFilter)
{
	for (int i=0;i<m_vectTreeRule.size();i++)
	{
		CTString str = m_vectTreeRule.at(i)->strFilter;
		if (str.Compare(strFilter) == SC_EQUAL)
			return m_vectTreeRule.at(i)->strRule;
	}
	return CTString("");
}

void	CDecTree::AddNode(CTreeNode *pNode)	
{
	m_arNode.Add(pNode);
}

CTreeNode* CDecTree::GenRootNode(int nGenType,int nPruneType)
{
	m_rootNode=new CTreeNode(this);
	m_rootNode->NextSibling=NULL;
	m_rootNode->FirstChild=NULL;
	m_rootNode->Parent=NULL;
	m_rootNode->PrevSibling=NULL;
	
	m_rootNode->SetStopCheck(m_pbCheck);
	//m_rootNode->SetPrgWnd(m_pPrgWnd,m_prgMsg); -by pyx 2006-02-15
	//Step 1: copy lists got by initiating data
	int nDiscretSize= this->m_DiscreteFieldList.size();
	int nCur,nContSize= this->m_ContinueFieldList.size();
	m_rootNode->SetTargetField(&m_TargetField);
	if ( nDiscretSize>0){// copy discreteField List	
		//copy fields
		for (nCur=0;nCur<nDiscretSize;nCur++){		
			m_rootNode->m_DiscreteFieldList.push_back( this->m_DiscreteFieldList[nCur]);
		}
	}
	
	if (nContSize>0){// copy continueField List
		for (nCur=0;nCur<nContSize;nCur++){		//copy fields
			m_rootNode->m_ContinueFieldList.push_back(this->m_ContinueFieldList[nCur]);
		}
	}
	
	m_rootNode->SetTreeParam(this->m_nMaxBranches,this->m_nMaxDepth,this->m_nMinCaseLeaf,
		this->m_fMaxErrorRate,this->m_fMinInfoGain,this->m_fMinGINIReduction);
	m_rootNode->m_nNodeType=ROOT;
	//add by liytao
	// GenerateType:
	//GENTREE_GINI	//树生成GINI系数
	//GENTREE_ENTROPY	//树生成信息增益

	//PruneType:
	//PPEPRUNE_GINI				//先剪枝GINI系数
	//PREPRUNE_ENTROPY			//先剪枝信息增益
	//PREPRUNE_DEPTH			//先剪枝深度
	//POSTPRUNE_ERRRATE			//后剪枝错误率
	if(m_pbCheck!=NULL)
	{
		if (*m_pbCheck)
		{
			delete m_rootNode;
			m_rootNode=NULL;
			return NULL;
		}
	}
	bool bOK;

	m_rootNode->m_vtCheckData.create(m_nTotalCase,1);
	
	if (nGenType==GENTREE_GINI)
		bOK=m_rootNode->CalcGINIReduction(nPruneType);
	if (nGenType==GENTREE_ENTROPY)
		bOK=m_rootNode->CalcEntropyGain(nPruneType);
	m_rootNode->ClearTempAccess();
	
	m_rootNode->m_vtCheckData.destruction();
	if(m_pbCheck!=NULL)
	{
		if (*m_pbCheck)
		{
			delete m_rootNode;
			m_rootNode=NULL;
			return NULL;
		}
	}
	if(!bOK)
		return m_rootNode;

	m_rootNode->m_nLevel=1;  //add end	
	
 	return m_rootNode;
}

void CDecTree::CompareAndSave(FieldClass *pFieldClass,DBVariant Value,CFieldType cType,bool bDist)
{
	if (!bDist)
		return;

	int  NumValue=pFieldClass->NumBranch;
	bool bInVector=FALSE;
	int  nSaveVal=-1;
	for(int i=0;i<NumValue;i++){
		bInVector=FALSE;
		switch(pFieldClass->cFieldType){
		case fBoolean:
			if (pFieldClass->VarValue[i].bVal==Value.bVal)
			//if((pFieldClass->VarValue[i].bVal&&bVal)||(!pFieldClass->VarValue[i].bVal&&!bVal))
				bInVector=TRUE;
			break;
		case fInt:
			if(pFieldClass->VarValue[i].nVal==Value.nVal)
				bInVector=TRUE;
			break;
		case fDouble:
		case fCurrency:
		case fDate:
			if(fabs(pFieldClass->VarValue[i].dblVal-Value.dblVal)<0.0000001)
				bInVector=TRUE;
			break;
		case fString:
			if(strcmp(pFieldClass->VarValue[i].strVal,Value.strVal)==0)
				bInVector=TRUE;
			break;
		default:
			break;
		}
		if(bInVector){
			nSaveVal=i;
			break;
		}
	}

	DBVariant AddedValue;
	if(!bInVector){
		switch(pFieldClass->cFieldType){
		case fBoolean:
			AddedValue.bVal=Value.bVal;
			pFieldClass->VarValue.push_back(AddedValue);
			break;
		case fInt:
			AddedValue.nVal=Value.nVal;
			//memset(AddedValue.strVal,0x00,strVal.GetLength()+1);
			pFieldClass->VarValue.push_back(AddedValue);
			break;
		case fDouble:
		case fCurrency:
		case fDate:
			AddedValue.dblVal=Value.dblVal;
			pFieldClass->VarValue.push_back(AddedValue);
			break;
		case fString:
			AddedValue.strVal=new char[strlen(Value.strVal)+1];
			memset(AddedValue.strVal,0x00,strlen(Value.strVal)+1);
			memcpy(AddedValue.strVal,Value.strVal,strlen(Value.strVal)+1);
			pFieldClass->VarValue.push_back(AddedValue);
			break;	
		default:
			break;
		}
		nSaveVal=pFieldClass->NumBranch;
		pFieldClass->NumBranch+=1;
	}
}

bool CDecTree::EmptyData()
{
	//delete m_pSourceAccess;
	delete m_pTargetAccess;
	m_pTargetAccess = NULL;
	if (m_rootNode==NULL)
		return FALSE;

	FieldClass	**iterator_;
	std::vector<FieldClass*>::iterator iterator;
	for (iterator=m_ContinueFieldList.begin();iterator!=m_ContinueFieldList.end();iterator++)
	{
		(FieldClass*)(*(iterator))->VarValue.empty();
		(FieldClass*)(*(iterator))->FieldLabel.empty();
		delete *iterator; 
	}
	for (iterator=m_DiscreteFieldList.begin();iterator!=m_DiscreteFieldList.end();iterator++)
	{
		(FieldClass*)(*(iterator))->VarValue.empty();
		(FieldClass*)(*(iterator))->FieldLabel.empty();
		delete *iterator; 
	}
	this->m_ContinueFieldList.clear();
	this->m_DiscreteFieldList.clear();

	m_TargetField.VarValue.empty();
	m_TargetField.FieldLabel.empty();
	m_TargetField.ChildIndex.empty();

	for (int i=0;i<m_arSourceData.GetSize();i++)
	{
		FieldSourceData *pSource = (FieldSourceData*)m_arSourceData.GetAt(i);
		pSource->arStringData.RemoveAll();
		pSource->vectDoubleData.destruction();
		pSource->vectIntData.destruction();
	}
	m_arSourceData.RemoveAll();

	DeleteAllNode(m_rootNode);
	m_rootNode = NULL;
	return TRUE;
}

void   CDecTree::DeleteAllNode(CTreeNode * pTreeNode)
{
	CTString strVal;
	CTreeNode *pCur,*pPrev;
	if (pTreeNode == NULL)
		return;
	
	pCur= (CTreeNode *)pTreeNode->FirstChild;
	if (pCur!=NULL)
	{
		while (pCur->NextSibling!=NULL)
			pCur= (CTreeNode *)pCur->NextSibling;
		pPrev= (CTreeNode *)pCur->PrevSibling;
	}
	else
	{
		pPrev=NULL;
	}
	while(pPrev!=NULL)
	{
		DeleteAllNode(pCur);
		pPrev->NextSibling=NULL;
		pCur=pPrev;
		pPrev=(CTreeNode *)pPrev->PrevSibling;
	}
	DeleteAllNode(pCur);
	
	delete pTreeNode;
	return;
	
	int numChild = CTreeNode::GetChildrenCount(pTreeNode);
	for (int i=0; i< numChild; i++)
	{
		CTreeNode * pChild = (CTreeNode *)CTreeNode::GetChild(pTreeNode,i+1);
		if (pChild != NULL)
		{
			DeleteAllNode(pChild);
			pChild->ClearNode();
			pChild->ClearNode();
			delete pChild;
		}
	}
}


float CDecTree::EvaluateTree()
{
    int nRightSum=0,nWrongSum=0;
    bool bOK;
	if (m_rootNode == NULL)
		return 0;
    for(int i=0;i<m_nTotalCase;i++)
	{
            bOK=m_rootNode->Evaluate(m_arSourceData,i);
            if (bOK)
                    nRightSum++;
            else
                    nWrongSum++;
    }
    return ((float)((float)nRightSum/(float)(nWrongSum+nRightSum)));
}


bool CDecTree::CreateTree(int nGenType, int nPruneType)
{
	bool bOk=InitData();
	if (m_pbCheck!=NULL)
	{
		if (*m_pbCheck)
			return FALSE; 
	}
	if (!bOk)
		return FALSE;
	/*
		if (m_pPrgWnd!=NULL)
				m_pPrgWnd->SendMessage(m_prgMsg,13,0);*/ //-by pyx 2006-02-15
		
	CTreeNode::nPrgPos=0;
	CTreeNode *proot=GenRootNode(nGenType,nPruneType);
	if (m_pbCheck!=NULL)
	{
		if (*m_pbCheck)
			return FALSE; 
	}
	if(proot==NULL)
		return FALSE;
	//if (m_pPrgWnd!=NULL)
		//		m_pPrgWnd->SendMessage(m_prgMsg,15,0); -by pyx 2006-02-15
		
	int nOK=m_rootNode->GenerateTree(nGenType,nPruneType);
	if (m_pbCheck!=NULL)
	{
		if (*m_pbCheck)
			return FALSE; 
	}
	if (nOK==-1)
		return FALSE;
	bool bOK;
	bOK=m_rootNode->MergeSameChildren();
	if(nPruneType== POSTPRUNE_ERRRATE) 
		PostPruneTree(nPruneType);
 	m_dbCorrect = EvaluateTree();
 	this->PrtField(m_rootNode);
	return TRUE;
}


//add by liytao
bool CDecTree::PostPruneTree(int nPruneType)
{
	m_rootNode->PostPrune(nPruneType);
	return TRUE;
}
// add end

void CDecTree::SetStopCheck(bool *bCheck)
{
	if(bCheck!=NULL)
	m_pbCheck=bCheck;
}

bool CDecTree::GetCheck()
{
	if (m_pbCheck!=NULL)
		return *m_pbCheck;
	else return FALSE;
}

//预测时，若存在相同的列，则删除－add by xds 20060727.
bool CDecTree::PreDecTree(CResult *pResult)
{
	int nTemp=0, nRowIndex=0;
	CTString strFieldName("");
	
	if (m_ContinueFieldList.size() < 1 &&
		m_DiscreteFieldList.size() < 1)
		return FALSE;
	//总行数
	int nRowNums = m_pSourceAccess->GetRecordCount();
	//目标索引
	int nTargIndex = m_pSourceAccess->IndexByName(m_TargetField.strFieldName);
	if (nRowNums < 1 || nTargIndex < 0) return false;
	if (m_bOutput) //输出到文件:只预测不评估
	{
		CEvaluate ev(this);//预测值—规则
		CField *pFieldTag = NULL;
		CField *pFieldCrd = NULL;
		CField *pFieldRul = NULL;
		//原始列数
		int nOrgColNums=m_pSourceAccess->m_pFieldList->GetFieldCount();
		if (m_TargetField.cFieldType == fInt)
			pFieldTag = m_pSourceAccess->m_pFieldList->CreateField(fInt);
		else
			pFieldTag = m_pSourceAccess->m_pFieldList->CreateField(fString);
		strFieldName.Format("%s_预测值",m_TargetField.strFieldName.GetData());
		if(m_pSourceAccess->m_pFieldList->FieldByName(strFieldName)!=NULL)
		{
			nTemp = m_pSourceAccess->IndexByName(strFieldName);
			m_pSourceAccess->m_pFieldList->Delete(nTemp);
			//strFieldName+="_1";
			nOrgColNums -= 1;
		}
		pFieldTag->SetFieldName(strFieldName);
		m_pSourceAccess->m_pFieldList->Add(pFieldTag);
		for (int ii=0;ii<m_TargetField.FieldLabel.size();ii++)
		{
			pFieldTag->m_ValueLabel.SetCurPos(ii);
			pFieldTag->m_ValueLabel.AddValue(m_TargetField.FieldLabel.at(ii)->varValue.nVal,m_TargetField.FieldLabel.at(ii)->strLabelName);
		}
		//可信度
		pFieldCrd = m_pSourceAccess->m_pFieldList->CreateField(fDouble);
		strFieldName.Format("可信度_预测值");
		if(m_pSourceAccess->m_pFieldList->FieldByName(strFieldName)!=NULL)
		{
			nTemp = m_pSourceAccess->IndexByName(strFieldName);
			m_pSourceAccess->m_pFieldList->Delete(nTemp);
			//strFieldName+="_1";
			nOrgColNums -= 1;
		}
		pFieldCrd->SetFieldName(strFieldName);
		m_pSourceAccess->m_pFieldList->Add(pFieldCrd);
		//预测值—规则
		pFieldRul = m_pSourceAccess->m_pFieldList->CreateField(fDouble);
		strFieldName.Format("规则_预测值");
		if(m_pSourceAccess->m_pFieldList->FieldByName(strFieldName)!=NULL)
		{
			nTemp = m_pSourceAccess->IndexByName(strFieldName);
			m_pSourceAccess->m_pFieldList->Delete(nTemp);
			//strFieldName+="_1";
			nOrgColNums -= 1;
		}
		pFieldRul->SetFieldName(strFieldName);
		m_pSourceAccess->m_pFieldList->Add(pFieldRul);
		//记录开始
		m_pSourceAccess->First();
		CFieldList *pFieldList=m_pSourceAccess->m_pFieldList;
		while(!m_pSourceAccess->Eof())
		{
			CTreeNode * pTreeNode=NULL;
			DBVariant* Value = SearchTreeLeaf(m_rootNode,pFieldList,pTreeNode);
			if (pTreeNode != NULL)
			{
				m_pSourceAccess->Edit();
				if (m_TargetField.cFieldType == fString)
				{
					pFieldTag->SetAsString(Value->strVal);
					nTemp = atoi(Value->strVal);
				}
				else if ((m_TargetField.cFieldType == fInt)||
					(m_TargetField.cFieldType == fBoolean))
				{
					pFieldTag->SetAsInteger(Value->nVal);
					nTemp = Value->nVal;
				}
				else
				{
					pFieldTag->SetAsDouble(Value->dblVal);
					nTemp = Value->dblVal;
				}
				double dAvail;
				pTreeNode->GetMaxClass(dAvail);
				pFieldCrd->SetAsDouble(dAvail);
				if (!ev.In(nTemp)) dAvail = 1 - dAvail;
				pFieldRul->SetAsDouble(dAvail);
				nRowIndex ++;
			}
			else
			{
				pFieldTag->SetNull();
				pFieldCrd->SetNull();
				pFieldRul->SetNull();
			}
			//m_pSourceAccess->Post();			
			m_pSourceAccess->Next();
		}
		m_nMissingRecNum = m_pSourceAccess->GetRecordCount() - nRowIndex;
		////SaveFile
		CTString szNewFile("");
		CTString szNowFile = m_pSourceAccess->GetFileName();
		bool bOld = m_pSourceAccess->SaveFile(szNowFile,&szNewFile);
		//真时:为NewFile
		//假时:为NowFile.
		m_SaveFileName.Format("决策树预测后保存的文件名为：%s\n",bOld ? szNowFile.GetData() : szNewFile.GetData());
	}
	else   //不输出到文件:预测+评估
	{
		CDWordArray   arrCol;
		CDoubleMatrix dataMatrix;
		CMapManager   dataMap;
		CDoubleMatrix  m;   //实际值-预测值-概率

		m.create(nRowNums,3);//创建
		if (m_TargetField.cFieldType == fString)
		{
			CDataInterface face;
			arrCol.Add(nTargIndex);
			face.m_DataAccess.Swap(m_pSourceAccess);
			face.GetColsData(arrCol,dataMatrix, dataMap);
			m_pSourceAccess->Swap(&face.m_DataAccess);
		}
		CField *pField = m_pSourceAccess->FieldByIndex(nTargIndex);
		//首
		m_pSourceAccess->First();
		CFieldList *pFieldList=m_pSourceAccess->m_pFieldList;
		while(!m_pSourceAccess->Eof())
		{
			CTreeNode * pTreeNode=NULL;
			DBVariant* Value = SearchTreeLeaf(m_rootNode,pFieldList,pTreeNode);
			if (pTreeNode != NULL && pField != NULL)
			{
				if (m_TargetField.cFieldType == fString)
				{
					m(0)(nRowIndex) = dataMap.GetIntValue(0,pField->GetAsString());
					m(1)(nRowIndex) = dataMap.GetIntValue(0,Value->strVal);
				}
				else if ((m_TargetField.cFieldType == fInt)||(m_TargetField.cFieldType == fBoolean))
				{
					m(0)(nRowIndex) = pField->GetAsInteger(); //实际值
					m(1)(nRowIndex) = Value->nVal;            //预测值
				}
				else
				{
					m(0)(nRowIndex) = pField->GetAsDouble(); //实际值
					m(1)(nRowIndex) = Value->dblVal;         //预测值
				}
				//置信度
				double dAvail = 0;
				pTreeNode->GetMaxClass(dAvail);
				m(2)(nRowIndex) = dAvail;//概率 = 1 - 置信度？
				nRowIndex ++;
			}
			m_pSourceAccess->Next();
		}
		//缺失值
		m_nMissingRecNum = nRowNums - nRowIndex;
		//数据处理
		m.rerowsize(nRowIndex);
		//模型评估
		CEvaluate ev(this);
		ev.EV(m,pResult);
	}
	return TRUE;
}

void CDecTree::PrtField(CTreeNode * pTreeNode)
{
	if (pTreeNode == NULL)
		return;
//	TRACE("Name:");
//	TRACE(pTreeNode->m_SplitField.strFieldName);
//	TRACE("  Cont:");
// 	int nSize = pTreeNode->m_SplitField.VarValue.size();
//	for (int i=0;i<nSize;i++)
//	{
//		TRACE("%.2f,",pTreeNode->m_SplitField.VarValue.at(i).dblVal);
//	}
//	TRACE("\n");
	
//	for (i= 0; i< pTreeNode->m_NumChild;i++)
//	{
//		CTreeNode * pChild = (CTreeNode *) CTreeNode::GetChild(pTreeNode,i+1);
//		PrtField(pChild);
//	}

}

DBVariant* CDecTree::SearchTreeLeaf(CTreeNode * pTreeNode, CFieldList *pFieldList, CTreeNode * &pNode)
{

	int    i;
	CTreeNode *pCur = pTreeNode;
	pNode = pTreeNode;
	bool bFind = FALSE;
	int nIndex;
	int nCldNum = 1;
	CField *pField=NULL;
	while(pCur!=NULL)
	{ 
		bFind=FALSE;
		pField = pFieldList->FieldByName(pCur->m_SplitField.strFieldName);
// 		TRACE("%s\n",pCur->m_SplitField.strFieldName);
		pNode = pCur;
		if ((pField == NULL)||(pField->IsNull()))
			break;
		if (pCur->m_bDiscrete)
		{
			if (pField->GetFieldType() == fString)
			{
				CTString strVal = pField->GetAsString();
				for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
				{
					if (strVal.Compare(pCur->m_SplitField.VarValue.at(i).strVal)==SC_EQUAL)
					{
						if (i < pCur->m_SplitField.ChildIndex.size())
						{
							nCldNum = pCur->m_SplitField.ChildIndex.at(i);
							pCur = (CTreeNode*)Tree::GetChild(pCur,i+1);
							bFind = TRUE;
						}
						else
							pCur = NULL;
						break;
					}
				}
			}
			else if ((pField->GetFieldType() == fBoolean)||(pField->GetFieldType() == fInt))
			{
				int nVal = pField->GetAsInteger();
				for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
				{
					if (nVal == pCur->m_SplitField.VarValue.at(i).nVal)
					{
						if (i < pCur->m_SplitField.ChildIndex.size())
						{
							nCldNum = pCur->m_SplitField.ChildIndex.at(i);
							pCur = (CTreeNode*)Tree::GetChild(pCur,i+1);
							bFind = TRUE;
						}
						else
							pCur = NULL;
						break;
					}
				}
			}
			else
			{
				double dVal = pField->GetAsDouble();
				for (i=0;i<pCur->m_SplitField.VarValue.size();i++)
				{
					if (pCur->m_SplitField.VarValue.at(i).dblVal == dVal)
					{
						if (i < pCur->m_SplitField.ChildIndex.size())
						{
							nCldNum = pCur->m_SplitField.ChildIndex.at(i);
							pCur = (CTreeNode*)Tree::GetChild(pCur,i+1);
							bFind = TRUE;
						}
						else
							pCur = NULL;
						break;
					}
				}
			}
		}
		else
		{
			double dVal = 0;
			if ((pField->GetFieldType() == fBoolean)||(pField->GetFieldType() == fInt))
				dVal = pField->GetAsInteger();
			else
				dVal = pField->GetAsDouble();
			int nSs = pCur->m_SplitField.VarValue.size();
			if (nSs > 0)
			for (i=0;i<pCur->m_SplitField.VarValue.size()-1;i++)
			{
				if ((dVal >= pCur->m_SplitField.VarValue.at(i).dblVal)&&((dVal < pCur->m_SplitField.VarValue.at(i+1).dblVal)))
				{
					if (i < pCur->m_SplitField.ChildIndex.size())
					{
						nCldNum = pCur->m_SplitField.ChildIndex.at(i);
						pCur = (CTreeNode*)Tree::GetChild(pCur,i+1);
						bFind = TRUE;
					}
					else
						pCur = NULL;
					break;
				}
			}
		}
		
		if (!bFind)
			break;
	}

	int nMaxClass = 0;
	nIndex = 0;
	for (i = 0; i < pNode->m_ClassCounts.size();i++)
	{
		if(nMaxClass<pNode->m_ClassCounts.at(i))
		{
			nMaxClass = pNode->m_ClassCounts.at(i);
			nIndex=i;
		}
	}
	return &m_TargetField.VarValue.at(nIndex);
}

void CTreeNode::SetStopCheck(bool *bCheck)
{
	if(bCheck!=NULL)
		m_pbCheck=bCheck;

}

/*
void CDecTree::SetPrgWnd(CWnd *pPrgWnd,UINT prgMsg)
{
	m_pPrgWnd=pPrgWnd;
	m_prgMsg=prgMsg;
}

void CTreeNode::SetPrgWnd(CWnd *pPrgWnd,UINT prgMsg)
{
	m_pPrgWnd=pPrgWnd;
	m_prgMsg=prgMsg;
}*/ //-by pyx 2006-02-15


bool	CDecTree::SaveTree(CTString  strFileName)
{	
	int i=0, nSize=0, nVal=0;
	CTString szVal(""), szText(""), szTemp("");
	CTreeNode * pSubNode=NULL;
	
	TCLFile theFile;
	bool bOpen = theFile.Open(strFileName, TCLFile::modeCreate|TCLFile::modeReadWrite);
	
	if (!bOpen)
		return false;
	//Java使用
	//算法名称
	int nLen = 0;
	szText = PROGNAME;
	szText += " ";
	szText += DECTREE;
	//数据源
	szText += " ";
	szText += DATASOURCE;
	szText += " ";
	szText += "\"";
	szText+= m_pSourceAccess->GetFileName();
	szText+= "\"";
	//输入变量 
	szText += " ";
	szText += VARINPUT;
	nSize = m_ContinueFieldList.size();
	for (i=0; i<nSize; i++)
	{
		szTemp = m_ContinueFieldList.at(i)->strFieldName;
		szText += " " + szTemp;
	}
	//目标变量
	szText += " ";
	szText += VAROUTPUT;
	szText += " " + m_TargetField.strFieldName;
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
	szVal = DECTREE;
	theFile << szVal;

	//输出信息
	theFile << m_bShowLabel;      //是否显示Label
	//Hit值范围
	nSize = m_szArrHitRange.GetSize();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_szArrHitRange.GetAt(i);
	}
	
	//保存基本信息
	theFile << m_nGenType << m_nPruneType << m_nMaxBranches << m_nMaxDepth << m_nMinCaseLeaf;
	theFile << m_fMinInfoGain << m_fMinGINIReduction << m_fMaxErrorRate;
	
	nSize = m_DiscreteFieldList.size();
	theFile << nSize;
	for (i =0; i< m_DiscreteFieldList.size(); i++)
	{
		theFile << m_DiscreteFieldList.at(i)->strFieldName;
		theFile << m_DiscreteFieldList.at(i)->cFieldType;
	}

	nSize = m_ContinueFieldList.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_ContinueFieldList.at(i)->strFieldName;
		theFile << m_ContinueFieldList.at(i)->cFieldType;
	}
	theFile << m_TargetField.strFieldName;
	theFile << m_TargetField.cFieldType;
	theFile << m_TargetField.NumBranch;
	nSize = m_TargetField.VarValue.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		switch(m_TargetField.cFieldType)
		{
		case fBoolean:
			theFile << m_TargetField.VarValue.at(i).bVal;
			break;
		case fInt:
			theFile << m_TargetField.VarValue.at(i).nVal;
			break;
		case fDouble:
			theFile << m_TargetField.VarValue.at(i).dblVal;
			break;
		case fString:
			szVal = m_TargetField.VarValue.at(i).strVal;
			theFile << szVal;
			break;
		default:
			;
		}
	}
	nSize = m_TargetField.FieldLabel.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_TargetField.FieldLabel.at(i)->varValue.nVal;
		theFile << m_TargetField.FieldLabel.at(i)->strLabelName;
	}
	theFile << m_rootNode->m_bDiscrete;
	theFile << m_rootNode->m_TotalNum;
	theFile << m_rootNode->m_SplitField.strFieldName;
	theFile << m_rootNode->m_SplitField.cFieldType;
	theFile << m_rootNode->m_SplitField.NumBranch;
	nSize = m_rootNode->m_SplitField.VarValue.size();
	theFile << nSize;
	for(i=0; i<nSize; i++)
	{
		if (m_rootNode->m_bDiscrete)
		{
			switch(m_rootNode->m_SplitField.cFieldType) {
			case fBoolean:
				theFile << m_rootNode->m_SplitField.VarValue.at(i).bVal;;
				break;
			case fInt:
				theFile << m_rootNode->m_SplitField.VarValue.at(i).nVal;
				break;
			case fDouble:
				theFile << m_rootNode->m_SplitField.VarValue.at(i).dblVal;
				break;
			case fString:
				szVal = m_rootNode->m_SplitField.VarValue.at(i).strVal;
				theFile << szVal;
				break;
			default:
				break;
			}
		}
		else
		{
			theFile << m_rootNode->m_SplitField.VarValue.at(i).dblVal;
		}
	}
	nSize = m_rootNode->m_SplitField.ChildIndex.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_rootNode->m_SplitField.ChildIndex.at(i);
	}

	nSize = m_rootNode->m_ClassCounts.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << m_rootNode->m_ClassCounts.at(i);
	}
	nSize = CTreeNode::GetChildrenCount(m_rootNode);
	theFile << nSize;
	for (i=1; i<nSize+1; i++)
	{
		pSubNode= (CTreeNode *)CTreeNode::GetChild(m_rootNode,i);
		if (pSubNode != NULL)
		{
			nVal = 1;
			theFile << nVal;
  			SaveNode(pSubNode,theFile);
		}
		else
		{
			nVal = 0;
			theFile << nVal;
		}
	}

	theFile.Close();
	return true;
}

bool	CDecTree::SaveNode(CTreeNode *pTreeNode, TCLFile &theFile)
{
	int i=0, nSize=0, nVal=0;
	CTreeNode * pSubNode=NULL;
	
	theFile << pTreeNode->m_bDiscrete;
	theFile << pTreeNode->m_TotalNum;
	theFile << pTreeNode->m_SplitField.strFieldName;
	theFile << pTreeNode->m_SplitField.NumBranch;
	theFile << pTreeNode->m_SplitField.cFieldType;
	nSize = pTreeNode->m_SplitField.VarValue.size();
	theFile << nSize;
	for(i=0; i<nSize; i++)
	{
		if (pTreeNode->m_bDiscrete)
		{
			switch(pTreeNode->m_SplitField.cFieldType)
			{
			case fBoolean:
				theFile << pTreeNode->m_SplitField.VarValue.at(i).bVal;
				break;
			case fInt:
				theFile << pTreeNode->m_SplitField.VarValue.at(i).nVal;
				break;
			case fDouble:
				theFile << pTreeNode->m_SplitField.VarValue.at(i).dblVal;
				break;
			case fString:
				theFile << pTreeNode->m_SplitField.VarValue.at(i).strVal;
				break;
			default:
				break;
			}
		}
		else
		{
			double dVal = pTreeNode->m_SplitField.VarValue.at(i).dblVal;
			theFile << dVal;
		}
	}
	nSize = pTreeNode->m_SplitField.ChildIndex.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pTreeNode->m_SplitField.ChildIndex.at(i);
	}
	nSize = pTreeNode->m_ClassCounts.size();
	theFile << nSize;
	for (i=0; i<nSize; i++)
	{
		theFile << pTreeNode->m_ClassCounts.at(i);;
	}
	nSize = pTreeNode->m_NumChild;
	theFile << nSize;
	for (i=1; i<nSize+1; i++)
	{
		pSubNode = (CTreeNode *)CTreeNode::GetChild(pTreeNode,i);
		if (pSubNode != NULL)
		{
			nVal = 1;
			theFile << nVal;
 			SaveNode(pSubNode,theFile);
		}
		else
		{
			nVal = 0;
			theFile << nVal;
		}
	}
	return true;
}

bool	CDecTree::LoadTree(CTString  strFileName)
{
	int i=0, nSize=0;
	int nVal=0,nBrach=0;
	char cType;
	CFieldType fldType;
	CTreeNode *pSubNode=NULL;
	CTString szVal("");
	CTString szFldName("");

	EmptyData();
	TCLFile theFile;
	bool bOpen = theFile.Open(strFileName, TCLFile::modeReadWrite);
	if (!bOpen)
		return false;
	//截尾长度
	theFile >> nSize;
	//theFile.ReadString(szTemp);
	//nSize = szTemp.length();
	theFile.Seek(nSize);
	//开始写模型参数
	theFile >> szVal;
	if (szVal.Compare(DECTREE) != SC_EQUAL)
		return false;
	
	//输出信息
	theFile >> m_bShowLabel;      //是否显示Label
	//Hit值范围
	m_szArrHitRange.RemoveAll();
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szVal;
		m_szArrHitRange.Add(szVal);
	}

	//保存基本信息
	theFile >> m_nGenType >> m_nPruneType >> m_nMaxBranches >> m_nMaxDepth >> m_nMinCaseLeaf;
	theFile >> m_fMinInfoGain >> m_fMinGINIReduction >> m_fMaxErrorRate;

	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szFldName;
		theFile >> fldType;
		FieldClass *field = new FieldClass;
		field->NumBranch=0;
		field->strFieldName=szFldName;
		field->cFieldType = fldType;
		m_DiscreteFieldList.push_back(field);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> szFldName;
		theFile >> fldType;
		FieldClass *field = new FieldClass;
		field->NumBranch=0;
		field->strFieldName=szFldName;
		field->cFieldType = fldType;
		m_ContinueFieldList.push_back(field);
	}
	
	theFile >> m_TargetField.strFieldName;
	theFile >> m_TargetField.cFieldType;
	theFile >> m_TargetField.NumBranch;
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		DBVariant  var;
		switch(m_TargetField.cFieldType)
		{
		case fBoolean:
			theFile >> var.bVal;
			break;
		case fInt:
			theFile >> var.nVal;
			break;
		case fDouble:
			theFile >> var.dblVal;
			break;
		case fString:
			theFile >> szVal;
			var.strVal = new char[szVal.GetLength()+1];
			memset(var.strVal,0x00,szVal.GetLength()+1);
			strcpy(var.strVal,szVal.GetBuffer(szVal.GetLength()));
			szVal.ReleaseBuffer();
			break;
		default:
			;
		}
		m_TargetField.VarValue.push_back(var);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		LabelData * pLabel = new LabelData;
		theFile >> pLabel->varValue.nVal;
		theFile >> pLabel->strLabelName;
		m_TargetField.FieldLabel.push_back(pLabel);
	}
	//TreeNode
	m_rootNode = new CTreeNode;
	m_rootNode->NextSibling=NULL;
	m_rootNode->FirstChild=NULL;
	m_rootNode->Parent=NULL;
	m_rootNode->PrevSibling=NULL;
	m_rootNode->m_nLevel = 1;
	m_rootNode->SetTargetField(&m_TargetField);
	theFile >> m_rootNode->m_bDiscrete;
	theFile >> m_rootNode->m_TotalNum;
	theFile >> szFldName;
	theFile >> cType;
	theFile >> nBrach;
	m_rootNode->m_SplitField.cFieldType = cType;
	m_rootNode->m_SplitField.NumBranch = nBrach;
	m_rootNode->m_SplitField.strFieldName = szFldName;
	theFile >> nSize;
	for(i=0; i<nSize; i++)
	{
		DBVariant  var;
		if (m_rootNode->m_bDiscrete)
		{
			switch(m_rootNode->m_SplitField.cFieldType)
			{
			case fBoolean:
				theFile >> var.bVal;
				break;
			case fInt:
				theFile >> var.nVal;
				break;
			case fDouble:
				theFile >> var.dblVal;
				break;
			case fString:
				theFile >> szVal;
				var.strVal=new char[szVal.GetLength()+1];
				memset(var.strVal,0x00,szVal.GetLength()+1);
				strcpy(var.strVal,szVal.GetBuffer(szVal.GetLength()));
				szVal.ReleaseBuffer();
				break;
			default:
				break;
			}
		}
		else
		{
			theFile >> var.dblVal;
		}
		m_rootNode->m_SplitField.VarValue.push_back(var);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		m_rootNode->m_SplitField.ChildIndex.push_back(nVal);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		 m_rootNode->m_ClassCounts.push_back(nVal);
	}
	theFile >> nSize;
	m_rootNode->m_NumChild = nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		if (nVal == 1)
		{
			pSubNode = new CTreeNode;
			pSubNode->NextSibling=NULL;
			pSubNode->FirstChild=NULL;
			pSubNode->Parent=NULL;
			pSubNode->PrevSibling=NULL;
			pSubNode->m_nLevel =m_rootNode->m_nLevel+1;
			pSubNode->SetTargetField(&m_TargetField);
			CTreeNode::AddChild(m_rootNode,pSubNode);
			LoadNode(pSubNode,theFile);
		}
	}
	theFile.Close();
	return true;
}

bool	CDecTree::LoadNode(CTreeNode *pTreeNode, TCLFile &theFile)
{
	int i=0, nSize=0;
	int nVal=0;
	CTString szVal;
	CTreeNode *pSubNode=NULL;
	
	theFile >> pTreeNode->m_bDiscrete;
	theFile >> pTreeNode->m_TotalNum;
	theFile >> pTreeNode->m_SplitField.strFieldName;
	theFile >> pTreeNode->m_SplitField.NumBranch;
	theFile >> pTreeNode->m_SplitField.cFieldType;

	theFile >> nSize;
	for(i=0; i<nSize; i++)
	{
		DBVariant  var;
		if (pTreeNode->m_bDiscrete)
		{
			switch(pTreeNode->m_SplitField.cFieldType)
			{
			case fBoolean:
				theFile >> var.bVal;
				break;
			case fInt:
				theFile >> var.nVal;
				break;
			case fDouble:
				theFile >> var.dblVal;
				break;
			case fString:
				theFile >> szVal;
				var.strVal=new char[szVal.GetLength()+1];
				memset(var.strVal,0x00,szVal.GetLength()+1);
				strcpy(var.strVal,szVal.GetBuffer(szVal.GetLength()));
				szVal.ReleaseBuffer();
				break;
			default:
				break;
			}
		}
		else
		{
			theFile >> var.dblVal;
		}
		pTreeNode->m_SplitField.VarValue.push_back(var);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		pTreeNode->m_SplitField.ChildIndex.push_back(nVal);
	}
	theFile >> nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		pTreeNode->m_ClassCounts.push_back(nVal);
	}
	theFile >> nSize;
	pTreeNode->m_NumChild = nSize;
	for (i=0; i<nSize; i++)
	{
		theFile >> nVal;
		if (nVal == 1)
		{
			pSubNode= new CTreeNode;
			pSubNode->NextSibling=NULL;
			pSubNode->FirstChild=NULL;
			pSubNode->Parent=NULL;
			pSubNode->PrevSibling=NULL;
			pSubNode->m_nLevel =pTreeNode->m_nLevel+1;
			pSubNode->SetTargetField(pTreeNode->m_pTargetField);
			CTreeNode::AddChild(pTreeNode,pSubNode);
			LoadNode(pSubNode,theFile);
		}
	}
	return true;
}
