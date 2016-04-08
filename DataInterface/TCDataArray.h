// TCDataArray.h: interface for the CTCDataArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCDATAARRAY_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_)
#define AFX_TCDATAARRAY_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
#include "../Public/TCLFile.h"
#include "../Public/System.h"

#include  <math.h>
//#_S 2005-4-30 zlq  ֧�ִ����ݵ�����
template<class Type>
class TCDataArray  
{
	struct Page{
		Type *m_pData ;
		bool m_bModify;//�������Ƿ��޸�
		int m_nStartPos;
		int m_nLen;//��һ�������鳤��һ��
		//���ؾ���  | ���Ծ���-ҳ���� |  0����ҳ�� 
		int Distance(int nPos)
		{
			if (nPos<m_nStartPos)
				return nPos - m_nStartPos ;
			if (nPos >=m_nStartPos+m_nLen)
				return nPos - (m_nStartPos + m_nLen-1);
			
			return 0;
		}
		
	};

	typedef	 std::vector<Page*> TCPageAry;
	
	TCPageAry m_pgs;

	int m_nVisitNum;//���ʴ���
	int m_nConflict;
	int m_nMaxConflictPer;//����ײ�� 

	int m_nPageAdjustType;//ҳ����ڷ��� 0 ���ԭ�� 1 ���û��ʹ�� ,2 ʹ��Ƶ��

	int m_nTotalSize;
	TCLFile *m_pFile;
	int m_nMaxPages;//���ҳ����

	int m_nMaxMemsize;//����������ڴ���ʱ,�ڴ�������¼����
	int m_nBufsize;//������,�Ѿ������ļ���  Ĭ��10000 * 10
public:
	bool PageAdjustType(int nType);//����ҳ��������� 0 ���ԭ�� 1 ���û��ʹ��
	void SetMaxPages(int nPages);
	bool SetAt(int nPos, Type dc);
	void Clear();
	Type GetAt(int nPos);
	int GetLen();//�õ����鳤��
	void SetSize(int nSize);//�趨�����С
	//ָ����������С,���������ڴ���ʱ��������ߴ�
	//TCDataArray(int nBufsize = 10000 * 10,int nMaxMemsize = 10000*50,int nConflictPer =10);
	TCDataArray(int nBufsize ,int nMaxMemsize = 10000*200,int nConflictPer =10);
	TCDataArray( );
	virtual ~TCDataArray();
		Type &operator[](int nPos)
		{
			//if (nPos <0 || nPos>= m_nTotalSize)
			//	return false;

			
			if (m_pFile)
			{
				Page *pPage ;
				if (m_nPageAdjustType ==1)
					pPage = GetPage1(nPos);
				else 
					pPage = GetPage0(nPos);
				
				pPage->m_bModify = true;
				return * (pPage->m_pData+nPos - pPage->m_nStartPos) ;
				
			}
			else
				return *(m_pgs[0]->m_pData+nPos) ;
			
			//return true;
		}
protected:
	bool Load(Page*pPage,int nStartPos,int nLen);
	
	CTString GetTempFile();

private:
	Page* GetPage0(int nPos);
	Page* GetPage1(int nPos);
	Page* NewPage(int nLen);


};



template<class Type>
TCDataArray<Type>::TCDataArray()
{
	static int nBufsize = 10000 * 10;
	static int nMaxMemsize = 10000*50;
	static int nConflictPer =10;
	static int nMaxPages=2;
	static int nPageAdjustType = 1;


	
	m_nVisitNum = 0;
	m_nConflict = 0;
	
	m_nBufsize = nBufsize;
	m_nMaxMemsize = nMaxMemsize;
	m_nMaxConflictPer = nConflictPer;
	if (nBufsize<=0)
		m_nBufsize = 100;
	if (nMaxMemsize <=0)
		m_nMaxMemsize = 100;
	if (nConflictPer<=0)
		m_nMaxConflictPer = 100;
	m_nMaxPages = 2;
	SetMaxPages(nMaxPages);
	
	m_nTotalSize = 0;
	m_pFile = NULL;

	m_nPageAdjustType = 1;
	PageAdjustType(nPageAdjustType);
}

