#ifndef CGI_H
#define CGI_H
#include <string>

class CGI
{
    public:
        CGI();
        virtual ~CGI();

        CGI(std::string scriptName, std::string queryString);
        CGI(std::string scriptName, std::string queryString, std::string m_contentLength);

    protected:
        std::string m_scriptName;
        std::string m_queryString;
        std::string m_bodyContent;
        std::string m_contentLength;
};

#endif // CGI_H
