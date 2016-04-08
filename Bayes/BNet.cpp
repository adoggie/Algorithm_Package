// BNet.cpp: implementation of the CBNet class.
//
//////////////////////////////////////////////////////////////////////
#include "../DataInterface/DataInterface.h"
#include "../cpknl/cpknl.h"
#include "../Result/Result.h"
#include "../tlAdvn/Analyze.h"
//#include "stdafx.h"
#include "BNet.h"
#include "BNode.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

// extern CList<CRect,CRect> rectrect;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
string CBayesOutput::Export()
{
	string str;

	return str;
}

bool   CBayesOutput::Import(string strModel)
{
	return true;
}




CBNet::CBNet()
{
	m_sErrorInfo.Empty();
	m_bErrorFlag = false;
	m_DataMatrix.destruction();
	m_nNodeNum = 0;
	m_StateNum = CIntVector(0);
	m_NetName = _T("");
	m_NeedSave = 1;
	m_CausTbl = CIntMatrix(m_nNodeNum,m_nNodeNum,0);
	m_nSimNum=1000;
	m_nGSimNum=5000;
	//m_NodePtrList = CPtrList();//初始化节点列表
}

CBNet::CBNet(CDataInterface* pDataInterface)
{
	m_sErrorInfo.Empty();
	m_bErrorFlag = false;
	this->m_pDataInterface=pDataInterface;
	int nRow=m_pDataInterface->GetRowCount();
	int nCol=m_pDataInterface->GetColCount();
	if (nCol>0 && nRow>0)
	{
		m_DataMatrix=GetData();
		if (m_bErrorFlag)
		{
			m_sErrorInfo = "新建网络失败！\n\n\n"+m_sErrorInfo;
			return;
		}
 		m_nNodeNum=m_DataMatrix.mcol();
		m_StateNum = CBNet::GetStateNum();
	}
	else
	{
		m_DataMatrix.destruction();
		m_nNodeNum = 0;
		m_StateNum = CIntVector(0);
	}
	m_NeedSave = 1;
	m_NetName = _T("");
	m_CausTbl = CIntMatrix(m_nNodeNum,m_nNodeNum,0);
	m_nSimNum=10000;
	m_nGSimNum=5000;
}

CBNet::CBNet(int nn,char** namelist,CIntMatrix & ct)
{
	
}

CBNet::CBNet(CBNet& ns)
{
	
}

