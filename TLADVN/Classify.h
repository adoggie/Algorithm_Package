// Clasify.h: interface for the CClassify class.
//                        ��������Ļ�����
//                    ���ܣ����־����㷨�Ĺ�ͬ����������
//                    ��ע��  ������
//					  ʱ�䣺2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_)
#define AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

//#include "Factor.h"
#include "AlgorithmBase.h"

struct TClassifyGroup
{
	int nIndex1;
	int nIndex2;
	double dbCoeff;
	TClassifyGroup()
	{
		memset(this,0,sizeof(TClassifyGroup));
	}	
};

//�������
class AFX_EXT_CLASS CClassify : public CAlgorithmBase  
{
public:
	CResult * OnClassify(CDataInterface *pDataInterface, std::string VO);
	int m_nFomlizeMethod;
//	0��������ӷ�(Average Linkage between Group)
//	1���������ӷ�(Average Linkage within Group)
//	2����������(Single Linkage)
//	3����ȫ����(Complete Linkage)
//	4�����ľ��෨(Centroid Method)//����ķ������
	int m_nClusterMethod;
	int m_nAbsent;										//���ڴ��������
	int m_nPresent;										//�����ڴ��������
	int m_nRoot;										//�����Ĵ���
	int m_nPower;										//power�Ĵ���
	bool m_bSimulate;
	CClassify();
	virtual ~CClassify();
	bool Formalize(const int nIndex);					//��׼������
	bool ClusterMeasures(int nMethod,int nIndex);		//��Ⱦ���
	void GeneralProcedure();							//ѭ������
	TIntStruct m_tVarInfo;


private:
	int m_nDistance;
	bool GetValueFromVO(CDataInterface *pDataInterface, std::string VO);//��VO�ַ����ж�ȡ���Ʋ���
	double * GetData( CDataInterface * pDataInterface, TIntStruct& tVarInfo, int& nCaseNum );
	void CreateResult();
	void UpdateMean(const int nindex);
	void ProcessIndex(const int nIndex);
	void Free();
	void UpdateSimulateMatrix(const int p,const int q);	//����Ԫ��
	int m_nCurRow;
	double * SimulateAdd(int nRow,int nCol);
	double SimulateData(int nRow,int nCol);
	void ClusterMethod(int nIndex,int p,int q);			//����
	void Binary(int nIndex);							//��ֵ����
	void Count(int nIndex);								//��������
	void Interval(int nIndex);							//�������
	void OutputIcicle();								//�������ͼ
	void Swap(TClassifyGroup *p1, TClassifyGroup *p2);      //��������
	void QSortArray(CPtrArray *pArray, int nLow, int nHigh);//��������

	double * m_pSimulate;
	double * m_pN;
	double * m_pSum;
	int * m_pIndex;
	int * m_aRecNo;										//�������ݼ�¼��
	bool m_bOutputIcicle;								//�Ƿ��������ͼ
	bool m_bWeightSet;									//�Ƿ�������Ȩ��
	bool m_bCaseCluster;								//�Ƿ����������
	bool m_bAbort;										//�û��Ƿ�ȡ��������
	//bool m_bOutputCaseInfo;								//δ֪��;
	CPtrArray m_arClassifyResult;
protected:
	void CalNodeYPos(PedigreeTreeNode &node,CPtrArray &leafAry);
	int m_nOutChartSel;
	bool m_bOutputPedigree;
};

#endif // !defined(AFX_CLASSIFY_H__ABBD2DBF_5787_457B_9B04_A318775315F5__INCLUDED_)
