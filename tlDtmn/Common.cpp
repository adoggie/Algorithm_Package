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
//比较两个字段所指向的值,前提类型相同
返回结果:
A<B 时, <0
A=B 时, 0
A>B 时, >0
//#_S 2003-9-10 $ 14 zlq
*/
int CCommon::CompareFieldValue(CField *pFieldA, CField *pFieldB)
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
/*
//比较两个记录的值,多个关键字
pIndexA:pDAA对应的字段索引号
pIndexB:pDAB对应的字段索引号(pIndexA的个数应该严格相等)
返回结果:
A<B 时, <0
A=B 时, 0
A>B 时, >0
//#_S 2003-9-10 $ 14 zlq
*/
int CCommon::CompareRecordValue(CDataAccess *pDAA, CDWordArray *pIndexA, CDataAccess *pDAB,CDWordArray *pIndexB)
{
	int nCount = pIndexA->GetSize();//关键字段
	for (int i=0; i<nCount ; i++)
	{
		CField *pFieldA = pDAA->m_pFieldList->FieldByIndex(pIndexA->GetAt(i));
		CField *pFieldB = pDAB->m_pFieldList->FieldByIndex(pIndexB->GetAt(i));
		int rst = CompareFieldValue(pFieldA,pFieldB);
		if (rst!=0)
			return rst;
	}
	return 0;//相等
}


//拷贝字段的值,前提类型相同
void CCommon::CopyFieldValue(CField *pFieldSource, CField *pFieldOut)
{
	if (pFieldSource->IsNull())
	{//为空值时
		pFieldOut->SetNull();
		return;
	}


	//都是非空值情况
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
判断pField是否是double 或者 整型 货币类型
*/
bool CCommon::IsNumField(CField *pField)
{
	if (pField->GetFieldType()== fDouble ||pField->GetFieldType()== fInt || pField->GetFieldType()== fCurrency) 
		return true;
	else
		return false;

}
/*
两条记录指定比较序号的差异等级
返回  总索引个数 - 第一个不相同的位置号(0为基准).  
所有相同,则返回0

pIndexA:pDAA对应的字段索引号  序号为零大索引,依次递减
(pIndexA和pIndexB个数相同)
如pIndexA和pIndexB对应的字段,所有相同,则返回0
如pIndexA的最后一个不同,其余相同,则返回1
如pIndexA的第一个不同,其余不管,则返回总个数
//#_S 2003-9-10 $ 17 zlq
*/
int CCommon::RecordDiffLevel(CDataAccess *pDAA, CDWordArray *pIndexA, CDataAccess *pDAB, CDWordArray *pIndexB)
{
	int nCount = pIndexA->GetSize();//关键字段
	for (int i=0; i<nCount ; i++)
	{
		CField *pFieldA = pDAA->m_pFieldList->FieldByIndex(pIndexA->GetAt(i));
		CField *pFieldB = pDAB->m_pFieldList->FieldByIndex(pIndexB->GetAt(i));
		int rst = CompareFieldValue(pFieldA,pFieldB);
		if (rst!=0)
			return nCount -i ;
	}
	return 0;//相等
}

/*
*得到字符描述的值，如果有相应的值标签，则显示对应的标签值
*#_S 2004-9-17 $ 10 zlq 修改
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
	if (dataType==fString)//通过准确的查找替代以前的模糊查找
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