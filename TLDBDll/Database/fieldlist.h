// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFIELDS_FIELDLIST0002_INCLUDED
#define _INC_CFIELDS_FIELDLIST0002_INCLUDED

#include "cfield.h"
#include "cpages.h"


#include "cintfield.h"
#include "cbooleanfield.h"
#include "ccurrencyfield.h"
#include "cdatefield.h"
#include "cdoublefield.h"
#include "cstringfield.h"

class AFX_EXT_CLASS CFieldList
 
{
	friend class CPages;
	CPages * m_pPages;
public:
	int IndexOf(CTString strFieldName);
	
	CTString GetExclusiveName(CField* pFld);

	~CFieldList();
	CFieldList(CPages * pPages);

	bool Add(CField* pField);
	void Clear();
	bool Delete(int nIndex);
	bool Insert(CField * pField,int nIndex);
	bool Modify(int nIndex,CFieldType NewFieldType);
	bool Modify(int nIndex,CField * pField);
	CField* FieldByName(CTString FieldName);
	CField* FieldByIndex(int index); 
	int IndexOf(CField* pField);

	int GetFieldCount();
	CField* CreateField(CFieldType FieldType);
	CField* CreateField(CField * pTempleField);
	bool Mov(int nFrom, int nTo);
private:
	CField* m_pField;
};

#endif /* _INC_CFIELDS_3CF32E040212_INCLUDED */

