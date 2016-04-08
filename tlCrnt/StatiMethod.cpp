// StatiMethod.h: interface for the CStatiMethod class.
//		CDescriptive ��ֵ
//			CFrequency Ƶ��
//			CExplore ����
//				�Ķ�ɽ		2006/03/29
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "StatiMethod.h"
#include "DesLayerConf.h"
#include "FreqConf.h"
#include "Analyze.h"
#include "Script.h"
#include "Statistics.h"
#include <string.h>
#ifndef _UNIX
	#include <memory.h>
	#include <stdio.h>
#endif

const char STR_TOTAL[]	= _T("����");
const char STR_SEP[]    = _T("+"); //�ָ���
const int  DISP_MAX     = 2*10000;
const int  DISP_EXP_MAXMIN = 5;  //��ʾ�����С����
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStatiMethod::CStatiMethod()
{
	m_tStatMethodS[0]=TIDName(EXPLORE_ERANGE,"����");
	m_tStatMethodS[1]=TIDName(EXPLORE_EMINIMUM,"��Сֵ");
	m_tStatMethodS[2]=TIDName(EXPLORE_EMAXIMUM,"���ֵ");
	m_tStatMethodS[3]=TIDName(EXPLORE_EMEDIUM,"��λ��");
	m_tStatMethodS[4]=TIDName(EXPLORE_ESUM,"�ܺ�");
	m_tStatMethodS[5]=TIDName(EXPLORE_EMEAN,"ƽ��ֵ");
	m_tStatMethodS[6]=TIDName(EXPLORE_EMEANERROR,"��ֵ��׼��");
	m_tStatMethodS[7]=TIDName(EXPLORE_ESTD,"��׼��");
	m_tStatMethodS[8]=TIDName(EXPLORE_EVARIANCE,"����");
	/*
	m_tStatMethodS[9]=TIDName(EXPLORE_ESKEWNESS,"ƫ̬ϵ��");
	m_tStatMethodS[10]=TIDName(EXPLORE_ESKEWNESSERROR,"ƫ̬ϵ�����");
	m_tStatMethodS[11]=TIDName(EXPLORE_EKURTOSIS,"��̬");
	m_tStatMethodS[12]=TIDName(EXPLORE_EKURTOSISERROR,"��̬���");
	*/
	m_tStatMethodS[9]=TIDName(EXPLORE_ESKEWNESS,"ƫ��"); //zfm 20030331
	m_tStatMethodS[10]=TIDName(EXPLORE_ESKEWNESSERROR,"ƫ�����");//zfm 20030331
	m_tStatMethodS[11]=TIDName(EXPLORE_EKURTOSIS,"���");//zfm 20030331
	m_tStatMethodS[12]=TIDName(EXPLORE_EKURTOSISERROR,"������");//zfm 20030331
		
	m_tStatMethodS[13]=TIDName(EXPLORE_EMODE,"����");
	m_tStatMethodS[14]=TIDName(EXPLORE_ETRIMMED,"5%��β��ֵ");
	m_tStatMethodS[15]=TIDName(EXPLORE_EINTERRANGE,"���䷶Χ");
	m_tStatMethodS[16]=TIDName(EXPLORE_ECONFIDENCE,"��������");

	m_bCalcVar = TRUE;
}

CStatiMethod::~CStatiMethod()
{	
}

void CStatiMethod::Free(CPtrArray *par, BOOL bLayer)
{
	for(int i=0;i<par->GetSize();i++)
	{
		delete par->GetAt(i);
	}
	par->RemoveAll();
}

void CStatiMethod::ParseStatMethod(AnaWord VOstr)
{
}

//�õ���ǩ
CTString CStatiMethod::GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue)
{
	BOOL bShowLabel = FALSE;
	CTString szText(""),szLabel("");
	if (m_pDataInterface == NULL)
	{
		szLabel.Format("%.4f",fValue);
		return szLabel;
	}
	bShowLabel = m_pDataInterface->GetShowValueLabel();
	CFieldType fieldtype = m_pDataInterface->GetFieldType(nColIndex);
	CField *pField = m_pDataInterface->FieldByIndex(nColIndex);
	if (pField == NULL)
		return szLabel;
	if (fieldtype == fString)
	{
		szText = dataMap.GetStrValue(nPos,int(fValue));
		if (bShowLabel)
		{
			if (!pField->GetValueVL(szText,szLabel))
				szLabel = szText;
		}
		else
		{
			szLabel = szText;
		}
	}
	else
	if (fieldtype == fDouble)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(fValue,szLabel))
				szLabel.Format("%.4f",fValue);
		}
		else
		{
			szLabel.Format("%.4f",fValue);
		}
	}
	else
	if (fieldtype == fInt)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(int(fValue),szLabel))
				szLabel.Format("%.0f",fValue);
		}
		else
		{
			szLabel.Format("%.0f",fValue);
		}
	}
	else
	if (fieldtype == fBoolean)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(bool(fValue),szLabel))
				szLabel.Format("%.0f",fValue);
		}
		else
		{
			szLabel.Format("%.0f",fValue);
		}
	}
	else
	{
		szLabel.Format("%.0f",fValue);
	}
	return szLabel;
}

void CStatiMethod::Release(CPtrArray *pArrFld)
{
	int i = 0;
	if (!pArrFld) return;
	int nSize = pArrFld->GetSize();
	for (i=0; i<nSize; i++)
	{
		delete pArrFld->GetAt(i);
	}
	pArrFld->RemoveAll();
}

void CStatiMethod::CalcParam(CDoubleVector &v, CDoubleVector &w, BOOL bCalcMaxMin)
{
	m_bCalcVar = TRUE;
	int nVlen = v.vlen();
	
	//��Ȩ��
	if (m_bWeight)
	{
		if (nVlen > 0 && nVlen < 2)
		{
			m_tOut.m_fMin = m_tOut.m_fMax = v(0);
			m_tOut.m_fSum = w(0);
			m_tOut.m_fSmy = v(0)*w(0);
			m_tOut.m_fMean = v(0);
			m_bCalcVar = FALSE;
			return;
		}
		//������
		if (bCalcMaxMin)
			v.sumvarmaxmin_w(w,m_tOut.m_fSmy,m_tOut.m_fS2,m_tOut.m_fMax,m_tOut.m_fMin,m_tOut.m_fSum);
		else
			v.sumvar_w(w,m_tOut.m_fSmy,m_tOut.m_fS2,m_tOut.m_fSum);
	}
	else
	{
		if (nVlen > 0 && nVlen < 2)
		{
			m_tOut.m_fMin = m_tOut.m_fMax = v(0);
			m_tOut.m_fSum = nVlen;
			m_tOut.m_fSmy = v(0);
			m_tOut.m_fMean = v(0);
			m_bCalcVar = FALSE;
			return;
		}
		//������
		if (bCalcMaxMin)
			v.sumvarmaxmin(m_tOut.m_fSmy,m_tOut.m_fS2,m_tOut.m_fMax,m_tOut.m_fMin);
		else
			v.sumvar(m_tOut.m_fSmy,m_tOut.m_fS2);
		m_tOut.m_fSum = v.vlen(); 
	}
	m_tOut.m_fS  = sqrt(fabs(m_tOut.m_fS2));
	if (m_tOut.m_fSum > DOUBLE_ZERO)
	{
		m_tOut.m_fMean = m_tOut.m_fSmy/m_tOut.m_fSum;
		m_tOut.m_fMe   = sqrt(fabs(m_tOut.m_fS2/m_tOut.m_fSum));
	}
}

void CStatiMethod::CalcFldInfo(double fValue, double fVarValue, CPtrArray *pArrFld, int nSize)
{
	CFldInfo *pFld = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArrFld->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pFld = (CFldInfo *)pArrFld->GetAt(nMid);
		fTmp = pFld->fValue-fValue;
		if (fTmp < -DOUBLE_ZERO)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else
		if (fTmp > DOUBLE_ZERO)
		{
			nEnd = nMid-1;
		}
		else
		{
			pFld->nIndex ++;
			pFld->v(pFld->nIndex) = fVarValue;
			return;
		}
	}
	//����
	pFld = new CFldInfo;
	pFld->fValue  = fValue;
	//��������
	pFld->nIndex = 0;
	pFld->v.create(nSize);
	pFld->v(pFld->nIndex) = fVarValue;
	//����
	pArrFld->InsertAt(nMid,pFld);
}

void CStatiMethod::CalcFldInfo(double fValue, double fVarValue, double fWeight, CPtrArray *pArrFld, int nSize)
{
	CFldInfo *pFld = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArrFld->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pFld = (CFldInfo *)pArrFld->GetAt(nMid);
		fTmp = pFld->fValue-fValue;
		if (fTmp < -DOUBLE_ZERO)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else
		if (fTmp > DOUBLE_ZERO)
		{
			nEnd = nMid-1;
		}
		else
		{
			pFld->nIndex ++;
			pFld->v(pFld->nIndex) = fVarValue;
			pFld->w(pFld->nIndex) = fWeight;
			pFld->fWeight += fWeight;
			return;
		}
	}
	//����
	pFld = new CFldInfo;
	pFld->fValue  = fValue;
	pFld->fWeight = fWeight;
	//��������
	pFld->nIndex = 0;
	pFld->v.create(nSize);
	pFld->w.create(nSize);
	pFld->v(pFld->nIndex) = fVarValue;
	pFld->w(pFld->nIndex) = fWeight;
	//����
	pArrFld->InsertAt(nMid,pFld);
}

void CStatiMethod::CalcFldInfo(double fValue, double fWeight, CPtrArray *pArrFld)
{
	CFldInfo *pFld = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArrFld->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pFld = (CFldInfo *)pArrFld->GetAt(nMid);
		fTmp = pFld->fValue-fValue;
		if (fTmp < -DOUBLE_ZERO)
		{
			nStart = nMid+1;
			nMid += 1;
		}
		else
		if (fTmp > DOUBLE_ZERO)
		{
			nEnd = nMid-1;
		}
		else
		{
			pFld->fWeight += fWeight;
			return;
		}
	}
	//����
	pFld = new CFldInfo;
	pFld->fValue  = fValue;
	pFld->fWeight = fWeight;
	//����
	pArrFld->InsertAt(nMid,pFld);
}

void CStatiMethod::CalcRangeInfo(double fValue, double fWeight, CPtrArray *pArrFld)
{
	CRangeInfo *pRange = NULL;
	double fTmp = 0;
	int nStart = 0;
	int nEnd = pArrFld->GetSize()-1;//Array������
	int nMid = 0;
	while (nStart <= nEnd)
	{
		nMid = (nEnd + nStart)/2;
		pRange = (CRangeInfo *)pArrFld->GetAt(nMid);
		if (pRange->bRange)
		{
			if (fValue >= pRange->fValTo + DOUBLE_ZERO)
			{
				nStart = nMid+1;
				nMid += 1;
			}
			else
				if (fValue < pRange->fValFrom-DOUBLE_ZERO)
				{
					nEnd = nMid-1;
				}
				else
				{
					pRange->fTotalWeight += fWeight;
					return;
				}
		}
		else
		{
			fTmp = pRange->fValFrom-fValue;
			if (fTmp < -DOUBLE_ZERO)
			{
				nStart = nMid+1;
				nMid += 1;
			}
			else
				if (fTmp > DOUBLE_ZERO)
				{
					nEnd = nMid-1;
				}
				else
				{
					pRange->fTotalWeight += fWeight;
					return;
				}
		}
	}
	//����
	pRange = new CRangeInfo;
	pRange->bRange = FALSE;
	pRange->fValFrom = fValue;
	pRange->fTotalWeight = fWeight;
	//����
	pArrFld->InsertAt(nMid,pRange);
}

