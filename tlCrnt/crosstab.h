//////////////////////////////////////////////////////////////////////
// CrossTab.h                                                       //
//																	//
// Define:        �����crosstabulation�㷨�ඨ��                   //
// Creator:       �Ķ�ɽ                                              //
// Date:          2006.03.20                                        //
// Specification:                                                   //
// Modification:                                                    //
//////////////////////////////////////////////////////////////////////

#ifndef _CROSSTAB_H_
#define _CROSSTAB_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"
const int CALC_COLFLD = 0;
const int CALC_ROWFLD = 1;
const int CALC_TMPFLD = 2;
//public:
class CCrossTab  
{
public:
	CCrossTab();
	virtual ~CCrossTab();
	CResult * OnCrossTab(CDataInterface *pDataInterface, string VO); // ������Ϣ��Ӧ����

private:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//�ڲ���ں���
	void CrossTab();
	//ͨ��ֵ�õ���Ӧ�ı�ǩ
	CTString GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue);
	//�������ܱ�
	void CreateSummaryTable(CDoubleMatrix &dataMatrix, int nRowIndex, int nColIndex);
	//���������
	void CreateCrossTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol);
	//����ͳ������
	void CreateStatisticTable(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol, int nRow, int nCol);
	//������������ͳ��
	//BOOL CalcColInfo(CDoubleMatrix &dataMatrix, CDoubleVector &vecCol, CPtrArray *pArrColInf);
	//������������ͳ��
	//int CalcRowInfo(CDoubleMatrix &dataMatrix, CIntVector &vecRowIndex, CPtrArray *pArrRowInf);
	//�õ���ʾ�е�����
	CFldInfo* GetFldInfByValue(double fValue, CPtrArray *pArrColInf);
	//�������ݵ������
	BOOL InsertCrossTab(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CPtrArray *pArrRow, CPtrArray *pArrCol, CTLTable * pCrossTable, int nRow);
	//����ĳһ�����ݵ������
	void InsertCorssTabOfRow(CMapManager &dataMap, CTLTable * pCrossTable, CFldInfo* pRowInf, CPtrArray *pArrCol, int nRowStart, int nRow, double fTotalWeight, BOOL bLast = FALSE);
	//����õ�ָ��
	void CalcFldInf(double fValue, double fVarValue, double fWeight, CPtrArray *pArrFld, int nSize, int nType);
	//��������Ϣ
	BOOL CalcFldInf(  CDoubleVector &v,CDoubleVector &vc, CDoubleVector &w, CPtrArray *pArrFld, int nType);
	//�������ϵ����������
	BOOL CalcCorrlation(CDoubleMatrix &dataMatrix, int nRow, int nCol);
	//���㿨�������ɶ�
	BOOL CalcChiSquare(CDoubleMatrix &dataMatrix, CPtrArray *pArrRow, CPtrArray *pArrCol);
	//�ͷ��ڴ�
	void Release(CPtrArray *pArrFld);
private:
	TIntStruct m_tColInfo;    //��
	TIntStruct m_tRowInfo;    //��
	int m_nCrossTableCnt;     //��������
	int m_nType;              //�������
	__int64 m_n64Weight;      //Ȩ�غͻ��ܼ�¼��
	int m_nWeight;			  //Ȩ������
	int  m_nColSize;          //������
	int  m_nCurRows;          //��ǰ������
	BOOL m_bWeight;           //�Ƿ��Ȩ��
	BOOL m_bOutputChiSquare;  //�Ƿ��������
	BOOL m_bOutputCorrelation;//�Ƿ�������ϵ��
	BOOL m_bOutputExpCount;   //�Ƿ���������
	double m_fPChiSquare;     //Ƥ��ѷ����
	double m_fPChiSquareSig;  //Ƥ��ѷ����������
	double m_fLChiSquare;     //��Ȼ����
	double m_fLChiSquareSig;  //��Ȼ����������
	int    m_nDF;             //���ɶ�
	double m_fPearsonCorr;    //Ƥ��ѷ���ϵ��
	double m_fSig;            //������
	//
	CTLTable *m_pSummaryTable;//���ܱ�
	CTLTable *m_pCorrelationTable;//���ϵ����
	CTLTable *m_pChiSquareTable;  //������
	CResult        *m_pResult;    //���ݼ�ָ��
	CDataInterface *m_pDataInterface; //�ⲿ�ӿ�
};

#endif // !defined(_CROSSTAB_H_)
