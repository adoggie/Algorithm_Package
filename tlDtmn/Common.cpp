// Common.cpp: implementation of the CCommon class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Common.h"
#include <assert.h>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommon::CCommon()
{

}

CCommon::~CCommon()
{

}
/*
//�Ƚ������ֶ���ָ���ֵ,ǰ��������ͬ
���ؽ��:
A<B ʱ, <0
A=B ʱ, 0
A>B ʱ, >0
//#_S 2003-9-10 $ 14 zlq
*/
int CCommon::CompareFieldValue(CField *pFieldA, CField *pFieldB)
{
	if (pFieldA->IsNull())
	{//AΪ��ֵʱ
		if (pFieldB->IsNull())
			return 0;
		else
			return -1;
	}

	if (pFieldB->IsNull())
	{//A>B  ���ڿ�ֵ
		return 1;
	}

	//���Ƿǿ�ֵ���
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
/*
//�Ƚ�������¼��ֵ,����ؼ���
pIndexA:pDAA��Ӧ���ֶ�������
pIndexB:pDAB��Ӧ���ֶ�������(pIndexA�ĸ���Ӧ���ϸ����)
���ؽ��:
A<B ʱ, <0
A=B ʱ, 0
A>B ʱ, >0
//#_S 2003-9-10 $ 14 zlq
*/
int CCommon::CompareRecordValue(CDataAccess *pDAA, CDWordArray *pIndexA, CDataAccess *pDAB,CDWordArray *pIndexB)
{
	int nCount = pIndexA->GetSize();//�ؼ��ֶ�
	for (int i=0; i<nCount ; i++)
	{
		CField *pFieldA = pDAA->m_pFieldList->FieldByIndex(pIndexA->GetAt(i));
		CField *pFieldB = pDAB->m_pFieldList->FieldByIndex(pIndexB->GetAt(i));
		int rst = CompareFieldValue(pFieldA,pFieldB);
		if (rst!=0)
			return rst;
	}
	return 0;//���
}


//�����ֶε�ֵ,ǰ��������ͬ
void CCommon::CopyFieldValue(CField *pFieldSource, CField *pFieldOut)
{
	if (pFieldSource->IsNull())
	{//Ϊ��ֵʱ
		pFieldOut->SetNull();
		return;
	}


	//���Ƿǿ�ֵ���
	assert(pFieldSource->GetFieldType()==pFieldOut->GetFieldType());
	switch(pFieldSource->GetFieldType())
	{
	case fDate:
	case fCurrency:
	case fDouble:
		{
			double valueA = pFieldSource->GetAsDouble();
			pFieldOut->SetAsDouble(valueA);
			break;
		}
	case fBoolean:
	case fInt:
		{
			int valueA = pFieldSource->GetAsInteger();
			pFieldOut->SetAsInteger(valueA);
			break;
		}
	case fString:
		{
			CTString valueA = pFieldSource->GetAsString();
			pFieldOut->SetAsString(valueA);
			break;
		}
	case fBlob:
		{
		}
	}
}

CCommon::CItem::CItem()
{
	m_ftType = fDouble;
	*((double*)&m_buf) = 0;
	m_bIsNull = true;
}
CCommon::CItem::~CItem()
{
	switch(m_ftType)
	{
	case fDouble:
		{
			break;
		}
	case fInt:
		{
			break;
		}
	case fString:
		{
			CTString *pStr = *(CTString**)&m_buf;
			delete pStr;
			break;
		}
	case fDate:
		{
			break;
		}
	case fCurrency:
		{
			break;
		}
	case fBoolean:
		{
			break;
		}
	case fBlob:
		{
			break;
		}
	}

}

void CCommon::CItem::SetType(CFieldType ftType)
{
	if (ftType == fString)
	{
		if (m_ftType == fString)
		{
		}
		else
		{
			*((CTString** )&m_buf) = new CTString;
		}
	}
	else
	{
		if (m_ftType == fString)
		{
			delete *((CTString** )&m_buf);
		}
	}
	m_ftType = ftType;

}

void CCommon::CItem::SetNull()
{
	m_bIsNull = true;
}

bool CCommon::CItem::IsNull()
{
	return m_bIsNull;
}

bool CCommon::CItem::SetAsDouble(double value)
{
	return false;
}
/*
�ж�pField�Ƿ���double ���� ���� ��������
*/
bool CCommon::IsNumField(CField *pField)
{
	if (pField->GetFieldType()== fDouble ||pField->GetFieldType()== fInt || pField->GetFieldType()== fCurrency) 
		return true;
	else
		return false;

}
/*
������¼ָ���Ƚ���ŵĲ���ȼ�
����  ���������� - ��һ������ͬ��λ�ú�(0Ϊ��׼).  
������ͬ,�򷵻�0

pIndexA:pDAA��Ӧ���ֶ�������  ���Ϊ�������,���εݼ�
(pIndexA��pIndexB������ͬ)
��pIndexA��pIndexB��Ӧ���ֶ�,������ͬ,�򷵻�0
��pIndexA�����һ����ͬ,������ͬ,�򷵻�1
��pIndexA�ĵ�һ����ͬ,���಻��,�򷵻��ܸ���
//#_S 2003-9-10 $ 17 zlq
*/
int CCommon::RecordDiffLevel(CDataAccess *pDAA, CDWordArray *pIndexA, CDataAccess *pDAB, CDWordArray *pIndexB)
{
	int nCount = pIndexA->GetSize();//�ؼ��ֶ�
	for (int i=0; i<nCount ; i++)
	{
		CField *pFieldA = pDAA->m_pFieldList->FieldByIndex(pIndexA->GetAt(i));
		CField *pFieldB = pDAB->m_pFieldList->FieldByIndex(pIndexB->GetAt(i));
		int rst = CompareFieldValue(pFieldA,pFieldB);
		if (rst!=0)
			return nCount -i ;
	}
	return 0;//���
}

/*
*�õ��ַ�������ֵ���������Ӧ��ֵ��ǩ������ʾ��Ӧ�ı�ǩֵ
*#_S 2004-9-17 $ 10 zlq �޸�
*/
CTString CCommon::GetValueLabel(CField *pField ,int dataType,TDataSet *ptempdata,CDataInterface *pDI)
{
	CTString str;
	if (dataType==fString)
		str.Format("%s",ptempdata->strVal);
	else{
		if (dataType==fInt)
			str.Format("%d",(int)ptempdata->dValue);
		if (dataType==fDouble)
			str.Format("%f",ptempdata->dValue);
		
	}
	/*
	if (pDI)
	{
		if (pDI->GetShowValueLabel()==false)
			return str;
	}*/
	
	CTString labelVal;
	str.TrimLeft();
	str.TrimRight();
	if (dataType==fString)//ͨ��׼ȷ�Ĳ��������ǰ��ģ������
	{									
		if(pField->GetValueVL(str,labelVal))
		{
			str=labelVal;
		}
	}
	else{
		if (dataType==fInt)
		{
			if(pField->GetValueVL((int)ptempdata->dValue,labelVal))
			{
				str=labelVal;
			}
		}
		if (dataType==fDouble)
		{
			if(pField->GetValueVL((int)ptempdata->dValue,labelVal))
			{
				str=labelVal;
			}
		}
	}
	
	return str;
}