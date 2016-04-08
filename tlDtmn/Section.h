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

	//命令语句的制作C++的实现
	bool CheckSection();//主要是分析proc,endp这两个参数是否存在
	void SetSectionData(char * pCmdData);//设置命令语句的地址
	std::string GetSectionDataValue(std::string strKey);//获取proc后的命令参数，回车为结束标志
	bool GetSectionDataValue(std::string strKey, std::string strName, std::string &strValue);

protected:
	std::string Find(std::string strKey, std::string strName="");
	bool  Filter();
	char*	m_pCmdData;
};

#endif // !defined(AFX_SECTION_H__986CC80E_A15F_472D_ADA8_E3878FEB3083__INCLUDED_)
