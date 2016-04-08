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

//�����е���
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

//��������Ϣ�б����
class DecTreeColList
{
public:
	DecTreeColList();
	~DecTreeColList();
	
public:
	int				 getColCount();					    //�õ��еĸ���
	DecTreeCol*		 ColByName(CTString ColName);        //ͨ���е����Ƶõ���
	DecTreeCol*		 ColByIndex(int nIndex);		    //ͨ���е������ŵõ���
	DecTreeCol*      CreateCol(CFieldType FieldType);   //�������ʹ�����

	//	int              getColListData(CUIntArray &nColAry,TCColMatrix &ColMatrix, CDataAccess *pDataAccess); // ��ֵ�����ݿ��д����б���//�����е���	-by pyx 2006-02-16
	
	
public:
	TDoubleAry		 m_dElement;
	TIntAry			 m_nElement;
	CTString          m_strElement;
	DecTreeCol		 *m_pdtCol;
	CDataAccess	     *m_pDataAccess;

	int              m_nIndex;
	
};


#endif // !defined(AFX_DECTREECOL_H__A408FC1A_F976_4AD3_9EC5_60DBF5A0423C__INCLUDED_)
