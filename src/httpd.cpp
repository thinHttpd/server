#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "HttpRequest.h"
#include "Response.h"
#define QUEUE 20

using namespace std;

string idx = "index.html";

void accept_request(int client);
void cat(int client, FILE* resource);
int recvline(int sock, char *buf, int size)
string getRequest(int client);
void print_error(const char* error_message);
int create_connect(u_short* port);

//请求处理
void accept_request(int client)
{
	string method;		//请求方法
	string buff;		//请求头
	string url;
	string version;
	int cgi = 0;
	string path = "htdocs";
	int content_length = 0;
	struct stat st;		//检测文件是否存在结构体

	//读取用户端发来的请求数据
	buff = getRequest(client);
	HttpRequest hr(buff);
	//取请求方法
	method = hr.getMethod();
	//
	url = hr.getUri();
	//
	version = hr.getVersion();
	//
	map<string, vector<string>> headMap = hr.getHeader();
	//不是GET请求就需要cgi	
	if(strcmp(method,"GET") != 0)
		cgi = 1;
	else	
	{
		//有参数就请求cgi
		//if(hasQuery_string) cgi = 1;
	}
	//拼接到htdocs的后面	
	sprintf(path, url);
	
	//假如path后是'/',需要寻找默认文件
	if(path[path.length() - 1] == '/')
		sprintf(path, "index.html");
	//没找到文件
	if(stat(path, &st) == -1)
	{
		Response response(client,"404");
		response.sendHttpHead();
	}
	else
	{	
		//如果是目录,尾部添加index.html
		if((st.st_mode & S_IFMT) == S_IFDIR)
		{
			sprintf(path, "/index.html");
			if(stat(path, &st) == -1)
			{
				Response response(client,"404");
				response.sendHttpHead();
			}
		}
		//如果是可执行文件，需要cgi
		if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
			cgi = 1;
		if(!cgi)
		{
			Response response(client, "200");
			response.sendHttpHead();
		}
		else //调用cgi	
	}
	//关闭连接
	close(client);
}


//读取文件
void cat(int client, FILE* resource)
{
	char buf[1024];
	fgets(buf, sizeof(buf), resource);
	send(client, buf, strlen(buf), 0);	
	while(!feof(resource))
	{
		fgets(buf, sizeof(buf), resource);
		send(client, buf, strlen(buf), 0);
	}
}

//取得一行请求数据
int recvline(int sock, char *buf, int size)
{
	int n, i = 0;
	char c;
	while((i < size - 1)&&(c != '\n'))
	{
		n = recv(sock, &c, 1, 0);
		if(n>0)
		{
			if(c == '\r')
			{
				buff[i++] = '\r';
				n = recv(sock, &c, 1, MSG_PEEK);   //MSG_PEEK检查套字
				if(n>0 && c=='\n') recv(sock, &c, 1, 0);
				else c = '\n';
			}
			buff[i++] = c;
		}
		else c = '\n';
	}	
	buf[i] = '\0';
	return i;
}

//取得请求信息
string getRequest(int client)
{
	string buf;
	char linebuf[1024];
	int numline = recv(client, *linebuf, sizeof(linebuf)); 
	while(numline>0)
	{
		string str(linebuf);
		strcat(buf, str);
		numline = recv(client, *linebuf, sizeof(linebuf)); 
	}
	return buf;
}

//错误处理
void print_error(const char* error_message)
{
	perror(error_message);
	exit(1);
}


//建立连接
int create_connect(u_short* port)
{
	int ss = socket(AF_INET, SOCK_STREAM, 0);	//建立套接字
	if(ss == -1) print_error("[-]error: socket can not to be created!");

	struct sockaddr_in s_sockaddr;			//套接字地址结构
	//memset(&s_sockaddr, 0, sizeof(s_sockaddr));
	s_sockaddr.sin_family = AF_INET;
	s_sockaddr.sin_port = htons(*port);		//htons() ??
	s_sockaddr.sin_adde.s_addr = htonl(INADDR_ANY); //htonl() ??

	//绑定地址与socket,如果端口号为0，系统会分配临时端口号
	if(bind(ss, (struct sockaddr* )&s_sockaddr, sizeof(s_sockaddr)) == -1)
		print_error("[-]error: bind failed!");
	
	//如果bind后port仍然是0，则需要手动调用getsockname()获取端口号
	if(*port == 0)
	{
		//调用getsockname()给socket随机分配端口号
		int len = sizeof(s_sockaddr);
		if(getsockname(ss, (struct sockaddr*)&s_sockaddr), &len) == -1)
			print_error("[-]error: getsockname() failed!");
		*port = ntohs(s_sockaddr.sin_port);
	}

	if(linten(ss, QUEUE) < 0) print_error("[-]error: listen failed!");
	
	return ss;
}


int main()
{
	int httpd = 0;		//套接字
	int client_sock = 0;
	struct sockaddr_in c_sockaddr;
	int c_sockaddr_len = sizeof(c_sockaddr);
	u_short port = 0;
	
	httpd = create_connect(&port);  //建立连接
	cout << "[+]success: httpd running on port " << port << endl;
	
	//等待用户连接
	while(1)
	{
		client_sock = accept(httpd, (struct sockaddr*)&c_sockaddr, &c_sockaddr_len);
		if(client_sock == -1)
			print_error("[-]error: accept error!");
		accept_request(client_sock);
	}
	close(httpd);
	return 0;	
}
