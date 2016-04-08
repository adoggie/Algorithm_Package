// TCurSys.cpp: implementation of the TCurSys class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TCurSys.h"
#include "../arithInterface/EnvParam.h"
#include "../DataInterface/DataInterface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCurSys::TCurSys()
{
	//m_pData = NULL;
	//m_pEnv = new CEnvParam();
}

TCurSys::~TCurSys()
{
	int nSize = m_UserIDAry.size();
	for (int i=0;i<nSize;i++)
	{
		delete m_DBAry[i];
		delete m_pIteractionAry[i];
		delete m_pEnvAry[i];
	}
	m_DBAry.clear();
	m_pIteractionAry.clear();
	m_pEnvAry.clear();
}

/*
 *�����û�ID��ȡ��ǰ���ݼ�
 */
CDataInterface * TCurSys::GetCurData(int nUserID,string sDBName)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return NULL;
	return m_DBAry[nPos]->GetDataByName(sDBName);
}

/*
 * ����ϵͳ��ǰ�����ݼ�,���û���û�����ʧ��
 */
void TCurSys::SetCurData(int nUserID, CDataInterface *pData)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return ;
	
	if (m_DBAry[nPos]->GetDataByName("temp") == pData)
		return;//ͬ���򷵻�
	if (m_DBAry[nPos])
		delete m_DBAry[nPos];
	
//	m_DBAry[nPos] = pData;
	
}
/*
 *�����û�ID��ȡ��������
 */
CEnvParam* TCurSys::GetCurEnv(int nUserID)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return NULL;
	return m_pEnvAry[nPos];
}

/*
 *�����û�ID��������ϵͳ�������λ��,û���ҵ����򷵻أ�1
 */
int TCurSys::FindPos(int nUserID)
{
	vector<int>::iterator it = find(m_UserIDAry.begin(),m_UserIDAry.end(),nUserID);
	if (it != m_UserIDAry.end())
		return it- m_UserIDAry.begin();
	else
		return -1;
}

/*
 *���һ���û���������û����ڣ���ɾ�������
 */
void TCurSys::AddUser(int nUserID)
{
	int nPos = FindPos(nUserID);
	if (nPos !=-1)
		RemoveUser(nUserID);

	m_UserIDAry.push_back(nUserID);
	m_DBAry.push_back(new OpenDBAry());
	m_pIteractionAry.push_back(new Interaction());
	m_pEnvAry.push_back(new CEnvParam());
}
/*
 *ɾ��һ���û���������û������ڣ�����ʧ��
 */
bool TCurSys::RemoveUser(int nUserID)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return false;

	m_UserIDAry.erase(m_UserIDAry.begin()+nPos);

	delete *(m_DBAry.begin()+nPos);
	m_DBAry.erase(m_DBAry.begin()+nPos);

	delete *(m_pIteractionAry.begin()+nPos);
	m_pIteractionAry.erase(m_pIteractionAry.begin()+nPos);

	delete *(m_pEnvAry.begin()+nPos);
	m_pEnvAry.erase(m_pEnvAry.begin()+nPos);


	return true;
}

OpenDBAry::OpenDBAry()
{
}

OpenDBAry::~OpenDBAry()
{
	int nSize = m_CurDBAry.size();
	for (int i=0;i<nSize;i++)
	{
		delete m_CurDBAry[i];
	}
}
/*
 * ����ҵ��򷵻�λ�ã����򷵻�-1
 */
int OpenDBAry::GetPosByName(string sName)
{
	vector<string>::iterator it = find(m_sNameAry.begin(),m_sNameAry.end(),sName);
	if (it != m_sNameAry.end())
		return it- m_sNameAry.begin();
	else
		return -1;
}
/*
 * �����������������ݼ������û�У��򷵻�NULL
 */
CDataInterface * OpenDBAry::GetDataByName(string sName)
{
	int nPos = GetPosByName(sName);
	if (nPos ==  - 1)
		return NULL;
	else
		return m_CurDBAry[nPos];
}

int OpenDBAry::GetCount()
{
	return m_CurDBAry.size();
}
/*
 *����һ���±�������ʱ����
 */
string OpenDBAry::NewTable()
{
	
	string strName ;
	int i=m_CurDBAry.size();
	do {
		char buf[50];
		sprintf(buf,"~temp%d",i++);
		strName = buf;		
	} while(GetPosByName(strName)!=-1);
	m_CurDBAry.push_back(new CDataInterface());
	m_sNameAry.push_back(strName);
	return strName;
}

string TCurSys::CreateUserData(int nUserID)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return "";
	return m_DBAry[nPos]->NewTable();
}

bool TCurSys::OpenTable(int nUserID,string sTableName, string sFileName)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return false;
	if (m_DBAry[nPos]->GetPosByName(sTableName)!=-1)
		return false;//��ǰ����Ѿ�����
	return m_DBAry[nPos]->OpenTable(sTableName, sFileName);
}

bool OpenDBAry::OpenTable(string sTableName, string sFileName)
{
	int nPos = GetPosByName(sTableName);
	if (nPos !=-1)
		return false;

	CDataInterface *pDI = new CDataInterface();
	if (pDI->m_DataAccess.LoadFile(sFileName.c_str())==false)
	{
		delete pDI;
		return false;
	}
	m_CurDBAry.push_back(pDI);
	m_sNameAry.push_back(sTableName);
	return true;
}

bool TCurSys::CloseTable(int nUserID,string sTableName)
{
	int nPos = FindPos(nUserID);
	if (nPos ==-1)
		return false;
	return m_DBAry[nPos]->CloseTable(sTableName);
}

bool OpenDBAry::CloseTable(string sTableName)
{
	int nPos = GetPosByName(sTableName);
	if (nPos ==-1)
		return false;

	delete m_CurDBAry[nPos];
	m_CurDBAry.erase(m_CurDBAry.begin()+nPos);
	m_sNameAry.erase(m_sNameAry.begin()+nPos);
	return true;

}
