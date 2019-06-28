#include"../include/Threadpool.h"
#include"../include/WorkerThread.h"
#include<unistd.h>
using namespace wd;

void Threadpool::start()
{
  for(size_t idx = 0; idx <_threadNum; ++idx)
  {
    shared_ptr<Thread> pThread(new WorkerThread(*this));
    _threads.push_back(std::move(pThread));  // vector<shared_ptr<Thread> >  move()   std::move是将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存的搬迁或者内存拷贝。
  }
  for(auto & pThread: _threads)
  {
    pThread->start();//create()
  }
}


//运行在主线程中
void Threadpool::stop()
{
   if(!_isExit )
   {
     while(!_taskQue.empty()){//生产者消费者
	    ::sleep(1);
	    cout << ">> threadpool sleep 1秒" << endl;
	 }  
     _isExit = true;//默认初始为false
     cout << ">> Threadpool->stop: _isExit =" << _isExit << endl;
      
     _taskQue.wakeup();
      
     for(auto & pthread : _threads)    
     {
	    pthread->join();
	 }   
   } 
}	

Threadpool::~Threadpool()
{
   if(!_isExit){
        stop();
   }
}

void Threadpool::addTask(Task* ptask)
{
   _taskQue.push(ptask); 
}

Task* Threadpool::getTask()
{
   return _taskQue.pop();
}

//子线程要做的事情

void Threadpool::threadFunc()
{
   while(!_isExit ){
      Task* ptask = getTask(); 
      if(ptask){
	     ptask->process();//执行任务的时间不确定 
	     delete ptask;  //delete??
	  }
   }
}










