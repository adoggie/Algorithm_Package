// RBFN_Algorithm.cpp: implementation of the RBFN_Algorithm class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <assert.h>
#include "RBFN_Algorithm.h"
#include <time.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MININTERVAL 1e-30

CRBFNetOutput::CRBFNetOutput()
{
}

bool CRBFNetOutput::Export(string szFileName)
{
	return true;
}

bool CRBFNetOutput::Import(string szFileName)
{
	return true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRBFN_Algorithm::CRBFN_Algorithm()
{
	m_bHasNetStructure=false;
	m_bAbort = false;
	m_bError=false;
	m_strAbortInfo="";
	m_strErrorInfo="";	
}

CRBFN_Algorithm::~CRBFN_Algorithm()
{

}


//bHasMaxMin==true则直接用mxColMaxMin标准化mx
//bHasMaxMin==false则先从mx中获得各列最大最小值放入mxColMaxMin，再标准化mx
//mxColMaxMin的第0行存放最大值，第1行存放最小值
//可考虑用均值方差标准化，效果可能会更好
bool CRBFN_Algorithm::MxStandardize(CDoubleMatrix& mx, CDoubleMatrix& mxColMaxMin, 
									CDoubleMatrix& mxStd, bool bHasMaxMin)
{
	int i,j;
	const int nRow=mx.mrow();
	const int nCol=mx.mcol();
	
	if(bHasMaxMin==false)
	{
		//获得最大最小值作为下面标准化的参数
		mxColMaxMin.create(2,nCol);
		for(j=0;j<nCol;j++)
		{
			mxColMaxMin(0,j)=mx(0,j);//初始化最大值
			mxColMaxMin(1,j)=mx(0,j);//初始化最小值
			for(i=1;i<nRow;i++)
			{
				if(mx(i,j)>mxColMaxMin(0,j))
					mxColMaxMin(0,j)=mx(i,j);
				else if(mx(i,j)<mxColMaxMin(1,j))
						mxColMaxMin(1,j)=mx(i,j);
			}
		}
	}
		
	mxStd.create(nRow,nCol);
	double dInterval;
	for(j=0;j<nCol;j++)
	{
		dInterval=mxColMaxMin(0,j)-mxColMaxMin(1,j);
		if(dInterval<MININTERVAL)
			continue;//原则上mxColMaxMin(0,j)本身应该远大于MININTERVAL
		
		for(i=0;i<nRow;i++)
		{
			mxStd(i,j)=(mx(i,j)-mxColMaxMin(1,j))/dInterval;
		}
	}
	
	return true;
}

bool CRBFN_Algorithm::MxInverseStandardize(CDoubleMatrix& mxStd, 
										   CDoubleMatrix& mxColMaxMin, CDoubleMatrix& mx)
{
	int i,j;
	int nRow,nCol;
	nRow=mxStd.mrow();
	nCol=mxStd.mcol();

	mx.create(nRow,nCol);	
	double dInterval;
	for(j=0;j<nCol;j++)
	{
		dInterval=mxColMaxMin(0,j)-mxColMaxMin(1,j);
		if(dInterval<MININTERVAL)
			continue;
		for(i=0;i<nRow;i++)
		{
			mx(i,j)=mxStd(i,j)*dInterval+mxColMaxMin(1,j);
		}
	}
	return true;
}

bool CRBFN_Algorithm::GetHideCenter(CDoubleMatrix& mxStd, int nCenterNum, 
									CDoubleMatrix& mxStdCenter)
{
	//随机取中心
/*
		srand((unsigned)time( NULL ));
			int *num=new int[nCenterNum];
			int i=0,j;
			while(i<nCenterNum)
			{
				num[i]=int(double(rand())/RAND_MAX*mxStd.mrow());			
				for(j=0;j<i;j++)
				{
					if(num[i]==num[j])
					{
						i--;
						break;
					}
				}
				i++;
			}
			mxStdCenter.create(nCenterNum,mxStd.mcol());
			for(i=0;i<nCenterNum;i++)
			{
				for(j=0;j<mxStd.mcol();j++)
				{
					mxStdCenter(i,j)=mxStd(num[i],j);
				}
			}
*/

	//快速聚类获取隐含层中心，这样构成的RBF网络推广能力比较好
	//迭代结束条件：最大迭代次数和两次迭代最大误差固定
	if(!m_KMeans.KMeans(mxStd,nCenterNum,true,1e-20,200,mxStdCenter))
	{
		if(m_KMeans.IsError()) 
		{
			m_strErrorInfo="RBF神经网络错误信息：获取隐含层中心错误！\n"
								+string("---->>")+m_KMeans.GetErrorInfo();
			m_bError=true;
			return false;
		}
		else if(m_KMeans.IsAbort()) 
		{
			m_strAbortInfo+="RBF神经网络中止信息：获取隐含层中心中止！\n"
								+string("---->>")+m_KMeans.GetAbortInfo();
			return false;
		}
		else assert(false);
	}		

	return true;
}

bool CRBFN_Algorithm::CalculateSigmaSquare(CDoubleMatrix& mxStdCenter,CDoubleMatrix& mxSigmaSquare)
{
	int i,j,k;
	int nCenterNum=mxStdCenter.mrow();
	assert(nCenterNum>=2);
	int nCol=mxStdCenter.mcol();
	CDoubleMatrix mxDistSquare(nCenterNum,nCenterNum,0.0);
	CDoubleMatrix mxDistSquareDelDiag(nCenterNum,nCenterNum-1,0.0);//去除mxDistSquare对角线上0距离
	double dMin1,dMin2; //最小值和次最小值

	//求RBF基函数的宽度SigmaSquare=离各中心点最近的两个中心点距离平方的平均值
	for(i=0;i<nCenterNum;i++)
	{
		for(j=i+1;j<nCenterNum;j++)
		{
			for(k=0;k<nCol;k++)
			{
				mxDistSquare(i,j)+=(mxStdCenter(i,k)-mxStdCenter(j,k))
									*(mxStdCenter(i,k)-mxStdCenter(j,k));
			}
			mxDistSquare(j,i)=mxDistSquare(i,j);
		}	
	}
	for(i=0;i<nCenterNum;i++)
	{
		k=0;
		for(j=0;j<nCenterNum;j++)
		{
			if(i==j)continue;
			mxDistSquareDelDiag(i,k)=mxDistSquare(i,j);
			k++;
		}		
	}

	mxSigmaSquare.create(nCenterNum,1);		
	if(nCenterNum==2)
	{
		mxSigmaSquare(0,0)=mxDistSquareDelDiag(0,0);
		mxSigmaSquare(1,0)=mxDistSquareDelDiag(1,0);
	}
	else
	{
		for(i=0;i<nCenterNum;i++)
		{
			if(mxDistSquareDelDiag(i,0)<mxDistSquareDelDiag(i,1))
			{
				dMin1=mxDistSquareDelDiag(i,0);
				dMin2=mxDistSquareDelDiag(i,1);
			}
			else 
			{
				dMin1=mxDistSquareDelDiag(i,1);
				dMin2=mxDistSquareDelDiag(i,0);
			}
			
			for(j=2;j<nCenterNum-1;j++)
			{
				if(mxDistSquareDelDiag(i,j)<dMin1)
				{
					dMin2=dMin1;
					dMin1=mxDistSquareDelDiag(i,j);
				}
				else if(mxDistSquareDelDiag(i,j)<dMin2)
				{
					dMin2=mxDistSquareDelDiag(i,j);
				}
			}
			mxSigmaSquare(i,0)=(dMin1+dMin2)/2;
		}
	}
	
	return true;
}

bool CRBFN_Algorithm::CalculateWeight(CDoubleMatrix& mxX,CDoubleMatrix& mxY,CDoubleMatrix& mxWeight)
{
	//调用求广义逆函数pinv()，可处理矩阵奇异问题，另有一求逆函数inv()不能处理奇异问题
	//CDoubleMatrix按列存储，对于记录数很多的数据，不宜用转置运算，用dot()来处理
	//直接调用mxX.pinv()由于按列存储的问题当矩阵行数很多速度会较慢，因为其内部计算时定义了转置矩阵
	CDoubleMatrix tempM;
	tempM=mxX.dot(mxX); //mxX'*mxX 
	tempM=tempM.pinv();
	tempM=mxX*(tempM.trans()); 
	mxWeight=tempM.dot(mxY);


//////////////////////////////////////////////////////////////////////////
//	岭回归中求广义逆方法，可处理矩阵奇异问题,列数多的时候速度慢
//	mxWeight=RidgeEstimate(mxX,mxY,0);

	return true;
}

//网络结构训练。多线程调用时可以通过查询m_strAbortInfo了解当前训练状态
bool CRBFN_Algorithm::Train(CDoubleMatrix& mxInput, CDoubleMatrix& mxTarget, int nCenterNum, 
		   CDoubleMatrix& mxOutput, double& dMSE, double& dStdMSE)
{	
	//开始计时
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	//数据检查/////////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxInput.mrow()<1)
	{
		m_strErrorInfo="RBF神经网络错误信息：没有输入数据！";
		m_bError=true;
		return false;
	}
	if(mxInput.mrow()!=mxTarget.mrow())
	{
		m_strErrorInfo="RBF神经网络错误信息：输入数据的记录数与目标数据的记录数不一致！";
		m_bError=true;
		return false;
	}
	if(nCenterNum<2)
	{
		m_strErrorInfo="RBF神经网络错误信息：指定隐含层中心数应大于等于2！";
		m_bError=true;
		return false;
	}
	if(mxInput.mrow()<nCenterNum)
	{
		m_strErrorInfo="RBF神经网络错误信息：指定隐含层中心数应小于等于输入数据的记录数！";
		m_bError=true;
		return false;
	}
	
	//开始计算///////////////////////////////////////////////////////////////////////
	m_bAbort=false;
	m_bHasNetStructure=false;
	
	CDoubleMatrix mxStdInput,mxStdTarget,mxHideOut,mxStdOutput;

	m_RBFNS.nCenterNum=nCenterNum;
	m_RBFNS.nInputCol=mxInput.mcol();
	m_strAbortInfo="RBF神经网络开始训练...\n";
	string strStop="RBF神经网络训练中止!\n";
	
	//标准化
	if(!m_bAbort)
	{
		MxStandardize(mxInput,m_RBFNS.mxInputColMaxMin,mxStdInput,false);
		MxStandardize(mxTarget,m_RBFNS.mxTargetColMaxMin,mxStdTarget,false);
		m_strAbortInfo+="RBF神经网络训练：步骤一完毕;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//得到隐含层中心
	if(!m_bAbort)
	{
		if(GetHideCenter(mxStdInput,nCenterNum,m_RBFNS.mxStdCenter))
		{
			m_strAbortInfo+="RBF神经网络训练：步骤二完毕;\n";
		}
		else
		{
			if(m_KMeans.IsError())
				return false;
			else if(m_KMeans.IsAbort())
				return false;
			else assert(false);			
		}
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}

	//计算RBF基函数的宽度
	if(!m_bAbort)
	{
		CalculateSigmaSquare(m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare);
		m_strAbortInfo+="RBF神经网络训练：步骤三完毕;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//求中间层输出
	if(!m_bAbort)
	{
		CalculateHideOut(mxStdInput,m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare,mxHideOut);
		m_strAbortInfo+="RBF神经网络训练：步骤四完毕;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}

	//求权值
	if(!m_bAbort)
	{
		CalculateWeight(mxHideOut,mxStdTarget,m_RBFNS.mxWeight);
		m_strAbortInfo+="RBF神经网络训练：步骤五完毕;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//至此网络结果已获得
	m_bHasNetStructure=true;
	m_strAbortInfo+="RBF神经网络结构已训练完毕!\n开始计算训练误差...\n";
			
	//求mse
	if(!m_bAbort)
	{
		mxStdOutput=mxHideOut*m_RBFNS.mxWeight;
		dStdMSE=MSE(mxStdTarget,mxStdOutput);
		MxInverseStandardize(mxStdOutput,m_RBFNS.mxTargetColMaxMin,mxOutput);
		dMSE=MSE(mxTarget,mxOutput);
		m_strAbortInfo+="RBF神经网络训练：计算训练误差完毕!\n";	
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;		
	}

	m_strAbortInfo+="RBF神经网络训练正常结束!\n";	
	

	//结束计时
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"训练样本数："<<mxInput.mrow()<<"  "<<"输入变量数："<<mxInput.mcol()<<"  "
		<<"隐含层中心数："<<nCenterNum<<"\n"
		<<"训练总时间："<<FormatTimeSpan(timeBegin,timeEnd)<<"\n"
		<<"其中获取隐含层中心："+m_KMeans.GetRunInfo();	
	m_strRunInfo=oss.str();
	
	return true;
}


bool CRBFN_Algorithm::SaveRBFNS(string& strRBFNS)
{
	//如果不存在网络结构则返回false
	m_bError=false;
	if(!HasNetStructure())
	{
		m_strErrorInfo="RBF神经网络错误信息：网络结构不存在！";
		m_bError=true;
		return false;
	}	
	
	ostringstream ossRBFN;
	int i,j;
	
	//设置精度，最大限度减少保存数值到字符串的精度损失
	ossRBFN.precision(30);
	
	//保存隐含层中心数
	ossRBFN<<m_RBFNS.nCenterNum<<" ";
	//保存输入维数
	ossRBFN<<m_RBFNS.nInputCol<<" ";
	//保存目标维数
	ossRBFN<<1<<" ";		
		
	//保存用于标准化的最大最小值
	for(i=0;i<2;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)
		{
			ossRBFN<<m_RBFNS.mxInputColMaxMin(i,j)<<" ";
		}
			
	}
	for(i=0;i<2;i++)
	{
		ossRBFN<<m_RBFNS.mxTargetColMaxMin(i,0)<<" ";	
	}
	
	//保存隐含层中心
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)
		{
			ossRBFN<<m_RBFNS.mxStdCenter(i,j)<<" ";
		}
	}
		
	//保存RBF基函数的宽度
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		ossRBFN<<m_RBFNS.mxSigmaSquare(i,0)<<" ";
	}
		
	//保存权值，+1 for 偏置
	for(i=0;i<m_RBFNS.nCenterNum+1;i++)
	{
		ossRBFN<<m_RBFNS.mxWeight(i,0)<<" ";
	}
		
	strRBFNS=ossRBFN.str();
		
	return true;
}

//载入过去保存下来的网络结构,若正常保存,则载入应该正确,故始终返回true
bool CRBFN_Algorithm::LoadRBFNS(string& strRBFNS)
{
	int i,j;
	istringstream issRBFN(strRBFNS);
	
	m_bHasNetStructure=false;
	
	//载入隐含层中心数
	issRBFN>>m_RBFNS.nCenterNum;		
	//载入输入维数	
	issRBFN>>m_RBFNS.nInputCol; 	
	//载入目标维数,暂时不用,为以后多输出预留
	issRBFN>>m_RBFNS.nTargetCol;	
	
	//载入用于标准化的最大最小值
	m_RBFNS.mxInputColMaxMin.create(2,m_RBFNS.nInputCol);		
	for(i=0;i<2;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)			
		{
			issRBFN>>m_RBFNS.mxInputColMaxMin(i,j);
		}
	}
	m_RBFNS.mxTargetColMaxMin.create(2,1);
	for(i=0;i<2;i++)
	{
		issRBFN>>m_RBFNS.mxTargetColMaxMin(i,0);	
	}
	
	//载入隐含层中心
	m_RBFNS.mxStdCenter.create(m_RBFNS.nCenterNum,m_RBFNS.nInputCol);
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)
		{
			issRBFN>>m_RBFNS.mxStdCenter(i,j);
		}
	}
	
	//载入RBF基函数的宽度
	m_RBFNS.mxSigmaSquare.create(m_RBFNS.nCenterNum,1);
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		issRBFN>>m_RBFNS.mxSigmaSquare(i,0);
	}
	
	//载入权值
	m_RBFNS.mxWeight.create(m_RBFNS.nCenterNum+1,1); //+1 for 偏置
	for(i=0;i<m_RBFNS.nCenterNum+1;i++)
	{
		issRBFN>>m_RBFNS.mxWeight(i,0);
	}

	m_bHasNetStructure=true;

	return true;
}

