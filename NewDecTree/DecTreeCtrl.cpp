// DecTreeCtrl.cpp: implementation of the CDecTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DecTreeCtrl.h"

//#include "DecTreePropDlg.h"	// Added by ClassView
#include "DecTree.h"	// Added by ClassView
#include "DecTreeThread.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDecTreeCtrl::CDecTreeCtrl()
{
	this->m_pDecTree = new CDecTree;
	//m_DecTreeProp.SetDecTree(m_pDecTree); -by pyx 2006-02-15
}

CDecTreeCtrl::CDecTreeCtrl(UINT nID)
{
	this->m_pDecTree = new CDecTree;
	//m_DecTreeProp.SetDecTree(m_pDecTree); -by pyx 2006-02-15
	m_nID=nID;
}

CDecTreeCtrl::~CDecTreeCtrl()
{
	if (m_pDecTree != NULL)
		delete m_pDecTree;
	m_pDecTree = NULL;
	//m_fields.RemoveAll();
}

void CDecTreeCtrl::Run(CDataInterface *pDataInterface, CPtrArray *fields)
//void CDecTreeCtrl::Run(CDataInterface *pDataInterface, CPtrArray *fields ,CWnd *pMsgWnd,CWnd *pPrgWnd)
{
 //	CDecTreeThread	*pDecTreeRunner;
 	/*CTString strTmp;
 	int nSize=fields->GetSize();
 	m_fields.RemoveAll();
 	for (int i =0;i<nSize;i++)
 	{
 		strTmp=fields->GetAt(i);
 		m_fields.Add(strTmp);
 	}*/

 	//m_pDecTreeRunner=new CDecTreeThread(&m_DecTreeProp,pDataInterface,fields,pMsgWnd,pPrgWnd);  -by pyx 2006-02-15
	//m_pDecTreeRunner=new CDecTreeThread();	-by pyx 2006-02-17
 	m_pDecTreeRunner->m_nID=m_nID;
	//m_pDecTreeRunner->m_bAutoDelete=TRUE;	-by pyx 2006-02-17
	//m_pDecTreeRunner->CreateThread();	-by pyx 2006-02-17
 
}







void CDecTreeCtrl::Stop()
{
	if(m_pDecTreeRunner!=NULL)
		m_pDecTreeRunner->Stop();
}
