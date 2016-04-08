
#include  "TString.h"
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <typeinfo>

int main(){
	typeid(void);
	CTString s1;
	s1.Format("%s",CTString("abc"));
	return 0;

}




































