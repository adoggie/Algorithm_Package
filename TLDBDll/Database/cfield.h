// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFIELD_3CF32DA80116_INCLUDED
#define _INC_CFIELD_3CF32DA80116_INCLUDED

#include "cvaluelabel.h"
#include "showcharacteristic.h"
#include "../../Public/TCLFile.h"
#include "Missing.h"

class CFields;
#define IncreaseSize 500
//////////////////////////////
	//�õ��ֶ�����
typedef char  CFieldType;

#define fDouble 'F'     //˫���ȸ���������
#define fInt 'N'       //��������
#define fString 'C'    //�ַ���������
#define fDate 'D'      //����������
#define fCurrency 'M'  //����������
#define fBoolean 'L'   //����������
#define fBlob 'B'      //������

#define DoubleZero       1E-30

//********************** �ֶνṹ���� ****************************
/*
struct FieldString
{
	//BYTE Indicate;
	CTString strValue;
};

struct FieldDouble
{
	BYTE Indicate;
	double dwValue;	
};

struct FieldNUM
{
	BYTE Indicate;
	double dwValue;	
};*/
typedef double FieldNUM;
typedef CTString FieldString;
typedef double FieldCurrency;
/*
struct FieldBoolean
{
	BYTE Indicate;
	BYTE bmValue;
};
struct FieldCurrency
{ 
	BYTE Indicate;
	//BYTE bmMoneyType;
	double dwValue;
};
struct FieldDate
{
	BYTE Indicate;
	FILETIME ftValue;
};*/
 /*
struct FieldInt
{
	BYTE Indicate;
	int nValue;
};*/
///////////////////////////////// 
//********************** ҳ���鶨�� ****************************
//��
#define F_NUM				(CTArray<FieldNUM,FieldNUM>)
#define PF_NUM				CTArray<FieldNUM,FieldNUM>* 
/*
#define F_DOUBLE				(CTArray<FieldDouble,FieldDouble>)
#define PF_DOUBLE				CTArray<FieldDouble,FieldDouble>*
*/
//
#define F_STRING				(CTArray<FieldString,FieldString>)
#define PF_STRING				CTArray<FieldString,FieldString>*
//*
//#define F_INT					(CTArray<FieldInt,FieldInt>)
//#define PF_INT					CTArray<FieldInt,FieldInt>*
//
//#define F_BOOL					(CTArray<FieldBoolean,FieldBoolean>)
//#define PF_BOOL					CTArray<FieldBoolean,FieldBoolean>*
//
//#define F_CURRENCY				(CTArray<FieldCurrency,FieldCurrency>)
//#define PF_CURRENCY				CTArray<FieldCurrency,FieldCurrency>*
//
//#define F_DATE					(CTArray<FieldDate,FieldDate>)
//#define PF_DATE					CTArray<FieldDate,FieldDate>*
//*/
/////////////////////////////////
struct CViewSet
{//###_Mod 2002-11-1 zlq
	//*************�е�����******************//
	int			nCWidth;
	COLORREF	cCColor;
	byte		byteCAlign;
	CTString		strCFontName;
	byte		byteCFontStyle;
    byte		byteCFontSize;
	//*************�е�����******************//
	int			nRWidth;
	COLORREF	cRColor;
	byte		byteRAlign;
	CTString		strRFontName;
	byte		byteRFontStyle;
	byte		byteRFontSize;
};

//*****************��������******************************
//### 2002-9-25 Eric ==>
typedef byte CMeasure;
#define msOrdinal 0x01
#define msNominal 0x02
#define msScale   0x03
//*****************����ָʾ******************************
//�ڴ��ֶ�ָʾ�ֽڱ�־ //###_Mod 2002-9-27 zlq
//#define fvNULL 0xFE //��ֵ

//#define fvValid 0x01 //��Чֵ
//#define fvConvert 0xFD //��Чֵ

