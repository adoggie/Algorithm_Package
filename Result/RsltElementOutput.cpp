// RsltElementOutput.cpp: implementation of the CRsltElementOutput class.
//
//////////////////////////////////////////////////////////////////////

#include "RsltElementOutput.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRsltElementOutput::CRsltElementOutput()
{
	m_nType=RSLT_ELEMENTOUTPUT;
	m_pOutput=NULL;
}

CRsltElementOutput::~CRsltElementOutput()
{
	if(m_pOutput)
		delete m_pOutput;
	m_pOutput=NULL;	
}

CRsltElementOutput::CRsltElementOutput(CTString strName, COutput *pOutput)
{
	m_nID=0;
	m_strName=strName;
	m_nType=RSLT_ELEMENTOUTPUT; //��ǰ���������Ϊ��Ԫ�ر���
	m_pOutput=pOutput;

}

void CRsltElementOutput::Serialize(TCLFile &file)
{
	CRsltVariable::Serialize(file);
}

void CRsltElementOutput::Serialize(unsigned char *&pBuf)
{
	CRsltVariable::Serialize(pBuf);
	m_pOutput->Serialize(pBuf); 
}

int CRsltElementOutput::GetBufLen()
{
	int nLen = CRsltVariable::GetBufLen();
	nLen = nLen + m_pOutput->GetBufLen();
	return nLen;
}


void CRsltElementOutput::Print()
{
	CRsltVariable::Print();
	m_pOutput->Print(); 
}