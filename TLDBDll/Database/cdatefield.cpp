// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cdatefield.h"
//#include <afxdisp.h>

CDateField::CDateField(CFields* pOwner)
{
	m_pFields = pOwner;
	m_pDefaultValue =(LPBYTE) new FieldNUM;//ytli Modify 2002/08/30

	if(m_pDefaultValue)	
		*(FieldNUM*)m_pDefaultValue = NULLValue;	
	m_pTempData = new FieldNUM;
	*((FieldNUM*)m_pTempData) = NULLValue;
	
	m_byteMeasure=msScale;
}

CDateField::~CDateField()
{
	delete (FieldNUM *)m_pDefaultValue;
	delete (FieldNUM *)m_pTempData;
}
//##ModelId=3D09872001EB
CFieldType CDateField::GetFieldType()
{
	return fDate;
}



//##ModelId=3D0987870257
int CDateField::GetDataLength(LPBYTE lpData)
{	
	return 8;
}

//##ModelId=3D098790034A
int CDateField::GetTypeSize()
{	
	return 9;
}
double CDateField::GetAsDouble()
{
/*===time  	COleDateTime OleTime( *(FILETIME *)m_pValue );
	return OleTime.m_dt;   */
	return 0;
}
CTString CDateField::GetAsString()
{
	CTString strRetValue;
	if(*(FieldNUM*)m_pValue != NULLValue)
	{
/*===time  		m_tm = COleDateTime(*(FILETIME *)m_pValue );
		strRetValue = m_tm.Format(GetDateType( m_byteEnumData ));   */
	}
	else
	{
		strRetValue = "";	
	}
	return strRetValue;
}

bool CDateField::GetAsBool()
{
/*===time  	FILETIME ft = *(FILETIME *)m_pValue;
	COleDateTime OleTime(ft);

	if( (OleTime.m_dt-1.0/3)<DoubleZero )//因为中国的时区为+8,8/24即1.0/3
		return false;
	else
		return true;   */
	return true;
}



int CDateField::GetAsInteger()
{	

/*===time  	FILETIME ft = *(FILETIME *)m_pValue;
	COleDateTime OleTime(ft);

	return (int)OleTime.m_dt;   */
	return 0;
}


FieldCurrency CDateField::GetAsCurrency()
{	
/*===time  	FILETIME ft = *(FILETIME *)m_pValue;
	COleDateTime OleTime(ft);

	FieldCurrency b;
	b       = OleTime.m_dt;

	return b;   */
	return 0;
}

FILETIME CDateField::GetAsDateTime()
{
	return *(FILETIME *)m_pValue;
}

bool CDateField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{		
		CField::SetAsBool(Value);
	}
	else 
	{
/*===time  		COleDateTime OleTime( (double)Value );		
		SYSTEMTIME st;	
		FILETIME p;
		
		memset(&st,0,sizeof(st));
		st.wDay          = OleTime.GetDay();
		st.wHour         = OleTime.GetHour();
		st.wMinute       = OleTime.GetMinute();
		st.wMonth        = OleTime.GetMonth();
		st.wSecond       = OleTime.GetSecond();
		st.wYear         = OleTime.GetYear();
		
		SystemTimeToFileTime(&st, &p);

		SetAsDateTime( &p );

		*(FILETIME *)m_pValue = p;   */
	
	}

	return true;
}

bool CDateField::SetAsDouble(double Value)
{
	if(!m_pValue)
	{		
		CField::SetAsDouble(Value);
	}
	else 
	{	
/*===time  		COleDateTime OleTime( Value );
		
		SYSTEMTIME st;	
		FILETIME   p;
		
		memset(&st,0,sizeof(st));
		st.wDay          = OleTime.GetDay();
		st.wHour         = OleTime.GetHour();
		st.wMinute       = OleTime.GetMinute();
		st.wMonth        = OleTime.GetMonth();
		st.wSecond       = OleTime.GetSecond();
		st.wYear         = OleTime.GetYear();
		
		SystemTimeToFileTime(&st, &p);

		*(FILETIME *)m_pValue   = p;   */
	}

	return true;
}