template<class Type>
TCDataArray<Type>::TCDataArray(int nBufsize ,int nMaxMemsize,int nConflictPer)
{

	m_nVisitNum = 0;
	m_nConflict = 0;
	
	
	m_nBufsize = nBufsize;
	m_nMaxMemsize = nMaxMemsize;
	m_nMaxConflictPer = nConflictPer;
	if (nBufsize<=0)
		m_nBufsize = 100;
	if (nMaxMemsize <=0)
		m_nMaxMemsize = 100;
	if (nConflictPer<=0)
		m_nMaxConflictPer = 100;

	m_nMaxPages = 2;
	m_nTotalSize = 0;
	m_pFile = NULL;

	m_nPageAdjustType = 1;
}

template<class Type>
TCDataArray<Type>::~TCDataArray()
{
	Clear();
}

template<class Type>
CTString TCDataArray<Type>::GetTempFile()
{
/*===uc
    	TCHAR  szFileName[256];
    
    	//ȡ��ϵͳ��ʱ·��.
        TCHAR szTempPath[MAX_PATH];
        DWORD dwResult = ::GetTempPath(MAX_PATH, szTempPath);
        ASSERT(dwResult);
        //ȡ��Ψһ���ļ���
        UINT nResult = GetTempFileName(szTempPath, _T("~TLVTemp"),0,szFileName);
        CString str = szFileName;
		return str;
	*/
	CTString szFileName = "c:/temp.tmp";
	return szFileName;
}


template<class Type>
void TCDataArray<Type>::SetSize(int nSize)
{
	if (nSize <=0)
		return ;
	Clear();
	m_nTotalSize = nSize;
	
	if (nSize<=m_nMaxMemsize)
	{
		int nOld = m_nBufsize ;
		m_nBufsize = nSize;
		NewPage( nSize);
		m_nBufsize  = nOld;
	}

	if (m_pgs.size()==0)
	{
		NewPage(m_nBufsize);

		std::string strFile = GetTempFile().GetData();
		m_pFile = new TCLFile();//(strFile,CFile::modeCreate|CFile::modeReadWrite|CFile::typeBinary);
		m_pFile->Open(strFile.c_str());
	}
}


template<class Type>
void TCDataArray<Type>::Clear()
{
	m_nTotalSize = 0;

	for (int i=0;i<m_pgs.size();i++)
	{
		delete [] m_pgs[i]->m_pData;
		delete  m_pgs[i];
	}
	//m_pgs.RemoveAll();
	m_pgs.clear();


	if(m_pFile)
	{
		std::string filename = m_pFile->GetFileName().GetData();
		delete 	m_pFile;
		//DeleteFile(filename.c_str());  ===uc
		m_pFile=0;
	}
} 

template<class Type>
int TCDataArray<Type>::GetLen()
{
	return m_nTotalSize;
}

template<class Type>
Type TCDataArray<Type>::GetAt(int nPos)
{
	Type dc;
	if (nPos <0 || nPos>= m_nTotalSize)
		return dc;

	
	if (m_pFile)
	{
		Page *pPage ;
		if (m_nPageAdjustType ==1)
			pPage = GetPage1(nPos);
		else 
			pPage = GetPage0(nPos);
		return *(pPage->m_pData+nPos-pPage->m_nStartPos);
	}
	else
		return *(m_pgs[0]->m_pData+nPos);

	
}

