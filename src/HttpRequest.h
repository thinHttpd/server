//
// Created by ace on 2018/11/28.
//

#ifndef HTTPREQUEST_HTTPREQUEST_H
#define HTTPREQUEST_HTTPREQUEST_H


#include <string>
#include <map>
#include <vector>

class HttpRequest {
private:
    std::string mehtod;
    std::string version;
    std::string uri;
    std::string requsetBody;
    std::map<std::string, std::vector<std::string>> headers;


    std::string trim(const std::string &str) {
        using namespace std;
        string::size_type pos = str.find_first_not_of(' ');
        if (pos == string::npos) {
            return str;
        }
        string::size_type pos2 = str.find_last_not_of(' ');
        if (pos2 != string::npos) {
            return str.substr(pos, pos2 - pos + 1);
        }
        return str.substr(pos);
    }
    void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
    {
        std::string::size_type pos1, pos2;
        pos2 = s.find(c);
        pos1 = 0;
        while(std::string::npos != pos2)
        {
            v.push_back(s.substr(pos1, pos2-pos1));

            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if(pos1 != s.length())
            v.push_back(s.substr(pos1));
    }
    std::map<std::string, std::vector<std::string>> getRequestMap(std::string req) {
        using namespace std;
        map<string, vector<string>> res;
        vector<string> reqs;
        splitString(req, reqs, "\r\n");
        vector<string> reqLine;
        splitString(reqs[0], reqLine, " ");
        mehtod = trim(reqLine[0]);
        uri = trim(reqLine[1]);
        version = trim(reqLine[2]);
        /*vector<string> tmp;
        tmp.push_back(reqLine[0]);
        res.insert(pair<string, vector<string>>("method", tmp));
        tmp.pop_back();
        tmp.push_back(reqLine[1]);
        res.insert(pair<string, vector<string>>("uri", tmp));
        tmp.pop_back();
        tmp.push_back(reqLine[2]);
        res.insert(pair<string, vector<string>>("version", tmp));
*/
        for (int i = 1; i < reqs.size() - 1; ++i) {
            vector<string> tmp;
            int pos = 0;
            if (reqs[i].size() > 0) {
                for (int j = 0; j < reqs[i].size(); ++j) {
                    if(reqs[i][j] == ':'){
                       pos = j;
                        break;
                    }
                }
//                splitString(reqs[i], tmp, ":");
//                tmp[0] = trim(reqs[i].substr(0,pos));
                string s1 = trim(reqs[i].substr(0,pos));
//                tmp[1] = trim(reqs[i].substr(pos+1,reqs[i].size()));
                string s2 = trim(reqs[i].substr(pos+1,reqs[i].size()));
                vector<string> attr;
//                splitString(tmp[1],attr,",");
                splitString(s2,attr,",");
//                res.insert(pair<string,vector<string>>(tmp[0], attr));
                res.insert(pair<string,vector<string>>(s1, attr));
            }
        }
        string p = "POST";
        if (mehtod == p) {
            requsetBody = trim(reqs[reqs.size() - 1]);
            /*vector<string> body;
            body.push_back(reqs[reqs.size() - 1]);
            res.insert(pair<string, vector<string>>("requestBody", body));*/
        } else{
            requsetBody = "";
        }
        return res;
    }
    void init(std::string req){
        headers = getRequestMap(req);
    }
public:
    HttpRequest();
    HttpRequest(std::string req);

    void setReq(std::string req);

    const std::string &getMethod() const;

    const std::string &getVersion() const;

    const std::string &getUri() const;

    const std::string &getRequsetBody() const;

    const std::map<std::string, std::vector<std::string>> &getHeaders() const;

    /* std::string getMethod();

     std::string getVersion();

     std::string getUri();

     std::string getUserAgent();

     std::string getConnection();

     std::string getReferer();

     std::string getContentType();

     std::string getRequestBody();

     long getContentLength();

     std::vector<std::string> getAccept();

     std::vector<std::string> getAcceptLanguage();

     std::vector<std::string> getAcceptEncoding();
 */

};


#endif //HTTPREQUEST_HTTPREQUEST_H
