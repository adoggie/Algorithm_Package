// EnvParam.cpp: implementation of the CEnvParam class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "EnvParam.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
VOBase::VOBase()
{
}

VOBase::~VOBase()
{
}



StructBase::StructBase()
{
}

StructBase::~StructBase()
{
}

//CTString StructBase::Export()//����ΪXML��ʽ
//{
//	return "";
//}
//
//bool StructBase::Import(CTString str)
//{
//	return false;
//}


CEnvParam::CEnvParam()
{

}

CEnvParam::~CEnvParam()
{

}

//******  ָ����Щ������Ҫ���뵽������  *** 
bool CEnvParam::HasOutParm(CTString sName)//�ж��Ƿ���Ҫ���ĳ����������yresid
{
	return false;
}
CTString CEnvParam::GetOutParamName(CTString sName)//
{
	return false;
}
//*************���û�������************
bool CEnvParam::SetVar(CTString sVarName,double value)
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDoubleVector* value)//value �ڴ�ռ����㷨�Լ��ͷš�
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDoubleMatrix* value)//value �ڴ�ռ����㷨�Լ��ͷš�
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDataAccess* value)//value �ڴ�ռ����㷨�Լ��ͷš�
{
	return false;
}

