//
// Created by ace on 2018/11/28.
//

#include <regex>
#include "HttpRequest.h"

HttpRequest::HttpRequest() {};
HttpRequest::HttpRequest(std::string req) {
    init(req);
}
bool HttpRequest::hasSource() {
    using namespace std;
    string tmp = this->getUri();
    std::vector<string> res;
    splitString(uri,res,"?");
    if(res.size() < 2){
        return false;
    }
    return true;
}


std::vector<std::string> HttpRequest::getSource(){
    using namespace std;
    string uri = this->getUri();
    vector<string> res;
    regex reg1(".*\\.html(.*)");
    regex reg2(".*\\.php(.*)");
    regex reg3(".*\\.py(.*)");
    regex reg4(".*\\.jsp(.*)");
    if(regex_match(uri,reg1)){
        res.push_back(".html");
    }
    if(regex_match(uri,reg2)){
        res.push_back(".php");
    }
    if(regex_match(uri,reg3)){
        res.push_back(".py");
    }
    if(regex_match(uri,reg4)){
        res.push_back(".jsp");
    }
    return res;
}
void HttpRequest::setReq(std::string req){
    init(req);
}

const std::string &HttpRequest::getMethod() const {
    return mehtod;
}

const std::string &HttpRequest::getVersion() const {
    return version;
}

const std::string &HttpRequest::getUri() const {
    return uri;
}

const std::string &HttpRequest::getRequsetBody() const {
    return requsetBody;
}

const std::map<std::string, std::vector<std::string>> &HttpRequest::getHeaders() const {
    return headers;
}
