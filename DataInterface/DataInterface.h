#ifndef _DATAINTERFACE_H
#define _DATAINTERFACE_H
#include "Define.h"
#include "../TLDBDll/Database/DataAccess.h"
#include "../TLDBDll/Database/fieldlist.h"
#include "../TLDBDll/Database/cfield.h"
#include "../cpknl/cpknl.h"
#include "../Public/common.h"
//////////////////////////////////////////////////////////////////////////////////////////
//#define nMaxMemsize 10000*50 //����������ڴ���ʱ,�ڴ�������¼����
#include "TCDataArray.h"

//�ַ���������֮���ӳ�亯�� ������Ϊ�Զ����룬���㿪ʼ)
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

  ������� �ַ���������֮���ӳ�亯�� ������Ϊ�Զ����룬���㿪ʼ)
  ���ַ������͵��Զ�����Ϊ�������ͣ����ṩӳ���ϵ
  ���໹�ṩ���湦�ܣ���װ�ع��ܡ�
  ����ȷ���������������ж�Ϊ��������
  
*/
class AFX_EXT_CLASS CMapManager
{
	CDWordArray m_IndicateAry;//ָʾ���� 0 ��������  1�ַ�����
	CTStringArray m_strFieldNameAry;
	CPtrArray m_MapAry;
public:
	void Clear();//�����������
	int GetIntValue(CTString strFieldName,CTString sValue);//�����ַ����õ�����������
	int GetIntValue(int nCol, CTString sValue);//�����ַ����õ�����������
	
	void Save(char *&pBuf);//���浽�������� pBuf�Զ���λ
	void Load(char *&pBuf);//�ӻ�����װ����� pBuf�Զ���λ
	int GetBufLen();//�õ�����ʱ����ĳ���
	CTString GetFieldName(int nCol);//�õ���Ӧ�е��ֶ�������0��ʼ��
	virtual ~CMapManager();
	int GetSize();//�õ�����
	int IndexByName(CTString strName);//�����ֶ����õ������ţ���0����ʼ��
	bool AddFieldName(CTString strFieldName,bool IsStringCol);//׷��һ���ֶ���������Ϊ�Ƿ�ʱ�ַ�����
	
	bool HasLabel(int nIndex);//�ж϶�Ӧ�����Ƿ�Ϊ�ַ�����
	bool HasLabel(CTString strFieldName);//�ж϶�Ӧ�����Ƿ�Ϊ�ַ�����
	int AddValue(int nCol,CTString strValue);//�����м��ַ�ֵ�������ַ�ֵ��Ӧ������
	CTString GetStrValue(CTString strFieldName,int nValue);//�������ֵõ��ַ�����
	CTString GetStrValue(int nCol,int nValue);//�������ֵõ��ַ�����

};
	




////���棺�벻Ҫ�õ�ǰ���е�����˽�����Ժͺ��� �������п���ȡ���ݳ���
class AFX_EXT_CLASS CDataInterface 
{
public:
	CDataInterface();
	virtual ~CDataInterface();



//////////////////////////////////////////////////////////////////////////////////////////////////
////
//// �������ݿ��������
//// ����ԴΪ�������ݿ⣬Ҳ����ͨ���������ݿ���ʷ������ϵ��������͵����ݿ�
//// TLDB��ʾ�������ݿ�
//////////////////////////////////////////////////////////////////////////////////////////////////
private:
	CTString GetTLDBFieldName( int nIndex);
	int     GetTLDBFieldType(int nIndex);
 
	double  GetTLDBMean(long nIndex, bool bCountMissingValue=0);//��ƽ��ֵ
	int     GetTLDBColData(int nIndex,TDataSet* &pArray,int nGetDataType=0);
	bool    IsTLDBColEmpty(int nIndex);
	bool    GetTLDBItemValue(int nRow,int nCol,double &rtValue);
	int     GetTLDBColDataCount(int nIndex,bool bRemoveNULL=false);

public:
	CDataAccess  m_DataAccess;
protected:
	int         m_nFields,m_nRowCount,m_nColCount;//�������ݼ��ļ�¼�����ֶ���
    TIDName    *m_pFieldStruct; //����������ݼ������ݽṹ


//////////////////////////////////////////////////////////////////////////////////
////
//// �������ݽӿں���
////
//////////////////////////////////////////////////////////////////////////////////
public:
	bool getVarID(CTStringArray &strFieldAry,TIntStruct &Inar);
	static bool  GetProfileIntValue(CTString strSection,CTString strSectionKey,int &value);
	int GetColData(int nCol,TDColAry &DataAry,int nGetDataType);
	int GetColsData(CDWordArray &nColAry,TCDataMatrix& DataMatrix,int nGetDataType);
	//����һ����Ծ����
	int GetColsData(CDWordArray &nColAry,CDoubleMatrix& DataMatrix,int nGetDataType);
	int GetColsData(CDWordArray &nColAry,CDoubleMatrix& DataMatrix,CMapManager &Map);

	bool    Initialize();//��ʼ������������Ϣ

