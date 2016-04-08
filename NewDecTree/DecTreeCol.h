#if !defined(AFX_DECTREECOL_H__A408FC1A_F976_4AD3_9EC5_60DBF5A0423C__INCLUDED_)
#define AFX_DECTREECOL_H__A408FC1A_F976_4AD3_9EC5_60DBF5A0423C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecTreeCol.h : header file
//
//#include "DecTreeData.h"

/////////////////////////////////////////////////////////////////////////////
// DecTreeCol 
class DecTreeColList;
typedef TCDataArray<DecTreeColList> TDColList;
//typedef CTypedPtrArray<CPtrArray,TDColList*> TCColMatrix; -by pyx 2006-02-15
CPtrArray TCColMatrix;

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
	int				 getColCount();					    //得到列的个数
	DecTreeCol*		 ColByName(CTString ColName);        //通过列的名称得到列
	DecTreeCol*		 ColByIndex(int nIndex);		    //通过列的索引号得到列
	DecTreeCol*      CreateCol(CFieldType FieldType);   //根据类型创建列

	//	int              getColListData(CUIntArray &nColAry,TCColMatrix &ColMatrix, CDataAccess *pDataAccess); // 把值从数据库中传到列表中//管理列的类	-by pyx 2006-02-16
	
	
public:
	TDoubleAry		 m_dElement;
	TIntAry			 m_nElement;
	CTString          m_strElement;
	DecTreeCol		 *m_pdtCol;
	CDataAccess	     *m_pDataAccess;

	int              m_nIndex;
	
};


#endif // !defined(AFX_DECTREECOL_H__A408FC1A_F976_4AD3_9EC5_60DBF5A0423C__INCLUDED_)
