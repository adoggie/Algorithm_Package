#if !defined(AFX_BLURRYINTERFACE_H__E25CC968_81D0_40EB_BD41_75CA4F32E203__INCLUDED_)
#define AFX_BLURRYINTERFACE_H__E25CC968_81D0_40EB_BD41_75CA4F32E203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlurryInterface.h : header file
//

#include "BlurryCluster.h"

class CBlurryCluster;

/////////////////////////////////////////////////////////////////////////////
// CBlurryInterface command target


class AFX_EXT_CLASS CBlurryInterface//  : public CObject
{
public:
	CBlurryInterface::CBlurryInterface(bool bFirstRun=true);
	virtual ~CBlurryInterface();
	void CBlurryInterface::SetAbortAttib();


// Attributes
public:
	double m_dLambda;
	int m_nSelect;
	CDataInterface *m_pDataInterface;
	CPtrArray *m_pPtrArray;
	CBlurryCluster *m_pBlurryCluster;

// Operations
public:
	bool	SetDataParam(CBlurryVO *pVO);
	void	SetDataSource(CDataInterface *pDataInterface,CPtrArray *pRolePtrArray);
	CResult * RunTrain(CDataInterface *pDataInterface,CEnvParam &pEnv, Interaction *&itt,CBlurryVO *pVO);
	CResult * Run(CDataInterface *pDataInterface,CPtrArray *pPtrArray);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLURRYINTERFACE_H__E25CC968_81D0_40EB_BD41_75CA4F32E203__INCLUDED_)
