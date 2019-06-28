#include"../include/MutexLock.h"
#include<iostream>
using std::cout;
using std::endl;


namespace wd
{
 MutexLock::MutexLock() 
 :_isLocking(false)
 { 
    if(pthread_mutex_init(&_mutex,NULL))//pthread_mutexattr_init()函数成功完成之后会返回零，其他任何返回值都表示出现了错误。
    {
	  cout << "error:  create mutex failed!" << endl;
	}
 }

 MutexLock::~MutexLock()
 {
    if(pthread_mutex_destroy(&_mutex))//互斥锁销毁函数在执行成功后返回 0，否则返回错误码。
	{
	   cout << "error : destory mutex failed!" << endl;
	}
 }	 


void MutexLock::lock()
{
  pthread_mutex_lock(&_mutex); 
  _isLocking = true; 
}

void MutexLock::unlock()
{
  pthread_mutex_unlock(&_mutex); 
  _isLocking = false;
}	

bool MutexLock::isLocking() const 
{
   return _isLocking;
}

pthread_mutex_t* MutexLock::getMutexLockPtr() //互斥变量使用特定的数据类型：pthread_mutex_t
{
   return &_mutex;
}	

}// end of namespace
















