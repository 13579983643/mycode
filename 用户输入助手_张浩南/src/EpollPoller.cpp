#include"../include/EpollPoller.h"
#include"../include/SocketUtil.h"
#include"../include/Acceptor.h"
#include<assert.h>
#include<iostream>
using::std::cout;
using::std::endl;

namespace wd
{
EpollPoller::EpollPoller(Acceptor & acceptor)//对I/O多路复用的封装，类acceptor中包含 listenSock_ ,  InetAddress 
:acceptor_(acceptor)  
,epollfd_(createEpollFd())
,eventfd_(createEventFd() )//来实现多进程或多线程的之间的事件通知的。调用 exec 后会自动关闭文件描述符
,listenfd_(acceptor_.fd())
,isLooping_(false)
,eventsList_(1024)
{
		  addEpollFdRead(epollfd_, listenfd_); //先注册要监听的时间类型 epoll_ctl()
		  addEpollFdRead(epollfd_,eventfd_ );  
}

EpollPoller::~EpollPoller()
{
   ::close(epollfd_);
}	

void EpollPoller::loop()
{
  isLooping_ = true;
  while(isLooping_ )
  {
     waitEpollfd();
  }
}	


void EpollPoller::unloop()
{
   if(isLooping_)
       isLooping_  = false;
}

void EpollPoller::setConnectionCallback(EpollCallback cb  ) 
{
  onConnectionCb_ = cb;
}	

void EpollPoller::setMessageCallback(EpollCallback cb)  
{
    onMessageCb_ = cb;
}

void EpollPoller::setCloseCallback(EpollCallback cb)  
{
   onCloseCb_ = cb;
}

void EpollPoller::waitEpollfd()
{
  int nready;
  do{
    nready = ::epoll_wait(epollfd_, &(*eventsList_.begin()),eventsList_.size(), 5000 ); 
  }while(nready == -1 && errno == EINTR);
   //该函数返回需要处理的事件数目 
  if(nready == -1){
     perror("epoll_wait error");  
     exit(EXIT_FAILURE);
  }else if(nready == 0){
     //printf("epoll_wait timeout\n"); 
  }else{
     //做一个扩容操作
     if(nready == static_cast<int>(eventsList_.size() )){
	    eventsList_.resize(eventsList_.size() *2);  
	 }	 
     //遍历每一个激活的文件描述符
     for(int idx =0; idx != nready; ++idx)
	 {
	   // typedef std::vector<struct epoll_event> EventList; 
       if(eventsList_[idx].data.fd == listenfd_ ){
		 //连接到达；有数据来临； 
         if(eventsList_[idx].events & EPOLLIN )
         {
		   handleConnection();
		 } 
	   }else if(eventsList_[idx].data.fd == eventfd_ ){
	       handleRead();
	       cout << "> doPendingFunctors()" << endl;
	       doPendingFunctors();
	   }else{  
	       if(eventsList_[idx].events & EPOLLIN)
	       {
		      handleMessage(eventsList_[idx].data.fd);
		   } 
	   } 
	 }//end for
  }//end else
}	

void EpollPoller::handleConnection()
{
  int peerfd = acceptor_.accept();
  addEpollFdRead(epollfd_, peerfd);
  TcpConnectionPtr conn(new TcpConnection(peerfd, this));//typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
  //conn->send("welcome to server.\n");
  conn->setConnectionCallback(onConnectionCb_); 
  conn->setMessageCallback(onMessageCb_ );  
  conn->setCloseCallback(onCloseCb_);  //??
   
  std::pair<ConnectionMap::iterator,bool> ret; 
  ret = connMap_.insert(std::make_pair(peerfd,conn));
  assert(ret.second == true); //??
  (void)ret;
  //connMap_[peerfd] = conn; 
  conn->handleConnectionCallback();  
}	
void EpollPoller::handleMessage(int peerfd)
{
  bool isClosed = isConnectionClosed(peerfd); //通过预览数据，判断
  ConnectionMap::iterator it = connMap_.find(peerfd);
  assert(it != connMap_.end()); 
   
  if(isClosed){
      it->second->handleCloseCallback(); 
      delEpollReadFd(epollfd_, peerfd);
	  connMap_.erase(it);   
  }else{
     it->second->handleMessageCallback(); 
  }
}	

//在计算线程中执行
void EpollPoller::runInLoop(const Functor && cb)//??仿函数？
{
        {
                MutexLockGuard mlg(_mutex);  //lock()
                _pendingFunctors.push_back(std::move(cb));//std::vector<Functor>
        }
        wakeup();//write()
}

void EpollPoller::doPendingFunctors()
{
        std::vector<Functor> tmp;

        {
                MutexLockGuard mlg(_mutex);
                tmp.swap(_pendingFunctors);
        cout <<  "Functor szie " << tmp.size() << endl;
    }
        for(auto & functor : tmp)
        {
                functor();
        }
    cout << "---------end--------------" << endl;

}

void EpollPoller::handleRead()
{
        uint64_t howmany;
        int ret = ::read(eventfd_, &howmany, sizeof(howmany));
        if(ret != sizeof(howmany))
        {
                perror("read error");
        }
}

void EpollPoller::wakeup()
{
        uint64_t one = 1;  //8字节     uint64_t unsigned long long
        int ret = ::write(eventfd_, &one, sizeof(one));
        if(ret != sizeof(one))
        {
                perror("write error");
        }
}


}//end of namespace wd










