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
		str.Format("����ʱ��ǰ:%d,:%d ��ʱ��:%d",m_mid - m_start,m_end - m_mid,m_end - m_start);
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
//�����忽��һ�����ݿ��ֶνṹ����û�м�¼��Ϣ
CDataAccess::CDataAccess(CDataAccess*pTemplet)
{
	m_pBuf				= new BYTE;
	m_nBufLen			= 1;
	m_pPages			= new CPages();
	m_pFieldList		= new CFieldList(m_pPages);
	NewFile();
	CDataAccess *pDestDA = this;

	for (int i = 0;i< pTemplet->m_pFieldList->GetFieldCount();i++)
	{//�����ֶ���Ϣ
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
			//===temp AfxMessageBox("û���ҵ���Ӧ��xlsPumper.dll,װ��ʧ��!");
			m_bIsOpen =false;
			return false;
		}
		NewFile();
		//m_strFileName = lpszFileName;
		pLoadFile pLf;
		pLf =(pLoadFile) GetProcAddress(hDll,"LoadFile");
		m_bIsOpen = false;
		if(pLf == NULL)
			//===temp AfxMessageBox("û���ҵ���Ӧ��LoadFile,װ��ʧ��!");
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
	else if(str==_T("dbf"))// foxpro����dBase [4/21/2003]swx
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
			//===temp AfxMessageBox("ȱ��ImportDbf.dll! �ļ����ܴ�");
			return false;
		}   */
	}	
	else //if (str == "txt")
    {
/*  		int nMaxPageCount,nPageNormalSize,nUsePage;
		this->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
		this->SetMaxPageCount(5);//�趨ҳ���С
		HINSTANCE hwnd = AfxLoadLibrary("TLTextCV.dll");
		if (!hwnd)
		{
			//===temp AfxMessageBox("û���ҵ���Ӧ��TLTextCV.dllװ��ʧ��!");
			m_bIsOpen = false;
			this->SetMaxPageCount(nMaxPageCount);//�ָ�ҳ�����趨
			return false;
		}
		NewFile();

		pLoadFile pLf;
		pLf =(pLoadFile) GetProcAddress(hwnd,"LoadFile");
		m_bIsOpen =   (*pLf)(this,lpszFileName);
		AfxFreeLibrary(hwnd);
		this->SetMaxPageCount(nMaxPageCount);//�ָ�ҳ�����趨
		rst =  m_bIsOpen ;   */
	}
	//*************ҳ������**********************
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

