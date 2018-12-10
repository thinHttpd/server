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
#include "CGI.h"
#define QUEUE 20

using namespace std;

string idx = "index.html";

struct files
{
	int client;
	const char* path;
};

void accept_request(int client);
void cat(int client, const char* path, const string file_type);
int recvline(int sock, char *buf, int size);
void getRequest(int client, string& buff);
void getBody(int sock, int length, string& buff);
void print_error(const char* error_message);
int create_connect(u_short* port);

//请求处理
void accept_request(int client)
{
	string method;		//请求方法
	string buff;		//请求头
	string uri;
	string requesturi;
	string version;
	int cgi = 0;
	string path = "htdocs";
	int content_length = 0;
	struct stat st;		//检测文件是否存在结构体
	pthread_t thread;
	struct files fout;
	//string state_code = "404";
	string query_string = "";
	//读取用户端发来的请求数据
	getRequest(client, buff);
	cout << buff << endl;
	HttpRequest hr(buff);
	//取请求方法
	method = hr.getMethod();
	//
	uri = hr.getUri();
	requesturi = hr.getUri();
	//
	version = hr.getVersion();
	//
	map<string, vector<string>> headMap = hr.getHeaders();
	//不是GET请求就需要cgi	
	if(method != "GET")
		cgi = 1;
	else	
	{
		//有参数就请求cgi
		if(hr.hasSource())
		{
			cgi = 1;
			vector<string> tmp;
    		hr.splitString(uri,tmp,"?");
    		uri = tmp[0];
    		query_string = tmp[1];
			//get query_string = after ? of uri
			//uri = before ? of uri
			//requesturi = hr.getUri()
		}
	}
	//拼接到htdocs的后面	
	path += uri;
	
	//假如path后是'/',需要寻找默认文件
	if(path[path.length() - 1] == '/')
		path += idx;
	const char* p = path.data();
	//没找到文件
	if(stat(p, &st) == -1)
	{
		Response response(client, "404");
		response.sendHttpHead();
		cout << "[-] 404 Not Found!" <<endl;
		cout << "[-] I will close client!" <<endl;
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
				cout << "[-] 404 Not Found index" <<endl;
				cout << "[-] I will close client!" <<endl;
				//关闭连接
				close(client);
			}
		}
		string file_type = hr.getSource();
		//如果是可执行文件，需要cgi
		if((file_type == "php") || (file_type == "py") || (file_type == "jar"))
			cgi = 1;
		
		if(!cgi)
		{
			cat(client, p, file_type);
		}
		else
		{
			// //调用cgi	
			cout << "[+]use cgi" << endl;
			//cat(client, p, file_type);
			int body_length = 0;
			string body_content = "";
			if(method == "GET")
			{

			}
			else
			{
				map<string,vector<string>>::iterator iter;
				iter = headMap.find("content-length");
				body_length = stoi((iter->second)[0], nullptr);
				//cout << body_length << endl;
				getBody(client, body_length, body_content);
			}
			path = "./" + path;
			cout << path <<endl;
			cout << body_length << ":::::" << body_content <<endl;
			cout << query_string << endl;
			CGI *u_cgi = new CGI(path,requesturi,query_string);
			u_cgi->run(method, body_content, body_length);
  		    cout<< u_cgi->getStatusCode() <<endl;
  		    cout<< u_cgi->getOutput() <<endl;
  		    //delete(u_cgi);
  		    close(client);
		}
	}
}


//发送文件
void cat(int client, const char* path, const string file_type)
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
		response.sendContext(resource, n, file_type);
	}	
	fclose(resource);
	//关闭连接
	cout << "[*]" << client << endl;
	close(client);
	cout << "[-] I will close client!" <<endl;
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

void getBody(int sock, int length, string& buff)
{
	char *body_buf = new char[length+1];
	char c;
	for(int i=0;i<length;i++)
	{
		recv(sock, &c, 1, 0);
		body_buf[i] = c;
	}
	string str(body_buf);
	buff += str;
	delete[] body_buf;
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
			print_error("[-] error: accept error!");
		cout << "[+] Success: accept a request!" << endl;

		// 开了多线程无法通过测试，无法自动断开连接，但是开了 CGI 速度上通过科学的“推测”将会快！

		// int pid = fork();
		// if (pid == 0)
		// {
			accept_request(client_sock);
			cout<<"[-] Exiting thread.."<<endl;
			//exit(0);
			cout<<"[-] Exit thread failed"<<endl;
		// }
		// else {
		// 	printf("[+] Create a new thread for %d \n",client_sock);
		// }

		
	}
	close(httpd);
	return 0;	
}
