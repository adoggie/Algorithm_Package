// BPArithmetic.cpp: implementation of the CBPArithmetic class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BPArithmetic.h"
//调整开始
/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
const unsigned int WM_PROGRESS_TEXT = RegisterWindowMessage("WM_PROGRESS_TEXT");
const unsigned int WM_PROGRESS_STEP = RegisterWindowMessage("WM_PROGRESS_STEP");
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBPArithmetic::CBPArithmetic()
{
        m_bAbort = false;
}

CBPArithmetic::~CBPArithmetic()
{

}



/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->        第一次前向计算初始化                                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::LMForwardCalculateInit( int nInputLayerNumber,
                                                                                                        int nHideLayerNumber,
                                                                                                        int nOutputLayerNumber,
                                                                                                        CBPMatrix &m_mxDemoDataInput,
                                                                                                        CBPMatrix &m_mxInput,
                                                                                                        CBPMatrix &m_mxIToHWeight,
                                                                                                        CBPMatrix &m_mxHideThreshold,
                                                                                                        CBPMatrix &m_mxHToOWeight,
                                                                                                        CBPMatrix &m_mxOutThreshold
                                                                                                   )
{

        /************************************************************************
        *                                        --------->Use Matlab Method <---------              *
        ************************************************************************/

                
        /////////////////////////////////////////////////////////////////////////
        // 构造输入层元素的矩阵
        // 构造规则:
        //                1. 样本数目做为矩阵的行数;
        //                2. 单个样本的输入层的数目做为矩阵的列数;
        //                3. 矩阵中的元素即为对应的输入层的值
        //
        {//===yh  in:m_mxDemoDataInput, nInputLayerNumber     out: m_mxInput
                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);
                // 得到样本的输入值
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput,(unsigned int)0,(unsigned int)0);
                
                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();
                
                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);
        }
        //TRACE("row %d,col:%d\r\n",m_mxInput.GetMatrixRowNumber(),m_mxInput.GetMatrixColNumber());


        /////////////////////////////////////////////////////////////////////////
        // 构造权值矩阵 -----> 由单个样本输入层与隐含层之间的权值做为元素组成
        // 构造规则:
        //                1. 单个样本的输入层的数目做为矩阵行数;
        //                2. 单个样本的隐含层的数目做为矩阵的列数;
        //                3. 对矩阵中的元素进行随机初始化,值在(-1,1)之间;
        //                4. 所有样本的输入层和隐含层的数目是相等的;
        //                5. 所有样本使用的是同一个权值矩阵.
        //
        {//===yh        out  m_mxIToHWeight  
                CBPMatrix        cm_mxIToHWeight(nHideLayerNumber, nInputLayerNumber);
                
                // 随机初始化矩阵内元素的值
                cm_mxIToHWeight.RandomInitialize ();
                m_mxIToHWeight.CopyMatrix (cm_mxIToHWeight);
        }


        /////////////////////////////////////////////////////////////////////
        //        构造样本隐含层的阀值矩阵
        //        构造规则:
        //                1. 样本的数目做为矩阵行数;
        //                2. 单个样本的隐含层的数目做为矩阵的列数;
        //                3. 随机初始化矩阵中的元素,使值在(-1,1)之间;
        //                4. 矩阵中每行的数据都和第一行数据相对应的位置相等.
        //
        {
                CBPMatrix cm_mxHideThreshold(nHideLayerNumber,(unsigned int)1);
                
                // 随机初始化矩阵内元素的值
                cm_mxHideThreshold.RandomInitialize ();
                m_mxHideThreshold.CopyMatrix(cm_mxHideThreshold);
        }


        /////////////////////////////////////////////////////////////////////
        //        构造权值矩阵        ----->        由单个样本的隐含层与输出层之间权值做为元素
        //                                                        组成
        //        构造规则:
        //                1. 单个样本的隐含层的数目做为矩阵的行数;
        //                2. 单个样本的输出层的数目做为矩阵的列数;
        //                3. 对矩阵中的元素进行随机初始化,值在(-1,1)之间;
        //                4. 所有样本的隐含层和输出层的数目是相等的;
        //                5. 所有样本使用的是同一个权值矩阵.
        //

        CBPMatrix cm_mxHToOWeight(nOutputLayerNumber, nHideLayerNumber);

        // 对矩阵的元素随机初始化
        cm_mxHToOWeight.RandomInitialize ();

        m_mxHToOWeight.CopyMatrix (cm_mxHToOWeight);


        /////////////////////////////////////////////////////////////////////
        //        构造样本的输出层的阀值矩阵
        //        构造规则:
        //                1. 样本的数目做为矩阵的行数;
        //                2. 单个样本的输出层的数目做为矩阵的列数;
        //                3. 随机初始化矩阵中的元素,使值在(-1,1)之间;
        //                4. 矩阵中每行的数据都和第一行数据相对应的位置相等.
        //

        CBPMatrix cMatrixOutputLayerValveValue(nOutputLayerNumber,(unsigned int)1);

        // 随机初始化矩阵内元素的值
        cMatrixOutputLayerValveValue.RandomInitialize ();

        m_mxOutThreshold.CopyMatrix(cMatrixOutputLayerValveValue);


}

