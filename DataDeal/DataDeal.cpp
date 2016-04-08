// DataDeal.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "../DataInterface/DataInterface.h"
#include <string>
#include "Calc.h"
#include "Sample.h"
#include "Random.h"
#include "FillMissing.h"

#ifndef _UNIX
//using namespace std;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif


void PrintDA(CDataAccess &da)
{
#ifdef _DEBUG
	int nCnt = da.GetRecordCount();
	int nFieldCnt = da.m_pFieldList->GetFieldCount();
	da.First();
	while (!da.Eof() )
	{
		for (int i=0;i<nFieldCnt;i++)
		{
			CField *pFeild = da.m_pFieldList->FieldByIndex(i);
			if (pFeild->IsNull())
				printf(".\t");
			else			
			{
				printf(pFeild->GetAsString());
				//pFeild->PrintValue();
				printf("\t");
			}

		}
		printf("\r\n");
		da.Next();
	}
#endif
}

/*************************************************************
过滤出条件表达式不为零的记录，为零的则删除

pData				:要处理的数据集
sCondition			:条件表达式

返回结果：
true为正常， false格式错误
*************************************************************/
extern "C" __declspec( dllexport ) bool Filter(CDataInterface *pData, LPCSTR  sCondition)
{
	std::string strSc = sCondition;
	CCalc cl;
	if (cl.Analyze(pData,strSc)==false)
	{
		printf("语法格式不对！\r\n");
		return false;
	}

	//遍历每一个记录
	pData->m_DataAccess.First();
	while (! pData->m_DataAccess.Eof())
	{
		double value = cl.Run();
		//printf("%f\r\n",value);
		if (value)
			pData->m_DataAccess.Next();
		else
			pData->m_DataAccess.Delete();
	}

	//PrintDA(pData->m_DataAccess);
	//*/
	return true;
}



/*************************************************************
对条件表达式不为零的记录进行表达式进行计算，结果赋值给目标字段
如果没有目标字段，则创建目标字段

pData				:要处理的数据集
sExpr				:计算表达式
sCondition			:条件表达式
sTargetFieldName	:目标字段名

返回结果：
true为正常， false格式错误
*************************************************************/
extern "C" __declspec( dllexport ) bool Compute(CDataInterface *pData, LPCSTR sExpr,LPCSTR sCondition,LPCSTR sTargetFieldName)
{			 
	CCalc cCond,cExpr;
	if (sCondition)
	{
		if (cCond.Analyze(pData,sCondition)==false)
		{
			printf("条件语句语法格式不对!\r\n");
			return false;
		}
	}

	if (cExpr.Analyze(pData,sExpr)==false)
	{
		printf("计算语法格式不对!\r\n");
		return false;
	}
	//验证
	int nType = cExpr.Type();
	if (nType == FAIL_STD)
		return false;
	else if(nType == NOR_STD || nType == GEN_STD)
	{
		int i = 0;
		CDoubleVector v;
		int nCount = pData->m_DataAccess.GetRecordCount();
		if (nCount < 1)
			return false;
		v.create(nCount);
		pData->m_DataAccess.First();
		while (!pData->m_DataAccess.Eof())
		{
			double dValue = cExpr.Value();
			v(i++) = dValue;
			pData->m_DataAccess.Next();
		}
		//
		cExpr.SetValue(nType, v);
	}
	CField *pField = pData->m_DataAccess.FieldByName(sTargetFieldName);
	if (pField==NULL)
	{//创建一个新字段
		pField = pData->m_DataAccess.m_pFieldList->CreateField(fDouble);
		pData->m_DataAccess.m_pFieldList->Add(pField);
		pField->SetFieldName(sTargetFieldName);
	}

	//遍历每一个记录
	pData->m_DataAccess.First();
	while (! pData->m_DataAccess.Eof())
	{
		if (sCondition)
		{
			double dCond = cCond.Run();
			if (dCond == 0)
			{
				pData->m_DataAccess.Next();
				continue;
			}
		}
		double dValue = cExpr.Run();
		pData->m_DataAccess.Edit();
		pField->SetAsDouble(dValue);

		pData->m_DataAccess.Next();
	}

	//PrintDA(pData->m_DataAccess);
	//*/
	return true;
}

/*************************************************************
数据抽样

  返回结果：
true为正常， false格式错误
*************************************************************/
extern "C" __declspec( dllexport ) bool Sample(CDataInterface *pData, LPCTSTR lpszVO)
{			 
	CSample sp;
	return sp.Run(pData, std::string(lpszVO));
}

/*************************************************************
数据抽样

  返回结果：
true为正常， false格式错误
*************************************************************/
extern "C" __declspec( dllexport ) bool Random(CDataInterface *pData,  LPCTSTR lpszVO)
{			 
	CRandom rd;
	return rd.Run(pData, std::string(lpszVO));
}

/*************************************************************
数据抽样

  返回结果：
true为正常， false格式错误
*************************************************************/
extern "C" __declspec( dllexport ) bool FillMissing(CDataInterface *pData,  LPCTSTR lpszVO)
{			 
	CFillMissing fm;
	return fm.Run(pData, std::string(lpszVO));
}
