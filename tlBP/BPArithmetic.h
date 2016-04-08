// BPArithmetic.h: interface for the CBPArithmetic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPARITHMETIC_H__71A641F1_8CFD_4571_8772_2B84DA70AF50__INCLUDED_)
#define AFX_BPARITHMETIC_H__71A641F1_8CFD_4571_8772_2B84DA70AF50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Matrix.h"
// Define the max system error
const double	MAX_SYSTEM_ERROR	=	1000;

// ������ֵ
const double	MAX_ADJUST_VALUE	=	10e6;

// ϵͳ���
const double	SYSTEM_ERROR_LEVEL	=	0.000001;

// the max loop times
const unsigned int LOOP_MAX_TIMES	=	10000;

// ������ʼֵ
const double	INIT_STEP	=	0.001;
class CBPArithmetic  
{
	bool m_bAbort;
	
public:
	void Reset();
	void Abort();
	CBPArithmetic();
	virtual ~CBPArithmetic();
/////////////////////////////////////////////////////////////////////////////
//	Levenberg-Marquart		---->	��һ��ǰ������ʼ��				   //
/////////////////////////////////////////////////////////////////////////////
void LMForwardCalculateInit( int nInputLayerNumber,
													int nHideLayerNumber,
													int nOutputLayerNumber,
													CBPMatrix &m_mxDemoDataInput,
													CBPMatrix &m_mxInput,
													CBPMatrix &m_mxIToHWeight,
													CBPMatrix &m_mxHideThreshold,
													CBPMatrix &m_mxHToOWeight,
													CBPMatrix &m_mxOutThreshold
												   );
/////////////////////////////////////////////////////////////////////////////
//	Levenberg-Marquart		---->		ǰ�����						   //
/////////////////////////////////////////////////////////////////////////////
void LMForwardCalculate ( int nInputLayerNumber,
												int nHideLayerNumber,
												int nOutputLayerNumber,
												bool bSimulateDataFlag,
												int nComboFunc,
												CBPMatrix &m_mxDemoDataInput,
												CBPMatrix &m_mxInput,
												CBPMatrix &m_mxIToHWeight,
												CBPMatrix &m_mxHideThreshold,
												CBPMatrix &m_mxHide,
												CBPMatrix &m_mxHToOWeight,
												CBPMatrix &m_mxOut,
												CBPMatrix &m_mxOutThreshold
											   );
/////////////////////////////////////////////////////////////////////////////
//	Back propagation		---->		ǰ�����(Only for Training)		   //
/////////////////////////////////////////////////////////////////////////////
void BPForwardCalculate ( int nInputLayerNumber,
												int nHideLayerNumber,
												int nOutputLayerNumber,
												bool bSimulateDataFlag,
												int nComboFunc,
												CBPMatrix &m_mxDemoDataInput,
												CBPMatrix &m_mxInput,
												CBPMatrix &m_mxIToHWeight,
												CBPMatrix &m_mxHideThreshold,
												CBPMatrix &m_mxHide,
												CBPMatrix &m_mxHToOWeight,
												CBPMatrix &m_mxOut,
												CBPMatrix &m_mxOutThreshold,
												CBPMatrix &cMatrixExHideLayerValveValue,
												CBPMatrix &cMatrixExOutputLayerValveValue
											   );

/////////////////////////////////////////////////////////////////////////////
//	Back propagation		---->		ǰ�����(Only for Simulating)	   //
/////////////////////////////////////////////////////////////////////////////
void BPForwardCalculate2( int nInputLayerNumber,
												int nHideLayerNumber,
												int nOutputLayerNumber,
												bool bSimulateDataFlag,
												int nComboFunc,
												CBPMatrix &m_mxDemoDataInput,
												CBPMatrix &m_mxInput,
												CBPMatrix &m_mxIToHWeight,
												CBPMatrix &m_mxHideThreshold,
												CBPMatrix &m_mxHide,
												CBPMatrix &m_mxHToOWeight,
												CBPMatrix &m_mxOut,
												CBPMatrix &m_mxOutThreshold
											   );
/////////////////////////////////////////////////////////////////////////////
//	Back propagation		---->		��������						   //
/////////////////////////////////////////////////////////////////////////////
bool CBPArithmetic::BPDemoDataTrainRepeat (	int nInputLayerNumber,
													int nHideLayerNumber,
													int nOutputLayerNumber,
													bool bSimulateDataFlag,
													int nComboFunc,
													double &nSystemErrorOld,
													double nSystemErrorNew,
													double nSystemErrorLevel,
													//double nSystemError,//���� ===temp
													double nStep,
													unsigned int nMaxTrainTimes,
													//unsigned int nTrainTimes,//���� ===temp
													//DWORD ID_SYSTEM_ERROR,
													//DWORD ID_TRAIN_TIMES,
													CBPMatrix &m_mxDemoDataInput,
													CBPMatrix &m_mxInput,
													CBPMatrix &m_mxIToHWeight,
													CBPMatrix &m_mxHideThreshold,
													CBPMatrix &m_mxHide,
													CBPMatrix &m_mxHToOWeight,
													CBPMatrix &m_mxOut,
													CBPMatrix &m_mxOutThreshold
												 );
/////////////////////////////////////////////////////////////////////////////
//	Levenberg-Marquart		---->		��������						   //
/////////////////////////////////////////////////////////////////////////////
bool LMDemoDataTrainRepeat (	int nInputLayerNumber,
													int nHideLayerNumber,
													int nOutputLayerNumber,
													bool bSimulateDataFlag,
													int nComboFunc,
													double nSystemErrorOld,
													double nSystemErrorNew,
													double nSystemErrorLevel,
													//double nSystemError,//���� ===temp
													double nStep,
													unsigned int nMaxTrainTimes,
													//unsigned int nTrainTimes,//���� ===temp
													//DWORD ID_SYSTEM_ERROR,
													//DWORD ID_TRAIN_TIMES,
													CBPMatrix &m_mxDemoDataInput,
													CBPMatrix &m_mxInput,
													CBPMatrix &m_mxIToHWeight,
													CBPMatrix &m_mxHideThreshold,
													CBPMatrix &m_mxHide,
													CBPMatrix &m_mxHToOWeight,
													CBPMatrix &m_mxOut,
													CBPMatrix &m_mxOutThreshold
												 );
};

#endif // !defined(AFX_BPARITHMETIC_H__71A641F1_8CFD_4571_8772_2B84DA70AF50__INCLUDED_)
