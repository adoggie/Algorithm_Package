// Correlation.h: interface for the CCorrelation class.
//				相关分析：包括person, spearman, kendall
//					夏东山			2006-03-25
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Correlation.h"

const int DEGREE_BASE  = 10;     //分组后至少计算的次数
const int VEC_DIV_LEN  = 7000;   //总数的均分开平方根
const int VEC_SPT_LEN  = 10000;  //分组后的新向量维数
const int VEC_MAX_LEN  = 60000;  //需要分组的最大维数
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCorrelation::CCorrelation()
{
	m_bSpearman = false;
	m_bPearson = false;
	m_bKendall = false;
	m_aCorr  = NULL;
	m_aSig  = NULL;
	m_adMeans = NULL;
	m_adStdDevs = NULL;
	m_anNofRow = NULL;
	m_anNofCorr = NULL;
	m_bOneTail = false;
	m_adWeightofCorr = NULL;
	m_bWeightMissing = false;
	m_bPairwise = false;//成对删除
	m_bWeight   = false;
	m_nWeight = -1;
}

CCorrelation::~CCorrelation()
{
	Clear();
	if( m_tVarInfo.iCount>0 )
		m_tVarInfo.DeleteObject();
}

//CORRELATIONS
//	/DATA=[FILE]
//	/VARLIST [varlist] 
//	/WEIGHT [varname]
//	/METHOD [PEARSON] [SPEARMAN] [KENDALL]
//	/MISSING=[{PAIRWISE**},{LISTWISE}]
//	/SIG=[{TWOTAIL**},{ONETAIL}]
bool CCorrelation::GetValueFromVo(CDataInterface *pDataInterface, string VO)
{
	CTStringArray xname;
	TIntStruct tVarTemp;

	AnaWord VOstr;
	VOstr.Import(VO);
	//解析－权重
	if (AnaSyntax::exist("/WEIGHT", VOstr))
	{
		AnaSyntax::getVAR("/WEIGHT", VOstr, xname);
		if (!(pDataInterface->getVarID(xname,tVarTemp)))
			return false;
		if (tVarTemp.iCount == 1)
			m_nWeight = tVarTemp.pValue[0];
		else
			return false;
		tVarTemp.DeleteObject();
		m_bWeight = true;
	}
	else
	{
		m_bWeight = false;
	}
	//解析－变量
	xname.RemoveAll();
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return false;

	m_bSpearman = AnaSyntax::findstr("/METHOD", "SPEARMAN", VOstr);
	m_bKendall = AnaSyntax::findstr("/METHOD", "KENDALL", VOstr);
	if (!m_bSpearman && !m_bKendall)
		m_bPearson = true;
	else
		m_bPearson = AnaSyntax::findstr("/METHOD", "PEARSON", VOstr);

	if (!m_bKendall)
		m_bPairwise = !AnaSyntax::findstr("/MISSING", "LISTWISE", VOstr);
	else
		m_bPairwise = false;

	m_bOneTail = AnaSyntax::findstr("/SIG", "ONETAIL", VOstr);
	
	return true;
}

