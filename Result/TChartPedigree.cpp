// TChartLine.cpp: implementation of the CTChartPedigree class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartPedigree.h"
#include "../Public/TCLFile.h"

#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */

const int DIV_X_CNT = 10;//X轴的总分数
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartPedigree::CTChartPedigree()
{
	m_ChartKind = Pedigree;
	m_bDataType = TRUE;
}

CTChartPedigree::CTChartPedigree(BOOL bDataType)
{
	m_ChartKind = Pedigree;
	m_bDataType = bDataType;
}


CTChartPedigree::~CTChartPedigree()
{
	DeleteTree(m_treeNode);
	m_PointArray.RemoveAll();
	Release(&m_arrNumY);
}

void CTChartPedigree::SetValue(double fMinX, double fMaxX, double fMinY, double fMaxY, const PedigreeTreeNode *pNode, CPtrArray &arrNumY)
{
	m_fMinX = fMinX;//X轴最小值
	m_fMaxX = fMaxX;//X轴最大值
	m_fMinY = fMinY;//Y轴最小值
	m_fMaxY = fMaxY;//Y轴最大值
	m_nCount= fMaxY-fMinY;//总个数
	assert(pNode != NULL);
	//拷贝树
	m_treeNode.bLeaf     = pNode->bLeaf;
	m_treeNode.dCredit   = pNode->dCredit;
	m_treeNode.yPosition = pNode->yPosition;
	m_treeNode.NodePtrArray.Copy(pNode->NodePtrArray);
	//取出叶子代码及Y值
	int nSize = arrNumY.GetSize();
	for (int i=0; i<nSize; i++)
	{
		m_arrNumY.Add(arrNumY.GetAt(i));
	}
	arrNumY.RemoveAll();
	if (m_fMaxY < m_treeNode.yPosition) m_fMaxY = m_treeNode.yPosition;
	if (m_fMinY > m_treeNode.yPosition) m_fMinY = m_treeNode.yPosition;
	CalcMaxTreeNode(m_treeNode);
	//计算树中的各个点
	CalcPedigree(m_treeNode);
	//Prepare Data
	TDataPointPedigree *pPredigree = new TDataPointPedigree;
	PrepareData(pPredigree);
	//Data
	SetData(pPredigree,1,EPedigree,ThrArr);
	delete pPredigree;
	pPredigree = NULL;
}

void CTChartPedigree::PrepareData(TDataPointPedigree *pPredigree)
{
	int i = 0, nSize = 0;
	double fValue = 0;
	double fStep =  0;
	CTString szText,szTemp;
	YValue v;
	VPoint pt;

	fStep = (m_fMaxX - m_fMinX)/DIV_X_CNT;//X轴的刻度
	
	for (i=0;i<DIV_X_CNT;i++) //X轴刻度值
	{
		if (m_bDataType)
			fValue = m_fMinX + i*fStep;
		else
			fValue = (1-fStep*i); 
		pPredigree->arrXAxis.Add(fValue);
	}
	if (m_bDataType)
		pPredigree->arrXAxis.Add(m_fMaxX);//X轴最大刻度
	else 
		pPredigree->arrXAxis.Add(1-m_fMaxX);//X轴最小刻度

	nSize = m_PointArray.GetSize();
	for (i=0;i<nSize;i++)
	{
		v = m_PointArray.GetAt(i);
		if (m_bDataType) //分层聚类
		{
			szText.Format("%d",v.leaf);
			pPredigree->arrYAxis.Add(szText);//刻度值
		}
		else             //模糊聚类
		{
			CPtrArray *pTmpAry = (CPtrArray*)m_arrNumY.GetAt(v.leaf);//只是顺序号
			for(int j=0;j<pTmpAry->GetSize();j++)
			{
				int nVal = *((int*)pTmpAry->GetAt(j));//单元号
				szTemp.Format("%d",nVal+1);
				if(j==0)
					szText = szTemp;
				else
					szText += "," + szTemp;
			}
			pPredigree->arrYAxis.Add(szText);
		}
	}
		
	nSize = m_LinePoint.GetSize();
	for (i=0;i<nSize;i++)
	{
		pt = m_LinePoint.GetAt(i);//两点一线
		pPredigree->arrPoint.Add(pt.x);
		pPredigree->arrPoint.Add(pt.y);
	}
}

