#include"../include/InetAddress.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>  //stdlib.h里面定义了五种类型、一些宏和通用工具函数
#include<arpa/inet.h>  //包含了一些网络编中需要的头文件,还有一些结构体.
#include<string.h>

namespace wd
{
 InetAddress::InetAddress(short port) 
 {
   ::memset(&addr_, 0, sizeof(addr_)); 
   addr_.sin_family = AF_INET;  //IPV4
   addr_.sin_port = htons(port);  //htons用于将指定的16位hostshort转换成网络字符顺序
   addr_.sin_addr.s_addr =INADDR_ANY;  //服务器自动填充本机地址
 
 } 

 InetAddress::InetAddress(const char * pIp ,short port)   
 {
   ::memset(&addr_ , 0 ,sizeof(addr_) );
   addr_.sin_family = AF_INET;
   addr_.sin_port = htons(port); 
   addr_.sin_addr.s_addr = inet_addr(pIp); //inet_addr用来将IP地址字符串换成网络所使用的二进制数字
 
 }

 InetAddress::InetAddress(const struct sockaddr_in & addr)  
 :addr_(addr)  //addr为结构体指针变量，系统会把远程主机的信息保存到这个结构体中
 {} 

  const struct sockaddr_in* InetAddress::getSockAddrPtr() const  //const修饰*this是本质，至于说“表示该成员函数不会修改类的数据 
  {
      return &addr_;
  }

 std::string InetAddress::ip() const  
 {
   return std::string(inet_ntoa(addr_.sin_addr));  //struct in_addr sin_addr 为IP地址  将一个32位网络字节序的二进制IP地址转换成相应的点分十进制的IP地址
 }

 unsigned short InetAddress::port() const   
 {
   return ntohs(addr_.sin_port);//将16位网络字符顺序转换成主机字符顺序
 }


}//end of namespace wd