template<class Type>
bool TCDataArray<Type>::SetAt(int nPos, Type dc)
{
	if (nPos <0 || nPos>= m_nTotalSize)
		return false;

	if (m_pFile)
	{
		Page *pPage ;
		if (m_nPageAdjustType ==1)
			pPage = GetPage1(nPos);
		else 
			pPage = GetPage0(nPos);
		* (pPage->m_pData+nPos - pPage->m_nStartPos) =dc;
		pPage->m_bModify = true;
	}
	else
		* (m_pgs[0]->m_pData+nPos) =dc;
	
	return true;
}

template<class Type>
bool TCDataArray<Type>::Load(Page*pPage,int nStartPos,int nLen)
{

	int nItemLen = sizeof (Type);
	if (pPage->m_bModify)
	{
		m_pFile->Seek(pPage->m_nStartPos*nItemLen,TCLFile::tBegin);
		m_pFile->Write(pPage->m_pData,  nItemLen *pPage->m_nLen);
	}
	m_pFile->Seek(nStartPos*nItemLen,TCLFile::tBegin);
	m_pFile->Read(pPage->m_pData, nItemLen*nLen);
	pPage->m_nStartPos = nStartPos;
	pPage->m_nLen = nLen;
	pPage->m_bModify = false;

	return true;
}

//��������󳤶�Ϊm_nBufsize

template<class Type>
TCDataArray<Type>::Page* TCDataArray<Type>::NewPage(int nLen)
{
	Page *	pPage = new Page;
	pPage->m_pData= new Type[m_nBufsize];
	pPage->m_nLen = nLen;
	pPage->m_nStartPos = 0;
	pPage->m_bModify = false;
	//m_pgs.Add(pPage);
	m_pgs.push_back(pPage);
	return pPage;
}


template<class Type>
TCDataArray<Type>::Page* TCDataArray<Type>::GetPage0(int nPos)
{
	if (m_pFile)
	{
		m_nVisitNum ++;
		if (m_nVisitNum >=100)
		{
			if (m_nConflict*100/m_nVisitNum >m_nMaxConflictPer)
			{
				if (m_nMaxPages<6)
				{
					m_nMaxPages++;
				}
			}
			m_nVisitNum = 0;
			m_nConflict = 0;	
		}

		Page *pPage = NULL;
		int nMinDistance = abs(m_pgs[0]->Distance(nPos));
		int nMinIndex = 0;//���û���ҵ�ʱ,���������ҳ��������
		if (nMinDistance ==0)
			pPage = m_pgs[0];
		
		int nLeftIdle=nPos+1;//���ڴ��̵���С��Χ (������ǰԪ��)
		int nRightIdle = m_nTotalSize - nPos ;//���Χ(������ǰԪ��)

		for (int i=0;i<m_pgs.size();i++)
		{
			int nTemp = m_pgs[i]->Distance(nPos);
			int nAbsoluteTemp =abs(nTemp); 
			if (nTemp ==0)
			{//����Ѿ��ҵ�ҳ,��
				pPage = m_pgs[i];
				break;
			}

			if (nAbsoluteTemp <nMinDistance)
			{
				nMinDistance = nAbsoluteTemp;
				nMinIndex = i;
			}

			if (nTemp<0 && nRightIdle > nAbsoluteTemp)
				nRightIdle = nAbsoluteTemp ;
			if (nTemp>0 && nLeftIdle  > nAbsoluteTemp)
				nLeftIdle = nAbsoluteTemp;
		}

		if (pPage ==NULL)
		{
			m_nConflict ++;

			int nStart =nPos-nLeftIdle+1;
			int nLen = nRightIdle+nLeftIdle-1;
			if (nLeftIdle >=m_nBufsize )
			{
				nStart = nPos - m_nBufsize+1;
				nLen = nPos-nStart+nRightIdle;
			}

			if (nLen > m_nBufsize)
				nLen = m_nBufsize;

			
			if (m_pgs.size()<m_nMaxPages)
			{//С�����ҳ��,�����µ�
				pPage = NewPage(nLen);
			}
			else
			{//�������ԭ���ҵ�����ҳ
				pPage = m_pgs[nMinIndex];
			}


			Load(pPage,nStart,nLen);
		}

		return pPage;
	}
	return NULL;
}

