// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////
#include "Output.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutput::COutput()
{
	m_szName = "";
}

COutput::COutput(CTString szName)
{
	m_szName = szName;
}

COutput::~COutput()
{
	RemoveAll();
}

void COutput::Release()
{
	RemoveAll();
}

int COutput::GetBufLen()
{
	int nLen=0;
	int i=0, nSize=0;

	//名称
	AddStringLen(nLen,m_szName);
	//Int
	nSize = m_vInteger.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputInteger *pInt = m_vInteger.at(i);
		nLen += pInt->GetBufLen();
	}
	//Double
	nSize = m_vDouble.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputDouble *pDouble = m_vDouble.at(i);
		nLen += pDouble->GetBufLen();
	}
	//String
	nSize = m_vString.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputString *pStr = m_vString.at(i);
		nLen += pStr->GetBufLen();
	}
	//StringArray
	nSize = m_vStringArray.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputStringArray *pStrArr = m_vStringArray.at(i);
		nLen += pStrArr->GetBufLen();
	}
	//Vector
	nSize = m_vVector.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputVector *pVec = m_vVector.at(i);
		nLen += pVec->GetBufLen();
	}
	//Matrix
	nSize = m_vMatrix.size();
	AddDigitLen(nLen,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputMatrix *pMatrix = m_vMatrix.at(i);
		nLen += pMatrix->GetBufLen();
	}

	return nLen;
}

void COutput::Serialize(unsigned char *&pBuf)
{
	int i=0, nSize=0;
	
	//名称
	SaveString(pBuf,m_szName);
	//Int
	nSize = m_vInteger.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputInteger *pInt = m_vInteger.at(i);
		pInt->Serialize(pBuf);
	}
	//Double
	nSize = m_vDouble.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputDouble *pDouble = m_vDouble.at(i);
		pDouble->Serialize(pBuf);
	}
	//String
	nSize = m_vString.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputString *pStr = m_vString.at(i);
		pStr->Serialize(pBuf);
	}
	//StringArray
	nSize = m_vStringArray.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputStringArray *pStrArr = m_vStringArray.at(i);
		pStrArr->Serialize(pBuf);
	}
	//Vector
	nSize = m_vVector.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputVector *pVec = m_vVector.at(i);
		pVec->Serialize(pBuf);
	}
	//Matrix
	nSize = m_vMatrix.size();
	SaveDigit(pBuf,nSize);
	for (i=0; i<nSize; i++)
	{
		COutputMatrix *pMatrix = m_vMatrix.at(i);
		pMatrix->Serialize(pBuf);
	}
}

void COutput::Print()
{
	int i=0, j=0, nSize=0;
	
	printf("Output Name is %s\n",m_szName.GetData());
	//Int
	nSize = m_vInteger.size();
	printf("\nInclude %d Integer data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputInteger *pInt = m_vInteger.at(i);
		pInt->Print();
	}
	//Double
	nSize = m_vDouble.size();
	printf("\nInclude %d double data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputDouble *pDouble = m_vDouble.at(i);
		pDouble->Print();
	}
	//String
	nSize = m_vString.size();
	printf("\nInclude %d String data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputString *pStr = m_vString.at(i);
		pStr->Print();
	}
	//StringArray
	nSize = m_vStringArray.size();
	printf("\nInclude %d StringArray data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputStringArray *pStrArr = m_vStringArray.at(i);
		pStrArr->Print();
	}
	//Vector
	nSize = m_vVector.size();
	printf("\nInclude %d Vector data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputVector *pVec = m_vVector.at(i);
		pVec->Print();
	}
	//Matrix
	nSize = m_vMatrix.size();
	printf("\nInclude %d Matrix data.\n",nSize);
	for (i=0; i<nSize; i++)
	{
		COutputMatrix *pMatrix = m_vMatrix.at(i);
		pMatrix->Print();
	}
}

void COutput::SetName(CTString szName)
{
	m_szName = szName;
}

void COutput::Remove(COutput::EKind eKind)
{
	int i=0, nSize=0;
	switch(eKind)
	{
	case N:
		{
			nSize = m_vInteger.size();
			for (i=0; i<nSize; i++)
			{
				COutputInteger *pInteger = m_vInteger.at(i);
				delete pInteger;
				pInteger = NULL;
			}
			m_vInteger.clear();
		}
		break;
	case D:
		{
			nSize = m_vDouble.size();
			for (i=0; i<nSize; i++)
			{
				COutputDouble *pDouble = m_vDouble.at(i);
				delete pDouble;
				pDouble = NULL;
			}
			m_vDouble.clear();
		}
		break;
	case S:
		{
			nSize = m_vString.size();
			for (i=0; i<nSize; i++)
			{
				COutputString *pString = m_vString.at(i);
				delete pString;
				pString = NULL;
			}
			m_vString.clear();
		}
		break;
	case A:
		{
			nSize = m_vStringArray.size();
			for (i=0; i<nSize; i++)
			{
				COutputStringArray *pStrArr = m_vStringArray.at(i);
				delete pStrArr;
				pStrArr = NULL;
			}
			m_vStringArray.clear();
		}
		break;
	case V:
		{
			nSize = m_vVector.size();
			for (i=0; i<nSize; i++)
			{
				COutputVector *pVector = m_vVector.at(i);
				delete pVector;
				pVector = NULL;
			}
			m_vVector.clear();
		}
		break;
	case M:
		{
			nSize = m_vMatrix.size();
			for (i=0; i<nSize; i++)
			{
				COutputMatrix *pMatrix = m_vMatrix.at(i);
				delete pMatrix;
				pMatrix = NULL;
			}
			m_vMatrix.clear();
		}
		break;
	}
}

