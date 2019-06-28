#include"../include/StrUtils.h"
//字符串工具
using namespace std;

int min(int a, int b ,int c)
{
  int tmp = a< b? a:b;  
  return tmp < c ? tmp :c;
}

bool isUTF8(const string &one)
{
    return(one[0] & 0xe0 && one[1] & 0x80 && one[2] & 0x80); //UTF-8（8-bit Unicode Transformation Format）是一种针对Unicode的可变长度字符编码，又称万国码
} 
//&&”(逻辑与)是逻辑运算符。 “逻辑与”相当于生活中说的“并且”，就是两个条件都同时成立的情况下“逻辑与”的运算结果才为“真”。“&”是位运算符。与运算(&),双目运算。二个位都置位(等于1)时，结果等于1，其它的结果都等于0。


vector<string> Str2Vec(const string & rhs)//
{
  vector<string> ret;
  if(isUTF8(rhs) ){
      for(string::size_type col = 0; col < rhs.size();col +=3  ){
	    string sing = rhs.substr(col,3);//UTF-8不是固定字长编码的，而是一种变长的编码方式。它可以使用1~4个字节表示一个符号，根据不同的符号而变化字节长度。
        ret.push_back(sing);
	  }
  }else{
      for(string::size_type col =0; col< rhs.size(); ++col){
          string sing = rhs.substr(col,1);
          ret.push_back( sing );      }
  }
  return ret;
}








