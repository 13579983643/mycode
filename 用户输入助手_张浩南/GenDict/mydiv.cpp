#include"Dict.h"

int main(){
   string libpath = "./lib/";				
   string outpath ="../data/" ;  
   Dict myDict(libpath,outpath);
   myDict.genEn();  			 
   myDict.genCn(); 
   myDict.output();
   cout << "Both dictionaries built successfully" << endl;
}












