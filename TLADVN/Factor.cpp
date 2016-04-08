// Factor.cpp: implementation of the CFactor class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
// #include "tlAdvn.h"
#include "Factor.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const double EPS=0.000001;
const int ID=25;
CFactor temp;

CFactor::CFactor()
{
	m_pResult			= NULL;

	m_bOutput			= false;
	m_bComBigOne        = false;
	m_bRotateFlag       = false;
	m_bCompenFlag       = false;
	m_bEigenFlag        = false;
	m_bZhengjiao        = true;
	m_bKMO              = true;	//2003-1-25 Eric 暂时代表球度方差检验
	m_bAntiImage		= true;
	m_bAntiImage2       = true;
	m_bShengyu			= true;	//2003-1-25 Eric 残差
	m_bZaisheng			= true;	//2003-1-25 Eric 再生
	
	m_nProcType			= 1;//主成分分析--2,因子分析--1.
	
	m_nRow				= 0;
	m_nCol				= 0;
	m_nSel              = 0;
	m_nIteration        = 0;
	m_dkmo              = 0;
	m_dExtact			= 0.8;
	m_fPro              = (float)0.9;
	m_nMissingMethod    = 0;
	
	m_bCovarian			= false;
}

CFactor::~CFactor()
{
	InitFree();
}

bool IsExisted(CDataInterface *p,CTString str)
{
	for(int i=0;i<p->GetColCount();i++)
	{
		if(p->GetFieldName(i)==str)
		{
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//              主成分分析的计算函数
//////////////////////////////////////////////////////////////////////////

bool CFactor::GetValueFromVOPrin(CDataInterface *pDataInterface, std::string VO)
{
//	PCOMPONENT
//		/DATA=[FILE]
//		/VARLIST=[varlist]
//		/MISSING=[{NONE**}]
//		/METHOD=[{CORR**},{COV}]
//		/RULE=[{CUMULANT(0.9**)},{NUMFACTOR(1)}]
//		/PRINT [FACTORS]
	m_bAntiImage = FALSE;
	m_bAntiImage2 = FALSE;
	m_bShengyu = FALSE;
	m_bZaisheng = FALSE;
	m_nScoreMethod			= 0;
	m_nRotateMethod			= 0;
	m_nExtractMethod		= 0;
	m_nMaxIterCount = 100;
	m_nIteration = 0;
	m_bScore = FALSE;
	m_bKMO = TRUE;

	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo2)))
		return FALSE;
	m_nMissingMethod = 0;
	m_fPro = 0.9;
	m_nExtractStandard = 0;
	m_nAnaMethod = int(AnaSyntax::findstr("/METHOD", "COV",VOstr));
	if (AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr)) 
	{
		m_nExtractStandard = 1;
		m_nFactorNum = AnaSyntax::getvalue("/RULE", "NUMFACTOR",VOstr);
	}
	if (AnaSyntax::findstr("/RULE", "CUMULANT",VOstr))
		m_fPro = AnaSyntax::getvalue("/RULE", "CUMULANT",VOstr);
	m_bOutput = AnaSyntax::findstr("/PRINT", "FACTORS",VOstr);
	return TRUE;
}



CResult * CFactor::OnFactor2(CDataInterface *pDataInterface, std::string &VO)
{	
	m_bPrincipal=TRUE;//zkc 2003.8.20
	m_strResultExplain.RemoveAll();
	m_pOriDataInterface = pDataInterface;
	m_nProcType = 2;
	m_bOutput = FALSE;
	m_bComBigOne = FALSE;
	m_pResult = NULL;
	m_pResult = new CResult("主成份分析");
	if (!(GetValueFromVOPrin(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//取数据
	m_VectInputData = GetData(pDataInterface,m_tVarInfo2);//同时给m_pDataInterface赋值
	if(!m_VectInputData.vlen())
	{
		delete m_pResult;
		m_pResult = NULL;
		OnFactorExit();
		return NULL;
	}
	//处理权重.
	int i = 0;
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight(i)<0)
			m_weight[i] = 0;
	}
	//WriteData(m_weight,"weight");
	
	CPtrArray arField;
	int * pt;
	for(i=0;i<m_tVarInfo2.iCount;i++)
	{
		pt = new int;
		*pt = m_tVarInfo2.pValue[i];
		arField.Add(pt);
	}
	
	
	int m          = m_tVarInfo2.iCount;
	CDoubleMatrix matrix = VectorToMatrix(m_VectInputData ,m_nRow,m);//Ywj  2005-05-09
	this->SetRowsCols(m_nRow,m);
	m_VectData = copy(m_VectInputData);
	//取参数
	m_VectDataFormlized = FormlizeEx(m_VectInputData,m_weight);
	
	
	double dbKappa          = 4;
	
	if(m_fPro > 1)
	{
		m_fPro = 1;
	}
	if(m_fPro<0)
	{
		m_fPro = 0;
	}
	
	//进行相应的计算
	//1:因子提取：		
	ComponMethod(m_nAnaMethod);//0:主成分法 ; 1:主因子法
	
	//注:不进行因子旋转
	//2:整理因子	
	CDoubleMatrix ResortMatrix = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
	m_VectRotateCom.create( ResortMatrix.mrow()*ResortMatrix.mcol());
	ProcessRotateMatrix(ResortMatrix);
	m_VectRotateCom = MatrixToVector(ResortMatrix);
	//3:主成分
	PrincipalComponent();//存于m_pScore中.
	
//使用了m_pScore;
	if (m_bOutput)
		PrincipalTable(m_pResult);
	
	CreateResult(m_pResult);
	
	KMO(1);
	Sphericity(matrix);		
	OnFactorExit();

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{				
		CTLTable* pTable = new CTLTable;
		pTable->InsertColumn(0,"结果说明");
		for(i=0;i<m_strResultExplain.GetSize();i++)
		{
			pTable->InsertItem(i,m_strResultExplain.GetAt(i));
		}
		m_pResult->InsetAt(0,new CRsltElementTable("结果说明",pTable));
	}

	m_bComBigOne = false;
	m_bCovarian = false;
	m_bScore = false;
	m_nRotateMethod = 0;
	
	return m_pResult;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//	求解特征值,pData:输入矩阵的指针
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::Eigenvalue(CDoubleVector& Vect_Data)
{
	m_VectEigenValue.create(m_nCol);
	m_EigenVector.create(m_nCol*m_nCol); // Ywj  2005-05-09
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.00001);//求解特征向量m_pEigenVector
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//特征向量排序m_pEigenVector
	int i =0;
	for(i =0;i<m_nCol;i++)
		m_VectEigenValue[i] = Vect_Data[i+i*m_nCol];
	
	CreateEigenValueResult(Vect_Data,m_nCol);
	//TraceVector(Vect_Data);
	//TraceVector(m_VectEigenValue);
	m_nSel = GetSelNum(m_VectEigenValue);//计算符合要求的特征值的个数
	//TraceVector(m_EigenVector);
	
//	double sum(0);
//	for( i=0;i<m_nCol;i++)
//	{
//		sum += Vect_Data[m_nCol*i+i];
//	}
//	m_nSel=0;
//	double pro = 0;
//	for(i=0;i<m_nCol;i++)
//	{	
//		if( ( pro < m_fPro && m_nExtractStandard == 0)
//			||( m_nSel < m_nFactorNum && m_nSel < m_nCol  && m_nExtractStandard == 1)			
//			)
//		{	
//			pro += Vect_Data[i*m_nCol + i]/sum;
//			m_nSel++;
//		}
//		//为regression因子得分做的检查
//		if( Vect_Data[i*m_nCol+i] <= 1e-8 )
//		{
//			m_bEigenFlag = true;
//		}
//	}

	
	//处理特征向量
	ProcessEigenVector(Vect_Data);
}
void CFactor::Eigenvalue(CDoubleVector& Vect_Data,CDoubleVector& Vect_EigVector)
{
	int nLen = Vect_Data.vlen();
	assert(nLen>0);
	int nCol = sqrt(nLen);
	if(Vect_EigVector.vlen() != nLen )
		Vect_EigVector.create(nLen); // Ywj  2005-05-09
	cjcbj(Vect_Data,nCol,Vect_EigVector,0.00001);//求解特征向量m_pEigenVector
	SortEigenVect(Vect_Data,Vect_EigVector,nCol,nCol,1);//特征向量排序m_pEigenVector
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////
//	初始化和内存释放函数
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::InitFree()
{
	
	if(m_pDataInterface)
		delete m_pDataInterface;m_pDataInterface = NULL;

	m_bRotateFlag       = false;
	m_bCompenFlag       = false;
	m_bEigenFlag        = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//	协方差分析方法的调整数据步骤，调整主成分矩阵(Eric:因子载荷矩阵)和共性方差矩阵
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::AdjuestResult()
{
	assert( m_nSel > 0 && m_VectComMatrix.vlen()>0);
	int i,j;
	////TraceVector(m_VectComMatrix);
	////TraceVector(m_VectFirstCovariance);
	for(i=0;i<m_nCol;i++)
	{
		for(j=0;j<m_nSel;j++)
		{
			m_VectComMatrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
		}
	}

	//TraceVector(m_VectComMatrix);
	for(i=0;i<m_nCol;i++)
	{
		m_VectCommunality[i] *= 1/m_VectCorrelation[i*m_nCol+i];
	}
	//TraceVector(m_VectCommunality);
	////TraceVector(m_VectCommunality);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//	主成分法的外部调用接口，						
//		nMethod == 0 为主成分法相关系数矩阵法					
//		nMethod == 1 为主成分法协方差矩阵法	

//		nMethod == 2 主因子法相关系数矩阵法				
//		nMethod == 3 主因子法协方差矩阵法																									
//////////////////////////////////////////////////////////////////////////////////////////////////
void  CFactor::ComponMethod(int nMethod)
{
	//	int i,j;
	CDoubleVector Vect_Data;
	switch(nMethod)
	{
	case 0://主成分,相关系数矩阵法
		m_bCompenFlag = true;
		m_bCovarian = false;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,0);//计算相关系数矩阵.
		Eigenvalue(Vect_Data);//求解特征根．pData输入矩阵的指针
		break;
	case 1://主成分,协方差矩阵法
		m_bCompenFlag = true;
		m_bCovarian = true;
		
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,1);//计算协方差
		m_VectFirstCovariance.create( m_nCol*m_nCol);
		m_VectFirstCovariance = copy(Vect_Data);
		Eigenvalue(Vect_Data);//求解特征根．pData输入矩阵的指针
		
		//{{{START 2003-2-13 Eric
		if(m_nAnaMethod == 1 && m_bZaisheng)
		{			
			m_UnAdjustComMatrix.create(m_nCol * m_nSel);
			m_UnAdjustComMatrix = copy(m_VectComMatrix);
		}
		//}}}END  2003-2-13 Eric
		
		break;
	case 2://主因子,相关系数矩阵法
		m_bCovarian = false;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,0);
		//TraceVector(Vect_Data);
		//{{{START 2003-2-8 Eric
		//Eigenvalue(pData);//求解特征根．pData输入矩阵的指针
		CalcFirstCommunalityAndEig(Vect_Data,0);//求解相关系数矩阵的特征根，求解相关系数矩阵的共性方差．pData输入矩阵的指针
		//}}}END  2003-2-8 Eric
		PrinAxiFactor(0);
		break;
	case 3://主因子,协方差矩阵法
		m_bCovarian = true;
		Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,1);
		
		if(m_VectFirstCovariance.vlen() != 0)
		{
			m_VectFirstCovariance.destruction();
		}

		m_VectFirstCovariance.create( m_nCol*m_nCol);
		m_VectFirstCovariance = copy(Vect_Data);
		CalcFirstCommunalityAndEig(Vect_Data,1);//求解协方差矩阵的特征根，求解协方差矩阵的共性方差．pData输入协方差矩阵的指针
		//}}}END  2003-2-9 Eric
		
		PrinAxiFactor(1);
		
	}
	Vect_Data.Sort();
	CreateChartResult(Vect_Data,m_nCol);
	Vect_Data.destroy();
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//	主因子法
//	  nIndex == 0 : 相关系数矩阵法
//	  nIndex == 1 : 协方差矩阵法
//
/////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::PrinAxiFactor(int nIndex)
{
	assert( m_VectCorrelation.vlen()>0 && m_VectCommunality.vlen()>0 && m_EigenVector.vlen()>0 );
	int i;
	double e = 1.0;
	long   lcount(0);
	CDoubleVector Vect_R (m_nCol*m_nCol);
	m_nIteration = 0;
	double fPreE;
	bool bZoomIn = true;
	
	m_InitCommunality.create(m_nCol);
	m_InitCommunality = copy(m_VectCommunality);
	
	//TraceVector(m_VectCorrelation);
	//TraceVector(m_InitCommunality);
	while(e>0.0001 && bZoomIn)
	{
		if(m_nIteration > m_nMaxIterCount) //超过迭代次数
			break;
		m_nIteration++;
		//求解约相关系数矩阵
        Vect_R = copy(m_VectCorrelation);
		//TraceVector(Vect_R);
		for( i=0;i<m_nCol;i++)
		{
			Vect_R[i*m_nCol+i] = m_VectCommunality[i];
		}
		//TraceVector(Vect_R);
		//求解约相关系数矩阵的特征值和特征向量
		cjcbj(Vect_R,m_nCol,m_EigenVector,0.00001);
		
		//求解因子负荷矩阵,计算共性矩阵
		ProcessEigenVector(Vect_R,1);
		//TraceVector(Vect_R);
		if( nIndex == 1 )
		{//协方差矩阵法
			AdjuestResult();
			m_bCovarian = true;
		}
		else
		{
			m_bCovarian = false;
		}
		
		//计算e
		assert(m_VectPreCommunality.vlen()>0);
		double max = -1e10;
		for(i=0;i<m_nCol;i++)
		{
			m_VectPreCommunality[i] -=	  m_VectCommunality[i];
			m_VectPreCommunality[i] =	  fabs(m_VectPreCommunality[i]);
//			m_InitCommunality[i]	=	  m_VectPreCommunality[i];
		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_VectPreCommunality[i] > max)
			{
				max = m_VectPreCommunality[i];
			}
		}
		e = max;
		
		if(m_nIteration == 1)
		{//第一次循环
			fPreE = e;
		}
		else if(e-fPreE/*e减前一个e*/ > 0)
		{//不能收敛
			bZoomIn = false;
		}
	}
	//TraceVector(m_InitCommunality);
	//TraceVector(m_VectCommunality);
	//TraceVector(m_VectSelEigenValue);
	//TraceVector(m_VectEigenValue);
	//输出循环次数
	//输出因子载荷矩阵
	//delete [] pR;
	Vect_R.destruction();
	//{{{START 2003-2-11 Eric
	
	if(!bZoomIn)
	{//不能收敛.
		m_nIteration = -1;
	}
	//}}}END  2003-2-11 Eric
	//pR = NULL;
	CreateEigenValueTable();	
}


//////////////////////////////////////////////////////////////////////////////////////////
//	处理特征向量矩阵
//		pData: 指向特征值向量矩阵
//		(Eric:似乎没用到)nIndex == 0 求符合要求的特征向量
//		(Eric:似乎没用到)nIndex == 1 不需要判断特征值是否大于判别参数，循环时用
//////////////////////////////////////////////////////////////////////////////////////////
void CFactor::ProcessEigenVector(CDoubleVector Vect_Data,int nIndex)
{
	CIntVector Vect_Index(m_nCol);
	CDoubleVector Vect_pE(m_nCol);

	//提取特征值
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		Vect_pE[i] = Vect_Data[i*m_nCol+i];
	}
//	//TraceVector(Vect_pE);

	//对特征值从大到小处理,排序后放入m_pEigenValue中;记录从大到小的列号放入pIndex，
	int j=0;
	while(j<m_nCol)
	{
		//{{{START 2003-2-9 Eric
		double max = -1e100;
		//}}}END  2003-2-9 Eric
		int nIndex = 0;
		for(i=0;i<m_nCol;i++)
		{
			if( Vect_pE[i] > max )
			{
				max = Vect_pE[i];
				nIndex = i;
			}
		}
		Vect_Index[j] = nIndex;
		j++;
		Vect_pE[nIndex] = -1;
	}
	//TraceVector(Vect_Index);

	m_VectSelEigenValue.create(m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		m_VectSelEigenValue[i] = Vect_Data[Vect_Index[i]*m_nCol+Vect_Index[i]];//存储符合要求的特征值
	}
	//TraceVector(m_VectSelEigenValue);
	//对符合要求的特征值进行计算
	//提取特征向量

	m_VectComMatrix.create( m_nCol * m_nSel);
	int k = 0;
	//TraceVector(m_EigenVector);/////////////////////ggggggggggggggg
	for(i=0;i<m_nSel;i++)
	{
		//遍历所有的特征值，判断是否符合要求
		int ntemp = Vect_Index[i];
		//		if( ( pData[ntemp*m_nCol+ntemp] >= m_dExtact && nIndex==0)||nIndex==1 )
		{
			//如果符合要求就将那一列数据赋值给m_pComMatrix的对应列
			for(int j=0;j<m_nCol;j++)
			{
				assert( j*m_nSel+k < m_nCol * m_nSel && j*m_nCol+ntemp < m_nCol*m_nCol);
				//m_VectComMatrix[j*m_nSel+k] = m_EigenVector[j*m_nCol+ntemp];
				m_VectComMatrix[j+k*m_nCol] = m_EigenVector[j*m_nCol+ntemp];
				//TRACE("%.4f " , m_EigenVector[j*m_nCol+ntemp]);
			}
			k++;
		}
	}
	//TraceVector(m_VectComMatrix);

 	//{{{START 2003-1-28 Eric
	//计算由选中的特征向量组成的矩阵，主成分分析中要用．
	m_SelEigenVector.create(m_nCol * m_nSel);
	m_SelEigenVector = copy(m_VectComMatrix);
	//}}}END  2003-1-28 Eric

	//求解主成分矩阵: //
	for(i=0;i<m_nSel;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			m_VectComMatrix[i*m_nCol+j] *= sqrt(fabs(m_VectSelEigenValue[i])); //pData[pIndex[j]*m_nCol+pIndex[j]]);
		}
	}

	m_VectUnAdjustCom .create(m_nCol * m_nSel);
	m_VectUnAdjustCom = copy(m_VectComMatrix);


	//求解共性方差矩阵
	//TraceVector(m_VectCommunality);
	if(m_VectCommunality.vlen())
	{

		m_VectPreCommunality.create(m_nCol);
		m_VectPreCommunality = copy(m_VectCommunality);
		
	}	
	m_VectCommunality .create(m_nCol);
	double sum(0);
	//TraceVector(m_VectComMatrix);
	for(i=0;i<m_nCol;i++)
	{
		sum = 0 ;
		for(int j=0;j<m_nSel;j++)
		{
			sum += m_VectComMatrix[i+j*m_nCol]*m_VectComMatrix[i+j*m_nCol];
		}
		m_VectCommunality[i] = sum;
	}
	m_VectEigenValue = GetOratedExplains(VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel));
}






