#if !defined(_JAVA_READ_INCLUDE_)
#define _JAVA_READ_INCLUDE_

//公共用的
const char PROGNAME[]         = "/PROGNAME";       //算法名称
const char DATASOURCE[]       = "/DATASOURCE";     //数据源
const char VARINPUT[]         = "/VARINPUT";       //输入变量
const char VAROUTPUT[]        = "/DESTOUTPUT";     //目标变量
const char DATETIME[]         = "/DATETIME";       //生成时间

//算法名字
const char REGRESSIONLINEAR[]   = "REGRESSIONLINEAR";  //线性回归
const char REGRESSIONLOGISTIC[] = "REGRESSIONLOGISTIC";//二值逻辑
const char ART[]                = "ART";               //神经网络(ART,HP,KH)
const char HPFIELD[]            = "HOPFIELD";          //神经网络(ART,HP,KH)
const char KOHONEN[]            = "KOHONEN";           //神经网络(ART,HP,KH)
const char RBFNET[]             = "RBFNET";            //RBF
const char BPNET[]              = "BPNET";             //BP
const char DECTREE[]            = "DECTREE";           //决策树
const char SVM[]                = "SVM";               //支持向量机 

#endif