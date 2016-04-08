// TimeX11.h: interface for the CTimeX11 class.
//                      X_11���ڵ���
//                    ��ע�� ������
//					  ʱ�䣺 2005��10��20��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_)
#define AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"



class AFX_EXT_CLASS CTimeX11 : public CAlgorithmBase  
{public:
	CTimeX11();
	virtual ~CTimeX11();
	CResult * CTimeX11::OnTimeX11(CDataInterface *pDataInterface, string VO);
	int m_nMove;			//ƽ���ķ�����12��2 ����3��3
	int m_nYear;			//��ʼʱ�䣬��
	int m_nMonth;			//��ʼ ��
	int m_nMethod;			//�ӷ����ǳ˷���0���˷�  1���ӷ�
	TIntStruct m_tVarInfo;	//�����±�

private:
	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
    CRsltVariable * m_pRsltVariable;
	BOOL	GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void	WeekCalcu(int year, int month);//��������ÿ�����ж����죬�ж��ٸ���һ�ܶ�����������m_DaysinMonth_vec��m_WeekDescrib_mx
	BOOL	Non11(CDoubleVector& Org_vec, int nMove, int Method);//��Ҫ�ļ��㺯��
	void	DrawResult(BOOL TOut, int VarInfo);//��ͼ
	int		DaysInMonth(int year, int month);//�������ж�����
	CDoubleVector Mov_ave(CDoubleVector& SmoothVec, int nMove);
	void	AdjustI(CDoubleVector& Ivec);
//Ϊ��̷������õı���
	int m_nRow;				//�۲���
	CDoubleVector m_DaysinMonth_vec;//�����ж����죬
	CDoubleMatrix m_WeekDescrib_mx;//��������һ�ܶ�����
	CDoubleVector m_CT_SmoothedVec;//���յõ���CT������
	CDoubleVector m_S_SmoothedVec;//���յõ���S������
	CDoubleVector m_Dr_FinalVec;//���յõ���Dr������
	CDoubleVector m_I_FinalVec;//���յõ���I��ƫ��
	CDoubleVector m_Justed_vec;//�����������
	CDoubleVector m_Org_vec;//ԭʼ������
};

#endif // !defined(AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_)
