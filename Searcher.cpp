#include "MyMap.h"
#include "provided.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Struct designed for search function only
struct UrlScore
{
	string url;
	int count;
	int matched;
	bool operator==(const UrlScore &a) const
	{
		return url == a.url;
	}
};

class SearcherImpl
{
public:
    vector<string> search(string terms);
    bool load(string filenameBase);
private:
	Indexer m_indices;
};

// Comparison function for std::sort function
// which makes std::sort sort in descending order
bool cmpScore(const UrlScore &a, const UrlScore &b)
{
	return (a.count > b.count);
}

vector<string> SearcherImpl::search(string terms)
{
	// This is the vector being returned
	vector<string> result;

	// Lowercase the terms
	strToLower(terms);

	// Split the terms into tags
	Tokenizer token(terms);
	MyMap<string, int> tags; // <term, count=1>
	string tag;
	// Avoid duplicates
	while(token.getNextToken(tag))
		tags.associate(tag, 1);

	// Compute the value of qualifier
	int number = tags.size();
	int target = max((int)(0.7*number), 1);

	// This vector stores UrlScore items with no duplicates
	vector<UrlScore> url;
	vector<UrlScore>::iterator it;
	UrlScore temp;

	// Get the first term of the query
	string keyword;
	tags.getFirst(keyword);
	for(int i=0; i<number; i++)
	{
		vector<UrlCount> matching = m_indices.getUrlCounts(keyword); // The only statement that really finds matching pages of a keyword
		                                                             // having time complexity of O(logN)
		// Determine the final score of each matching URL of the keyword
		for(size_t j=0; j<matching.size(); j++)
		{
			// The following codes behave like a Multiset without using Multiset
			temp.url = matching[j].url;
			it = find(url.begin(), url.end(), temp);
			// If no duplicate is found, then add a new URL with its score
			if(it == url.end())
			{
				UrlScore newscore;
				newscore.url = matching[j].url;
				newscore.count = matching[j].count;
				newscore.matched = 1;
				url.push_back(newscore);
			}
			// If duplicate is found, then update its score without adding new URL
			else
			{
				it->count += matching[j].count;
				it->matched++;
			}
		}
		// Continue the j for loop with the next term
		tags.getNext(keyword);
	}

	// Sort the URL's by its score
	sort(url.begin(), url.end(), cmpScore);

	// Push back only valid URL's
	for(size_t k=0; k<url.size(); k++)
	{
		if(url[k].matched >= target)
			result.push_back(url[k].url);
	}

    return result;
}

bool SearcherImpl::load(string filenameBase)
{
	return m_indices.load(filenameBase);
}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
