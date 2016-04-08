

/**************************************************************************/
/******************   遗传算法程序头文件Genetic.h   ***********************/
/******************   创建者：  刘堂友              ***********************/
/******************   创建日期：Oct.7 2002          ***********************/
/******************   修改者：                      ***********************/
/**************************************************************************/    
#ifndef HOPGEN_H
#define HOPGEN_H 


#include "../cpknl/cpknl.h"

struct Individual						 /* 个体*/
{
	CIntVector  btChrom;                     /* 染色体 */
	double  dblDistance;                 /* 个体对应的各点到各个聚类中心的距离之和*/
	double  dblFitness;                  /* 个体适应度*/
	int     nParentOne;			         /* 父个体1  */
	int     nParentTwo;			         /* 父个体2  */
	int     nXSite;				         /* 交叉位置 */
	CDoubleVector dblCenterCrd;                /* 聚类中心坐标*/
};
//Art
//Koh
struct Kohonen
{
	
};
//Hop
struct BestIndividual                    /* 最佳个体*/
{ 
	CIntVector  btChrom;                     /* 最佳个体染色体 */
    double  dblFitness;                  /* 最佳个体适应度 */
    double  dblDistance;                 /* 个体对应的各点到各个聚类中心的距离之和 */
    int     nGeneration;                 /* 最佳个体生成代 */
	CDoubleVector dblCenterCrd;          /* 聚类中心坐标 */
	CDoubleVector dblRangeMax;           /* 聚类范围的最大值 */
	CDoubleVector dblRangeMin;           /* 聚类范围的最小值 */
	CDoubleVector dblRange;				 /* 原始数据最大最小值 */
};


class CHopGen//:public CObject
{
public:
	CHopGen();
	CHopGen(CDoubleVector dblData,int NumPoints,int NumDem,int NumCluster,int ClusterType,unsigned int dwTimeOut=0,
		    int nMaxGen=0,double dblCrossRatio=0.45,double  dblMutationRatio=0.005);
		
	~CHopGen();
public:
	void Initialize(CDoubleVector dblData, int NumPoints,int NumDem,int NumCluster,int ClusterType,unsigned int  dwTimeOut,
		            int nMaxGen=0,double dblCrossRatio=0.45,double  dblMutationRatio=0.005);//初始化各成员变量及分配内存

	void InitClusterCenter(CDoubleVector &dblCenter);//初始化聚类中心

	void InitHopfield();//初始化Hopfield网络，归一化数据，对参数设置初值
	void UnInitHopfield();

	void InitGenetic(int nMaxGen,double dblCrossRatio,double  dblMutationRatio);
	void UnInitGenetic();
	BestIndividual* GeneticCluster();
	BestIndividual* HopGenCluster();
///////////////////////////////////////////////////////////////
// 添加：QT
// 时间：2002.12.24
// 说明：m_bAbort 用于终止运算的变量
public:
	bool m_bAbort;
	void Abort() { m_bAbort=true; };
// QT
////////////////////////////////////////////////////////////////
protected:
//***遗传算法聚类函数************************************************************************************//
	//void   InitData();
	void   ClusterGA(unsigned int dwBegTimeTick);//遗传算法进行聚类
	void   InitPop();//初始化群体
	void   Generation();//群体更新
	void   Statistics(Individual *Pop);//适应度统计
	double ObjFunc(double x);//适应度计算
	void   Decode(Individual &Indvd);//计算各点到各聚累中心的距离
	int    Select();//轮盘赌选择
	void   Crossover(Individual pParent1, Individual pParent2,int Number);//交叉与变异
	void   Mutation(int nNumber);//变异
	void   InitRandomize();//设定随机数种子并初始化随机数发生器
	double Randomizer();//随机数发生器,产生[0,1]之间一个随机数
	void   AdvanceRandom();//重新启动随机数发生器
	int    Random(int nUpBound);//产生随机数
	int    InitPopsize();//初始化种群个数
	int    Flip(double probability);//贝努利试验
//*******************************************************************************************************//

//**Hopfield聚类函数*************************************************************************************//
	void    InitState();					//初始化状态
	void    CalDistance();					//计算距离
	void    CalStates();					//计算状态
	double  CalEnergy();					//神经网络能量
	bool    Recheck();						//检查网络是否处于稳定状态
	void    NormalizeData();				//归一化数据
	void    AntiNormalizeData();			//反向归一化数据
//*******************************************************************************************************//
	void    CalClusterRange();				//计算聚类的范围	
private:
	unsigned int   m_dwTimeOut;				//运行的时间
//***共有成员变量************************************************************************************//
	CDoubleVector m_dblData;					   /* 参与聚类的各点的坐标*/
	int     m_NumPoints;                   /* 参与聚类的点数      */
	int     m_NumDem;                      /* 聚类点的维数        */
	int     m_NumCluster;                  /* 各点聚成m_NumClass类*/
	int     m_ClusterType;                 /* 聚类方法,0为Hopfield神经网络算法,1为遗传算法，2为Hopfield+遗传算法*/
	CDoubleVector m_dblOldCenterCrd;		//聚类的中心坐标
//***************************************************************************************************//

//***遗传算法成员变量********************************************************************************//
	int     m_nMaxGen;					   /* 最大世代数 */
	int     m_NumPop;                      /* 种群大小   */
	int     m_LenChrom;                    /* 染色体长度 */
	double  m_dblCrossRatio;               /* 交叉概率   */
	double  m_dblMutationRatio;            /* 变异概率   */

	Individual *m_OldPop;				   /* 当前代种群 */
	Individual *m_NewPop;				   /* 新一代种群 */
	Individual m_TempIndvd[6];             /* 另时个体   */
	BestIndividual *m_BestIndvd;            /* 最佳个体   */

	double m_dblSumFitness;                /* 种群中个体适应度累计 */
	double m_dblMax;                       /* 种群中个体最大适应度 */
	double m_dblAvg;                       /* 种群中个体平均适应度 */
	double m_dblMin;					   /* 种群中个体最小适应度 */
	int    m_nMaxSite;
	int    m_nMinSite;
	
	int    m_nGen;                         /* 当前世代数         */                        
	int    m_nMutation;                    /* 当前代变异发生次数 */
	int    m_nCross;                       /* 当前代交叉发生次数 */

	int    m_nLoop;                        /* 适应度不再增大的迭代次数      */
	int    m_nAddLoop;                     /* 适应度每次增加幅度很微小的次数*/

	CDoubleVector m_dblOldRand;				   /* 0～1.0间的随机数 */
	int     m_nRand;
//***************************************************************************************************//

//***Hopfield成员变量********************************************************************************//
	CDoubleVector m_dblCenterCrd;                /* 聚类中心坐标              */
	CDoubleVector m_dblUU;                       /* 神经元输出(激活前)        */
	CDoubleVector m_dblVV;                       /* 神经元输出(激活后)        */
	CDoubleVector m_dblDistance;                 /* 各点到各聚类中心的距离    */
	double m_dblStep;                      /* 微分方程步长              */
	int    m_NumTrain;                     /* 训练次数 */
	CDoubleVector m_dblRange;  //m_dblRange[0]最小，m_dblRange[1]最大,用于原始数据归一化时使用
	CDoubleVector m_dblTempVV;                   /* 激活函数查表数据*/
//***************************************************************************************************//
};
#endif




































