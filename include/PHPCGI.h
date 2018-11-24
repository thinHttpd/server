#ifndef PHPCGI_H
#define PHPCGI_H
#include "CGI.h"


class PHPCGI : CGI
{
    public:
        PHPCGI();
        virtual ~PHPCGI();
        PHPCGI(std::string scriptName, std::string queryString);

    protected:

    private:
};

#endif // PHPCGI_H
