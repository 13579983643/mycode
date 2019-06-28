#include"../include/Socket.h"
#include"../include/SocketUtil.h"
#include"../include/InetAddress.h"

namespace wd
{
 //网络上的两个程序通过一个双向的通信连接实现数据的交换，这个连接的一端称为一个socket。建立网络通信连接至少要一对端口号(socket)。socket本质是编程接口(API)
Socket::Socket(int sockfd) 
:sockfd_(sockfd) 
{}
  
Socket::Socket()
:sockfd_(createSocketFd())//fd = ::socket( AF_INET, SOCK_STREAM, 0) tcp
{}

Socket::~Socket() 
{
  ::close(sockfd_);
}

void Socket::nonblock()
{
  setNonblock(sockfd_);  //::fcntl(fd, F_GETFL, 0);  fcntl()针对(文件)描述符提供控制.参数fd是被参数cmd操作(如下面的描述)的描述符.
}	

void Socket::shutdownWrite()
{
  if(::shutdown(sockfd_ ,SHUT_WR ) == -1) //shutdown函数不能关闭套接字，只能关闭输入和输出流，然后发送EOF，假设套接字为A，那么这个函数会关闭所有和A相关的套接字，包括复制的；而close能直接关闭套接字。
  {
     perror("shutdown write error!");
  }
}

InetAddress Socket::getLocalAddr(int sockfd)
{
  struct sockaddr_in addr;   
  socklen_t len = sizeof(sockaddr_in);  //socket编程中的accept函数的第三个参数的长度必须和int的长度相同。于是便有了socklen_t类型。
  if(::getsockname(sockfd, (struct sockaddr*)&addr , &len) == -1)//getsockname用来获取sockfd当前关联的地址,结果存在addr指向的空间中,成功返回0，失败返回-1，并将errno设置为对应的错误。
  {
     perror("getsockname error");
  }
  return InetAddress(addr); //addr_(addr)
}

InetAddress Socket::getPeerAddr(int sockfd) 
{
  struct sockaddr_in addr;
  socklen_t len = sizeof(sockaddr_in);
  if(::getpeername(sockfd, (struct sockaddr*)&addr, &len) == -1 )//getpeername()函数来获取当前连接的客户端的IP地址和端口号。
  {
    perror("getpeername error");
  }
  return InetAddress(addr); 
}

}//end of namespace wd













