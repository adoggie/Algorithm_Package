// FillMissing.cpp: implementation of the CFillMissing class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FillMissing.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../DataInterface/DataInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFillMissing::CFillMissing()
{
}

CFillMissing::~CFillMissing()
{
	int i=0, nSize = m_vIM.size();
	for (i=0; i<nSize; i++)
	{
		IM *pIM = m_vIM.at(i);
		delete pIM;
	}
	m_vIM.clear();
}

bool CFillMissing::Parse(const std::string &VO)
{
	int i=0, j=0;
	AnaWord aw;
	AnaSyntax syn;
	
	aw.Import(VO);
	int nRowCount = m_pData->m_DataAccess.GetRecordCount();
	if (!AnaSyntax::exist(FILLMETHOD,aw))
		return false;
	int nCount = aw.GetWordCount();
	for (i=0; i<nCount; i++)
	{
		CTString szTemp = aw.GetAt(i);
		if (szTemp == NEARMEAN || szTemp == NEARMEDIAN )
		{
			IM *pIM = new IM;
			pIM->szMethod = szTemp;
			pIM->szVar    = aw.GetAt(i+2); //�����ֶ�
			szTemp = aw.GetAt(i+3);        //�ٽ��㷶Χ
			szTemp.MakeUpper();
			if (szTemp == ALL0 || szTemp == ALL1 || szTemp == ALL2)
				pIM->nValue = 0;
			else
			{
				pIM->nValue = atoi(szTemp);
				if (pIM->nValue < 0 || 
					pIM->nValue > nRowCount)
					pIM->nValue = nRowCount;
			}
			szTemp = aw.GetAt(i+4);        //�Ƿ�ΪĿ���ֶ���
			if (szTemp == END)
			{
				i += 4;
			}
			else
			{
				i += 5;
				pIM->szTag = szTemp;
			}
			m_vIM.push_back(pIM);
		}
		else if (szTemp == COLMEAN    || szTemp == COLMOD ||
			     szTemp == COLMEDIAN  || szTemp == LINEINSERT || szTemp == LINEHOPE)
		{
			IM *pIM = new IM;
			pIM->szMethod = szTemp;
			pIM->szVar    = aw.GetAt(i+2);  //�����ֶ�����
			szTemp = aw.GetAt(i+3);
			if (szTemp == END)              //�Ƿ�ΪĿ������ֶ�����
			{
				i += 3;
			}
			else
			{
				i += 4;
				pIM->szTag = szTemp;
			}
			m_vIM.push_back(pIM);
		}
	}
	return true;
}

bool CFillMissing::Verify()
{
	return true;
}

bool CFillMissing::Run(CDataInterface *pData, std::string lpszVO)
{
	int i=0, nSize=0;

	m_pData = pData;
	if (!Parse(lpszVO))
		return false;
	
	nSize = m_vIM.size();
	for (i=0; i<nSize; i++)
	{
		IM *pIM = m_vIM.at(i);	
		CField *pField = NULL;
		if (strlen(pIM->szTag) > 0) //����Ŀ���ֶΣ�����Ŀ���ֶ�
		{
			pField = m_pData->m_DataAccess.FieldByName(pIM->szTag);
			if (pField==NULL)
			{
				//����һ�����ֶ�
				pField = m_pData->m_DataAccess.m_pFieldList->CreateField(fDouble);
				m_pData->m_DataAccess.m_pFieldList->Add(pField);
				pField->SetFieldName(pIM->szTag);
			}
		}	
		//���ò�ֵ�㷨
		if (pIM->szMethod == COLMEAN)         //�о�ֵ
		{
			Mean(pIM->szVar, pField);
		}
		else if (pIM->szMethod == NEARMEAN)   //�ٽ����ֵ
		{
			NearMean(pIM->szVar,pIM->nValue,pField);
		}
		else if (pIM->szMethod == COLMEDIAN)  //��ֵ
		{
			Median(pIM->szVar,pField);
		}
		else if (pIM->szMethod == NEARMEDIAN) //�ٽ�����ֵ
		{
			NearMedian(pIM->szVar,pIM->nValue,pField);
		}
		else if (pIM->szMethod == LINEINSERT) //���Բ�ֵ
		{
			LineInsert(pIM->szVar,pField);
		}
		else if (pIM->szMethod == LINEHOPE)   //����Ԥ��
		{
			LineHope(pIM->szVar,pField);
		}
		else if (pIM->szMethod == COLMOD)     //������
		{
			Mod(pIM->szVar,pField);
		}
	}
	return true;
}

/*
���Բ�ֵ��
 */
