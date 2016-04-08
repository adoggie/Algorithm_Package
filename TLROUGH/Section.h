// Section.h: interface for the CSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_)
#define AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"
//#include <vector>

class CSection  
{
public:
	CSection();
	CSection(char * pCmdData);
	virtual ~CSection();

	//������������C++��ʵ��
	bool CheckSection();//��Ҫ�Ƿ���proc,endp�����������Ƿ����
	void SetSectionData(char * pCmdData);//�����������ĵ�ַ
	CTString GetSectionDataValue(CTString strKey);//��ȡproc�������������س�Ϊ������־
	bool GetSectionDataValue(CTString strKey, CTString strName, CTString &strValue);

protected:
	CTString Find(CTString strKey, CTString strName="");
	bool  Filter();
	char*	m_pCmdData;
};

#endif // !defined(AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_)
