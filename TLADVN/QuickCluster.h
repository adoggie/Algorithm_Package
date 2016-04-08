// QuickCluster.h: interface for the CQuickCluster class.
//					���پ������
//          ���ܣ� ���پ����㷨ʵ��
//          ��ע�� ������
//			ʱ�䣺 2005��10��          							
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_)
#define AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Classify.h"
#include <vector>

#include "../DataInterface/Define.h"
#include "../TLADVN/StdAfx.h"


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CQuickCluster : public CClassify
{
public:
	CShowData* m_pShow;
	bool Do();
	CResult * OnQuickCluster(CDataInterface * m_pDataInterface,std::string VO);
	CQuickCluster();
	virtual ~CQuickCluster();

	int			m_nNC;											//ָ���������ĸ���

private:
	void CreateInitResult();
	void CreateResult();
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);
	//void GetData(CDataInterface *pDataInterface, CPtrArray &rArray, int nGetType );
	void GetData(CDataInterface *pDataInterface, TCDataMatrix &rArray, int nGetType );

	//bool UpdateMean(int * pCluster);							//����������
	bool UpdateMean(TIntAry* pCluster);	//����������
	void CopyClusterIndex(TIntAry *x ,TIntAry *pSource,int nLen);		//���������ݵĹ�������		
	
	bool CompareCluster(int * x,int * y,int nLen);				//�Ƚ����ξ���Ľ���Ƿ���ͬ
	void		Iteration();									//ѭ������
	bool		AssignCasesToNearestCluster();					//��������Ʒ��ͷ��β����һ�ε����������
	bool		UpdateInitialClusterCenter(int pInitalCenter[]);//���³�ʼ��������
	bool		InitalClusterCenter();							//��ʼ��������
	void		Clear();										//�ڴ��ͷź���
	bool        IsClusterChanged();
//	double		EucDistance(const int i,const int j);			//��������֮��ľ���
//	double		EucDistance(double * x,double * y,int nLen);	//������������֮��ľ���
//	double		GetMinDistance(int &x1,int &x2);				//��������С�ľ���

	double		* m_pMNC;										//�����ĵ�ָ��
	CDoubleMatrix	m_CenterOFCluster;							//�����ĵľ���(һ����һ��)
	CDoubleVector	m_DisPointToCenter;							//�����ൽ���ĵ�ľ����ܺ�
	CIntVector		m_DistanceToshortestCluster;				//�������ĵ�֮�����̾���
	double		* m_pWeightCases;								//Ȩ������
	int			m_nValidWeightCases;							//�Ϸ���Ȩ����
	//CPtrArray   m_aDataSet;										//������ָ������
	TCDataMatrix m_DataMatrix;

	TIntAry    m_IntCluster;                                    //��ʶ�������������

	TIntAry   m_IntTempCluster;  //��ʶ�������������,��ʱ�洢���������ξ���ıȽ�
	int         * m_pCount;										//��ʶÿһ��İ���Ԫ�صĸ���	
	double		* m_pClusterWeight;								//��ʶÿһ��İ���Ԫ�صļ�Ȩ����
	int         m_nIterationNum;								//ָ��ѭ���Ĵ���
	bool		m_bCountFlag;									//ָ��ѭ�������ı�־								
	int			m_nCountMax;
	CResult *	m_pResult;
	bool		m_bOutputCaseInfo;								//�Ƿ��������������Ϣ
	double		* m_pPrevMNC;									//����ǰ�ľ�������
protected:
		
};

#endif // !defined(AFX_QUICKCLUSTER_H__166656C2_E71A_4072_B93D_3E547DD1BC09__INCLUDED_)
