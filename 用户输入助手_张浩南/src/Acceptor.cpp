#include"../include/Acceptor.h"
#include"../include/SocketUtil.h"
#include<iostream>
using std::cout;
using std::endl;

namespace wd
{
  Acceptor::Acceptor(int listenfd, const InetAddress & addr) //初始构造函数监听 句柄+地址
  : listenSock_(listenfd)
  ,addr_(addr)
  {}

  void Acceptor::ready()
  {
    setReuseAddr(true); 
    setReusePort(true);    
    bind();
    listen();
  }  
  int Acceptor::accept()
  {
    int peerfd = ::accept(listenSock_.fd(), NULL, NULL );
    if(peerfd == -1)
	{
	   perror("accept error");
	}
    return peerfd;
  }  
 
void Acceptor::setReuseAddr(bool flag)
{
  int on = (flag?1:0); //成立执行1
  if(::setsockopt(listenSock_.fd(), SOL_SOCKET, SO_REUSEADDR, &on,static_cast<socklen_t>(sizeof(on)) ) == -1)  
  //一般不会立即关闭而经历TIME_WAIT的过程）后想继续重用该socket：
  //BOOL bReuseaddr=TRUE;
  //setsockopt(s,SOL_SOCKET ,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL));
  {
    perror("setsockopt reuseaddr error");
    ::close(listenSock_.fd() );
    exit(EXIT_FAILURE);//EXIT_FAILURE 可以作为exit()的参数来使用，表示没有成功地执行一个程序
  }
}


void Acceptor::setReusePort(bool flag)
{
#ifdef SO_REUSEADDR 
    //这表明如果标识符_XXXX已被#define命令定义过则对程序段1进行编译；否则对程序段2进行编译  
     int on = (flag?1:0);
     if(::setsockopt(listenSock_.fd(),SOL_SOCKET,SO_REUSEPORT,&on, static_cast<socklen_t>(sizeof(on)))== -1 )//int a=1;double b;a=static_cast<int>(b); //用于基本类型间转换
     {
	   perror("setsockopt reuseport error");
       ::close(listenSock_.fd());
       exit(EXIT_FAILURE );
	 }

#else
    if(flag)
    {
      fprintf(stderr, "SO_REUSEADDR is not supported!\n"); //fprintf( )会根据参数format 字符串来转换并格式化数据, 然后将结果输出到参数stream 指定的文件中, 直到出现字符串结束('\0')为止。
	  //stderr（标准错误），是不带缓冲的，这使得出错信息可以直接尽快地显示出来。
	}
#endif
}
  
void Acceptor::bind()
{
  if(-1 == ::bind(listenSock_.fd(), (const struct sockaddr*)addr_.getSockAddrPtr(),sizeof(InetAddress) ))
  //getSockAddrPtr()?
  {
    perror("bind error");
    ::close(listenSock_.fd() );
    exit(EXIT_FAILURE );
  }
}

void Acceptor::listen()
{
  if(-1 == ::listen(listenSock_.fd(), 10 ))
  {
     perror("listen error");
     ::close(listenSock_.fd());
     exit(EXIT_FAILURE );
  } 
}	

}//end of namespace









