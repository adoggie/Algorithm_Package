// DataInterface.cpp : Defines the entry point for the DLL application.
//
//typedef unsigned char byte  ;

#ifdef _UNIX
#include <limits>
#endif
#include "DataInterface.h"
#ifndef _UNIX
#include <limits>
#endif
//#include "htmlhelp.h"
#include "TCDataArray.h"
#ifdef WINPLAT //  Windows平台专用
	
bool __stdcall DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return true;
}

//	#define NULLValue  std::_Inf._D //空值定义//===uc 未完成
	#define NULLValue std::numeric_limits<double>::infinity() //空值定义//===uc 未完成
#else          //  Linux平台专用
	#define NULLValue (1.7976931348623158e+308)//空值定义//===uc 未完成
#endif

//using namespace std;


//判断对应的列是否为字符类型
bool CMapManager::HasLabel(int nIndex)
{
	return m_IndicateAry.GetAt(nIndex);
}
//判断对应的列是否为字符类型
bool CMapManager::HasLabel(CTString strFieldName)
{
	int nIndex = IndexByName(strFieldName);
	if (nIndex==-1)
		return false;
	return m_IndicateAry.GetAt(nIndex);
}

int CMapManager::AddValue(int nCol,CTString strValue)
{
	CTMap*pMap = (CTMap*)m_MapAry.GetAt(nCol);
	if (pMap==NULL)
		return -1;
	//return pMap->GetInt(strValue);
	return pMap->Add(strValue); //modify by xds 20060728
}
//根据数字得到字符描述
CTString CMapManager::GetStrValue(CTString strFieldName,int nValue)
{
	int nIndex = IndexByName(strFieldName);
	if (nIndex==-1)
		return "";
	return GetStrValue(nIndex, nValue);
}
//根据数字得到字符描述
CTString CMapManager::GetStrValue(int nCol,int nValue)
{
	if (nCol<0 || nCol>=m_MapAry.GetSize())
		return "";

	CTMap*pMap = (CTMap*)m_MapAry.GetAt(nCol);
	if (pMap==NULL)
		return "";
	return pMap->GetStr(nValue);
}

//追加一个字段名，后面为是否时字符类型
bool CMapManager::AddFieldName(CTString strFieldName,bool IsStringCol)
{
	if (IndexByName(strFieldName)!=-1)
		return false;
	m_strFieldNameAry.Add(strFieldName);
	m_IndicateAry.Add(IsStringCol);
	if (IsStringCol)
		m_MapAry.Add(new CTMap());
	else
		m_MapAry.Add(NULL);
	return true;
}

//根据字段名得到索引号（从0开开始）
int CMapManager::IndexByName(CTString strName)
{
	for (int i=0;i<m_strFieldNameAry.GetSize();i++)
	{
		if (m_strFieldNameAry.GetAt(i) == strName)
			return i;
	}
	return -1;
}
//得到列数
int CMapManager::GetSize()
{
	return m_IndicateAry.GetSize();
}

CMapManager::~CMapManager()
{
	Clear();
}


CTMap::CTMap()
{

}

CTMap::~CTMap()
{
	Clear();
}

int CTMap::Add(CTString strValue)
{
	int nIndex;
	if (IndexByName(strValue,nIndex))
	{
		return m_dwAry.GetAt(nIndex);
	}
	CTString *pStr = new CTString(strValue);
	m_ptrStrAry.InsertAt(nIndex,pStr);
	int rst = m_ptrNumAry.GetSize();
	m_dwAry.InsertAt(nIndex,rst);
	m_ptrNumAry.Add(pStr);
	return rst;

}

CTString CTMap::GetStr(int nValue)
{
	CTString *pStr = (CTString*)m_ptrNumAry.GetAt(nValue);
	return *pStr;
}

int CTMap::GetInt(CTString strValue)
{
	int nValue;
	if (IndexByName(strValue,nValue))
	{
		return m_dwAry.GetAt(nValue);
	}
	else
		return -1;//出错

}

bool CTMap::IndexByName(CTString strValue,int &nIndex)
{
	int nLargeIndex = m_ptrStrAry.GetSize()-1;
	int nSmallIndex = 0;
	int nTemp = 0;
	while (nLargeIndex >= nSmallIndex)
	{
		nTemp = (nLargeIndex + nSmallIndex)/2;
		int zzz = strcmp((const char*) strValue,(const char*)*(CTString*)m_ptrStrAry.GetAt(nTemp));
		if (zzz ==0)
		{
			nIndex = nTemp;
			return true;
		}

		if (zzz < 0)
		//if(pPage <= m_ptrStrAry.GetAt(nTemp))
		{//在前面的页中
			nLargeIndex = nTemp-1;
			continue; 
		}
		else
		{//在后面的页中
			nSmallIndex = nTemp+1;
			continue ;
		}
	}

	nIndex = nSmallIndex;
	return false;
}

void CTMap::Print()
{
//	int nSize = m_ptrStrAry.GetSize();
//	TRACE("******  总大小：%d\r\n",nSize);
//	for (int i=0;i<nSize ;i++)
//	{
//		int v1 = i;
//		CTString *pStr = (CTString *)m_ptrNumAry.GetAt(i);
//		int nIndex =0;
//		if (IndexByName(*pStr,nIndex))
//		{
//			int kkk = m_dwAry.GetAt(nIndex);
//			if (kkk!= v1)
//				TRACE("XXX V1:%d STR:%s %d\r\n",v1,*pStr,kkk);
//			else
//				TRACE("V1:%d\t%d\tSTR:%s\r\n",v1,kkk,*pStr);
//		}
////		else
////		{
////			TRACE("XXXXX  Error V1:%d STR:%s\r\n",v1,*pStr);
////		}
//	}
//// 	TRACE("---------\r\n",nSize);
//	for ( i=0;i<nSize ;i++)
//	{
//		int v1 = i;
//		CTString *pStr = (CTString *)m_ptrStrAry.GetAt(i);
//
//		{
//			int kkk = GetInt(*pStr);
//			CTString strOut = GetStr(kkk);
//// 			TRACE("V1:%d\tSTR:%10s\t%d\t%d>>:%s\r\n",v1,*pStr,kkk,(*pStr)==strOut,strOut);
//		}
//
//	}
}

int CTMap::GetBufLen()
{
	int nSize = m_ptrNumAry.GetSize();
	int nNum = sizeof(int);
	for (int i=0;i<nSize ;i++)
	{
		CTString*pStr = (CTString*)m_ptrStrAry.GetAt(i);
		nNum = nNum + pStr->GetLength()+sizeof(int);
	}
	return nNum;
}

void CTMap::Save(char *&pBuf)
{
	int nSize = m_ptrNumAry.GetSize();
	memcpy((void*)pBuf,&nSize,sizeof(int));
	pBuf = pBuf+ sizeof(int);
	for (int i=0;i<nSize ;i++)
	{
		CTString*pStr = (CTString*)m_ptrNumAry.GetAt(i);
		int nLen = pStr->GetLength();
		memcpy((void*)pBuf,&nLen,sizeof(int));
		pBuf = pBuf+ sizeof(int);

		memcpy((void*)pBuf,(const char*)*pStr,pStr->GetLength());
		pBuf = pBuf+ pStr->GetLength();
	}

}

void CTMap::Load(char *&pBuf)
{
	int nSize ;
	memcpy(&nSize,(void*)pBuf,sizeof(int));
	pBuf = pBuf+ sizeof(int);
	for (int i=0;i<nSize ;i++)
	{
		CTString str;
		int nLen ;
		memcpy(&nLen,(void*)pBuf,sizeof(int));
		pBuf = pBuf+ sizeof(int);
		memset(str.GetData(),0,nLen+1);
		memcpy((void*)(const char*)str,(void*)pBuf,nLen);
		str.ReleaseBuffer();
		Add(str);
		pBuf = pBuf+ nLen;
	}
}

void CTMap::Clear()
{
	int nSize = m_ptrStrAry.GetSize();
	for (int i=0;i<nSize;i++)
	{
		CTString *pStr = (CTString*)m_ptrStrAry.GetAt(i);
		delete pStr;
	}
	m_ptrStrAry.RemoveAll();
	m_ptrNumAry.RemoveAll();
	m_dwAry.RemoveAll();
}

int CTMap::GetCount()
{
	return m_ptrStrAry.GetSize();
}

//得到对应列的字段名（从0开始）
CTString CMapManager::GetFieldName(int nCol)
{
	if (nCol<0 || nCol>=GetSize())
		return "";
	return m_strFieldNameAry.GetAt(nCol);
}
//得到保存时所需的长度
int CMapManager::GetBufLen()
{
	int nSize = GetSize();
	int nLen = sizeof(int);

	int i=0;
	for ( i=0;i<nSize;i++)
	{
		nLen = nLen+sizeof(int);
	}
	
	for ( i=0;i<nSize;i++)
	{
		CTString str = m_strFieldNameAry.GetAt(i);
		int nV = str.GetLength();
		nLen = nLen+sizeof(int)+ nV;
	}

	for ( i=0;i<nSize;i++)
	{
		CTMap*pMap = (CTMap*)m_MapAry.GetAt(i);
		if (pMap)
		{
			int nV = pMap->GetBufLen();
			nLen = nLen+ nV;
		}
		else
		{
			nLen = nLen+sizeof(int);
		}

	}
	return nLen;
}

void CMapManager::Load(char *&pBuf)
{
	Clear();

	int nSize = 0;
	memcpy(&nSize,pBuf,sizeof(int));
	pBuf = pBuf+sizeof(int);
	int i=0;
	for (i=0;i<nSize;i++)
	{
		int nV = 0;
		memcpy(&nV,pBuf,sizeof(int));
		pBuf = pBuf+sizeof(int);
		m_IndicateAry.Add(nV);
	}
	for ( i=0;i<nSize;i++)
	{
		CTString str = m_strFieldNameAry.GetAt(i);
		int nV = 0;
		memcpy(&nV,pBuf,sizeof(int));
		pBuf = pBuf+sizeof(int);
		memset(str.GetData(),0,nV+1);
		memcpy(str.GetData(),pBuf,nV);
		str.ReleaseBuffer();
		pBuf = pBuf+nV;
	}
	for ( i=0;i<nSize;i++)
	{
		int nV = 0;
		memcpy(&nV,pBuf,sizeof(int));
		pBuf = pBuf+sizeof(int);
		if (nV!=0)
		{
			CTMap*pMap = new CTMap();
			pMap->Load(pBuf);
			m_MapAry.Add(pMap);
		}
		else
			m_MapAry.Add(NULL);
	}
}

