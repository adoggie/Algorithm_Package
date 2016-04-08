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

	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	CBooleanField(CFields* pOwner);

	virtual ~CBooleanField();

	//�й�����ת��������GetAsXXX����:
	virtual bool      GetAsBool();

	virtual FieldCurrency GetAsCurrency();

	virtual FILETIME GetAsDateTime();
	
	virtual double    GetAsDouble();

	virtual int       GetAsInteger();	

	virtual CTString   GetAsString();	

	//�й�����ת��������SetAsXXX����:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);


	//�õ��ֶ�����
	virtual CFieldType GetFieldType();

	int GetTypeSize();
	
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

#endif /* _INC_CBOOLEANFIELD_3DCB404C03AB_INCLUDED */

