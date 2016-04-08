
#ifndef ERICDONG_CPAGEARRAYSORT_
#define ERICDONG_CPAGEARRAYSORT_

#include "../../Public/common.h"


//extern CPageState;
//typedef byte  CPageState;
class CPage;

class CPageArraySort
{

	int m_nTotalSize;   //�ܷ���ռ�
	int m_nWorkSize;    //�߼���¼����ĸ���
	int m_nCurrent;     //ָʾ��ǰҳ
	int m_nPageCount;   //�ڴ������ɵ�ҳ�ܸ���,���ڵ���m_nWorkSize
	CPage ** m_ptrBuf;

	bool RemoveAll();
	bool InsertAt(int nIndex,CPage * pPage);
	bool DeleteRemove(CPage* pPage); 

	bool FindIndexInEmpty(CPage*pPage,int &nIndex);

	int FindPageByAllAddr(CPage*pPage);//ͨ����ַ������ҳ��λ��.����

	bool RemoveAt(int nIndex);
	bool GrowInsert(CPage* pPage);

	bool FindIndex(CPage* pPage,int& nIndex);

public:
	bool FindIndexByRecNo(int RecNo,int &nIndex);
	//nRightĬ�ϴ����ܼ�¼����
	CPage* FindIndexByRecNo(int RecNo,int&nLeft,int& nRight);
	void AddPage(CPage*pPage);
	void RemovePage(CPage *pPage);
	CPage *GetCurPage();
	bool SetCurPage(CPage *pPage);
	CPage *GetAvailablePage(int *pnIndex =NULL);//�õ�һ�����õ�ҳ
	bool MoveToEmptyArea(int nIndex);
	bool MoveToWorkArea(CPage *pPage);

	CPage *GetPageAt(int nIndex);//�õ�nIndex����Ӧ��ҳ
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
