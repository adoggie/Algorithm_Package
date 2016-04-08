// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDOUBLEFIELD_3CF337CE032E_INCLUDED
#define _INC_CDOUBLEFIELD_3CF337CE032E_INCLUDED

#include "cfield.h"
#include "cdataset.h"
#include "cfields.h"

class CDataSet;

//##ModelId=3CF337CE032E
class AFX_EXT_CLASS CDoubleField 
: public CField
{
public:

	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	CDoubleField(CFields* pOwner);

	virtual ~CDoubleField();

	//�й�����ת��������GetAsXXX����:
	virtual double           GetAsDouble();

	virtual bool             GetAsBool();

	virtual FieldCurrency    GetAsCurrency();
	
	virtual FILETIME        GetAsDateTime();
	
	virtual int              GetAsInteger();	

	virtual CTString          GetAsString();	

	//�й�����ת��������SetAsXXX����:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);

	virtual int GetTypeSize();

	//�õ��ֶ�����
	virtual CFieldType GetFieldType();

	//�������ݴ�С��������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��
    //lpData���ڱ䳤ʱ���������ַ��ͣ�ָ�����ݣ�����ʱΪ��
	//##ModelId=3CF42CC30261
	virtual int GetDataLength(LPBYTE lpData = NULL);



	//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
    //��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);





	//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
	virtual bool GetFromDisplay(CTString str);


	//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
	virtual void SetDefaultValue();

	//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
	virtual bool CopyToDefaultValue();
};

#endif /* _INC_CDOUBLEFIELD_3CF337CE032E_INCLUDED */

