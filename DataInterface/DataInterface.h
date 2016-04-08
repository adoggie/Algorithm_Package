#ifndef _DATAINTERFACE_H
#define _DATAINTERFACE_H
#include "Define.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../TLDBDll/Database/fieldlist.h"
#include "../TLDBDll/Database/cfield.h"
#include "../cpknl/cpknl.h"
#include "../Public/common.h"
//////////////////////////////////////////////////////////////////////////////////////////
//#define nMaxMemsize 10000*50 //如果都放在内存中时,内存中最大记录条数
#include "TCDataArray.h"

//字符串和数字之间的映射函数 （数字为自动编码，从零开始)
class CTMap  
{
	CDWordArray m_dwAry;
	CPtrArray m_ptrStrAry;
	CPtrArray m_ptrNumAry;
public:
	int GetCount();
	void Clear();
	void Load(char *&pBuf);
	void Save(char *&pBuf);
	int GetBufLen();
	void Print();
	bool IndexByName(CTString strValue,int &nIndex);
	int GetInt(CTString strValue);
	CTString GetStr(int nValue);
	int Add(CTString strValue);
	CTMap();
	virtual ~CTMap();

};


/*

  管理多列 字符串和数字之间的映射函数 （数字为自动编码，从零开始)
  将字符串类型的自动更改为数字类型，并提供映射关系
  改类还提供保存功能，和装载功能。
  这样确保，进入运算后的列都为数字类型
  
*/
class AFX_EXT_CLASS CMapManager
{
	CDWordArray m_IndicateAry;//指示数组 0 数字类型  1字符类型
	CTStringArray m_strFieldNameAry;
	CPtrArray m_MapAry;
public:
	void Clear();//清空所有内容
	int GetIntValue(CTString strFieldName,CTString sValue);//根据字符串得到编码后的数字
	int GetIntValue(int nCol, CTString sValue);//根据字符串得到编码后的数字
	
	void Save(char *&pBuf);//保存到缓冲区， pBuf自动移位
	void Load(char *&pBuf);//从缓冲区装入对象， pBuf自动移位
	int GetBufLen();//得到保存时所需的长度
	CTString GetFieldName(int nCol);//得到对应列的字段名（从0开始）
	virtual ~CMapManager();
	int GetSize();//得到列数
	int IndexByName(CTString strName);//根据字段名得到索引号（从0开开始）
	bool AddFieldName(CTString strFieldName,bool IsStringCol);//追加一个字段名，后面为是否时字符类型
	
	bool HasLabel(int nIndex);//判断对应的列是否为字符类型
	bool HasLabel(CTString strFieldName);//判断对应的列是否为字符类型
	int AddValue(int nCol,CTString strValue);//往列中加字符值，返回字符值对应的数字
	CTString GetStrValue(CTString strFieldName,int nValue);//根据数字得到字符描述
	CTString GetStrValue(int nCol,int nValue);//根据数字得到字符描述

};
	




////敬告：请不要用当前类中的所有私有属性和函数 ，否则将有可能取数据出错
class AFX_EXT_CLASS CDataInterface 
{
public:
	CDataInterface();
	virtual ~CDataInterface();



//////////////////////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库操作函数
//// 数据源为天律数据库，也可以通过天律数据库访问服务器上的其它类型的数据库
//// TLDB表示天律数据库
//////////////////////////////////////////////////////////////////////////////////////////////////
private:
	CTString GetTLDBFieldName( int nIndex);
	int     GetTLDBFieldType(int nIndex);
 
	double  GetTLDBMean(long nIndex, bool bCountMissingValue=0);//求平均值
	int     GetTLDBColData(int nIndex,TDataSet* &pArray,int nGetDataType=0);
	bool    IsTLDBColEmpty(int nIndex);
	bool    GetTLDBItemValue(int nRow,int nCol,double &rtValue);
	int     GetTLDBColDataCount(int nIndex,bool bRemoveNULL=false);

public:
	CDataAccess  m_DataAccess;
protected:
	int         m_nFields,m_nRowCount,m_nColCount;//外来数据集的记录数和字段数
    TIDName    *m_pFieldStruct; //存放外来数据集的数据结构


//////////////////////////////////////////////////////////////////////////////////
////
//// 公共数据接口函数
////
//////////////////////////////////////////////////////////////////////////////////
public:
	bool getVarID(CTStringArray &strFieldAry,TIntStruct &Inar);
	static bool  GetProfileIntValue(CTString strSection,CTString strSectionKey,int &value);
	int GetColData(int nCol,TDColAry &DataAry,int nGetDataType);
	int GetColsData(CDWordArray &nColAry,TCDataMatrix& DataMatrix,int nGetDataType);
	//增加一个针对矩阵的
	int GetColsData(CDWordArray &nColAry,CDoubleMatrix& DataMatrix,int nGetDataType);
	int GetColsData(CDWordArray &nColAry,CDoubleMatrix& DataMatrix,CMapManager &Map);

	bool    Initialize();//初始化各种数据信息

