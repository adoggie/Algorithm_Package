#ifndef TCALLOCATE_H
#define TCALLOCATE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Defines.h"
#include <stdio.h>
#include <vector>
#include "../Public/TString.h"
#include "../Public/System.h"


using namespace std;
#include <assert.h>

//#define  MEMORY_SIZE 2048
template<class Type>
class AFX_EXT_CLASS CAllocate  
{
	struct Page{
		Type *m_pData ;
		bool m_bModify;//缓冲区是否被修改
		int m_nStartPos;
		int m_nLen;//不一定与数组长度一致
		//返回距离  | 测试距离-页距离 |  0则在页内 
		int Distance(int nPos)
		{
			if (nPos<m_nStartPos)
				return nPos - m_nStartPos ;
			if (nPos >=m_nStartPos+m_nLen)
				return nPos - (m_nStartPos + m_nLen-1);
			
			return 0;
		}
		
	};

	CAllocate *m_pParent;
	int m_nSubFrom;
	int m_nSubSize;
	int m_nSubFre;

	typedef	vector<Page*> TCPageAry;
	TCPageAry m_pgs;
	
	int m_nVisitNum;//访问次数
	int m_nConflict;
	int m_nMaxConflictPer;//最大冲撞率 
	
	int m_nPageAdjustType;//页面调节方法 0 最近原则 1 最久没有使用 ,2 使用频数
	
	int m_nTotalSize;
	FILE *m_pFile;
	int m_nMaxPages;//最大页面数
	
	int m_nMaxMemsize;//如果都放在内存中时,内存中最大记录条数
	int m_nBufsize;//缓存区,已经存在文件里  默认10000 * 10
	public:
		bool PageAdjustType(int nType)//设置页面调度类型 0 最近原则 1 最久没有使用
		{
			if (m_pParent)
			{
				return false;
			}

			if (nType ==0 || nType ==1)
			{
				m_nPageAdjustType = nType;
				return true;
			}
			else
				return false;
		}
		
		void SetMaxPages(int nPages)
		{
			if (m_pParent)
			{
				return ;
			}

			if (nPages<=0)
				return;
			m_nMaxPages = nPages;
		}
		
		bool SetAt(int nPos, Type dc)
		{
			if (m_pParent)
			{
				return m_pParent->SetAt(nPos+m_nSubFrom,dc);
			}

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
				* (m_pgs.at(0)->m_pData+nPos) =dc;
			
			return true;
		}
		
		
		Type GetAt(int nPos)const 
		{
			if (m_pParent)
			{
				return m_pParent->GetAt(nPos+m_nSubFrom);
			}

			if (nPos <0 || nPos>= m_nTotalSize)
				return 0;
			
			
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
				return *(m_pgs.at(0)->m_pData+nPos);
			
			
		}

		CAllocate & GetSub(int nFrom,int nSize)
		{
			CAllocate *pSub = new CAllocate;
			if (nSize<=0 || nFrom<0)
				return *pSub;
			if (nFrom+nSize>GetSize())
				return *pSub;

			pSub->SetParent(this);
			pSub->m_nSubFrom = nFrom;
			pSub->m_nSubSize = nSize;
			return *pSub;
		}
		
		int GetSize()const //得到数组长度
		{
			if (m_pParent)
			{
				return m_nSubSize;
			}

			return m_nTotalSize;
		}
		
		int GetCellSize()
		{
			if (m_pFile!=NULL)
				return m_nBufsize*m_pgs.size();
			return m_nTotalSize;
		}
		
		int GetCellCount()
		{
			if (m_pFile != NULL)
				return (m_nTotalSize+GetCellSize()-1)/GetCellSize();
			return 1;
		}
		
		void Create(int nSize)//设定数组大小
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
				
				CTString strFile = GetTempFile();
				m_pFile = fopen((char *)strFile,"w+");
			}
		}

		Type &operator()(int nPos)
		{
			//if (nPos <0 || nPos>= m_nTotalSize)
			//	return false;
			if (m_pParent)
			{
				return (*m_pParent)(nPos+m_nSubFrom);
			}
			
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
				return *(m_pgs.at(0)->m_pData+nPos) ;
			
			//return true;
		}

		CAllocate& operator = (const CAllocate& Sour)
		{
			if (this != &Sour)
			{
				Initialize();
				Copy(Sour);
			}
			return *this;
		}
		
//===temp  
		void Destroy()
		{
		}
		void SetMemMaxSize(int nn)
		{
		}
		//复制函数,从sour 复制到本类
		void Copy(const CAllocate &Sour)
		{
			Clear();
			Create(Sour.GetSize());
			
			for (int i=0;i<m_nTotalSize;i++)
			{
				SetAt(i,Sour.GetAt(i));
			}
		}

		void Create(int nSize,Type kkk)//设定数组大小
		{
			Create(nSize);
			for (int i=0;i<nSize;i++)
				SetAt(i,kkk);
		}

		void Resize(int nSize)
		{
			if (nSize <= m_nTotalSize)
				m_nTotalSize = nSize;
		}
		//指定缓冲区大小,和所有在内存里时数组的最大尺寸
		//CAllocate(int nBufsize = 10000 * 10,int nMaxMemsize = 10000*50,int nConflictPer =10);
		CAllocate(int nSize ,Type inivalue =0)
		{
			Initialize();
			Create(nSize,inivalue);	
		}



		CAllocate()
		{
			Initialize();
		}
		CAllocate(CAllocate&Sour)
		{
			if (this != &Sour)
			{
				Initialize();
				Copy(Sour);
			}
		}
		
		virtual ~CAllocate() {   Clear();     }
