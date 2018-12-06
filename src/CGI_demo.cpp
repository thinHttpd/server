#include <iostream>
#include "CGI.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    // 假设请求 http://127.0.0.1/tz.php?var=1，网站根目录是 ./htdocs
    

    // 基础的
    // 运行的文件名，这个要可能重新抽取出
    string scriptName = "tz.php";
    // 具体的路径，一般是 ./htdocs + 请求时的 URI（并去掉?后面的内容）
    string fileName = "./htdocs/tz.php";
    // 请求的 request URI
    string requestUri = "/tz.php?var=1";
    // queryString
    string queryString = "var=1&b=1";


    // POST
    // HTTP 主体内容
    string bodyContent = "a=1";
    // contentLength
    int contentLength = 3;
    // 请求方式
    string method = "POST";


    CGI *get = new CGI(scriptName,fileName,requestUri,queryString);
    
    get->run();
    cout<< get->getStatusCode() <<endl;
    cout<< get->getOutput() <<endl;

    delete(get);



    // CGI *post = new CGI(scriptName,fileName,requestUri,queryString);

    // post->run(method, bodyContent, contentLength);
    // cout<< post->getStatusCode() <<endl;
    // cout<< post->getOutput() <<endl;
    
    // delete(post);
    
    return 0;
}
