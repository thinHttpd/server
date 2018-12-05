#include "Response.h"
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

void msgSend(int client , char* _buf , string _msg,int length = 0);//发送信息小函数
void noFound(int client , string version , string state);//404错误，找不到资源文件
void ok(int client , string version , string state);//200，正常返回信息
void inetServerError(int client , string version , string state);//最常见的服务器端错误

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
void Response::sendHttpHead()//返回头部
{
    //404,200
    char buf[1024];
    if(state == "404")
    {
       noFound(client,version,state);
    }
    else if(state == "200")
    {
        ok(client,version,state);
    }
    else if(state == "500")
    {
        inetServerError(client,version,state);
    }

}

/** @brief sendContext
  *
  * @todo: document this function
  */
void Response::sendContext(FILE* file , long length , string type)//从文件描述符中读取指定内容
{
    char buf[409600];
    msgSend(client,buf,"Connection: keep-alive\r\n");
    if(type.compare("html")==0)//HTML格式
    {
        msgSend(client,buf,"Content-type: text/html; charset=utf-8\r\n");
    }
    else if(type.compare("plain")==0)//纯文本格式
    {
        msgSend(client,buf,"Content-type: text/plain; charset=utf-8\r\n");
    }
    else if(type.compare("xml")==0)//XML格式
    {
        msgSend(client,buf,"Content-type: text/xml; charset=utf-8\r\n");
    }
    else if(type.compare("gif")==0)//gif图片格式
    {
        msgSend(client,buf,"Content-type: image/gif\r\n");
    }
    else if(type.compare("jpeg")==0)//jpg图片格式
    {
        msgSend(client,buf,"Content-type: image/jpeg\r\n");
    }
    else if(type.compare("png")==0)//png图片格式
    {
        msgSend(client,buf,"Content-type: image/png\r\n");
    }

    msgSend(client,buf,"Content-Length: %ld\r\n",length);
    msgSend(client,buf,"\r\n");

    while(length>0)
    {
        int i = 0;
        for(i=0;i<409600||length>0;i++)
        {
            buf[i] = fgetc(file);
            length--;
        }
        send(client,buf,(i+1),0);
    }

}

void noFound(int client , string version , string state)//404错误，找不到资源文件
{
    char buf[1024];
    msgSend(client,buf,"Connection: keep-alive\r\n");
    string msg = version + " " + state + " No Found\r\n";
    msgSend(client, buf, msg);
    msgSend(client,buf,"Content-type:text/html; charset=utf-8\r\n");
    msgSend(client,buf,"\r\n");
//    sprintf(buf,"\r\n");
//    send(client,buf,sizeof(buf),0);
//    msg = "<P>找不到资源哦 ";
//    sprintf(buf,msg.c_str());
//    send(client,buf,strlen(msg.c_str()),0);
    msgSend(client, buf,"<P>找不到资源哦");
}

void ok(int client , string version , string state)//200，正常返回信息
{
    char buf[1024];
    string msg = version + " " + state + " " +"OKKK\r\n";
    msgSend(client,buf,msg);
}

void inetServerError(int client , string version , string state)//最常见的服务器端错误
{
    char buf[1024];
    msgSend(client,buf,"Connection: keep-alive\r\n");
    string msg = version + " " + state + " " +"Internal Server Error\r\n";
    msgSend(client,buf,msg);
    msgSend(client,buf,"Content-type:text/html; charset=utf-8\r\n");
    msgSend(client,buf,"\r\n");
}

    void msgSend(int client, char* buf , string msg,int length)//发送信息小函数
{
    sprintf(buf,msg.c_str());
    send(client,buf,strlen(msg.c_str()),length);
}
