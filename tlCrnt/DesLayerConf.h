// DesLayerConf.h: interface for the CDesLayerConf class.
//
//重新编写:夏东山          改编写时间：06.03.28
//文件的用途：均值分析层信息结构
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DESLAYERCONF_H__9F3F954B_F9FF_4967_B504_AA4B68D7E604__INCLUDED_)
#define AFX_DESLAYERCONF_H__9F3F954B_F9FF_4967_B504_AA4B68D7E604__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class AFX_EXT_CLASS CDesLayerConf  
{
public:
	void SetLayer(int nLayer);
	void Remove(CTString strName);
	void Clear();
	bool IsInLayer(CTString strName);
	int m_nLayer;
	CTStringArray m_FNameList;
	CDesLayerConf();
	virtual ~CDesLayerConf();
};

#endif // !defined(AFX_DESLAYERCONF_H__9F3F954B_F9FF_4967_B504_AA4B68D7E604__INCLUDED_)
