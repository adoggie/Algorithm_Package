// BNode.cpp: implementation of the CBNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BNode.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBNode::CBNode()
{
	m_nNodeID=-1;		//节点ID号
	//m_NodeName=NULL;	//节点名称
	m_nStateNum=0;		//状态数目
	//m_State=CIntVector();	//状态取值
	m_StateLable.RemoveAll();
	//m_StateList.RemoveAll();	//清空状态标签链表
	m_CPT=CDoubleMatrix(0);	//条件概率表
	m_ParntNum=0;	//父节点数目
	m_ParntVector=CIntVector();	//父节点列表（向量）

}
CBNode::CBNode(int id,CTString name,int sn)
{
	m_nNodeID=id;
	m_NodeName=name;
	m_nStateNum=sn;
	m_CPT=CDoubleMatrix(0);
	m_ParntNum=0;
	m_ParntVector=CIntVector();
}
CBNode::CBNode(int mNodeID,CTString name,int sn,CDoubleMatrix cpt)
{
	m_nNodeID=mNodeID;
	m_NodeName=name;
	m_nStateNum=sn;	
	m_CPT=cpt;
	m_ParntNum=0;
	m_ParntVector=CIntVector();

}
//CBNode(int mNodeID,CTString name,int sn,CIntVector state,CDoubleMatrix cpt,CIntMatrix ct)
//{
//	m_nNodeID=id;
//	m_NodeName=name;
//	m_nStateNum=sn;
//	m_State=state;		
//	m_CPT=cpt;
//	setParnt(ct);
//}

CBNode::~CBNode()
{	
	m_StateLable.RemoveAll();
	//m_StateList.RemoveAll();	//清空状态标签链表
}
void CBNode::setID(int mNodeID)
{
	m_nNodeID=mNodeID;
}

void CBNode::setName(CTString NodeName)
{
	m_NodeName=NodeName;
}

void CBNode::setStateNum(int sn)
{
	m_nStateNum=sn;
}

//void CBNode::setState(CIntVector State)
//{
//	m_State=State;
//}

void CBNode::setCPT(CDoubleMatrix cpt)
{
	m_CPT=cpt;
}

void CBNode::setParntNum(int parntNum)
{
	m_ParntNum = parntNum;
}

void CBNode::setParntVector(CIntVector parntVector)
{
	m_ParntVector = parntVector;
}

int  CBNode::getID()
{
	return m_nNodeID;
}

CTString CBNode::getName()
{
	return m_NodeName;
}

int  CBNode::getStateNum()
{
	return m_nStateNum;
}

//CIntVector CBNode::getState()
//{
//	return m_State;
//}

CDoubleMatrix CBNode::getCPT()
{
	return m_CPT;
}

int CBNode::getParntNum()
{
	return m_ParntNum;
}

CIntVector CBNode::getParntVector()
{
	return m_ParntVector;
}




















