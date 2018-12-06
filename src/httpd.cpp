#include <sys/socket.h>
#include <pthread.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include "HttpRequest.h"
#include "Response.h"
#define QUEUE 20

using namespace std;

string idx = "index.html";

struct files
{
	int client;
	const char* path;
};

void accept_request(int client);
void *cat(void* data);
void dog(int client, const char* path);
int recvline(int sock, char *buf, int size);
void getRequest(int client, string& buff);
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
	pthread_t thread;
	struct files fout;
	//string state_code = "404";
	//读取用户端发来的请求数据
	getRequest(client, buff);
	cout << buff << endl;
	HttpRequest hr(buff);
	//取请求方法
	method = hr.getMethod();
	//
	url = hr.getUri();
	//
	version = hr.getVersion();
	//
	map<string, vector<string>> headMap = hr.getHeaders();
	//不是GET请求就需要cgi	
	if(method == "GET")
		cgi = 1;
	else	
	{
		//有参数就请求cgi
		//if(hasQuery_string) cgi = 1;
	}
	//拼接到htdocs的后面	
	path += url;
	
	//假如path后是'/',需要寻找默认文件
	if(path[path.length() - 1] == '/')
		path += idx;
	const char* p = path.data();
	//没找到文件
	if(stat(p, &st) == -1)
	{
		Response response(client, "404");
		response.sendHttpHead();
		cout << "[-]i will close client!" <<endl;
	//关闭连接
	close(client);
	}
	else
	{	
		//如果是目录,尾部添加/index.html
		if((st.st_mode & S_IFMT) == S_IFDIR)
		{
			path = path + "/" + idx;
			p = path.data();
			if(stat(p, &st) == -1)
			{
				Response response(client, "404");
				response.sendHttpHead();
				cout << "[-]i will close client!" <<endl;
				//关闭连接
				close(client);
			}
		}
		//如果是可执行文件，需要cgi
		if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
			cgi = 1;
		if(!cgi)
		{
			// fout.client = client;
			// fout.path = p;
			// int ret_thrd = pthread_create(&thread, NULL, cat, (void*)&fout);
			// if(ret_thrd != 0)
			// 	print_error("[-]thread error!");
			dog(client, p);
		}
		else
		{
			//调用cgi	
			cout << "[-]cgi diao!" << endl;
			fout.client = client;
			fout.path = p;
			int ret_thrd = pthread_create(&thread, NULL, cat, (void*)&fout);
			if(ret_thrd != 0)
				print_error("[-]thread error!");
			//dog(client, p);
		}
	}
	
}


//发送文件
void dog(int client, const char* path)
{
	FILE* resource = NULL;
	string state_code = "404";
	resource = fopen(path, "rb");
	if(resource == NULL)
	{
		Response response(client, state_code);
		response.sendHttpHead();
	}
	else
	{
		fseek(resource,0,SEEK_END);
		long n = ftell(resource);	//获取文件长度
		fseek(resource,0,SEEK_SET);
		state_code = "200";
		Response response(client, state_code);
		response.sendHttpHead();
		response.sendContext(resource, n, "html");
	}	
	fclose(resource);
	cout << "[-]i will close client!" <<endl;
	//关闭连接
	close(client);
}

//发送文件
void *cat(void* data)
{
	cout << "[-]thread is sending" << endl;
	struct files *fout = (struct files*) data;
	FILE* resource = NULL;
	string state_code = "404";
	resource = fopen(fout->path, "rb");
	if(resource == NULL)
	{
		Response response(fout->client, state_code);
		response.sendHttpHead();
	}
	else
	{
		fseek(resource,0,SEEK_END);
		long n = ftell(resource);	//获取文件长度
		fseek(resource,0,SEEK_SET);
		state_code = "200";
		Response response(fout->client, state_code);
		response.sendHttpHead();
		response.sendContext(resource, n, "html");
	}	
	fclose(resource);
	cout << "[-]i will close client" << fout->client << "!" <<endl;
	//关闭连接
	close(fout->client);
}

//取得一行请求数据
int recvline(int sock, char *buff, int size)
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
				//cout << "[-]get r" << endl;
				buff[i++] = '\r';
				n = recv(sock, &c, 1, MSG_PEEK);   //MSG_PEEK检查套字
				if(n>0 && c=='\n') recv(sock, &c, 1, 0);
				else c = '\n';
			}
			buff[i++] = c;
		}
		else c = '\n';
	}	
	buff[i] = '\0';
	return i;
}

//取得请求信息
void getRequest(int client, string& buff)
{
	char linebuf[1024];
	int numline = recvline(client, linebuf, sizeof(linebuf)); 
	while(numline>2)
	{
		string str(linebuf);
		buff += str;
		numline = recvline(client, linebuf, sizeof(linebuf)); 
	}
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
	s_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //htonl() ??

	//绑定地址与socket,如果端口号为0，系统会分配临时端口号
	if(bind(ss, (struct sockaddr* )&s_sockaddr, sizeof(s_sockaddr)) == -1)
		print_error("[-]error: bind failed!");
	
	//如果bind后port仍然是0，则需要手动调用getsockname()获取端口号
	if(*port == 0)
	{
		//调用getsockname()给socket随机分配端口号
		socklen_t len;
		bzero(&len, sizeof(s_sockaddr));
		if(getsockname(ss, (struct sockaddr*)&s_sockaddr, &len) == -1)
			print_error("[-]error: getsockname() failed!");
		*port = ntohs(s_sockaddr.sin_port);
	}

	if(listen(ss, QUEUE) < 0) print_error("[-]error: listen failed!");
	
	return ss;
}


int main()
{
	int httpd = 0;		//套接字
	int client_sock = 0;
	struct sockaddr_in c_sockaddr;
	socklen_t c_sockaddr_len;
	bzero(&c_sockaddr_len, sizeof(c_sockaddr));
	u_short port = 8082;
	
	httpd = create_connect(&port);  //建立连接
	cout << "[+]success: httpd running on port " << port << endl;
	
	//等待用户连接
	while(1)
	{
		client_sock = accept(httpd, (struct sockaddr*)&c_sockaddr, &c_sockaddr_len);
		if(client_sock == -1)
			print_error("[-]error: accept error!");
		cout << "[+]success: accept a request!" << endl;
		accept_request(client_sock);
	}
	close(httpd);
	return 0;	
}