	void    SetRowCount(int nRowS){m_nRowCount=nRowS;};
	long    GetRowCount();//取得行数（记录数）
	void    SetColCount(int nColS){m_nColCount=nColS;};
	int     GetColCount();//取得列数（字段数）	
	CTString GetFieldName(int nCol);//取得字段名
	bool    SetFieldName(int nCol,CTString strName);//设置字段名称    
	int     GetFieldType(int nCol);//取得字段的数据类型
	bool    SetFieldType(int nCol,int nType,int nDataStyle=-1);;//设置字段类型
	int     GetFieldWidth(int nIndex);// 取得字段的数据宽度
	bool    SetFieldWidth(int nIndex,int nWidth);//设置字段的数据宽度
	int     GetFieldDecimal(int nIndex);// 取得字段的数据小数位数
	bool    SetFieldDecimal(int nIndex,int nDecimal);// 设置字段的数据小数位数	
	int     GetFieldAlign(int nIndex);// 取得字段的数据的对齐方式
	bool    SetFieldAlign(int nIndex,int nAlign);// 设置字段的数据的对齐方式
	int     GetFieldMeasure(int nIndex);// 取得字段的数据的变量尺度类型
	bool    SetFieldMeasure(int nIndex,int nMeasure);// 设置字段的数据的变量尺度类型
	CTString GetFieldLable(int nIndex);// 取得字段的数据的注解
	bool    SetFieldLable(int nIndex,CTString strLable);// 设置字段的数据的注解
	int     GetWeightNumb();
	bool    SetWeightNumb(int nNumb);
	int GetWeightData( int nData[], int nGetType=0 );// 读取权重列数据
	int GetWeightData( double dData[] );// 读取权重列数据
	int GetWeightData( TDoubleAry &dataAry );
	int GetWeightData( TIntAry &DataAry, int nGetType );
	int GetWeightData(int nIndex);

	bool    DeleteField(int nIndex);//删除指定索引的字段

	void    SetFieldAttribute(int nDataType,CField* pDBField=NULL);//根据数据类型设置指定字段的其他属性
	bool    FitType(int nCol,int nFit=1);//判断指定的列的数据类型是否符合指定的数据类型
	bool    IsColEmpty(int nCol);//判断指定列是否为空
	long    GetColDataCount(int nCol,bool bRemoveNULL=false);//取得指定列的数据个数
	int     GetColData(int nCol,TDataSet* &pDataSet,int nGetDataType=0);//取得指定列的数据并返回其个数，pDataSet为返回的数据集
																		// 说明：方式0删除除空值
																		//       方式1对空值填均值
																		//       方式2删除空值和MissingValue填
																		//       方法3对空值和MissingValue填均值
																		//		  方式4在空值情况将TDataSet结构中iIndex赋-1表示该值缺失
																		//       方式5在空值和MissingValue情况将TDataSet结构中iIndex赋-1表示该值缺失
	int     GetTLDBColDataLabel(int nIndex,TDataSet* &pArray,int nGetDataType=0);
	bool    bHaveTLDBFieldVL(int nIndex);
	double  GetColMeanValue(int nCol, bool bCountMissingValue=false);//取得指定列（字段）的数据均值
	/////////////////////////////
	//数据预处理
	//
	//添加：QT
	//时间：2002.12.13
	//
	int Pretreatment( CDataInterface * pDI, int nType = 0 );  // 传入参数为CDataInterface
	int Pretreatment( CDataAccess& rDataAccess, int nType = 0 );     // 传入参数为CDataAccess
	int Pretreatment( int nType = 0 );						  // 对自己做预处理
	int Pretreatment( CDataAccess& rDataAccess, CTArray< int, int >& aFieldNo, int nType = 0 ); // 仅对指定的字段做预处理
	//
	/////////////////////////////


	////////////////////////////////////////
	//添加：QT
	//时间：2002.12.13
	//
	bool IsMissingValue( CField * pField, bool bCountMissingValue=false ); // 当前记录指定字段是否为缺失值
	//
	////////////////////////////////////////
	void SetShowValueLabel(bool bShow);
	bool GetShowValueLabel();

//////////////////////////////////////////////////////////////////////////////////
////
//// 内部公共属性——严禁直接调用 
////
//////////////////////////////////////////////////////////////////////////////////
	CField* FieldByIndex(int index);
	CField* FieldByName(CTString FieldName);
	int IndexByName(CTString sFieldName);
	int IndexByField(CField* pField);
	CTString NameByIndex(int nIndex);
protected:

	CTString m_strWeightCase;//当前设为权重的列号
	bool	m_bShowValueLabel;

};


extern "C" __declspec(dllexport) bool   IsEqual(TDataSet* pArray,int nCount);//判断指定数据集中数据是否全相等
extern "C" __declspec(dllexport) bool   IsEqual2(TDColAry& Array,int nCount);//判断指定数据集中数据是否全相等
extern "C" __declspec(dllexport) double GetColMeanValue(TDataSet *pColArray,int nArray);//取得指定数据集的数据均值
extern "C" __declspec(dllexport) int    GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt,TDataSet* &pGroupArray);//对指定的数据集按指定的条件进行分组，并返回分组得到的数据集及数据个数
extern "C" __declspec(dllexport) int    GetGroupDataW(TDataSet* pArray,double *pWeight, int nCount,TIntStruct tInt,TDataSet* &pGroupArray, CDoubleVector &v, CDoubleVector &w);//对指定的数据集按指定的条件进行分组，并返回分组得到的数据集及数据个数
extern "C" __declspec(dllexport) void   FindHelp(CTString strHtm);//对指定的数据集按指定的条件进行分组，并返回分组得到的数据集及数据个数

#endif