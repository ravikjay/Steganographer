#include "provided.h"
#include "http.h"
#include <string>
#include <iostream>
using namespace std;

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
    string page = "";
    if (HTTP().get(url, page)) // If you successfully find the URL (i.e. valid URL)
    {
        if (Steg::hide(page, msg, host)) // Then hide the message in the page's URL code
            return true;
    }
    else
    {
        // give error message for user to know what happened
        cout << "Error fetching content from URL." << endl;
        return false;
    }
    return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
    string page = "";
    if (HTTP().get(url, page)) // If you successfully find the URL (i.e. valid URL)
    {
        if (Steg::reveal(page, msg)) // Then find the message in the page's URL code
            return true;
    }
    else
    {
        // give error message for user to know what happened 
        cout << "Error fetching content from URL." << endl;
        return false;
    }
    return false;
}


