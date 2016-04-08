// BPNetInterface.h: interface for the CBPNetInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPNETINTERFACE_H__4966CE91_E607_4CF5_B999_98DF97BCC9EF__INCLUDED_)
#define AFX_BPNETINTERFACE_H__4966CE91_E607_4CF5_B999_98DF97BCC9EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BPNet.h"
#include "../Public/TString.h"
#define  DLLEXPORT	__declspec(dllexport)

class DLLEXPORT CBPNetInterface // : public CObject  
{
private:
	int nTopologyType;//¿‡–Õ
	CBPNet *m_pNeuralNet;

public:
	CBPNetInterface();
	virtual ~CBPNetInterface();

	void Abort();
// 	bool	SetDataParam(TBPNetVO *pVO);
// 	bool SetDataSource(  CDataInterface * pData, CPtrArray& arFieldRole);
// 	CResult * Run();
};

#endif // !defined(AFX_BPNETINTERFACE_H__4966CE91_E607_4CF5_B999_98DF97BCC9EF__INCLUDED_)
