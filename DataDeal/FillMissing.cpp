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
			pIM->szVar    = aw.GetAt(i+2); //变量字段
			szTemp = aw.GetAt(i+3);        //临近点范围
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
			szTemp = aw.GetAt(i+4);        //是否为目标字段名
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
			pIM->szVar    = aw.GetAt(i+2);  //变量字段名称
			szTemp = aw.GetAt(i+3);
			if (szTemp == END)              //是否为目标变量字段名称
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
		if (strlen(pIM->szTag) > 0) //存在目标字段，怎用目标字段
		{
			pField = m_pData->m_DataAccess.FieldByName(pIM->szTag);
			if (pField==NULL)
			{
				//创建一个新字段
				pField = m_pData->m_DataAccess.m_pFieldList->CreateField(fDouble);
				m_pData->m_DataAccess.m_pFieldList->Add(pField);
				pField->SetFieldName(pIM->szTag);
			}
		}	
		//调用插值算法
		if (pIM->szMethod == COLMEAN)         //列均值
		{
			Mean(pIM->szVar, pField);
		}
		else if (pIM->szMethod == NEARMEAN)   //临近点均值
		{
			NearMean(pIM->szVar,pIM->nValue,pField);
		}
		else if (pIM->szMethod == COLMEDIAN)  //中值
		{
			Median(pIM->szVar,pField);
		}
		else if (pIM->szMethod == NEARMEDIAN) //临近点中值
		{
			NearMedian(pIM->szVar,pIM->nValue,pField);
		}
		else if (pIM->szMethod == LINEINSERT) //线性插值
		{
			LineInsert(pIM->szVar,pField);
		}
		else if (pIM->szMethod == LINEHOPE)   //线性预测
		{
			LineHope(pIM->szVar,pField);
		}
		else if (pIM->szMethod == COLMOD)     //列众数
		{
			Mod(pIM->szVar,pField);
		}
	}
	return true;
}

/*
线性插值法
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
	//对源字段顺序读取
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
		else if(nIndex1==1 || nIndex1 == nRowCount) //首(尾)记录为空，不做处理
			continue;
		else                                        //记录为空，进行插值处理
		{
			nIndex2 = nIndex1;                      // 查找下一个非空数据
			do 
			{
				nIndex2 ++;
				m_pData->m_DataAccess.SetCurRecNo(nIndex2);
			}
			while (nIndex2 <= nRowCount && pField->IsNull());
			
			if (nIndex2 > nRowCount)                // 后面没有非空记录，退出处理过程
				break;
			else 
			{
				dPost = pField->GetAsDouble();      // 得到后点数据
				// 对中间的所有空值做线性插值处理
				for (nIndex3=nIndex1; nIndex3<nIndex2; nIndex3++)
				{
					double x, y, z;
					x = nIndex1;
					y = nIndex2;
					z = nIndex3;
					dMissingValue = (z-y)/(x-1-y)*dPrev + (z-x+1)/(y-x+1)*dPost;
					
					// 将得到的插值写入目的字段
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
列均值填充
*/
void CFillMissing::Mean(LPCSTR lpszFieldName, CField *pFieldTag)
{
	int nCnt = 0;
	double dbSum = 0;

	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;

	CFieldType ft = pField->GetFieldType();
	//遍历每一个记录
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
 * 临近点均值法
 */
void CFillMissing::NearMean(LPCSTR lpszFieldName, int nNumber, CField *pFieldTag)
{
	if (nNumber == 0) //全部
	{
		Mean(lpszFieldName,pFieldTag);
		return;
	}
	//临近点
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nNewSize = nNumber*2;
	CDoubleVector v;
	//遍历每一个记录
	m_pData->m_DataAccess.First();
	while (! m_pData->m_DataAccess.Eof())
	{
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			v.create(nNewSize); //创建
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
 * 临近点中值法
 */
void CFillMissing::NearMedian(LPCSTR lpszFieldName, int nNumber, CField *pFieldTag)
{
	//全部
	if (nNumber == 0)
	{
		Median(lpszFieldName, pFieldTag);
		return;
	}
	//临近点
	CField *pField = m_pData->m_DataAccess.FieldByName(lpszFieldName);
	if (pField==NULL)
		return;
	CFieldType ft = pField->GetFieldType();
	int nNewSize = nNumber*2;
	CDoubleVector v;
	//遍历每一个记录
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
				//数据排序
				v.Sort();
				//求中位数
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

  线性预测法
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
	
	//缺失值处理
	for (i=1; i<nRowCount+1; i++)
	{
		m_pData->m_DataAccess.SetCurRecNo(i);
		m_pData->m_DataAccess.Edit();
		if (pField->IsNull())
		{
			nNullPoint ++;
			vIndex(nRowCount - nNullPoint) = i;  //按反序将序号从末尾写入序号数组
			//索引
			dIndexMean += i;
		}
		else
		{			
			dValue = pField->GetAsDouble();
			vIndex(nValidCount) = i;            // 按正序从头将序号写入序号数组
			vValue(nValidCount) = dValue;       // 将值写入值数组
			if (pFieldTag != NULL)
				pFieldTag->SetAsDouble(dValue);
			//索引
			dIndexMean += i;
			dValueMean += dValue;
			nValidCount ++;
		}
	}
	//无有效数据
	if (nValidCount < 1) 
		return;
	// 序列号的平均数
	dIndexMean /= nValidCount; // 序列号的平均数
	dValueMean /= nValidCount; // 非空数值的平均数
	// 分别计算分子和分母
	dNumerator = dDenominator = 0;
	for (i=0; i<nValidCount; i++)
	{
		dNumerator   += (vIndex(i)-dIndexMean)*(vValue(i)-dValueMean);
		dDenominator += pow(vIndex(i)-dIndexMean, 2);
	}
	//系数常量
	dCoefficient = dNumerator / dDenominator;           // 因变量的系数
	dAggreesion = dValueMean - dCoefficient*dIndexMean; // 常数值
	// 从序号数组末尾得到空值序列号，将相应的填充值写入目标字段
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

  列众数
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
	//排序
	vValue.Sort();
	//求众数
	double dMod = Mod(vValue);
	//填充
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

  列中值
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
	//排序
	vValue.Sort();
	//求中位数
	if (nValidCount %2 == 0)
		dMedian = (vValue(nValidCount/2-1) + vValue(nValidCount/2))/2;
	else
		dMedian = vValue(nValidCount/2);
	//填充
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
 
 bDown ：true ，向下找有有效数据，如果没有找到足够的，则返回false
 v: 存放数据的数组
 nNumber：查找的个数
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
