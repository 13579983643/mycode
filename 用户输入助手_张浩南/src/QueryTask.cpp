#include"../include/QueryTask.h"
//typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
void QueryTask::process(){
		if(_queryWord.size() == 0){ 
				string nonsense = "what do you mean.";
				_conn->sendInLoop(nonsense); //真TM谬论; 胡扯; 胡言乱语; 愚蠢的行为; 冒失; 不可接受的行为; 毫无意义的话; 没有意义的文章;
				return;
		}
		cout << "----------" << pthread_self() << "---------"<< endl;
		cout << "Task Processing Target:" << _queryWord << "size = " << _queryWord.size() << "sockfd=" << _conn->getSockfd() << endl;
		Cache<> * curCache = &_cacheMgr.getCache();
		string res; 
		cout << "before get" << endl;  
		res = curCache->get(_queryWord);
		if(res.size() > 0) {
				cout << "find through Cache"  << endl;
				_conn->sendInLoop(res);
				return;
		}
		//wd::MutexLockGuard mas(_mutex);
		string actualWord;
		if(isUTF8(_queryWord)){
				for(string::size_type col = 0; col < _queryWord.size(); col+=3){
						string sing = _queryWord.substr(col, 3);
						queryIndex(sing);
						//cout << sing << " : " << _conn->getSockfd() << " " << pthread_self() << " |||";
						actualWord += sing;
				}
		}
		else{
				for(string::size_type col = 0; col <_queryWord.size(); ++col){
						string sing = _queryWord.substr(col, 1);
						queryIndex(sing);
						actualWord += sing;
				}
		}
		cout << endl;
        cout << "resquesize = " << _resultque.size() << " " << endl; 
        res = toJson(/*actualWord*/);
        cout << actualWord  << " = " << res << endl;
        _conn->sendInLoop(res);  //判断是否处于当前IO线程，是则执行这个函数，如果不是则将函数加入队列。  
        curCache->set(_queryWord,res );
        
		cout << "Cache UPdated" << endl;   
}

void QueryTask::queryIndex(const  string& sin ){
    auto head = _index[sin]; //新的auto：变量的自动类型推断。
    //for(auto c:s)  对于s中的每个字符
    for(auto c : head){
	    string word = _dict[c].first;  //vector< pair< string, int> > &_dict
        if(_diff.find(word) != _diff.end())//set<string> _diff
            continue;
        int freq = _dict[c].second; 
        int dist = getDis(word);
        if(dist >= 5)
            continue;
        _diff.insert(word);
        MyResult instance{word, freq, dist};
        _resultque.push(instance);	
	} 
    cout << sin << ":" << _conn->getSockfd() << " " << pthread_self() << " "<< _resultque.size() << _queryWord.substr(0, _queryWord.size() ) << "|||" << endl;     

} 

int QueryTask::getDis(const string & rhs  )
{
    vector<string> s1(Str2Vec(_queryWord)), s2(Str2Vec(rhs));//string _queryWord; Str2Vec return vector<string> ret
    int len1 = s1.size(); 
    int len2 = s2.size();
    int **d = new int* [len1 + 1];
    for(int i = 0; i <= len1; i++)
        d[i] = new int[len2 + 1];
    int i,j;
    for(i = 0; i <= len1; i++)
        d[i][0] = i;
    for(j = 0; j <= len2; j++)
        d[0][j] = j;
    for(i = 1; i <= len1; i++)
    {
            for(j = 1; j <= len2; j++)
            {
                int cost= s1[i - 1] == s2[j - 1] ? 0 : 1;
                int deletion = d[i - 1][j] + 1;
            int insertion = d[i][j - 1] + 1;
            int substitution = d[i - 1][j - 1] + cost;
            d[i][j] = min(deletion, insertion, substitution);
        }
    }
    int ret = d[len1][len2];
    for(int i = 0; i <= len1; i++)
    {
        delete[] d[i];
    }
    delete[] d;
    return ret;
}	

void  QueryTask::listResult(){
   cout << "For QueryWord\"" << _queryWord << "\",we find the following  lines:" << endl;
   size_t total = _resultque.size() >= _num ? _num : _resultque.size();  
   //priority_queue<MyResult, vector<MyResult>, MyCompare> _resultque;
   while(total){
      MyResult pend = _resultque.top();
      cout << pend._word << " " << pend._freq << " " << pend._dist << endl;
      _resultque.pop(); 
      --total; 
   } 
}

string QueryTask::toJson(/*const string &rhs*/){
   Json::Value root;  //Json::ValueType 枚举值； Value类的对象代表一个JSON值，既可以代表一个文档，也可以代表文档中一个值。如同JSON中定义的“值”一样，Value是递归的>
   Json::FastWriter writer;//在此我们使用 Json::Writer 的子类(派生类)：Json::FastWriter、Json::StyledWriter、Json::StyledStreamWriter。顾名思义，用Json::FastWriter 来处理 json 应该是最快的；负责将内存中的Value对象转换成JSON文档，输出到文件或者是字符串中>     
   Json::Value word;
   size_t total = _resultque.size() >= _num ? _num : _resultque.size();
   while(total){
        MyResult pend = _resultque.top();
        _resultque.pop();
        word = pend._word;
        root.append(word);
        --total;
    }

    string json_file = writer.write(root);

    return json_file;
} 














