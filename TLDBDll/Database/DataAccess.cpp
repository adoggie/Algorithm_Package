// DataAccess.cpp: implementation of the CDataAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
#include <assert.h>
#include "DataAccess.h"
#include "fieldlist.h"
#include "cfields.h"
#include "cfakerecnos.h"
#include "../../Public/System.h"
//#include "Sort.h"
#include "DriverBase.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
class CTimeTest
{
	DWORD m_start;
	DWORD m_mid;
	DWORD m_end;
public:
	CTimeTest()
	{
		m_start = GetTickCount();
	}
	Book()
	{
		m_mid = GetTickCount();
	}
	~CTimeTest()
	{
		m_end = GetTickCount();
		CTString str;
		str.Format("花费时间前:%d,:%d 总时间:%d",m_mid - m_start,m_end - m_mid,m_end - m_start);
		//===temp AfxMessageBox(str);
	}
};
//*/




CDataAccess::CDataAccess()
{
	m_pBuf				= new BYTE;
	m_nBufLen			= 1;
	m_pPages			= new CPages();
	m_pFieldList		= new CFieldList(m_pPages);
	NewFile();
}
//###_Mod 2002-10-31 zlq
//从样板拷贝一个数据库字段结构，但没有记录信息
CDataAccess::CDataAccess(CDataAccess*pTemplet)
{
	m_pBuf				= new BYTE;
	m_nBufLen			= 1;
	m_pPages			= new CPages();
	m_pFieldList		= new CFieldList(m_pPages);
	NewFile();
	CDataAccess *pDestDA = this;

	for (int i = 0;i< pTemplet->m_pFieldList->GetFieldCount();i++)
	{//复制字段信息
		CField * pSourceField = pTemplet->m_pFieldList->FieldByIndex(i);
		CField * pDestField = pTemplet->m_pFieldList->CreateField(pSourceField);
		pDestField->Clone(pSourceField );
		pDestDA->m_pFieldList->Add(pDestField);	
	}
}

CDataAccess::~CDataAccess()
{
	if(m_pPages)
	{
		delete m_pPages;
		m_pPages = 0;
	} 

	if(m_pFieldList)
	{
		delete m_pFieldList;
		m_pFieldList = 0;
	} 
	if(m_pBuf)
	{
		delete m_pBuf;
		m_pBuf = 0;
	}
}


//typedef AFX_EXT_API  bool (*pLoadFile)(CDataAccess* pDataAccess,LPCTSTR lpszFileName);
bool CDataAccess::LoadFile(LPCTSTR lpszFileName)
{
	m_strFileName = lpszFileName;
	m_strFileName = TCSystem::ConvertPath(m_strFileName);
	CTString str = lpszFileName;
	int iPos = str.ReverseFind('.');
	if(iPos != 0)
		str.Delete(0,iPos+1);
	str.MakeLower();
	bool rst = false;
	if(str == "xls")
	{
/*===temp  		HINSTANCE hDll = AfxLoadLibrary("xlsPumper.dll");
		if (!hDll)
		{
			//===temp AfxMessageBox("没有找到相应的xlsPumper.dll,装载失败!");
			m_bIsOpen =false;
			return false;
		}
		NewFile();
		//m_strFileName = lpszFileName;
		pLoadFile pLf;
		pLf =(pLoadFile) GetProcAddress(hDll,"LoadFile");
		m_bIsOpen = false;
		if(pLf == NULL)
			//===temp AfxMessageBox("没有找到相应的LoadFile,装载失败!");
		else
			m_bIsOpen = (*pLf)(this,lpszFileName);
		AfxFreeLibrary(hDll);
		rst =  m_bIsOpen;   */
	}
	else if (str == "mkw" ||str == "tl")
	{
		m_bIsOpen = m_pPages->LoadFile(m_strFileName.GetData());

		rst =  m_bIsOpen ;
	}
	else if(str==_T("dbf"))// foxpro或者dBase [4/21/2003]swx
	{
/*  		NewFile();
		HINSTANCE hIns=::LoadLibrary("DbfResolve.dll");
		if(hIns!=NULL)
		{
			typedef bool (*ImportDbf)(CTString ,CDataAccess& );
			ImportDbf pImportDbf=(ImportDbf)::GetProcAddress(hIns,"ImportDbf");
			if(pImportDbf)
				m_bIsOpen=(bool)pImportDbf(lpszFileName,*this);
			rst =  m_bIsOpen ;
		}
		else
		{
			//===temp AfxMessageBox("缺少ImportDbf.dll! 文件不能打开");
			return false;
		}   */
	}	
	else //if (str == "txt")
    {
/*  		int nMaxPageCount,nPageNormalSize,nUsePage;
		this->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
		this->SetMaxPageCount(5);//设定页面大小
		HINSTANCE hwnd = AfxLoadLibrary("TLTextCV.dll");
		if (!hwnd)
		{
			//===temp AfxMessageBox("没有找到相应的TLTextCV.dll装载失败!");
			m_bIsOpen = false;
			this->SetMaxPageCount(nMaxPageCount);//恢复页个数设定
			return false;
		}
		NewFile();

		pLoadFile pLf;
		pLf =(pLoadFile) GetProcAddress(hwnd,"LoadFile");
		m_bIsOpen =   (*pLf)(this,lpszFileName);
		AfxFreeLibrary(hwnd);
		this->SetMaxPageCount(nMaxPageCount);//恢复页个数设定
		rst =  m_bIsOpen ;   */
	}
	//*************页面设置**********************
	if (rst)
		this->m_pPages->AdjustPageCount();
	return rst;
}

