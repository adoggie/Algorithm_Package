// DesLayerConf.cpp: implementation of the CDesLayerConf class.
//
//���±�д:�Ķ�ɽ          �ı�дʱ�䣺06.03.28
//�ļ�����;����ֵ��������Ϣ�ṹ
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