bool CDateField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{		
		CField::SetAsInteger(Value);
	}
	else 
	{
/*===time  		COleDateTime OleTime( (double)Value );
		
		SYSTEMTIME st;	
		FILETIME   p;
		
		memset(&st,0,sizeof(st));
		st.wDay          = OleTime.GetDay();
		st.wHour         = OleTime.GetHour();
		st.wMinute       = OleTime.GetMinute();
		st.wMonth        = OleTime.GetMonth();
		st.wSecond       = OleTime.GetSecond();
		st.wYear         = OleTime.GetYear();
		
		SystemTimeToFileTime(&st, &p);

		*(FILETIME *)m_pValue   = p;   */
	}

	return true;
}

bool CDateField::SetAsCurrency(FieldCurrency Value)
{
	if(!m_pValue)
	{		
		CField::SetAsCurrency(Value);
	}
	else 
	{
		//if( Value.Indicate==fvNULL )
		//	return false;
/*===time  		COleDateTime OleTime( Value);
		
		SYSTEMTIME st;	
		FILETIME   p;
		
		memset(&st,0,sizeof(st));
		st.wDay          = OleTime.GetDay();
		st.wHour         = OleTime.GetHour();
		st.wMinute       = OleTime.GetMinute();
		st.wMonth        = OleTime.GetMonth();
		st.wSecond       = OleTime.GetSecond();
		st.wYear         = OleTime.GetYear();
		
		SystemTimeToFileTime(&st, &p);

		*(FILETIME *)m_pValue   = p;   */
	}

	return true;
}

bool CDateField::SetAsString(CTString Value)
{	
	if(!m_pValue)
	{
		CField::SetAsString(Value);
	}
	else 
	{
/*===time  		Value.MakeLower(); //转化为小写.
		
		SYSTEMTIME st;
		if( !StringToSysTime( Value, st, m_byteEnumData) )
		{
			*(FieldNUM*)m_pValue = NULLValue;
			return false;
		}
		else
		{
			if(!SystemTimeToFileTime(&st, (FILETIME *)m_pValue))
				return false;
		}	   */
	}

	return true;	
}

bool CDateField::SetAsDateTime(FILETIME* Value)
{
	if(!m_pValue)
	{
		CField::SetAsDateTime(Value);
	}
	else 
	{		
		*(FILETIME *)m_pValue = *Value;
	}

	return true;
}






//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
bool CDateField::GetData(void*pDestArrar,int nIndex)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = (*(PF_NUM)pDestArrar)[nIndex];
	return true;
}
bool CDateField::GetData(void* lpData)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = *(FieldNUM*)lpData;
	return true;
}

