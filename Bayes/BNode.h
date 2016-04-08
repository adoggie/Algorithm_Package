// BNode.h: interface for the CBNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BNODE_H__CB580A26_82EB_47C6_A6D9_9C8B843B5902__INCLUDED_)
#define AFX_BNODE_H__CB580A26_82EB_47C6_A6D9_9C8B843B5902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "stdafx.h"
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../TLADVN/Analyze.h"

typedef struct Nodeparnt
{
	int			ParntNum;//父节点个数
	CIntVector	    ParntIDVector;//父节点ID号列表
}Nodeparnt;

//记录状态标签(全部转换为字符型)列表
class CStateLable
{
	CTStringArray	m_StateArray;
public:
	int add(double dLable)//读入浮点型，尝试增加新的标签，若无重复则增加；并返回下标index
	{
		int i;
		CTString str;
		int size = m_StateArray.GetSize();
		str.Format("%f",dLable);
		for (i=0;i<size;i++)
		{
			if (str == m_StateArray[i])
				return i;
		}
		m_StateArray.InsertAt(size,str,1);
		return size;
	}
	int add(int iLable)//读入整型，尝试增加新的标签
	{
		int i;
		CTString str;
		int size = m_StateArray.GetSize();
		str.Format("%d",iLable);
		for (i=0;i<size;i++)
		{
			if (str == m_StateArray[i])
				return i;
		}
		m_StateArray.InsertAt(size,str,1);
		return size;
	}				
	int add(bool bLable)//读入布尔型，尝试增加新的标签
	{
		int i;
		CTString str;
		int size = m_StateArray.GetSize();
		if (bLable)
			str = "True";
		else
			str = "False";
		for (i=0;i<size;i++)
		{
			if (str == m_StateArray[i])
				return i;
		}
		m_StateArray.InsertAt(size,str,1);
		return size;
	}
	int add(CTString sLable)//读入字符型，尝试增加新的标签
	{
		int i;
		int size = m_StateArray.GetSize();			
		for (i=0;i<size;i++)
		{
			if (sLable == m_StateArray[i])
				return i;
		}
		m_StateArray.InsertAt(size,sLable,1);
		return size;
	}
	CTString GetLableByIndex(int iIndex)//根据下标返回标签字符串
	{
		return m_StateArray[iIndex];
	}
	int GetIndex(CTString sLable)//根据标签字符串返回下标
	{
		int i;
		int size =  m_StateArray.GetSize();
		for (i=0;i<size;i++)
			if (sLable == m_StateArray[i])
				return i;
			return -1;
	}
	int GetLableCount()//返回标签个数
	{
		return m_StateArray.GetSize();
	}
	void RemoveAll()//清空标签列表
	{
		 m_StateArray.RemoveAll();
	}
};
class CBNode  
{

public:
	CBNode();
	CBNode(int id,CTString name,int sn);
	CBNode(int mNodeID,CTString name,int sn,CDoubleMatrix cpt);

//	CBNode(int mNodeID,CTString name,int sn,CIntVector state,CDoubleMatrix cpt,CIntMatrix ct);
	virtual ~CBNode();

	//CStringList	m_StateList;		//节点状态标签	
	CStateLable	m_StateLable;		//状态标签列表 2005-11-28
private:
	int			m_nNodeID;       //节点的ID号，取非负整数
	CTString	m_NodeName;
	int			m_nStateNum;     //节点状态个数
	//CIntVector	m_State;            //状态取值
	CDoubleMatrix	m_CPT;
	int			m_ParntNum;			//父节点个数
	CIntVector	m_ParntVector;		//父节点ID号向量

public:
	void setID(int mNodeID);
	void setName(CTString NodeName);
	void setStateNum(int sn);
	//void setState(CIntVector State);
	void setCPT(CDoubleMatrix cpt);
	void setParntNum(int parntNum);
	void setParntVector(CIntVector parntVector);
	int  getID();
	CTString getName();
	int  getStateNum();
	//CIntVector getState();
	CDoubleMatrix getCPT();
	int getParntNum();
	CIntVector getParntVector();	
};

#endif // !defined(AFX_BNODE_H__CB580A26_82EB_47C6_A6D9_9C8B843B5902__INCLUDED_)
