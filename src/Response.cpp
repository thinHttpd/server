#include "Response.h"
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

void noFound(int client , string version , string state);//404�����Ҳ�����Դ�ļ�
void msgSend(int client , char* _buf , string _msg);//������ϢС����


Response::Response(int _client,string _state,string _version)
{
    client = _client;
    state = _state;
    version = _version;
    //ctor
}

Response::~Response()
{
    //dtor
}

/** @brief sendHttpState
  *
  * @todo: document this function
  */
/*void Response::sendHttpState()
{

}*/

/** @brief sendHttpHead
  *
  * @todo: document this function
  */
void Response::sendHttpHead()
{
    //404,200
    char buf[1024];
    if(state == "404")
    {
       noFound(client,version,state);
    }
    else if(state == "200")
    {
        string msg = version + " " + state + " " +"OKKK\r\n";
        sprintf(buf,msg.c_str());
        send(client,buf,sizeof(buf),0);
        sprintf(buf,"Content-type:text/html\r\n");
        send(client,buf,sizeof(buf),0);
        sprintf(buf,"\r\n");
        send(client,buf,sizeof(buf),0);
    }

}

/** @brief sendContext
  *
  * @todo: document this function
  */
void Response::sendContext(FILE* file)
{
    char buf[1024];

    //���ļ��������ж�ȡָ������
    fgets(buf,sizeof(buf),file);

    while(!feof(file))
    {
        send(client,buf,strlen(buf),0);
        fgets(buf,sizeof(buf),file);
    }
}

void noFound(int client , string version , string state)//404�����Ҳ�����Դ�ļ�
{
    char buf[1024];
    string msg = version + " " + state + " No Found\r\n";
    sprintf(buf,msg.c_str());
    send(client,buf,strlen(msg.c_str()),0);
    msg = "Content-type:text/html\r\n";
    sprintf(buf,msg.c_str());
    send(client,buf,strlen(msg.c_str()),0);
    msgSend(client,buf,"\r\n");
//    sprintf(buf,"\r\n");
//    send(client,buf,sizeof(buf),0);
//    msg = "<P>�Ҳ�����ԴŶ ";
//    sprintf(buf,msg.c_str());
//    send(client,buf,strlen(msg.c_str()),0);
    msgSend(client, buf,"<P>�Ҳ�����ԴŶ");
}

void OK()


    void msgSend(int client, char* buf , string msg)//������ϢС����
{
    sprintf(buf,msg.c_str());
    send(client,buf,strlen(msg.c_str()),0);
}