////////////////////////////////////////////////////////////////////////////////////////////////
//
//	求平均值
//		nIndex ＝＝ 0 按行求平均值
//		nIndex ＝＝ 1 按列求平均值
//
////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Bartlett巴特莱特因子得分法,算法见文档《Bartlett巴特莱特因子得分法》
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreBartlett()
{
	return Score(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	矩阵相乘，c = a 乘于 b; 
//		a : m*n
//		b : n*k
//		c : m*k
////////////////////////////////////////////////////////////////////////////////////////////////
void CFactor::MatrixMultiply(CDoubleVector a, CDoubleVector b, int m, int n, int k, CDoubleVector& c)
{
	//assert(a&&b&&c);
//	int i,j,l,u;
//    for (i=0; i<=m-1; i++)
//	{
//		for (j=0; j<=k-1; j++)
//		{ 
//			u=i*k+j; c[u]=0.0;
//			for (l=0; l<=n-1; l++)
//			{
//				c[u]=c[u]+a[i*n+l]*b[l*k+j];
//			}
//		}
//	}
//    return;

	int i,j,l,u;
    for (i=0; i<=k-1; i++)
	{
		for (j=0; j<=m-1; j++)
		{ 
			u=i*m+j; c[u]=0.0;
			for (l=0; l<=n-1; l++)
			{
				c[u]=c[u]+a[j+l*m]*b[l+i*n];
			}
		}
	}
    return;
}


////////////////////////////////////////////////////////////////////////////////////
//	Thompsom因子得分法，算法见文档   <<Thompsom汤普森因子得分法>>
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreThompsom()
{
	return Score(1);
}

////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//
//	因子得分法(算法见文档) 
//	　nIndex == 0 :Bartlett巴特莱特因子得分法
//	　nIndex == 1 :Thompsom因子得分法								
//
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::Score(int nIndex)
{
	int i,j;
	CDoubleVector Vect_Matrix = NULL;
	//1.处理因子载荷矩阵
	
	if(m_VectComMatrix.vlen() && m_VectCorrelation.vlen())
	{
		Vect_Matrix= copy(m_VectComMatrix);
	}
	else 
		return false;
	//WriteData(Vect_Matrix,m_nCol,m_nSel,"旋转前的因子载荷矩阵.");
	////TraceVector(m_VectComMatrix);
	////TraceVector(m_VectCorrelation);
	//如果因子旋转的话,就用旋转后的因子载荷矩阵
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() )
	{
		Vect_Matrix= copy(m_VectRotateCom);
		//{{{START 2003-2-17 Eric
		//WriteData(Vect_Matrix,m_nCol,m_nSel,"旋转后的因子载荷矩阵.");
		
		for(i=0;i<m_nCol;i++)
		{
			for(j=0;j<m_nSel;j++)
			{
				Vect_Matrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
			}
		}
		//}}}END  2003-2-17 Eric
	}
	
	//double * pRAdjuest = NULL;//计算W的一部分
	CDoubleVector Vect_RAdjuest(m_nCol*m_nCol);
	double	 sum(0);
	if( !Vect_RAdjuest.vlen() )
	{
		return false;
	}
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			sum = 0;
			for(int k=0;k<m_nSel;k++)
			{
				sum += Vect_Matrix[i+k*m_nCol] * Vect_Matrix[j+k*m_nCol];
			}
			Vect_RAdjuest[i*m_nCol+j] = sum;			
		}
	}

	if(m_bCovarian)
	{
		CDoubleVector Vect_OriDataCorr =
			RelationMatrixEx(m_VectInputData, m_nRow,m_nCol,m_weight,0);
		
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				Vect_RAdjuest[i*m_nCol+j] = Vect_OriDataCorr[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
			}
		}
	}
	else
	{
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				Vect_RAdjuest[i*m_nCol+j] = m_VectCorrelation[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
			}
		}
	}
	//3.计算W
	CDoubleMatrix AD = VectorToMatrix(Vect_RAdjuest,m_nCol,m_nCol);
	double det;
	Matinv1(AD,m_nCol,det);
	Vect_RAdjuest=MatrixToVector(AD);
	CDoubleVector Vect_ATran = MatrixTran(Vect_Matrix,m_nCol,m_nSel);         //共线性矩阵的转置矩阵
	CDoubleVector Vect_Temp(m_nSel * m_nCol);
	MatrixMultiply(Vect_ATran,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp);
	CDoubleVector Vect_Temp2(m_nSel * m_nSel);
	MatrixMultiply(Vect_Temp,Vect_Matrix,m_nSel,m_nCol,m_nSel,Vect_Temp2);
	switch(nIndex)
	{  	
	case 0://Bartlett巴特莱特因子得分法 
		break;
	case 1://Thompsom因子得分法			
		for(i=0;i<m_nSel;i++)
		{
			for(int j=0;j<m_nSel;j++)
			{
				Vect_Temp2[i*m_nSel+j] += 1;
			}
		}
		break;
	case 2:
		break;
	}
	
	if( !brinv(Vect_Temp2,m_nSel) )
	{
		return false;
	}
	
	Vect_Temp.create(m_nSel*m_nCol);
	MatrixMultiply(Vect_Temp2,Vect_ATran,m_nSel,m_nSel,m_nCol,Vect_Temp);
	Vect_Temp2.create(m_nSel*m_nCol);
	MatrixMultiply(Vect_Temp,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp2);
	//4.计算得分
	if( m_nSel != 0 )
	{
		Vect_Temp.create(m_nRow * m_nSel);
		Vect_ATran = MatrixTran(Vect_Temp2,m_nSel,m_nCol);
		MatrixMultiply(m_VectDataFormlized,Vect_ATran,m_nRow,m_nCol,m_nSel,Vect_Temp);
		m_VectScore = copy(Vect_Temp);
	}
	

	//如果因子旋转的话,就用旋转后的因子载荷矩阵
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() )
	{
		Vect_Matrix= copy(m_VectRotateCom);
		for(i=0;i<m_nCol;i++)
		{
			for(j=0;j<m_nSel;j++)
			{
				Vect_Matrix[i*m_nSel+j] *= 1/sqrt(fabs(m_VectCorrelation[i*m_nCol+i])) ;
			}
		}
	
		CDoubleVector Vect_RAdjuest(m_nCol*m_nCol);
		double	 sum(0);
		if( !Vect_RAdjuest.vlen() )
		{
			return false;
		}
		for(i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nCol;j++)
			{
				sum = 0;
				for(int k=0;k<m_nSel;k++)
				{
					sum += Vect_Matrix[i+k*m_nCol] * Vect_Matrix[j+k*m_nCol];
				}
				Vect_RAdjuest[i*m_nCol+j] = sum;			
			}
		}
		//2.
		
		if(m_bCovarian)
		{
			CDoubleVector Vect_OriDataCorr =
				RelationMatrixEx(m_VectInputData, m_nRow,m_nCol,m_weight,0);
			
			for(i=0;i<m_nCol;i++)
			{
				for(int j=0;j<m_nCol;j++)
				{
					Vect_RAdjuest[i*m_nCol+j] = Vect_OriDataCorr[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
				}
			}
		}
		else
		{
			for(i=0;i<m_nCol;i++)
			{
				for(int j=0;j<m_nCol;j++)
				{
					Vect_RAdjuest[i*m_nCol+j] = m_VectCorrelation[i*m_nCol+j] - Vect_RAdjuest[i*m_nCol+j];
				}
			}
		}
		//3.计算W
		CDoubleMatrix AD = VectorToMatrix(Vect_RAdjuest,m_nCol,m_nCol);
		double det;
		Matinv1(AD,m_nCol,det);
		Vect_RAdjuest=MatrixToVector(AD);
		CDoubleVector Vect_ATran = MatrixTran(Vect_Matrix,m_nCol,m_nSel);         //共线性矩阵的转置矩阵
		CDoubleVector Vect_Temp(m_nSel * m_nCol);
		MatrixMultiply(Vect_ATran,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp);
		CDoubleVector Vect_Temp2(m_nSel * m_nSel);
		MatrixMultiply(Vect_Temp,Vect_Matrix,m_nSel,m_nCol,m_nSel,Vect_Temp2);
		switch(nIndex)
		{  	
		case 0://Bartlett巴特莱特因子得分法 
			break;
		case 1://Thompsom因子得分法			
			for(i=0;i<m_nSel;i++)
			{
				for(int j=0;j<m_nSel;j++)
				{
					Vect_Temp2[i*m_nSel+j] += 1;
				}
			}
			break;
		case 2:
			break;
		}
		
		if( !brinv(Vect_Temp2,m_nSel) )
		{
			return false;
		}
		
		Vect_Temp.create(m_nSel*m_nCol);
		MatrixMultiply(Vect_Temp2,Vect_ATran,m_nSel,m_nSel,m_nCol,Vect_Temp);
		Vect_Temp2.create(m_nSel*m_nCol);
		MatrixMultiply(Vect_Temp,Vect_RAdjuest,m_nSel,m_nCol,m_nCol,Vect_Temp2);
		//4.计算得分
		if( m_nSel != 0 )
		{
			Vect_Temp.create(m_nRow * m_nSel);
			Vect_ATran = MatrixTran(Vect_Temp2,m_nSel,m_nCol);
			MatrixMultiply(m_VectDataFormlized,Vect_ATran,m_nRow,m_nCol,m_nSel,Vect_Temp);
			m_VectRotateScore = copy(Vect_Temp);
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////
//	因子得分之Regression方法
///////////////////////////////////////////////////////////////////////////////////
bool CFactor::ScoreRegression()
{
	//判断是否需要计算因子得分
	if( !NeedScoreRegression() )
		return false;
	CDoubleVector Vect_W(m_nCol*m_nSel);
	//pW = new double [m_nCol*m_nSel];
	//主成分法，不旋转
	if( m_bCompenFlag && !m_bRotateFlag )
	{
		for(int i=0;i<m_nCol;i++)
		{
			for(int j=0;j<m_nSel;j++)
			{
				Vect_W[i*m_nSel+j] = m_VectComMatrix[i*m_nSel] * (1 / m_VectSelEigenValue[j] ) ;
			}
		}
	}
	//主成分，旋转
	else if( m_bCompenFlag && m_bRotateFlag )
	{
		CDoubleVector Vect_ATran = MatrixTran(m_VectComMatrix,m_nCol,m_nSel);
		CDoubleVector Vect_Temp(m_nSel*m_nSel);
		MatrixMultiply(Vect_ATran,m_VectComMatrix,m_nSel,m_nCol,m_nSel,Vect_Temp);
		if( !brinv( Vect_Temp,m_nSel ) )
		{
			return false;

		}
		MatrixMultiply(m_VectComMatrix,Vect_Temp,m_nCol,m_nSel,m_nSel,Vect_W);
	}
	else//其他方法
	{
		if( !brinv(m_VectCorrelation,m_nCol))
		{
			//Vect_W.destruction();
			return false;
		}
		if( m_bZhengjiao ) //正交方法
		{
			MatrixMultiply(m_VectCorrelation,m_VectComMatrix,m_nCol,m_nCol,m_nSel,Vect_W);
		}
		else
		{
			//先求解因子载荷矩阵的相关系数矩阵
			Formalize(m_VectComMatrix,m_nCol,m_nSel);
			CDoubleVector RelationMatrix = RelationMatrixEx(m_VectComMatrix,m_nCol,m_nSel,m_weight,0);
			
			CDoubleVector Vect_S (m_nCol*m_nSel) ; 
			MatrixMultiply(m_VectComMatrix,RelationMatrix,m_nCol,m_nSel,m_nSel,Vect_S);
			MatrixMultiply(m_VectCorrelation,Vect_S,m_nCol,m_nCol,m_nSel,Vect_W);
		}
	}
	CDoubleVector Vect_F(m_nRow*m_nSel);
	MatrixMultiply(m_VectData,Vect_W,m_nRow,m_nCol,m_nSel,Vect_F);
	m_VectRotateScore = copy(Vect_F);
	////TraceVector(m_VectScore);
	return true;
}



bool CFactor::NeedScoreRegression()
{
	bool bFlag = false;
	//在不旋转因子的主成分分析法（PC）中，有任何一个特征值（根） < 1e-8
	if( m_bCompenFlag && !m_bRotateFlag && m_bEigenFlag )
	{
		return false;		
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//	正交因子旋转，算法可见文档：《正交旋转.doc》						//
//		matlab 源代码可见：orthorth2.m									//
//      本过程输入因子载荷矩阵，输出旋转后的因子载荷矩阵				//
//////////////////////////////////////////////////////////////////////////
bool CFactor::Orth(int nMethod)
{	
	assert( m_VectComMatrix.vlen()>0 );
	//--------------2、赋初值----------------------------//
	int s = 1;
	CDoubleMatrix matrix_AA = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
	CDoubleMatrix matrix_A = matrix_AA;
	CDoubleMatrix matrix_A0;
	matrix_A0 = diag(1.0/sqrt(sum(prod(matrix_AA,matrix_AA))))*matrix_AA;
	double delta(1);
	
	//--------------3、因子旋转--------------------------//
	int m = m_nSel;
	double SV_Pre(0),SV_Cur(0);
	int MAXITERATION=300;
	int iteration=0;
	while( delta > 1e-5 &&iteration<MAXITERATION)
	{
		iteration++;
		int l=0;
		for(l=0;l<m-1;l++)
		{
			for(int k=l+1;k<m;k++)
			{
				//计算旋转角度
				double p = Angle(matrix_A0,l,k,nMethod);
				//旋转
				if( fabs(sin(p)) > 1e-8 )
				{
					CDoubleMatrix matrix_temp1(m_nCol,2);
					matrix_temp1 = matrix_A.col(l)|matrix_A.col(k);				
					
					CDoubleMatrix matrix_temp2 = GetRotationMatrix(p);		
					CDoubleMatrix matrix_temp3 = matrix_temp1 * matrix_temp2;
					//TraceMatrix(matrix_temp3);
					for( int i=0;i<m_nCol;i++)
					{
						matrix_A(i,l) = matrix_temp3(i,0);
						matrix_A(i,k) = matrix_temp3(i,1);
					}				
				}
			}
		}
		TraceMatrix(matrix_A);
		matrix_A0 = diag(1.0/sqrt(sum(prod(matrix_A,matrix_A)))) * matrix_A;
		SV_Cur = sv2(matrix_A0);
		if( s > 1 )
		{
			delta = fabs(SV_Cur - SV_Pre);
		}
		SV_Pre = SV_Cur;
		s++;		
	}	
	m_resortMatrix = matrix_A;
	m_VectRotEigenValue = GetOratedExplains(matrix_A);
	return true;
}
//////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//  计算因子载荷矩阵的各列方差和		
///////////////////////////////////////////////////////////////////////////////////////
double CFactor::sv2(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	double sv(0);
	for(int j=0;j<m;j++)
	{
		double temp1 = 0;
		double temp2 = 0;
		for(int k=0;k<p;k++)
		{
			temp1 += pow(matrix_A(k,j),4);
			temp2 += pow(matrix_A(k,j),2);
		}
		temp1 *= p;
		temp2 = temp2*temp2;
		sv += temp1 - temp2;
	}
	return sv/(p*p);
}


//////////////////////////////////////////////////////////////////////////////////////
//
//	计算旋转角度
//  　matrix_f因子载荷矩阵,l,m: 需要旋转的列号，
//	　nMethod :方法  1:varimax,2:Equimax,3:Quartimax
//
//////////////////////////////////////////////////////////////////////////////////////
double CFactor::Angle(CDoubleMatrix matrix_f, int l, int k, int nMethod)
{
	int p = matrix_f.mrow();
	int m = matrix_f.mcol();
	
	CDoubleVector vector_u;
	CDoubleVector vector_v;
	
	vector_u = col(matrix_f,l)*col(matrix_f,l) - col(matrix_f,k)*col(matrix_f,k);
	vector_v = 2.0*col(matrix_f,l)*col(matrix_f,k);
	
	double  d_A = sum(vector_u);
	double  d_B = sum(vector_v);
	double  d_C = sum(vector_u*vector_u-vector_v*vector_v);
	double  d_D = sum(2.0*vector_u*vector_v);
	
	double d_X(0);
	double d_Y(0);
	
	switch( nMethod )
	{
	case 1:
		d_X = d_D - 2*d_A*d_B/p;
		d_Y = d_C - (d_A*d_A - d_B*d_B)/p;
		break;
	case 2:
		//{{{START 2003-2-8 Eric
		//d_X = d_D - 2*d_A*d_B/p;
		//d_Y = d_C - (d_A*d_A - d_B*d_B)/(2*p);
		d_X = d_D - m*d_A*d_B/p;
		d_Y = d_C - m*(d_A*d_A - d_B*d_B)/(2*p);
		//}}}END  2003-2-8 Eric
		break;
	case 3:
		d_X = d_D;
		d_Y = d_C;
		break;
	}
	if( d_X >=0  && d_Y >=0 )
	{
		return atan(d_X/d_Y)/4.0;
	}
	if( d_X >= 0 && d_Y < 0 )
	{
		return (atan(d_X/d_Y) + pi)/4.0;
	}
	if( d_X < 0 && d_Y <= 0 )
	{
		return (atan(d_X/d_Y) - pi)/4.0;
	}
	if( d_X < 0 && d_Y > 0 )
	{
		return 	(atan(d_X/d_Y))/4.0;
	}
	return 0;
}



CDoubleMatrix CFactor::GetRotationMatrix(double angle)
{
	CDoubleMatrix matrix(2,2);
	matrix(0,0) =  cos(angle);
	matrix(0,1) = -sin(angle);
	matrix(1,0) =  sin(angle);
	matrix(1,1) =  cos(angle);
	return matrix;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	proman方法：斜交旋转法
//	　输入为已经过方差极大正交旋转（varimax）后，得到的正交因子载荷矩阵matrix_A，然后
//	　再用以下方法进行计算最后得到斜交因子相关阵L、斜交因子结构阵S、斜交因子载荷阵Af
//	　输入变量：经过varimax旋转得到的因子载荷矩阵，用户输入的参数kappa值（kappa>=2）
//	　前面需检验kappa范围		
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::Proman(CDoubleMatrix matrix_A, double dbKappa)
{
	assert( dbKappa > 1);
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	if( p>0 && m>0 )
	{
	}
	else
		return false;
	
	//----------将A按行规格化------------
	CDoubleMatrix matrix_A1 = FormalizeWithRow(matrix_A);
	
	//-----------计算h(i,j)--------------
	CDoubleMatrix matrix_H(p,m);
	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			if( matrix_A1(i,j) >= 0 )
			{
				matrix_H(i,j) =  pow( fabs( matrix_A1(i,j) ),dbKappa);
			}
			else
			{
				matrix_H(i,j) = -pow( fabs( matrix_A1(i,j) ),dbKappa );
			}
		}
	}
	
	//--------------计算G----------------
	CDoubleMatrix matrix_G;
	matrix_G = pinv( (CDoubleMatrix)( trans(matrix_A1) * matrix_A1)  ) * trans(matrix_A1) * matrix_H;
	
	//将G按列规格化，得到斜交参考阵lamta
	CDoubleMatrix matrix_lamta = FormalizeWithCol(matrix_G);
	
	//将lamta按行规格化
	CDoubleMatrix matrix_ilamta = pinv( matrix_lamta );
	CDoubleMatrix matrix_Tp = FormalizeWithRow(matrix_ilamta);
	
	//计算斜交变换阵T
	CDoubleMatrix matrix_T = trans(matrix_Tp);
	
	//计算斜交因子相关阵L、斜交因子结构阵S、斜交因子载荷阵Af
	CDoubleMatrix matrix_L  = matrix_Tp * matrix_T;
	CDoubleMatrix matrix_Af = matrix_A  * pinv( matrix_Tp );
	CDoubleMatrix matrix_S  = matrix_A  * matrix_T;
	MatrixToPointer(matrix_L,&m_pL);
	MatrixToPointer(matrix_Af,&m_pAf);
	MatrixToPointer(matrix_S,&m_pS);
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//	对输入的矩阵进行按行的规格化，返回规格化后的矩阵
/////////////////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CFactor::FormalizeWithRow(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	CDoubleMatrix matrix_B(p,m);
	CDoubleMatrix matrix_temp = prod(matrix_A,matrix_A);
	CDoubleVector vector_temp = sum( matrix_temp );
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_B(i,j) = matrix_A(i,j) / sqrt(fabs(vector_temp[i]));
		}
	}	
	return matrix_B;
}

/////////////////////////////////////////////////////////////////////////////////////////
//	对输入的矩阵进行按列的规格化，返回规格化后的矩阵
/////////////////////////////////////////////////////////////////////////////////////////
CDoubleMatrix CFactor::FormalizeWithCol(CDoubleMatrix matrix_A)
{
	int p = matrix_A.mrow();
	int m = matrix_A.mcol();
	CDoubleMatrix matrix_B(p,m);
	CDoubleMatrix matrix_temp = prod(matrix_A,matrix_A);
	CDoubleVector vector_temp = sum( trans(matrix_temp)  );
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_B(i,j) = matrix_A(i,j)/sqrt(fabs(vector_temp[j]));
		}
	}
	return matrix_B;
}