//����Ƶ������Ȩ�أ�-�Ƿ񳬳���ʾ��Χ����������ʾ
BOOL CStatiMethod::CalcFrequency(CDoubleVector &v, CDoubleVector &w, int nColIndex, CPtrArray &arrFld, int nDispMax, BOOL bCalc)
{	
	int i = 0, nVlen = 0;
	double fTmp = 0,fWeight = 0;
	double fM1=0,fM2=0,fM3=0,fM4=0,fPow=0;
	//�Ƿ�Ϊ�ַ���
	if (m_pDataInterface->GetFieldType(nColIndex) == fString)
		bCalc = FALSE;
	//��ά��
	nVlen = v.vlen();
	//��ʼ����
	if (m_bWeight) //Ȩ��
	{
		if (bCalc) //����
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp    = v(i);
				fWeight = w(i);
				//����ƫֵ����ֵ
				fPow = pow(fTmp-m_tOut.m_fMean,2);
				fM1 += fWeight * pow(fTmp-m_tOut.m_fMean,3);
				fM2 += fWeight * fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM3 += fWeight * fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM4 += fWeight * fPow;
				//����
				CalcFldInfo(fTmp,fWeight,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
		else  //������
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp    = v(i);
				fWeight = w(i);
				//����
				CalcFldInfo(fTmp,fWeight,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
	}
	else  //��Ȩ��
	{
		if (bCalc) //����
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp = v(i);
				//����ƫֵ����ֵ
				fPow = pow(fTmp-m_tOut.m_fMean,2);
				fM1 += pow(fTmp-m_tOut.m_fMean,3);
				fM2 += fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM3 += fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM4 += fPow;
				//����
				CalcFldInfo(fTmp,1,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
		else  //������
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp = v(i);
				//����
				CalcFldInfo(fTmp,1,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
	}
	if (bCalc) //����ƫֵ����ֵ���Ƿ����
	{
		//ƫ��ϵ�����
		m_bOutPartial  = TRUE;
		m_bOutPartialE = TRUE;
		m_bOutApex     = TRUE;
		m_bOutApexE    = TRUE;
		if (m_tOut.m_fSum-2.0 == 0)
		{
			m_bOutPartial  = FALSE;
			m_bOutPartialE = FALSE;
			m_bOutApex     = FALSE;
			m_bOutApexE    = FALSE;
		}
		else
		{
			m_bOutPartial = TRUE;
			m_tOut.m_fPartialE = sqrt((m_tOut.m_fSum*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-2.0)*(m_tOut.m_fSum+1.0)*(m_tOut.m_fSum+3.0)/6.0));
			if (m_tOut.m_fSum-3.0 == 0)
			{
				m_bOutApexE = FALSE;
				m_bOutApex  = FALSE;
			}
			else
			{
				//���
				m_tOut.m_fApexE = sqrt((4.0*(m_tOut.m_fSum*m_tOut.m_fSum-1.0)*m_tOut.m_fPartialE*m_tOut.m_fPartialE)/((m_tOut.m_fSum-3.0)*(m_tOut.m_fSum+5.0)));
				if (m_tOut.m_fSum-1.0 == 0)
				{
					m_bOutApex    = FALSE;
					m_bOutPartial = FALSE;
				}
				else
				{
					m_tOut.m_fApex    = (m_tOut.m_fSum*(m_tOut.m_fSum+1.0)*fM4-3.0*fM2*fM2*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*(m_tOut.m_fSum-3.0)*pow(m_tOut.m_fS,4.0))+3.0;
					m_tOut.m_fPartial = (m_tOut.m_fSum*fM1)/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*pow(m_tOut.m_fS,3.0));
				}
			}
		}
	}
	return TRUE;
}

//��������ʱ������Ƶ��
BOOL CStatiMethod::CalcFrequency(CDoubleVector &v, CDoubleVector &w, CIntVector &vIndex, int nColIndex, CPtrArray &arrFld, int nDispMax)
{
	BOOL bCalc = TRUE, bNotFindMid = TRUE;
	int i = 0, nVlen = 0;
	double fTmp = 0, fValue = 0, fWeight = 0;
	double fM1=0,fM2=0,fM3=0,fM4=0,fPow=0;
	double  fFreWeight = 0.0, fFreTotalWeight = 0.0, fOldFreWeight = 0.0;
	//�Ƿ�Ϊ�ַ���
	if (m_pDataInterface->GetFieldType(nColIndex) == fString)
		bCalc = FALSE;
	//��ά��
	nVlen = v.vlen();
	//��ʼ����
	if (m_bWeight) //Ȩ��
	{
		if (bCalc) //����
		{
			fValue = v(0);
			for (int i=0;i<nVlen;i++) 
			{
				fTmp    = v(i);
				fWeight = w(vIndex(i)-1);
				//����ƫֵ����ֵ
				fPow = pow(fTmp-m_tOut.m_fMean,2);
				fM1 += fWeight * pow(fTmp-m_tOut.m_fMean,3);
				fM2 += fWeight * fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM3 += fWeight * fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM4 += fWeight * fPow;
				//�õ���λ��������
				if (fValue - fTmp >= -DOUBLE_ZERO &&
					fValue - fTmp <= DOUBLE_ZERO)
				{
					fFreWeight += fWeight;
					fFreTotalWeight += fWeight;
				}
				else
				{
					//��λ��������
					if (bNotFindMid && fFreTotalWeight > m_tOut.m_fSum/2.0)
					{
						m_tOut.m_fMid = fValue;
						bNotFindMid = FALSE;
					}
					if (fFreWeight > fOldFreWeight)
					{
						fOldFreWeight = fFreWeight;
						m_tOut.m_fMany = fValue;
					}
					/*
					else
					if (fFreWeight == fOldFreWeight)
					{
						if (m_tOut.m_fMany > fValue)
							m_tOut.m_fMany = fValue;
					}
					*/
					//�µ�
					fValue    = fTmp;
					fFreWeight = fWeight;
					fFreTotalWeight += fWeight;
				}
				//����
				CalcRangeInfo(fTmp,fWeight,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
		else  //������
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp    = v(i);
				fWeight = w(vIndex(i)-1);
				//����
				CalcRangeInfo(fTmp,fWeight,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
	}
	else  //��Ȩ��
	{
		if (bCalc) //����
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp = v(i);
				//����ƫֵ����ֵ
				fPow = pow(fTmp-m_tOut.m_fMean,2);
				fM1 += pow(fTmp-m_tOut.m_fMean,3);
				fM2 += fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM3 += fPow;
				fPow *= fTmp-m_tOut.m_fMean;
				fM4 += fPow;
				//�õ���λ��������
				if (fValue - fTmp >= -DOUBLE_ZERO &&
					fValue - fTmp <= DOUBLE_ZERO)
				{
					fFreWeight += 1;
					fFreTotalWeight += 1;
				}
				else
				{
					//��λ��������
					if (fFreWeight > fOldFreWeight)
					{
						fOldFreWeight = fFreWeight;
						m_tOut.m_fMany = fValue;
					}
					/*
					else
					if (fFreWeight == fOldFreWeight)
					{
						if (m_tOut.m_fMany > fValue)
							m_tOut.m_fMany = fValue;
					}
					*/
					//�µ�
					fValue    = fTmp;
					fFreWeight = 1;
					fFreTotalWeight += 1;
				}
				//����
				CalcRangeInfo(fTmp,1,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
		else  //������
		{
			for (int i=0;i<nVlen;i++) 
			{
				fTmp = v(i);
				//����
				CalcRangeInfo(fTmp,1,&arrFld);
				if (arrFld.GetSize() > nDispMax) return FALSE;
			}
		}
	}
	if (bCalc) //����ƫֵ����ֵ���Ƿ����
	{
		//��λ��������
		if (m_bWeight)
		{
			if (bNotFindMid && fFreTotalWeight > m_tOut.m_fSum/2.0)
				m_tOut.m_fMid = fValue;
		}
		else
		{
			if (nVlen%2 != 0)
			{
				m_tOut.m_fMid = v(nVlen/2);
			}
			else
			{
				m_tOut.m_fMid = v((nVlen-1)/2) + v(nVlen/2);
				m_tOut.m_fMid /= 2;
			}
		}
		if (fFreWeight > fOldFreWeight)
		{
			m_tOut.m_fMany = fValue;
		}
		/*
		else
		if (fFreWeight > fOldFreWeight)
		{
			if (m_tOut.m_fMany > fValue)
				m_tOut.m_fMany = fValue;
		}
		*/
		//ƫ��ϵ�����
		m_bOutPartial  = TRUE;
		m_bOutPartialE = TRUE;
		m_bOutApex     = TRUE;
		m_bOutApexE    = TRUE;
		if (m_tOut.m_fSum-2.0 == 0)
		{
			m_bOutPartial  = FALSE;
			m_bOutPartialE = FALSE;
			m_bOutApex     = FALSE;
			m_bOutApexE    = FALSE;
		}
		else
		{
			m_bOutPartial = TRUE;
			m_tOut.m_fPartialE = sqrt((m_tOut.m_fSum*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-2.0)*(m_tOut.m_fSum+1.0)*(m_tOut.m_fSum+3.0)/6.0));
			if (m_tOut.m_fSum-3.0 == 0)
			{
				m_bOutApexE = FALSE;
				m_bOutApex  = FALSE;
			}
			else
			{
				//���
				m_tOut.m_fApexE = sqrt((4.0*(m_tOut.m_fSum*m_tOut.m_fSum-1.0)*m_tOut.m_fPartialE*m_tOut.m_fPartialE)/((m_tOut.m_fSum-3.0)*(m_tOut.m_fSum+5.0)));
				if (m_tOut.m_fSum-1.0 == 0)
				{
					m_bOutApex    = FALSE;
					m_bOutPartial = FALSE;
				}
				else
				{
					m_tOut.m_fApex    = (m_tOut.m_fSum*(m_tOut.m_fSum+1.0)*fM4-3.0*fM2*fM2*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*(m_tOut.m_fSum-3.0)*pow(m_tOut.m_fS,4.0))+3.0;
					m_tOut.m_fPartial = (m_tOut.m_fSum*fM1)/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*pow(m_tOut.m_fS,3.0));
				}
			}
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
CDescriptive::CDescriptive()
{
	int *pValue=new int[8];
	pValue[0]=0; //����
	pValue[1]=1; //��Сֵ
	pValue[2]=2; //���ֵ
	pValue[3]=4; //�ܺ�
	pValue[4]=5; //ƽ��ֵ
	pValue[5]=6; //��ֵ��׼��
	pValue[6]=7; //��׼��
	pValue[7]=8; //����
	
	m_tStatMethod=TIntStruct(8,pValue);
	Free(&m_arrLayerConf,TRUE);
}

CDescriptive::~CDescriptive()
{
	m_nCounter = 0;
	m_nColIndex = 0;
	m_tVarSel.DeleteObject();
	m_tStatMethod.DeleteObject();
	Free(&m_arrLayerConf,TRUE);
	m_szArrLayerName.RemoveAll();
}
//overload.����Ϊarray��������ڴ��޷��ͷš�
void CDescriptive::Free(CPtrArray *pArr, BOOL bLayer)
{
	if (bLayer) //�����߼��ж�
	{
		for(int i=0;i<pArr->GetSize();i++)
		{
			delete ((CDesLayerConf *)pArr->GetAt(i));
		}
	}
	else
	{
		for(int i=0;i<pArr->GetSize();i++)
		{
			delete ((TDataSet *)pArr->GetAt(i));
		}
	}
	pArr->RemoveAll();
}

void CDescriptive::Output(CTLTable *pTable, int nCol)
{
	//���
	//Ȩ�غͻ��¼��
	pTable->SetItemText(m_nCurRows, ++nCol, int(m_tOut.m_fSum));
	for (int j=0;j<m_tStatMethod.iCount;j++)
	{
		int nID = m_tStatMethod.pValue[j];
		switch (nID)
		{
		case 0://����
			pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fMax-m_tOut.m_fMin);
			break;
		case 1://��Сֵ
			pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fMin);
			break;
		case 2://���ֵ
			pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fMax);
			break;
		case 4://�ܺ�
			pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fSmy);
			break;
		case 5://ƽ��ֵ
			if (m_tOut.m_fSum > DOUBLE_ZERO)
				pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fMean);
			break;
		case 6://��ֵ��׼��
			if (m_tOut.m_fSum > DOUBLE_ZERO && m_bCalcVar)		
				pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fMe);
			break;
		case 7://��׼��
			if (m_bCalcVar)
				pTable->SetItemText(m_nCurRows, ++nCol, m_tOut.m_fS);
			break;
		case 8://����
			if (m_bCalcVar)
				pTable->SetItemText(m_nCurRows, ++nCol,  m_tOut.m_fS2);
			break;
		default:
			break;
		}
	}
}
//when no Layer(group),we calc the value,then output...
//split no weight and have weight.otherwise affect speed.
void CDescriptive::DispResultOfNoLayer()
{
	int nCol = 0, nColIndex = 0;
	double fMin = 0.0, fMax = 0.0;
	double fSum = 0.0, fSmy = 0.0;
	double fMean = 0.0, fS2  = 0.0, fS   = 0.0, fMe = 0.0;
	CDWordArray arrCol;
	CDoubleMatrix dataMatrix;
	
	m_nCurRows = 0;
	CTLTable *pTable = new CTLTable;
	pTable->CreateTable(2, 2);//Create
	pTable->SetTitle(_T("��ֵ����"));
	pTable->SetCols(m_tStatMethod.iCount+2);
	pTable->InsertColumn(0,"");
	//�Ƿ���Ȩ��
	if (m_bWeight)
	{
		pTable->InsertColumn(1,_T("Ȩ�غ�"));
	}
	else
	{
		pTable->InsertColumn(1,_T("��Ч��¼��"));
	}
	//ͳ�Ʒ�������
	for (int iIndex=0; iIndex<m_tStatMethod.iCount; iIndex++)
	{
		int id = m_tStatMethod.pValue[iIndex];
		pTable->InsertColumn(iIndex+2,m_tStatMethodS[id].name);
	}
	//����һ���������Ķ���
	CRsltVariable *pETable = new CRsltElementTable(_T("��ֵ����"),pTable);
	m_pResult->Add(pETable);
	//�ܵļ�¼��
	int nAllCount = m_pDataInterface->m_DataAccess.GetRecordCount();
	for (int i=0; i<m_tVarSel.iCount; i++)
	{
		////////////////////////////////////////
		arrCol.RemoveAll();
		//�Ƿ���Ȩ��
		if (m_bWeight)
		{
			arrCol.Add(m_nWeight);
		}
		nColIndex = m_tVarSel.pValue[i];//ȡ���ֶε��к�
		arrCol.Add(nColIndex);
		//����ȱʡֵɾ��
		int nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning = "ע�� ���� " + m_pDataInterface->GetFieldName(nColIndex) + "û�к��ʵļ�¼���о�ֵ������";
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
			continue;
		}
		//�Ƿ��������
		if (nRowCnt > 1) m_bCalcVar = TRUE;
		else m_bCalcVar = FALSE;
		m_tOut.Init();   //������ʼ��
		//������
		nCol = m_bWeight ? 1 : 0;
		CDoubleVector v1(dataMatrix(nCol));
		CDoubleVector v2(dataMatrix(0));
		CalcParam(v1,v2);
		//���
		nCol = 0;
		//����һ����¼
		pTable->InsertItem(m_nCurRows, m_pDataInterface->GetFieldName(nColIndex));
		Output(pTable,nCol);
		//ȱʧֵ
		if (nRowCnt != nAllCount)
		{
			CTString szWarning("");
			szWarning.Format(" ���� %d ��ȱʧֵ��", nAllCount-nRowCnt);
			szWarning = "ע�� ���� " + m_pDataInterface->GetFieldName(nColIndex) + szWarning;
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
		}
		m_nCurRows ++;
	}
}

//pRetArray:���صĲ����ƣ�nLayer:���
void CDescriptive::ParseLayerName(CTStringArray *pArrRet, int nLayer)
{
	CTString szTemp("");
	CDesLayerConf* pConf = NULL;

	if (nLayer > m_arrLayerConf.GetSize())
	{
		m_nCounter ++;//����һ�þ�̬���������ε��û����.
		if (m_nCounter <= m_nTotalDisp)
		{
			szTemp.Empty();
			for (int i=0;i<pArrRet->GetSize();i++)
			{
				if( i == 0 )
					szTemp = pArrRet->GetAt(i);
				else
					szTemp += STR_SEP + pArrRet->GetAt(i);
			}
			//Add to array.save.
			m_szArrLayerName.Add(szTemp);
		}
		return;
	}
	//��NO.
	if (nLayer < 1)	nLayer = 1;
	pConf = (CDesLayerConf*)m_arrLayerConf.GetAt(nLayer-1);
	//�ò��µı�����Ŀ
	int nFCount = pConf->m_FNameList.GetSize();
	for (int i=0; i<nFCount; i++)
	{
		CTStringArray szArrTemp;//��ʱ����array,for recursion.
		if (pArrRet != NULL)
			szArrTemp.Copy(*pArrRet);
		szArrTemp.Add(pConf->m_FNameList.GetAt(i));
		ParseLayerName(&szArrTemp, nLayer+1);
	}
}

//nLayerIndex����ʾ��ǰ�Ĳ���������
//����ָ��������Ӧ�������������в㿪ʼ��λ�ã���nCurLayer:��ǰָ���Ĳ㡣
int CDescriptive::ParseLayerIndex(int nLayerIndex, int nCurLayer)
{
	int nL1 = 0, nL2 = 0, nL3 = 0;
	int nLayer = m_arrLayerConf.GetSize();
	if (nCurLayer > nLayer+1 || m_nTotalDisp <= 0)
		return nLayerIndex;
	if (nLayer == 1) //��һ��
	{
		return nLayerIndex;
	}
	else
	if (nLayer == 2) //������
	{
		nL1 = ((CDesLayerConf*)m_arrLayerConf.GetAt(0))->m_FNameList.GetSize();
		nL2 = ((CDesLayerConf*)m_arrLayerConf.GetAt(1))->m_FNameList.GetSize();
		if (nCurLayer == 0) 
			return (nLayerIndex/nL2);
		else
			return (nLayerIndex%nL2 + nL1);
	}
	else
	{
		nL1 = ((CDesLayerConf*)m_arrLayerConf.GetAt(0))->m_FNameList.GetSize();
		nL2 = ((CDesLayerConf*)m_arrLayerConf.GetAt(1))->m_FNameList.GetSize();
		nL3 = ((CDesLayerConf*)m_arrLayerConf.GetAt(2))->m_FNameList.GetSize();
		if (nCurLayer == 0)
			return (nLayerIndex/(nL2*nL3));
		else
		if (nCurLayer == 1)
			return ((nLayerIndex%(nL2*nL3))/nL3 + nL1);
		else 
			return (nLayerIndex%nL3 + nL1 + nL2);
	}
}

void CDescriptive::Swap(TDataSet *p1, TDataSet *p2)
{
	TDataSet t;
	t = *p1;
	*p1 = *p2;
	*p2 = t;
}

void CDescriptive::QSortDataSet(CPtrArray *pArrDataSet, int nLow, int nHigh)
{
	int i,j;
    TDataSet t, *p1 = NULL, *p2 = NULL;

	i = nHigh;
	j = nLow;
	t = *(TDataSet *)pArrDataSet->GetAt((int)((i+j)/2));
	do{
		while((*(TDataSet *)pArrDataSet->GetAt(j)) < t)  j++;
		while((*(TDataSet *)pArrDataSet->GetAt(i)) > t)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap((TDataSet *)pArrDataSet->GetAt(i),(TDataSet *)pArrDataSet->GetAt(j));
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSortDataSet(pArrDataSet,nLow,i);
	if(j < nHigh) QSortDataSet(pArrDataSet,j,nHigh);
}

//����.-����
void CDescriptive::SortDataSet(CPtrArray *pArrDataSet)  
{
	QSortDataSet(pArrDataSet,0,pArrDataSet->GetSize()-1);
	return;

    int i=0, j=0, k=0;
    TDataSet t, *p1 = NULL, *p2 = NULL;
	int nLen = pArrDataSet->GetSize();
	k = nLen/2;
	while (k > 0)
	{ 
		for (j=k; j<=nLen-1; j++)
		{
			t = *(TDataSet *)pArrDataSet->GetAt(j); 
			i = j-k;
			while ((i>=0)&&(*(TDataSet *)pArrDataSet->GetAt(i) > t))
			{
				p1 = (TDataSet *)pArrDataSet->GetAt(i+k);
				p2 = (TDataSet *)pArrDataSet->GetAt(i);
				*p1 = *p2;
				i  = i-k;
			}
			p1 = (TDataSet *)pArrDataSet->GetAt(i+k);
			*p1 = t;
		}
		k=k/2;
	}
}

void CDescriptive::Swap(double &f1, double &f2, int &n1, int &n2)
{
	double fT = f1;
	f1 = f2;
	f2 = fT;
	int nT = n1;
	n1 = n2;
	n2 = nT;
}

void CDescriptive::QSortVector(CDoubleVector& dataVector, int *pVectorIndex, int nLow, int nHigh)
{
	int i,j;
	double pt;

	i = nHigh;
	j = nLow;
	pt = dataVector((int)((i+j)/2));
	do{
		while(dataVector(j) < pt)  j++;
		while(dataVector(i) > pt)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap(dataVector(i),dataVector(j),pVectorIndex[i],pVectorIndex[j]);
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSortVector(dataVector,pVectorIndex,nLow,i);
	if(j < nHigh) QSortVector(dataVector,pVectorIndex,j,nHigh);
}

//����.��������-����
void CDescriptive::SortVector(CDoubleVector& dataVector, int *pVectorIndex)   
{
	QSortVector(dataVector,pVectorIndex,0,dataVector.vlen()-1);
	return;

	double fTmp = 0.0;
    int i=0, j=0, k=0, nTmp = 0;
	//�ܳ���
	int nVlen = dataVector.vlen();
	k = nVlen/2;
	while (k > 0)
	{ 
		for (j=k; j<nVlen; j++)
		{
 			fTmp = dataVector[j];
			nTmp = pVectorIndex[j];
			i = j-k;
			while ((i >= 0) && (dataVector[i] > fTmp))
			{
				dataVector[i+k] = dataVector[i];
				pVectorIndex[i+k] = pVectorIndex[i];
				i=i-k;
			}
			dataVector[i+k]   = fTmp;
			pVectorIndex[i+k] = nTmp;
		}
		k=k/2;
	}
}

CTString CDescriptive::GetValueString(CMapManager* pDataMap, int nLayerIndex , double fValue)
{
	int nIndex = 0;
	CTString szTmp("");
	//���з�ʽ��Ȩ��->����->�����
	nIndex = m_nArrInd.GetAt(nLayerIndex);//�ڼ�������
	if (nIndex > -1 && m_pDataInterface->GetFieldType(nIndex) == fString)//�ַ���
	{
		nIndex = m_bWeight ? 1 : 0 + 1 + nLayerIndex;
		szTmp = pDataMap->GetStrValue(nIndex,int(fValue));
	}
	else
	{
		szTmp.Format("%.2f",fValue);
	}
	return szTmp;
}

BOOL CDescriptive::IsOverDisp()
{
	if (m_nCurRows >= DISP_MAX)
	{
		CTString szWarning("");
		szWarning.Format("ע��������ڷ���������̫�󣬲��ʺ�����ǰ�������ֽ�����������ݡ�");
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString(szWarning);
		m_pResult->Add(pWarningTextRslt);
		return TRUE;
	}
	return FALSE;
}

void CDescriptive::InsertTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CFldInfo *pFld, CTLTable *pTable, TIndStep* pT1, TIndStep *pT2, TIndStep *pT3)
{ 
	CTString szTmp1("");
	double fValue1 = 0;
	int nCol = 0;
	
	m_bCalcVar = FALSE;
	m_tOut.Init();   //������ʼ��
	if (m_bWeight) //Ϊ�˼����߼��жϣ�����ж�
	{
		if ( pT1 != NULL && pT2 == NULL ) //��һ��
		{
			if (pT1->nType == 0)
			{
				CalcParam(pFld->v,pFld->w);
			}
			else
			{	CDoubleVector v1(dataMatrix(1));
				CDoubleVector v2(dataMatrix(0));
				CalcParam(v1,v2);
			}
			//����һ����¼
			if (pT1->nType == 0)//�ܵ�
				szTmp1 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(0),pFld->fValue);
			else
				szTmp1.Format("%s",_T("����"));
			pTable->InsertItem(m_nCurRows, szTmp1);
		}
	}
	else //��Ȩ��
	{
		if (pT1 != NULL && pT2 == NULL) //��һ��
		{
			if (pT1->nType == 0)
			{
				CalcParam(pFld->v,pFld->v);
			}
			else
			{
				CDoubleVector v1(dataMatrix(0));
				CDoubleVector v2(dataMatrix(0));
				
				CalcParam(v1,v2);
			}
			//����һ����¼
			if (pT1->nType == 0)//�ܵ�
				szTmp1 = GetLabel(dataMap,1,m_nArrInd.GetAt(0),pFld->fValue);
			else
				szTmp1.Format("%s",_T("����"));
			pTable->InsertItem(m_nCurRows, szTmp1);
		}
	}
	//���
	Output(pTable,nCol);
	//������
	m_nCurRows++;
}

//dataMatrix:���ݾ���,pTable:�����,nValIndex:��Ӧ�������������nLayerDisp:��Ҫ��ʾ�Ĳ�
//pT1:��һ�㿪ʼ����������, pT2:�ڶ��㿪ʼ����������, pT3:�����㿪ʼ���������� 
void CDescriptive::InsertTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable, TIndStep* pT1, TIndStep *pT2, TIndStep *pT3)
{ 
	TDataSet *pDataSec = NULL, *pDataThr = NULL;
	CTString szTmp1(""), szTmp2(""), szTmp3("");
	double fMin = 0.0, fMax = 0.0;
	double fSum = 0.0, fSmy = 0.0;
	double fSsy = 0.0, fCss = 0.0;
	double fS2  = 0.0, fS   = 0.0, fMe = 0.0;
	double fValue1 = 0.0, fValue2 = 0.0, fValue3 = 0.0;
	int i = 0, j = 0, k = 0, nCol = 0, nIndex = 0;

	if (m_bWeight) //Ϊ�˼����߼��жϣ�����ж�
	{
		if ( pT1 != NULL && pT2 == NULL ) //��һ��
		{
			//��ֵ
			fMin = fMax = dataMatrix(1)(m_FirVecInd[pT1->nStartInd-pT1->nStep]);
			fValue1 = dataMatrix(1)(pT1->nStartInd-pT1->nStep);//������������
			for (i=pT1->nStartInd-pT1->nStep;i<pT1->nStartInd;i++)
			{
				//Ȩ�غ�
				fSum += dataMatrix(0)(m_FirVecInd[i]);
				//Max-Min
				if (fMin > dataMatrix(1)(m_FirVecInd[i]))
					fMin = dataMatrix(1)(m_FirVecInd[i]);
				if (fMax < dataMatrix(1)(m_FirVecInd[i]))
					fMax = dataMatrix(1)(m_FirVecInd[i]);
				//W*Y
				fSmy += dataMatrix(0)(m_FirVecInd[i])*dataMatrix(1)(m_FirVecInd[i]);
				//W*Y*Y
				fSsy += dataMatrix(0)(m_FirVecInd[i])*pow(dataMatrix(1)(m_FirVecInd[i]), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼
			if (pT1->nType == 0)//�ܵ�
				szTmp1 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
				szTmp1.Format("%s",_T("����"));
			pTable->InsertItem(m_nCurRows, szTmp1);
		}
		else
		if (pT2 != NULL && pT3 == NULL)//������
		{
			pDataSec = (TDataSet *)pT2->arrData.GetAt(pT2->nStartInd-pT2->nStep);
			//��ֵ
			fMin = fMax = dataMatrix(1)(pDataSec->iIndex);
			fValue1 = dataMatrix(2)(pT1->nStartInd-pT1->nStep);
			fValue2 = dataMatrix(3)(pDataSec->iIndex);
			for (i=pT2->nStartInd-pT2->nStep;i<pT2->nStartInd;i++)
			{
				pDataSec = (TDataSet *)pT2->arrData.GetAt(i);
				//Ȩ�غ�
				fSum += dataMatrix(0)(pDataSec->iIndex);
				//Max-Min
				if (fMin > dataMatrix(1)(pDataSec->iIndex))
					fMin = dataMatrix(1)(pDataSec->iIndex);
				if (fMax < dataMatrix(1)(pDataSec->iIndex))
					fMax = dataMatrix(1)(pDataSec->iIndex);
				//W*Y
				fSmy += dataMatrix(0)(pDataSec->iIndex)*dataMatrix(1)(pDataSec->iIndex);
				//W*Y*Y
				fSsy += dataMatrix(0)(pDataSec->iIndex)*pow(dataMatrix(1)(pDataSec->iIndex), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼-�ܵ�
			if (pT1->nType == 0 )
				szTmp1 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
			if (pT1->nType == 1 )
				szTmp1.Format("%s",_T("����"));
			else
				szTmp1.Empty();
			if (pT2->nType == 0 )
				szTmp2 = GetLabel(dataMap,1+2,m_nArrInd.GetAt(1),fValue2);
				//szTmp2 = GetValueString(&dataMap,1,fValue2);
			else
			if (pT2->nType == 1 )
				szTmp2.Format("%s",_T("����"));
			else
				szTmp2.Empty();
			pTable->InsertItem(m_nCurRows, szTmp1);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp2);
		}
		else
		{
			pDataSec = (TDataSet *)pT2->arrData.GetAt(pT2->nStartInd-pT2->nStep);
			pDataThr = (TDataSet *)pT3->arrData.GetAt(pT3->nStartInd-pT3->nStep);
			//��ֵ
			fMin = fMax = dataMatrix(1)(pDataThr->iIndex);
			fValue1 = dataMatrix(2)(pT1->nStartInd-pT1->nStep);
			fValue2 = dataMatrix(3)(pDataSec->iIndex);
			fValue3 = dataMatrix(4)(pDataThr->iIndex);
			for (i=pT3->nStartInd-pT3->nStep;i<pT3->nStartInd;i++)
			{
				pDataThr = (TDataSet *)pT3->arrData.GetAt(i);
				//Ȩ�غ�
				fSum += dataMatrix(0)(pDataThr->iIndex);
				//Max-Min
				if (fMin > dataMatrix(1)(pDataThr->iIndex))
					fMin = dataMatrix(1)(pDataThr->iIndex);
				if (fMax < dataMatrix(1)(pDataThr->iIndex))
					fMax = dataMatrix(1)(pDataThr->iIndex);
				//W*Y
				fSmy += dataMatrix(0)(pDataThr->iIndex)*dataMatrix(1)(pDataThr->iIndex);
				//W*Y*Y
				fSsy += dataMatrix(0)(pDataThr->iIndex)*pow(dataMatrix(1)(pDataThr->iIndex), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼-�ܵ�
			if (pT1->nType == 0 )
				szTmp1 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
			if (pT1->nType == 1 )
				szTmp1.Format("%s",_T("����"));
			else
				szTmp1.Empty();
			if (pT2->nType == 0 )
				szTmp2 = GetLabel(dataMap,1+2,m_nArrInd.GetAt(1),fValue2);
				//szTmp2 = GetValueString(&dataMap,1,fValue2);
			else
			if (pT2->nType == 1 )
				szTmp2.Format("%s",_T("����"));
			else
				szTmp2.Empty();
			if (pT3->nType == 0 )
				szTmp3 = GetLabel(dataMap,1+3,m_nArrInd.GetAt(2),fValue3);
				//szTmp3 = GetValueString(&dataMap,2,fValue3);
			else
			if (pT3->nType == 1 )
				szTmp3.Format("%s",_T("����"));
			else
				szTmp3.Empty();
			pTable->InsertItem(m_nCurRows, szTmp1);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp2);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp3);
		}
	}
	else //��Ȩ��
	{
		if (pT1 != NULL && pT2 == NULL) //��һ��
		{
			//��ֵ
			fMin = fMax = dataMatrix(0)(m_FirVecInd[pT1->nStartInd-pT1->nStep]);
			fValue1 = dataMatrix(1)(pT1->nStartInd-pT1->nStep);
			for (i=pT1->nStartInd-pT1->nStep;i<pT1->nStartInd;i++)
			{
				//��¼��
				fSum += 1;
				//Max-Min
				if (fMin > dataMatrix(0)(m_FirVecInd[i]))
					fMin = dataMatrix(0)(m_FirVecInd[i]);
				if (fMax < dataMatrix(0)(m_FirVecInd[i]))
					fMax = dataMatrix(0)(m_FirVecInd[i]);
				//W*Y
				fSmy += dataMatrix(0)(m_FirVecInd[i]);
				//W*Y*Y
				fSsy += pow(dataMatrix(0)(m_FirVecInd[i]), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼
			if (pT1->nType == 0)//�ܵ�
				szTmp1 = GetLabel(dataMap,1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
				szTmp1.Format("%s",_T("����"));
			pTable->InsertItem(m_nCurRows, szTmp1);
		}
		else
		if (pT2 != NULL && pT3 == NULL ) //������
		{
			pDataSec = (TDataSet *)pT2->arrData.GetAt(pT2->nStartInd-pT2->nStep);
			//��ֵ
			fMin = fMax = dataMatrix(0)(pDataSec->iIndex);
			fValue1 = dataMatrix(1)(pT1->nStartInd-pT1->nStep);
			fValue2 = dataMatrix(2)(pDataSec->iIndex);
			for (i=pT2->nStartInd-pT2->nStep;i<pT2->nStartInd;i++)
			{
				pDataSec = (TDataSet *)pT2->arrData.GetAt(i);
				//��¼��
				fSum += 1;
				//Max-Min
				if (fMin > dataMatrix(0)(pDataSec->iIndex))
					fMin = dataMatrix(0)(pDataSec->iIndex);
				if (fMax < dataMatrix(0)(pDataSec->iIndex))
					fMax = dataMatrix(0)(pDataSec->iIndex);
				//W*Y
				fSmy += dataMatrix(0)(pDataSec->iIndex);
				//W*Y*Y
				fSsy += pow(dataMatrix(0)(pDataSec->iIndex), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼-�ܵ�
			if (pT1->nType == 0 )
				szTmp1 = GetLabel(dataMap,1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
			if (pT1->nType == 1 )
				szTmp1.Format("%s",_T("����"));
			else
				szTmp1.Empty();
			if (pT2->nType == 0 )
				szTmp2 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(1),fValue2);
				//szTmp2 = GetValueString(&dataMap,1,fValue2);
			else
			if (pT2->nType == 1 )
				szTmp2.Format("%s",_T("����"));
			else
				szTmp2.Empty();
			pTable->InsertItem(m_nCurRows, szTmp1);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp2);
		}
		else //����
		{
			pDataSec = (TDataSet *)pT2->arrData.GetAt(pT2->nStartInd-pT2->nStep);
			pDataThr = (TDataSet *)pT3->arrData.GetAt(pT3->nStartInd-pT3->nStep);
			//��ֵ
			fMin = fMax = dataMatrix(0)(pDataThr->iIndex);
			fValue1 = dataMatrix(1)(pT1->nStartInd-pT1->nStep);
			fValue2 = dataMatrix(2)(pDataSec->iIndex);
			fValue3 = dataMatrix(3)(pDataThr->iIndex);
			for (i=pT3->nStartInd-pT3->nStep;i<pT3->nStartInd;i++)
			{
				pDataThr = (TDataSet *)pT3->arrData.GetAt(i);
				//��¼��
				fSum += 1;
				//Max-Min
				if (fMin > dataMatrix(0)(pDataThr->iIndex))
					fMin = dataMatrix(0)(pDataThr->iIndex);
				if (fMax < dataMatrix(0)(pDataThr->iIndex))
					fMax = dataMatrix(0)(pDataThr->iIndex);
				//W*Y
				fSmy += dataMatrix(0)(pDataThr->iIndex);
				//W*Y*Y
				fSsy += pow(dataMatrix(0)(pDataThr->iIndex), 2.0);
			}
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
			{
				fCss = fSsy - pow(fSmy,2.0)/fSum;
				fS2 = fCss/(fSum-1);
				fS  = sqrt(fS2);
				fMe = fS/sqrt(fSum);
			}
			//����һ����¼-�ܵ�
			if (pT1->nType == 0 )
				szTmp1 = GetLabel(dataMap,1,m_nArrInd.GetAt(0),fValue1);
				//szTmp1 = GetValueString(&dataMap,0,fValue1);
			else
			if (pT1->nType == 1 )
				szTmp1.Format("%s",_T("����"));
			else
				szTmp1.Empty();
			if (pT2->nType == 0 )
				szTmp2 = GetLabel(dataMap,1+1,m_nArrInd.GetAt(1),fValue2);
				//szTmp2 = GetValueString(&dataMap,1,fValue2);
			else
			if (pT2->nType == 1 )
				szTmp2.Format("%s",_T("����"));
			else
				szTmp2.Empty();
			if (pT3->nType == 0 )
				szTmp3 = GetLabel(dataMap,1+2,m_nArrInd.GetAt(2),fValue3);
				//szTmp3 = GetValueString(&dataMap,2,fValue3);
			else
			if (pT3->nType == 1 )
				szTmp3.Format("%s",_T("����"));
			else
				szTmp3.Empty();
			pTable->InsertItem(m_nCurRows, szTmp1);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp2);
			pTable->SetItemText(m_nCurRows, ++nCol, szTmp3);
		}
	}
	//output.
	//Ȩ�غͻ��¼��
	pTable->SetItemText(m_nCurRows, ++nCol, int(fSum));
	for (j=0;j<m_tStatMethod.iCount;j++)
	{
		int nID = m_tStatMethod.pValue[j];
		switch (nID)
		{
		case 0://����
			pTable->SetItemText(m_nCurRows, ++nCol, fMax-fMin);
			break;
		case 1://��Сֵ
			pTable->SetItemText(m_nCurRows, ++nCol, fMin);
			break;
		case 2://���ֵ
			pTable->SetItemText(m_nCurRows, ++nCol, fMax);
			break;
		case 4://�ܺ�
			pTable->SetItemText(m_nCurRows, ++nCol, fSmy);
			break;
		case 5://ƽ��ֵ
			pTable->SetItemText(m_nCurRows, ++nCol, fSmy/(long)fSum);
			break;
		case 6://��ֵ��׼��
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
				pTable->SetItemText(m_nCurRows, ++nCol, fMe);
			break;
		case 7://��׼��
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
				pTable->SetItemText(m_nCurRows, ++nCol, fS);
			break;
		case 8://����
			if (long(fSum) != 1 && (fSum > DoubleZero || fSum < -DoubleZero))
				pTable->SetItemText(m_nCurRows, ++nCol, fS2);
			break;
		default:
			break;
		}
	}
	//������
	m_nCurRows++;
}

//dataMatrix�����ݾ���,pTable�������nValIndex:��Ӧ�ı���������
//nLayerDisp����ʾ����Ϣ,����ʼλ��
void CDescriptive::Output(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable)
{
	int nLayer = m_arrLayerConf.GetSize();
	if (nLayer == 1)// ��һ��
	{
		Output1(dataMatrix,dataMap,pTable);
	}
	else
	if (nLayer == 2)// ������	
	{
		Output2(dataMatrix,dataMap,pTable);
	}
	else            // ����
	{
		Output3(dataMatrix,dataMap,pTable);
	}
}

void CDescriptive::Output1(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable)
{
	TIndStep t1;
	double fTmp1 = 0.0;
	double fValue = 0, fVarValue = 0, fWeight = 0;
	int   i1 = 0, nStep1 = 0;
	int nFirStart = 0, nVlen = 0;
	CFldInfo *pFldInfo = NULL;
	CPtrArray arrFld;
	
	//������
	nFirStart = (m_bWeight ? 1 : 0) + 1;//��һ�����ʼλ��	
	//��ֵ
	m_nCurRows = nStep1 = 0;
	nVlen  = dataMatrix(nFirStart).vlen();
	/********************************************************************************/
	if (m_bWeight)
	{
		for (i1=0;i1<nVlen;i1++)
		{
			fValue  = dataMatrix(nFirStart)(i1);
			fVarValue = dataMatrix(nFirStart-1)(i1);
			fWeight = dataMatrix(0)(i1);
			CalcFldInfo(fValue,fVarValue,fWeight,&arrFld,nVlen);//��������
		}
		//����
		nVlen = arrFld.GetSize();
		for (i1=0;i1<nVlen;i1++)
		{
			pFldInfo = (CFldInfo *)arrFld.GetAt(i1);
			pFldInfo->v.resize(pFldInfo->nIndex+1);
			pFldInfo->w.resize(pFldInfo->nIndex+1);
		}
	}
	else
	{
		for (i1=0;i1<nVlen;i1++)
		{
			fValue  = dataMatrix(nFirStart)(i1);
			fVarValue = dataMatrix(nFirStart-1)(i1);
			CalcFldInfo(fValue,fVarValue,&arrFld,nVlen);//��������
		}
		//����
		nVlen = arrFld.GetSize();
		for (i1=0;i1<nVlen;i1++)
		{
			pFldInfo = (CFldInfo *)arrFld.GetAt(i1);
			pFldInfo->v.resize(pFldInfo->nIndex+1);
		}
	}
	nVlen = m_nCurRows;
	m_nCurRows = arrFld.GetSize();
	if (IsOverDisp()) return;//������ʾ��Χ
	m_nCurRows = nVlen;
	/********************************************************************************/
	nVlen = arrFld.GetSize();
	for (i1=0;i1<nVlen;i1++)
	{
		pFldInfo = (CFldInfo *)arrFld.GetAt(i1);
		t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 0;//��ʾ����
		InsertTable(dataMatrix,dataMap,pFldInfo,pTable,&t1);
	}
	//�ܵ�
	if (IsOverDisp()) return;
	t1.nStartInd = nVlen, t1.nStep = nVlen, t1.nType = 1;//��ʾ����
	InsertTable(dataMatrix,dataMap,pFldInfo,pTable,&t1);
}

void CDescriptive::Output2(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable)
{
	TIndStep t1, t2;
	int   i1 = 0, i2 = 0;
	int   nStep1 = 0, nStep2 = 0;
	int   nVlen = 0, nFirStart = 0, nSecStart = 0;
	double fTmp1 = 0.0, fTmp2 = 0.0;
	BOOL bSecFirst = FALSE;

	CPtrArray arrSecData;   //����ÿ����ͬһ������Ϣ
	CPtrArray arrSecDataSet;//ÿ��ͬһ������Ϣ
	TDataSet *pDataSec = NULL;

	//������
	nFirStart = (m_bWeight ? 1 : 0) + 1;//��һ�����ʼλ��	
	nSecStart = nFirStart + 1;//�ڶ������ʼλ��	
	//��ֵ
	m_nCurRows = nStep1 = 0;
	nVlen  = dataMatrix(nFirStart).vlen();
	fTmp1 = dataMatrix(nFirStart)(0);
	//
	for (i1=0;i1<nVlen;i1++)  //i1->��һ��
	{
		if (fTmp1 == dataMatrix(nFirStart)(i1))
		{
			nStep1 += 1;
			pDataSec = new TDataSet;
			pDataSec->dValue = dataMatrix(nSecStart)(m_FirVecInd[i1]);
			pDataSec->iIndex = m_FirVecInd[i1];//����������
			arrSecData.Add(pDataSec);
			arrSecDataSet.Add(pDataSec);//�����ܵ�
		}
		else
		{
			SortDataSet(&arrSecData);   //����
			///////////////////�����ڶ�����Ϣ//////////////
			nStep2 = 0;
			bSecFirst = TRUE;
			pDataSec = (TDataSet *)arrSecData.GetAt(0);
			fTmp2 = pDataSec->dValue;
			for (i2=0;i2<arrSecData.GetSize();i2++)
			{
				pDataSec = (TDataSet *)arrSecData.GetAt(i2);
				if (fTmp2 == pDataSec->dValue)
				{
					nStep2 += 1;
				}
				else    //���
				{
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
					t2.arrData.RemoveAll();
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;//��ʾ����
					t2.arrData.Copy(arrSecData);
					if (IsOverDisp()) 
					{
						Free(&arrSecDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
					nStep2 = 1;
					bSecFirst = FALSE;
					fTmp2 = pDataSec->dValue;
				}
			}
			//
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = (nStep2 == i2 ? 0 : 2);//��ʾ����?
			t2.arrData.RemoveAll();
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;//��ʾ����
			t2.arrData.Copy(arrSecData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
			t1.nType     = 2;//��ʾ��
			t2.nType     = 1;//��ʾ����
			t2.nStep     = arrSecData.GetSize();
			t2.nType     = true;
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
			///////////////////�����ڶ�����Ϣ//////////////
			arrSecData.RemoveAll();    //���
			pDataSec = new TDataSet;
			pDataSec->dValue = dataMatrix(nSecStart)(m_FirVecInd[i1]);
			pDataSec->iIndex = m_FirVecInd[i1];//����������
			arrSecData.Add(pDataSec);
			arrSecDataSet.Add(pDataSec);//�����ܵ�
			nStep1 = 1;
			fTmp1 = dataMatrix(nFirStart)(i1);
		}
	}
	//ѭ��������...
	SortDataSet(&arrSecData);      //����
	nStep2 = 0;
	bSecFirst = TRUE;
	pDataSec = (TDataSet *)arrSecData.GetAt(0);
	fTmp2 = pDataSec->dValue;
	for (i2=0;i2<arrSecData.GetSize();i2++)
	{
		pDataSec = (TDataSet *)arrSecData.GetAt(i2);
		if (fTmp2 == pDataSec->dValue)
		{
			nStep2 += 1;
		}
		else    //���
		{
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
			t2.arrData.RemoveAll();
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;//��ʾ����
			t2.arrData.Copy(arrSecData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
			nStep2 = 1;
			bSecFirst = FALSE;
			fTmp2 = pDataSec->dValue;
		}
	}
	//
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = (nStep2 == i2 ? 0 : 2);//��ʾ����?
	t2.arrData.RemoveAll();
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;//��ʾ����
	t2.arrData.Copy(arrSecData);
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
	t1.nType     = 2;//��ʾ��
	t2.nType     = 1;//��ʾ����
	t2.nStep     = arrSecData.GetSize();
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
	//�ܵ�
	SortDataSet(&arrSecDataSet);      //����
	nStep2 = 0;
	bSecFirst = TRUE;
	pDataSec = (TDataSet *)arrSecDataSet.GetAt(0);
	fTmp2 = pDataSec->dValue;
	for (i2=0;i2<arrSecDataSet.GetSize();i2++)
	{
		pDataSec = (TDataSet *)arrSecDataSet.GetAt(i2);
		if (fTmp2 == pDataSec->dValue)
		{
			nStep2 += 1;
		}
		else    //���
		{
			t1.nStartInd = nVlen, t1.nStep = nVlen, t1.nType = bSecFirst ? 1 : 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;  //��ʾ��
			t2.arrData.Copy(arrSecDataSet);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
			nStep2 = 1;
			bSecFirst = FALSE;
			fTmp2 = pDataSec->dValue;
		}
	}
	//
	t1.nStartInd = nVlen, t1.nStep = nVlen, t1.nType = (nStep2 == i2 ? 1 : 2);//��ʾ��?
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 0;  //��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
	t1.nType     = 2;//��ʾ��
	t2.nType     = 1;//��ʾ����
	t2.nStep     = arrSecDataSet.GetSize();
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2);
	//�ͷ��ڴ�
	Free(&arrSecDataSet);
}

void CDescriptive::Output3(CDoubleMatrix &dataMatrix, CMapManager &dataMap, CTLTable *pTable)
{
	TIndStep t1, t2, t3;
	int   i1 = 0, i2 = 0, i3 = 0, i31 = 0;
	int   nStep1 = 0, nStep2 = 0,  nStep3 = 0;
	int   nVlen = 0, nFirStart = 0, nSecStart = 0, nThrStart = 0;
	double fTmp1 = 0.0, fTmp2 = 0.0, fTmp3 = 0.0;
	BOOL bSecFirst = FALSE, bThrFirst = FALSE;

	CPtrArray arrSecData;   //����ÿ����ͬһ������Ϣ
	CPtrArray arrThrData;   //����ÿ����ͬһ������Ϣ
	CPtrArray arrSecDataSet;//ÿ��ͬһ������Ϣ
	CPtrArray arrThrDataSet;//ÿ��ͬһ������Ϣ
	CPtrArray arrThrTmpDataSet;//��ʱ���������Ĳ�����Ϣ
	TDataSet *pDataSec = NULL, *pDataThr = NULL;

	//������
	nFirStart = (m_bWeight ? 1 : 0) + 1;//��һ�����ʼλ��	
	nSecStart = nFirStart + 1;//�ڶ������ʼλ��	
	nThrStart = nSecStart + 1;//���������ʼλ��	
	//��ֵ
	m_nCurRows = nStep1 = 0;
	nVlen  = dataMatrix(nFirStart).vlen();
	fTmp1 = dataMatrix(nFirStart)(0);
	//
	for (i1=0;i1<nVlen;i1++)  //i1->��һ��
	{
		if (fTmp1 == dataMatrix(nFirStart)(i1))
		{
			nStep1 += 1;
			pDataSec = new TDataSet;//����ڶ�����Ϣ
			pDataSec->dValue = dataMatrix(nSecStart)(m_FirVecInd[i1]);
			pDataSec->iIndex = m_FirVecInd[i1];//����������
			arrSecData.Add(pDataSec);
			arrSecDataSet.Add(pDataSec);//����ڶ����ܵ���Ϣ
		}
		else
		{
			///////////////////�����ڶ�����Ϣ//////////////
			SortDataSet(&arrSecData);   //����
			nStep2 = 0;
			bSecFirst = TRUE;
			pDataSec = (TDataSet *)arrSecData.GetAt(0);
			fTmp2 = pDataSec->dValue;
			for (i2=0;i2<arrSecData.GetSize();i2++) //second.
			{
				pDataSec = (TDataSet *)arrSecData.GetAt(i2);
				if (fTmp2 == pDataSec->dValue)
				{
					nStep2 += 1;
					pDataThr = new TDataSet;//�����������Ϣ
					pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
					pDataThr->iIndex = pDataSec->iIndex;//����������
					arrThrData.Add(pDataThr);
					arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
					arrThrTmpDataSet.Add(pDataThr);//��ʱ�����������Ϣ
				}
				else    
				{
					///////////////////������������Ϣ//////////////
					SortDataSet(&arrThrData);   //����
					nStep3 = 0;
					bThrFirst = TRUE;
					pDataThr = (TDataSet *)arrThrData.GetAt(0);
					fTmp3 = pDataThr->dValue;
					for (i3=0;i3<arrThrData.GetSize();i3++)
					{
						pDataThr = (TDataSet *)arrThrData.GetAt(i3);
						if (fTmp3 == pDataThr->dValue)
						{
							nStep3 += 1;
						}
						else //���
						{
							t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
							t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
							t2.arrData.RemoveAll();
							t2.arrData.Copy(arrSecData);
							t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
							t3.arrData.RemoveAll();
							t3.arrData.Copy(arrThrData);
							if (IsOverDisp())
							{
								Free(&arrSecDataSet);
								Free(&arrThrDataSet);
								return;
							}
							InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
							nStep3 = 1;
							bThrFirst = FALSE;
							bSecFirst = FALSE;
							fTmp3 = pDataThr->dValue;
						}
					}
					//������ֶν�����
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
					t2.arrData.RemoveAll();
					t2.arrData.Copy(arrSecData);
					t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
					t3.arrData.RemoveAll();
					t3.arrData.Copy(arrThrData);
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					//����һ������
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
					t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					nStep2 = 1;
					bSecFirst = FALSE;
					bThrFirst = FALSE;
					fTmp2 = pDataSec->dValue;
					pDataThr = new TDataSet;//�����������Ϣ
					pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
					pDataThr->iIndex = pDataSec->iIndex;//����������
					arrThrData.RemoveAll();        //���
					arrThrData.Add(pDataThr);
					arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
					arrThrTmpDataSet.Add(pDataThr);//��ʱ�����������Ϣ
				}
			}
			//�ڶ���ֶν�����
			SortDataSet(&arrThrData);   //����
			nStep3 = 0;
			bThrFirst = TRUE;
			pDataThr = (TDataSet *)arrThrData.GetAt(0);
			fTmp3 = pDataThr->dValue;
			for (i3=0;i3<arrThrData.GetSize();i3++)
			{
				pDataThr = (TDataSet *)arrThrData.GetAt(i3);
				if (fTmp3 == pDataThr->dValue)
				{
					nStep3 += 1;
				}
				else //���
				{
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
					t2.arrData.RemoveAll();
					t2.arrData.Copy(arrSecData);
					t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
					t3.arrData.RemoveAll();
					t3.arrData.Copy(arrThrData);
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					nStep3 = 1;
					bThrFirst = FALSE;
					bSecFirst = FALSE;
					fTmp3 = pDataThr->dValue;
				}
			}
			//������ֶν�����
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
			t2.arrData.RemoveAll();
			t2.arrData.Copy(arrSecData);
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			t3.arrData.RemoveAll();
			t3.arrData.Copy(arrThrData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			//����һ������
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
			t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			//�Ե�������о�ֵ����
			nStep3 = 0;
			bThrFirst = TRUE;
			SortDataSet(&arrThrTmpDataSet);   //����
			pDataThr = (TDataSet *)arrThrTmpDataSet.GetAt(0);
			fTmp3 = pDataThr->dValue;
			t3.arrData.Copy(arrThrTmpDataSet);
			for (i3=0;i3<arrThrTmpDataSet.GetSize();i3++)
			{
				pDataThr = (TDataSet *)arrThrTmpDataSet.GetAt(i3);
				if (fTmp3 == pDataThr->dValue)
				{
					nStep3 += 1;
				}
				else //���
				{
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ����?
					t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					nStep3 = 1;
					bThrFirst = FALSE;
					fTmp3 = pDataThr->dValue;
				}
			}
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ��?
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			t2.nType     = 2;//��ʾ��
			t3.nType     = 1;//��ʾ����
			t3.nStep     = arrThrTmpDataSet.GetSize();
			bThrFirst = FALSE;
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			//���
			arrThrData.RemoveAll();
			arrThrTmpDataSet.RemoveAll();
			///////////////////�����ڶ�����Ϣ//////////////
			arrSecData.RemoveAll();    //���
			pDataSec = new TDataSet;
			pDataSec->dValue = dataMatrix(nSecStart)(m_FirVecInd[i1]);
			pDataSec->iIndex = m_FirVecInd[i1];//����������
			arrSecData.Add(pDataSec);
			arrSecDataSet.Add(pDataSec);//�����ܵ�
			nStep1 = 1;
			fTmp1 = dataMatrix(nFirStart)(i1);
		}
	}
	//ѭ��������...
	SortDataSet(&arrSecData);   //����
	nStep2 = 0;
	bSecFirst = TRUE;
	pDataSec = (TDataSet *)arrSecData.GetAt(0);
	fTmp2 = pDataSec->dValue;
	for (i2=0;i2<arrSecData.GetSize();i2++) //second.
	{
		pDataSec = (TDataSet *)arrSecData.GetAt(i2);
		if (fTmp2 == pDataSec->dValue)
		{
			nStep2 += 1;
			pDataThr = new TDataSet;//�����������Ϣ
			pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
			pDataThr->iIndex = pDataSec->iIndex;//����������
			arrThrData.Add(pDataThr);
			arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
			arrThrTmpDataSet.Add(pDataThr);//��ʱ�����������Ϣ
		}
		else    //���
		{
			///////////////////������������Ϣ//////////////
			SortDataSet(&arrThrData);   //����
			nStep3 = 0;
			bThrFirst = TRUE;
			pDataThr = (TDataSet *)arrThrData.GetAt(0);
			fTmp3 = pDataThr->dValue;
			for (i3=0;i3<arrThrData.GetSize();i3++)
			{
				pDataThr = (TDataSet *)arrThrData.GetAt(i3);
				if (fTmp3 == pDataThr->dValue)
				{
					nStep3 += 1;
				}
				else //���
				{
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
					t2.arrData.RemoveAll();
					t2.arrData.Copy(arrSecData);
					t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
					t3.arrData.RemoveAll();
					t3.arrData.Copy(arrThrData);
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					nStep3 = 1;
					bThrFirst = FALSE;
					bSecFirst = FALSE;
					fTmp3 = pDataThr->dValue;
				}
			}
			//������ֶν�����
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
			t2.arrData.RemoveAll();
			t2.arrData.Copy(arrSecData);
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			t3.arrData.RemoveAll();
			t3.arrData.Copy(arrThrData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			//����һ������
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
			t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
			if (IsOverDisp()) 
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep2 = 1;
			bSecFirst = FALSE;
			bThrFirst = FALSE;
			fTmp2 = pDataSec->dValue;
			pDataThr = new TDataSet;//�����������Ϣ
			pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
			pDataThr->iIndex = pDataSec->iIndex;//����������
			arrThrData.RemoveAll();        //���
			arrThrData.Add(pDataThr);
			arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
			arrThrTmpDataSet.Add(pDataThr);//��ʱ�����������Ϣ
		}
	}
	//�ڶ���ֶν�����
	SortDataSet(&arrThrData);   //����
	nStep3 = 0;
	bThrFirst = TRUE;
	pDataThr = (TDataSet *)arrThrData.GetAt(0);
	fTmp3 = pDataThr->dValue;
	for (i3=0;i3<arrThrData.GetSize();i3++)
	{
		pDataThr = (TDataSet *)arrThrData.GetAt(i3);
		if (fTmp3 == pDataThr->dValue)
		{
			nStep3 += 1;
		}
		else //���
		{
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
			t2.arrData.RemoveAll();
			t2.arrData.Copy(arrSecData);
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			t3.arrData.RemoveAll();
			t3.arrData.Copy(arrThrData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep3 = 1;
			bThrFirst = FALSE;
			fTmp3 = pDataThr->dValue;
		}
	}
	//������ֶν�����
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 0 : 2;//��ʾ����?
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
	t2.arrData.RemoveAll();
	t2.arrData.Copy(arrSecData);
	t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
	t3.arrData.RemoveAll();
	t3.arrData.Copy(arrThrData);
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	//����һ������
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
	t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	//�Ե�������о�ֵ����
	nStep3 = 0;
	bThrFirst = TRUE;
	SortDataSet(&arrThrTmpDataSet);   //����
	pDataThr = (TDataSet *)arrThrTmpDataSet.GetAt(0);
	fTmp3 = pDataThr->dValue;
	t3.arrData.Copy(arrThrTmpDataSet);
	for (i3=0;i3<arrThrTmpDataSet.GetSize();i3++)
	{
		pDataThr = (TDataSet *)arrThrTmpDataSet.GetAt(i3);
		if (fTmp3 == pDataThr->dValue)
		{
			nStep3 += 1;
		}
		else //���
		{
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ����?
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep3 = 1;
			bThrFirst = FALSE;
			fTmp3 = pDataThr->dValue;
		}
	}
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ��?
	t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	t2.nType     = 2;//��ʾ��
	t3.nType     = 1;//��ʾ����
	t3.nStep     = arrThrTmpDataSet.GetSize();
	bThrFirst = FALSE;
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	//���
	arrThrData.RemoveAll();
	arrThrTmpDataSet.RemoveAll();
	Free(&arrThrDataSet);//Release memory
	/////////////////////Begin�ڶ���///////////////////////
	nStep2 = 0, bSecFirst = TRUE;
	nStep3 = 0, bThrFirst = TRUE;
	t2.arrData.Copy(arrSecDataSet);
	SortDataSet(&arrSecDataSet);   //����
	pDataSec = (TDataSet *)arrSecDataSet.GetAt(0);
	fTmp2 = pDataSec->dValue;
	for (i2=0;i2<arrSecDataSet.GetSize();i2++)
	{
		pDataSec = (TDataSet *)arrSecDataSet.GetAt(i2);
		if (fTmp2 == pDataSec->dValue)
		{
			nStep2 += 1;
			pDataThr = new TDataSet;//�����������Ϣ
			pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
			pDataThr->iIndex = pDataSec->iIndex;//����������
			arrThrData.Add(pDataThr);
			arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
		}
		else
		{
			SortDataSet(&arrThrData);   //����-������������Ϣ
			nStep3 = 0;
			bThrFirst = TRUE;
			pDataThr = (TDataSet *)arrThrData.GetAt(0);
			fTmp3 = pDataThr->dValue;
			for (i3=0;i3<arrThrData.GetSize();i3++)
			{
				pDataThr = (TDataSet *)arrThrData.GetAt(i3);
				if (fTmp3 == pDataThr->dValue)
				{
					nStep3 += 1;
				}
				else //���
				{
					t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 1 : 2;//��ʾ����?
					t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
					t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
					t3.arrData.RemoveAll();
					t3.arrData.Copy(arrThrData);
					if (IsOverDisp())
					{
						Free(&arrSecDataSet);
						Free(&arrThrDataSet);
						return;
					}
					InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
					nStep3 = 1;
					bThrFirst = FALSE;
					bSecFirst = FALSE;
					fTmp3 = pDataThr->dValue;
				}
			}
			//������ֶν�����
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 1 : 2;//��ʾ����?
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			t3.arrData.RemoveAll();
			t3.arrData.Copy(arrThrData);
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			//����һ������
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
			t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep2 = 1;
			bSecFirst = FALSE;
			bThrFirst = FALSE;
			fTmp2 = pDataSec->dValue;
			pDataThr = new TDataSet;//�����������Ϣ
			pDataThr->dValue = dataMatrix(nThrStart)(pDataSec->iIndex);
			pDataThr->iIndex = pDataSec->iIndex;//����������
			arrThrData.RemoveAll();        //���
			arrThrData.Add(pDataThr);
			arrThrDataSet.Add(pDataThr);   //����������ܵ���Ϣ
			arrThrTmpDataSet.Add(pDataThr);//��ʱ�����������Ϣ
		}
	}
	//ѭ��������
	SortDataSet(&arrThrData);   //����-������������Ϣ
	nStep3 = 0, bThrFirst = TRUE;
	t3.arrData.Copy(arrThrData);
	pDataThr = (TDataSet *)arrThrData.GetAt(0);
	fTmp3 = pDataThr->dValue;
	for (i3=0;i3<arrThrData.GetSize();i3++)
	{
		pDataThr = (TDataSet *)arrThrData.GetAt(i3);
		if (fTmp3 == pDataThr->dValue)
		{
			nStep3 += 1;
		}
		else //���
		{
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 1 : 2;//��ʾ����?
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			if (IsOverDisp())
			{
				Free(&arrSecDataSet);
				Free(&arrThrDataSet);
				return;
			}
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep3 = 1;
			bThrFirst = FALSE;
			bSecFirst = FALSE;
			fTmp3 = pDataThr->dValue;
		}
	}
	//������ֶν�����
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = bSecFirst ? 1 : 2;//��ʾ����?
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 0 : 2;//��ʾ����?
	t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	//����һ������
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
	t3.nStartInd = i3, t3.nStep = arrThrData.GetSize(), t3.nType = 1;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	arrThrData.RemoveAll();//���
	/////////////////////End�ڶ���/////////////////////////
	/////////////////////Begin������///////////////////////
	bSecFirst = TRUE;
	nStep3 = 0, bThrFirst = TRUE;
	SortDataSet(&arrThrDataSet);   //����
	t2.arrData.Copy(arrSecDataSet);
	t3.arrData.Copy(arrThrDataSet);
	pDataThr = (TDataSet *)arrThrDataSet.GetAt(0);
	fTmp3 = pDataThr->dValue;
	for (i3=0;i3<arrThrDataSet.GetSize();i3++)
	{
		pDataThr = (TDataSet *)arrThrDataSet.GetAt(i3);
		if (fTmp3 == pDataThr->dValue)
		{
			nStep3 += 1;
		}
		else //���
		{
			t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
			t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ������
			t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
			InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
			nStep3 = 1;
			bThrFirst = FALSE;
			bSecFirst = FALSE;
			fTmp3 = pDataThr->dValue;
		}
	}
	//������ֶν�����
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = bThrFirst ? 1 : 2;//��ʾ������
	t3.nStartInd = i3, t3.nStep = nStep3, t3.nType = 0;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	//����һ������
	t1.nStartInd = i1, t1.nStep = nStep1, t1.nType = 2;//��ʾ��
	t2.nStartInd = i2, t2.nStep = nStep2, t2.nType = 2;//��ʾ��
	t3.nStartInd = i3, t3.nStep = arrThrDataSet.GetSize(), t3.nType = 1;//��ʾ����
	if (IsOverDisp())
	{
		Free(&arrSecDataSet);
		Free(&arrThrDataSet);
		return;
	}
	InsertTable(dataMatrix,dataMap,pTable,&t1,&t2,&t3);
	/////////////////////End������/////////////////////////
	Free(&arrSecDataSet);
	Free(&arrThrDataSet);
}

void CDescriptive::GetLayerName(CTStringArray *pArrName, int nLayerDisp)
{
	CTString szTemp("");
	int nStart = 0, nFind = 0;
	if (!pArrName) return;
	if (nLayerDisp < 0 ||
		nLayerDisp >= m_szArrLayerName.GetSize()) return;
	pArrName->RemoveAll();
	//parse
	szTemp = m_szArrLayerName.GetAt(nLayerDisp);
	nFind = szTemp.Find(STR_SEP);
	if (nFind == -1 && strlen(szTemp))
		pArrName->Add(_T(szTemp));
	while (nFind != -1)
	{
		pArrName->Add(_T(szTemp.Mid(nStart,nFind-nStart)));
		nStart = nFind + 1;
		nFind = szTemp.Find(STR_SEP,nStart);
		if (nFind < 0 && nStart < strlen(szTemp))
			pArrName->Add(_T(szTemp.Mid(nStart)));
	}
}

//when have Layer(group),we calc the value,then output...
void CDescriptive::DispResultOfHaveLayer()
{
	CTString szTemp("");
	CDWordArray arrCol;
	CTStringArray szArrRet;
	int nSize = 0, nCol = 0, nRowCnt = 0, nVlen = 0;
	int nVarSel = 0, nLayerTotal, nLayerDisp = 0;
	CDesLayerConf *pConf = NULL;
	CDoubleMatrix dataMatrix;
	CMapManager dataMap;

	//����
	m_nTotalDisp = 1;
	nLayerTotal = m_arrLayerConf.GetSize();
	if( nLayerTotal < 1 ) 
	{
		DispResultOfNoLayer();
		return;
	}
	int i;
	for (i=0; i<nLayerTotal; i++)
	{
		pConf = (CDesLayerConf*)m_arrLayerConf.GetAt(i);
		m_nTotalDisp *= pConf->m_FNameList.GetSize();
	}
	//��Ų���Ϣarray
	m_nCounter = 0;
	m_szArrLayerName.RemoveAll();
	//�õ�����Ϣ
	ParseLayerName(&szArrRet,0);
	int nAllCount = m_pDataInterface->m_DataAccess.GetRecordCount();
	//����-��ṹ�����
	for (nVarSel=0; nVarSel<m_tVarSel.iCount; nVarSel++)
	{
		nCol = 0;//Col Index.
		m_nColIndex = m_tVarSel.pValue[nVarSel];
		//��������
		for (nLayerDisp=0; nLayerDisp<m_nTotalDisp; nLayerDisp++)
		{
			//������
			GetLayerName(&szArrRet,nLayerDisp);
			//Title.
			szTemp.Format(_T("��ֵ����--%s"),m_pDataInterface->GetFieldName(m_nColIndex).GetData());
			CTLTable *pTable = new CTLTable;
			pTable->CreateTable(2, 2);//Create
			pTable->SetTitle(_T(szTemp));
			//�ܵ�����
			pTable->SetCols(m_tStatMethod.iCount + 1 + nLayerTotal);
			//����һ���������Ķ���
			CRsltVariable *pETable = new CRsltElementTable(_T(szTemp),pTable);
			m_pResult->Add(pETable);
			/////////////////////////////////////////////////////////////////////////////
			for (i=0;i<szArrRet.GetSize();i++)
			{
				pTable->InsertColumn(nCol++,_T(szArrRet.GetAt(i)));
			}
			///////////////////////////////////////////////////////////////////////////
			//�Ƿ���Ȩ��
			if (m_bWeight)
				pTable->InsertColumn(nCol++,_T("Ȩ�غ�"));
			else
				pTable->InsertColumn(nCol++,_T("��¼��"));
			//ͳ�Ʒ�������
			for (int iIndex=0; iIndex<m_tStatMethod.iCount; iIndex++)
			{
				int nID = m_tStatMethod.pValue[iIndex];
				pTable->InsertColumn(nCol++,m_tStatMethodS[nID].name);
			}
			//����
			/////////////////////////////////////////////////////////////////////////////
			dataMap.Clear();
			arrCol.RemoveAll();
			m_nArrInd.RemoveAll();
			m_FirVecInd.destruction();
			dataMatrix.destruction();
			if (m_bWeight)
			{
				arrCol.Add(m_nWeight);
			}
			arrCol.Add(m_nColIndex);
			//������
			for (i=0;i<szArrRet.GetSize();i++)
			{
				int nIndex = m_pDataInterface->IndexByName(szArrRet.GetAt(i));
				arrCol.Add(nIndex);
				m_nArrInd.Add(nIndex);
			}
			//Read
			nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix, dataMap);
			if (nRowCnt < 1)
			{
				CTString szWarning("");
				szWarning.Format("ע�� ����%s-û�к��ʵļ�¼���о�ֵ������",m_pDataInterface->GetFieldName(m_nColIndex).GetData());
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
				pWarningTextRslt->AddString( szWarning );
				m_pResult->Add( pWarningTextRslt );
				continue;
			}
			//��������
			if (nLayerTotal > 1)//����һ������
			{
				nCol = m_bWeight ? 1 : 0 + 1;
				CDoubleVector v(dataMatrix(nCol));
				v.Sort(m_FirVecInd);
				for (i=0;i<m_FirVecInd.vlen();i++)
					m_FirVecInd[i] -= 1;
			}
			///////////////////////////////////////////////////////////////////////////
			//�����nLayerIndex,���ʱ
			Output(dataMatrix, dataMap, pTable);
			//ȱʧֵ
			if (nRowCnt != nAllCount)
			{
				CTString szWarning("");
				szWarning.Format("ע�� ���� %s ���� %d ��ȱʧֵ��",m_pDataInterface->GetFieldName(m_nColIndex).GetData(),nAllCount-nRowCnt);
				CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
				pWarningTextRslt->AddString( szWarning );
				m_pResult->Add( pWarningTextRslt );
			}
		}
	}
}
/*
MEANANALYSE
	/DATA=[FILE]
	/VARLIST [varlsit]
	/BYFIRST [varlsit]
	/BYSECOND [varlsit]
	/BYTHIRD [varlsit]
	/DISPLAY [TOTAL**] [MEAN**] [MIN**] [MAX**] [VAR**] [STDDEV**]
		 [RANGE**] [MEANSTDDEV**]
*/

BOOL CDescriptive::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	CTStringArray xname;
	TIntStruct tVarTemp;

	AnaWord VOstr;
	VOstr.Import(VO);
	//������Ȩ��
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		if (tVarTemp.iCount == 1)
			m_nWeight = tVarTemp.pValue[0];
		else
			return FALSE;
		tVarTemp.DeleteObject();
		m_bWeight = TRUE;
	}
	else
	{
		m_bWeight = FALSE;
	}
	//����������
	xname.RemoveAll();
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarSel)))
		return FALSE;
	
	xname.RemoveAll();
	if (AnaSyntax::exist("/BYFIRST", VOstr))//��ʾ��һ�����
	{
		AnaSyntax::getVAR("/BYFIRST", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		CDesLayerConf *pDefConf = new CDesLayerConf;
		pDefConf->m_FNameList.Copy(xname);
		m_arrLayerConf.Add(pDefConf);
		tVarTemp.DeleteObject();
	}
	xname.RemoveAll();
	if (AnaSyntax::exist("/BYFIRST", VOstr) && AnaSyntax::exist("/BYSECOND", VOstr))//��ʾ�ڶ������
	{
		AnaSyntax::getVAR("/BYSECOND", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		CDesLayerConf *pDefConf = new CDesLayerConf;
		pDefConf->m_FNameList.Copy(xname);
		m_arrLayerConf.Add(pDefConf);
		tVarTemp.DeleteObject();
	}
	xname.RemoveAll();
	if (AnaSyntax::exist("/BYFIRST", VOstr) && AnaSyntax::exist("/BYSECOND", VOstr) && AnaSyntax::exist("/BYTHIRD", VOstr))//��ʾ���������
	{
		AnaSyntax::getVAR("/BYTHIRD", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		CDesLayerConf *pDefConf = new CDesLayerConf;
		pDefConf->m_FNameList.Copy(xname);
		m_arrLayerConf.Add(pDefConf);
		tVarTemp.DeleteObject();
	}
	//ͳ�Ʒ���
	int nIndex = 0, nCount = 0, nVar = 0;
	if (AnaSyntax::findstr("/DISPLAY", "RANGE", VOstr))
	{
		nCount ++;
		nVar = 1;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MIN", VOstr))
	{
		nCount ++;
		nVar |= 2;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MAX", VOstr))
	{
		nCount ++;
		nVar |= 4;
	}
	if (AnaSyntax::findstr("/DISPLAY", "TOTAL", VOstr))
	{
		nCount ++;
		nVar |= 8;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MEAN", VOstr))
	{
		nCount ++;
		nVar |= 16;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MEANSTDDEV", VOstr))
	{
		nCount ++;
		nVar |= 32;
	}
	if (AnaSyntax::findstr("/DISPLAY", "STDDEV", VOstr))
	{
		nCount ++;
		nVar |= 64;
	}
	if (AnaSyntax::findstr("/DISPLAY", "VAR", VOstr))
	{
		nCount ++;
		nVar |= 128;
	}
	if(nCount>0)
	{
		m_tStatMethod.DeleteObject();//ȱʡʱȫѡ
		int *pValue=new int[nCount];
		memset(pValue,0,sizeof(int)*nCount);
		if (nVar & 1)//����
		{
			pValue[nIndex]=0; 
			nIndex ++;
		}
		if (nVar & 2)//��Сֵ
		{
			pValue[nIndex]=1; 
			nIndex ++;
		}
		if (nVar & 4)//���ֵ
		{
			pValue[nIndex]=2; 
			nIndex ++;
		}
		if (nVar & 8)//�ܺ�
		{
			pValue[nIndex]=4; 
			nIndex ++;
		}
		if (nVar & 16)//ƽ��ֵ
		{
			pValue[nIndex]=5; 
			nIndex ++;
		}
		if (nVar & 32)//��ֵ��׼��
		{
			pValue[nIndex]=6; 
			nIndex ++;
		}
		if (nVar & 64)//��׼��
		{
			pValue[nIndex]=7; 
			nIndex ++;
		}
		if (nVar & 128)//����
		{
			pValue[nIndex]=8; 
			nIndex ++;
		}
		m_tStatMethod=TIntStruct(nCount,pValue);
	}
	return TRUE;
}

CResult*  CDescriptive::OnDescriptive(CDataInterface* pDataInterface, string VO)
{
	//��ʼ��
	m_pDataInterface = pDataInterface;
	if (!pDataInterface) return m_pResult;
	m_pResult = new CResult(_T("��ֵ����"));//����һ����������
	//�������
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//���㲢��ʾ
	DispResultOfHaveLayer();
	return m_pResult;
}

BOOL CDescriptive::Descriptive(CDataInterface *pDataInterface, int *pSelFeildArray,int nSelFldCount,double *pOutArray,int nMethod)
{
	CDWordArray arrCol;
	double fValue = 0.0, fTmp = 0.0;
	int i = 0, nColIndex = 0;
	int *pValue = NULL;
	CDoubleMatrix dataMatrix;

	if (nSelFldCount <= 0)
		return FALSE;

	//��ʼ��
	m_pDataInterface = pDataInterface;
	//Release Memory
	m_tVarSel.DeleteObject();
	m_tStatMethod.DeleteObject();

	pValue        = new int[1];
	pValue[0]     = nMethod;
	m_tStatMethod = TIntStruct(1,pValue);//ͳ�Ʒ���

	pValue = new int[nSelFldCount];
	//CopyMemory(pValue,pSelFeildArray,sizeof (int)*nSelFldCount);
	memcpy(pValue,pSelFeildArray,sizeof (int)*nSelFldCount); //--UNIX PORT
	m_tVarSel = TIntStruct(nSelFldCount,pValue);
	for (i=0;i<m_tVarSel.iCount;i++)
	{
		////////////////////////////////////////
		arrCol.RemoveAll();
		//�Ƿ���Ȩ��
		m_nWeight = m_pDataInterface->GetWeightNumb();
		if ( m_nWeight >= 0)
		{
			m_bWeight = TRUE;
			arrCol.Add(m_nWeight);
		}
		else
		{
			m_bWeight = FALSE;
		}
		nColIndex = m_tVarSel.pValue[i];//ȡ���ֶε��к�
		arrCol.Add(nColIndex);
		//����ȱʡֵɾ��
		int nRow = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
		if (nRow < 1) continue;
		////////////////////////////////////////
		for (int j=0;j<m_tStatMethod.iCount;j++)
		{
			int nID = m_tStatMethod.pValue[j];
			switch (nID)
			{
			case 4://�ܺ�
				{
					if (m_bWeight)
						fValue = dataMatrix(1).dot(dataMatrix(0));
					else
						fValue = dataMatrix(0).sum();
					pOutArray[i] = fValue;
				}
				break;
			case 5://ƽ��ֵ
				{
					if (m_bWeight)
						fValue = dataMatrix(1).mean_w(dataMatrix(0));
					else
						fValue = dataMatrix(0).mean();
					pOutArray[i] = fValue;
				}
				break;
			case 6://��ֵ��׼��
				{
					if (m_bWeight)
					{
						fTmp   = dataMatrix(0).sum();
						fValue = dataMatrix(1).var_w(dataMatrix(0));
					}
					else
					{
						fTmp   = dataMatrix.mrow();
						fValue = CDoubleVector(dataMatrix(0)).var();
					}
					pOutArray[i] = sqrt(fValue/fTmp);
				}
				break;
			case 7://��׼��
				{
					if (m_bWeight)
						fValue = dataMatrix(1).var_w(dataMatrix(0));
					else
						fValue = CDoubleVector(dataMatrix(0)).var();
					pOutArray[i] = sqrt(fValue);
				}
				break;
			case 8://����
				{
					if (m_bWeight)
						fValue = dataMatrix(1).var_w(dataMatrix(0));
					else
						fValue = CDoubleVector(dataMatrix(0)).var();
					pOutArray[i] = fValue;
				}
				break;
			default:
				break;
			}
		}
	}
	//Release Memory.
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
CExplore::CExplore()
{
	m_bBoth=TRUE;
	m_bWeight=FALSE;
	m_bPlots =FALSE;
	m_bOutliers=FALSE;
	m_bDescriptive=TRUE;
	m_bStatistics=FALSE;
	m_bPercentiles=FALSE;
	m_bMEstimators=FALSE;
	pWeight=NULL;
	m_nWeight = -1;
	m_nConfidence=95;
	//�ٷ���
	m_fPercentile[0][0] = 0.05;
	m_fPercentile[1][0] = 0.10;
	m_fPercentile[2][0] = 0.25;
	m_fPercentile[3][0] = 0.50;
	m_fPercentile[4][0] = 0.75;
	m_fPercentile[5][0] = 0.90;
	m_fPercentile[6][0] = 0.95;
	//����
	int *pValue=new int[macDESCRIPMETHOD];
	for(int i=0;i<macDESCRIPMETHOD;i++)
		pValue[i]=m_tStatMethodS[i].id;
	m_tStatMethod = TIntStruct(macDESCRIPMETHOD,pValue);
}

CExplore::~CExplore()
{
	m_tStatMethod.DeleteObject();
	m_tDependent.DeleteObject();//��ʾѡ�е��ֶ�
	m_tFactor.DeleteObject();   //��ʾѡ�е��ֶ�
}

int CExplore::GetMethodS(int* &pMethodS)
{
	pMethodS=new int[macDESCRIPMETHOD];//17��
	for(int ii=0;ii<macDESCRIPMETHOD;ii++)
		pMethodS[ii]=m_tStatMethodS[ii].id;
	
	return macDESCRIPMETHOD;
}

//DESCRIPTIVE
//	/DATA=[FILE]
//	/VARLIST [varlist]
//	/BY [varlist] 
//	/WEIGHT [varname]
//	/DPTABLE [DESCRI**] [PERCENT] [OUTLINEAR] [CIN(0.95**)]
//	/DISPLAY [TABLE] [GRAPH]
BOOL CExplore::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	CTStringArray xname;
	TIntStruct tVarTemp;

	TIntStruct m_tStatMethod;  //ͳ�Ʒ���
	AnaWord VOstr;
	VOstr.Import(VO);
	//������Ȩ��
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		if (tVarTemp.iCount == 1)
			m_nWeight = tVarTemp.pValue[0];
		else
			return FALSE;
		tVarTemp.DeleteObject();
		m_bWeight = TRUE;
	}
	else
	{
		m_bWeight = FALSE;
	}
	//����������
	xname.RemoveAll();
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);//��ʾѡ�е��Ա����ֶ�
	if (!(pDataInterface->getVarID(xname,m_tDependent)))
		return FALSE;

	xname.RemoveAll();
	if (AnaSyntax::exist("/BY", VOstr))
	{
		AnaSyntax::getVAR("/BY", VOstr, xname);//��ʾѡ�е�������ֶ�
		if (!(pDataInterface->getVarID(xname,m_tFactor)))
			return FALSE;
	}
	m_bDescriptive = AnaSyntax::findstr("/DPTABLE", "DESCRI", VOstr);
	m_bPercentiles = AnaSyntax::findstr("/DPTABLE", "PERCENT", VOstr);
	m_bOutliers = AnaSyntax::findstr("/DPTABLE", "OUTLINEAR", VOstr);
	m_bStatistics = AnaSyntax::findstr("/DISPLAY", "TABLE", VOstr);
	m_bPlots = AnaSyntax::findstr("/DISPLAY", "GRAPH", VOstr);
	if (m_bStatistics && m_bPlots) m_bBoth = TRUE;
	//����
	if (AnaSyntax::findstr("/DPTABLE", "CIN", VOstr))
	{
		m_nConfidence = AnaSyntax::getvalue("/DPTABLE", "CIN", VOstr);
	}
	return TRUE;
}

//������������ں���(1),�д��嵯��
CResult* CExplore::OnExplore(CDataInterface* pDataInterface,string VO)
{
	//��ʼ��
	m_pResult        = NULL;
	m_pDataInterface = pDataInterface;
	//��������
	m_pResult = new CResult(_T("��������"));//����һ����������
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	if (m_tFactor.iCount)
		OnExplore(pDataInterface);
	else
		OnExplore();
	return m_pResult;
}

//������������ں���(2),�޴��嵯��,ͨ���ַ�
CResult* CExplore::Explore(CDataInterface* pDataInterface,CTString strScript)
{
	//��ʼ��
	m_pResult        = NULL;
	m_pDataInterface = pDataInterface;
	CScript sct;
	sct.ImportStr(strScript);//�����ű�	

	if (CAnalyze::GetListData(&sct,"����",pDataInterface,m_tDependent)
		&&CAnalyze::GetListData(&sct,"����",pDataInterface,m_tFactor) )
	{
		if (m_tDependent.iCount==0)
			return NULL;//����Ϊ��
		//1ͳ��������************************************
		//*****************1.1ͳ�Ʊ�*************************
		CTString strParam = "ͳ�Ʊ�";
		int nIndex  = sct.FindParamName(strParam);
		if (nIndex!=-1)
		{//���ָ��
			nIndex = sct.GetItemCount(strParam);
			for (int i=0;i<nIndex; i++)
			{
				CTString strItem;   
				sct.GetItemData(strParam,i,strItem);
				if (strItem == "����")
					m_bDescriptive = true;
				else if (strItem == "��ֵ����")
					m_bMEstimators = true;
				else if (strItem == "��ֵ")
					m_bOutliers = true;
				else if (strItem == "�ٷ�λ��")
					m_bPercentiles = true;
			}
		}
		//****************1.2��������**********************
		strParam = "��������";
		nIndex  = sct.FindParamName(strParam);
		if (nIndex!=-1)
		{//���ָ��,�򲻰�Ĭ�ϴ���
			nIndex = sct.GetItemCount(strParam);//ֻ��һ��
			if (nIndex!=1)
				return NULL;
			int nItem;
			sct.GetItemData(strParam,0/*ֻ��һ��*/,nItem);
			if (nIndex<=0 || m_nConfidence>100)
				return NULL;//������Χ
			m_nConfidence = nItem;
		}
		else
		{//Ĭ�ϴ���
			m_nConfidence = 95;
		}
		//****************1.3��ʾ**********************
		strParam = "��ʾ";
		nIndex  = sct.FindParamName(strParam);
		if (nIndex!=-1)
		{//���ָ��,�򲻰�Ĭ�ϴ���
			nIndex = sct.GetItemCount(strParam);
			m_bPlots = false;
			m_bStatistics = false;
			for (int i=0;i<nIndex; i++)
			{
				CTString strItem;
				sct.GetItemData(strParam,i,strItem);
				if (strItem == "ͳ�Ʊ�")
					m_bStatistics = true;
				else if (strItem == "ͳ��ͼ")
					m_bPlots = true;
			}
			if (m_bPlots && m_bStatistics)
			{
				m_bBoth = true;	m_bPlots = false;m_bStatistics = false;
			}
				
		}
		OnExplore(pDataInterface);
		return m_pResult;
	}
	return NULL;
}

BOOL CExplore::CalcStemLeaf(CDoubleMatrix &dataMatrix, CIntVector &vecInd ,TStemLeaf *&pStemLeaf, double &fStemLeaf, int &nRow_num)
{
	int nVari = 0, nVark = 0, nMax_tmp = 0, nMin_tmp = 0, nCnt_num = 0;
	int nMin_num = 0, nMax_num = 0, nTest_rank = 0, nRank = 0, nMax_flag = 0, nMin_flag = 0;
	double fDn = 0.0, fMd = 0.0, fUp = 0.0, fMin = 0.0, fMax = 0.0, fAbsMax = 0.0;
	double fMax_num = 0.0, fMin_num = 0.0, fbi = 0.0;
	double fStep_width = 0.0, fComp_value = 0.0, fStart_value = 0.0, fStart_value_temp = 0.0;
	CTString szTemp(""),szLeaf(""),szFreq(""),szStem("");

	int nVlen = dataMatrix.mrow();
	int nCol  = m_bWeight ? 1 : 0;
	
	nMin_tmp = nVlen;
	fDn = m_fPercentile[2][1];
	fMd = m_fPercentile[3][1];
	fUp = m_fPercentile[4][1];
	
	//������Сֵ
	int i;
	for (i=0;i<nVlen-1;i++)
	{
		if (dataMatrix(nCol)(i) < fDn-1.5*(fUp-fDn))
		{
			nMin_tmp = i;
			if (m_bWeight)
				fMin_num += dataMatrix(0)(vecInd(i)-1);
			else
				fMin_num += 1; //add by xds 20060309
			nMin_num ++;
		}
	}
	if (nMin_tmp < nVlen-1)
	{
		fMin = dataMatrix(nCol)(nMin_tmp+1);
		if (m_bWeight)
			fbi += fMin_num;
		else
			fbi += 1; //add by xds 20060309

		nVari += nMin_num;
	}
	else
	{
		fMin = dataMatrix(nCol)(0);
	}
	//�������ֵ
	for (i=nVlen-1;i>0;i--)
	{
		if (dataMatrix(nCol)(i) > fUp + 1.5*(fUp-fDn))
		{
			if (m_bWeight)
				fMax_num += dataMatrix(0)(vecInd(i)-1);
			else
				fMax_num += 1; //add by xds 20060309
			nMax_num ++;
		}
	}
	if (nMax_tmp > 0)
	{
		fMax = dataMatrix(nCol)(nMax_tmp-1);
		if (m_bWeight)
			fbi += fMax_num;
		else
			fbi += 1;//add by xds 20060309
		nVari += nMax_num;
	}
	else
	{
		fMax = dataMatrix(nCol)(nVlen-1);
	}	
    if (fabs(fMin) > fMax)
		fAbsMax = fabs(fMin);
	else
		fAbsMax = fMax;
	
	if ((fAbsMax/10) > 1)
	{
		do 
		{
			nCnt_num ++;
			fAbsMax = fAbsMax/10;
		}while(fAbsMax/10 > 1);
	}
	//����fStemLeafֵ
	fStemLeaf = 1;
    if (nCnt_num != 0) 
    {
		for(i=0;i<nCnt_num;i++)
			fStemLeaf *= 10;
	}
	//ȷ���Ƿ������Ҫ5�ֻ���2��
	//�����������
    nTest_rank = (int)((fMax-fMin)/fStemLeaf+1);
	if ((fStemLeaf >= 100)&&(nTest_rank == 1))
	{
        do
		{
			fStemLeaf = fStemLeaf/10;
			nTest_rank  = (int)((fMax-fMin)/fStemLeaf+1);
		}while((fStemLeaf >= 100) && (nTest_rank == 1));
	}
	//rank:�Ƿ�ϸ��
	nRank = 1;
    if (nTest_rank == 1)
	{
		if(nVlen > 10)
			nRank = 5;
		else if(nVlen > 5)
            nRank = 2; 
	}
	else if(nTest_rank == 2)
	{
		if(nVlen/2 > 10)
			nRank = 5;
		else if(nVlen/2 > 5)
            nRank = 2;
	}
	else 
    {
		if((nVlen/nTest_rank) > 100)
			nRank = 10;
        else if((nVlen/nTest_rank) > 50)
			nRank = 5;
		else if((nVlen/nTest_rank) > 10)
			nRank = 2;
	}
	// �������
	fStep_width = fStemLeaf/nRank;
    nRow_num = 1;
	//��ʼ�������ֵ
	fStart_value = (floor(fMin/fStemLeaf))*fStemLeaf;
	int nStep = 0;
    double fTrans_start = fStart_value;
	do
	{
        fTrans_start += fStep_width; 
		nStep ++;
	}while(fTrans_start < fMin);
	//��ʼֵ
    fTrans_start -= fStep_width;
	//�Ƚϵ���ֵ�����ǿ�ʼ����ֵ��һ������
	fComp_value = fTrans_start + fStep_width;
	if (fComp_value <= fMax)
	{
		do
		{
            nRow_num ++;  
			fComp_value += fStep_width;
		}while(fComp_value <= fMax);
	}
	if (m_bWeight)
	{
		if (fMax_num > 0)
			nMax_flag = 1;
		if (fMin_num > 0)
			nMin_flag = 1;
	}
	else
	{
		if (nMax_num > 0)
			nMax_flag = 1;
		if (nMin_num > 0)
			nMin_flag = 1;
	}
    nRow_num += nMax_flag + nMin_flag;
	pStemLeaf = new TStemLeaf[nRow_num];
	//������½�Խ��
	if (m_bWeight)
	{
		if (fMin_num > 0)
		{
			szTemp.Format("%d",(int)(fMin_num+0.5));
			pStemLeaf[0].freq = szTemp;
			pStemLeaf[0].stem = "С����:";
			szTemp.Format("%.5f",dataMatrix(nCol)(nMin_tmp));
			pStemLeaf[0].leaf = "(<="+szTemp+")";
		}
	}
	else
	{
		if (nMin_num > 0)
		{
			szTemp.Format("%d",nMin_num);
			pStemLeaf[0].freq = szTemp;
			pStemLeaf[0].stem = "С����:";
			szTemp.Format("%.5f",dataMatrix(nCol)(nMin_tmp));
			pStemLeaf[0].leaf = "(<="+szTemp+")";
		}
	}
	//�������ݵĴ��
	fComp_value = fTrans_start + fStep_width;
    double fbk=0;
	nVari = 0;
	nVark = 0;
	fStart_value_temp = (nStep-1)*fStep_width;
    //ѭ������Ӧ�ò������������ֵ
	if (m_bWeight)
	{
		for (i=nMin_num;i<nVlen && dataMatrix(nCol)(i) <= fMax;i++) 
		{
			if (dataMatrix(nCol)(i) < fComp_value)
			{
				if (fStemLeaf > 10)
					szTemp.Format("%f",(dataMatrix(nCol)(i)-fStart_value)/fStemLeaf*10);
				else if(fStemLeaf==1) 
					szTemp.Format("%f",(dataMatrix(nCol)(i)-fStart_value)*10/fStemLeaf);
				else
					szTemp.Format("%f",dataMatrix(nCol)(i)-fStart_value);
				
				int kk =  atoi(szTemp);  //ȡ��������
				szTemp.Format("%d",kk);
				szLeaf += szTemp;
				fbk += dataMatrix(0)(vecInd(i)-1);
				if (strlen(szLeaf) > DISP_MAX/5) return FALSE;
				nVark ++;
			}
			else
			{
				szFreq.Format("%d",(int)(fbk+0.5));
				szStem.Format("%d",(int)(fStart_value/fStemLeaf));
				pStemLeaf[nVari+nMin_flag].freq = szFreq;
				pStemLeaf[nVari+nMin_flag].leaf = szLeaf;
				pStemLeaf[nVari+nMin_flag].stem = szStem; 
				fStart_value_temp += fStep_width;
				if ((fStart_value_temp > fStemLeaf) ||(fabs(fStart_value_temp-fStemLeaf)<1e-10))
				{
					fStart_value += fStemLeaf;
					fStart_value_temp = 0;
				}
				fComp_value += fStep_width;
				szLeaf = "";
				szFreq = "";
				fbi += dataMatrix(0)(vecInd(i)-1);
				nVari ++;
				fbk = 0;
				nVark = 0;
				i--;
			}
		}
	}
	else
	{
		for(i=nMin_num;i<nVlen && dataMatrix(nCol)(i) <= fMax;i++) 
		{
			if (dataMatrix(nCol)(i) < fComp_value)
			{				
				if (fStemLeaf > 10)
					szTemp.Format("%f",(dataMatrix(nCol)(i)-fStart_value)/fStemLeaf*10);
				else if(fStemLeaf == 1) 
					szTemp.Format("%f",(dataMatrix(nCol)(i)-fStart_value)*10/fStemLeaf);
				else
					szTemp.Format("%f",dataMatrix(nCol)(i)-fStart_value);
				
				int kk=  atoi(szTemp);  //ȡ��������
				szTemp.Format("%d",kk);
				szLeaf += szTemp;
				if (strlen(szLeaf) > DISP_MAX/5) return FALSE;
				nVark++;
			}
			else
			{
				szFreq.Format("%d",nVark);
				szStem.Format("%d",(int)(fStart_value/fStemLeaf));
				pStemLeaf[nVari+nMin_flag].freq = szFreq;
				pStemLeaf[nVari+nMin_flag].leaf = szLeaf;
				pStemLeaf[nVari+nMin_flag].stem = szStem; 

				fStart_value_temp += fStep_width;
				if ((fStart_value_temp > fStemLeaf) || (fabs(fStart_value_temp-fStemLeaf)<1e-10))
				{
					fStart_value += fStemLeaf;
					fStart_value_temp = 0;
				}
				fComp_value += fStep_width;
				szLeaf = "";
				szFreq = "";
				nVari ++;
				fbk = 0;
				nVark = 0;
				i--;
			}
		}
	}
	//ѭ�����һ�εĽ�����ܽ��뵽дֵ�����Ե����жϲ�����
	if (m_bWeight)
	{
		if (fabs(fbk)>0.000001)
		{
			szFreq.Format("%d",int (fbk+0.5));
			szStem.Format("%d",(int)(fStart_value/fStemLeaf));
			pStemLeaf[nVari+nMin_flag].freq = szFreq;
			pStemLeaf[nVari+nMin_flag].leaf = szLeaf;
			pStemLeaf[nVari+nMin_flag].stem = szStem; 
		}
		//������½�Խ��
		if (fMax_num > 0)
		{
			szTemp.Format("%d",int(fMax_num+0.5));
			pStemLeaf[nRow_num-1].freq = szTemp;
			pStemLeaf[nRow_num-1].stem = "�����:";
			szTemp.Format("%.5f",dataMatrix(nCol)(nMax_tmp));
			pStemLeaf[nRow_num-1].leaf = "(>="+szTemp+")";
		}
	}
	else
	{
		if (nVark != 0)
		{
			szFreq.Format("%d",nVark);
			szStem.Format("%d",(int)(fStart_value/fStemLeaf));
			pStemLeaf[nVari+nMin_flag].freq = szFreq;
			pStemLeaf[nVari+nMin_flag].leaf = szLeaf;
			pStemLeaf[nVari+nMin_flag].stem = szStem; 
		}
		//������½�Խ��
		if (nMax_num > 0)
		{
			szTemp.Format("%i",nMax_num);
			pStemLeaf[nRow_num-1].freq = szTemp;
			pStemLeaf[nRow_num-1].stem = "�����:";
			szTemp.Format("%.5f",dataMatrix(nCol)(nMax_tmp));
			pStemLeaf[nRow_num-1].leaf = "(>="+szTemp+")";
		}
	}
    return TRUE;
}

TPlotReco CExplore::GetCoordinate(CDoubleMatrix &dataMatrix)
{
	TPlotReco tPCoor;
	int nCol = 0, nVlen = 0, nDiv = 0;
	double fBegin = 0.0, fValue = 0.0, fQue = 0.0, fTmp = 0.0;

	nCol  = m_bWeight ? 1 : 0;
	nVlen = dataMatrix.mrow();
	fValue = dataMatrix(nCol)(0);

	if (nVlen == 1 || dataMatrix(nCol)(0) == dataMatrix(nCol)(nVlen-1))
	{
		fTmp = (fValue < 1) ? fValue : floor(fValue);
		tPCoor.co_num = 2;
		tPCoor.re_min = 0;
		tPCoor.re_max = 2*fValue;
		tPCoor.co_value = fValue;
		return tPCoor;
	}
    tPCoor.co_num = 0; 
	fQue = dataMatrix(nCol)(nVlen-1)-fValue;
    if (nVlen < 10 || (fQue/5) < 1)
	{
		nDiv = nVlen >= 10 ? 5: (((fQue/5)<1) ? 2 : 5);
		fValue = floor(fValue) == fValue ? floor(fValue-fQue/5) : floor(fValue);
		if (fValue < 0)
			fValue = 0;
		tPCoor.re_min = fValue; 
		do 
		{
			fValue += fQue/nDiv;   
			tPCoor.co_num ++; 
		}while(fValue <= dataMatrix(nCol)(nVlen-1));
		tPCoor.re_max    = tPCoor.re_min+ceil((fQue/nDiv)*tPCoor.co_num);
		tPCoor.co_value = (tPCoor.re_max-tPCoor.re_min)/tPCoor.co_num;
	}
	else
	{
		if(floor(fValue) == fValue)
		{
			fTmp = floor(fValue-fQue/5);
			if (fValue > 0)
			{
				if (fTmp >= fValue)
					fTmp -= 10;
				if (fTmp < 0)
					fTmp = 0;
			}
		}
		else
		{
			fTmp = floor(fValue);
		}
		
		fBegin = (int)fTmp;		
		if (((int)fBegin%10) != 0)
		{
			do
			{
				fBegin--;
			}
			while(((int)fBegin%10) != 0);
		}
		if (fBegin < 0)
			fBegin = 0;
		tPCoor.re_min = fBegin; 
		do 
		{
			fBegin += ceil(fQue/5);   
			tPCoor.co_num ++; 
		}while(fBegin <= dataMatrix(nCol)(nVlen-1));
		tPCoor.re_max   = tPCoor.re_min+ceil(fQue/5)*tPCoor.co_num;
		tPCoor.co_value = ceil(fQue/5);
	}
	return tPCoor;
}
//�ú������ص�x������5��ֵ��
//��x[0]��ʼ�ֱ�Ϊ�� ��Сֵ�ĺ��ߡ�BOX�����ߣ��м�ֵ��BOX�����ߡ�����ֵ�ĺ���
//�緵�ص�intֵ����5����˵����extremֵ����Ҫ������ͼ������ʾ��
void CExplore::AddBoxPlot(CDoubleMatrix &dataMatrix, CRsltVariable *pVariable, CTString szFldName)
{
	int nCount = 0;
	TDataPointStrArr *tPointArr = NULL;
	CTChartBoxplot* pCB = new CTChartBoxplot;

	int nCol = m_bWeight ? 1 : 0;
	pCB->SetParam(m_bWeight,TRUE,TRUE);
	pCB->SetPercent(m_fPercentile);
	CDoubleVector v(dataMatrix(nCol));
	pCB->CalcBoxPlot(v,(void **)&tPointArr,nCount);
	tPointArr->strName += SEP_RISER + szFldName;
	pCB->SetBoxplot((void *)(tPointArr),1,StrArr,"��״ͼ");//1��ʾͼ����
	CRsltElementChart* pREC = new CRsltElementChart("��״ͼ",pCB);
	pVariable->Add(pREC); 
	delete tPointArr;
	/*
	int i = 0;
	CTString szCount;
	TDataPointStrArr tPointArr;
	int nVari=5,nVark=5,nMax=0,nMin=0;
	double fMin=0,fMax=0,fMd=0,fDn=0,fUp=0;
	double *pfX = NULL;
	
	int nVlen = dataMatrix.mrow();
	int nCol  = m_bWeight ? 1 : 0;
	
	nMin = nVlen;
	fDn = m_fPercentile[2][1];
	fMd = m_fPercentile[3][1];
	fUp = m_fPercentile[4][1];
	//��������
	//tPlotReco = GetCoordinate(dataMatrix);
	for (i=nVlen-1;i>0;i--)
	{
		if (dataMatrix(nCol)(i) <= (fUp+1.5*(fUp-fDn))) break;
		nMax = i;
	}
	for (i=0;i<nVlen-1;i++)
	{
		if (dataMatrix(nCol)(i) >= (fDn-1.5*(fUp-fDn))) break;
		nMin = i;
	}
	if (nMax > 0)
	{
		fMax = dataMatrix(nCol)(nMax-1);
		nVari += nVlen - nMax;
	}
	else
	{
		fMax = dataMatrix(nCol)(nVlen-1);
	}
	if (nMin < nVlen-1)
	{
		fMin = dataMatrix(nCol)(nMin+1);
		nVari += nMin + 1;
	}
	else
	{
		fMin = dataMatrix(nCol)(0);
	}
    
	pfX = new double[nVari];
	pfX[0]=fMin;
    pfX[1]=fDn;
	pfX[2]=fMd;
	pfX[3]=fUp;
	pfX[4]=fMax;
	if (nVari > 5 && nMax > 0)
	{
		for (i=nMax;i<nVlen;i++)
		{
			pfX[nVark] = dataMatrix(nCol)(i);
			nVark ++;
		}
	}
	if (nVari > 5 && nMin < nVlen)
	{
		for (i=0;i<nMin+1;i++)
		{
			pfX[nVark] = dataMatrix(nCol)(i);
			nVark ++;
		}
	}
	//���
	tPointArr.strName = szFldName;
	if (nVark > DISP_MAX/5)
	{
		szCount.Format("������%s������������ʾ��Χ���������װͼ��",szFldName.GetData());
		CRsltElementText *pEle = new CRsltElementText("��״ͼ");
		CRsltElementText* pText = (CRsltElementText*)pEle;
		pText->AddString(szCount);
		pVariable->Add(pEle);
	}
	else
	{
		for(i=0;i<nVark;i++)
		{
			tPointArr.arrYVal.Add(pfX[i]); 
		}
		delete []pfX;
		pfX = NULL;
		CTChartBoxplot* pCB=new CTChartBoxplot;
		pCB->SetBoxplot((void *)(&tPointArr),1,StrArr,"��״ͼ");
		CRsltElementChart* pREC=new CRsltElementChart("��״ͼ",pCB);
		pVariable->Add(pREC); 
	}
	*/
}

void CExplore::AddStemPlot(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName, CTString szGroup)
{
	/*
	double fStemLeaf     = 0; 
	TStemLeaf *pStemLeaf = NULL;
	CTString szTemp(""),szFreq(""),szStem(""),szLeaf("");

	//���㾥 Ҷ
	int nStemLeaf = 0;
	BOOL bRet = CalcStemLeaf(dataMatrix,vecInd,pStemLeaf,fStemLeaf,nStemLeaf);
	CRsltElementText *pEle = new CRsltElementText("��Ҷͼ");
	if (!bRet || nStemLeaf > DISP_MAX/5)
	{
		szTemp.Format("������%s����Ҷ�������ȷ�Χ%d���������Ҷͼ��",szFldName,DISP_MAX/5);
		pEle->AddString(szTemp);
		pVariable->Add(pEle);
	}
	else
	{
		szTemp.Format("%s ��Ҷͼ",szFldName);
		pEle->AddString(szTemp);
		pEle->AddString(szGroup);
		szTemp.Format("%s"," Ƶ ��  ��           ��  :  Ҷ");
		pEle->AddString(szTemp);
		for(int i=0;i<nStemLeaf;i++)
		{
			szTemp.Format("%s\n",pStemLeaf[i].freq);
			szFreq += szTemp;
			szTemp.Format("%s%s\n",pStemLeaf[i].stem," . ");
			szStem += szTemp;
			szTemp.Format("%s\n",pStemLeaf[i].leaf);
			szLeaf += szTemp;
		}
		pEle->AddString(szFreq);
		pEle->AddString(szStem);
		pEle->AddString(szLeaf);
		szTemp.Format("%s%4.2f\n%s","֦�Ŀ��:����" ,fStemLeaf,"ÿ��Ҷ:������1������");
		pEle->AddString(szTemp);
		pVariable->Add(pEle);
	}
	delete [] pStemLeaf;
	pStemLeaf=NULL;
	*/
}

BOOL CExplore::CalcTrimMean(CDoubleMatrix &dataMatrix, CIntVector &vecInd)
{
	int nCol  = m_bWeight ? 1 : 0;
	int nVlen = dataMatrix.mrow();

	if (m_bWeight) //��Ȩ��
	{
		double fTc = 0;
		int k1 = -1, k2 = -1;

		CDoubleVector wTmp(nVlen,0);
		wTmp(0) = dataMatrix(0)(vecInd(0)-1);
		//���βƽ����
		int i;
		for (i=1;i<nVlen;i++)
		{
			wTmp(i)= wTmp(i-1) + dataMatrix(0)(vecInd(i)-1);
		}
		fTc = 0.05 * m_tOut.m_fSum;
		for (i=0;i<nVlen-1;i++)
		{
			if (wTmp(i) < fTc && wTmp(i+1) >= fTc)
			{
				k1 = i;
				break;
			}
		}
		for (i=1;i<nVlen;i++)
		{
			if (m_tOut.m_fSum - wTmp(i) < fTc && m_tOut.m_fSum - wTmp(i-1) >= fTc)
			{
				k2 = i;
				break;
			}
		}
		if (k2 < 0) return FALSE;
		if (k2 == k1+1)
		{
			m_tOut.m_fTrimMean = dataMatrix(nCol)(k2);
		}
		else
		{
			for (i=k1+2;i<k2;i++)
			{
				m_tOut.m_fTrimMean += dataMatrix(0)(vecInd(i)-1) * dataMatrix(nCol)(i);
			}
			m_tOut.m_fTrimMean += (wTmp(k1+1)-fTc) * dataMatrix(nCol)(k1+1) + (m_tOut.m_fSum-wTmp(k2-1)-fTc) * dataMatrix(nCol)(k2);
			m_tOut.m_fTrimMean /= (0.9*m_tOut.m_fSum);
		}
	}
	else //��Ȩ��
	{
		double fTc = 0;
		int k1 = -1, k2 = -1;

		CDoubleVector wTmp(nVlen,0);
		wTmp(0) = 1;
		//���βƽ����
		int i;
		for (i=1;i<nVlen;i++)
		{
			wTmp(i)= wTmp(i-1) + 1;
		}
		fTc = 0.05 * m_tOut.m_fSum;
		for (i=0;i<nVlen-1;i++)
		{
			if (wTmp(i) < fTc && wTmp(i+1) >= fTc)
			{
				k1 = i;
				break;
			}
		}
		for (i=1;i<nVlen;i++)
		{
			if (m_tOut.m_fSum - wTmp(i) < fTc && m_tOut.m_fSum - wTmp(i-1) >= fTc)
			{
				k2 = i;
				break;
			}
		}
		if (k2 < 0) return FALSE;
		if (k2 == k1+1)
		{
			m_tOut.m_fTrimMean = dataMatrix(nCol)(k2);
		}
		else
		{
			for (i=k1+2;i<k2;i++)
			{
				m_tOut.m_fTrimMean += dataMatrix(nCol)(i);
			}
			m_tOut.m_fTrimMean += (wTmp(k1+1)-fTc) * dataMatrix(nCol)(k1+1) + (m_tOut.m_fSum-wTmp(k2-1)-fTc) * dataMatrix(nCol)(k2);
			m_tOut.m_fTrimMean /= (0.9*m_tOut.m_fSum);
		}
		/*
		if (nVlen > 2)
		{
			int nMin = (int)floor(0.05*nVlen+0.5) == 0;
			if (nMin == 0) nMin = 1;
			for (int i=nMin;i<nVlen-nMin;i++)
			{
				m_fTrimMean += dataMatrix(nCol)(i);
			}
			m_fTrimMean /= (nVlen-2*nMin);
		}
		else
		{
			m_fTrimMean = 0;
		}
		*/
	}
	return TRUE;
}

void CExplore::CalcPercent(CDoubleMatrix &dataMatrix, CIntVector &vecInd)
{
	BOOL bRet = FALSE;
	int k1[DISP_PERCENT], k2[DISP_PERCENT];
	double fTc1[DISP_PERCENT], fTc2[DISP_PERCENT];
	double fG1[DISP_PERCENT], fG2[DISP_PERCENT];
	double fG1Star[DISP_PERCENT], fG2Star[DISP_PERCENT];

	int nCol  = m_bWeight ? 1 : 0;
	int nVlen = dataMatrix.mrow();
	//�ٷ���
	int i;
	for (i=0;i<DISP_PERCENT;i++)
	{
		if (nVlen == 0)
		{
			m_fPercentile[i][1] = 0;
			bRet = TRUE;
		}
		else
		if (nVlen == 1)
		{
			m_fPercentile[i][1] = dataMatrix(nCol)(0);
			bRet = TRUE;
		}
	}
	//�ж��Ƿ񷵻�
	if (bRet) return;
	//���Ȩ�ص�����
	CDoubleVector wTmp(nVlen,0);
	if (m_bWeight)
	{
		wTmp(0) = dataMatrix(0)(vecInd(0)-1);
		//��ٷ���
		for (i=1;i<nVlen;i++)
		{
			wTmp(i) = wTmp(i-1) + dataMatrix(0)(vecInd(i)-1);
		}
		for (i=0;i<DISP_PERCENT;i++)
		{
			fTc1[i] = m_tOut.m_fSum * m_fPercentile[i][0];
			fTc2[i] = (m_tOut.m_fSum+1) * m_fPercentile[i][0];
			k1[i] = k2[i] = 0;
		}
		//��ֵ
		int j;
		for (j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc1[j] && wTmp(i+1) > fTc1[j])
				{
					k1[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc2[j] && wTmp(i+1) > fTc2[j])
				{
					k2[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			//fG1Star[j] = fTc1[j] - wTmp(k1[j]);
			fG2Star[j] = fTc2[j] - wTmp(k2[j]);
			if (fG2Star[j] >= 1)
			{
				m_fPercentile[j][1] = dataMatrix(nCol)(k1[j]+1);
			}
			else
			{
				if (wTmp(vecInd(k2[j]+1)-1) >= 1)
				{
					m_fPercentile[j][1] = (1-fG2Star[j])*dataMatrix(nCol)(k2[j]) + fG2Star[j]*dataMatrix(nCol)(k2[j]+1);
				}
				else
				{
					//fG1[j] = fG1Star[j]/dataMatrix(0)((vecInd(k1[j]+1)-1));
					fG2[j] = fG2Star[j]/dataMatrix(0)((vecInd(k2[j]+1)-1));
					m_fPercentile[j][1] = (1-fG2[j])*dataMatrix(nCol)(k2[j]) + fG2[j]*dataMatrix(nCol)(k2[j]+1);
				}
			}
			if (m_fPercentile[j][1] < dataMatrix(nCol)(0))
				m_fPercentile[j][1] = dataMatrix(nCol)(0);
			if (m_fPercentile[j][1] > dataMatrix(nCol)(nVlen-1))
				m_fPercentile[j][1] = dataMatrix(nCol)(nVlen-1);
		}
	}
	else
	{
		wTmp(0) = 1;
		//�ٷ���
		int i;
		for (i=0;i<DISP_PERCENT;i++)
		{
			if (nVlen == 0)
			{
				m_fPercentile[i][1] = 0;
				bRet = TRUE;
			}
			else
			if (nVlen == 1)
			{
				m_fPercentile[i][1] = dataMatrix(nCol)(0);
				bRet = TRUE;
			}
		}
		//�ж��Ƿ񷵻�
		if (bRet) return;
		/*//��ٷ���
		for (i=1;i<nVlen;i++)
		{
			wTmp(i) = wTmp(i-1) + 1;
		}
		for (i=0;i<DISP_PERCENT;i++)
		{
			fTc1[i] = m_fSum * m_fPercentile[i][0];
			fTc2[i] = (m_fSum+1) * m_fPercentile[i][0];
			k1[i] = k2[i] = 0;
		}
		//��ֵ
		for (int j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc1[j] && wTmp(i+1) > fTc1[j])
				{
					k1[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			for (i=0;i<nVlen-1;i++)
			{
				if (wTmp(i) <= fTc2[j] && wTmp(i+1) > fTc2[j])
				{
					k2[j] = i;
					break;
				}
			}
		}
		for (j=0;j<DISP_PERCENT;j++)
		{
			//fG1Star[j] = fTc1[j] - wTmp(k1[j]);
			fG2Star[j] = fTc2[j] - wTmp(k2[j]);
			if (fG2Star[j] >= 1)
			{
				m_fPercentile[j][1] = dataMatrix(nCol)(k2[j]+1);
			}
			else
			{
				if (wTmp(vecInd(k2[j]+1)-1) >= 1)
				{
					m_fPercentile[j][1] = (1-fG2Star[j])*dataMatrix(nCol)(k2[j]) + fG2Star[j]*dataMatrix(nCol)(k2[j]+1);
				}
				else
				{
					//fG1[j] = fG1Star[j];
					fG2[j] = fG2Star[j];
					m_fPercentile[j][1] = (1-fG2[j])*dataMatrix(nCol)(k2[j]) + fG2[j]*dataMatrix(nCol)(k2[j]+1);
				}
			}
			if (m_fPercentile[j][1] < dataMatrix(nCol)(0))
				m_fPercentile[j][1] = dataMatrix(nCol)(0);
			if (m_fPercentile[j][1] > dataMatrix(nCol)(nVlen-1))
				m_fPercentile[j][1] = dataMatrix(nCol)(nVlen-1);
		}
		*/
		//�ٷ���int
		for ( i=0;i<DISP_PERCENT;i++)
		{
			double k = m_fPercentile[i][0]*nVlen;
			int j = (long)ceil(k);
			if (nVlen%2 == 0)
			{	
				if (j==int(k))
				{
					m_fPercentile[i][1] = (dataMatrix(nCol)(j) + dataMatrix(nCol)(j-1))/2;
				}
				else
				{
					m_fPercentile[i][1] = dataMatrix(nCol)(j-1);
				}
			}
			else
			{
				long nIndex = (long)k;
				double fLeave = k- nIndex;
				if (m_fPercentile[i][0] == 0.25)
				{
					if (fLeave > 0.50)
					{
						m_fPercentile[i][1] = (dataMatrix(nCol)(nIndex+1) + dataMatrix(nCol)(nIndex))/2;
					}
					else
					{
						m_fPercentile[i][1] = dataMatrix(nCol)(nIndex);
					}					
				}
				else
				if (m_fPercentile[i][0] == 0.50)
				{
					m_fPercentile[i][1] = dataMatrix(nCol)(nIndex);
				}
				else
				if (m_fPercentile[i][0] == 0.75)
				{
					if (fLeave < 0.50)
					{
						m_fPercentile[i][1] = (dataMatrix(nCol)(nIndex) + dataMatrix(nCol)(nIndex-1))/2;
					}
					else
					{
						m_fPercentile[i][1] = dataMatrix(nCol)(nIndex);
					}
				}
				else
				{
					if (j==int(k))
					{
						m_fPercentile[i][1] = (dataMatrix(nCol)(j) + dataMatrix(nCol)(j-1))/2;
					}
					else
					{
						m_fPercentile[i][1] = dataMatrix(nCol)(j-1);
					}
				}
			}
		}
	}
}

void CExplore::CalcPartial_Apex(CDoubleMatrix &dataMatrix, CIntVector &vecInd)
{
	BOOL bNotFindMid = TRUE;
	int nRow = 0, nCol = 0, nVlen = 0;
	double fM1 = 0.0, fM2 = 0.0, fM3 = 0.0, fM4 = 0.0;
	double fPow = 0.0, fValue = 0.0, fWeight = 0.0, fTmp = 0.0;
	double  fFreValue = 0.0, fFreTotal = 0.0, fOldFreValue = 0.0;
	
	//��ֵ
	nRow = 0;
	nCol = m_bWeight ? 1 : 0;
	nVlen  = dataMatrix.mrow();
	fValue = dataMatrix(nCol)(0);
	
	//����-���еĶ��ֿ��������߼��ж�
	if (m_bWeight) //Ȩ��
	{
		for (int i=0;i<nVlen;i++) 
		{
			fTmp    = dataMatrix(nCol)(i);
			fWeight = dataMatrix(0)(vecInd(i)-1);
			//����ƫֵ����ֵ
			fPow = pow(fTmp-m_tOut.m_fMean,2);
			fM1 += fWeight * pow(fTmp-m_tOut.m_fMean,3);
			fM2 += fWeight * fPow;
			fPow *= fTmp-m_tOut.m_fMean;
			fM3 += fWeight * fPow;
			fPow *= fTmp-m_tOut.m_fMean;
			fM4 += fWeight * fPow;
			if (fValue == fTmp)
			{
				fFreValue += fWeight;
				fFreTotal += fWeight;
			}
			else
			{
				//��λ��������
				if (bNotFindMid && fFreTotal > m_tOut.m_fSum/2.0)
				{
					m_tOut.m_fMid = fValue;
					bNotFindMid = FALSE;
				}
				if (fFreValue > fOldFreValue)
				{
					fOldFreValue = fFreValue;
					m_tOut.m_fMany = fValue;
				}
				else
				if (fFreValue == fOldFreValue)
				{
					if (m_tOut.m_fMany > fValue)
						m_tOut.m_fMany = fValue;
				}
				//�µ�
				fValue    = fTmp;
				fFreValue = fWeight;
				fFreTotal += fWeight;
			}
		}
	}
	else  //��Ȩ��
	{
		for (int i=0;i<nVlen;i++) 
		{
			fTmp    = dataMatrix(nCol)(i);
			//����ƫֵ����ֵ
			fPow = pow(fTmp-m_tOut.m_fMean,2);
			fM1 += pow(fTmp-m_tOut.m_fMean,3);
			fM2 += fPow;
			fPow *= fTmp-m_tOut.m_fMean;
			fM3 += fPow;
			fPow *= fTmp-m_tOut.m_fMean;
			fM4 += fPow;
			if (fValue == fTmp)
			{
				fFreValue += 1;
				fFreTotal += 1;
			}
			else
			{
				//��λ��������
				if (fFreValue > fOldFreValue)
				{
					fOldFreValue = fFreValue;
					m_tOut.m_fMany = fValue;
				}
				else
				if (fFreValue == fOldFreValue)
				{
					if (m_tOut.m_fMany > fValue)
						m_tOut.m_fMany = fValue;
				}
				//�µ�
				fValue    = fTmp;
				fFreValue = 1;
				fFreTotal += 1;
			}
		}
	}
	//��λ��������
	if (m_bWeight)
	{
		if (bNotFindMid && fFreTotal > m_tOut.m_fSum/2.0)
			m_tOut.m_fMid = fValue;
	}
	else
	{
		if (nVlen%2 != 0)
		{
			m_tOut.m_fMid = dataMatrix(nCol)(nVlen/2);
		}
		else
		{
			m_tOut.m_fMid = dataMatrix(nCol)((nVlen-1)/2) + dataMatrix(nCol)(nVlen/2);
			m_tOut.m_fMid /= 2;
		}
	}
	if (fFreValue > fOldFreValue)
	{
		fOldFreValue = fFreValue;
		m_tOut.m_fMany = fValue;
	}
	else
	if (fFreValue == fOldFreValue)
	{
		if (m_tOut.m_fMany > fValue)
			m_tOut.m_fMany = fValue;
	}
	//ƫ��ϵ�����
	m_bOutPartial  = TRUE;
	m_bOutPartialE = TRUE;
	m_bOutApex     = TRUE;
	m_bOutApexE    = TRUE;
	if (m_tOut.m_fSum-2.0 == 0)
	{
		m_bOutPartial  = FALSE;
		m_bOutPartialE = FALSE;
		m_bOutApex     = FALSE;
		m_bOutApexE    = FALSE;
	}
	else
	{
		m_bOutPartial = TRUE;
		m_tOut.m_fPartialE = sqrt((m_tOut.m_fSum*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-2.0)*(m_tOut.m_fSum+1.0)*(m_tOut.m_fSum+3.0)/6.0));
		if (m_tOut.m_fSum-3.0 == 0)
		{
			m_bOutApexE = FALSE;
			m_bOutApex  = FALSE;
		}
		else
		{
			//���
			m_tOut.m_fApexE = sqrt((4.0*(m_tOut.m_fSum*m_tOut.m_fSum-1.0)*m_tOut.m_fPartialE*m_tOut.m_fPartialE)/((m_tOut.m_fSum-3.0)*(m_tOut.m_fSum+5.0)));
			if (m_tOut.m_fSum-1.0 == 0)
			{
				m_bOutApex    = FALSE;
				m_bOutPartial = FALSE;
			}
			else
			{
				m_tOut.m_fApex    = (m_tOut.m_fSum*(m_tOut.m_fSum+1.0)*fM4-3.0*fM2*fM2*(m_tOut.m_fSum-1.0))/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*(m_tOut.m_fSum-3.0)*pow(m_tOut.m_fS,4.0))+3.0;
				m_tOut.m_fPartial = (m_tOut.m_fSum*fM1)/((m_tOut.m_fSum-1.0)*(m_tOut.m_fSum-2.0)*pow(m_tOut.m_fS,3.0));
			}
		}
	}
}

void CExplore::CreateResult(CDoubleMatrix &dataMatrix, CIntVector &vecInd, CRsltVariable *pVariable, CTString szFldName)
{
	CTString szTemp("");
	double fTmp = 0.0, fUp = 0.0, fDn = 0.0;
	int nRow = 0, nCol = 0, nRowCnt = 0, nColIndex = 0;
	int nMinMax[2];

	CTLTable* pTable            = NULL; //�����
	CRsltElementTable* pRsltEle = NULL; //��Ԫ�ض���

	nCol = m_bWeight ? 1 : 0;
	//����
	if(m_bBoth || m_bStatistics)
	{
		CDoubleVector v1(dataMatrix(nCol));
		CDoubleVector v2(dataMatrix(0));
		CalcParam(v1,v2);
		//����
		CDoubleVector v3(dataMatrix(nCol));
		v3.Sort(vecInd);
		//����������
		double q = ((double)(1-m_nConfidence/100.0))/2.0;
		fTmp = -tinv(q,m_tOut.m_fSum-1);
		fTmp *= m_tOut.m_fS/sqrt(m_tOut.m_fSum);
		fUp = m_tOut.m_fMean + fTmp;
		fDn = m_tOut.m_fMean - fTmp;
		//��λ����������ƫֵ.��ֵ
		CalcPartial_Apex(dataMatrix,vecInd);
		//����ٷ�λ��
		CalcPercent(dataMatrix, vecInd);
		//������ͳ������
		if(m_bDescriptive)
		{
			//�����ۼ�̬�Ʊ�
			pTable = CreateTableS(1,szFldName,"");
			pTable->SetItemText(0,1,m_tOut.m_fMean);
			pTable->SetItemText(0,2,m_tOut.m_fMe);
			pTable->SetItemText(1,1,m_tOut.m_fMid);
			pTable->SetItemText(2,1,m_tOut.m_fMany);
			pTable->SetItemText(3,1,m_tOut.m_fMin);
			pTable->SetItemText(4,1,m_tOut.m_fMax);
			if (CalcTrimMean(dataMatrix, vecInd))
				pTable->SetItemText(5,1,m_tOut.m_fTrimMean);	
			else
				pTable->SetItemText(5,1,"");	
			pTable->SetItemText(6,1,fUp);
			pTable->SetItemText(7,1,fDn);
			pTable->SetItemText(8,1,m_tOut.m_fSmy);
			szTemp.Format("%.0f",m_tOut.m_fSum);
			pTable->SetItemText(9,1,szTemp);
			pTable->SetItemText(10,1,int(m_pDataInterface->m_DataAccess.GetRecordCount()-dataMatrix.mrow()));
			pRsltEle = new CRsltElementTable("��������",pTable);
			pVariable->Add(pRsltEle);
			///������ɢ̬�Ʊ�
			pTable=CreateTableS(2,szFldName,"");
			pTable->SetItemText(0,1,m_tOut.m_fMax-m_tOut.m_fMin);
			pTable->SetItemText(1,1,m_fPercentile[4][1]-m_fPercentile[2][1]);//���䷶Χ
			pTable->SetItemText(2,1,m_tOut.m_fS2);
			pTable->SetItemText(3,1,m_tOut.m_fS);
			if (m_tOut.m_fMean > 0)
				pTable->SetItemText(4,1,m_tOut.m_fS/m_tOut.m_fMean);
			else
				pTable->SetItemText(4,1,"");
			if (m_bOutPartial)
				pTable->SetItemText(5,1,m_tOut.m_fPartial);
			else
				pTable->SetItemText(5,1,"");
			if (m_bOutPartialE)
				pTable->SetItemText(5,2,m_tOut.m_fPartialE);
			else
				pTable->SetItemText(5,2,"");
			if (m_bOutApex)
				pTable->SetItemText(6,1,m_tOut.m_fApex);
			else
				pTable->SetItemText(6,1,"");
			if (m_bOutApexE)
				pTable->SetItemText(6,2,m_tOut.m_fApexE);
			else
				pTable->SetItemText(6,2,"");
			pRsltEle = new CRsltElementTable("��ɢ�̶�",pTable);
			pVariable->Add(pRsltEle);
		}
		//�������С�����ֵ
		if(m_bOutliers)
		{
			nRowCnt = dataMatrix.mrow();
			nRow = nRowCnt/2 > DISP_EXP_MAXMIN ? DISP_EXP_MAXMIN : nRowCnt/2;
			nMinMax[0] = nMinMax[1] = nRow;
			if (nRow > 0) 
			{
				///�����쳣ֵֵ��
				pTable=CreateTableS(3,szFldName,"",nMinMax);
				int j;
				for(j=0;j<nRow;j++)
				{
					pTable->SetItemText(j,1,vecInd(nRowCnt-1-j));
					pTable->SetItemText(j,2,dataMatrix(nCol)(nRowCnt-1-j));
				}
				for(j=0;j<nRow;j++)
				{
					pTable->SetItemText(j+nRow,1,vecInd(j));
					pTable->SetItemText(j+nRow,2,dataMatrix(nCol)(j));
				}
				pRsltEle=new CRsltElementTable("��ֵ",pTable);
				pVariable->Add(pRsltEle);
			}
			else
			{
				CRsltElementText* pText = new CRsltElementText("���棺��ֵ" );
				CTString str;
				str=_T("������Ч����̫�٣��޷����ɼ�ֵ��!\n");
				pText->AddString(str);
				pVariable->Add(pText);
			}
		}
		//��ٷ�λ��
		if(m_bPercentiles)
		{			
			///�����ٷ�λ����
			pTable=CreateTableS(4,szFldName,"");
			for (int j=0; j<DISP_PERCENT; j++)
			{
				szTemp.Format("%3.2f",m_fPercentile[j][1]);
				pTable->SetItemText(0,j+1,szTemp);
			}
			pRsltEle = new CRsltElementTable("�ٷ�λ��",pTable);
			pVariable->Add(pRsltEle);
		}
	}
}

void CExplore::OnExplore()
{
	CDWordArray    arrCol;
	CIntVector    vecInd;           //����
	CDoubleMatrix dataMatrix;
	CDoubleMatrix weightMatrix;
	CRsltVariable *pVariable = NULL;//��������
	int nCol = 0, nRowCnt = 0, nColIndex = 0;

	if ( m_bStatistics    && !m_bDescriptive &&
	    !m_bPercentiles   && !m_bMEstimators && !m_bOutliers)
		return;
	if (m_tStatMethod.iCount < 1 || m_tDependent.iCount < 1)
		return;
	//�ܵļ�¼��
	int nAllCount = m_pDataInterface->m_DataAccess.GetRecordCount();
	//��ȡȨ��
	if (m_bWeight)
	{
		arrCol.Add(m_nWeight);
		nRowCnt = m_pDataInterface->GetColsData(arrCol,weightMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning.Format("ע�� ����ѡ���Ȩ�����ݲ��ʺ�����ǰ������������ѡ��");
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
			return;
		}
		//�ܵ�Ȩ��
		m_n64Weight = __int64(weightMatrix(0).sum());
		weightMatrix.destruction();
	}
	else
	{
		m_n64Weight = nAllCount;
	}
	//�Ա���
	for(int i=0;i<m_tDependent.iCount;i++)
	{
		arrCol.RemoveAll();
		dataMatrix.destruction();
		if (m_bWeight)
		{
			arrCol.Add(m_nWeight);
		}
		nColIndex = m_tDependent.pValue[i];//ȡ���ֶε��к�
		arrCol.Add(nColIndex);
		//����ȱʡֵɾ��
		nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning.Format("ע�� ����%s-û�к��ʵļ�¼��������ͳ�Ʒ�����",m_pDataInterface->GetFieldName(nColIndex).GetData());
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
			continue;
		}
		//ͳ�Ʒ���
		CTString szFldName = m_pDataInterface->GetFieldName(nColIndex);				
		pVariable = new CRsltVariable(szFldName);
		m_tOut.Init();   //������ʼ��
		//��������
		CreateResult(dataMatrix, vecInd, pVariable, szFldName);
		//��ͳ��ͼ������
		if ((m_bBoth || m_bPlots))
		{
			if (nRowCnt < DISP_MAX * 4)
				AddStemPlot(dataMatrix, vecInd, pVariable, szFldName);
			if (nRowCnt < DISP_MAX * 2)
				AddBoxPlot(dataMatrix, pVariable, szFldName);
		}
		//���뵽result
		if (pVariable->Count() > 0)
			m_pResult->Add(pVariable);
		else
		{
			delete pVariable;
			pVariable=NULL;
		}
	}
}

//add by liytao 20030212
int CExplore::GetValidData(CDataInterface *pDataInterface,TDataSet *&pDataArray, CTString strSelName,CTString strFactName)
{
	CDataAccess *pAccess=&pDataInterface->m_DataAccess; 
	int nCount=0,nSize=0;
	long nRecord=pAccess->GetRecordCount();
	if (nRecord<=0 )
		return 0;
	pDataArray=new TDataSet[nRecord];
	pAccess->First();
	
	int nRowIndex=1,nFieldType,id=-1;
	CField* pField;
	while(!pAccess->Eof())
	{
		BOOL bInvalid=FALSE;
		/*for (i=0;!bInvalid&&i<nSize;i++)
		{
		
		  id=CAnalyze::GetFieldIndex(pDataInterface,NameList.GetAt(i));
		  pField=pAccess->m_pFieldList->FieldByIndex(id);
		  if(pDataInterface->IsMissingValue( pField, TRUE))//(pField->IsNull()))
		  {
		  bInvalid=TRUE;
		  
			}
			}
			if (bInvalid)
			{
			nRowIndex++;
			pAccess->Next();
			continue;
			}
		*/
		//check weight value
		if (m_bWeight)       //add by xds 20060713
		{
			id=m_nWeight;
			pField=pAccess->m_pFieldList->FieldByIndex(id);
			if(pDataInterface->IsMissingValue( pField, TRUE))//(pField->IsNull()))
			{
				bInvalid=TRUE;
				
			}
			else if( pField->GetAsDouble()<0)
				bInvalid=TRUE;
		}
		if (bInvalid)
		{
			nRowIndex++;
			pAccess->Next();
			continue;
		}
		
		if (strFactName!=_T(""))
		{
			id=CAnalyze::GetFieldIndex(pDataInterface,strFactName);
			if (id>=0)
			{
				pField=pAccess->m_pFieldList->FieldByIndex(id);
				if(pDataInterface->IsMissingValue( pField, TRUE))//(pField->IsNull()))
				{
					bInvalid=TRUE;
					
				}
			}
		}
		if (bInvalid)
		{
			nRowIndex++;
			pAccess->Next();
			continue;
		}
		id=CAnalyze::GetFieldIndex(pDataInterface,strSelName);
		pField=pAccess->m_pFieldList->FieldByIndex(id);
		nFieldType=pField->GetFieldType();
		if(!pDataInterface->IsMissingValue( pField, TRUE))//(pField->IsNull()))
		{
			if(nFieldType==fString)
				pDataArray[nCount].strVal=pField->GetAsString();
			else
			{
				pDataArray[nCount].dValue=pField->GetAsDouble();
			}			
			pDataArray[nCount++].iIndex=nRowIndex;
		}
		
		nRowIndex++;
		pAccess->Next();	
	}
	
	return nCount;
}
//add end
void CExplore::OnExplore(CDataInterface* pDataInterface)
{
	if(m_bStatistics && !m_bDescriptive && !m_bPercentiles && !m_bMEstimators
		&& !m_bOutliers)
		return ;
	
	int *pMethodS=NULL;
	int nMethodCount=GetMethodS(pMethodS);//ȡ�õ�ǰ��Ҫ��ֵ�ķ������䷽������
	if(nMethodCount<=0 || m_tDependent.iCount<=0)
		return ;
	
	//add by liytao 20030106
	int i=0;
	int nRC=pDataInterface->m_DataAccess.GetRecordCount();
	pWeight=new double [nRC];
	memset(pWeight ,0x00,sizeof(double)*nRC);
	if (m_bWeight)
	{
		m_pDataInterface->SetWeightNumb(m_nWeight);//��Ȩ������
		int nRet=pDataInterface->GetWeightData(pWeight);//Ȩ�ر���������
		if(nRet>0)
			m_bWeight=TRUE;
	}
	else 
	{
		for (i=0;i<nRC;i++)
			pWeight[i]=1;
	}
	//add end	
	TDataSet *pDataArray=NULL;//���ָ���е�����
	CRsltVariable* pVariable=NULL;//��������
	CStatistics statistics;
	//TStatBoxPlotS tStatPlotS;

	int nCount = 0;
	
	int kk=0,jj=0,jjg=0,kki=0,kkg=0,ii=0; //add by liytao 20030125
	//CRsltVariable *pDepeVariable;//add by liytao 20030125
	CRsltElementText* pText=NULL;//add by liytao 20030125
	if(m_tFactor.iCount>0)
	{
		BOOL bAddError=TRUE;
		for(kk=0;kk<m_tFactor.iCount;kk++)
		{
			//ȡ�����ӱ���������
			TDataSet *pFactArray=NULL;//���ָ���е�����
			int nFactIndex=m_tFactor.pValue[kk],
				nFactCount=(nFactIndex>=0)?pDataInterface->GetColData(nFactIndex,pFactArray,2):0;//�ڿ�ֵ��MissingValue�����ɾ���ü�¼
			//����һ�����ӱ����Ľ������
			CTString strFactName=pDataInterface->GetFieldName(nFactIndex);
			
			if(nFactCount>0)
			{
				TGroupInfor *pGroupS=NULL;//�����ӽ��з���
				int nGroup=CStatistics::GetGroupingData(pFactArray,nFactCount,pGroupS);//20030125
				if(nGroup>0)
				{
					if(nGroup>50) 
					{
						//add by liytao 20030225
						pText=new 	CRsltElementText("����("+strFactName+")");
						CTString strInfo;
						strInfo.Format("���ӱ���Ϊ%sʱ�������̫��û���㹻�ڴ����",strFactName.GetData());
						pText->AddString(strInfo);
						m_pResult->Add(pText);
						if(pFactArray)
						{
							delete [] pFactArray;
							pFactArray=NULL;
						}
						continue;
						//add end
					}
					int nFieldType=pDataInterface->GetFieldType(nFactIndex);
					CRsltVariable* pFactVariable=new CRsltVariable(strFactName);
					if((m_bBoth || m_bPlots) && nGroup>=300 && bAddError)
					{
						bAddError=FALSE;
						pText=new 	CRsltElementText("����("+strFactName+")");
						pText->AddString("�����ݷ��鳬��300ʱ���������к�״ͼ������");
						m_pResult->InsetAt(0,pText);
						//						AfxMessageBox("��ǰ���ݶԺ�״ͼ���������壬����������״ͼ");
					}
					//����ÿһ��Ӧ��������ڵ�ǰ���ӽ��з���
					for(jj=0;jj<m_tDependent.iCount;jj++)
					{
						//ȡ���Ա���������,������������п�ֵ�������뵱ǰ�е�ƽ��ֵ(1����ʾ��
						TDataSet *pDependArray=NULL;
						int nDependIndex=m_tDependent.pValue[jj],nDependCount;
						//nDependCount=(nDependIndex>=0)?pDataInterface->GetColData(nDependIndex,pDependArray):0; modified by liytao20030108
						//nDependCount=(nDependIndex>=0)?pDataInterface->GetColData(nDependIndex,pDependArray,2):0; modified by liytao 20030212
						CTString strDepName;
						if (nDependIndex>=0)
							strDepName=pDataInterface->GetFieldName(nDependIndex);
						nDependCount=(nDependIndex>=0)?GetValidData(pDataInterface,pDependArray,strDepName,strFactName):0;
						if(nDependCount>0)
						{
							////////////////�����װͼ///////////////
							CDoubleVector v,w;
							CTChartBoxplot* pCB = new CTChartBoxplot;
							pCB->SetParam(m_bWeight,FALSE,FALSE);	
							////////////////�����װͼ///////////////
							CTString strDepName=pDataInterface->GetFieldName(nDependIndex);
							//AfxMessageBox(strDepName);
							CRsltVariable *pDepeVariable=new CRsltVariable(strDepName);
							//�Ե�ǰ�Ա�����������������ӵ�ÿ�����ݽ�����������
							CTStringArray UnequalGroup;// add by liytao 20030124
							for(kki=0;kki<nGroup;kki++)
							{
								int nSubCount=pGroupS[kki].pIndex.iCount;
								if(nSubCount>0)
								{
									CTString strValue;
									CField *pField=pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nFactIndex);
			
									if(nFieldType!=fString)
									{
										//**************added by zkc 2004.5.18**************
										CTString labelVal;
										if(pDataInterface->GetShowValueLabel() && pField->GetValueVL(pGroupS[kki].dValue,labelVal))
										{
											strValue=labelVal;
										}
										else
										{
											strValue.Format("%.2f",pGroupS[kki].dValue);
										}
									}
									else
									{
										CTString labelVal;										
										if(pDataInterface->GetShowValueLabel() && pField->GetValueVL(pGroupS[kki].strVal,labelVal))
										{
											strValue=labelVal;
										}
										else
										{
											strValue=pGroupS[kki].strVal;
										}
									}
									//***************end of addition by zkc 2004.5.18******
									//int nDataCount=GetGroupData(pDependArray,nDependCount,pGroupS[kki].pIndex,pDataArray);
									int nDataCount=GetGroupDataW(pDependArray,pWeight,nDependCount,pGroupS[kki].pIndex,pDataArray,v,w);
									if( nDataCount>0)//add by liytao 20030212
									{
										if(!IsEqual(pDataArray,nDataCount))
										{//�����ǰ�������һ������������������ȣ��򲻽�������ͳ��
											CTString strGroup="",strdd="";
											strdd.Format("���ӱ���: %s=%s",strFactName.GetData(),strValue.GetData());
											strGroup.Format("%s=%s",strFactName.GetData(),strValue.GetData());
											CRsltVariable *pGroupVariable=new CRsltVariable(strGroup);
											CreateResult(strDepName,strdd,pDataArray,nSubCount,nDataCount,pMethodS,nMethodCount,pGroupVariable,pWeight);
											
											if(m_bBoth || m_bPlots)
												AddStemPlot(pGroupVariable,pDataArray,nDataCount,strDepName,strdd);
											
											pDepeVariable->Add(pGroupVariable);		
											//AfxMessageBox("get grouping stop");
										}
										//add by liytao 20030124
										else
										{
											CTString strInfo;
											strInfo.Format("%s =%s",strFactName.GetData(),strValue.GetData());
											UnequalGroup.Add(strInfo);
										}
										//add end
									}
									if((m_bBoth || m_bPlots) && nGroup<300 && nDataCount>0)
									{
										CIntVector indvec;
										TDataPointStrArr *tPointArr = NULL;
										pCB->CalcPercent(v,w,indvec);
										pCB->CalcBoxPlot(v,(void **)&tPointArr,nCount);
										tPointArr->strName += SEP_RISER + strValue;
										pCB->SetBoxplot((void *)(tPointArr),1,StrArr,"��״ͼ");//1��ʾͼ����
										delete tPointArr;
									}
									if(pDataArray)
									{
										delete [] pDataArray;
										pDataArray=NULL;
									}
								}
							}
							//add by liytao 20030124
							if ( UnequalGroup.GetSize()>0)
							{
								pText=new 	CRsltElementText("����");
								CTString str;
								CTString strTmp;
								for (i=0;i<UnequalGroup.GetSize();i++)
								{
									strTmp.Format("��%sʱ %s Ϊ����\n",UnequalGroup[i].GetData(),strDepName.GetData());
									str=str+strTmp;
								}
								pText->AddString(str);
								pDepeVariable->Add(pText); 
							}
							//add end
							UnequalGroup.RemoveAll(); //add by liytao 20030125
							//���ﻹҪ��������
							if((m_bBoth || m_bPlots) && nGroup<300)
							{
								pCB->SetXAxilLabel(strFactName);
								pCB->SetYAxilLabel(strDepName);
								CRsltElementChart* pREC = new CRsltElementChart("��״ͼ",pCB);
								pDepeVariable->Add(pREC); 
							}
							if(pDepeVariable->Count()>0)
								pFactVariable->Add(pDepeVariable);		
							else
							{
								delete pDepeVariable;
								pDepeVariable=NULL;
							}
						}
						if(pDependArray)
						{
							delete [] pDependArray;
							pDependArray=NULL;
						}
					}
					if(pFactArray)
					{
						delete [] pFactArray;
						pFactArray=NULL;
					}
					if(pFactVariable->Count()>0)
						m_pResult->Add(pFactVariable);
					else
					{
						delete pFactVariable;
						pFactVariable=NULL;
					}
				}
				else
				{
					//add by liytao 20030228
					pText=new 	CRsltElementText("����("+strFactName+")");
					CTString strInfo;
					strInfo.Format("���ӱ���Ϊ%sʱ����������Ч����,�޷�����",strFactName.GetData());
					pText->AddString(strInfo);
					m_pResult->Add(pText);
					if(pFactArray)
					{
						delete [] pFactArray;
						pFactArray=NULL;
					}
					continue;
					//add end				
				}
				if(pGroupS)
				{
					for( kkg=0;kkg<nGroup;kkg++)
						pGroupS[kkg].DeleteObject();
					delete [] pGroupS;
					pGroupS=NULL;
				}
			}
			else
			{
				//add by liytao 20030228
				pText=new 	CRsltElementText("����("+strFactName+")");
				CTString strInfo;
				strInfo.Format("���ӱ���Ϊ%sʱ����Ч����,�޷�����",strFactName.GetData());
				pText->AddString(strInfo);
				m_pResult->Add(pText);
				if(pFactArray)
				{
					delete [] pFactArray;
					pFactArray=NULL;
				}
				continue;
				//add end
			}
		}
	}
	else
	{
		for(ii=0;ii<m_tDependent.iCount;ii++)
		{
			int index=m_tDependent.pValue[ii],count;//ȡ���ֶε��к�
			//count=(index>=0)?pDataInterface->GetColData(index,pDataArray):0;
			//count=(index>=0)?pDataInterface->GetColData(index,pDataArray,2):0;//ȡ�����ݼ��е�ǰ�е����ݼ������modified by liytao 20030212
			CTString strDepName;
			if (index>=0)
			{
				strDepName=pDataInterface->GetFieldName(index);
				count=(index>=0)?GetValidData(pDataInterface,pDataArray,strDepName,_T("")):0;
			}
			else
				count=0;
			if(count>0)
			{
				CTString strName=pDataInterface->GetFieldName(index);				
				pVariable=new CRsltVariable(strName);
				CreateResult(strName,"",pDataArray,pDataInterface->GetRowCount(),count,pMethodS,nMethodCount,pVariable,pWeight);
				//��ͳ��ͼ������
				if(m_bBoth || m_bPlots)
				{
					AddStemPlot(pVariable,pDataArray,count,strName,"");
					AddBoxPlot(pVariable,pDataArray,count,strName);
				}
			}
			
			if(pDataArray)
			{
				delete [] pDataArray;
				pDataArray=NULL;
			}
			if(pVariable && pVariable->Count()>0) //modify by xds 20060306
				m_pResult->Add(pVariable);
			else
			if (pVariable)
			{
				delete pVariable;
				pVariable=NULL;
			}
		}
	}
	delete [] pMethodS;
	pMethodS=NULL;
	delete []pWeight;//add by liytao 20030227
	pWeight=NULL;
}

//add by liytao 20030108
void  CExplore::CreateResult(CTString strTitle,CTString strHead,TDataSet* pDataArray,int nSumData,int nData,int *pMethodS,int nMethodCount,CRsltVariable* &pVariable,double weight[])
{
	/// ��ͳ�Ʊ�����
	CTLTable* pTable=NULL;//�����
	CRsltElementTable* pEle=NULL;//��Ԫ�ض���
	CStatistics Statistics;
	CTString  string="";
	double dbW=Statistics.GetWeighted(pDataArray,nData,weight);
	Statistics.SetData(pDataArray,nData,dbW,weight);
	int j=0;//add by liytao 20030125
	double *rtValue=NULL;
	if(m_bBoth || m_bStatistics)
	{
		//������ͳ������
		if(m_bDescriptive)
		{
			///�����ۼ�̬�Ʊ�
			double xx=0,yy=0;
			//double *rtValue=Statistics.Describle(pDataArray,nData,pMethodS,nMethodCount); modified by liytao 20030111
			rtValue=Statistics.Describle(pDataArray,nData,pMethodS,nMethodCount,1);
			//rtValue=Statistics.Describle(pDataArray,nData,pMethodS,nMethodCount,1);
			if  (Statistics.bRet)
			{
				pTable=CreateTableS(1,strTitle,strHead);
				pTable->SetItemText(0,1,rtValue[5]);
				pTable->SetItemText(0,2,rtValue[6]);
				pTable->SetItemText(1,1,rtValue[3]);
				pTable->SetItemText(2,1,rtValue[13]);
				pTable->SetItemText(3,1,rtValue[1]);
				pTable->SetItemText(4,1,rtValue[2]);
				pTable->SetItemText(5,1,rtValue[14]);
				//Statistics.f_conf_mean(m_nConfidence,xx,yy); modified by liytao 20030111
				Statistics.f_conf_mean(m_nConfidence,xx,yy,1);	
				pTable->SetItemText(6,1,xx);
				pTable->SetItemText(7,1,yy);
				pTable->SetItemText(8,1,rtValue[4]);
				//pTable->SetItemText(9,1,nData); modified by liytao 20030108
				pTable->SetItemText(9,1,(int)(dbW+0.5));
				pTable->SetItemText(10,1,(nSumData-nData>0)?nSumData-nData:0);
				pEle=new CRsltElementTable("��������",pTable);
				pVariable->Add(pEle);
				
				///������ɢ̬�Ʊ�
				pTable=CreateTableS(2,strTitle,strHead);
				pTable->SetItemText(0,1,rtValue[0]);
				pTable->SetItemText(1,1,rtValue[15]);
				pTable->SetItemText(2,1,rtValue[8]);
				pTable->SetItemText(3,1,rtValue[7]);
				if (fabs(rtValue[5]) > DOUBLE_ZERO)
					pTable->SetItemText(4,1,rtValue[7]/rtValue[5]);
				else
					pTable->SetItemText(4,1,"");
				
				pTable->SetItemText(5,1,rtValue[9]);
				pTable->SetItemText(5,2,rtValue[10]);
				pTable->SetItemText(6,1,rtValue[11]);
				pTable->SetItemText(6,2,rtValue[12]);
				pEle=new CRsltElementTable("��ɢ�̶�",pTable);
				pVariable->Add(pEle);
			}
			else
			{
				
				//CRsltElementText *pText=new CRsltElementText("����");
				//CTString str;
				//str.Format("�޷�������(%s)",strHead);
				//pText->AddString(str);
				//pVariable->Add(pText);
			}
			
			delete []rtValue;
			rtValue=NULL;
		}
		
		//�������С�����ֵ
		if(m_bOutliers)
		{
			int nMaxMin[2];
			TDataSet *pColMaxArray=NULL,*pColMinArray=NULL;
			nMaxMin[0]=Statistics.f_extrmax(pColMaxArray);
			nMaxMin[1]=Statistics.f_extrmin(pColMinArray);
			if(nMaxMin[0]>0&&nMaxMin[1]>0) //add by liytao 20030209
			{
				///�����쳣ֵֵ��
				pTable=CreateTableS(3,strTitle,strHead,nMaxMin);
				for(j=0;j<nMaxMin[0];j++)
				{
					pTable->SetItemText(j,1,pColMaxArray[j].iIndex);
					pTable->SetItemText(j,2,pColMaxArray[j].dValue);
				}
				for(j=0;j<nMaxMin[1];j++)
				{
					pTable->SetItemText(j+nMaxMin[0],1,pColMinArray[j].iIndex);
					pTable->SetItemText(j+nMaxMin[0],2,pColMinArray[j].dValue);
				}
				
				pEle=new CRsltElementTable("��ֵ",pTable);
				pVariable->Add(pEle);
			}
			else
			{
				CRsltElementText* pText=new CRsltElementText("���棺��ֵ" );
				CTString str;
				str=_T("--��Ч����̫�٣��޷����ɼ�ֵ��\n");
				pText->AddString(str);
				pVariable->Add(pText);
				
			}
			delete [] pColMaxArray;
			pColMaxArray=NULL;
			delete [] pColMinArray;
			pColMinArray=NULL;
		}
		
		//��ٷ�λ��
		if(m_bPercentiles)
		{
			rtValue=NULL;
			//int nCount=Statistics.f_percentile(rtValue); modified by liytao 20030115
			int nCount=Statistics.f_percentile(rtValue,1);
			
			///�����ٷ�λ����
			pTable=CreateTableS(4,strTitle,strHead);
			for(j=0;j<nCount;j++)
			{
				string.Format("%3.2f",rtValue[j]);
				pTable->SetItemText(0,j+1,string);					
			}
			
			pEle=new CRsltElementTable("�ٷ�λ��",pTable);
			pVariable->Add(pEle);
			
			delete [] rtValue;
			rtValue=NULL;
		}
	}
}
//add end

//��������
CTLTable* CExplore::CreateTableS(int which,CTString strTitle,CTString strHead,int nMaxMin[2])
{
	CTLTable *pTable=NULL;
	int ii=0,nRowHeight=25,nPreWidth=0,nHorzAngle=0;
	CTString string,str;
	switch(which)
	{
	case 1://�����ۼ�̬�Ʊ�
	case 2://������ɢ̬�Ʊ�
		pTable=new CTLTable;
		pTable->CreateTable(2, 2);
		pTable->SetCols(3);
		pTable->SetRows(which==1?12:7);
		str=which==1?"��������":"��ɢ�̶�";
		string.Format("%s",str.GetData());
		pTable->SetTitle(string);
		pTable->InsertColumn(0,strTitle);
		pTable->InsertColumn(1,"ͳ��");
		pTable->InsertColumn(2,"��׼���");
		if(which==1)
		{
			pTable->InsertItem(0,"��ֵ",true);
			pTable->InsertItem(1,"��λ��",true);
			pTable->InsertItem(2,"����",true);
			pTable->InsertItem(3,"��Сֵ",true);
			pTable->InsertItem(4,"���ֵ",true);
			pTable->InsertItem(5,"5%��β��ֵ",true);
			string.Format("%d%s",m_nConfidence,"%���� ����");
			pTable->InsertItem(6,string,true);
			pTable->InsertItem(7,"  �� ���� ����",true);
			pTable->InsertItem(8,"�ܺ�",true);
			//pTable->InsertItem(9,"��Ч��¼",true); modified by liytao 20030108
			if (m_bWeight )
				pTable->InsertItem(9,"Ȩ�غ�",true);
			else
				pTable->InsertItem(9,"��Ч��¼",true);
			pTable->InsertItem(10,"ȱʧ��¼",true);
		}
		else
		{
			pTable->InsertItem(0,"����",true);
			pTable->InsertItem(1,"���䷶Χ",true);
			pTable->InsertItem(2,"����",true);
			pTable->InsertItem(3,"��׼��",true);
			pTable->InsertItem(4,"����ϵ��",true);
			pTable->InsertItem(5,"ƫ��",true);
			pTable->InsertItem(6,"���",true);
		}
		break;
	case 3:   //�����쳣ֵ��
		{
			pTable=new CTLTable;
			int maxminRows=nMaxMin[0]+nMaxMin[1];
			pTable->CreateTable(2, 2);
			pTable->SetCols(3);
			pTable->SetRows(maxminRows+1);
			string.Format("%s--%s",strTitle.GetData(),"��ֵ");
			pTable->SetTitle(string);
			pTable->InsertColumn(0,"");
			pTable->InsertColumn(1,"����");
			pTable->InsertColumn(2,"����");
			for(ii=0;ii<nMaxMin[0];ii++)
			{
				string.Format("%s%d",ii==0?"���ֵ ":"������ ",ii+1);
				pTable->InsertItem(ii,string,true);
			}
			for(ii=0;ii<nMaxMin[1];ii++)
			{
				string.Format("%s%d",ii==0?"��Сֵ ":"������ ",ii+1);
				pTable->InsertItem(ii+nMaxMin[0],string,true);
			}
		}
		break;
	case 4://�����ٷ�λ����
		{
			pTable=new CTLTable;
			pTable->CreateTable(2, 2);
			pTable->SetCols(8);
			pTable->SetRows(2);
			string.Format("%s--%s",strTitle.GetData(),"�ٷ�λ��");
			pTable->SetTitle(string);
			
			pTable->InsertColumn(0,"�ٷ�λ��");
			double percent[7]={0.05,0.1,0.25,0.50,0.75,0.90,0.95};
			for(ii=0;ii<7;ii++)
			{
				string.Format("%3.2f",percent[ii]);
				pTable->InsertColumn(ii+1,string);
			}
			pTable->InsertItem(0,"��Ӧֵ",true);
		}
		break;
	default:
		return NULL;
	}
	pTable->SetHead(strHead);
	return pTable;
}

void CExplore::AddBoxPlot(CRsltVariable* pVariable,TDataSet *pDataArray,int count,CTString strName)
{
	TDataPointStrArr tPointArr;
	CStatistics Statistics;
	double dbW=Statistics.GetWeighted(pDataArray,count,pWeight);
	Statistics.SetData(pDataArray,count,dbW,pWeight);
	TDoubleStruct dstruct;
    TPlotReco  tPlotReco;
	dstruct.nCount=Statistics.f_plotval(dstruct.dValue,TRUE,tPlotReco,1);

	tPointArr.strName = strName;
	for(int i=0;i<dstruct.nCount;i++)
	{
		tPointArr.arrYVal.Add(dstruct.dValue[i]);  
	}
	if(dstruct.dValue)
		delete [] dstruct.dValue;
	dstruct.dValue=NULL;
	
	CTChartBoxplot* pCB=new CTChartBoxplot;
	pCB->SetBoxplot((void *)(&tPointArr),1,StrArr,"��״ͼ");
	CRsltElementChart* pREC=new CRsltElementChart("��״ͼ",pCB);
	pVariable->Add(pREC); 
}	

TStatBoxPlot CExplore::GetPlotBox(TDataSet *pDataArray,int count,CTString strName)
{
	CStatistics Statistics;
	Statistics.SetData(pDataArray,count);
	TPlotReco  tPlotReco;
	TStatBoxPlot tStatPlot;
	tStatPlot.nCount=count;
	tStatPlot.pPlotVal.nCount=Statistics.f_plotval(tStatPlot.pPlotVal.dValue,FALSE,tPlotReco);
	return tStatPlot;
}

void CExplore::AddStemPlot(CRsltVariable* pVariable,TDataSet *pDataArray,int count,CTString strName,CTString strGroup)
{
	/*
	CStatistics Statistics;
	//Statistics.SetData(pDataArray,count); modified by liytao 20030108
	double dbW=Statistics.GetWeighted(pDataArray,count,pWeight);
	Statistics.SetData(pDataArray,count,dbW,pWeight);
	
	double dStemLeaf=0; TStemLeaf* pStemLeaf=NULL;
	int nStemLeaf=Statistics.f_StemLeaf(pStemLeaf,dStemLeaf,1);
	
	CTString str,string,string2,string3;
	CRsltElementText *pText = new CRsltElementText("��Ҷͼ");
	
	string.Format("%s ��Ҷͼ",strName);
	pText->AddString(string);
	
	pText->AddString(strGroup);
	
	string.Format("%s"," Ƶ ��  ��           ��  :  Ҷ");
	pText->AddString(string);
	
	string=string2=string3=_T("");
	for(int ii=0;ii<nStemLeaf;ii++)
	{
		str.Format("%s\n",pStemLeaf[ii].freq);
		string+=str;
		str.Format("%s%s\n",pStemLeaf[ii].stem," . ");
		string2+=str;
		str.Format("%s\n",pStemLeaf[ii].leaf);
		string3+=str;
	}
	
	pText->AddString(string);
	pText->AddString(string2);
	pText->AddString(string3);
	
	string.Format("%s%4.2f\n%s","֦�Ŀ��:����" ,dStemLeaf,"ÿ��Ҷ:������1������");
	pText->AddString(string);
	
	pVariable->Add(pText);
	
	delete [] pStemLeaf;
	pStemLeaf=NULL;
	*/
}


/////////////////////////////////////////////////////////////////////////
const char ALLPRECET[] = _T("100.00000");
CFrequency::CFrequency()
{
	int *pValue=new int[14];
	pValue[0]=5;   //ƽ��ֵ
	pValue[1]=4;   //�ܺ�
	pValue[2]=1;   //��Сֵ
	pValue[3]=2;   //���ֵ
	pValue[4]=3;   //��λ��
	pValue[5]=13;  //����
	pValue[6]=7;   //��׼��
	pValue[7]=0;   //����
	pValue[8]=8;   //����
	pValue[9]=6;   //��ֵ��׼��
	
	pValue[10]=9;  //ƫ
	pValue[11]=10; //ƫ��
	pValue[12]=11; //��
	pValue[13]=12; //����
	
	m_tStatMethod=TIntStruct(14,pValue);
	//add end
	m_bWeight = FALSE;
	m_pResult = NULL;
	m_ValConfArray.RemoveAll();
}

CFrequency::~CFrequency()
{
	if (m_tStatMethod.iCount>0)
		m_tStatMethod.DeleteObject();//Add by xds 20060106.
	if (m_tFrequency.iCount>0)
		m_tFrequency.DeleteObject();
	//Release memory
	Free(&m_ValConfArray);
}

void CFrequency::Free(CPtrArray *par)
{
	CFreqConf *pfC = NULL;
	int nSize = m_ValConfArray.GetSize();
	for (int i=0;i<nSize;i++)
	{
		pfC = (CFreqConf *)(m_ValConfArray.GetAt(i));
		delete pfC;
	}
	m_ValConfArray.RemoveAll();	
}

BOOL CFrequency::IsOverDisp(int nColIndex)
{
	if (m_nCurRows >= DISP_MAX/2)
	{
		CTString szWarning("");
		szWarning.Format("ע������ %s �ڷ���������̫�󣬲��ʺ�����ǰ������\n�ֽ�����������ݣ��Ҳ���ͳ�����ݿ���������ע�⣡",m_pDataInterface->GetFieldName(nColIndex).GetData());
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
		pWarningTextRslt->AddString( szWarning );
		m_pResult->Add( pWarningTextRslt );
		return TRUE;
	}
	return FALSE;
}

//FREQUENCY
//	/DATA=[FILE]
//	/VARLIST [varlist] 
//	/WEIGHT [varname]
//	/DISPLAY [TOTAL**] [MEAN**] [MIN**] [MAX**] [VAR**] [STDDEV**]
//		 [RANGE**] [MEANSTDDEV**] [MOD**] [MEDIAN**] [SKEW**] [QURT**]
//	/VARRANGE [varname({0},{1},{10})]...
BOOL CFrequency::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	CTStringArray xname;
	TIntStruct tVarTemp;

	AnaWord VOstr;
	VOstr.Import(VO);
	//������Ȩ��
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return FALSE;
		if (tVarTemp.iCount == 1)
			m_nWeight = tVarTemp.pValue[0];
		else
			return FALSE;
		tVarTemp.DeleteObject();
		m_bWeight = TRUE;
	}
	else
	{
		m_bWeight = FALSE;
	}
	//����������
	xname.RemoveAll();
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tFrequency)))
		return FALSE;
	//ͳ�Ʒ���
	int nIndex = 0, nCount = 0, nVar = 0;
	if (AnaSyntax::findstr("/DISPLAY", "RANGE", VOstr))
	{
		nCount ++;
		nVar = 1;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MIN", VOstr))
	{
		nCount ++;
		nVar |= 2;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MAX", VOstr))
	{
		nCount ++;
		nVar |= 4;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MEDIAN", VOstr))
	{
		nCount ++;
		nVar |= 8;
	}
	if (AnaSyntax::findstr("/DISPLAY", "TOTAL", VOstr))
	{
		nCount ++;
		nVar |= 16;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MEAN", VOstr))
	{
		nCount ++;
		nVar |= 32;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MEANSTDDEV", VOstr))
	{
		nCount ++;
		nVar |= 64;
	}
	if (AnaSyntax::findstr("/DISPLAY", "STDDEV", VOstr))
	{
		nCount ++;
		nVar |= 128;
	}
	if (AnaSyntax::findstr("/DISPLAY", "VAR", VOstr))
	{
		nCount ++;
		nVar |= 256;
	}
	if (AnaSyntax::findstr("/DISPLAY", "SKEW", VOstr))
	{
		nCount ++;
		nCount ++;
		nVar |= 512;
	}
	if (AnaSyntax::findstr("/DISPLAY", "QURT", VOstr))
	{
		nCount ++;
		nCount ++;
		nVar |= 1024;
	}
	if (AnaSyntax::findstr("/DISPLAY", "MOD", VOstr))
	{
		nCount ++;
		nVar |= 2048;
	}
	if(nCount>0)
	{
		int *pValue=new int[nCount];
		memset(pValue,0,sizeof(int)*nCount);
		if (nVar & 1)//����
		{
			pValue[nIndex]=0;
			nIndex ++;
		}
		if (nVar & 2)//��Сֵ
		{
			pValue[nIndex]=1; 
			nIndex ++;
		}
		if (nVar & 4)//���ֵ
		{
			pValue[nIndex]=2; 
			nIndex ++;
		}
		if (nVar & 8)//��λ��
		{
			pValue[nIndex]=3; 
			nIndex ++;
		}
		if (nVar & 16)//�ܺ�
		{
			pValue[nIndex]=4; 
			nIndex ++;
		}
		if (nVar & 32)//ƽ��ֵ
		{
			pValue[nIndex]=5; 
			nIndex ++;
		}
		if (nVar & 64)//��ֵ��׼��
		{
			pValue[nIndex]=6; 
			nIndex ++;
		}
		if (nVar & 128)//��׼��
		{
			pValue[nIndex]=7; 
			nIndex ++;
		}
		if (nVar & 256)//����
		{
			pValue[nIndex]=8; 
			nIndex ++;
		}
		if (nVar & 512)//ƫ��ϵ��
		{
			pValue[nIndex]=9; 
			nIndex ++;
			pValue[nIndex]=10; //ƫ��ϵ�����
			nIndex ++;
		}
		if (nVar & 1024)//���
		{
			pValue[nIndex]=11; 
			nIndex ++;
			pValue[nIndex]=12;//���ϵ�����
			nIndex ++;
		}
		if (nVar & 2048)//����
		{
			pValue[nIndex]=13; 
			nIndex++;
		}
		m_tStatMethod.DeleteObject();
		m_tStatMethod=TIntStruct(nCount,pValue);
	}
	//Range
	//	/VARRANGE [varname({0},{1},{10})]...
	//�������
	if (AnaSyntax::exist("/VARRANGENUM", VOstr))
	{
		int strPos = AnaSyntax::FindFirstWordPos("/VARRANGENUM", VOstr);
		int endPos = VOstr.GetWordCount();
		assert(strPos>0 && strPos<=endPos);
		int i=0;
		int j=0;
		int numVar = xname.GetSize();
		for (i=strPos+1; i<endPos; i++)
		{
			if (VOstr.GetAt(i)[0] == '/')
				break;
			for (j=0; j<numVar; j++)
			{
				if (xname.GetAt(j) == VOstr.GetAt(i))
				{
					CFreqConf *pConf = new CFreqConf;
					pConf->m_nFldIndex = m_tFrequency.pValue[j];
					pConf->m_fRangeFrom = AnaSyntax::getvalue("/VARRANGENUM", xname.GetAt(j).GetData(), VOstr);
					pConf->m_fRangeTo = AnaSyntax::getvalue("/VARRANGENUM", xname.GetAt(j).GetData(), VOstr, 2);
					pConf->m_fRangeNum = AnaSyntax::getvalue("/VARRANGENUM", xname.GetAt(j).GetData(), VOstr, 3);
					pConf->m_nRangeOpt = RANGE_NUM;
					m_ValConfArray.Add(pConf);
				}
			}
		}//for /VARRANGE ��ߵ�
	}//if

	//���䳤�� 
	if (AnaSyntax::exist("/VARRANGELENGTH", VOstr))
	{
		int strPos = AnaSyntax::FindFirstWordPos("/VARRANGELENGTH", VOstr);
		int endPos = VOstr.GetWordCount();
		assert(strPos>0 && strPos<=endPos);
		int i=0;
		int j=0;
		int numVar = xname.GetSize();
		for (i=strPos+1; i<endPos; i++)
		{
			if (VOstr.GetAt(i)[0] == '/')
				break;
			for (j=0; j<numVar; j++)
			{
				if (xname.GetAt(j) == VOstr.GetAt(i))
				{
					CFreqConf *pConf = new CFreqConf;
					pConf->m_nFldIndex = m_tFrequency.pValue[j];
					pConf->m_fRangeFrom = AnaSyntax::getvalue("/VARRANGELENGTH", xname.GetAt(j).GetData(), VOstr);
					pConf->m_fRangeTo = AnaSyntax::getvalue("/VARRANGELENGTH", xname.GetAt(j).GetData(), VOstr, 2);
					pConf->m_fRangeNum = AnaSyntax::getvalue("/VARRANGELENGTH", xname.GetAt(j).GetData(), VOstr, 3);
					pConf->m_nRangeOpt = RANGE_LENGTH;
					m_ValConfArray.Add(pConf);
				}
			}
		}//for /VARRANGE ��ߵ�
	}//if

	return TRUE;
}

//��ǰ���Ƿ�������䷶Χ
BOOL CFrequency::IsExistRange(int nColIndex)
{
	if (m_ValConfArray.GetSize() < 1) return FALSE;
	for (int i=0;i<m_ValConfArray.GetSize();i++)
	{
		CFreqConf *pFreqTemp = (CFreqConf *)m_ValConfArray.GetAt(i);
		if (pFreqTemp->m_nFldIndex != nColIndex)
			continue;
		if (pFreqTemp->m_fRangeNum > - DOUBLE_ZERO && pFreqTemp->m_fRangeNum < DOUBLE_ZERO) return FALSE;
		return TRUE;
	}
	return FALSE;
}

//����������Ϣ
void CFrequency::CreatePtrArray(CPtrArray &arrFld, int nColIndex)
{
	CFreqConf *pFreq = NULL;
	if (m_ValConfArray.GetSize() < 1) return;
	int i;
	for (i=0;i<m_ValConfArray.GetSize();i++)
	{
		CFreqConf *pFreqTemp = (CFreqConf *)m_ValConfArray.GetAt(i);
		if (pFreqTemp->m_nFldIndex != nColIndex)
			continue;
		pFreq = pFreqTemp;
		break;
	}
	if (pFreq == NULL) return;
	int    nDV = 0;  //����
	double fStep = 0;//����
	BOOL   bCalc = FALSE;
	if (pFreq->m_nRangeOpt == RANGE_NUM)//�������
	{
		nDV = int(pFreq->m_fRangeNum);
		fStep = (pFreq->m_fRangeTo - pFreq->m_fRangeFrom) / nDV;
	}
	else
	{
		fStep = pFreq->m_fRangeNum;
		nDV = int((pFreq->m_fRangeTo - pFreq->m_fRangeFrom) / fStep);
		if (fStep*nDV < pFreq->m_fRangeTo - pFreq->m_fRangeFrom - DOUBLE_ZERO)
			nDV += 1;
	}
	CFreqConf *pFreqRange = new CFreqConf[nDV];
	for (i=0;i<nDV;i++) //�����ֵ
	{
		if (i==0)
		{
			pFreqRange[i].m_fRangeFrom = pFreq->m_fRangeFrom;
		}
		else
		{
			pFreqRange[i].m_fRangeFrom = pFreqRange[i-1].m_fRangeTo;
		}
		if (i==nDV-1)
		{
			pFreqRange[i].m_fRangeTo = pFreq->m_fRangeTo;
		}
		else
		{
			pFreqRange[i].m_fRangeTo = pFreqRange[i].m_fRangeFrom + fStep;
		}
	}
	//��ֵ��array
	for (i=0;i<nDV;i++)
	{
		CRangeInfo *pRange = new CRangeInfo;
		pRange->fValFrom = pFreqRange[i].m_fRangeFrom;
		pRange->fValTo   = pFreqRange[i].m_fRangeTo;
		arrFld.Add(pRange);
	}
	//��������������
	if (nDV >0)
	{
		CRangeInfo *pRange = new CRangeInfo;
		pRange->fValFrom = MIN_VALUE;
		pRange->fValTo   = pFreqRange[0].m_fRangeFrom;
		arrFld.InsertAt(0,pRange);
		pRange = new CRangeInfo;
		pRange->fValFrom = pFreqRange[nDV-1].m_fRangeTo;
		pRange->fValTo   = MAX_VALUE;
		arrFld.Add(pRange);
		//
		pRange = (CRangeInfo *)arrFld.GetAt(0);
	}
	else
	{
		CRangeInfo *pRange = new CRangeInfo;
		pRange->fValFrom = MIN_VALUE;
		pRange->fValTo   = MAX_VALUE;
		arrFld.Add(pRange);
	}
	//�ͷ��ڴ�
	delete []pFreqRange;
}

//Ƶ�ʷ�������ں���,ͳһ�ӿ�,xds.
CResult* CFrequency::OnFrequency(CDataInterface* pDataInterface, string VO)
{
	//��ʼ��
	m_pResult        = NULL;
	m_pDataInterface = pDataInterface;
	//��������
	m_pResult = new CResult(_T("Ƶ�ʷ���"));//����һ����������
	//�������
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "��ȡ���������������������У�";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "����!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//���㲢��ʾ
	DispFrequency();
	return m_pResult;
}
//nType=0,����;1,+;-1,-
void CFrequency::InsertTable(double fValueFrom, double fValueTo, double fFreValue, double fFreTotal , CTLTable* pTable, int nType)
{
	CTString szTemp("");
	
	if (nType == -1)
		szTemp.Format("��-�ޣ�%.2f��",fValueTo);
	else if (nType == 0)
		szTemp.Format("��%.2f��%.2f��",fValueFrom,fValueTo);
	else if (nType == 1)
		szTemp.Format("��%.2f��+�ޣ�",fValueFrom);
	else
		szTemp.Format("��-�ޣ�+�ޣ�");
	
	//�����¼
	pTable->InsertItem(m_nCurRows,_T(szTemp),TRUE);
	pTable->SetItemText(m_nCurRows,1,int(fFreValue));
	if (m_n64Weight > 0)
		szTemp.Format("%.5f",100*double(fFreValue/m_n64Weight));
	else
		szTemp = _T("NA");
	pTable->SetItemText(m_nCurRows,2,szTemp);
	if (m_n64Weight > 0)
		szTemp.Format("%.5f",100*double(fFreTotal/m_n64Weight));
	else
		szTemp = _T("NA");
	pTable->SetItemText(m_nCurRows++,3,szTemp);
}

void CFrequency::InsertTable(CMapManager &dataMap, int nColIndex, double fMapValue, double fFreValue, double fFreTotal , CTLTable* pTable ,int nType ,BOOL bValid)
{
	CTString szTemp("");

	int nCol = m_bWeight ? 1 : 0;

	if (!bValid) //�����ںϷ��ı�������
	{
		pTable->InsertItem(0,_T("ȱʧֵ"),TRUE);
		szTemp.Format("%u",m_n64Weight);
		pTable->SetItemText(0,1,szTemp);
		pTable->SetItemText(0,2,m_n64Weight == 0 ? "NA" : ALLPRECET);
		pTable->SetItemText(0,3,ALLPRECET);
		
		pTable->InsertItem(1,_T("�ܼ�"),TRUE);
		szTemp.Format("%u",m_n64Weight);
		pTable->SetItemText(1,1,szTemp);
		pTable->SetItemText(1,2,ALLPRECET);
		pTable->SetItemText(1,3,"");
		return;
	}
	//ȱʧֵ
	if (nType == 0)
	{
		/*
		if (m_pDataInterface->GetFieldType(nColIndex) == fString)
			szTemp = dataMap.GetStrValue(nCol,int(fMapValue));
		else
			szTemp.Format("%f",fMapValue);
		*/
		szTemp = GetLabel(dataMap,nCol,nColIndex,fMapValue);
		//�����¼
		pTable->InsertItem(m_nCurRows,_T(szTemp),TRUE);
		pTable->SetItemText(m_nCurRows,1,int(fFreValue));
		if (m_n64Weight > 0)
			szTemp.Format("%.5f",100*double(fFreValue/m_n64Weight));
		else
			szTemp = _T("NA");
		pTable->SetItemText(m_nCurRows,2,szTemp);
		if (m_n64Weight > 0)
			szTemp.Format("%.5f",100*double(fFreTotal/m_n64Weight));
		else
			szTemp = _T("NA");
		pTable->SetItemText(m_nCurRows++,3,szTemp);
	}
	else
	if (nType == 1)
	{
		/*
		if (m_pDataInterface->GetFieldType(nColIndex) == fString)
			szTemp = dataMap.GetStrValue(nCol,int(fMapValue));
		else
			szTemp.Format("%f",fMapValue);
		*/
		szTemp = GetLabel(dataMap,nCol,nColIndex,fMapValue);
		//�����¼
		pTable->InsertItem(m_nCurRows,_T(szTemp),TRUE);
		pTable->SetItemText(m_nCurRows,1,int(fFreValue));
		if (m_n64Weight > 0)
			szTemp.Format("%.5f",100*double(fFreValue/m_n64Weight));
		else
			szTemp = _T("NA");
		pTable->SetItemText(m_nCurRows,2,szTemp);
		if (__int64(fFreTotal) != m_n64Weight)
		{
			if (m_n64Weight > 0)
				szTemp.Format("%.5f",100*double(fFreTotal/m_n64Weight));
			else
				szTemp = _T("NA");
		}
		else
		{
			szTemp = ALLPRECET;
		}
		pTable->SetItemText(m_nCurRows++,3,szTemp);
	}
	else
	if (nType == 2)
	{
		if (__int64(fFreTotal) != m_n64Weight)
		{
			pTable->InsertItem(m_nCurRows,_T("ȱʧֵ"),TRUE);
			pTable->SetItemText(m_nCurRows,1,int(m_n64Weight-fFreTotal));
			if (m_n64Weight > 0)
				szTemp.Format("%.5f",100*double((m_n64Weight-fFreTotal)/m_n64Weight));
			else
				szTemp = _T("NA");
			pTable->SetItemText(m_nCurRows,2,szTemp);
			pTable->SetItemText(m_nCurRows++,3,ALLPRECET);
		}
	}
	else
	if (nType == 3)
	{
		//�ܵ�
		pTable->InsertItem(m_nCurRows,_T("�ܼ�"),TRUE);
		szTemp.Format("%u",m_n64Weight);
		pTable->SetItemText(m_nCurRows,1,szTemp);
		pTable->SetItemText(m_nCurRows,2,ALLPRECET);
		pTable->SetItemText(m_nCurRows++,3,"");
	}
}
//nType:0����ȷ���ݣ�1���Ƿ����ݣ�ȫȱʧ��2���������ƣ�����ʾ ��
void CFrequency::CreateFreqTable(CDoubleMatrix &dataMatrix, CMapManager &dataMap, int nColIndex, CPtrArray *pArrFld,int nType)
{
	int i = 0, j = 0, nSize = 0;
	double  fFreValue = 0, fFreTotal = 0, fRangeTotal = 0;
	CFldInfo *pFld = NULL;
	CRangeInfo *pRange = NULL;
	CTString szTemp(""),szValName("");
		
	BOOL bRange = IsExistRange(nColIndex);//�Ƿ�������仮��
	m_nCurRows = 0;
	CTLTable* pTable=new CTLTable;
	if (nType == 0 || nType == 1)
	{
		pTable->CreateTable(2,2);
		pTable->SetCols(4);
		szTemp = m_pDataInterface->GetFieldName(nColIndex);
		szValName.Format("%s  %s",szTemp.GetData(),"Ƶ�ʱ�");
		pTable->SetTitle(szValName);
		
		pTable->InsertColumn(0,"����ֵ");
		pTable->InsertColumn(1,"Ƶ  ��");
		pTable->InsertColumn(2,"�ٷֱ�");
		pTable->InsertColumn(3,"�ۼưٷֱ�");
	}
	else
	{
		pTable->CreateTable(2,2);
		pTable->SetCols(1);
		pTable->SetRows(1);
		szTemp.Format("ע������ %s �ڷ���������̫�󣬲��ʺ�����ǰ��������ע�⣡",m_pDataInterface->GetFieldName(nColIndex).GetData());
		pTable->InsertColumn( 0, szTemp);
		//Add
		CRsltVariable *pVariable = new CRsltElementTable(szTemp,pTable);
		m_pResult->Add(pVariable);
		return;
	}
	//Ȩ�غ�
	if (nType == 1) //�����ںϷ��ı�������
	{
		InsertTable(dataMap,nColIndex,0,0,0,pTable,0,FALSE);
		//Add
		CRsltVariable *pVariable = new CRsltElementTable(szTemp,pTable);
		m_pResult->Add(pVariable);
		return;
	}
	//Ƶ������
	nSize = pArrFld->GetSize();
	if (bRange)//��ǰָ�����д������仮��
	{
		for (i=0;i<nSize;i++)
		{
			pRange = (CRangeInfo *)pArrFld->GetAt(i);
			fFreTotal += pRange->fTotalWeight;
			if (nSize > 1) //������
			{
				InsertTable(pRange->fValFrom, pRange->fValTo, pRange->fTotalWeight, fFreTotal,pTable,
					i==0 ? -1 :(i==nSize-1 ? 1 : 0));
			}
			else
			{
				InsertTable(pRange->fValFrom, pRange->fValTo, pRange->fTotalWeight, fFreTotal,pTable,2);
			}	
		}
		//�Ƿ����ȱʧֵ
		InsertTable(dataMap,nColIndex,pRange->fValFrom,pRange->fTotalWeight,fFreTotal,pTable,2);
		//�ܵ�
		InsertTable(dataMap,nColIndex,pRange->fValFrom,pRange->fTotalWeight,fFreTotal,pTable,3);
	}
	else
	{
		for (i=0;i<nSize;i++)
		{
			pFld = (CFldInfo *)pArrFld->GetAt(i);
			fFreTotal += pFld->fWeight;
			//����Ƶ������
			InsertTable(dataMap,nColIndex,pFld->fValue, pFld->fWeight,fFreTotal,pTable,0);
		}
		//�Ƿ����ȱʧֵ
		InsertTable(dataMap,nColIndex,pFld->fValue,pFld->fWeight,fFreTotal,pTable,2);
		//�ܵ�
		InsertTable(dataMap,nColIndex,pFld->fValue,pFld->fWeight,fFreTotal,pTable,3);
	}
	//Add
	CRsltVariable *pVariable = new CRsltElementTable(szTemp,pTable);
	m_pResult->Add(pVariable);
}

void CFrequency::CalcOther(CPtrArray *pArrFld, int nColIndex)
{
	int i=0;
	BOOL bMid = FALSE;
	double fFre = 0, fTotal = 0;
	CFldInfo *pFld = NULL;
	
	//�Ƿ�Ϊ�ַ���
	if (m_pDataInterface->GetFieldType(nColIndex) == fString) return;
	//��ֵ
	int nSize = pArrFld->GetSize();
	if (nSize > 0)
	{
		pFld = (CFldInfo *)pArrFld->GetAt(0);
		m_tOut.m_fMin = pFld->fValue;
		pFld = (CFldInfo *)pArrFld->GetAt(nSize-1);
		m_tOut.m_fMax = pFld->fValue;
	}
	for (i=0;i<nSize;i++)
	{
		pFld = (CFldInfo *)pArrFld->GetAt(i);
		//��λ��
		if (!bMid)
		{
			fTotal += pFld->fWeight;
			if (fTotal >= m_tOut.m_fSum/2)
			{
				bMid = TRUE;
				m_tOut.m_fMid = pFld->fValue;
			}
		}
		//����
		if (pFld->fWeight > fFre)
		{
			fFre = pFld->fWeight;
			m_tOut.m_fMany = pFld->fValue;
		}
	}
}

void CFrequency::CalcOther(CDoubleVector &v, int nColIndex)
{
	int i=0;
	BOOL bMid = FALSE;
	double fFre = 0, fTotal = 0;
	CFldInfo *pFld = NULL;
	
	//�Ƿ�Ϊ�ַ���
	if (m_pDataInterface->GetFieldType(nColIndex) == fString) return;
	//��ֵ
	int nSize = v.vlen();
	if (nSize > 0)
	{
		m_tOut.m_fMin = v(0);
		m_tOut.m_fMax = v(nSize-1);
	}
}

void CFrequency::DispFrequency()
{
	CTString szTemp("");
	int nRow = 0, nCol = 0, nRowCnt = 0, nColIndex = 0;

	CDWordArray arrCol;
	CDoubleMatrix dataMatrix;
	CDoubleMatrix weightMatrix;
	CMapManager dataMap;
	CPtrArray   arrFld;
	
	//�ܵļ�¼��
	int nAllCount = m_pDataInterface->m_DataAccess.GetRecordCount();
	//��ȡȨ��
	if (m_bWeight)
	{
		arrCol.Add(m_nWeight);
		nRowCnt = m_pDataInterface->GetColsData(arrCol,weightMatrix,0);
		if (nRowCnt < 1)
		{
			CTString szWarning("");
			szWarning.Format("ע�� ����ѡ���Ȩ�ز��ʺϽ���Ƶ�ʷ�����");
			CRsltElementText * pWarningTextRslt = new CRsltElementText( "ע��" );
			pWarningTextRslt->AddString( szWarning );
			m_pResult->Add( pWarningTextRslt );
			return;
		}
		//�ܵ�Ȩ��
		m_n64Weight = __int64(weightMatrix(0).sum());
		weightMatrix.destruction();
	}
	else
	{
		m_n64Weight = nAllCount;
	}
	//�����
	CTLTable *pTable = new CTLTable;
	pTable->CreateTable(2, 2);//Create
	pTable->SetTitle(_T("ͳ����"));
	pTable->SetCols(m_tStatMethod.iCount+2);
	pTable->InsertColumn(0,"��������");
	//�Ƿ���Ȩ��
	if (m_bWeight)
	{
		pTable->InsertColumn(1,_T("Ȩ�غ�"));
	}
	else
	{
		pTable->InsertColumn(1,_T("��¼��"));
	}
	//ͳ�Ʒ�������
	for (int iIndex=0; iIndex<m_tStatMethod.iCount; iIndex++)
	{
		int id = m_tStatMethod.pValue[iIndex];
		pTable->InsertColumn(iIndex+2,m_tStatMethodS[id].name);
	}
	//����һ���������Ķ���
	CRsltVariable *pETable = new CRsltElementTable(_T("ͳ����"),pTable);
	m_pResult->Add(pETable);
	//����
	for (int i=0; i<m_tFrequency.iCount; i++)
	{
		////////////////////////////////////////
		arrCol.RemoveAll();
		dataMap.Clear();
		dataMatrix.destruction();
		if (m_bWeight)
		{
			arrCol.Add(m_nWeight);
		}
		nColIndex = m_tFrequency.pValue[i];//ȡ���ֶε��к�
		arrCol.Add(nColIndex);
		//����ȱʡֵɾ��
		nRowCnt = m_pDataInterface->GetColsData(arrCol,dataMatrix,dataMap);
		if (nRowCnt < 1)
		{
			CreateFreqTable(dataMatrix,dataMap,nColIndex,&arrFld,1);
			continue;
		}
		////////////////////////////////////////
		nCol = m_bWeight ? 1 : 0;
		Release(&arrFld);//�ͷ��ڴ�
		m_tOut.Init();   //������ʼ��
		//������ֵ
		if (m_pDataInterface->GetFieldType(nColIndex) != fString){
			CDoubleVector v1(dataMatrix(nCol));
			CDoubleVector v2(dataMatrix(0));
			CalcParam(v1,v2,FALSE);//��������
		}
		//�������仮��	
		if (IsExistRange(nColIndex))
		{
			CIntVector vIndex;
			CDoubleVector v(dataMatrix(nCol));
			v.Sort(vIndex);//����
			//��������
			CreatePtrArray(arrFld,nColIndex);
			CDoubleVector v1(dataMatrix(nCol));
			CDoubleVector v2(dataMatrix(0));
			if (!CalcFrequency(v1,v2,
				vIndex,nColIndex,arrFld,DISP_MAX/2))//����Ƶ�ʷֲ�
			{
				CreateFreqTable(dataMatrix,dataMap,nColIndex,&arrFld,2);
				continue;
			}
			CDoubleVector v3(dataMatrix(nCol));
			CalcOther(v3,nColIndex);//��ֵ
			//�Ƿ��ַ���-�����Ƶ�ʱ�
			CreateFreqTable(dataMatrix,dataMap,nColIndex,&arrFld);
			vIndex.destroy();
		}
		else
		{
			CDoubleVector v1(dataMatrix(nCol));
			CDoubleVector v2(dataMatrix(0));
			
			if (!CalcFrequency(v1,v2,
				nColIndex,arrFld,DISP_MAX/2,TRUE))//����Ƶ�ʷֲ�
			{
				CreateFreqTable(dataMatrix,dataMap,nColIndex,&arrFld,2);
				continue;
			}
			CalcOther(&arrFld,nColIndex);//��ֵ����λ��������
			//�Ƿ��ַ���-�����Ƶ�ʱ�
			CreateFreqTable(dataMatrix,dataMap,nColIndex,&arrFld);
		}
		if (m_pDataInterface->GetFieldType(nColIndex) == fString) continue;
		//���
		nCol = 1;
		//����һ����¼
		pTable->InsertItem(nRow, m_pDataInterface->GetFieldName(nColIndex));
		//Ȩ�غͻ��¼��
		szTemp.Format("%.0f",m_tOut.m_fSum);
		pTable->SetItemText(nRow, nCol, szTemp);
		for (int j=0;j<m_tStatMethod.iCount;j++)
		{
			int nID = m_tStatMethod.pValue[j];
			switch (nID)
			{
			case 0://����
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMax-m_tOut.m_fMin);
				break;
			case 1://��Сֵ
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMin);
				break;
			case 2://���ֵ
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMax);
				break;
			case 3://��λ��
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMid);
				break;
			case 4://�ܺ�
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fSmy);
				break;
			case 5://ƽ��ֵ
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMean);
				break;
			case 6://��ֵ��׼��
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMe);
				break;
			case 7://��׼��
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fS);
				break;
			case 8://����
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fS2);
				break;
			case 9://ƫ
				if (m_bOutPartial)
					pTable->SetItemText(nRow, ++nCol, m_tOut.m_fPartial);
				else
					pTable->SetItemText(nRow, ++nCol, "");
				break;
			case 10://ƫ��
				if (m_bOutPartialE)
					pTable->SetItemText(nRow, ++nCol, m_tOut.m_fPartialE);
				else
					pTable->SetItemText(nRow, ++nCol, "");
				break;
			case 11://��
				if (m_bOutApex)
					pTable->SetItemText(nRow, ++nCol, m_tOut.m_fApex);
				else
					pTable->SetItemText(nRow, ++nCol, "");
				break;
			case 12://����
				if (m_bOutApexE)
					pTable->SetItemText(nRow, ++nCol, m_tOut.m_fApexE);
				else
					pTable->SetItemText(nRow, ++nCol, "");
				break;
			case 13://����
				pTable->SetItemText(nRow, ++nCol, m_tOut.m_fMany);
				break;
			default:
				break;
			}
		}
		nRow ++;
	}
}

