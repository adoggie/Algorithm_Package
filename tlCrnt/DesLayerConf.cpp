// DesLayerConf.cpp: implementation of the CDesLayerConf class.
//
//重新编写:夏东山          改编写时间：06.03.28
//文件的用途：均值分析层信息结构
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DesLayerConf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDesLayerConf::CDesLayerConf()
{
	m_nLayer=0;
	m_FNameList.RemoveAll();
}

CDesLayerConf::~CDesLayerConf()
{
	
}

bool CDesLayerConf::IsInLayer(CTString strName)
{
	int nSize=m_FNameList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		CTString str;
		if(strName==m_FNameList.GetAt(i))
			return true;
		
	}
	return false;
}

void CDesLayerConf::Clear()
{
	m_FNameList.RemoveAll();
}

void CDesLayerConf::Remove(CTString strName)
{
	int nSize=m_FNameList.GetSize();
	for(int i=0;i<nSize;i++)
	{
		CTString str;
		if(strName==m_FNameList.GetAt(i))
			m_FNameList.RemoveAt(i);
	}
}

void CDesLayerConf::SetLayer(int nLayer)
{
	m_nLayer=nLayer;
}
