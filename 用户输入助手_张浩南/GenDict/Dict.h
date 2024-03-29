#ifndef __DICT_H__
#define __DICT_H__

#include<iostream>
#include"trie.h"
#include<string>
#include"map"
//#include"include/cppjieba/Jieba.hpp"
using namespace std;

const char* const DICT_PATH = "/home/iwnl/Desktop/SpellCorrection/GenDict/dict/jieba.dict.utf8";
const char* const HMM_PATH = "/home/iwnl/Desktop/SpellCorrection/GenDict/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "/home/iwnl/Desktop/SpellCorrection/GenDict/dict/user.dict.utf8";
const char* const IDF_PATH = "/home/iwnl/Desktop/SpellCorrection/GenDict/dict/idf.utf8";
const char* const STOP_WORD_PATH = "/home/iwnl/Desktop/SpellCorrection/GenDict/dict/stop_words.utf8";

class Dict{
		public:
				Dict(const string &lib,const string& data)
						:_libPath(lib),_outPath(data){ }

				void genCn();
				void genEn();
				void output();

		private:
				map<string, int>  _cnDict;
				Trie _enDict;
				string _libPath; 
				string _outPath;
};

#endif
