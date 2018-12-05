﻿#include "Response.h"
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;
Response::Response(int _client, string _state, const string _version)
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
      string msg = version +" "+ state +" No Found\r\n";
      sprintf(buf,msg.c_str());
      send(client,buf,strlen(buf),0);
      sprintf(buf,"Content-type: text/html\r\n");
      send(client,buf,strlen(buf),0);
      sprintf(buf,"Connection: keep-alive\r\n");
      send(client,buf,strlen(buf),0);
      sprintf(buf,"\r\n");
      send(client,buf,strlen(buf),0);
      sprintf(buf, "<p> 404 Not Found </p> <br> <small>thinHttpd</small>");
      send(client, buf, strlen(buf), 0);
  }
  else if(state == "200")
  {
      string msg = version + state +"OKKK\r\n";
      sprintf(buf,msg.c_str());
      send(client,buf,strlen(buf),0);
      sprintf(buf,"Content-type:text/html\r\n");
      send(client,buf,strlen(buf),0);
      sprintf(buf,"Connection: keep-alive\r\n");
      send(client,buf,strlen(buf),0);
      sprintf(buf,"\r\n");
      send(client,buf,strlen(buf),0);
  }
}

/** @brief sendContext
  *
  * @todo: document this function
  */
void Response::sendContext(int client, FILE* file)
{
  char buf[1024];

  //从文件描述符中读取指定内容
  fgets(buf, strlen(buf), file);

  while (!feof(file))
  {
    send(client, buf, strlen(buf), 0);
    fgets(buf, strlen(buf), file);
  }
}


