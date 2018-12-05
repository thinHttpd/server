#include "CGI.h"

CGI::CGI(std::string scriptName, std::string queryString)
{
    this->m_scriptName = scriptName;
    this->m_queryString = queryString;
}

char * CGI::run()
{
    // 设置 QUERY STRING 环境变量
    putenv(std::string("QUERY_STRING="+this->m_queryString).c_str());
    // 设置 SCRIPT NAME
    putenv(std::string("SCRIPT_NAME="+this->m_scriptName).c_str());

    // 开始调用 CGI 程序

    // fork 模型和 PHP 多线程类似
    // 队友请注意：父进程调用了pipe()再fork()之后，子进程会继承父进程的pipe()
    // 此时有2条管道，4个口子。如果要进自行父子单向数据传输，则需要关闭特定的2个口子。父进程要写给子进程，则关闭父 进程的读，关闭子程序的写。子进程要写给父进程则相反

    // 开一个管道
    // int pipe[2];

    // if (pipe(pipe) < 0)
    // {
    //     return NULL;
    // }
    char *path;
    sprintf(path, "phpcgi");
    exec(path, this->m_queryString.c_str());

}

std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "error!";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}