bool CDataAccess::NewFile( )
{	
	m_strFileName = "";
	m_pPages->NewFile();
	m_bIsOpen = true;
	return true;
}

//以旧版本文件保存 nVersion 版本号
bool CDataAccess::SaveOldVersionFile(LPCTSTR lpszFileName,CTString *pstrOutFile,int nVersion)
{
	m_pPages->SetSaveFileVersion(nVersion);
	bool bRst = SaveFile(lpszFileName,pstrOutFile);
	m_pPages->SetSaveFileVersion(0);//恢复为最新
	return bRst;
}
//pstrOutFile 返回false 时且保存为天律文件时有效, 为保存时出错时的另存为的文件名字,由内部生成
bool CDataAccess::SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile)
{
	bool bOldVesion = false;
	if (*pstrOutFile =="OLD1.1")
	{
		bOldVesion = true;
	}
	m_strFileName = lpszFileName;
	m_strFileName = TCSystem::ConvertPath(m_strFileName);
	CTString str = lpszFileName;
	int iPos = str.ReverseFind('.');
	if(iPos != 0)
		str.Delete(0,iPos+1);
	str.MakeLower();
	if (str=="xls")
	{
/*===temp  		typedef void (*pPumpToExcel)(CDataAccess* pCDataAccess,LPCTSTR lpszFileName);
		HINSTANCE hDll = AfxLoadLibrary("xlsPumper.dll");
		if (!hDll)
		{
			//===temp AfxMessageBox("没有找到相应的xlsPumper.dll,装载失败!");
			return false;
		}
		pPumpToExcel pSave;
		pSave =(pPumpToExcel) GetProcAddress(hDll,"PumpToExcel");
		if(pSave == NULL)
			//===temp AfxMessageBox("没有找到相应的PumpToExcel,装载失败!");
		else
			(*pSave)(this,lpszFileName);//"D:\\Demo3.xls"
		AfxFreeLibrary(hDll);   */
		return true;
		
	}
	else if (str == "mkw")
	{
		if (bOldVesion ==false)
		{
			return         m_pPages->SaveFile(m_strFileName.GetData(),pstrOutFile);
		}
		else
		{//老版本文件
/* 			typedef bool (*pPumpToOld)(CDataAccess* pCDataAccess,LPCTSTR lpszFileName,CTString *strOut);
 			HINSTANCE hDll = AfxLoadLibrary("OdData.dll");
			if (!hDll)
			{
				//===temp AfxMessageBox("没有找到相应的OdData.dll,装载失败!");
				return false;
			}
			bool rst = false;
			pPumpToOld pSave;
			pSave =(pPumpToOld) GetProcAddress(hDll,"SaveOldData");
			if(pSave == NULL)
				//===temp AfxMessageBox("没有找到相应的SaveOldData,装载失败!");
			else
				rst = (*pSave)(this,lpszFileName,pstrOutFile);//"D:\\Demo3.xls"
			AfxFreeLibrary(hDll);
			return rst;   */
		}
	}

	else //txt ...
	{
		/*
		ofstream out(lpszFileName,ios::trunc);
		//assert(out);
		
		CDataAccess * pDataAccess ;
		pDataAccess = this;
		pDataAccess->First();
		int nFieldcount= pDataAccess->m_pFieldList->GetFieldCount();
		bool isFirst = true;
		for (int i =0;i<nFieldcount;i++)
		{//先输出字段名，其它不输出
				if (isFirst)
				{
					out << pDataAccess->m_pFieldList->FieldByIndex(i)->GetFieldName();
					isFirst = false;
				}
				else
				{
					out<<"\t"+pDataAccess->m_pFieldList->FieldByIndex(i)->GetFieldName();
				}
			
		}
		out<<endl;
		while (!pDataAccess->Eof())
		{
			bool isFirst = true;
			for (int i =0;i<nFieldcount;i++)
			{
				if (isFirst)
				{
					out<<pDataAccess->m_pFieldList->FieldByIndex(i)->SetToDisplay();
					isFirst = false;
				}
				else
				{
					out<<"\t"+pDataAccess->m_pFieldList->FieldByIndex(i)->SetToDisplay();
				}
			}
			out<<endl;
			pDataAccess->Next();
		}
		*/
	}
	return true;
}