RBFNetStructure CRBFN_Algorithm::GetRBFNS()
{
	return m_RBFNS;		
}


bool CRBFN_Algorithm::Predict(CDoubleMatrix& mxPreInput, CDoubleMatrix& mxPreOutput)
{
	//开始计时
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	//数据检查/////////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxPreInput.mrow()<1)
	{
		m_strErrorInfo="RBF神经网络错误信息：没有输入数据！";
		m_bError=true;
		return false;
	}
	if(!HasNetStructure())
	{
		m_strErrorInfo="RBF神经网络错误信息：网络结构不存在！";
		m_bError=true;
		return false;
	}
	if(mxPreInput.mcol()!=m_RBFNS.nInputCol)
	{
		m_strErrorInfo="RBF神经网络错误信息：输入数据的维数与网络结构不匹配！";
		m_bError=true;
		return false;
	}
	
	//开始计算///////////////////////////////////////////////////////////////////////
	CDoubleMatrix mxPreStdInput,mxPreStdOutput,mxHideOut;
	//标准化
	MxStandardize(mxPreInput,m_RBFNS.mxInputColMaxMin,mxPreStdInput,true);
	//求中间层输出
	CalculateHideOut(mxPreStdInput,m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare,mxHideOut);
	//输出
	mxPreStdOutput=mxHideOut*m_RBFNS.mxWeight;
	MxInverseStandardize(mxPreStdOutput,m_RBFNS.mxTargetColMaxMin,mxPreOutput);

	//结束计时
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"预测样本数："<<mxPreInput.mrow()<<"  "<<"输入变量数："<<mxPreInput.mcol()<<"  "
		<<"隐含层中心数："<<m_RBFNS.nCenterNum<<"\n"
		<<"预测总时间："<<FormatTimeSpan(timeBegin,timeEnd);	
	m_strRunInfo=oss.str();

	return true;
}

