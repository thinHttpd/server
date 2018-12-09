#include <iostream>
#include "CGI.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    // 假设请求 http://127.0.0.1/tz.php?var=1，网站根目录是 ./htdocs
    

    // 基础的
    // 运行的文件名，这个要可能重新抽取出
    // string scriptName = "tz.php"; //这个可以不用
    // 具体的路径，一般是 ./htdocs + 请求时的 URI（并去掉?后面的内容）
    string fileName = "./htdocs/tz.php";
    // 请求的 request URI
    string requestUri = "/tz.php?var=1";
    // queryString
    string queryString = "var=1&b=1";



    // POST
    // HTTP 主体内容
    string bodyContent = "ry=tia";
    // contentLength
    int contentLength = 6;
    // 请求方式
    string method = "POST";
    // 如果页面没有传送 content type，一般 form 提交默认这个，否则直接解析为 POST 数组（非必传，CGI类有默认）
    string contentType = "application/x-www-form-urlencoded";


    // CGI *get = new CGI(fileName,requestUri,queryString);
    
    // get->run();
    // cout<< get->getStatusCode() <<endl;
    // cout<< get->getOutput() <<endl;

    // delete(get);



    CGI *post = new CGI(fileName,requestUri,queryString);

    post->run(method, bodyContent, contentLength);
    cout<< post->getStatusCode() <<endl;
    cout<< post->getOutput() <<endl;
    
    delete(post);
    
    return 0;
}
