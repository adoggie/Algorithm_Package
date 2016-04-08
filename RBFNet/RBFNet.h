// RBFNet.h: interface for the CRBFNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RBFNET_H__21187335_F661_4844_B425_05A94A597C6F__INCLUDED_)
#define AFX_RBFNET_H__21187335_F661_4844_B425_05A94A597C6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RBFN_Algorithm.h"
#include "../DataInterface/DataInterface.h"
//����������������ݼ���������ʾ����������������
struct RBFNetResult
{
	CDoubleMatrix mxTarget;
	CDoubleMatrix mxOutput;
	double dMSE;
	double dStdMSE;
	string strRunInfo;
};


class CRBFNet  
{
public:
	CRBFNet();
	virtual ~CRBFNet();
	CDataInterface *m_pDataInterface;
	CPtrArray m_arFieldRole;
	//zlq add end
	CComplexMatrix		m_TempMatrix;
	//temp
	CTStringArray	m_strInAry,m_strTargetAry;
	bool m_bSaveModel;//�Ƿ񱣳�ģ��
	CTString m_ModelFile;//����ģ�͵����֡�·��

public:
	CRBFN_Algorithm m_RBFN;
	
protected:
	CRBFNetVO	*	m_pRBFNetVO;
	CResult * m_pResult;
	
	
public:
	CTString m_strMethod;
	int m_nCenterNum;
	
	
public:
	CResult * TrainRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO);
	CResult * PredRun(CDataInterface *pDataInterface,CEnvParam *pEnvParam, Interaction * pInt, string strVO, string strModel);
public:
	bool SetDataSource(CDataInterface *pDataInterface, const CPtrArray &arFieldRole);
	CResult * Run();
	void Abort();
	virtual bool Export(string szFileName);
	virtual bool Import(string szFileName);
	void	SetTrainVO(string strVO);
	bool	SetPredVo(string VO);
private:
	//��������ʱ�Ľ����ʾ
	CResult* ShowResults(CTString strModel, RBFNetResult& RBFNResult);
	//����������ʱ�Ľ����ʾ
	CResult* ShowResults(CTString strModel, CTString strInfo);
	CTString GetSaveFileName();
	
};

#endif // !defined(AFX_RBFNET_H__21187335_F661_4844_B425_05A94A597C6F__INCLUDED_)
