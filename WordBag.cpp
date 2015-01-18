#include "MyMap.h"
#include "provided.h"
#include <cstdlib>
using namespace std;

class WordBagImpl
{
	public:
		WordBagImpl(const string& text);
		bool getFirstWord(string& word, int& count);
		bool getNextWord(string& word, int& count);

	private:
		MyMap<string, int> m_words;
};

/*
============================================================================================
=============================WordBagImpl Class Implementation===============================
============================================================================================
*/
WordBagImpl::WordBagImpl(const string& text)
{
	string text_lower = text;
	strToLower(text_lower);

	Tokenizer token(text_lower);
	string word;
	while(token.getNextToken(word))
	{
		int *count = m_words.find(word);
		if(count == NULL)
			m_words.associate(word, 1);
		else
		{
			int inc = *count;
			inc++;
			m_words.associate(word, inc);
		}
	}
}

bool WordBagImpl::getFirstWord(string& word, int& count)
{
	string firstWord;
	int *getFirst = m_words.getFirst(firstWord);
    if(getFirst != NULL)
	{
		word = firstWord;
		count = *getFirst;
		return true;
	}
	else
		return false;
}

bool WordBagImpl::getNextWord(string& word, int& count)
{
    string nextWord;
	int *getNext = m_words.getNext(nextWord);
    if(getNext != NULL)
	{
		word = nextWord;
		count = *getNext;
		return true;
	}
	else
		return false;
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const std::string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}
