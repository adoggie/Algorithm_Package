// DecTreeCol.cpp : implementation file
//

#include "StdAfx.h"
#include "DecTreeCol.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif*/


/////////////////////////////////////////////////////////////////////////////
// DecTreeCol


DecTreeCol::DecTreeCol()
{
}

DecTreeCol::~DecTreeCol()
{
}


/////////////////////////////////////////////////////////////////////////////
// DecTreeCol commands
CTString DecTreeCol::getColName()
{
	return m_strColName;
}

void DecTreeCol::setColName(CTString strColName)
{
	m_strColName = strColName;
}

int DecTreeCol::getColLength()
{
	return m_nColLength;
}

void DecTreeCol::setColLength(int nLength)
{
	m_nColLength = nLength;
}
//////////////////////////////////////////////////////////////////////////
//DecTreeColList 
// constructor
DecTreeColList::DecTreeColList(/*CDataAccess  *pDataAccess*/)
{
	//m_pDataAccess = pDataAccess;
}

DecTreeColList::~DecTreeColList()
{
}

//////////////////////////////////////////////////////////////////////////
//DectreeColLis methods
int	DecTreeColList::getColCount()
{
	CFieldList *pFieldList = m_pDataAccess->m_pFieldList;
	return pFieldList->GetFieldCount();
}

/*
int DecTreeColList::getColListData(CUIntArray &nColAry,TCColMatrix &ColMatrix, CDataAccess *pDataAccess)
{
	ColMatrix.RemoveAll();
	long lRecord = pDataAccess->GetRecordCount();

	if(lRecord == 0)
		return 0;
	
	m_pDataAccess->First();
	int nRowIndex = 0;
	DecTreeColList *pdtColList;

	if(!pDataAccess->Eof())
	{
		for(int i=0; i<nColAry.GetSize(); i++)
		{
			CField *pField = m_pDataAccess->m_pFieldList->FieldByIndex(nColAry.GetAt(i));
			switch(pField->GetFieldType()) {
			case fDouble:
				pdtColList->m_dElement.SetAt(i,pField->GetAsDouble());
				pdtColList->m_nIndex = nRowIndex + 1;
				break;
			case fInt:
				pdtColList->m_nElement.SetAt(i,pField->GetAsInteger());
				pdtColList->m_nIndex = nRowIndex + 1;
				break;
			case fString:
				pdtColList->m_strElement = pField->GetAsString();
				pdtColList->m_nIndex = nRowIndex + 1;
			}
			
			ColMatrix.GetAt(i)->SetAt(nRowIndex,pdtColList);
						
		}

		nRowIndex ++;
		pDataAccess->Next();
	}

	return nRowIndex;
}
*/
/*
DecTreeCol* DecTreeColList::ColByName(CTString ColName)  
{

}

DecTreeCol*	DecTreeColList::ColByIndex(int nIndex)		
{

}

DecTreeCol*DecTreeColList::CreateCol(CFieldType FieldType)
{

}
*/