private:
	void Initialize()
	{
			static int nBufsize = 10000 * 10;
			static int nMaxMemsize = 10000*50;
			static int nConflictPer =10;
			static int nMaxPages=2;
			static int nPageAdjustType = 1;
			static bool bRead = false;
			if (bRead ==false)
			{
				int nTemp =0;
				if (GetProfileIntValue("ArraySetup","MaxMemsize",nTemp))
					nMaxMemsize = nTemp;
				if (GetProfileIntValue("ArraySetup","Bufsize",nTemp))
					nBufsize = nTemp;
				if (GetProfileIntValue("ArraySetup","MaxPages",nTemp))
					nMaxPages = nTemp;
				if (GetProfileIntValue("ArraySetup","ConflictPer",nTemp))
					nConflictPer = nTemp;
				if (GetProfileIntValue("ArraySetup","PageAdjustType",nTemp))
					nPageAdjustType = nTemp;
			}
			
			m_pParent = NULL;
			bRead = true;
			
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
	void SetParent(CAllocate *pParent)
	{
		Clear();
		m_pParent = pParent;
	}

	bool Load(Page*pPage,int nStartPos,int nLen)
	{
		int nItemLen = sizeof (Type);
		if (pPage->m_bModify)
		{
			fseek(m_pFile,pPage->m_nStartPos*nItemLen,SEEK_SET);
			fwrite(pPage->m_pData,sizeof(char),pPage->m_nLen*nItemLen,m_pFile);
		}
		fseek(m_pFile,nStartPos*nItemLen,SEEK_SET);
		fread(pPage->m_pData,sizeof(char),nItemLen*nLen,m_pFile);
		pPage->m_nStartPos = nStartPos;
		pPage->m_nLen = nLen;
		pPage->m_bModify = false;
		
		return true;
	}

	void Clear()
	{
		m_nTotalSize = 0;
		
		for (int i=0;i<m_pgs.size();i++)
		{
			delete [] m_pgs.at(i)->m_pData;
			delete  m_pgs.at(i);
		}
		m_pgs.clear();
		
		
		if(m_pFile)
		{
			fclose(m_pFile);
			m_pFile=0;
		}
		m_pParent = NULL;
	}		


	CTString GetTempFile()
	{
		CTString  szFileName;
		
		//取得系统临时路径.
		CTString szTempPath;
		bool dwResult = TCSystem::GetTempPath(szTempPath);
		//取得唯一的文件名
		TCSystem::GetTempFileName(szTempPath, ("~VTemp"),szFileName);

		return szFileName;
	}		
	
private:
	Page* GetPage0(int nPos)
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
			int nMinDistance = abs(m_pgs.at(0)->Distance(nPos));
			int nMinIndex = 0;//如果没有找到时,附近最近的页的索引号
			if (nMinDistance ==0)
				pPage = m_pgs.at(0);
			
			int nLeftIdle=nPos+1;//不在磁盘的最小范围 (包括当前元素)
			int nRightIdle = m_nTotalSize - nPos ;//最大范围(包括当前元素)
			
			for (int i=0;i<m_pgs.size();i++)
			{
				int nTemp = m_pgs.at(i)->Distance(nPos);
				int nAbsoluteTemp =abs(nTemp); 
				if (nTemp ==0)
				{//如果已经找到页,则
					pPage = m_pgs.at(i);
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
				{//小于最大页数,申请新的
					pPage = NewPage(nLen);
				}
				else
				{//根据最近原则找到可用页
					pPage = m_pgs.at(nMinIndex);
				}
				
				
				Load(pPage,nStart,nLen);
			}
			
			return pPage;
		}
		return NULL;
	}		


	Page* GetPage1(int nPos)
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
			
			int nLeftIdle=nPos+1;//不在磁盘的最小范围 (包括当前元素)
			int nRightIdle = m_nTotalSize - nPos ;//最大范围(包括当前元素)
			
			for (int i=0;i<m_pgs.size();i++)
			{
				int nTemp = m_pgs.at(i)->Distance(nPos);
				int nAbsoluteTemp =abs(nTemp); 
				if (nTemp ==0)
				{//如果已经找到页,则
					pPage = m_pgs.at(i);
					//移动到最前
					if (i!=0)
					{
						m_pgs.erase(m_pgs.begin()+i);
						//m_pgs.insert(0,pPage);
						m_pgs.insert(m_pgs.begin(),pPage);
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
				{//小于最大页数,申请新的
					pPage = NewPage(nLen);
				}
				else
				{//根据最久没有使用原则找到可用页
					pPage = m_pgs.at(m_pgs.size()-1);
				}
				
				Load(pPage,nStart,nLen);
				//移动到最前
				m_pgs.erase(m_pgs.begin()+m_pgs.size()-1);
				//m_pgs.insert(0,pPage);
				m_pgs.insert(m_pgs.begin(),pPage);
			}
			
			return pPage;
		}
		return NULL;
	}
	

	Page* NewPage(int nLen)
	{
		Page *	pPage = new Page;
		pPage->m_pData= new Type[m_nBufsize];
		pPage->m_nLen = nLen;
		pPage->m_nStartPos = 0;
		pPage->m_bModify = false;
		m_pgs.push_back(pPage);
		return pPage;
	}
	
	
	/***********************************************
	得到markway.ini文件的配置信息
	zlq
	***********************************************/
	int GetProfileIntValue(CTString strSection, CTString strSectionKey, int &value)
	{
		CTString strPath;
		TCSystem::GetModulePath(strPath);
		strPath += "Markway.ini";
		int rst = TCSystem::GetProfileIntValue (strSection,strSectionKey,value); 
		return rst;
	}

#ifdef WINPLAT	
	//-- not used
	void GetPath(CTString& modulPath)
	{
		TCSystem::GetModulePath(modulPath);
	}
#endif
};

#endif // TCALLOCATE_H

