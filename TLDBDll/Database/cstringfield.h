// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSTRINGFIELD_3CF32E1A01C4_INCLUDED
#define _INC_CSTRINGFIELD_3CF32E1A01C4_INCLUDED



#include "cfield.h"
#include "cfields.h"

//##ModelId=3CF32E1A01C4
class AFX_EXT_CLASS CStringField 
: public CField
{
protected:


public:		
	CStringField(CFields* pOwner);
	//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	virtual ~CStringField();

	//有关数据转换的六个GetAsXXX函数:
	virtual CTString       GetAsString();

	virtual bool          GetAsBool();

	virtual FieldCurrency GetAsCurrency();

	virtual FILETIME     GetAsDateTime();

	virtual double        GetAsDouble();

	virtual int           GetAsInteger();	

	//有关数据转换的六个SetAsXXX函数:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);

	//返回数据大小，不包括指示位
	//##ModelId=3D098831005D
	virtual int GetDataLength(LPBYTE lpData = NULL);

	//得到字段类型
	virtual CFieldType GetFieldType();

	//返回数据类型的大小，在为数据链表分配新空间时确定（包含指
	//示位和数据  string类型时 为4）
	//##ModelId=3D098835022F
	virtual int GetTypeSize();


	//根据lpData指向的数据，复制该数据值到本字段，
    //两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);


	//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
	virtual bool GetFromDisplay(CTString str);


	//将原来版本的字段数据转换到当前字段的数据。
	//1。如果本字段空间为空，生成空间
	//2。要进行原来字段空值判断！
	//2。1如果为空则本字段为空
	//2。2如果不为空则将原来的数据以原来的数据类型读出，然后调用本字段的SetAs***设置。
	//然后，如果有新的next字段，则调用该字段的ConvertToCurField函数，并且负责释放本空间，并且清0
	//virtual void ConvertToCurField(CField* pOldField) ;

	//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
	virtual void SetDefaultValue();

	//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
	virtual bool CopyToDefaultValue();
};

#endif /* _INC_CSTRINGFIELD_3CF32E1A01C4_INCLUDED */

