#include "../headtldb.h"
#include "Missing.h"

#include "cfield.h"
CMissing::CMissing(CField * pField)
{
	m_nCount		= 0;
	m_pField		= pField;
	m_byteType		= mtNoMissing;

	m_dDataA		= 0;
	m_dDataB		= 0;
	m_dDataC		= 0;

	m_strDataA		= "";
	m_strDataB		= "";
	m_strDataC		= "";

}

CMissing::~CMissing()
{
}

CMissingType CMissing::GetMissingType()
{
	return m_byteType;
}
int	CMissing::GetCount() 
{
	return m_nCount;
}
//设置值类型
bool CMissing::SetMissingType( CMissingType nType) 
{
	if ( nType == mtNoMissing )
	{//缺失值，要将个数设置为零
		m_byteType = nType;
		m_nCount =  0;
	}
	else if ( nType == mtDiscrete )
		m_byteType = nType;
	else if (nType == msRange )
		m_byteType = nType;
	else 
		return false;

	return true;

}
 
bool CMissing::LoadFromBuffer(LPBYTE &lpData) 
{
	LPBYTE pOrigin = lpData;

	m_byteType = *(lpData);
	lpData += 1;
	if(m_byteType == mtNoMissing)
		return true;
	m_nCount = *(short*)(lpData);
	lpData += 2;

	if(m_nCount>0)
	{
		switch(m_pField->GetFieldType())
		{
		case fDouble :
		case fInt :
			m_dDataA = *(double*)(lpData);
			lpData += sizeof(double);
			if(m_nCount>1)
			{
				m_dDataB = *(double*)(lpData);
				lpData += sizeof(double);
			}
			if(m_nCount>2)
			{
				m_dDataC = *(double*)(lpData);
				lpData += sizeof(double);
			}
			break;
		case fString :
			{
			short nLength = *(short*)lpData;	//字符串最多65535字节长。
			lpData += 2;
			char szTemp[65535];
			strncpy(szTemp,(char*)lpData,nLength);
			szTemp[nLength]=0;
			m_strDataA = szTemp;
			lpData += nLength;

			if(m_nCount>1)
			{
				nLength = *(short*)lpData;	//字符串最多65535字节长。
				lpData += 2;
				strncpy(szTemp,(char*)lpData,nLength);
				szTemp[nLength]=0;
				m_strDataB = szTemp;
				lpData += nLength;
			}
			if(m_nCount>2)
			{
				nLength = *(short*)lpData;	//字符串最多65535字节长。
				lpData += 2;
				strncpy(szTemp,(char*)lpData,nLength);
				szTemp[nLength]=0;
				m_strDataC = szTemp;
				lpData += nLength;
			}
			}
			break;
		default :
			break;
		}
		return true;
	}
	else
		return true;
}
 
int	CMissing::SaveToBuffer(LPBYTE lpData) 
{
	LPBYTE lpOrigin = lpData;

	(*lpData) = m_byteType;			 //missing Type;
	lpData += 1;
	if(m_byteType == mtNoMissing)
		return lpData -lpOrigin;

	*(short*)(lpData) =(short)m_nCount;//Missing的个数，最多65535个。
	lpData += 2; 

	if(m_nCount>0)
	{
		switch(m_pField->GetFieldType())
		{
		case fDouble :
		case fInt :
			*(double*)(lpData) = m_dDataA;
			lpData += sizeof(double);
			if(m_nCount>1)
			{
				*(double*)(lpData) = m_dDataB;
				lpData += sizeof(double);
			}
			if(m_nCount>2)
			{
				*(double*)(lpData) = m_dDataC;
				lpData += sizeof(double);
			}
			break;
		case fString :
			{
			short nLength = m_strDataA.GetLength();	//字符串最多65535字节长。
			*(short*)(lpData) = nLength;
			lpData += 2;
			strcpy((char*)lpData,LPCTSTR(m_strDataA)); //
			lpData += nLength;

			if(m_nCount>1)
			{
				nLength = m_strDataB.GetLength();
				*(short*)(lpData) = nLength;
				lpData += 2;
				strcpy((char*)lpData,LPCTSTR(m_strDataB));
				lpData += nLength;
			}
			if(m_nCount>2)
			{
				nLength = m_strDataC.GetLength();
				*(short*)(lpData) = nLength;
				lpData += 2;
				strcpy((char*)lpData,LPCTSTR(m_strDataC));
				lpData += nLength;
			}
			}
			break;
		default :
			break;
		}
	}
	return lpData -lpOrigin;
}
 
int	CMissing::QueryLength() 
{
	int nLength = 0;
	nLength += sizeof(m_byteType);

	if(m_byteType == mtNoMissing)
		return nLength;

	nLength += sizeof(short); //m_nCount

	if(m_nCount>0)
	{
		switch(m_pField->GetFieldType())
		{
		case fDouble :
		case fInt :
			nLength += m_nCount*sizeof(double);
			break;
		case fString :
			nLength += m_strDataA.GetLength()+2;	

			if(m_nCount>1)
			{
				nLength += m_strDataB.GetLength()+2;	
			}
			if(m_nCount>2)
			{
				nLength += m_strDataC.GetLength()+2;	
			}
			break;
		default :
			break;
		}
	}
	return nLength;
}
 
void CMissing::Clone(CMissing* pMissing) 
{
	m_byteType		= pMissing->m_byteType;
	m_nCount  		= pMissing->m_nCount;  
	           		             
	m_dDataA  		= pMissing->m_dDataA;  
	m_dDataB 		= pMissing->m_dDataB;  
	m_dDataC  		= pMissing->m_dDataC;  
	           		             
	m_strDataA		= pMissing->m_strDataA;
	m_strDataB		= pMissing->m_strDataB;
	m_strDataC		= pMissing->m_strDataC;
}
 


int	CMissing::GetDoubleData(double & DataA,double & DataB,double & DataC)
{	
	if(m_nCount > 0)
		DataA = m_dDataA;
	if(m_nCount > 1)
		DataB = m_dDataB;
	if(m_nCount > 2)
		DataC = m_dDataC;
	return m_nCount;
}	 

bool CMissing::SetDoubleData(int nCount,double DataA,double DataB,double DataC) 
{
	m_nCount = nCount;

	if(m_nCount > 0)
		m_dDataA = DataA;
	if(m_nCount > 1)
		m_dDataB = DataB;
	if(m_nCount > 2)
		m_dDataC = DataC;
	return m_nCount?true:false;
}

int	CMissing::GetStrData(CTString &DataA,CTString &DataB,CTString &DataC)
{
	if(m_nCount > 0)
		DataA = m_strDataA;
	if(m_nCount > 1)
		DataB = m_strDataB;
	if(m_nCount > 2)
		DataC = m_strDataC;
	return m_nCount;
}

bool CMissing::SetStrData(int nCount,CTString DataA,CTString DataB,CTString DataC)
{
	m_nCount = nCount;

	if(m_nCount > 0)
		m_strDataA = DataA;
	if(m_nCount > 1)
		m_strDataB = DataB;
	if(m_nCount > 2)
		m_strDataC = DataC;
	return m_nCount?true:false;
}

