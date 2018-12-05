#include <iostream>
#include "CGI.h"
#include "string.h"
#include "stdlib.h"

CGI::CGI(std::string scriptName, std::string fileName, std::string requestUri, std::string queryString)
{
    // 记录必要参数，方便多次利用（可以看作CGI的上下文
    this->m_scriptName = scriptName;
    this->m_fileName = fileName;
    this->m_requestUri = requestUri;
    this->m_queryString = queryString;

    // 设置 QUERY_STRING 环境变量
    CGI::put2env("QUERY_STRING=" + this->m_queryString);

    // 设置 REQUEST_URI 环境变量
    CGI::put2env("REQUEST_URI=" + this->m_requestUri);

    // 设置 SCRIPT_NAME
    CGI::put2env("SCRIPT_NAME=" + this->m_scriptName);
    
    // 设置 SCRIPT_FILENAME
    CGI::put2env("SCRIPT_FILENAME=" + this->m_fileName);

}

void CGI::run()
{

    std::string cmd = "php-cgi";

    char *path = new char(strlen(cmd.c_str()));
    strcpy(path,cmd.c_str());

    std::string resultString = this->exec(path);
    
    char *resultChar = new char(strlen(resultString.c_str()));
    
    strcpy(resultChar,resultString.c_str());

    this->m_output = resultChar;

}

std::string CGI::exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe){
        // 和 CGI 对接方面出问题，返回 502
        this->m_statusCode = 502;
        return "\r\n thinHttpd CGI error!";
    }
    
    char buf[1024];
    std::string result = "";

    while (!feof(pipe)) {
        if (fgets(buf, 1024, pipe) != NULL)
            result += buf;
    }

    this->m_statusCode = 200;
    pclose(pipe);
    return result;
}

void CGI::put2env(std::string msg) {
    char buf[1024];
    sprintf(buf,"%s",msg.c_str());
    putenv(buf);
}

int CGI::getStatusCode(){
    return this->m_statusCode;
}

char * CGI::getOutput(){
    return this->m_output;
}