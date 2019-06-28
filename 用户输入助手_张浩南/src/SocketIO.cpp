#include"../include/SocketIO.h"
#include"../include/SocketUtil.h"

namespace wd
{
SocketIO::SocketIO(int sockfd) 
:sockfd_(sockfd)
{}

size_t SocketIO::readn(char* buf , size_t count)
{
  size_t nleft = count;
  char* pbuf = buf;
  while(nleft >0)
  { 
    int nread = ::read(sockfd_ , pbuf, nleft); //read()会把参数fd所指的文件传送count 个字节到buf 指针所指的内存中。返回值为实际读取到的字节数, 如果返回0, 表示已到达文件尾或是无可读取的数据。若参数count 为0, 则read()不会有作用并返回0。
	if(-1 == nread)
    {
	  if(errno == EINTR)//忽略掉中断信号
        continue;  
      return EXIT_FAILURE;
	}else if(0 == nread){  //为End Of File的缩写，在操作系统中表示资料源无更多的资料可读取。
	  break;
	}
    pbuf += nread;  //内存地址
    nleft -= nread;
  } 
  return (count - nleft);
}

size_t SocketIO::writen(const char* buf,size_t count)
{
 size_t nleft =count; 
 const char* pbuf = buf; 
 while(nleft > 0 )
 {
   int nwrite = ::write(sockfd_, pbuf, nleft); 
   if(nwrite == -1)
   {
     if(errno == EINTR)
       continue;
     return EXIT_FAILURE;  
   }
   nleft -= nwrite;
   pbuf += nwrite;
 }
 return (count - nleft);
}	

size_t SocketIO::recv_peek(char* buf, size_t count)
{
  // recv_peek - 仅仅查看套接字缓冲区数据，但不移除数据
  int nread;
  do{
        nread = ::recv(sockfd_, buf, count, MSG_PEEK);  //MSG_PEEK 查看当前数据。数据将被复制到缓冲区中，但并不从输入队列中删除。
  }while(nread == -1 && errno == EINTR); 
  return nread;
}

size_t SocketIO::readline(char* buf, size_t maxlen)
{
  size_t nleft = maxlen -1;
  char* pbuf = buf;
  size_t total =0;
  while(nleft >0)
  {
     size_t nread = recv_peek(pbuf, nleft); 
     if(nread <= 0) 
     return nread;
     //检查'\n'     
     for(size_t idx =0; idx != nread; ++idx )
	 {  
	   if(pbuf[idx] == '\n')  
	   {
	      size_t nsize = idx +1;
	      if(readn(pbuf, nsize)!= nsize )
			return EXIT_FAILURE; 	  
          pbuf += nsize;
          total += nsize;
		  *pbuf = 0; //重置
	      return total;
	   }
	 }
     if(readn(pbuf, nread) != nread) 
         return EXIT_FAILURE; 
     pbuf += nread;
     nleft -= nread;
     total += nread;
  }
  *pbuf = 0 ;
  return maxlen-1;
}

}//end of namespace wd



