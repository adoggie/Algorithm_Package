// ExponSmoothing.h: interface for the CExponSmoothing class.
//						ָ��ƽ��ģ��
//                    ���ܣ�ָ��ƽ��ģ�͵Ĺ�ͬ����������
//					  ��ע��  ������
//					  ʱ�䣺2005��10��20��
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_)
#define AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ConfTypes.h"
#include "../Public/TString.h"


class AFX_EXT_CLASS CExponSmoothing  
{
public:
	CResult* OnExponSmoothing(CDataInterface *pDataInterface, const string VO);
	CExponSmoothing();
	virtual ~CExponSmoothing();

	void InsertSSECols(int nSeason,int nTrend,CTLTable* pSSETable);
	void SetSSEItem(int nSeason,int nTrend,CTLTable* pSSETable,int nItem,SmoothSetting *pSetting,double dbSSE);
	double GetSSE(CDoubleVector ErrVec);
	double Mean(CDoubleVector Vec);
	int Mean(CDoubleVector Vec,int nPeriodicity,int nPNO,double* pdbSum);
	bool ExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,double dbalpha,double dbdelta,double dbgamma,double dbphi,CResult* pResult,double *pSSE);
	bool GridExponSmooth(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,CResult* pResult);
	void GridSearch(CDoubleVector XVec,CTString strFName,int nSeason,int nTrend,SmoothConf * palphaconf,SmoothConf * pdeltaconf,SmoothConf * pgammaconf,SmoothConf * pphiconf,CResult* &pResult);
	bool GetValueFromVO(CDataInterface *pDataInterface, string VO);

	CDataInterface* m_pDataInterface;
	bool m_bShowBestModels;//ֻ��ʾ10�����ģ��
	bool m_bShowPredict;//�Ƿ���ʾԤ�ⲽ��
	int m_nPredictNum;//Ԥ�ⲽ��
	int m_nSeason;//����ѡ��	//0-�޼��ڱ��� ��1���ӷ�  ��2 ���˷�
	int m_nTrend;//����ѡ��		//0�������ƣ�1���������ƣ�2��ָ������ ��3��˥������
	int m_nAlpha;//0-���� ��1��������Ѱ
	int m_nDelta;//0-���� ��1��������Ѱ
	int m_nGamma;//0-���� ��1��������Ѱ
	int  m_nPhi;//0-���� ��1��������Ѱ

	int m_nPeriodicity;
	double m_dbalpha;
	SmoothConf m_alphaconf;
	double m_dbdelta;
	SmoothConf m_deltaconf;
	double m_dbgamma;
	SmoothConf m_gammaconf;
	double m_dbphi;
	SmoothConf m_phiconf;
	TIntStruct	m_LstSelVar;//ѡ������±꼯
	CTString strDBName;
	CTStringArray m_FNames;

};

//add by liytao 20030410
#define NOSEASONAL    0 //�޼��ڱ���
#define ADDITIVE	1	//�ӷ�
#define MULTIPLY	2	//�˷�

#define STRNOSEASONAL    _T("�޼��ڱ���")
#define STRADDITIVE		_T("�ӷ�")
#define STRMULTIPLY		_T("�˷�")


#define NOTREND		0	//������
#define LINEAR		1	//����
#define EXPONENTIAL 2	//ָ��
#define DAMPED		3	//˥��

#define STRNOTREND		_T("������")
#define STRLINEAR		_T("����")
#define STREXPONENTIAL 	_T("ָ��")
#define STRDAMPED		_T("˥��")
 	
//add end



#endif // !defined(AFX_EXPONSMOOTHING_H__E951CC7F_A07A_492F_A4CD_DE93790E01D9__INCLUDED_)