#define DISPERSE_DATA 0x01  //��ɢ������   ///cwh
#define CONTINU_DATA  0x02  //����������   ///cwh

typedef byte DateType;
#define ddmmmyyyy_line                48  //dd-mmm-yyyy
#define ddmmmyy_line                  49  //dd-mmm-yy
#define mmddyyyy_bias                 50  //mm/dd/yyyy                    
#define mmddyy_bias                   51  //mm/dd/yy
#define ddmmyyyy_dot				  52  //dd.mm.yyyy
#define ddmmyy_dot                    53  //dd.mm.yy
#define yyddd                         54  //yyddd
#define yyyyddd                       55  //yyyyddd
#define qQyyyy_blank                  56  //q Q yyyy
#define qQyy_blank                    57  //q Q yy
#define mmmyyyy_blank                 58  //mmm yyyy
#define mmmyy_blank                   59  //mmm yy
#define wwWKyyyy_blank                60  //ww WK yyyy
#define wwWKyy_blank                  61  //ww WK yy
#define ddmmmyyyyhhmm_colon           62  //dd-mmm-yyyy hh:mm
#define ddmmmyyyyhhmmss_colon         63  //dd-mmm-yyyy hh:mm:ss
#define ddmmmyyyyhhmmssss_colon       64  //dd-mmm-yyyy hh:mm:ss.ss
#define hhmm_colon                    65  //hh:mm
#define hhmmss_colon                  66  //hh:mm:ss
#define hhmmssss_colon                67  //hh:mm:ss.ss
#define dddhhmm_colon                 68  //ddd hh:mm
#define dddhhmmss_colon               69  //ddd hh:mm:ss
#define dddhhmmssss_colon             70  //ddd hh:mm:ss.ss
#define week_en                       71  //monday,Tuesday,...
#define week_en_bre                   72  //Mon,Tue,Wed,...
#define month_en                      73  //Juanuary,February
#define month_en_bre                  74  //Jan,Feb,Mar,...
//����Ϊ�����й�ϰ�ߵ����ڱ�ʾ��ʽ:
#define yyyymmdd_line                 75  //yyyy-mm-dd
#define yymmdd_line                   76  //yy-mm-dd
#define yyyymmdd_dot                  77  //yyyy.mm.dd
#define yymmdd_dot                    78  //yy.mm.dd
#define yyyymmdd_bias                 79  //yyyy/mm/dd
#define yymmdd_bias                   80  //yy/mm/dd
#define yyyymmddhhmm_line             81  //yyyy-mm-dd hh:mm
#define yyyymmddhhmmss_line           82  //yyyy-mm-dd hh:mm:ss
#define yyyymmddhhmmssss_line         83  //yyyy-mm-dd hh:mm:ss.ss
#define yyyymmddhhmm_dot              84  //yyyy.mm.dd hh:mm
#define yyyymmddhhmmss_dot            85  //yyyy.mm.dd hh:mm:ss
#define yyyymmddhhmmssss_dot          86  //yyyy.mm.dd hh:mm:ss.ss
#define yyyymmddhhmm_bias             87  //yyyy/mm/dd hh:mm
#define yyyymmddhhmmss_bias           88  //yyyy/mm/dd hh:mm:ss
#define yyyymmddhhmmssss_bias         89  //yyyy/mm/dd hh:mm:ss.ss

#define DateDisplayMin                48
#define DateDisplayMax                89
//*************��չ����***************
#define Ext_Enum                      01  //ö��
#define Ext_ValueLabel                02  //ֵ��ǩ
#define Ext_ShowCharacteristic        03  //��ʾ����

#define Ext_TitleLength100        03  //��ʾ����
#define Ext_TitleLength110        05  //��ʾ����
#define Align4(n)   ( (n + sizeof(int) - 1) & ~(sizeof(int) - 1) )
//##ModelId=3CF32DA80116

