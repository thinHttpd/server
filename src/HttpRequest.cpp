//
// Created by ace on 2018/11/28.
//

#include "HttpRequest.h"

HttpRequest::HttpRequest() {};
HttpRequest::HttpRequest(std::string req) {
    init(req);
};
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
