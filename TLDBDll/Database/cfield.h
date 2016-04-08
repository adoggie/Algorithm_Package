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
	//得到字段类型
typedef char  CFieldType;

#define fDouble 'F'     //双精度浮点型数据
#define fInt 'N'       //整型数据
#define fString 'C'    //字符串型数据
#define fDate 'D'      //日期型数据
#define fCurrency 'M'  //货币型数据
#define fBoolean 'L'   //布尔型数据
#define fBlob 'B'      //型数据

#define DoubleZero       1E-30

//********************** 字段结构定义 ****************************
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
//********************** 页数组定义 ****************************
//新
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
	//*************列的设置******************//
	int			nCWidth;
	COLORREF	cCColor;
	byte		byteCAlign;
	CTString		strCFontName;
	byte		byteCFontStyle;
    byte		byteCFontSize;
	//*************行的设置******************//
	int			nRWidth;
	COLORREF	cRColor;
	byte		byteRAlign;
	CTString		strRFontName;
	byte		byteRFontStyle;
	byte		byteRFontSize;
};

//*****************测量方法******************************
//### 2002-9-25 Eric ==>
typedef byte CMeasure;
#define msOrdinal 0x01
#define msNominal 0x02
#define msScale   0x03
//*****************数据指示******************************
//内存字段指示字节标志 //###_Mod 2002-9-27 zlq
//#define fvNULL 0xFE //空值

//#define fvValid 0x01 //有效值
//#define fvConvert 0xFD //有效值

#define DISPERSE_DATA 0x01  //离散型数据   ///cwh
#define CONTINU_DATA  0x02  //连续型数据   ///cwh

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
//以下为符合中国习惯的日期表示方式:
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
//*************扩展属性***************
#define Ext_Enum                      01  //枚举
#define Ext_ValueLabel                02  //值标签
#define Ext_ShowCharacteristic        03  //显示特性

#define Ext_TitleLength100        03  //显示特性
#define Ext_TitleLength110        05  //显示特性
#define Align4(n)   ( (n + sizeof(int) - 1) & ~(sizeof(int) - 1) )
//##ModelId=3CF32DA80116

class AFX_EXT_CLASS CField  
{
friend class CPage;
	
protected:	
	//该变量是在字段新分配空间时（newdata），用本空间
	void * m_pTempData;//new //#_修改 2002-11-26 $ 10:21:10 zlq

	char* m_pTempBuf;//数据转换时用，不是跨过程
	int    m_nTempBufLength;
	CTString m_sName;
	//int  m_nComma;   //逗号筛选标志。1表示筛选；0表示不筛选
public:
	virtual void pg_Add(void*pDestArrar) ;
	virtual void pg_Add(void*pDestArrar,int nCount) ;
	virtual void pg_InsertAt(void*pDestArrar,int nIndex);
	void pg_RemoveAt(void *pDestArrar,int nIndex);
	void pg_RemoveAll(void *pDestArrar);
	void pg_Delete(void *pDestArrar);
	LPBYTE pg_GetAt(void*pDestArrar,int nIndex);
	void* pg_New();
	//第一字节：数据指示，
	//其余数据部分,指向当前数据链表中的数据
	//##ModelId=3CF33A3303E4
	LPBYTE m_pValue;
	LPBYTE m_pOldValue;

	//##ModelId=3CF33F8D00D7


	//用以得到当前的状态(读，写，插入等)
	CFields* m_pFields;
//private:	  //===暂时
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

	//该字段是否是枚举
	//##ModelId=3CF570100115
	bool m_bIsEnum;

	//某种类型处理的枚举
	//##ModelId=3CF570A30261
	byte m_byteEnumType;

	//指示显示格式等，如日期的显示处理格式
	//##ModelId=3CF57141016E
	byte m_byteEnumData;

	//##ModelId=3D0437DC01A2
	CValueLabel m_ValueLabel;

	CShowCharacteristic m_ShowCharacteristic;
	//???
	int m_nNextModifyField;

	//缺省值，不忘销毁操作
	LPBYTE m_pDefaultValue;

	//定义初始化
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
	//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
	virtual LPCTSTR GetValueLable();
	virtual CTString GetValLable();
	int GetDCDataType(int nIndex);//取得数据类型（离散型/连续型）

	//设置显示相关的特性
	bool SetViewSet(CViewSet ViewSet);
	//取得显示相关的特性
	bool GetViewSet(CViewSet &ViewSet);
	CField();

	virtual ~CField();

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
	virtual CFieldType GetFieldType() = 0;

	//根据缓冲区，生成字段信息
	//##ModelId=3CF4300801E2
	virtual int LoadFromBuffer(LPBYTE lpData);

	//得到记录内容，（如果对应的指针无效，则为空值）
	//并添加到链表中（由调用者添加）
	//返回true为读数据成功
	//false 读数据失败，指针不变
	//##ModelId=3CF3402202D0
	virtual void LoadDataFromBuffer( LPBYTE& lpData);
	
	//返回数据大小，不包括指示位,用于对文件等读时确定下一个的位置
    //lpData用于变长时，（如是字符型，指向数据）定长时为空
	//##ModelId=3CF42CC30261
	virtual int GetDataLength(LPBYTE lpData = NULL);	

	//返回数据类型的大小，在为数据链表分配新空间时确定（包含指
	//示位和数据  string类型时 为4）
	//##ModelId=3CF479B502B6
	virtual int GetTypeSize()=0;

	//##ModelId=3CF49D4200B3
	virtual bool IsNull();	

	virtual void SetNull();

	//##ModelId=3CFADAB101DE
	//virtual int SaveToBuffer_(LPBYTE lpData);
	virtual int SaveToBuffer(TCLFile *pFile);
	

	//lpData指向的数据为数据指示+数据内容,返回数据长度
	//##ModelId=3D0542FE0325
	virtual void SaveDataToBuffer(LPBYTE& lpData);

	//数据克隆，根据传入的字段样板克隆字段的基本信息
	//##ModelId=3D07E95801D4
	virtual void Clone(CField* pField);

	
	//申请新的数据空间，在数据集进行insert append等操作时，
	//先调用
	virtual void RealNewData();

	void DeleteData();

	void NewData();

	//删除m_pValue所指向的空间
	virtual void  RealDeleteData();

	//根据lpData指向的数据，复制该数据值到本字段，
    //两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
	virtual bool GetData(void*pDestArrar,int nIndex);
	virtual bool GetData(void* lpData);

	//将本字段的数据生成一个拷贝，地址空间也由本函数生成，
	//并返回该地址，用于数据传递和保存旧值
	//virtual LPBYTE SetData();

	//得到当前字段的值（m_pValue指向的）保存到文件时所需的长度，
	//包括指示位，也可能就指示位也够了，用于确定缓冲区的大小，
	//注意：大小与值相关
	virtual int QuerySaveLength();

	//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
	virtual bool GetFromDisplay(CTString str);

	
	//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
	virtual void SetDefaultValue();

	//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
	virtual bool CopyToDefaultValue();

	//将原来版本的字段数据转换到当前字段的数据。
	//1。如果本字段空间为空，生成空间
	//2。要进行原来字段空值判断！
	//2。1如果为空则本字段为空
	//2。2如果不为空则将原来的数据以原来的数据类型读出，然后调用本字段的SetAs***设置。
	//然后，如果有新的next字段，则调用该字段的ConvertToCurField函数，并且负责释放本空间，并且清0
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

