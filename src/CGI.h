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
        char * run();
        std::string exec(const char* cmd);

    protected:
        std::string m_scriptName;
        std::string m_fileName;
        std::string m_requestUri;
        std::string m_queryString;
        std::string m_bodyContent;
        std::string m_contentLength;

    private:
        void put2env(std::string msg);
};

#endif // CGI_H