void CFillMissing::LineInsert(LPCSTR lpszFieldName, CField *pFieldTag)
{
	double dPrev=0, dPost=0, dMissingValue=0;
	int nIndex1=0, nIndex2=0, nIndex3=0;
	
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	
	CFieldType ft = pField->GetFieldType();
	int nRowCount = m_pData->m_DataAccess.GetRecordCount();
	//��Դ�ֶ�˳���ȡ
	for (nIndex1=1; nIndex1<=nRowCount; nIndex1++)
	{
		m_pData->m_DataAccess.SetCurRecNo(nIndex1);
		m_pData->m_DataAccess.Edit();
		if (!pField->IsNull())
		{
			dPrev = pField->GetAsDouble();
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(dPrev);
				else
					pFieldTag->SetAsDouble(dPrev);
			}
		}
		else if(nIndex1==1 || nIndex1 == nRowCount) //��(β)��¼Ϊ�գ���������
			continue;
		else                                        //��¼Ϊ�գ����в�ֵ����
		{
			nIndex2 = nIndex1;                      // ������һ���ǿ�����
			do 
			{
				nIndex2 ++;
				m_pData->m_DataAccess.SetCurRecNo(nIndex2);
			}
			while (nIndex2 <= nRowCount && pField->IsNull());
			
			if (nIndex2 > nRowCount)                // ����û�зǿռ�¼���˳��������
				break;
			else 
			{
				dPost = pField->GetAsDouble();      // �õ��������
				// ���м�����п�ֵ�����Բ�ֵ����
				for (nIndex3=nIndex1; nIndex3<nIndex2; nIndex3++)
				{
					double x, y, z;
					x = nIndex1;
					y = nIndex2;
					z = nIndex3;
					dMissingValue = (z-y)/(x-1-y)*dPrev + (z-x+1)/(y-x+1)*dPost;
					
					// ���õ��Ĳ�ֵд��Ŀ���ֶ�
					m_pData->m_DataAccess.SetCurRecNo(nIndex3);
					m_pData->m_DataAccess.Edit();
					if (pFieldTag != NULL)
					{
						if (ft == fInt)
							pFieldTag->SetAsInteger((int)(dMissingValue+0.5));
						else
							pFieldTag->SetAsDouble(dMissingValue);
					}
					else
					{
						if (ft == fInt)
							pField->SetAsInteger((int)(dMissingValue+0.5));
						else
							pField->SetAsDouble(dMissingValue);
					}
				}	
				nIndex1 = nIndex2 - 1;
			}
		}
	}
}

/*
�о�ֵ���
*/
void CFillMissing::Mean(LPCSTR lpszFieldName, CField *pFieldTag)
{
	int nCnt = 0;
	double dbSum = 0;

	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;

	CFieldType ft = pField->GetFieldType();
	//����ÿһ����¼
	m_pData->m_DataAccess.First();
	while (! m_pData->m_DataAccess.Eof())
	{
		if (!pField->IsNull())
		{
			dbSum += pField->GetAsDouble();
			nCnt++;
		}
		m_pData->m_DataAccess.Next();
	}

	double dbAvg = dbSum/nCnt;
	m_pData->m_DataAccess.First();
	while (! m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(dbAvg + 0.5);
				else
					pFieldTag->SetAsDouble(dbAvg);
			}
			else
			{
				if (ft == fInt)
					pField->SetAsInteger(dbAvg + 0.5);
				else
					pField->SetAsDouble(dbAvg);
			}
		}
		else
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(pField->GetAsInteger());
				else
					pFieldTag->SetAsDouble(pField->GetAsDouble());
			}
		}
		m_pData->m_DataAccess.Next();
	}
	return ;
}

/*
 * �ٽ����ֵ��
 */
void CFillMissing::NearMean(LPCSTR lpszFieldName, int nNumber, CField *pFieldTag)
{
	if (nNumber == 0) //ȫ��
	{
		Mean(lpszFieldName,pFieldTag);
		return;
	}
	//�ٽ���
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nNewSize = nNumber*2;
	CDoubleVector v;
	//����ÿһ����¼
	m_pData->m_DataAccess.First();
	while (! m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			v.create(nNewSize); //����
			if (GetVaildData(v,pField,nNumber,false) &&
				GetVaildData(v,pField,nNumber,true))
			{
				double dbSum = v.mean();
				if (pFieldTag != NULL)
				{
					if (ft == fInt)
						pFieldTag->SetAsInteger(dbSum + 0.5);
					else
						pFieldTag->SetAsDouble(dbSum);
				}
				else
				{
					if (ft == fInt)
						pField->SetAsInteger(dbSum + 0.5);
					else
						pField->SetAsDouble(dbSum);
				}
			}
			else
			{
				if (pFieldTag != NULL)
					pFieldTag->SetNull();
			}
		}
		else
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(pField->GetAsInteger());
				else
					pFieldTag->SetAsDouble(pField->GetAsDouble());
			}
		}
		m_pData->m_DataAccess.Next();
	}
}

