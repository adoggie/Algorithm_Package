// Result.cpp : Defines the initialization routines for the DLL.
//
#pragma  warning(disable:4251)

#include "Result.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//*/
/*
static AFX_EXTENSION_MODULE ResultDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("RESULT.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ResultDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(ResultDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("RESULT.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ResultDLL);
	}
	return 1;   // ok
}
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction - CRsltObject --
//////////////////////////////////////////////////////////////////////


CRsltObject::CRsltObject()
{
	m_nID=0;
	m_nType=0;
	m_strName="";

}

CRsltObject::~CRsltObject()
{

}

void CRsltObject::Serialize(TCLFile &file)
{
	file<<m_nID<<m_nType<<m_strName;
}


void CRsltObject::Serialize(unsigned char *&pBuf)
{
	int nTotalSize = GetBufLen();
	SaveDigit(pBuf,nTotalSize);
	SaveDigit(pBuf,m_nID);
	SaveDigit(pBuf,m_nType);
	SaveString(pBuf,m_strName);
	printf("***** 总长:%d Result Object ID is%d, Result Type is %d, Result Name is %s\r\n",nTotalSize,m_nID,m_nType,m_strName.GetData());
}

int CRsltObject::GetBufLen()
{
	//      总长度       m_nID           m_nType    字符串长度    字符串
	return sizeof(int)+sizeof(m_nID)+sizeof(m_nType)+sizeof(int)+m_strName.GetLength();
}

void CRsltObject::Print()
{
	printf("Result Object ID is%d, Result Type is %d, Result Name is %s\n",m_nID,m_nType,m_strName.GetData());
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction - CResultS--
//////////////////////////////////////////////////////////////////////

CResultS::CResultS()
{
	m_strName="统计信息";
}

CResultS::~CResultS()
{
	Clear();
}

void CResultS::Serialize(TCLFile &file)
{
	CRsltObject::Serialize(file);
	int nReSize = m_tpResultS.size();
	file << nReSize;
	for (int i=0;i<nReSize;i++)
		m_tpResultS.at(i)->Serialize(file);

}

void CResultS::Print()
{
#ifdef _DEBUG
	CRsltObject::Print();
	int nSize = m_tpResultS.size();
	printf("Result Size is %d\n",nSize);
	for (int i=0;i<nSize; i++)
	{
		m_tpResultS.at(i)->Print();
	}
#endif
}

int CResultS:: Add(CResult* pResult)
{
	if(pResult->GetID()<=0)
	{
		unsigned int id=GetNoUsedID();
		pResult->SetID(id);
	}
	if (pResult != NULL)
	{
		m_tpResultS.push_back(pResult);
			return m_tpResultS.size();
	}
	return -1;
}

int  CResultS:: Find(CResult* pResult)
{
	if(pResult!=NULL)
		for(int ii=0;ii<Count();ii++)
		{
			if(Item(ii)==pResult)
				return ii;
		}
		
	return -1;
}

int CResultS::  Count()
{
	return m_tpResultS.size();
}

CResult* CResultS::Item(int nIndex)
{
	if(nIndex<0 || nIndex>=Count())
		return NULL;

	return m_tpResultS.at(nIndex);
}

CResult*  CResultS::Item(unsigned int nID)
{
	for(int ii=0;ii<Count();ii++)
	{
		CResult* pResult=m_tpResultS.at(ii);
		if(pResult->GetID()==nID)
			return pResult;		
	}

	return NULL;
}


bool CResultS:: InsertAt(int nIndex,CResult* pResult)
{
	if(pResult!=NULL)
	{
		if(nIndex<0)
			nIndex=0;
		else if(nIndex>Count())
			nIndex=Count();
// yzmAdd  原来在插入操作时没有设置ID ，造成很多功能不能实现
		if(pResult->GetID()<=0)
		{
			unsigned int id=GetNoUsedID();
			pResult->SetID(id);
		}
		
		m_tpResultS.insert(m_tpResultS.begin()+nIndex,pResult);
		
		return true;
	}
	return false;
}

void CResultS:: RemoveAt(int nIndex,int nCount)
{
	if(nIndex>=0 && nIndex<Count() && nCount>0)
	{
		for (int i=0;i<nCount;i++)
			m_tpResultS.erase(m_tpResultS.begin()+nIndex);
	}
}

void  CResultS::RemoveAll()
{
	m_tpResultS.clear();
}

void CResultS:: Delete(int nIndex,int nCount)
{
	if(nIndex>=0 && nIndex<Count() && nCount>0)
	{
		int count=Count(),
			nxx=count-nIndex+1,
			max=nCount>nxx?nxx:nCount;
		for(int ii=0;ii<max;ii++)
		{
			CResult* pResult=m_tpResultS.at(nIndex);
			m_tpResultS.erase(m_tpResultS.begin()+nIndex);
			delete pResult;
			pResult=NULL;
		}
	}
}

void CResultS::Clear()
{
	int count=Count();
	for(int  ii=0;ii<count;ii++)
	{
		CResult* pResult=m_tpResultS.at(ii);
		delete pResult;
		pResult=NULL;
	}
	m_tpResultS.clear();
}

/*
函 数 名 称	GetNoUsedID	所 属 类	CResultS
功 能 说 明	取得在当前对象集合中所有直接子对象未用的最小ID号，ＩＤ号从１开始。当前集合中的一直接子对象被删除后，它同时释放它所占用的ＩＤ号，即在同一个集合中所有的直接子对象的ＩＤ号是唯一的，但ＩＤ号可以被复用。
参    数	参数名称	类  型	参数说明
tInt	TIntStruct	存放着所要寻找的对象位置的结构
bSelFirstStat	bool	是否在寻找所指位置对象的第一个叶对象

返  回  值	返回当前集合中所有直接子对象未用的最小ＩＤ号
*/
unsigned int CResultS::GetNoUsedID()
{
	int count=Count();
	for(unsigned int ii=1;ii<1000;ii++)   //yzmModify 取序号应该从1开始
	{
		bool bNoUse=true;
		for(int jj=0;jj<count;jj++)
		{
			CResult* pResult=Item(jj);
			unsigned int id=pResult->GetID();
			if(id==ii)
			{
				bNoUse=false;
				break;
			}
		}
		if(bNoUse)
			return ii;
	}
	return 0;
}