void CMapManager::Save(char *&pBuf)
{
	int nSize = GetSize();
	memcpy(pBuf,&nSize,sizeof(int));
	pBuf = pBuf+sizeof(int);
	int i=0;
	for (i=0;i<nSize;i++)
	{
		int nV = m_IndicateAry.GetAt(i);
		memcpy(pBuf,&nV,sizeof(int));
		pBuf = pBuf+sizeof(int);
	}
	for ( i=0;i<nSize;i++)
	{
		CTString str = m_strFieldNameAry.GetAt(i);
		int nV = str.GetLength();
		memcpy(pBuf,&nV,sizeof(int));
		pBuf = pBuf+sizeof(int);
		memcpy(pBuf,(const char*) str,nV);
		pBuf = pBuf+nV;
	}
	for ( i=0;i<nSize;i++)
	{
		CTMap*pMap = (CTMap*)m_MapAry.GetAt(i);
		if (pMap)
		{
			int nV = pMap->GetBufLen();
			memcpy(pBuf,&nV,sizeof(int));
			pBuf = pBuf+sizeof(int);
			pMap->Save(pBuf);
		}
		else
		{
			int nV = 0;
			memcpy(pBuf,&nV,sizeof(int));
			pBuf = pBuf+sizeof(int);
		}

	}
}
//根据字符串得到编码后的数字
int CMapManager::GetIntValue(CTString strFieldName, CTString sValue)
{
	int nIndex = IndexByName(strFieldName);
	if (nIndex==-1)
		return -1;
	return GetIntValue(nIndex, sValue);
}
//根据字符串得到编码后的数字
int CMapManager::GetIntValue(int nCol, CTString sValue)
{
	if (nCol<0 || nCol>=m_MapAry.GetSize())
		return -1;
	CTMap*pMap = (CTMap*)m_MapAry.GetAt(nCol);
	if (pMap==NULL)
		return -1;
	return pMap->GetInt(sValue);
}
//清空所有内容
void CMapManager::Clear()
{
	for (int i=0;i<m_MapAry.GetSize();i++)
	{
		CTMap*pMap = (CTMap*)m_MapAry.GetAt(i);
		if (pMap)
			delete pMap;
	}

	m_MapAry.RemoveAll();
	m_IndicateAry.RemoveAll();
	m_strFieldNameAry.RemoveAll();
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDataInterface::CDataInterface()
{
	m_pFieldStruct=NULL;

	m_nFields=m_nRowCount=m_nColCount=0;


	m_strWeightCase="";

	m_bShowValueLabel=false;
}

CDataInterface::~CDataInterface()
{
	Initialize();
}

/// cwh 2002-9-16 
//根据指定的方式进行初始化
bool CDataInterface::Initialize()
{  
	m_nFields=0;//外来数据集的记录数和字段数
	m_nRowCount=0;
	m_nColCount=0;
	if(m_pFieldStruct)//存放外来数据集的数据结构
		delete [] m_pFieldStruct;

	m_pFieldStruct=NULL;


	m_strWeightCase="";

	return true;
}



////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 一般数据库的操作函数 
///// cwh 2002-9-12 (Begin)  
///////////////////////////////////////////////////////////////////////////////////////////
/*
////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
///nDbProvider为连接的数据库类型，0表示Oracle 数据库,SQL Server 数据库
///strServer为数据库的服务器地址，strUserID为用户ID，strPassWord为用户密码
///strDBName在连接Oracle 数据库时，可以为空
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::ConnectServer(int nDbProvider,CTString strServer,CTString strUserID,CTString strPassWord,CTString strDBName)
{
	//初始化
	if (!m_RstAdoConn)
	{
		m_RstAdoConn = m_pConnection.CreateInstance ("ADODB.CONNECTION");
		if (!SUCCEEDED (m_RstAdoConn))
		{
			m_bConnOpen = false;
			return false;
		}
	}

	//连接数据库
	_bstr_t strConn=_T("");
	switch (nDbProvider)
	{
	case 0:	// Oracle 数据库;
		strConn = strConn + "Provider=MSDAORA.1;Password=";
		strConn = strConn + strPassWord + ";User ID=" + strUserID+";Data Source=" + strServer;
		break;
	case 1:	// SQL Server 数据库;
		strConn = strConn + "Provider=SQLOLEDB.1;Password=";
		strConn = strConn + strPassWord + ";User ID=" + strUserID + ";Data Source=" + strServer;
		strDBName.TrimRight ();	
		strConn = strConn + ";Initial Catalog=" + strDBName;
		break;
	default:
		return false;
	}

	try
	{
		m_RstAdoConn = m_pConnection->Open (strConn,"","",adModeUnknown);
	}
	catch (_com_error e)
	{
		CTString es = e.ErrorMessage ();
		if (es == "IDispatch error #3149")
			es.Format ("数据库连接失败:\n\n%s", "用户名或密码有错，请重新联接。");
		else
		{
			es = "数据库连接失败:\n\n";
			es += "服务器名错或";
			es += e.ErrorMessage ();
		}
		AfxMessageBox (es, MB_ICONERROR + MB_OK);
	}

	bool bConnection=m_pConnection->State;
	if (bConnection)
	{
		m_pCmd.CreateInstance ("ADODB.Command");
		m_pCmd->ActiveConnection = m_pConnection;
	}
	return bConnection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
//打开一个指定的表单
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::OpenTable(CTString strTable)
{
	if(strTable.IsEmpty() || !SUCCEEDED(m_RstAdoConn))
		return false;

	m_strTable=strTable;
	m_pRecordset=Select();
	m_bConnOpen=m_pRecordset?true:false;

	return m_bConnOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
/// 取得当前数据表中有字段数
////////////////////////////////////////////////////////////////////////////////////////////////////
long  CDataInterface::GetDBFieldCount()
{
	return m_bConnOpen?m_pRecordset->Fields->GetCount():0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
/// 取得当前数据表中有记录数
////////////////////////////////////////////////////////////////////////////////////////////////////
long  CDataInterface::GetDBRecordCount()
{
	return m_bConnOpen?m_pRecordset->GetRecordCount():0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
/// 在当前数据表中取得指定字段的数据类型
////////////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetDBFieldType(int nCol)
{
	if(m_bConnOpen)
	{
		_variant_t pIndex;
		pIndex.intVal=nCol;
		FieldPtr pFields=m_pRecordset->Fields->GetItem(pIndex);
		return pFields->GetType();
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
/// 在当前数据表中取得指定字段的名称
////////////////////////////////////////////////////////////////////////////////////////////////////
CTString CDataInterface::GetDBFieldName(int nCol)
{
	if(m_bConnOpen && nCol>=0 && nCol<GetDBFieldCount())
	{
		FieldPtr pFieldS;
		_variant_t pIndex;
		pIndex.intVal=nCol;
		pFieldS=m_pRecordset->Fields->GetItem(pIndex);
		return pFieldS->Value.bstrVal;
	}

	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
///在当前数据表中取得指定单元（第nRow行,第nCol列）的数据值
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::GetDBFieldValue(long nRow,int nCol,_variant_t vtValue)
{
	if(m_bConnOpen)
	{
		_variant_t vrtIndex;
		vrtIndex.intVal=nRow;
		m_pRecordset->Move(vrtIndex);
		vrtIndex.intVal=nCol;
		vtValue=m_pRecordset->GetCollect(vrtIndex);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// 一般数据库
/// cwh 2002-9-12
///nCol为字段索引,-1表示取出全部字段，nOrderType表示按什么排序，0表示不排序
////////////////////////////////////////////////////////////////////////////////////////////////////
_RecordsetPtr CDataInterface::Select(int nCol,int nOrderType)
{
	if(m_strTable.IsEmpty())
		m_bConnOpen=false;

	if(m_bConnOpen || (nCol==-1 && nOrderType==0 && SUCCEEDED(m_RstAdoConn)))
	{
    	_RecordsetPtr pRecorset;
		pRecorset.CreateInstance("ADODB.Recordset");
		_bstr_t strCommand="SELECT ";
		if(nCol==-1)
		{
			strCommand+="* FROM ";
			strCommand+=(_bstr_t)m_strTable;
		}
		else
		{
			CTString strFieldName=GetDBFieldName(nCol);
			strCommand+=(_bstr_t)strFieldName;
			strCommand+=" FROM ";
			strCommand+=(_bstr_t)m_strTable;
			if(nOrderType!=0)
			{
				strCommand+=" ORDER BY ";
				strCommand+=(_bstr_t)strFieldName;
				strCommand+=(nOrderType==1)?" ASCE":" DESC";				
			}
		}
		m_pCmd->CommandText = strCommand;
		pRecorset = m_pCmd->Execute (NULL, NULL, adCmdText);
		return pRecorset;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-9-12
/// 一般数据库
/// 取得多列数据，其它变量意义同上
////////////////////////////////////////////////////////////////////////////////////////////////////
_RecordsetPtr CDataInterface::Select(int *pColS,int nCount,int nOrderType,int nOrderByCol)
{
	int nFieldS=GetDBFieldCount();
	if(pColS==NULL || nCount<=0 || nCount>nFieldS 
		|| nOrderByCol<0 || nOrderByCol>=nFieldS)
		return NULL;

	if(m_bConnOpen )
	{
		CTString strFieldName="";
		_RecordsetPtr pRecorset;
		pRecorset.CreateInstance("ADODB.Recordset");
		_bstr_t strCommand="SELECT ";
		for(int ii=0;ii<nCount;ii++)
		{
			strCommand+=(_bstr_t)GetDBFieldName(ii);
			strCommand+=(ii<nCount-1)?",":" ";
		}
		strCommand+=(_bstr_t)m_strTable;
		if(nOrderType!=0)
		{
			strCommand+=" ORDER BY ";
			strCommand+=(_bstr_t)GetDBFieldName(nOrderByCol);
			strCommand+=(nOrderType==1)?" ASCE":" DESC";				
		}
		m_pCmd->CommandText = strCommand;
		pRecorset = m_pCmd->Execute (NULL, NULL, adCmdText);
		return pRecorset;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 一般数据库的操作函数 
/////     (End)  
///////////////////////////////////////////////////////////////////////////////////////////
*/










////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-18
//根据指定的列索引或列ID号返回字段的名称（即数据列名称）
//nIndex为指定的列索引或列ID号，IsID表示当前是不是根据ID号取值
////////////////////////////////////////////////////////////////////////////////////////////
/*CTString  CDataInterface::GetGrdFieldName( int nIndex,bool IsID)
{
	CGrdField* pField=m_cFieldS.Item(nIndex);
	return pField?pField->GetFieldName():"";
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
// 取得指定字段的数据类型
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetGrdFieldType(int nIndex)
{
	CGrdField* pField=m_cFieldS.Item(nIndex);
	return pField?pField->GetFieldType():0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-18
//取得当前数据源中字段数（即数据列数） 
////////////////////////////////////////////////////////////////////////////////////////////
long   CDataInterface::GetGrdColCount()
{
	return m_nColCount;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-18
//取得当前数据源中记录数（即数据行数） 
////////////////////////////////////////////////////////////////////////////////////////////
long  CDataInterface::GetGrdRowCount()  
{
	return m_nRowCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//cwh 2002-8-19
//判断指定列是否为空（即该列没有有用的数据）
//nIndex为指定的列号
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::IsGrdColEmpty(int nIndex)
{
	if(nIndex<=0 || nIndex>m_nColCount)
		return true;

	return m_pDataGrid?!m_pDataGrid->BeColHaveData(nIndex):true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-19
//取得指定列的有效的数据个数（即不为空值的数据个数）
//nIndex为指定的列号
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetGrdColDataCount(int nIndex,bool bRemoveNULL)
{
	if(nIndex<=0 || nIndex>m_nColCount)
		return 0;

	return m_pDataGrid?(!bRemoveNULL?m_pDataGrid->GetColDataCount(nIndex):m_nRowCount):0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-20
//取得指定列的有效的数据个数（即不为空值的数据个数）
//nIndex为指定的列号
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::GetGrdItemValue(int nRow,int nCol,double &rtValue)
{
	if(nRow<0 || nRow>=m_nRowCount || nCol<=0 || nCol>m_nColCount)
		return false;
	
	CTString string=m_pDataGrid->GetItemText(nRow,nCol);
	if(!string.IsEmpty())
	{
		rtValue=atof(string);
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-20
//取得指定列的有效的数据的均值（即不为空值的数据均值）
//nIndex为指定的列号
////////////////////////////////////////////////////////////////////////////////////////////
double CDataInterface::GetGrdMean(long nIndex)
{
	if(nIndex>m_nColCount || nIndex<=0)
		return 0;

	int count=0,
		nFromRow=m_pDataGrid->GetFixedRowCount();
	double sumValue=0,value=0;
	if(m_pDataGrid)
		for(int ii=nFromRow;ii<=m_nRowCount;ii++)
		{
			if(GetGrdItemValue(ii,nIndex,value))
			{
				count++;
				sumValue+=value;
			}
		}
		
	return count==0?0:sumValue/count;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 本地表单控件数据源 
//cwh 2002-8-20
//取得指定列的数据,并返回数据个数
//nIndex为指定的列号，nGetDataType为当前数据为空时，指定数据的处理方式
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetGrdColData(int nIndex,TDataSet* &pArray,int nGetDataType)
{
    //如果从数据表单上直接取数据，因字段集合和数据集合相差一列，（即索引列），所以要加上一

	if(nIndex>m_nColCount || nIndex<=0)
		return 0;

	CTString string="";
	int ii,index=0,count,
		nFromRow=m_pDataGrid->GetFixedRowCount(),
		nType=GetGrdFieldType(nIndex-1);
	if(nType<0)
		return 0;
	switch(nGetDataType)
	{
	case 0:		
		count=GetGrdColDataCount(nIndex);//取得指定字段中有效数据的个数
		pArray =new TDataSet[count];
		for(ii=nFromRow;ii<=m_nRowCount;ii++)
		{
			string=m_pDataGrid->GetItemText(ii,nIndex);//加入数据库的取值方法
			if(nType==fString)
			{
				pArray[index].strVal=string;
				pArray[index++].iIndex=ii+1;
			}
			else
			{
				string.TrimLeft(' ');
				if(!string.IsEmpty())
				{
					pArray[index].iIndex=ii;
					pArray[index].strVal=string;/////////////// zkc 2002-11-15 为行列转换目的修改
					pArray[index++].dValue=atof(string);
				}
			}
			

		
		}
		break;
	case 1:
		{
			count=m_nRowCount;
			pArray =new TDataSet[count];
			double dMeanValue=GetGrdMean(nIndex);
			for(ii=nFromRow;ii<=m_nRowCount;ii++)
			{
				string=m_pDataGrid->GetItemText(ii,nIndex);//加入数据库的取值方法
				if(nType==fString)
					pArray[index].strVal=string;
				else
				{
					string.TrimLeft(' ');
					if(!string.IsEmpty())
						pArray[index].dValue=atof(string);
					else
						pArray[index].dValue=dMeanValue;
					pArray[index].strVal=string;////// zkc 2002-11-15 为行列转换目的修改
				}
				pArray[index++].iIndex=ii;
			}
		}
		break;
	default:
		return 0;
	}
	
	return count;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 本地表单控件数据源的操作函数 
///// cwh 2002-8-6 (End)  
///////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 天律数据库的操作函数 
///// cwh 2002-8-6 (Begin)  
///////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得指定字段的名称
/////////////////////////////////////////////////////////////////////////////////
CTString CDataInterface::GetTLDBFieldName( int nIndex)
{
	if(nIndex<0 || nIndex>=m_DataAccess.m_pFieldList->GetFieldCount())
		return "";
	CField*pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);

	if(pField)
		return pField->GetFieldName();
	
	return "";
}

////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得指定字段的数据类型
/////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetTLDBFieldType(int nIndex)
{
	if(nIndex<0 || nIndex>=m_DataAccess.m_pFieldList->GetFieldCount())
		return 0;
	CField*pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);

	if(pField)
		return pField->GetFieldType();
	
	return 0;
}

//@ 功能：判别 该列 是否存在标签值
//@ 返回：若存在返回true,否则false
//@ author：梅肖明 2004-10-9
bool CDataInterface::bHaveTLDBFieldVL(int nIndex)
{
	if(nIndex<0 || nIndex>=m_DataAccess.m_pFieldList->GetFieldCount())
		return false;
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);

	if(pField){
	   return pField->bHaveValueLabel();
	}
	
	return false;
}




////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得天律数据库中指定字段的平均值
/////////////////////////////////////////////////////////////////////////////////
double CDataInterface::GetTLDBMean( long nIndex, bool bCountMissingValue )
{
	int nFieldType=GetTLDBFieldType(nIndex);
	if(nFieldType==fString || nFieldType<=0)
		return 0;

	double dSum=0;
	long nCount=0,nRecord=m_DataAccess.GetRecordCount();
	m_DataAccess.First();
	for(long i=0;i<nRecord;i++)
	{
		CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
		
		if(!IsMissingValue( pField, bCountMissingValue ))
		{
			nCount++;
			dSum+=pField->GetAsDouble();
		}
		if (m_DataAccess.GetRecordCount()==m_DataAccess.GetRecNo())
			return nCount==0?0:dSum/nCount;
		m_DataAccess.Next();		
	}
	
	return nCount==0?0:dSum/nCount;
}


////////////////////////////////////////////////////////////////////////////////
//// 天律数据库
//// 取得天律数据库中指定字段的所有数据，nGetDataType为取数据方式
//// 复制GetTLDBColData(..)后而改写: 若有标签值取标签值
///  说明：方式0删除除空值
///       方式1对空值填均值
///       方式2删除空值和MissingValue
///       方法3对空值和MissingValue填均值
///		  方式4在空值情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
///       方式5在空值和MissingValue情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
////author: 梅肖明 2002-10-14
/////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetTLDBColDataLabel(int nIndex,TDataSet* &pArray,int nGetDataType)
{
	int nCount=0,nFieldType=fString;//GetTLDBFieldType(nIndex);
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0 || nGetDataType>5 || nGetDataType<0 || nFieldType<=0)
		return 0;

	double dMeanValue=0;
	if(nGetDataType==0||nGetDataType==2)
	{
		long nCount=GetTLDBColDataCount(nIndex,true);
		if(nCount>0)
			pArray=new TDataSet[nCount];
		else
			return 0;
	}
	else
	{
		if(nRecord<=0)
			return 0;
		pArray=new TDataSet[nRecord];
		if( nGetDataType==1||nGetDataType==3 )
			dMeanValue=GetTLDBMean(nIndex, nGetDataType==3);
	}


	nCount=0;
	m_DataAccess.First();
	int nRowIndex=1;
	while(!m_DataAccess.Eof())
	{
		CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
		//////////////{{{{
		//pArray[nCount].strVal=pField->GetValueLable();
		pArray[nCount].strVal= pField->GetValLable();//add by xds 20060608
		if(pArray[nCount].strVal!=""){
			pArray[nCount++].iIndex=nRowIndex;
		}
		else {
		//////////////}}}}
		
		if(!IsMissingValue( pField, nGetDataType==2||nGetDataType==3||nGetDataType==5 ))//(pField->IsNull()))
		{
			if(nFieldType==fString)
				pArray[nCount].strVal=pField->GetAsString();
			else
			{
				pArray[nCount].dValue=pField->GetAsDouble();
			}			
			pArray[nCount++].iIndex=nRowIndex;
		}
		else
		{
			switch( nGetDataType )
			{
			case 1:
			case 3:
				if(nFieldType==fString)
					pArray[nCount].strVal="";
				else
				{
					pArray[nCount].dValue=dMeanValue;
					pArray[nCount].strVal=""; 		
				}
				pArray[nCount++].iIndex = -nRowIndex;
				break;
			case 4:
			case 5:
				if(nFieldType==fString)
					pArray[nCount].strVal="";
				pArray[nCount++].iIndex = -nRowIndex;
				break;
			default:
				break;
			}
		}	
		}

		nRowIndex++;
		m_DataAccess.Next();	
	}

	return nCount;
}

////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得天律数据库中指定字段的所有数据，nGetDataType为取数据方式
///
///
/// 修改 QT 2003.01.04
/// 目的：添加取数据方式2和3
/// 说明：方式0删除除空值
///       方式1对空值填均值
///       方式2删除空值和MissingValue填
///       方法3对空值和MissingValue填均值
///		  方式4在空值情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
///       方式5在空值和MissingValue情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
/////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetTLDBColData(int nIndex,TDataSet* &pArray,int nGetDataType)
{
	int nCount=0,nFieldType=GetTLDBFieldType(nIndex);
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0 || nGetDataType>5 || nGetDataType<0 || nFieldType<=0)
		return 0;

	double dMeanValue=0;
	if(nGetDataType==0||nGetDataType==2)
	{
		long nCount=GetTLDBColDataCount(nIndex,true);
		if(nCount>0)
			pArray=new TDataSet[nCount];
		else
			return 0;
	}
	else
	{
		if(nRecord<=0)
			return 0;
		pArray=new TDataSet[nRecord];
		if( nGetDataType==1||nGetDataType==3 )
			dMeanValue=GetTLDBMean(nIndex, nGetDataType==3);
	}


	nCount=0;
	m_DataAccess.First();
	int nRowIndex=1;
	while(!m_DataAccess.Eof())
	{
		CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
		if(!IsMissingValue( pField, nGetDataType==2||nGetDataType==3||nGetDataType==5 ))//(pField->IsNull()))
		{
			if(nFieldType==fString)
				pArray[nCount].strVal=pField->GetAsString();
			else
			{
				pArray[nCount].dValue=pField->GetAsDouble();
//				pArray[nCount].strVal=pField->SetToDisplay();//zkc 2002-11-15 修改目的为了行列转化
			}			
			pArray[nCount++].iIndex=nRowIndex;
		}
		else
		{
			switch( nGetDataType )
			{
			case 1:
			case 3:
				if(nFieldType==fString)
					pArray[nCount].strVal="";
				else
				{
					pArray[nCount].dValue=dMeanValue;//
					pArray[nCount].strVal=""; ///kc 2002-11-15 修改目的为了行列转化			
				}
				pArray[nCount++].iIndex = -nRowIndex;
				break;
			case 4:
			case 5:
				if(nFieldType==fString)
					pArray[nCount].strVal="";
				pArray[nCount++].iIndex = -nRowIndex;
				break;
			default:
				break;
			}
		}
/*			if(nGetDataType==1)
			{
				if(nFieldType==fString)
					pArray[nCount].strVal="";
				else
				{
					pArray[nCount].dValue=dMeanValue;//
					pArray[nCount].strVal=""; ///kc 2002-11-15 修改目的为了行列转化			
				}
				pArray[nCount++].iIndex=nRowIndex;
			}
*/
		nRowIndex++;
		m_DataAccess.Next();	
	}

	return nCount;
}

