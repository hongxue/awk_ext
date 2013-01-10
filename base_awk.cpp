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
#include "base_awk.h"
base_awk::~base_awk () { 
   
        map<string,void *>::iterator itor = dl_arr.begin();
        for(;itor!=dl_arr.end();itor++){
           dlclose(itor->second);
        }
        close (); 
    }

   int base_awk::loadlib(string &lib_ini)
  {
        string lib_list_file(lib_ini);
        lib_list_file.append("/list.txt");
        if(access(lib_list_file.c_str(),F_OK)!=0){
           cout<<"list file not exists";
           return 0;
        }
        FILE *file_handle = fopen(lib_list_file.c_str(),"r");
        char line[1024];
        string mod,path;
        while(!feof(file_handle)){
            bzero(line,1024);
            mod="";
            path="";
            fgets(line,1024,file_handle);    
            char *t_mod = strtok(line," ");
            char *tmp_path = strtok(NULL," ");
            char *t_path = strtok(tmp_path,"\r\n");
            if(t_mod==NULL || t_path==NULL) continue;
            mod.append(t_mod);
            path.append(t_path);
            if(dl_arr.find(mod)!=dl_arr.end()){
                cout<<"dl alread load"<<endl;
                continue;
            }
            if(access(path.c_str(),F_OK)!=0){
                continue;
            }
            void *handle = dlopen(path.c_str(),RTLD_LAZY); 
            if(handle==NULL) 
            {
                cout<<dlerror()<<endl;
                cout<<"error"<<endl;
                continue;
            }
            dl_arr.insert(std::make_pair<string,void *>(mod,handle));
            
            
            awk_module_init_func tmp_init_func = (awk_module_init_func)dlsym(handle,"awk_module_init");
            if(tmp_init_func!=NULL){
                int md_id = this->get_module_id();
                (tmp_init_func)(md_id);
            }
            
            func_entry tmp=(func_entry)dlsym(handle,"get_func_entry");
            if(tmp==NULL){
                cout<<dlerror()<<endl;
                continue;
            };
            awk_func_entry func_list =  (*tmp)();
            awk_func_entry::iterator itor=func_list.begin();
            awk_func_entry::iterator itor_end = func_list.end();
            for(;itor!=itor_end;itor++){
             string func_name = mod+"_"+itor->first;
             if (addFunction(func_name.c_str(), 0, 5,(StdAwk::FunctionHandler)itor->second) <= -1){
                 cout<<"load function "<<func_name<<" error"<<endl;
                 continue;
              }
            }
        }
        fclose(file_handle);
		return 0;
	}
void setRtnInfo(Run &run,Value &ret,int code,Value &val){
      Value val_code(run);
      string code_str="ret";
      string error_f = "error_info";
      string success_f="res_info";
      Value::Index idx_code(code_str.c_str(),code_str.length());
      val_code.setInt(code);
      ret.setIndexedVal(idx_code,val_code);
      if(code<0){
         Value::Index idx_info(error_f.c_str(),error_f.length());
         ret.setIndexedVal(idx_info,val);
      }else{
       Value::Index idx_info(success_f.c_str(),success_f.length());
       ret.setIndexedVal(idx_info,val);
      }
 }
