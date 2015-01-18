#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

#include <queue>
#include <cstdlib>

/*
============================================================================================
================================MyMap Class Declaration=====================================
============================================================================================
*/
template <class KeyType, class ValueType>
class MyMap
{
	public:
		// Constructor
		MyMap()
			: m_size(0), m_root(NULL){}

		// Destructor
		~MyMap(){ clear(); }

		// Member Functions / Accessor
		void clear();
		int size() const;
		void associate(const KeyType& key, const ValueType& value);
		const ValueType* find(const KeyType& key) const;
		ValueType* find(const KeyType& key)
		{
			// Do not change the implementation of this overload of find
			const MyMap<KeyType,ValueType>* constThis = this;
			return const_cast<ValueType*>(constThis->find(key));
		}
		ValueType* getFirst(KeyType& key);
		ValueType* getNext(KeyType& key);

	private:
		// Copy Constructor
		MyMap(const MyMap &other);

		// Assignment Operator
		MyMap &operator=(const MyMap &other);

		// Data Members
		struct Node
		{
			// Constructor
			Node(const KeyType &key, const ValueType &value)
				: m_key(key), m_left(NULL), m_right(NULL)
			{ 
				m_value = new ValueType;
				*m_value = value;
			}
			// Data Members
			KeyType m_key;
			ValueType *m_value;
			Node *m_left, *m_right;
		};

		int m_size;
		Node *m_root;
		std::queue<Node*> m_level;
};

/*
============================================================================================
================================MyMap Class Implementation==================================
============================================================================================
*/
template <class KeyType, class ValueType>
void MyMap<KeyType, ValueType>::clear()
{
	if(m_root == NULL)
		return;

	std::queue<Node*> targets;
	targets.push(m_root);
	while(!targets.empty())
	{
		Node *target = targets.front();
		targets.pop();

		if(target->m_left != NULL)
			targets.push(target->m_left);
		if(target->m_right != NULL)
			targets.push(target->m_right);

		delete target->m_value;
		delete target;
	}
}

template <class KeyType, class ValueType>
inline
int MyMap<KeyType, ValueType>::size() const 
{ 
	return m_size; 
}

template <class KeyType, class ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	// Add a root node
	if(m_root == NULL)
	{
		m_root = new Node(key, value);
		m_size++;
		return;
	}

	Node *temp = m_root;
	for(;;)
	{
		// The key already exists
		if(key == temp->m_key)
		{
			ValueType *newvalue = temp->m_value;
			*newvalue = value;
			return;
		}
		else if(key < temp->m_key)
		{
			if(temp->m_left != NULL)
				temp = temp->m_left;
			else
			{
				temp->m_left = new Node(key, value);
				m_size++;
				return;
			}
		}
		else if(key > temp->m_key)
		{
			if(temp->m_right != NULL)
				temp = temp->m_right;
			else
			{
				temp->m_right = new Node(key, value);
				m_size++;
				return;
			}
		}
	}
}

template <class KeyType, class ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
	Node *temp = m_root;

	while(temp != NULL)
	{
		if(key == temp->m_key)
			return temp->m_value;
		else if(key < temp->m_key)
			temp = temp->m_left;
		else
			temp = temp->m_right;
	}

	return NULL;
}

// Return the value of the root node
template <class KeyType, class ValueType>
ValueType* MyMap<KeyType, ValueType>::getFirst(KeyType& key)
{
	if(m_root == NULL)
		return NULL;
	else
	{
		key = m_root->m_key;
		m_level.push(m_root);
		return m_root->m_value; 
	}
}

// Print the nodes one by one in level order
template <class KeyType, class ValueType>
ValueType* MyMap<KeyType, ValueType>::getNext(KeyType& key)
{
	if(m_level.empty())
		return NULL;

	// Skip the root
	if(m_level.front() == m_root)
	{
		const Node* temp = m_level.front();
		m_level.pop();
		if(temp->m_left != NULL)
			m_level.push(temp->m_left);
		if(temp->m_right != NULL)
			m_level.push(temp->m_right);

		if(m_level.empty())
			return NULL;
	}
	
	const Node* temp = m_level.front();
	m_level.pop();

	if(temp->m_left != NULL)
		m_level.push(temp->m_left);
	if(temp->m_right != NULL)
		m_level.push(temp->m_right);

	key = temp->m_key;
	return temp->m_value;
}

#endif // MYMAP_INCLUDED
