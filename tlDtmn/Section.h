// Section.h: interface for the CSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_)
#define AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <vector>
//using namespace std;

class CSection  
{
public:
	CSection();
	CSection(char * pCmdData);
	virtual ~CSection();

	//������������C++��ʵ��
	bool CheckSection();//��Ҫ�Ƿ���proc,endp�����������Ƿ����
	void SetSectionData(char * pCmdData);//�����������ĵ�ַ
	std::string GetSectionDataValue(std::string strKey);//��ȡproc�������������س�Ϊ������־
	bool GetSectionDataValue(std::string strKey, std::string strName, std::string &strValue);

protected:
	std::string Find(std::string strKey, std::string strName="");
	bool  Filter();
	char*	m_pCmdData;
};

#endif // !defined(AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_)
