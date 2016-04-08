// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBOOLEANFIELD_3DCB404C03AB_INCLUDED
#define _INC_CBOOLEANFIELD_3DCB404C03AB_INCLUDED

#include "cdataset.h"

//##ModelId=3DCB404C03AB
class AFX_EXT_CLASS CBooleanField 
: public CField
{

public:

	//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	CBooleanField(CFields* pOwner);

	virtual ~CBooleanField();

	//有关数据转换的六个GetAsXXX函数:
	virtual bool      GetAsBool();

	virtual FieldCurrency GetAsCurrency();

	virtual FILETIME GetAsDateTime();
	
	virtual double    GetAsDouble();

	virtual int       GetAsInteger();	

	virtual CTString   GetAsString();	

	//有关数据转换的六个SetAsXXX函数:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);


	//得到字段类型
	virtual CFieldType GetFieldType();

	int GetTypeSize();
	
	//返回数据大小，不包括指示位,用于对文件等读时确定下一个的位置
    //lpData用于变长时，（如是字符型，指向数据）定长时为空
	//##ModelId=3CF42CC30261
	virtual int GetDataLength(LPBYTE lpData = NULL);



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

};

#endif /* _INC_CBOOLEANFIELD_3DCB404C03AB_INCLUDED */