// yzmAdd 根据id查结果集
CRsltObject* CResultS::GetByID(unsigned int id)
{
	CResult *pResult=NULL,*pRetResult=NULL;
	for(int ii=0;ii<Count();ii++)//根节点对应一个结果集pResult,先查找结果集
	{
		pResult=Item(ii);
		if(pResult->GetID()==id)
		{
			pRetResult=pResult;
			break;
		}
	}
	return pRetResult;
}

int CResultS::GetSumCount()
{
	int nCount=0,
		nSize=Count();
	for(int i=0;i<nSize;i++)
	{
		CResult* pResult=Item(i);
		nCount+=pResult->GetSumCount();
	}

	return nCount;
}

void CResultS::Copy(CRsltObject* pRsltObject)
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction -CResult --
//////////////////////////////////////////////////////////////////////

CResult::CResult()
{
	m_nID=0;
	m_strName="";
}

CResult::CResult(CTString strName)
{
	m_nID=0;
	m_nType=RSLT_RESULT;
	m_strName=strName;
}

CResult::CResult(char * strName)
{
	m_nID=0;
	m_nType=RSLT_RESULT;
	m_strName=strName;
}

CResult::~CResult()
{
	Clear();
}

void CResult::Serialize(TCLFile &file)
{
	CRsltObject::Serialize(file);
	int nSize = m_tpResult.size();
	file << nSize;
	for (int i=0;i<nSize;i++)
		m_tpResult.at(i)->Serialize(file);
}
void CResult::Serialize(unsigned char *&pBuf)
{
	CRsltObject::Serialize(pBuf);
	int nSize = m_tpResult.size();
	SaveDigit(pBuf,nSize);
	for (int i=0;i<nSize;i++)
		m_tpResult.at(i)->Serialize(pBuf);
}

int CResult::GetBufLen()
{
	int nLen = CRsltObject::GetBufLen();
	int nSize = m_tpResult.size();
	nLen = nLen +sizeof(int);
	for (int i=0;i<nSize;i++)
		nLen = nLen +m_tpResult.at(i)->GetBufLen();
	return nLen;
}

void CResult::Print()
{
#ifdef _DEBUG
	CRsltObject::Print();
	int nSize = m_tpResult.size();
	printf("Result Variable Size is %d\n",nSize);
	for (int i=0;i<nSize;i++)
	{
		m_tpResult.at(i)->Print();
	}
#endif
}

int CResult::Add(CRsltVariable* pVariable)
{
	if(pVariable)
	{
		if(pVariable->GetID()<=0)
			pVariable->SetID(GetNoUsedID());

		if(pVariable->GetType()==RSLT_ELEMENT)
		{
			for(int ii=0;ii<Count();ii++)
			{
				if(Item(ii)->GetType()!=RSLT_ELEMENT)
				{
					m_tpResult.insert(m_tpResult.begin()+ii,pVariable);
					return ii;
				}
			}
		}
		m_tpResult.push_back(pVariable);
		return m_tpResult.size();
	}

	return -1;
}

CRsltVariable*  CResult::Item(int nIndex)
{
	return (nIndex>=0 && nIndex<Count())?m_tpResult.at(nIndex):NULL;
}

CRsltVariable*  CResult::Item(unsigned int nID)
{
	for(int ii=0;ii<Count();ii++)
	{
		CRsltVariable* pVariable=m_tpResult.at(ii);
		if(pVariable->GetID()==nID)
			return pVariable;		
	}

	return NULL;
}

int CResult::Find(CRsltVariable* pVariable)
{
	if(pVariable)
	{
		for(int ii=0;ii<Count();ii++)
			if(m_tpResult.at(ii)==pVariable)
				return ii;
	}

	return -1;
}

bool CResult::InsetAt(int nIndex,CRsltVariable* pVariable)
{
	if(nIndex>=0 && nIndex<Count())
	{
	   if (pVariable->GetID()<=0)
		  pVariable->SetID(GetNoUsedID());
	   m_tpResult.insert(m_tpResult.begin()+nIndex,pVariable);
	   return true;
	}

	return false;
}

int CResult::Count()
{
	return m_tpResult.size();
}

void CResult::RemoveAt(int nIndex,int nCount)
{
	int count=Count();
	if(nIndex>=0 && nIndex<count && nCount>=0)
	{
		if(nCount+nIndex>count)
			nCount=count-nIndex;

		for(int ii=0;ii<nCount;ii++)
			m_tpResult.erase(m_tpResult.begin()+nIndex);
	}
}

void CResult::RemoveAll()
{
	for (int i=0; i< m_tpResult.size();i++)
	{
		CRsltVariable *pVar = m_tpResult.at(i);
		pVar->Clear();
	}
	m_tpResult.clear();
}

bool CResult::Delete(int nIndex, int nCount)
{
	int count=Count();
	if(nIndex>=0 && nIndex<count && nCount>=0)
	{
		if(nCount+nIndex>count)
			nCount=count-nIndex;

		for(int ii=0;ii<nCount;ii++)
		{
			CRsltVariable* pVariable=m_tpResult.at(nIndex);
			m_tpResult.erase(m_tpResult.begin()+nIndex);
			delete pVariable;
			pVariable=NULL;
		}
		return true;
	}

	return false;
}

