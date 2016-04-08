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

//CTString StructBase::Export()//导出为XML方式
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

//******  指定那些参数需要输入到参数中  *** 
bool CEnvParam::HasOutParm(CTString sName)//判断是否需要输出某个参数，如yresid
{
	return false;
}
CTString CEnvParam::GetOutParamName(CTString sName)//
{
	return false;
}
//*************设置环境变量************
bool CEnvParam::SetVar(CTString sVarName,double value)
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDoubleVector* value)//value 内存空间由算法自己释放。
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDoubleMatrix* value)//value 内存空间由算法自己释放。
{
	return false;
}
bool CEnvParam::SetVar(CTString sVarName,CDataAccess* value)//value 内存空间由算法自己释放。
{
	return false;
}

