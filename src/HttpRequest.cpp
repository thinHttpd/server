//
// Created by ace on 2018/11/28.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest() {};
HttpRequest::HttpRequest(std::string req) {
    init(req);
};
/**
 * 判断uri是否要请求资源
 * @return
 */
bool HttpRequest::hasSource() {
    using namespace std;
    string tmp = this->getUri();
    std::vector<string> res;
    splitString(uri,res,"?");
    if(res.size() < 2){
        return false;
    }
    return true;
};

void HttpRequest::setReq(std::string req){
    init(req);
}
const std::string &HttpRequest::getMehtod() const {
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