CBNet::~CBNet()
{
	if (!this->m_NodePtrList.IsEmpty())
	{
		int len = this->m_NodePtrList.GetCount();
		CBNode	*pNode;
		for (int i=0;i<len;i++)
		{		
			pNode = (CBNode*)this->m_NodePtrList.GetTail();		
			this->m_NodePtrList.RemoveTail();
			delete pNode;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
// 根据界面数据初始化网络
/////////////////////////////////////////////////////////////////////////////////
void CBNet::InitiateNet(CDataInterface *pDataInterface,CBayesVO*pBayesVO)
{
	int i,j;
	this->m_pDataInterface=pDataInterface;
	m_arFieldName.Copy(pBayesVO->strInputField);

	int nRow=m_pDataInterface->GetRowCount();
	int nCol=m_arFieldName.GetSize();
	if (nCol>0 && nRow>0)
	{
		m_DataMatrix=GetData();
 		m_nNodeNum=m_DataMatrix.mcol();
		CBNet::GetStateNum();
	}
	else
	{
		m_DataMatrix.destruction();
		m_nNodeNum = 0;
		m_StateNum = CIntVector(0);
	}
	m_NeedSave = 1;
	m_NetName = _T("");
	int nTotalCount = pBayesVO->strInputField.GetSize();
	int nIndexCount = pBayesVO->strCauIndex.GetSize()/2;
	m_CausTbl.create(nTotalCount,nTotalCount,0);
	for (i=0;i<nIndexCount;i++)
	{
		CTString strFrom = pBayesVO->strCauIndex.GetAt(i*2);
		int nFromIndex = -1;//pDataInterface->IndexByName(strFrom);
		for (j=0;j<nTotalCount;j++)
		{
			if (strFrom == pBayesVO->strCauIndex.GetAt(j))
			{
				nFromIndex = j;
				break;
			}
		}
		if (nFromIndex == -1)
			continue;
		CTString strTo = pBayesVO->strCauIndex.GetAt(i*2+1);
		int nToIndex = -1;//pDataInterface->IndexByName(strTo);
		for (j=0;j<nTotalCount;j++)
		{
			if (strTo == pBayesVO->strCauIndex.GetAt(j))
			{
				nToIndex = j;
				break;
			}
		}
		if (nToIndex == -1)
			continue;
		m_CausTbl(nFromIndex,nToIndex) = 1;
	}

	Bayesnodessort();

//	m_CausTbl = CIntMatrix(m_nNodeNum,m_nNodeNum,0);
	m_nSimNum=10000;
	m_nGSimNum=5000;
}

//////////////////////////////////////////////////////////////
//产生父节点状态表排列组合
//////////////////////////////////////////////////////////////
void CBNet::CreateStateTbl(CIntMatrix& StateTbl,CIntVector parntVector,int num)
{
	int i,j,k,seed,state;
	CIntVector	formatVector = CIntVector(num,0);
	formatVector(num-1) = 1;
	if (num>1)
		for (i=num-2;i>=0;i--)
		{
			seed = 1;
			for (j=i+1;j<num;j++)
				seed = seed*m_StateNum(parntVector(j));
			formatVector(i) = seed;
		}

	for (i=0;i<num;i++)
	{
		j = 0;
		state = 0;
		while (j<StateTbl.mrow())
		{
			for (k=0;k<formatVector(i);k++)
			{
				StateTbl(j,i) = state;
				j++;
			}
			state++;
			if (state >= m_StateNum(parntVector(i)))
				state = 0;
		}
	}
}

//////////////////////////////////////////////////////
//节点排序
//////////////////////////////////////////////////////
bool CBNet::Bayesnodessort()
{
	CIntMatrix mCausTbl = this->m_CausTbl;
	int m=0;                             //ID号计数器
	int size=m_nNodeNum;
	bool tag=true;
	CIntVector mChangeTbl;
	mChangeTbl=CIntVector(size,0);      //新旧节点编号对照表
	CIntVector NodeTbl;
	NodeTbl= CIntVector(size,0);	      //定义NodeTbl为全1的数组，标示节点是否已被编号，已编号则赋值为0
	for(int i=0;i<size;i++)
		NodeTbl(i)=1;

	//首先编号根节点
	for(i=0;i<size;i++)
	{
		int sum=0;
		for(int j=0;j<size;j++)
			sum=sum+mCausTbl(j,i);
		if(sum==0)                        //判断为根节点
		{
			for (int k=0;k<size;k++)
				sum=sum+mCausTbl(i,k);
			if (sum==0)
			{
// 				AfxMessageBox("图中存在孤立节点！");
				return false;
			}
			mChangeTbl(i)=m;             //原节点i的ID编码为m
			m++;
			NodeTbl(i)=0;                //修改标示
		}
	}

    int sum=0;
	for(i=0;i<size;i++)
		sum=sum+NodeTbl(i);
	while(sum!=0 && tag)                 //尚有未编号节点且尚未遇到环
	{
		tag=false;                       //上一循环中是否有新的节点被编号
		for(i=0;i<size;i++)
			if(NodeTbl(i)!=0)            //若是有尚未编号的节点
			{
				sum=0;
				for(int j=0;j<size;j++)
				{
					if(mCausTbl(j,i)==1)
						sum=sum+NodeTbl(j);
				}
				if(sum==0)               //父节点是否都已被编号
				{
					mChangeTbl(i)=m;     //原节点i的ID编号为m
					m++;
					NodeTbl(i)=0;
					tag=true;
				}				
			}
			sum=0;
			for(int k=0;k<size;k++)
				sum=sum+NodeTbl(k);
	}
	sum=0;
	for(i=0;i<size;i++)
		sum=sum+NodeTbl(i);
	if(sum!=0 && !tag)                  //若有节点始终不能被编号，则认为有环
	{
// 		AfxMessageBox("图中存在环形通路！");
		return false;
	}
	else
	{
		this->m_ChangeTbl = mChangeTbl;
		return true;
	}
}

//////////////////////////////////////////////////////////////////////
//更新节点关系表等
//////////////////////////////////////////////////////////////////////
void CBNet::newCausTbl(CIntMatrix& CausTbl,CIntVector ChangeTbl,CIntVector& StateNum,CIntMatrix& DataMatrix)//,CIntMatrix& InitMatrix,CIntVector& AimVector)
{	
	//节点关系表的更新
	CIntMatrix	tempIntMatrix = CausTbl.copy(); //按照Change_Tbl的节点对照关系，先后交换原来Caus_Tbl中的行和列
	int size = m_nNodeNum;             //取网络的节点数
	for(int i=0;i<size;i++)          //按对照关系在行的维度上重新排序
	{
		for (int j=0;j<size;j++)
			CausTbl(ChangeTbl(i),j) = tempIntMatrix(i,j);
	}	
	tempIntMatrix=CausTbl.copy();               //按对照关系在列的维度上再次排序  
	for(i=0;i<size;i++)
	{
		for (int j=0;j<size;j++)
			CausTbl(j,ChangeTbl(i)) = tempIntMatrix(j,i);
	}
	
	CIntVector	tempIntVector = StateNum.copy();
	for(i=0;i<size;i++)      //按对照关系将状态个数重新排序
		StateNum(ChangeTbl(i)) = tempIntVector(i);
	
	if (DataMatrix.mcol()>0)
	{
		tempIntMatrix=DataMatrix.copy();
		for(i=0;i<size;i++)      //按对照关系将样本集合重新排序
		{
			for (int j=0;j<DataMatrix.mrow();j++) 
			DataMatrix(j,ChangeTbl(i))=tempIntMatrix(j,i);
		}
	}
	
	//节点列表重新排序
	CBNode*		pNode;
	POSITION	pos;
	CPtrList	tempList;
	for (i=0;i<m_nNodeNum;i++)
	{
		pos = m_NodePtrList.FindIndex(i);
		pNode = (CBNode*)m_NodePtrList.GetAt(pos);
		tempList.AddTail(pNode);
	}
	for (i=0;i<m_nNodeNum;i++)
	{
		pos = tempList.FindIndex(i);
		pNode = (CBNode*)tempList.GetAt(pos);
		pNode->setID(ChangeTbl(i));
		pos = m_NodePtrList.FindIndex(ChangeTbl(i));
		m_NodePtrList.SetAt(pos,pNode);
	}
}

	
//CDoubleMatrix CBNet::DataMatrixSetCompletion(int NodeNum,int GSimNum,CIntMatrix CausTbl,CIntVector StateNum,CDoubleMatrix DataMatrix)
	
//缺失数据处理模块(Gibbs采样法)
//输入数据
//NodeNum--    节点个数；
//GSimNum--    采样次数，推荐次数＝250，000×Mis_Num/(N*Sample_Num^2)；
//CausTbl--    排序后的节点关系表；
//StateNum--   排序后的各节点状态个数；
//DataMatix--  排序后的样本集合；

//查找缺失数据所在的行、列号
//k=Samples==0;
//k=find(k');
//if isempty(k)   %未发现缺失数据
//    fprintf('*** Data Set is Complete! ***\n');
//    return; 
//else
//    fprintf('*** totally %u values missed! ***\n',size(k,1));
//end
//Row=fix(k/N);
//Col=rem(k,N);
//%特别地，针对最后一列的变量行号减1，列号置为N
//temp=Col==0;
//Row=Row-temp;
//Col(find(temp))=N;
//Row=Row+1;
//%Mis_Num--缺失数据个数
//Mis_Num=size(Row,1);
//%先对所有缺失数据随机取样
//for i=1:Mis_Num
//	Samples(Row(i),Col(i))=ceil(State_Num(Col(i))*rand);
//end
//CPT=Samples_State_Calculate(N,Samples,State_Num,Caus_Tbl);
//%开始循环次数为Sim_Num的Gibbs采样
//for i=1:Sim_Num
//    j=1;
//    %Curr_Row－－当前处理的样本序号；S--当前处理的样本;
//    Curr_Row=Row(j);
//    S=Samples(Row(j),:);
//    if (sum(Caus_Tbl(:,Col(j)))~=0)  %若其中包含元素”1”，则为非根节点
//        Parnt=find(Caus_Tbl(:,Col(j)));
//        %查找各父节点状态，计算父节点的状态组合在CPT{j}中的列号
//		Column_Num=1;
//		for k=1:size(Parnt,1)
//			Column_Num=(Column_Num-1)*State_Num(Parnt(k))+S(Parnt(k));
//        end
//		%获得节点j在给定父节点下的条件概率
//		Prob=CPT{Col(j)}(:,Column_Num);
//		%产生随机数，并判断所属区间，给出State_Vec(j)的取值
//		Sample=rand;
//		State=1;
//			left=0;
//		while ~(Sample>=left & Sample<=left+Prob(State))			
//			left=left+Prob(State);
//			State= State+1;
//	%         if State>size(Prob,1)
//	%             fprintf('state %u ---- size of prob %u --- node %u\n',State,size(Prob,1),j);
//	%         end
//        end
//    else    %是根节点
//        Prob=CPT{Col(j)};
//		Sampl=rand;
//		State=1;
//		left=0;
//		while ~(Sampl>=left & Sampl<=left+CPT{Col(j)}(State))			
//			left=left+Prob(State);
//			State= State+1;
//        end
//    end
//	Samples(Row(j),Col(j))=State;
//    for j=2:Mis_Num
//        if Row(j)~=Curr_Row
//            CPT=Samples_State_Calculate(N,Samples,State_Num,Caus_Tbl);
//            Curr_Row=Row(j);
//        end
//        if (sum(Caus_Tbl(:,Col(j)))~=0)  %若其中包含元素”1”，则为非根节点
//            %查找当前CPT表，决定当前缺失数据的条件概率表
//            S=Samples(Row(j),:);
//            Parnt=find(Caus_Tbl(:,Col(j)));
//            %查找各父节点状态，计算父节点的状态组合在CPT{j}中的列号
//			Column_Num=1;
//			for k=1:size(Parnt,1)
//				Column_Num=(Column_Num-1)*State_Num(Parnt(k))+S(Parnt(k));
//            end
//			%获得节点j在给定父节点下的条件概率
//			Prob=CPT{Col(j)}(:,Column_Num);
//			%产生随机数，并判断所属区间，给出State_Vec(j)的取值
//			Sample=rand;
//			State=1;
//				left=0;
//			while ~(Sample>=left & Sample<=left+Prob(State))			
//				left=left+Prob(State);
//				State= State+1;
//		%         if State>size(Prob,1)
//		%             fprintf('state %u ---- size of prob %u --- node %u\n',State,size(Prob,1),j);
//		%         end
//            end
//        else
//            Prob=CPT{Col(j)};
//			Sampl=rand;
//			State=1;
//			left=0;
//			while ~(Sampl>=left & Sampl<=left+CPT{Col(j)}(State))			
//				left=left+Prob(State);
//				State= State+1;
//            end
//        end
//		Samples(Row(j),Col(j))=State;
//    end
//    CPT=Samples_State_Calculate(N,Samples,State_Num,Caus_Tbl);
//end

//////////////////////////////////////////////////////////
//依据网络结构生成每个节点的父节点个数和数组
//////////////////////////////////////////////////////////
void CBNet::SetParntArray()
{
	int i,j,k,parntNum=0;
	int size = m_nNodeNum;
	CBNode*	pNode;
	for (i=0;i<size;i++)
	{
		pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(i));
		int id = pNode->getID();
		parntNum=0;
		for(j=0;j<size;j++)
		{
			if(m_CausTbl(j,id)==1)
				parntNum++;
		}
		pNode->setParntNum(parntNum);
		CIntVector parntVector(parntNum);   //父结点向量
		k = 0;
		for(j=0;j<size;j++)
		{
			if(m_CausTbl(j,id)==1)
			{
				parntVector(k)=j;
				k++;
			}
		}
		pNode->setParntVector(parntVector);
	}
}

////////////////////////////////////////////////////////////////
//初始化各节点的条件概率表——均匀分布
////////////////////////////////////////////////////////////////
void CBNet::CPTInitiate()//,CIntMatrix DataMatrix,CIntVector StateNum,CIntMatrix CausTbl)
{
	CDoubleMatrix NodeCPTMatrix;     //节点CPT矩阵               //父结点个数
	int ColNum,i,j;                    //父结点组合数，即CPT表列数
	int size = m_nNodeNum;
	CBNode* pNode;
	for (int k=0;k<size;k++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(k));
		int NodeID = pNode->getID();

		int			parntNum = pNode->getParntNum();
		CIntVector	parntVector = pNode->getParntVector();
		
		ColNum=1;
		if(parntNum==0)                      //该节点为根
		{
			NodeCPTMatrix = CDoubleMatrix(m_StateNum(NodeID),ColNum,0.0);
			for(i=0;i<m_StateNum(NodeID);i++)
			{
				NodeCPTMatrix(i,0)=(double)1/(double)m_StateNum(NodeID);
			}
		}
		else                               //该节点不为根
		{
			for(i=0;i<parntNum;i++)
			{
				ColNum=ColNum*m_StateNum(parntVector(i));
			}
			NodeCPTMatrix = CDoubleMatrix(m_StateNum(NodeID),ColNum,0.0);
			for(i=0;i<m_StateNum(NodeID);i++)
			{
				for (j=0;j<ColNum;j++)
					NodeCPTMatrix(i,j)=(double)1/(double)m_StateNum(NodeID);//均值化处理
			}
		}
		pNode->setCPT(NodeCPTMatrix);
	}
}

////////////////////////////////////////////////////////////////
//计算各节点的条件概率表
////////////////////////////////////////////////////////////////
void CBNet::CPTCalculate()
{
	CDoubleMatrix NodeCPTMatrix;     //节点CPT矩阵
	int ColNum,i,j,k;                    //父结点组合数，即CPT表列数
	int size = m_nNodeNum;
	CBNode* pNode;

	for (int n=0;n<size;n++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(n));
		int NodeID = pNode->getID();
		int			parntNum = pNode->getParntNum();
		CIntVector	parntVector = pNode->getParntVector();
		
		int Under=0;
		ColNum=1;
		CIntVector AboveVector = CIntVector(m_StateNum(NodeID),0);
		for(i=0;i<m_StateNum(NodeID);i++)
		{
			AboveVector(i)=0;
		}
		
		if(parntNum==0)                      //该节点为根
		{
			Under=m_DataMatrix.mrow();	
			for(i=0;i<m_DataMatrix.mrow();i++)
			{
				for(j=0;j<m_StateNum(NodeID);j++)
				{
					if(m_DataMatrix(i,NodeID)==j)
						AboveVector(j)=AboveVector(j)+1;
				}	
			}
			NodeCPTMatrix = CDoubleMatrix(m_StateNum(NodeID),ColNum,0.0);
			for(i=0;i<m_StateNum(NodeID);i++)
			{
				NodeCPTMatrix(i,0)=(double)AboveVector(i)/(double)Under;
			}
		}
		else                               //该节点不为根
		{
			for(i=0;i<parntNum;i++)
			{
				ColNum=ColNum*m_StateNum(parntVector(i));
			}
			NodeCPTMatrix = CDoubleMatrix(m_StateNum(NodeID),ColNum,0.0);
			CIntMatrix StateTbl(ColNum,parntNum);
			CreateStateTbl(StateTbl,parntVector,parntNum);			
			for(i=0;i<StateTbl.mrow();i++)
			{
				Under = 0;
				for(j=0;j<m_StateNum(NodeID);j++)
				{
					AboveVector(j)=0;
				}
				for(j=0;j<m_DataMatrix.mrow();j++)
				{
					bool find;
					find=FindRow(StateTbl.row(i),m_DataMatrix.row(j),parntVector,parntNum);
					if(find)
					{
						Under++;
						for(k=0;k<m_StateNum(NodeID);k++)
						{
							if(m_DataMatrix(j,NodeID)==k)
								AboveVector(k)=AboveVector(k)+1;
						}				
					}
				}
				if (Under != 0)
				{
					for(j=0;j<m_StateNum(NodeID);j++)
					{
						NodeCPTMatrix(j,i)=(double)AboveVector(j)/(double)Under;
					}
				}
				else	//分母为零
				{
					for(j=0;j<m_StateNum(NodeID);j++)
					{
						NodeCPTMatrix(j,i)=(double)1/(double)m_StateNum(NodeID);//均值化处理
					}
				}
			}
		}
		pNode->setCPT(NodeCPTMatrix);
	}
}

