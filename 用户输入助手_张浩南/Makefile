INC_DIR:= ./include
SRC_DIR:= ./src
SRCS:= $(wildcard $(SRC_DIR)/*.cpp)    #搜索当前目录下的文件名
OBJS:= $(patsubst %.cpp, %.o, $(SRCS)) #把SOURCES中'.cpp'替换为'.o'
LIBS:= -lpthread -ljson -llog4cpp

CXX:=g++

CXXFLAGS:= -w -g -std=c++11 $(addprefix -I, $(INC_DIR)) $(LIBS) -Wno-deprecated
#-w 如果make在执行过程中改变目录，则打印当前目录名 -g 打开调试选项
# $(addprefix fixstring,string1 string2 ...)  fixstring表示任意要添加的固定前缀，在逗号的后面可以是一个或多个要添加前缀的子字符串，多个子字符串之间用空格隔开，当然，你也可以像前面的例子一样用变量。
# 用来关闭Xcode中关于废弃API的警告  -Wno-deprecated
EXE:=./bin/server.exe

$(EXE):$(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(CXXFLAGS)

clean:
	rm -rf $(EXE)
	rm -rf $(OBJS)
