#ifndef CGI_H
#define CGI_H
#include <string>

class CGI
{
    public:
    	CGI(){};
    	~CGI(){};
        CGI(std::string fileName, std::string requestUri, std::string queryString);
        void run();
        void run(std::string method, std::string bodyContent, int contentLength, std::string contentType = "application/x-www-form-urlencoded");
        char * getOutput();
        int getStatusCode();

    protected:
        std::string m_scriptName;
        std::string m_fileName;
        std::string m_requestUri;
        std::string m_queryString;
        std::string m_bodyContent;
        std::string m_method;
        std::string m_contentType;
        std::string m_cmd = "php-cgi";
        int m_contentLength;

    private:
        void put2env(std::string key, std::string value);
        std::string exec(const char* cmd);
        std::string exec(const char *cmd, const char *input);
        char * m_output;
        int m_statusCode;
        int cgiPipe[2];
        int inputPipe[2];
};

#endif // CGI_H