//////////////////////////////////////////////////////////
//寻找父节点状态符合条件的样本行
//////////////////////////////////////////////////////////
bool CBNet::FindRow(CIntVector StateTbl,CIntVector DataMatrix,CIntVector parntVector,int parntNum)
{
	for(int j=0;j<parntNum;j++)
	{
		if(StateTbl(j)!=DataMatrix(parntVector(j)))
			return false;
	}
     return true;
}


//////////////////////////////////////////////////////////////////
//  Monte Carlo模拟采样
//////////////////////////////////////////////////////////////////
bool CBNet::BayesNetMCSim(/*CShowData*pDialogData*/)
{
	int			NodeNum = this->m_nNodeNum;					//节点个数
	CIntVector	StateNum = this->m_StateNum;				//各个节点的状态个数
	CIntMatrix	CausTbl = this->m_CausTbl;					//因果关系表
	int			SimNum = this->m_nSimNum;					//随机取样个数
	CIntMatrix SimDataMatrix = CIntMatrix(SimNum,NodeNum);	//一行为一个样本，一列为一个变量
	//CIntVector StateVector;								//节点状态向量
    CIntVector parntVector;									//父结点向量
	int parntNum=0;											//父结点个数
	CDoubleVector prob;										//概率
	
	for(int nowSimNum=0;nowSimNum<SimNum;nowSimNum++)
	{
		//TRACE("nowSimNum = %d\n",nowSimNum);
		for(int nowNode=0;nowNode<NodeNum;nowNode++)    //逐个产生每一个节点的状态
		{
			//TRACE("nowNode = %d:---",nowNode);
			CBNode *pNode = (CBNode *)(m_NodePtrList.GetAt(m_NodePtrList.FindIndex(nowNode)));
			parntNum = pNode->getParntNum();
			if(parntNum!=0)           //非根节点
			{
				parntVector = pNode->getParntVector();
				/*
				TRACE("parent: ");
				for (i=0;i<parntNum;i++)
				TRACE("%d  ",parntVector(i));
				TRACE("\n");*/
				
				//查找各父节点状态，计算父节点的状态组合在CPT表中的列号
				int columNum=0;
				for(int i=0;i<parntNum;i++)
				{
					//TRACE("ColumNum %d = %d\n",i,columNum);
					columNum = columNum*StateNum(parntVector(i))+SimDataMatrix(nowSimNum,parntVector(i));
				}
				//TRACE("ColumNum %d = %d\n",i,columNum);
				prob = pNode->getCPT().col(columNum);
				double sample = random(0,1);
				int state=0;
				double right=prob(0);
				while (sample>right)
				{
					state++;
					right=right+prob(state);
				}
				//TRACE("state = %d\n",state);
				SimDataMatrix(nowSimNum,nowNode)=state;
			}
			else     //该节点为根节点，则随机给定该节点的状态
			{
				prob=pNode->getCPT().col(0);
				double sample=random(0,1);
				int state=0;
				double right=prob(0);
				while (sample>right)
				{
					state++;
					right=right+prob(state);
				}
				/*
				TRACE("state = %d\n",state);
				if (nowNode == 2 && state == 2)
				{
				TRACE("prob =:");
				for (i=0;i<2;i++)
				TRACE("%.8f  ",prob(i));
				TRACE("\nsample = %.8f\n",sample);
				Sleep(1);
			}*/
				
				SimDataMatrix(nowSimNum,nowNode) = state;
			}
		}
	}
	this->m_SimDataMatrix = SimDataMatrix;
	return true;
}