CResult* CCorrelation::Correlation(CDataInterface *pDataInterface, string VO)
{
	int nCount=0;
	m_pResult = new CResult("相关分析");
	if (!GetValueFromVo(pDataInterface, VO))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误！" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	m_pDataInterface = pDataInterface;
	//计算相关性
	if (!CalcCorrelation()) 
	{
		CTString strWarning = "没有合法的数据输入，请重新选择！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误！" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//结果集
	CreateResult(m_pDataInterface,m_pResult);
	//释放内存
	Clear();
	return m_pResult;
}

//先分块，再处理
bool CCorrelation::KendallSplit(CDoubleMatrix &data_Matrix, int nCol_x, int nCol_y, int nVarCount)
{
	double p=0.0, fSign=0.0, dSqrtx=0.0, dSqrty=0.0;
	double *pSign, **pTau_x, **pTau_y;
	int i=0, j=0, nStart=0, nEnd=0, nDiv=0;
	CDoubleVector vec_x, vec_y,vec_x1, vec_y1;

	int nRowCount = data_Matrix(i).vlen();
	if (nRowCount < VEC_MAX_LEN+1) //不需要分块处理
		return false;
	srand(fabs(data_Matrix(nCol_x)(nRowCount/2)));
	nDiv  = sqrt(nRowCount/VEC_DIV_LEN)+DEGREE_BASE;
	pSign = new double[nDiv];
	pTau_x = new double *[nDiv];
	pTau_y = new double *[nDiv];
	for (i=0;i<nDiv; i++)//Tau, Tau', Tau"
	{
		*(pTau_x+i) = new double[3];
		*(pTau_y+i) = new double[3];
	}
	for (i=0;i<nDiv;i++)
	{
		for (j=0;j<3;j++)
		{
			pTau_x[i][j] = pTau_y[i][j] = 0.0;
		}
	}
	for (i=0;i<nDiv;i++)
	{
		p = double(rand())/RAND_MAX;
		if ((nRowCount*p+VEC_SPT_LEN) > nRowCount) //不足长度-大于3万分组且VEC_SPT_LEN＝5千的情况下，不会出现异常；否则需要判断首尾是否越界。
		{
			nStart = nRowCount*p - VEC_SPT_LEN;
			nEnd   = nRowCount*p;
		}
		else
		{
			nStart = nRowCount*p;
			nEnd   = nRowCount*p + VEC_SPT_LEN;
		}
		vec_x.create(VEC_SPT_LEN);
		vec_y.create(VEC_SPT_LEN);
		for (j=nStart;j<nEnd;j++) //赋值
		{
			vec_x(j-nStart) = data_Matrix(nCol_x)(j);
			vec_y(j-nStart) = data_Matrix(nCol_y)(j);
		}
		//处理
		vec_x1 = vec_x.copy();
		vec_y1 = vec_y.copy();
		//求i,j字段的Tau, Tau', Tau"
		CalcRank(vec_x,vec_x1,pTau_x[i]);
		CalcRank(vec_y,vec_y1,pTau_y[i]);
		//计算同序对与反序对之差
		pSign[i] = CalcSign(vec_x1, vec_y1);
	}
	//处理
	double fK = VEC_SPT_LEN * (VEC_SPT_LEN-1);
	for (i=0;i<nDiv;i++)
	{
		dSqrtx = sqrt((fK-pTau_x[i][0])/2);
		dSqrty = sqrt((fK-pTau_y[i][0])/2);
		if (dSqrtx * dSqrty > DOUBLE_ZERO || dSqrtx * dSqrty < -DOUBLE_ZERO)
			pSign[i] /= (dSqrtx * dSqrty);
	}
	//排序
	int nIndex = 0;
	CDoubleVector vec_sign(pSign,nDiv);
	vec_sign.Sort();
	if (vec_sign(nDiv-1) >= DOUBLE_ZERO)
	{
		if (nDiv<3) //取次大
			nIndex = 0;
		else        //取次大
			nIndex = nDiv-2;
	}
	else
	{
		if (nDiv<2) //取次大
			nIndex = 0;
		else
		if (nDiv<3)
			nIndex = 1;
		else        //取次大
			nIndex = nDiv-2;
	}
	//还原
	dSqrtx = sqrt((fK-pTau_x[nIndex][0])/2);
	dSqrty = sqrt((fK-pTau_y[nIndex][0])/2);
	if (dSqrtx * dSqrty > DOUBLE_ZERO || dSqrtx * dSqrty < -DOUBLE_ZERO)
	{
		pSign[nIndex] *= dSqrtx;
		pSign[nIndex] *= dSqrty;
	}
	//计算Kendall相关系数和显著性
	CalcKendallCorrAndSig( nCol_x, nCol_y, pSign[nIndex], pTau_x[nIndex], pTau_y[nIndex], VEC_SPT_LEN, nVarCount );
	//释放内存
	delete []pSign;
	for (i=0;i<nDiv;i++)
	{
		delete []pTau_x[i];
		delete []pTau_y[i];
	}
	return true;
}

bool CCorrelation::KendallSplit(TDataSet * pData_x, TDataSet * pData_y, int nCol_x, int nCol_y, int nValidCorr, int nVarCount)
{
	double p=0.0, fSign=0.0, dSqrtx=0.0, dSqrty=0.0;
	double *pSign, **pTau_x, **pTau_y;
	int i=0, j=0, k=0, nStart=0, nEnd=0, nDiv=0;

	if (nValidCorr < VEC_MAX_LEN+1) //不需要分块处理
		return false;
	srand(fabs(pData_x[nValidCorr/2].dValue));
	nDiv  = sqrt(nValidCorr/VEC_DIV_LEN)+DEGREE_BASE;
	pSign = new double[nDiv];
	pTau_x = new double *[nDiv];
	pTau_y = new double *[nDiv];
	for (i=0;i<nDiv; i++)//Tau, Tau', Tau"
	{
		*(pTau_x+i) = new double[3];
		*(pTau_y+i) = new double[3];
	}
	for (i=0;i<nDiv;i++)
	{
		for (j=0;j<3;j++)
		{
			pTau_x[i][j] = pTau_y[i][j] = 0.0;
		}
	}
	//分配内存
	TDataSet pData_x1[VEC_SPT_LEN];
	TDataSet pData_y1[VEC_SPT_LEN];
	TDataSet pData_x2[VEC_SPT_LEN];
	TDataSet pData_y2[VEC_SPT_LEN];
	for (i=0;i<nDiv;i++)
	{
		p = double(rand())/RAND_MAX;
		if ((nValidCorr*p+VEC_SPT_LEN) > nValidCorr) //不足长度-大于3万分组且VEC_SPT_LEN＝5千的情况下，不会出现异常；否则需要判断首尾是否越界。
		{
			nStart = (nValidCorr*p - VEC_SPT_LEN) < 0 ? 0 : (nValidCorr*p - VEC_SPT_LEN);
			nEnd   = (nValidCorr*p - VEC_SPT_LEN) < 0 ? VEC_SPT_LEN : nValidCorr*p;
		}
		else
		{
			nStart = nValidCorr*p;
			nEnd   = nValidCorr*p + VEC_SPT_LEN;
		}
		for (j=nStart;j<nEnd;j++) //赋值
		{
			pData_x1[j-nStart].dValue = pData_x[j].dValue;
			pData_y1[j-nStart].dValue = pData_y[j].dValue;
			pData_x1[j-nStart].iIndex = pData_y1[j-nStart].iIndex = j-nStart+1;
		}
		for (k=0; k<VEC_SPT_LEN; k++)
		{
			*(pData_x2+k) = *(pData_x1+k);
			*(pData_y2+k) = *(pData_y1+k);
		}
		//memcpy(pData_x2,pData_x1,sizeof(TDataSet)*VEC_SPT_LEN);
		//memcpy(pData_y2,pData_y1,sizeof(TDataSet)*VEC_SPT_LEN);
		//求i,j字段的Tau, Tau', Tau"
		CalcRank(pData_x1,pData_x2,VEC_SPT_LEN,pTau_x[i]);
		CalcRank(pData_y1,pData_y2,VEC_SPT_LEN,pTau_y[i]);
		//计算同序对与反序对之差
		pSign[i] = CalcSign(pData_x2, pData_y2,VEC_SPT_LEN);
	}
	//处理
	double fK = VEC_SPT_LEN * (VEC_SPT_LEN-1);
	for (i=0;i<nDiv;i++)
	{
		dSqrtx = sqrt((fK-pTau_x[i][0])/2);
		dSqrty = sqrt((fK-pTau_y[i][0])/2);
		if (dSqrtx * dSqrty > DOUBLE_ZERO || dSqrtx * dSqrty < -DOUBLE_ZERO)
			pSign[i] /= (dSqrtx * dSqrty);
	}
	//排序
	int nIndex = 0;
	CDoubleVector vec_sign(pSign,nDiv);
	CIntVector vec_index;
	vec_sign.Sort(vec_index);
	if (vec_sign(nDiv-1) >= DOUBLE_ZERO)
	{
		if (nDiv<3) //取次大
			nIndex = 0;
		else        //取次大
			nIndex = nDiv-2;
	}
	else
	{
		if (nDiv<2) //取次大
			nIndex = 0;
		else
		if (nDiv<3)
			nIndex = 1;
		else        //取次大
			nIndex = nDiv-2;
	}
	//还原
	dSqrtx = sqrt((fK-pTau_x[nIndex][0])/2);
	dSqrty = sqrt((fK-pTau_y[nIndex][0])/2);
	if (dSqrtx * dSqrty > DOUBLE_ZERO || dSqrtx * dSqrty < -DOUBLE_ZERO)
	{
		pSign[nIndex] *= dSqrtx;
		pSign[nIndex] *= dSqrty;
	}
	//计算Kendall相关系数和显著性
	CalcKendallCorrAndSig( nCol_x, nCol_y, pSign[nIndex], pTau_x[nIndex], pTau_y[nIndex], VEC_SPT_LEN, nVarCount );
	//释放内存
	delete []pSign;
	for (i=0;i<nDiv;i++)
	{
		delete []pTau_x[i];
		delete []pTau_y[i];
	}
	return true;
}

//只能对整行删除
bool CCorrelation::GetData2Matrix(CDoubleMatrix &dataMatrix)
{
	CDWordArray arrCol;
	int i=0, nColIndex=0;

	//variant...
	for (i=0; i<m_tVarInfo.iCount; i++)
	{
		nColIndex = m_tVarInfo.pValue[i];
		arrCol.Add(nColIndex);
	}
	//是否有权重,添加到最后-仅对Person.
	if (m_bPearson && m_bWeight)
	{
        arrCol.Add(m_nWeight);
	}
	//暂用缺省值删除
	int nNouse = m_pDataInterface->GetColsData(arrCol,dataMatrix,0);
	//是否存在缺省值删除
	//if (dataMatrix.mrow() < m_pDataInterface->GetRowCount())
	//	m_bWeightMissing = true;
	if (dataMatrix.mrow() < 1)
		return false;
	return true;
}

//统一接口
void CCorrelation::CalcRank(CDoubleVector &vec_data, CDoubleVector &vec_change, double *pTau)
{
	double fTmp=0,dR=0,fCCPrev=0;
	int nIndex=0, nCount=0;

	int nVlen = vec_data.vlen();//排序
	CIntVector vec_index;
	vec_data.Sort(vec_index);
	while (nIndex<nVlen) //遍历
	{
		nCount = 1;//寻找相同的值
		while ((nCount+nIndex)<nVlen)
		{
			fTmp = vec_data(nIndex+nCount)-vec_data(nIndex+nCount-1);
			if (fTmp > DOUBLE_ZERO || fTmp < -DOUBLE_ZERO)
				break;
			nCount++;
		}
		//秩
		dR = fCCPrev + double((nCount+1))/2.0;
		//将秩赋给相应的位置
		for (int j=nIndex; j<nIndex+nCount; j++)
			vec_change(vec_index(j)-1) = dR;

		if( pTau && nCount>1 )
		{
			double fT = double(nCount) * (nCount-1);
			pTau[0] += fT;
			pTau[1] += fT * ( nCount - 2 );
			pTau[2] += fT * ( 2*nCount + 5 );
		}
		nIndex  += nCount;
		fCCPrev += nCount;
	}
}

double CCorrelation::CalcSign(CDoubleVector &vec_x, CDoubleVector &vec_y)
{
	int  nRow1=0, nRow2=0;
	double fSign=0;

	int nVlen = vec_x.vlen();
	
	for (nRow1=0; nRow1<nVlen-1; nRow1++)
	{
		for (nRow2=nRow1+1; nRow2<nVlen; nRow2++)
		{
			if ( (vec_x(nRow2)>vec_x(nRow1) && vec_y(nRow2)>vec_y(nRow1)) ||
				 (vec_x(nRow2)<vec_x(nRow1) && vec_y(nRow2)<vec_y(nRow1)) )
				fSign += 1;
			else
			if ( (vec_x(nRow2)>vec_x(nRow1) && vec_y(nRow2)<vec_y(nRow1)) ||
				 (vec_x(nRow2)<vec_x(nRow1) && vec_y(nRow2)>vec_y(nRow1)) )
				fSign -= 1;
		}
	}
	return fSign;
}

void CCorrelation::Kendall(CDoubleMatrix &dataMatrix)
{
	int i=0, j=0, nRow=0;

	int nRowCount = dataMatrix.mrow();
	int nVarCount = m_tVarInfo.iCount;
	// 对i和j字段求两两相关性
	for (i=0; i<nVarCount; i++)
	{
		for (j=i; j<nVarCount; j++)
		{
			if (i==j || i>j)
			{
				m_aCorr[i*nVarCount+j]     = 1.0;
				m_aSig[i*nVarCount+j]      = 0.0;
				m_anNofCorr[i*nVarCount+j] = nRowCount;//相关样本数
			}
			else
			{
				m_anNofCorr[i*nVarCount+j] = nRowCount;//相关样本数
				//复制相关两字段数据（去除缺失值）
				double anTau_i[3], anTau_j[3];         //Tau, Tau', Tau"
				for( int t=0; t<3; t++ )
				{
					anTau_i[t] = anTau_j[t] = 0.0;     // 初始化为0
				}
				if (!KendallSplit(dataMatrix,i,j,nVarCount))//不需要分块处理-块小，放在里面处理
				{
					//以空间换时间
					CDoubleVector vec_i,vec_i_i,vec_j,vec_j_j;
					vec_i = dataMatrix(i).copy();
					vec_i_i = dataMatrix(i).copy();
					//求i,j字段的Tau, Tau', Tau"
					CalcRank(vec_i,vec_i_i,anTau_i);
					vec_i.destroy();
					vec_j = dataMatrix(j).copy();
					vec_j_j = dataMatrix(j).copy();
					CalcRank(vec_j,vec_j_j,anTau_j);
					vec_j.destroy();
					//计算同序对与反序对之差
					double fS_ij = CalcSign(vec_i_i, vec_j_j);
					//计算Kendall相关系数和显著性
					CalcKendallCorrAndSig( i, j, fS_ij, anTau_i, anTau_j, nRowCount, nVarCount );
				}
			}
		}
	}

	// 复制对称矩阵上半部分到下半部分
	for( j=0; j<nVarCount-1; j++ )
	{
		for( i=j+1; i<nVarCount; i++ )
		{
			m_anNofCorr[i*nVarCount+j] = m_anNofCorr[i+j*nVarCount];
			m_aCorr[i*nVarCount+j] = m_aCorr[i+j*nVarCount];
			m_aSig[i*nVarCount+j] = m_aSig[i+j*nVarCount];
		}	
	}
}

void CCorrelation::Spearman(CDoubleMatrix &dataMatrix)
{
	int i=0, j=0, nRow=0;
	long lWeightSum=0;
	double fTemp_i=0.0, fTemp_j=0.0, fTemp_ij=0.0, fTemp_ii=0.0, fTemp_jj=0.0;

	int nRowCount = dataMatrix.mrow();
	int nVarCount = m_tVarInfo.iCount;
	double * adCrossDevs    = new double[nVarCount*nVarCount]; //相关差积
	double * adCrossDevSelf = new double[nVarCount*nVarCount]; //两两相关的自身差积

	CDoubleVector vec_i,vec_j,vec_j_j;
	// 对i和j字段求两两相关性
	for (i=0; i<nVarCount; i++)
	{
		//以空间换时间
		if (i < nVarCount-1) //需要计算
		{
			vec_i = dataMatrix(i).copy();
			//求秩
			CDoubleVector v(dataMatrix(i)) ;
			CalcRank(v,vec_i);
			//值
			lWeightSum = vec_i.vlen();
			fTemp_i    = vec_i.sum();
			fTemp_ii   = vec_i.dot(vec_i);
		}
		for (j=i; j<nVarCount; j++)
		{
			if (i>=j)//没有必要计算
			{
				adCrossDevs[i*nVarCount+j]    = 2;
				adCrossDevSelf[i*nVarCount+j] = 2;
				adCrossDevSelf[i+nVarCount*j] = 2;
				m_anNofCorr[i*nVarCount+j]    = 2;
				continue;
			}
			//复制相关两字段数据
			if (j==nVarCount-1)
			{
				vec_j_j = dataMatrix(j).copy();
				CDoubleVector v(dataMatrix(j));
				CalcRank(v,vec_j_j);
			}
			else
			{
				vec_j = dataMatrix(j).copy();
				vec_j_j = dataMatrix(j).copy();
				CalcRank(vec_j,vec_j_j);
				vec_j.destruction();
			}
			// 求相关差积和自身差积
			fTemp_j    = vec_j_j.sum();
			fTemp_ij   = vec_i.dot(vec_j_j);
			fTemp_jj   = vec_j_j.dot(vec_j_j);
			adCrossDevs[i*nVarCount+j]    = fTemp_ij - (fTemp_i * fTemp_j / lWeightSum);
			adCrossDevSelf[i*nVarCount+j] = fTemp_ii - (fTemp_i * fTemp_i / lWeightSum );
			adCrossDevSelf[i+nVarCount*j] = fTemp_jj - (fTemp_j * fTemp_j / lWeightSum );
			m_anNofCorr[i*nVarCount+j]    = lWeightSum;
		}
	}
	// 用Pearson方法求相关系数和显著性
	CalcPearsonCorrAndSig(adCrossDevs, adCrossDevSelf, nVarCount);
	delete[] adCrossDevs;
	delete[] adCrossDevSelf;
}

void CCorrelation::CalcMeanAndStdDev(CDoubleMatrix &dataMatrix)
{
	int i=0;
	int nVarCount = m_tVarInfo.iCount;
	
	for (i=0;i<nVarCount;i++)
	{
		if (m_bWeight) 
		{
			m_anNofRow[i] = dataMatrix(nVarCount).sum();                //各字段的N值
			m_adMeans[i]  = dataMatrix(i).mean_w(dataMatrix(nVarCount));//各字段的均值
			if (m_anNofRow[i] <= 1+DOUBLE_ZERO)
				m_adStdDevs[i] = -1;
			else
				m_adStdDevs[i] = sqrt(dataMatrix(i).var_w(dataMatrix(nVarCount)));
		}
		else
		{
			m_anNofRow[i] = dataMatrix(i).vlen();   //各字段的N值
			m_adMeans[i]  = dataMatrix(i).mean();  //各字段的均值
			if (m_anNofRow[i] <= 1+DOUBLE_ZERO)
				m_adStdDevs[i] = -1;
			else
				m_adStdDevs[i] = sqrt(CDoubleVector(dataMatrix(i)).var());
		}
	}
}

void CCorrelation::Pearson(CDoubleMatrix &dataMatrix)
{
	int i=0, j=0, nRow=0;
	double fWeight=1.0, fWeightSum=0.0;
	double fTemp_i=0.0, fTemp_j=0.0, fTemp_ij=0.0, fTemp_ii=0.0, fTemp_jj=0.0;

	int nRowCount = dataMatrix.mrow();
	int nVarCount = m_tVarInfo.iCount;
	double *adCrossDevs    = new double[nVarCount*nVarCount];
	double *adCrossDevSelf = new double[nVarCount*nVarCount]; //两两相关的自身差积
	
	//计算均值及标准差
	CalcMeanAndStdDev(dataMatrix);
	// 对i和j字段计算两两相关性
	for( i=0; i<nVarCount; i++ )
	{
		if (m_bWeight)//设置了权重
		{
			fWeightSum = dataMatrix(nVarCount).sum();
			fTemp_i = dataMatrix(i).dot(dataMatrix(nVarCount));
		}
		else
		{
			fWeightSum = dataMatrix(i).vlen();
			fTemp_i = dataMatrix(i).sum();
			fTemp_ii = dataMatrix(i).dot(dataMatrix(i));
		}
		for( j=i; j<nVarCount; j++ )
		{
			if (i>=j)//没有必要计算
			{
				adCrossDevs[i*nVarCount+j]      = 2;
				adCrossDevSelf[i*nVarCount+j]   = 2;
				adCrossDevSelf[i+nVarCount*j]   = 2;
				m_adWeightofCorr[i*nVarCount+j] = 2;
				m_anNofCorr[i*nVarCount+j]      = 2;
				continue;
			}
			//求相关变量差积和相关样本数
			if (m_bWeight)//设置了权重
			{
				fTemp_ij = fTemp_ii = fTemp_jj = 0;
				fTemp_j = dataMatrix(j).dot(dataMatrix(nVarCount));
				for (nRow=0; nRow<nRowCount; nRow++)
				{
					fWeight = dataMatrix(nVarCount)(nRow);
					fTemp_ij += dataMatrix(i)(nRow) * dataMatrix(j)(nRow) * fWeight;
					fTemp_ii += dataMatrix(i)(nRow) * dataMatrix(i)(nRow) * fWeight;
					fTemp_jj += dataMatrix(j)(nRow) * dataMatrix(j)(nRow) * fWeight;
				}
			}
			else
			{
				fTemp_j = dataMatrix(j).sum();
				fTemp_ij = dataMatrix(i).dot(dataMatrix(j));
				fTemp_jj = dataMatrix(j).dot(dataMatrix(j));
			}
			//结果
			adCrossDevs[i*nVarCount+j]      = fTemp_ij - (fTemp_i * fTemp_j / fWeightSum);
			adCrossDevSelf[i*nVarCount+j]   = fTemp_ii - (fTemp_i * fTemp_i / fWeightSum );
			adCrossDevSelf[i+nVarCount*j]   = fTemp_jj - (fTemp_j * fTemp_j / fWeightSum );
			m_adWeightofCorr[i*nVarCount+j] = fWeightSum;
			m_anNofCorr[i*nVarCount+j]      = fWeightSum;
		}
	}
	// 计算相关系数和显著性
	CalcPearsonCorrAndSig(adCrossDevs, adCrossDevSelf, nVarCount);
	delete[] adCrossDevs;
	delete[] adCrossDevSelf;
}

void CCorrelation::Swap(TDataSet &t1, TDataSet &t2)
{
	TDataSet t = t1;
	t1 = t2;
	t2 = t;
}
//升序
void CCorrelation::QSort(TDataSet *pData, int nLow, int nHigh)
{
	int i,j;
	TDataSet t;

	i = nHigh;
	j = nLow;
	t = pData[(int)((i+j)/2)];
	do{
		while(pData[j] < t)  j++;
		while(pData[i] > t)  i--;
		if(i>=j)
		{
			if(i != j)
			{
				Swap(pData[i],pData[j]);
			}
			i--;
			j++;
		}
	}while(j<=i);

	if(nLow < i)  QSort(pData,nLow,i);
	if(j < nHigh) QSort(pData,j,nHigh);
}

bool CCorrelation::CalcCorrelation()
{
	int nCount = m_tVarInfo.iCount*m_tVarInfo.iCount;
	//分配内存
	m_anNofCorr = new double[nCount];
	m_aCorr     = new double[nCount];
	m_aSig      = new double[nCount];
	//若只有一个变量，重新设为整行删除
	if ((m_bWeight && m_tVarInfo.iCount<3) ||
		(!m_bWeight && m_tVarInfo.iCount<2))
		m_bPairwise = false;
	//分类
	if (m_bPairwise) //成对
	{
		CPtrArray arFieldData;
		if (!GetData(m_pDataInterface, arFieldData, 5))
			return false;
		// 读取权重值
		double *pWeight  = NULL;
		int nValidWeight = 0;
		m_bWeightMissing = false;
		//只对Pearson
		if (m_bPearson && m_bWeight)
		{
			pWeight = new double[m_pDataInterface->GetRowCount()];
			nValidWeight = m_pDataInterface->GetWeightData(pWeight);
			if (nValidWeight<1)
			{
				delete []pWeight;
				pWeight = NULL;
				return false;
			}
			if (nValidWeight < m_pDataInterface->GetRowCount())
				m_bWeightMissing = true;
		}
		//开始计算相关性
		if (m_bPearson)
		{
			m_adMeans   = new double[m_tVarInfo.iCount];
			m_adStdDevs = new double[m_tVarInfo.iCount];
			m_anNofRow  = new double[m_tVarInfo.iCount];
			m_adWeightofCorr = new double[nCount];

			CalcMeanAndStdDev( arFieldData, pWeight, m_tVarInfo.iCount,m_pDataInterface->GetRowCount() );
			Pearson( arFieldData, pWeight );
		}
		if (m_bKendall)
		{
			Kendall( arFieldData );
		}
		if (m_bSpearman)
		{
			Spearman( arFieldData );
 		}

		for( int i=0; i<arFieldData.GetSize(); i++ )
		{
			TDataSet * pDataSet = (TDataSet*)arFieldData.GetAt(i);
			delete[] pDataSet;
		}
		arFieldData.RemoveAll();
		if (pWeight)
			delete []pWeight;
		pWeight = NULL;
	}
	else
	{
		CDoubleMatrix dataMatrix;
		if (!GetData2Matrix(dataMatrix))
			return false;
		if (m_bPearson) //皮尔森相关
		{
			m_adMeans        = new double[m_tVarInfo.iCount];
			m_adStdDevs      = new double[m_tVarInfo.iCount];
			m_anNofRow       = new double[m_tVarInfo.iCount];
			m_adWeightofCorr = new double[nCount];
			
			Pearson(dataMatrix);
		}
		else 
		if (m_bKendall) //肯德尔相关
		{
			Kendall(dataMatrix);
		}
		else
		if (m_bSpearman)//斯皮尔曼相关
		{
			Spearman(dataMatrix);
		}
	}
	return true;
}

void CCorrelation::CreateResult(CDataInterface* pDataInterface,CResult* &pResult)
{
	int i,j;

	CTLTable* briefrTable=NULL;

	if( m_aCorr==NULL ) // 无合法数据输入提示
	{
		CTString	strWarning;
		strWarning.Format( " 注：没有合法的数据输入，无法执行相关分析！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "注意" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
		return;
	}
	if( m_bPearson && m_bWeightMissing )
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：有一个以上的样本数据因权重为零、负数或缺失而删除！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
	}
	if ((m_bSpearman || m_bKendall) && m_bWeight)
	{
		CTString	strWarning;
		strWarning.Format( " 权重处理：权重在本相关分析中无意义，不做任何处理！" );
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "权重处理" );
		pWarningTextRslt->AddString( strWarning );
		pResult->Add( pWarningTextRslt );
	}

	CTString strTableName, strTableTitle;
	if( m_bPearson )
	{
		strTableName.Format( "皮尔森相关系数矩阵" );
		strTableTitle.Format( "皮尔森相关分析" );

		// 创建均值和标准差矩阵表
		CTLTable * pMeansTable = new CTLTable;
		pMeansTable->CreateTable(2, 2);
		pMeansTable->SetRows( 1+m_tVarInfo.iCount );
		pMeansTable->SetCols( 4 );
		pMeansTable->SetTitle( "描述性统计量" );

		pMeansTable->InsertColumn( 0, "" );
		pMeansTable->InsertColumn( 1, "均值" );
		pMeansTable->InsertColumn( 2, "标准差" );
		pMeansTable->InsertColumn( 3, "记录数" );

		for(i=0;i<m_tVarInfo.iCount;i++)
		{
			pMeansTable->InsertItem( i, pDataInterface->GetFieldName(m_tVarInfo.pValue[i]) );
			pMeansTable->SetItemText( i, 1, m_adMeans[i] );
			if( m_anNofRow[i]>1 )
				pMeansTable->SetItemText( i, 2, m_adStdDevs[i] );

			if( m_anNofRow[i]<1e8 )
				pMeansTable->SetItemText( i, 3, (int)m_anNofRow[i] );
			else
				pMeansTable->SetItemText( i, 3, m_anNofRow[i] );
		}

		CRsltElementTable * pMeansTableRslt = new CRsltElementTable( "描述性统计量", pMeansTable );
		pResult->Add( pMeansTableRslt );
	}
	else if( m_bSpearman )
	{
		strTableName.Format( "斯皮尔曼相关系数矩阵" );
		strTableTitle.Format( "斯皮尔曼相关分析" );
	}
	else
	{
		strTableName.Format( "肯德尔相关系数矩阵" );
		strTableTitle.Format( "肯德尔相关分析" );
	}

	//创建相关系数矩阵表
	CTLTable  *pTable = new CTLTable;
	pTable->CreateTable(2, 2);
	if( m_bPairwise )
		pTable->SetRows(1+m_tVarInfo.iCount*3);
	else
		pTable->SetRows(1+m_tVarInfo.iCount*2);

	pTable->SetCols(2+m_tVarInfo.iCount);
	pTable->SetTitle( strTableTitle );
	
	pTable->InsertColumn( 0, "" );
	pTable->InsertColumn( 1, "" );
	for( i=0 ; i<m_tVarInfo.iCount; i++ )
		pTable->InsertColumn( i, pDataInterface->GetFieldName(m_tVarInfo.pValue[i]) );
	
	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		pTable->InsertItem( i*3, pDataInterface->GetFieldName(m_tVarInfo.pValue[i]), false );
		if( m_bPairwise )
		{
			pTable->InsertItem( i*3+1, "", false );
			pTable->InsertItem( i*3+2, "" );
		}
		else
			pTable->InsertItem( i*3+1, "" );
	}
	
	bool bNA = false;
	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);	
	if (nOutNote)
	{
		briefrTable=new CTLTable;
		//briefrTable->m_colorCell=RGB(255,0,0);
		briefrTable->InsertColumn(0,"结果说明");
	}

	for(i=0;i<m_tVarInfo.iCount;i++)
	{
		pTable->SetItemText( i*3, 1, "相关系数" );
		pTable->SetItemText( i*3+1, 1, "P    值" ); 
		if( m_bPairwise )
			pTable->SetItemText( i*3+2, 1, "相关记录数" );

		CTString strNamei;
		strNamei=pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);

		for(j=0;j<m_tVarInfo.iCount;j++)
		{
			if( m_anNofCorr[i*m_tVarInfo.iCount+j]>1 )
			{
				if( fabs(m_aCorr[i*m_tVarInfo.iCount+j])<1e-16 && 
					(fabs(m_aSig[i*m_tVarInfo.iCount+j])<1e-16 )) //||
					//fabs(m_aSig[i*m_tVarInfo.iCount+j]-0.5)<1e-16) )
				{
					if( i!=j )
					{
						pTable->SetItemText( i*3, j+2, "NA");
						pTable->SetItemText( i*3+1, j+2, "NA");
					}
					bNA = true;
				}
				else
				{
					pTable->SetItemText( i*3, j+2, m_aCorr[i*m_tVarInfo.iCount+j]);
					CTString strInfo;	
					//add by liytao 20030812
					CTString strNamej="";
					if (i<j)
					{
						strNamej=pDataInterface->GetFieldName(m_tVarInfo.pValue[j]);
						strInfo = strNamei + "和" + strNamej;

						CTString strRelate,strVal;
						if (m_aCorr[i*m_tVarInfo.iCount+j]>0)
							strRelate="正";
						else
							strRelate="负";
						CTString strStat,strTmp;
						if(m_bOneTail  ){
							if (m_aSig[i*m_tVarInfo.iCount+j]<0.05)
								strStat=",有显著的统计学意义";
							else
								strStat=",无显著的统计学意义";
							strTmp.Format("概率检验值=%f",m_aSig[i*m_tVarInfo.iCount+j]);
						
						}
						else{
							if (m_aSig[i*m_tVarInfo.iCount+j]*2<0.05)
								strStat=",有显著的统计学意义";
							else
								strStat=",无显著的统计学意义";
							strTmp.Format("概率检验值=%f",m_aSig[i*m_tVarInfo.iCount+j]*2);
							//strVal+=strTmp;
						}

						strVal.Format("(相关系数r=%f,%s)",m_aCorr[i*m_tVarInfo.iCount+j],strTmp.GetData());
						double dbRelate=fabs(m_aCorr[i*m_tVarInfo.iCount+j]);
						if (dbRelate==0){
							if (m_bPearson)
								strRelate="不存在线性相关";
							else
								strRelate="不存在等级相关";
						}
						else if (dbRelate<=0.3)
							strRelate+="微弱相关";
						else if (dbRelate<=0.5)
							strRelate+="低度相关";
						else if (dbRelate<=0.8)
							strRelate+="显著相关";
						else if (dbRelate<=1)
							strRelate+="高度相关";


						//strRelate=strRelate+strVal;
						//strInfo+=strRelate;

						strInfo=strInfo+strRelate+strVal+strStat;	

						if (briefrTable)
							briefrTable->InsertItem(briefrTable->GetTableRowS()+1,strInfo);
					}
					//add end
					if( i!=j && 1-fabs(m_aCorr[i*m_tVarInfo.iCount+j])>1e-16 ) 
						//fabs(m_aSig[i*m_tVarInfo.iCount+j])>1e-16 )
					{
						if(m_bOneTail  )
							pTable->SetItemText( i*3+1, j+2, m_aSig[i*m_tVarInfo.iCount+j]);
						else
							pTable->SetItemText( i*3+1, j+2, m_aSig[i*m_tVarInfo.iCount+j] * 2);
					}
				}
			}
			else
			{
				pTable->SetItemText( i*3, j+2, "NA");
				if( i!=j )
				{
					pTable->SetItemText( i*3+1, j+2, "NA");
				}
				bNA = true;
			}

			if( m_bPairwise )
			{
				if( m_anNofCorr[i*m_tVarInfo.iCount+j]<1e8 )
					pTable->SetItemText( i*3+2, j+2, (int)(m_anNofCorr[i*m_tVarInfo.iCount+j]+0.5));
				else
					pTable->SetItemText( i*3+2, j+2, m_anNofCorr[i*m_tVarInfo.iCount+j]);
			}
		}
	}

	CTString strTail, strTemp;
	if( m_bOneTail )
		strTail.Format( "\n * 显著性检验为单尾检验" );
	else
		strTail.Format( "\n * 显著性检验为双尾检验" );
	if( m_bPairwise )
		strTail += "\n * 成对排除缺失值";
	else
	{
		strTemp.Format( "\n * 整行排除缺失值  有效样本数N=%0.0f", m_anNofCorr[0] );
		strTail += strTemp;
	}

	if( bNA )
		strTail += "\n NA: 因一个以上变量无效无法进行计算";

	pTable->SetTail( strTail );
	if (briefrTable!=NULL&&briefrTable->GetTableRowS()>1){
			if (pResult->Count()<=0)
				pResult->Add(new CRsltElementTable("结果说明",briefrTable));
			else
				pResult->InsetAt(0,new CRsltElementTable("结果说明",briefrTable));
	}
	else if (briefrTable!=NULL)
		delete briefrTable;
	CRsltElementTable * pVariable=new CRsltElementTable( strTableName, pTable);
	pResult->Add(pVariable);			
}

