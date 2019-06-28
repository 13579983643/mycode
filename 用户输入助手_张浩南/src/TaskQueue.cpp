#include"../include/TaskQueue.h"
using namespace wd;

//生产者所在的线程
void TaskQueue::push(Task* t)//Task 任务是什么??
{
  MutexLockGuard autoLock(_mutex);//自动调用析构函数，
  while( full() ) //_que.size() == _queSize ;
  {
     _notFull.wait();//Condition类_notFull 中 pthread_cond_wait()为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起
  }
  _que.push(t);   //std::queue<Task*> _que;
  _notEmpty.notify(); //pthread_cond_broadcast(&_cond); //???
}

//消费者所在的线程
Task* TaskQueue::pop()
{
  MutexLockGuard autoLock(_mutex); 
  while(_flag && empty())
  {
     _notEmpty.wait();//队列为空，在此处阻塞 
  }
  if(_flag){
    Task* t = _que.front();
    _que.pop();
	_notFull.notify();
    return t; 
  }else
     return NULL;
}