//�Ծɰ汾�ļ����� nVersion �汾��
bool CDataAccess::SaveOldVersionFile(LPCTSTR lpszFileName,CTString *pstrOutFile,int nVersion)
{
	m_pPages->SetSaveFileVersion(nVersion);
	bool bRst = SaveFile(lpszFileName,pstrOutFile);
	m_pPages->SetSaveFileVersion(0);//�ָ�Ϊ����
	return bRst;
}
//pstrOutFile ����false ʱ�ұ���Ϊ�����ļ�ʱ��Ч, Ϊ����ʱ����ʱ�����Ϊ���ļ�����,���ڲ�����
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
			//===temp AfxMessageBox("û���ҵ���Ӧ��xlsPumper.dll,װ��ʧ��!");
			return false;
		}
		pPumpToExcel pSave;
		pSave =(pPumpToExcel) GetProcAddress(hDll,"PumpToExcel");
		if(pSave == NULL)
			//===temp AfxMessageBox("û���ҵ���Ӧ��PumpToExcel,װ��ʧ��!");
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
		{//�ϰ汾�ļ�
/* 			typedef bool (*pPumpToOld)(CDataAccess* pCDataAccess,LPCTSTR lpszFileName,CTString *strOut);
 			HINSTANCE hDll = AfxLoadLibrary("OdData.dll");
			if (!hDll)
			{
				//===temp AfxMessageBox("û���ҵ���Ӧ��OdData.dll,װ��ʧ��!");
				return false;
			}
			bool rst = false;
			pPumpToOld pSave;
			pSave =(pPumpToOld) GetProcAddress(hDll,"SaveOldData");
			if(pSave == NULL)
				//===temp AfxMessageBox("û���ҵ���Ӧ��SaveOldData,װ��ʧ��!");
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
		{//������ֶ��������������
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
	{//#_�޸� 2002-11-26 $ 8:30:46 zlq
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
�������ƣ�

����˵����
		��ֵ��¼�����ݼ��Ѿ����ڿ�д״̬
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-10-23
�޸�:

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

	int nBufCount = 10000*10;//===temp  ��̬��С
	int nTemp =0;
	if (TCSystem::GetProfileIntValue("SortSetup","SortMemNum(MB)",nTemp ))
	{
		nTemp = nTemp *1024*1024 /nRecordLen;
		if (nTemp> 500)//ȷ������500����¼
			nBufCount = nTemp;
	}
	if (nTotalCount < nBufCount)//��ֹ�ڴ��趨����,��󲻳����ܼ�¼����
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
			*(pIndexBuf+i-nStartPos) = (int)pTemp;//�����ַ
		}
		
		qsort(pIndexBuf,nEndPos - nStartPos,sizeof(int),compare);
		
		
		//�鲢
		int nSizeA = nEndPos - nStartPos;
		int nSizeB = pIn->GetRecordCount();
		if (nSizeB==0)
		{
			//pOut->SetSize(nSizeA);
			for (int i=0;i<nSizeA ;i++)
			{
				pOut->Append();//  ����  *(pBuf+i)
				LPBYTE lp = (LPBYTE)*(pIndexBuf+i);
				pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );
			}
		}
		else
		{
			//pOut->SetSize(nSizeA+nSizeB);//���������С
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
					pOut->Append();//����  pIn
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
					pOut->Append();//����  pBuf
					LPBYTE lp = (LPBYTE)*(pIndexBuf+nA);
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );
					nA++;
					continue;
				}
				
				//if (pIn->GetAt(nB).dValue > (pBuf+nA)->dValue)
				if (Comp(pDataB,(LPBYTE)*(pIndexBuf+nA)) >0 )
				{
					pOut->Append();//����  pBuf
					LPBYTE lp = (LPBYTE)*(pIndexBuf+nA);
					pOut->m_pPages->m_pFields->LoadDataFromBuffer(lp  ,nTemp0 );

					nA++;
				}
				else
				{
					pOut->Append();//����  pIn
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
		nCurPos = nEndPos;//���µ�ǰλ��
	}
	
	
	//�������� //#_S 2005-5-17 zlq
	//����
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
�������ƣ�

    CTarray<int,int> FieldIndexArray;  Ҫ������ֶ�����.(�˺����ֶ���Ŵ�1��ʼ)

              FieldIndexArray.Add(-2);//�ڶ����ֶν���

              FieldIndexArray.Add(1);//��һ���ֶ�����

              FieldIndexArray.Add(3);//�������ֶ�����


       �������ݼ�����,�½����ź����DataAccess ��������һ���ֶ�(���0)������¼ԭ�������
	�����ֶ�ʵ���ϲ���ֵ,Ϊ�˼����ö�����
�޸�:

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
		//����ԭ���ļ�¼����Ϊһ���ֶ� Ϊ�˼�����,û�и�ֵ
		CField* pField = pDataAccess->m_pFieldList->CreateField(fInt);
		pDataAccess->m_pFieldList->Insert(pField,0);
		pField->SetFieldName("ԭ��¼��");
		return true;
	}

}
/************************************************************************************
�������ƣ�

����˵����

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
#define fDouble 'F'     //˫���ȸ���������
#define fInt 'N'       //��������
#define fString 'C'    //�ַ���������
#define fDate 'D'      //����������
#define fCurrency 'M'  //����������
#define fBoolean 'L'   //����������
#define fBlob 'B'      //������
*/
//�Ӵ����CDataAccessʵ��,�����ṹ�����ݵ���ǰ��ʵ��.�����ԭ������.
bool CDataAccess::CopyDataFromOther(CDataAccess *pSourceDA)
{
	NewFile();
	CDataAccess *pDestDA = this;
	if (!pSourceDA->m_pFieldList->GetFieldCount())
		return false;
	int fieldCount = pSourceDA->m_pFieldList->GetFieldCount();
	for (int i = 0;i< fieldCount;i++)
	{//�����ֶ���Ϣ
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
		{//�����ֶ���Ϣ
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
�������ƣ�
����˵����
        ��������
��ϸ���ͣ�
���������
�������ͣ�
������
	//###_Mod 2002-10-29 zlq
�޸�: 
************************************************************************************/
typedef bool (*pFilter)(CDataAccess* pSourceDataAccess,CDataAccess* pDestDataAccess,LPCTSTR lpszFilter,CDealType DealType,CDealCount dcCount);
typedef double (*pGetFilterValue)( CDataAccess * pDataAccess );
typedef void   (*pRecycleMemory)();

bool CDataAccess::Filter(CTString strCondition, CDataAccess** pFilterData,CDealType DealType,CDealCount dcCount)
{
/*===uc  	HINSTANCE hwnd = LoadLibrary("Filter.dll");
	if (!hwnd)
	{
		//===temp AfxMessageBox("û���ҵ���Ӧ��Filter.dllװ��ʧ��!");
		return false;
	}

	pFilter pLf;
	pLf =(pFilter) GetProcAddress(hwnd,"Filter");
	if (!pLf)
	{
		//===temp AfxMessageBox("û���ҵ���Ӧ��Filter����װ��ʧ��!");
		FreeLibrary(hwnd);
		return false;
	}
	CDataAccess * pDest =NULL;
	if (dcCount == dcAll)
	{
		pDest =new CDataAccess(this);
		if (DealType ==dtFilterd)
		{//�Ǹ���һ�������ֶ�
			CField*pField = pDest->m_pFieldList->CreateField(fDouble);
			pDest->m_pFieldList->Add(pField);
		
		}
		*pFilterData = pDest;
	}
	else
	{
		//===temp AfxMessageBox("û���õ�������!");
	}
	
	LPCTSTR pCondition =(LPCTSTR) strCondition;
	bool result =  (*pLf)(this,pDest,pCondition,DealType,dcCount);
	if ((!result) && pDest)
		delete pDest;//#_�޸� 2002-11-14 $ 17:14:38 zlq
	FreeLibrary(hwnd);
	return result ;	   */
	return false;
}


/************************************************************************************
�������ƣ�
����˵����
        �����ڴ�������ҳ���� ����С��2ҳ
��ϸ���ͣ�
���������
�������ͣ�
������
	//###_Mod 2002-10-29 zlq
�޸�: 
************************************************************************************/
bool CDataAccess::SetMaxPageCount(int nCount)
{
	return m_pPages->SetMaxPageCount(nCount);	
}

/************************************************************************************
�������ƣ�
����˵����
        ����ÿ��ҳ������ʱ�Ĵ�С һ��50��
��ϸ���ͣ�
���������
�������ͣ�
������
	//###_Mod 2002-10-29 zlq
�޸�: 
************************************************************************************/
bool CDataAccess::SetNormalCountPerPage(int nCount)
{
	return m_pPages->SetNormalCountPerPage(nCount);
}

/************************************************************************************
�������ƣ�
����˵����
        �õ�ҳ�������� ���� ����ҳ���� ÿ��ҳ������ʱ�Ĵ�С �Ѿ�ʹ�õ�ҳ
��ϸ���ͣ�
���������
�������ͣ�
������
	//###_Mod 2002-10-29 zlq
�޸�: 
************************************************************************************/
void CDataAccess::GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage)
{
	m_pPages->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
}



//ȡ���������ͣ���ɢ��/�����ͣ�
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
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-15
�޸�:

************************************************************************************/
/*
bool CDataAccess::GetStatisticButMedian(int *pnFieldIndex,CStatistic * pStat,int nCount)
{
	int i;			    
	
	//����ֶ�Խ��
	for(i = 0;i<nCount;i++)
	{
		if( *(pnFieldIndex+i) < 0 || *(pnFieldIndex+i) >= m_pFieldList->GetFieldCount())
		{
			return false;
		}
	}

	//Field ����
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

	//����ֵ
	for(i = 0;i<nCount;i++)
	{
		(pStat+i)->dValidCount = 0;
		(pStat+i)->dTotalCount = 0;
		(pStat+i)->dSum = 0;
	}

	//��ʼͳ��
	First();  
	double dValue ;


	while(!Eof())
	{
		for(i=0;i<nCount;i++)
		{
			if(byFieldMark[i] == 0 && (aryFields[i])->IsNull())
			{//���Field��û��ֵ
				(pStat+i)->dTotalCount++;
			}
			else
			{  	
				//ÿ���ֶθ�һ�γ�ֵ
				if(byFieldMark[i] != 1)
				{//���ֶο�ʼ��ֵ 
					dValue = (aryFields[i])->GetAsDouble();
					(pStat+ i)->dMin = dValue;			  //min
					(pStat+ i)->dMax	= dValue;			  //max
					byFieldMark[i] = 1;
				}

				//ͳ��
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

//����pDA�͵�ǰ������//#_S 2003-4-17 $ 15 zlq  ����ϲ�ӿ�ʵ������������Ƶ�.
void CDataAccess::Swap(CDataAccess*pDA)
{
	BYTE* buf = new BYTE[sizeof(CDataAccess)];
	memcpy(buf,pDA,sizeof(CDataAccess));
	memcpy(pDA,this,sizeof(CDataAccess));
	memcpy(this,buf,sizeof(CDataAccess));
	delete []buf;
}

//#ifdef MarkWay_Demo  //��ѧԱ��  (��¼��������)
	//�������ǰ��������,����������������ڲ�����DemoRecordCount*100-24
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





