bool CDataAccess::Bof()
{
	return 	m_pPages->Bof();
}
bool CDataAccess::Eof()
{
	return 	m_pPages->Eof();
}

bool CDataAccess::SetCurRecNo(int RecNo)
{
    if (RecNo<1||RecNo>m_pPages->GetRecordCount())
            return false;
    return  m_pPages->SetCurRecNo(RecNo);}

int CDataAccess::Next()
{	
	return 	m_pPages->Next();
}

int CDataAccess::Prior()
{
	return 	m_pPages->Prior ();
}

int CDataAccess::MoveBy(int iSkipLen ,CFromPos MoveFlags)
{	
	return 	m_pPages ->MoveBy(iSkipLen,MoveFlags);
}

int CDataAccess::First()
{
	return 	m_pPages->First ();
}

int CDataAccess::Last()
{	
	return 	m_pPages->Last ();
}


int CDataAccess::GetRecordCount()
{	
	if (m_pPages->GetRecordCount()==UnknownCount)
	{//#_修改 2002-11-26 $ 8:30:46 zlq
		m_pPages->SetCurRecNo(UnknownCount);
	}
	return 	m_pPages->GetRecordCount();
}

int CDataAccess::GetRecNo()
{	
	return 	m_pPages->GetRecNo();
}


bool CDataAccess::Edit()
{
	return m_pPages->_Edit();
}

bool CDataAccess::Append()
{	 
	return m_pPages->_Append();
}

bool CDataAccess::Delete()
{	  
	return m_pPages->Delete();
}

bool CDataAccess::Insert()
{	  
	return m_pPages ->_Insert();
}

bool CDataAccess::Cancel()
{	
	return false;
	return m_pPages ->Cancel() ;

}

bool CDataAccess::Post()
{	 
	return true;//m_pPages->Post();
}
/************************************************************************************
函数名称：

功能说明：
		赋值记录，数据集已经处于可写状态
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-10-23
修改:

************************************************************************************/
bool CDataAccess::CopyRecord(CPages * pDestPages,CPages * pSourcePages)
{
	for(int j = 0;j < pSourcePages->m_pFields->GetFieldCount();j++)
	{
		CField * pSourceField = pSourcePages->m_pFields->FieldByIndex(j);
		CField * pDestField = pDestPages->m_pFields->FieldByIndex(j+1);
		if(pSourceField->IsNull())
		{
			pDestField->SetNull();
		}
		else
		{
			switch(pSourceField->GetFieldType())
			{
			case fString :
				pDestField->SetAsString(pSourceField->GetAsString());
				break;
			case fInt :
				pDestField->SetAsInteger(pSourceField->GetAsInteger());
				break;
			case fBoolean :
				pDestField->SetAsBool(pSourceField->GetAsBool());
				break;
			case fDate :
				pDestField->SetAsDateTime(&pSourceField->GetAsDateTime());
				break;

			case fCurrency :
				pDestField->SetAsCurrency(pSourceField->GetAsCurrency());
				break;
			case fDouble:
				pDestField->SetAsDouble(pSourceField->GetAsDouble());
				break;
			}
		}
	}
	return true;
}
bool CDataAccess::CopyRecord(CDataAccess * pDest,CDataAccess * pSource)
{
	for(int j = 0;j < pSource->m_pFieldList->GetFieldCount();j++)
	{
		CField * pSourceField = pSource->m_pFieldList->FieldByIndex(j);
		CField * pDestField = pDest->m_pFieldList->FieldByIndex(j);
		if(pSourceField->IsNull())
		{
			pDestField->SetNull();
		}
		else
		{
			switch(pSourceField->GetFieldType())
			{
			case fString :
				pDestField->SetAsString(pSourceField->GetAsString());
				break;
			case fInt :
				pDestField->SetAsInteger(pSourceField->GetAsInteger());
				break;
			case fBoolean :
				pDestField->SetAsBool(pSourceField->GetAsBool());
				break;
			case fDate :
				pDestField->SetAsDateTime(&pSourceField->GetAsDateTime());
				break;

			case fCurrency :
				pDestField->SetAsCurrency(pSourceField->GetAsCurrency());
				break;
			case fDouble:
				pDestField->SetAsDouble(pSourceField->GetAsDouble());
				break;
			}
		}
	}
	return true;
}