void CResult::Clear()
{
	int count=Count();
	for(int ii=0;ii<count;ii++)
	{
		CRsltVariable* pVariable=m_tpResult.at(ii);
		delete pVariable;
		pVariable=NULL;
	}
	m_tpResult.clear();
}

//优化集合的结构（即将其中的基元素结果类对象放在前部，把变量结果类对象放在后部）
void CResult::Optimize()
{
	int count=Count();
	for(int ii=0;ii<count;ii++)
	{
		CRsltVariable* pVariable=m_tpResult.at(ii);
		if(pVariable->GetType()==RSLT_VARIABLE)
		{
			m_tpResult.insert(m_tpResult.begin()+ii);
			m_tpResult.push_back(pVariable);
			ii--;
			count--;
		}
	}
}


unsigned int CResult::GetNoUsedID()
{
	int count=Count();
	for(unsigned int ii=1;ii<1000;ii++)
	{
		bool bNoUse=true;
		for(int jj=0;jj<count;jj++)
		{
			if(Item(jj)->GetID()==ii)
			{
				bNoUse=false;
				break;
			}
		}
		if(bNoUse)
			return ii;
	}
	return 0;
}


CRsltVariable* CResult::GetFirstItem()
{
	if(Count()<=0)
		return NULL;

	CRsltVariable* pVariable=Item(0);
	switch(pVariable->GetType())
	{
	case RSLT_VARIABLE:      //结果类型-变量结果
		return pVariable->GetFirstItem();
		break;
	case RSLT_ELEMENTTEXT:   //结果类型-基元素文本结果
	case RSLT_ELEMENTTABLE:  //结果类型-基元素表结果
	case RSLT_ELEMENTPLOT:   //结果类型-基元素图结果
		return (CRsltVariable*)pVariable;
		break;
	default:
		return NULL;
	}

	return NULL;
}

CRsltObject* CResult::GetByID(unsigned int id)
{
	CRsltVariable* pVariable=NULL,*pRetVariable;
	for(int ii=0;ii<Count();ii++)
	{
		pVariable=Item(ii);
		unsigned int nID=pVariable->GetID();
		if(nID==id)
		{
			pRetVariable=pVariable;
			break;
		}
	}
	return pRetVariable;
}

