// Manager.cpp: implementation of the TCManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Manager.h"

#include "../Public/common.h"
#include "../Public/Language.h"
#include "../Public/TString.h"
//#include  "../TLDBDll/Database/dataaccess.h"
//#include "../TLDBDll/Database/fieldlist.h"
//#include "../TLDBDll/Database/CField.h"
#include "../DataInterface/Define.h" 
#include "../DataInterface/TCDataArray.h"
#include "../DataInterface/DataInterface.h"
#include "../Result/Result.h"

//#include "../Public/Util.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../arithInterface/EnvParam.h"
#include "TCurSys.h"
#ifndef _UNIX
#include <stdlib.h>
#include <io.h>
#endif
#ifdef _UNIX 
#include "../Public/Util.h"
#include <dlfcn.h>
HINSTANCE LoadLibrary(const char * name){
	return dlopen(name,RTLD_LAZY);
}

void * GetProcAddress(  HINSTANCE hm,const char * name){
	return dlsym(hm,name);
}

void FreeLibrary(HINSTANCE hm){
	dlclose(hm);
}

#endif

//#include "../data/table.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCManager::TCManager()
{

}

TCManager::~TCManager()
{

}
#define Expire 20070700  
#define GTime(Time)  (Time.wYear *10000 + Time.wMonth *100+ Time.wDay)
/*

*/


TCurSys g_system;

