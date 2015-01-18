#include "provided.h"
#include "http.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

const string INDEX_PREFIX = "oogleIndex";

void crawlerCallback(string url, bool success);
void addFromFile(string filename, WebCrawler& wc);
void doSearches();
void help();

int main()
{
    cout << "oogle - the amazing search engine" << endl;
    help();
    const string validCommands("afcsqhp");
    WebCrawler wc;
    for (;;)
    {
        cout << "Command: ";
        string line;
        if (!getline(cin,line))
            break;
        istringstream lineStream(line);
        string cmd;
	if (!(lineStream >> cmd))  // empty line
            continue;
        if (cmd.size() != 1  ||  validCommands.find(cmd[0]) == string::npos)
        {
            cout << "***** Unknown command \"" << cmd << "\" *****" << endl;
            help();
            continue;
        }
        bool requiresArg = (cmd[0] == 'a'  ||  cmd[0] == 'f');
        string arg;
        if (lineStream >> arg)
        {
            if (!requiresArg)
            {
                cout << "***** The \"" << cmd << "\" command takes no arguments *****" << endl;
                help();
                continue;
            }
        }
        else if (requiresArg)
        {
            cout << "***** The \"" << cmd << "\" command requires an argument *****" << endl;
            help();
            continue;
        }
        switch (cmd[0])
        {
          case 'a':
            wc.addUrl(arg);
            cout << "Added " << arg << endl;
            break;
          case 'f':
            addFromFile(arg, wc);
            break;
          case 'c':
            cout << "Processing " << wc.getNumberOfUrls() << " URLs:" << endl;
            wc.crawl(crawlerCallback);
            break;
          case 's':
            if (!wc.save(INDEX_PREFIX))
            {
                cout << "***** Error saving crawler index" << endl;
                continue;
            }
            doSearches();
            break;
          case 'q':
            return 0;
            break;
          case 'h':
            help();
            break;
          case 'p':
            HTTP().set("http://a.com", "aaa bbb ccc ddd ddd");
            HTTP().set("http://b.com", "bbb ccc ccc ddd eee");
            HTTP().set("http://c.com", "ddd eee fff fff ggg");
            cout << "Using the pseudoweb (The h command shows the 3 pages)" << endl;
            break;
        }
    }
}

void crawlerCallback(string url, bool success)
{
    if (success)
        cout << "Processed " << url << endl;
    else
        cout << "***** Error processing " << url << endl;
}

void addFromFile(string filename, WebCrawler& wc)
{
    ifstream urlFile(filename.c_str());
    if (!urlFile)
    {
        cout << "***** Error opening " << filename << endl;
        return;
    }
    string url;
    while (getline(urlFile, url))
    {
        cout << "Added " << url << endl;
        wc.addUrl(url);
    }
}

void doSearches()
{
    Searcher s;
    if (!s.load(INDEX_PREFIX))
    {
        cout << "***** Error loading searcher index" << endl;
        return;
    }
    cout << "Entering an empty line will end searching" << endl;
    for (;;)
    {
        cout << "Enter search terms: ";
        string terms;
        if (!getline(cin, terms)  ||  terms.empty())
            break;
        vector<string> urls = s.search(terms);
        if (urls.empty())
            cout << "There are no matches" << endl;
        else
        {
            cout << "There are " << urls.size() << " matches:" << endl;
            for (size_t k = 0; k != urls.size(); k++)
                cout << "   " << urls[k] << endl;
        }
    }
}

void help()
{
    cout << "Commands available:" << endl
         << "   a someUrl  -- add a URL to be crawled later" << endl
         << "   f filename -- add URLs from a file (one URL per line)" << endl
         << "   c          -- crawl the added URLs, building an index" << endl
         << "   s          -- start perfroming various searches" << endl
         << "   q          -- quit" << endl
         << "   h          -- help:  show this list of commands" << endl
         << "   p          -- use this pseudo-web instead of the real Web:" << endl
         << "                   http://a.com -> \"aaa bbb ccc ddd ddd\"" << endl
         << "                   http://b.com -> \"bbb ccc ccc ddd eee\"" << endl
         << "                   http://c.com -> \"ddd eee fff fff ggg\"" << endl;
}
