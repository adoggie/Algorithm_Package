// System.h: interface for the CSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSTEM_H__B436B8AB_7842_4645_959A_0FD261805493__INCLUDED_)
#define AFX_SYSTEM_H__B436B8AB_7842_4645_959A_0FD261805493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <functional>
//**************  宏定义 ************************

#define S_VAR_PRE  "var"
#include "TString.h"
class TCTable;

class TCSystem  
{
	static CTString m_sIniPath;//ini 文件的配置路径
	static CTString m_sTempPath;//临时文件的配置路径
public:
    static CTString ConvertPath(const char *src);
	static void SetTempPath(CTString sFilePath);
	static void SetIniPath(CTString sFilePath);

	static bool GetProfileStrValue(CTString strSection, CTString strSectionKey, CTString&value);
	static bool GetModuleInfo(CTString strName,CTString&sDllName,CTString&sProcName,int &nProcType);
	static void GetSystemTime(SYSTEMTIME * st);
	static bool GetProfileIntValue(CTString strSection, CTString strSectionKey, int &value);
	static void GetTempFileName(CTString szTempPath,CTString PrefixString,CTString&sName);
	static bool GetTempPath(CTString &strPath);
	//数据库到矩阵的转换。
	//static bool TableToMatrix(TCTable &tb, CTString sFieldsList,TCMatrix<double> &mt);
	//static bool TableToMatrix(TCTable &tb,vector<int> nFieldsList, TCMatrix<double> &mt);
	static bool GetModulePath(CTString &strPath);
	static bool GetWorkPath(CTString & strPath);
	TCSystem();
	virtual ~TCSystem();

//----------------------------------------------------
/*
	class : Properties
	brief : 读取配置文件参数  section/key/value ,trituple formula
	methods: 
		FromIniFile()
		value()
		intValue()
	others:
		2006.10.09	zhangbin
*/
#ifdef _UNIX
class Properties{
public:
	typedef std::map<std::string,std::string> IniPropertyEntry;
	typedef std::map<std::string,IniPropertyEntry > IniSectionList;
	static Properties FromeIniFile(const std::string& file){
		Properties cp;
		cp.IniLoad(file);
		return cp;
	}
	
	std::string value(const std::string& section,const std::string& key,const std::string& _default=""){
		std::string ret;
		ret = _default;
		IniSectionList::iterator itr;
		IniPropertyEntry::iterator peitr;
		if( (itr=sections.find( section )) != sections.end()){
			peitr=itr->second.find(key);
			if( peitr!=itr->second.end()){ //--key existed
				return peitr->second;
			}
		}
		return ret;
	}

	int		intValue(const std::string& section,const std::string& key,int _default=0){
		std::string str;
		int ret;
		ret = _default;
		str = value(section,key);
		if( str.size()){
			ret = atoi(str.c_str());
		}
		return ret;
	}
private:
	IniSectionList sections;
	
	static bool IniValueRestrictedRange(int ch){
		return (ch>0x20&&ch<=0x7e)?true:false;
	}

	static void IniStringTrim(std::string & str){
		std::string::iterator itr;
		std::string stemp;
		itr = std::find_if(str.begin(),str.end(),IniValueRestrictedRange);
		stemp.assign(itr,str.end());
		std::string::reverse_iterator ritr;
		ritr = std::find_if(stemp.rbegin(),stemp.rend(),IniValueRestrictedRange);
		str.assign(stemp.begin(),ritr.base());
	}

	void IniLoad(const std::string& file){
		FILE *fp;
		fp = fopen(file.c_str(),"r");
		if(!fp)	return;
		char buff[1024];
		std::string line,stemp,k,v;
		std::string::iterator itr;
		std::string title;
		size_t np1,np2;
		sections.clear();
		while(fgets(buff,sizeof(buff),fp)){
			line.assign(buff,strlen(buff)-1);
			IniStringTrim(line);
			if( !line.size() || *line.begin()=='#'){
				continue;
			}		
			np1 = line.find('[');
			np2 = line.find(']');
			if( np2>np1 ){
				IniPropertyEntry list;
				title.assign(line.c_str(),np1+1,np2-np1-1);
				sections.insert(IniSectionList::value_type(title,list));
				continue;
			}
			if( title==""){	continue;}	//invalid section title
			itr =std::find(line.begin(),line.end(),'=');
			if( itr==line.end())	continue;
			k.assign(line.begin(),itr);
			v.assign(++itr,line.end());
			IniStringTrim(k);IniStringTrim(v);
			IniPropertyEntry & rlist = sections[title];
			rlist.insert(IniPropertyEntry::value_type(k,v));			
		}
		fclose(fp);
	}
public:
	void IniIterative(){
		IniSectionList::iterator itr;
		IniPropertyEntry::iterator peitr;
		for(itr = sections.begin();itr!=sections.end();itr++){
//			std::cout<< itr->first<<std::endl;
			for( peitr = itr->second.begin();peitr!=itr->second.end();peitr++){
			;//	std::cout<<"\t"<<peitr->first<<":"<<peitr->second<<std::endl;
				
			}
		}
	}

};
 #endif

};

#endif // !defined(AFX_SYSTEM_H__B436B8AB_7842_4645_959A_0FD261805493__INCLUDED_)
