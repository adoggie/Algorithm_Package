========================================================================
       DYNAMIC LINK LIBRARY : TLADVN
========================================================================
功能说明：
   在高级统计模块中，我们的算法包争取提供各种高级统计方和和复杂的统计模型的算法实现功能。
因此，在今后的一段时间里，这些算法将不断的得到优化和完善，同时也提供新的算法文件出来。
   下面将在这个文件中列出各个算法的修改和有待改进的地方。
//////////////////////////////////////////////////////////////////////////
//  高级统计部分
//////////////////////////////////////////////////////////////////////////
//线性回归分析测试
bool	TestLinearRegression();
//logistic 回归 
bool	TestLogisticReg();
//概率单位回归
bool	TestProbReg();
//有序回归
bool	TestOrdinalReg();
//曲线回归
bool	TestCurvrReg();
//判别分析 
bool TestDiscriminant();

//主成分分析 
bool	TestPrimaryComponent();

//因子分析 
bool    TestFactors();

//生存分析 
bool    TestSurviveTable();//寿命表
bool    TestKMProcess();//KM过程 
bool	TestRatioRisk();//比例风险模型

//聚类分析 
bool	TestClusterfy();//分层聚类

bool	TestQuickCluster();//快速聚类
//多因素方差分析 
bool	TestMultiAnova();

//时间序列分析 
bool	TestX_11();//X_11季节调整
//季节解构 
bool    TestSeasonalAdjust();
bool	TestMA();//MA模型
bool	TestAR();//AR模型
bool	TestArima();//ARIMA模型 
bool	TestExpMa();//指数平滑模型

