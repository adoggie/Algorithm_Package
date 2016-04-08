// TGraphData.h: interface for the CTGraphData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TGRAPHDATA_H__37151931_094E_4C2C_942A_22D9986EF36A__INCLUDED_)
#define AFX_TGRAPHDATA_H__37151931_094E_4C2C_942A_22D9986EF36A__INCLUDED_
#include "../Public/TString.h"
#include "../Public/TCLFile.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma  warning(disable:4251)

typedef struct tagDataPointValVal
{
	double fXVal;
	double fYVal;
} TDataPointValVal;

typedef struct tagDataPointValStr
{
	double fXVal;
	CTString fYVal;
} TDataPointValStr;

typedef struct tagDataPointStrVal
{
	CTString	fXVal;
	double		fYVal;
}TDataPointStrVal;

typedef struct tagDataPointStrArr
{
	CTString strName;
	CTArray<double,double> arrYVal;
}TDataPointStrArr;


typedef struct tagDataPointForHighLow
{		
	CTStringArray YAxisArr;
	double	nYCount;
	CTString strX;
	CTArray<double,double> nWeight;
	CTArray<double,double> MultYArr;
}TDataPointForHighLow;

//add by xds 200605010-谱系图
typedef struct tagDataPointPedigree
{
	CTArray <double, double> arrXAxis;
	CTStringArray arrYAxis;
	CTArray <double, double> arrPoint;
}TDataPointPedigree;
//决策树
typedef struct tagDataPointDecTree
{
	CTArray<double, double> arrRectangle;//带边框的矩形，四个数据为一个矩形
	CTStringArray szArrRectangle;        //带边框矩形中的文字数据
	CTArray<double, double> arrRectFill; //不带边框的矩形，四个数据为一个矩形
	CTStringArray szArrRectFill;         //不带边框矩形中的文字数据
	CTArray<double, double> arrLinePoint;//两点一线
}TDataPointDecTree;
//枚举/////////////////////////////////////////////////////////////
enum EDataType
{
	ValVal=0,
	ValStr,
	StrVal,
	StrArr,
	ThrArr,
	FriArr
};


//////////////////////////////////////////////////////////////////////
//Class name:   CTGraphData
//Author:			蒋国清
//Function:		  数据类
//Describe:		  
//Date-Time:	2002-8-15	9:08	
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CTGraphData
{
public:
	CTGraphData();
	virtual ~CTGraphData();
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	virtual void Serialize(TCLFile &file);
	virtual void Print();
	void Clear();
	void SetData(void *pData, int nLen, EDataType eDataType,CTString strLengend="");

	EDataType  m_eDataType;
	CPtrArray  m_Data;
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TGRAPHDATA_H__37151931_094E_4C2C_942A_22D9986EF36A__INCLUDED_)