//////////////////////////////////////////////////////////////////////////////////////////
//	因子提取方法 alpha2方法  m ; 提取得因子个数，nCountMax ：循环的最大次数
//////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::FactorAlpha2(CDoubleMatrix matrix_Z,int m, int nCountMax)
{
	bool bFlag(false);
	int n = matrix_Z.mrow();
	int p = matrix_Z.mcol();
	//第二步：计算相关系数矩阵及其特征根和特征向量
	CSymDbMatrix  matrix_R = RelationMatrixEx(matrix_Z);
	CDoubleMatrix matrix_EigVector;
	CSymDbMatrix  matrix_EigValue = eig(matrix_R,matrix_EigVector);
	//循环求解
	CDoubleMatrix matrix_r = pinv((CDoubleMatrix)matrix_R);
	int ncount = 1;
	CDoubleMatrix matrix_H0 = diag( diag(1.0 - 1.0/matrix_r) );
	CComplexMatrix matrix_A;
	while( ncount < nCountMax )
	{
		CDoubleMatrix matrix_U2   = eye(p) - matrix_H0;
		CSymDbMatrix  matrix_temp = diag(1.0/sqrt(diag(matrix_H0))) * ( (CDoubleMatrix)matrix_R - matrix_U2) * diag(1.0/sqrt(diag(matrix_H0)));
		CDoubleMatrix matrix_tempEigVector;
		CDoubleMatrix matrix_tempEigValue = eig(matrix_temp,matrix_tempEigVector);
		
		//将特征根从大到小排列，并相应地调整特征向量
		SortEigenVect(matrix_tempEigValue,matrix_tempEigVector,1);
		//根据因子个数提取特征根和特征向量
		CComplexMatrix matrix_lamta3 = eye(m);
		int i=0;
		for(i=0;i<m;i++)
		{
			matrix_lamta3(i,i) = matrix_tempEigValue(i,i);
		}
		CDoubleMatrix matrix_l3(p,m);
		matrix_l3 = slice(matrix_tempEigVector,p,m);
		
		//计算因子载荷矩阵A
		matrix_A = ((CComplexMatrix)sqrt(matrix_H0)) * (CComplexMatrix)matrix_l3 * diag(diag(matrix_lamta3));
		
		//计算共性方差
		CDoubleMatrix matrix_H1 = real( (CComplexMatrix)diag( diag( matrix_A * trans(matrix_A) )));
		
		//判断循环的终止条件
		CDoubleMatrix matrix_abs = matrix_H1 - matrix_H0;
		double max = -1e10;
		for(i=0;i<matrix_abs.mrow();i++)
		{
			for(int j=0;j<matrix_abs.mcol();j++)
			{
				matrix_abs(i,j) = fabs(matrix_abs(i,j));
				if( matrix_abs(i,j) > max )
				{
					max = matrix_abs(i,j);
				}
			}
		}
		if( max < 10e-5 )
		{
			bFlag = true;
			break;
		}
		matrix_H0 = copy(matrix_H1);
		ncount++;
	}
	m_VectComMatrix = MatrixToVector(matrix_A.real());//////////////////////////////////////////////////////////////////////////
	return bFlag;
}


////////////////////////////////////////////////////////////////////////////////////////////
//	因子分析提取之Image（镜像）因子提取法，算法可见文档《Image（镜像）因子提取》
//		本段代码直接翻译自matlab源代码，
//		参数：nIndex == 0为相关方差矩阵法
//            nIndex == 1为协系数矩阵法
////////////////////////////////////////////////////////////////////////////////////////////
bool CFactor::FactImage(CDoubleMatrix matrix_z, int nIndex)
{
	int i,j;
	
	if(nIndex == 1)
	{
		m_bCovarian = true;
	}
	else
	{
		m_bCovarian = false;
	}
	
	CreateEig(nIndex);
	int p = matrix_z.mcol();
	int n = matrix_z.mrow();
	//求解相关系数矩阵 或协方差矩阵.
	CDoubleMatrix matrix_R2 = RelationMatrixEx(matrix_z,nIndex);
	TraceMatrix(matrix_R2);
	//这是本算法第一次求解协方差.
	if(nIndex == 1)
	{//协方差
		m_VectFirstCovariance.create(m_nCol*m_nCol);
		m_VectFirstCovariance = MatrixToVector( matrix_R2);//////////////////////////////////////////////////////////////////////////
		
	}
	
	
	CDoubleMatrix matrix_eigvector;
	//求解特征值和特征向量
	CDoubleMatrix matrix_eigvalue ;//= eig((CSymDbMatrix)matrix_R2,matrix_eigvector);
	CDoubleVector Vect_Cov = MatrixToVector(matrix_R2);
	FromCorrelationToCommunality(Vect_Cov);//求解初次的共性方差
 	CDoubleVector Vect_EigenVector(m_nCol*m_nCol);
	Eigenvalue(Vect_Cov,Vect_EigenVector);//求解特征向量m_pEigenVector

	//从小到大排序
	matrix_eigvalue = VectorToMatrix(Vect_Cov,m_nCol,m_nCol);
	m_VectSelEigenValue.create(m_nCol);
	for(i=0;i<m_nCol;i++)
		m_VectSelEigenValue[i]=matrix_eigvalue(i,i);
	CDoubleVector vect = m_VectSelEigenValue;
	CreateChartResult(vect.Sort(),m_nCol);

	matrix_eigvector= trans(VectorToMatrix(Vect_EigenVector,m_nCol,m_nCol));
	
	double sum(0);
	CDoubleVector vector_llamtavalue(p);
	for( i=0;i<p;i++)
	{
		vector_llamtavalue(i) = matrix_eigvalue(i,i);
		sum += matrix_eigvalue(i,i);
	}
	double proportion(0);
	//TraceVector(vector_llamtavalue);
	//选入的因子个数
	//{{{START 2003-2-19 Eric
	//由于不清楚以上代码的逻辑,所以重新计算m_nSel;
	if(m_nExtractStandard == 0)
	{
		proportion = 0;
		i = 0;
		while (proportion < m_fPro)
		{
			proportion += vector_llamtavalue[i] / sum;
			i++;
		}
	}
	
	//}}}END  2003-2-19 Eric
	
	int m = i;
	m_nSel = m;
	CDoubleMatrix matrix_lamtavalue = eye(m,m);
	int k=0;
	for(k=0;k<m;k++)
	{
		matrix_lamtavalue(k,k) = vector_llamtavalue[k];
	}
	TraceMatrix(matrix_lamtavalue);
	TraceMatrix(matrix_R2);
	CDoubleMatrix matrix_r  = pinv((CDoubleMatrix)matrix_R2);
	TraceMatrix(matrix_r);
	CDoubleMatrix matrix_s2 = eye(p,p);
	for(i=0;i<p;i++)
	{
		matrix_s2(i,i) = 1.0/matrix_r(i,i);
	}
	CDoubleMatrix matrix_s    = diag( sqrt( diag( matrix_s2 )));
	TraceMatrix(matrix_s);
	CDoubleMatrix matrix_temp = pinv( matrix_s );
	TraceMatrix(matrix_temp);
	CSymDbMatrix matrix_srs  = matrix_temp * (CDoubleMatrix)matrix_R2 * matrix_temp;
	TraceMatrix((CDoubleMatrix)matrix_srs);
	//求解eig
	Vect_Cov = MatrixToVector((CDoubleMatrix)matrix_srs);
 	Eigenvalue(Vect_Cov,Vect_EigenVector);//求解特征向量m_pEigenVector

	//TraceVector(Vect_Cov);
	//TraceVector(Vect_EigenVector);
	TraceMatrix(matrix_eigvalue);
	//从小到大排序
	matrix_eigvalue = VectorToMatrix(Vect_Cov,p,p);
	matrix_eigvector= trans(VectorToMatrix(Vect_EigenVector,p,p));
	
	//排序，从大到小
	//SortEigenVect(matrix_eigvalue,matrix_eigvector,1);
	//TraceMatrix(matrix_eigvalue);
	//TraceMatrix(matrix_eigvector);
	
	
	matrix_lamtavalue = eye(m);
	for(i=0;i<m;i++)
	{
		matrix_lamtavalue(i,i) = matrix_eigvalue(i,i);
	}
	TraceMatrix(matrix_lamtavalue);
	CDoubleMatrix matrix_lvalue(p,m);
	matrix_lvalue = slice(matrix_eigvector,p,m);
	TraceMatrix(matrix_lvalue);
	CDoubleMatrix matrix_f = matrix_s * matrix_lvalue * ( matrix_lamtavalue - eye(m,m) ) * diag(1.0/sqrt( diag (matrix_lamtavalue)));
    TraceMatrix(matrix_f);

	m_VectEigenValue = GetOratedExplains(matrix_f);

	m_VectComMatrix = MatrixToVector(matrix_f);
	ProcessComMatrix(m_VectComMatrix,matrix_f.mrow(),matrix_f.mcol());

	m_InitCommunality.create(m_nCol);
	m_InitCommunality = copy(m_VectCommunality);
	m_VectCommunality.create(p);
	for(i=0;i<p;i++)
	{
		m_VectCommunality[i] = 0;
		for(j=0;j<m;j++)
		{
			m_VectCommunality[i] += ( matrix_lamtavalue(j,j) - 1 )*( matrix_lamtavalue(j,j) - 1 ) * matrix_lvalue(i,j) * matrix_lvalue(i,j) / ( matrix_lamtavalue(j,j) * matrix_r(i,i));
		}
	}
	//TraceVector(m_VectCommunality);
	CDoubleMatrix matrix_icm  = (CDoubleMatrix)matrix_R2 + (matrix_s2 * pinv( (CDoubleMatrix)matrix_R2 ) * matrix_s2) - ( 2.0 * matrix_s2 );
	CDoubleMatrix matrix_aicm = matrix_s2 * pinv( (CDoubleMatrix)matrix_R2 ) * matrix_s2;
	CDoubleMatrix matrix_diagR= eye(p,p);
	for(i=0;i<p;i++)
	{
		matrix_diagR(i,i) = matrix_R2(i,i);
	}
	CDoubleMatrix matrix_fr = matrix_diagR * matrix_f; 
	TraceMatrix(matrix_fr);
	return true;
	
}

bool CFactor::GetValueFromVOFac(CDataInterface *pDataInterface, std::string VO)
{
//	FACTOR
//		/DATA=[FILE]
//		/VARLIST=[varlist]
//		/MISSING=[{NONE**}]
//		/EXTRACTION=[{PCOMP**},{PFACTOR},{IMAGE},{ML},{GLS},{UNLS}]
//		/METHOD=[{CORR**},{COV}]
//		/RULE=[{CUMULANT(0.9**)},{NUMFACTOR(1**)}]
//		/SCALE [{BARTLETT**},{THOMPSON}]
//		/ROTATION [{NONE**},{VARIMAX},{EQUAMAX},{QUARTIMAX}]
//		/CRITERIA [MINEIGEN({1**})] [ITERATE({30**})]
//		/RESULT [CORRRESD] [REPR] [ANTICORR] [ANTICOV] [KMO]
//		/PRINT [FACTORS]

	AnaWord VOstr;
	VOstr.Import(VO);
	CTStringArray xname;
	AnaSyntax::getVAR("/VARLIST", VOstr, xname);
	if (!(pDataInterface->getVarID(xname,m_tVarInfo)))
		return FALSE;
	
	m_nMissingMethod = 0;

	if (AnaSyntax::findstr("/EXTRACTION", "PFACTOR",VOstr))
		m_nExtractMethod = 1;
	else if (AnaSyntax::findstr("/EXTRACTION", "IMAGE",VOstr))
		m_nExtractMethod = 2;
	else if (AnaSyntax::findstr("/EXTRACTION", "ML",VOstr))
		m_nExtractMethod = 3;
	else if (AnaSyntax::findstr("/EXTRACTION", "GLS",VOstr))
		m_nExtractMethod = 4;
	else if (AnaSyntax::findstr("/EXTRACTION", "UNLS",VOstr))
		m_nExtractMethod = 5;
	else
		m_nExtractMethod = 0;

	m_nAnaMethod = int(AnaSyntax::findstr("/METHOD", "COV",VOstr));

	m_fPro = 0.9;
	m_nExtractStandard = int(AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr));
	if (AnaSyntax::findstr("/RULE", "CUMULANT",VOstr))
		m_fPro = AnaSyntax::getvalue("/RULE", "CUMULANT",VOstr);
	m_nFactorNum = 1;
	if (AnaSyntax::findstr("/RULE", "NUMFACTOR",VOstr))
		m_nFactorNum = AnaSyntax::getvalue("/RULE", "NUMFACTOR",VOstr);

	m_bScore = AnaSyntax::exist("/SCALE", VOstr);
	m_nScoreMethod = int(AnaSyntax::findstr("/SCALE", "THOMPSON",VOstr));
	
	if (AnaSyntax::findstr("/ROTATION", "VARIMAX",VOstr))
		m_nRotateMethod = 1;
	else if (AnaSyntax::findstr("/ROTATION", "EQUAMAX",VOstr))
		m_nRotateMethod = 2;
	else if (AnaSyntax::findstr("/ROTATION", "QUARTIMAX",VOstr))
		m_nRotateMethod = 3;
	else
		m_nRotateMethod = 0;
			
	m_nIteration = 0;
	if (AnaSyntax::findstr("/CRITERIA", "MINEIGEN",VOstr))
		m_nIteration = AnaSyntax::getvalue("/CRITERIA", "MINEIGEN",VOstr);
	m_nMaxIterCount = 30;
	if (AnaSyntax::findstr("/CRITERIA", "ITERATE",VOstr))
		m_nMaxIterCount = AnaSyntax::getvalue("/CRITERIA", "ITERATE",VOstr);

	m_bShengyu = AnaSyntax::findstr("/RESULT", "CORRRESD",VOstr);
	m_bZaisheng = AnaSyntax::findstr("/RESULT", "REPR",VOstr);
	m_bAntiImage = AnaSyntax::findstr("/RESULT", "ANTICORR",VOstr);
	m_bAntiImage2 = AnaSyntax::findstr("/RESULT", "ANTICOV",VOstr);
	m_bKMO = AnaSyntax::findstr("/RESULT", "KMO",VOstr);
	
	m_bOutput = AnaSyntax::findstr("/PRINT", "FACTORS",VOstr);

	return TRUE;
}

CResult * CFactor::OnFactor(CDataInterface *pDataInterface, std::string VO)
{
	

	m_bPrincipal=false;//zkc 2003.8.20
	m_strResultExplain.RemoveAll();
	m_pOriDataInterface = pDataInterface;
	m_bIsMValued=false;
	m_nProcType = 1;
	m_bComBigOne = false;
	m_pResult = NULL;
	m_pResult = new CResult("因子分析");
	if (!(GetValueFromVOFac(pDataInterface, VO)))
	{
		CTString strWarning = "读取参数发生错误，请重新运行！";
		CRsltElementText * pWarningTextRslt = new CRsltElementText( "错误!" );
		pWarningTextRslt->AddString( strWarning );
		m_pResult->Add( pWarningTextRslt );
		return m_pResult;
	}
	//取数据
	m_VectInputData = GetData(pDataInterface,m_tVarInfo);//同时给m_pDataInterface赋值//取得权重列，如果是无权重，则权重全为１
	if(!m_VectInputData.vlen())
	{
		delete m_pResult;
		m_pResult = NULL;
		OnFactorExit();
		return NULL;
	}
	//处理权重.
	int i = 0;
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight(i)<0)
			m_weight[i] = 0;
	}
	
	int m          = m_tVarInfo.iCount;
	CDoubleMatrix matrix=VectorToMatrix(m_VectInputData,m_nRow,m);
	this->SetRowsCols(m_nRow,m);
	m_VectData = copy(m_VectInputData);
	//取参数
	m_VectDataFormlized = FormlizeEx(m_VectInputData,m_weight);

	double dbKappa          = 4;

	if(m_fPro > 1)
	{
		m_fPro = 1;
	}
	if(m_fPro<0)
	{
		m_fPro = 0;
	}


	//进行相应的计算
	//1：因子提取：		
	switch( m_nExtractMethod)
	{
	case 0:
		m_strExtractMethod = "提取方法：主成分法";
		ComponMethod(m_nAnaMethod);//主成分法
		break;		
	case 1:
		m_strExtractMethod = "提取方法：主因子法";
		ComponMethod(m_nAnaMethod+2);//主因子法
		break;			
	case 2:
		if(m_nRow < 4)
		{
// 			AfxMessageBox("样本数太少,无法进行分析!");
			return NULL;
		}
		m_strExtractMethod = "提取方法：镜像因子法";
		FactImage(matrix,m_nAnaMethod);
		break;
	case 3:
		m_strExtractMethod = "提取方法：极大似然法";
		m_bCovarian = false;
		ml_fact(matrix,0,m_nFactorNum,m_nMaxIterCount,7);
		break;
	case 4:
		m_strExtractMethod = "提取方法：普通最小二乘法";
		m_bCovarian = false;
		ml_fact(matrix,1,m_nFactorNum,m_nMaxIterCount,7);
		break;
	case 5:
		m_strExtractMethod = "提取方法：无权重最小二乘法";
		m_bCovarian = false;
		ml_fact(matrix,2,m_nFactorNum,m_nMaxIterCount,7);
		break;
	default:
		break;

	}
	//2：因子旋转

	switch(m_nRotateMethod)
	{
	case 1:
		m_strRotateMethod = "旋转方法：方差最大旋转";
		Orth(1);//方差最大旋转
		break;
	case 2:
		m_strRotateMethod = "旋转方法：平均正交旋转";
		Orth(2);//平均正交旋转
		break;
	case 3:
		//WriteData(m_pComMatrix,m_nCol,m_nSel,"四次方正交旋转:旋转前的因子载荷矩阵");
		m_strRotateMethod = "旋转方法：四次方正交旋转";
		Orth(3);//四次方正交旋转
		break;
	case 0://不旋转
	default:
		m_resortMatrix = VectorToMatrix(m_VectComMatrix,m_nCol,m_nSel);
		//TraceVector(m_VectComMatrix);
		break;
	}


//{{{START 2003-1-26 Eric
	//3整理因子	
	m_VectRotateCom .create( m_resortMatrix.mrow()*m_resortMatrix.mcol());
	TraceMatrix(m_resortMatrix);
	//TraceVector(m_VectComMatrix);
	ProcessRotateMatrix(m_resortMatrix);
	m_VectRotateCom = MatrixToVector(m_resortMatrix);
//}}}END  2003-1-26 Eric
//{{{START 2003-2-13 Eric
	//对于协方差,要进行调整.
	if(m_bCovarian)
	{
		m_UnAdjustComMatrix.create( m_nCol * m_nSel);
		m_UnAdjustComMatrix = copy(m_VectComMatrix);
		m_VectComMatrix.create(m_nCol * m_nSel);
		switch( m_nExtractMethod)
		{
		case 0:	//主成分
			
			AdjuestResult();

			break;
		case 1:	//主因子法
			break;
		case 2://镜像因子法

			AdjuestResult(); //得分之前要调整.
			break;
		case 3://极大似然法

			break;
		}

	}//}}}END  2003-2-13 Eric

	//4：因子得分
	if( m_bScore )
	{
	switch( m_nScoreMethod )
	{
		case 0:
			m_strScoreMethod = "巴特莱特因子得分法";
			Score(0);//barblate			
			break;
		case 1:
			m_strScoreMethod = "汤姆森因子得分法";
			Score(1);//tomash			
			break;
		case 2:		
			m_strScoreMethod = "回归因子得分法";
			ScoreRegression();//regression
			break;
		default:
			break;
		}
	}
	CreateResult(m_pResult);

//{{{START 2003-2-11 Eric
	if(m_nIteration != -1)
//}}}END  2003-2-11 Eric
	{
		KMO();
		Sphericity(matrix);		
	}
	
	OnFactorExit();

	int nOutNote = 0;//是否显示注释,0 不显示,其它显示 #_S 2005-5-26 zlq
	CDataInterface::GetProfileIntValue("Options","OutNote",nOutNote);        
	if (nOutNote)
	{			
		CTLTable* pTable = new CTLTable;
		pTable->InsertColumn(0,"结果说明");
		for(i=0;i<m_strResultExplain.GetSize();i++)
		{
			pTable->InsertItem(i,m_strResultExplain.GetAt(i));
		}
		m_pResult->InsetAt(0,new CRsltElementTable("结果说明",pTable));
	}
	m_bComBigOne = false;
	m_bCovarian = false;
	m_bScore = false;
	m_nRotateMethod = 0;
	
	return m_pResult;
}

