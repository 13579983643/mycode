
#include"../include/TcpConnection.h"
#include"../include/EpollPoller.h"
#include<string.h>
#include<stdio.h>
#include<iostream>
//（一）TCP网络编程的本质：三个半事件

//1. 连接的建立，包括服务端接受(accept) 新连接和客户端成功发起(connect) 连接。TCP 连接一旦建立，客户端和服务端是平等的，可以各自收发数据。

//2. 连接的断开，包括主动断开(close 或shutdown) 和被动断开(read(2) 返回0)。

//3. 消息到达，文件描述符可读。这是最为重要的一个事件，对它的处理方式决定了网络编程的风格（阻塞还是非阻塞，如何处理分包，应用层的缓冲如何设计等等）。

//3.5 消息发送完毕，这算半个。对于低流量的服务，可以不必关心这个事件；另外，这里“发送完毕”是指将数据写入操作系统的缓冲区，将由TCP 协议栈负责数据的发送与重传，不代表对方已经收到数据。

using::std::cout;
using::std::endl;
using::std::string;

namespace wd
{
 TcpConnection::TcpConnection(int sockfd, EpollPoller* loop) //都是muduo库对I/O复用机制的封装，不过默认使用的是EpollPoller。 
 :sockfd_(sockfd) 
 ,sockIO_(sockfd)
 ,localAddr_(wd::Socket::getLocalAddr(sockfd))
 ,peerAddr_(wd::Socket::getPeerAddr(sockfd) )  
 ,isShutdownWrite_(false) 
 ,loop_(loop)//EventLoop是对Reactor模式的封装，由于Muduo的并发原型是 Multiple reactors + threadpool  (one loop per thread + threadpool)，所以每个线程最多只能有一个EventLoop对象。
 {
   sockfd_.nonblock(); 
 }

 TcpConnection::~TcpConnection()
 {  
    if(! isShutdownWrite_ )
    {
	  isShutdownWrite_ = true;
	  shutdown();
	}    
 
 }	 

 std::string TcpConnection::receive()  
 {
   char buf[65536]; //因16位计算机的存在，2的16次方即65536
   memset(buf, 0 ,sizeof(buf));
   size_t ret = sockIO_.readline(buf, sizeof(buf)); 
   if(ret == 0 )
   {
      return std::string();
   }else{
      return std::string(buf);//string类的构造函数：string(const char *s);  //用c字符串初始化s
   }
 }

void TcpConnection::send(const std::string & msg) //消息
{
  sockIO_.writen(msg.c_str(),msg.size() ); 
  std::string  logwarn = "Message sent through socket" + std::to_string(sockIO_.getFd());//数值转换字符串函数
  LogWarn(logwarn ); //日志
  std::cout << "sockfd =" << sockIO_.getFd() << std::endl; 
}	

void TcpConnection::shutdown()
{
  if(!isShutdownWrite_)
     sockfd_.shutdownWrite(); 
  isShutdownWrite_ = true;  
}	

std::string TcpConnection::toString() 
{
  char str[100];
  //Linux下的C函数snprintf可以用来构建字符串，可以将不同类型的字符连接起来 
  snprintf(str, sizeof(str),"%s:%d -> %s:%d ", 
		   localAddr_.ip().c_str(), localAddr_.port(),
	       peerAddr_.ip().c_str(),peerAddr_.port());

  return std::string(str);
}


void TcpConnection::setConnectionCallback(TcpConnectionCallback cb ) //TcpConnectionCallback cb ?? 
{
    onConnectionCb_ = cb;
}	

void TcpConnection::setMessageCallback(TcpConnectionCallback cb  )
{
   onMessageCb_ = cb;
}	

void TcpConnection::setCloseCallback(TcpConnectionCallback cb )
{
   onCloseCb_ = cb;
}	

void TcpConnection::handleConnectionCallback()
{
    if(onConnectionCb_ )
         onConnectionCb_(shared_from_this());    
}	

void TcpConnection::handleMessageCallback()
{
   if(onMessageCb_ )
         onMessageCb_(shared_from_this() ); 
}	

void TcpConnection::handleCloseCallback()// set + cb  // handle + on 
{
   if(onCloseCb_ )//?on
        onCloseCb_(shared_from_this());//功能为返回一个当前类的std::share_ptr ,当类A被share_ptr管理，且在类A的成员函数里需要把当前类对象作为参数传给其他函数时，就需要传递一个指向自身的share_ptr。
}	

void TcpConnection::sendInLoop(const std::string & msg)  
{
    loop_->runInLoop(std::bind( &TcpConnection::send, this, msg ));//
}


}// end of namespace wd