//并且取出每一个叶子的代码和Y值
void CTChartPedigree::CalcMaxTreeNode(PedigreeTreeNode& node)
{
	if (m_fMaxY < node.yPosition) m_fMaxY = node.yPosition;
	if (m_fMinY > node.yPosition) m_fMinY = node.yPosition;
	int nCount = node.NodePtrArray.GetSize();
	for(int i=0;i<nCount;i++)
	{
		PedigreeTreeNode *pNode = (PedigreeTreeNode*)node.NodePtrArray.GetAt(i);
		CalcMaxTreeNode(*pNode);
	}
	//是否为叶子
	if(node.bLeaf)
	{
		YValue pt;
		pt.leaf=node.elemsCode;
		pt.ypos=node.yPosition;
		m_PointArray.Add(pt);
	}
}

//画每一个系列点
void CTChartPedigree::CalcPedigree(PedigreeTreeNode& node)
{
	VPoint* pSubPoint=NULL;
	int nCount=node.NodePtrArray.GetSize();
	//取Child的点
	int i;
	if(!node.bLeaf && nCount>0)
	{
		for(i=0;i<nCount;i++)
		{
			PedigreeTreeNode *pNode=(PedigreeTreeNode*)node.NodePtrArray.GetAt(i);
			CalcPedigree(*pNode);//递归调用，先画Child
		}
		
		pSubPoint = new VPoint[nCount];
		
		for( i=0;i<nCount;i++)
		{
			PedigreeTreeNode *pNode=(PedigreeTreeNode*)node.NodePtrArray.GetAt(i);
			//if (m_bDataType)
				pSubPoint[i].x = pNode->dCredit;
			//else
			//	pSubPoint[i].x = 1-pNode->dCredit;
			pSubPoint[i].y = pNode->yPosition;
		}
	}
	else
		return;
	
	//取父亲的点
	VPoint pt;
	//if (m_bDataType)
		pt.x = node.dCredit;
	//else
	//	pt.x = 1-node.dCredit;
	//画横线
	double fMinY=0,fMaxY=0;
	for( i=0;i<nCount;i++)
	{
		pt.y = pSubPoint[i].y;
		//add
		m_LinePoint.Add(pSubPoint[i]);
		m_LinePoint.Add(pt);
		if(i == 0)
		{
			fMaxY = fMinY = pSubPoint[i].y;
		}
		else
		{
			if(fMinY > pSubPoint[i].y) fMinY = pSubPoint[i].y;
			if(fMaxY < pSubPoint[i].y) fMaxY = pSubPoint[i].y;
		}
	}
	//画竖线
	pt.y = fMinY;
	m_LinePoint.Add(pt);
	pt.y = fMaxY;
	m_LinePoint.Add(pt);
	
	delete[] pSubPoint;
}

//画图结束后清除树结点中内存
void CTChartPedigree::DeleteTree(PedigreeTreeNode &node)
{
	for(int i=0;i<node.NodePtrArray.GetSize();i++)
	{
		PedigreeTreeNode *p=(PedigreeTreeNode *)(node.NodePtrArray.GetAt(i));
		if(!p->bLeaf)
		{
			DeleteTree(*p);
			delete p;
		}
	}
	node.NodePtrArray.RemoveAll();
}

void CTChartPedigree::Release(CPtrArray *pArr)
{
	int i,j,nSize;

	for(i=0;i<pArr->GetSize();i++)
	{
		CPtrArray *pArrTemp = (CPtrArray *)(pArr->GetAt(i));
		nSize = pArrTemp->GetSize();
		for (j=0; j<nSize; j++)
		{
			delete pArrTemp->GetAt(j);
		}
		pArrTemp->RemoveAll();
		delete pArrTemp;
	}
	pArr->RemoveAll();
}
