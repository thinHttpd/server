#ifndef RESPONSE_H
#define RESPONSE_H
#include <stdio.h>
#include <iostream>
#include <socket.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;
class Response
{
    public:
        Response(int client,string state,const string version = "HTTP/1.1");//构造函数
        virtual ~Response();//初始化函数
        void sendHttpHead();//返回头部
        //void sendHttpState();//返回状态
        void sendContext(int client,FILE* file);//返回内容
    protected:
    private:
        int client;
        string state;
        string version;
};

#endif // RESPONSE_H