bool CDateField::StringToSysTime( CTString str, SYSTEMTIME& st, DateType byteType )
{
	if(byteType>DateDisplayMax && byteType<DateDisplayMin)
		return false;
/*===time
	CTString strMonth = "janfebmaraprmayjunjulaugsepoctnovdec";
	str.MakeLower();
	ZeroMemory( &st, sizeof(SYSTEMTIME) );
	CStringArray strArrayPart;
	int i = 0;
	int iPos = 0;
	int iCnt = 0;
	int iLength = str.GetLength();
	while( iPos+iCnt < iLength )
	{
		if(str[iPos+iCnt] == '/' || str[iPos+iCnt] == 0x20 ||
		   str[iPos+iCnt] == ':' || str[iPos+iCnt] == '.'  ||
		   str[iPos+iCnt] == '-' || str[iPos+iCnt] == '_'    )
		{		
			if(iCnt!=0)
				strArrayPart.Add( str.Mid(iPos, iCnt) );
			iPos += iCnt+1;
			iCnt = 0;
		}
		else
		{
			iCnt++;
		}
	}
	if(iCnt!=0)
		strArrayPart.Add( str.Mid(iPos, iCnt) );

	int nSize = strArrayPart.GetSize();

	switch( byteType )
	{
	case ddmmmyyyy_line://dd-mmm-yyyy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			int nMonth = strMonth.Find(strArrayPart[1]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[2] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case ddmmmyy_line://dd-mmm-yy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			int nMonth = strMonth.Find(strArrayPart[1]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[2] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case mmddyyyy_bias://mm/dd/yyyy	
		{
			if( nSize<3 )
				return false;

			st.wMonth = atoi( strArrayPart[0] );
			st.wDay   = atoi( strArrayPart[1] );
			st.wYear  = atoi( strArrayPart[2] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
		
	case mmddyy_bias://mm/dd/yy
		{
			if( nSize<3 )
				return false;

			st.wMonth = atoi( strArrayPart[0] );
			st.wDay   = atoi( strArrayPart[1] );
			st.wYear  = atoi( strArrayPart[2] );

			if(!(st.wMonth>0 && st.wMonth<13 &&
				 st.wDay>0   && st.wDay<32   &&
			     st.wYear>0                     ))
				return false;

			break;
		}
	case ddmmyyyy_dot://dd.mm.yyyy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			st.wMonth = atoi( strArrayPart[1] );			
			st.wYear  = atoi( strArrayPart[2] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case ddmmyy_dot://dd.mm.yy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			st.wMonth = atoi( strArrayPart[1] );			
			st.wYear  = atoi( strArrayPart[2] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case yyddd://yyddd
		{
			if( nSize<1 )
				return false;

			int nYear = atoi( strArrayPart[0].Left(2)  );
			int nDays = atoi( strArrayPart[0].Right(3) );

			st.wYear = nYear;

			for( i=1; i<13; i++ )
			{
				if( i==1||i==3||i==5||i==7||i==8||i==10||i==12 )
				{
					if( nDays<32 )
					{
						st.wDay   = nDays;
						st.wMonth = i;
						break;
					}
					else
						nDays -= 31;
				}
				else if( i==4||i==6||i==9||i==11 )
				{
					if( nDays<31 )
					{
						st.wDay   = nDays;
						st.wMonth = i;
						break;
					}
					else
						nDays -= 30;
				}
				else//i==2
				{
					COleDateTime time;					
					if( time.SetDate(nYear,2,29)==0 )//成功,闰年st
					{
						if( nDays<30 )
						{
							st.wDay   = nDays;
							st.wMonth = i;
							break;
						}
						else
							nDays -= 29;
					}
					else
					{
						if( nDays<29 )
						{
							st.wDay   = nDays;
							st.wMonth = i;
							break;
						}
						else
							nDays -= 28;
					}
				}
			}
			break;
		}
	case yyyyddd://yyyyddd
		{
			if( nSize<1 )
				return false;

			int nYear = atoi( strArrayPart[0].Left(4)  );
			int nDays = atoi( strArrayPart[0].Right(3) );

			st.wYear = nYear;

			for( i=1; i<13; i++ )
			{
				if( i==1||i==3||i==5||i==7||i==8||i==10||i==12 )
				{
					if( nDays<32 )
					{
						st.wDay   = nDays;
						st.wMonth = i;
						break;
					}
					else
						nDays -= 31;
				}
				else if( i==4||i==6||i==9||i==11 )
				{
					if( nDays<31 )
					{
						st.wDay   = nDays;
						st.wMonth = i;
						break;
					}
					else
						nDays -= 30;
				}
				else//i==2
				{
					COleDateTime time;					
					if( time.SetDate(nYear,2,29)==0 )//成功,闰年st
					{
						if( nDays<30 )
						{
							st.wDay   = nDays;
							st.wMonth = i;
							break;
						}
						else
							nDays -= 29;
					}
					else
					{
						if( nDays<29 )
						{
							st.wDay   = nDays;
							st.wMonth = i;
							break;
						}
						else
							nDays -= 28;
					}
				}
			}
			break;
		}
	case qQyyyy_blank://q Q yyyy
		{
			if( nSize<3 )
				return false;

			st.wMonth  = atoi( strArrayPart[0] )*3;
			st.wYear   = atoi( strArrayPart[2] );
			break;
		}
	case qQyy_blank://q Q yy
		{
			if( nSize<3 )
				return false;

			st.wMonth  = atoi( strArrayPart[0] )*3;
			st.wYear   = atoi( strArrayPart[2] );
			break;
		}
	case mmmyyyy_blank://mmm yyyy
		{
			if( nSize<2 )
				return false;

			int nMonth = strMonth.Find(strArrayPart[0]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[1] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case mmmyy_blank://mmm yy
		{
			if( nSize<2 )
				return false;

			int nMonth = strMonth.Find(strArrayPart[0]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[1] );

			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;

			break;
		}
	case wwWKyyyy_blank://ww WK yyyy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			st.wYear  = atoi( strArrayPart[2] ); 
			break;
		}
	case wwWKyy_blank://ww WK yy
		{
			if( nSize<3 )
				return false;

			st.wDay   = atoi( strArrayPart[0] );
			st.wYear  = atoi( strArrayPart[2] ); 
			break;
		}
	case ddmmmyyyyhhmm_colon://dd-mmm-yyyy hh:mm
		{		
			if( nSize<5 )
				return false;

			st.wDay    = atoi( strArrayPart[0] );
			int nMonth = strMonth.Find(strArrayPart[1]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[2] );
			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;			

			st.wHour   = atoi( strArrayPart[3] );
			st.wMinute = atoi( strArrayPart[4] );

			break;
		}
	case ddmmmyyyyhhmmss_colon://dd-mmm-yyyy hh:mm:ss
		{
			if( nSize<6 )
				return false;

			st.wDay    = atoi( strArrayPart[0] );
			int nMonth = strMonth.Find(strArrayPart[1]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[2] );
			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;			

			st.wHour   = atoi( strArrayPart[3] );
			st.wMinute = atoi( strArrayPart[4] );
			st.wSecond = atoi( strArrayPart[5] );

			break;
		}
	case ddmmmyyyyhhmmssss_colon://dd-mmm-yyyy hh:mm:ss.ss
		{
			if( nSize<7 )
				return false;

			st.wDay    = atoi( strArrayPart[0] );
			int nMonth = strMonth.Find(strArrayPart[1]);
			if( nMonth==-1 || nMonth%3!=0 )
				return false;
			st.wMonth = nMonth/3+1;			
			st.wYear  = atoi( strArrayPart[2] );
			if(!( st.wMonth>0 && st.wMonth<13 &&
				  st.wDay>0   && st.wDay<32   &&
			      st.wYear>0                    ) )
				return false;			

			st.wHour   = atoi( strArrayPart[3] );
			st.wMinute = atoi( strArrayPart[4] );
			st.wSecond = atoi( strArrayPart[5] );
			st.wMilliseconds = atoi( strArrayPart[6] )*10;

			break;
		}
	case hhmm_colon://hh:mm
		{
			if( nSize<2 )
				return false;

			st.wYear    = 2000;
			st.wMonth   = 1;
			st.wDay     = 1;
			st.wHour    = atoi( strArrayPart[0] );
			st.wMinute  = atoi( strArrayPart[1] );
			break;
		}
	case hhmmss_colon://hh:mm:ss
		{
			if( nSize<3 )
				return false;

			st.wYear    = 2000;
			st.wMonth   = 1;
			st.wDay     = 1;
			st.wHour    = atoi( strArrayPart[0] );
			st.wMinute  = atoi( strArrayPart[1] );
			st.wSecond  = atoi( strArrayPart[2] );
			break;
		}
	case hhmmssss_colon://hh:mm:ss.ss
		{
			if( nSize<4 )
				return false;

			st.wYear    = 2000;
			st.wMonth   = 1;
			st.wDay     = 1;
			st.wHour           = atoi( strArrayPart[0] );
			st.wMinute         = atoi( strArrayPart[1] );
			st.wSecond         = atoi( strArrayPart[2] );
			st.wMilliseconds   = atoi( strArrayPart[3] );
			break;
		}

	case week_en://Monday,Tuesday,st.st
		{
			break;
		}
	case week_en_bre://Mon,Tue,Wed,st..
		{
			break;
		}
	case month_en://Juanuary,February
		{
			break;
		}
	case month_en_bre://Jan,Feb,Mar,...
		{
			break;
		}
	//以下为符合中国习惯的日期表示方式:
	case yyyymmdd_line://yyyy-mm-dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yymmdd_line://yy-mm-dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yyyymmdd_dot://yyyy.mm.dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yymmdd_dot://yy.mm.dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yyyymmdd_bias://yyyy/mm/dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yymmdd_bias://yy/mm/dd
		{
			if( nSize<3 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			break;
		}
	case yyyymmddhhmm_line://yyyy-mm-dd hh:mm
		{
			if( nSize<5 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			break;
		}
	case yyyymmddhhmmss_line ://yyyy-mm-dd hh:mm:ss
		{
			if( nSize<6 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			st.wSecond      = atoi( strArrayPart[5] );
			break;
		}
	case yyyymmddhhmmssss_line://yyyy-mm-dd hh:mm:ss.ss
		{
			if( nSize<7 )
				return false;

			st.wYear           = atoi( strArrayPart[0] );
			st.wMonth          = atoi( strArrayPart[1] );
			st.wDay            = atoi( strArrayPart[2] );
			st.wHour           = atoi( strArrayPart[3] );
			st.wMinute         = atoi( strArrayPart[4] );
			st.wSecond         = atoi( strArrayPart[5] );
			st.wMilliseconds   = atoi( strArrayPart[6] )*10;
			break;
		}
	case yyyymmddhhmm_dot://yyyy.mm.dd hh:mm
		{
			if( nSize<5 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			break;
		}
	case yyyymmddhhmmss_dot://yyyy.mm.dd hh:mm:ss
		{
			if( nSize<6 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			st.wSecond      = atoi( strArrayPart[5] );
			break;
		}
	case yyyymmddhhmmssss_dot://yyyy.mm.dd hh:mm:ss.ss
		{
			if( nSize<7 )
				return false;

			st.wYear           = atoi( strArrayPart[0] );
			st.wMonth          = atoi( strArrayPart[1] );
			st.wDay            = atoi( strArrayPart[2] );
			st.wHour           = atoi( strArrayPart[3] );
			st.wMinute         = atoi( strArrayPart[4] );
			st.wSecond         = atoi( strArrayPart[5] );
			st.wMilliseconds   = atoi( strArrayPart[6] )*10;
			break;
		}
	case yyyymmddhhmm_bias://yyyy/mm/dd hh:mm
		{
			if( nSize<5 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			break;
		}
	case yyyymmddhhmmss_bias://yyyy/mm/dd hh:mm:ss
		{
			if( nSize<6 )
				return false;

			st.wYear        = atoi( strArrayPart[0] );
			st.wMonth       = atoi( strArrayPart[1] );
			st.wDay         = atoi( strArrayPart[2] );
			st.wHour        = atoi( strArrayPart[3] );
			st.wMinute      = atoi( strArrayPart[4] );
			st.wSecond      = atoi( strArrayPart[5] );
			break;
		}
	case yyyymmddhhmmssss_bias://yyyy/mm/dd hh:mm:ss.ss
		{
			if( nSize<7 )
				return false;

			st.wYear           = atoi( strArrayPart[0] );
			st.wMonth          = atoi( strArrayPart[1] );
			st.wDay            = atoi( strArrayPart[2] );
			st.wHour           = atoi( strArrayPart[3] );
			st.wMinute         = atoi( strArrayPart[4] );
			st.wSecond         = atoi( strArrayPart[5] );
			st.wMilliseconds   = atoi( strArrayPart[6] )*10;
			break;
		}
	}*/
	return true;
}
//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
bool CDateField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;
/*===time  
		str.MakeLower(); //转化为小写.
	
		SYSTEMTIME st;
		ZeroMemory(&st, sizeof(st));
		if( !StringToSysTime( str, st, m_byteEnumData) )
		{
			*(FieldNUM*)m_pValue = NULLValue;
			return false;
		}
		else
		{
			if(!SystemTimeToFileTime(&st, (FILETIME *)m_pValue))
				return false;
		}	
   */
	return true;
}