class AFX_EXT_CLASS CField  
{
friend class CPage;
	
protected:	
	//�ñ��������ֶ��·���ռ�ʱ��newdata�����ñ��ռ�
	void * m_pTempData;//new //#_�޸� 2002-11-26 $ 10:21:10 zlq

	char* m_pTempBuf;//����ת��ʱ�ã����ǿ����
	int    m_nTempBufLength;
	CTString m_sName;
	//int  m_nComma;   //����ɸѡ��־��1��ʾɸѡ��0��ʾ��ɸѡ
public:
	virtual void pg_Add(void*pDestArrar) ;
	virtual void pg_Add(void*pDestArrar,int nCount) ;
	virtual void pg_InsertAt(void*pDestArrar,int nIndex);
	void pg_RemoveAt(void *pDestArrar,int nIndex);
	void pg_RemoveAll(void *pDestArrar);
	void pg_Delete(void *pDestArrar);
	LPBYTE pg_GetAt(void*pDestArrar,int nIndex);
	void* pg_New();
	//��һ�ֽڣ�����ָʾ��
	//�������ݲ���,ָ��ǰ���������е�����
	//##ModelId=3CF33A3303E4
	LPBYTE m_pValue;
	LPBYTE m_pOldValue;

	//##ModelId=3CF33F8D00D7


	//���Եõ���ǰ��״̬(����д�������)
	CFields* m_pFields;
//private:	  //===��ʱ
	//##ModelId=3CF3400001AE
	CTString m_sLabel;

	//##ModelId=3CF471890091
	int m_nExtLength;

	//##ModelId=3CF471A102C6
	int m_nExtCount;

	//##ModelId=3CF4957C0351
	int m_nWidth;

	//##ModelId=3CF4954303DB
	int m_nDecimals;

	//���ֶ��Ƿ���ö��
	//##ModelId=3CF570100115
	bool m_bIsEnum;

	//ĳ�����ʹ����ö��
	//##ModelId=3CF570A30261
	byte m_byteEnumType;

	//ָʾ��ʾ��ʽ�ȣ������ڵ���ʾ�����ʽ
	//##ModelId=3CF57141016E
	byte m_byteEnumData;

	//##ModelId=3D0437DC01A2
	CValueLabel m_ValueLabel;

	CShowCharacteristic m_ShowCharacteristic;
	//???
	int m_nNextModifyField;

	//ȱʡֵ���������ٲ���
	LPBYTE m_pDefaultValue;

	//�����ʼ��
    bool m_bHasDefault;

	CMeasure m_byteMeasure;	 

	CMissing *m_pMissing;

public:	
	bool IsMissingValue();
	void PrintValue();//===temp
	virtual bool GetValueVL(int nValue,CTString &strLabel);
	virtual bool GetValueVL(double dValue,CTString &strLabel);
	virtual bool GetValueVL(CTString strValue,CTString &strLabel);
	virtual bool GetValueVL(bool bValue,CTString &strLabel);
	virtual bool GetValueVL(FILETIME ftValue,CTString &strLabel);
	void CopyCommonProperty(CField*pSourceField);

	virtual DateType GetEnumType();
	virtual bool SetEnumType(DateType nType=ddmmmyyyy_line);
	//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	int GetDCDataType(int nIndex);//ȡ���������ͣ���ɢ��/�����ͣ�

	//������ʾ��ص�����
	bool SetViewSet(CViewSet ViewSet);
	//ȡ����ʾ��ص�����
	bool GetViewSet(CViewSet &ViewSet);
	CField();

	virtual ~CField();

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
	virtual CFieldType GetFieldType() = 0;

	//���ݻ������������ֶ���Ϣ
	//##ModelId=3CF4300801E2
	virtual int LoadFromBuffer(LPBYTE lpData);

	//�õ���¼���ݣ��������Ӧ��ָ����Ч����Ϊ��ֵ��
	//����ӵ������У��ɵ�������ӣ�
	//����trueΪ�����ݳɹ�
	//false ������ʧ�ܣ�ָ�벻��
	//##ModelId=3CF3402202D0
	virtual void LoadDataFromBuffer( LPBYTE& lpData);
	
