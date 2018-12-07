#include <iostream>
#include <thread>
#include "CGI.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"

CGI::CGI(std::string scriptName, std::string fileName, std::string requestUri, std::string queryString)
{
    // 记录必要参数（可以看作CGI的上下文
    this->m_scriptName = scriptName;
    this->m_fileName = fileName;
    this->m_requestUri = requestUri;
    this->m_queryString = queryString;

}

void CGI::run()
{

    char *path = new char(strlen(this->m_cmd.c_str()));
    strcpy(path,this->m_cmd.c_str());

    std::string resultString = this->exec(path);
    
    char *resultChar = new char(strlen(resultString.c_str()));
    
    strcpy(resultChar,resultString.c_str());

    this->m_output = resultChar;

}

void CGI::run(std::string method, std::string bodyContent, int contentLength, std::string contentType)
{
    // 记录必要参数，方便多次利用（可以看作CGI的上下文
    this->m_method = method;
    this->m_bodyContent = bodyContent;
    this->m_contentLength = contentLength;
    this->m_contentType = contentType;

    char *path = new char(strlen(this->m_cmd.c_str()));
    strcpy(path,this->m_cmd.c_str());

    char *input = new char(strlen(this->m_bodyContent.c_str()));
    strcpy(input,this->m_bodyContent.c_str());

    std::string resultString = this->exec(path,input);
    
    char *resultChar = new char(strlen(resultString.c_str()));
    
    strcpy(resultChar,resultString.c_str());

    this->m_output = resultChar;
}

std::string CGI::exec(const char* cmd)
{
    // FILE* pipe = popen(cmd, "r");
    // if (!pipe){
    //     // 和 CGI 对接方面出问题，返回 502
    //     this->m_statusCode = 502;
    //     return "\r\n thinHttpd CGI error!";
    // }
    
    // char buf[1024];
    // std::string result = "";

    // while (!feof(pipe)) {
    //     if (fgets(buf, 1024, pipe) != NULL)
    //         result += buf;
    // }

    // this->m_statusCode = 200;
    // pclose(pipe);
    // return result;

    // 改用：管道通信

    // 维护1个管道，用于输出输出流的保存，克服 popen 只能单向的弱点
    // 这个通道直接放在类的属性里，方便和 cgi 线程共享

    if (pipe(this->cgiPipe) < 0) {
        this->m_statusCode = 500;
        return "\r\n thinHttpd inner error!";
    }


    // 其实 C 语言的 fork 多线程和 PHP pcntl_fork 多线程基本一致
    // 但是这里试用了一下 C++ 11 新增加的 Thread，但是是不可行的，exec 无法分配新的内存
    int pid = fork();
    std::string result = "";
    if(pid<0)
    {
        this->m_statusCode = 502;
        return "\r\n thinHttpd CGI Gateway error!";
    }
    else if (pid ==0)
    {
        // 设置 QUERY_STRING 环境变量
        CGI::put2env("QUERY_STRING", this->m_queryString);
    
        // 设置 REQUEST_URI 环境变量
        CGI::put2env("REQUEST_URI", this->m_requestUri);
    
        // 设置 SCRIPT_NAME
        CGI::put2env("SCRIPT_NAME", this->m_scriptName);
        
        // 设置 SCRIPT_FILENAME
        CGI::put2env("SCRIPT_FILENAME", this->m_fileName);


        // Linux管道 关闭读
        close(this->cgiPipe[0]);
        // stdout 指针同时指向同一个文件表
        dup2(this->cgiPipe[1],1);
        
        char *cmd = new char(strlen(this->m_cmd.c_str()));
        strcpy(cmd,this->m_cmd.c_str());
        
        // 后面是多参数，以 NULL 结尾，类似于 PHP 原始版的 ...%arg
        execlp(cmd, cmd, NULL);
        // 用 system 的话输入输出重定向不了
        // system(cmd);

        close(this->cgiPipe[1]);

    } else {
        // 下面是父进程
        close(this->cgiPipe[1]);

        char resultChar;
        
        // 一个字节一个字节读，不然会漏文件尾？！不确定，总之有个 bug
        while (read(this->cgiPipe[0], &resultChar, 1) > 0)
            result += resultChar;
    
        // 恢复正常输入输出流
        close(this->cgiPipe[0]);

        this->m_statusCode = 200;
        return result;
    }

   
}


