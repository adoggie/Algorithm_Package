
#include "jnisock.h"
int main(){
	AlgoCommProxy prx("127.0.0.1",6005);
	char * msg="hello!";
	DataBlock request,response;
	request.assign((unsigned char*)msg,(unsigned char*)msg+strlen(msg)+1);
	prx.Send(request,response);	
	return 0;
}
