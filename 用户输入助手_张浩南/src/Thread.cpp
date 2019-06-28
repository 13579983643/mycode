#include"../include/Thread.h"
#include<iostream>

using std::cout;
using std::endl;
using namespace wd; 

Thread::Thread()
:_pthid(0)   //pthread_t 
,_isRunning(false)  //bool
{}


void Thread::start()
{
  pthread_create(&_pthid ,NULL ,threadFunc, this);
  _isRunning = true;
}

void * Thread::threadFunc(void* arg)
{
  Thread*pthread = static_cast<Thread*>(arg); //static_cast还可以在两个类对象之间进行转换，比如把类型为A的对象a，转换为类型为B的对象
 if(pthread) 
   pthread->run();  //执行任务， 为用户界面线程提供默认的消息循环。
 return NULL;
}

void Thread::join()
{
  pthread_join(_pthid,NULL );
  _isRunning = false; 
}

Thread::~Thread()
{
   if(_isRunning)
   {
      pthread_detach(_pthid); //线程被创建成可结合的。为了避免存储器泄漏，每个可结合线程都应该要么被显示地回收，即调用pthread_join；要么通过调用pthread_detach函数被分离。
      _isRunning = false;
   }
}	






