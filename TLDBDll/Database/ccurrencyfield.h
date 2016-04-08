// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCURRENCYFIELD_3CF3381B0108_INCLUDED
#define _INC_CCURRENCYFIELD_3CF3381B0108_INCLUDED

#include "cfield.h"

#include "cfields.h"


//##ModelId=3CF3381B0108
class AFX_EXT_CLASS CCurrencyField 
: public CField
{
public:
	//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();



	CCurrencyField(CFields* pOwner);

	virtual ~CCurrencyField();

	//有关数据转换的六个GetAsXXX函数:
	virtual FieldCurrency GetAsCurrency();

	virtual bool      GetAsBool();	

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

	//virtual int SaveToBuffer(LPBYTE lpData);
	


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

	//设置货币类型
	void SetCurrencyType(CTString strType);

	//取得货币类型
	CTString GetCurrencyType();

	//数据克隆，根据传入的字段样板克隆字段的基本信息
	//##ModelId=3D07E95801D4
	virtual void Clone(CField* pField);

	CTString m_strCurrencyType;
protected:
	//##ModelId=3CF56FA6016D
	virtual void LoadExt(LPBYTE lpData, int count);	

	//##ModelId=3CFADAE30081
	virtual void SaveExt(LPBYTE lpData, int count); 	
};

#endif /* _INC_CCURRENCYFIELD_3CF3381B0108_INCLUDED */

