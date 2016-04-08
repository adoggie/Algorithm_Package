

/**************************************************************************/
/******************   �Ŵ��㷨����ͷ�ļ�Genetic.h   ***********************/
/******************   �����ߣ�  ������              ***********************/
/******************   �������ڣ�Oct.7 2002          ***********************/
/******************   �޸��ߣ�                      ***********************/
/**************************************************************************/    
#ifndef HOPGEN_H
#define HOPGEN_H 


#include "../cpknl/cpknl.h"

struct Individual						 /* ����*/
{
	CIntVector  btChrom;                     /* Ⱦɫ�� */
	double  dblDistance;                 /* �����Ӧ�ĸ��㵽�����������ĵľ���֮��*/
	double  dblFitness;                  /* ������Ӧ��*/
	int     nParentOne;			         /* ������1  */
	int     nParentTwo;			         /* ������2  */
	int     nXSite;				         /* ����λ�� */
	CDoubleVector dblCenterCrd;                /* ������������*/
};
//Art
//Koh
struct Kohonen
{
	
};
//Hop
struct BestIndividual                    /* ��Ѹ���*/
{ 
	CIntVector  btChrom;                     /* ��Ѹ���Ⱦɫ�� */
    double  dblFitness;                  /* ��Ѹ�����Ӧ�� */
    double  dblDistance;                 /* �����Ӧ�ĸ��㵽�����������ĵľ���֮�� */
    int     nGeneration;                 /* ��Ѹ������ɴ� */
	CDoubleVector dblCenterCrd;          /* ������������ */
	CDoubleVector dblRangeMax;           /* ���෶Χ�����ֵ */
	CDoubleVector dblRangeMin;           /* ���෶Χ����Сֵ */
	CDoubleVector dblRange;				 /* ԭʼ���������Сֵ */
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
		            int nMaxGen=0,double dblCrossRatio=0.45,double  dblMutationRatio=0.005);//��ʼ������Ա�����������ڴ�

	void InitClusterCenter(CDoubleVector &dblCenter);//��ʼ����������

	void InitHopfield();//��ʼ��Hopfield���磬��һ�����ݣ��Բ������ó�ֵ
	void UnInitHopfield();

	void InitGenetic(int nMaxGen,double dblCrossRatio,double  dblMutationRatio);
	void UnInitGenetic();
	BestIndividual* GeneticCluster();
	BestIndividual* HopGenCluster();
///////////////////////////////////////////////////////////////
// ��ӣ�QT
// ʱ�䣺2002.12.24
// ˵����m_bAbort ������ֹ����ı���
public:
	bool m_bAbort;
	void Abort() { m_bAbort=true; };
// QT
////////////////////////////////////////////////////////////////
protected:
//***�Ŵ��㷨���ຯ��************************************************************************************//
	//void   InitData();
	void   ClusterGA(unsigned int dwBegTimeTick);//�Ŵ��㷨���о���
	void   InitPop();//��ʼ��Ⱥ��
	void   Generation();//Ⱥ�����
	void   Statistics(Individual *Pop);//��Ӧ��ͳ��
	double ObjFunc(double x);//��Ӧ�ȼ���
	void   Decode(Individual &Indvd);//������㵽���������ĵľ���
	int    Select();//���̶�ѡ��
	void   Crossover(Individual pParent1, Individual pParent2,int Number);//���������
	void   Mutation(int nNumber);//����
	void   InitRandomize();//�趨��������Ӳ���ʼ�������������
	double Randomizer();//�����������,����[0,1]֮��һ�������
	void   AdvanceRandom();//�������������������
	int    Random(int nUpBound);//���������
	int    InitPopsize();//��ʼ����Ⱥ����
	int    Flip(double probability);//��Ŭ������
//*******************************************************************************************************//

//**Hopfield���ຯ��*************************************************************************************//
	void    InitState();					//��ʼ��״̬
	void    CalDistance();					//�������
	void    CalStates();					//����״̬
	double  CalEnergy();					//����������
	bool    Recheck();						//��������Ƿ����ȶ�״̬
	void    NormalizeData();				//��һ������
	void    AntiNormalizeData();			//�����һ������
//*******************************************************************************************************//
	void    CalClusterRange();				//�������ķ�Χ	
private:
	unsigned int   m_dwTimeOut;				//���е�ʱ��
//***���г�Ա����************************************************************************************//
	CDoubleVector m_dblData;					   /* �������ĸ��������*/
	int     m_NumPoints;                   /* �������ĵ���      */
	int     m_NumDem;                      /* ������ά��        */
	int     m_NumCluster;                  /* ����۳�m_NumClass��*/
	int     m_ClusterType;                 /* ���෽��,0ΪHopfield�������㷨,1Ϊ�Ŵ��㷨��2ΪHopfield+�Ŵ��㷨*/
	CDoubleVector m_dblOldCenterCrd;		//�������������
//***************************************************************************************************//

//***�Ŵ��㷨��Ա����********************************************************************************//
	int     m_nMaxGen;					   /* ��������� */
	int     m_NumPop;                      /* ��Ⱥ��С   */
	int     m_LenChrom;                    /* Ⱦɫ�峤�� */
	double  m_dblCrossRatio;               /* �������   */
	double  m_dblMutationRatio;            /* �������   */

	Individual *m_OldPop;				   /* ��ǰ����Ⱥ */
	Individual *m_NewPop;				   /* ��һ����Ⱥ */
	Individual m_TempIndvd[6];             /* ��ʱ����   */
	BestIndividual *m_BestIndvd;            /* ��Ѹ���   */

	double m_dblSumFitness;                /* ��Ⱥ�и�����Ӧ���ۼ� */
	double m_dblMax;                       /* ��Ⱥ�и��������Ӧ�� */
	double m_dblAvg;                       /* ��Ⱥ�и���ƽ����Ӧ�� */
	double m_dblMin;					   /* ��Ⱥ�и�����С��Ӧ�� */
	int    m_nMaxSite;
	int    m_nMinSite;
	
	int    m_nGen;                         /* ��ǰ������         */                        
	int    m_nMutation;                    /* ��ǰ�����췢������ */
	int    m_nCross;                       /* ��ǰ�����淢������ */

	int    m_nLoop;                        /* ��Ӧ�Ȳ�������ĵ�������      */
	int    m_nAddLoop;                     /* ��Ӧ��ÿ�����ӷ��Ⱥ�΢С�Ĵ���*/

	CDoubleVector m_dblOldRand;				   /* 0��1.0�������� */
	int     m_nRand;
//***************************************************************************************************//

//***Hopfield��Ա����********************************************************************************//
	CDoubleVector m_dblCenterCrd;                /* ������������              */
	CDoubleVector m_dblUU;                       /* ��Ԫ���(����ǰ)        */
	CDoubleVector m_dblVV;                       /* ��Ԫ���(�����)        */
	CDoubleVector m_dblDistance;                 /* ���㵽���������ĵľ���    */
	double m_dblStep;                      /* ΢�ַ��̲���              */
	int    m_NumTrain;                     /* ѵ������ */
	CDoubleVector m_dblRange;  //m_dblRange[0]��С��m_dblRange[1]���,����ԭʼ���ݹ�һ��ʱʹ��
	CDoubleVector m_dblTempVV;                   /* ������������*/
//***************************************************************************************************//
};
#endif




































