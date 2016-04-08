// BP.h: interface for the CBP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_)
#define AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Matrix.h"
#include "BPArithmetic.h"

class CBPNetOutput;

class CBP  
{
	CBPArithmetic m_bpa;
	// ���ڴ洢������������ݵľ���
	CBPMatrix		m_mxDemoDataInput;
	// ��������������ֵ����
	CBPMatrix		m_mxInput;
	// ����㵽�������Ȩֵ����
	CBPMatrix		m_mxIToHWeight;
	// ������ķ�ֵ����
	CBPMatrix		m_mxHideThreshold;
	// �����㵽������Ȩֵ����
	CBPMatrix		m_mxHToOWeight;
	// �����ķ�ֵ����
	CBPMatrix		m_mxOutThreshold;
	// ����������ֵ�ľ���
	CBPMatrix		m_mxHide;
	// ���������ֵ�ľ���
	CBPMatrix		m_mxOut;
	// ��������ʱ����������������
	CBPMatrix		m_mxSimuNet;

	// ϵͳ���
	double		m_SystemErrorNew;
	bool		m_bSimulateDataFlag;// ������ѵ��(0)�������ݷ���(1)

public:
	int m_nInputLayerNumber;//�������Ŀ:
	int	m_nHideLayerNumber;//��������Ŀ:
	int	m_nOutputLayerNumber;//�������Ŀ:

	int		m_nComboArithmetic;//����ѵ�����㷨 0:LM  1:BP
	int m_nComboFunc;//����ѵ���ļ���� 0:Sigmoid 1:tanh 2:Tansig
	double m_nSystemErrorLevel;//ϵͳ����:
	//double	m_nSystemError;
	//unsigned int	m_nTrainTimes;
	unsigned int	m_nMaxTrainTimes;//���ѵ������:
	double	m_dStep;//����
	

	CBPMatrix m_mxTrainOut;  //����ѵ�����(�洢ģ������������ֵ����)
public:
	CBP();
	virtual ~CBP();
public:
	double		m_SystemErrorOld;
	void Reset();
	void Serialize(CMemoStream *pStream, bool IsStoring);
	void SetWnd();
	void Abort();
	// ����ѵ��
	bool TrainingDemo(CBPMatrix &mxSour);
	// ���ݷ���  mxSimuResult������ŷ������ݽ���ľ���
	bool SimulateData(CBPMatrix &mxSour,CBPMatrix &mxSimuResult);
	bool	CreatePredModel(CBPNetOutput *&pOutStruc);
	void	initPred(CBPNetOutput &Output);
	//ģ��
	virtual bool Export(TCLFile &theFile);
	virtual bool Import(TCLFile &theFile);
private:
	// ����ѵ��
	bool DemoDataTrainRepeat();
	// ǰ������ʼ��
	void ForwardCalculateInit();
};

#endif // !defined(AFX_BP_H__39BCCDC2_69A3_40C7_AF4C_C30F290B27F8__INCLUDED_)