///////////////////////////////////////////////////////////////////////
// 网络推理
///////////////////////////////////////////////////////////////////////
bool CBNet::BayesNetInference(/*CShowData*pDialogData*/)
{
	//贝叶斯信念网络推理
	CIntMatrix SimDataMatrix = this->m_SimDataMatrix;	//模拟样本集合
	CIntVector StateNum = this->m_StateNum;			//各节点状态个数
	CIntMatrix InitMatrix = this->m_InitMatrix;			//观察值矩阵
	CIntVector AimVector = this->m_AimVector;			//目标变量向量
	//CIntVector oldAimVector = this->m_OldAimVector;		//排序前的目标变量向量
	
	int i,j,Under=0;
	CIntVector MoveStep(AimVector.vlen(),0);
	MoveStep(0) = 0;
	if (AimVector.vlen()>1)
	{
		for (i=1;i<AimVector.vlen();i++)
			MoveStep(i) = MoveStep(i-1)+StateNum(AimVector(i-1));
		int VectorLen = 0;
		for (i=0;i<AimVector.vlen();i++)
			VectorLen = VectorLen+StateNum(AimVector(i));
		CIntVector AboveVector(VectorLen,0);
		CDoubleVector ProbVector(VectorLen,0);							//推理结果，按目标变量各状态的顺序给出相应的概率
		CIntMatrix tempSimDataMatrix = CIntMatrix(0,m_SimDataMatrix.mcol());//模拟样本集合中符合观察值的子集合
		//寻找并获得符合观察值的样本子集合SimDataMatrix
		for(i=0;i<SimDataMatrix.mrow();i++)
		{
			int find=0;
			for(j=0;j<InitMatrix.mrow();j++)
			{
				if(SimDataMatrix(i,InitMatrix(j,0))==InitMatrix(j,1))
					find++;
			}
			if(find == InitMatrix.mrow())
				tempSimDataMatrix = commat(SimDataMatrix.row(i),tempSimDataMatrix,2);//矩阵连接		
		}
		Under=tempSimDataMatrix.mrow();
		if (Under==0)//模拟次数太小，没有找到满足观察值的样本，导致概率计算时，分母为零
			return false;
		//统计SimDataMatrix中目标变量取不同状态的样本个数和概率
		for(i=0;i<tempSimDataMatrix.mrow();i++)
		{
			for (j=0;j<AimVector.vlen();j++)
				AboveVector(MoveStep(j)+tempSimDataMatrix(i,AimVector(j)))++;
		}	
		for (i=0;i<VectorLen;i++)
			ProbVector(i) = (double)AboveVector(i)/(double)Under;
		this->m_ProbVector = ProbVector;
	}
	return true;
}

