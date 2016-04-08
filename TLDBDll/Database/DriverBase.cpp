// DriverBase.cpp: implementation of the CDriverBase class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
#include "DriverBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDriverBase::CDriverBase()
{
	m_nRecordCount = 0;
}

CDriverBase::~CDriverBase()
{

}

int CDriverBase::GetRecordCount()
{
	return m_nRecordCount;
}

int CDriverBase::GetFileVersion()
{
	return 0;
}
