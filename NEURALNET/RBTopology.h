//////////////////////////////////////////////////////////////////////
// RBTopology.h:                                                    //
//																	//
// Define:        RB�㷨���˽ṹ�ඨ��                              //
// Creator:       ���                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _RBTOPOLOGY_H_
#define _RBTOPOLOGY_H_


#include "Topology.h"

class CRBTopology : public CTopology  
{
public:
//	virtual void SaveNetwork(){};
//	virtual bool Testing(CDataInterface * pData, CTStringArray arFieldRole, CResult * pResult, bool bOutputInfo){ return false; };
//	virtual bool Training(CDataInterface * pData, CTStringArray arFieldRole, CResult * pResult, bool bOutputInfo){ return false; };
	CRBTopology();
	virtual ~CRBTopology();

};

#endif // !defined(_RBTOPOLOGY_H_)