int CResult::GetSumCount()
{
	int nCount=0,
		nSize=Count();
	for(int i=0;i<nSize;i++)
	{
		CRsltVariable* pVar=Item(i);
		pVar->GetSumCount(nCount);
	}

	return nCount;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction   --CRsltVariable----
//////////////////////////////////////////////////////////////////////

CRsltVariable::CRsltVariable()
{
	m_nID=0;
	m_nType=RSLT_VARIABLE; //当前结果的类型为变量结果;
}

CRsltVariable::CRsltVariable(CTString strName)
{
	m_nID=0;
	m_nType=RSLT_VARIABLE; //当前结果的类型为变量结果;
	m_strName=strName;
}

CRsltVariable::~CRsltVariable()
{
	Clear();
}

void CRsltVariable::Serialize(TCLFile &file)
{
	CRsltObject::Serialize(file);
	int nSize = m_tpVariable.size();
	file << nSize;
	for (int i=0; i< nSize;i++)
		m_tpVariable.at(i)->Serialize(file);
}
void CRsltVariable::Serialize(unsigned char *&pBuf)
{
	CRsltObject::Serialize(pBuf);
	int nSize = m_tpVariable.size();
	SaveDigit(pBuf,nSize);
	for (int i=0; i< nSize;i++)
		m_tpVariable.at(i)->Serialize(pBuf);
	
}

int CRsltVariable::GetBufLen()
{
	int nLen = CRsltObject::GetBufLen();
	int nSize = m_tpVariable.size();
	nLen = nLen +sizeof(int);
	for (int i=0;i<nSize;i++)
		nLen = nLen +m_tpVariable.at(i)->GetBufLen();
	return nLen;
}

void CRsltVariable::Print()
{
	CRsltObject::Print();
	int nSize = m_tpVariable.size();
	printf("Result Variable Size is %d\n",nSize);
	for (int i=0;i < nSize; i++)
	{
		m_tpVariable.at(i)->Print();
	}
}

int CRsltVariable::Add(CRsltVariable* pElement)
{
	if(pElement!=NULL)
	{
		if(pElement->GetID()<=0)
			pElement->SetID(GetNoUsedID());
		m_tpVariable.push_back(pElement);
		return m_tpVariable.size();
	}

	return -1;
}

CRsltVariable* CRsltVariable::Item(int nIndex)
{
	if(nIndex>=0 && nIndex<Count())
		return m_tpVariable.at(nIndex);

	return NULL;
}

int  CRsltVariable::Find(CRsltVariable* pElement)
{
	if(pElement!=NULL)
	{
		for(int ii=0;ii<Count();ii++)
			if(m_tpVariable.at(ii)==pElement)
				return ii;
	}

	return -1;
}

bool CRsltVariable::InsetAt(int nIndex,CRsltVariable* pElement)
{
	if(pElement)
	{
		if(nIndex<0)
			nIndex=0;
		else if(nIndex>Count())
			nIndex=Count();
// yzmAdd
		if(pElement->GetID()<=0)
			pElement->SetID(GetNoUsedID());
		m_tpVariable.insert(m_tpVariable.begin()+nIndex,pElement);
		return true;
	}
	
	return false;
}

int CRsltVariable::Count()
{
	return m_tpVariable.size();
}

void CRsltVariable::RemoveAt(int nIndex,int nCount)
{
	int count=Count();
	if(nIndex>=0 && nIndex<count && nCount>=0)
	{
		if(nCount+nIndex>count)
			nCount=count-nIndex;

		for(int ii=0;ii<nCount;ii++)
			m_tpVariable.erase(m_tpVariable.begin()+nIndex);
	}
}

void CRsltVariable::RemoveAll()
{
	m_tpVariable.clear();
}

bool CRsltVariable::Delete(int nIndex, int nCount)
{
	int count=Count();
	if(nIndex>=0 && nIndex<count && nCount>=0)
	{
		if(nCount+nIndex>count)
			nCount=count-nIndex;

		for(int ii=0;ii<nCount;ii++)
		{
			CRsltVariable* pEle=m_tpVariable.at(nIndex);
			m_tpVariable.erase(m_tpVariable.begin()+nIndex);
			delete pEle;
			pEle=NULL;
		}
		return true;
	}

	return false;
}

void CRsltVariable::Clear()
{
	int count=Count();

	for(int ii=0;ii<count;ii++)
	{
		CRsltVariable* pElement=m_tpVariable.at(ii);
		if(pElement)
			delete pElement;
		pElement=NULL;
	}
	m_tpVariable.clear();
}

unsigned int CRsltVariable::GetNoUsedID()
{
	int count=Count();
	for(unsigned int ii=1;ii<1000;ii++)
	{
		bool bNoUse=true;
		for(int jj=0;jj<count;jj++)
		{
			if(Item(jj)->GetID()==ii)
			{
				bNoUse=false;
				break;
			}
		}
		if(bNoUse)
			return ii;
	}
	return 0;
}

CRsltVariable* CRsltVariable::GetFirstItem()
{
	if(Count()<=0)
		return (CRsltVariable*)this;

	CRsltVariable* pEle=Item(0);
	switch(pEle->GetType())
	{
	case RSLT_VARIABLE:
		return ((CRsltVariable*)pEle)->GetFirstItem();
		break;
	case RSLT_ELEMENTTEXT:
	case RSLT_ELEMENTTABLE:
	case RSLT_ELEMENTPLOT:
		return pEle;
		break;
	default:
		return NULL;
	}

	return NULL;
}

CRsltObject* CRsltVariable::GetByID(unsigned int id)
{
	CRsltVariable* pEle=NULL,*pRetEle;
	for(int ii=0;ii<Count();ii++)
	{
		pEle=Item(ii);
		if(pEle->GetID()==id)
		{
			pRetEle=pEle;
			break;
		}
	}
    return pRetEle;
}


bool  CRsltVariable::GetSumCount(int &nCount)
{
	int nSize=Count();
	if(nSize>0)
	{
		for(int i=0;i<nSize;i++)
		{
			CRsltVariable* pEle=Item(i);
			pEle->GetSumCount(nCount);
		}
	}
	else
		nCount++;

	return true;
}

void CRsltVariable::Copy(CRsltObject* pRsltObject)
{
}

//////////////////////////////////////////////////////////////////////
/////// 表信息类 /////////////////////////////////////////////////////////
CRsltElementTable::CRsltElementTable()
{
	m_nType=RSLT_ELEMENTTABLE; //当前结果的类型为基元素表结果
	m_table=NULL;
}

CRsltElementTable::CRsltElementTable(CTString strName,CTLTable* pTable)
{
	m_nID=0;
	m_strName=strName;
	m_nType=RSLT_ELEMENTTABLE; //当前结果的类型为基元素表结果
	m_table=pTable;
}

CRsltElementTable::~CRsltElementTable()
{
	if(m_table)// && m_table->GetSafeHwnd())
		delete m_table;
	m_table=NULL;
}

void CRsltElementTable::Serialize(TCLFile &file)
{
	CRsltVariable::Serialize(file);
	m_table->Serialize(file);
}
void CRsltElementTable::Serialize(unsigned char *&pBuf)
{
	CRsltVariable::Serialize(pBuf);
	m_table->Serialize(pBuf);
}

int CRsltElementTable::GetBufLen()
{
	int nLen = CRsltVariable::GetBufLen();
	nLen = nLen + m_table->GetBufLen();
	return nLen;
}

void CRsltElementTable::Print()
{
	CRsltVariable::Print();
	m_table->Print();
}
//////////////////////////////////////////////////////////////////////////
/////// 文本信息类 ///////////////////////////////////////////////////////
//构造时共同部分
void CRsltElementText::IniData()
{
	m_nType=RSLT_ELEMENTTEXT; //当前结果的类型为基元素文本结果
}

CRsltElementText::CRsltElementText()
{
	IniData();
	m_strName="";
}

CRsltElementText::CRsltElementText(CTString strName)
{
	IniData();
	m_nID=0;
	m_strName=strName;	
}

CRsltElementText::CRsltElementText(char* strName)
{
	IniData();
	m_nID=0;
	m_strName=strName;	
}

CRsltElementText::~CRsltElementText()
{
	m_strTextArray.RemoveAll();
}


int CRsltElementText::AddString(CTString strValue)
{
	return m_strTextArray.Add(strValue);
}

int CRsltElementText::Count()
{
	return m_strTextArray.GetSize();
}

CTString CRsltElementText::GetString(int nIndex)
{
	return (nIndex>=0 && nIndex<Count())?m_strTextArray.GetAt(nIndex):"";
}

bool CRsltElementText::DeleteAt(int nIndex)
{
	if(nIndex>=0 && nIndex<Count())
	{ 
		m_strTextArray.RemoveAt(nIndex);
		return true;
	}

	return false;
};


void CRsltElementText::Serialize(TCLFile &file)
{
	CRsltVariable::Serialize(file);
	int nSize = m_strTextArray.GetSize();
	file << nSize;
	for (int i=0; i< nSize;i++)
	{
		CTString str = m_strTextArray.GetAt(i);
		file << str;
	}
}


int CRsltElementText::GetBufLen()
{
	int nLen = CRsltVariable::GetBufLen();
	int nSize = m_strTextArray.GetSize();
	nLen = nLen + sizeof(int); 
	for (int i=0; i< nSize;i++)
	{
		CTString str = m_strTextArray.GetAt(i);
		AddStringLen(nLen,str); 
	}
	return nLen;
}

void CRsltElementText::Serialize(unsigned char *&pBuf)
{
	CRsltVariable::Serialize(pBuf);
	int nSize = m_strTextArray.GetSize();
	SaveDigit(pBuf,nSize);
	for (int i=0; i< nSize;i++)
	{
		CTString str = m_strTextArray.GetAt(i);
		SaveString(pBuf,str);
	}
}

void CRsltElementText::Print()
{
	CRsltVariable::Print();
	int nSize = m_strTextArray.GetSize();
	printf("Result Text Size is %d",nSize);
	for (int i=0; i< nSize;i++)
	{
		CTString str = m_strTextArray.GetAt(i);
		printf(str);
		printf("\n");
	}
}

int  CRsltElementText::GetStringRow(CTString string)
{
	int index=0,
		row=0;
	CTString str;
	str.Format("%s\n","");
	while(index>=0)
	{
		index=string.Find(str,index);
		if(index>=0)
		{
			row++;
			index++;
		}
		else
			break;
	}

	return row;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRsltElementChart::CRsltElementChart()
{
	m_nType=RSLT_ELEMENTPLOT;
	m_pChart=NULL;
}


CRsltElementChart::~CRsltElementChart()
{  
	if(m_pChart)// && m_pChart->GetSafeHwnd() )
		delete m_pChart;
	m_pChart=NULL;	
}

CRsltElementChart::CRsltElementChart(CTString strName, CTChart *pChart)
{
	m_nID=0;
	m_strName=strName;
	m_nType=RSLT_ELEMENTPLOT; //当前结果的类型为基元素表结果
	m_pChart=pChart;

}

void CRsltElementChart::Serialize(TCLFile &file)
{

	CRsltVariable::Serialize(file);
	int nChartKind = 1;
	int nChartType = 1;//区别PP图和QQ图
	
	file >> nChartKind;
	if (nChartKind == CTChart::PP)
		nChartType = 0;
	file >> nChartType; 
	
	CTChart::EChartKind ChartKind=CTChart::EChartKind (nChartKind);
	int nChart = ChartKind;
	file >> nChart;
	switch(ChartKind)
	{
	case CTChart::Line:
		m_pChart=new CTChartLine;
		break;
	case CTChart::Bar:
		m_pChart=new CTChartBar;
		break;
	case CTChart::Pie:
		m_pChart=new CTChartPie;
		break;
	case CTChart::Area:
		m_pChart=new CTChartArea;
		break;
	case CTChart::Boxplot:
		m_pChart=new CTChartBoxplot;
		break;
	case CTChart::Histogram:
		m_pChart=new CTChartHistogram;
		break;
	case CTChart::Pareno:
		m_pChart=new CTChartPareto;
		break;
	case CTChart::ErrorBar:
		m_pChart=new CTChartErrBar;
		break;
	case CTChart::Sequence:
		m_pChart=new CTChartSequence;
		break;
	case CTChart::Scatter:
		m_pChart=new CTChartScatterPlot;
		break;
	case CTChart::Autocorrelations:
		m_pChart=new CTChartAutocorrelations;
		break;
	 case CTChart::Crosscorrelations:
		m_pChart=new CTChartCrossCorrelations;
		break;
	 case CTChart::Control://WM 2002/12/04
		m_pChart=new CTChartControl;
		break;
	 case CTChart::ROC:    //WM 2002/12/04
		m_pChart=new CTChartROC;
		break;
	 case CTChart::HighLow:
		m_pChart=new CTChartHighLow;
		break;
	 case CTChart::PP:     //WM 2002/12/04
		m_pChart=new CTChartPP(nChartType);
		break;			
	 case CTChart::Pedigree:    //梅肖明 2004-06-01
		m_pChart=new CTChartPedigree();
		break;	
	}
// 	m_pChart->Serialize(file); 
}

void CRsltElementChart::Serialize(unsigned char *&pBuf)
{
	CRsltVariable::Serialize(pBuf);
	m_pChart->Serialize(pBuf); 
}

int CRsltElementChart::GetBufLen()
{
	int nLen = CRsltVariable::GetBufLen();
	nLen = nLen + m_pChart->GetBufLen();
	return nLen;
}


void CRsltElementChart::Print()
{
	CTChart::EChartKind ChartKind= m_pChart->m_ChartKind;
	switch(ChartKind)
	{
	case CTChart::Line:
		printf("Chart Type is %s\n","Line");
		break;
	case CTChart::Bar:
		printf("Chart Type is %s\n","Bar");
		break;
	case CTChart::Pie:
		printf("Chart Type is %s\n","Pie");
		break;
	case CTChart::Area:
		printf("Chart Type is %s\n","Area");
		break;
	case CTChart::Boxplot:
		printf("Chart Type is %s\n","Boxplot");
		break;
	case CTChart::Histogram:
		printf("Chart Type is %s\n","Histogram");
		break;
	case CTChart::Pareno:
		printf("Chart Type is %s\n","Pareno");
		break;
	case CTChart::ErrorBar:
		printf("Chart Type is %s\n","ErrorBar");
		break;
	case CTChart::Sequence:
		printf("Chart Type is %s\n","Sequence");
		break;
	case CTChart::Scatter:
		printf("Chart Type is %s\n","Scatter");
		break;
	case CTChart::Autocorrelations:
		printf("Chart Type is %s\n","Autocorrelations");
		break;
	case CTChart::Crosscorrelations:
		printf("Chart Type is %s\n","Crosscorrelations");
		break;
	case CTChart::Control://WM 2002/12/04
		printf("Chart Type is %s\n","Control");
		break;
	case CTChart::ROC:    //WM 2002/12/04
		printf("Chart Type is %s\n","ROC");
		break;
	case CTChart::HighLow:
		printf("Chart Type is %s\n","HighLow");
		break;
	case CTChart::PP:     //WM 2002/12/04
		printf("Chart Type is %s\n","PP");
		break;			
	case CTChart::Pedigree:    //梅肖明 2004-06-01
		printf("Chart Type is %s\n","Pedigree");
		break;	
	}

	m_pChart->Print(); 
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRsltEleDecTree::CRsltEleDecTree()
{
	m_nType=RSLT_ELEMENTDECTREE;
	m_strName="决策树";
	
}

CRsltEleDecTree::CRsltEleDecTree(CTString strName,CDecTreeWnd *pDecTreeWnd)
{
	m_nType=RSLT_ELEMENTDECTREE;
	m_strName=strName;
	m_pDecTreeWnd=pDecTreeWnd;
}

CRsltEleDecTree::~CRsltEleDecTree()
{
	if (m_pDecTreeWnd!=NULL)
		delete m_pDecTreeWnd;
	m_pDecTreeWnd=NULL;
}

void CRsltEleDecTree::Serialize(TCLFile &file)
{
	CRsltVariable::Serialize(file);
	m_pDecTreeWnd->Serialize(file);
}

void CRsltEleDecTree::Serialize(unsigned char *&pBuf)
{
	CRsltVariable::Serialize(pBuf);
	m_pDecTreeWnd->Serialize(pBuf); 
}

int CRsltEleDecTree::GetBufLen()
{
	int nLen = CRsltVariable::GetBufLen();
	nLen = nLen + m_pDecTreeWnd->GetBufLen(); 
	return nLen;
}

void CRsltEleDecTree::Print()
{
	CRsltVariable::Print();
	m_pDecTreeWnd->Print();
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CTLTable::CTLTable()
{
	m_strTitle = ("");
	m_strHead  =  ("");
	m_strTail  =  ("");
	m_strException = "";
	m_nRows = 0;
	m_nCols = 0;
}

CTLTable::~CTLTable()
{
	Clear();
}

void CTLTable::Clear()
{
	m_arColumnInfo.RemoveAll();
	for(int i = 0 ;i<m_arRowInfo.GetSize();i++)
	{
		TRowData * pRowData = (TRowData *)m_arRowInfo.GetAt(i);
		if (pRowData != NULL)
		{
			for (int j=0; j< pRowData->m_arData.GetSize();j++)
			{
				TableData *pData = (TableData *)pRowData->m_arData.GetAt(j);
				if (pData != NULL)
				{
					if (pData->nDataType == rString)
						delete [] pData->pValue;
				}
			}
			pRowData->m_arData.RemoveAll();
		}
	}
	m_arRowInfo.RemoveAll();
	m_arDataInfo.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CTLTable drawing
void CTLTable::Serialize(TCLFile &file)
{
}

void CTLTable::Serialize(unsigned char *&pBuf)
{
	SaveDigit(pBuf,m_nRows);//表单的总行数
	SaveDigit(pBuf,m_nCols);//表单的总列数
	SaveString(pBuf,m_strTitle);
	SaveString(pBuf,m_strHead);
	SaveString(pBuf,m_strTail);
	int i=0;
	for (i=0; i< m_nCols;i++)
		SaveString(pBuf,m_arColumnInfo.GetAt(i));
	for (i = 0; i< m_nRows;i++)
	{
		TRowData * pRow = (TRowData *)m_arRowInfo.GetAt(i);
		//if (pRow != NULL)
		{
			//printf("%s    ", (char *)pRow->m_szName);
			SaveString(pBuf,pRow->m_szName);
			for (int j=0; j< m_nCols-1;j++)
			{
				TableData *pData =  (TableData *)pRow->m_arData.GetAt(j);
				
				if (pData != NULL)
				{
					SaveDigit(pBuf,pData->nDataType);//保存类型
					if (pData->nDataType == rInt)
						SaveDigit(pBuf,pData->nValue)
					else if (pData->nDataType == rDouble)
						SaveDigit(pBuf,pData->dValue)
					else if (pData->nDataType == rString)
					{
						CTString str = pData->pValue;
						SaveString(pBuf,str);
					}
				}
				else
				{//出错 空
					int nv = rNull;
					SaveDigit(pBuf,nv);//保存类型
				}
			}
			
		}
	}
}

int CTLTable::GetBufLen()
{
	int nLen = 0;
	AddDigitLen(nLen,m_nRows);//表单的总行数
	AddDigitLen(nLen,m_nCols);//表单的总列数
	AddStringLen(nLen,m_strTitle);
	AddStringLen(nLen,m_strHead);
	AddStringLen(nLen,m_strTail);

	int i=0;
	for (i=0; i< m_nCols;i++)
		AddStringLen(nLen,m_arColumnInfo.GetAt(i));
	for (i = 0; i< m_nRows;i++)
	{
		TRowData * pRow = (TRowData *)m_arRowInfo.GetAt(i);
		//if (pRow != NULL)
		{
			//printf("%s    ", (char *)pRow->m_szName);
			AddStringLen(nLen,pRow->m_szName);
			for (int j=0; j< m_nCols-1;j++)
			{
				TableData *pData =  (TableData *)pRow->m_arData.GetAt(j);
				
				if (pData != NULL)
				{
					AddDigitLen(nLen,pData->nDataType);//保存类型
					if (pData->nDataType == rInt)
						AddDigitLen(nLen,pData->nValue);
					else if (pData->nDataType == rDouble)
						AddDigitLen(nLen,pData->dValue);
					else if (pData->nDataType == rString)
					{
						CTString str = pData->pValue;
						AddStringLen(nLen,str);
					}
				}
				else
				{//出错
					nLen = nLen + sizeof(int);
				}
			}
			
		}
	}
	return nLen;
}

void CTLTable::Print()
{
	printf("Table Title is %s\n",m_strTitle.GetData());
	printf("Table Head is %s\n",m_strHead.GetData());
	printf("Table Tail is %s\n",m_strTail.GetData());
	printf("Table Begin:\n");

	int i=0; 
	for (i=0; i< m_nCols;i++)
		printf("%s    ",m_arColumnInfo.GetAt(i).GetData());
	printf("\n");

	for (i = 0; i< m_nRows;i++)
	{
		TRowData * pRow = (TRowData *)m_arRowInfo.GetAt(i);
		if (pRow != NULL)
		{
			printf("%s    ", (char *)pRow->m_szName);
			for (int j=0; j< m_nCols-1;j++)
			{
				TableData *pData =  (TableData *)pRow->m_arData.GetAt(j);
				if (pData != NULL)
				{
					if (pData->nDataType == rInt)
						printf("%d\t", pData->nValue);
					else if (pData->nDataType == rDouble)
						printf("%.2f\t",pData->dValue);
					else if (pData->nDataType == rString)
						printf("%s\t",pData->pValue);
				}
				else
				{
					printf("NULL\t");
				}
			}
			printf("\n");
		}

	}
	printf("Table End\n");
}

/////////////////////////////////////////////////////////////////////////////
// CTLTable diagnostics

/////////////////////////////////////////////////////////////////////////////
// CTLTable message handlers

bool CTLTable::CreateTable(int nRow, int nCol)
{
	m_nRows = 0;
	m_nCols = 0;
//	m_nRows = nRow;
//	m_nCols = nCol;
//	m_arColumnInfo.SetSize(nCol);
//	m_arRowInfo.SetSize(nRow-1);
	return true;
}

void CTLTable::SetRows(int nRow)
{
	//m_nRows = 0;
//	m_nRows = nRow;
//	m_arRowInfo.SetSize(nRow-1);
}

void CTLTable::SetCols(int nCol)
{
	//m_nCols = 0;
//	m_nCols = nCol;
//	m_arColumnInfo.SetSize(nCol);
}

void CTLTable::SetTitle(char *pszTitle)
{
	m_strTitle = pszTitle;
}

void CTLTable::SetTitle(CTString pszTitle)
{
	m_strTitle = pszTitle;
}

void CTLTable::SetTail(CTString str)
{
	m_strTail = str;	
}

void CTLTable::SetTail(char * str)
{
	m_strTail = str;
}

void CTLTable::SetHead(char * str)
{
	m_strHead = str;
}

void CTLTable::SetHead(CTString str)
{
	m_strHead = str;
}

void CTLTable::InsertColumn(int nItemNum, CTString strColumnName)
{
//	if ((nItemNum < m_nCols)&&(nItemNum >= 0))
//		m_arColumnInfo.InsertAt(nItemNum,strColumnName);
//	else
		m_arColumnInfo.Add(strColumnName);
	m_nCols++;
}

void CTLTable::InsertItem(int nSubItem, CTString strRowName,bool bFlag)
{
	TRowData *pRowData = new TRowData;
	pRowData->m_szName = strRowName;
	for (int i = 0; i< m_nCols-1;i++)
	{
		pRowData->m_arData.Add(NULL);
	}
	

//	if ((nSubItem < m_nRows-1)&&(nSubItem >= 0))
//		m_arRowInfo.InsertAt(nSubItem,pRowData);
//	else
		m_arRowInfo.Add(pRowData);
	m_nRows++;
}

/*
void CTLTable::InsertColumn(int nItemNum, char *pColumnName)
{
	if ((nItemNum < m_nCols)&&(nItemNum >= 0))
		m_arColumnInfo.InsertAt(nItemNum,pColumnName);
	else
		m_arColumnInfo.Add(pColumnName);
	m_nCols++;
}

void CTLTable::InsertItem(int nSubItem, char *pRowName,bool bFlag)
{
	TRowData *pRowData = new TRowData;
	pRowData->m_szName = pRowName;
	if ((nSubItem < m_nRows-1)&&(nSubItem >= 0))
		m_arRowInfo.InsertAt(nSubItem,pRowData);
	else
		m_arRowInfo.Add(pRowData);
	m_nRows++;
}
*/
/*
 * nItemNum 为行号，从0开始（不考虑头）  
 * nSubItem 列号  ，从1开始（不考虑头）  
 */
void CTLTable::SetItemText(const int nItemNum, const int nSubItem, int nData)
{
	AutoSize(nItemNum,nSubItem);



	if ((nItemNum >= 0) &&(nSubItem > 0) &&(nItemNum < m_nRows) &&(nSubItem < m_nCols))
	{
		TableData * pData = new TableData;
		pData->nDataType = rInt;
		pData->nValue = nData;
		TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		if (pRow == NULL)
		{
			pRow = new TRowData;
			m_arRowInfo.SetAt(nItemNum,pRow);
		}

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);

		if (pRow->m_arData.GetAt(nSubItem-1) )
			delete (TableData*)pRow->m_arData.GetAt(nSubItem-1);
		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
	else
	{
		TableData * pData = new TableData;
		pData->nDataType = rInt;
		pData->nValue = nData;
		TRowData *pRow = new TRowData;
		m_arRowInfo.SetAt(nItemNum,pRow);

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);

		if (pRow->m_arData.GetAt(nSubItem-1) )
			delete (TableData*)pRow->m_arData.GetAt(nSubItem-1);

		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
}

void CTLTable::SetItemText(const int nItemNum, const int nSubItem, double dbData)
{
	AutoSize(nItemNum,nSubItem);



	if ((nItemNum >= 0) &&(nSubItem > 0) &&(nItemNum < m_nRows) &&(nSubItem < m_nCols))
	{
		TableData * pData = new TableData;
		pData->nDataType = rDouble;
		pData->dValue = dbData;
		TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		if (pRow == NULL)
		{
			pRow = new TRowData;
			m_arRowInfo.SetAt(nItemNum,pRow);
		}

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);
		if (pRow->m_arData.GetAt(nSubItem-1) )
			delete (TableData*)pRow->m_arData.GetAt(nSubItem-1);

		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
	else
	{
		TableData * pData = new TableData;
		pData->nDataType = rDouble;
		pData->dValue = dbData;
		TRowData *pRow = new TRowData;
		m_arRowInfo.SetAt(nItemNum,pRow);
		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);

		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
}

void CTLTable::SetItemText(const int nItemNum, const int nSubItem, char *pszName)
{
	AutoSize(nItemNum,nSubItem);



	if ((nItemNum >= 0) &&(nSubItem > 0) &&(nItemNum < m_nRows) &&(nSubItem < m_nRows))
	{
		TableData * pData = new TableData;
		pData->nDataType = rString;
		pData->pValue = new char[strlen(pszName)+1];
		memcpy(pData->pValue,pszName,strlen(pszName)+1);
		TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		if (pRow == NULL)
		{
			pRow = new TRowData;
			m_arRowInfo.SetAt(nItemNum,pRow);
		}

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);
		if (pRow->m_arData.GetAt(nSubItem-1) )
			delete (TableData*)pRow->m_arData.GetAt(nSubItem-1);

		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
	else
	{
		TableData * pData = new TableData;
		pData->nDataType = rString;
		pData->pValue = new char[strlen(pszName)+1];
		memcpy(pData->pValue,pszName,strlen(pszName)+1);

		TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);
		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
}

void CTLTable::SetItemText(const int nItemNum,const int nSubItem,CTString strData)
{
	AutoSize(nItemNum,nSubItem);


	if ((nItemNum >= 0) &&(nSubItem > 0) &&(nItemNum < m_nRows) &&(nSubItem < m_nCols))
	{
		TableData * pData = new TableData;
		pData->nDataType = rString;
		pData->pValue = new char[strData.GetLength()+1];
		memcpy(pData->pValue,(char *)strData,strData.GetLength()+1);
		TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		if (pRow == NULL)
		{
			pRow = new TRowData;
			m_arRowInfo.SetAt(nItemNum,pRow);
		}

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);
		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
	else
	{
		TableData * pData = new TableData;
		pData->nDataType = rString;
		pData->pValue = new char[strData.GetLength()+1];
		memcpy(pData->pValue,(char *)strData,strData.GetLength()+1);
		TRowData *pRow = new TRowData;
		m_arRowInfo.SetAt(nItemNum,pRow);

		while (pRow->m_arData.GetSize()<m_nCols-1)
			pRow->m_arData.Add(NULL);

		pRow->m_arData.SetAt(nSubItem-1,pData);
	}
}

long CTLTable::GetTableRowS()
{
	return m_nRows;
}

int CTLTable::GetTableColS()
{
	return m_nCols;
}

TableData*  CTLTable::GetItemText(const int nItemNum,const int nSubItem)
{
	if ((nItemNum >= 0) &&(nSubItem > 0) &&(nItemNum < m_nRows) &&(nSubItem < m_nCols))
	{
		TRowData * pRow = (TRowData *)m_arRowInfo.GetAt(nItemNum);
		return  (TableData*)pRow->m_arData.GetAt(nSubItem-1);
	}
	return NULL;
}

CTString CTLTable::GetColumnInfo(int nIndex)
{
	if(nIndex<0 || nIndex>=GetTableColS())
		return "";

	return m_arColumnInfo.GetAt(nIndex);
}

CTString CTLTable::GetRowInfo(int nIndex)
{
	if(nIndex<0 || nIndex>=GetTableRowS())
		return  "";
	TRowData *pRow = (TRowData *)m_arRowInfo.GetAt(nIndex);
	return pRow->m_szName;
}

/*
 * nRows 为个数   ，（不考虑头）  －1 不动
 * nSubItem 列数  ，（考虑头行）  －1 不动
 */
void CTLTable::AutoSize(int nRows, int nCols)
{
	if (nRows !=-1 && nRows>=0)
		while(nRows>=m_nRows)//自动添加行
			InsertItem(0,"");

	if (nCols != -1 && nCols>0)
	{
		for(int i = 0 ;i<m_arRowInfo.GetSize();i++)
		{
			TRowData * pRowData = (TRowData *)m_arRowInfo.GetAt(i);
			while(pRowData->m_arData.GetSize()<nCols)
			{
				pRowData->m_arData.Add(NULL);
			}
		}
	}
}
