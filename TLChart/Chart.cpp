// Chart.cpp: implementation of the CTChart class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Chart.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChart::CChart()
{
	m_nAVType = ERR_INT;
	m_bWeight = false;   //Ȩ��
	m_bSplit  = false;   //��״ͼ�Ƿ񵥶�����
	m_bShowLabel = false;//�Ƿ��ñ�ǩ�������ʾ
	m_pResult = new CResult;
}

CChart::~CChart()
{
	m_arrX.RemoveAll();
	m_arrY.RemoveAll();
	m_arrD.RemoveAll();
	if (!m_tVarX.IsEmpty()) m_tVarX.DeleteObject();
	if (!m_tVarY.IsEmpty()) m_tVarY.DeleteObject();
	if (!m_tVarD.IsEmpty()) m_tVarD.DeleteObject();
}
/*
CResult *CChart::OnChart(CDataInterface *pDataInterface, string VO)
{
	m_pDataInterface = pDataInterface;
	
	Init();//��ʼ��
	return m_pResult;
}
*/
void CChart::Init()
{
	if (m_pDataInterface == NULL) return;
	//�Ƿ���ʾ��ǩ
	m_pDataInterface->SetShowValueLabel(m_bShowLabel);
	//����Ȩ��
	if (m_bWeight) m_pDataInterface->SetWeightNumb(m_nWeight); 
}

CTString CChart::GetLastErrMsg()
{
	return m_szErrMsg;
}

void CChart::SetChartType(const char *pszChartType)
{
	//����ͼ������
	if (_stricmp(pszChartType,CHART_LINE) == 0)           
		m_szChartName = "ֱ��ͼ";
	else if (_stricmp(pszChartType,CHART_BAR) == 0)       
		m_szChartName = "��״ͼ";
	else if (_stricmp(pszChartType,CHART_PIE) == 0)       
		m_szChartName = "Բ��ͼ";
	else if (_stricmp(pszChartType,CHART_AREA) == 0)      
		m_szChartName = "���ͼ";
	else if (_stricmp(pszChartType,CHART_BOXPLOT) == 0)   
		m_szChartName = "��״ͼ";
	else if (_stricmp(pszChartType,CHART_HISTOGRAM) == 0) 
		m_szChartName = "ֱ��ͼ";
	else if (_stricmp(pszChartType,CHART_SCATTER) == 0)   
		m_szChartName = "ɢ��ͼ";
	else if (_stricmp(pszChartType,CHART_PARETO) == 0)    
		m_szChartName = "����ͼ";
	else if (_stricmp(pszChartType,CHART_ERRORBAR) == 0)  
		m_szChartName = "���ͼ";
	else if (_stricmp(pszChartType,CHART_SEQUENCE) == 0)  
		m_szChartName = "����ͼ";
	else if (_stricmp(pszChartType,CHART_CONTROL) == 0)   
		m_szChartName = "����ͼ";
	else if (_stricmp(pszChartType,CHART_ROC) == 0)       
		m_szChartName = "ROC����ͼ";
	else if (_stricmp(pszChartType,CHART_HIGHLOW) == 0)   
		m_szChartName = "�ߵ�ͼ";
	else if (_stricmp(pszChartType,CHART_PP) == 0)        
		m_szChartName = "PPͼ";
	else if (_stricmp(pszChartType,CHART_QQ) == 0)        
		m_szChartName = "QQͼ";
	else if (_stricmp(pszChartType,CHART_SELFCORRELATIONS) == 0)   
		m_szChartName = "�����ͼ";
	else if (_stricmp(pszChartType,CHART_CROSSCORRELATIONS) == 0) 
		m_szChartName = "�����ͼ";
	else if (_stricmp(pszChartType,CHART_EMP) == 0)       
		m_szChartName = "�����ۻ�ͼ�;����ܶ�ͼ";
	else                                       
		m_szChartName = "δ֪ͼ";
	m_szChartType = pszChartType;
}