//////////////////////////////////////////////////////////
// 产生随机数
//////////////////////////////////////////////////////////
double CBNet::random(double mi,double ma)
{
	int minInteger=(int)(mi*10000);
	int maxInteger=(int)(ma*10000);
	int randInteger=rand()*rand();
	int diffInteger=maxInteger-minInteger;
	int resultInteger=randInteger%diffInteger+minInteger;
	return resultInteger/10000.0;
}

////////////////////////////////////////////////////////////////
// 从节点链表中获得节点的状态个数
////////////////////////////////////////////////////////////////
bool CBNet::GetStateNum()
{
	int NodeNum,sum;
	NodeNum = m_NodePtrList.GetCount();
	CIntVector StateNum(NodeNum,0);
	POSITION	pos;
	CBNode*		pNode;
	pos = m_NodePtrList.GetHeadPosition();
	sum=0;
	while (pos != NULL)
	{
		pNode = (CBNode*)m_NodePtrList.GetNext(pos);
		int sn = pNode->getStateNum();
		if (sn>1)
			StateNum(sum) = sn;
		else
		{
			//AfxMessageBox("节点状态个数必须大于1 ！");
			return false;
		}
		sum++;
	}
	m_StateNum = StateNum.copy();
	return true;
}

/////////////////////////////////////////////////////////////
// 将来自界面的样本数据块读入矩阵并初始化节点列表
/////////////////////////////////////////////////////////////
CIntMatrix  CBNet::GetData()
{
	int nRow=m_pDataInterface->GetRowCount();
	int nCol=m_arFieldName.GetSize();
	int i,j;
	CIntMatrix matrix;
	CIntVector dataType(nCol,0);
	//获得各变量的数据类型
	for (i=0;i<nCol;i++)
	{
		int nIndex = m_pDataInterface->IndexByName(m_arFieldName.GetAt(i));
		dataType(i) = m_pDataInterface->GetFieldType(nIndex);
		if (dataType(i) == fDate || dataType(i) == fCurrency)
		{
			m_sErrorInfo = _T("数据类型错误，贝叶斯网络不接受日期和货币型数据！");
			m_bErrorFlag = true;		
			matrix.destruction();
			return matrix;
		}
	}
	//逐个产生节点，建立节点列表
	for (i=0;i<nCol;i++)
	{
		CTString fieldname = m_arFieldName.GetAt(i);
		if (!fieldname.IsEmpty() && fieldname.GetLength()<126)
		{			
			CBNode *pNode=new CBNode;
			pNode->setID(i);
			pNode->setName(fieldname);
			m_NodePtrList.AddTail(pNode);		
		}
		else
		{
			m_sErrorInfo = _T("字段名称不能为空或长度超过126个字符！");
			m_bErrorFlag = true;
			matrix.destruction();
			return matrix;
		}
	}
	//读取数据，去掉缺失值，建立由下标构成的数据矩阵，同时生成每个节点的标签列表
	CField* pField;
	CDataAccess* pDataAccess=&m_pDataInterface->m_DataAccess;	
	CIntMatrix tempMatrix(nRow,nCol,0);
	pDataAccess->First();
	int nCount = 0;
	bool Flag = false;
	int index = 0;
	for (i=0;i<nRow;i++)
	{
		Flag = false;
		for (j=0;j<nCol;j++)
		{
			pField=pDataAccess->m_pFieldList->FieldByName(m_arFieldName.GetAt(j));
			if(m_pDataInterface->IsMissingValue(pField))
			{
				Flag = true;
				break;
			}
		}
		if (!Flag)
		{
			for (j=0;j<nCol;j++)
			{
				pField=pDataAccess->m_pFieldList->FieldByName(m_arFieldName.GetAt(j));
				CBNode*	pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(j));
				switch(dataType(j)) {
				case fInt:
					  {
						  CTString str;
						  int cell = pField->GetAsInteger();
						  if (pField->GetValueVL(cell,str))
						  {
							  if (str.GetLength()>126)
								  str = str.Left(126);
							  index = pNode->m_StateLable.add(str);	
						  }
						  else
							  index = pNode->m_StateLable.add(cell);
						  if (index<32)
							  tempMatrix(nCount,j) = index;
						  else
						  {
							  m_sErrorInfo.Format("贝叶斯网络不接受大于 31 的状态个数，\n\n请考虑减少变量%s中的状态个数！",pNode->getName());
							  m_bErrorFlag = true;
							  matrix.destruction();
							  return matrix;
						  }
						  break;
					  }
				case fDouble:
					  {
						  CTString str;
						  double cell = pField->GetAsDouble();
						  if (pField->GetValueVL(cell,str))
						  {
							  if (str.GetLength()>126)
								  str = str.Left(126);
							  index = pNode->m_StateLable.add(str);
						  }
						  else
							  index = pNode->m_StateLable.add(cell);
						  if (index<32)
							  tempMatrix(nCount,j) = index;
						  else
						  {
							  m_sErrorInfo.Format("贝叶斯网络不接受大于 31 的状态个数，\n\n请考虑减少变量%s中的状态个数！",pNode->getName());
							  m_bErrorFlag = true;
							  matrix.destruction();
							  return matrix;
						  }
						  break;
					  }
				case fString:
					 {
						 CTString str;
						 CTString cell = pField->GetAsString();
						 if (pField->GetValueVL(cell,str))
						 {
							 if (str.GetLength()>126)
								 str = str.Left(126);
							 index = pNode->m_StateLable.add(str);
						 }
						 else
						 {
							 if (cell.GetLength()>126)
								 cell = cell.Left(126);
							 index = pNode->m_StateLable.add(cell);
						 }
						 if (index<32)
							 tempMatrix(nCount,j) = index;
						 else
						 {
							 m_sErrorInfo.Format("贝叶斯网络不接受大于 31 的状态个数，\n\n请考虑减少变量%s中的状态个数！",pNode->getName());
							 m_bErrorFlag = true;
							 matrix.destruction();
							 return matrix;
						 }
						 break;
					 }
				case fBoolean:
					  {
						  CTString str;
						  bool cell = pField->GetAsBool();
						  if (pField->GetValueVL(cell,str))
						  {
							  if (str.GetLength()>126)
								  str = str.Left(126);
							  index = pNode->m_StateLable.add(str);
						  }
						  else
							  index = pNode->m_StateLable.add(cell);
						  if (index<32)
							  tempMatrix(nCount,j) = index;
						  else
						  {
							  m_sErrorInfo.Format("贝叶斯网络不接受大于 31 的状态个数，\n\n请考虑减少变量%s中的状态个数！",pNode->getName());
							  m_bErrorFlag = true;
							  matrix.destruction();
							  return matrix;
						  }
						  break;
					  }
				}				
			}
			nCount++;
		}
		pDataAccess->Next();
	}	
	if (nCount == 0)
	{
		matrix.destruction();
		int NodeNum = this->m_NodePtrList.GetCount();
		for (i=0;i<NodeNum;i++)
		{
			CBNode *pNode = (CBNode*)this->m_NodePtrList.GetTail();
			this->m_NodePtrList.RemoveTail();
			delete pNode;
		}
		return matrix;
	}
	//设置节点状态个数
	for (j=0;j<nCol;j++)
	{
		pField=pDataAccess->m_pFieldList->FieldByName(m_arFieldName.GetAt(j));
		CBNode*	pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(j));
		pNode->setStateNum(pNode->m_StateLable.GetLableCount());
	}
	//去掉数据矩阵tempMatrix末尾的空行，建立数据矩阵matrix
	matrix = CIntMatrix(nCount,nCol,0);
	if (nRow == nCount)
		matrix = tempMatrix;
	else
	{
		nRow = nCount;		
		for (i=0;i<nRow;i++)
		{
			for (j=0;j<nCol;j++)
			{
				matrix(i,j) = tempMatrix(i,j);
			}
		}
		tempMatrix.destruction();
	}
	return matrix;
}