////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 判断天律数据库中指定字段的数据是否为空
/////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::IsTLDBColEmpty(int nIndex)
{
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0)
		return true;
	m_DataAccess.First();
	for(long i=0;i<nRecord;i++)
	{
		if(!(m_DataAccess.m_pFieldList->FieldByIndex(nIndex)->IsNull()))
			return false;
		if (m_DataAccess.GetRecordCount()==m_DataAccess.GetRecNo())
			return true;
		m_DataAccess.Next();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得天律数据库中指定行列的数据
/////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::GetTLDBItemValue(int nRow,int nCol,double &rtValue)
{
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=nRow)
		return false;
	m_DataAccess.SetCurRecNo(nRow);
	if((m_DataAccess.m_pFieldList->FieldByIndex(nCol)->IsNull()))
			return false;

	rtValue=m_DataAccess.m_pFieldList->FieldByIndex(nCol)->GetAsDouble();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////
//// 天律数据库
//// cwh 2002-10-14
//// 取得天律数据库中指定字段的数据个数，bRemoveNULL 表示是否要除去空值
/////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetTLDBColDataCount(int nIndex,bool bRemoveNULL)
{
	long nRecord=m_DataAccess.GetRecordCount();
	if(!bRemoveNULL || nRecord<=0)
		return nRecord;

	long nCount=0;
	m_DataAccess.First();
	for(long i=0;i<nRecord;i++)
	{
		if(!(m_DataAccess.m_pFieldList->FieldByIndex(nIndex)->IsNull()))
			nCount++;
		if (m_DataAccess.GetRecordCount()==m_DataAccess.GetRecNo())
			return nCount;
		m_DataAccess.Next();
	}

	return nCount;
}

////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 天律数据库的操作函数 
///// cwh 2002-8-6 (End)  
////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
/////
///// 公共数据接口
///// cwh 2002-9-16 (Begin)  
////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 取得列数（字段数）
////////////////////////////////////////////////////////////////////////////////////////////
int  CDataInterface::GetColCount()
{
	return m_DataAccess.m_pFieldList->GetFieldCount();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 取得行数（记录数）
////////////////////////////////////////////////////////////////////////////////////////////
long CDataInterface::GetRowCount()
{
	return m_DataAccess.GetRecordCount();
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-16
/// 取得字段名
////////////////////////////////////////////////////////////////////////////////////////////
CTString CDataInterface::GetFieldName(int nCol)
{
	return GetTLDBFieldName(nCol);

}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段名称   
////////////////////////////////////////////////////////////////////////////////////////////
bool  CDataInterface::SetFieldName(int nCol,CTString strName) 
{
	CField* pField=NULL;
	pField=m_DataAccess.m_pFieldList->FieldByIndex(nCol);
	if(pField && pField->GetFieldName()!=strName)
		pField->SetFieldName(strName);
	else
		return false;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13 c
/// cwh 2002-9-16 m
/// 取得字段的数据类型
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetFieldType(int nCol)
{
	return GetTLDBFieldType(nCol);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段类型
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldType(int nCol,int nType,int nDataStyle)
{
	CField* pNewField=NULL,
		*pField=m_DataAccess.m_pFieldList->FieldByIndex(nCol);
	
	pNewField = m_DataAccess.m_pFieldList->CreateField(nType);
	if(pField)
	{
		pNewField->CopyCommonProperty(pField);
		if(nDataStyle>0  && nType==fDate)
			pNewField->SetDateConvertType(nDataStyle);
	}
	
	SetFieldAttribute(nType,pNewField);
	
	if(pField)
		m_DataAccess.m_pFieldList->Modify(nCol,pNewField);
	else
		m_DataAccess.m_pFieldList->Insert(pNewField,nCol);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////
/////
//cwh 2002-8-21
//根据数据类型nDataType设置指定字段pGrdField的其他属性
////////////////////////////////////////////////////////////////////////////////////
void CDataInterface::SetFieldAttribute(int nDataType,CField* pDBField)
{
	if(pDBField==NULL)
		return;
	
	int nWidth=pDBField->GetWidth(),
		nDecimal=pDBField->m_nDecimals;
	switch(nDataType)
	{
	case fDate:
		nWidth=12;
		nDecimal = 0;
		break;
	case fBoolean:
		nWidth=2;
		nDecimal = 0;
		break;
	case fDouble:
		if(nWidth>16)
			nWidth=16;
		if(nWidth<8)
			nWidth=8;
		////////////////////////////{{
	//	nDecimal=2;
	//	nDecimal = pDBField->m_nDecimals;
		///////////////////////////}}
		break;
	case fCurrency:
		{
		}
		break;
	default:
		break;
	}
	
	pDBField->SetDecimals(nDecimal);
	pDBField->SetWidth(nWidth);/**/
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 取得字段的数据宽度
////////////////////////////////////////////////////////////////////////////////////////////
int  CDataInterface::GetFieldWidth(int nIndex)// 
{
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return 0;
	
	return pField->GetWidth();

}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段的数据宽度
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldWidth(int nIndex,int nWidth)//
{

	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return false;
	
	pField->SetWidth(nWidth);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 取得字段的数据小数位数
////////////////////////////////////////////////////////////////////////////////////////////
int  CDataInterface::GetFieldDecimal(int nIndex)// 
{


	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return 0;
	
	return pField->GetDecimals();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段的数据小数位数
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldDecimal(int nIndex,int nDecimal)// 
{
	
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return false;
	
	pField->SetDecimals(nDecimal);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 取得字段的数据的对齐方式
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetFieldAlign(int nIndex)// 
{
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return 0;
	
	return pField->m_ShowCharacteristic.m_byteCAlign;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段的数据的对齐方式
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldAlign(int nIndex,int nAlign)// 
{

	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return false;
	pField->m_ShowCharacteristic.m_byteCAlign=nAlign;
	pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	int n=pField->m_ShowCharacteristic.m_byteCAlign;


	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 取得字段的数据的变量尺度类型
////////////////////////////////////////////////////////////////////////////////////////////
int  CDataInterface::GetFieldMeasure(int nIndex)// 
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段的数据的变量尺度类型
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldMeasure(int nIndex,int nMeasure)// 
{
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return false;
	pField->SetMeasure(nMeasure);
	

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 取得字段的数据的注解
////////////////////////////////////////////////////////////////////////////////////////////
CTString CDataInterface::GetFieldLable(int nIndex)// 
{
	
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return "";
	
	return pField->GetLabel();

	return "";
}

////////////////////////////////////////////////////////////////////////////////////////////
/// cwh 2002-10-6
/// 公共数据接口
/// 设置字段的数据的注解
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::SetFieldLable(int nIndex,CTString strLable)// 
{
	CField* pField=NULL;
	pField=m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if(pField)
		pField->SetLabel(strLable);
	else
		return false;

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 删除指定索引的字段
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::DeleteField(int nIndex)
{
	int nFields=m_DataAccess.m_pFieldList->GetFieldCount();
	if(nIndex>=0 && nIndex<nFields)
		return m_DataAccess.m_pFieldList->Delete(nIndex);
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 判断指定列是否为空
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::IsColEmpty(int nCol)
{
	return IsTLDBColEmpty(nCol);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 计算指定列的数据个数,bRemoveNULL表示是否除去缺失值
////////////////////////////////////////////////////////////////////////////////////////////
long CDataInterface::GetColDataCount(int nCol,bool bRemoveNULL)
{
	return GetTLDBColDataCount(nCol,bRemoveNULL);
}




////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 取得指定列的数据均值
//
//  修改：QT
//  添加对缺失值的考虑
////////////////////////////////////////////////////////////////////////////////////////////
double CDataInterface::GetColMeanValue(int nCol, bool bCountMissingValue )
{
	return GetTLDBMean(nCol, bCountMissingValue);		
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 取得指定列的数据,nGetDataType表示取数据的方式
/// 
///
/// 修改 QT 2003.01.04
/// 目的：添加取数据方式2和3
/// 说明：方式0删除除空值
///       方式1对空值填均值
///       方式2删除空值和MissingValue填
///       方法3对空值和MissingValue填均值
///		  方式4在空值情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
///       方式5在空值和MissingValue情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetColData(int nCol,TDataSet* &pDataSet,int nGetDataType)
{
	return GetTLDBColData(nCol,pDataSet,nGetDataType);
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
///对指定的数据集pColArray求均值
////////////////////////////////////////////////////////////////////////////////////////////
double  GetColMeanValue(TDataSet *pColArray,int nArray)
{
	if(nArray<=0)
		return 0;

	double sum=0;
	for(int ii=0;ii<nArray;ii++)
		sum+=pColArray[ii].dValue;

	return sum/nArray;
}

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 判断当前集合中的数据是否全相等
////////////////////////////////////////////////////////////////////////////////////////////
bool IsEqual(TDataSet* pArray,int nCount)
{
	if(nCount==1)
		return true;

	int sum = 0;
	for(int j=0;j<nCount;j++)
	{
		if( pArray[0].dValue == pArray[j].dValue )
			sum++;
	}
	return sum==nCount?true:false;

}
bool IsEqual2(TDColAry& pArray,int nCount)
{
	if(nCount==1)
		return true;

	double v = pArray.GetAt(0).dValue ;
	for(int j=0;j<nCount;j++)
	{
		if (v != pArray .GetAt(j).dValue)
			return false;
	}
	return true;

}
////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 根据tInt中的有关索引在pArray中取得相应的数据
////////////////////////////////////////////////////////////////////////////////////////////
int GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt, TDataSet* &pGroupArray)
{
	if(tInt.iCount<=0 || nCount<=0)
		return 0;

	int nGrpCount=0;
	pGroupArray=new TDataSet[tInt.iCount];
	for(int ii=0;ii<tInt.iCount;ii++)
	{
		int index=tInt.GetIDAt(ii);
		for(int kk=0;kk<nCount;kk++)
		{
			if(pArray[kk].iIndex==index)
			{
				pGroupArray[nGrpCount].iIndex=index;
				pGroupArray[nGrpCount].dValue=pArray[kk].dValue;
				pGroupArray[nGrpCount++].strVal =pArray[kk].strVal;
				break;
			}
		}
	}

	return nGrpCount;
}
//对指定的数据集按指定的条件进行分组，并返回分组得到的数据集及数据个数
int GetGroupDataW(TDataSet* pArray,double *pWeight, int nCount,TIntStruct tInt,TDataSet* &pGroupArray, CDoubleVector &v, CDoubleVector &w)
{
	if(tInt.iCount<=0 || nCount<=0)
		return 0;
	v.create(nCount);
	w.create(nCount);

	int nGrpCount=0;
	pGroupArray=new TDataSet[tInt.iCount];
	for(int ii=0;ii<tInt.iCount;ii++)
	{
		int index=tInt.GetIDAt(ii);
		for(int kk=0;kk<nCount;kk++)
		{
			if(pArray[kk].iIndex==index)
			{
				w(nGrpCount) = pWeight[kk];       //权重
				v(nGrpCount) = pArray[kk].dValue; //数据
				pGroupArray[nGrpCount].iIndex=index;
				pGroupArray[nGrpCount].dValue=pArray[kk].dValue;
				pGroupArray[nGrpCount++].strVal =pArray[kk].strVal;
				break;
			}
		}
	}
	//重新定义长度
	v.resize(nGrpCount);
	w.resize(nGrpCount);
	return nGrpCount;
}
//add by liytao 20030108
////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// 根据tInt中的有关索引在pArray中取得相应的数据
////////////////////////////////////////////////////////////////////////////////////////////
double GetGroupData(TDataSet* pArray,int nCount,TIntStruct tInt, TDataSet* &pGroupArray,double weight[])
{
	if(tInt.iCount<=0 || nCount<=0)
		return 0;

	int nGrpCount=0;
	double dbweight=0;
	pGroupArray=new TDataSet[tInt.iCount];
	for(int ii=0;ii<tInt.iCount;ii++)
	{
		int index=tInt.GetIDAt(ii);
		for(int kk=0;kk<nCount;kk++)
		{
			if(pArray[kk].iIndex==index)
			{
				pGroupArray[nGrpCount].iIndex=index;
				pGroupArray[nGrpCount].dValue=pArray[kk].dValue*weight[kk];
				pGroupArray[nGrpCount++].strVal =pArray[kk].strVal;
				dbweight+=weight[kk];
				break;
			}
		}
	}

	return dbweight;
}
//add end

////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 判断指定的列的数据类型是否符合指定的数据类型
////////////////////////////////////////////////////////////////////////////////////////////
bool CDataInterface::FitType(int nCol,int nFit)
{
	int nType=GetFieldType(nCol);
	bool beNumber=(nType==fDouble || nType==fInt || nType==fCurrency || nType==fBoolean)?true:false;
	switch(nFit)
	{
	case 0:   //表示允许所有的数据类型
		return true;
	case 1:   //表示只允许数字类型的数据
		if( beNumber)
			return true;
		break;
	case 2:   //表示只允许数字和字串类型的数据
		if(beNumber || nType==fString)
			return true;
		break;
	case 3:   //表示只允许数字和日期类型的数据
		if(beNumber || nType==fDate)
			return true;
		break;
	case 4:   //表示只允许数字和布尔类型的数据
		if(nType==fBoolean)
			return true;
		break;
	case 5:   //表示只允许非字串类型的数据
		if(nType!=fString)
			return true;
		break;
	case 6:   //表示只允许非日期类型的数据
		if(nType!=fDate)
			return true;
		break;
	case 7:   //表示只允许整型数据
		if(nType==fInt)
			return true;
		break;
	case 8:
		if(nType==fInt || nType == fDouble || nType==fCurrency)
		{
			return true;
		}
		break;
	case 9:
		if(nType==fInt || nType==fBoolean)
			return true;
	case 10:
		if (nType == fBoolean)
			return true;
	default:
		return false;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////
// 
// 数据预处理函数（对自身进行预处理)
//
// 返回值：被处理的记录数
// 参  数：1、处理方式。 缺省为0
//			     0：抛弃空值
//               1: 抛弃空值和缺失值
//               2：均值填充空值
//				 3：均值填充空值和缺失值
//	
// 操  作：
//         
//
int CDataInterface::Pretreatment( int nType )
{
	return Pretreatment( m_DataAccess, nType );
}
// QT
///////////////////////////////////////////////////////////////////////////
   
/////////////////////////////////////////////////////////////////////////
// 
// 数据预处理函数（对指定的数据库接口进行预处理）
//
// 返回值：被处理的记录数
// 参  数：1、数据接口指针。可以为自己本身。
//		   2、处理方式。 缺省为0
//			     0：抛弃空值
//               1: 抛弃空值和缺失值
//               2：均值填充空值
//				 3：均值填充空值和缺失值
//	
// 操  作：
//         
//
int CDataInterface::Pretreatment( CDataInterface *pDI, int nType )
{
	int nTreatedNum = Pretreatment( pDI->m_DataAccess, nType );

	SetWeightNumb( pDI->GetWeightNumb() );

	return nTreatedNum;
}
// QT
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// 
// 数据预处理函数（对指定字段进行预处理）
//
// 返回值：被处理的记录数
// 参  数：1、数据库引用。
//		   2、指定字段号数组(已从小到大排序)
//		   3、处理方式。 缺省为0
//				-1: 不做缺失值处理，只提取指定字段。
//			     0：抛弃空值
//               1: 抛弃空值和缺失值
//               2：均值填充空值
//				 3：均值填充空值和缺失值
//	
// 操  作：
//         
//
int CDataInterface::Pretreatment( CDataAccess& rDataAccess, CTArray< int, int >& aFieldNo, int nType )
{
	int nTreated = 0, iFieldIndex;
	int nFieldCount = aFieldNo.GetSize();
	int nRowCount = rDataAccess.GetRecordCount();

	if( nFieldCount==0 )
		return 0;

	if( &rDataAccess!=&m_DataAccess ) // 需要复制数据库记录
	{
		CDataAccess& rDAScr = rDataAccess;
		CDataAccess& rDADest = m_DataAccess;
		CField * pFieldScr = NULL, * pFieldDest = NULL;
		
		// 创建字段并复制字段信息
		for ( iFieldIndex = 0; iFieldIndex< nFieldCount; iFieldIndex++ )
		{
			pFieldScr = rDAScr.m_pFieldList->FieldByIndex( aFieldNo.GetAt(iFieldIndex) );
			pFieldDest = rDADest.m_pFieldList->CreateField( pFieldScr );
			pFieldDest->Clone( pFieldScr );
			rDADest.m_pFieldList->Add( pFieldDest );	
		}

		// 复制数据
///////////
		rDAScr.First();
		while (!rDAScr.Eof())
		{
			rDADest.Append();
			for ( int i=0; i<nFieldCount; i++ )
			{//复制字段信息
				CField * pSourceField = rDAScr.m_pFieldList->FieldByIndex( aFieldNo.GetAt(i) );
				CField * pDestField = rDADest.m_pFieldList->FieldByIndex(i);
				if (!pSourceField->IsNull())
				{
					CFieldType fieldtype= pSourceField->GetFieldType();
					switch (fieldtype)
					{
						case fDouble :
							pDestField->SetAsDouble(pSourceField->GetAsDouble());
							break;
						case fInt :
							pDestField->SetAsInteger(pSourceField->GetAsInteger());
							break;
						case fString :
							pDestField->SetAsString(pSourceField->GetAsString());
							break;
						case fDate :
							pDestField->SetAsDateTime(&pSourceField->GetAsDateTime());
							break;
						case fCurrency :
							pDestField->SetAsCurrency(pSourceField->GetAsCurrency());
							break;
						case fBoolean :
							pDestField->SetAsBool(pSourceField->GetAsBool());
							break;
						case fBlob :
							break;
						default:
							assert(0);
					}
				}


			}		
			//rDADest.Post();//提交方法取消
			rDAScr.Next();
		}

///////////
	}

	if( nType==-1 )
		return 0;

	bool* abGetMeanValue = new bool[nFieldCount]; // 各字段只计算一次均值用于均值填充方式
	double* adMeanValue = new double[nFieldCount]; // 如需要保存各字段的均值
	bool* abNullRow = new bool[nFieldCount]; // 保存字段是否全空
	// 初始化abGetMeanValue数组
	for( iFieldIndex=0; iFieldIndex<nFieldCount; iFieldIndex++ )
	{
		abGetMeanValue[iFieldIndex] = false;
		if( IsColEmpty( iFieldIndex ))
			abNullRow[iFieldIndex] = true;
		else
			abNullRow[iFieldIndex] = false;
	}

	bool bReplaced; // 当前记录是否已经过填充处理。有些记录需填充多个缺失值
	for ( int i=nRowCount; i>0; i-- ) // 从后往前处理，避免记录号改变问题
	{
		m_DataAccess.SetCurRecNo( i );

		bReplaced = false;  // 未经过填充处理
		for( iFieldIndex=0; iFieldIndex<nFieldCount; iFieldIndex++ ) // 对各字段
		{
			CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( iFieldIndex );
			if( IsMissingValue( pField, nType==1||nType==3 ) ) // 有缺失值
			{
				if( nType==0 || nType==1 ) // 抛弃整条记录
				{
					if( !abNullRow[iFieldIndex] )
					{
						m_DataAccess.Delete();

						nTreated++;
						break;	// 一条记录只要有一个缺失值则整个丢弃
					}
				}
				else // 填充均值
				{
					if( !abNullRow[iFieldIndex] )
					{
						// 计算该字段均值
						if( pField->GetFieldType()!=fString && !abGetMeanValue[iFieldIndex] )
						{
							adMeanValue[iFieldIndex] = GetColMeanValue( iFieldIndex, nType==1||nType==3 );
							abGetMeanValue[iFieldIndex] = true;
							m_DataAccess.SetCurRecNo( i ); // 还原当前数据记录号
						}

						m_DataAccess.Edit();
						switch( pField->GetFieldType() )
						{
							case fInt:
							case fDouble:
							case fCurrency:
							case fDate:
								pField->SetAsDouble( adMeanValue[iFieldIndex] );
								break;
							case fString:
								pField->SetNull();
								break;
							default:
								break;
						}
						//m_DataAccess.Post();//提交方法取消

						if ( !bReplaced ) // 该记录首个填充处理字段
						{
							nTreated++;
							bReplaced = true;
						}
					}
				}
			}
		}
	}
	if( nType==0 || nType==1 )
	{
		for ( iFieldIndex = nFieldCount-1; iFieldIndex>=0; iFieldIndex-- )
		{
			if( abNullRow[iFieldIndex] )
				m_DataAccess.m_pFieldList->Delete( iFieldIndex );
		}
	}

	delete[] abGetMeanValue;
	delete[] adMeanValue;
	delete[] abNullRow;

	return nTreated;
}
// QT
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// 
// 数据预处理函数（对指定的数据库进行预处理）
//
// 返回值：被处理的记录数
// 参  数：1、数据库引用。
//		   2、处理方式。 缺省为0
//			     0：抛弃空值
//               1: 抛弃空值和缺失值
//               2：均值填充空值
//				 3：均值填充空值和缺失值
//	
// 操  作：
//         
//
int CDataInterface::Pretreatment( CDataAccess& rDataAccess, int nType )
{
	int nTreated = 0, iFieldIndex;
	int nFieldCount = rDataAccess.m_pFieldList->GetFieldCount();
	int nRowCount = rDataAccess.GetRecordCount();

	if( &rDataAccess!=&m_DataAccess ) // 需要复制数据库记录
	{
		CDataAccess& rDAScr = rDataAccess;
		CDataAccess& rDADest = m_DataAccess;
		CField * pFieldScr = NULL, * pFieldDest = NULL;
		
		// 创建字段并复制字段信息
		for ( iFieldIndex = 0; iFieldIndex< nFieldCount; iFieldIndex++ )
		{
			pFieldScr = rDAScr.m_pFieldList->FieldByIndex( iFieldIndex );
			pFieldDest = rDADest.m_pFieldList->CreateField( pFieldScr );
			pFieldDest->Clone( pFieldScr );
			rDADest.m_pFieldList->Add( pFieldDest );	
		}

		// 复制全部数据
		rDADest.CopyDataFromOther( &rDAScr );
	}

	bool* abGetMeanValue = new bool[nFieldCount]; // 各字段只计算一次均值用于均值填充方式
	double* adMeanValue = new double[nFieldCount]; // 如需要保存各字段的均值
	bool* abNullRow = new bool[nFieldCount]; // 保存字段是否全空
	// 初始化abGetMeanValue数组
	for( iFieldIndex=0; iFieldIndex<nFieldCount; iFieldIndex++ )
	{
		abGetMeanValue[iFieldIndex] = false;

		if( IsColEmpty( iFieldIndex ))
			abNullRow[iFieldIndex] = true;
		else
			abNullRow[iFieldIndex] = false;
	}

	bool bReplaced; // 当前记录是否已经过填充处理。有些记录需填充多个缺失值
	for ( int i=nRowCount; i>0; i-- ) // 从后往前处理，避免记录号改变问题
	{
		m_DataAccess.SetCurRecNo( i );

		bReplaced = false;  // 未经过填充处理
		for( iFieldIndex=0; iFieldIndex<nFieldCount; iFieldIndex++ ) // 对各字段
		{
			CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( iFieldIndex );
			if( IsMissingValue( pField, nType==1||nType==3 ) ) // 有缺失值
			{
				if( nType==0 || nType==1 ) // 抛弃整条记录
				{
					if( !abNullRow[iFieldIndex] )
					{
						m_DataAccess.Delete();

						nTreated++;
						break;	// 一条记录只要有一个缺失值则整个丢弃
					}
				}
				else // 填充均值
				{
					if( !abNullRow[iFieldIndex] )
					{
						// 计算该字段均值
						if( pField->GetFieldType()!=fString && !abGetMeanValue[iFieldIndex] )
						{
							adMeanValue[iFieldIndex] = GetColMeanValue( iFieldIndex, nType==1||nType==3 );
							abGetMeanValue[iFieldIndex] = true;
							m_DataAccess.SetCurRecNo( i ); // 还原当前数据记录号
						}

						m_DataAccess.Edit();
						switch( pField->GetFieldType() )
						{
							case fInt:
							case fDouble:
							case fCurrency:
							case fDate:
								pField->SetAsDouble( adMeanValue[iFieldIndex] );
								break;
							case fString:
								pField->SetNull();
								break;
							default:
								break;
						}
						//m_DataAccess.Post();//提交方法取消

						if ( !bReplaced ) // 该记录首个填充处理字段
						{
							nTreated++;
							bReplaced = true;
						}
					}
				}
			}
		}
	}
	if( nType==0 || nType==1 )
	{
		for ( iFieldIndex = nFieldCount-1; iFieldIndex>=0; iFieldIndex-- )
		{
			if( abNullRow[iFieldIndex] )
				m_DataAccess.m_pFieldList->Delete( iFieldIndex );
		}
	}

	delete[] abGetMeanValue;
	delete[] adMeanValue;
	delete[] abNullRow;
	
	return nTreated;
}
// QT
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// 
// 当前记录指定字段是否为缺失值
//
// 返回值：被处理的记录数
// 参  数：1、指定字段
//		   2、是否考虑缺失值。缺省为不考虑.
//		
// 操  作：
//         
//
bool CDataInterface::IsMissingValue( CField * pField, bool bCountMissingValue )
{
	bool bValueMissed = false;
	double dData[3], dOrgData;
	int nCount, iIndex;

	if( pField->IsNull() )
		bValueMissed = true;
	else if( pField->m_pMissing!=NULL && bCountMissingValue )
	{
		if( pField->m_pMissing->GetMissingType()!=mtNoMissing )
		{
			dOrgData = pField->GetAsDouble();	
			pField->m_pMissing->GetDoubleData( dData[0], dData[1], dData[2] );
			nCount =  pField->m_pMissing->GetCount();

			if( pField->m_pMissing->GetMissingType()==mtDiscrete )
			{
				for( iIndex=0; iIndex<nCount; iIndex++ )
				{
					if( dOrgData==dData[iIndex] )
					{
						bValueMissed = true;
						break;
					}
				}
			}
			else if( pField->m_pMissing->GetMissingType()==msRange )
			{
				if( dOrgData>=dData[0] && dOrgData<=dData[1] )
				 	bValueMissed = true;
				else if( nCount==3 && dOrgData==dData[2] )
					bValueMissed = true;
			}
		}
	}

	return bValueMissed;
}
// QT
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 读取权重列数据
//
// 返回值：合法的权重记录数
// 参  数：1、输出的权重数据
//		   2、权重合法性检测方式。缺省为0
//				0：权重必须是正整数
//				1：权重必须是正数，浮点数四舍五入取整          
//		
// 操  作：
//         
int CDataInterface::GetWeightData( int nData[], int nGetType )
{
	int nWeightNo, nRow, nValid;

	if( (nWeightNo=GetWeightNumb())<0 ) // 是否设置了权重列
		return -1;

	CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( nWeightNo );

	assert( pField->GetFieldType()==fInt ||
		    pField->GetFieldType()==fDouble ||
			pField->GetFieldType()==fCurrency ); // 权重列必须是数值型

	nRow = nValid = 0;
	m_DataAccess.First();
	while( !m_DataAccess.Eof() )
	{
		if( IsMissingValue( pField, true ) ) // 缺失值检查
		{
			nData[nRow] = 0;
		}
		else
		{
			double dData = pField->GetAsDouble(); 
			switch( nGetType )
			{
			case 0: // 权重必须是正整数的情况
				if( dData>0 && dData==int(dData) ) // 是否为正整数？
				{
					nData[nRow] = int(dData);
					nValid++;
				}
				else
					nData[nRow] = 0;

				break;
			case 1: // 权重必须是正数的情况
				nData[nRow] = int( dData+0.5 );
				if( nData[nRow]>0 ) // 是否为正数？
					nValid++;
				else
					nData[nRow] = 0;
				break;
			default:
				assert( false );
				break;
			}
		}

		nRow++;
		m_DataAccess.Next();
	}

	return nValid;
}

int CDataInterface::GetWeightData( TIntAry &DataAry, int nGetType )
{
	int nWeightNo, nRow, nValid;

	if( (nWeightNo=GetWeightNumb())<0 ) // 是否设置了权重列
		return -1;

	CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( nWeightNo );

	assert( pField->GetFieldType()==fInt ||
		    pField->GetFieldType()==fDouble ||
			pField->GetFieldType()==fCurrency ); // 权重列必须是数值型

	nRow = nValid = 0;
	m_DataAccess.First();
	while( !m_DataAccess.Eof() )
	{
		if( IsMissingValue( pField, true ) ) // 缺失值检查
		{
			DataAry.SetAt(nRow,0);
		}
		else
		{
			double dData = pField->GetAsDouble(); 
			switch( nGetType )
			{
			case 0: // 权重必须是正整数的情况
				if( dData>0 && dData==int(dData) ) // 是否为正整数？
				{
					DataAry.SetAt(nRow,int(dData));

					nValid++;
				}
				else
					DataAry.SetAt(nRow,0);

				break;
			case 1: // 权重必须是正数的情况
				DataAry.SetAt(nRow,int( dData+0.5 ));
				if( int( dData+0.5 )>0 ) // 是否为正数？
					nValid++;
				else
					DataAry.SetAt(nRow,0);
				break;
			default:
				assert( false );
				break;
			}
		}

		nRow++;
		m_DataAccess.Next();
	}

	return nValid;
}
// QT
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 读取权重列数据(浮点型)
//
// 返回值：合法的权重记录数
// 参  数：输出的权重数据
//		
// 说  明：如果非法权重，则相应数据赋值0.0
//         
int CDataInterface::GetWeightData( double dData[] )
{
	int nWeightNo, nRow, nValid;

	if( (nWeightNo=GetWeightNumb())<0 ) // 是否设置了权重列
		return -1;

	CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( nWeightNo );

	assert( pField->GetFieldType()==fInt ||
		    pField->GetFieldType()==fDouble ||
			pField->GetFieldType()==fCurrency ); // 权重列必须是数值型

	nRow = nValid = 0;
	m_DataAccess.First();
	while( !m_DataAccess.Eof() )
	{
		if( IsMissingValue( pField, true ) ) // 缺失值检查
		{
			dData[nRow] = 0.0;
		}
		else
		{
			dData[nRow] = pField->GetAsDouble(); 
			if( dData[nRow]>0 ) // 是否为正数？
				nValid++;
			else
				dData[nRow] = 0.0;
		}

		nRow++;
		m_DataAccess.Next();
	}

	return nValid;
}
int CDataInterface::GetWeightData( TDoubleAry &dataAry )
{
	int nWeightNo, nRow, nValid;

	if( (nWeightNo=GetWeightNumb())<0 ) // 是否设置了权重列
		return -1;

	CField * pField = m_DataAccess.m_pFieldList->FieldByIndex( nWeightNo );

	assert( pField->GetFieldType()==fInt ||
		    pField->GetFieldType()==fDouble ||
			pField->GetFieldType()==fCurrency ); // 权重列必须是数值型

	nRow = nValid = 0;
	m_DataAccess.First();
	while( !m_DataAccess.Eof() )
	{
		if( IsMissingValue( pField, true ) ) // 缺失值检查
		{
			dataAry.SetAt(nRow,0);
			//dData[nRow] = 0.0;
		}
		else
		{
			dataAry.SetAt(nRow,pField->GetAsDouble());
			//dData[nRow] = pField->GetAsDouble(); 
			if( dataAry.GetAt(nRow)>0 ) // 是否为正数？
				nValid++;
			else
				dataAry.SetAt(nRow,0);
		}

		nRow++;
		m_DataAccess.Next();
	}

	return nValid;
}


// QT
////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetWeightData(int nIndex)
{
	int nWeightNumb=GetWeightNumb();
	if(nWeightNumb<0 || nWeightNumb>=GetColCount())
		return 0;

	if(nIndex<1 || nIndex>GetRowCount())
		return 0;

	m_DataAccess.SetCurRecNo(nIndex);
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nWeightNumb);
	if( pField->IsNull() )
		return 0;

	if(pField)
	{
		int nWeightData=pField->GetAsInteger();
		if(nWeightData<0)
			return 0;

		return nWeightData;
	}
	return 0;	
}
//得到权重的字段索引号,-1 为没有设置.//#_S 2003-3-18 $ 10 zlq
int  CDataInterface::GetWeightNumb()
{
	int nWeightCase=-1;
	if(!m_strWeightCase.IsEmpty())
	{//#_S 2003-3-18 $ 10 zlq 修改  替换原来的查找方法
		CField* pField = m_DataAccess.m_pFieldList->FieldByName(m_strWeightCase);
		if (pField)
		{//根据字段找到索引号
			nWeightCase = m_DataAccess.m_pFieldList->IndexOf(pField);
		}
	}

	if(nWeightCase<0)
		m_strWeightCase="";

	return nWeightCase;
}

bool  CDataInterface::SetWeightNumb(int nNumb)
{
	int nFields=m_DataAccess.m_pFieldList->GetFieldCount();
	if(nNumb>=0 && nNumb<nFields)
	{
		CField * pField=m_DataAccess.m_pFieldList->FieldByIndex(nNumb);
		m_strWeightCase=pField?pField->GetFieldName():"";
	}
	else
		m_strWeightCase="";

	return m_strWeightCase.IsEmpty()?false:true;
//	m_nWeightCase=nNumb;
}

//取当前exe路径
//author: mxm
void GetModulePath(CTString& modulPath)
{
/*=	//取 路径文件名
	unsigned short buffer[_MAX_PATH];

==uc  	GetModuleFileName(NULL, (LPTSTR)buffer,_MAX_PATH);
	modulPath.Format("%s",buffer);
	
	//得到 最后"\\"的位置
	char szTitle[_MAX_PATH];
	strcpy(szTitle,(const char *)buffer);
	int len=modulPath.GetLength();
	for(int j=len;j>1;j--) //get rid of old Title
	{
		if(szTitle[j]=='\\') //
			break;
	}
	
	//取 最后"\\"之前的路径, 去文件名
	modulPath.Empty();
	memcpy(modulPath.GetBuffer(j+1),szTitle,j+1); 
	modulPath.ReleaseBuffer();   */
}



void CDataInterface::SetShowValueLabel(bool bShow)
{
	this->m_bShowValueLabel=bShow;
}

bool CDataInterface::GetShowValueLabel()
{
	return m_bShowValueLabel;
}


//template <Type>

////////////////////////////////////////////////////////////////////////////////
//DataMatrix 此处申请空间,外部释放 
//             全部有效
//// 取得天律数据库中指定字段的所有数据，nGetDataType为取数据方式
///
/// 目的：添加取数据方式2和3
/// 说明：方式0删除除空值
///       方式1对空值填均值
///       方式2删除空值和MissingValue填
///       方法3对空值和MissingValue填均值
///		  方式4在空值情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
///       方式5在空值和MissingValue情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
/////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetColsData(CDWordArray &nColAry, TCDataMatrix &DataMatrix, int nGetDataType)
{
	DataMatrix.clear();
	//DataMatrix.RemoveAll();
	//int nCount=0;//,nFieldType=GetTLDBFieldType(nIndex)
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0 || nGetDataType>5 || nGetDataType<0 )
		return 0;
	int i=0;
	for (i=0;i<nColAry.GetSize();i++)
	{//Bufsize = 10000 * 10,int nMaxMemsize = 10000*50,int nConflictPer =10);
		TDColAry *pAry = new TDColAry();
		pAry->SetSize(nRecord);
		DataMatrix.push_back(pAry);
		//DataMatrix.Add(pAry);
	}

	m_DataAccess.First();
	int nRowIndex=0;
	TDataCol dc;
	while(!m_DataAccess.Eof())
	{
		int i=0;
		for (i=0;i<nColAry.GetSize();i++)
		{
			CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
			if(!IsMissingValue( pField, nGetDataType==2||nGetDataType==3||nGetDataType==5 ))//(pField->IsNull()))
			{
				dc.dValue = pField->GetAsDouble();
				dc.iIndex = nRowIndex +1;
				DataMatrix[i]->SetAt(nRowIndex,dc);
			}
			else
			{
				switch( nGetDataType )
				{
				case 0:
				case 2://暂时填

				case 1:
				case 3:
				case 4:
				case 5:
					dc.iIndex = -(nRowIndex+1);
					DataMatrix[i]->SetAt(nRowIndex,dc);
					break;
				default:
					break;
				}
			}
		}
		nRowIndex++;
		m_DataAccess.Next();	
	}
	if (nGetDataType == 1 || nGetDataType ==3)
	{//均值填
		int nCol=0;
		for (nCol=0;nCol<nColAry.GetSize();nCol++)
		{
			TDColAry *pAry = DataMatrix[nCol];
			int nValidCount = 0;
			double dSum = 0;
			int nRow=0;
			for (nRow=0;nRow < pAry->GetLen() ;nRow++)
			{
				if (pAry->GetAt(nRow).iIndex>=0)
				{
					dSum = dSum+ pAry->GetAt(nRow).dValue;
					nValidCount++;
				}
			}
			//均值
			TDataCol dc;
			
			double dAvg = dSum/nValidCount;
			dc.dValue = dAvg;
			//填充
			for ( nRow=0;nRow < pAry->GetLen() ;nRow++)
			{
				if (pAry->GetAt(nRow).iIndex<=0)
				{
					dc.iIndex = pAry->GetAt(nRow).iIndex;
					pAry->SetAt(nRow,dc);
				}
			}

		}
	}
	
	//****************  0   2 ************************
	if (nGetDataType == 0 || nGetDataType ==2)
	{//删除处理
		//******** 获得有效行的个数以及对应的索引号  *****
		TIntAry validIndex;
		validIndex.SetSize(nRecord);
		int nValidCount = 0;
		for (int nRow=0;nRow < nRecord ;nRow++)
		{
			bool bHasMissing = false;
			for (int nCol=0;nCol<nColAry.GetSize();nCol++)
			{
				TDColAry *pAry = DataMatrix[nCol];
				if (pAry->GetAt(nRow).iIndex<0)
				{
					bHasMissing = true;
					break;
				}
			}
			if (bHasMissing ==false)
			{
				validIndex.SetAt(nValidCount,nRow);
				nValidCount++;				
			}	
		}
		
		//转移到新的数组中去
		for (int nCol=0;nCol<nColAry.GetSize();nCol++)
		{
			TDColAry *pAry = DataMatrix[nCol];
			TDColAry *pNewAry = new TDColAry();
			DataMatrix[nCol]= pNewAry;//替换成新的
			pNewAry->SetSize(nValidCount);

			for (int nRow=0;nRow < nValidCount ;nRow++)
				pNewAry->SetAt(nRow,pAry->GetAt(validIndex.GetAt(nRow) ));

			delete pAry;
		}

		return nValidCount;//返回有效个数
	}
	return nRowIndex ;
}

/************************************************************
本函数可以获取字符类型的字段，转化到数字后，放入数字矩阵中
Map对象负责管理各列的字符和数字之间的映射关系

缺失值处理方式：整行删除
//#_S 2005-11-29 zlq
************************************************************/
int CDataInterface::GetColsData(CDWordArray &nColAry, CDoubleMatrix &Matrix_Data, CMapManager &Map)
{
	for (int i=0;i<nColAry.GetSize();i++)
	{
		CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
		Map.AddFieldName(pField->GetFieldName(),pField->GetFieldType() ==fString);
	}
	
	long nRecord=m_DataAccess.GetRecordCount();
	
	CDoubleMatrix MM(nRecord,nColAry.GetSize());

	m_DataAccess.First();
	int nRowIndex=0;
	while(!m_DataAccess.Eof())
	{
		for (int i=0;i<nColAry.GetSize();i++)
		{
			CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
			if (pField->GetFieldType()==fString)
			{
				CTString strValue;
				strValue = pField->GetAsString();
				if (strValue.GetLength())
				{
					int nCV = Map.AddValue(i,strValue);
					MM(nRowIndex,i) = nCV;
				}
				else
				{
					MM(nRowIndex,i) = NULLValue;
				}
				
				continue;
			}

			if(!IsMissingValue( pField, true ))//(pField->IsNull()))
			{
				MM(nRowIndex,i) = pField->GetAsDouble();
			}
			else
			{
				MM(nRowIndex,i) = NULLValue;
			}
		}
		nRowIndex++;
		m_DataAccess.Next();	
	}

	
	//****************   ************************
	if (1)
	{//删除处理
		//******** 获得有效行的个数以及对应的索引号  *****
		TIntAry validIndex;
		validIndex.SetSize(nRecord);
		int nValidCount = 0;
		for (int nRow=0;nRow < nRecord ;nRow++)
		{
			bool bHasMissing = false;
			for (int nCol=0;nCol<nColAry.GetSize();nCol++)
			{
				if (MM(nRow,nCol) ==NULLValue)
				{
					bHasMissing = true;
					break;
				}
			}
			if (bHasMissing ==false)
			{
				validIndex.SetAt(nValidCount,nRow);
				nValidCount++;				
			}	
		}
		
		//转移到新的数组中去,这里将添加到一个矩阵中去。
		Matrix_Data.create(nValidCount,nColAry.GetSize());

		for (int nCol=0;nCol<nColAry.GetSize();nCol++)
		{
			for (int nRow=0;nRow < nValidCount ;nRow++)
				Matrix_Data(nRow,nCol) = MM(validIndex.GetAt(nRow),nCol);
		}
		

		return nValidCount;//返回有效个数
	}
	Matrix_Data = MM;
	return nRowIndex ;
}


/************************************************************
说明同上//#_S 2005-11-15 zlq 修改
************************************************************/
int CDataInterface::GetColsData(CDWordArray &nColAry, CDoubleMatrix &Matrix_Data, int nGetDataType)
{
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0 || nGetDataType>6 || nGetDataType<0 )
		return 0;
	Matrix_Data.create(nRecord,nColAry.GetSize());

	m_DataAccess.First();
	int nRowIndex=0;

	if ((nGetDataType == 0)||(nGetDataType == 2))//缺失值删除
	{
		while(!m_DataAccess.Eof())
		{
			bool bMissing = false;
			for (int i=0;i<nColAry.GetSize();i++)
			{
				CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
				if(pField->IsNull())
				{
					bMissing = true;
					break;
				}
				Matrix_Data(nRowIndex,i) = pField->GetAsDouble();
			}
			if (!bMissing)
				nRowIndex++;
			m_DataAccess.Next();	
		}
		Matrix_Data.rerowsize(nRowIndex);
	}
	else if ((nGetDataType == 1)||(nGetDataType == 3))//缺失值用均值填充
	{
		int nValidCount = 0;
		int nMissCount = 0;
		CIntVector  Row_Vect(nRecord*nColAry.GetSize());
		CIntVector  Col_Vect(nRecord*nColAry.GetSize());
		CDoubleVector Vect_Sum(nColAry.GetSize(),0);
		while(!m_DataAccess.Eof())
		{
			for (int i=0;i<nColAry.GetSize();i++)
			{
				CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
				if(!pField->IsNull())
				{
					double dValue = pField->GetAsDouble();
					Matrix_Data(nRowIndex,i) = dValue;
					Vect_Sum(i) += dValue;
					nValidCount++;
				}
				else
				{
					Row_Vect(nMissCount) = nRowIndex;
					Col_Vect(nMissCount) = i;
					nMissCount++;
				}
			}
			nRowIndex++;
			m_DataAccess.Next();
		}
		Vect_Sum /= nValidCount;
		for (int i=0;i<nMissCount;i++)
		{
			Matrix_Data(Row_Vect(i),Col_Vect(i)) = Vect_Sum(Col_Vect(i));
		}
//		nRowIndex = nValidCount + nMissCount;
	}
	else if ((nGetDataType ==  6))
	{
		int nValidCount = 0;
		int nMissCount = 0;
		double dValue = 0;
		CDoubleVector	Col_Value(nColAry.GetSize(),0);
		CIntVector		MissCount(nColAry.GetSize(),0);
		while(!m_DataAccess.Eof())
		{
			for (int i=0;i<nColAry.GetSize();i++)
			{
				CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nColAry.GetAt(i));
				if(!pField->IsNull())
				{
					dValue = pField->GetAsDouble();
					Matrix_Data(nRowIndex,i) = dValue;
					for (int j=0;j<MissCount(i);j++)
					{
						if (nRowIndex==MissCount(i))
							Matrix_Data(nRowIndex-j-1,i) = dValue;
						else
							Matrix_Data(nRowIndex-j-1,i) = (Col_Value(i)+dValue)/2;
					}
					MissCount(i) = 0;
					Col_Value(i) = dValue;
				}
				else
				{
					MissCount(i) += 1;
				}
			}
			nRowIndex++;
			m_DataAccess.Next();
		}
		for (int i=0;i<nColAry.GetSize();i++)
		{
			for (int j=0;j<MissCount(i);j++)
			{
				Matrix_Data(nRowIndex-j-1,i) = Col_Value(i);
			}
		}
	}
	return nRowIndex;
}


////////////////////////////////////////////////////////////////////////////////////////////
/// 公共数据接口
/// cwh 2002-9-13
/// 取得指定列的数据,nGetDataType表示取数据的方式
/// 
///
/// 修改 QT 2003.01.04
/// 目的：添加取数据方式2和3
/// 说明：方式0删除除空值
///       方式1对空值填均值
///       方式2删除空值和MissingValue填
///       方法3对空值和MissingValue填均值
///		  方式4在空值情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
///       方式5在空值和MissingValue情况将TDataSet结构中iIndex用负数表示该值缺失，
///				绝对值为记录号
////////////////////////////////////////////////////////////////////////////////////////////
int CDataInterface::GetColData(int nCol, TDColAry &DataAry, int nGetDataType)
{
	int nCount=0,nFieldType=GetTLDBFieldType(nCol);
	long nRecord=m_DataAccess.GetRecordCount();
	if (nRecord<=0 || nGetDataType>5 || nGetDataType<0 || nFieldType<=0)
		return 0;

	
	double dMeanValue=0;
	if(nGetDataType==0||nGetDataType==2)
	{
		long nCount=GetTLDBColDataCount(nCol,true);
		if(nCount>0)
			DataAry.SetSize(nCount);
		else
			return 0;
	}
	else
	{
		if(nRecord<=0)
			return 0;
		DataAry.SetSize(nRecord);
		if( nGetDataType==1||nGetDataType==3 )
			dMeanValue=GetTLDBMean(nCol, nGetDataType==3);
	}


	nCount=0;
	m_DataAccess.First();
	int nRowIndex=1;
	CField* pField=m_DataAccess.m_pFieldList->FieldByIndex(nCol);
	while(!m_DataAccess.Eof())
	{
		
		if(!IsMissingValue( pField, nGetDataType==2||nGetDataType==3||nGetDataType==5 ))//(pField->IsNull()))
		{
			TDataCol dc;dc.dValue = pField->GetAsDouble();
			dc.iIndex = nRowIndex;
			DataAry.SetAt(nCount,dc);

			nCount++;
		}
		else
		{
			TDataCol dc;
			switch( nGetDataType )
			{
			case 1:
			case 3:
				
				dc.dValue =dMeanValue;
				dc.iIndex = -nRowIndex;
				DataAry.SetAt(nCount,dc);
				
				nCount++;
				break;
			case 4:
			case 5:
				//dc.dValue =dMeanValue;
				dc.iIndex = -nRowIndex;
				DataAry.SetAt(nCount,dc);
				nCount++;
				break;
			default:
				break;
			}
		}

		nRowIndex++;
		m_DataAccess.Next();	
	}

	return nCount;
}

/***********************************************
得到markway.ini文件的配置信息
zlq
***********************************************/
bool CDataInterface::GetProfileIntValue(CTString strSection, CTString strSectionKey, int &value)
{
	CTString strPath;
	GetModulePath(strPath);
	strPath = strPath+"Markway.ini";
	CTString strValue       = _T("");
/*	char inBuf[80];
===uc   
	int rst =GetPrivateProfileString (strSection,strSectionKey, NULL, inBuf, 80, strPath); 
	value = atoi(inBuf);
	//strValue=inBuf;
	return rst;   */
	return false;
}

CField* CDataInterface::FieldByIndex(int index)
{
	return m_DataAccess.m_pFieldList->FieldByIndex(index);
}

CField* CDataInterface::FieldByName(CTString FieldName)
{
	return m_DataAccess.m_pFieldList->FieldByName(FieldName);
}

int CDataInterface::IndexByName(CTString sFieldName)
{
	return m_DataAccess.m_pFieldList->IndexOf(sFieldName);
}

int CDataInterface::IndexByField(CField* pField)
{
	return m_DataAccess.m_pFieldList->IndexOf(pField);
}



CTString CDataInterface::NameByIndex(int nIndex)
{
	CField *pField = m_DataAccess.m_pFieldList->FieldByIndex(nIndex);
	if (pField)
		return pField->GetFieldName();
	return "";
}

bool CDataInterface::getVarID(CTStringArray &strFieldAry, TIntStruct &Inar)
{
	if (strFieldAry.GetSize() == 0)
		return false;
	int nSize = strFieldAry.GetSize();
	int *pInt = new int[nSize];
	for (int i=0;i<nSize ;i++)
	{
		
		int nIndex = IndexByName(strFieldAry.GetAt(i));
		if (nIndex ==-1)
		{
			delete []pInt;
			return false;
		}
		*(pInt+i) = nIndex;
	}
	TIntStruct tl(nSize,pInt);
	Inar = tl;
	return true;
}
