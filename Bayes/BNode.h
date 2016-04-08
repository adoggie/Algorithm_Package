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
	int			ParntNum;//���ڵ����
	CIntVector	    ParntIDVector;//���ڵ�ID���б�
}Nodeparnt;

//��¼״̬��ǩ(ȫ��ת��Ϊ�ַ���)�б�
class CStateLable
{
	CTStringArray	m_StateArray;
public:
	int add(double dLable)//���븡���ͣ����������µı�ǩ�������ظ������ӣ��������±�index
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
	int add(int iLable)//�������ͣ����������µı�ǩ
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
	int add(bool bLable)//���벼���ͣ����������µı�ǩ
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
	int add(CTString sLable)//�����ַ��ͣ����������µı�ǩ
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
	CTString GetLableByIndex(int iIndex)//�����±귵�ر�ǩ�ַ���
	{
		return m_StateArray[iIndex];
	}
	int GetIndex(CTString sLable)//���ݱ�ǩ�ַ��������±�
	{
		int i;
		int size =  m_StateArray.GetSize();
		for (i=0;i<size;i++)
			if (sLable == m_StateArray[i])
				return i;
			return -1;
	}
	int GetLableCount()//���ر�ǩ����
	{
		return m_StateArray.GetSize();
	}
	void RemoveAll()//��ձ�ǩ�б�
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

	//CStringList	m_StateList;		//�ڵ�״̬��ǩ	
	CStateLable	m_StateLable;		//״̬��ǩ�б� 2005-11-28
private:
	int			m_nNodeID;       //�ڵ��ID�ţ�ȡ�Ǹ�����
	CTString	m_NodeName;
	int			m_nStateNum;     //�ڵ�״̬����
	//CIntVector	m_State;            //״̬ȡֵ
	CDoubleMatrix	m_CPT;
	int			m_ParntNum;			//���ڵ����
	CIntVector	m_ParntVector;		//���ڵ�ID������

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
