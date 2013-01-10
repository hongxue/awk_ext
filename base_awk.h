#include <qse/awk/StdAwk.hpp>
#include <qse/cmn/opt.h>
#include <qse/cmn/main.h>
#include <qse/cmn/mbwc.h>
#include <qse/cmn/stdio.h>
#include <cstring>
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
typedef QSE::StdAwk StdAwk;
typedef QSE::StdAwk::Run Run;
typedef QSE::StdAwk::Value Value;

#include <dlfcn.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

void setRtnInfo(Run &run ,Value &ret,int code,Value &val);
typedef vector< std::pair<string,StdAwk::FunctionHandler> > awk_func_entry;
typedef awk_func_entry (*func_entry)();
typedef void (*awk_module_init_func)(int p);
class base_awk: public StdAwk
{
public:
    map<string,void *> dl_arr;
public:
  base_awk ():m_id(0) { }
	~base_awk () ;
    int m_id;
	int loadlib(string &lib_ini);
    int get_module_id(){
        m_id=m_id+100;
        return m_id;
    }
};