struct Item{
	int m_nLen;
	int m_nOffset;
	bool m_bIsString;
	int m_nAsc;
};

CTArray<Item,Item> g_ItemAry;
int Comp(void *pData1,void *pData2)
{
	int nLen = g_ItemAry.GetSize();
	for (int i=0;i<nLen ;i++)
	{
		int rst = 0;//========
		if (g_ItemAry.GetAt(i).m_bIsString)
		{
			int nOffset = g_ItemAry.GetAt(i).m_nOffset;
			CTString s1((LPCTSTR)( (BYTE*)pData1+ nOffset ),g_ItemAry.GetAt(i).m_nLen);
			CTString s2((LPCTSTR)( (BYTE*)pData2+ nOffset ),g_ItemAry.GetAt(i).m_nLen);
			rst = strcmp(s1,s2);
		}
		else
		{
			int nOffset = g_ItemAry.GetAt(i).m_nOffset;
			double a1 = *(double*)( (BYTE*)pData1+ nOffset );
			double a2 = *(double*)( (BYTE*)pData2+ nOffset );
			if (a1 == a2)
				rst = 0;
			else if (a1 ==NULLValue)
				rst= -g_ItemAry.GetAt(i).m_nAsc;
			else if (a2 == NULLValue)
				rst= g_ItemAry.GetAt(i).m_nAsc;
			else if (a1>a2)
				rst = g_ItemAry.GetAt(i).m_nAsc;
			else
				rst = -g_ItemAry.GetAt(i).m_nAsc;
		}
		
		if (rst!=0)
			return rst;
	}
	return 0;
}

int compare( const void *arg1, const void *arg2 )
{
	return Comp((void*)*( int *)arg1,(void*)*( int *)arg2);

}