#ifdef _UNIX
bool TCManager::DealPacket(TCPacket &inP,TCPacket &outP,bool &forked)
#else
bool TCManager::DealPacket(TCPacket &inP, TCPacket &outP)
#endif
{
#ifdef WINPLAT
	SYSTEMTIME st;
	GetLocalTime(&st);
	
	if ( GTime(st) >= Expire )
	{
		CTString spath;
		TCSystem::GetTempPath(spath);
		spath = spath + "Sys.dat";
		TCLFile tf(spath.GetData(),TCLFile::modeReadWrite|TCLFile::modeCreate);
		tf.Write(spath.GetData(),spath.GetLength());
	}
#else

#endif

		

	outP.SetPacketName("Results");
	string sPackName = inP.GetPacketName();
	if (sPackName == "Command")
	{//1 
		char *pChar = inP.GetItemData("Text");
		int nLen = inP.GetItemDataLen("Text");
		if (nLen>0)
		{//1.1
			//inP.SetPacketName("Command");
			string strName = inP.GetItemDataAsString("Text");
			AnaWord aw;
			aw.Import(strName);
#ifdef WINPLAT
	if ( GTime(st) >= Expire )
		return SetResultState(false,outP,GetLanguage(TimeHasExpired)); 			
#else

#endif
			if (aw.GetAt(0) == "Login" )//��ȡ����Ϣ
			{
				Login(inP,outP);
			}
			else if (aw.GetAt(0)=="Logout")
			{
				Logout(inP,outP);
			}
			else if (aw.GetAt(0)=="ClearCurData")
			{
				ClearCurData(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="CreateTable")
			{
				CreateTable(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="OpenTable")
			{ 
				OpenTable(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="CloseTable")
			{ 
				CloseTable(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="InsertCol")
			{ 
				InsertCol(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="EditCol")
			{ 
				EditCol(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="DeleteCol")
			{ 
				DeleteCol(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="Filter")
			{ 
				Filter(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="SetWeight")
			{ 
				SetWeight(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="Compute")
			{ 
				Compute(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="Sort")
			{ 
				Sort(inP,aw,outP);
			}
			else if (aw.GetAt(0) == "SAMPLE")
			{
				Sample(inP,aw,strName,outP);
			} 
			else if (aw.GetAt(0) == "RANDOM")
			{
				Random(inP,aw,strName,outP);
			}
			else if (aw.GetAt(0) == "FILLMISSING")
			{
				FillMissing(inP,aw,strName,outP);
			}
			/*
			else if (aw.GetAt(0)=="XXXXX")
			{
				XXXXX(aw,outP);
			}
			else if (aw.GetAt(0)=="XXXXX")
			{
				XXXXX(aw,outP);
			}
			*/

			else if (aw.GetAt(0)=="GetDataInfo")
			{
				GetDataInfo(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="GetDataRowText")
			{
				GetDataRowText(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="AddRowText")
			{
				AddRowText(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="EditRowText")
			{
				EditRowText(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="InsertRowText")
			{
				InsertRowText(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="DeleteRowText")
			{
				DeleteRowText(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="SaveFile")
			{
				SaveFile(inP,aw,outP);
			}
			else if (aw.GetAt(0)=="SetIniPath")
			{
				TCSystem::SetIniPath(aw.GetAt(1));
				return SetResultState(true,outP);
			}
			else if (aw.GetAt(0)=="SetTempPath")
			{
				TCSystem::SetTempPath(aw.GetAt(1));
				return SetResultState(true,outP);
			}
			else if (aw.GetAt(0) == "PasteCol")
			{
				PasteCol(inP,aw,outP);
			}
			else //if (aw.GetAt(0) == "XXXX" )
			{//1.1.3
				#ifdef _UNIX && _FORK_ALGO
					printf("fork child process for algorithmic\n");				
					pid_t pid = fork();
					if( pid ){ //parent process
						forked = true;
					}else{
						algorithmic(inP,strName,aw,outP);
						forked = false;
					}
				#else
					algorithmic(inP,strName,aw,outP);
				#endif
			}
		}
	}

	return true;
}


/*
#��ȡ�ֶκͼ�¼��Ϣ �ļ���û�У��򷵻ص�ǰ����Ϣ
>Command
	Text
		GetDataInfo ����
<Command
	Text
		ColInfo	����Ϣ
		Fail ������Ϣ
 */
bool TCManager::GetDataInfo(TCPacket &inP,AnaWord &aw,TCPacket &outP)
{
	if (aw.GetWordCount()!=2)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
	{
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	}

	CDataAccess &da = pDI->m_DataAccess;

	
	string strScript ;
	int nCnt = da.GetRecordCount();
	char  buffer[50];
	memset(buffer,0,50);
	sprintf(buffer,"%d",nCnt);
	string sRecordCount = buffer;
	for (int i=0;i<da.GetFieldCount();i++)
	{
		CField *pField = da.FieldByIndex(i);
		string ss = (const char*)pField->GetFieldName();				
		ss = AnaWord::GetUniquely(ss);
		CFieldType ft = pField->GetFieldType();
		if (ft ==fString )
			ss = ss + " s ";
		else if (ft == fDate)
			ss = ss + " d ";
		else if (ft == fInt)
			ss = ss + " n ";
		else if (ft == fDouble)
			ss = ss + " f ";
		else if (ft == fBoolean)
			ss = ss + " b ";
		
		char buf[10];
		sprintf(buf," %d ",pField->GetWidth());
		string sWidth = buf;
		strScript = strScript+ " "+ ss +sWidth;
	}
	sRecordCount = "ColInfo "+sRecordCount + strScript;
	outP.AddItem("Text",sRecordCount);
	return SetResultState(true,outP);
}

/*
 *	���ļ���ʽ�������
>Command
	Text
		GetDataRowText ���� (rowstart,rowend)
	
<Command
	Text
		TableText ����

 */
bool TCManager::GetDataRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount()!=7)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sRowStart = aw.GetAt(3);
	string sRowEnd = aw.GetAt(5);
	int nRowStart = atoi(sRowStart.c_str());
	int nEndRow = atoi(sRowEnd.c_str());
	
	pDI->m_DataAccess.SetCurRecNo(nRowStart+1);
	string strOut = "TableText";
	for (int i=0;i<nEndRow-nRowStart+1 ;i++)
	{
		for (int j=0;j<pDI->m_DataAccess.GetFieldCount();j++)
		{
			CTString str = pDI->m_DataAccess.FieldByIndex(j)->GetAsString();
			string ss =(char*) str;
			if (ss.size()==0)
				ss = ".";
			strOut = strOut + " " +ss ;
			
		}
		
		pDI->m_DataAccess.Next();
	}
	outP.AddItem("Text",strOut);
	return SetResultState(true,outP);
}
/*
	InsertCol ���� �����к� {�ֶ���A ���� ����}
	�к��Ǵ�0��ʼ����1��ʾ�Ӻ���׷�ӣ������ͱ�ʾ��s �ַ��� d �������� n ��������
 */
bool TCManager::InsertCol(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount()%3 !=0)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sColStart = aw.GetAt(2);
	int nColStart = atoi(sColStart.c_str());
	if (nColStart==-1)
		nColStart = pDI->m_DataAccess.GetFieldCount();
	for (int i=1;i<	aw.GetWordCount()/3 ;i++)
	{
		string sName =aw.GetAt(i*3+0);
		string sType =aw.GetAt(i*3+1);
		string sLength =aw.GetAt(i*3+2);
		CFieldType ft = fDouble;
		if (sType == "s" || sType == "S")
			ft = fString;
		else if (sType == "d" || sType == "D")
			ft = fDate;
		else if (sType == "n" || sType == "N")
			ft = fInt;
		else if (sType == "f" || sType == "F")
			ft = fDouble;
		else if (sType == "b" || sType == "B")
			ft = fBoolean;
		
		CField *pField = pDI->m_DataAccess.m_pFieldList->CreateField(ft);
		pField->SetWidth(atoi(sLength.c_str()));
		pDI->m_DataAccess.m_pFieldList->Insert(pField,nColStart++);
		if (pField->SetFieldName(sName)==false)
			return SetResultState(false,outP,GetLanguage(SetFieldNameFail));

	}


	return SetResultState(true,outP);
}
/*
	EditCol ���� �����к� {�ֶ���A ���� ����}
	�к��Ǵ�0��ʼ����1��ʾ�Ӻ���׷�ӣ������ͱ�ʾ��s �ַ��� d �������� n ��������
 */
bool TCManager::EditCol(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount()%3 !=0)//��֤��3��������
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sColStart = aw.GetAt(2);
	int nColStart = atoi(sColStart.c_str());
	if (nColStart<0 || nColStart+aw.GetWordCount()/3-1 > pDI->GetColCount())
		return SetResultState(false,outP,GetLanguage(ColIndexOutOfRangeFail)); 

	for (int i=1;i<	aw.GetWordCount()/3 ;i++)
	{
		string sName =aw.GetAt(i*3+0);
		string sType =aw.GetAt(i*3+1);
		string sLength =aw.GetAt(i*3+2);
		CFieldType ft = fDouble;
		if (sType == "s" || sType == "S")
			ft = fString;
		else if (sType == "d" || sType == "D")
			ft = fDate;
		else if (sType == "n" || sType == "N")
			ft = fInt;
		else if (sType == "f" || sType == "F")
			ft = fDouble;
		else if (sType == "b" || sType == "B")
			ft = fBoolean;

		CField *pField = pDI->m_DataAccess.m_pFieldList->CreateField(ft);
		pField->SetWidth(atoi(sLength.c_str()));
		//pDI->m_DataAccess.m_pFieldList->Insert(pField,nColStart++);
		pDI->m_DataAccess.m_pFieldList->Modify(nColStart++,pField);
		if (pField->SetFieldName(sName)==false)
			return SetResultState(false,outP,GetLanguage(SetFieldNameFail));
	}


	return SetResultState(true,outP);
}

/*
	SetWeight ���� �ֶ��� 
 */
bool TCManager::SetWeight(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount() !=3)//��֤��3��������
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sField = aw.GetAt(2);
	int nIndex = pDI->m_DataAccess.IndexByName(sField);
	if (nIndex == -1)
		return SetResultState(false,outP,GetLanguage(FieldNameNotFind));

	pDI->SetWeightNumb(nIndex);
	return SetResultState(true,outP);

}

/*
	DeleteCol���� �����к� ����
	�к��Ǵ�0��ʼ����1��ʾ�Ӻ���׷�ӣ�
 */
bool TCManager::DeleteCol(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount() !=4)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sColStart = aw.GetAt(2);
	int nColStart = atoi(sColStart.c_str());
	string sColNum = aw.GetAt(3);
	int nColNum = atoi(sColNum.c_str());
	if (nColStart<0 || nColStart+nColNum-1 > pDI->GetColCount())
		return SetResultState(false,outP,GetLanguage(ColIndexOutOfRangeFail)); 

	while (nColNum)
	{
		pDI->m_DataAccess.m_pFieldList->Delete(nColStart);
		nColNum--;
	}


	return SetResultState(true,outP);
}


/*
	PasteCol ���� Դ������ Ŀ��������
 */
bool TCManager::PasteCol(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	if (aw.GetWordCount() !=4)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	
	string sTableName = aw.GetAt(1);
	string sSrcName  = aw.GetAt(2);
	string sDstName  = aw.GetAt(3);
	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI == NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	CField *pSrcField = pDI->FieldByName(sSrcName.c_str());
	if (pSrcField == NULL)
		return SetResultState(false,outP,GetLanguage(FieldNameNotFind)); 
	CFieldType ft = pSrcField->GetFieldType();
	CField *pDstField = pDI->FieldByName(sDstName.c_str());
	if (pDstField == NULL)
	{
		pDstField = pDI->m_DataAccess.m_pFieldList->CreateField(ft);
		pDstField->SetFieldName(sDstName);
		pDstField->SetWidth(pSrcField->GetWidth());
		pDI->m_DataAccess.m_pFieldList->Add(pDstField);
	}
	pDI->m_DataAccess.First();
	while (!pDI->m_DataAccess.Eof())
	{
		pDI->m_DataAccess.Edit();
		if (pSrcField->IsNull())
		{
			pDstField->SetNull();
		}
		else
		{
			if (ft == fDouble)
				pDstField->SetAsDouble(pSrcField->GetAsDouble());
			else if (ft == fInt)
				pDstField->SetAsInteger(pSrcField->GetAsInteger());
			else if (ft == fBoolean)
				pDstField->SetAsBool(pSrcField->GetAsBool());
			else if (ft == fString)
				pDstField->SetAsString(pSrcField->GetAsString());
			else
				pDstField->SetAsDateTime(&pSrcField->GetAsDateTime());
		}
		pDI->m_DataAccess.Next();
	}
	return SetResultState(true,outP);
}

/*
���ݹ���
Filter /DATA =���� /EXPR \"�������ʽ\"
 */
bool TCManager::Filter(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	int nTableIndex = -1;
	int nConditionIndex = -1;
	for (int i=2;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i-2)=="/DATA" && aw.GetAt(i-1)=="=")
		{
			nTableIndex = i;
		}
		if (aw.GetAt(i-1)=="/EXPR")
		{
			nConditionIndex = i;
		}
	}

	if (nTableIndex == -1 || nConditionIndex == -1)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 
	
	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(nTableIndex));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string sCondition = aw.GetAt(nConditionIndex);

#ifdef WINPLAT
	CTString szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	szPath += "/DataDeal.dll";	
	HINSTANCE hmod = LoadLibrary(szPath.GetData());
	if (hmod==NULL)
	{//û���ҵ���Ӧ��DLL
		return SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
	}
	
	typedef int (*pFilter)(CDataInterface *pData, LPCSTR  sCondition);
	pFilter proc;
	proc = (pFilter)GetProcAddress(hmod,"Filter");
	if (proc==NULL)
	{//"û���ҵ���Ӧ�Ĺ�����"
		return SetResultState(false,outP,GetLanguage(ProcNameNotFind));
	}

	int rst = (*proc)(pDI,sCondition.c_str());
	if (rst == 0)
		return SetResultState(false,outP,"");
	else
		return SetResultState(true,outP);
			
#else

#endif

	return SetResultState(true,outP);
}

/*
���ݼ���
Compute /DATA =����/DESTNAMEĿ���ֶ�/EXPR  \"������ʽ\" /CONDITION  \"�������ʽ\"
 */
bool TCManager::Compute(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	
	int nTableIndex = -1;
	int nConditionIndex = -1;
	int nExprIndex = -1;
	int nTargetIndex = -1;
	for (int i=2;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i-2)=="/DATA" && aw.GetAt(i-1)=="=")
		{
			nTableIndex = i;
		}
		if (aw.GetAt(i-1)=="/CONDITION")
		{
			nConditionIndex = i;
		}
		if (aw.GetAt(i-1)=="/DESTNAME")
		{
			nTargetIndex = i;
		}
		if (aw.GetAt(i-1)=="/EXPR")
		{
			nExprIndex = i;
		}
	}
 
	if (nTableIndex == -1 || nTargetIndex == -1 || nExprIndex == -1)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 
	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(nTableIndex));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	string sTarget = aw.GetAt(nTargetIndex);
	string sCalc = aw.GetAt(nExprIndex);
	string sCondition ;
	if (nConditionIndex!=-1)
		sCondition = aw.GetAt(nConditionIndex);

	LPCSTR lpszCon = NULL;
	if (sCondition !="")
		lpszCon = sCondition.c_str();
#ifdef WINPLAT
	CTString szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	szPath += "/DataDeal.dll";	
	HINSTANCE hmod = LoadLibrary(szPath.GetData());
	if (hmod==NULL)
	{//û���ҵ���Ӧ��DLL
		SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
		return false;
	}
	
	typedef int (*pCompute)(CDataInterface *pData, LPCSTR sExpr,LPCSTR sCondition,LPCSTR sTargetFieldName);
	pCompute proc;
	proc = (pCompute)GetProcAddress(hmod,"Compute");
	if (proc==NULL)
	{//"û���ҵ���Ӧ�Ĺ�����"
		SetResultState(false,outP,GetLanguage(ProcNameNotFind));
		return false;
	}
	int rst = (*proc)(pDI,sCalc.c_str(),lpszCon,sTarget.c_str());
	if (rst == 0)
		return SetResultState(false,outP,"");
	else
		return SetResultState(true,outP);
			
#else

#endif


	return SetResultState(true,outP);
}

/*

��������
Sort  /DATA =���� /EXPR \"[�ֶ���1] ASC [�ֶ���2] DESC ...\"
 */
bool TCManager::Sort(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	
	int nTableIndex = -1;
	int nExprIndex = -1;

	int i=0;
	for (i=2;i<aw.GetWordCount();i++)
	{
		if (aw.GetAt(i-2)=="/DATA" && aw.GetAt(i-1)=="=")
			nTableIndex = i;
		if (aw.GetAt(i-1)=="/EXPR")
			nExprIndex = i;
	}
 
	if (nTableIndex == -1 ||  nExprIndex == -1)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 
	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(nTableIndex));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	

	string strExpr = aw.GetAt(nExprIndex);
	AnaWord aw2;
	aw2.Import(strExpr);
	if (aw2.GetWordCount()%2 != 0)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	CTArray<int,int> sortAry;
	for (i=0;i<aw2.GetWordCount()/4;i++)
	{
		string strLeft  = aw2.GetAt(i*4);
		string strRight = aw2.GetAt(i*4+2);
		if (strLeft != "[" || strRight != "]")
			return SetResultState(false,outP,GetLanguage(FormatIsError));
		string strField = aw2.GetAt(i*4+1);
		int nIndex = pDI->IndexByName(strField.c_str());
		if (nIndex ==-1)
			return SetResultState(false,outP,GetLanguage(FormatIsError));
		string strDirection = aw2.GetAt(i*4 + 3);
		nIndex++;
		if (strDirection == "DESC")
			nIndex = -nIndex;
		else if (strDirection != "ASC")
			return SetResultState(false,outP,GetLanguage(FormatIsError)); 
		sortAry.Add(nIndex);
	}

	pDI->m_DataAccess.MergeSort(sortAry);

	return SetResultState(true,outP);
}
//ȱʧֵ���
bool TCManager::FillMissing(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	string szTableName = "";
	int nCount = aw.GetWordCount();
	for (int i=0;i<nCount;i++)
	{
		if (aw.GetAt(i) == "/DATA" &&  i< nCount-2 )
		{
			szTableName = aw.GetAt(i+2);
			break;
		}
	}
	if (szTableName.size()==0)
		return SetResultState(false,outP,GetLanguage(FormatIsError));
	//�ӿ�ָ��
	CDataInterface *pDI = g_system.GetCurData(nUserID,szTableName);
	if (pDI ==NULL)
	{
		if (g_system.OpenTable(nUserID,szTableName,szTableName)==false)
			return SetResultState(false,outP,GetLanguage(OperationFail)); 
		
		pDI = g_system.GetCurData(nUserID,szTableName);
		if (pDI ==NULL)
			return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	}
#ifdef WINPLAT
	CTString szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	szPath += "/DataDeal.dll";	
	HINSTANCE hmod = LoadLibrary(szPath.GetData());
	if (hmod==NULL)
	{//û���ҵ���Ӧ��DLL
		SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
		return false;
	}
	
	typedef int (*pFillMissing)(CDataInterface *pData, LPCTSTR lpszVO);
	pFillMissing proc;
	proc = (pFillMissing)GetProcAddress(hmod,"FillMissing");
	if (proc==NULL)
	{//"û���ҵ���Ӧ�Ĺ�����"
		SetResultState(false,outP,GetLanguage(ProcNameNotFind));
		return false;
	}
	
	int rst = (*proc)(pDI,szVO.c_str());
	if (rst == 0)
		return SetResultState(false,outP,"");
	else
		return SetResultState(true,outP);
	
#else
	
#endif
	return SetResultState(true,outP);
}

//���ݳ���
bool TCManager::Sample(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	string szTableName = "";
	int nCount = aw.GetWordCount();
	for (int i=0;i<nCount;i++)
	{
		if (aw.GetAt(i) == "/DATA" &&  i< nCount-2 )
		{
			szTableName = aw.GetAt(i+2);
			break;
		}
	}
	if (szTableName.size()==0)
		return SetResultState(false,outP,GetLanguage(FormatIsError));
	//�ӿ�ָ��
	CDataInterface *pDI = g_system.GetCurData(nUserID,szTableName);
	if (pDI ==NULL)
	{
		if (g_system.OpenTable(nUserID,szTableName,szTableName)==false)
			return SetResultState(false,outP,GetLanguage(OperationFail)); 
		
		pDI = g_system.GetCurData(nUserID,szTableName);
		if (pDI ==NULL)
			return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	}
#ifdef WINPLAT
	CTString szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	szPath += "/DataDeal.dll";	
	HINSTANCE hmod = LoadLibrary(szPath.GetData());
	if (hmod==NULL)
	{//û���ҵ���Ӧ��DLL
		SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
		return false;
	}
	
	typedef int (*pSample)(CDataInterface *pData, LPCTSTR lpszVO);
	pSample proc;
	proc = (pSample)GetProcAddress(hmod,"Sample");
	if (proc==NULL)
	{//"û���ҵ���Ӧ�Ĺ�����"
		SetResultState(false,outP,GetLanguage(ProcNameNotFind));
		return false;
	}
	
	int rst = (*proc)(pDI,szVO.c_str());
	if (rst == 0)
		return SetResultState(false,outP,"");
	else
		return SetResultState(true,outP);
	
#else
	
#endif
	return SetResultState(true,outP);
	return true;
}

bool TCManager::Random(TCPacket &inP, AnaWord &aw, string szVO, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	string szTableName = "";
	int nCount = aw.GetWordCount();
	for (int i=0;i<nCount;i++)
	{
		if (aw.GetAt(i) == "/DATA" &&  i< nCount-2 )
		{
			szTableName = aw.GetAt(i+2);
			break;
		}
	}
	if (szTableName.size()==0)
		return SetResultState(false,outP,GetLanguage(FormatIsError));
	//�ӿ�ָ��
	CDataInterface *pDI = g_system.GetCurData(nUserID,szTableName);
	if (pDI ==NULL)
	{
		if (g_system.OpenTable(nUserID,szTableName,szTableName)==false)
			return SetResultState(false,outP,GetLanguage(OperationFail)); 
		
		pDI = g_system.GetCurData(nUserID,szTableName);
		if (pDI ==NULL)
			return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	}
#ifdef WINPLAT
	CTString szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	szPath += "/DataDeal.dll";	
	HINSTANCE hmod = LoadLibrary(szPath.GetData());
	if (hmod==NULL)
	{//û���ҵ���Ӧ��DLL
		SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
		return false;
	}
	
	typedef int (*pRandom)(CDataInterface *pData, LPCTSTR lpszVO);
	pRandom proc;
	proc = (pRandom)GetProcAddress(hmod,"Random");
	if (proc==NULL)
	{//"û���ҵ���Ӧ�Ĺ�����"
		SetResultState(false,outP,GetLanguage(ProcNameNotFind));
		return false;
	}
	
	int rst = (*proc)(pDI,szVO.c_str());
	if (rst == 0)
		return SetResultState(false,outP,"");
	else
		return SetResultState(true,outP);
	
#else
	
#endif
	return SetResultState(true,outP);
	return true;
}

/*
 *REG /DATA=���� ��������  λ��δȷ�� 
 *���ø����㷨
 */
bool TCManager::algorithmic(TCPacket &inP,string strName,AnaWord &aw, TCPacket &outP)
{
	CTString spath;
	TCSystem::GetTempPath(spath);
	spath = spath + "Sys.dat";
	//--UNIX PORT
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	if ( GTime(st) >= Expire )
	{
		if(_access(spath.GetData(),0))//-- file not existed
		{ 
			return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
		}
	}
/*	
#ifdef WINPLAT 
	WIN32_FIND_DATA wfd;
	if (FindFirstFile(spath.GetData(),&wfd) != INVALID_HANDLE_VALUE)
	{
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	}
			
#else

#endif
*/

	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	
	bool bMeOpenTable = false;  //�Ƿ�Ϊc++�Լ��򿪵�
	string sTableName = "";
	int nCount = aw.GetWordCount();
	for (int i=0;i<nCount;i++)
	{
		if (aw.GetAt(i) == "/DATA" &&  i< nCount-2 )
		{
			sTableName = aw.GetAt(i+2);
		}
	}
	if (sTableName.size()==0)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	
	CDataInterface *pDI = g_system.GetCurData(nUserID,sTableName);
	if (pDI ==NULL)
	{		
		if (g_system.OpenTable(nUserID,sTableName,sTableName)==false)
			return SetResultState(false,outP,GetLanguage(OperationFail)); 
		
		pDI = g_system.GetCurData(nUserID,sTableName);
		if (pDI ==NULL)
			return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
		bMeOpenTable = true;
	}
	CTString str = aw.GetAt(0);			
	CTString sDllName, sProcName;
	int nProcType;
	if (TCSystem::GetModuleInfo(str,sDllName, sProcName,nProcType)==true)
	{
	//--UNIX PORT
//#ifdef WINPLAT
		HINSTANCE hmod = LoadLibrary(sDllName.GetData());
		if (hmod==NULL)       //û���ҵ���Ӧ��DLL
		{
			if (bMeOpenTable) //�Լ��ͷ�
				g_system.CloseTable(nUserID, sTableName); 
			SetResultState(false,outP,GetLanguage(DLLFileNotFind));//"û���ҵ���Ӧ��DLL"
			return false;
		}
				
		if (nProcType ==2)
		{
			typedef int (*pTrain)(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt, string VO, string &ppOutStruct,CResult **ppRst);
			pTrain proc;
			proc = (pTrain)GetProcAddress(hmod,sProcName.GetData());
			if (proc==NULL)       //"û���ҵ���Ӧ�Ĺ�����"
			{
				if (bMeOpenTable) //�Լ��ͷ�
					g_system.CloseTable(nUserID, sTableName); 
				SetResultState(false,outP,GetLanguage(ProcNameNotFind));
				return false;
			}
			Interaction itTemp;
			string VO = strName;
			string ppOutStruct;
			CResult *pRst = NULL;
			int rst =(*proc)(pDI,g_system.GetCurEnv(0),&itTemp,VO,ppOutStruct,&pRst);
			if (pRst)
			{
				int nBuflen = pRst->GetBufLen();
				BYTE *pOut = new BYTE[nBuflen];
				BYTE *pTemp = pOut;
				pRst->Serialize(pTemp);
				outP.SetPacketName("Result");
				delete pRst;
				outP.AddItem("Out",( char*)pOut,nBuflen);
				delete []pOut;
			}
			//////////////////�ͷ���Դ///////////////////
			if (bMeOpenTable)
				g_system.CloseTable(nUserID, sTableName); 
			::FreeLibrary((HMODULE)hmod);
			//////////////////�ͷ���Դ///////////////////
			if (rst != 0)
				return SetResultState(false,outP,"");
			else
				return SetResultState(true,outP);
			
		}
		else
		{//===temp
		}
					
//#else
//	int rc; /* return codes */
//	void *FunctionLib =NULL; /* Handle to shared lib file */
//	//int (*Function)(int a); /* Pointer to loaded routine */
//	//typedef int (*FunctionA)(int a);
//	typedef int (*pTrain)(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt, string VO, string &ppOutStruct,CResult **ppRst);
//	const char *dlError; /* Pointer to error string */
//
//	//FunctionLib = dlopen("/share/b/low.so",RTLD_LAZY);
//	FunctionLib = dlopen(sDllName.GetData(),RTLD_LAZY);
//	dlError = dlerror();
//	if( dlError )
//	{
//		printf(" AAA 3-Open Library with absolute path return-%s- \r\n", dlError);
//		exit(1);
//	}

//	Function = (pTrain)dlsym( FunctionLib, sProcName.GetData());// 
//	dlError = dlerror();
//	if( dlError ) 
//	{
//		printf(" AAA 4-Find symbol printUPPERCASE ����-%s- \r\n", dlError);
//		exit(1);
//	}
//
//	Interaction itTemp;
//	string VO = strName;
//	string ppOutStruct;
//	CResult *pRst = NULL;
//	int rst = (*Function)(pDI,g_system.GetCurEnv(0),&itTemp,VO,ppOutStruct,&pRst);
//	if (pRst)
//	{
//		int nBuflen = pRst->GetBufLen();
//		BYTE *pOut = new BYTE[nBuflen];
//		BYTE *pTemp = pOut;
//		pRst->Serialize(pTemp);
//		outP.SetPacketName("Result");
//		delete pRst;
//		outP.AddItem("Out",( char*)pOut,nBuflen);
//		delete []pOut;
//	}
//	if (rst != 0)
//		return SetResultState(false,outP,"");
//	else
//		return SetResultState(true,outP);
//
//	printf(" AAA 5-printUPPERCASE return-%s- value :%d\r\n", dlError,rc);
//
//
	/* 6-Close the shared library handle */
	/* Note: after the dlclose, "printUPPERCASE" is not loaded */
//	rc = dlclose(FunctionLib);
//	if( rc )
//	{
//		dlError = dlerror();
//		printf(" AAA 6-Close handle return-%s- \r\n",dlError); 
//		exit(1);
//	}
//#endif
	} //-- end with GetModuleInfo
	else
	{//===temp ��ʱ������
	}
	return true;
}
/*
#�������ļ����ֶνṹ [] ��ʾ����Ϊ��
CreateTable  \{ ���� ���� ���� {,���� ���� ����} \}	

���͵Ļ���s �ַ��� d �������� n ��������
���� Text ������

 */
bool TCManager::CreateTable(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	string sTableName = g_system.CreateUserData(nUserID);
	CDataInterface *pDI = g_system.GetCurData(nUserID,sTableName);

	AnaWord awNew;
	if (AnaSyntax::GetMidWords(aw,"{","}",",",awNew)==false)
	{
		outP.AddItem("Text",FormatIsError);
		return false;
	}

	int nCnt = awNew.GetWordCount();
	for (int i=0;i<nCnt/3;i++)
	{
		CTString sType = awNew.GetAt(3*i+1);
		CFieldType ft = fDouble;
		if (sType == "s" || sType == "S")
			ft = fString;
		else if (sType == "d" || sType == "D")
			ft = fDate;
		else if (sType == "n" || sType == "N")
			ft = fInt;
		else if (sType == "f" || sType == "F")
			ft = fDouble;
		else if (sType == "b" || sType == "B")
			ft = fBoolean;

		int nWidth = atoi(awNew.GetAt(3*i+2).c_str());
		pDI->m_DataAccess.AddField(awNew.GetAt(3*i),ft,nWidth);
	}
	outP.AddItem("Text",sTableName);//���ر���

	return SetResultState(true,outP);
}
/*
Text	
	OpenTable ����  �ļ���
*/
bool TCManager::OpenTable(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	if (aw.GetWordCount()!=3)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	if (g_system.OpenTable(nUserID,aw.GetAt(1),aw.GetAt(2))==false)
		return SetResultState(false,outP,GetLanguage(OperationFail)); 
	else
		return SetResultState(true,outP);
}
/*  �ر�һ����
Text	
	CloseTable ����  
*/
bool TCManager::CloseTable(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	if (aw.GetWordCount()!=2)
		return SetResultState(false,outP,GetLanguage(FormatIsError)); 

	if (g_system.CloseTable(nUserID,aw.GetAt(1))==false)
		return SetResultState(false,outP,GetLanguage(OperationFail)); 
	else
		return SetResultState(true,outP);
}

/*
#����ǰ���������
>Command
	Text
		AddRowText ����  {������}
//	Data
//		����������
<Command
	Text
		State OK|Fail [������Ϣ]
 */
bool TCManager::AddRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(CurrentDataIsNULL));

	AnaWord awNew;
	if (AnaSyntax::GetMidWords(aw,"{","}","",awNew)==false)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	int nCnt = awNew.GetWordCount()	;
	//awNew.Print();

	int nFieldCount = pDI->m_DataAccess.GetFieldCount();
	if (nCnt%nFieldCount != 0 )
		return SetResultState(false,outP,GetLanguage(FormatIsError));
	
	for (int i=0;i<nCnt;i++)
	{
		if (i%nFieldCount == 0 )
			pDI->m_DataAccess.Append();
		pDI->m_DataAccess.FieldByIndex(i%nFieldCount)->SetAsString(awNew.GetAt(i));
	}

	return SetResultState(true,outP);
}

//EditRowText���� ��ʼ�к� ���� \{ ������ \}
bool TCManager::EditRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(CurrentDataIsNULL));

	string sStartNo = aw.GetAt(2);
	string sRowNum = aw.GetAt(3);
	int nStartNo = atoi(sStartNo.c_str());
	int nRowNum  = atoi(sRowNum.c_str());
	if (nStartNo <0 || nRowNum+nStartNo>pDI->GetRowCount())
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	AnaWord awNew;
	if (AnaSyntax::GetMidWords(aw,"{","}","",awNew)==false)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	int nCnt = awNew.GetWordCount()	;
	//awNew.Print();

	int nFieldCount = pDI->m_DataAccess.GetFieldCount();
	if (nCnt%nFieldCount != 0 )
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	for (int i=0;i<nCnt;i++)
	{
		if (i%nFieldCount == 0 )
		{
			pDI->m_DataAccess.SetCurRecNo(++nStartNo);
			pDI->m_DataAccess.Edit();
		}
		
		if (awNew.GetAt(i)=="." || awNew.GetAt(i)=="")
			pDI->m_DataAccess.FieldByIndex(i%nFieldCount)->SetNull();
		else
			pDI->m_DataAccess.FieldByIndex(i%nFieldCount)->SetAsString(awNew.GetAt(i));
	}

	return SetResultState(true,outP);
}

