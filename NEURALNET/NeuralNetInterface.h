//////////////////////////////////////////////////////////////////////
// NeuralNetInterface.h                                             //
//																	//
// Define:        神经网络分析系统导出类定义                        //
// Creator:       屈韬                                              //
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
//神经网络的入口类，主要是通过此类中的SetDataSource把接口和数据传输到m_pNeuralNet，
//并且把参数传输到m_pNeuralNet中去，开始执行Run进行神经网络计算，训练和预测
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