bool CRBFN_Algorithm::Test(CDoubleMatrix& mxTestInput, CDoubleMatrix& mxTestTarget, 
		  CDoubleMatrix& mxTestOutput, double& dMSE, double& dStdMSE)
{
	//开始计时
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	

	//数据检查////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxTestInput.mrow()!=mxTestTarget.mrow())
	{
		m_strErrorInfo="RBF神经网络错误信息：输入数据的记录数与目标数据的记录数不一致！";
		m_bError=true;
		return false;
	}

	//开始计算///////////////////////////////////////////////////////////////
	if(!Predict(mxTestInput,mxTestOutput))
		return false;
	
	dMSE=MSE(mxTestTarget,mxTestOutput);
	CDoubleMatrix mxTestStdTarget,mxTestStdOutput;
	MxStandardize(mxTestTarget,m_RBFNS.mxTargetColMaxMin,mxTestStdTarget,true);
	MxStandardize(mxTestOutput,m_RBFNS.mxTargetColMaxMin,mxTestStdOutput,true);
	dStdMSE=MSE(mxTestStdTarget,mxTestStdOutput);
	
	//结束计时
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"测试样本数："<<mxTestInput.mrow()<<"  "<<"输入变量数："<<mxTestInput.mcol()<<"  "
		<<"隐含层中心数："<<m_RBFNS.nCenterNum<<"\n"
		<<"测试总时间："<<FormatTimeSpan(timeBegin,timeEnd);	
	m_strRunInfo=oss.str();
	
	return true;
}