	void    SetRowCount(int nRowS){m_nRowCount=nRowS;};
	long    GetRowCount();//ȡ����������¼����
	void    SetColCount(int nColS){m_nColCount=nColS;};
	int     GetColCount();//ȡ���������ֶ�����	
	CTString GetFieldName(int nCol);//ȡ���ֶ���
	bool    SetFieldName(int nCol,CTString strName);//�����ֶ�����    
	int     GetFieldType(int nCol);//ȡ���ֶε���������
	bool    SetFieldType(int nCol,int nType,int nDataStyle=-1);;//�����ֶ�����
	int     GetFieldWidth(int nIndex);// ȡ���ֶε����ݿ��
	bool    SetFieldWidth(int nIndex,int nWidth);//�����ֶε����ݿ��
	int     GetFieldDecimal(int nIndex);// ȡ���ֶε�����С��λ��
	bool    SetFieldDecimal(int nIndex,int nDecimal);// �����ֶε�����С��λ��	
	int     GetFieldAlign(int nIndex);// ȡ���ֶε����ݵĶ��뷽ʽ
	bool    SetFieldAlign(int nIndex,int nAlign);// �����ֶε����ݵĶ��뷽ʽ
	int     GetFieldMeasure(int nIndex);// ȡ���ֶε����ݵı����߶�����
	bool    SetFieldMeasure(int nIndex,int nMeasure);// �����ֶε����ݵı����߶�����
	CTString GetFieldLable(int nIndex);// ȡ���ֶε����ݵ�ע��
	bool    SetFieldLable(int nIndex,CTString strLable);// �����ֶε����ݵ�ע��
	int     GetWeightNumb();
	bool    SetWeightNumb(int nNumb);
	int GetWeightData( int nData[], int nGetType=0 );// ��ȡȨ��������
	int GetWeightData( double dData[] );// ��ȡȨ��������
	int GetWeightData( TDoubleAry &dataAry );
	int GetWeightData( TIntAry &DataAry, int nGetType );
	int GetWeightData(int nIndex);

	bool    DeleteField(int nIndex);//ɾ��ָ���������ֶ�

	void    SetFieldAttribute(int nDataType,CField* pDBField=NULL);//����������������ָ���ֶε���������
	bool    FitType(int nCol,int nFit=1);//�ж�ָ�����е����������Ƿ����ָ������������
	bool    IsColEmpty(int nCol);//�ж�ָ�����Ƿ�Ϊ��
	long    GetColDataCount(int nCol,bool bRemoveNULL=false);//ȡ��ָ���е����ݸ���
	int     GetColData(int nCol,TDataSet* &pDataSet,int nGetDataType=0);//ȡ��ָ���е����ݲ������������pDataSetΪ���ص����ݼ�
																		// ˵������ʽ0ɾ������ֵ
																		//       ��ʽ1�Կ�ֵ���ֵ
																		//       ��ʽ2ɾ����ֵ��MissingValue��
																		//       ����3�Կ�ֵ��MissingValue���ֵ
																		//		  ��ʽ4�ڿ�ֵ�����TDataSet�ṹ��iIndex��-1��ʾ��ֵȱʧ
																		//       ��ʽ5�ڿ�ֵ��MissingValue�����TDataSet�ṹ��iIndex��-1��ʾ��ֵȱʧ
	int     GetTLDBColDataLabel(int nIndex,TDataSet* &pArray,int nGetDataType=0);
	bool    bHaveTLDBFieldVL(int nIndex);
	double  GetColMeanValue(int nCol, bool bCountMissingValue=false);//ȡ��ָ���У��ֶΣ������ݾ�ֵ
	/////////////////////////////
	//����Ԥ����
	//
	//��ӣ�QT
	//ʱ�䣺2002.12.13
	//
	int Pretreatment( CDataInterface * pDI, int nType = 0 );  // �������ΪCDataInterface
	int Pretreatment( CDataAccess& rDataAccess, int nType = 0 );     // �������ΪCDataAccess
	int Pretreatment( int nType = 0 );						  // ���Լ���Ԥ����
	int Pretreatment( CDataAccess& rDataAccess, CTArray< int, int >& aFieldNo, int nType = 0 ); // ����ָ�����ֶ���Ԥ����
	//
	/////////////////////////////


	////////////////////////////////////////
	//��ӣ�QT
	//ʱ�䣺2002.12.13
	//
	bool IsMissingValue( CField * pField, bool bCountMissingValue=false ); // ��ǰ��¼ָ���ֶ��Ƿ�Ϊȱʧֵ
	//
	////////////////////////////////////////
	void SetShowValueLabel(bool bShow);
	bool GetShowValueLabel();

//////////////////////////////////////////////////////////////////////////////////
////
//// �ڲ��������ԡ����Ͻ�ֱ�ӵ��� 
////
//////////////////////////////////////////////////////////////////////////////////
	CField* FieldByIndex(int index);
	CField* FieldByName(CTString FieldName);
	int IndexByName(CTString sFieldName);
	int IndexByField(CField* pField);
	CTString NameByIndex(int nIndex);
protected:

	CTString m_strWeightCase;//��ǰ��ΪȨ�ص��к�
	bool	m_bShowValueLabel;

};


extern "C" __declspec(dllexport) bool   IsEqual(TDataSet* pArray,int nCount);//�ж�ָ�����ݼ��������Ƿ�ȫ���
extern "C" __declspec(dllexport) bool   IsEqual2(TDColAry& Array,int nCount);//�ж�ָ�����ݼ��������Ƿ�ȫ���
extern "C" __declspec(dllexport) double GetColMeanValue(TDataSet *pColArray,int nArray);//ȡ��ָ�����ݼ������ݾ�ֵ
extern "C" __declspec(dllexport) int    GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt,TDataSet* &pGroupArray);//��ָ�������ݼ���ָ�����������з��飬�����ط���õ������ݼ������ݸ���
extern "C" __declspec(dllexport) int    GetGroupDataW(TDataSet* pArray,double *pWeight, int nCount,TIntStruct tInt,TDataSet* &pGroupArray, CDoubleVector &v, CDoubleVector &w);//��ָ�������ݼ���ָ�����������з��飬�����ط���õ������ݼ������ݸ���
extern "C" __declspec(dllexport) void   FindHelp(CTString strHtm);//��ָ�������ݼ���ָ�����������з��飬�����ط���õ������ݼ������ݸ���

#endif