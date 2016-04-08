//----HopGen.cpp-----//

/********************************************************************************/
/************** 用Hopfield神经网络、遗传算法进行聚类程序主文件 ******************/
/******************      创建者：  刘堂友                 ***********************/
/******************      创建日期：Oct.7 2002             ***********************/
/******************      修改者：                         ***********************/
/********************************************************************************/ 
#include "StdAfx.h"
#include "math.h"
#include "HopGen.h"
#include "../cpknl/cpknl.h"
//const UINT WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
//const UINT WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
/****缺省构造函数*****************************/
CHopGen::CHopGen()
{
	m_OldPop=NULL;
	m_NewPop=NULL;
	m_BestIndvd=NULL;

	m_bAbort = false;
}

/****初始化参数的构造函数*********************/
CHopGen::CHopGen(CDoubleVector  dblData,
					 int     m_NumPoints,
					 int     NumDem,
					 int     NumClass,
					 int     ClusterType,
					 unsigned int   dwTimeOut,
					 int     nMaxGen,
					 double  dblCrossRatio,
					 double  dblMutationRatio)
{
	m_OldPop=NULL;
	m_NewPop=NULL;
	Initialize(dblData,
			   m_NumPoints,
		       NumDem,
			   NumClass,
			   ClusterType,
			   dwTimeOut,
			   nMaxGen,
			   dblCrossRatio,
			   dblMutationRatio);
}

/****释构函数，释放已分配的内存****************/
CHopGen::~CHopGen()
{
}

