//////////////////////////////////////////////////////////////////////////
//			初级统计算法调用分配，CallCrnt返回0表示成功，1表示失败
//          
//            作者：马良庄   2006年3月17日  
//////////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Define.h"
#include "../DataInterface/TCDataArray.h"
#include "../DataInterface/DataInterface.h"
#include "../DataInterface/Define.h"
#include "../Public/common.h"
#include "../Public/System.h"
#include "../Public/TString.h"
#include "../Public/AnaWord.h"
#include "../Public/AnaSyntax.h"
#include "../Result/Result.h"    
#include "../cpknl/cpknl.h"

#include "StdAfx.h"
#include "AAnova.h"
#include "BinomialTest.h"
#include "ChiSquare.h"
#include "CKTest.h"
#include "Correlation.h"
#include "crosstab.h"
#include "DesLayerConf.h"
#include "OneSmplTTest.h"
#include "PairTTest.h"
#include "PartialCorre.h"
#include "NonParamTest.h"
#include "RunTest.h"
#include "TTest.h"
#include "Statistics.h"
