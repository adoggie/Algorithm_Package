// Timer.h: interface for the CTimer class.
//						计时器
//			功能： 输出程序运行所消耗的时间长度，精确到0.001秒
//                 可以在debug和release 状态下都能够运行
//          作者： 余卫军   天律公司统计算法小组
//			时间： 2005年11月2日

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
#define AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#ifdef _UNIX
	#define _timeb	timeb
	#define _ftime	ftime
#else
#include <process.h>
#endif 

class CTimer  
{
public:
	CTimer();
	virtual ~CTimer();
	//开始计时，strTitle为输出的标记。
	void Tic(CTString strTitle = "");
	void Toc(CTString strTitle = "");//结束时间，为临时的计时， 单位为秒
	void TotalTime();//输出累计总时间
    
private:
   bool     m_bFirst;      //是否第一次
   double   m_dBeginTime;  //开始时间
   CTString  m_strTitle;      //标记名称 

   struct _timeb timebuffer_begin;//起始时间
   struct _timeb timebuffer_end;  //终止时间
   
private:
   double GetSeconds(struct _timeb& str_t);
   double GetElapsedTime();
};

#endif // !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
