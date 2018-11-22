#ifndef CGI_H
#define CGI_H
#include <string>

class CGI
{
    public:
        CGI();
        virtual ~CGI();

        CGI(std::string scriptName, std::string queryString);

    protected:
        std::string handler_;
        std::string scriptName_;
        std::string queryString_;
        std::string bodyContent_;
};

#endif // CGI_H
