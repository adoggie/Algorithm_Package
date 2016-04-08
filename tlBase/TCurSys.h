// TCurSys.h: interface for the TCurSys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCURSYS_H__02478AF7_85A3_45CF_92A5_974F7CD91F6C__INCLUDED_)
#define AFX_TCURSYS_H__02478AF7_85A3_45CF_92A5_974F7CD91F6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "../arithInterface/EnvParam.h"
using namespace std;
class CDataInterface;
class CEnvParam;

class OpenDBAry
{
	vector<CDataInterface *> m_CurDBAry;
	vector<string> m_sNameAry;
public:
	bool CloseTable(string sTableName);
	bool OpenTable(string sTableName, string sFileName);
	string NewTable();
	int GetCount();
	CDataInterface * GetDataByName(string sName);
	int GetPosByName(string sName);
	OpenDBAry();
	virtual ~OpenDBAry();
};

class TCurSys  
{
	//CDataInterface *m_pData;
	vector<int>m_UserIDAry;
	vector<OpenDBAry *> m_DBAry;
	vector<Interaction*> m_pIteractionAry;
	vector<CEnvParam *> m_pEnvAry;
	
public:
	
	void SetCurData(int nUserID,CDataInterface *pData);
	CDataInterface * GetCurData(int nUserID,string sDBName);
public:
	bool CloseTable(int nUserID,string sTableName);
	bool OpenTable(int nUserID,string sTableName,string sFileName);
	string CreateUserData(int nUserID);
	bool RemoveUser(int nUserID);
	void AddUser(int nUserID);
	int FindPos(int nUserID);
	CEnvParam* GetCurEnv(int nUserID);
	TCurSys();
	virtual ~TCurSys();

};

#endif // !defined(AFX_TCURSYS_H__02478AF7_85A3_45CF_92A5_974F7CD91F6C__INCLUDED_)
