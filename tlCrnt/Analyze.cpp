// Analyze.cpp: implementation of the CAnalyze class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Analyze.h"
#include "DesLayerConf.h"// add by liytao 20030109
#include "FreqConf.h" //add by liytao 20030124
#include "Script.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnalyze::CAnalyze()
{

}

CAnalyze::~CAnalyze()
{

}

//if nType=0, delete info in pTo if some item is not null
void CAnalyze::CopyDesConf(CPtrArray *pFrom, CPtrArray *pTo, CTStringArray* pstrResArray,int nType)
{
	if( pFrom==NULL&&pTo==NULL)
		return ;
	//add by liytao 20030212
	if (pTo==NULL)
		pTo=new CPtrArray;
	//add end
	int i,j;
	CDesLayerConf *pConfTo;
	int nSize=0;
	if (pFrom==NULL)
	{
		pFrom=new CPtrArray;
		nSize=0;
	}
	else
		nSize=pFrom->GetSize();
	if (nSize<=0)
	{
		nSize=pTo->GetSize();
		for (i=0;i<nSize;i++)
		{
			pConfTo=(CDesLayerConf*)pTo->GetAt(i);
			pConfTo->m_FNameList.RemoveAll();
			delete pConfTo;
		}
		pTo->RemoveAll();
		return ;
	}
	int nToSize=pTo->GetSize();
	j=0;
	for (i=0;i<nSize;i++)
	{
		CDesLayerConf *pConfFrom=(CDesLayerConf*)pFrom->GetAt(i);
		// do nothing in the case of invalid config
		if (pConfFrom->m_nLayer<=0||pConfFrom->m_FNameList.GetSize()<=0)
			continue;
		if (j<nToSize)
			pConfTo=(CDesLayerConf*)pTo->GetAt(i);
		else
		{
			pConfTo=new CDesLayerConf; 
			pTo->InsertAt(j,pConfTo);
		}
		if (pConfTo!=NULL&&nType ==0&&pConfFrom->m_FNameList.GetSize()>0)
		{
			pConfTo->m_FNameList.RemoveAll();
			pConfTo->m_FNameList.Copy(pConfFrom->m_FNameList);
			//add by liytao 20030111
			if( pstrResArray!=NULL)
				pstrResArray->Append(pConfFrom->m_FNameList);

			//add end
			pConfTo->m_nLayer=pConfFrom->m_nLayer;
			j++;
		}
	}
	if(nType==0)
	{
		for (i=j;i<nToSize;i++)
		{
			pConfTo=(CDesLayerConf*)pTo->GetAt(i);
			pConfTo->m_FNameList.RemoveAll();
			delete pConfTo;
		}
		if( nToSize>j)
			pTo->RemoveAt(j,nToSize-j);
	}
}

BOOL CAnalyze::GetFNameList(CPtrArray *pConfArray, CTStringArray *pstrNameArray)
{
	CDesLayerConf* pConf;
	if( pConfArray==NULL||pstrNameArray==NULL)
		return FALSE;
	int nSize=pConfArray->GetSize();
	for (int i=0;i<nSize;i++)
	{
		pConf=(CDesLayerConf*)pConfArray->GetAt(i);
		if( pConf!=NULL)
		{
			int nLen=pConf->m_FNameList.GetSize();
			for (int j=0;j<nLen;j++)
				pstrNameArray->Add(pConf->m_FNameList.GetAt(j));
		}
	}
	return TRUE;
}

int CAnalyze::IsExist(CTStringArray* strArray, CTString strName)
{
	if( strArray==NULL)
		return -1;
	int nSize=strArray->GetSize();
	for (int i=0;i<nSize;i++)
		if( strArray->GetAt(i)==strName)
			return i;
	return -1;
}