void CCorrelation::Clear()
{
	if( m_aSig )
	{
		delete [] m_aSig;
		m_aSig = NULL;
	}
	if( m_aCorr )  
	{
		delete [] m_aCorr;
		m_aCorr = NULL;
	}
	if( m_adMeans ) 
	{
		delete[] m_adMeans;
		m_adMeans = NULL;
	}
	if( m_adStdDevs ) 
	{
		delete[] m_adStdDevs;
		m_adStdDevs = NULL;
	}
	if( m_adWeightofCorr ) 
	{
		delete[] m_adWeightofCorr;
		m_adWeightofCorr = NULL;
	}
	if( m_anNofRow ) 
	{
		delete[] m_anNofRow;
		m_anNofRow = NULL;
	}
	if( m_anNofCorr ) 
	{
		delete[] m_anNofCorr;
		m_anNofCorr = NULL;
	}
}

void CCorrelation::Pearson(CPtrArray &rFieldArray, double aWeight[])
{
	int iRow, i, j;
	double dWeight = 1, dWeightSum = 0;
	double dTemp_i, dTemp_j, dTemp_ij, dTemp_ii, dTemp_jj;

	int nVarCount = m_tVarInfo.iCount;
	int nRowCount = m_pDataInterface->GetRowCount();
	double * adCrossDevs = new double[nVarCount*nVarCount];
	double * adCrossDevSelf = new double[nVarCount*nVarCount]; //两两相关的自身差积
	
	// 对i和j字段计算两两相关性
	for (i=0; i<nVarCount; i++)
	{
		TDataSet* pFieldData_i = (TDataSet*)rFieldArray.GetAt(i);
		for (j=i; j<nVarCount; j++)
		{
			if (i>=j)//没有必要计算
			{
				adCrossDevs[i*nVarCount+j]      = 2;
				adCrossDevSelf[i*nVarCount+j]   = 2;
				adCrossDevSelf[i+nVarCount*j]   = 2;
				m_adWeightofCorr[i*nVarCount+j] = 2;
				m_anNofCorr[i*nVarCount+j]      = 2;
				continue;
			}
			TDataSet* pFieldData_j = (TDataSet*)rFieldArray.GetAt(j);
			// 求相关变量差积和相关样本数
			dWeight = 1, dWeightSum = 0;
			dTemp_i = dTemp_j = dTemp_ij = dTemp_ii = dTemp_jj = 0;
			for( iRow=0; iRow<nRowCount; iRow++ )
			{
				if( pFieldData_i[iRow].iIndex<0 || pFieldData_j[iRow].iIndex<0 )// 成对删除缺失值
					continue;
				if (aWeight)
					dWeight = aWeight[iRow];
				dTemp_i += pFieldData_i[iRow].dValue * dWeight;
				dTemp_j += pFieldData_j[iRow].dValue * dWeight;
				dTemp_ij += pFieldData_i[iRow].dValue * pFieldData_j[iRow].dValue * dWeight;
				dTemp_ii += pFieldData_i[iRow].dValue * pFieldData_i[iRow].dValue * dWeight;
				dTemp_jj += pFieldData_j[iRow].dValue * pFieldData_j[iRow].dValue * dWeight;
				dWeightSum += dWeight;
			}

			adCrossDevs[i*nVarCount+j] = dTemp_ij - (dTemp_i * dTemp_j / dWeightSum);
			adCrossDevSelf[i*nVarCount+j] = dTemp_ii - (dTemp_i * dTemp_i / dWeightSum );
			adCrossDevSelf[i+nVarCount*j] = dTemp_jj - (dTemp_j * dTemp_j / dWeightSum );
			m_adWeightofCorr[i*nVarCount+j] = dWeightSum;
			m_anNofCorr[i*nVarCount+j] = dWeightSum;
		}
	}
	// 计算相关系数和显著性
	CalcPearsonCorrAndSig( adCrossDevs, adCrossDevSelf, nVarCount );
	delete[] adCrossDevs;
	delete[] adCrossDevSelf;
}

