# Thinhttpd

Thinhttpd的流程大概是启动服务器进行监听端口，接受请求，处理请求，最后返回结果。

## 第一步 启动服务器

先要为服务器设置socket，然后建立服务器socket地址（内含端口号），端口号是由用户在GUI处传入。

```c++
int httpd = 0;						//套接字
int client_sock = 0;				//客户套接字
struct sockaddr_in c_sockaddr;		//客户套接字地址
u_short port = 8082;				//端口号
```

启动连接，监听端口

```c++
httpd = create_connect(&port);  	//建立连接
```

建立连接的内部实现  create_connect(u_short* port）

### 1.建立套接字和套接字地址结构

```c++
int ss = socket(AF_INET, SOCK_STREAM, 0);	//建立套接字

struct sockaddr_in s_sockaddr;	
```

### 2.绑定地址和socket

```c++
bind(ss, (struct sockaddr* )&s_sockaddr, sizeof(s_sockaddr)；
```

### 3.开始监听

```c++
listen(ss, QUEUE)；
```

## 第二步 接受请求

当接受到请求，我们会为每个客户生成唯一的socket，防止信息误发。

```c++
client_sock = accept(httpd, (struct sockaddr*)&c_sockaddr, &c_sockaddr_len)；
```

我们运用了多线程来处理请求，每当请求到来就会生成新线程处理直至完成，其中系统还会继续接受新的请求。

```c++
int pid = fork();
if (pid == 0)
{
	accept_request(client_sock);
	exit(0);
}
```

好处是可以并发地发送客户访问的内容。

例如用户访问的html里有多张图片，不用并发的话，将会是先获得一张图片的请求，然后阻塞到这张图片发送完毕再接受下一张图片，这样的利用效率太低而且客户会等待很久。

而并发，服务器会同时接受多个图片请求，并一同处理并返回。

处理请求前先要获取请求头，用 '\r\n' 进行分割每一行，把整个请求头都拿出来，请求体会先放着以待CGI判断。

```c++
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
```

其中recvline就是取请求的每一行，当遇到空行就会停止，也就是把请求头和体分割。



## 第三步 处理请求

### 1.我们拿到请求头后，会生成一个HttpResquest对象对头进行处理

```c++
HttpRequest hr(buff);
//取请求方法
method = hr.getMethod();
//取请求uri
uri = hr.getUri();
requesturi = hr.getUri();
//取请求版本
version = hr.getVersion();
//取各个请求头部
map<string, vector<string>> headMap = hr.getHeaders();
```

### 2.先按请求方法来判断是否调用cgi

除了GET方法都要运行cgi

```c++
if(method != "GET")
		cgi = 1;
```

如果GET后带有请求参数，也会调用cgi

```c++
if(hr.hasSource())
{
	cgi = 1;
}
```

### 3.拼接访问路径

web服务的根目录会由GUI传入，例如：

```c++
path = ”htdocs“;
//拼接路径
path += uri;
```

如果访问的是目录，则会在后面添加index.html

```c++
path += idx;	//idx = “index.html”
```

### 4.判断文件是否存在

如果文件不存在，直接生成Response的对象进行返回404页面

（Response是专门用来返回结果给用户的类）stat是用来获取指定路径的文件或者文件夹的信息

```c++
if(stat(p, &st) == -1)
{
	Response response(client, "404");
	response.sendHttpHead();
	//关闭连接
	close(client);
}
```

### 5.文件如果存在，就会按照文件类型再次判断是否调用cgi

目前我们只支持php、py、jar这类文件进行调用cgi

```c++
string file_type = hr.getSource();
if((file_type == "php") || (file_type == "py") || (file_type == "jar"))
			cgi = 1;
```

file_type是由HttpRequest类的方法取得，后面也可以拿来制作返回的content-type

### 6.如果不需要调用cgi，系统会直接读取文件内容作为返回结果

```c++
if(!cgi)
{
	cat(client, p, file_type);
}
```

### 7.如果需要调用cgi

我们也是把GET和非GET请求分开

先要生成CGI对象

```c++
CGI *u_cgi = new CGI(path,requesturi,query_string); 
```

构造函数所需参数是

```c++
 // 具体的路径，一般是 ”./“ + 根目录 + 请求时的URI（并去掉?后面的内容）
 string path = "./htdocs/tz.php";
 // 请求的 request URI
 string requestUri = "/tz.php?id=1";
 // queryString 请求参数 ？之后
 string query_string = "id=1";
```

#### 如果是GET请求，就可以直接运行cgi

```c++
u_cgi->run();
```

#### 但如果不是CET，我们就需要把剩余的请求体取出

先把请求体长度content-length取出

因为有些请求带的Content-Length是大写的，搞得我们测试时总会一直在加载，所以我们分了两种情况

```c++
map<string,vector<string>>::iterator iter;
iter = headMap.find("Content-Length");
// not found Content-Length but have content-length
if(iter == headMap.end())
{
	iter = headMap.find("content-length");
}
body_length = stoi((iter->second)[0], nullptr);
```

请求头长度拿到，我们就可以开始取body，调用getBody函数，会按长度来取body内容

### 最后所需参数拿到，启动cgi

```c++
u_cgi->run(method, body_content, body_length);  //请求方法、请求体内容、请求体长度
```

## 第四步 返回结果

404在前面已经直接返回了

### 非CGI返回

cat函数会把文件的长度读取出来，制作content-length并作为读取结束标志，还会生成Response对象进行返回

```c++
fseek(resource,0,SEEK_END);
long n = ftell(resource);	//获取文件长度
fseek(resource,0,SEEK_SET);
Response response(client, state_code);
response.sendHttpHead();	//发送response头
response.sendContext(resource, n, file_type);	//发送文件内容
```

在发送文件内容时，我们遇到了很多bug，最终选择了按字符来读取文件内容，并组合成一个4096的字符串进行分包发送

```c++
while(length>0)
{
     int i = 0;
     for(i=0;i<4096 && length>0;i++)
     {
         buf[i] = fgetc(file);
         length--;
     }
     send(client,buf,(i),0);
}
```

### CGI返回

调用运行cgi后，cgi类会保存执行状态和执行结果

我们先判断执行状态码，

#### 如果是500错误或502错误

会像404一样直接生成Response类进行返回错误页面。

```c++
else if(u_cgi->getStatusCode() == 500)
{
     Response response(client, "500");
	 response.sendHttpHead();
 }
```

#### 如果执行成功

由于cgi返回的结果并不单单是返回内容，还有content-type 和 set-cookies的头部信息，

我们就要先把返回的结果进行进一步处理

先取出body，并且求出body长度，中间同样利用'\r\n'进行分割

```c++
string send_str = u_cgi->getOutput();
string::size_type pos = send_str.find_last_of("\r\n");
string res = "";
if(pos != string::npos){
        res = send_str.substr(pos+1);
}
long str_len = res.length();
Response response(client, "200");
response.sendHttpHead();
response.sendString(send_str, str_len);
```

生成Response类，返回200状态码

利用body长度生成content-length

最后按照顺序返回，这时我们的返回的结果是整个包发送

```c++
void Response::sendString(string msg, long body_Length)//返回字符串内容（内部包含部分属性信息）
{
    char *buf = new char[msg.length()];
    msgSend(client,buf,"Connection: close\r\n");
    sprintf(buf,"Content-Length: %ld\r\n",body_Length);
    send(client,buf,strlen(buf),0);
    msgSend(client,buf,msg);
}
```