//InsertRowText ���� ��ʼ�к� ���� [\{ ������ \}]
bool TCManager::InsertRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(CurrentDataIsNULL));

	string sStartNo = aw.GetAt(2);
	string sRowNum = aw.GetAt(3);
	int nStartNo = atoi(sStartNo.c_str());
	int nRowNum  = atoi(sRowNum.c_str());
	if (nStartNo <=0 )
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	if (aw.GetWordCount() == 4)
	{//����ȫΪ��ֵ
		pDI->m_DataAccess.SetCurRecNo(nStartNo);
		for (int i=0;i<nRowNum; i++)
			pDI->m_DataAccess.Insert();
		return SetResultState(true,outP);

	}

	AnaWord awNew;
	if (AnaSyntax::GetMidWords(aw,"{","}","",awNew)==false)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	int nCnt = awNew.GetWordCount()	;
	//awNew.Print();

	int nFieldCount = pDI->m_DataAccess.GetFieldCount();
	if (nCnt%nFieldCount != 0 )
		return SetResultState(false,outP,GetLanguage(FormatIsError));
	
	
	pDI->m_DataAccess.SetCurRecNo(nStartNo+1);
	for (int i=0;i<nCnt;i++)
	{
		if (i%nFieldCount == 0 )
		{
			pDI->m_DataAccess.Insert();
		}

		pDI->m_DataAccess.FieldByIndex(i%nFieldCount)->SetAsString(awNew.GetAt(i));
	}

	return SetResultState(true,outP);
}
//Text	DeleteRowText���� ��ʼ�к� ����
bool TCManager::DeleteRowText(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(CurrentDataIsNULL));

	string sStartNo = aw.GetAt(2);
	string sRowNum = aw.GetAt(3);
	int nStartNo = atoi(sStartNo.c_str());
	int nRowNum  = atoi(sRowNum.c_str());
	if (nStartNo <=0 || nRowNum+nStartNo>pDI->GetRowCount()+1)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	
	pDI->m_DataAccess.SetCurRecNo(nStartNo);
	for (int i=0;i<nRowNum;i++)
	{
		pDI->m_DataAccess.Delete();
	}

	return SetResultState(true,outP);
}
/*
#����ǰ�����ݱ��浽�ļ���
>Command
	Text
		SaveFile ���� �ļ���
<Command
	Text
		State OK|Fail [������Ϣ]
*/
bool TCManager::SaveFile(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));

	if (aw.GetWordCount()<3)
		return SetResultState(false,outP,GetLanguage(FormatIsError));

	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(CurrentDataIsNULL));

	string strFileName = aw.GetAt(2);
	if (strFileName.substr(strFileName.size()-3) == "txt")
	{
		TCLFile file(strFileName.c_str(),TCLFile::modeCreate|TCLFile::modeWrite);

		//*
		CDataAccess * pDataAccess ;
		pDataAccess = &pDI->m_DataAccess;
		pDataAccess->First();
		int nFieldcount= pDataAccess->m_pFieldList->GetFieldCount();
		bool isFirst = true;
		for (int i =0;i<nFieldcount;i++)
		{//������ֶ��������������
				if (isFirst)
				{
					CTString strT = pDataAccess->m_pFieldList->FieldByIndex(i)->GetFieldName();
					file.Write(strT.GetData(),strT.GetLength());
					isFirst = false;
				}
				else
				{
					CTString strT = pDataAccess->m_pFieldList->FieldByIndex(i)->GetFieldName();
					char aa = '\t';
					file <<aa;
					file.Write(strT.GetData(),strT.GetLength());
				}
			
		}

		char aa = '\r';
		file <<aa;
		aa = '\n';
		file <<aa;

		while (!pDataAccess->Eof())
		{
			bool isFirst = true;
			for (int i =0;i<nFieldcount;i++)
			{
				if (isFirst)
				{
					CTString strT = pDataAccess->m_pFieldList->FieldByIndex(i)->GetAsString();
					file.Write(strT.GetData(),strT.GetLength());
					isFirst = false;
				}
				else
				{
					CTString strT = pDataAccess->m_pFieldList->FieldByIndex(i)->GetAsString();
					char aa = '\t';
					file <<aa;
					file.Write(strT.GetData(),strT.GetLength());
				}
			}
			char aa = '\r';
			file <<aa;
			aa = '\n';
			file <<aa;
			pDataAccess->Next();
		}//*/
	}
	else
	{
		CTString strOut;
		pDI->m_DataAccess.SaveFile(strFileName.c_str(),&strOut);
	}

	
	return SetResultState(true,outP);
}

