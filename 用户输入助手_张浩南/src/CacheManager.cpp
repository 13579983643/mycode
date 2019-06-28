 ///
 /// CacheManager.cpp
 /// Created by stabey(stabeyzhou@gmail.com).
 /// 2018-07-13 22:24:32
 /// @Brief: 
 ///

#include "../include/CacheManager.h"

void CacheManager::initCache(){
    Cache<> mainCache;  //Cache模板  
    mainCache.readFromFile(_cacheDiskPath);
    _cacheList.push_back(mainCache);
    LogInfo("The main Cache was initialize.");
}

Cache<>& CacheManager::getCache(){
    //wd::MutexLockGuard mlg(_mutex);
    auto it = _Thread2Cache.find(pthread_self());  // map< pthread_t ,size_t >
    //用find函数来定位数据出现位置，它返回的一个迭代器，当数据出现时，它返回数据所在位置的迭代器，如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器，
	
	
	cout << "judge it" << endl;
    if(it == _Thread2Cache.cend()){//cend 用于测试迭代器是否超过了其范围的末尾,可以使用此成员函数替代 end() 成员函数，以保证返回值为 const_iterator。 它一般与 auto 类型推导关键字联合使用
        _Thread2Cache.insert(make_pair(pthread_self(), _cacheList.size()));//inux使用进程模拟线程，gettid 函数返回实际的进程ID（内核中的线程的ID）.pthread_self 函数返回 pthread_create创建线程时的ID（POSIX thread ID）
        Cache<> newCache;
        newCache.readFromFile(_cacheDiskPath);
        _cacheList.push_back(newCache);
        it = _Thread2Cache.find(pthread_self());
        LogWarn("A new Cache was mapped to a thread.");
    }
    return _cacheList[it->second];
}


void CacheManager::periodcUpdate(){
    int updated = 0;
    for(auto &c : _cacheList){// vector< Cache<> >
        c.setUpdateTag();     //isUpdating = true
        c.waitSet();
        updated += c._newCacheCnt;
        cout << c._newCacheCnt << endl;
    }
    cout << "----------Cache Update Start------" << endl;
    int total = updated < _cacheList[0]._size ? updated : _cacheList[0]._size;//新缓存数要比总缓存数要少
    cout << "total = " << total << endl;
    if(total == 0){
        LogWarn("No cache to update in this period.");
        cout << "No need to updated" << endl;
        cout << "------------Cache Updarte end -------------" << endl;
        return;
    }
    int ret = total;
    int seq = 0;
    while(total){
        for(auto &c : _cacheList){
            if(c._newCacheCnt == 0)
                continue;
            else{ 
                int in_seq = seq;
                Node<> *nodePending = c._head;
                while(in_seq--){
                    nodePending = nodePending->next;//去掉头节点
                    cout << nodePending << endl;
                }
                cout << nodePending->_query << " " << nodePending->_jsonfile << endl;
                string getFromMain = _cacheList[0].get(nodePending->_query);
                cout << getFromMain<< " " << getFromMain.size() <<  endl;
                if(getFromMain.size() == 0){
                    _cacheList[0].removeUpdateTag();//_isUpdating = false;
                    cout << "before set" << _cacheList[0]._cacheMap.size() << endl;
                    _cacheList[0].set(nodePending->_query, nodePending->_jsonfile);//加入新词
                    cout << "after set" << _cacheList[0]._cacheMap.size() << endl;
                    _cacheList[0].setUpdateTag();
                }
                --c._newCacheCnt;
                --total;
                if(total == 0)
                    break;
            }
        }
        ++seq;
    }
    for(auto &c : _cacheList){
        c._newCacheCnt = 0;
        c.removeUpdateTag();
    } 
    _cacheList[0].writeToFile(_cacheDiskPath);
    string logwarn = to_string(ret) + " cache on disk updated.";
    LogWarn(logwarn);
    cout << ret << "Cache ON Disk Updated" << endl;
    cout << "------------Cache Updarte end -------------" << endl;
}