//////////////////////////////////////////////////////////////////////////
//train function
//////////////////////////////////////////////////////////////////////////
CResult* CBNet::Train(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
					  CBayesVO*pBayesVO, CBayesOutput &ppOutStruct)
{
	CResult *pResult = new CResult;

	InitiateNet(pData,pBayesVO);
	LearnParam();
	pResult = CreateResult();
	
	return pResult;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CResult * CBNet::Pred(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt,
					  CBayesVO*pBayesVO, CBayesOutput &ppOutStruct)
{
	BayesNetInference();
	return Output();
}

///////////////////////////////////////////////////////////////////////
// 计算网络每个节点的条件概率表
///////////////////////////////////////////////////////////////////////
bool CBNet::LearnParam()
{	
	//m_OldAimVector = m_AimVector.copy();
	CBNet::newCausTbl(m_CausTbl,m_ChangeTbl,m_StateNum,m_DataMatrix);  //按照新的节点排序，变换节点关系表、节点状态个数、样本集合以及一直变量和目标变量的节点顺序
	this->SetParntArray();//计算各节点的父节点个数和数组	
	if (m_DataMatrix.mcol()!=0)
		CPTCalculate();	//计算各节点的CPT表
	else
		CPTInitiate();//按均匀分布初始化各节点的CPT表

	return true;
}

CResult * CBNet::CreateResult()
{
	CBNode*		pNode;
	int id,nodeNum,i,j,rows,cols;
	CTString str;
	CIntVector parntVector,stateNum;
	CDoubleMatrix	cpt;
	CResult *pResult = new CResult("贝叶斯网络");

	for (i=0;i<m_arFieldName.GetSize();i++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
		CTLTable *pTable = new CTLTable;
		CRsltElementTable *pVariable = new CRsltElementTable(pNode->getName(),pTable);
		pTable->SetTitle(pNode->getName());
		stateNum = m_StateNum;
		nodeNum = m_nNodeNum;

		POSITION	pos = m_NodePtrList.FindIndex(0);
		pNode = (CBNode*)m_NodePtrList.GetAt(pos);
		id = pNode->getID();
		int nParntNum = pNode->getParntNum();
		parntVector = pNode->getParntVector();
		cpt = pNode->getCPT();
		rows = cpt.mrow();
		cols = cpt.mcol();
		//初始化为0号节点的CPT表

		//父节点名称和父节点状态——上表头
		if (nParntNum > 0)
		{
			int step = 1;
			pTable->InsertColumn(0,"");
			for (i=0;i<nParntNum;i++)
			{
				pos = m_NodePtrList.FindIndex(parntVector(i));
				pNode = (CBNode*)m_NodePtrList.GetAt(pos);
				int state = 0;
				for (j=0;j<(int)(cols/step);j++)
				{
					pTable->InsertColumn(i*cols/step+j+1,pNode->m_StateLable.GetLableByIndex(state));
					state++;
					if (state>=stateNum(parntVector(i)))
						state = 0;
				}
				step = step*stateNum(parntVector(i));
			}
		}
		
		//表的内容
		for (i=0;i<rows;i++)//行		
		{
			pTable->InsertItem(i,pNode->m_StateLable.GetLableByIndex(i));//左表头
			for (j=0;j<cols;j++)//列
			{
				pTable->SetItemText(i,j,cpt(i,j)*100);
			}
		}	
		
		pResult->Add(pVariable);
	}

	return pResult;
}

///////////////////////////////////////////////////////////////////////
// 输出结果概率表
///////////////////////////////////////////////////////////////////////
CResult* CBNet::Output(/*CWnd* pWnd*/)
{
	int i,j;
	CTString str;
	CResult*	pResult = new CResult("贝叶斯信念网络");
	//输出已知条件表格——推理依据
	CTLTable * pConditionTable = new CTLTable;
	
	pConditionTable->SetTitle("已知变量的状态观察值");

	pConditionTable->InsertColumn(0,"节点名称");
	CBNode*	pNode;
	for(i=0;i<m_InitMatrix.mrow();i++)
	{
		pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(m_InitMatrix(i,0)));
		pConditionTable->InsertColumn(i+1,pNode->getName());
	}
	pConditionTable->InsertItem(1,"状态取值");
	for(i=0;i<m_InitMatrix.mrow();i++)
	{
		pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(m_InitMatrix(i,0)));
		str = pNode->m_StateLable.GetLableByIndex(m_InitMatrix(i,1));
		//str = pNode->m_StateList.GetAt(pNode->m_StateList.FindIndex(m_InitMatrix(i,1)-1));
		pConditionTable->SetItemText(1,i+1,str);
	}
	
	CRsltVariable * pETable= new CRsltElementTable("已知条件",pConditionTable);
	pResult->Add(pETable);
	//输出文本——有效记录条数
	CRsltElementText *pRsltElementText = new CRsltElementText("有效记录条数");
	str.Format("%d",this->m_DataMatrix.mrow());
	pRsltElementText->AddString("有效记录条数 = "+str);
	pResult->Add(pRsltElementText);
	//输出文本——蒙特卡洛模拟次数
	pRsltElementText = new CRsltElementText("蒙特卡洛模拟次数");
	str.Format("%d",this->m_nSimNum);
	pRsltElementText->AddString("蒙特卡洛模拟次数 = "+str);
	pResult->Add(pRsltElementText);
	//输出目标变量概率——推理结果
	//表结果
	int step = 0;
	for (i=0;i<m_AimVector.vlen();i++)
	{
		CBNode*	pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(m_AimVector(i)));
		CTLTable * pResultTable = new CTLTable;		
		
		CTString name = pNode->getName();

		//pConditionTable->CreateTable(m_pWnd);
		pResultTable->SetTitle(name);

		pResultTable->SetRows(2);
		pResultTable->SetCols(m_StateNum(m_AimVector(i))+1);

		pResultTable->InsertColumn(0,"状态取值");
		
		for(j=0;j<m_StateNum(m_AimVector(i));j++)
		{
			str = pNode->m_StateLable.GetLableByIndex(j);
			//str = pNode->m_StateList.GetAt(pNode->m_StateList.FindIndex(j));
			pResultTable->InsertColumn(j+1,str);
		}
		pResultTable->InsertItem(1,"概率值×100％");
		for(j=0;j<m_StateNum(m_AimVector(i));j++)
		{
			str.Format("%.2f",m_ProbVector(step+j)*100);
			pResultTable->SetItemText(1,j+1,str);
		}
		CRsltVariable * pETable= new CRsltElementTable("推理结果－"+name,pResultTable);
		pResult->Add(pETable);
		step = step+m_StateNum(m_AimVector(i));
	}
	//图结果
	step = 0;
	for (i=0;i<m_AimVector.vlen();i++)
	{
		CBNode*	pNode = (CBNode*)this->m_NodePtrList.GetAt(this->m_NodePtrList.FindIndex(m_AimVector(i)));
		TDataPointStrVal* pDPSV = new TDataPointStrVal[m_StateNum(m_AimVector(i))];
	
		for(j=0;j<m_StateNum(m_AimVector(i));j++)
		{			
			pDPSV[j].fYVal = m_ProbVector(step+j)*100;
			CTString val;
			val.Format("%.2f",m_ProbVector(step+j)*100);
			str = pNode->m_StateLable.GetLableByIndex(j);
			//str = pNode->m_StateList.GetAt(pNode->m_StateList.FindIndex(j));
			str = str+"\n("+val+"%)";
			pDPSV[j].fXVal = str;
		}
		CTChartBar* pTChartBar=new CTChartBar;
		pTChartBar->SetBar(pDPSV,m_StateNum(m_AimVector(i)),StrVal);
		pTChartBar->SetTitle(pNode->getName());
//		pTChartBar->SetAxisRange(0,1,(double)m_StateNum(m_AimVector(i)),0,100);		
		CRsltElementChart* pREC=new CRsltElementChart(pNode->getName()+"－概率柱状图",pTChartBar);
		pResult->Add(pREC);
		step = step+m_StateNum(m_AimVector(i));
	}

	return pResult;
}

