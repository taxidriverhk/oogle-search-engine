#include "MyMap.h"
#include "provided.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

struct UrlIdCount
{
	int UrlId;
	int count;
};

class IndexerImpl
{
public:
	// Member Functions
    bool incorporate(string url, WordBag& wb);
    vector<UrlCount> getUrlCounts(string word);
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
	// Data Members
	MyMap<string, int> m_urlToId;
	MyMap<int, string> m_idToUrl;
	MyMap<string, vector<UrlIdCount> > m_wordToIdCounts;

	//Helper Function
	void associate(string word, int count, int urlId);
};

// Non-member Helper Function
template<typename KeyType, typename ValueType>
bool saveMyMap(std::string filename, MyMap<KeyType,ValueType>& m);
template<typename KeyType, typename ValueType>
bool loadMyMap(std::string filename, MyMap<KeyType,ValueType>& m);

/*
============================================================================================
=============================IndexerImpl Class Implementation===============================
============================================================================================
*/
bool IndexerImpl::incorporate(string url, WordBag& wb)
{
	// Each URL can incorporate with a WordBag for only once
	// (i.e. no website has two different contents)
	if(m_urlToId.find(url) != NULL)
		return false;

	// Function to generate the next URL ID
	int n = 0;
	int urlId = 0;
	do
	{
		if(n % 2 == 0)
			urlId = rand() % 10000;
		else
			urlId = -1 * (rand() % 10000);
		n++;
	} while(m_idToUrl.find(urlId) != NULL);

	// Link the URL ID to the corresponding URL ID, and vice versa
	m_urlToId.associate(url, urlId);
	m_idToUrl.associate(urlId, url);

    string word;
	int count;

	// Insert all words from the WordBag into the m_wordToIdCounts map;
	if(wb.getFirstWord(word, count) == true)
	{
		associate(word, count, urlId);
		while(wb.getNextWord(word, count) != false)
			associate(word, count, urlId);
	}

	return true;
}

vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
	strToLower(word);
	vector<UrlCount> result;

	vector<UrlIdCount> *UrlIds = m_wordToIdCounts.find(word);
	if(UrlIds != NULL)
	{
		for(size_t i=0; i<UrlIds->size(); i++)
		{
			UrlCount Url;
			int id = (*UrlIds)[i].UrlId;
			Url.url = *(m_idToUrl.find(id));
			Url.count = (*UrlIds)[i].count;
			result.push_back(Url);
		}
	}

	return result;
}

bool IndexerImpl::save(string filenameBase)
{
    return saveMyMap(filenameBase + ".u2i", m_urlToId) &&
		   saveMyMap(filenameBase + ".i2u", m_idToUrl) &&
		   saveMyMap(filenameBase + ".w2ic", m_wordToIdCounts); 
}

bool IndexerImpl::load(string filenameBase)
{
	return loadMyMap(filenameBase + ".u2i", m_urlToId) &&
		   loadMyMap(filenameBase + ".i2u", m_idToUrl) &&
		   loadMyMap(filenameBase + ".w2ic", m_wordToIdCounts);
}

// Helper function for associating words with UrlIdCount
void IndexerImpl::associate(string word, int count, int urlId)
{
	UrlIdCount UrlId;
	UrlId.UrlId = urlId;
	UrlId.count = count;
	// If the word has not associated with a vector before, 
	// then associate it with a new vector
	if(m_wordToIdCounts.find(word) == NULL)
	{
		vector<UrlIdCount> UrlIds;
		UrlIds.push_back(UrlId);
		m_wordToIdCounts.associate(word, UrlIds);
	}
	// If the word is associated with a vector,
	// then insert a new URL ID for that vector
	else
	{
		vector<UrlIdCount> *UrlIds = m_wordToIdCounts.find(word);
		(*UrlIds).push_back(UrlId);
		m_wordToIdCounts.associate(word, *UrlIds);
	}
}

/* 
* Non-member Helper Functions for saving Indexer 
*/
void writeItem(ostream& stream, int i) // Write item of int type to stream
{
	stream << i << endl;
}

void writeItem(ostream& stream, string s) // Write item of string type to stream
{
	stream << s << endl;
}

void writeItem(ostream& stream, const UrlIdCount &u) // Write item of UrlIdCount type to stream
{
	writeItem(stream, u.UrlId);
	writeItem(stream, u.count);
}

void writeItem(ostream& stream, const vector<UrlIdCount> &v) // Write item of vector<UrlIdCount> type to stream
{
	stream << v.size() << endl;
	for(size_t i=0; i<v.size(); i++)
		writeItem(stream, v[i]);
}

// Helper function that saves the contents of MyMap m into the file filename
template<typename KeyType, typename ValueType>
bool saveMyMap(std::string filename, MyMap<KeyType,ValueType>& m)
{
	// Return if failed to create the file
	ofstream output(filename);
	if(!output)
		return false;

	KeyType key;
	ValueType *value;

	// Write the size of the MyMap m onto the first line of the output
	writeItem(output, m.size());

	// Write elements from MyMap in level order
	for(value = m.getFirst(key); value != NULL; value = m.getNext(key))
	{
		writeItem(output, key);
		writeItem(output, *value);
	}

	return true;
}

/* 
* Non-member Helper Functions for loading Indexer 
*/
bool readItem(istream& stream, int &i) // Read item of int type from stream
{
	// Check if the line is really an integer
	string line;
	if (!getline(stream, line)) // return false if getline fails
		return false;

	// Convert the string to int
	istringstream iss(line);
	return iss >> i ? true : false;
}

bool readItem(istream& stream, string &s) // Read item of string type from stream
{
	return getline(stream, s) ? true : false;
}

bool readItem(istream& stream, UrlIdCount &u) // Read item of UrlIdCount type from stream
{
	if(!readItem(stream, u.UrlId))
		return false;
	if(!readItem(stream, u.count))
		return false;

	return true;
}

bool readItem(istream& stream, vector<UrlIdCount> &v) // Read item of vector<UrlIdCount> type from stream
{
	// Read the size of the vector first, return false if failed
	int size;
	if(!readItem(stream, size))
		return false;

	// Read each element from the vector
	UrlIdCount u;
	for(int i=0; i<size; i++)
	{
		if(!readItem(stream, u))
			return false;
		
		v.push_back(u);
	}
	return true;
}


// Helper function that saves the contents of MyMap m into the file filename
template<typename KeyType, typename ValueType>
bool loadMyMap(std::string filename, MyMap<KeyType,ValueType>& m)
{
	// Clear the map first
	m.clear();

	// Return if failed to open the file
	ifstream input(filename);
	if(!input)
		return false;

	// Read the number of associations in m from stream, returning false if we can't
	int size;
	if(!readItem(input, size))
		return false;

	// Read the elements from the stream, and do association
	for(int i=0; i<size; i++)
	{
		KeyType key;
		ValueType value;
		if(!readItem(input, key))
			return false;
		if(!readItem(input, value))
			return false;
		m.associate(key, value);
	}

	return true;
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
    m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
    delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
    return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
    return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