double CRBFN_Algorithm::MSE(CDoubleMatrix& mxTarget, CDoubleMatrix& mxOutput)
{
	CDoubleMatrix mx;
	double mse=0;
	int i;

	mx=mxTarget-mxOutput;
	for(i=0;i<mx.mrow();i++)
	{
		mse=mse+mx(i,0)*mx(i,0);		
	}
	mse=mse/mx.mrow();
	
	return mse;
}

//带偏置
bool CRBFN_Algorithm::CalculateHideOut(CDoubleMatrix& mxStdInput, CDoubleMatrix& mxStdCenter, 
		CDoubleMatrix& mxSigmaSquare, CDoubleMatrix& mxHideOut)
{
	double temp;
	int i,j,k;
	int nCenterNum=mxStdCenter.mrow();

	mxHideOut.create(mxStdInput.mrow(),nCenterNum+1);//+1 for 偏置
	for(i=0;i<mxStdInput.mrow();i++)
	{
		for(j=0;j<nCenterNum;j++)
		{
			temp=0.0;
			for(k=0;k<mxStdInput.mcol();k++)
			{
				temp=temp+(mxStdInput(i,k)-mxStdCenter(j,k))
					*(mxStdInput(i,k)-mxStdCenter(j,k));
			}
			temp=exp(-temp/(2*mxSigmaSquare(j,0)));
			mxHideOut(i,j)=temp;
		}
	}

	//for 偏置 
	for(i=0;i<mxStdInput.mrow();i++)
	{
		mxHideOut(i,nCenterNum)=1;
	}

	return true;
}

