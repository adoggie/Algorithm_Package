// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef _INC_CVALUELABEL_3CF58AD803D0_INCLUDED
#define _INC_CVALUELABEL_3CF58AD803D0_INCLUDED

class CField;


class CIntField;

class CBooleanField;
class CCurrencyField;
class CDateField;
class CDoubleField;
class CStringField;

typedef double FieldNUM;
typedef double FieldCurrency;
//##ModelId=3CF58AD803D0
class AFX_EXT_CLASS CValueLabel 
{
	bool m_bIsNum;
public:	
	CField* m_pField;
	//各个数据值的链表
	//##ModelId=3CF58B1B003D
	CPtrList m_DataList;

	//各数据值对应的标签链表
	//##ModelId=3CF58AFD01E9
	//CStringList m_strLabelList;
	CTStringArray m_strLabelList;//===uc  内存问题

	//##ModelId=3CF747160354
	

	int m_nCurPos;

public:
	void SetOwner(CField*pField);
	
	CValueLabel();
	~CValueLabel();

	void Clone(CValueLabel * pValueLabel);

	int QueryLength();

	//##ModelId=3CF5916C01A1
	virtual int LoadFromBuffer(LPBYTE lpData, int count);

	//返回标签个数
	//##ModelId=3CF5936302DB
    int GetCount();

	//##ModelId=3CFB07A6024B
	virtual int SaveToBuffer(LPBYTE lpData);

	void SetCurPos(int nIndex);

	CTString GetExplainString();

	void SetExplainString(CTString str);

	//有关数据转换的六个GetAsXXX函数:
	bool      GetAsBool();

	FieldCurrency GetAsCurrency();

	FILETIME GetAsDateTime();

	double    GetAsDouble();

	int       GetAsInteger();	

	CTString   GetAsString();		

	//有关数据转换的六个SetAsXXX函数:
	bool SetAsDouble(double Value);

	bool SetAsInteger(int Value);		
	
	bool SetAsString(CTString Value);

	bool SetAsCurrency(FieldCurrency Value);

	bool SetAsBool(bool Value);

	bool SetAsDateTime(FILETIME* Value);
	//### 2002-11-11 Eric ======>
	bool AddAs(int nValue,CTString strExplain); 

	bool AddValue(CTString Value,CTString strExplain);

	bool AddValue(int Value,CTString strExplain); 

	bool AddValue(double Value,CTString strExplain); 

	bool AddValue(FILETIME* Value,CTString strExplain); 

	bool Delete(int nIndex);

	bool IsValueExist(LPBYTE lpData);

	bool IsValid(CTString Value);//要添加的标签值是否有效

	//### 2002-11-11 Eric<======
};

#endif /* _INC_CVALUELABEL_3CF58AD803D0_INCLUDED */