//在输出窗口中输出 共性方差矩阵	 创建因子载荷矩阵  创建旋转后的因子载荷矩阵	 创建斜交结果  创建因子得分结果
void CFactor::CreateResult(CResult * pResult)
{
	if( m_nSel == 0 )    //没有因子被选入
		return;
	//创建因子提取结果表
	CTLTable * pTable = NULL;
	CRsltElementTable * pETable = NULL;
	int i=0;

	double min=0;
	double max=0;
	CTString minFieldName;
	CTString maxFieldName;
	CTString strExplain;
	if(m_VectCommunality.vlen()>0)
	{
//共性方差矩阵
		if(m_nProcType == 1)
		{
			CreateCommulityTable();
		}			

		if(!m_InitCommunality.vlen())
		{
			m_InitCommunality.create( m_nCol);
			m_InitCommunality = copy(m_VectCommunality);
		}

		if(m_nProcType == 1)
		{
			
			int i=0;
			for(i=0;i<m_tVarInfo.iCount;i++)
			{
				if(i==0)
				{
					min=m_InitCommunality[i];
					max=m_InitCommunality[i];
					maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
					minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
				}
				else//ufoufo
				{
					if(min>m_InitCommunality[i])
					{
						min=m_InitCommunality[i];
						minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
						
					}
					if(max<m_InitCommunality[i])
					{
						max=m_InitCommunality[i];
						maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);						
					}					
				}

			}
			strExplain.Format("%s提取的信息最充分，为%.4f；%s提取的信息最不充分，为%.4f。",
				maxFieldName.GetData(),max,minFieldName.GetData(),min);	
		//	m_strResultExplain.Add(strExplain);
			
		}
		else
		{
			pTable = new CTLTable;
			pTable->SetTitle("共性方差矩阵");
//			pTable->SetCols(2);
//			
//			pTable->SetRows(m_tVarInfo2.iCount+1);

			pTable->InsertColumn(0,"");
			pTable->InsertColumn(0,"提取结果");
			
			int i=0;
			for(i=0;i<m_tVarInfo2.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				pTable->SetItemText(i,1,m_InitCommunality[i]);
				if(i==0)
				{
					min=m_InitCommunality[i];
					max=m_InitCommunality[i];
					maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
					minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
				}
				else//ufoufo
				{
					if(min>m_InitCommunality[i])
					{
						min=m_InitCommunality[i];
						minFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);
						
					}
					if(max<m_InitCommunality[i])
					{
						max=m_InitCommunality[i];
						maxFieldName=m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]);						
					}					
				}
			}
			pETable= new CRsltElementTable("共性方差矩阵",pTable);  //申请一个表对象类的对象
			pResult->Add(pETable);
			
		}

		if( m_bComBigOne )
		{
			pTable->SetTail("因为矩阵中有元素大于1,因子载荷矩阵不输出!");
			//{{{START 2003-2-12 Eric
			m_bAntiImage = false;
			m_bAntiImage2 = false;
			m_bShengyu = false;
			m_bZaisheng = false;
			m_bKMO = false;
			m_bScore = false;
			m_nMissingMethod = false;
			//}}}END  2003-2-12 Eric
		}
		
	}

	if(m_bCovarian)
	{//涉及协方差的分析方法中,要对共性方差进行调整以判断是否合法,未调整前的共性方差要参加再生协方差的计算.

/*		if(m_nAnaMethod == 1 && m_bZaisheng)
		{//要计算再生协方差	
			if(m_pUnAdjustComMatrix != NULL)
			{
				delete m_pUnAdjustComMatrix;
				m_pUnAdjustComMatrix = NULL;
			}
			m_pUnAdjustComMatrix = new double[m_nCol * m_nSel];
			memcpy(m_pUnAdjustComMatrix,m_pComMatrix,sizeof(double) * m_nCol * m_nSel);
		}
  */
		//double * pOriDataCorr =
		//	RelationMatrixEx(m_pInputData, m_nRow,m_nCol,m_weight,1);
//{{{START 2003-2-19 Eric
		//if(m_nExtractMethod == 0)
		//	AdjuestResult();
//}}}END  2003-2-19 Eric
		//WriteData(m_pCommunality,m_nSel,1,"最后的共性方差");
		////TraceVector(m_VectCommunality);
		////TraceVector(m_VectCommunality);
		for(i = 0;i<m_nSel;i++)
		{
			if(m_VectCommunality.vlen()>0 && m_VectCommunality[i] > 1)
				m_nIteration = -1;
		}


		if(m_nIteration == -1)
		{
			pTable = new CTLTable;
//			pTable->SetRows(1);
//			pTable->SetCols(1);

			CTString str;
			str.Format("警告：\n共性方差大于方差,因子提取失败!");
			pTable->InsertColumn(0,str);
			pETable= new CRsltElementTable("警告",pTable);
			m_pResult->Add(pETable);

			return;
		}
		else 
		{//后面要计算KMO,球度检验
			if(m_nAnaMethod == 1 && m_bZaisheng)
			{//要计算再生协方差	
				if(m_VectComMatrix.vlen()>0 && m_UnAdjustComMatrix.vlen()>0)
				{
				//	delete m_pComMatrix;
				//	m_pComMatrix = NULL;
					m_VectComMatrix.create(m_UnAdjustComMatrix.vlen());
					m_VectComMatrix = copy(m_UnAdjustComMatrix);
					m_UnAdjustComMatrix.destruction();
				}
			}
		}

	}
	if( !m_bComBigOne)
	{
//创建因子载荷矩阵
		pTable = new CTLTable;
		pTable->SetTitle("因子载荷矩阵");
		pTable->SetHead(m_strExtractMethod);
//		pTable->SetCols(m_nSel+1);
//		if(m_nProcType == 1)
//			pTable->SetRows(m_tVarInfo.iCount+1);
//		else
//			pTable->SetRows(m_tVarInfo2.iCount+1);


		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("因子%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}
//{{{START 2003-2-21 Eric
		ProcessComMatrix(m_VectComMatrix,m_nCol,m_nSel);
//}}}END  2003-2-21 Eric

		if(m_nProcType == 1)
		{	
			int nCol = m_tVarInfo.iCount;
			for(i=0;i<nCol;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{
					pTable->SetItemText(i,j+1,m_VectComMatrix[i+j*nCol]);
				}
			}
		}
		else
		{
			int nCol = m_tVarInfo2.iCount;
			for(i=0;i<nCol;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{
					pTable->SetItemText(i,j+1,m_VectComMatrix[i+j*nCol]);
				}
			}
		}
		if( m_nIteration > 0 )
		{
			CTString str;
			str.Format("总共得迭代次数%d ",m_nIteration);
			pTable->SetTail(str);
		}
		pETable=new CRsltElementTable("因子载荷矩阵",pTable);//申请一个表对象类的对象
		pResult->Add(pETable);
//		if(m_VectEigenValue.vlen()>0 && m_nProcType == 1 && ((m_nExtractMethod ==2)||(1==m_nExtractMethod)))
//			CreateSquareSumExplainTable();//输出因子荷载平方和贡献表

		if (m_VectComMatrix.vlen()>0 && m_nSel>1 )
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_VectComMatrix.vlen()/m_nSel];
			int n = m_VectComMatrix.vlen();
			int nCol = m_VectComMatrix.vlen()/m_nSel;
			int  k=0; 
			for(k=0; k<m_VectComMatrix.vlen()/m_nSel; k++)
			{
				pDPVV[k].fXVal = m_VectComMatrix(k);
				pDPVV[k].fYVal = m_VectComMatrix(k+nCol);
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "因子载荷图";
			pCL->SetTitle(strTit);
			CTString strX = "第一因子";
			pCL->SetXAxilLabel(strX);
			CTString strY = "第二因子";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,nCol,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("因子载荷图",pCL);
			delete pDPVV;
			pResult->Add(pREC);
//			m_VectComMatrix.destruction();
		}

		if(m_VectScore.vlen() > 0 && m_nSel >1 )
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_nRow];        
			int  k=0; 
			for(k=0; k<m_nRow; k++)
			{
				pDPVV[k].fXVal = m_VectScore[k];
				pDPVV[k].fYVal = m_VectScore[k+m_nRow];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTitle = "因子得分图";
			pCL->SetTitle(strTitle);
			CTString strX = "第一因子";
			pCL->SetXAxilLabel(strX);
			CTString strY = "第二因子";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,m_nRow,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("因子得分图",pCL);
			delete pDPVV;
			pResult->Add(pREC);
			m_VectScore.destruction();
		}
	}

//创建旋转后的因子载荷矩阵	
	if( m_nRotateMethod != 0 && m_VectRotateCom.vlen() && !m_bComBigOne && m_nProcType == 1)
	{
		pTable = new CTLTable;
		pTable->SetTitle("旋转后的因子载荷矩阵");
		pTable->SetHead(m_strRotateMethod);
		pTable->SetCols(m_nSel+1);
	//	if(m_nProcType == 1)
	//			pTable->SetRows(m_tVarInfo.iCount+1);
	//		else
	//			pTable->SetRows(m_tVarInfo2.iCount+1);
	

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("因子%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}

		if(m_nProcType == 1)
		{
			for(i=0;i<m_tVarInfo.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{                                                        
					pTable->SetItemText(i,j+1,m_VectRotateCom[i+j*m_nCol]);
				}
			}
		}
		else
		{
			for(i=0;i<m_tVarInfo2.iCount;i++)
			{
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				for(int j=0;j<m_nSel;j++)
				{                                                        
					pTable->SetItemText(i,j+1,m_VectRotateCom[i+j*m_nCol]);
				}
			}
		}
		pETable=new CRsltElementTable("旋转后的因子载荷矩阵",pTable);//申请一个表对象类的对象
		pResult->Add(pETable);
		//旋转后的因子方差贡献表
		if(m_VectRotEigenValue.vlen()>0 && m_nProcType == 1)
			CreateRotedExplainTable();//输出旋转后的因子方差贡献表

		if (m_VectRotateCom.vlen() > 0 && m_nSel > 1)
		{
			int nCol = m_VectRotateCom.vlen()/m_nSel;
			TDataPointValVal* pDPVV =new TDataPointValVal[nCol];        
			for(int  k=0; k<nCol; k++)
			{
				pDPVV[k].fXVal=m_VectRotateCom[k];
				pDPVV[k].fYVal=m_VectRotateCom[k+nCol];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "旋转因子载荷图";
			pCL->SetTitle(strTit);
			CTString strX = "第一旋转因子";
			pCL->SetXAxilLabel(strX);
			CTString strY = "第二旋转因子";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,nCol,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("旋转因子载荷图",pCL);
			delete pDPVV;
			pResult->Add(pREC);
		}
		if (m_VectRotateScore.vlen() > 0 && m_nSel > 1)
		{
			TDataPointValVal* pDPVV =new TDataPointValVal[m_nRow];        
			for(int  k=0; k<m_nRow; k++)
			{
				pDPVV[k].fXVal = m_VectRotateScore[k];
				pDPVV[k].fYVal = m_VectRotateScore[k+m_nRow];
			}
			CTChartScatterPlot* pCL=new CTChartScatterPlot;
			CTString strTit = "旋转因子得分图";
			pCL->SetTitle(strTit);
			CTString strX = "第一旋转因子";
			pCL->SetXAxilLabel(strX);
			CTString strY = "第二旋转因子";
			pCL->SetYAxilLabel(strY);
			pCL->SetScatter(pDPVV,m_nRow,ValVal);
			CRsltElementChart *pREC=new CRsltElementChart("旋转因子得分图",pCL);
			delete pDPVV;
			pResult->Add(pREC);
			m_VectRotateScore.destruction();
		}
	}
//创建斜交结果
	if( m_nRotateMethod == 4 && m_nProcType == 1)
	{
		//斜交因子相关阵
		pTable = new CTLTable;
		pTable->SetTitle("斜交因子相关阵");
//		pTable->SetCols(m_nSel+1);
//		pTable->SetRows(m_nSel+1);

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertColumn(i+1,str);	
		}
		for(i=0;i<m_nSel;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			for(int j=0;j<m_nSel;j++)
			{
				pTable->SetItemText(i,j+1,m_pL[i*m_nSel+j]);
			}
		}
		pETable=new CRsltElementTable("斜交因子相关阵",pTable);//申请一个表对象类的对象
		pResult->Add(pETable);
		//斜交因子结构阵
		int nTemp;
		if(m_nProcType == 1)
			nTemp = m_tVarInfo.iCount;
		else
			nTemp = m_tVarInfo2.iCount;

		CDoubleMatrix matrix(m_pAf,nTemp,m_nSel);

		CreateMatrixResult(matrix,"斜交因子结构阵");
		//斜交因子载荷阵
		CDoubleMatrix matrix2(m_pS,nTemp,m_nSel);
		CreateMatrixResult(matrix2,"斜交因子载荷阵");
	}

//创建因子得分结果
	if( m_VectScore.vlen()>0 && m_bScore && m_nProcType == 1 && m_bOutput)
	{
		int nOrgColNums=m_pOriDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();
		int i=0;
		for(i=0;i<m_nSel;i++)
		{
			CTString strFieldName;
			CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
			strFieldName.Format("因子%d",i+1);
			while(IsExisted(m_pOriDataInterface,strFieldName))
			{
				strFieldName+="_1";
			}
			pField->SetFieldName(strFieldName);
			m_pOriDataInterface->m_DataAccess.m_pFieldList->Add(pField);

		}
		
		int nRecNo = 1;
		m_pOriDataInterface->m_DataAccess .First();
		
		for(i = 0;i<m_nRow;)
		{
			while(nRecNo < m_arrRecNo[i])
			{
				m_pOriDataInterface->m_DataAccess.Edit();
				for(int j = 0;j<m_nSel;j++)
				{
					CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
					pField->SetNull();
					
				}
				m_pOriDataInterface->m_DataAccess.Post();			
				m_pOriDataInterface->m_DataAccess.Next();
				nRecNo++;
			}
			m_pOriDataInterface->m_DataAccess.Edit();
		//	////TraceVector(m_VectScore);
			
			for(int j = 0;j<m_nSel;j++)
			{
				CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
				double v =m_VectScore[i+j*m_nRow];
				pField->SetAsDouble(v);
			}
			i++;
			m_pOriDataInterface->m_DataAccess.Post();			
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}	
	/*
		int i;
		CTLTable * pTable = NULL;
		pTable = new CTLTable();

		pTable->SetTitle("因子得分");
		pTable->SetHead(m_strScoreMethod);
		pTable->SetRows(m_nRow);
		pTable->SetCols(3);
		pTable->InsertColumn(0,"记录号");
		pTable->InsertColumn(1,"权重");

		char szNum1[16];
		char szNum2[10];
		strcpy(szNum1,"因子");

		for(i = 0;i < m_nSel;i++)
		{
			pTable->InsertColumn(i + 2,strcat(szNum1,itoa(i+1,szNum2,10)));
			szNum1[4] = 0;
		}
		int nRecNo = 1;
		m_pDataInterface->m_DataAccess.First();

		for(i = 0;i<m_nRow;)
		{
			while(nRecNo < m_arrRecNo[i])
			{
				pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
				pTable->SetItemText(nRecNo,1,0);
				for(int j = 0;j<=m_nSel;j++)
				{
					pTable->SetItemText(nRecNo,j+2,"缺失");
				}
				m_pDataInterface->m_DataAccess.Next();
				nRecNo++;
			}

			pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
			pTable->SetItemText(nRecNo,1,m_weight(i));
			for(int j = 0;j<m_nSel;j++)
			{
				pTable->SetItemText(nRecNo,j+2,m_pScore[i*m_nSel+j]);
			}
			i++;
			m_pDataInterface->m_DataAccess.Next();
			nRecNo++;
		}

		pTable->SetTail("注:权重为0表示样本或权重中有缺失值,或权重小于等于0");

		CRsltObject * pETable= new CRsltElementTable("因子得分",pTable);
		m_pResult->Add(pETable);
		*/
	}
	m_VectScore.destruction();
	m_VectRotateCom.destruction();
}
 
//在输出窗口中输出特征根的碎石图
void	CFactor::CreateChartResult(CDoubleVector & vect, int nLen)
{
	CRsltElementChart* pREC;
	TDataPointValVal* pDPSV=new TDataPointValVal[nLen];
	int vl = vect.vlen();
	for(int j=0;j<nLen;j++)
	{
		pDPSV[j].fXVal =j+1;
		pDPSV[j].fYVal =vect(vl-j-1);
	}
	
	CTChartLine *pTChartLine=new CTChartLine;
	CTString strTitle = "特征根的碎石图";
	pTChartLine->SetTitle(strTitle);
	CTString strX = "元素数目";
	pTChartLine->SetXAxilLabel(strX);
	CTString strY =  "特征值";
	pTChartLine->SetYAxilLabel(strY);
	pTChartLine->SetLine(pDPSV,nLen,ValVal);
	pTChartLine->SetData(pDPSV,nLen,EScatter,ValVal);
	pREC=new CRsltElementChart(strTitle,pTChartLine);
	m_pResult->Add(pREC);    
	delete[] pDPSV;
}

