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
#include <string>
#include <iostream>
#include "base_awk.h"
#include "core_data.h"
using namespace std;


typedef QSE::StdAwk StdAwk;
typedef QSE::StdAwk::Run Run;
typedef QSE::StdAwk::Value Value;
map<string,string> get_main_args(int c,char **argv);

map<string,string> get_main_args(int c,char **argv){
    map<string,string> rtn;
    for(int i=1;i<c;i++){
       string tmp(argv[i]);
       string k,v;
       if(tmp.length()<=2){
          continue;
       }
       if(tmp.substr(0,1)=="-"){
         k=tmp.substr(0,2);
         v= tmp.substr(2);
       }else{
         k="input_file";
         v = tmp;
       }
       rtn.insert(std::make_pair<string,string>(k,v));
       
    }
    return rtn;
}

int main(int argc,char **argv)
{
    base_awk awk;
    awk.open();
    if (awk.addArgument ("apache_log_awk") <= -1){
  	cout<<"set flag error"<<endl;
		return -1; 
    }
    awk.setTrait (awk.getTrait() |QSE_AWK_DEPTH_INCLUDE);
    int foo=awk.addGlobal(QSE_T("global_var"));
    Run *run = NULL;

    map<string,string> args_info = get_main_args(argc,argv);
    map<string,string>::iterator itor_end = args_info.end();
    map<string,string>::iterator itor  = args_info.find("input_file");
    if(itor!=itor_end){
        awk.addArgument(itor->second.c_str());
    }
    itor = args_info.find("-l");
    if(itor!=itor_end){
       awk.loadlib(itor->second);
    }else{
        char *env_name="AWK_LIB_PATH";
        char *lib_path = getenv(env_name);
        if(lib_path!=NULL){
           string lib_path_str(lib_path);
           awk.loadlib(lib_path_str);
        }
    }
    itor = args_info.find("-f");
    if(itor!=itor_end){
        base_awk::SourceFile in(itor->second.c_str());
        run = awk.parse (in, StdAwk::Source::NONE);
        if (run == QSE_NULL){
            core_data::clear();
		    cout<<"parse file error"<<endl;
		    return -1; 
        }
    }else{
       cout<<"script file not set"<<endl;
       return 0;
    }
   
    
    Value global_val(run);
    itor = args_info.find("-v");
    if(itor!=itor_end){
        char buf[1000];
        bzero(buf,1000);
        memcpy(buf,itor->second.c_str(),itor->second.length());
        char *tmp_p = strtok(buf,",");
        int ii=1;
        while(tmp_p!=NULL){
            Value::IntIndex idx(ii++);
            global_val.setIndexedStr(idx,tmp_p,strlen(tmp_p));
            tmp_p=strtok(NULL,",");  
         }
    }
    awk.setGlobal(foo,global_val);
	
    base_awk::Value ret;
	if (awk.loop (&ret) <= -1) 
	{ 
        core_data::clear();
		cout<<"exec error"<<endl; 
		return -1; 
	}
        core_data::clear();
	return 0;
}