void mysort(CDataAccess &ColAry,CDataAccess ** ppOutDA)
{
	int nTemp0;
	CFields *pFields = ColAry.m_pPages->m_pFields;
	int nTotalCount = ColAry.GetRecordCount();
	int nRecordLen = pFields->QueryLength();

	int nBufCount = 10000*10;//===temp  动态大小
	int nTemp =0;
	if (TCSystem::GetProfileIntValue("SortSetup","SortMemNum(MB)",nTemp ))
	{
		nTemp = nTemp *1024*1024 /nRecordLen;
		if (nTemp> 500)//确保至少500条记录
			nBufCount = nTemp;
	}
	if (nTotalCount < nBufCount)//防止内存设定过大,最大不超过总记录条数
		nBufCount = nTotalCount;
	

	//int nBufsize = 50;
	int *pIndexBuf = new int[nBufCount ];
	BYTE *pData = new BYTE[nRecordLen*nBufCount];

	BYTE *pDataB = new BYTE[nRecordLen];
	
	CDataAccess*pIn = new CDataAccess(&ColAry);
	CDataAccess*pOut = new CDataAccess(&ColAry);
	int nCurPos =0;
	while(nCurPos<nTotalCount)
	{
		int nStartPos = nCurPos;
		int nEndPos = nStartPos+nBufCount;
		if (nEndPos >= nTotalCount)
			nEndPos = nTotalCount;
		
		for (int i= nStartPos ;i<nEndPos;i++)
		{
			ColAry.SetCurRecNo(i+1);
			void *pTemp = pData+ (i-nStartPos)*nRecordLen;
			pFields->SaveDataToBuffer((LPBYTE)pTemp);			
			*(pIndexBuf+i-nStartPos) = (int)pTemp;//保存地址
		}
		
		qsort(pIndexBuf,nEndPos - nStartPos,sizeof(int),compare);
		
		
		//归并
		int nSizeA = nEndPos - nStartPos;
		int nSizeB = pIn->GetRecordCount();
		if (nSizeB==0)
		{
			//pOut->SetSize(nSizeA);
			for (int i=0;i<nSizeA ;i++)
			{
				pOut->Append();//  输入  *(pBuf+i)
				LPBYTE lp = (LPBYTE)*(pIndexBuf+i);
				pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );
			}
		}
		else
		{
			//pOut->SetSize(nSizeA+nSizeB);//设置输出大小
			int nA = 0;
			int nB = 0;
			//pIn->SetCurRecNo(nB+1);
			pIn->First();
			LPBYTE lpb =(LPBYTE)pDataB;
			pIn->m_pPages->m_pFields->SaveDataToBuffer(lpb);

			//int nOut =0;
			while (nA<nSizeA || nB <nSizeB)
			{
				if (nA >=nSizeA )
				{
					pOut->Append();//输入  pIn
					LPBYTE lpb =(LPBYTE)pDataB;
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lpb  ,nTemp0 );

					nB++;
					pIn->Next();
					lpb =(LPBYTE)pDataB;
					pIn->m_pPages->m_pFields->SaveDataToBuffer(lpb);
					continue;
				}
				if (nB >=nSizeB)
				{
					pOut->Append();//输入  pBuf
					LPBYTE lp = (LPBYTE)*(pIndexBuf+nA);
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );
					nA++;
					continue;
				}
				
				//if (pIn->GetAt(nB).dValue > (pBuf+nA)->dValue)
				if (Comp(pDataB,(LPBYTE)*(pIndexBuf+nA)) >0 )
				{
					pOut->Append();//输入  pBuf
					LPBYTE lp = (LPBYTE)*(pIndexBuf+nA);
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );

					nA++;
				}
				else
				{
					pOut->Append();//输入  pIn
					LPBYTE lpb =(LPBYTE)pDataB;
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lpb  ,nTemp0 );
					nB++;
					pIn->Next();
					lpb =(LPBYTE)pDataB;
					pIn->m_pPages->m_pFields->SaveDataToBuffer(lpb);
				}
			}
			
		}
		
		CDataAccess*pTemp = pOut;
		pOut = pIn;
		pIn = pTemp;
		delete pOut;
		pOut = new CDataAccess(&ColAry);
		//***********************************
		nCurPos = nEndPos;//更新当前位置
	}
	
	
	//复制数据 //#_S 2005-5-17 zlq
	//交换
	if (ppOutDA ==NULL)
	{
		CPages*pTemp = ColAry.m_pPages;
		ColAry.m_pPages = pIn->m_pPages;
		pIn->m_pPages = pTemp;
		ColAry.m_pPages->BindFieldList(ColAry.m_pFieldList);
		pIn->m_pPages->BindFieldList(pIn->m_pFieldList);
		delete pIn;
		
	}
	else
	{
		*ppOutDA = pIn;
	}
	delete pOut;
	
	delete [] pIndexBuf;
	delete [] pData;
	delete [] pDataB;
}


/************************************************************************************
函数名称：

    CTarray<int,int> FieldIndexArray;  要排序的字段索引.(此函数字段序号从1开始)

              FieldIndexArray.Add(-2);//第二个字段降序

              FieldIndexArray.Add(1);//第一个字段升序

              FieldIndexArray.Add(3);//第三个字段升序


       现有数据集保留,新建的排好序的DataAccess 中增加了一个字段(序号0)保留记录原来的序号
	新增字段实际上不赋值,为了兼容用而保留
修改:

************************************************************************************/
bool CDataAccess::MergeSort(CDataAccess * & pDataAccess,CTArray<int,int> &FieldArray)
{
	{
		g_ItemAry.RemoveAll();
		for (int i=0;i<FieldArray.GetSize() ;i++)
		{
			int nIndex = FieldArray.GetAt(i);
			Item it;
			if (nIndex>0)
			{
				nIndex = nIndex-1;
				it.m_nAsc = 1;
			}
			else
			{
				nIndex = -nIndex-1;
				it.m_nAsc = -1;
			}
			
			int nOffset =0;
			for (int j=0;j<nIndex ;j++)
			{
				CField *pField = m_pFieldList->FieldByIndex(j);
				if (pField->GetFieldType()==fString)
					nOffset += Align4(pField->GetWidth());
				else
					nOffset += sizeof(double);
			}
			

			CField *pField = m_pFieldList->FieldByIndex(nIndex);
			if (pField->GetFieldType()==fString)
				it.m_bIsString = true;
			else
				it.m_bIsString = false;
			it.m_nOffset = nOffset;
			it.m_nLen = pField->GetWidth();
			g_ItemAry.Add(it);

		}
		mysort(*this,&pDataAccess);
		g_ItemAry.RemoveAll();
		///////////////////////////
		//增加原来的记录号作为一个字段 为了兼容用,没有赋值
		CField* pField = pDataAccess->m_pFieldList->CreateField(fInt);
		pDataAccess->m_pFieldList->Insert(pField,0);
		pField->SetFieldName("原记录号");
		return true;
	}

}
/************************************************************************************
函数名称：

功能说明：

************************************************************************************/
bool CDataAccess::MergeSort(CTArray<int, int> & FieldArray)
{
	//*
	{
		g_ItemAry.RemoveAll();
		for (int i=0;i<FieldArray.GetSize() ;i++)
		{
			int nIndex = FieldArray.GetAt(i);
			Item it;
			if (nIndex>0)
			{
				nIndex = nIndex-1;
				it.m_nAsc = 1;
			}
			else
			{
				nIndex = -nIndex-1;
				it.m_nAsc = -1;
			}
			
			int nOffset =0;
			for (int j=0;j<nIndex ;j++)
			{
				CField *pField = m_pFieldList->FieldByIndex(j);
				if (pField->GetFieldType()==fString)
					nOffset += Align4(pField->GetWidth());
				else
					nOffset += sizeof(double);
			}
			

			CField *pField = m_pFieldList->FieldByIndex(nIndex);
			if (pField->GetFieldType()==fString)
				it.m_bIsString = true;
			else
				it.m_bIsString = false;
			it.m_nOffset = nOffset;
			it.m_nLen = pField->GetWidth();
			g_ItemAry.Add(it);

		}
		mysort(*this,NULL);
		g_ItemAry.RemoveAll();
		return true;
	}
	//*/

}


