#include"../include/QueryTask.h"
#include"../include/Mylog.h" 
#include<fstream>

MyDict* MyDict::_Instance = NULL;//字典
  
void MyDict::init(){
    initDict();
    initIndex();
    LogInfo("Dict Initialize.");
} 

void MyDict::initDict(){
    string words;
    int freq;
{//enDict

    ifstream enDict(_enPath);
    while(enDict >> words >> freq){
        _dict.push_back(pair<string, int>(words, freq));
    }
}

{//cnDict
    ifstream cnDict(_cnPath);
    while(cnDict >> words >> freq){
        _dict.push_back(pair<string, int>(words, freq));
    }
}
}

void MyDict::initIndex(){
    int num = 0;
    for(auto c : _dict){
        string one = c.first;
        if(isUTF8(one)){
            for(string::size_type col = 0; col < one.size(); col+=3){
                string sing = one.substr(col, 3);
                _index[sing].insert(num);
            }
        }
        else{
            for(string::size_type col = 0; col < one.size(); ++col){
                string sing = one.substr(col, 1);
                _index[sing].insert(num);
            }
        }
        ++num;
    }
} 

void MyDict::list(){
    auto map_it = _index.cbegin();
    while(map_it != _index.cend()  ){
	    cout << map_it->first << ":";
	    for(auto c: map_it->second ){
		    cout << " "<<_dict[c].first;  
		}
	    cout << endl;
		++map_it;
	}   
}