/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->                前向计算                                                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::LMForwardCalculate ( int nInputLayerNumber,
                                                                                                int nHideLayerNumber,
                                                                                                int nOutputLayerNumber,
                                                                                                bool bSimulateDataFlag,
                                                                                                int nComboFunc,
                                                                                                CBPMatrix &m_mxDemoDataInput,
                                                                                                CBPMatrix &m_mxInput,
                                                                                                CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold
                                                                                           )
{

        if(bSimulateDataFlag)
        {

                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // 得到样本的输入值
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // 得到所有样本的隐含层的净输入
        // 构造规则:
        //                1. 样本的数目做为矩阵行数;
        //                2. 单个样本的隐含层的数目做为矩阵的列数;
        //                3. 矩阵元素中的值即为对应的样本的隐含层的净输入:
        //                   由 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        得到.
        //
        CBPMatrix cmxHPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
        //row *col 4* 321
        {
                CBPMatrix mxHideThresholdEx;
                mxHideThresholdEx.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
                cmxHPureInput = m_mxIToHWeight * m_mxInput;
                //4*321          4*8             8*321
                
                cmxHPureInput += mxHideThresholdEx;
                
        }

        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的隐含层的输出
        //        构造规则:
        //                1. 隐含层的输出y与隐含层的输入x的关系可用函数表示
        //                        y = f(x)
        //                2. 矩阵的维数和隐含层的净输入矩阵的维数相等
        //

        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cmxHPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cmxHPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cmxHPureInput.Tansig();
                break;

        default:
                return;
        }
        
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);
        m_mxHide.Print("m_mxHide");
        // 4* 321 
        /////////////////////////////////////////////////////////////////////
        //        得到所有样本输出层的净输入
        //        构造规则;
        //                1. 样本的数目做为矩阵的行数;
        //                2. 单个样本的输出层的数目做为矩阵的列数;
        //                3. 矩阵中元素的值即为对应样本的输出层的净输入:
        //                        由
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        得到
        //

        CBPMatrix cMatrixExOutputLayerValveValue;
        cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
        //1 * 321 
        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
        // 1 * 321
        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        // 1 * 321                    1 * 4              4*321
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的输出层的输出
        //        构造规则:
        //                1. 矩阵的维数与得到的所有样本的输出层的净输入组成的矩阵一样;
        //                2. 输出层的输出y和输出层的输入可用关系式
        //                        y = f(x)
        //                表示
        //

        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);
m_mxOut.Print("m_mxOut");

        //1 * 321

}


