// MultiDimRule.h: interface for the CMultiDimRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIDIMRULE_H__C97461CA_FCBF_4664_98FC_4596E8544682__INCLUDED_)
#define AFX_MULTIDIMRULE_H__C97461CA_FCBF_4664_98FC_4596E8544682__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FileAssociationRule.h"

class CMultiDimRule : public CFileAssociationRule  
{
public:
	CMultiDimRule();
	virtual ~CMultiDimRule();

	CPtrArray m_arInputArray;
	CPtrArray m_arTargetArray;

	bool IsTarget(int Index);
	void CreateFrequentSet(CRuleTrainVO * pt,bool bString);
	CResult* CreateRule(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt);
	CResult* Predict(CDataInterface *pDataInterface,CEnvParam *pEnv, Interaction *itt);

	int MergeDims(CDataInterface *pDataInterface,CPtrArray* inputs,TDColAry &CustomArray,TDColAry &ProductArray,int &transCount);
	void	SetDataParam(CRuleTrainVO	*pVO);	
	void	SetDataInterface(CDataInterface *pDataInterface, CPtrArray *parField);
	void WritePtrData(CDataInterface* pDataInterface,CPtrArray* inputs,CPtrArray *par, char *pszTitle,bool *pStop,CResult * pResult);
	void Confidence(CPtrArray* par,CResult * pResult,bool * pbStop,int transCount,bool bSave);
};

#endif // !defined(AFX_MULTIDIMRULE_H__C97461CA_FCBF_4664_98FC_4596E8544682__INCLUDED_)