//在输出窗口中输出矩阵的值
void CFactor::CreateMatrixResult(CDoubleMatrix matrix, char *pszTitle)
{
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle(pszTitle);
//	pTable->SetCols(matrix.mcol()+1);
//	pTable->SetRows(matrix.mrow()+1);

	pTable->InsertColumn(0,"");
	int i=0;
	for(i=0;i<matrix.mcol();i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertColumn(i+1,str);	
	}
	for(i=0;i<matrix.mrow();i++)
	{
		if(m_nProcType == 1)
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		else
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		int j=0;
		for(j=0;j<matrix.mcol();j++)
		{
			pTable->SetItemText(i,j+1,matrix(i,j));
		}
	}
	CRsltElementTable * pETable= new CRsltElementTable(pszTitle,pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
}




// 求特征根及特征向量
//	a:     双精度二维矩阵：n*n,存放对称矩阵A，返回时对角线上存放n个特征值
//	n:     A的阶数
//	v：    n*n 返回特征向量
//	eps：  控制精度要求
void CFactor::cjcbj(CDoubleVector& Vect_A,int n,CDoubleVector& Vect_V,double eps) 
{ 
	int i,j,p,q,u,w,t,s;
	double ff,fm,cn,sn,omega,x,y,d;
	for (i=0; i<=n-1; i++)
	{ Vect_V[i*n+i]=1.0;
	for (j=0; j<=n-1; j++)
	if (i!=j) Vect_V[i*n+j]=0.0;
	}
	ff=0.0;
	for (i=1; i<=n-1; i++)
	for (j=0; j<=i-1; j++)
	{ d=Vect_A[i*n+j]; ff=ff+d*d; }
	ff=sqrt(2.0*ff);

	loop0:
	ff=ff/(1.0*n);

	loop1:
	for (i=1; i<=n-1; i++)
	for (j=0; j<=i-1; j++)
	{ d=fabs(Vect_A[i*n+j]);
	if (d>ff)
	{ p=i; q=j;
	goto loop;
	}
	}
	if (ff<eps) return;
	goto loop0;

	loop: 
	u=p*n+q; w=p*n+p; t=q*n+p; s=q*n+q;
	x=-Vect_A[u]; y=(Vect_A[s]-Vect_A[w])/2.0;
	omega=x/sqrt(x*x+y*y);
	if (y<0.0) omega=-omega;
	sn=1.0+sqrt(1.0-omega*omega);
	sn=omega/sqrt(2.0*sn);
	cn=sqrt(1.0-sn*sn);
	fm=Vect_A[w];
	Vect_A[w]=fm*cn*cn+Vect_A[s]*sn*sn+Vect_A[u]*omega;
	Vect_A[s]=fm*sn*sn+Vect_A[s]*cn*cn-Vect_A[u]*omega;
	Vect_A[u]=0.0; Vect_A[t]=0.0;
	for (j=0; j<=n-1; j++)
	if ((j!=p)&&(j!=q))
	{ u=p*n+j; w=q*n+j;
	fm=Vect_A[u];
	Vect_A[u]=fm*cn+Vect_A[w]*sn;
	Vect_A[w]=-fm*sn+Vect_A[w]*cn;
	}
	for (i=0; i<=n-1; i++)
	if ((i!=p)&&(i!=q))
	{ u=i*n+p; w=i*n+q;
	fm=Vect_A[u];
	Vect_A[u]=fm*cn+Vect_A[w]*sn;
	Vect_A[w]=-fm*sn+Vect_A[w]*cn;
	}
	for (i=0; i<=n-1; i++)
	{ u=i*n+p; w=i*n+q;
	fm=Vect_V[u];
	Vect_V[u]=fm*cn+Vect_V[w]*sn;
	Vect_V[w]=-fm*sn+Vect_V[w]*cn;
	}
	goto loop1;
}
//使矩阵的每一列的和为正.  说明
void CFactor::ProcessComMatrix(CDoubleVector& Vect_Data, int nRow, int nCol)
{
	for(int i=0;i<nCol;i++)
	{
		double sum = 0;
		int j=0;
		for(j=0;j<nRow;j++)
		{
			sum += Vect_Data[j*nCol+i];			
		}
		if( sum < 0 )
		{
			for(j=0;j<nRow;j++)
			{
				Vect_Data[j*nCol+i] = -1 * Vect_Data[j*nCol+i];
			}
		}
	}
}

//将旋转所得因子矩阵进行如下计算：计算每列元素的平方和
//然后根据该平方和将因子从左到右，由大到小排列。 同时使每列的和为正.
void CFactor::ProcessRotateMatrix(CDoubleMatrix &matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	int i;


	//计算每列的和及平方和.
	CDoubleVector vectsumsquare(nCol);
	CDoubleVector vectsum(nCol);
	for(i=0;i<nCol;i++)
	{
		double sumsquare = 0;
		double sum = 0;
		for(int j=0;j<nRow;j++)
		{
			sumsquare += matrix(j,i) * matrix(j,i);
			sum += matrix(j,i);
		}
		vectsumsquare[i] = sumsquare;
		vectsum[i] = sum;
	}



	for(i=0;i<nCol;i++)
	{
		//从第i列以后的列中,选择最大者.
		double max = vectsumsquare[i];

		int nIndex = i;//保存最大值所在的列

		int j=0;
		for(j=i;j<nCol;j++)
		{
			if( vectsumsquare[j] > max )
			{
				max    = vectsumsquare[j];
				nIndex = j;
			}
		}

		double temp ;
		double fTemp1;

		//两列交换.
		for(j=0;j<nRow;j++)
		{
			temp = matrix(j,i);
			if(vectsum[nIndex]<0)
			{
				fTemp1 = -matrix(j,nIndex);
			}
			else
			{
				fTemp1 = matrix(j,nIndex);
			}
			matrix(j,nIndex) = temp;
			matrix(j,i) = fTemp1;
		}

		//平方和交换.
		temp = vectsumsquare[i];
		fTemp1 = vectsumsquare[nIndex];
		vectsumsquare[nIndex] = temp;
		vectsumsquare[i] = fTemp1;

		//和交换
		temp = vectsum[i];
		fTemp1 = vectsum[nIndex];
		vectsum[nIndex] = temp;
		vectsum[i] = fTemp1;

		vectsum[i] = 1;//正数
	}


}

void CFactor::CreateCommulityTable()//输出共性方差矩阵
{
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("共性方差矩阵");
	bool bDoubleCol = false;
	if(m_InitCommunality.vlen()>0)
	{
		bDoubleCol = true;
		//pTable->SetCols(3);
	}
//	else
//		pTable->SetCols(2);


	int nRow =m_tVarInfo.iCount;
//	pTable->SetRows(nRow+1);

	pTable->InsertColumn(0,"");
	if(bDoubleCol)
	{
		pTable->InsertColumn(1,"初始值");
		pTable->InsertColumn(2,"提取值");
	}
	else
		pTable->InsertColumn(1,"提取值");
		
	
	int i=0;
	for(i=0;i<nRow;i++)
	{
		if(m_nProcType == 1)
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
		else
			pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
		
		if(bDoubleCol)
		{
			pTable->SetItemText(i,2,m_VectCommunality[i]);	
			pTable->SetItemText(i,1,m_InitCommunality[i]);
		}
		else
			pTable->SetItemText(i,1,m_VectCommunality[i]);

		
	}
	CRsltElementTable * pETable= new CRsltElementTable("共性方差矩阵",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);

}
//在输出窗口中输出 特征根和累计贡献率
void CFactor::CreateEigenValueResult(CDoubleVector Vect_Data,const int nLen)
{
	double sum = 0;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		sum += Vect_Data[i*nLen+i];
	}
	//特征根和累计贡献率
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("特征根和累计贡献率");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"因子");
	pTable->InsertColumn(1,"特征根");
	pTable->InsertColumn(2,"方差贡献率%");
	pTable->InsertColumn(3,"累计贡献率%");
	double sumpro = 0;
	CTString strExplain;
	CTString strTotal;
	double last=0;
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str);
		pTable->SetItemText(i,1,Vect_Data[i*nLen+i]);
		sumpro += Vect_Data[i*nLen+i] / sum ;
		pTable->SetItemText(i,2,100 * Vect_Data[i*nLen+i] / sum );
		pTable->SetItemText(i,3,100 * sumpro);
		if(m_nExtractStandard==0 &&last<m_fPro)
		{
			strExplain.Format("第%d个因子特征根值为%.4f,它解释了信息量的%.2f%%。\n",i+1,Vect_Data[i*nLen+i],100 * Vect_Data[i*nLen+i] / sum);
			strTotal+=strExplain;
		}
		else if(m_nExtractStandard==1&& i<m_nFactorNum)
		{
			strExplain.Format("第%d个因子特征根值为%.4f,它解释了信息量的%.2f%%。\n",i+1,Vect_Data[i*nLen+i],100 * Vect_Data[i*nLen+i] / sum);
			strTotal+=strExplain;
		}
		last=sumpro;
	}
	m_strResultExplain.Add(strTotal);
	CRsltElementTable * pETable= new CRsltElementTable("特征根和累计贡献率",pTable);
	m_pResult->Add(pETable);	

	
	///zkc 2003.10.23 输出特征向量
	pTable = new CTLTable;
	pTable->SetTitle("特征向量");
//	pTable->SetCols(nLen+1);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");	
	for(i=0;i<nLen;i++)
	{
		CTString colHeader;
		colHeader.Format("特征向量%d",i+1);
		pTable->InsertColumn(i+1,colHeader);	
		
	}
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		for(int j=0;j<nLen;j++)
		{
			pTable->SetItemText(i,j+1,m_EigenVector[i*nLen+j]);
		}
	}
	pETable= new CRsltElementTable("特征向量",pTable);
	m_pResult->Add(pETable);	
	
}

//在输出窗口中输出 特征根和累计贡献率
void CFactor::CreateEigenValueTable()
{
	if(m_nIteration >m_nMaxIterCount)
	{
		CTLTable*	pTable = new CTLTable;
//		pTable->SetRows(1);
//		pTable->SetCols(1);

		CTString str;
		str.Format("警告：\n达到最大迭代次数,提取的因子可能不精确!");
		pTable->InsertColumn(0,str);
		CRsltElementTable *pETable= new CRsltElementTable("警告",pTable);
		m_pResult->Add(pETable);
		
		return;
	}
	
	double sum = m_VectSelEigenValue.sum();
	//特征根和累计贡献率
	int nLen = m_VectSelEigenValue.vlen();
	//////////////////////////////////////////////////////////////////////////
	//输出约相关系数矩阵得相关系数表
	//////////////////////////////////////////////////////////////////////////
	if(m_VectSelEigenValue.vlen()>0)
	{
		sum = m_VectSelEigenValue.sum();
		//特征根和累计贡献率
		CTLTable * pTable = new CTLTable;
		CTString sTemp("");
		sTemp.Format("约相关系数矩阵的特征根和累计贡献率 ");
		pTable->SetTitle(sTemp);
//		pTable->SetCols(4);
//		pTable->SetRows(1+nLen);

		pTable->InsertColumn(0,"因子");
		pTable->InsertColumn(1,"特征根");
		pTable->InsertColumn(2,"方差贡献率%");
		pTable->InsertColumn(3,"累计贡献率%");
		double sumpro = 0;
		CTString strExplain;
		CTString strTotal;
		double last=0;
		int i=0;
		for(i=0;i<nLen ;i++)
		{
			CTString str;
			str.Format("%d",i+1);
			pTable->InsertItem(i,str);
			pTable->SetItemText(i,1,m_VectSelEigenValue[i]);
			sumpro += m_VectSelEigenValue[i] / sum ;
			pTable->SetItemText(i,2,100 * m_VectSelEigenValue[i] / sum );
			pTable->SetItemText(i,3,100 * sumpro);
			if(m_nExtractStandard==0 &&last<m_fPro)
			{
				strExplain.Format("第%d个因子特征根值为%.4f,它解释了信息量的%.2f%%。\n",i+1,m_VectSelEigenValue[i],100 * m_VectSelEigenValue[i] / sum);
				strTotal+=strExplain;
			}
			else if(m_nExtractStandard==1&& i<m_nFactorNum)
			{
				strExplain.Format("第%d个因子特征根值为%.4f,它解释了信息量的%.2f%%。\n",i+1,m_VectSelEigenValue[i],100 * m_VectSelEigenValue[i] / sum);
				strTotal+=strExplain;
			}
			last=sumpro;
		}
		m_strResultExplain.Add(strTotal);
		CRsltElementTable * pETable= new CRsltElementTable("特征根和累计贡献率",pTable);
		m_pResult->Add(pETable);	
	}
	//////////////////////////////////////////////////////////////////////////
	
}

//在输出窗口中输出	反镜像协方差  巴特莱特球度检验	返回卡方值.
double CFactor::Sphericity(CDoubleMatrix matrix_x)
{
	int n = matrix_x.mrow();
	int p = matrix_x.mcol();
	//{{{START 2003-1-25 Eric
//计算anti-image covariance反镜像协方差
	CDoubleMatrix matrix_R = RelationMatrixEx(matrix_x);
	CDoubleMatrix matrix_r = pinv(matrix_R); 
	CDoubleMatrix matrix_A(p,p);

	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			matrix_A(i,j) = matrix_r(i,j) / ( matrix_r(i,i) * matrix_r(j,j) );
		}
	}
	//}}}END  2003-1-25 Eric

	//{{{START 2003-1-25 Eric
//计算chi-square value for Bartlett's test of sphericity
	//double chiSquare = - (n-1-(2*p+5)/6.0)*log(det(matrix_R));

	double fWeight = 0;
	double chiSquare;

	//m_weight.vlen()肯定大于０
	for(i = 0;i<m_weight.vlen();i++)
	{
		if(m_weight[i] < 0)
		{
			m_weight[i] = 0;
		}
		fWeight += m_weight[i]; 
	}

	double fTemp2 = log(det(matrix_R));
	chiSquare = - (fWeight-1-(2*p+5)/6.0)*fTemp2;

	//}}}END  2003-1-25 Eric
	int freedom = p*(p-1) / 2;
	//{{{START 2003-1-28 Eric
	double alpha;
	if(chiSquare > 1e308)
	{//
		alpha = 1 - 1;//chi2cdf(chiSquare,freedom)=1;
	}
	else
	{
		if(chiSquare < -1e308)
		{//警告
			if(m_bKMO)
			{
				CTLTable * pTable = new CTLTable;
//				pTable->SetRows(1);
//				pTable->SetCols(1);

				CTString str;
				if(freedom<=0)
					str.Format("警告：\n自由度(%d)非正数,结果可能不正确!",freedom);
				else
					str.Format("警告：\n数据不合理,结果可能不正确!");
				pTable->InsertColumn(0,str);
				CRsltElementTable * pETable= new CRsltElementTable("警告",pTable);
				m_pResult->Add(pETable);
			}

			m_bKMO = false;

		}
		else
			alpha = 1 - chi2cdf(chiSquare,freedom);
	}


	//}}}END  2003-1-28 Eric
	if( m_bAntiImage2 && !m_bComBigOne )
	{
//反镜像协方差矩阵
		CTLTable * pTable = new CTLTable;
		pTable->SetTitle("反镜像协方差矩阵");
//		pTable->SetRows(matrix_A.mrow()+1);
//		pTable->SetCols(matrix_A.mcol()+1);

		pTable->InsertColumn(0,"");
		for(i=0;i<matrix_A.mcol();i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<matrix_A.mcol();i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<matrix_A.mrow();j++)
			{
				pTable->SetItemText(i,j+1,matrix_A(i,j));
			}
		}
		CRsltElementTable * pETable= new CRsltElementTable("反镜像协方差矩阵",pTable);
		m_pResult->Add(pETable);
	}

	if(m_bKMO)//
	{
//巴特莱特球度检验
		CTLTable * pTable = new CTLTable;
		pTable->SetTitle("巴特莱特球度检验");
//		pTable->SetRows(5);
//		pTable->SetCols(2);

		pTable->InsertColumn(0,"参数");
		pTable->InsertColumn(1,"值");

		pTable->InsertItem(0,"卡方值");
		pTable->InsertItem(1,"自由度");
		pTable->InsertItem(2,"显著性");

		pTable->SetItemText(0,1,chiSquare);
		pTable->SetItemText(1,1,freedom);
		pTable->SetItemText(2,1,alpha);
		CTString strExplain;
		CTString strType;
		if(	m_bPrincipal)
		{
			strType="主成分";
		}
		else
		{
			strType="因子";
		}
		
		if(alpha<=0.05)
		{
			strExplain.Format("引入变量间不独立，适合进行%s分析（概率检验值=%.4f）。",strType.GetData(),alpha);
		}
		else
		{
			strExplain.Format("引入变量间独立，不适合进行%s分析（概率检验值=%.4f）。",strType.GetData(),alpha);
		}
		m_strResultExplain.Add(strExplain);
		CRsltElementTable * pETable= new CRsltElementTable("巴特莱特球度检验",pTable);
		m_pResult->Add(pETable);
	}
//}}}END  2003-1-25 Eric
	return chiSquare;
}

//求解偏相关系数，x1,x2是指定的列号，返回他们俩的系数值
double CFactor::PCorrelation(const int x1, const int x2)
{
	int n = m_nRow;                //输入数据的行数
	int k = m_nCol;				   //x的列数
	int j=0;
	CDoubleVector Vect_X(n * k);
	CIntVector Vect_Pos(k);
	
	//后面的列放控制变量数据，前面两列放查看变量数据
	if(m_nProcType == 1)
	{
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nColNum = m_tVarInfo.pValue[i];
			if( nColNum != x1 && nColNum != x2 && nColNum >= 0 )
			{
				for(int ii=0;ii<n;ii++)
				{
					Vect_X[ii*k+(j+2)] = m_VectInputData[k*ii+nColNum];
				}
				j++;
			}
		}
	}
	else
	{
		for(int i=0;i<m_tVarInfo2.iCount;i++)
		{
			int nColNum = m_tVarInfo2.pValue[i];
			if( nColNum != x1 && nColNum != x2 && nColNum >= 0 )
			{
				for(int ii=0;ii<n;ii++)
				{
					Vect_X[ii*k+(j+2)] = m_VectInputData[k*ii+nColNum];
				}
				j++;
			}
		}
	}

	int ii=0;
	for(ii=0;ii<n;ii++)
	{
		Vect_X[ii*k]   = m_VectInputData[ii*k+x1];
		Vect_X[ii*k+1] = m_VectInputData[ii*k+x2];
	}
	Formalize(Vect_X,n,k);
	CDoubleVector Vect_Rr = RelationMatrixEx(Vect_X,n,k,m_weight);  //calculate relation matrix
 	for(ii=0;ii<k;ii++)
	{
		Vect_Pos[ii] = ii;
	}
	double r = partial(Vect_Rr,Vect_Pos,k,k);	

	return r;
}

CDoubleMatrix CFactor::GetPCorrelationMatrix(CDoubleVector Vect_Prr)
{
	CDoubleMatrix matrix(m_nCol,m_nCol);
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			if( i == j )
			{
				matrix(i,j) = 1;
			}
			else if ( j > i )
			{
				matrix(i,j) = -PCorrelation(i,j,Vect_Prr);
			}
			else
			{
				matrix(i,j) = matrix(j,i);
			}
		}
	}
	return matrix;
}

//相关的统计量计算和输出
void CFactor::KMO(int nType)
{
	int i;//j;
	CTLTable * pTable = NULL;
	CRsltElementTable * pETable = NULL;
	////TraceVector( m_VectInputData);
	Formalize(m_VectInputData,m_nRow,m_nCol);
	////TraceVector( m_VectInputData);
	////TraceVector(m_VectFirstCovariance);
	CDoubleVector rr;
	if(m_bCovarian && m_VectFirstCovariance.vlen())
	{
		rr = copy(m_VectFirstCovariance);  
	}
	else
	{
		rr = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight);  
	}


	i=0;
	if( m_nCol > 2 && m_nCol<13&& (m_bAntiImage || m_bKMO) )
	{
	
		CDoubleMatrix matrix = GetPCorrelationMatrix(rr);
		//TraceMatrix(matrix);
		int n = matrix.mcol();

		CDoubleVector vector_msa(n);

		double t1 = 0;
		double t2 = 0;
		bool bExist=true;
		for(i=0;i<n;i++)
		{
			double rsum=0;
			double asum=0;
			for(int j=0;j<n;j++)
			{
				if( j!=i)
				{
					rsum += rr[i+j*n]*rr[i+j*n];
					asum += matrix(i,j)*matrix(i,j);
				}
			}
			t1+=rsum;
			t2+=asum;
			if(rsum+asum==0)
			{
				bExist=false;
				break;
			}
			vector_msa[i] = rsum / (rsum + asum);
		}
		
		m_dkmo = t1 / (t1+t2);	
		////TraceVector(vector_msa);
		for(i=0;i<n;i++)
		{
			matrix(i,i) = vector_msa[i];
		}	
		//TraceMatrix(matrix);
		//{{{START 2003-1-26 Eric 不计算反镜像相关系数矩阵
		
		if(m_bAntiImage && !m_bComBigOne &&bExist)
		{			
			pTable = new CTLTable;
//			pTable->SetRows(1+n);
//			pTable->SetCols(1+n);

			pTable->SetTitle("反镜像相关系数矩阵");
			pTable->InsertColumn(0,"");
			if(nType==0)
			{
				for(i=0;i<n;i++)
				{
					pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
					pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
				}
				
			}
			else
			{
				for(i=0;i<n;i++)
				{
					pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
					pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));
				}
			}
			for(i=0;i<n;i++)
			{
				for(int j=0;j<n;j++)
				{
					pTable->SetItemText(i,j+1,matrix(i,j));
					double aa=matrix(i,j);
					int a=4;
				}
			}
			pETable= new CRsltElementTable("反镜像相关系数矩阵",pTable);
			m_pResult->Add(pETable);
		}
	
	//}}}END  2003-1-26 Eric
}

	//再生相关系数
	CDoubleVector Vect_Matrix ;


//处理因子载荷矩阵
	////TraceVector(m_VectComMatrix);
	if(m_VectComMatrix.vlen())
	{
		Vect_Matrix = copy(m_VectComMatrix);
	}
	else 
		return ;

	//使用未旋转的因子载荷矩阵
	if( (m_nExtractMethod == 0 || m_nExtractMethod == 1) && m_nAnaMethod == 1 )
	{
		Vect_Matrix = copy(m_VectUnAdjustCom);
	}
	
	CDoubleMatrix matrix_temp = VectorToMatrix(Vect_Matrix,m_nCol,m_nSel);
	
	CDoubleMatrix matrix_Rep =  matrix_temp * trans(matrix_temp);


	if( m_bZaisheng )
	{
		CTString str;
		if( m_nAnaMethod == 0 )
		{
			str = _T("再生相关系数");
		}
		else
		{
			str = _T("再生协方差系数");
		}  

//再生相关系数Rep 或 再生协方差系数Rep
		pTable = new CTLTable();
		pTable->SetTitle(str);
//		pTable->SetRows(1+m_nCol);
//		pTable->SetCols(1+m_nCol);

		pTable->InsertColumn(0,"");
		int i=0;
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<m_nCol;j++)
			{
				pTable->SetItemText(i,j+1,matrix_Rep(i,j));
			}
		}
		pETable= new CRsltElementTable( str, pTable);
		m_pResult -> Add(pETable);
	}

//相关系数矩阵残差 或 协方差矩阵残差
	if( m_bShengyu )
	{
		CTString str;
		if( m_nAnaMethod == 0 )
		{
			str = _T("相关系数矩阵残差");
		}
		else
		{
			str = _T("协方差矩阵残差");
		}

		pTable = new CTLTable();
		pTable->SetTitle(str);
//		pTable->SetRows(1+m_nCol);
//		pTable->SetCols(1+m_nCol);

		pTable->InsertColumn(0,"");
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertColumn(i+1,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

		}
		for(i=0;i<m_nCol;i++)
		{
			if(m_nProcType == 1)
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]));
			else
				pTable->InsertItem(i,m_pDataInterface->GetFieldName(m_tVarInfo2.pValue[i]));

			for(int j=0;j<m_nCol;j++)
			{
				if( i!= j)
				{
					pTable->SetItemText(i,j+1,rr[i+j*m_nCol] - matrix_Rep(i,j));
				}
			}
		}
		//delete [] rr; rr = NULL;
		//rr.destruction();
		pETable= new CRsltElementTable(str,pTable);
		m_pResult->Add(pETable);
	}
}