/****初始化各成员变量及分配内存******************/
void CHopGen::Initialize(CDoubleVector dblData, 
					 int     NumPoints,
					 int     NumDem,  
					 int     NumCluster,
					 int     ClusterType,
					 unsigned int   dwTimeOut,
					 int     nMaxGen,
					 double  dblCrossRatio,
					 double  dblMutationRatio)
{
	m_dblData=dblData;
	m_NumPoints=NumPoints;	
	m_NumDem=NumDem;
	m_NumCluster=NumCluster;
	m_dwTimeOut=dwTimeOut;
	m_ClusterType=ClusterType;
	m_LenChrom=m_NumPoints;

	switch(m_ClusterType){
	case 0:
		InitHopfield();
		break;
	case 1:
		InitGenetic(nMaxGen,dblCrossRatio,dblMutationRatio);
		break;
	case 2:
		InitHopfield();
// 		InitGenetic(nMaxGen,dblCrossRatio,dblMutationRatio);
		break;
	default:
		break;
	}

	if( m_bAbort ) return;

//	try{
		m_BestIndvd=new BestIndividual;
		m_BestIndvd->btChrom.create(m_LenChrom);
		m_BestIndvd->dblCenterCrd.create(m_NumCluster*m_NumDem);
		m_BestIndvd->dblRangeMax.create(m_NumCluster*m_NumDem);
		m_BestIndvd->dblRangeMin.create(m_NumCluster*m_NumDem);
		m_BestIndvd->dblRange.create(m_NumDem*2);

		m_BestIndvd->dblDistance=0;
		m_BestIndvd->dblFitness=0;
		m_BestIndvd->nGeneration=0;
//	}
//	catch(CMemoryException MemExp){
//////////////////////////////////////////////////////////////////////
//                                                                  //
//        请加入处理内存异常程序代码                                //
//                                                                  //
//////////////////////////////////////////////////////////////////////
// 	}

	//////////////////////////////////////
	// 保存训练数据各字段最大最小值
	//
	// 添加: QT
	// 时间: 2003.01.24
	//
//	memcpy( m_BestIndvd->dblRange, m_dblRange, 2*m_NumDem*sizeof(double) );
	m_BestIndvd->dblRange = m_dblRange;
	// QT
	//////////////////////////////////////

}
void CHopGen::NormalizeData()
{
	int i,j;

	/////////////////////////////////////////////////////
	// 输出进度信息
	// QT
/*
	CTString strProgressText;
	strProgressText = _T("标准化数据样本...");
	m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
*/
	//
	//////////////////////////////////////////////////////

	//m_dblRange[0]最小，m_dblRange[1]最大
	m_dblRange.create(m_NumDem*2);
	for(i=0;i<m_NumDem;i++){
		m_dblRange[i*2+0]=9.99e50;//m_dblData[0*m_NumDem+0];
		m_dblRange[i*2+1]=-9.99e50;
	}

	for(i=0;i<m_NumPoints && !m_bAbort;i++){
		for(j=0;j<m_NumDem;j++){
			if(m_dblRange[2*j+0]>m_dblData[i*m_NumDem+j])
				m_dblRange[2*j+0]=m_dblData[i*m_NumDem+j];
			if(m_dblRange[2*j+1]<m_dblData[i*m_NumDem+j])
				m_dblRange[2*j+1]=m_dblData[i*m_NumDem+j];
		}
	}
///////////////
// 	TRACE( "\n\nCal Range\n" );
	for( i=0; i<m_NumDem; i++ )
// 	   TRACE("%6.4f  %6.4f\n",m_dblRange[2*i],m_dblRange[2*i+1]);
//////////////	
	for(i=0;i<m_NumPoints && !m_bAbort;i++){
		for(j=0;j<m_NumDem;j++){
			m_dblData[i*m_NumDem+j]=(m_dblData[i*m_NumDem+j]-m_dblRange[2*j+0])/(m_dblRange[2*j+1]-m_dblRange[2*j+0]);
		}
	}

}
void CHopGen::AntiNormalizeData()
{
	int i,j;
	if( !m_bAbort )
	{
		for(i=0;i<m_NumPoints;i++){
			for(j=0;j<m_NumDem;j++){
				m_dblData[i*m_NumDem+j]=m_dblData[i*m_NumDem+j]*(m_dblRange[2*j+1]-m_dblRange[2*j+0])+m_dblRange[2*j+0];
			}
		}
		for(i=0;i<m_NumCluster;i++){
			for(j=0;j<m_NumDem;j++){
				m_BestIndvd->dblCenterCrd[i*m_NumDem+j]=m_BestIndvd->dblCenterCrd[i*m_NumDem+j]*(m_dblRange[2*j+1]-m_dblRange[2*j+0])+m_dblRange[2*j+0];
//				TRACE("%5.4f   ",m_BestIndvd->dblCenterCrd[i*m_NumDem+j]);
			}
//			TRACE("\n");
		}
	}
}
void CHopGen::InitHopfield()
{
	int NN=m_NumPoints*m_NumCluster;
	NormalizeData();

	if( m_bAbort )
		return;

	m_dblStep=0.0018;
	m_NumTrain=(int)(3200/(1+exp(-NN/90000)));//-1200);
//	try{		
		m_dblCenterCrd.create(m_NumCluster*m_NumDem);
		InitClusterCenter(m_dblCenterCrd);
		m_dblDistance.create(NN);
		m_dblUU.create(NN);
		m_dblVV.create(NN);
		m_dblTempVV.create(14000);
		int i=-1;
		double uu=-0.1;
		int a=0;
		double sub=0.000014;
		for(i=0;i<14000 && !m_bAbort;i++){		
			uu+=sub;
			if(uu<0.000014&&uu>(-0.000014))
				a=i;
			m_dblTempVV[i]=(1+tanh(uu/0.02))/2;	
		}
//	}
//	catch(CMemoryException MemExp){
////////////////////////////////////////////////////////////////////////
////                                                                  //
////        请加入处理内存异常程序代码                                //
////                                                                  //
////////////////////////////////////////////////////////////////////////
//	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：InitGenetic
//函数名称：int nMaxGen,最大世代数
//			double dblCrossRatio,交叉概率
//			double  dblMutationRatio变异概率
//函数功能：初始化遗传算法部分
//////////////////////////////////////////////////////////////////////////
void CHopGen::InitGenetic(int nMaxGen,double dblCrossRatio,double  dblMutationRatio)
{
	m_dblSumFitness=0;
	m_dblMax=-1;
	m_dblAvg=0;
	m_dblMin=0;
	m_nGen=0;
	m_nMutation=0;
	m_nCross=0;
	m_nLoop=0;
	m_nAddLoop=0;
	m_nRand=0;
	m_nMaxSite=0;
	m_nMinSite=0;
	m_nMaxGen=nMaxGen;
	m_dblCrossRatio=dblCrossRatio;
	m_dblMutationRatio=dblMutationRatio;

	m_LenChrom=m_NumPoints;
	m_NumPop=InitPopsize();
//	try{
		m_OldPop=new Individual[m_NumPop];
		m_NewPop=new Individual[m_NumPop];
		m_dblOldRand.create(m_NumPop);
		int i;
		for(i=0;i<m_NumPop;i++){
			m_OldPop[i].btChrom.create(m_LenChrom);
			m_OldPop[i].dblCenterCrd.create(m_NumCluster*m_NumDem);

			m_NewPop[i].btChrom.create(m_LenChrom);
			m_NewPop[i].dblCenterCrd.create(m_NumCluster*m_NumDem);
			m_dblOldRand[i]=0;
		}
	
		for(i=0;i<6;i++){
			m_TempIndvd[i].btChrom.create(m_LenChrom);
			m_TempIndvd[i].dblCenterCrd.create(m_NumCluster*m_NumDem);
		}
		if(m_ClusterType==2)
			m_dblOldCenterCrd.create(m_NumCluster*m_NumDem);
//	}
//	catch(CMemoryException MemExp){
////////////////////////////////////////////////////////////////////////
////                                                                  //
////        请加入处理内存异常程序代码                                //
////                                                                  //
////////////////////////////////////////////////////////////////////////
//	}
	
}

/******遗传算法进行聚类主函数************************************/
BestIndividual* CHopGen::GeneticCluster()
{	
	InitPop();
	ClusterGA(0);
	return m_BestIndvd; 
}

//////////////////////////////////////////////////////////////////////////
//函数名称：ClusterGA
//函数功能：遗传算法进行聚类
//////////////////////////////////////////////////////////////////////////
void CHopGen::ClusterGA(unsigned int dwBegTimeTick)
{
	int i;
	bool bTimeOut=false;
	InitRandomize();
	for(i=0;i<m_NumPop;i++){
		Decode(m_OldPop[i]);
		m_OldPop[i].dblFitness=ObjFunc(m_OldPop[i].dblDistance);
		m_OldPop[i].nParentOne=0;
		m_OldPop[i].nParentTwo=0;
		m_OldPop[i].nXSite=0;
	}
	Statistics(m_OldPop);
	Individual *TempIndvd;
	TempIndvd=NULL;
	m_nGen=0;
	
	int nLoop=m_NumPop*15;
	do{		
		Generation();		
		Statistics(m_NewPop);	
		if(m_ClusterType==1){
			m_NewPop[m_nMinSite].btChrom = m_NewPop[m_nMaxSite].btChrom;
			m_NewPop[m_nMinSite].dblCenterCrd = m_NewPop[m_nMaxSite].dblCenterCrd;
			m_NewPop[m_nMinSite].dblFitness=m_NewPop[m_nMaxSite].dblFitness;
			m_NewPop[m_nMinSite].dblDistance=m_NewPop[m_nMaxSite].dblDistance;
			m_NewPop[m_nMinSite].nParentOne=m_NewPop[m_nMaxSite].nParentOne;
			m_NewPop[m_nMinSite].nParentTwo=m_NewPop[m_nMaxSite].nParentTwo;
			m_NewPop[m_nMinSite].nXSite=m_NewPop[m_nMaxSite].nXSite;
		}
		TempIndvd=m_OldPop;
		m_OldPop=m_NewPop;
		m_NewPop=TempIndvd;

		m_nGen++;
/*
		if(GetTickCount()-dwBegTimeTick>=m_dwTimeOut&&m_dwTimeOut>0)
			bTimeOut=true;
*/
	}while(!bTimeOut&&(m_nGen<m_nMaxGen));// &&(m_nLoop<nLoop));//&&(m_nAddLoop<nLoop));
}

//////////////////////////////////////////////////////////////////////////
//函数名称：HopGenCluster
//--Hopfield聚类主函数-----------------------------------------------// 
//////////////////////////////////////////////////////////////////////////
BestIndividual* CHopGen::HopGenCluster()
{
	int i,j,k;
	int NumLoopIn;
	bool   bFinish=false;	
	CTString strProgressText;
	
	double dblmax=0;
	unsigned char   maxsite=0;
	bool   bTimeOut=false;
	unsigned int  dwCurTimeTick=0;
//	unsigned int  dwBegTimeTick=GetTickCount();
/***Hopfield聚类******************************************************/

// 	if(m_ClusterType==0){
		NumLoopIn=0;
		InitState();
		CalDistance();

		double nInterval = (double)m_NumTrain/100; 
		int nStep = 0;
		do{			
			CalStates();
			CalDistance();
			bFinish=Recheck();

			if( NumLoopIn>=nStep*nInterval )
			{
/*
				strProgressText.Format("正在训练Hopfield拓扑结构... 完成 %d%%", int(NumLoopIn/nInterval));
				m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
				m_pProgressWnd->SendMessage( WM_PROGRESS_STEP, 0, 0 );
*/

				nStep++;
			}
			NumLoopIn++;
//			dwCurTimeTick=GetTickCount();
//			if((dwCurTimeTick-dwBegTimeTick)>=m_dwTimeOut&&m_dwTimeOut>0)
//				bTimeOut=true;

		}while(!bTimeOut&&!bFinish&&NumLoopIn<m_NumTrain&&!m_bAbort); 

		if( !m_bAbort )
		{
			for(j=0;j<m_NumPoints;j++){
				dblmax=0;
				maxsite=0;
				for(k=0;k<m_NumCluster;k++){
					if(m_dblVV[j*m_NumCluster+k]>dblmax){
						dblmax=m_dblVV[j*m_NumCluster+k];
						maxsite=(unsigned char)k;			    
					}
				}
				m_BestIndvd->btChrom[j]=maxsite;
			}
//			TRACE("\n");
			for(j=0;j<m_NumCluster;j++){
				for(i=0;i<m_NumDem;i++){
					m_BestIndvd->dblCenterCrd[j*m_NumDem+i]=m_dblCenterCrd[j*m_NumDem+i];
//					TRACE("%5.4f   ",m_dblCenterCrd[j*m_NumDem+i]);
				}
//				TRACE("\n");
			}
			m_BestIndvd->dblFitness=0;
			m_BestIndvd->nGeneration=0;
		}
		UnInitHopfield();
		AntiNormalizeData();
//	}
///***遗传算法聚类********************************************************************/
//	else if(m_ClusterType==1){
//		InitPop();
//		ClusterGA(0);
//		UnInitGenetic();
//	}
///****Hopfield + 遗传算法聚类********************************************************/
//	else{
//		double nInterval = (double)m_NumPop/100; 
//		int nStep = 0;
//		for(i=0;i<m_NumPop && !m_bAbort;i++){
//			NumLoopIn=0;
//			InitState();
//			CalDistance();
//			do{			
//				CalStates();
//				CalDistance();
//				bFinish=Recheck();
//				NumLoopIn++;
//			}while(!bFinish&&NumLoopIn<m_NumTrain&&!m_bAbort); 
//	
//			if( !m_bAbort )
//			{
//				//if(m_ClusterType==2){
//				CIntVector btChrom(m_LenChrom);
//				CIntVector btIndex(m_NumCluster);
//				for(j=0;j<m_NumPoints;j++){
//					dblmax=0;
//					maxsite=0;
//					for(k=0;k<m_NumCluster;k++){
//						if(m_dblVV[j*m_NumCluster+k]>dblmax){
//							dblmax=m_dblVV[j*m_NumCluster+k];
//							maxsite=(unsigned char)k;			    
//						}
//					}
//					btChrom[j]=maxsite;
//				}
//				/**********由于用Hopfield算法算出聚类中心位置次序是随机的，为了每次让聚类中心位置次序一致，需作如下调整***/
//				if(i==0){
//					m_dblOldCenterCrd = m_dblCenterCrd;
//					m_OldPop[i].btChrom = btChrom;				
//				}
//				else{
//					int n,minSite;
//					double minDistance,Distance;
//					double a,b;
//					for(n=0;n<m_NumCluster;n++){
//			///////////////////////////////////////////////////////////
//						Distance=0;
//						for(k=0;k<m_NumDem;k++){
//							a=m_dblCenterCrd[n*m_NumDem+k];
//							b=m_dblOldCenterCrd[0*m_NumDem+k];
//							Distance+=pow(m_dblCenterCrd[n*m_NumDem+k]-m_dblOldCenterCrd[0*m_NumDem+k],2);
//						}
//						minDistance=sqrt(Distance);
//						minSite=0;
//			///////////////////////////////////////////////////////////
//						for(j=1;j<m_NumCluster;j++){
//							Distance=0;
//							for(k=0;k<m_NumDem;k++){
//								Distance+=pow(m_dblCenterCrd[n*m_NumDem+k]-m_dblOldCenterCrd[j*m_NumDem+k],2);
//							}	
//							Distance=sqrt(Distance);
//							if(Distance<minDistance){
//								minDistance=Distance;
//								minSite=j;
//							}
//						}
//						btIndex[n]=minSite;
//			///////////////////////////////////////////////////////////
//					}
//					for(j=0;j<m_LenChrom;j++){
//						m_OldPop[i].btChrom[j]=btIndex[btChrom[j]];
//					}
//				}	
//			/*********************************************************************************************************/
///*
//				dwCurTimeTick=GetTickCount();
//				if((dwCurTimeTick-dwBegTimeTick)>=m_dwTimeOut&&m_dwTimeOut>0){
//					bTimeOut=true;	
//					break;
//				}
//*/
//
//				if( i>=nStep*nInterval )
//				{
///*
//					strProgressText.Format("正在训练拓扑结构... 完成 %d%%", int(i/nInterval));
//					m_pProgressWnd->SendMessage( WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
//					m_pProgressWnd->SendMessage( WM_PROGRESS_STEP, 0, 0 );
//*/
//
//					nStep++;
//				}
//			}
//
//		}
//		UnInitHopfield();
//		if(bTimeOut){
//			int NumPopTemp;
//			NumPopTemp=m_NumPop;
//			m_NumPop=i;
//			for(i=0;i<m_NumPop;i++){
//				Decode(m_OldPop[i]);
//				m_OldPop[i].dblFitness=ObjFunc(m_OldPop[i].dblDistance);
//				m_OldPop[i].nParentOne=0;
//				m_OldPop[i].nParentTwo=0;
//				m_OldPop[i].nXSite=0;
//			}
//			Statistics(m_OldPop);
//			m_NumPop=NumPopTemp;
//		}
//		else if(!m_bAbort){
//			ClusterGA(dwBegTimeTick);
//		}
//		UnInitGenetic();
//		AntiNormalizeData();
// 	}
/*****************************************************************************************************************/
//	if( m_bAbort )
//	{
//		delete m_BestIndvd;
//		m_BestIndvd = NULL;
//	}
//	else
		CalClusterRange();
	return m_BestIndvd;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：CalClusterRange
//函数功能：计算聚类的范围
//////////////////////////////////////////////////////////////////////////
void CHopGen::CalClusterRange()
{
	int i,j;
	for(i=0;i<m_NumCluster;i++){
		for(j=0;j<m_NumDem;j++){
			m_BestIndvd->dblRangeMax[i*m_NumDem+j]=-9.9999e30;
			m_BestIndvd->dblRangeMin[i*m_NumDem+j]=9.9999e30;
		}
	}

	CIntVector PointsInPerClass(m_NumCluster);
	for(i=0;i<m_NumPoints;i++){
		for(j=0;j<m_NumDem;j++){
			if(m_dblData[i*m_NumDem+j]>m_BestIndvd->dblRangeMax[m_BestIndvd->btChrom[i]*m_NumDem+j])
				m_BestIndvd->dblRangeMax[m_BestIndvd->btChrom[i]*m_NumDem+j]=m_dblData[i*m_NumDem+j];
			if(m_dblData[i*m_NumDem+j]<m_BestIndvd->dblRangeMin[m_BestIndvd->btChrom[i]*m_NumDem+j])
				m_BestIndvd->dblRangeMin[m_BestIndvd->btChrom[i]*m_NumDem+j]=m_dblData[i*m_NumDem+j];
		}
		PointsInPerClass[m_BestIndvd->btChrom[i]]++;
	}
	for(i=0;i<m_NumCluster;i++){
		if(PointsInPerClass[i]==0){
			for(j=0;j<m_NumDem;j++){
				m_BestIndvd->dblRangeMax[i*m_NumDem+j]=0;
				m_BestIndvd->dblRangeMin[i*m_NumDem+j]=0;
				m_BestIndvd->dblCenterCrd[i*m_NumDem+j]=0;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：InitState
//-初始化神经元状态--------------------------------------------------//
//////////////////////////////////////////////////////////////////////////
void CHopGen::InitState()
{
	int i,j,k;
	double dblAllDis;
	CDoubleVector dblTempDistance(m_NumCluster);
// 	srand((unsigned)time(NULL));
	for(i=0;i<m_NumPoints;i++){
		dblAllDis=9.9e20;
		for(j=0;j<m_NumCluster;j++){
			dblTempDistance[j]=0;
			for(k=0;k<m_NumDem;k++)
				dblTempDistance[j]+=pow(m_dblData[i*m_NumDem+k]-m_dblCenterCrd[j*m_NumDem+k],2);
		
			if(dblAllDis>dblTempDistance[j])
				dblAllDis=dblTempDistance[j];
		}
		for(j=0;j<m_NumCluster;j++){
			dblTempDistance[j]=dblTempDistance[j]-dblAllDis;
			m_dblVV[i*m_NumCluster+j]=0.8*exp(-10*dblTempDistance[j])+0.1*rand()/RAND_MAX;
			m_dblUU[i*m_NumCluster+j]=-0.02*log(1/m_dblVV[i*m_NumCluster+j]-1);
		}

	}
}


//////////////////////////////////////////////////////////////////////////
//函数名称：CalDistance
//函数功能：计算各点到各聚累中心的距离----------------------------------------//
//////////////////////////////////////////////////////////////////////////
void CHopGen::CalDistance()
{
	int i,j,k;
	double dblTempOne,dblTempTwo;
/***求个聚类中心坐标***********************************************/
	for(i=0;i<m_NumCluster;++i){
		for(j=0;j<m_NumDem;++j){
			dblTempOne=0;
			dblTempTwo=0;
			for(k=0;k<m_NumPoints;k++){
				dblTempOne+=m_dblData[k*m_NumDem+j]*m_dblVV[k*m_NumCluster+i];
				dblTempTwo+=m_dblVV[k*m_NumCluster+i];
			}
			m_dblCenterCrd[i*m_NumDem+j]=dblTempOne/dblTempTwo;
		}
	}
/******************************************************************/

/***计算各点到各聚累中心的距离*************************************/
	for(k=0;k<m_NumPoints;++k){
		for(i=0;i<m_NumCluster;++i){		
			dblTempOne=0;
			for(j=0;j<m_NumDem;++j){
				dblTempOne+=pow(m_dblData[k*m_NumDem+j]-m_dblCenterCrd[i*m_NumDem+j],2);
			}
			m_dblDistance[k*m_NumCluster+i]=sqrt(dblTempOne);
		}
	}
/*******************************************************************/
}

//////////////////////////////////////////////////////////////////////////
//函数名称：CalStates
//函数功能：计算神经元状态的叠代运算，即duxi/dt-------------------------------//
//////////////////////////////////////////////////////////////////////////
void CHopGen::CalStates()
{
	int i,j,x,x0,N;
	double dblK,dblTemp;
	double dblU0=0.02;

	dblK=0;
	for(i=0;i<m_NumPoints;i++)
		for(j=0;j<m_NumCluster;j++)
			dblK+=m_dblVV[i*m_NumCluster+j];
	dblK=(dblK-m_NumPoints)/m_NumPoints;
	dblK=2/(1+exp(-2*dblK))-1;
///////////////////////////////////////////////////
//dblK*=0.5;
///////////////////////////////////////////////////
	dblTemp=0;
	for(x=0;x<m_NumPoints;x++){
		x0=x*m_NumCluster;
		for(i=0;i<m_NumCluster;i++){
			dblTemp=-dblK;
			for(j=0;j<m_NumCluster;j++){
				dblTemp-=m_dblVV[x0+j];				
				if(i==j)  continue;				
				dblTemp-=m_dblVV[x0+j]*m_dblVV[x0+i];	
				dblTemp-=m_dblDistance[x0+i]*m_dblVV[x0+j];				
			}
			m_dblUU[x0+i]+=m_dblStep*(dblTemp+1);
		}
		for(i=0;i<m_NumCluster;i++){
			N=(int)(m_dblUU[x0+i]*71428.57+0.5)+7141;
			if(N<=0)
				m_dblVV[x0+i]=m_dblTempVV[0];
			else if(N>13999)
				m_dblVV[x0+i]=m_dblTempVV[13999];
			else
				m_dblVV[x0+i]=m_dblTempVV[N];
		}
	}

}


//////////////////////////////////////////////////////////////////////////
//函数名称：CalEnergy
//函数功能：计算神经网络能量-------- -----------------------------------------//
//////////////////////////////////////////////////////////////////////////
double CHopGen::CalEnergy()
{		
		int i,j,x,x0;
		double dblEnergy=0;
		double dblSumV=0;
		for(x=0;x<m_NumPoints;x++){
			x0=x*m_NumCluster;
			for(i=0;i<m_NumCluster;i++){
				dblSumV+=m_dblVV[x0+i];
				for(j=0;j<m_NumCluster;j++){
					if(i==j)  continue;
					dblEnergy+=m_dblVV[x0+i]*m_dblVV[x0+j];
				}
			}
		}
		dblEnergy=0;
		for(i=0;i<m_NumCluster;i++){			
			for(x=0;x<m_NumPoints;x++){
				dblEnergy+=m_dblDistance[x*m_NumCluster+i]*m_dblVV[x*m_NumCluster+i];//*m_dblVV[x*m_NumCluster+i];
			}
		}
	
		return dblEnergy;
}


//////////////////////////////////////////////////////////////////////////
//函数名称：Recheck
//函数功能：--检查网络是否处于稳定状态- ---------------------------------------//
//////////////////////////////////////////////////////////////////////////
bool CHopGen::Recheck()
{
	int i,j,nMax,nMin,x0;
	double dblTemp=0;

	//neuron state must access 0 or 1
	int NN=m_NumPoints*m_NumCluster;
	for(i=0;i<NN;i++){
		if((fabs(m_dblVV[i])>0.35)&&(fabs(m_dblVV[i])<0.65))
			return false;
	}
	//every row have and only have one 1
	for(i=0;i<m_NumPoints;i++){
		dblTemp=0.0;
		x0=i*m_NumCluster;
		nMax=0;
		nMin=0;
		for(j=0;j<m_NumCluster;j++){
			if(m_dblVV[x0+j]>0.65)
				nMax++;
			if(m_dblVV[x0+j]<0.35)
				nMin++;
		}
		if(nMax==1||nMin==m_NumCluster-1)
			continue;
		else{
//			for(x=0;x<m_NumCluster;x++){
//				TRACE("%5.3f  ",m_dblVV[i*m_NumCluster+x]);
//			}
//			TRACE("\n");
			return false;
		}
	}
 	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：InitPopsize
//函数功能：****初始化种群个数************************/
//////////////////////////////////////////////////////////////////////////
int CHopGen::InitPopsize()
{
	int NumPop=0;
	switch(m_ClusterType){
	case 0:
		NumPop=0;
		break;
	case 1:
		NumPop=(int)(100+10*log(m_NumPoints));
		break;
	case 2:
		NumPop=(int)(1000/(1+exp(-m_NumPoints/20000))-450);
		break;
	default:
		NumPop=0;
		break;
	}

	return NumPop;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：InitPop
//函数功能：***初始化群体**************************************/
//////////////////////////////////////////////////////////////////////////
void CHopGen::InitPop()
{
	int i,j;
	for(i=0;i<m_NumPop;i++){
		for(j=0;j<m_LenChrom;j++){
			m_OldPop[i].btChrom[j]=(unsigned char)(rand()%m_NumCluster);
		}
		m_OldPop[i].dblDistance=0;
		m_OldPop[i].dblFitness=0;
		m_OldPop[i].nParentOne=0;
		m_OldPop[i].nParentTwo=0;
		m_OldPop[i].nXSite=0;
//		::FillMemory(m_OldPop[i].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double),0);
		m_OldPop[i].dblCenterCrd.create(m_NumCluster*m_NumDem);
	}
	m_nLoop=0;
	m_nAddLoop=0;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：ObjFunc
//函数功能：******适应度计算*************************************************/
//////////////////////////////////////////////////////////////////////////
double CHopGen::ObjFunc(double x)
{
	double y;
	y=100.0/x;
	return y;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Statistics
//输入参数：Individual *Pop 个体
//函数功能：适应度统计**************************************************/
//////////////////////////////////////////////////////////////////////////
void CHopGen::Statistics(Individual *Pop)
{
	int j;
	double dblMax,dblMin;//,dblAvg;

	m_dblSumFitness=Pop[0].dblFitness;
	dblMin=Pop[0].dblFitness;
	dblMax=Pop[0].dblFitness;
	m_nMaxSite=0;
	m_nMinSite=0;
	for(j=1;j<m_NumPop;j++){
		m_dblSumFitness=m_dblSumFitness+Pop[j].dblFitness;
		if(Pop[j].dblFitness>dblMax){
			dblMax=Pop[j].dblFitness;
			m_nMaxSite=j;
		}
		if(Pop[j].dblFitness<m_dblMin){
			dblMin=Pop[j].dblFitness;
			m_nMinSite=j;
		}
	}	
	//dblAvg=m_dblSumFitness/m_NumPop;

	double dblAddRatio=0;
	if(dblMax>m_dblMax){		
		m_dblMax=dblMax;
		memcpy(m_BestIndvd->btChrom,Pop[m_nMaxSite].btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_BestIndvd->dblCenterCrd,Pop[m_nMaxSite].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_BestIndvd->dblDistance=Pop[m_nMaxSite].dblDistance;
		m_BestIndvd->dblFitness=Pop[m_nMaxSite].dblFitness;
		m_BestIndvd->nGeneration=m_nGen;
		m_nLoop=0;
	}
	else
		m_nLoop++;

}

//////////////////////////////////////////////////////////////////////////
//函数名称：Decode
//输入参数：Individual &Indvd 个体
//函数功能：计算各点到各聚累中心的距离-------------------------------------------
//////////////////////////////////////////////////////////////////////////
void CHopGen::Decode(Individual &Indvd)
{
	int i,j;
	CIntVector PointsCluster(m_NumCluster);     //聚为某一类的点的个数
/******求各聚类中心的坐标***********************************************/
	for(i=0;i<m_NumCluster;i++){
		//PointsCluster[i]=0;
		for(j=0;j<m_NumDem;j++){
			Indvd.dblCenterCrd[i*m_NumDem+j]=0;
		}
	}
	for(i=0;i<m_LenChrom;++i){
		PointsCluster[Indvd.btChrom[i]]++;
		for(j=0;j<m_NumDem;++j){
			Indvd.dblCenterCrd[Indvd.btChrom[i]*m_NumDem+j]+=m_dblData[i*m_NumDem+j];
		}
	}
	//求各聚类中心的坐标
	for(i=0;i<m_NumCluster;i++){
		for(j=0;j<m_NumDem;j++){
			Indvd.dblCenterCrd[i*m_NumDem+j]/=PointsCluster[i];
		}
	}
/***********************************************************************/


/******求各聚类点到聚类中心的距离之和***********************************/
	double dblTempOne=0;
	Indvd.dblDistance=0;
	for(i=0;i<m_NumPoints;++i){		
		dblTempOne=0;
		for(j=0;j<m_NumDem;++j){
			dblTempOne+=pow(m_dblData[i*m_NumDem+j]-Indvd.dblCenterCrd[Indvd.btChrom[i]*m_NumDem+j],2);
		}
		Indvd.dblDistance+=sqrt(dblTempOne);
	}
/***********************************************************************/
	delete []PointsCluster;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Select
//函数功能：轮盘赌选择******************************************************/
//////////////////////////////////////////////////////////////////////////
int CHopGen::Select()  
{
    double sum, pick;
    int i;
    pick = Randomizer();
    sum = 0;
    if(m_dblSumFitness != 0)
    {
        for(i = 0; (sum < pick) && (i < m_NumPop); i++)
            sum += m_OldPop[i].dblFitness/m_dblSumFitness;
		if(i>0)
			i=i-1;
		else
			i=0;
	}
    else{
        i = (int)(Randomizer()* m_NumPop);
	}

    return(i);
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Generation
//函数功能：群体更新*********************************************************/
//////////////////////////////////////////////////////////////////////////
void CHopGen::Generation()
{
	int mate1,mate2;
	int Ind=0;
	

	do{
		mate1=Select();
		do{
			mate2=Select();
		}while(mate2==mate1);
		Crossover(m_OldPop[mate1],m_OldPop[mate2],Ind);
	
		m_NewPop[Ind].nParentOne=mate1;
		m_NewPop[Ind].nParentTwo=mate2;

		m_NewPop[Ind+1].nParentOne=mate1;
		m_NewPop[Ind+1].nParentTwo=mate2;
	
		Ind=Ind+2;
	}while(Ind<m_NumPop);
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Crossover
//输入参数：Individual pParent1, 个体1
//			Individual pParent2, 个体2
//			int Number 交叉变异数
//函数功能：交叉与变异*************************************************/
void CHopGen::Crossover(Individual pParent1, Individual pParent2,int Number)
{
	int k,j,jm;
	int jcross;//,mutate;

	if(Flip(m_dblCrossRatio)){
		double dblFirstMax=0,dblSecondMax;
		int    nFirstMax=0,nSecondMax=0;

////////第一次交叉产生两个子个体//////////////////////////////////////////////
		jcross=Random(m_LenChrom-1);
		jm=Random(m_LenChrom-1);
		if(jm<jcross){
			k=jcross;
			jcross=jm;
			jm=k;
		}
		for(j=jcross;j<jm;j++){
			m_TempIndvd[0].btChrom[j]=pParent1.btChrom[j];
			m_TempIndvd[1].btChrom[j]=pParent2.btChrom[j];
		}
	
		for(j=0;j<jcross;j++){
			m_TempIndvd[0].btChrom[j]=pParent2.btChrom[j];
			m_TempIndvd[1].btChrom[j]=pParent1.btChrom[j];
		}
		for(j=jm;j<m_LenChrom;j++){
			m_TempIndvd[0].btChrom[j]=pParent2.btChrom[j];
			m_TempIndvd[1].btChrom[j]=pParent1.btChrom[j];
		}
		for(j=0;j<2;j++)
			m_TempIndvd[j].nXSite=jcross;
			//m_TempIndvd[j].nParentOne=
//////////////////////////////////////////////////////////////////


////////第二次交叉产生两个子个体////////////////////////////////////////////////
		jcross=Random(m_LenChrom-1);
		jm=Random(m_LenChrom-1);
		if(jm<jcross){
			k=jcross;
			jcross=jm;
			jm=k;
		}
		for(j=jcross;j<jm;j++){
			m_TempIndvd[2].btChrom[j]=pParent1.btChrom[j];
			m_TempIndvd[3].btChrom[j]=pParent2.btChrom[j];
		}
	
		for(j=0;j<jcross;j++){
			m_TempIndvd[2].btChrom[j]=pParent2.btChrom[j];
			m_TempIndvd[3].btChrom[j]=pParent1.btChrom[j];
		}
		for(j=jm;j<m_LenChrom;j++){
			m_TempIndvd[2].btChrom[j]=pParent2.btChrom[j];
			m_TempIndvd[3].btChrom[j]=pParent1.btChrom[j];
		}
		for(j=2;j<4;j++)
			m_TempIndvd[j].nXSite=jcross;
///////////////////////////////////////////////////////////////////////


////////对四个子个体进行适应度计算//////////////////////////////////////
		for(j=0;j<4;j++){
			Decode(m_TempIndvd[j]);
			m_TempIndvd[j].dblFitness=ObjFunc(m_TempIndvd[j].dblDistance);
		}
/////////////////////////////////////////////////////////////////////////

/////////复制两个父个体////////////////////////////////////////////////
		memcpy(m_TempIndvd[4].btChrom,pParent1.btChrom,m_LenChrom*sizeof(unsigned char));
		m_TempIndvd[4].dblDistance=pParent1.dblDistance;
		m_TempIndvd[4].dblFitness=pParent1.dblFitness;
		memcpy(m_TempIndvd[4].dblCenterCrd,pParent1.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[4].nXSite=pParent1.nXSite;
		
		memcpy(m_TempIndvd[5].btChrom,pParent2.btChrom,m_LenChrom*sizeof(unsigned char));
		m_TempIndvd[5].dblDistance=pParent2.dblDistance;
		m_TempIndvd[5].dblFitness=pParent2.dblFitness;
		memcpy(m_TempIndvd[5].dblCenterCrd,pParent2.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[5].nXSite=pParent2.nXSite;
/////////////////////////////////////////////////////////////////////////////////

////////从六个个体重找出适应度最大的两个个体作为下一代///////////////////////////
		dblFirstMax=0;
		dblSecondMax=0;
		k=0;
		for(j=0;j<6;j++){
			if(dblFirstMax<m_TempIndvd[j].dblFitness){
				dblSecondMax=dblFirstMax;
				dblFirstMax=m_TempIndvd[j].dblFitness;
				nSecondMax=nFirstMax;
				nFirstMax=j;
				k++;
			}
		}
		if(k<=1){
			for(j=1;j<6;j++){
				if(dblSecondMax<m_TempIndvd[j].dblFitness){
					dblSecondMax=m_TempIndvd[j].dblFitness;
					nSecondMax=j;
				}
			}
		}

		memcpy(m_NewPop[Number].btChrom,m_TempIndvd[nFirstMax].btChrom,m_LenChrom*sizeof(char));
		memcpy(m_NewPop[Number].dblCenterCrd,m_TempIndvd[nFirstMax].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_NewPop[Number].dblFitness=m_TempIndvd[nFirstMax].dblFitness;
		m_NewPop[Number].dblDistance=m_TempIndvd[nFirstMax].dblDistance;
		m_NewPop[Number].nXSite=m_TempIndvd[nFirstMax].nXSite;

		memcpy(m_NewPop[Number+1].btChrom,m_TempIndvd[nSecondMax].btChrom,m_LenChrom*sizeof(char));
		memcpy(m_NewPop[Number+1].dblCenterCrd,m_TempIndvd[nSecondMax].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_NewPop[Number+1].dblFitness=m_TempIndvd[nSecondMax].dblFitness;
		m_NewPop[Number+1].dblDistance=m_TempIndvd[nSecondMax].dblDistance;
		m_NewPop[Number+1].nXSite=m_TempIndvd[nSecondMax].nXSite;
/////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else{
		memcpy(m_TempIndvd[0].btChrom,pParent1.btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_TempIndvd[0].dblCenterCrd,pParent1.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[0].dblFitness=pParent1.dblFitness;
		m_TempIndvd[0].dblDistance=pParent1.dblDistance;
		m_TempIndvd[0].nXSite=pParent1.nXSite;

		memcpy(m_TempIndvd[1].btChrom,pParent1.btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_TempIndvd[1].dblCenterCrd,pParent1.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[1].dblFitness=pParent1.dblFitness;
		m_TempIndvd[1].dblDistance=pParent1.dblDistance;
		m_TempIndvd[1].nXSite=pParent1.nXSite;

		Mutation(Number);

		memcpy(m_TempIndvd[0].btChrom,pParent2.btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_TempIndvd[0].dblCenterCrd,pParent2.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[0].dblFitness=pParent2.dblFitness;
		m_TempIndvd[0].dblDistance=pParent2.dblDistance;
		m_TempIndvd[0].nXSite=pParent2.nXSite;

		memcpy(m_TempIndvd[1].btChrom,pParent2.btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_TempIndvd[1].dblCenterCrd,pParent2.dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_TempIndvd[1].dblFitness=pParent2.dblFitness;
		m_TempIndvd[1].dblDistance=pParent2.dblDistance;
		m_TempIndvd[1].nXSite=pParent2.nXSite;

		Mutation(Number+1);
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Mutation
//输入参数：int nNumber 变异的数量
//函数功能：变异*****************************************************/
//////////////////////////////////////////////////////////////////////////
void CHopGen::Mutation(int nNumber)
{
	unsigned char Newbyte=0;

	for(int j=0;j<m_LenChrom;j++){
		if(Flip(m_dblMutationRatio)){
			Newbyte =(unsigned char)(rand()%m_NumCluster);
			if(Newbyte ==m_TempIndvd[0].btChrom[j]){
				Newbyte =Newbyte +1;
				if(Newbyte >=m_NumCluster)
					Newbyte =Newbyte -m_NumCluster;
			}
			m_TempIndvd[1].btChrom[j]=Newbyte;
		}
	}
	Decode(m_TempIndvd[1]);
	m_TempIndvd[1].dblFitness=ObjFunc(m_TempIndvd[1].dblDistance);
	if(m_TempIndvd[1].dblFitness>m_TempIndvd[0].dblFitness){
		memcpy(m_NewPop[nNumber].btChrom,m_TempIndvd[1].btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_NewPop[nNumber].dblCenterCrd,m_TempIndvd[1].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_NewPop[nNumber].dblFitness=m_TempIndvd[1].dblFitness;
		m_NewPop[nNumber].nXSite=-1;
	}
	else{
		memcpy(m_NewPop[nNumber].btChrom,m_TempIndvd[0].btChrom,m_LenChrom*sizeof(unsigned char));
		memcpy(m_NewPop[nNumber].dblCenterCrd,m_TempIndvd[0].dblCenterCrd,m_NumCluster*m_NumDem*sizeof(double));
		m_NewPop[nNumber].dblFitness=m_TempIndvd[0].dblFitness;
		m_NewPop[nNumber].nXSite=-2;
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Randomizer
//函数功能：随机数发生器,产生[0,1]之间一个随机数 *********************/
//////////////////////////////////////////////////////////////////////////
double CHopGen::Randomizer() 
{
    m_nRand++;
    if(m_nRand >=m_NumPop)
    {
        m_nRand = 0;
        AdvanceRandom();
    }
    return((double) m_dblOldRand[m_nRand]);
}


//////////////////////////////////////////////////////////////////////////
//函数名称：AdvanceRandom
//函数功能：重新启动随机数发生器 *********************************************/
//////////////////////////////////////////////////////////////////////////
void CHopGen::AdvanceRandom()  
{
    int i;
    double new_RandomOne,new_RandomTwo;
	int nBlock=(int)(floor(m_NumPop/2));
	int nBias=0;
	for(i=0;i<nBlock;i++){
		new_RandomOne = m_dblOldRand[i] - m_dblOldRand[nBlock+i];
		new_RandomTwo = m_dblOldRand[i] + m_dblOldRand[nBlock+i];
		if(new_RandomOne < 0.0)
			new_RandomOne += 1.0;
		if(new_RandomTwo>1.0)
			new_RandomTwo -= 1.0;
		m_dblOldRand[i] = new_RandomOne;
		m_dblOldRand[nBlock+i]=new_RandomTwo;
		
	}
}


//////////////////////////////////////////////////////////////////////////
//函数名称：InitRandomize
//函数功能：设定随机数种子并初始化随机数发生器 *************************************/
void CHopGen::InitRandomize()  
{
//     srand((unsigned)time(NULL));   
    for(int j=0;j<m_NumPop;j++){
		m_dblOldRand[j]=((double)(rand()%m_NumPop))/m_NumPop;
    }
	m_nRand=0;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Flip
//输入参数：double probability 概率大小
//函数功能：贝努利试验*****************************************************/
int CHopGen::Flip(double probability)
{
	double ppp;
	ppp=Random(100)/100.0;
	if(ppp<=probability)
		return 1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//函数名称：Random
//参数输入：int nUpBound
//函数功能：产生随机数
//////////////////////////////////////////////////////////////////////////
int CHopGen::Random(int nUpBound)
{
	return (int)(rand()*nUpBound/RAND_MAX);
}

void CHopGen::UnInitHopfield()
{
}

//////////////////////////////////////////////////////////////////////////
//函数名称：UnInitGenetic
//函数功能：撤销遗产算法的空间
//////////////////////////////////////////////////////////////////////////
void CHopGen::UnInitGenetic()
{
	int i=0;
	if(m_OldPop!=NULL){
		delete []m_OldPop;
		m_OldPop=NULL;
	}
	if(m_NewPop!=NULL){
		delete []m_NewPop;
		m_NewPop=NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名称：InitClusterCenter
//输入参数：CDoubleVector &dblCenter  聚类中心
//函数功能：初始化聚类中心
//////////////////////////////////////////////////////////////////////////
void CHopGen::InitClusterCenter(CDoubleVector &dblCenter)
{
	int i,j,k,m;
	int Num,BegNum;
	CDoubleVector dblData;
	
	int NumPerClass=100;
	//如果样本量过大，进行采样///////////////////////////////////////
	Num=m_NumCluster*NumPerClass;	
	if(m_NumPoints>Num){
		double dblInterval=((double)m_NumPoints)/Num;
		BegNum=Random(NumPerClass);
		dblData.create(Num*m_NumDem);
		for(i=0;i<Num && !m_bAbort;i++){
			for(j=0;j<m_NumDem;j++){
				dblData[i*m_NumDem+j]=m_dblData[(int)(BegNum+i*dblInterval)*m_NumDem+j];
			}
		}
	}
	else{
		Num=m_NumPoints;
		dblData.create(Num*m_NumDem);
		for(i=0;i<Num && !m_bAbort;i++){
			for(j=0;j<m_NumDem;j++){
				dblData[i*m_NumDem+j]=m_dblData[i*m_NumDem+j];
			}
		}
	}
	////////////////////////////////////////////////////////////////////
	if( !m_bAbort )
	{
		double dblRadius=sqrt(1/((double)m_NumCluster))/2;
		double dblDis;//=new double[Num];
		double dblTemp,maxD;
		int nNull;
		for(i=0;i<m_NumCluster;i++){
			maxD=-1;
			for(j=0;j<Num && !m_bAbort;j++){
				dblDis=0;
				for(k=0;k<Num;k++){
					dblTemp=0;
					for(m=0;m<m_NumDem;m++){
						dblTemp+=pow(dblData[k*m_NumDem+m]-dblData[j*m_NumDem+m],2);
					}
					dblDis+=exp(-dblTemp/pow(dblRadius/2,2));
				}
				if(dblDis>maxD){
					maxD=dblDis;
					for(m=0;m<m_NumDem;m++){
						dblCenter[i*m_NumDem+m]=dblData[j*m_NumDem+m];
					}
				}

			}
			if( m_bAbort )
				break;

			if(i<m_NumCluster-1){	
				nNull=0;
				for(k=0;k<Num;k++){
					dblTemp=0;
					for(j=0;j<m_NumDem;j++)
						dblTemp+=pow(dblData[k*m_NumDem+j]-dblCenter[i*m_NumDem+j],2);
					dblTemp=sqrt(dblTemp);
					if(dblTemp>dblRadius && nNull>0){
						for(j=0;j<m_NumDem;j++)
							dblData[(k-nNull)*m_NumDem+j]=dblData[k*m_NumDem+j];
					}
					else{
						nNull++;
					}
				}
				Num=Num-nNull;
			}
		}
	}

}






















