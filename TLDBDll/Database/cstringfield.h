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
	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	virtual ~CStringField();

	//�й�����ת��������GetAsXXX����:
	virtual CTString       GetAsString();

	virtual bool          GetAsBool();

	virtual FieldCurrency GetAsCurrency();

	virtual FILETIME     GetAsDateTime();

	virtual double        GetAsDouble();

	virtual int           GetAsInteger();	

	//�й�����ת��������SetAsXXX����:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);

	//�������ݴ�С��������ָʾλ
	//##ModelId=3D098831005D
	virtual int GetDataLength(LPBYTE lpData = NULL);

	//�õ��ֶ�����
	virtual CFieldType GetFieldType();

	//�����������͵Ĵ�С����Ϊ������������¿ռ�ʱȷ��������ָ
	//ʾλ������  string����ʱ Ϊ4��
	//##ModelId=3D098835022F
	virtual int GetTypeSize();


	//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
    //��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);


	//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
	virtual bool GetFromDisplay(CTString str);


	//��ԭ���汾���ֶ�����ת������ǰ�ֶε����ݡ�
	//1��������ֶοռ�Ϊ�գ����ɿռ�
	//2��Ҫ����ԭ���ֶο�ֵ�жϣ�
	//2��1���Ϊ�����ֶ�Ϊ��
	//2��2�����Ϊ����ԭ����������ԭ�����������Ͷ�����Ȼ����ñ��ֶε�SetAs***���á�
	//Ȼ��������µ�next�ֶΣ�����ø��ֶε�ConvertToCurField���������Ҹ����ͷű��ռ䣬������0
	//virtual void ConvertToCurField(CField* pOldField) ;

	//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
	virtual void SetDefaultValue();

	//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
	virtual bool CopyToDefaultValue();
};

#endif /* _INC_CSTRINGFIELD_3CF32E1A01C4_INCLUDED */

