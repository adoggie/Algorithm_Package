//////////////////////////////////////////////////////////////////////
// NeuralNetInterface.h                                             //
//																	//
// Define:        ���������ϵͳ�����ඨ��                        //
// Creator:       ���                                              //
// Date:          2002.10.24                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////
#ifndef _NEURALNETINTERFACE_H_
#define _NEURALNETINTERFACE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNet.h"
// #include "PropBasic.h"
#define DLLEXPORT __declspec(dllexport)
//////////////////////////////////////////////////////////////////////////
//�����������࣬��Ҫ��ͨ�������е�SetDataSource�ѽӿں����ݴ��䵽m_pNeuralNet��
//���ҰѲ������䵽m_pNeuralNet��ȥ����ʼִ��Run������������㣬ѵ����Ԥ��
//////////////////////////////////////////////////////////////////////////
class DLLEXPORT CNeuralNetInterface //: public CObject
{
private:
	int nTopologyType;
	CNeuralNet *m_pNeuralNet;

public:
	void Abort();
	bool SetDataSource(  CDataInterface * pData, CTStringArray& arFieldRole );
	bool SetDataParam(CNeuralNetVO	*pNeuralNet);
	static unsigned int WorkThreadProc();
	void DeleteAll();
	void Run();
	CNeuralNetInterface();
	virtual ~CNeuralNetInterface();

};

#endif // !defined(_NEURALNETINTERFACE_H_)
