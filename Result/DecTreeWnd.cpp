// DecTreeWnd.cpp : implementation file
//

#include "DecTreeWnd.h"
#include "Result.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// CDecTreeWnd

CDecTreeWnd::CDecTreeWnd()
{
}

CDecTreeWnd::~CDecTreeWnd()
{
}

void CDecTreeWnd::Serialize(TCLFile &file)
{
	m_manager.Serialize(file);
}

void CDecTreeWnd::Serialize(unsigned char *&pBuf)
{
	m_manager.Serialize(pBuf);
}

void CDecTreeWnd::Print()
{
	m_manager.Print();
}

void CDecTreeWnd::SetNodeTree(CPtrArray *m_pNodeList)
{
	m_manager.m_pNodeList=m_pNodeList;//(m_pNodeList);
}

int CDecTreeWnd::GetBufLen()
{
	return m_manager.GetBufLen(); 
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ManagerNode::ManagerNode()
{	
	m_bFixWidth = TRUE;
}

ManagerNode::~ManagerNode()
{
	Clear(TRUE);
}

void ManagerNode::SetData(CPtrArray *pArrList)
{
	m_pNodeList = pArrList;
}

void ManagerNode::GetData(tagDataPointDecTree *pData)
{
	int i=0, nSize=0;
	VRect  vr;
	VPoint vp;
	
	//计算并生成树
	CreateTree();
	//填充带边框的矩形及文字
	nSize = m_RectangleList.GetSize();
	for (i=0; i<nSize;i++)
	{
		vr = m_RectangleList.GetAt(i);
		pData->arrRectangle.Add(vr.l);//左
		pData->arrRectangle.Add(vr.t);//上
		pData->arrRectangle.Add(vr.r);//右
		pData->arrRectangle.Add(vr.b);//下
		pData->szArrRectangle.Add(vr.szLabel);//矩形中的文字
	}
	//填充不带边框的矩形及文字
	nSize = m_RectFillList.GetSize();
	for (i=0; i<nSize;i++)
	{
		vr = m_RectFillList.GetAt(i);
		pData->arrRectFill.Add(vr.l);//左
		pData->arrRectFill.Add(vr.t);//上
		pData->arrRectFill.Add(vr.r);//右
		pData->arrRectFill.Add(vr.b);//下
		pData->szArrRectFill.Add(vr.szLabel);//矩形中的文字
	}
	//点－线
	nSize = m_PointList.GetSize();
	for (i=0; i<nSize; i++)
	{
		vp = m_PointList.GetAt(i);
		pData->arrLinePoint.Add(vp.x);//点-X轴坐标
		pData->arrLinePoint.Add(vp.y);//点-Y轴坐标
	}
}

void ManagerNode::Clear(BOOL bReleaseMemory)
{	
	//清空
	m_PointList.RemoveAll();
	m_RectangleList.RemoveAll();
	m_RectFillList.RemoveAll();
	//释放内存
	if (bReleaseMemory)
	{
		DecisionTreeNode *pNode = getRoot();
		if(pNode == NULL) return;
		int nSize = pNode->getChildNum(); 
		for(int i=0; i<nSize&&i<10; i++)
		{
			deleteNode(pNode->getChild(i),i);
		}
		delete pNode;
		pNode = NULL;
	}
}

void ManagerNode::Print()
{
	int i=0, nSize=0;
	DecisionTreeNode * pNode = NULL;

	nSize = m_pNodeList->GetSize ();
	printf("NodeList Size is %d\n",nSize);
	for(i=0; i<nSize; i++)
	{
		pNode =(DecisionTreeNode *)m_pNodeList->GetAt(i);
		pNode->Print();
	}
}

void ManagerNode::Serialize(unsigned char *&pBuf)
{
	int nLen = 	0;
	int nSize = m_pNodeList->GetSize();
	SaveDigit(pBuf,nSize);
	for(int i=0;i<nSize;i++)
	{
		DecisionTreeNode *pNode =(DecisionTreeNode *)m_pNodeList->GetAt(i);
		pNode->Serialize(pBuf);
	}
}

void ManagerNode::Serialize(TCLFile &file)
{
	int i,j,k,nCol=0,nRow=0,nSize,nChild;
	DecisionTreeNode * pNode,*pChild;
	bool bFound;
	nSize=m_pNodeList->GetSize ();
	file << nSize;
	for(i=0;i<nSize;i++)
	{
		pNode =(DecisionTreeNode *)m_pNodeList->GetAt (i);
		pNode->Serialize(file);			
		
	}
	for(i=0;i<nSize;i++)
	{
		pNode =(DecisionTreeNode *)m_pNodeList->GetAt (i);
		nChild=pNode->getChildNum();
		file << nChild;
		for (j=0;j<nChild;j++)
		{
			bFound=false;
			pChild=(DecisionTreeNode*)pNode->getChild(j);
			file << k;
			for (k=0;k<nSize&&!bFound;k++)
			{
				if (pChild==(DecisionTreeNode*)m_pNodeList->GetAt(k))
					bFound=true;
			}
		}
	}		
}

DecisionTreeNode *ManagerNode::getRoot()
{
	DecisionTreeNode *pNode = NULL;
	for(int i=0;i<m_pNodeList->GetSize();i++)
	{
		pNode =(DecisionTreeNode *)m_pNodeList->GetAt(i);
		if(!pNode->isRoot) continue;
		return pNode;
	}
	return NULL;
}

void ManagerNode::setSibling()
{
	DecisionTreeNode *pRootNode = getRoot();
	if(pRootNode == NULL) return;
	CTString szLabel = pRootNode->getNodeLabel();
	if(szLabel.GetLength() > 0)
	{
		pRootNode->setLevel(1);
		setSibling(pRootNode,1);
	}
}

void ManagerNode::setSibling(DecisionTreeNode *pNode, int nLevel)
{
	int i,nChildNum,STARTNUM=0;
	DecisionTreeNode *pChild      = NULL;
	DecisionTreeNode *pTempNode   = NULL;
	DecisionTreeNode *pLeftChild  = NULL;
	DecisionTreeNode *pRightChild = NULL;

	pTempNode = NULL;
	nChildNum = pNode->getChildNum();
	CTString szLabel = pNode->getNodeLabel();
	for(i=STARTNUM; i<nChildNum; i++)
	{
		pNode->getChild(i)->setLevel(nLevel+1);
	}
	if (nChildNum > STARTNUM)
	{
		pChild=pNode->getChild(STARTNUM);
		pChild->setLeftSibling(pTempNode);
		pChild=pNode->getChild(nChildNum-1);
		pChild->setRightSibling(pTempNode);

		pLeftChild = pNode->getChild(STARTNUM);
		for (i=STARTNUM+1; i<nChildNum; i++)
		{
			pRightChild = pNode->getChild(i);
			pRightChild->setLeftSibling(pLeftChild);
			pLeftChild = pRightChild;
		}
		pRightChild = pNode->getChild(nChildNum-1);
		for (i=nChildNum-2; i>=STARTNUM; i--)
		{
			pLeftChild = pNode->getChild(i);
			pLeftChild->setRightSibling(pRightChild);
			pRightChild = pLeftChild;
		}
		for (i=STARTNUM; i<nChildNum; i++)
		{
			pTempNode = pNode->getChild(i);
			setSibling(pTempNode,nLevel+1);
		}
	}
}

void ManagerNode::setNodeCode()
{
	DecisionTreeNode *pRootNode = getRoot();
	pRootNode->setNodeCode("R");
	pRootNode->numTH = 0; 
	setNodeCode(pRootNode);
}

void ManagerNode::setNodeCode(DecisionTreeNode *pNode)
{
	int i = 0, nChildNum = 0;
	CTString szTemp, szNodeCode;

	nChildNum = pNode->getChildNum();
	szNodeCode = pNode->getNodeCode();
	if( nChildNum < 1) return;
	for(i=0; i<nChildNum; i++)
	{
		szTemp.Format("%d",i);
		while (szTemp.GetLength() < 3)
			szTemp = "0" + szTemp;
		pNode->getChild(i)->setNodeCode(szNodeCode + szTemp.GetData());
		pNode->getChild(i)->numTH = i; 
		setNodeCode(pNode->getChild(i));
	}
}

void ManagerNode::moveChild(DecisionTreeNode *pNode, int nInterval)
{
	int i=0, nChildNum=0;
	DecisionTreeNode *pTempNode = NULL;

	nChildNum = pNode->getChildNum();
	int nXCool = pNode->getXRelativeCoor();
	int nYCool = pNode->getYRelativeCoor();
	nXCool = nXCool - nInterval;
	pNode->setRelativeCoor(nXCool, nYCool);
	if (nChildNum < 1) return;
	for(i=0; i<nChildNum; i++)
	{
		pTempNode = pNode->getChild(i);
		moveChild(pTempNode,nInterval);
	}
}

void ManagerNode::getAncestorSibling(DecisionTreeNode *pNode, CTString szNodeCode, int nInterval)
{
	int nLength = 0,nChildNum = 0;
	CTString szTempCode,szCode,szLabel;
	
	nChildNum  = pNode->numTH;
	szTempCode = pNode->getNodeCode();

	DecisionTreeNode *pParentNode = pNode->getParent();
	szLabel = pParentNode->getNodeLabel();
	
	if(nChildNum >= 10)
		nLength = szTempCode.GetLength() - 2;
	else
		nLength = szTempCode.GetLength() - 1;
	szTempCode = szTempCode.Left(nLength);
	szCode = szNodeCode.Left(nLength);
	if(szCode == szTempCode)
	{
		moveChild(pNode,nInterval);
		ReCalcParentX(pNode);
	}
	else
	{
		DecisionTreeNode *pTempNode = pNode->getParent();
		if (pTempNode != NULL)
		{
			if ( !(pTempNode->isRoot) )
			{
				getAncestorSibling(pNode->getParent(),szNodeCode,nInterval);
			}
		}
	}
}

void ManagerNode::setNodeSize(DecisionTreeNode *pNode)
{
	CTString szLabel;
	int i, nChildNum, nWidth=0, nHeight=0;
	DecisionTreeNode *pTempNode = NULL;
	
	szLabel   = pNode->getNodeLabel(); //可能使用getNodeID
	nChildNum = pNode->getChildNum();
	
	nWidth  = m_bFixWidth ? 2*FIX_WIDTH : (strlen(szLabel)+2)*WIDTH_X;
	nHeight = HEIGHT_Y;
	pNode->setWidth (nWidth);
	pNode->setHeight (nHeight);
	if (nChildNum < 1) return;
	for (i=0; i<nChildNum; i++)
	{
		pTempNode = pNode->getChild(i);
		setNodeSize(pTempNode);
	}	
}

int ManagerNode::GetTreeHeight()
{
	int nSize=0, nHeight=0;
	DecisionTreeNode *pNode = NULL;

	nSize = m_pNodeList->GetSize();
	for (int i=0; i<nSize; i++)
	{
		pNode = (DecisionTreeNode*)m_pNodeList->GetAt(i);
		if ((pNode->getHeight() + pNode->getYAbsolateCoor()) > nHeight)
			nHeight = pNode->getHeight () + pNode->getYAbsolateCoor();
	}
	return nHeight;	
}

int ManagerNode::GetTreeWidth()
{
	int nSize=0, nWidth=0;
	DecisionTreeNode *pNode = NULL;

	nSize = m_pNodeList->GetSize();
	for (int i=0;i<nSize;i++)
	{
		pNode = (DecisionTreeNode*)m_pNodeList->GetAt(i);
		if ((pNode->getWidth() + pNode->getXAbsolateCoor()) > nWidth)
			nWidth = pNode->getWidth () + pNode->getXAbsolateCoor();
	}
	return nWidth;	
	
}

void ManagerNode::CalcBlurX(DecisionTreeNode *pNode, int nCoorX, int nCoorY)
{
	int i=0, nChildNum=0;
	int nCoor_X=0,nCoor_Y=0,nWidth=0,nParentHeight=0;
	DecisionTreeNode *pTempNode = NULL;
	
	nChildNum     = pNode->getChildNum ();
	nParentHeight = pNode->getHeight ();
	nCoor_Y = nCoorY + nParentHeight + SUBTREE_INTERVAL;
	if (nChildNum > 0)
	{
		pTempNode = pNode->getChild(0);
		nWidth = nChildNum * (pTempNode->getWidth()) + (nChildNum -1)*SIBLING_INTERVAL;
		nCoor_X = nCoorX - (nWidth - pNode->getWidth())/2;
		for (i=0; i<nChildNum; i++)
		{
			pTempNode=pNode->getChild(i);
			if (i>0)
			{
				nCoor_X = nCoor_X + pTempNode->getWidth() + SIBLING_INTERVAL;
			}
			CalcBlurX(pTempNode,nCoor_X,nCoor_Y);
		}
		nWidth= (pNode->getChild(nChildNum-1)->getXRelativeCoor()) - (pNode->getChild(0)->getXRelativeCoor()); 
		nWidth = nWidth + (pNode->getChild (nChildNum-1)->getWidth());
		nCoor_X = (pNode->getChild(0)->getXRelativeCoor()) + nWidth/2 - pNode->getWidth()/2 ;
		
		pNode->setRelativeCoor(nCoor_X,nCoorY);
	}
	else
	{
		pNode->setRelativeCoor(nCoorX,nCoorY);
	}
}

void ManagerNode::ReCalcParentX(DecisionTreeNode *pNode)
{
	CTString szLabel;
	int nLevel=0, nChildNum=0;
	int nCoor_X=0, nCoor_Y=0, nWidth=0;
	DecisionTreeNode *pParentNode = NULL;

	nLevel = pNode->getLevel();
	if (nLevel > 1)
	{
		pParentNode = pNode->getParent();
		nChildNum = pParentNode->getChildNum();
		nWidth = pParentNode->getChild(nChildNum-1)->getXRelativeCoor();
		nWidth = nWidth - pParentNode->getChild(0)->getXRelativeCoor();
		nWidth = nWidth + pParentNode->getChild(nChildNum-1)->getWidth();
		nCoor_X = (pParentNode->getChild(0)->getXRelativeCoor()) + nWidth/2 - pParentNode->getWidth()/2;
		nCoor_Y = pParentNode->getYRelativeCoor();
		pParentNode->setRelativeCoor(nCoor_X,nCoor_Y);
		ReCalcParentX(pParentNode);
	}
}

void ManagerNode::OverlapProcess(DecisionTreeNode *pNode)
{
	int nNumth,nTempNumth;
	int i,nChildNum,nLevel,nTotalNum;
	int nCoor_X,nCoor_tempX,nWidth,nNeedWidth;
	CTString szCode,szTempCode,szCode1,szTempCode1;
	DecisionTreeNode *pTempNode = NULL;
	DecisionTreeNode *pChildNode= NULL;
	
	nChildNum = pNode->getChildNum();
	if (nChildNum > 0)
	{
		for(i=nChildNum-1; i>=0; i--)
		{
			pChildNode = pNode->getChild(i);
			OverlapProcess(pChildNode);
		}
	}
	nLevel = pNode->getLevel();
	szCode = pNode->getNodeCode();
	nNumth = pNode->numTH ;
	if(nNumth >= 10)
		szCode1 = szCode.Left(szCode.GetLength()-2);  
	else
		szCode1 = szCode.Left(szCode.GetLength()-1);  
	nCoor_X = pNode->getXRelativeCoor();
	nWidth = pNode->getWidth ();
	nTotalNum = m_pNodeList->GetSize();
	for (i=0; i<nTotalNum; i++)
	{
		pTempNode = (DecisionTreeNode*)m_pNodeList->GetAt(i);
		szTempCode = pTempNode->getNodeCode();
		nTempNumth = pTempNode->numTH; 
		if(nTempNumth >= 10)
			szTempCode1 = szTempCode.Left(szTempCode.GetLength()-2 ); 
		else
			szTempCode1 = szTempCode.Left(szTempCode.GetLength()-1 );
		
		if ((pTempNode->getLevel () == nLevel) && (szTempCode < szCode))
		{
			nCoor_tempX = pTempNode->getXRelativeCoor();
			nNeedWidth=pTempNode->getWidth()+SIBLING_INTERVAL;
			if (nCoor_X < (nCoor_tempX + nNeedWidth ))
			{
				getAncestorSibling(pTempNode,szCode, nCoor_tempX + nNeedWidth - nCoor_X);
			}
		}
	}
}

void ManagerNode::setAbsolateCoor()
{
	int nCoor_MinX=0,nTempX=0;
	int i, nTotalNum, nNeedAdd;
	int nCoor_AbsX,nCoor_AbxY;
	DecisionTreeNode *pNode = NULL;

	nTotalNum = m_pNodeList->GetSize();
	for(i=0; i<nTotalNum; i++)
	{
		pNode = (DecisionTreeNode *)m_pNodeList->GetAt(i);
		nTempX = pNode->getXRelativeCoor ();
		if (nTempX < nCoor_MinX)
		{
			nCoor_MinX = nTempX;
		}
	}
	nNeedAdd = 0 - nCoor_MinX + LEFT_XCOOR;
	for(i=0; i<nTotalNum; i++)
	{
		pNode = (DecisionTreeNode *)m_pNodeList->GetAt(i);
		nCoor_AbsX = pNode->getXRelativeCoor() + nNeedAdd;
		nCoor_AbxY = pNode->getYRelativeCoor();
		pNode->setAbsolateCoor(nCoor_AbsX,nCoor_AbxY);
	}
}

void ManagerNode::Recursion(DecisionTreeNode *pParentNode)
{
	int nFirst, nNum;
	int nWidth, nHeight;
	int nCoor_X, nCoor_Y;
	int nStart_X, nStart_Y, nEnd_X, nEnd_Y;
	int nStart_ArcX, nStart_ArcY, nEnd_ArcX, nEnd_ArcY;
	
	VRect vrc;
	VPoint vp;
	SIZE size;
	CTString szLabel,szRoot,szArcLabel;
	DecisionTreeNode *pNode = NULL;
	DecisionTreeNode *pChildNode = NULL;
	
	//计算带框的矩形及文字
	size.cx = 2;
	size.cy = 2;
	pNode = pParentNode;	
	szLabel = pNode->getNodeLabel();
	nWidth  = pNode->getWidth();
	nHeight = pNode->getHeight();
	nCoor_X = pNode->getXAbsolateCoor();
	nCoor_Y = pNode->getYAbsolateCoor();

	//添加带边框的矩形及文字
	vrc.l = nCoor_X;
	vrc.t = nCoor_Y;
	vrc.r = nCoor_X + nWidth;
	vrc.b = nCoor_Y + nHeight;
	vrc.szLabel = szLabel;
	m_RectangleList.Add(vrc);
	
	//计算分支
	nNum = pNode->arcNum;
	nStart_X = pNode->getXAbsolateCoor()+pNode->getWidth()/2;
	nStart_Y = pNode->getYAbsolateCoor()+pNode->getHeight();
	for (nFirst=0; nFirst<nNum; nFirst++)
	{
		pChildNode = pNode->getChild(nFirst);
		
		nEnd_X   = pChildNode->getXAbsolateCoor()+pChildNode->getWidth()/2;
		nEnd_Y   = pChildNode->getYAbsolateCoor();
		//计算不带边框的矩形及文字
		szArcLabel = pNode->getArcLabel(nFirst);
		int nW = m_bFixWidth ? FIX_WIDTH : (strlen(szArcLabel)*WIDTH_X*1.2);
		int nH = HEIGHT_Y;
		nStart_ArcX = nEnd_X - nW/2;
		nStart_ArcY = nEnd_Y - (nEnd_Y-nStart_Y)*HEIGHT_B/2 - nH/2;
		nEnd_ArcX = nStart_ArcX + nW;
		nEnd_ArcY = nStart_ArcY + nH;
		//添加不带边框的矩形及文字
		vrc.l = nStart_ArcX;
		vrc.t = nStart_ArcY;
		vrc.r = nEnd_ArcX;
		vrc.b = nEnd_ArcY;
		vrc.szLabel = szArcLabel;
		m_RectFillList.Add(vrc);
		//添加点－线
		if (nFirst == 0)
		{
			vp.x = nStart_X;
			vp.y = nStart_Y;
			m_PointList.Add(vp); 
			vp.x = nStart_X;
			vp.y = nStart_Y + (nEnd_Y - nStart_Y)*HEIGHT_T;
			m_PointList.Add(vp);
		}
		{
			vp.x = nStart_X;
			vp.y = nStart_Y + (nEnd_Y - nStart_Y)*HEIGHT_T;
			m_PointList.Add(vp);
			vp.x = nEnd_X;
			vp.y = nStart_Y + (nEnd_Y - nStart_Y)*HEIGHT_T;
			m_PointList.Add(vp);
		}
		{
			vp.x = nEnd_X;
			vp.y = nStart_Y + (nEnd_Y - nStart_Y)*HEIGHT_T;
			m_PointList.Add(vp);
			vp.x = nEnd_X;
			vp.y = nStart_ArcY;
			m_PointList.Add(vp);
		}
		{			
			vp.x = nEnd_X;
			vp.y = nEnd_ArcY;
			m_PointList.Add(vp);
			vp.x = nEnd_X;
			vp.y = nEnd_Y;
			m_PointList.Add(vp);
		}
		//递归
		Recursion(pChildNode);
	}
	/*
	for (nFirst=0; nFirst<nNum; nFirst++)
	{
		pChildNode = pNode->getChild(nFirst);

		nEnd_X   = pChildNode->getXAbsolateCoor()+pChildNode->getWidth()/2;
		nEnd_Y   = pChildNode->getYAbsolateCoor();
		//添加点－线
		vp.x = nStart_X;
		vp.y = nStart_Y;
		m_PointList.Add(vp); 
		vp.x = nEnd_X;
		vp.y = nEnd_Y;
		m_PointList.Add(vp); 
		
		//计算不带边框的矩形及文字
		szArcLabel = pNode->getArcLabel(nFirst);
		int nLength = strlen(szArcLabel)*WIDTH_X;
		nStart_ArcX =(nStart_X + nEnd_X)/2 - strlen(szLabel)*WIDTH_X/2;
		nStart_ArcY =(nStart_Y + nEnd_Y)/2 - HEIGHT_Y/2;
		nEnd_ArcX   = nStart_ArcX + strlen(szLabel)*WIDTH_X*1.2;
		nEnd_ArcY   = nStart_ArcY + HEIGHT_Y;
		//添加不带边框的矩形及文字
		if (nLength > 100)
		{
			vrc.l = nStart_ArcX + 20;
		}
		else
		{
			vrc.l = nStart_ArcX - 5;
		}
		vrc.t = nStart_ArcY;
		vrc.r = nEnd_ArcX;
		vrc.b = nEnd_ArcY + 15;
		vrc.szLabel = szArcLabel;
		m_RectFillList.Add(vrc);
		//递归
		Recursion(pChildNode);
	}
	*/
}

void ManagerNode::CreateTree()
{
	Clear();                 //清空
	setSibling();            //左右兄弟的指向
	setNodeCode();           //建立每个节点的NodeCode(节点ID)，确定位置使用	
	DecisionTreeNode *pRootNode = getRoot(); //取得根节点
	setNodeSize(pRootNode);                  //确定每个节点的大小
	CalcBlurX(pRootNode,ORIGIN_X,ORIGIN_Y);      //初步计算相对坐标值
	OverlapProcess(pRootNode);               //遍历树，防止节点位置重合，移动，完全计算相对位置
	setAbsolateCoor();                       //由相对坐标计算绝对坐标
	Recursion(pRootNode);                    //树的绘制
}

void ManagerNode::deleteNode(DecisionTreeNode *pNode, int nNum)
{
	int i = 0,nChildNum = 0;
	DecisionTreeNode *pTempNode = NULL;

	nChildNum  = pNode->getChildNum();
	if (nChildNum > 0)
	{
		for(i=0;i<nChildNum;i++)
		{
			pTempNode = pNode->getChild(i);
			deleteNode(pTempNode,i);			
		}
		pNode->getParent()->setChild(NULL,nNum);
		pNode->getParent()->setChildNum(pNode->getParent()->getChildNum()-1);
		delete pNode;
		pTempNode = NULL;
	}
	else
	{
		pNode->getParent()->setChild(NULL,nNum);
		pNode->getParent()->setChildNum(pNode->getParent()->getChildNum()-1);
		delete pNode;
		pNode = NULL;
	}	
}

int ManagerNode::GetBufLen()
{
	int nLen = 	0;
	int nSize = m_pNodeList->GetSize();
	AddDigitLen(nLen,nSize);
	for(int i=0;i<nSize;i++)
	{
		DecisionTreeNode *pNode =(DecisionTreeNode *)m_pNodeList->GetAt(i);
		AddDigitLen(nLen,pNode->GetBufLen());			
	}
	
	return nLen;
}

//////////////////////////////////////////////////////////////////////
//
// DecisionTreeNode.cpp: implementation of the DecisionTreeNode class.
//
//////////////////////////////////////////////////////////////////////
DecisionTreeNode::DecisionTreeNode()
{
	nodeLabel="";
	parent = NULL;
	isRoot=false;
	level=0;
	position=0; 
	childNum=0;
	nodeCode="";
	arcLabelList = new LISTSTR();//CPtrArray
	childList = new CPtrArray();
}

DecisionTreeNode::DecisionTreeNode(CTString nodeLabel,int arcNum,DecisionTreeNode * parentNode)
{
	//验证nodeLabel的存在
	setNodeLabel(nodeLabel);
	setArcNum(arcNum);
	setParent(parentNode);
	isRoot=false;
	level=0;
	position=0;
	childNum=0;
	nodeCode="";
	arcLabelList= new LISTSTR();
	childList  =new CPtrArray();
}


DecisionTreeNode::~DecisionTreeNode()
{
}

void DecisionTreeNode::Serialize(TCLFile &file)
{
}


void DecisionTreeNode::Print()
{
	if(isRoot)
		printf("为根结点");
	else
	{
		printf("Parent Node Code: %s",parent->nodeCode);
		printf("    Parent Node Label:%s",parent->nodeLabel);
	}
	
	printf("Node Code: %s",nodeCode);
	printf("    Node Label:%s",nodeLabel);
	printf("    Node Position:%d\n",position);
}

void DecisionTreeNode::setLevel(int num)
{
	level=num;
}

int DecisionTreeNode::getLevel()
{
	return level;
}

int DecisionTreeNode::getPosition()
{
	return position;
}

void DecisionTreeNode::setPosition(int num)
{
	position=num;
}

void    DecisionTreeNode::setNodeLabel(CTString label)
{
	nodeLabel= label;
}

CTString DecisionTreeNode::getNodeLabel()
{
	return nodeLabel;
}

void DecisionTreeNode::setParent(DecisionTreeNode * parentNode)
{
	parent=parentNode;
}

DecisionTreeNode * DecisionTreeNode::getParent()
{
	return parent;
}

void  DecisionTreeNode::setArcNum(int num)
{
	childList->SetSize(num);
	arcNum=num;
	
}

int   DecisionTreeNode::getArcNum()
{
	return arcNum;
}

void     DecisionTreeNode::setArcLabel(CTString arcLabel,int arcNum)
{
	arcLabelList->insert(arcLabelList->end(),arcLabel);
}

CTString  DecisionTreeNode::getArcLabel(int num)
{
	int i=0;
	CTString strRn;
	
	for(m_list=arcLabelList->begin();m_list!=arcLabelList->end();++m_list)
	{
		if(i==num)
		{
			strRn=*m_list;
		}
		i++;
	}
	return strRn;
}

void DecisionTreeNode::setChild(DecisionTreeNode * child,int nChildNum)
{
	if(nChildNum<childList->GetSize())
		childList->SetAt(nChildNum,child);
}

DecisionTreeNode *  DecisionTreeNode::getChild(int nChildNum)
{
	DecisionTreeNode* newNode;
	if(nChildNum<childList->GetSize())
		newNode=(DecisionTreeNode *)childList->GetAt(nChildNum);
	else
		newNode=new DecisionTreeNode();
	return newNode;

}

void DecisionTreeNode::setLeftNeighbor(DecisionTreeNode* left)
{
	leftNeighbor=left;
}

DecisionTreeNode*   DecisionTreeNode::getLeftNeighbor()
{
	return leftNeighbor;
	
}

void DecisionTreeNode::setLeftSibling(DecisionTreeNode* left)
{
	leftSibling=left;
	
}

DecisionTreeNode*   DecisionTreeNode::getLeftSibling()
{
	return leftSibling;
	
}

void DecisionTreeNode::setRightSibling(DecisionTreeNode* right)
{
	rightSibling=right;
}

DecisionTreeNode *DecisionTreeNode::getRightSibling()
{
	return rightSibling;
}

void  DecisionTreeNode::setChildNum(int num)
{
	childNum=num;
}

int DecisionTreeNode::getChildNum()
{
	return childNum;
}
void  DecisionTreeNode::setNodeCode(CTString code)
{
	nodeCode=code;
}

CTString  DecisionTreeNode::getNodeCode()
{
	return nodeCode;
}

void  DecisionTreeNode::setWidth(int num)
{
	width=num;
}

int   DecisionTreeNode::getWidth()
{
	return width;
}

void  DecisionTreeNode::setHeight(int num)
{
	height=num;
}

int   DecisionTreeNode::getHeight()
{
	return height;
	
}

void  DecisionTreeNode::setRelativeCoor(int cx,int cy)
{
	rel_XCoorl=cx;
	rel_YCoorl=cy;
	
}

void  DecisionTreeNode::setRelativeCoor(SIZE size)
{
	rel_XCoorl=size.cx;
	rel_YCoorl=size.cy;
}

int   DecisionTreeNode::getXRelativeCoor()
{
	return rel_XCoorl;
}

int   DecisionTreeNode::getYRelativeCoor()
{
	return rel_YCoorl;
}

SIZE  DecisionTreeNode::getRelativeCoor()
{
	SIZE size;
	size.cx=rel_XCoorl;
	size.cy=rel_YCoorl;
	return size;
}

void  DecisionTreeNode::setAbsolateCoor(int cx,int cy)
{
	abs_XCoorl=cx;
	abs_YCoorl=cy;
}

void  DecisionTreeNode::setAbsolateCoor(SIZE size)
{
	abs_XCoorl=size.cx;
	abs_YCoorl=size.cy;
	
}

int   DecisionTreeNode::getXAbsolateCoor()
{
	return abs_XCoorl;
}

int   DecisionTreeNode::getYAbsolateCoor()
{
	return abs_YCoorl;
}

SIZE  DecisionTreeNode::getAbsolateCoor()
{
	SIZE size;
	size.cx=abs_XCoorl;
	size.cy=abs_YCoorl;
	return size;
}

/*
	bool				isRoot;
	int					level;       //节点的级数
	int					position;   //1：左子树，2:右子树,0：其他类型
	int					nChildNum;
	int					numTH;     //父亲的第几个孩子，从0开始计数
CTString            nodeLabel;             //节点的Label信息
	int                 arcNum ;            //子节点个数
	DecisionTreeNode*   parent;                //父节点
CPtrArray  *        childList;
	DecisionTreeNode*   leftNeighbor;          //左邻居  
	DecisionTreeNode*   leftSibling;
	DecisionTreeNode*   rightSibling;         //左右兄弟
	CTString            nodeCode;             //节点内码，父节点内码加第几个孩子。
LISTSTR*			arcLabelList;  //是链表
*/
void DecisionTreeNode::Serialize(unsigned char *&pBuf)
{
	SaveString(pBuf,nodeLabel);

	int nSize = arcLabelList->size();
	SaveDigit(pBuf,nSize);
	int i=0;
	for(i=0;i<nSize ;i++)
	{
		CTString str = arcLabelList->at(i);
		SaveString(pBuf,str);
	}

	
	nSize = childList->GetSize();
	SaveDigit(pBuf,nSize);
	for(i=0; i< nSize;i++)
	{
		DecisionTreeNode* node =(DecisionTreeNode*)childList->GetAt(i);
		node->Serialize(pBuf);
	}
}

int DecisionTreeNode::GetBufLen()
{
	int nLen =0;
	AddStringLen(nLen,nodeLabel);

	int nSize = arcLabelList->size();
	AddDigitLen(nLen,nSize);
	int i=0;
	for(i=0;i<nSize ;i++)
	{
		CTString str = arcLabelList->at(i);
		AddStringLen(nLen,str);
	}


	nSize = childList->GetSize();
	AddDigitLen(nLen,nSize);
	for( i=0; i< nSize;i++)
	{
		DecisionTreeNode* node =(DecisionTreeNode*)childList->GetAt(i);
		AddDigitLen(nLen,node->GetBufLen());
	}
	return nLen;
}