///////////////////////////////////////////////////////////////////////
//存储网络结构
///////////////////////////////////////////////////////////////////////
bool CBNet::SaveNet()
{
	CTString fileName = m_NetName;
	FILE * fp = fopen(fileName,"w");
	if (fp == NULL)
	{
// 		AfxMessageBox("无法保存文件，请确认您是否具有磁盘写权限？");
		return false;
	}
	int i,j,k;
	fprintf(fp,"%d\n",m_nNodeNum);
	fprintf(fp,"%%Node_Name\n");//写Node_Name
	CBNode*	pNode;
	for (i=0;i<m_nNodeNum;i++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
		fprintf(fp,"--%s",pNode->getName());
	}		
	fprintf(fp,"\n%%Caus_Tbl\n");//写%Caus_Tbl
	for (i=0;i<m_nNodeNum;i++)
	{
		for (j=0;j<m_nNodeNum-1;j++)
			fprintf(fp,"%d ",m_CausTbl(i,j));
		fprintf(fp,"%d\n",m_CausTbl(i,j));
	}
//	fprintf(fp,"\n%%Location_inImage\n");//写%Location_inImage
//	for (i=0;i<m_nNodeNum;i++)
//	{
//		CRect rect;
//		rect = rectrect.GetAt(rectrect.FindIndex(i));
//		fprintf(fp,"%u %u %u %u\n",rect.left,rect.top,rect.right,rect.bottom);
//	}
	fprintf(fp,"%%State_Num\n");//写%State_Num
	for (i=0;i<m_nNodeNum-1;i++)
		fprintf(fp,"%d ",m_StateNum(i));
	fprintf(fp,"%d\n",m_StateNum(i));
	fprintf(fp,"%%State_List\n");//写%State_List
	for (i=0;i<m_nNodeNum;i++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
		for (j=0;j<m_StateNum(i)-1;j++)
		{
			fprintf(fp,"%s ",pNode->m_StateLable.GetLableByIndex(j));
			//fprintf(fp,"%s ",pNode->m_StateList.GetAt(pNode->m_StateList.FindIndex(j)));
		}			
		fprintf(fp,"%s\n",pNode->m_StateLable.GetLableByIndex(j));
		//fprintf(fp,"%s\n",pNode->m_StateList.GetAt(pNode->m_StateList.FindIndex(j)));
	}
	fprintf(fp,"%%CPT\n");
	CDoubleMatrix	cpt;
	for (i=0;i<m_nNodeNum-1;i++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
		cpt = pNode->getCPT();
		int nRow = cpt.mrow();
		int nCol = cpt.mcol();
		fprintf(fp,"# %d %d %d\n",i+1,nRow,nCol);
		for (j=0;j<nRow;j++)
		{
			for (k=0;k<nCol-1;k++)
				fprintf(fp,"%.8f ",cpt(j,k));
			fprintf(fp,"%.8f\n",cpt(j,k));
		}
	}
	pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
	cpt = pNode->getCPT();
	int nRow = cpt.mrow();
	int nCol = cpt.mcol();
	fprintf(fp,"# %d %d %d\n",i+1,nRow,nCol);
	for (j=0;j<nRow-1;j++)
	{
		for (k=0;k<nCol-1;k++)
			fprintf(fp,"%.8f ",cpt(j,k));
		fprintf(fp,"%.8f\n",cpt(j,k));
	}
	for (k=0;k<nCol-1;k++)
		fprintf(fp,"%.8f ",cpt(j,k));
	fprintf(fp,"%.8f",cpt(j,k));	
	fclose(fp);
	return true;
}

