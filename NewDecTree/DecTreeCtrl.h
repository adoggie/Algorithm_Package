// DecTreeCtrl.h: interface for the CDecTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////
#include "DecTree.h"

#if !defined(AFX_DECTREECTRL_H__F9EDE55F_44B2_4557_8672_C45291F842BD__INCLUDED_)
#define AFX_DECTREECTRL_H__F9EDE55F_44B2_4557_8672_C45291F842BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTreeNode;
class CDecTreeThread;
class AFX_EXT_CLASS CDecTreeCtrl  
{
public:
	void Stop();
	CDecTree  *m_pDecTree;
	//CDecTreePropDlg m_DecTreeProp; -by pyx 2006-02-15
	CDataInterface *m_pDataInterface;
	CDecTreeCtrl();
	CDecTreeCtrl(UINT nID);
	virtual ~CDecTreeCtrl();
	//void Run(CDataInterface *pDataInterface, CPtrArray *fields ,CWnd *pMsgWnd,CWnd *pPrgWnd);void Run(CDataInterface *pDataInterface, CPtrArray *fields ,CWnd *pMsgWnd,CWnd *pPrgWnd);
	
	void Run(CDataInterface *pDataInterface, CPtrArray *fields);
	

	UINT m_nID;
	//CStringArray m_fields;
	CDecTreeThread	*m_pDecTreeRunner;
};

#endif // !defined(AFX_DECTREECTRL_H__F9EDE55F_44B2_4557_8672_C45291F842BD__INCLUDED_)