/*
 * �ٽ�����ֵ��
 */
void CFillMissing::NearMedian(LPCSTR lpszFieldName, int nNumber, CField *pFieldTag)
{
	//ȫ��
	if (nNumber == 0)
	{
		Median(lpszFieldName, pFieldTag);
		return;
	}
	//�ٽ���
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nNewSize = nNumber*2;
	CDoubleVector v;
	//����ÿһ����¼
	m_pData->m_DataAccess.First();
	while (! m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{				
			v.create(nNewSize);
			if (GetVaildData(v,pField,nNumber,false) &&
				GetVaildData(v,pField,nNumber,true ))
			{
				//��������
				v.Sort();
				//����λ��
				double dMedian = (v(nNumber/2-1) + v(nNumber/2))/2;
				if (pFieldTag != NULL)
				{
					if (ft == fInt)
						pFieldTag->SetAsInteger(dMedian + 0.5);
					else
						pFieldTag->SetAsDouble(dMedian);
				}
				else
				{
					if (ft == fInt)
						pField->SetAsInteger(dMedian + 0.5);
					else
						pField->SetAsDouble(dMedian);
				}
			}
			else
			{
				if (pFieldTag != NULL)
					pFieldTag->SetNull();
			}
		}
		else
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(pField->GetAsInteger());
				else
					pFieldTag->SetAsDouble(pField->GetAsDouble());
			}
		}
		m_pData->m_DataAccess.Next();
	}
}

/*

  ����Ԥ�ⷨ
*/

void CFillMissing::LineHope(LPCTSTR lpszFieldName, CField *pFieldTag)
{
	int i=0, nValidCount=0, nNullPoint=0;
	double dValue=0;
	CIntVector vIndex;
	CDoubleVector vValue;
	
	double dAggreesion=0, dCoefficient=0;
	double dIndexMean=0, dValueMean=0, dNumerator=0, dDenominator=0;
	
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nRowCount = m_pData->m_DataAccess.GetRecordCount();
	vIndex.create(nRowCount);
	vValue.create(nRowCount);
	
	//ȱʧֵ����
	for (i=1; i<nRowCount+1; i++)
	{
		m_pData->m_DataAccess.SetCurRecNo(i);
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			nNullPoint ++;
			vIndex(nRowCount - nNullPoint) = i;  //��������Ŵ�ĩβд���������
			//����
			dIndexMean += i;
		}
		else
		{			
			dValue = pField->GetAsDouble();
			vIndex(nValidCount) = i;            // �������ͷ�����д���������
			vValue(nValidCount) = dValue;       // ��ֵд��ֵ����
			if (pFieldTag != NULL)
				pFieldTag->SetAsDouble(dValue);
			//����
			dIndexMean += i;
			dValueMean += dValue;
			nValidCount ++;
		}
	}
	//����Ч����
	if (nValidCount < 1) 
		return;
	// ���кŵ�ƽ����
	dIndexMean /= nValidCount; // ���кŵ�ƽ����
	dValueMean /= nValidCount; // �ǿ���ֵ��ƽ����
	// �ֱ������Ӻͷ�ĸ
	dNumerator = dDenominator = 0;
	for (i=0; i<nValidCount; i++)
	{
		dNumerator   += (vIndex(i)-dIndexMean)*(vValue(i)-dValueMean);
		dDenominator += pow(vIndex(i)-dIndexMean, 2);
	}
	//ϵ������
	dCoefficient = dNumerator / dDenominator;           // �������ϵ��
	dAggreesion = dValueMean - dCoefficient*dIndexMean; // ����ֵ
	// ���������ĩβ�õ���ֵ���кţ�����Ӧ�����ֵд��Ŀ���ֶ�
	for (i=nRowCount-1; i>=nValidCount; i-- )
	{
		m_pData->m_DataAccess.SetCurRecNo(vIndex(i));
		m_pData->m_DataAccess.Edit();
		if (pFieldTag != NULL)
		{
			if (ft == fInt)
				pFieldTag->SetAsInteger(dAggreesion + dCoefficient*vIndex(i) + 0.5);
			else
				pFieldTag->SetAsDouble(dAggreesion + dCoefficient*vIndex(i));
		}
		else
		{
			if (ft == fInt)
				pField->SetAsInteger(dAggreesion + dCoefficient*vIndex(i) + 0.5);
			else
				pField->SetAsDouble(dAggreesion + dCoefficient*vIndex(i));
		}
	}
}
/*

  ������
*/