void COutput::RemoveAll()
{
	int i=0, nSize=0;
	
	//INT
	nSize = m_vInteger.size();
	for (i=0; i<nSize; i++)
	{
		COutputInteger *pInteger = m_vInteger.at(i);
		delete pInteger;
		pInteger = NULL;
	}
	m_vInteger.clear();
	//DOUBLE
	nSize = m_vDouble.size();
	for (i=0; i<nSize; i++)
	{
		COutputDouble *pDouble = m_vDouble.at(i);
		delete pDouble;
		pDouble = NULL;
	}
	m_vDouble.clear();
	//STRING
	nSize = m_vString.size();
	for (i=0; i<nSize; i++)
	{
		COutputString *pString = m_vString.at(i);
		delete pString;
		pString = NULL;
	}
	m_vString.clear();
	//STRINGARRAY
	nSize = m_vStringArray.size();
	for (i=0; i<nSize; i++)
	{
		COutputStringArray *pStrArr = m_vStringArray.at(i);
		delete pStrArr;
		pStrArr = NULL;
	}
	m_vStringArray.clear();
	//VECTOR
	nSize = m_vVector.size();
	for (i=0; i<nSize; i++)
	{
		COutputVector *pVector = m_vVector.at(i);
		delete pVector;
		pVector = NULL;
	}
	m_vVector.clear();
	//MATRIX
	nSize = m_vMatrix.size();
	for (i=0; i<nSize; i++)
	{
		COutputMatrix *pMatrix = m_vMatrix.at(i);
		delete pMatrix;
		pMatrix = NULL;
	}
	m_vMatrix.clear();
}

void COutput::Add(CTString szName,int nValue)
{
	COutputInteger *pInteger = new COutputInteger;
	pInteger->szName = szName;
	pInteger->nValue = nValue;

	m_vInteger.push_back(pInteger);
}

void COutput::Add(CTString szName, double dValue)
{
	COutputDouble *pDouble = new COutputDouble;
	pDouble->szName = szName;
	pDouble->dValue = dValue;
	
	m_vDouble.push_back(pDouble);
}

void COutput::Add(CTString szName, CTString szValue)
{
	COutputString *pString = new COutputString;
	pString->szName = szName;
	pString->szValue = szValue;
	
	m_vString.push_back(pString);
}

void COutput::Add(CTString szName, CTStringArray szValueArray)
{
	COutputStringArray *pStrArr = new COutputStringArray;
	pStrArr->szName = szName;
	pStrArr->szValueArray.Copy(szValueArray);

	m_vStringArray.push_back(pStrArr);
}

void COutput::Add(CTString szName, CDoubleVector &v)
{
	COutputVector *pVector = new COutputVector;
	pVector->szName = szName;
	pVector->v = copy(v);
	
	m_vVector.push_back(pVector);
}

void COutput::Add(CTString szName, CDoubleMatrix &m)
{
	COutputMatrix *pMatrix = new COutputMatrix;
	pMatrix->szName = szName;
	pMatrix->m = copy(m);
	
	m_vMatrix.push_back(pMatrix);
}

bool COutput::Remove(CTString szName, int nValue)
{
	int i=0, nSize=0;
	vector<COutputInteger *>::iterator it;

	nSize = m_vInteger.size();
	for (i=0; i<nSize; i++)
	{
		COutputInteger *pInteger = m_vInteger.at(i);
		if (pInteger->nValue != nValue || pInteger->szName != szName)
			continue;
		it = m_vInteger.begin() + i;
		m_vInteger.erase(it);
		delete pInteger;
		pInteger = NULL;
	}
	return true;
}

bool COutput::Remove(CTString szName, double dValue)
{
	int i=0, nSize=0;
	vector<COutputDouble *>::iterator it;
	
	nSize = m_vDouble.size();
	for (i=0; i<nSize; i++)
	{
		COutputDouble *pDouble = m_vDouble.at(i);
		if (pDouble->dValue != dValue || pDouble->szName != szName)
			continue;
		it = m_vDouble.begin() + i;
		m_vDouble.erase(it);
		delete pDouble;
		pDouble = NULL;
	}
	return true;
}

