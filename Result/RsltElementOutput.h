// RsltElementOutput.h: interface for the CRsltElementOutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSLTELEMENTOUTPUT_H__BCD50F09_21DA_4EB9_A736_B2D4103983EB__INCLUDED_)
#define AFX_RSLTELEMENTOUTPUT_H__BCD50F09_21DA_4EB9_A736_B2D4103983EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Result.h"
#include "Output.h"

class AFX_EXT_CLASS CRsltElementOutput : public CRsltVariable  
{
public:
	CRsltElementOutput();
	virtual ~CRsltElementOutput();
	
	void Serialize(unsigned char *&pBuf);
	int GetBufLen();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	CRsltElementOutput(CTString strName,COutput* pOutput);
public:
	COutput *m_pOutput;
};

#endif // !defined(AFX_RSLTELEMENTOUTPUT_H__BCD50F09_21DA_4EB9_A736_B2D4103983EB__INCLUDED_)