void CFillMissing::Mod(LPCTSTR lpszFieldName, CField *pFieldTag)
{
	int i=0, nValidCount=0;
	CDoubleVector vValue;

	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nRowCount = m_pData->m_DataAccess.GetRecordCount();
	vValue.create(nRowCount);
	m_pData->m_DataAccess.First();
	while (!m_pData->m_DataAccess.Eof())
	{
		if (!pField->IsNull())
		{
			double dValue = pField->GetAsDouble();
			vValue(nValidCount) = dValue;
			nValidCount ++;
		}
		m_pData->m_DataAccess.Next();
	}
	vValue.resize(nValidCount);
	//����
	vValue.Sort();
	//������
	double dMod = Mod(vValue);
	//���
	m_pData->m_DataAccess.First();
	while (!m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(dMod + 0.5);
				else
					pFieldTag->SetAsDouble(dMod);
			}
			else
			{
				if (ft == fInt)
					pField->SetAsInteger(dMod + 0.5);
				else
					pField->SetAsDouble(dMod);
			}
		}
		else
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(pField->GetAsInteger());
				else
					pFieldTag->SetAsDouble(pField->GetAsDouble());
			}
		}
		m_pData->m_DataAccess.Next();
	}
}

/*

  ����ֵ
*/

void CFillMissing::Median(LPCTSTR lpszFieldName, CField *pFieldTag)
{
	int i=0, nValidCount=0;
	double dMedian=0;
	CDoubleVector vValue;

	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nRowCount = m_pData->m_DataAccess.GetRecordCount();
	vValue.create(nRowCount);
	m_pData->m_DataAccess.First();
	while (!m_pData->m_DataAccess.Eof())
	{
		if (!pField->IsNull())
		{
			double dValue = pField->GetAsDouble();
			vValue(nValidCount) = dValue;
			nValidCount ++;
		}
		m_pData->m_DataAccess.Next();
	}
	vValue.resize(nValidCount);
	//����
	vValue.Sort();
	//����λ��
	if (nValidCount %2 == 0)
		dMedian = (vValue(nValidCount/2-1) + vValue(nValidCount/2))/2;
	else
		dMedian = vValue(nValidCount/2);
	//���
	m_pData->m_DataAccess.First();
	while (!m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(dMedian + 0.5);
				else
					pFieldTag->SetAsDouble(dMedian);
			}
			else
			{
				if (ft == fInt)
					pField->SetAsInteger(dMedian + 0.5);
				else
					pField->SetAsDouble(dMedian);
			}
		}
		else
		{
			if (pFieldTag != NULL)
			{
				if (ft == fInt)
					pFieldTag->SetAsInteger(pField->GetAsInteger());
				else
					pFieldTag->SetAsDouble(pField->GetAsDouble());
			}
		}
		m_pData->m_DataAccess.Next();
	}
}

int CFillMissing::Mod(CDoubleVector &v)
{
	int i=0;
	int nMod = 1, nIndex=1;
	int nCount = v.vlen();
	
	double dv = v(0);
	double dret = dv;
	for (i=1; i<nCount; i++)
	{
		if (dv == v(i))
			nIndex ++;
		else
		{
			if (nIndex > nMod)
			{
				nMod = nIndex;
				dret = dv;
			}
			nIndex = 1;
			dv = v(i);
		}
	}
	return dret;
}
/*
 
 bDown ��true ������������Ч���ݣ����û���ҵ��㹻�ģ��򷵻�false
 v: ������ݵ�����
 nNumber�����ҵĸ���
 */
bool CFillMissing::GetVaildData(CDoubleVector &v, CField *pField, int nNumber, bool bDown)
{
	bool bRst = false;
	int nOldNo = m_pData->m_DataAccess.GetRecNo();
	CDataAccess *pDa = &m_pData->m_DataAccess;

	if (!bDown)
	{//up
		nNumber--;
		pDa->Prior();
		while (!pDa->Bof())
		{
			if (!pField->IsNull())
			{
				v(nNumber) = pField->GetAsDouble();
				if (--nNumber<0)
				{
					bRst = true;
					break;
				}
			}
			pDa->Prior();
		}
	}
	else
	{
		int nCurNum = nNumber;
		pDa->Next();
		while (!pDa->Eof())
		{
			if (!pField->IsNull())
			{
				v(nCurNum) = pField->GetAsDouble();
				nCurNum++;
				if (nCurNum>=2*nNumber)
				{
					bRst = true;
					break;
				}
			}
			pDa->Next();
		}
	}
	m_pData->m_DataAccess.SetCurRecNo(nOldNo);
	return bRst;
}