std::string CGI::exec(const char *cmd, const char *input)
{
    // 之所以分开两个函数来处理，是因为 POST、PUT、PATCH 等请求需要传送 stdin 到 CGI 解译器，需要多开一个 pipe 管道通信用于传递
    // 这个管道暂时放在这个函数内定义，声明周期和此函数一致

    // 管道放到头文件去声明，让他变成自动储存持续性，方便在子进程调用
    // int inputPipe[2];
    
    if (pipe(this->cgiPipe) < 0) {
        this->m_statusCode = 500;
        return "\r\n thinHttpd inner error!";
    }
    if (pipe(this->inputPipe) < 0) {
        this->m_statusCode = 502;
        return "\r\n thinHttpd inner error!";
    }


    // 其实 C 语言的 fork 多线程和 PHP pcntl_fork 多线程基本一致
    // 但是这里试用了一下 C++ 11 新增加的 Thread，但是是不可行的，exec 无法分配新的内存
    int pid = fork();
    std::string result = "";
    if(pid<0)
    {
        this->m_statusCode = 502;
        return "\r\n thinHttpd CGI Gateway error!";
    }
    else if (pid ==0)
    {
       
        // 设置 QUERY_STRING 环境变量
        CGI::put2env("QUERY_STRING", this->m_queryString);
    
        // 设置 REQUEST_URI 环境变量
        CGI::put2env("REQUEST_URI", this->m_requestUri);
    
        // 设置 SCRIPT_NAME
        CGI::put2env("SCRIPT_NAME", this->m_scriptName);
        
        // 设置 SCRIPT_FILENAME
        CGI::put2env("SCRIPT_FILENAME", this->m_fileName);

        // 设置 REQUEST_METHOD
        CGI::put2env("REQUEST_METHOD", this->m_method);

        // 设置 CONTENT_LENGTH
        CGI::put2env("CONTENT_LENGTH", std::to_string(this->m_contentLength));
        CGI::put2env("HTTP_CONTENT_LENGTH", std::to_string(this->m_contentLength));

        // 设置 CONTENT_TYPE
        CGI::put2env("CONTENT_TYPE", this->m_contentType);

        

        // Linux管道 关闭读
        close(this->cgiPipe[0]);
        // stdout 指针同时指向同一个文件表
        dup2(this->cgiPipe[1],1);


        // 下面这个管道是 POST 等请求方式特有的，需要传递字符串到子进程的 stdin
        // 关闭写，只从这个管道里面读取数据
        close(this->inputPipe[1]);
        // stdin 指针同时指向同一个文件表
        dup2(this->inputPipe[0],0);
        // 可以看到和上一个管道是刚刚好相反的设置，因为一个是读一个是写


        char *cmd = new char(strlen(this->m_cmd.c_str()));
        strcpy(cmd,this->m_cmd.c_str());
        
        // 后面是多参数，以 NULL 结尾，类似于 PHP 原始版的 ...%arg
        char *cmd2 = new char(strlen(this->m_bodyContent.c_str()));
        strcpy(cmd2,this->m_cmd.c_str());
        execlp(cmd, cmd2 , NULL);

        // 用 system 的话输入输出重定向不了
        // system(cmd);
        
        close(this->cgiPipe[1]);
        close(this->inputPipe[0]);

    } else {
        // 下面是父进程
        close(this->cgiPipe[1]);

        // 同样是对 stdin 的特殊处理，关闭父进程的读
        close(this->inputPipe[0]);

        char resultChar;

        fwrite(input,this->m_contentLength,this->m_contentLength,stdin);

        for (int i = 0; i < this->m_contentLength; i++) {
            write(this->inputPipe[1], input+i, 1);
        }
        
        // 一个字节一个字节读，不然会漏文件尾？！不确定，总之有个 bug: 只能读 918 个字符
        while (read(this->cgiPipe[0], &resultChar, 1) > 0)
        {
            result += resultChar;
        }
    
        // 恢复正常输入输出流
        close(this->cgiPipe[0]);
        close(this->inputPipe[1]);

        this->m_statusCode = 200;
        return result;
    }
}

void CGI::put2env(std::string key,std::string value)
{
    char keybuf[256];
    char valuebuf[1024];
    sprintf(keybuf,"%s",key.c_str());
    sprintf(valuebuf,"%s",value.c_str());

    // 实测 putenv 只能记录最后一组 key value
    // 因此采用 setenv
    setenv(keybuf,valuebuf,1);
}

int CGI::getStatusCode()
{
    return this->m_statusCode;
}

char * CGI::getOutput()
{
    return this->m_output;
}