void CCorrelation::Spearman( CPtrArray &rFieldArray)
{
	int i, j, k, iRow, nValidCorr;
	long nWeightSum;
	double dTemp_i, dTemp_j, dTemp_ij, dTemp_ii, dTemp_jj;
	int nVarCount = m_tVarInfo.iCount;
	int nRowCount = m_pDataInterface->GetRowCount();
	double * adCrossDevs = new double[nVarCount*nVarCount]; // 相关差积
	double * adCrossDevSelf = new double[nVarCount*nVarCount]; // 两两相关的自身差积

	// 对i和j字段求两两相关性
	for( i=0; i<nVarCount; i++ )
	{
		TDataSet * pData_i = (TDataSet*)rFieldArray.GetAt(i);

		for( j=i; j<nVarCount; j++ )
		{
			if (i>=j)//没有必要计算
			{
				adCrossDevs[i*nVarCount+j]    = 2;
				adCrossDevSelf[i*nVarCount+j] = 2;
				adCrossDevSelf[i+nVarCount*j] = 2;
				m_anNofCorr[i*nVarCount+j]    = 2;
				continue;
			}
			TDataSet * pData_j = (TDataSet*)rFieldArray.GetAt(j);
			
			//以空间换时间
			TDataSet * pSortData_i = new TDataSet[nRowCount];
			TDataSet * pSortData_j = new TDataSet[nRowCount];
			TDataSet * pSortData_i_i = new TDataSet[nRowCount];
			TDataSet * pSortData_j_j = new TDataSet[nRowCount];

			// 复制相关两字段数据（去除缺失值）
			nValidCorr = 0;
			for( iRow=0; iRow<nRowCount; iRow++ )
			{
				if( pData_i[iRow].iIndex<0 || pData_j[iRow].iIndex<0 )// 成对删除缺失值
					continue;
				pSortData_i[nValidCorr] = pData_i[iRow];
				pSortData_j[nValidCorr] = pData_j[iRow];
				nValidCorr++;
			}
			//copy
			for (k=0; k<nValidCorr; k++)
			{
				*(pSortData_i_i+k) = *(pSortData_i+k);
				*(pSortData_j_j+k) = *(pSortData_j+k);
			}
			//memcpy(pSortData_i_i,pSortData_i,sizeof(TDataSet)*nValidCorr);
			//memcpy(pSortData_j_j,pSortData_j,sizeof(TDataSet)*nValidCorr);
			// 求秩
			CalcRank( pSortData_i, pSortData_i_i, nValidCorr );
			CalcRank( pSortData_j, pSortData_j_j, nValidCorr );

			// 求相关差积和自身差积
			dTemp_i = dTemp_j = dTemp_ij = dTemp_ii = dTemp_jj = 0;
			nWeightSum = 0;
			for( iRow=0; iRow<nValidCorr; iRow++ )
			{
				dTemp_i += pSortData_i_i[iRow].dValue;
				dTemp_j += pSortData_j_j[iRow].dValue;
				dTemp_ij += pSortData_i_i[iRow].dValue * pSortData_j_j[iRow].dValue;
				dTemp_ii += pSortData_i_i[iRow].dValue * pSortData_i_i[iRow].dValue;
				dTemp_jj += pSortData_j_j[iRow].dValue * pSortData_j_j[iRow].dValue;

				nWeightSum += 1;
			}

			adCrossDevs[i*nVarCount+j] = dTemp_ij - (dTemp_i * dTemp_j / nWeightSum);
			adCrossDevSelf[i*nVarCount+j] = dTemp_ii - (dTemp_i * dTemp_i / nWeightSum );
			adCrossDevSelf[i+nVarCount*j] = dTemp_jj - (dTemp_j * dTemp_j / nWeightSum );

			m_anNofCorr[i*nVarCount+j] = nWeightSum;
			delete[] pSortData_i;
			delete[] pSortData_j;
			delete[] pSortData_i_i;
			delete[] pSortData_j_j;
		}
	}
	// 用Pearson方法求相关系数和显著性
	CalcPearsonCorrAndSig( adCrossDevs, adCrossDevSelf, nVarCount );

	delete[] adCrossDevs;
	delete[] adCrossDevSelf;
}