int CAnalyze::Purify(CPtrArray *pDesConfs)
{
	int i=0;
	int nSize=pDesConfs->GetSize();
	CDesLayerConf *pConf;
	int begin=0,end=nSize,nCount=0;
	while (begin<end)
	{
		pConf=(CDesLayerConf*)pDesConfs->GetAt(begin);
		if (pConf->m_FNameList.GetSize()<=0)
		{
			pConf->Clear();
			delete pConf;
			nCount++;
			pDesConfs->RemoveAt(begin);
			end--;

		}
		else
		{
			begin++;
		}
	}
	return nCount;
}
//add by liytao 20030227
int CAnalyze::Purify(CDataInterface * pDataInterface,CPtrArray *pDesConfs)
{
	int i=0;
	int nSize=pDesConfs->GetSize();
	CDesLayerConf *pConf;
	int begin=0,end=nSize,nCount=0;
	while (begin<end)
	{
		pConf=(CDesLayerConf*)pDesConfs->GetAt(begin);
		int nLen=pConf->m_FNameList.GetSize();
		if (nLen<=0)
		{
			pConf->Clear();
			delete pConf;
			nCount++;
			pDesConfs->RemoveAt(begin);
			end--;
		}
		else
		{
			// check whether data type is valid for calc
			i=nLen-1;
			int nCol=-1;
			while(i>=0)
			{
				nCol=CAnalyze::GetFieldIndex(pDataInterface,pConf->m_FNameList.GetAt(i));
				if (!pDataInterface->FitType(nCol))
				{
					pConf->m_FNameList.RemoveAt(i);
					nLen--;
				}
				i--;
			}
			if (nLen<=0)
			{
				pConf->Clear();
				delete pConf;
				nCount++;
				pDesConfs->RemoveAt(begin);
				end--;
			}
			else
				begin++;
		}
	}
	return nCount;
}


//add end

int CAnalyze::GetFieldIndex(CDataInterface *pDataInterface, CTString strName)
{
	if( pDataInterface==NULL)
		return -1;
	int nSize=pDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
	for (int i=0;i<nSize;i++)
		if( pDataInterface->GetFieldName(i)==strName)
			return i;
	return -1;
}
//add by 
void CAnalyze::CopyFreqConf(CPtrArray *pFrom, CPtrArray * pTo)
{
	if (pFrom==NULL&&pTo==NULL)
		return;
	int nSize;
	if ( pFrom==NULL)
	{
		pFrom=new CPtrArray;
		nSize=0;
	}
	else
		nSize=pFrom->GetSize();
	if( nSize<=0)
	{
		int nCount=pTo->GetSize();
		CFreqConf *pfC=NULL;
		for (int i=0;i<nCount;i++)
		{
			pfC=(CFreqConf *)(pTo->GetAt(i));
			delete pfC;
		}
		pTo->RemoveAll();	
		return;
	}
	if (pTo==NULL)
		pTo=new CPtrArray();
	else
	{
		int nCount=pTo->GetSize();
		CFreqConf *pfC=NULL;
		for (int i=0;i<nCount;i++)
		{
			pfC=(CFreqConf *)(pTo->GetAt(i));
			delete pfC;
		}
		pTo->RemoveAll();	
	}
	nSize=pFrom->GetSize();
	for ( int j=0;j<nSize;j++)
	{
		CFreqConf* pConf,*pFromConf;
		pFromConf=(CFreqConf*)pFrom->GetAt(j);
		pConf=new CFreqConf();
		pConf->m_nFldIndex = pFromConf->m_nFldIndex;
		pConf->m_nRangeOpt = pFromConf->m_nRangeOpt;
		pConf->m_fRangeNum = pFromConf->m_fRangeNum;
		pConf->m_fRangeFrom= pFromConf->m_fRangeFrom;
		pConf->m_fRangeTo  = pFromConf->m_fRangeTo;
		pTo->Add(pConf);
	}
}
/*******************************************************
//从脚本中取出对应参数的字段名对应的序号
psct:传入的脚本
strParam:传入标明各字段所在的参数名
pDataInterface:数据集
tInt:[输出]  输出整型数组
//#_S 2003-8-18 $ 18 zlq
*******************************************************/
bool CAnalyze::GetListData(CScript *psct, CTString strParam, CDataInterface* pDataInterface,TIntStruct &tInt)
{
	int nCount = psct->GetItemCount(strParam);
	if (nCount ==0)
		return false;
	CDataAccess*pDA = &pDataInterface->m_DataAccess;
	int *pValue=new int[nCount];//保存字段索引号数组
	for (int i=0; i<nCount; i++)
	{
		CTString strFieldName ;
		if (!psct->GetItemData(strParam,i,strFieldName))
			return false;
		int nIndex = pDA->m_pFieldList->IndexOf(pDA->m_pFieldList->FieldByName(strFieldName ));
		if (nIndex== -1)
		{
			delete []pValue;
			return false;
		}
			
		pValue[i]= nIndex;//填写字段索引号
	}

	tInt=TIntStruct(nCount,pValue);
	return TRUE;
}