bool CDataAccess::Statistic(int nIndex ,CStatistic& stat) 
{
	if(m_pPages->GetRecordCount()<1)
		return false;


	int i=m_pPages->GetRecordCount();
	int nOldRecNO = m_pPages->GetRecNo();
	m_pPages->First();
	while(m_pPages->m_pFields->FieldByIndex(nIndex)->IsNull())
	{	
		stat.dTotalCount++;
		m_pPages->Next();
	}
	stat.dMax = m_pPages->m_pFields->FieldByIndex(nIndex)->GetAsDouble();
	stat.dMin = stat.dMax;

	//Midian

	do
	{
		if (m_pPages->m_pFields->FieldByIndex(nIndex)->IsNull())
		{
		}
		else
		{
			double fValue = m_pPages->m_pFields->FieldByIndex(nIndex)->GetAsDouble();

			stat.dSum += fValue; //Sum
			stat.dValidCount ++; //ValidCount

			if(fValue>stat.dMax)
				stat.dMax = fValue;//Max
			if(fValue<stat.dMin)
				stat.dMin = fValue;	//Min

			//Midian==>
			//Midian<==
		}
		stat.dTotalCount ++; //TotalCount
		m_pPages->Next();
	}while(--i);

	stat.dMean = stat.dSum/stat.dValidCount; //Mean
	stat.dRange = stat.dMax - stat.dMin;		 //Range

	SetCurRecNo(nOldRecNO);
	return true;
}


/*
#define fDouble 'F'     //双精度浮点型数据
#define fInt 'N'       //整型数据
#define fString 'C'    //字符串型数据
#define fDate 'D'      //日期型数据
#define fCurrency 'M'  //货币型数据
#define fBoolean 'L'   //布尔型数据
#define fBlob 'B'      //型数据
*/
//从传入的CDataAccess实例,拷贝结构和数据到当前本实例.会清空原来数据.
bool CDataAccess::CopyDataFromOther(CDataAccess *pSourceDA)
{
	NewFile();
	CDataAccess *pDestDA = this;
	if (!pSourceDA->m_pFieldList->GetFieldCount())
		return false;
	int fieldCount = pSourceDA->m_pFieldList->GetFieldCount();
	for (int i = 0;i< fieldCount;i++)
	{//复制字段信息
		CField * pSourceField = pSourceDA->m_pFieldList->FieldByIndex(i);
		CField * pDestField = pSourceDA->m_pFieldList->CreateField(pSourceField);
		pDestField->Clone(pSourceField );
		pDestDA->m_pFieldList->Add(pDestField);	
	}

	int nOldRecNo = pSourceDA->GetRecNo();
	pSourceDA->First();
	while (!pSourceDA->Eof())
	{
		pDestDA->Append();
		for (int i = 0;i< pSourceDA->m_pFieldList->GetFieldCount();i++)
		{//复制字段信息
			CField * pSourceField = pSourceDA->m_pFieldList->FieldByIndex(i);
			CField * pDestField = pDestDA->m_pFieldList->FieldByIndex(i);
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

					//default:
						//assert(0);
				}
			}


		}		
		pDestDA->Post();
		pSourceDA->Next();

	}
	return true;
}