//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CDateField::SetDefaultValue()
{
	*((FieldNUM *)m_pValue) = * ((FieldNUM *)m_pDefaultValue);
	memcpy(m_pValue,m_pDefaultValue,9);
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CDateField::CopyToDefaultValue()
{
	*((FieldNUM *)m_pDefaultValue) = * ((FieldNUM *)m_pValue);
	return true;
}

int CDateField::GetDay()
{
/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetDay();   */
	return 0;
}

int CDateField::GetYear()
{
/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetYear();   */
	return 0;
}

int CDateField::GetMonth()
{
/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetMonth();   */
	return 0;
}

int CDateField::GetHour()
{

/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetHour();   */
	return 0;
}

int CDateField::GetMinute()
{
/*===time  
	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetMinute();   */
	return 0;
}

int CDateField::GetSecond()
{
/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetSecond();   */
	return 0;
}

int CDateField::GetDayOfWeek()
{
/*===time  	COleDateTime temp(*(FILETIME *)m_pValue);
	return temp.GetDayOfWeek();   */
	return 0;
}

CTString CDateField::GetDateType( DateType byteType )
{
	//assert(byteType>=DateDisplayMin && byteType<=DateDisplayMax);
	CTString str = "%Y/%m/%d %H:%M:%S";
	switch( byteType )
	{
	case ddmmmyyyy_line://dd-mmm-yyyy
		str = "%d-%b-%Y";
		break;
	case ddmmmyy_line://dd-mmm-yy
		str = "%d-%b-%y";
		break;
	case mmddyyyy_bias ://mm/dd/yyyy
		str = "%m/%d/%Y";
		break;
	case mmddyy_bias ://mm/dd/yy
		str = "%m/%d/%y";
		break;
	case ddmmyyyy_dot://dd.mm.yyyy
		str = "%d.%m.%Y";
		break;
	case ddmmyy_dot://dd.mm.yy
		str = "%d.%m.%y";
		break;

	case ddmmmyyyyhhmm_colon://dd-mmm-yyyy hh:mm
		str = "%d-%b-%Y %H:%M";
		break;
	case ddmmmyyyyhhmmss_colon://dd-mmm-yyyy hh:mm:ss
		str = "%d-%b-%Y %H:%M:%S";
		break;
	case ddmmmyyyyhhmmssss_colon://dd-mmm-yyyy hh:mm:ss.ss
		str = "%d-%b-%Y %H:%M:%S";
		break;
	case hhmm_colon://hh:mm
		str = "%H:%M";
		break;
	case hhmmss_colon://hh:mm:ss
		str = "%H:%M:%S";
		break;
	case hhmmssss_colon://hh:mm:ss.ss
		str = "%H:%M:%S";
		break;

	//以下为符合中国习惯的日期表示方式:
	case yyyymmdd_line://yyyy-mm-dd
		str = "%Y-%m-%d";
		break;
	case yymmdd_line://yy-mm-dd
		str = "%y-%m-%d";
		break;
	case yyyymmdd_dot://yyyy.mm.dd
		str = "%Y.%m.%d";
		break;
	case yymmdd_dot://yy.mm.dd
		str = "%y.%m.%d";
		break;
	case yyyymmdd_bias://yyyy/mm/dd
		str = "%Y/%m/%d";
		break;
	case yymmdd_bias://yy/mm/dd
		str = "%y/%m/%d";
		break;
	case yyyymmddhhmm_line://yyyy-mm-dd hh:mm
		str = "%Y-%m-%d %H:%M";
		break;
	case yyyymmddhhmmss_line ://yyyy-mm-dd hh:mm:ss
		str = "%Y-%m-%d %H:%M:%S";
		break;
	case yyyymmddhhmmssss_line://yyyy-mm-dd hh:mm:ss.ss
		str = "%Y-%m-%d %H:%M:%S";
		break;
	case yyyymmddhhmm_dot://yyyy.mm.dd hh:mm
		str = "%Y.%m.%d %H:%M";
		break;
	case yyyymmddhhmmss_dot://yyyy.mm.dd hh:mm:ss
		str = "%Y.%m.%d %H:%M:%S";
		break;
	case yyyymmddhhmmssss_dot://yyyy.mm.dd hh:mm:ss.ss
		str = "%Y.%m.%d %H:%M:%S";
		break;
	case yyyymmddhhmm_bias://yyyy/mm/dd hh:mm
		str = "%Y/%m/%d %H:%M";
		break;
	case yyyymmddhhmmss_bias://yyyy/mm/dd hh:mm:ss
		str = "%Y/%m/%d %H:%M:%S";
		break;
	case yyyymmddhhmmssss_bias://yyyy/mm/dd hh:mm:ss.ss
		str = "%Y/%m/%d %H:%M:%S";
		break;	
	}
	return str;
}

bool CDateField::SetEnumType(DateType Type)
{
	if( Type>=DateDisplayMin && Type<=DateDisplayMax )
	{
		m_bIsEnum = true;
		m_byteEnumData = Type;
	}
	else
	{
		m_bIsEnum = false;		
	}
	return true;
}

DateType CDateField::GetEnumType()
{
	if( !m_bIsEnum )
		return 255;
	else
		return m_byteEnumData;
}


//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
LPCTSTR CDateField::GetValueLable()
{
	double nValue = GetAsDouble();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(fabs(m_ValueLabel.GetAsDouble() - nValue) < DoubleZero)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}
CTString CDateField::GetValLable()
{
	double nValue = GetAsDouble();
	
	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(fabs(m_ValueLabel.GetAsDouble() - nValue) < DoubleZero)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}