	//�������ݴ�С��������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��
    //lpData���ڱ䳤ʱ���������ַ��ͣ�ָ�����ݣ�����ʱΪ��
	//##ModelId=3CF42CC30261
	virtual int GetDataLength(LPBYTE lpData = NULL);	

	//�����������͵Ĵ�С����Ϊ������������¿ռ�ʱȷ��������ָ
	//ʾλ������  string����ʱ Ϊ4��
	//##ModelId=3CF479B502B6
	virtual int GetTypeSize()=0;

	//##ModelId=3CF49D4200B3
	virtual bool IsNull();	

	virtual void SetNull();

	//##ModelId=3CFADAB101DE
	//virtual int SaveToBuffer_(LPBYTE lpData);
	virtual int SaveToBuffer(TCLFile *pFile);
	

	//lpDataָ�������Ϊ����ָʾ+��������,�������ݳ���
	//##ModelId=3D0542FE0325
	virtual void SaveDataToBuffer(LPBYTE& lpData);

	//���ݿ�¡�����ݴ�����ֶ������¡�ֶεĻ�����Ϣ
	//##ModelId=3D07E95801D4
	virtual void Clone(CField* pField);

	
	//�����µ����ݿռ䣬�����ݼ�����insert append�Ȳ���ʱ��
	//�ȵ���
	virtual void RealNewData();

	void DeleteData();

	void NewData();

	//ɾ��m_pValue��ָ��Ŀռ�
	virtual void  RealDeleteData();

	//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
    //��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);

	//�����ֶε���������һ����������ַ�ռ�Ҳ�ɱ��������ɣ�
	//�����ظõ�ַ���������ݴ��ݺͱ����ֵ
	//virtual LPBYTE SetData();

	//�õ���ǰ�ֶε�ֵ��m_pValueָ��ģ����浽�ļ�ʱ����ĳ��ȣ�
	//����ָʾλ��Ҳ���ܾ�ָʾλҲ���ˣ�����ȷ���������Ĵ�С��
	//ע�⣺��С��ֵ���
	virtual int QuerySaveLength();

	//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
	virtual bool GetFromDisplay(CTString str);

	
	//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
	virtual void SetDefaultValue();

	//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
	virtual bool CopyToDefaultValue();

	//��ԭ���汾���ֶ�����ת������ǰ�ֶε����ݡ�
	//1��������ֶοռ�Ϊ�գ����ɿռ�
	//2��Ҫ����ԭ���ֶο�ֵ�жϣ�
	//2��1���Ϊ�����ֶ�Ϊ��
	//2��2�����Ϊ����ԭ����������ԭ�����������Ͷ�����Ȼ����ñ��ֶε�SetAs***���á�
	//Ȼ��������µ�next�ֶΣ�����ø��ֶε�ConvertToCurField���������Ҹ����ͷű��ռ䣬������0
	virtual void ConvertToCurField(CField* pOldField) ;

	void SetLabel(CTString strLabel);

	CTString GetLabel();

	bool SetFieldName(CTString strLabel);

	CTString GetFieldName();

    void SetDecimals(int nDecimals);

	int GetDecimals();

	void SetWidth(int nWidth);

	int GetWidth();
	 
	//### 2002-9-25 Eric===>
	virtual CMeasure GetMeasure();	 

	virtual void SetMeasure(CMeasure measure); 
	//### 2002-9-25 Eric<===
	
	bool SetDateConvertType(BYTE cType);
	DateType GetDateConvertType();
	virtual bool bHaveValueLabel();
protected:

	//##ModelId=3CF56FA6016D
	virtual void LoadExt(LPBYTE lpData, int count);	

	//##ModelId=3CFADAE30081
	virtual void SaveExt(LPBYTE lpData, int count); 
};

#endif /* _INC_CFIELD_3CF32DA80116_INCLUDED */

