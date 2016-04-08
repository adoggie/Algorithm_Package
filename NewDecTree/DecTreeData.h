#if !defined(AFX_DECTREECOLLISTA_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_)
#define AFX_DECTREECOLLIST_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\public\TCLFile.h"
#include "..\public\TString.h"
#include "..\DataInterface\DataInterface.h"
#include "..\cpknl\cpknl.h"
#include "..\Result\Result.h"

//管理列的类
class DecTreeCol
{
public:
	DecTreeCol();
	~DecTreeCol();
	
public:
	CTString			 getColName();
	void			 setColName(CTString strColName);
	
	int				 getColLength();
	void			 setColLength(int nLength);
	
	
public:
	TDoubleAry		 m_dElement;
	TIntAry			 m_nElement;
	CTString		 m_strColName;
	int	             m_nColLength;
	
};

//管理列信息列表的类
class DecTreeColList
{
public:
	DecTreeColList();
	~DecTreeColList();

public:
	int				getColNum(int nIndex);


public:
	DecTreeCol		dtCol;

};









#endif // !defined(AFX_DECTREECOLLIST_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_)
