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


//bHasMaxMin==true��ֱ����mxColMaxMin��׼��mx
//bHasMaxMin==false���ȴ�mx�л�ø��������Сֵ����mxColMaxMin���ٱ�׼��mx
//mxColMaxMin�ĵ�0�д�����ֵ����1�д����Сֵ
//�ɿ����þ�ֵ�����׼����Ч�����ܻ����
bool CRBFN_Algorithm::MxStandardize(CDoubleMatrix& mx, CDoubleMatrix& mxColMaxMin, 
									CDoubleMatrix& mxStd, bool bHasMaxMin)
{
	int i,j;
	const int nRow=mx.mrow();
	const int nCol=mx.mcol();
	
	if(bHasMaxMin==false)
	{
		//��������Сֵ��Ϊ�����׼���Ĳ���
		mxColMaxMin.create(2,nCol);
		for(j=0;j<nCol;j++)
		{
			mxColMaxMin(0,j)=mx(0,j);//��ʼ�����ֵ
			mxColMaxMin(1,j)=mx(0,j);//��ʼ����Сֵ
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
			continue;//ԭ����mxColMaxMin(0,j)����Ӧ��Զ����MININTERVAL
		
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
	//���ȡ����
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

	//���پ����ȡ���������ģ��������ɵ�RBF�����ƹ������ȽϺ�
	//�����������������������������ε���������̶�
	if(!m_KMeans.KMeans(mxStd,nCenterNum,true,1e-20,200,mxStdCenter))
	{
		if(m_KMeans.IsError()) 
		{
			m_strErrorInfo="RBF�����������Ϣ����ȡ���������Ĵ���\n"
								+string("---->>")+m_KMeans.GetErrorInfo();
			m_bError=true;
			return false;
		}
		else if(m_KMeans.IsAbort()) 
		{
			m_strAbortInfo+="RBF��������ֹ��Ϣ����ȡ������������ֹ��\n"
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
	CDoubleMatrix mxDistSquareDelDiag(nCenterNum,nCenterNum-1,0.0);//ȥ��mxDistSquare�Խ�����0����
	double dMin1,dMin2; //��Сֵ�ʹ���Сֵ

	//��RBF�������Ŀ��SigmaSquare=������ĵ�������������ĵ����ƽ����ƽ��ֵ
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
	//����������溯��pinv()���ɴ�������������⣬����һ���溯��inv()���ܴ�����������
	//CDoubleMatrix���д洢�����ڼ�¼���ܶ�����ݣ�������ת�����㣬��dot()������
	//ֱ�ӵ���mxX.pinv()���ڰ��д洢�����⵱���������ܶ��ٶȻ��������Ϊ���ڲ�����ʱ������ת�þ���
	CDoubleMatrix tempM;
	tempM=mxX.dot(mxX); //mxX'*mxX 
	tempM=tempM.pinv();
	tempM=mxX*(tempM.trans()); 
	mxWeight=tempM.dot(mxY);


//////////////////////////////////////////////////////////////////////////
//	��ع���������淽�����ɴ��������������,�������ʱ���ٶ���
//	mxWeight=RidgeEstimate(mxX,mxY,0);

	return true;
}

//����ṹѵ�������̵߳���ʱ����ͨ����ѯm_strAbortInfo�˽⵱ǰѵ��״̬
bool CRBFN_Algorithm::Train(CDoubleMatrix& mxInput, CDoubleMatrix& mxTarget, int nCenterNum, 
		   CDoubleMatrix& mxOutput, double& dMSE, double& dStdMSE)
{	
	//��ʼ��ʱ
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	//���ݼ��/////////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxInput.mrow()<1)
	{
		m_strErrorInfo="RBF�����������Ϣ��û���������ݣ�";
		m_bError=true;
		return false;
	}
	if(mxInput.mrow()!=mxTarget.mrow())
	{
		m_strErrorInfo="RBF�����������Ϣ���������ݵļ�¼����Ŀ�����ݵļ�¼����һ�£�";
		m_bError=true;
		return false;
	}
	if(nCenterNum<2)
	{
		m_strErrorInfo="RBF�����������Ϣ��ָ��������������Ӧ���ڵ���2��";
		m_bError=true;
		return false;
	}
	if(mxInput.mrow()<nCenterNum)
	{
		m_strErrorInfo="RBF�����������Ϣ��ָ��������������ӦС�ڵ����������ݵļ�¼����";
		m_bError=true;
		return false;
	}
	
	//��ʼ����///////////////////////////////////////////////////////////////////////
	m_bAbort=false;
	m_bHasNetStructure=false;
	
	CDoubleMatrix mxStdInput,mxStdTarget,mxHideOut,mxStdOutput;

	m_RBFNS.nCenterNum=nCenterNum;
	m_RBFNS.nInputCol=mxInput.mcol();
	m_strAbortInfo="RBF�����翪ʼѵ��...\n";
	string strStop="RBF������ѵ����ֹ!\n";
	
	//��׼��
	if(!m_bAbort)
	{
		MxStandardize(mxInput,m_RBFNS.mxInputColMaxMin,mxStdInput,false);
		MxStandardize(mxTarget,m_RBFNS.mxTargetColMaxMin,mxStdTarget,false);
		m_strAbortInfo+="RBF������ѵ��������һ���;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//�õ�����������
	if(!m_bAbort)
	{
		if(GetHideCenter(mxStdInput,nCenterNum,m_RBFNS.mxStdCenter))
		{
			m_strAbortInfo+="RBF������ѵ������������;\n";
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

	//����RBF�������Ŀ��
	if(!m_bAbort)
	{
		CalculateSigmaSquare(m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare);
		m_strAbortInfo+="RBF������ѵ�������������;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//���м�����
	if(!m_bAbort)
	{
		CalculateHideOut(mxStdInput,m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare,mxHideOut);
		m_strAbortInfo+="RBF������ѵ�������������;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}

	//��Ȩֵ
	if(!m_bAbort)
	{
		CalculateWeight(mxHideOut,mxStdTarget,m_RBFNS.mxWeight);
		m_strAbortInfo+="RBF������ѵ�������������;\n";
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;
	}
	
	//�����������ѻ��
	m_bHasNetStructure=true;
	m_strAbortInfo+="RBF������ṹ��ѵ�����!\n��ʼ����ѵ�����...\n";
			
	//��mse
	if(!m_bAbort)
	{
		mxStdOutput=mxHideOut*m_RBFNS.mxWeight;
		dStdMSE=MSE(mxStdTarget,mxStdOutput);
		MxInverseStandardize(mxStdOutput,m_RBFNS.mxTargetColMaxMin,mxOutput);
		dMSE=MSE(mxTarget,mxOutput);
		m_strAbortInfo+="RBF������ѵ��������ѵ��������!\n";	
	}
	else
	{
		m_strAbortInfo+=strStop;
		return false;		
	}

	m_strAbortInfo+="RBF������ѵ����������!\n";	
	

	//������ʱ
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"ѵ����������"<<mxInput.mrow()<<"  "<<"�����������"<<mxInput.mcol()<<"  "
		<<"��������������"<<nCenterNum<<"\n"
		<<"ѵ����ʱ�䣺"<<FormatTimeSpan(timeBegin,timeEnd)<<"\n"
		<<"���л�ȡ���������ģ�"+m_KMeans.GetRunInfo();	
	m_strRunInfo=oss.str();
	
	return true;
}


bool CRBFN_Algorithm::SaveRBFNS(string& strRBFNS)
{
	//�������������ṹ�򷵻�false
	m_bError=false;
	if(!HasNetStructure())
	{
		m_strErrorInfo="RBF�����������Ϣ������ṹ�����ڣ�";
		m_bError=true;
		return false;
	}	
	
	ostringstream ossRBFN;
	int i,j;
	
	//���þ��ȣ�����޶ȼ��ٱ�����ֵ���ַ����ľ�����ʧ
	ossRBFN.precision(30);
	
	//����������������
	ossRBFN<<m_RBFNS.nCenterNum<<" ";
	//��������ά��
	ossRBFN<<m_RBFNS.nInputCol<<" ";
	//����Ŀ��ά��
	ossRBFN<<1<<" ";		
		
	//�������ڱ�׼���������Сֵ
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
	
	//��������������
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)
		{
			ossRBFN<<m_RBFNS.mxStdCenter(i,j)<<" ";
		}
	}
		
	//����RBF�������Ŀ��
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		ossRBFN<<m_RBFNS.mxSigmaSquare(i,0)<<" ";
	}
		
	//����Ȩֵ��+1 for ƫ��
	for(i=0;i<m_RBFNS.nCenterNum+1;i++)
	{
		ossRBFN<<m_RBFNS.mxWeight(i,0)<<" ";
	}
		
	strRBFNS=ossRBFN.str();
		
	return true;
}

//�����ȥ��������������ṹ,����������,������Ӧ����ȷ,��ʼ�շ���true
bool CRBFN_Algorithm::LoadRBFNS(string& strRBFNS)
{
	int i,j;
	istringstream issRBFN(strRBFNS);
	
	m_bHasNetStructure=false;
	
	//����������������
	issRBFN>>m_RBFNS.nCenterNum;		
	//��������ά��	
	issRBFN>>m_RBFNS.nInputCol; 	
	//����Ŀ��ά��,��ʱ����,Ϊ�Ժ�����Ԥ��
	issRBFN>>m_RBFNS.nTargetCol;	
	
	//�������ڱ�׼���������Сֵ
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
	
	//��������������
	m_RBFNS.mxStdCenter.create(m_RBFNS.nCenterNum,m_RBFNS.nInputCol);
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		for(j=0;j<m_RBFNS.nInputCol;j++)
		{
			issRBFN>>m_RBFNS.mxStdCenter(i,j);
		}
	}
	
	//����RBF�������Ŀ��
	m_RBFNS.mxSigmaSquare.create(m_RBFNS.nCenterNum,1);
	for(i=0;i<m_RBFNS.nCenterNum;i++)
	{
		issRBFN>>m_RBFNS.mxSigmaSquare(i,0);
	}
	
	//����Ȩֵ
	m_RBFNS.mxWeight.create(m_RBFNS.nCenterNum+1,1); //+1 for ƫ��
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
	//��ʼ��ʱ
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	
	
	//���ݼ��/////////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxPreInput.mrow()<1)
	{
		m_strErrorInfo="RBF�����������Ϣ��û���������ݣ�";
		m_bError=true;
		return false;
	}
	if(!HasNetStructure())
	{
		m_strErrorInfo="RBF�����������Ϣ������ṹ�����ڣ�";
		m_bError=true;
		return false;
	}
	if(mxPreInput.mcol()!=m_RBFNS.nInputCol)
	{
		m_strErrorInfo="RBF�����������Ϣ���������ݵ�ά��������ṹ��ƥ�䣡";
		m_bError=true;
		return false;
	}
	
	//��ʼ����///////////////////////////////////////////////////////////////////////
	CDoubleMatrix mxPreStdInput,mxPreStdOutput,mxHideOut;
	//��׼��
	MxStandardize(mxPreInput,m_RBFNS.mxInputColMaxMin,mxPreStdInput,true);
	//���м�����
	CalculateHideOut(mxPreStdInput,m_RBFNS.mxStdCenter,m_RBFNS.mxSigmaSquare,mxHideOut);
	//���
	mxPreStdOutput=mxHideOut*m_RBFNS.mxWeight;
	MxInverseStandardize(mxPreStdOutput,m_RBFNS.mxTargetColMaxMin,mxPreOutput);

	//������ʱ
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"Ԥ����������"<<mxPreInput.mrow()<<"  "<<"�����������"<<mxPreInput.mcol()<<"  "
		<<"��������������"<<m_RBFNS.nCenterNum<<"\n"
		<<"Ԥ����ʱ�䣺"<<FormatTimeSpan(timeBegin,timeEnd);	
	m_strRunInfo=oss.str();

	return true;
}

bool CRBFN_Algorithm::Test(CDoubleMatrix& mxTestInput, CDoubleMatrix& mxTestTarget, 
		  CDoubleMatrix& mxTestOutput, double& dMSE, double& dStdMSE)
{
	//��ʼ��ʱ
	struct _timeb timeBegin,timeEnd;
	_ftime(&timeBegin);	

	//���ݼ��////////////////////////////////////////////////////////////////
	m_bError=false;
	if(mxTestInput.mrow()!=mxTestTarget.mrow())
	{
		m_strErrorInfo="RBF�����������Ϣ���������ݵļ�¼����Ŀ�����ݵļ�¼����һ�£�";
		m_bError=true;
		return false;
	}

	//��ʼ����///////////////////////////////////////////////////////////////
	if(!Predict(mxTestInput,mxTestOutput))
		return false;
	
	dMSE=MSE(mxTestTarget,mxTestOutput);
	CDoubleMatrix mxTestStdTarget,mxTestStdOutput;
	MxStandardize(mxTestTarget,m_RBFNS.mxTargetColMaxMin,mxTestStdTarget,true);
	MxStandardize(mxTestOutput,m_RBFNS.mxTargetColMaxMin,mxTestStdOutput,true);
	dStdMSE=MSE(mxTestStdTarget,mxTestStdOutput);
	
	//������ʱ
	_ftime(&timeEnd);
	ostringstream oss;
	oss<<"������������"<<mxTestInput.mrow()<<"  "<<"�����������"<<mxTestInput.mcol()<<"  "
		<<"��������������"<<m_RBFNS.nCenterNum<<"\n"
		<<"������ʱ�䣺"<<FormatTimeSpan(timeBegin,timeEnd);	
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

//��ƫ��
bool CRBFN_Algorithm::CalculateHideOut(CDoubleMatrix& mxStdInput, CDoubleMatrix& mxStdCenter, 
		CDoubleMatrix& mxSigmaSquare, CDoubleMatrix& mxHideOut)
{
	double temp;
	int i,j,k;
	int nCenterNum=mxStdCenter.mrow();

	mxHideOut.create(mxStdInput.mrow(),nCenterNum+1);//+1 for ƫ��
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

	//for ƫ�� 
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
	oss<<nHour<<"Сʱ "<<nMin<<"���� "<<dSec<<"��";

	return oss.str();
}

string CRBFN_Algorithm::GetRunInfo()
{
	return m_strRunInfo;
}

//��д����ع��㷨���ο������
//�ٶ������ݲ�ʹ��
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
		
//���Գƾ��������ֵ����������
//Vect_Data˫���ȶ�ά����nRow*nRow,��ŶԳƾ���,����ʱ�Խ����ϴ��nRow������ֵ
//VectEigVector��nRow*nRow,������������
//eps�����ƾ���Ҫ��
	cjcbj(Vect_Data, nRow, VectEigVector, eps);
//������ֵ������������������ֵ����,nIndex == 0 ��С�������� nIndex == 1 �Ӵ�С����
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
	
//�������������ÿһ�ж�Ӧһ������ֵ
	EigVector = trans(VectorToMatrix(VectEigVector,nRow,nRow));
	Sy=EigVector.dot(Sy);//Sy=V'*Sy	
*/
	CDoubleMatrix MatCoeff;	
//	MatCoeff=EigVector*(Sy/(EigValue+dkValue*CDoubleMatrix(nRow,1,1)));
	return MatCoeff;
 }

