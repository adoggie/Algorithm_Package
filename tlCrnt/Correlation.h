// Correlation.h: interface for the CCorrelation class.
//				��ط���������person, spearman, kendall
//					�Ķ�ɽ			2006-03-25
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CORRELATION_H__9D6D990D_FE54_487A_B5A1_CA96C23EA531__INCLUDED_)
#define AFX_CORRELATION_H__9D6D990D_FE54_487A_B5A1_CA96C23EA531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class  CCorrelation  
{
public:
	CCorrelation();
	virtual ~CCorrelation();
	CResult* Correlation(CDataInterface * pDataInterface,string VO);
private://�������ɾ��������ٶ�
	bool GetValueFromVo(CDataInterface *pDataInterface, string VO);
	bool CalcCorrelation();
	bool GetData2Matrix(CDoubleMatrix &dataMatrix);
	double CalcSign(CDoubleVector &vec_x, CDoubleVector &vec_y);
	void CalcRank(CDoubleVector &vec_data, CDoubleVector &vec_change, double *pTau = NULL);
	void Kendall(CDoubleMatrix &dataMatrix);
	void Spearman(CDoubleMatrix &dataMatrix);
	void CalcMeanAndStdDev(CDoubleMatrix &dataMatrix);
	void Pearson(CDoubleMatrix &dataMatrix);
	inline void Swap(TDataSet &t1, TDataSet &t2);             //����
	void QSort(TDataSet *pData, int nLow, int nHigh);         //��������
	void Clear();
	//�Դ����ݽ��зֿ���㣬Ȼ����sign,ȡ�����ڶ���.
	bool KendallSplit(CDoubleMatrix &dataMatrix, int nCol_x, int nCol_y, int nVarCount);
	bool KendallSplit(TDataSet * pData_x, TDataSet * pData_y, int nCol_x, int nCol_y, int nValidCorr, int nVarCount);
private:
	void Pearson( CPtrArray& rFiledArray, double aWeight[]);
	void Spearman( CPtrArray& rFiledArray);
	void Kendall( CPtrArray& rFiledArray);
	bool GetData(CDataInterface* pDataInterface, CPtrArray& pArray, int nGetType );
	void CreateResult(CDataInterface* pDataInterface,CResult* &pResult);
	void CalcMeanAndStdDev( CPtrArray &rFieldArray, double aWeight[], int k, int n );
	void CalcRank( TDataSet * pData, TDataSet *pChange, int n, double *pTau = NULL );
	void Sort( TDataSet * pData, int n, bool bByValue=true );
	double CalcSign( TDataSet * pData_x, TDataSet * pData_y, int n );
	void CalcPearsonCorrAndSig( double adCrossDevs[], double adCrossDevSelf[], int k );
	void CalcKendallCorrAndSig( int x, int y, double fS, double aTau_x[3], double aTau_y[3], int n, int k );

public:
	bool    m_bSpearman ;
	bool    m_bPearson ;
	bool    m_bKendall;
	TIntStruct m_tVarInfo;
private:
	bool    m_bWeight;
	int     m_nWeight;//Ȩ������
	CResult        *m_pResult;       //�����
	CDataInterface *m_pDataInterface;//���ݽӿ�
private:
	double *m_aCorr;
	double *m_aSig;
	double *m_adMeans;
	double *m_adStdDevs;
	double *m_adWeightofCorr;
	double *m_anNofRow;
	double *m_anNofCorr;
	bool m_bOneTail;
	bool m_bWeightMissing;
	bool m_bPairwise;
};

#endif // !defined(AFX_CORRELATION_H__9D6D990D_FE54_487A_B5A1_CA96C23EA531__INCLUDED_)
