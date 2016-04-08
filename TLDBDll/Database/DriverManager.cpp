// DriverManager.cpp: implementation of the CDriverManager class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
#include "DriverManager.h"
#include "cfileoperation.h"
#include "cheader.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDriverManager::CDriverManager()
{

}

CDriverManager::~CDriverManager()
{

}
bool CDriverManager::LoadFile(LPCTSTR lpszFileName,CDriverBase** pDriver,CFields* pFields)
{
	CTString str = (CTString)lpszFileName;
	str  = GetExtName(str);
	str.MakeLower();
	if (str == "mkw" )
	{
/*===uc  		int nVersion = -1;
		{
			//���ȸ��ݴ�����ļ��������ļ���ʵ��
			TRY
			{//�ļ��򿪷�ʽ��
				TCLFile cf(lpszFileName,TCLFile::modeRead|TCLFile::shareDenyWrite );
				CHeader ch;

				int m_nFileVersion = sizeof(CHeader);
				cf.Read(&m_nFileVersion,4);
				if (m_nFileVersion == 0xFFFFFF03	)
					nVersion = 100;
				if (m_nFileVersion == 0x01FFFE03 )
					nVersion = 110;
				if (m_nFileVersion == 0x02FFFE03)
					nVersion = 120;
				//nVersion = ch.GetFileVersion() ;
			}
			CATCH( CFileException, e )
			{
				return false;
			}
			END_CATCH

		}

		if (nVersion == -1)
			return false;

		if (nVersion == 100 || nVersion == 110)
		{
			typedef  bool (*pReadTLData)(CDriverBase** ppDriver,CFields* pFields,LPCSTR sFile);
			HINSTANCE hDll = AfxLoadLibrary("OdData.dll");
			if (!hDll)
			{
				CTString strMSG = "û���ҵ���Ӧ��OdData.dll,װ��ʧ��!";
				//===temp AfxMessageBox(strMSG);
				return false;
			}
			CDriverBase * pOriginData ;
			pReadTLData pImport;
			pImport = (pReadTLData)::GetProcAddress(hDll,"IniReadData");
			if((*pImport)(&pOriginData,pFields,lpszFileName))
			{
				*pDriver = pOriginData;
				return true;
			}
			else 
				return false;
		}
		else if (nVersion == 120 )   */
		{
			CDriverBase * pOriginData = new CFileOperation();
			
			*pDriver  = pOriginData;//1219 LPS Mod.������ȥй©.
			if (!((CFileOperation*)pOriginData)->IniReadFile(lpszFileName,pFields))
				return false;
		}

	}
	else if (str =="mdb")
	{
	}
	else 
	{
		return false;
	}
	return true;

}


CTString CDriverManager::GetExtName(CTString strFileName)
{
	int pos = strFileName.ReverseFind('.');
	
	if (pos ==-1)
		return "";
	else
	{
		strFileName.Delete(0,pos+1);
		return strFileName;
	}
}
