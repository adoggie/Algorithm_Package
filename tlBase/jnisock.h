
/*******************************************************
 	name: 	jinsock.h
	func:	桥接jni接口为tcp/socket接口
			适用于unix系统
			接口库 posix
	author:	zhangbin
	reversion:
			2006.10.16	zhangbin 
				1. created
*********************************************************/
#ifndef _JNISOCK_H
#define _JNISOCK_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "algsvc.h"


#define AlgoServerHost	"127.0.0.1"		//算法服务器地址
#define AlgoServerPort	6045			//算法服务器通信端口


class AlgoCommProxy{
	std::string _host;
	int			_port;
	int			_sock;	
private:	
	bool	Connect(){
		int ret;
		_sock = socket(AF_INET,SOCK_STREAM,0);
		sockaddr_in sa;
		sa.sin_family = AF_INET;	//-- AF_UNIX	optional
		sa.sin_port = htons( (unsigned short) _port);
		sa.sin_addr.s_addr = inet_addr( _host.c_str());
		ret = connect( _sock,(sockaddr*)&sa,sizeof(sa));
		if( ret ){
			close(_sock);
			_sock = 0;
			return false;
		}
		return true;
	}	
public:
	AlgoCommProxy(const std::string& svrHost,int svrPort):_host(svrHost),_port(svrPort){}
	~AlgoCommProxy(){
		if( _sock >=0){
			close(_sock);
		}
	}
	//连接服务器,发送java命令请求，接收处理结果并返回给java
	//socket block mode
	bool 	Send(const DataBlock& request,DataBlock & result){
		result.clear();
		int r;
		if( !request.size()){
			return false;
		}		
		if( !Connect()){ //-- reject or not started
			return false;		
		}	
		int len = request.size();
		const unsigned char * first = &request.at(0);
		while(1){
			r = send(_sock,(void*)&first,len,0);
			if( r < 0){
				return false;
			}else if( r==0){
				//-- half close
				shutdown(_sock,SHUT_WR);
				break;
			}else{
				len -=r;
				first+=r;
			}
		}
		len = 1024;
		char *buff = new char[len];
		//back_insert_iterator<DataBlock> result;
		while( (r = recv(_sock,buff,len,0))>=0 ){
			result.resize( result.size()+ len);
			memcpy( &result.at(result.size()),buff,len);
		}
		delete[] buff;
		close(_sock);	_sock = 0;
		return true;
	}
};




























#endif