/////////////////////////////////////////////////////////////////////////////
//        Levenberg-Marquart                ---->                反馈计算                                                   //
/////////////////////////////////////////////////////////////////////////////
bool CBPArithmetic::LMDemoDataTrainRepeat (        int nInputLayerNumber,
                                                                                                        int nHideLayerNumber,
                                                                                                        int nOutputLayerNumber,
                                                                                                        bool bSimulateDataFlag,
                                                                                                        int nComboFunc,
                                                                                                        double nSystemErrorOld,
                                                                                                        double nSystemErrorNew,
                                                                                                        double nSystemErrorLevel,
                                                                                                        //double nSystemError,//无用 ===temp
                                                                                                        double nStep,
                                                                                                        unsigned int nMaxTrainTimes,
                                                                                                        //unsigned int nTrainTimes,//无用 ===temp
                                                                                                        //DWORD ID_SYSTEM_ERROR,
                                                                                                        //DWORD ID_TRAIN_TIMES,
                                                                                                        CBPMatrix &m_mxDemoDataInput,
                                                                                                        CBPMatrix &m_mxInput,
                                                                                                        CBPMatrix &m_mxIToHWeight,
                                                                                                        CBPMatrix &m_mxHideThreshold,
                                                                                                        CBPMatrix &m_mxHide,
                                                                                                        CBPMatrix &m_mxHToOWeight,
                                                                                                        CBPMatrix &m_mxOut,
                                                                                                        CBPMatrix &m_mxOutThreshold
                                                                                                 )
{

        LMForwardCalculate (nInputLayerNumber,
                                                nHideLayerNumber,
                                                nOutputLayerNumber,
                                                bSimulateDataFlag,
                                                nComboFunc,
                                                m_mxDemoDataInput,
                                                m_mxInput,
                                                m_mxIToHWeight,
                                                m_mxHideThreshold,
                                                m_mxHide,
                                                m_mxHToOWeight,
                                                m_mxOut,
                                                m_mxOutThreshold
                                                );
                

        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的输出层的delta矩阵
        //        构造规则:
        //                1. 样本的数目为矩阵的行数;
        //                2. 样本输出层的数目为矩阵的列数;
        //                3. 矩阵中的元素的值y为:
        //                                y = -(前向计算出的输出层的值 - 样本的输出层的值) * f'(net)
        //


        CBPMatrix cMatrixTDemoOutput(m_mxDemoDataInput.GetMatrixRowNumber (), nOutputLayerNumber);
                
        // 得到样本中输出层的数据
        m_mxDemoDataInput.CopySubMatrix (cMatrixTDemoOutput, (unsigned int)nInputLayerNumber, (unsigned int)0);

        CBPMatrix cMatrixDemoOutput = cMatrixTDemoOutput.Transpose ();

        // 得到样本中输出层的误差
        CBPMatrix cMatrixOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerError = cMatrixDemoOutput - m_mxOut;

                
        nSystemErrorOld = cMatrixOutputLayerError.GetSystemError ();

        for(int nLoopTimes=1; nLoopTimes < nMaxTrainTimes; nLoopTimes++)        
        {
                if(nSystemErrorOld < nSystemErrorLevel)
                {
                        nLoopTimes--;
                        break;
                }

                CBPMatrix        cMatrixExHideLayerOutput;
                cMatrixExHideLayerOutput.nncpyi (m_mxHide, nOutputLayerNumber);


                CBPMatrix        cMatrixOutputLayerDelta (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber());
                
                // 注意: 此处'/' 是 '点乘'!!!
                cMatrixOutputLayerDelta = 1 - m_mxOut / m_mxOut;

                CBPMatrix cMatrixExOutputLayerDelta;
                cMatrixExOutputLayerDelta.nncpyd (cMatrixOutputLayerDelta);
                
                cMatrixExOutputLayerDelta = cMatrixExOutputLayerDelta * (-1.0);

        
                CBPMatrix cMatrixTHideToOutputWeightValue (m_mxHToOWeight.GetMatrixColNumber(), m_mxHToOWeight.GetMatrixRowNumber());
                cMatrixTHideToOutputWeightValue = m_mxHToOWeight.Transpose();

                CBPMatrix cMatrixExHideLayerDelta;
                // 注意: 此处'/' 是 '点乘'!!!
                cMatrixExHideLayerDelta.CopyMatrix ( (1 - (cMatrixExHideLayerOutput / cMatrixExHideLayerOutput)) / ( cMatrixTHideToOutputWeightValue * cMatrixExOutputLayerDelta) );
                                

                CBPMatrix        cMatrixExInputLayerValue;
                cMatrixExInputLayerValue.nncpyi (m_mxInput, nOutputLayerNumber);

                
                CBPMatrix        cMatrixJ11;
                cMatrixJ11.nncpy (cMatrixExHideLayerDelta.Transpose(), cMatrixExInputLayerValue.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ12;
                cMatrixJ12.nncpyi(cMatrixExInputLayerValue.Transpose (), cMatrixExHideLayerDelta.GetMatrixRowNumber());


                CBPMatrix        cMatrixJ1;
                // 注意: 此处'/' 是 '点乘'!!!
                cMatrixJ1.CopyMatrix (cMatrixJ11 / cMatrixJ12);


                CBPMatrix cMatrixJ21;
                cMatrixJ21.nncpy (cMatrixExOutputLayerDelta.Transpose (), cMatrixExHideLayerOutput.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ22;
                cMatrixJ22.nncpyi (cMatrixExHideLayerOutput.Transpose (), cMatrixExOutputLayerDelta.GetMatrixRowNumber ());

                CBPMatrix cMatrixJ2;
                // 注意: 此处'/' 是 '点乘'!!!
                cMatrixJ2.CopyMatrix (cMatrixJ21 / cMatrixJ22);

                
                CBPMatrix cMatrixZ;
                cMatrixZ.CopyMatrix (MergeMatrix(MergeMatrix(MergeMatrix(cMatrixJ1, cMatrixExHideLayerDelta.Transpose ()), cMatrixJ2), cMatrixExOutputLayerDelta.Transpose ()));


                CBPMatrix cMatrixMOutputLayerError;
                cMatrixMOutputLayerError.CopyMatrix ( cMatrixOutputLayerError.MergeColumnsToColumnVector () );


                CBPMatrix        cMatrixJE;
                cMatrixJE.CopyMatrix ( (cMatrixZ.Transpose ()) * cMatrixMOutputLayerError );


                CBPMatrix cMatrixJJ;
                cMatrixJJ.CopyMatrix ( (cMatrixZ.Transpose ()) * cMatrixZ );
                

                // 定义新的输入层到隐含层的权值
                CBPMatrix cMatrixNewInputToHideWeight;
                // 定义的新的隐含层的阀值
                CBPMatrix cMatrixNewHideLayerValve;
                // 定义新的隐含层到输出层的权值
                CBPMatrix cMatrixNewHideToOutputWeight;
                // 定义新的输出层的阀值
                CBPMatrix cMatrixNewOutputLayerValve;

                // 定义新的误差矩阵
                CBPMatrix cMatrixNewOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

                /////////////////////////////////////////////////////////////////
                // the weight value is adjusted

                while(nStep <= MAX_ADJUST_VALUE)
                {
                        CBPMatrix        cMatrixI (cMatrixZ.GetMatrixColNumber (), cMatrixZ.GetMatrixColNumber ());

                        cMatrixI.Eye ();


                        CBPMatrix cMatrixDX;
                        cMatrixDX.CopyMatrix ( (((cMatrixJJ + cMatrixI * nStep).Inverse ()) * cMatrixJE) * (-1.0) );
                                

                        /////////////////////////////////////////////////////////////////////////
                        // 拆分cMatrixDX矩阵

                        unsigned int nIndex = 0;
                                
                        // 得到输入层到隐含层的权值的修正量
                        CBPMatrix cMatrixInputToHideWeightChange(nHideLayerNumber, nInputLayerNumber);

                        cMatrixDX.CopySubMatrixFromVector (cMatrixInputToHideWeightChange, nIndex);

                        nIndex += nHideLayerNumber * nInputLayerNumber;

                        // 得到隐含层阀值的修正量
                        CBPMatrix cMatrixHideLayerValveChange (nHideLayerNumber, (unsigned int)1);
                                
                        cMatrixDX.CopySubMatrixFromVector (cMatrixHideLayerValveChange, nIndex);


                        nIndex += nHideLayerNumber;

                        // 得到隐含层到输出层的权值的修正量
                        CBPMatrix cMatrixHideToOutputWeightChange (nOutputLayerNumber, nHideLayerNumber);
                                
                        cMatrixDX.CopySubMatrixFromVector (cMatrixHideToOutputWeightChange, nIndex);

                        nIndex += nOutputLayerNumber * nHideLayerNumber;

                        // 得到输出层阀值的修正值
                        CBPMatrix cMatrixOutputValveChange (nOutputLayerNumber, (unsigned int)1);

                        cMatrixDX.CopySubMatrixFromVector (cMatrixOutputValveChange, nIndex);
        
                        cMatrixNewInputToHideWeight.CopyMatrix (m_mxIToHWeight + cMatrixInputToHideWeightChange);

                        cMatrixNewHideLayerValve.CopyMatrix (m_mxHideThreshold + cMatrixHideLayerValveChange);

                        cMatrixNewHideToOutputWeight.CopyMatrix (m_mxHToOWeight + cMatrixHideToOutputWeightChange);

                        cMatrixNewOutputLayerValve.CopyMatrix (m_mxOutThreshold + cMatrixOutputValveChange);
                
                        // 前向计算
                        LMForwardCalculate (nInputLayerNumber,
                                                                nHideLayerNumber,
                                                                nOutputLayerNumber,
                                                                bSimulateDataFlag,
                                                                nComboFunc,
                                                                m_mxDemoDataInput,
                                                                m_mxInput,
                                                                cMatrixNewInputToHideWeight,
                                                                cMatrixNewHideLayerValve,
                                                                m_mxHide,
                                                                cMatrixNewHideToOutputWeight,
                                                                m_mxOut,
                                                                cMatrixNewOutputLayerValve
                                                                );

                                
                        cMatrixNewOutputLayerError = cMatrixDemoOutput - m_mxOut;
                        nSystemErrorNew =         cMatrixNewOutputLayerError.GetSystemError ();

                        if(nSystemErrorNew < nSystemErrorOld)
                        {
                                break;
                        }
                        else
                        {
                                nStep *= 10;
                        }
                                
                }// End for while loop

                if ( nStep > MAX_ADJUST_VALUE)
                {
                        nLoopTimes--;
                        return false;
                }

                nStep        *= 0.1;

                // 赋值
                m_mxIToHWeight = cMatrixNewInputToHideWeight;
                m_mxHideThreshold = cMatrixNewHideLayerValve;
                m_mxHToOWeight = cMatrixNewHideToOutputWeight;
                m_mxOutThreshold = cMatrixNewOutputLayerValve;
                cMatrixOutputLayerError = cMatrixNewOutputLayerError;
                nSystemErrorOld = nSystemErrorNew;

                if (m_bAbort)
                {
                        break;
                }
                // 显示数据和程序运行状态



                // 显示系统误差
                CTString        strSystemError;
                strSystemError.Format ("%.16lf", nSystemErrorOld);
                // 显示训练次数
                CTString strTrainTimes;
                strTrainTimes.Format ("%u", nLoopTimes + 1);
                CTString strProgressText =  "次数:"+strTrainTimes + " 训练精度"+strSystemError;
//                 SendMessage( m_hWnd,WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
   
        }// End the "for" loop
        
        return true;
}

/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                前向计算(Only for Training)                   //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::BPForwardCalculate ( int nInputLayerNumber,
                                                                                                int nHideLayerNumber,
                                                                                                int nOutputLayerNumber,
                                                                                                bool bSimulateDataFlag,
                                                                                                int nComboFunc,
                                                                                                CBPMatrix &m_mxDemoDataInput,
                                                                                                CBPMatrix &m_mxInput,
                                                                                                CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold,
                                                                                                CBPMatrix &cMatrixExHideLayerValveValue,
                                                                                                CBPMatrix &cMatrixExOutputLayerValveValue
                                                                                           )
{

        if(bSimulateDataFlag)
        {

                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // 得到样本的输入值
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // 得到所有样本的隐含层的净输入
        // 构造规则:
        //                1. 样本的数目做为矩阵行数;
        //                2. 单个样本的隐含层的数目做为矩阵的列数;
        //                3. 矩阵元素中的值即为对应的样本的隐含层的净输入:
        //                   由 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        得到.
        //
                
        //CBPMatrix cMatrixExHideLayerValveValue;
        //cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());

        CBPMatrix cMatrixHideLayerPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixHideLayerPureInput = m_mxIToHWeight * m_mxInput;

        cMatrixHideLayerPureInput += cMatrixExHideLayerValveValue;

        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的隐含层的输出
        //        构造规则:
        //                1. 隐含层的输出y与隐含层的输入x的关系可用函数表示
        //                        y = f(x)
        //                2. 矩阵的维数和隐含层的净输入矩阵的维数相等
        //

        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Tansig();
                break;

        default:
                return;
        }
        
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);

        /////////////////////////////////////////////////////////////////////
        //        得到所有样本输出层的净输入
        //        构造规则;
        //                1. 样本的数目做为矩阵的行数;
        //                2. 单个样本的输出层的数目做为矩阵的列数;
        //                3. 矩阵中元素的值即为对应样本的输出层的净输入:
        //                        由
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        得到
        //

        //CBPMatrix cMatrixExOutputLayerValveValue;
        //cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());

        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的输出层的输出
        //        构造规则:
        //                1. 矩阵的维数与得到的所有样本的输出层的净输入组成的矩阵一样;
        //                2. 输出层的输出y和输出层的输入可用关系式
        //                        y = f(x)
        //                表示
        //

        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);

}

