//////////////////////////////////////////////////////////////////////
// BPTopology.h:                                                    //
//																	//
// Define:        BP�㷨���˽ṹ�ඨ��                              //
// Creator:       ���                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _BPTOPOLOGY_H_
#define _BPTOPOLOGY_H_

#include "Topology.h"

class CBPTopology : public CTopology  
{
public:
//	virtual void SaveNetwork(){};
//	virtual bool Testing(CDataInterface * pData, vector<void *>& arFieldRole, CResult * pResult, true bOutputInfo){ return false; };
//	virtual bool Training(CDataInterface * pData, vector<void *>& arFieldRole, CResult * pResult, true bOutputInfo){ return false; };
	CBPTopology();
	virtual ~CBPTopology();

};

#endif // !defined(_BPTOPOLOGY_H_)
