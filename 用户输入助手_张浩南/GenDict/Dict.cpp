
#include"Dict.h"
#include<stdio.h>
#include<unistd.h>  //通常封装了大量的系统调用
#include<sys/types.h>  //基本系统数据类型，此头文件还包含适当时应使用的多个基本派生类型
#include<dirent.h>
#include<stdlib.h>

void Dict::genCn(){  //中文分词
    string cn = _libPath + "cn/";
    chdir(cn.c_str());
    DIR* cur = opendir(".");
    struct dirent *entry;
    while((entry = readdir(cur)) != NULL){
        string filename = entry->d_name;
        if(filename != "." && filename != ".."){
				cppjieba::Jieba jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH);
        //   
		vector<string> words;//string是一个串类，在stl中可以直接定义数据类型为string
        vector<cppjieba::Word> jiebawords; 
        string s;
        string result;
        ifstream  is(filename); 
        string line;
        while(is){
		   getline(is,s);
		   jieba.CutAll(s,words)   //python的分词工具
		   for(auto c = words.begin(); c != words.end(); ++c )//遍历分好的词容器
           { 
			 if(c->size() >3){  //词大于3  ??
			  _cnDict[*c]++; // map ??词频加1
			 }		 
		   }
		}
		is.close();
    	}
	}	
    closedir(cur);
	chdir("../..");
}



void Dict::genEn(){
  string en = _libPath + "en/"; //  GenDict/lib/cn     GenDict/lib/en
  chdir(en.c_str());      
  DIR* cur = opendir(".");
  struct dirent *entry;
  while((entry = readdir(cur))!=NULL){
       string filename(entry->d_name);
       if(filename != "." && filename != ".."){     
         _enDict.read(filename);//英文分词  
       }
  }
  closedir(cur);
  chdir("../..");
}

void Dict::output(){

   //cnDict
   string cnFilepath = _outPath + "cnDict.dat";
   ofstream cn(cnFilepath);
   auto map_it = _cnDict.cbegin(); //返回一个常量迭代器，此迭代器指向映射中的第一个元素。
   while(map_it != _cnDict.cend() ){
      cn << map_it->first << " "<< map_it->second << endl;
      ++map_it;
   }
   cn.close();

  //enDict
   string enFilepath = _outPath + "enDict.dat"; 
   _enDict.list(enFilepath); 

}
// 函数说明：readdir()返回参数dir 目录流的下个目录进入点。结构dirent 定义如下：
// struct dirent
// {
//    ino_t d_ino; //d_ino 此目录进入点的inode
//    ff_t d_off; //d_off 目录文件开头至此目录进入点的位移
//    signed short int d_reclen; //d_reclen _name 的长度, 不包含NULL 字符
//    unsigned char d_type; //d_type d_name 所指的文件类型 d_name 文件名
//    har d_name[256];
// };









