// Script.h: interface for the CScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPT_H__E8329BCB_D7FB_461E_B722_326A6B689198__INCLUDED_)
#define AFX_SCRIPT_H__E8329BCB_D7FB_461E_B722_326A6B689198__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CScript  
{
	struct sItem{
		int nFirstPos;
		int nLastPos;
	};
	CPtrArray m_tempArray;
	CTStringArray m_strArray;
	CTString m_strName;
public:
	void Test();
	bool ExistItemData(CTString strParamName, CTString Value);
	bool GetItemData(CTString strParamName, int nPos,double &value);
	bool GetItemData(CTString strParamName, int nPos,int &value);
	bool GetItemData(CTString strParamName, int nPos,CTString &Value);
	int GetItemCount(CTString strParamName);
	int FindParamName(CTString strParamName);
	
	CTString GetParamName(int nIndex);
	int GetParamCount();
	bool ImportStr(CTString strDesc);
	CTString GetMainName();
	CScript();
	virtual ~CScript();

protected:
	void Release(CPtrArray *pArr);//ÊÍ·ÅÄÚ´æ
	CTString GetLineByParamName(CTString strParamName);
	bool GetItem(CTString strLine, CPtrArray &tempArray);
	CTString GetItemStr(CTString strLine);
	CTString GetName(CTString strLine);
};

#endif // !defined(AFX_SCRIPT_H__E8329BCB_D7FB_461E_B722_326A6B689198__INCLUDED_)
