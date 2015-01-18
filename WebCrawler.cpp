#include "http.h"
#include "provided.h"
#include <string>
#include <queue>
#include <cstdlib>
using namespace std;

class WebCrawlerImpl
{
public:
    void addUrl(string url);
    int getNumberOfUrls() const;
    void crawl(void (*callback)(string url, bool success));
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	Indexer m_indices;
	queue<string> m_uncrawledURLs;
};

void WebCrawlerImpl::addUrl(string url)
{
	m_uncrawledURLs.push(url);
}

int WebCrawlerImpl::getNumberOfUrls() const
{
	return m_uncrawledURLs.size();
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
	while(m_uncrawledURLs.empty() == false)
	{
		// Get the URL
		string contents;
		string url = m_uncrawledURLs.front();
		m_uncrawledURLs.pop();

		// Get the contents downloaded from the URL
		bool success = HTTP().get(url, contents);
		if(success == true)
		{
			WordBag wb(contents);
			m_indices.incorporate(url, wb);
		}

		// Call the callback function
		callback(url, success);
	}
}

bool WebCrawlerImpl::save(string filenameBase)
{
	return m_indices.save(filenameBase);
}

bool WebCrawlerImpl::load(string filenameBase)
{
    return m_indices.load(filenameBase);
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
    m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
    delete m_impl;
}

void WebCrawler::addUrl(string url)
{
    m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
    return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
    m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