void CCorrelation::Kendall(CPtrArray &rFieldArray)
{
	int iRow, i, j, k;
	
	int nVarCount = m_tVarInfo.iCount;
	int nRowCount = m_pDataInterface->GetRowCount();
	// 对i和j字段求两两相关性
	for( i=0; i<nVarCount; i++ )
	{
		TDataSet * pData_i = (TDataSet*)rFieldArray.GetAt(i);

		for( j=i; j<nVarCount; j++ )
		{
			if( i==j )
			{
				int nWeightSum = 0;
				for( iRow=0; iRow<nRowCount; iRow++ )
				{
					if( pData_i[iRow].iIndex>-1 )//成对删除缺失值
					{
						nWeightSum += 1;
					}
				}
				m_aCorr[i*nVarCount+j] = 1.0;
				m_aSig[i*nVarCount+j]  = 0.0;
				m_anNofCorr[i*nVarCount+j] = nWeightSum;
				continue;
			}
			else
			if (i>j)
			{
				m_aCorr[i*nVarCount+j] = 1.0;
				m_aSig[i*nVarCount+j]  = 0.0;
				m_anNofCorr[i*nVarCount+j] = nRowCount;
				continue;
			}
			//
			TDataSet * pData_j = (TDataSet*)rFieldArray.GetAt(j);
			//以空间换时间
			TDataSet * pSortData_i = new TDataSet[nRowCount];
			TDataSet * pSortData_j = new TDataSet[nRowCount];
			// 复制相关两字段数据（去除缺失值）
			int nValidCorr = 0;
			int nN = 0;
		
			for( iRow=0; iRow<nRowCount; iRow++ )
			{
				if( pData_i[iRow].iIndex<0 || pData_j[iRow].iIndex<0 )//成对删除缺失值
					continue;
				pSortData_i[nValidCorr] = pData_i[iRow];
				pSortData_j[nValidCorr] = pData_j[iRow];

				nN += 1;     //求相关样本数
				nValidCorr++;
			}
			
			m_anNofCorr[i*nVarCount+j] = nN;  //相关样本数
			double anTau_i[3], anTau_j[3];    //Tau, Tau', Tau"
			for( int t=0; t<3; t++ )
			{
				anTau_i[t] = anTau_j[t] = 0.0;//初始化为0
			}
			if (!KendallSplit(pSortData_i,pSortData_j,i,j,nValidCorr,nVarCount))//不需要分块处理
			{
				TDataSet * pSortData_i_i = new TDataSet[nRowCount];
				TDataSet * pSortData_j_j = new TDataSet[nRowCount];
				//保存				
				for (k=0; k<nValidCorr; k++)
				{
					*(pSortData_i_i+k) = *(pSortData_i+k);
					*(pSortData_j_j+k) = *(pSortData_j+k);
				}
				//memcpy(pSortData_i_i,pSortData_i,sizeof(TDataSet)*nValidCorr);
				//memcpy(pSortData_j_j,pSortData_j,sizeof(TDataSet)*nValidCorr);
				//求i,j字段的Tau, Tau', Tau"
				CalcRank( pSortData_i, pSortData_i_i, nValidCorr, anTau_i );
				CalcRank( pSortData_j, pSortData_j_j, nValidCorr, anTau_j );
				//计算同序对与反序对之差
				double fS_ij = CalcSign( pSortData_i_i, pSortData_j_j, nValidCorr );
				//计算Kendall相关系数和显著性
				CalcKendallCorrAndSig( i, j, fS_ij, anTau_i, anTau_j, nN, nVarCount );
				delete[] pSortData_i_i;
				delete[] pSortData_j_j;
			}
			delete[] pSortData_i;
			delete[] pSortData_j;
		}
	}

	// 复制对称矩阵上半部分到下半部分
	for( j=0; j<nVarCount-1; j++ )
	{
		for( i=j+1; i<nVarCount; i++ )
		{
			m_anNofCorr[i*nVarCount+j] = m_anNofCorr[i+nVarCount*j];
			m_aCorr[i*nVarCount+j] = m_aCorr[i+nVarCount*j];
			m_aSig[i*nVarCount+j] = m_aSig[i+nVarCount*j];
		}
	}
}