bool CRBFN_Algorithm::Export(TCLFile &theFile)
{
	int i=0, j=0, nTemp=0;
	
	//m_RBFNS.nCenterNum
	theFile << m_RBFNS.nCenterNum;
	//m_RBFNS.nInputCol
	theFile << m_RBFNS.nInputCol;
	//m_RBFNS.nTargetCol
	theFile << m_RBFNS.nTargetCol;

	//m_RBFNS.mxInputColMaxMin
	nTemp = m_RBFNS.mxInputColMaxMin.mrow();
	theFile << nTemp;
	nTemp = m_RBFNS.mxInputColMaxMin.mcol();
	theFile << nTemp;
	for (j=0;j<m_RBFNS.mxInputColMaxMin.mrow();j++)
	{
		for (i=0;i<m_RBFNS.mxInputColMaxMin.mcol();i++)
		{
			theFile << m_RBFNS.mxInputColMaxMin(j,i);
		}
	}

	//m_RBFNS.mxSigmaSquare
	nTemp = m_RBFNS.mxSigmaSquare.mrow();
	theFile << nTemp;
	nTemp = m_RBFNS.mxSigmaSquare.mcol();
	theFile << nTemp;
	for (j=0;j<m_RBFNS.mxSigmaSquare.mrow();j++)
	{
		for (i=0;i<m_RBFNS.mxSigmaSquare.mcol();i++)
		{
			theFile << m_RBFNS.mxSigmaSquare(j,i);
		}
	}	
	//m_RBFNS.mxStdCenter
	nTemp = m_RBFNS.mxStdCenter.mrow();
	theFile << nTemp;
	nTemp = m_RBFNS.mxStdCenter.mcol();
	theFile << nTemp;
	for (j=0;j<m_RBFNS.mxStdCenter.mrow();j++)
	{
		for (i=0;i<m_RBFNS.mxStdCenter.mcol();i++)
		{
			theFile << m_RBFNS.mxStdCenter(j,i);
		}
	}	
	//m_RBFNS.mxTargetColMaxMin
	nTemp = m_RBFNS.mxTargetColMaxMin.mrow();
	theFile << nTemp;
	nTemp = m_RBFNS.mxTargetColMaxMin.mcol();
	theFile << nTemp;
	for (j=0;j<m_RBFNS.mxTargetColMaxMin.mrow();j++)
	{
		for (i=0;i<m_RBFNS.mxTargetColMaxMin.mcol();i++)
		{
			theFile << m_RBFNS.mxTargetColMaxMin(j,i);
		}
	}	
	//m_RBFNS.mxWeight
	nTemp = m_RBFNS.mxWeight.mrow();
	theFile << nTemp;
	nTemp = m_RBFNS.mxWeight.mcol();
	theFile << nTemp;
	for (j=0;j<m_RBFNS.mxWeight.mrow();j++)
	{
		for (i=0;i<m_RBFNS.mxWeight.mcol();i++)
		{
			theFile << m_RBFNS.mxWeight(j,i);
		}
	}		
	return true;
}