CTString ReadString(FILE *fp,CTString format) 
{
	char buf[128];
	memset(buf,0,128);
	fscanf(fp,format,buf);
	int nLen = strlen(buf);
	CTString str = buf;
	return str;
}
int ReadInt(FILE *fp,CTString format) 
{
	char buf[128];
	memset(buf,0,128);
	fscanf(fp,format,buf);
	int value = atol(buf);
	return value;
}
double ReadDouble(FILE *fp,CTString format) 
{
	char buf[128];
	memset(buf,0,128);
	fscanf(fp,format,buf);
	double value = atof(buf);
	return value;
}

///////////////////////////////////////////////////////////////////////
//读取网络结构
///////////////////////////////////////////////////////////////////////
bool CBNet::ReadNet()
{  
	int i,j,k;
	CBNode*	pNode=NULL;
	if (this != NULL)
	{
		//先清空节点列表和数据矩阵
		if (!this->m_NodePtrList.IsEmpty())
		{
			int len = this->m_NodePtrList.GetCount();
			for (i=0;i<len;i++)
			{
				pNode = (CBNode*)m_NodePtrList.GetTail();
				m_NodePtrList.RemoveTail();
				delete pNode;
			}
		}
		this->m_DataMatrix.destruction();//清空数据矩阵
	}
	CTString fileName = m_NetName;
	FILE * fp = fopen(fileName,"r");
	char	buf[255];
	memset(buf,0,255);
	CTString netName = fileName;
	netName = netName.Right(netName.GetLength()-netName.ReverseFind('\\')-1);
	sscanf(netName,"%[^.bn]",buf);
	netName = buf;
	this->m_NetName = netName;
	if (fp == NULL)
	{
// 		AfxMessageBox("无法打开文件，请确认路径和文件名是否正确！");
		return false;
	}
	CTString str,name;
	int NodeNum = ReadInt(fp,"%s\n");//读入节点个数
	this->m_nNodeNum = NodeNum;
	
	fscanf(fp,"%s\n",buf);//读入“%Node_Name”
	memset(buf,0,255);
	for (i=0;i<NodeNum;i++)
	{
		str = ReadString(fp,"%[--]");
		name = ReadString(fp,"%[^--\n]");
		if (name.IsEmpty())
		{
// 			AfxMessageBox("无法正确读取网络，格式错误！");
			return false;
		}
		else
		{
			pNode = new CBNode;
			pNode->setName(name);
			pNode->setID(i);
			this->m_NodePtrList.AddTail(pNode);
		}
	}

	str = ReadString(fp,"%s\n");//读入“%Caus_Tbl”
	CIntMatrix	causTbl(NodeNum,NodeNum);
	for (i=0;i<NodeNum;i++)
		for (j=0;j<NodeNum;j++)
		{
			int value = ReadInt(fp,"%s\n");
			if (value == 0)
				causTbl(i,j) = value;
			else
				causTbl(i,j) = 1;
		}			
	this->m_CausTbl = causTbl;
	//计算每个节点的父节点个数m_PanrtNum和父节点向量m_ParntVector
	this->SetParntArray();

//	str = ReadString(fp,"%s\n");//读入“%Location_inImage”
//	rectrect.RemoveAll();
//	for (i=0;i<NodeNum;i++)
//	{
//		CRect rect;
//		rect.left = ReadInt(fp,"%s\n");
//		rect.top = ReadInt(fp,"%s\n");
//		rect.right = ReadInt(fp,"%s\n");
//		rect.bottom = ReadInt(fp,"%s\n");
//		rectrect.AddTail(rect);
//	}
	str = ReadString(fp,"%s\n");//读入“%State_Num”
	CIntVector	sns(NodeNum);
	int value;
	POSITION pos;
	for (i=0;i<NodeNum;i++)
	{
		value = ReadInt(fp,"%s\n");
		if (value<=0);
// 			AfxMessageBox("节点状态个数须为自然数！");
		else
		{
			sns(i) = value;
			pos = m_NodePtrList.FindIndex(i);
			pNode = (CBNode*)m_NodePtrList.GetAt(pos);
			pNode->setStateNum(value);
		}
	}		
	this->m_StateNum = sns;
	str = ReadString(fp,"%s\n");//读入“%State_List”
	for (i=0;i<NodeNum;i++)
	{
		pNode = (CBNode*)m_NodePtrList.GetAt(m_NodePtrList.FindIndex(i));
		//pNode->m_StateList.RemoveAll();
		pNode->m_StateLable.RemoveAll();
		for (j=0;j<m_StateNum(i);j++)
		{
			str = ReadString(fp,"%s\n");
			pNode->m_StateLable.add(str);
		}
	}
	str = ReadString(fp,"%s\n");//读入“%CPT”
	if (str != "%CPT")
	{
// 		AfxMessageBox("状态列表——格式错误！请检查文件格式！");
		return false;
	}
	int nRow,nCol;
	double	prob;
	for (i=0;i<NodeNum;i++)
	{
		str = ReadString(fp,"%[# ]\n");
		str = ReadString(fp,"%s\n");
		nRow = ReadInt(fp,"%s\n");
		nCol = ReadInt(fp,"%s\n");
		CDoubleMatrix	cpt(nRow,nCol);
		for (j=0;j<nRow;j++)
			for (k=0;k<nCol;k++)
			{					
				prob = ReadDouble(fp,"%s\n");
				cpt(j,k) = prob;
			}	
		pos = m_NodePtrList.FindIndex(i);
		pNode = (CBNode*)m_NodePtrList.GetAt(pos);
		pNode->setCPT(cpt);
	}
	return true;
}
	