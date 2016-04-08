// CMarkovChain.h: interface for the CMarkovChain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_)
#define AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <time.h>

class CMarkovChain  
{
public:
	CMarkovChain();
	virtual ~CMarkovChain();
	CResult * OnCMarkovChain(CDataInterface *pDataInterface, string VO);//�ⲿ����������
	CResult * OnMorkovChainPred(CDataInterface *pDataInterface, string VO);//�ⲿ����Ԥ�⺯��
	
private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	BOOL GetPredValueVo(CDataInterface *pDataInterface, string VO);//��ȡԤ��Ĳ���
	BOOL GetValueFromModel(CDataInterface *pDataInterface, string strModel);//��ģ����ȡ������
	BOOL Main();
	BOOL EMalgorithm(CDoubleVector& Pi_vec, CDoubleMatrix& Theta_Init_mx, 
		CDoubleMatrix* Theta_Trans, CDoubleMatrix& r,
		CDoubleVector start_vec, CDoubleVector length_vec, 
		CDoubleVector pairID_vec, int K, int cutting);
	void DrawResult();
	//--UNIX PORT
	#ifdef _UNIX
	unsigned int GetTickCount(){
		return (unsigned int)clock();
	}
	#endif

	//��ȡ������ʱ
	int m_nUserID;//ID���±�
	int m_nPath;//Path���±�
	int m_nTime;//
	int m_nCluster;//�������
	int m_nMaxIter;//����������
	BOOL m_bAutoCluster;//�Զ�����K
	double m_dMinConverge;//��������
	int m_nPageNum;//���ж��ٸ���ҳ���
	BOOL m_bSaveModel;//�Ƿ񱣴�ģ��
	CTString m_ModelFile;//����ģ�͵�·��
	BOOL m_bSaveData;//�Ƿ��û�ָ�����������ļ�·��������
	CTString m_DataFile;//�û�ָ�����������ļ�·��������

	//��ȡ��������
	
	//Ϊ��̷��㶨��ı���
	CDataInterface * m_pDataInterface;
	CDoubleMatrix * m_Theta_Trans;
	int m_nRow;//���ж��ٸ�����
	int m_nTotalID;//���ж��ٸ��û�
	CDoubleVector m_ID_vec;//
	CDoubleVector m_Path_vec;//
	CIntVector m_VecClus;//�û�������
	CResult * m_pResult;
	string m_strModel;//����ģ��(m_nCluster, m_nPageNum, m_nCluster��ת�ƾ����ֵ)
};

#endif // !defined(AFX_CMarkovChain_H__2A9063D7_E809_4128_80C6_EFBB96AAA482__INCLUDED_)
