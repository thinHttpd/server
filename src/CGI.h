#ifndef CGI_H
#define CGI_H
#include <string>
#include "pthread.h"

class CGI
{
    public:
    	CGI(){};
    	~CGI(){};
        CGI(std::string scriptName, std::string queryString);
        char * run();

    protected:
        std::string m_scriptName;
        std::string m_queryString;
        std::string m_bodyContent;
        std::string m_contentLength;
};

#endif // CGI_H
