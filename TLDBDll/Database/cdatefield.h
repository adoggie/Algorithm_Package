// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDATEFIELD_3CF337AB003F_INCLUDED
#define _INC_CDATEFIELD_3CF337AB003F_INCLUDED



#include "cfield.h"
#include "cdataset.h"
#include "cfields.h"


//enum En_Month{jan=1,feb,mar,apr,may,jun,jul,aug,sep,oct,dec};

//##ModelId=3CF337AB003F
class AFX_EXT_CLASS  CDateField : public CField
{
public:

//	SYSTEMTIME SystemTime;
	COleDateTime m_tm;

public:
	//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();

	int GetDayOfWeek();
	int GetSecond();
	int GetMinute();
	int GetHour();
	int GetMonth();
	int GetYear();
	int GetDay( );
	CDateField(CFields* pOwner);
	static CTString GetDateType( DateType byteType );

	virtual ~CDateField();
	virtual bool     SetEnumType(DateType Type=ddmmmyyyy_line);
	virtual DateType GetEnumType();

	//有关数据转换的六个GetAsXXX函数:
	virtual FILETIME        GetAsDateTime();

	virtual bool             GetAsBool();

	virtual FieldCurrency    GetAsCurrency();
	
	virtual int              GetAsInteger();

	virtual double           GetAsDouble();

	

	virtual CTString   GetAsString();		

	//有关数据转换的六个SetAsXXX函数:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);

//	SYSTEMTIME SystemTime;
	//得到字段类型
	virtual CFieldType GetFieldType();

	
	//返回数据大小，不包括指示位
	//##ModelId=3D0987870257
	virtual int GetDataLength(LPBYTE lpData = NULL);

	//返回数据类型的大小，在为数据链表分配新空间时确定（包含指
	//示位和数据  string类型时 为4）
	//##ModelId=3D098790034A
	virtual int GetTypeSize();



	//根据lpData指向的数据，复制该数据值到本字段，
    //两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);
	//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
	virtual bool GetFromDisplay(CTString str);

	
	//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
	virtual void SetDefaultValue();

	//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
	virtual bool CopyToDefaultValue();

	static bool StringToSysTime( CTString str, SYSTEMTIME& tm, DateType byteType );
};

#endif /* _INC_CDATEFIELD_3CF337AB003F_INCLUDED */