/************************************************************************************
函数名称：
功能说明：
        过滤条件
详细解释：
出入参数：
返回类型：
制作：
	//###_Mod 2002-10-29 zlq
修改: 
************************************************************************************/
typedef bool (*pFilter)(CDataAccess* pSourceDataAccess,CDataAccess* pDestDataAccess,LPCTSTR lpszFilter,CDealType DealType,CDealCount dcCount);
typedef double (*pGetFilterValue)( CDataAccess * pDataAccess );
typedef void   (*pRecycleMemory)();

bool CDataAccess::Filter(CTString strCondition, CDataAccess** pFilterData,CDealType DealType,CDealCount dcCount)
{
/*===uc  	HINSTANCE hwnd = LoadLibrary("Filter.dll");
	if (!hwnd)
	{
		//===temp AfxMessageBox("没有找到相应的Filter.dll装载失败!");
		return false;
	}

	pFilter pLf;
	pLf =(pFilter) GetProcAddress(hwnd,"Filter");
	if (!pLf)
	{
		//===temp AfxMessageBox("没有找到相应的Filter函数装载失败!");
		FreeLibrary(hwnd);
		return false;
	}
	CDataAccess * pDest =NULL;
	if (dcCount == dcAll)
	{
		pDest =new CDataAccess(this);
		if (DealType ==dtFilterd)
		{//是附加一个过滤字段
			CField*pField = pDest->m_pFieldList->CreateField(fDouble);
			pDest->m_pFieldList->Add(pField);
		
		}
		*pFilterData = pDest;
	}
	else
	{
		//===temp AfxMessageBox("没有用到本功能!");
	}
	
	LPCTSTR pCondition =(LPCTSTR) strCondition;
	bool result =  (*pLf)(this,pDest,pCondition,DealType,dcCount);
	if ((!result) && pDest)
		delete pDest;//#_修改 2002-11-14 $ 17:14:38 zlq
	FreeLibrary(hwnd);
	return result ;	   */
	return false;
}


/************************************************************************************
函数名称：
功能说明：
        设置内存中最大的页个数 不能小于2页
详细解释：
出入参数：
返回类型：
制作：
	//###_Mod 2002-10-29 zlq
修改: 
************************************************************************************/
bool CDataAccess::SetMaxPageCount(int nCount)
{
	return m_pPages->SetMaxPageCount(nCount);	
}

/************************************************************************************
函数名称：
功能说明：
        设置每个页的正常时的大小 一般50条
详细解释：
出入参数：
返回类型：
制作：
	//###_Mod 2002-10-29 zlq
修改: 
************************************************************************************/
bool CDataAccess::SetNormalCountPerPage(int nCount)
{
	return m_pPages->SetNormalCountPerPage(nCount);
}

/************************************************************************************
函数名称：
功能说明：
        得到页设置属性 包括 最大的页个数 每个页的正常时的大小 已经使用的页
详细解释：
出入参数：
返回类型：
制作：
	//###_Mod 2002-10-29 zlq
修改: 
************************************************************************************/
void CDataAccess::GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage)
{
	m_pPages->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
}



//取得数据类型（离散型/连续型）
int CDataAccess::GetDCDataType(int nIndex)
{

	CField* pField=m_pFieldList->FieldByIndex(nIndex);
	if(pField==NULL)
		return 0;
	
	int nMeasure=pField->GetMeasure(),
		nDataType=pField->GetFieldType();
	
	if(nDataType==fBoolean || nDataType==fString || nMeasure==msNominal || nMeasure==msOrdinal)
		return DISPERSE_DATA;        
	
	
	if(nMeasure==msScale )
		return CONTINU_DATA;
	
	
	return 0;
}

/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-15
修改:

************************************************************************************/
/*
bool CDataAccess::GetStatisticButMedian(int *pnFieldIndex,CStatistic * pStat,int nCount)
{
	int i;			    
	
	//检查字段越界
	for(i = 0;i<nCount;i++)
	{
		if( *(pnFieldIndex+i) < 0 || *(pnFieldIndex+i) >= m_pFieldList->GetFieldCount())
		{
			return false;
		}
	}

	//Field 数组
	CTArray<CField *,CField*> aryFields;	
	CByteArray byFieldMark;
	CField * pField	;
	for(i = 0;i<nCount;i++)
	{	  
		pField= m_pFieldList->FieldByIndex(*(pnFieldIndex+i));
		if( pField->GetFieldType() != fDouble && pField->GetFieldType() != fInt)
			return false;
		aryFields.Add(pField); 
		byFieldMark.Add(0);
	}

	//赋初值
	for(i = 0;i<nCount;i++)
	{
		(pStat+i)->dValidCount = 0;
		(pStat+i)->dTotalCount = 0;
		(pStat+i)->dSum = 0;
	}

	//开始统计
	First();  
	double dValue ;


	while(!Eof())
	{
		for(i=0;i<nCount;i++)
		{
			if(byFieldMark[i] == 0 && (aryFields[i])->IsNull())
			{//这个Field还没有值
				(pStat+i)->dTotalCount++;
			}
			else
			{  	
				//每个字段赋一次初值
				if(byFieldMark[i] != 1)
				{//此字段开始有值 
					dValue = (aryFields[i])->GetAsDouble();
					(pStat+ i)->dMin = dValue;			  //min
					(pStat+ i)->dMax	= dValue;			  //max
					byFieldMark[i] = 1;
				}

				//统计
				if(!((aryFields[i])->IsNull()))
				{	
					(pStat+ i)->dValidCount++;	   //valid count
					dValue = (aryFields[i])->GetAsDouble();
					(pStat+ i)->dSum += dValue;	   //sum
					if(dValue > (pStat+ i)->dMax)
					{
						(pStat+ i)->dMax = dValue;  //max
					}
					if(dValue < (pStat+ i)->dMin)
					{
						(pStat+ i)->dMin = dValue;  //min
					}
				}
			}
			(pStat+ i)->dTotalCount++;		   //totalcount
		}

		Next();
	}

	for(i=0;i<nCount;i++)
	{
		(pStat+ i)->dRange  = (pStat+ i)->dMax - (pStat+ i)->dMin; 
		if((pStat+ i)->dValidCount)
		{
			(pStat+ i)->dMean = (pStat+ i)->dSum /(pStat+ i)->dValidCount;  
		}
		else
		{
		}
	}
	return true;
}

*/


bool CDataAccess::Close()
{
	NewFile();
	m_bIsOpen = false;
	return true;
}

CTString CDataAccess::GetFileName()
{
	return m_strFileName;
}

bool CDataAccess::IsOpen()
{
	return m_bIsOpen;
}

bool CDataAccess::SetFileName(CTString strFileName)
{
	m_strFileName = strFileName;
	m_strFileName = TCSystem::ConvertPath(m_strFileName);
	return true;
}

//交换pDA和当前的数据//#_S 2003-4-17 $ 15 zlq  针对上层接口实例化而特殊设计的.
void CDataAccess::Swap(CDataAccess*pDA)
{
	BYTE* buf = new BYTE[sizeof(CDataAccess)];
	memcpy(buf,pDA,sizeof(CDataAccess));
	memcpy(pDA,this,sizeof(CDataAccess));
	memcpy(this,buf,sizeof(CDataAccess));
	delete []buf;
}

//#ifdef MarkWay_Demo  //是学员版  (记录条数限制)
	//如果发现前面跳过了,则在这里出错，调用内部函数DemoRecordCount*100-24
void CDataAccess::Flush(int nChekCount)
{
	void *pdata = m_pPages;
	if (m_pPages->GetRecordCount()*100-24 > nChekCount)
	{
		m_pPages =(CPages*) &m_pPages->m_nCurRecNo;
		pdata =   &m_pPages->m_nCurRecNo;
	}
	
	m_pPages = (CPages*)pdata;
}	

CField* CDataAccess::FieldByIndex(int index)
{
	return m_pFieldList->FieldByIndex(index);
}

CField* CDataAccess::FieldByName(CTString FieldName)
{
	return m_pFieldList->FieldByName(FieldName);
}

int CDataAccess::IndexByName(CTString sFieldName)
{
	return m_pFieldList->IndexOf(sFieldName);
}

int CDataAccess::IndexByField(CField* pField)
{
	return m_pFieldList->IndexOf(pField);
}

int CDataAccess::GetFieldCount()
{
	return m_pFieldList->GetFieldCount();
}

CTString CDataAccess::NameByIndex(int nIndex)
{
	CField *pField = m_pFieldList->FieldByIndex(nIndex);
	if (pField)
		return pField->GetFieldName();
	return "";
}

bool CDataAccess::AddField(std::string sName, CFieldType fType, int nWidth)
{
	CField *pField = m_pFieldList->CreateField(fType);
	pField->SetWidth( nWidth);
	m_pFieldList->Add(pField);
	return pField->SetFieldName(sName);
}





