//�����
bool CChart::ParseAxis(string VO)
{
	int nSize=0;
	bool bAxis = false;
	CTString szTemp("");
	CTStringArray szArrName;

	AnaWord VOstr;
	VOstr.Import(VO);
	szArrName.RemoveAll();
	//AXISDESC
	if (AnaSyntax::exist(AXISDESC_STR,VOstr)) 
	{
		AnaSyntax::getVAR(AXISDESC_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s������������û�б�ָ�������飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);//AXISDESC REC
		szTemp.MakeUpper();
		if (szTemp == REC_STR)
		{
			m_arrX.Add(REC);
		}
		else if (szTemp == RECSCALE_STR)
		{
			m_arrX.Add(RECSCALE);
		}
		else if (szTemp == RECTOTAL_STR)
		{
			m_arrX.Add(RECTOTAL);
		}
		else if (szTemp == RECTOTALSCALE_STR)
		{
			m_arrX.Add(RECTOTALSCALE);
		}
		else
		{
			m_szErrMsg.Format("%s����������������ʶ��%s���޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
		bAxis = true;
		m_nAVType = AXIS_DESC_INT;
	}
	szArrName.RemoveAll();
	//AXISNDESC
	if (AnaSyntax::exist(AXISNDESC_STR,VOstr))
	{
		if (bAxis)
		{
			m_szErrMsg.Format("%s��X������Ѿ���ָ��Ϊ�Ǳ������ͣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		AnaSyntax::getVAR(AXISNDESC_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�����������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (!m_pDataInterface->getVarID(szArrName,m_tVarX))//AXISNDESC ���
		{
			m_szErrMsg.Format("%s��X��������������ڣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		m_nAVType = AXIS_NDESC_INT;
	}
	return true;
}

//��������
bool CChart::ParseVar(string VO)
{
	int nSize=0;
	bool bVar = false;
	CTString szTemp("");
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	szArrName.RemoveAll();
	//VARDESC
	if (AnaSyntax::exist(VARDESC_STR,VOstr)) 
	{
		AnaSyntax::getVAR(VARDESC_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�ı�����������û�б�ָ�������飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		szTemp = szArrName.GetAt(0);//VARDESC REC
		szTemp.MakeUpper();
		if (szTemp == REC_STR)
		{
			m_arrY.Add(REC);
		}
		else if (szTemp == RECSCALE_STR)
		{
			m_arrY.Add(RECSCALE);
		}
		else if (szTemp == RECTOTAL_STR)
		{
			m_arrY.Add(RECTOTAL);
		}
		else if (szTemp == RECTOTALSCALE_STR)
		{
			m_arrY.Add(RECTOTALSCALE);
		}
		else
		{
			m_szErrMsg.Format("%s�ı���������������ʶ��%s���޷���ͼ�����飡Line=%d,File=%s",szTemp.GetData(),__LINE__,__FILE__);
			return false;
		}
		bVar = true;
		m_nAVType = VAR_DESC_INT;
	}
	//VARNDESC
	szArrName.RemoveAll();
	if (AnaSyntax::exist(VARNDESC_STR,VOstr)) 
	{
		if (bVar)
		{
			m_szErrMsg.Format("%s�ı��������Ѿ���ָ��Ϊ�Ǳ������ͣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		AnaSyntax::getVAR(VARNDESC_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�ı�������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (!m_pDataInterface->getVarID(szArrName,m_tVarY))//VARNDESC ���
		{
			m_szErrMsg.Format("%s�ı������������ڣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		m_nAVType = VAR_NDESC_INT;
	}
	return true;
}

//�Զ�����
bool CChart::ParseDefLine(string VO)
{
	int nSize=0;
	bool bVar = false;
	CTString szTemp("");
	CTStringArray szArrName;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	szArrName.RemoveAll();
	if (AnaSyntax::exist(DEFLINE_STR,VOstr))
	{
		AnaSyntax::getVAR(DEFLINE_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�е��Զ����б�������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (!m_pDataInterface->getVarID(szArrName,m_tVarD))
		{
			m_szErrMsg.Format("%s���Զ����б������������ڣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
	}
	return true;
}

//Ȩ��
bool CChart::ParseWeight(string VO)
{
	int nSize=0;
	CTString szTemp("");
	CTStringArray szArrName;
	TIntStruct tVarTemp;
	
	AnaWord VOstr;
	VOstr.Import(VO);
	//������Ȩ��
	m_bWeight = false;
	if (AnaSyntax::exist(WEIGHT_STR, VOstr))
	{
		AnaSyntax::getVAR(WEIGHT_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize != 1)
		{
			if (nSize == 0)
				m_szErrMsg.Format("%s�е�Ȩ�ر�������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			else 
				m_szErrMsg.Format("%s�е�Ȩ�ر����������ù��࣬�޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		if (!m_pDataInterface->getVarID(szArrName,tVarTemp))
		{
			m_szErrMsg.Format("%s�е�Ȩ�ر������������ڣ��޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		m_nWeight = tVarTemp.pValue[0];
		tVarTemp.DeleteObject();
		m_bWeight = true;
	}
	return true;
}

//������������ʽ
bool CChart::ParseDataType(string VO)
{
	int i=0, nSize=0;
	CTString szTemp("");
	CTStringArray szArrName;

	AnaWord VOstr;
	VOstr.Import(VO);
	
	//����
	if (AnaSyntax::exist(DTA_STR,VOstr))
	{
		AnaSyntax::getVAR(DTA_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�е�����������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<nSize; i++)
		{
			szTemp = szArrName.GetAt(i);
			szTemp.MakeUpper();
			if (szTemp == MEAN_STR)
			{
				m_arrX.Add(MEAN);
			}
			else if (szTemp == VAR_STR)
			{
				m_arrX.Add(VAR);
			}
			else if (szTemp == MOD_STR)
			{
				m_arrX.Add(MOD);
			}
			else if (szTemp == MID_STR)
			{
				m_arrX.Add(MID);
			}
			else if (szTemp == STD_STR)
			{
				m_arrX.Add(STD);
			}
			else if (szTemp == MIN_STR)
			{
				m_arrX.Add(MIN);
			}
			else if (szTemp == MAX_STR)
			{
				m_arrX.Add(MAX);
			}
			else if (szTemp == SUM_STR)
			{
				m_arrX.Add(SUM);
			}
			else
			{
				m_szErrMsg.Format("%s�е���������Ͳ���ʶ�� ��%s���޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),szTemp.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	// /DTV
	if (AnaSyntax::exist(DTV_STR,VOstr))
	{
		AnaSyntax::getVAR(DTV_STR, VOstr, szArrName);
		nSize = szArrName.GetSize();
		if (nSize < 1)
		{
			m_szErrMsg.Format("%s�еı�����������û�б�ָ�����޷���ͼ�����飡Line=%d,File=%s",m_szChartName.GetData(),__LINE__,__FILE__);
			return false;
		}
		for (i=0; i<nSize; i++)
		{
			szTemp = szArrName.GetAt(i);
			szTemp.MakeUpper();
			if (szTemp == MEAN_STR)
			{
				m_arrY.Add(MEAN);
			}
			else if (szTemp == VAR_STR)
			{
				m_arrY.Add(VAR);
			}
			else if (szTemp == MOD_STR)
			{
				m_arrY.Add(MOD);
			}
			else if (szTemp == MID_STR)
			{
				m_arrY.Add(MID);
			}
			else if (szTemp == STD_STR)
			{
				m_arrY.Add(STD);
			}
			else if (szTemp == MIN_STR)
			{
				m_arrY.Add(MIN);
			}
			else if (szTemp == MAX_STR)
			{
				m_arrY.Add(MAX);
			}
			else if (szTemp == SUM_STR)
			{
				m_arrY.Add(SUM);
			}
			else
			{
				m_szErrMsg.Format("%s�еı�����������ʶ�� ��%s�����飡Line=%d,File=%s",m_szChartName.GetData(),szTemp.GetData(),__LINE__,__FILE__);
				return false;
			}
		}
	}
	return true;
}

//�������
bool CChart::ParseOutput(string VO)
{
	CTString szTemp;
	CTStringArray szArrName;

	AnaWord VOstr;
	VOstr.Import(VO);
	//��ǩ
	if (AnaSyntax::exist(OUTPUT_STR, VOstr))
	{
		AnaSyntax::getVAR(OUTPUT_STR, VOstr, szArrName);
		int nSize = szArrName.GetSize();
		for (int i=0; i<nSize; i++)
		{
			szTemp = szArrName.GetAt(i);
			szTemp.MakeUpper();
			if (szTemp == SPLIT_STR)
			{
				m_bSplit = true; 
				continue;
			}
			if (szTemp != LABEL_STR) continue;
			m_bShowLabel = true;
			break;
		}
	}
	return true;
}

bool CChart::IsSameCallFunc(int nType)
{
	int i = 0, nSize = 0;
	EData eHead, eTemp;

	if (nType == XAXIS_INT)
	{
		nSize = m_arrX.GetSize();
		if (nSize < 2)
		{
			return true;
		}
		else
		{
			eHead = m_arrX.GetAt(0);
			for (i=1;i<nSize;i++)
			{
				eTemp = m_arrX.GetAt(i);
				if (eHead != eTemp) return false;
			}
		}
	}
	else if (nType == YAXIS_INT)
	{
		nSize = m_arrY.GetSize();
		if (nSize < 2)
		{
			return true;
		}
		else
		{
			eHead = m_arrY.GetAt(0);
			for (i=1;i<nSize;i++)
			{
				eTemp = m_arrY.GetAt(i);
				if (eHead != eTemp) return false;
			}
		}
	}
	else
	{
		nSize = m_arrD.GetSize();
		if (nSize < 2)
		{
			return true;
		}
		else
		{
			eHead = m_arrD.GetAt(0);
			for (i=1;i<nSize;i++)
			{
				eTemp = m_arrD.GetAt(i);
				if (eHead != eTemp) return false;
			}
		}
	}
	return true;
}

//Type:0-X,1-Y,2-D;
bool CChart::GetDataType(int nIndex, int nType, EData &eData)
{
	int nSize;

	if (nType == XAXIS_INT)//X��
	{
		nSize = m_arrX.GetSize();
		assert(nSize>0 && nIndex>-1);
		if (nSize < 1)
		{
			m_szErrMsg.Format("ͼ����X���������Ͳ����ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (nIndex >= nSize) nIndex = nSize-1;//���������ڳ���,ȡ���һ��.
		eData = m_arrX.GetAt(nIndex);
	}
	else if (nType == YAXIS_INT) //Y��
	{
		nSize = m_arrY.GetSize();
		assert(nSize>0 && nIndex>-1);
		if (nSize < 1)
		{
			m_szErrMsg.Format("ͼ���б����������Ͳ����ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (nIndex >= nSize) nIndex = nSize-1;//���������ڳ���,ȡ���һ��.
		eData = m_arrY.GetAt(nIndex);
	}
	else
	{
		nSize = m_arrD.GetSize();
		assert(nSize>0 && nIndex>-1);
		if (nSize < 1)
		{
			m_szErrMsg.Format("ͼ�����Զ�������������Ͳ����ڣ��޷���ͼ�����飡Line=%d,File=%s",__LINE__,__FILE__);
			return false;
		}
		if (nIndex >= nSize) nIndex = nSize-1;//���������ڳ���,ȡ���һ��.
		eData = m_arrD.GetAt(nIndex);
	}
	return true;
}

bool CChart::GetStatistical(CStatistics statistics, EData eData, int nRowCnt, double &fValue)
{
	switch(eData)
	{
	case MEAN://ƽ��ֵ
		fValue=statistics.f_mean(); 
		break;
	case STD:
		fValue=statistics.f_stadeviation();
		break;
	case MID:
		fValue=statistics.f_medium();
		break;
	case VAR:
		fValue=statistics.f_variance();
		break;
	case MOD:
		fValue=statistics.f_mode(); 
		break;
	case MIN:
		fValue=statistics.f_min(); 
		break;
	case RECORD:
		fValue=nRowCnt;
		break;
	case MAX:
		fValue=statistics.f_max() ;
		break;
	case SUM:
		fValue=statistics.f_sum(); 
		break;
	default:
		m_szErrMsg.Format("δ֪���������ͣ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

bool CChart::GetAxisLabel(EData eData, CTString &szLabel)
{
	switch(eData)
	{
	case REC:
		szLabel="����";
		break;
	case RECSCALE:
		szLabel="�ٷֱ�";
		break;
	case RECTOTAL:
		szLabel="�ۼ�����";
		break;
	case RECTOTALSCALE:
		szLabel="�ۼưٷֱ�";
		break;
	case MEAN:
		szLabel = "ƽ��ֵ";
		break;
	case STD:
		szLabel = "��׼��";
		break;
	case MID:
		szLabel = "��λ��";
		break;
	case VAR:
		szLabel = "����";
		break;
	case MOD:
		szLabel = "��λ��";
		break;
	case MIN:
		szLabel = "��Сֵ";
		break;
	case RECORD:
		szLabel = "��¼��"; //��һ����������Ӧ��ֵ
		break;
	case MAX:
		szLabel = "���ֵ";
		break;
	case SUM:
		szLabel = "�ܺ�";
		break;
	default:
		m_szErrMsg.Format("δ֪���������ͣ����飡Line=%d,File=%s",__LINE__,__FILE__);
		return false;
	}
	return true;
}

//�õ���ǩ
CTString CChart::GetLabel(CMapManager &dataMap, int nPos, int nColIndex,double fValue)
{
	bool bShowLabel = false;
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
	else if (fieldtype == fDouble)
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
	else if (fieldtype == fInt)
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
	else if (fieldtype == fBoolean)
	{
		if (bShowLabel)
		{
			if (!pField->GetValueVL(bool(int(fValue)),szLabel))
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

///////////////////////////////////////////////////////////////////////////////////

//Function name :  QuicksortRecursive
//Parameter		   pArray :ָ�������ָ��
//Parameter		   d :���������
//Parameter		   h :���������
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Parameter		   bAxis :falseʱ��Y����������
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������ĵݹ��������
//Describe			:������QuickSort����
////////////////////////////////////////////////////////////////////////
void CChart::QuickSortRecursive(TDataPointValVal *pArray,int d,int h,bool bAsceding,bool bAxis)
{
	int i,j;
	TDataPointValVal pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	do{
		if(bAsceding)
		{
			if(bAxis)
			{
				while(pArray[j].fXVal < pt.fXVal) j++;
				while(pArray[i].fXVal > pt.fXVal) i--;
			}
			else
			{
				while(pArray[j].fYVal < pt.fYVal) j++;
				while(pArray[i].fYVal > pt.fYVal) i--;
			}
		}
		else
		{
			if(bAxis)
			{
				while(pArray[j].fXVal > pt.fXVal)
					j++;
				while(pArray[i].fXVal < pt.fXVal)
					i--;
			}
			else
			{
				while(pArray[j].fYVal > pt.fYVal)
					j++;
				while(pArray[i].fYVal < pt.fYVal)
					i--;
			}
		}
		if(i>=j)
		{
			if(i != j)
			{
				TDataPointValVal tempPt;
				tempPt = pArray[i];
				pArray[i] = pArray[j];
				pArray[j] = tempPt;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(d < i)QuickSortRecursive(pArray,d,i,bAsceding,bAxis);
	if(j < h)QuickSortRecursive(pArray,j,h,bAsceding,bAxis);
}

//Function name :  QuickSort
//Parameter		   pArray :ָ��TDataPointValVal��ָ��
//Parameter		   nSize :���������
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Parameter		   bAxis :falseʱ��Y����������
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������������
////////////////////////////////////////////////////////////////////////
bool CChart::QuickSort(TDataPointValVal* pArray,int nSize,bool bAsceding,bool bAxis)
{
	bool bFlag = true;
	if (nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			QuickSortRecursive(pArray,low,high,bAsceding,bAxis);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ�������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
	return bFlag;
}


//Function name :  PointArrayRec
//Parameter		   pArray :ָ�������ָ��
//Parameter		   d :�����е�����
//Parameter		   h :�����е�����
//Parameter		   nCul :�����е�����
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������ĵݹ��������
//Describe			:������PointArraySort����
////////////////////////////////////////////////////////////////////////
void CChart::PointArrayRec(TDataPointValVal **pArray,int d,int h,int nCul,bool bAsceding)
{
	int i,j;
	TDataPointValVal* pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	do
	{
		if(bAsceding)
		{
			int k;
			for(k = 0;k<nCul;k++) //�Ƚ������е�j��k�е�fYval��ֵ(���pArray[j][k]<pArray[j+1][k])
			{							//�� pArray[j][k-1] < pArray[j-1][k-1]	(modified by fwd in 09-05)
				while(pArray[j][k].fYVal < pt[k].fYVal) 
				{	j++;	k=nCul;}
			}
			for(k = 0;k<nCul;k++)
			{
				while(pArray[i][k].fYVal > pt[k].fYVal)
				{	i--;	k=nCul;}
			}
		}
		else
		{	int k;
			for(k = 0;k<nCul;k++)
			{
				while(pArray[j][k].fYVal > pt[k].fYVal)
				{	j++;	k = nCul;}
			}
			for(k =0;k<nCul;k++)
			{
				while(pArray[i][k].fYVal < pt[k].fYVal)
				{	i--;	k= nCul;}
			}
		}
		if(i>=j)
		{
			if(i != j)
			{
				for(int nId=0; nId<nCul; nId++)
				{
					//swap(pArray[i][nId],pArray[j][nId]);
					//���������������
					TDataPointValVal ntemp = pArray[i][nId];
					pArray[i][nId] = pArray[j][nId];
					pArray[j][nId] = ntemp;
				}
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(d < i)PointArrayRec(pArray,d,i,nCul,bAsceding);
	if(j < h)PointArrayRec(pArray,j,h,nCul,bAsceding);
}

//Function name :  PointArraySort
//Parameter		   pArray :ָ�������ָ��
//Parameter		   nSize :�����е�����
//Parameter		   nCul :�����е�����
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������ĵݹ��������
//Describe			:������2ά�����ֵ����
////////////////////////////////////////////////////////////////////////
bool CChart::PointArraySort(TDataPointValVal** pArray,int nSize,int nCul,bool bAsceding)
{
	bool bFlag = true;
	if(nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			PointArrayRec(pArray,low,high,nCul,bAsceding);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ�������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
		return bFlag;
}

//��double�����������
//Function name :  QuickSort
//Parameter		   pArray[in] :double��ָ��
//Parameter		   nSize :���������
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������������
////////////////////////////////////////////////////////////////////////
bool CChart::QuickSort(double* pArray,int nSize,bool bAsceding)
{
	bool bFlag = true;
	if(nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			QuickSortRecursive(pArray,low,high,bAsceding);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ�������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
	return bFlag;
}

//�ݹ麯��
////////////////////////////////////
//Function name :  QuickSortRecursive
//Parameter		   pArray[in] :double��ָ��
//Parameter		   d :�����е�����
//Parameter		   h :�����е�����
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������ĵݹ��������
//Describe			:������QuickSort����
////////////////////////////////////////////////////////////////////////

void CChart::QuickSortRecursive(double *pArray,int d,int h,bool bAsceding)
{
	int i,j;
	double pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	do{
		if(bAsceding)
		{
			while(pArray[j] < pt) j++;
			while(pArray[i] > pt) i--;
		}
		else
		{
			while(pArray[j] > pt) j++;
			while(pArray[i] < pt) i--;
		}
		if(i>=j)
		{
			if(i != j)
			{
				double dTemp;
				dTemp = pArray[i];
				pArray[i] = pArray[j];
				pArray[j] = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(d < i)QuickSortRecursive(pArray,d,i,bAsceding);
	if(j < h)QuickSortRecursive(pArray,j,h,bAsceding);

}

//��StrVal������������
/////////////////////////////////////
//Function name :  QuickSort
//Parameter		   pArray :ָ��TDataPointStrVal��ָ��
//Parameter		   nSize :���������
//Parameter		   bAsceding :TURE ʱΪ����falseʱΪ����
//Parameter		   bAxis :falseʱ��Y����������
//Author			   :   fwd
//Style 				:	static
//Function			 :	 �������������
////////////////////////////////////////////////////////////////////////
bool CChart::QuickSort(TDataPointStrVal* pArray,int nSize,bool bAsceding,bool bAxis)
{
	bool bFlag = true;
	if(nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			QuickSortRecursive(pArray,low,high,bAsceding,bAxis);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ�������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
	return bFlag;
}

void CChart::QuickSortRecursive(TDataPointStrVal *pArray,int d,int h,bool bAsceding,bool bAxis)
{
	int i,j;
	TDataPointStrVal pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	//////////////////////////////////////
	//fXVal is string type
	do{
		if(bAsceding)
		{
			if(bAxis)
			{
				while(strcmp(pArray[j].fXVal, pt.fXVal)<0) j++;	 // pArray[j].fXVal <pt.fXVal
				while(strcmp(pArray[i].fXVal, pt.fXVal)>0) i--;
			}
			else
			{
				while(pArray[j].fYVal < pt.fYVal) j++;
				while(pArray[i].fYVal > pt.fYVal) i--;
			}
		}
		else
		{
			if(bAxis)
			{
				while(strcmp(pArray[j].fXVal , pt.fXVal)>0) //pArray[j].fXval > pt.fXVal
					j++;
				while(strcmp(pArray[i].fXVal , pt.fXVal)<0)
					i--;
			}
			else
			{
				while(pArray[j].fYVal > pt.fYVal)
					j++;
				while(pArray[i].fYVal < pt.fYVal)
					i--;
			}
		}
		if(i>=j)
		{
			if(i != j)
			{
				TDataPointStrVal tempPt;
				tempPt = pArray[i];
				pArray[i] = pArray[j];
				pArray[j] = tempPt;
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(d < i)QuickSortRecursive(pArray,d,i,bAsceding,bAxis);
	if(j < h)QuickSortRecursive(pArray,j,h,bAsceding,bAxis);
}

bool CChart::PointArraySort(TDataPointStrVal** pArray,int nSize,int nCul,bool bAsceding)
{
	bool bFlag = true;
	if(nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			PointArrayRec(pArray,low,high,nCul,bAsceding);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ�������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
		return bFlag;
}

void CChart::PointArrayRec(TDataPointStrVal **pArray,int d,int h,int nCul,bool bAsceding)
{
	int i,j;
	TDataPointStrVal* pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	do{
		if(bAsceding)
		{	int k;
			for(k = 0;k<nCul;k++) //�Ƚ������е�j��k�е�fYval��ֵ(���pArray[j][k]<pArray[j+1][k])
			{							//�� pArray[j][k-1] < pArray[j-1][k-1]	(modified by fwd in 09-05)
				while(pArray[j][k].fYVal < pt[k].fYVal) 
				{	j++;	k=nCul;}
			}
			for(k = 0;k<nCul;k++)
			{
				while(pArray[i][k].fYVal > pt[k].fYVal)
				{	i--;	k=nCul;}
			}
		}
		else
		{
			int k;
			for(k = 0;k<nCul;k++)
			{
				while(pArray[j][1].fYVal > pt[1].fYVal)
				{	j++;	k = nCul;}
			}
			for(k =0;k<nCul;k++)
			{
				while(pArray[i][1].fYVal < pt[1].fYVal)
				{	i--;	k= nCul;}
			}
		}
		if(i>=j)
		{
			if(i != j)
			{
				for(int nId=0; nId<nCul; nId++)
				{
					//swap(pArray[i][nId],pArray[j][nId]);
					//���������������
					TDataPointStrVal ntemp = pArray[i][nId];
					pArray[i][nId] = pArray[j][nId];
					pArray[j][nId] = ntemp;
				}
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(d < i)PointArrayRec(pArray,d,i,nCul,bAsceding);
	if(j < h)PointArrayRec(pArray,j,h,nCul,bAsceding);
}


bool CChart::QuickSort(CTString* pArray,int nSize)
{
	bool bFlag = true;
	if (nSize >1)
	{
		try
		{
			int low = 0;
			int	high = nSize - 1;
			QuickSortRecursive(pArray,low,high);			
		}
		catch(...)
		{
			m_szErrMsg.Format("��ͼʱ���ַ��������������������飡Line=%d,File=%s",__LINE__,__FILE__);
			bFlag = false;
		}
	}
	return bFlag;
}


void CChart::QuickSortRecursive(CTString *pArray,int d,int h)
{
	int i,j;
	CTString pt;
	i = h;
	j = d;
	pt = pArray[((int)((d+h)/2))];
	do{
		while(strcmp(pArray[j],pt)>0) j++;
		while(strcmp(pArray[i],pt)<0) i--;		
		
		if(i>=j)
		{
			if(i != j)
			{
				CTString dTemp;
				dTemp = pArray[i];
				pArray[i] = pArray[j];
				pArray[j] = dTemp;
			}
			i--;
			j++;
		}
	}while(j<=i);
	
	if(d < i)QuickSortRecursive(pArray,d,i);
	if(j < h)QuickSortRecursive(pArray,j,h);
}
////////////////////////////////////////////////////////////////////////////////////////