bool CCorrelation::GetData(CDataInterface *pDataInterface, CPtrArray &rArray, int nGetType )
{
	// 从数据库读取用户选定的所有字段数据
	if (m_tVarInfo.iCount<1) 
		return false;
	TDataSet * pDataArray = NULL;
	for( int i=0; i<m_tVarInfo.iCount; i++ )
	{		
		//取回数据
		if (pDataInterface->GetColData(m_tVarInfo.pValue[i], pDataArray, nGetType) > 0)
		{
			rArray.Add(pDataArray); // 将各字段数据放入字段数组
			pDataArray = NULL;
		}
	}
	return true;
}

void CCorrelation::CalcMeanAndStdDev(CPtrArray &rFieldArray, double aWeight[], int k, int n)
{
	int iField, iRow;
	double dWeight = 1, dWeightSum = 0, dTemp = 0;

	for( iField=0; iField<k; iField++ )
	{
		TDataSet* pFieldData = (TDataSet*)rFieldArray.GetAt( iField );

		dWeight = 1, dWeightSum = 0, dTemp = 0;
		// 求列均值和标准差
		for( iRow=0; iRow<n; iRow++ )
		{
			if (pFieldData[iRow].iIndex<0 )//成对删除缺失值
				continue;
			if (aWeight)
				dWeight = aWeight[iRow];
			dTemp += pFieldData[iRow].dValue * dWeight;
			dWeightSum += dWeight;
		}
		m_anNofRow[iField] = dWeightSum; // 各字段的N值
		m_adMeans[iField] = dTemp / dWeightSum; // 各字段的均值

		// 计算标准差
		if( dWeightSum<=1 )
			m_adStdDevs[iField] = -1;
		else
		{
			dWeight = 1, dTemp = 0;
			for( iRow=0; iRow<n; iRow++ )
			{
				if( pFieldData[iRow].iIndex<1 )//成对删除缺失值
					continue;
				if (aWeight)
					dWeight = aWeight[iRow];
				dTemp += dWeight * (pFieldData[iRow].dValue - m_adMeans[iField]) * (pFieldData[iRow].dValue - m_adMeans[iField]);
			}
			m_adStdDevs[iField] = sqrt(dTemp/(dWeightSum-1)); // 各字段的标准差
		}
	}
}

