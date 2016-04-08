
#ifndef ERICDONG_CPAGEARRAYSORT_
#define ERICDONG_CPAGEARRAYSORT_

#include "../../Public/common.h"


//extern CPageState;
//typedef byte  CPageState;
class CPage;

class CPageArraySort
{

	int m_nTotalSize;   //总分配空间
	int m_nWorkSize;    //逻辑记录非零的个数
	int m_nCurrent;     //指示当前页
	int m_nPageCount;   //内存中生成的页总个数,大于等于m_nWorkSize
	CPage ** m_ptrBuf;

	bool RemoveAll();
	bool InsertAt(int nIndex,CPage * pPage);
	bool DeleteRemove(CPage* pPage); 

	bool FindIndexInEmpty(CPage*pPage,int &nIndex);

	int FindPageByAllAddr(CPage*pPage);//通过地址来查找页的位置.最慢

	bool RemoveAt(int nIndex);
	bool GrowInsert(CPage* pPage);

	bool FindIndex(CPage* pPage,int& nIndex);

public:
	bool FindIndexByRecNo(int RecNo,int &nIndex);
	//nRight默认传入总记录条数
	CPage* FindIndexByRecNo(int RecNo,int&nLeft,int& nRight);
	void AddPage(CPage*pPage);
	void RemovePage(CPage *pPage);
	CPage *GetCurPage();
	bool SetCurPage(CPage *pPage);
	CPage *GetAvailablePage(int *pnIndex =NULL);//得到一个可用的页
	bool MoveToEmptyArea(int nIndex);
	bool MoveToWorkArea(CPage *pPage);

	CPage *GetPageAt(int nIndex);//得到nIndex所对应的页
	void Clear();

	int GetTotalSize();
	int GetWorkSize();
	int GetPageSize();

	void SetTotalSize(int nNewSize);


	CPage* FindRecNoInPage(int nRecNo ) ;
//////////////////////////////////////////
	CPageArraySort();
	~CPageArraySort();
};

#endif