//*********************************************************************
//Author:zkc
//Date:2003.4.8
//Functionality: 求一阶导数 --formula (6)。
//*********************************************************************
CDoubleVector CFactor::Ladder1(CDoubleVector vector_x,
			   CDoubleVector vector_gamma,
			   CDoubleMatrix matrix_omiga,
			   int nMothod,		//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
			   int p,  //p=nCol
			   int m,
			   int nMethod)
{
	CDoubleVector vector_h(p);
	int i=0;
	for(i=0;i<p;i++)
	{
		double h=0;
		for(int k=m;k<p;k++)
		{
			if(nMethod==0)
			{
				h=h+(1-1.0/vector_gamma(k))*matrix_omiga(i,k)*matrix_omiga(i,k);
			}
			else if(nMethod==1)
			{
				h=h+(vector_gamma(k)*vector_gamma(k)-vector_gamma(k))*matrix_omiga(i,k)*matrix_omiga(i,k);
			}
			else if(nMethod==2)
			{
				h=h+vector_gamma(k)*matrix_omiga(i,k)*matrix_omiga(i,k);

			}
		}
		if(nMethod==2)
		{
			h=2*vector_x(i)*h;
		}
		vector_h(i)=h;
	}
	return vector_h;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:按formula (7)求二阶导数H.
//********************************************************************//

CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_gamma,
			   CDoubleVector vector_x,
			   CDoubleMatrix matrix_omiga,
			   CDoubleMatrix matrix_r,//相关系数阵的
			   CDoubleVector vector_h,
			   int nMethod,		//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
			   int p,
			   int &m)///p=nCol
{
	CDoubleMatrix matrix_H(p,p);
	int i=0;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			double delta=0;
			if(i==j)
				delta=1;
			else
				delta=0;
			double H=0;
			if(nMethod==0)
				H=-1*delta*vector_h(i);
			else if(nMethod==1)
				H=delta*vector_h(i);

			int k=m;
			for(k=m;k<p;k++)
			{
				double temp=0;
				for(int n=0;n<m;n++)
				{
					if(nMethod==0)//极大似然法
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n)-2)*matrix_omiga(i,n)*matrix_omiga(j,n)/(vector_gamma(k)-vector_gamma(n));
					}
					else if(nMethod==1)//普通最小二乘法
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n)-2)*matrix_omiga(i,n)*matrix_omiga(j,n)*vector_gamma(n)/(vector_gamma(k)-vector_gamma(n));
					}
					else//无权重最小二乘法
					{
						temp=temp+(vector_gamma(k)+vector_gamma(n))*matrix_omiga(i,k)*matrix_omiga(j,k)/(vector_gamma(k)-vector_gamma(n));
					}
				}
				if(nMethod==0)
				{
					temp=temp+delta;
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k);
				}
				else if(nMethod==1)
				{
					temp=temp+matrix_r(i,i)*exp((vector_x(i)+vector_x(j))/2);
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k)*vector_gamma(k);
				}
				else if(nMethod==2)
					temp=temp*matrix_omiga(i,k)*matrix_omiga(j,k);
				H=H+temp;
			}
			if(nMethod==2)
			{
				H=H*vector_x(i)*vector_x(j);
				double temp=0;
				for(k=m;k<p;k++)
				{
					temp=temp+(vector_x(i)*vector_x(i)-vector_gamma(k)/2)*matrix_omiga(i,k)*matrix_omiga(i,k);
				}
				temp=temp*delta;
				H=H+temp;
				H=4*H;
			}
			matrix_H(i,j)=H;
		}
	}
	return matrix_H;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:矩阵的m次方。
//********************************************************************//

CDoubleMatrix CFactor::powMatrix(CDoubleMatrix matrix,int m)
{
	int nRow=matrix.mrow();
	int nCol=matrix.mcol();
	CDoubleMatrix temp(nRow,nCol);
	for(int i=0;i<nRow;i++)
	{
		for(int j=0;j<nCol;j++)
			temp(i,j)=pow(matrix(i,j),m);
	}
	return temp;

}


//*******************************************************************//
//Author:zkc
//Date:2003.4.8
//Functionality:按formula (8)求二阶导数H
//********************************************************************//

CDoubleMatrix CFactor::Ladder2(CDoubleVector vector_x,
					  CDoubleMatrix matrix_omiga,
					  int &m,int p,
					  int nMethod)
{
	CDoubleMatrix matrix_H(p,p);
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			double temp=0;
			for(int k=m;k<p;k++)
			{
				temp=temp+matrix_omiga(i,k)*matrix_omiga(j,k);
			}
			temp=temp*temp;
			if(nMethod==2)
				temp=temp*4*vector_x(i)*vector_x(j);
			matrix_H(i,j)=temp;
		}
	}
	return matrix_H;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:如果二阶导数matrix,的某一个的对角线元素小于0.05，则把对应的行
//				列元素赋值为0，该对角线元素赋值为1。
//********************************************************************//

void CFactor::ChangeLadder2(CDoubleMatrix &matrix)
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	for(int i=0;i<nRow;i++)
	{
		if(matrix(i,i)<0.05)
		{
			int m=0;
			for(m=0;m<nRow;m++)
				matrix(m,i)=0;
			for(m=0;m<nCol;m++)
				matrix(i,m)=0;
			matrix(i,i)=1;
		}
	}
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:求代数余子式
//********************************************************************//
CDoubleMatrix CFactor::GetSubMatrix(CDoubleMatrix matrix,
						   int m,		//行下标
						   int n)		//列下标
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	assert(nCol==nRow);
	CDoubleMatrix subMatrix(nRow-1,nCol-1);
	for(int i=0;i<nRow;i++)
	{
		int m1=i;
		if(i==m)
			continue;
		if(i>m)
			m1=i-1;
		for(int j=0;j<nCol;j++)
		{
			int n1=j;
			if(j==n)
				continue;
			if(j>n)
				n1=j-1;
			subMatrix(m1,n1)=matrix(i,j);
		}
	}
	return subMatrix;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:矩阵是否正定
//********************************************************************//
bool CFactor::IsNormal(CDoubleMatrix matrix)
{
	int nCol=matrix.mcol();
	int nRow=matrix.mrow();
	assert(nCol==nRow);
	for(int i=0;i<nRow;i++)
	{
		CDoubleMatrix subMatrix=GetSubMatrix(matrix,i,i);//对角线元素的代数余子式
		if(det(subMatrix)<=0)
			return false;
	}
	return true;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.9
//Functionality:判断vector_d每一个值是否均大于0.1
//********************************************************************//

bool CFactor::IsDLegal(CDoubleVector vector_d)
{
	for(int i=0;i<vector_d.vlen();i++)
	{
		if(vector_d(i)<=0.1)
		{
			return false;
		}
	}
	return true;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:求目标函数的值，同时计算matrix_psi,vector_gamma,matrix_omiga
//********************************************************************//
double CFactor::CalcFx(CDoubleVector vector_x,		//nCol
			  CDoubleMatrix matrix_R,		//相关系数矩阵,nColxnCol
			  CDoubleMatrix matrix_r,		//matrix_R的逆阵,nColxnCol
			  CDoubleMatrix &matrix_psi,	//nCol*nCol
			  CDoubleVector &vector_gamma,	//nCol
			  CDoubleMatrix &matrix_omiga,	//nColxnCol
			  int nMethod,					//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
			  int nCol,						//p=nCol
			  int &m)						//提取因子的个数
{

	if(nMethod==0||nMethod==1)
	{
		matrix_psi=sqrt(diag(exp(vector_x)));
	}
	else
	{
		matrix_psi=diag(exp(vector_x));
	}
	
	///////////formula (4)
	CDoubleMatrix matrix_4(nCol,nCol);
	if(nMethod==0||nMethod==1)
	{
		matrix_4=matrix_psi*matrix_r*matrix_psi;
	}
	else
	{
		matrix_4=matrix_R-powMatrix(matrix_psi,2);
	}
	CDoubleVector Vect_Gamma(nCol*nCol);//用计算特征向量和特征根，也用于存储特征根，对角线元素
	assert(Vect_Gamma.vlen()>0);

	//将matrix_4的数据复制到pGamma结构中
	int i=0;
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			Vect_Gamma[i*nCol+j]=matrix_4(i,j);
		}
	}
	CDoubleVector Vect_Omiga(nCol*nCol);//存储matrix_4的特征向量
	cjcbj(Vect_Gamma,nCol,Vect_Omiga,EPS);
	if(nMethod==0||nMethod==1)
		SortEigenVect(Vect_Gamma,Vect_Omiga,nCol,nCol,0);//小到大排序
	else
		SortEigenVect(Vect_Gamma,Vect_Omiga,nCol,nCol,1);//大到小排序
	for(i=0;i<nCol;i++)
	{
		vector_gamma(i)=Vect_Gamma[i*nCol+i];
	}
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			matrix_omiga(i,j)=Vect_Omiga[i*nCol+j];
		}
	}


	int p=nCol;
	
	/////////计算目标函数值f0
	double f0=0;
	int k=0;
	for(k=m;k<p;k++)
	{
		if(nMethod==0)
		{
			f0=f0+log(vector_gamma(k))+1.0/vector_gamma(k)-1;
		}
		else if(nMethod==1)
		{
			f0=f0+(vector_gamma(k)-1)*(vector_gamma(k)-1)/2.0;
		}
		else
		{
			f0=f0+vector_gamma(k)*vector_gamma(k)/2.0;
		}
	}
	return f0;

}

//*******************************************************************//
//Author:zkc
//Date:2003.4.11
//Functionality:向量除以一个数值.
//********************************************************************//
CDoubleVector CFactor::VecDiv(CDoubleVector vect,double val)
{
	CDoubleVector temp(vect.vlen());
	for(int i=0;i<vect.vlen();i++)
	{
		temp(i)=vect(i)/val;
	}
	return temp;
}

//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:当f>f0时，修正vector_x,matrix_psi,vector_gamma,matrix_omiga
//********************************************************************//
bool  CFactor::ChangeX(CDoubleVector &vector_x,		//nCol
			  CDoubleVector vector_d,		//nCol
			  CDoubleVector vector_oldx,	//nCol
			  CDoubleMatrix matrix_R,		//相关系数矩阵,nColxnCol
			  CDoubleMatrix matrix_r,		//matrix_R的逆阵,nColxnCol
			  CDoubleMatrix &matrix_psi,	//nCol*nCol
			  CDoubleVector &vector_gamma,	//nCol
			  CDoubleMatrix &matrix_omiga,	//nCol*nCol
			  int nMethod,					//0-极大似然法，1-普通最小二乘法，2-无权重最小二乘法
			  int nCol,						//p=nCol
			  double f0,					//f0值
			  double &fOrg,
			  int &m)						//提取因子的个数
{
	int id=0;
	CDoubleVector vector_x1=vector_oldx;
	double f=0;
	CDoubleMatrix matrix_psi1(nCol,nCol);
	CDoubleMatrix matrix_omiga1(nCol,nCol);
	CDoubleVector vector_gamma1(nCol);
	int m1=m;
	int dom=1;
	while(id<ID)
	{
		dom=2*dom;
		vector_x1=vector_oldx-VecDiv(vector_d,dom);
		f=CalcFx(vector_x1,matrix_R,matrix_r,matrix_psi1,vector_gamma1,matrix_omiga1,nMethod,nCol,m1);
		if(f<f0)
			break;	
		id++;
	}
	if(id<ID)
	{
		vector_x=vector_x1;
		vector_gamma=vector_gamma1;
		matrix_omiga=matrix_omiga1;
		matrix_psi=matrix_psi1;
		m=m1;
		fOrg=f;
		return true;
	}
	else
	{
		if(nMethod==2)
		{
			int dom=1;
			id=0;
			while(id<ID)
			{
				dom=2*dom;
				vector_x1=vector_oldx+VecDiv(vector_d,dom);
				f=CalcFx(vector_x1,matrix_R,matrix_r,matrix_psi1,vector_gamma1,matrix_omiga1,nMethod,nCol,m1);
				if(f<f0)
					break;	
				id++;
			}
			if(id<ID)
			{
				vector_x=vector_x1;
				vector_gamma=vector_gamma1;
				matrix_omiga=matrix_omiga1;
				matrix_psi=matrix_psi1;
				m=m1;
				fOrg=f;
				return true;
			}
			return false;
		}
	}
	return false;
}


//*******************************************************************//
//Author:zkc
//Date:2003.4.10
//Functionality:计算共性方差,同时按公式（12），（13）计算matrix_GAMMA，matrix_OMIGA
//********************************************************************//

CDoubleVector CFactor::CalcCommunality(CDoubleMatrix matrix_omiga,
							  CDoubleVector vector_gamma,
							  CDoubleMatrix matrix_psi,
							  CDoubleMatrix &matrix_OMIGA,
							  CDoubleMatrix &matrix_GAMMA,
							  int p,int m,
							  int nMethod)
{
	CDoubleMatrix matrix_GAMMA1(m,m,0.0);
	matrix_GAMMA=matrix_GAMMA1;
	CDoubleMatrix matrix_I(m,m,0.0);
	CDoubleMatrix matrix_I1(p,p,0.0);
	int i=0;
	for(i=0;i<p;i++)
	{
		if(i<m)
		{
			matrix_GAMMA(i,i)=fabs(vector_gamma(i));
			matrix_I(i,i)=1;
		}
		matrix_I1(i,i)=1;

	}
	CDoubleMatrix matrix_OMIGA1(p,m);
	matrix_OMIGA=matrix_OMIGA1;
	for(i=0;i<p;i++)
	{
		for(int j=0;j<m;j++)
		{
			matrix_OMIGA(i,j)=matrix_omiga(i,j);
		}
	}
	int nCol=m;
	if(nMethod==0)
		nCol=m;
	else if(nMethod==1)
		nCol=p;
	else
		nCol=m;
		
	CDoubleMatrix matrix_communality(p,nCol);//共性方差矩阵
	CDoubleVector vector_cummunality(p,0);//
	if(nMethod==0)
	{
		matrix_communality=powMatrix((matrix_psi*matrix_OMIGA),2)*(pinv(matrix_GAMMA)-matrix_I);
	}
	else if(nMethod==1)
	{
		CDoubleMatrix tempMatrix=diag(vector_gamma);
		matrix_communality=powMatrix((matrix_psi*matrix_omiga),2)*(pinv(tempMatrix)-matrix_I1);
	}
	else
	{
		matrix_communality=powMatrix(matrix_OMIGA,2)*matrix_GAMMA;
	}
	for(i=0;i<p;i++)
	{
		for(int j=0;j<matrix_communality.mcol();j++)
		{
			vector_cummunality(i)+=matrix_communality(i,j);
		}
	}
	return vector_cummunality;

}


//////////////////////////////////////////////////////////
//    ML，GLS，ULS因子分析       
//////////////////////////////////////////////////////////
void CFactor::ml_fact(CDoubleMatrix matrix,		//原始数据
					  int nMethod,				//0--极大似然法，1--普通最小二乘法，2--无权重最小二乘法
					  int m,					//提取因子的个数
					  int ncount,				//最大迭代次数
					  int ID)
{
//	CreateEig(0);
	int nRow = matrix.mrow();
	int nCol  = matrix.mcol();
	////////////////////////    2003.4.7   //////////////////////////////////////

	CDoubleVector Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight);  //calculate relation matrix

	CDoubleMatrix matrix_R= VectorToMatrix(Vect_Data,nCol,nCol);/////相关系数矩阵

	assert(Vect_Data.vlen()>0);
	m_EigenVector.create( m_nCol*m_nCol);//分配特征向量空间
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.000001);//求特征向量与特征值
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//将特征向量，与特征根按特征值从小到大排序
//	CreateEigenValueResult(pData,m_nCol);//输出特征根和累计贡献率
//	WriteData(m_pEigenVector,m_nCol,m_nCol,"after");

	int p=nCol;
	//根据贡献率决定提取因子个数
	if(m_nExtractStandard==0 && m_bIsMValued==false)
	{
		CDoubleVector vector_gamma(m_nCol);
		int i=0;
		for(i=0;i<m_nCol;i++)
		{
			vector_gamma(i)=Vect_Data[i*m_nCol+i];
		}
		double dRatio=m_fPro;//to define the contrubution ratio.
		double a=0;
		double b=sum(vector_gamma);
		for(i=0;i<p;i++)
		{
			a=a+vector_gamma(i);
			if(a/b>=dRatio)
			{
				m=i+1;
				break;
			}
		}
		m_bIsMValued=true;
	}
	CDoubleMatrix matrix_r = pinv(matrix_R);////相关系数矩阵逆矩阵
	CDoubleVector vector_CommunIntial(p,0);//共性方差矩阵的初始值
	CDoubleVector vector_EigenIntial(p,0);//特征根和累计贡献率的初始值
	int i=0;
	for(i=0;i<p;i++)
	{
		vector_CommunIntial(i)=1.0-1/matrix_r(i,i);
		vector_EigenIntial(i)=Vect_Data[i*nCol+i];
	}
	////////formula (1)
	CDoubleVector vector_x(nCol);
	for(i=0;i<nCol;i++)
	{
		if(nMethod==0||nMethod==1)
		{
			vector_x(i)=log(fabs((1-1.0*m/2/p)/matrix_r(i,i)));
		}
		else
		{
			vector_x(i)=sqrt(fabs((1-1.0*m/2/p)/matrix_r(i,i)));
		}		
	}
	CDoubleMatrix matrix_psi(nCol,nCol);
	CDoubleMatrix matrix_omiga(nCol,nCol);
	CDoubleVector vector_gamma(nCol);
	double f0=CalcFx(vector_x,matrix_R,matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,m);
	CDoubleMatrix matrix_OMIGA,matrix_GAMMA;//维数大小在函数体内定义


	CDoubleVector vector_d(p,1);
	int s=1;
	CDoubleVector vector_h(p);
	CDoubleMatrix matrix_H(p,p);	
	CDoubleVector vector_oldx;
	double f=0;
	int flag=0;
	while((abs(vector_d).Max()>=0.0001)&&(s<350))
	{
		vector_h=Ladder1(vector_x,vector_gamma,matrix_omiga,nMethod,p,m,nMethod);//formula (6)
		if(flag==0)
			matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);
		else
			matrix_H=Ladder2(vector_gamma,vector_x,matrix_omiga,matrix_r,vector_h,nMethod,p,m);//formula (7)
		flag++;		
		ChangeLadder2(matrix_H);
		if(!IsNormal(matrix_H))//H不正定
		{	
			//formula (8)
			matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);
			ChangeLadder2(matrix_H);
			if(!IsNormal(matrix_H))
				break;
			else
				vector_d=pinv(matrix_H)*vector_h;
		}
		else
		{
			vector_d=pinv(matrix_H)*vector_h;//formula (9)
			if(IsDLegal(vector_d))//vector_d的每一个值均大于0.1
			{
				matrix_H=Ladder2(vector_x,matrix_omiga,m,p,nMethod);//formula (8)
				ChangeLadder2(matrix_H);
				if(!IsNormal(matrix_H))
					break;
				else
					vector_d=pinv(matrix_H)*vector_h;//formula(9)
			}
		}
		vector_oldx=copy(vector_x);
		vector_x=vector_x-vector_d;	
		f=CalcFx(vector_x,matrix_R,matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,m);
		if(f>f0)
		{
			if(!ChangeX(vector_x,vector_d,vector_oldx,matrix_R,
					matrix_r,matrix_psi,vector_gamma,matrix_omiga,nMethod,nCol,f0,f,m))
			{
				break;
			}
		}
		f0=f;
		s=s+1;
	}

	
	CDoubleMatrix matrix_I(m,m,0.0);
	for(i=0;i<m;i++)
	{
		matrix_I(i,i)=1;
	}

	CDoubleVector vector_CommunLast=CalcCommunality(matrix_omiga,vector_gamma,matrix_psi,matrix_OMIGA,matrix_GAMMA,p,m,nMethod);//共性方差矩阵的最终提取值
	CDoubleMatrix matrix_LAMDA(p,m);//因子载荷矩阵
	if(nMethod==0||nMethod==1)
	{
		matrix_LAMDA=matrix_psi*matrix_OMIGA*sqrt(pinv(matrix_GAMMA)-matrix_I);
	}
	else
	{
		matrix_LAMDA=matrix_OMIGA*sqrt(matrix_GAMMA);
	}
	double chi=0,df=0,P=0;
	if(nMethod==0||nMethod==1)
	{
		chi=(sum(m_weight)-1-(2.0*p+5)/6-2*m/3)*f;//有权重情况
		df=((p-m)*(p-m)-p-m)/2;
		if(df<1)
			df=1;
		P=1-chi2cdf(chi,df);
	}
	m_nCol=p;
	m_nSel=m;
	m_VectComMatrix.create( m_nCol*m_nSel);
	for(i=0;i<m_nSel;i++)
	{
		for(int j=0;j<m_nCol;j++)
		{
			m_VectComMatrix[i*m_nCol+j]=matrix_LAMDA(j,i);
		}
	}
	////TraceVector(m_VectComMatrix);
	CDoubleVector vector_EigenLast(p,0);//特征根和累计贡献率的最终值
	
	
	CreateResultForMl_fact(matrix_LAMDA,vector_CommunLast,vector_CommunIntial,vector_EigenIntial,vector_EigenLast,nMethod,m,df,P,chi);
	vector_EigenIntial.Sort();
	CreateChartResult(vector_EigenIntial,p);
}

