#ifndef CGI_H
#define CGI_H
#include <string>
#include "pthread.h"

class CGI
{
    public:
    	CGI(){};
    	~CGI(){};
        CGI(std::string scriptName, std::string fileName, std::string requestUri, std::string queryString);
        void run();
        char * getOutput();
        int getStatusCode();

    protected:
        std::string m_scriptName;
        std::string m_fileName;
        std::string m_requestUri;
        std::string m_queryString;
        std::string m_bodyContent;
        std::string m_contentLength;

    private:
        void put2env(std::string msg);
        std::string exec(const char* cmd);
        char * m_output;
        int m_statusCode;
};

#endif // CGI_H