void CCorrelation::Sort( TDataSet * pData, int n, bool bByValue )   //实数的shell（希尔）排序
{
    int k, j, i;
    TDataSet tempData;

	k = n/2;
	while( k>0 )
	{ 
		for( j=k; j<=n-1; j++ )
		{
			tempData = pData[j]; 
			i = j - k;
			if( bByValue ) // 按值排序
			{
				while( (i>=0) && (pData[i]>tempData) )
				{
					pData[i+k] = pData[i];
					i = i-k;
				}
			}
			else // 按索引号排序
			{
				while( (i>=0) && (pData[i].iIndex>tempData.iIndex) )
				{
					pData[i+k] = pData[i];
					i = i-k;
				}
			}
			pData[i+k] = tempData;
		}
		k = k/2;
	}
}

void CCorrelation::CalcRank(TDataSet *pData, TDataSet *pChange, int n, double *pTau)
{
	int nCount = 1, iIndex = 0;
	double fTmp=0.0,dR=0.0, fCCPrev=0.0;

	QSort(pData,0,n-1);//排序
	while( iIndex<n )  //遍历整个数组
	{
		nCount = 1;
		while( (nCount+iIndex)<n )//寻找相同的值
		{
			fTmp = pData[iIndex+nCount].dValue-pData[iIndex+nCount-1].dValue;
			if (fTmp > DOUBLE_ZERO || fTmp < -DOUBLE_ZERO)
				break;
			nCount++;
		}
		
		//求秩
		dR = fCCPrev + double((nCount+1))/2.0;
		//将秩赋给相应的位置
		for(int j=iIndex; j<iIndex+nCount; j++ )
		{
			pChange[pData[j].iIndex-1].dValue = dR;
		}

		if( pTau && nCount>1 )
		{
			double nT = double(nCount) * (nCount-1);

			pTau[0] += nT;
			pTau[1] += nT * ( nCount - 2 );
			pTau[2] += nT * ( 2*nCount + 5 );
		}
		iIndex += nCount;
		fCCPrev += nCount;
	}
}

