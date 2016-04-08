// Timer.cpp: implementation of the CTimer class.
//						计时器
//			功能： 输出程序运行所消耗的时间长度，精确到0.001秒
//                 可以在debug和release 状态下都能够运行
//          作者： 余卫军   天律公司统计算法小组
//			时间： 2005年11月2日
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Timer.h"
#include <time.h>

FILE* stream  = NULL;
CTimer::CTimer()
{
  m_dBeginTime = 0.0;
  m_strTitle =_T("");
  m_bFirst = true;
}

CTimer::~CTimer()
{
	char* timeline = ctime( & ( timebuffer_end.time ) );
	if(stream)
	{
		fprintf( stream," \n%20s \n",timeline );
		fprintf( stream," *******************测试结束******************\n\n",timeline );
		fclose(stream);
	}
}
//开始计时，strTitle为输出的标记。
void CTimer::Tic(CTString strTitle)
{
    _ftime( &timebuffer_begin);//获取起始时间
	if(m_bFirst)
	{
		m_dBeginTime = GetSeconds(timebuffer_begin);
		stream = fopen( m_strPath.GetData(), "w" );
		if(stream == NULL)
			return;
		char* timeline = ctime( & ( timebuffer_begin.time ) );
		
		fprintf( stream,strTitle + " \n时间测试 :%20s \n",timeline );
		fprintf( stream," \n*******************测试开始******************\n\n",timeline );
		
		m_bFirst =false;
	}

}
//计时结束

void CTimer::Toc(CTString strTitle)
{
    _ftime( &timebuffer_end);//获取结束时间
	double dElapsed_time = GetElapsedTime();//获取消耗时间；
  	if(stream == NULL)
		return;
	
	fprintf( stream, strTitle + " \nElapsed time :%.4f seconds\n\n",dElapsed_time );
	_ftime( &timebuffer_begin);//chon获取结束时间
	   
}
void CTimer::TotalTime()//输出累计总时间
{
    _ftime( &timebuffer_end);//获取结束时间

	double dEndTime = GetSeconds(timebuffer_end);
	double dElapsed_time = dEndTime - m_dBeginTime;
	if(stream == NULL)
		return;
	
	fprintf( stream, m_strTitle + " \nElapsed time :%.4f seconds\n\n",dElapsed_time );
	
}
void CTimer::SetPath(CTString strPathName)//设置存储路径,并且包括文件名称
{
	m_strPath = strPathName;
}

inline double CTimer::GetSeconds(struct _timeb& struct_t)
{
	double dbSeconds = struct_t.time + struct_t.millitm/1000.0;
	return dbSeconds;
}
inline  double CTimer::GetElapsedTime()
{
   double dTime1 = GetSeconds(timebuffer_begin);
   double dTime2 = GetSeconds(timebuffer_end);
   return dTime2 - dTime1;
}

