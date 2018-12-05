#include <iostream>
#include "CGI.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    
    string scriptName = "tz.php";
    string queryString = "";
    CGI *test = new CGI();
    delete(test);
    return 0;
}