void CFactor::CreateResultForMl_fact(CDoubleMatrix matrix_LAMDA,		//因子载荷矩阵
								     CDoubleVector vector_CommunLast,	//共性方差初始值
								     CDoubleVector vector_CommunIntial,	//共性方差提取值
								     CDoubleVector vector_EigenIntial,	//特征根初始值
									 CDoubleVector vector_EigenLast,	//特征根最终值
									 int nMethod,						//0--最大似然法，1--一般最小二乘法，2--无权重最小二乘法
									 int m,								//提取因子数
									 int df,							//自由度
									 double P,							//显著性
									 double chi)						//卡方值
{



	CTString strExplain;


	int nLen=vector_CommunLast.vlen();
	double sum1 = 0;
	double sum2=0;
	int i=0;
	for(i=0;i<nLen;i++)
	{
		sum1+= vector_EigenIntial(i);
		sum2+= vector_EigenLast(i);
	}
	//特征根和累计贡献率初始值输出
	CTLTable * pTable = new CTLTable;
	pTable->SetTitle("特征根和累计贡献率初始值");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"因子");
	pTable->InsertColumn(1,"特征根");
	pTable->InsertColumn(2,"方差贡献率%");
	pTable->InsertColumn(3,"累计贡献率%");
	double sumpro = 0;
	CTString strAdd;
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		pTable->SetItemText(i,1,vector_EigenIntial[i]);
		sumpro += vector_EigenIntial[i] / sum1 ;
		pTable->SetItemText(i,2,100 * vector_EigenIntial[i] / sum1);
		pTable->SetItemText(i,3,100 * sumpro);
		if(i<m)
		{
			strExplain.Format("第%d个因子特征根值为%.4f,它解释了信息量的%.2f%%。\n",i+1,vector_EigenIntial[i],100 * vector_EigenIntial[i] / sum1);
			strAdd+=strExplain;
		}		
	}
	CRsltElementTable * pETable= new CRsltElementTable("特征根和累计贡献率初始值",pTable);
	m_pResult->Add(pETable);
	m_strResultExplain.Add(strAdd);

	///zkc 2003.10.23 输出特征向量
	pTable = new CTLTable;
	pTable->SetTitle("特征向量");
//	pTable->SetCols(nLen+1);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");	
	for(i=0;i<nLen;i++)
	{
		CTString colHeader;
		colHeader.Format("特征向量%d",i+1);
		pTable->InsertColumn(i+1,colHeader);	
		
	}
	for(i=0;i<nLen ;i++)
	{
		CTString str;
		str.Format("%d",i+1);
		pTable->InsertItem(i,str,false);
		for(int j=0;j<nLen;j++)
		{
			pTable->SetItemText(i,j+1,m_EigenVector[i*nLen+j]);
		}
	}
	pETable= new CRsltElementTable("特征向量",pTable);
	m_pResult->Add(pETable);	


	///共性方差输出
	pTable = new CTLTable;
	pTable->SetTitle("共性方差");
//	pTable->SetCols(2);
//	pTable->SetRows(1+nLen);

	pTable->InsertColumn(0,"");
	pTable->InsertColumn(1,"初始值");
	pTable->InsertColumn(2,"提取值");

	//zkc 2003.8.19
	double min=vector_CommunLast[0];
	double max=vector_CommunLast[0];
	int minIndex=0;
	int maxIndex=0;
	for(i=0;i<nLen ;i++)
	{
		CTString str=m_pDataInterface->GetFieldName(m_tVarInfo.pValue[i]);
		pTable->InsertItem(i,str,false);
		pTable->SetItemText(i,1,vector_CommunIntial[i]);
		pTable->SetItemText(i,2,vector_CommunLast[i]);
		if(min>vector_CommunLast[i])
		{
			min=vector_CommunLast[i];
			minIndex=i;
		}
		if(max<vector_CommunLast[i])
		{
			max=vector_CommunLast[i];
			maxIndex=i;
		}

	}
	pETable= new CRsltElementTable("共性方差",pTable);
	m_pResult->Add(pETable);

	strExplain.Format("%s提取的信息最充分，为%.4f；%s提取的信息最不充分，为%.4f。",
					m_pDataInterface->GetFieldName(m_tVarInfo.pValue[maxIndex]).GetData(),max,
					m_pDataInterface->GetFieldName(m_tVarInfo.pValue[minIndex]).GetData(),min);	
	m_strResultExplain.Add(strExplain);
	if(nMethod==0||nMethod==1)
	{
		pTable = new CTLTable;
		pTable->SetTitle("显著性检验");
//		pTable->SetCols(3);
//		pTable->SetRows(2);

		pTable->InsertColumn(0,"卡方值");
		pTable->InsertColumn(1,"自由度");
		pTable->InsertColumn(2,"显著性");
		CTString str;
		str.Format("%.4f",chi);
		pTable->InsertItem(0,str,false);
		pTable->SetItemText(0,1,df);	
		pTable->SetItemText(0,2,P);
		pETable= new CRsltElementTable("显著性检验",pTable);
		m_pResult->Add(pETable);
	}

}



void CFactor::PSI(CDoubleVector vector_x,CDoubleMatrix matrix_R,const int nMethod,CDoubleMatrix &matrix_l,CDoubleMatrix &matrix_lambda)
{
	int p = matrix_R.mrow();
	CDoubleMatrix matrix_r = pinv(matrix_R);
	CDoubleMatrix matrix_psi = diag( exp( vector_x ) );

	CDoubleMatrix matrix_psirpsi;
	switch( nMethod )
	{
	case 0:
	case 1:
		matrix_psirpsi = matrix_psi * matrix_r * matrix_psi;
		break;
	case 2:
		matrix_psirpsi = matrix_R - matrix_psi * matrix_psi;
		break;
	}
	matrix_l = (CDoubleMatrix) eig( (CSymDbMatrix) matrix_psirpsi,matrix_lambda);	
	switch( nMethod )
	{
	case 0:
	case 1:
		SortEigenVect(matrix_l,matrix_lambda,1);		
	case 2:
		SortEigenVect(matrix_l,matrix_lambda,0);
		break;
	}
}


CDoubleVector CFactor::DC(const int m,CDoubleMatrix matrix_l,CDoubleMatrix matrix_lambda,const int s,CDoubleVector vector_d,const int nMethod,CDoubleVector vector_x,CDoubleMatrix matrix_r)
{
	int p = matrix_l.mrow();
	int i=0,j=0,k=0;
	CDoubleVector vector_h( p );
	switch( nMethod )
	{
	case 0:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += (1-(1.0/matrix_lambda(k,k))) * matrix_l(i,k) * matrix_l(i,k);
			}
		}
		break;
	case 1:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += matrix_lambda(k,k) * matrix_lambda(k,k) - matrix_lambda(k,k) * matrix_l(k,k) * matrix_l(i,k);
			}
		}
		break;
	case 2:
		for(i=0;i<p;i++)
		{
			vector_h[i] = 0;
			for(k=m;k<p;k++)
			{
				vector_h[i] += matrix_lambda(k,k) * matrix_l(i,k) * matrix_l(i,k);
			}
			vector_h[i] *= 2 * vector_x[i];
		}
	}

	CDoubleMatrix matrix_H;

	switch( s )
	{
	case 1:
		matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,1,nMethod,vector_x,matrix_r);
		break;
	default:
		if( abs(vector_d).Min() > 0.1 )
		{
			matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,1,nMethod,vector_x,matrix_r);
		}
		else
		{
			matrix_H = HC(vector_h,m,matrix_l,matrix_lambda,2,nMethod,vector_x,matrix_r);
		}
		break;		
	}


	vector_d = MatrixToVector ((CDoubleMatrix)(pinv( matrix_H ) * VectorToMatrix(vector_h)));

	
	if( norm( vector_d ) > norm( vector_x ) )
	{
		vector_d = vector_d / ( norm( vector_d ) / sqrt( p ) ) * ( norm(vector_x) / sqrt( p ) );
	}


	return vector_d;
}



CDoubleMatrix CFactor::HC(CDoubleVector vector_h,const int m,CDoubleMatrix matrix_l,CDoubleMatrix matrix_lambda,int nWay,const int nMethod,CDoubleVector vector_x,CDoubleMatrix matrix_r)
{
	int p = matrix_l.mrow();
	int nout(0);
	int i=0,j=0,k=0,n=0;
	CDoubleMatrix matrix_H(p,p);
	CDoubleMatrix delta(p,p);
	while( nout == 0 )
	{
		switch(nWay)
		{
		case 1:
			{
				switch( nMethod )
				{
				case 0:
				case 1:
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							matrix_H(i,j) = 0;
							for(k=m;k<p;k++)
							{
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k);
							}
							matrix_H(i,j) *= matrix_H(i,j);
						}					
					}
					break;
				case 2:
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							matrix_H(i,j) = 0;
							for(k=m;k<m;k++)
							{
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k);
							}
							matrix_H(i,j) = 4 * vector_x[i] * vector_x[j] * matrix_H(i,j) * matrix_H(i,j);
						}
					}
					break;
				}
				
			}
			nout = 1;
			break;
		case 2:
			switch( nMethod )
			{
			case 0:
			case 1:
				for(i=0;i<p;i++)
				{
					for(j=0;j<p;j++)
					{
						delta(i,j) = 0;
						if( i == j )
						{
							delta(i,j) = 1;
						}
						matrix_H(i,j) = delta(i,j) * vector_h[i];
						switch( nMethod )
						{
						case 0:
							for(k=m;k<p;k++)
							{
								double temp = 0;
								for(n=0;n<m;n++)
								{
									temp =( matrix_lambda(k,k) + matrix_lambda(n,n) - 2 ) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,n) * matrix_l(j,n);
								}
								temp += delta(i,j);
								matrix_H(i,j) += matrix_l(i,k) * matrix_l(j,k) * temp;
							}
							break;
						case 1:
							for(k=m;k<p;k++)
							{
								double temp = 0;
								for(n=0;n<m;n++)
								{
									temp = matrix_lambda(n,n) * ( matrix_lambda(k,k) + matrix_lambda(n,n) - 2.0 ) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,n) * matrix_l(j,n);
								}
								temp += matrix_r(i,i) * exp( ( vector_x[i] + vector_x[j] ) / 2 );
								matrix_H(i,j) += matrix_lambda(k,k) * matrix_l(i,k) * matrix_l(j,k) * temp;
							}
							break;
						}
					}
				}
				break;
			case 2:
				{
					for(i=0;i<p;i++)
					{
						for(j=0;j<p;j++)
						{
							delta(i,j) = 0;
							if( i == j )
							{
								delta(i,j) = 1;
							}
							double temp1(0);
							double temp2(0);
							for(k=m;k<p;k++)
							{
								for(n=0;n<m;n++)
								{
									temp1 += (matrix_lambda(k,k) * matrix_lambda(n,n)) / ( matrix_lambda(k,k) - matrix_lambda(n,n) ) * matrix_l(i,k) * matrix_l(j,k);
								}
								temp1 = temp1 + matrix_l(i,k) * matrix_l(j,k) * temp1;
								temp2 = temp2 + ( vector_x[i] * vector_x[i] - matrix_lambda(k,k) / 2 ) * matrix_l(i,k) * matrix_l(i,k);
							}
							matrix_H(i,j) = 4 * ( vector_x[i] * vector_x[j] * temp1 + delta(i,j) * temp2);
						}
					}
					
				}
				break;
			}
			break;
		}
		for(i=0;i<p;i++)
		{
			if( matrix_H(i,i) < 0.05 )
			{
				for(j=0;j<p;j++)
				{
					matrix_H(j,i) = 0;
					matrix_H(i,j) = 0;
				}
				matrix_H(i,i) = 1;
			}
		}
		
		if( nWay == 2 && Definite(matrix_H) == 1 )
			nout = 1;
		else
			nWay = 1;
	}
	return matrix_H;
}


int CFactor::Definite(CDoubleMatrix matrix)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();

	if( nRow != nCol)
	{
// 		AfxMessageBox("Error!");
		return 0;
	}

	CDoubleMatrix matrix_tran = trans( matrix );
	int i=0;
	for(i=0;i<nRow;i++)
	{
		for(int j=0;j<nRow;j++)
		{
			if( !IsZero(matrix_tran(i,j) - matrix(i,j)) )
				return 0;			
		}
	}
	
	for(i=0;i<nRow;i++)
	{
		if(  det( (CDoubleMatrix)slice(matrix,0,0,i+1,i+1) ) <= 0 )
		{
			return 0;
		}
	}

	return 1;
}

//nIndex 1:协方差;0:相关系数
void CFactor::CreateEig(int nIndex)
{
	CDoubleVector Vect_Data = RelationMatrixEx(m_VectData,m_nRow,m_nCol,m_weight,nIndex);
	
	assert(Vect_Data.vlen()>0);
	m_EigenVector.create(m_nCol*m_nCol);
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.000001);
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);
	CreateEigenValueResult(Vect_Data,m_nCol);
	Vect_Data.destruction();
	
}

double CFactor::PCorrelation(const int x1, const int x2, CDoubleVector Vect_R)
{
	int k = m_nCol;				   //x的列数
	int j=0;
	CIntVector Vect_Pos(k);
	Vect_Pos[0] = x1;
	Vect_Pos[1] = x2;
	for(int i=0;i<k;i++)
	{
		if( i != x1 && i != x2 )
		{
			Vect_Pos[j+2] = i;
			j++;
		}
	}
	////TraceVector(Vect_R);

	////TraceVector(Vect_Pos);
	double r = partial(Vect_R,Vect_Pos,k,k);	
	
	return r;
}

//列标准化：

CDoubleVector  CFactor::FormlizeEx(CDoubleVector Vect_Data, CDoubleVector weight)
{
	//权重和
	double w_sum(0);
	int i=0;
	for(i=0;i<weight.vlen();i++)
	{
		w_sum += weight(i);		
	}
	CDoubleVector vector_z(m_nCol);
	//列均
	for(i=0;i<m_nCol;i++)
	{
		double sum(0);
		for(int j=0;j<m_nRow;j++)
		{
			sum += Vect_Data[i*m_nRow+j] * weight(j);
		}
		vector_z(i) = sum / w_sum;
	}
	////TraceVector(vector_z);
	//列方差
	CDoubleVector vector_s2(m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		double sum(0);
		for(int j=0;j<m_nRow;j++)
		{
			sum += weight(j) * ( Vect_Data[i*m_nRow + j] - vector_z(i) ) * ( Vect_Data[i*m_nRow + j] - vector_z(i) );
		}
		vector_s2(i) = sum / ( w_sum - 1 );
	}
	////TraceVector(vector_s2);
	//列标准化
	CDoubleVector Vect_X(m_nRow * m_nCol);
	for(i=0;i<m_nCol;i++)
	{
		for(int j=0;j<m_nRow;j++)
		{
			Vect_X[i*m_nRow+j] = (Vect_Data[i*m_nRow+j] - vector_z(i)) / sqrt(fabs(vector_s2(i))) ;
		}
	}
	return Vect_X;
}
//求解 相关系数矩阵 或 协方差矩阵.
CDoubleVector CFactor::RelationMatrixEx(CDoubleVector Vect_Data,int nRow,int nCol,CDoubleVector weight,int nIndex)
{
	//计算差积
	CDoubleMatrix C(nCol,nCol);
	CDoubleMatrix CC(nCol,nCol);
	CDoubleVector Vect_Relation ( nCol * nCol );
	double w_sum(0);
	int i=0;
	for(i=0;i<weight.vlen();i++)
	{
		w_sum += weight(i);		
	}
	for(i=0;i<nCol;i++)
	{
		for(int j=0;j<nCol;j++)
		{
			if( j >= i )
			{
				double sum(0);
				double sum1(0);
				double sum2(0);

				double sum_c(0);
				double sum_c1(0);
				double sum_c2(0);
				
				double sum_d(0);
				double sum_d1(0);
				double sum_d2(0);

				double sum_weight = 0;

				for(int k=0;k<nRow;k++)
				{
					bool bInput(false);
					switch(m_nMissingMethod)
					{
					case 0:
						{
							bInput = true;						
						}
						break;
					case 1:
						{
							if( !IsMissing(k,i) && !IsMissing(k,j) )
							{
								bInput = true;							
							}					
						}
						break;
					case 2:
						{
							bInput = true;							
						}
						break;
					}
					if( bInput )
					{
						sum  += weight(k) * Vect_Data[i*nRow+k]* Vect_Data[j*nRow+k];
						sum1 += weight(k) * Vect_Data[i*nRow+k];
						sum2 += weight(k) * Vect_Data[j*nRow+k];
						
						sum_c  += weight(k) * Vect_Data[i*nRow+k]* Vect_Data[i*nRow+k];
						sum_c1 += weight(k) * Vect_Data[i*nRow+k];
						sum_c2 += weight(k) * Vect_Data[i*nRow+k];
						
						sum_d  += weight(k) * Vect_Data[j*nRow+k]* Vect_Data[j*nRow+k];
						sum_d1 += weight(k) * Vect_Data[j*nRow+k];
						sum_d2 += weight(k) * Vect_Data[j*nRow+k];
						
						sum_weight += weight(k);
					}
				}
				C(i,j) = sum - ( sum1 * sum2 / sum_weight ) ;
				CC(i,j) = sum_c - ( sum_c1 * sum_c2 / sum_weight ) ;
				CC(j,i) = sum_d - ( sum_d1 * sum_d2 / sum_weight ) ;
				double dTemp;
				switch( nIndex )
				{
				case 0:
					//相关系数
					dTemp = sqrt(fabs(CC(i,j) * CC(j,i)));
					Vect_Relation[i*nCol+j] = C(i,j) / dTemp;
					break;
				case 1:
					//协方差
					Vect_Relation[i*nCol+j] = C(i,j) / (sum_weight - 1);
					break;
				}				
			}
			else if ( i == j && nIndex == 0 )
			{
				 Vect_Relation[i*nCol+j] = 1;			
			}
			else if( i > j )
			{
				Vect_Relation[i*nCol+j] = Vect_Relation[j*nCol+i] ;
			}
		}	
	}

	m_VectCorrelation.create(nCol*nCol);
    m_VectCorrelation = copy(Vect_Relation);
	//TraceVector(m_VectCorrelation);
	return Vect_Relation;
}








// 相关系数 或 协方差
CDoubleMatrix CFactor::RelationMatrixEx(CDoubleMatrix matrix, int nIndex)
{
	int nRow = matrix.mrow();
	int nCol = matrix.mcol();
	CDoubleVector Vect_Data(nRow * nCol);
//	Vect_Data << matrix;
	Vect_Data = MatrixToVector( matrix);
	CDoubleVector Vect_Relation = RelationMatrixEx(Vect_Data,nRow,nCol,m_weight,nIndex);
	CDoubleMatrix matrix_R=VectorToMatrix(Vect_Relation,nCol,nCol);
	return matrix_R;
}


bool CFactor::IsMissing(int nRow, int nCol)
{
	nRow++;
	int ncount = m_pDataInterface->GetRowCount();
	long nt = m_pDataInterface->m_DataAccess.GetRecNo();
	m_pDataInterface->m_DataAccess.SetCurRecNo(nRow);
	CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nCol);
	bool btemp = pField->IsNull();
	m_pDataInterface->m_DataAccess.SetCurRecNo(nt);
	return btemp;
}

bool CFactor::IsMissing(int nRow)
{
	nRow++;

	m_pDataInterface->m_DataAccess.SetCurRecNo(nRow);

	int nweightcol = m_pDataInterface->GetWeightNumb();

	if(m_nProcType)
	{
		for(int i=0;i<m_tVarInfo.iCount;i++)
		{
			int nt = m_tVarInfo.pValue[i];
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nt);
			if( pField->IsNull() )
			{
				return true;
			}
			if( nweightcol >= 0 )
			{
				 pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nweightcol);
				 if( pField->IsNull() )
				 {
					 return true;
				 }
			}
		}
	}
	else
	{
		for(int i=0;i<m_tVarInfo2.iCount;i++)
		{
			int nt = m_tVarInfo2.pValue[i];
			CField * pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nt);
			if( pField->IsNull() )
			{
				return true;
			}
			if( nweightcol >= 0 )
			{
				 pField = m_pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nweightcol);
				 if( pField->IsNull() )
				 {
					 return true;
				 }
			}
		}
	}
	return false;
}

