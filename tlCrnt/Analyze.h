// Analyze.h: interface for the CAnalyze class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYZE_H__DD61FDFC_FC7F_4365_B5BD_86FFBC9970B7__INCLUDED_)
#define AFX_ANALYZE_H__DD61FDFC_FC7F_4365_B5BD_86FFBC9970B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////
//// 分析的公共算法
class CScript;
class AFX_EXT_CLASS CAnalyze  
{
public:
	static bool GetListData(CScript *psct,CTString strParam ,CDataInterface* pDataInterface,TIntStruct &tInt);
	static void CopyFreqConf(CPtrArray * pFrom, CPtrArray * pTo);
	static int GetFieldIndex(CDataInterface* pDataInterface,CTString strName);
	static int Purify(CPtrArray *pDesConfs);
	static int Purify(CDataInterface * pDataInterface,CPtrArray *pDesConfs);
	CAnalyze();
	virtual ~CAnalyze();
	static int IsExist(CTStringArray *strArray,CTString strName);

	static BOOL GetFNameList(CPtrArray *pConfArray, CTStringArray *pstrNameArray);
	static void CopyDesConf(CPtrArray *pFrom,CPtrArray *pTo,CTStringArray * pstrResArray,int nType=0);
};

#endif // !defined(AFX_ANALYZE_H__DD61FDFC_FC7F_4365_B5BD_86FFBC9970B7__INCLUDED_)
