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


std::string HttpRequest::getSource(){
    using namespace std;
    string uri = this->getUri();
    string res="";
    string::size_type pos;
    vector<string> tmp;
    splitString(uri,tmp,"?");
    pos = tmp[0].find_last_of(".");
    if(pos != string::npos){
        res = tmp[0].substr(pos+1);
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
