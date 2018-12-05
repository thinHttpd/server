#ifndef RESPONSE_H
#define RESPONSE_H
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;
class Response
{
    public:
        Response(int client,string state,const string version = "HTTP/1.1");//���캯��
        virtual ~Response();//��ʼ������
        void sendHttpHead();//����ͷ��
        //void sendHttpState();//����״̬
        void sendContext(FILE* file);//��������
    protected:
    private:
        int client;
        string state;
        string version;
};

#endif // RESPONSE_H