//Ƶ�ʷ�������ں���(2),�޴��嵯��,ͨ���ַ�
CResult* CFrequency::Frequency(CDataInterface* pDataInterface,CTString strScript)
{    
	//��ʼ��
	m_pResult        = NULL;
	m_pDataInterface = pDataInterface;
	CScript sct;
	sct.ImportStr(strScript);//�����ű�
	//add end
	if (CAnalyze::GetListData(&sct,"������",pDataInterface,m_tFrequency))
	{
		//1ͳ��������************************************
		//*****************1.1��������*************************
		CTString strParam = "��������";
		int nIndex  = sct.FindParamName(strParam);
		CDWordArray selArray;
		if (nIndex!=-1)
		{//���ָ��,�򲻰�Ĭ�ϴ���
			nIndex = sct.GetItemCount(strParam);
			for (int i=0;i<nIndex; i++)
			{
				CTString strItem;
				sct.GetItemData(strParam,i,strItem);
				if (strItem == "��ֵ")//IDC_CHECK_MEAN
					selArray.Add(5);
				else if (strItem == "��ֵ")//IDC_CHECK_MEDIAN
					selArray.Add(3);
				else if (strItem == "����")//IDC_CHECK_MODE
					selArray.Add(13);
				else if (strItem == "�ܺ�")//IDC_CHECK_SUM
					selArray.Add(4);
			}
		}
		//****************1.2�ֲ�**********************
		strParam = "�ֲ�";
		nIndex  = sct.FindParamName(strParam);
		if (nIndex!=-1)
		{//���ָ��,�򲻰�Ĭ�ϴ���
			nIndex = sct.GetItemCount(strParam);
			for (int i=0;i<nIndex; i++)
			{
				CTString strItem;
				sct.GetItemData(strParam,i,strItem);
				if (strItem == "ƫ̬��")//IDC_CHECK_KURTOSIS
				{
					selArray.Add(11);
				selArray.Add(12);
				}
				else if (strItem == "��̬��")//IDC_CHECK_SKEWNESS
				{
					selArray.Add(9);
					selArray.Add(10);
				}	
			}
		}
		//****************1.3��������**********************
		strParam = "��������";
		nIndex  = sct.FindParamName(strParam);
		if (nIndex!=-1)
		{//���ָ��,�򲻰�Ĭ�ϴ���
			nIndex = sct.GetItemCount(strParam);
			for (int i=0;i<nIndex; i++)
			{
				CTString strItem;
				sct.GetItemData(strParam,i,strItem);
				if (strItem == "��׼��")//IDC_CHECK_STD
					selArray.Add(7);
				else if (strItem == "����")//IDC_CHECK_VAR
					selArray.Add(8);
				else if (strItem == "����")//IDC_CHECK_RG
					selArray.Add(0);
				else if (strItem == "��Сֵ")//IDC_CHECK_MIN
					selArray.Add(1);
				else if (strItem == "���ֵ")//IDC_CHECK_MAX
					selArray.Add(2);
				else if (strItem == "��ֵ��׼��")//IDC_CHECK_SE
					selArray.Add(6);
			}
		}
		//***************��Aarray�����m_tStatMethod************************
		if (selArray.GetSize())
		{
			m_tStatMethod.DeleteObject();
			
			int *pValue=new int[selArray.GetSize()];
			for (int i=0;i<selArray.GetSize();i++)
			{
				*(pValue+i) = selArray.GetAt(i);
			}
			m_tStatMethod=TIntStruct(selArray.GetSize(),pValue);
		}
		DispFrequency();
		return m_pResult;
	}
	return NULL;
}