/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                前向计算(Only for Simulating)           //
/////////////////////////////////////////////////////////////////////////////
void CBPArithmetic::BPForwardCalculate2( int nInputLayerNumber,
                                         int nHideLayerNumber,
                                         int nOutputLayerNumber,
                                         bool bSimulateDataFlag,
                                         int nComboFunc,
                                         CBPMatrix &m_mxDemoDataInput,
                                         CBPMatrix &m_mxInput,
                                         CBPMatrix &m_mxIToHWeight,
                                                                                                CBPMatrix &m_mxHideThreshold,
                                                                                                CBPMatrix &m_mxHide,
                                                                                                CBPMatrix &m_mxHToOWeight,
                                                                                                CBPMatrix &m_mxOut,
                                                                                                CBPMatrix &m_mxOutThreshold
                                                                                           )
{

        if(bSimulateDataFlag)
        {
                //5*150
                CBPMatrix cm_mxInput(m_mxDemoDataInput.GetMatrixRowNumber (), nInputLayerNumber);

                // 得到样本的输入值
                m_mxDemoDataInput.CopySubMatrix (cm_mxInput, (unsigned int)0, (unsigned int)0);

                CBPMatrix cMatrixTInputLayerValue = cm_mxInput.Transpose ();

                m_mxInput.CopyMatrix (cMatrixTInputLayerValue);


        }

        /////////////////////////////////////////////////////////////////////////
        // 得到所有样本的隐含层的净输入
        // 构造规则:
        //                1. 样本的数目做为矩阵行数;
        //                2. 单个样本的隐含层的数目做为矩阵的列数;
        //                3. 矩阵元素中的值即为对应的样本的隐含层的净输入:
        //                   由 
        //                                cm_mxInput * cm_mxIToHWeight 
        //                                + cm_mxHideThreshold
        //                        得到.
        //
                
        CBPMatrix cMatrixExHideLayerValveValue;//4*5
        cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
        //4*5
        CBPMatrix cMatrixHideLayerPureInput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixHideLayerPureInput = m_mxIToHWeight * m_mxInput;

        cMatrixHideLayerPureInput += cMatrixExHideLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的隐含层的输出
        //        构造规则:
        //                1. 隐含层的输出y与隐含层的输入x的关系可用函数表示
        //                        y = f(x)
        //                2. 矩阵的维数和隐含层的净输入矩阵的维数相等
        //
//4*5
        CBPMatrix cMatrixHideLayerOutput(nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixHideLayerOutput = cMatrixHideLayerPureInput.Tansig();
                break;

        default:
                return;
        }
        //4*5
        m_mxHide.CopyMatrix(cMatrixHideLayerOutput);

        /////////////////////////////////////////////////////////////////////
        //        得到所有样本输出层的净输入
        //        构造规则;
        //                1. 样本的数目做为矩阵的行数;
        //                2. 单个样本的输出层的数目做为矩阵的列数;
        //                3. 矩阵中元素的值即为对应样本的输出层的净输入:
        //                        由
        //                                cMatrixHideLayerOutput * cm_mxHToOWeight
        //                                + cMatrixOutputLayerValveValue
        //                        得到
        //
//7*5
        CBPMatrix cMatrixExOutputLayerValveValue;
        cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
//7*5
        CBPMatrix cMatrixOutputLayerPureInput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        cMatrixOutputLayerPureInput = m_mxHToOWeight * cMatrixHideLayerOutput;
        cMatrixOutputLayerPureInput  += cMatrixExOutputLayerValveValue;


        /////////////////////////////////////////////////////////////////////
        //        算出所有样本的输出层的输出
        //        构造规则:
        //                1. 矩阵的维数与得到的所有样本的输出层的净输入组成的矩阵一样;
        //                2. 输出层的输出y和输出层的输入可用关系式
        //                        y = f(x)
        //                表示
        //
//7*5
        CBPMatrix cMatrixOutputLayerOutput(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());

        switch(nComboFunc)
        {
        case 0:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Sigmoid ();
                break;

        case 1:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.tanh ();
                break;

        case 2:
                cMatrixOutputLayerOutput = cMatrixOutputLayerPureInput.Tansig ();
                break;

        default:
                return ;
        }

        m_mxOut.CopyMatrix(cMatrixOutputLayerOutput);

}