bool CRBFN_Algorithm::Import(TCLFile &theFile)
{
	int i=0, j=0, nRow=0, nCol=0;

	//m_RBFNS.nCenterNum
	theFile >> m_RBFNS.nCenterNum;
	//m_RBFNS.nInputCol
	theFile >> m_RBFNS.nInputCol;
	//m_RBFNS.nTargetCol
	theFile >> m_RBFNS.nTargetCol;
	//m_RBFNS.mxInputColMaxMin
	theFile >> nRow;
	theFile >> nCol;
	m_RBFNS.mxInputColMaxMin.create(nRow,nCol);
	for (j=0; j<nRow; j++)
	{
		for (i=0; i<nCol; i++)
		{
			theFile >> m_RBFNS.mxInputColMaxMin(j,i);
		}
	}
	//m_RBFNS.mxSigmaSquare
	theFile >> nRow;
	theFile >> nCol;
	m_RBFNS.mxSigmaSquare.create(nRow,nCol);
	for (j=0; j<nRow; j++)
	{
		for (i=0; i<nCol; i++)
		{
			theFile >> m_RBFNS.mxSigmaSquare(j,i);
		}
	}
	//m_RBFNS.mxStdCenter
	theFile >> nRow;
	theFile >> nCol;
	m_RBFNS.mxStdCenter.create(nRow,nCol);
	for (j=0; j<nRow; j++)
	{
		for (i=0; i<nCol; i++)
		{
			theFile >> m_RBFNS.mxStdCenter(j,i);
		}
	}	
	//m_RBFNS.mxTargetColMaxMin
	theFile >> nRow;
	theFile >> nCol;
	m_RBFNS.mxTargetColMaxMin.create(nRow,nCol);
	for (j=0; j<nRow; j++)
	{
		for (i=0; i<nCol; i++)
		{
			theFile >> m_RBFNS.mxTargetColMaxMin(j,i);
		}
	}		
	//m_RBFNS.mxWeight
	theFile >> nRow;
	theFile >> nCol;
	m_RBFNS.mxWeight.create(nRow,nCol);
	for (j=0; j<nRow; j++)
	{
		for (i=0; i<nCol; i++)
		{
			theFile >> m_RBFNS.mxWeight(j,i);
		}
	}	
	m_bHasNetStructure = true;
	return true;
}

