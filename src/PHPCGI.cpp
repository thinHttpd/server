#include "PHPCGI.h"

PHPCGI::PHPCGI()
{
    //ctor
}

PHPCGI::~PHPCGI()
{
    //dtor
}

PHPCGI::PHPCGI(std::string scriptName, std::string queryString)  : CGI(scriptName, queryString){

}