/////////////////////////////////////////////////////////////////////////////
//        Back propagation                ---->                反馈计算                                                   //
/////////////////////////////////////////////////////////////////////////////
bool CBPArithmetic::BPDemoDataTrainRepeat (        int nInputLayerNumber,
                        int nHideLayerNumber,           int nOutputLayerNumber,
                        bool bSimulateDataFlag,         int nComboFunc,
                        double &nSystemErrorOld,                 double nSystemErrorNew,
                        double nSystemErrorLevel,                //double nSystemError,//无用 ===temp
                        double nStep,                               unsigned int nMaxTrainTimes,
                        CBPMatrix &m_mxDemoDataInput,                CBPMatrix &m_mxInput,
                        CBPMatrix &m_mxIToHWeight,            CBPMatrix &m_mxHideThreshold,
                        CBPMatrix &m_mxHide,                                CBPMatrix &m_mxHToOWeight,
                        CBPMatrix &m_mxOut,                                CBPMatrix &m_mxOutThreshold )
{   
	// 根据BP算法修正nStep的初始值
	//nStep = 0.1;===temp
	
	const double Rate_inc=0.03;
	const double Study_inc=1.05;
	const double Study_dec=0.7;
	double inc;
	double alpha=0.5;
	double alphaold=alpha;
	
	
	// 前向计算
	LMForwardCalculate (nInputLayerNumber,        nHideLayerNumber,nOutputLayerNumber,
		bSimulateDataFlag,  nComboFunc,             m_mxDemoDataInput,
		m_mxInput,          m_mxIToHWeight,  m_mxHideThreshold,
		m_mxHide,  m_mxHToOWeight,  m_mxOut,  m_mxOutThreshold        );
	
	/////////////////////////////////////////////////////////////////////
	//        算出所有样本的输出层的delta矩阵
	//        构造规则:
	//                1. 样本的数目为矩阵的行数;
	//                2. 样本输出层的数目为矩阵的列数;
	//                3. 矩阵中的元素的值y为:
	//                                y = (前向计算出的输出层的值 - 样本的输出层的值) .* f'(net)
	//
	CBPMatrix cMatrixTDemoOutput(m_mxDemoDataInput.GetMatrixRowNumber (), nOutputLayerNumber);
	
	// 得到样本中输出层的数据
	m_mxDemoDataInput.CopySubMatrix (cMatrixTDemoOutput, (unsigned int)nInputLayerNumber, (unsigned int)0);
	
	CBPMatrix cMatrixDemoOutput = cMatrixTDemoOutput.Transpose ();
	
	// 得到样本中输出层的误差
	CBPMatrix cMatrixOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	cMatrixOutputLayerError = cMatrixDemoOutput - m_mxOut;
	
	nSystemErrorOld = cMatrixOutputLayerError.GetSystemError ()/(2*m_mxDemoDataInput.GetMatrixRowNumber ());
	
	//1*321 
	CBPMatrix        cMatrixOutputLayerDelta (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber());
	//4*1
	CBPMatrix cMatrixTHideToOutputWeightValue (m_mxHToOWeight.GetMatrixColNumber(), m_mxHToOWeight.GetMatrixRowNumber());
	// 定义新的输入层到隐含层的权值 4*8
	CBPMatrix cMatrixNewInputToHideWeight (m_mxIToHWeight.GetMatrixRowNumber (), m_mxIToHWeight.GetMatrixColNumber ());
	// 定义的新的隐含层的阀值 4*321
	CBPMatrix cMatrixNewHideLayerValve (nHideLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	// 定义新的隐含层到输出层的权值 1*4
	CBPMatrix cMatrixNewHideToOutputWeight (m_mxHToOWeight.GetMatrixRowNumber (), m_mxHToOWeight.GetMatrixColNumber ());
	// 定义新的输出层的阀值 1*321
	CBPMatrix cMatrixNewOutputLayerValve (nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	// 定义新的误差矩阵 1*321
	CBPMatrix cMatrixNewOutputLayerError(nOutputLayerNumber, m_mxDemoDataInput.GetMatrixRowNumber ());
	
	
	int nLoopTimes;
	for(nLoopTimes=1; nLoopTimes < nMaxTrainTimes; nLoopTimes++)        
	{
        
		if(nSystemErrorOld <1.01* nSystemErrorLevel)
		{
			nLoopTimes--;
			break;
		}
		
		// 求输出层的delta值
		// 注意: 此处'/' 是 '点乘'!!!
		cMatrixOutputLayerDelta = (m_mxOut - m_mxOut / m_mxOut) / cMatrixOutputLayerError;
        
		cMatrixTHideToOutputWeightValue = m_mxHToOWeight.Transpose();// 权重具体值
		
		// 求隐含层的delta值
		// 注意: 此处'/' 是 '点乘'!!!
		CBPMatrix cMatrixHideLayerDelta;
		cMatrixHideLayerDelta.CopyMatrix ( (m_mxHide - (m_mxHide / m_mxHide)) / ( cMatrixTHideToOutputWeightValue * cMatrixOutputLayerDelta) );

		// 权值和阀值变化量
		cMatrixNewHideToOutputWeight =cMatrixNewHideToOutputWeight*(alpha)+cMatrixOutputLayerDelta * (m_mxHide.Transpose ()) * (nStep);
		cMatrixNewOutputLayerValve = cMatrixOutputLayerDelta;
		
		cMatrixNewInputToHideWeight =cMatrixNewInputToHideWeight*(alpha) +cMatrixHideLayerDelta * (m_mxInput.Transpose ()) * (nStep);
		cMatrixNewHideLayerValve = cMatrixHideLayerDelta;
		
		CBPMatrix m_mxIToHWeighttemp;
		m_mxIToHWeighttemp.CopyMatrix(m_mxIToHWeight);
		
		CBPMatrix m_mxHToOWeighttemp;
		m_mxHToOWeighttemp.CopyMatrix(m_mxHToOWeight);
		
		// 赋值
		
		m_mxIToHWeight +=cMatrixNewInputToHideWeight;
		CBPMatrix cMatrixExHideLayerValveValue;
		cMatrixExHideLayerValveValue.nncpyi (m_mxHideThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
		cMatrixExHideLayerValveValue += cMatrixNewHideLayerValve;//隐含层阈值赋新值
		
        m_mxHToOWeight += cMatrixNewHideToOutputWeight;
		CBPMatrix cMatrixExOutputLayerValveValue;
		cMatrixExOutputLayerValveValue.nncpyi (m_mxOutThreshold, m_mxDemoDataInput.GetMatrixRowNumber ());
		cMatrixExOutputLayerValveValue += cMatrixNewOutputLayerValve;//输出层阈值赋新值
        
		// 前向计算（求出新的预测输出值m_mxOut）
		
		BPForwardCalculate (nInputLayerNumber,nHideLayerNumber,nOutputLayerNumber,
			bSimulateDataFlag,nComboFunc,m_mxDemoDataInput,m_mxInput,
			m_mxIToHWeight,m_mxHideThreshold,m_mxHide,m_mxHToOWeight,
			m_mxOut,m_mxOutThreshold,cMatrixExHideLayerValveValue,
			cMatrixExOutputLayerValveValue );
		
		cMatrixNewOutputLayerError = cMatrixDemoOutput - m_mxOut;
		nSystemErrorNew =         cMatrixNewOutputLayerError.GetSystemError ()/(2* m_mxDemoDataInput.GetMatrixRowNumber ());
		
		
		
		inc=(nSystemErrorNew - nSystemErrorOld)/nSystemErrorOld;
		if(inc>Rate_inc) 
		{
			m_mxIToHWeight.CopyMatrix(m_mxIToHWeighttemp); 
			m_mxHToOWeight.CopyMatrix(m_mxHToOWeighttemp);
			alpha=0;
			nStep*=Study_dec;
		}
		else if ((inc>=0)&&(inc<=Rate_inc))
		{
			nStep*=Study_inc;
			if(alpha==0)
			{
				alpha=alphaold;
			}
			cMatrixOutputLayerError = cMatrixNewOutputLayerError;
			nSystemErrorOld = nSystemErrorNew;        
		}
		else
		{
			if(alpha==0)
			{
				alpha=alphaold;
			}
			if(fabs(inc)<0.0001)
			{
				nStep*=2*Study_inc;
			}
			else
			{
				nStep*=Study_inc;
			}
			
			cMatrixOutputLayerError = cMatrixNewOutputLayerError;
			nSystemErrorOld = nSystemErrorNew;
		}
		
		if (m_bAbort)
			break;
		//*
		// 显示系统误差
		CTString        strSystemError;
		strSystemError.Format ("%.10lf", nSystemErrorOld);
		// 显示训练次数
		CTString strTrainTimes;
		strTrainTimes.Format ("%u", nLoopTimes + 1);
		CTString strProgressText =  "次数:"+strTrainTimes + "  误差:"+strSystemError;
// 		SendMessage( m_hWnd,WM_PROGRESS_TEXT, 0, (LPARAM)&strProgressText );
		//*/
	}// End the "for" loop
    if(nLoopTimes >= nMaxTrainTimes)
    {
//		CTString        strSystemError;
//		strSystemError.Format ("当前系统精度为%.10lf 没有收敛到设定系统精度%.10lf", nSystemErrorOld,nSystemErrorLevel);
//		if (AfxMessageBox(strSystemError+"\r\n是否接受目前结果还是请重新调整参数\r\n选择'是'为接受,选择'否'为放弃？",MB_YESNO) !=IDYES)
//		{
			return true;//test  ?????
// 		}
    }
        
	//***************************************************************************
	//***************************************************************************
	//***************************************************************************
	
     return true;
}

void CBPArithmetic::Abort()
{
        m_bAbort = true;
}

void CBPArithmetic::Reset()
{
        m_bAbort = false;
}