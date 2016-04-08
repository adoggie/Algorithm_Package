#if !defined(AFX_DECTREETHREAD_H__FBFF19F9_213F_462B_B6C6_7CBCCD373217__INCLUDED_)
#define AFX_DECTREETHREAD_H__FBFF19F9_213F_462B_B6C6_7CBCCD373217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../arithInterface/EnvParam.h"
#include "DecTree.h"

// DecTreeThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDecTreeThread thread
class CTreeNode;
class CDecTreeThread//	-by pyx 2006-02-16 : public CWinThread
{
	//DECLARE_DYNCREATE(CDecTreeThread)
public:
	CDecTreeThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CDataInterface *m_pDataInterface;
	void PrintTreeIf(CTreeNode * pNode,CTStringArray &strRuleAry);
	CPtrArray *m_pFields;
	UINT m_nID;
	CDecTree *m_pDecTree;
// Operations
	CPtrArray* bridge(CTreeNode * m_node);
	void    bridgeNode(CTreeNode * m_node,DecisionTreeNode * parentNode,int num,CPtrArray* pNodeList);
	int		GetTotalNodeModel(CPtrArray& arNode);
	void    GetNodeModel(CTreeNode *pNode,CPtrArray &arNode);
		
public:
	void Stop();
	public:
	BOOL InitInstance(CDecTreeOutput& Out);
	CResult* RunTrain(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,CDecTreeVO *pVO, CDecTreeOutput *&pOutStruc);
	CResult* RunPred(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt,TPredVO *pVO, CDecTreeOutput *pOutStruc);

public:
	virtual ~CDecTreeThread();
private:
	bool UpdateField(TPredVO *pVO, CDecTreeOutput *pOutStruc);
private:
	bool m_bStop;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECTREETHREAD_H__FBFF19F9_213F_462B_B6C6_7CBCCD373217__INCLUDED_)