/*
 *���÷���״̬������ɹ�bSuccessΪtrue ���� Ϊfalse strMsgΪ����
 State	���� [����]	�ı�����,����Ϊ0��Ϊ�ɹ�������Ϊʧ�ܴ��룬 ����Ϊʧ�ܵ��ַ�������
 */
bool TCManager::SetResultState(bool bSuccess, TCPacket &outP,string strMsg )
{
	string str = "1 ";
	if (bSuccess)
		str = "0 ";
	outP.AddItem("State",str+ strMsg);
	return bSuccess;
}
/*
 *�û���¼
 */
bool TCManager::Login(TCPacket &inP, TCPacket &outP)
{
	int nUserID = GetUserID(inP);

	g_system.AddUser(nUserID);
	return SetResultState(true,outP);
}

/*
 *�û�ȡ��
 */
bool TCManager::Logout(TCPacket &inP,TCPacket &outP)
{
	int nUserID = GetUserID(inP);

	g_system.RemoveUser(nUserID);
	return SetResultState(true,outP);
}

/*
 *�õ����У����������û�ID�����û���򷵻�0
 */
int TCManager::GetUserID(TCPacket &inP)
{
	char *pChar = inP.GetItemData("UserID");
	int nLen = inP.GetItemDataLen("UserID");
	if (nLen>0)
	{//1.1
		string strUserID = inP.GetItemDataAsString("UserID");
		return atoi(strUserID.c_str());
	}
	return 0;
}

/*
 *�����ǰ�û�ID�����ݼ�
 */
bool TCManager::ClearCurData(TCPacket &inP,AnaWord &aw, TCPacket &outP)
{
	int nUserID = GetUserID(inP);
	if (nUserID==0)
		return SetResultState(false,outP,GetLanguage(UserIDIsNotFound));
	CDataInterface *pDI = g_system.GetCurData(nUserID,aw.GetAt(1));
	if (pDI ==NULL)
		return SetResultState(false,outP,GetLanguage(UserTableIsNotFound));
	pDI->m_DataAccess.NewFile();
	return SetResultState(true,outP);
}




















































