bool CRBFN_Algorithm::HasNetStructure()
{
	return m_bHasNetStructure;
}

void CRBFN_Algorithm::Abort()
{
	m_bAbort=true;
	m_KMeans.Abort();
}

string CRBFN_Algorithm::GetAbortInfo()
{
	return m_strAbortInfo;
}

string CRBFN_Algorithm::GetErrorInfo()
{
	return m_strErrorInfo;
}

bool CRBFN_Algorithm::IsError()
{
	return m_bError;
}

bool CRBFN_Algorithm::IsAbort()
{
	return m_bAbort;
}

string CRBFN_Algorithm::FormatTimeSpan(_timeb timeBegin,_timeb timeEnd)
{
	ostringstream oss;
	double dTimeSpan=(timeEnd.time+timeEnd.millitm/1000.0)
					-(timeBegin.time+timeBegin.millitm/1000.0);
	
	long nHour=long(dTimeSpan)/3600;
	long nMin=long(dTimeSpan-3600*nHour)/60;
	double dSec=long((dTimeSpan-3600*nHour-60*nMin)*1000+0.5)/1000.0;
	oss<<nHour<<"小时 "<<nMin<<"分钟 "<<dSec<<"秒";

	return oss.str();
}

string CRBFN_Algorithm::GetRunInfo()
{
	return m_strRunInfo;
}

//改写自岭回归算法，参看其代码
//速度慢，暂不使用
CDoubleMatrix CRBFN_Algorithm::RidgeEstimate(CDoubleMatrix &Matrix_X,CDoubleMatrix &Matrix_y, double dkValue)
{
/*	const double eps=1e-30;
	CDoubleMatrix		SX;   //(X-EX)'*(X-EX)
	CDoubleMatrix		Sy;   //(X-EX)'*y
	SX=Matrix_X.dot(Matrix_X);//SX=X'*X
	Sy=Matrix_X.dot(Matrix_y);//Sy=X'*y

	CDoubleMatrix	EigVector=SX;
	int nRow=EigVector.mrow();
	
	CDoubleVector	Vect_Data = MatrixToVector(EigVector);
	CDoubleVector	VectEigVector(nRow*nRow);
		
//求解对称矩阵的特征值和特征向量
//Vect_Data双精度二维矩阵：nRow*nRow,存放对称矩阵,返回时对角线上存放nRow个特征值
//VectEigVector：nRow*nRow,返回特征向量
//eps：控制精度要求
	cjcbj(Vect_Data, nRow, VectEigVector, eps);
//对特征值和特征向量按照特征值排序,nIndex == 0 从小到大排序 nIndex == 1 从大到小排序
	bool nIndex=1;
	SortEigenVect(Vect_Data, VectEigVector, nRow, nRow, nIndex);
	
	CDoubleMatrix	EigValue(nRow,1);
	for( int i= 0; i < nRow; i++)
	{
		EigValue(i,0) = Vect_Data(i+i*nRow);
	}
	if(EigValue(EigValue.mrow()-1,0)<eps)
	{
		for(int i=0;i<EigValue.mrow();i++)
		{
			EigValue(i,0)+=eps;
		}
	}
	
//特征向量矩阵的每一列对应一个特征值
	EigVector = trans(VectorToMatrix(VectEigVector,nRow,nRow));
	Sy=EigVector.dot(Sy);//Sy=V'*Sy	
*/
	CDoubleMatrix MatCoeff;	
//	MatCoeff=EigVector*(Sy/(EigValue+dkValue*CDoubleMatrix(nRow,1,1)));
	return MatCoeff;
 }