bool COutput::Remove(CTString szName, CTString szValue)
{
	int i=0, nSize=0;
	vector<COutputString *>::iterator it;
	
	nSize = m_vString.size();
	for (i=0; i<nSize; i++)
	{
		COutputString *pString = m_vString.at(i);
		if (pString->szValue != szValue || pString->szName != szName)
			continue;
		it = m_vString.begin() + i;
		m_vString.erase(it);
		delete pString;
		pString = NULL;
	}
	return true;
}

bool COutput::Remove(CTString szName, CTStringArray szValueArray)
{
	int i=0, nSize=0;
	vector<COutputStringArray *>::iterator it;
	
	nSize = m_vStringArray.size();
	for (i=0; i<nSize; i++)
	{
		COutputStringArray *pStrArr = m_vStringArray.at(i);
		//暂时只是判断长度及名字
		if (pStrArr->szValueArray.GetSize() != szValueArray.GetSize() ||
			pStrArr->szName != szName)
			continue;
		it = m_vStringArray.begin() + i;
		m_vStringArray.erase(it);
		delete pStrArr;
		pStrArr = NULL;
	}
	return true;
}

bool COutput::Remove(CTString szName, CDoubleVector &v)
{
	int i=0, nSize=0;
	vector<COutputVector *>::iterator it;
	
	nSize = m_vVector.size();
	for (i=0; i<nSize; i++)
	{
		COutputVector *pVector = m_vVector.at(i);
		//暂时只是判断长度及名字
		if (pVector->v.vlen() != v.vlen() || pVector->szName != szName)
			continue;
		it = m_vVector.begin() + i;
		m_vVector.erase(it);
		delete pVector;
		pVector = NULL;
	}
	return true;
}

bool COutput::Remove(CTString szName, CDoubleMatrix &m)
{
	int i=0, nSize=0;
	vector<COutputMatrix *>::iterator it;
	
	nSize = m_vMatrix.size();
	for (i=0; i<nSize; i++)
	{
		COutputMatrix *pMatrix = m_vMatrix.at(i);
		//暂时只是判断长度及名字
		if (pMatrix->m.mrow() != m.mrow() || 
			pMatrix->m.mcol() != m.mcol() || 
			pMatrix->szName != szName)
			continue;
		it = m_vMatrix.begin() + i;
		m_vMatrix.erase(it);
		delete pMatrix;
		pMatrix = NULL;
	}
	return true;
}

bool COutput::Remove(int nIndex,COutput::EKind eKind)
{
	int i=0, nSize=0;
	
	switch(eKind)
	{
	case N:
		{
			vector<COutputInteger *>::iterator it;
			nSize = m_vInteger.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputInteger *pInteger = m_vInteger.at(nIndex);
			delete pInteger;
			pInteger = NULL;
			//移除节点
			it = m_vInteger.begin();
			m_vInteger.erase(it+nIndex);
		}
		break;
	case D:
		{
			vector<COutputDouble *>::iterator it;
			nSize = m_vDouble.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputDouble *pDouble = m_vDouble.at(nIndex);
			delete pDouble;
			pDouble = NULL;
			//移除节点
			it = m_vDouble.begin();
			m_vDouble.erase(it+nIndex);
		}
		break;
	case S:
		{
			vector<COutputString *>::iterator it;
			nSize = m_vString.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputString *pString = m_vString.at(nIndex);
			delete pString;
			pString = NULL;
			//移除节点
			it = m_vString.begin();
			m_vString.erase(it+nIndex);
		}
		break;
	case A:
		{
			vector<COutputStringArray *>::iterator it;
			nSize = m_vStringArray.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputStringArray *pStrArr = m_vStringArray.at(nIndex);
			delete pStrArr;
			pStrArr = NULL;
			//移除节点
			it = m_vStringArray.begin();
			m_vStringArray.erase(it + nIndex);
		}
		break;
	case V:
		{
			vector<COutputVector *>::iterator it;
			nSize = m_vVector.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputVector *pVector = m_vVector.at(nIndex);
			delete pVector;
			pVector = NULL;
			//移除节点
			it = m_vVector.begin();
			m_vVector.erase(it+nIndex);
		}
		break;
	case M:
		{
			vector<COutputMatrix *>::iterator it;
			nSize = m_vMatrix.size();
			if (nIndex < 0 || nIndex >= nSize)
				return false;
			COutputMatrix *pMatrix = m_vMatrix.at(nIndex);
			delete pMatrix;
			pMatrix = NULL;
			//移除节点
			it = m_vMatrix.begin();
			m_vMatrix.erase(it+nIndex);
		}
		break;
	}

	return true;
}