void CCorrelation::CalcPearsonCorrAndSig( double adCrossDevs[], double adCrossDevSelf[], int k )
{
	int i, j;

	// 计算i和j变量的相关系数和显著性
	for( i=0; i<k; i++ )
	{
		for( j=0; j<k; j++ )
		{
			if( i==j )
			{
				m_aCorr[i*k+j] = 1;
				m_aSig[i*k+j] = 0;
			}
			else if( i<j )
			{
				if( adCrossDevs[i*k+i]<1e-20 || adCrossDevs[j*k+j]<1e-20 )
				{
					m_aCorr[i*k+j] = 0;
				}
				else
				{
					m_aCorr[i*k+j] = adCrossDevs[i*k+j] / 
									  sqrt( fabs(adCrossDevSelf[i*k+j] * adCrossDevSelf[i+k*j]) );
				}

				// 用t分布求显著性
				if( m_bPearson )
				{
					if( 1-fabs(m_aCorr[i*k+j])<1e-20 || m_adWeightofCorr[i*k+j]<=2.0 )
						m_aSig[i*k+j] = 0.0;
					else if( adCrossDevs[i*k+i]<1e-20 || adCrossDevs[j*k+j]<1e-20 )
					{
						m_aSig[i*k+j] = 0.0;
					}
					else
					{
						double t = fabs( m_aCorr[i*k+j] ) * 
								   sqrt( (m_adWeightofCorr[i*k+j] - 2)/
										 (1 - m_aCorr[i*k+j] * m_aCorr[i*k+j]) );
						m_aSig[i*k+j] = 1 - tcdf( t, m_adWeightofCorr[i*k+j]-2 );
					}
				}
				else if( m_bSpearman )
				{

					if( 1-fabs(m_aCorr[i*k+j])<1e-20 || m_anNofCorr[i*k+j]<=2 )
						m_aSig[i*k+j] = 0.0;
					else if( adCrossDevs[i*k+i]<1e-20 || adCrossDevs[j*k+j]<1e-20 )
					{
						m_aSig[i*k+j] = 0.0;
					}
					else
					{
						double t = fabs( m_aCorr[i*k+j] ) * 
								   sqrt( (m_anNofCorr[i*k+j] - 2)/
										 (1 - m_aCorr[i*k+j] * m_aCorr[i*k+j]) );
						m_aSig[i*k+j]	= 1 - tcdf( t, m_anNofCorr[i*k+j]-2 );
					}
				}
			}
			else // 对称矩阵，复制上半部分到下半部分
			{
				m_aCorr[i*k+j] = m_aCorr[j*k+i];
				m_aSig[i*k+j] = m_aSig[j*k+i];
				m_anNofCorr[i*k+j] = m_anNofCorr[j*k+i];
			}
		}
	}
}
//权重无意义
double CCorrelation::CalcSign(TDataSet *pData_x, TDataSet *pData_y, int n)
{
	int  iRow1, iRow2;
	
	double fS = 0.0;
	for( iRow1=0; iRow1<n-1; iRow1++ )
	{
		for( iRow2=iRow1+1; iRow2<n; iRow2++ )
		{
			if( pData_x[iRow2].dValue>pData_x[iRow1].dValue && pData_y[iRow2].dValue>pData_y[iRow1].dValue ||
				pData_x[iRow2].dValue<pData_x[iRow1].dValue && pData_y[iRow2].dValue<pData_y[iRow1].dValue )
				fS += 1;
			else if( pData_x[iRow2].dValue>pData_x[iRow1].dValue && pData_y[iRow2].dValue<pData_y[iRow1].dValue ||
				pData_x[iRow2].dValue<pData_x[iRow1].dValue && pData_y[iRow2].dValue>pData_y[iRow1].dValue )
				fS -= 1;
		}
	}

	return fS;
}

void CCorrelation::CalcKendallCorrAndSig( int x, int y, double fS, double aTau_x[], double aTau_y[], int n, int k )
{
	double fK = double(n) * double(n-1);

	double dSqrtx = sqrt( (fK-(double)aTau_x[0])/2 );
	double dSqrty = sqrt( (fK-(double)aTau_y[0])/2 );

	if( fabs(dSqrtx)<1e-20 || fabs(dSqrty)<1e-20 )
		m_aCorr[x*k+y] = 0;
	else
		m_aCorr[x*k+y] = fS / ( dSqrtx * dSqrty );

	if( fK==0 || n-2==0 )
		m_aSig[x*k+y] = 0;
	else
	{
		double dD = ( fK * (2*n+5) - aTau_x[2] - aTau_y[2] ) / 18.0 +
				   ( aTau_x[1]*aTau_y[1] / (9.0*fK*(n-2)) ) +
				   ( aTau_x[0]*aTau_y[0] / (2.0*fK) );

		if( dD<1e-20 )
			m_aSig[x*k+y] = 0;
		else
		{
			double dZ = fS / sqrt( dD );
			m_aSig[x*k+y] = 1 - norm( fabs(dZ) );
		}
	}
}
