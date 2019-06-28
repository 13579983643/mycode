
#include"trie.h"

Node* CreateNode(){
    Node* p = new Node;
    for(int i =0; i< MAX; ++i)
       p->next[i] =NULL;
	p->num =0;
	return p;
};


void Trie::read(const string &filename )
{
  ifstream store(filename); 
  string wat;
  while(store >> wat){
     string end;
     for(auto &c : wat){
	    if(!isalpha(c)){
	      continue;  //跳过当前循环中剩下的语句
	    }
        c = tolower(c);
        end += c;
     }
     if(end.size() < 2)
	  continue;
     Node *t, *p = root;
     for(auto alp : end){
	    int idx = alp - 'a'; //97~122英文字母表
	    if(p->next[idx] == NULL){  //??
	      t = CreateNode();
	      p->next[idx] =t;
        }else{
	      p = p->next[idx]; 
		}
	 }
     ++p->num;
  }
  store.close();
}

void PreOrderList(Node* head, const string &pre, const string &filename)
{
  if(!head) 
     return;
  string cur = pre;
  Node *t; 
  ofstream out(filename, ofstream::app); //以追加的方式打开文件
  if(head->num != 0){
    out << pre << " " << head->num << endl; //out
  }
  for(int i =0 ; i< MAX; ++i){
     if( head->next[i] != NULL ){
	   t = head->next[i];
	   char append = 'a' +i; 
       string nextcur = cur + append;
       PreOrderList(t, nextcur,filename);
	 }
  }
  out.close();
}	

void Trie::list(const string &filename){
   string sin;
   PreOrderList(root, sin, filename);
}

