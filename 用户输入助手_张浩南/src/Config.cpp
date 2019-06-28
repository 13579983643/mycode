#include"../include/Config.h"
Config* Config::_Instance = NULL;
Config::Config(const string & path)
{
  ifstream is(path);
  string section;   
  string content;
  while(is >> section >> content){
     _configMap.insert(make_pair(section,content));
  }
   
  is.close();
}

//getInstance在单例模式(保证一个类仅有一个实例，并提供一个访问它的全局访问点)的类中常见，用来生成唯一的实例，getInstance往往是static的。


