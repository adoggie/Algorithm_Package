// Sample.cpp: implementation of the CSample class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Sample.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSample::CSample()
{
	m_fValue = 0;
	m_nType  = 0;
	m_bPercent = false;
}

CSample::~CSample()
{
	m_tVar.DeleteObject();
	m_tLayerVar.DeleteObject();
}

bool CSample::Parse(const string &VO)
{
	AnaWord aw;
	AnaSyntax syn;
	CTString szTemp("");
	
	aw.Import(VO);
	if (AnaSyntax::exist(FETCH, aw))
	{
		szTemp = AnaSyntax::getSingleStr(FETCH, aw);
		szTemp.MakeUpper();
		if (szTemp == ALL)
		{
			m_nType  = A;
			m_fValue = AnaSyntax::getvalue(FETCH, ALL, aw); 
		}
		else if (szTemp == BEF)
		{
			m_nType = B;
			m_fValue = AnaSyntax::getvalue(FETCH, BEF, aw); 
		}
		else if (szTemp == SEP)
		{
			m_nType = S;
			m_fValue = AnaSyntax::getvalue(FETCH, SEP, aw); 
		}
		else if (szTemp == LAYER)
		{
			m_nType = L;
			szTemp = AnaSyntax::getSingleStr(LAYER_, aw); 
			if (szTemp  == PERCENT)
			{
				m_bPercent  = true;
				m_fValue = AnaSyntax::getvalue(LAYER_, PERCENT, aw); 
			}
			else if (szTemp == NUMBER)
			{
				m_bPercent  = false;
				m_fValue = AnaSyntax::getvalue(LAYER_, NUMBER, aw); 
			}
			else
			{
				return false;
			}
			//变量
			CTStringArray szArrName;
			AnaSyntax::getVAR(VARIN,aw,szArrName);
		    if (!m_pData->getVarID(szArrName,m_tVar))
				return false;
			szArrName.RemoveAll();
			AnaSyntax::getVAR(LAYVARIN,aw,szArrName);
			if (!m_pData->getVarID(szArrName,m_tLayerVar))
				return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool CSample::Verify()
{
	return true;
}

bool CSample::Run(CDataInterface *pData, const string &VO)
{
	int nCur=0, nCount=0;

	m_pData = pData;
	if (!m_pData)
		return false;
	if (!Parse(VO))
		return false;
	
	CDataAccess *pDataNew = NULL;
	nCount = pData->m_DataAccess.GetRecordCount();//总记录条数
	if (nCount < 1) 
		return false;
	if (m_nType == A)
	{
		if (m_fValue < 0 || m_fValue >= 1)
			return false;
		Sample_A(&pData->m_DataAccess, pDataNew, m_fValue);
		pDataNew->Swap(&pData->m_DataAccess);
		delete pDataNew;
		pDataNew = NULL;
	}
	else if (m_nType == B)
	{
		if (m_fValue < 0 || m_fValue >= nCount)
			return false;
		int nStartNo = m_fValue;
		Sample_B(&pData->m_DataAccess, nStartNo);
	}
	else if (m_nType == S)
	{
		Sample_S(&pData->m_DataAccess, pDataNew, m_fValue);
		pDataNew->Swap(&pData->m_DataAccess);
		delete pDataNew;
		pDataNew = NULL;
	}
	else if (m_nType == L)
	{
		if (!m_bPercent) //非百分数
		{
			double f1 = m_fValue / nCount;
			double f2 = (m_fValue+1)/nCount;
			m_fValue = f1 + (f2 - f1)/2;
		}
		Sample_L(&pData->m_DataAccess, pDataNew, m_fValue);
		pDataNew->Swap(&pData->m_DataAccess);
		delete pDataNew;
		pDataNew = NULL;
	}
	else 
	{
		return false;
	}
	return true;
}

bool CSample::Sample_A(CDataAccess *pDataIn, CDataAccess *&pDataNew, double fPercent)
{
	int i=0, nTo=0;
	int nCount = pDataIn->GetRecordCount();//总记录条数
	if (fPercent < 0 || fPercent >=1)
		return false;
	nTo = nCount*fPercent;
	//开始抽样
	UINT unStart = (unsigned)time( NULL);
	srand(unStart);
	pDataNew = new CDataAccess(pDataIn);
	CIntVector Sample_Vec_id(nCount, 0);
	for (i=0; i<nCount; i++)
	{
		Sample_Vec_id(i) = i;
	}
	int nCountId= nCount;
	for(i=0; i<nTo; i++)
	{
		int idTemp = rand()*1.0/RAND_MAX*(nCountId-EPS_ACCUR);
		pDataIn->SetCurRecNo(Sample_Vec_id(idTemp)+1);
		pDataNew->Append();
		pDataNew->CopyRecord(pDataNew, pDataIn);
		nCountId--;
		Sample_Vec_id(idTemp) = Sample_Vec_id(nCountId);
	}
	return true;
}

bool CSample::Sample_B(CDataAccess *pDataIn, int nStartNo)
{
	int i=0;
	int nCount = pDataIn->GetRecordCount();
	if (nStartNo < 0 || nStartNo > nCount)
		return false;
	pDataIn->SetCurRecNo(nStartNo); //下一条需要删除
	for (i=0; i<nCount-nStartNo; i++)
	{
		pDataIn->Delete();
	}
	return true;
}

bool CSample::Sample_S(CDataAccess *pDataIn, CDataAccess *&pDataNew, double fPercent)
{
	int nCount = pDataIn->GetRecordCount();
	UINT unStart = (unsigned)time( NULL);
	srand(unStart);
	int nCur = rand() % int(fPercent) + 1;
	pDataNew = new CDataAccess(pDataIn);
	do
	{
		pDataIn->SetCurRecNo(nCur);
		pDataNew->Append();
		pDataNew->CopyRecord(pDataNew, pDataIn);
		
		nCur += m_fValue;
	}while (nCur<=nCount);
	return true;
}

int CSample::IsDiff(CDataAccess *pDAA, CTArray<int,int>arrIndexA, CDataAccess *pDAB, CTArray<int,int>arrIndexB)
{
	int nCount = arrIndexA.GetSize();//关键字段
	for (int i=0; i<nCount ; i++)
	{
		CField *pFieldA = pDAA->m_pFieldList->FieldByIndex(arrIndexA.GetAt(i));
		CField *pFieldB = pDAB->m_pFieldList->FieldByIndex(arrIndexB.GetAt(i));
		int rst = CompareFieldValue(pFieldA,pFieldB);
		if (rst!=0)
			return nCount -i ;
	}
	return 0;//相等
}

int CSample::CompareFieldValue(CField *pFieldA, CField *pFieldB)
{
	if (pFieldA->IsNull())
	{//A为空值时
		if (pFieldB->IsNull())
			return 0;
		else
			return -1;
	}
	
	if (pFieldB->IsNull())
	{//A>B  大于空值
		return 1;
	}
	
	//都是非空值情况
	assert(pFieldA->GetFieldType()==pFieldB->GetFieldType());
	switch(pFieldA->GetFieldType())
	{
	case fDate:
	case fCurrency:
	case fDouble:
		{
			double valueA = pFieldA->GetAsDouble();
			double valueB = pFieldB->GetAsDouble();
			if (fabs(valueA - valueB)<DoubleZero)
				return 0;
			if (valueA>valueB)
				return 1;
			else
				return -1;
			break;
		}
	case fBoolean:
	case fInt:
		{
			int valueA = pFieldA->GetAsInteger();
			int valueB = pFieldB->GetAsInteger();
			if (valueA == valueB)
				return 0;
			if (valueA>valueB)
				return 1;
			else
				return -1;
			break;
		}
	case fString:
		{
			return strcmp(pFieldA->GetAsString(),pFieldB->GetAsString());
			break;
		}
	case fBlob:
		{
		}
	}
	return 0;
}

bool CSample::Sample_L(CDataAccess *pDataIn, CDataAccess *&pDataRet, double dPercent)
{
	//*********************界面控制*****************************
	int nClassCount = m_tLayerVar.iCount;
	//************取得对应的分类字段索引号nSortArray (正负加1)****************
	int nData=0;
	CTArray<int,int> nSortArray;
	int n;
	for( n=0;n<nClassCount;n++)
	{
		nData = m_tLayerVar.pValue[n];
		nSortArray.Add(nData);
	}
	CDataAccess *pSortDA;
	//排序
	pDataIn->MergeSort(pSortDA,nSortArray);
	pDataRet = new CDataAccess(pSortDA);//先完全复制结构
	//将nSortArray恢复为索引号,变正
	for( n=0;n<nClassCount;n++)
	{
		nSortArray.SetAt(n,abs(nSortArray.GetAt(n)));
	}

	//***********************************************************************
	
	CDataAccess DATemp(pSortDA);//用来存放比较值
	pSortDA->First();
	DATemp.Append();
	DATemp.CopyRecord(&DATemp,pSortDA);//临时保存第一条信息
	int firstIndex=0,lastIndex=0;
	int nCur=0;
	CDataAccess* subDA;
	pSortDA->Next();
	while (!pSortDA->Eof())
	{
		int nLevel = IsDiff(pSortDA,nSortArray,&DATemp,nSortArray);
		if ( nLevel == 0 )
		{//完全相同则继续加
			lastIndex++;
		}
		else 
		{//有变化,则输出
			//**********子集抽样并保存***************
			nCur=pSortDA->GetRecNo();

			CDataAccess DATemp1(pSortDA);
			pSortDA->First();

			for (int i=firstIndex;i<=lastIndex;i++)
			{
				pSortDA->SetCurRecNo(i);
				DATemp1.Append();
				DATemp1.CopyRecord(&DATemp1,pSortDA);
			}
			pSortDA->SetCurRecNo(nCur);
			if (Sample_A(&DATemp1,subDA,dPercent))
			{
				subDA->First();
				while( !subDA->Eof())
				{
					pDataRet->Append();
					pDataRet->CopyRecord(pDataRet,subDA);
					subDA->Next();
				}
				if (subDA)
					delete subDA;

				subDA=NULL;
			}
			//*********当前值保存*************
			DATemp.Edit();
			DATemp.CopyRecord(&DATemp,pSortDA);//临时保存当前条信息
			
			firstIndex=nCur;
			lastIndex=nCur;

		}
		pSortDA->Next();
	}
	CDataAccess DATemp1(pSortDA);
	pSortDA->First();
	for (int i=firstIndex;i<=lastIndex;i++)
	{
		pSortDA->SetCurRecNo(i);
		DATemp1.Append();
		DATemp1.CopyRecord(&DATemp1,pSortDA);
	}
	pSortDA->SetCurRecNo(nCur);
	if(Sample_A(&DATemp1, subDA, dPercent))
	{
		subDA->First();
		while( !subDA->Eof())
		{
			pDataRet->Append();
			pDataRet->CopyRecord(pDataRet,subDA);
			subDA->Next();
		}
		if (subDA)
			delete subDA;
		subDA=NULL;
	}
	DATemp.Close();
	pDataRet->m_pFieldList->Delete(0);
	return true;
}