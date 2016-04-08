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
	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
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

	//�й�����ת��������GetAsXXX����:
	virtual FILETIME        GetAsDateTime();

	virtual bool             GetAsBool();

	virtual FieldCurrency    GetAsCurrency();
	
	virtual int              GetAsInteger();

	virtual double           GetAsDouble();

	

	virtual CTString   GetAsString();		

	//�й�����ת��������SetAsXXX����:
	virtual  bool SetAsDouble(double Value);

	virtual  bool SetAsInteger(int Value);		
	
	virtual  bool SetAsString(CTString Value);

	virtual  bool SetAsCurrency(FieldCurrency Value);

	virtual  bool SetAsBool(bool Value);

	virtual  bool SetAsDateTime(FILETIME* Value);

//	SYSTEMTIME SystemTime;
	//�õ��ֶ�����
	virtual CFieldType GetFieldType();

	
	//�������ݴ�С��������ָʾλ
	//##ModelId=3D0987870257
	virtual int GetDataLength(LPBYTE lpData = NULL);

	//�����������͵Ĵ�С����Ϊ������������¿ռ�ʱȷ��������ָ
	//ʾλ������  string����ʱ Ϊ4��
	//##ModelId=3D098790034A
	virtual int GetTypeSize();



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

	static bool StringToSysTime( CTString str, SYSTEMTIME& tm, DateType byteType );
};

#endif /* _INC_CDATEFIELD_3CF337AB003F_INCLUDED */

