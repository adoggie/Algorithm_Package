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
	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();



	CCurrencyField(CFields* pOwner);

	virtual ~CCurrencyField();

	//�й�����ת��������GetAsXXX����:
	virtual FieldCurrency GetAsCurrency();

	virtual bool      GetAsBool();	

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

	//virtual int SaveToBuffer(LPBYTE lpData);
	


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

	//���û�������
	void SetCurrencyType(CTString strType);

	//ȡ�û�������
	CTString GetCurrencyType();

	//���ݿ�¡�����ݴ�����ֶ������¡�ֶεĻ�����Ϣ
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