/************************************************************************************
函数名称：

功能说明：
	重载
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2003-1-25
修改:

************************************************************************************/
CDoubleVector  CFactor::GetData(CDataInterface *pDataInterface,TIntStruct tVarInfo,int bRemoveEqual)
{
		m_arrRecNo.RemoveAll();
        CDoubleVector Vect_Data;
        //取回数据，组织输入数据矩阵
        int nCaseNumb,m = tVarInfo.iCount +1;
        TDColAry * TDataArray=NULL;
        int nColNum=0;
        //数据检验
		//{{{START 2003-1-25 Eric
		//指示列是否相同的变量
		CTArray<double,double>  FirstValueArray;
		bool bHasValue = false;
		CTArray<int,int>  IsSameArray;  
		
		//导出数据
		int nWeightCol = pDataInterface->GetWeightNumb();
		CDataInterface *pTempDataInterface = new CDataInterface();
		CFieldList * pTempFieldList = pDataInterface->m_DataAccess .m_pFieldList;

		int i = 0;
		for(i = 0;i<pTempFieldList ->GetFieldCount();i++)
		{
			CField* pField = pTempDataInterface->m_DataAccess .m_pFieldList->CreateField(pTempFieldList->FieldByIndex(i));
			pField->SetFieldName(pTempFieldList->FieldByIndex(i)->GetFieldName());
			pTempDataInterface->m_DataAccess .m_pFieldList->Add(pField);
			FirstValueArray.Add(0);
			IsSameArray.Add(true);
		}

		//把没有NULL值的行导出. 
		pDataInterface->m_DataAccess .First();
		while(!pDataInterface->m_DataAccess .Eof())
		{
			bool bAppend = true;
/*			for(int j = 0;j<pDataInterface->GetColCount();j++)
			{
				if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->IsNull())
				{
					bAppend = false;
					break;
				}
			}
*/
			//非空,才导出
			int j = 0;
			for(j = 0;j<tVarInfo.iCount;j++)
			{
				if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex
					(tVarInfo.pValue[j])->IsNull())
				{
					bAppend = false;
					break;
				}
			}

			//权重为正数,才导出.
			CField * pField = pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(nWeightCol) ;
			if((nWeightCol>=0) && (pField->IsNull() || pField->GetAsDouble()<= 0))
			{
					bAppend = false;
			}

			if(bAppend)
			{
				pTempDataInterface->m_DataAccess.Append();

				m_arrRecNo.Add(pDataInterface->m_DataAccess.GetRecNo());

				int j = 0;
				for(j = 0;j<pDataInterface->GetColCount();j++)
				{
					if(pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->IsNull())
					{
						pTempDataInterface->m_DataAccess .m_pFieldList->FieldByIndex(j)->SetNull();
					}
					else
					{
						pTempDataInterface->m_DataAccess .m_pFieldList->FieldByIndex(j)->SetAsDouble(
							pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j)->GetAsDouble());
					}
				}
				pTempDataInterface->m_DataAccess.Post();

				//是否列值相同
				if(!bHasValue)
				{
					for(int j = 0;j<tVarInfo.iCount;j++)
					{
						FirstValueArray[tVarInfo.pValue[j]] = pDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(tVarInfo.pValue[j])->GetAsDouble();
					}
					bHasValue = true;
				}
				else
				{
					for(int j = 0;j<tVarInfo.iCount;j++)
					{
						if(fabs(FirstValueArray[tVarInfo.pValue[j]] 
							- pDataInterface->m_DataAccess.m_pFieldList->
							FieldByIndex(tVarInfo.pValue[j])->GetAsDouble()) > 1e-8
							)
						{//不相等
							IsSameArray[tVarInfo.pValue[j]] = false;
						}
					}
				}

			}
			pDataInterface->m_DataAccess.Next();
		}	   

		nCaseNumb = pTempDataInterface->GetRowCount();
		if(nCaseNumb < 2)
		{
//             AfxMessageBox("样本数太少，无法进行分析!");
			delete pTempDataInterface;pTempDataInterface = NULL;
            return NULL;
		}

		//看是否有相同列
		int j = 0;
		for(j = 0;j<tVarInfo.iCount;j++)
		{
			if(IsSameArray[tVarInfo.pValue[j]] == true && tVarInfo.pValue[j] != nWeightCol)
			{
//                     AfxMessageBox("至少有一列数据方差为零，无法进行分析!");
					delete pTempDataInterface; pTempDataInterface = NULL;
					return NULL;
			}
		}

		//}}}END  2003-1-25 Eric


		//{{{START 2003-1-26 Eric===================>
		/*
        for( i=0;i<tVarInfo.iCount;i++)
        {
                nColNum=tVarInfo.pValue[i];
                if( nColNum < 0 )
                {
                        AfxMessageBox("取数据时出错!");
						delete pTempDataInterface;
                        return NULL;
                }
                nCaseNumb = pTempDataInterface->GetColData(nColNum,pDataArray,1);//取回数据
                assert(nCaseNumb > 0 );
		//{{{START 2003-1-25 Eric
                //if(bRemoveEqual && IsEqual(pDataArray,nCaseNumb))
                if(bRemoveEqual && IsEqual(pDataArray,nCaseNumb)&&(nWeightCol != nColNum))
		//}}}END  2003-1-25 Eric
                {
		//{{{START 2003-1-25 Eric
                    //表示所有数据相同，应该除去
                    //tVarInfo.RemoveAt(i);
                    //i--;
			//某列的值相同报错!!
                    AfxMessageBox("不合理的数据--列的值相同!");
					delete pTempDataInterface;
					return NULL;
		//}}}END  2003-1-25 Eric
                }
        }
		*/
		//}}}END  2003-1-26 Eric<=====================

        m = tVarInfo.iCount;
		Vect_Data.create( pTempDataInterface->GetRowCount() * m );
        for(i=0;i<m;i++)
        {
            //if( !Vect_Data  )
                    //Vect_Data.create( pTempDataInterface->GetRowCount() * m );
            
            nColNum=tVarInfo.pValue[i];
            if( nColNum < 0 )
            {
//                     AfxMessageBox("取数据时出错!");
					delete pTempDataInterface; pTempDataInterface = NULL;
                    return NULL;
            }
			TDataArray = new TDColAry;
            nCaseNumb = pTempDataInterface->GetColData(nColNum,*TDataArray,1);//取回数据
            assert(nCaseNumb > 0 );
            //赋值
            //检查输入的数据，如果样本值相同就剔除
            for(int j=0;j<nCaseNumb;j++)
                    Vect_Data[i*nCaseNumb+j] = TDataArray->GetAt(j).dValue;
            if( TDataArray )
            {
                 delete TDataArray;
                 TDataArray=NULL;
            }
        }    
		//{{{START 2003-1-25 Eric
		/*
        if ( tVarInfo.iCount <= 0 )
        {
//                 AfxMessageBox("你选择的数据无效!");
                return NULL;
        }
		*/

		m_nRow = nCaseNumb;
		pTempDataInterface->SetWeightNumb(nWeightCol);
		m_weight = GetWeightVector(pTempDataInterface);//权重
		////TraceVector(m_weight);
		m_pDataInterface = pTempDataInterface;
		////TraceVector(Vect_Data);
        return Vect_Data;
}

/************************************************************************************
函数名称：

功能说明：
   计算主成分 
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2003-2-9
修改:

************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
bool CFactor::PrincipalComponent()
{	
	CDoubleVector Vect_Temp (m_nRow * m_nSel);

	//输出因子矩阵End<<<<<<<<<<<<<<
	////TraceVector(m_VectDataFormlized);
	////TraceVector(m_SelEigenVector);
	MatrixMultiply(m_VectDataFormlized,m_SelEigenVector,m_nRow,m_nCol,m_nSel,Vect_Temp);
	m_VectScore = copy(Vect_Temp);
	//TraceVector(m_VectScore);
	return true;
}


/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2003-2-9
修改:modified by zkc 2003.9.8.将输出结果直接加到主界面表中。

************************************************************************************/
void CFactor::PrincipalTable(CResult * pResult)
{
/*
	int i;
	CTLTable * pTable = NULL;
	pTable = new CTLTable();
	pTable->SetTitle("主成分分析");
	pTable->SetRows(m_nRow);
	pTable->SetCols(3);
	pTable->InsertColumn(0,"记录号");
	pTable->InsertColumn(1,"权重");

	char szNum1[16];
	char szNum2[10];
	strcpy(szNum1,"主成分");

	for(i = 0;i < m_nSel;i++)
	{
		pTable->InsertColumn(i + 2,strcat(szNum1,itoa(i+1,szNum2,10)));
		szNum1[6] = 0;
	}
	int nRecNo = 1;
	m_pOriDataInterface->m_DataAccess .First();
	for(i = 0;i<m_nRow;)
	{
		while(nRecNo < m_arrRecNo[i])
		{
			pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
			pTable->SetItemText(nRecNo,1,0);
			
			for(int j = 0;j<=m_nSel;j++)
			{
				pTable->SetItemText(nRecNo,j+2,"缺失");
			}
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}
		
		pTable->InsertItem(nRecNo,itoa(nRecNo,szNum2,10));
		pTable->SetItemText(nRecNo,1,m_weight(i));
		for(int j = 0;j<m_nSel;j++)
		{
			pTable->SetItemText(nRecNo,j+2,m_pScore[i*m_nSel+j]);
		}
		i++;
		m_pOriDataInterface->m_DataAccess.Next();
		nRecNo++;
	}
	pTable->SetTail("注:权重为0表示样本或权重中有缺失值,或权重小于等于0");
	CRsltObject * pETable= new CRsltElementTable("主成分分析",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
*/
/*

	TDataPointForScatter* pDPVV =new TDataPointForScatter;        
	
	for(int  k=0; k<m_nRow; k++)
	{
		pDPVV->XAxisArr.Add(m_VectScore[k]);
		pDPVV->YAxisArr.Add(m_VectScore[k+m_nRow]);
	}
	CTChartScatterPlot* pCL=new CTChartScatterPlot(NULL,pDPVV,m_nRow,0,"载荷图","第一主成分","第二主成分");
	CRsltElementChart *pREC=new CRsltElementChart("载荷图",pCL);
	delete pDPVV;
	pResult->Add(pREC);
*/	
/////////////
	int nOrgColNums=m_pOriDataInterface->m_DataAccess.m_pFieldList->GetFieldCount();

	int i=0;
	for(i=0;i<m_nSel;i++)
	{
		CTString strFieldName;
		CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->CreateField(fDouble);
		strFieldName.Format("主成分%d",i+1);
		while(IsExisted(m_pOriDataInterface,strFieldName))
		{
			strFieldName+="_1";
		}
		pField->SetFieldName(strFieldName);
		m_pOriDataInterface->m_DataAccess.m_pFieldList->Add(pField);
	}

	int nRecNo = 1;
	m_pOriDataInterface->m_DataAccess .First();

	for(i = 0;i<m_nRow;)
	{
				

		while(nRecNo < m_arrRecNo[i])
		{
			m_pOriDataInterface->m_DataAccess.Edit();
			for(int j = 0;j<m_nSel;j++)
			{
				CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
				pField->SetNull();
				
			}
			m_pOriDataInterface->m_DataAccess.Post();			
			m_pOriDataInterface->m_DataAccess.Next();
			nRecNo++;
		}
		m_pOriDataInterface->m_DataAccess.Edit();	
		for(int j = 0;j<m_nSel;j++)
		{
			CField *pField=m_pOriDataInterface->m_DataAccess.m_pFieldList->FieldByIndex(j+nOrgColNums);
			pField->SetAsDouble(m_VectScore[i+j*m_nRow]);
		}
		i++;
		m_pOriDataInterface->m_DataAccess.Post();			
		m_pOriDataInterface->m_DataAccess.Next();
		nRecNo++;
	}
}




/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2003-2-9
修改:

********************************************************/
void CFactor::FromCorrelationToCommunality(CDoubleVector Vect_Data)
{  
	int	i;
	//求解共性方差矩阵
	CDoubleMatrix R= VectorToMatrix(Vect_Data,m_nCol,m_nCol);
	
	CDoubleMatrix RR = pinv(R);
	

	m_VectCommunality.create( m_nCol);
	for(i = 0;i<m_nCol;i++)
	{
		m_VectCommunality[i] = R(i,i);
	}
	for( i = 0;i<m_nCol;i++) //因子荷载
	{
		double psi = 1/RR(i,i);
		m_VectCommunality[i] -= psi;
	}
	//TraceVector(m_VectCommunality);
}
/************************************************************************************
函数名称：

功能说明：
	主因子法中,第一个共性方差矩阵直接由相关系数矩阵计算得到.
	(以后的共性方差矩阵由特征根,特征向量计算得到)
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2003-2-9
修改:

***************************************************************************************/
void CFactor::CalcFirstCommunalityAndEig(CDoubleVector Vect_Data,int nIndex)
{
	assert(Vect_Data.vlen()>0);
	
	//求解共性方差矩阵 
	FromCorrelationToCommunality(Vect_Data);//FromcovarianceToCommunality.
	
	int i;
	m_EigenVector.create(m_nCol*m_nCol);
	cjcbj(Vect_Data,m_nCol,m_EigenVector,0.00001);//求解特征向量m_pEigenVector
	SortEigenVect(Vect_Data,m_EigenVector,m_nCol,m_nCol,1);//特征向量排序m_pEigenVector
	m_VectEigenValue.create(m_nCol);
	for(i =0;i<m_nCol;i++)
		m_VectEigenValue[i] = Vect_Data[i+i*m_nCol];
	//TraceVector(m_VectEigenValue);
	CreateEigenValueResult(Vect_Data,m_nCol);//输出特征值
	
	m_nSel = GetSelNum(m_VectEigenValue);	//计算符合要求的特征值的个数
}
int CFactor::GetSelNum(CDoubleVector Vect_Data)
{
	double sum(0);
	int i=0;
	for(i=0;i<m_nCol;i++)
	{
		sum += Vect_Data[i];
	}
	int nSel=0;
	double pro = 0;
	for(i=0;i<m_nCol;i++)
	{	
		if( ( pro < m_fPro && m_nExtractStandard == 0)
			||( nSel < m_nFactorNum && nSel < m_nCol  && m_nExtractStandard == 1)			
			)
		{	
			pro += Vect_Data[i]/sum;
			nSel++;
		}
		//为regression因子得分做的检查
		if( Vect_Data[i] <= 1e-8 )
		{
			m_bEigenFlag = true;
		}
	}
	return nSel;
}
CDoubleVector CFactor::GetOratedExplains(CDoubleMatrix MatrixA)//得到旋转后的荷载矩阵之后计算其方差贡献
{
	int nCol = MatrixA.mcol();
	CDoubleVector Ret(nCol);
	if (nCol > 0)
	{
		int nRow = MatrixA.mrow();
		CDoubleVector temp;
		int i =0;
		for(i =0;i<nCol;i++)
		{
			temp= MatrixA.col(i);
			Ret[i]=dot(temp,temp);
		}
	}
	return Ret;
}

void CFactor::CreateRotedExplainTable()
{
//	assert(m_VectRotEigenValue.vlen());
//	int nCol = m_VectRotEigenValue.vlen();
//	int i;
//	CTLTable * pTable = NULL;
//	pTable = new CTLTable();
//	pTable->SetTitle("旋转后各因子的方差贡献表");
////	pTable->SetRows(2);
////	pTable->SetCols(nCol+1);
//
//	CTString sTr("");
//	pTable->InsertColumn(0,sTr);
//	pTable->InsertItem(1,"方差贡献");
//	for(i =1;i<nCol+1;i++)
//	{
//		sTr.Format("因子%d",i);
//		pTable->InsertColumn(i,sTr);
//		pTable->SetItemText(1,i,m_VectRotEigenValue[i-1]);
//	}	
//	//pTable->SetTail("注:该表列出了旋转后各因子向量对方差的贡献");
//	CRsltObject * pETable= new CRsltElementTable("旋转后各因子对方差的贡献",pTable);//申请一个表对象类的对象
//	m_pResult->Add(pETable);
//
}

void CFactor::CreateSquareSumExplainTable()//输出因子荷载平方和
{
	assert(m_VectEigenValue.vlen());
	int nCol = m_VectEigenValue.vlen();
	int i;
	CTLTable * pTable = NULL;
	pTable = new CTLTable();
	CTString sTr("");
	sTr.Format("因子荷载平方和 ");	
	pTable->SetTitle(sTr);
//	pTable->SetRows(2);
//	pTable->SetCols(nCol+1);

	sTr.Empty();
	pTable->InsertColumn(0,sTr);
	pTable->InsertItem(1,"方差贡献");
	for(i =1;i<nCol+1;i++)
	{
		sTr.Format("因子%d",i);
		pTable->InsertColumn(i,sTr);
		pTable->SetItemText(1,i,m_VectEigenValue[i-1]);
	}	
	//pTable->SetTail("注:该表列出了各因子向量负载的方差的贡献");
	CRsltElementTable * pETable= new CRsltElementTable("因子荷载平方和",pTable);//申请一个表对象类的对象
	m_pResult->Add(pETable);
	
}
void CFactor::OnFactorExit() //计算完毕，清理变量的内存空间。
{
	if(m_VectUnAdjustCom.vlen())
		m_VectUnAdjustCom.destruction();

	if(m_VectEigenValue.vlen())
		m_VectEigenValue.destruction();

	if(m_VectSelEigenValue.vlen())
		m_VectSelEigenValue.destruction();

	if(m_VectData.vlen())
		m_VectData.destruction();

	if(m_VectRotEigenValue.vlen())
		m_VectRotEigenValue.destruction();

	if(m_EigenVector.vlen())
		m_EigenVector.destruction();

	if(m_VectComMatrix.vlen())
		m_VectComMatrix.destruction();

	if(m_VectCommunality.vlen())
		m_VectCommunality.destruction();

	if(m_VectCorrelation.vlen())
		m_VectCorrelation.destruction();

	if(m_VectPreCommunality.vlen())
		m_VectPreCommunality.destruction();

	if(m_VectScore.vlen()>0)
		m_VectScore.destruction();
	
	if(m_VectDataFormlized.vlen()>0)
		m_VectDataFormlized.destruction();

	if(m_VectFirstCovariance.vlen()>0)
		m_VectFirstCovariance.destruction();
	
	if(m_InitCommunality.vlen()>0)
		m_InitCommunality.destruction();	
	m_bKMO =true;
}

int CFactor::brinv(CDoubleVector& a,int n)
{ 
	int i,j,k,l,u,v;
	CIntVector is(n);
	CIntVector js(n);
	double d,p;
	
	for (k=0; k<=n-1; k++)
	{ 
		d=0.0;
		for (i=k; i<=n-1; i++)
			for (j=k; j<=n-1; j++)
			{ l=i*n+j; p=fabs(a[l]);
			if (p>d) { d=p; is[k]=i; js[k]=j;}
			}
			if (d+1.0==1.0)
			{ //free(is); free(js); printf("err**not inv\n");
				//		  delete is;delete js;
				is.destruction();
				js.destruction();
				return(0);
			}
			if (is[k]!=k)
				for (j=0; j<=n-1; j++)
				{ u=k*n+j; v=is[k]*n+j;
				p=a[u]; a[u]=a[v]; a[v]=p;
				}
				if (js[k]!=k)
					for (i=0; i<=n-1; i++)
					{ u=i*n+k; v=i*n+js[k];
					p=a[u]; a[u]=a[v]; a[v]=p;
					}
					l=k*n+k;
					a[l]=1.0/a[l];
					for (j=0; j<=n-1; j++)
						if (j!=k)
						{ u=k*n+j; a[u]=a[u]*a[l];}
						for (i=0; i<=n-1; i++)
							if (i!=k)
								for (j=0; j<=n-1; j++)
									if (j!=k)
									{ u=i*n+j;
									a[u]=a[u]-a[i*n+k]*a[k*n+j];
									}
									for (i=0; i<=n-1; i++)
										if (i!=k)
										{ u=i*n+k; a[u]=-a[u]*a[l];}
	}
	for (k=n-1; k>=0; k--)
	{ if (js[k]!=k)
	for (j=0; j<=n-1; j++)
	{ u=k*n+j; v=js[k]*n+j;
	p=a[u]; a[u]=a[v]; a[v]=p;
	}
	if (is[k]!=k)
		for (i=0; i<=n-1; i++)
		{ u=i*n+k; v=i*n+is[k];
		p=a[u]; a[u]=a[v]; a[v]=p;
		}
	}
	//  is.destruction();
	//   js.destruction();
	
	//delete js;
	return(1);
}