template<class Type>
TCDataArray<Type>::Page* TCDataArray<Type>::GetPage1(int nPos)
{
	if (m_pFile)
	{
		m_nVisitNum ++;
		if (m_nVisitNum >=100)
		{
			if (m_nConflict*100/m_nVisitNum >m_nMaxConflictPer)
			{
				if (m_nMaxPages<6)
				{
					m_nMaxPages++;
					//TRACE(">>>>>>>>>>> ��ͻ��:%d ҳ��Ϊ:%d\r\n",m_nConflict,m_nMaxPages);
				}
			}
			//TRACE("��ͻ��:%d ҳΪ:%d\r\n",m_nConflict,m_nMaxPages);
			if (m_nConflict >10)
				int a=1;
			m_nVisitNum = 0;
			m_nConflict = 0;	
		}

		Page *pPage = NULL;

		int nLeftIdle=nPos+1;//���ڴ��̵���С��Χ (������ǰԪ��)
		int nRightIdle = m_nTotalSize - nPos ;//���Χ(������ǰԪ��)

		for (int i=0;i<m_pgs.size();i++)
		{
			int nTemp = m_pgs[i]->Distance(nPos);
			int nAbsoluteTemp =abs(nTemp); 
			if (nTemp ==0)
			{//����Ѿ��ҵ�ҳ,��
				pPage = m_pgs[i];
				//�ƶ�����ǰ
				if (i!=0)
				{
					m_pgs.erase(m_pgs.begin()+i);
					m_pgs.insert(m_pgs.begin(),pPage);
					//m_pgs.RemoveAt(i);
					//m_pgs.InsertAt(0,pPage);
				}
				break;
			}


			if (nTemp<0 && nRightIdle > nAbsoluteTemp)
				nRightIdle = nAbsoluteTemp ;
			if (nTemp>0 && nLeftIdle  > nAbsoluteTemp)
				nLeftIdle = nAbsoluteTemp;
		}

		if (pPage ==NULL)
		{
			m_nConflict ++;

			int nStart =nPos-nLeftIdle+1;
			int nLen = nRightIdle+nLeftIdle-1;
			if (nLeftIdle >=m_nBufsize )
			{
				nStart = nPos - m_nBufsize+1;
				nLen = nPos-nStart+nRightIdle;
			}

			if (nLen > m_nBufsize)
				nLen = m_nBufsize;

			
			if (m_pgs.size()<m_nMaxPages)
			{//С�����ҳ��,�����µ�
				pPage = NewPage(nLen);
			}
			else
			{//�������û��ʹ��ԭ���ҵ�����ҳ
				pPage = m_pgs[m_pgs.size()-1];
			}

			Load(pPage,nStart,nLen);
			//�ƶ�����ǰ
			m_pgs.erase(m_pgs.end());
			m_pgs.insert(m_pgs.begin(),pPage);
			//m_pgs.RemoveAt(m_pgs.size()-1);
			//m_pgs.InsertAt(0,pPage);
		}

		return pPage;
	}
	return NULL;
}

template<class Type>
void TCDataArray<Type>::SetMaxPages(int nPages)
{
	if (nPages<=0)
		return;
	m_nMaxPages = nPages;
}

//����ҳ���������
template<class Type>
bool TCDataArray<Type>::PageAdjustType(int nType)
{
	if (nType ==0 || nType ==1)
	{
		m_nPageAdjustType = nType;
		return true;
	}
	else
		return false;
}

typedef TCDataArray<TDataCol> TDColAry;
typedef TCDataArray<double> TDoubleAry;
typedef TCDataArray<int> TIntAry;

typedef std::vector<TDColAry*> TCDataMatrix;
#endif // !defined(AFX_TCDATAARRAY_H__2AE9634B_11CA_48D7_9B51_28970418087B__INCLUDED_)
