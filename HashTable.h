//
//  HashTable.h
//  Project 4
//
//  Created by Ravi Jayanthi on 3/6/15.
//  Copyright (c) 2015 Ravi Jayanthi. All rights reserved.
//

#ifndef Project_4_HashTable_h
#define Project_4_HashTable_h

#include <iostream>
using namespace std;

template <typename KeyType, typename ValueType>
class HashTable
{
public:
    HashTable(unsigned int numBuckets, unsigned int capacity) // Hashtable constructor
    {
        m_numBuckets = numBuckets; // Set the private member variable equal to passed in value for use later
        m_capacity = capacity;
        m_assoc = 0;
        m_mostRecentlyAdded = nullptr;  // Currently there are no most or least recently added
        m_leastRecentlyAdded = nullptr;
        m_head = new Node*[numBuckets];
        for (int i = 0; i < numBuckets; i++) // Initialize the array of pointers to linked lists equal to nullptr
            m_head[i] = nullptr;
    }
    ~HashTable();
    bool isFull() const;
    bool set (const KeyType& key, const ValueType& value, bool permanent = false);
    bool get (const KeyType& key, ValueType& value) const;
    bool touch (const KeyType& key);
    bool discard (KeyType& key, ValueType& value);
private:
    HashTable (const HashTable&);
    HashTable& operator= (const HashTable&);
    unsigned int getBucketForKey(const KeyType& key) const; // Private Helper Method
    
    struct Node
    {
        Node (KeyType key, ValueType val) // Node constructor that everytime a new Node is constructed, so all these values are never unitialized
        {
            next = nullptr;
            prev = nullptr;
            historyNext = nullptr; // Unless I change it, the connecting pointers will be null
            historyPrev = nullptr;
            m_key = key;
            m_value = val;
            m_permanent = false; // We automatically make the permanence of each Node false
        }
        Node* historyPrev; // this is the value added just BEFORE the current one
        Node* historyNext; // this is the value added just AFTER the current one
        Node* next;
        Node* prev;
        KeyType m_key;
        ValueType m_value;
        bool m_permanent;
    };
    
    Node* m_mostRecentlyAdded;
    Node* m_leastRecentlyAdded;
    
    Node** m_head;
    unsigned int m_capacity;
    unsigned int m_assoc;
    unsigned int m_numBuckets;
};

template <typename KeyType, typename ValueType>
inline
unsigned int HashTable<KeyType, ValueType>::getBucketForKey(const KeyType& key) const
{
    unsigned int computeHash(KeyType); // prototype
    unsigned int determineBucketFromHash(KeyType); // prototype
    
    unsigned int result = computeHash(key); // Get the hash value for a specific key
    unsigned int bucketNum = result % m_numBuckets; // Mod by the number of buckets to find which bucket specifically
    return bucketNum;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
//    if (isFull())
//        return false;
    
    unsigned int tempIndex = getBucketForKey(key); // Find the specific bucket in the hashtable for key
    Node* cur = m_head[tempIndex];  // set the cur pointer to that bucket's pointer to its linked list of Nodes
    
    // If there is nothing in the bucket's linked list
    // It would be because the pointer in the area that points to this bucket's linked list is nullptr (i.e. it is empty)
    if (cur == nullptr)
    {
        if (isFull())
            return false;
        //cerr << "Added first value in new Bucket" << endl;
        Node* newNode = new Node(key, value);   // Create a new Node and pass in the proper parameters
        m_head[tempIndex] = newNode;
        newNode->m_permanent = permanent;   // Set the permanence
        newNode->prev = nullptr;
        newNode->next = nullptr;
        
        if (!newNode->m_permanent) // We only change the history (recency) if the permanence is false
        {
            newNode->historyPrev = nullptr;
            newNode->historyNext = m_mostRecentlyAdded; // This node is now going to be preceded by the previously most recently added
            if (m_mostRecentlyAdded != nullptr)
                m_mostRecentlyAdded->historyPrev = newNode;
            m_mostRecentlyAdded = newNode;  // Now we set our Node to the most recently added
            if (m_leastRecentlyAdded == nullptr)
                m_leastRecentlyAdded = newNode;
        }
        m_assoc++;
        return true;
    }
    
    while (cur->next != nullptr)    // Loop through and see if the key has been found in our table
    {
        //cerr << "Searching table..." << endl;
        if (cur->m_key == key)
        {
            cur->m_value = value; // Set the value in the node equal to what we want to setr
            //cerr << "Updating bucket -- A" << endl;
            if (!cur->m_permanent)
            {
                if (cur == m_leastRecentlyAdded)
                {
                    cur->historyNext = m_mostRecentlyAdded; // its now the newest thing in history, so it will have nothing newer after it
                    m_leastRecentlyAdded->historyNext = nullptr; // make the previously least recently added thing in history point its next to current
                    cur->historyPrev = m_leastRecentlyAdded; // have the current value point to the previously least recently added in history
                    m_mostRecentlyAdded->historyPrev = cur; // the previously most recently added thing will have this before it in history
                    m_mostRecentlyAdded = cur; // the most recently added thing in history is now this!
                    m_mostRecentlyAdded->historyPrev = nullptr; // there is nothing before our new most recently added thing in history
                    
                }
                else // execute this if the value is anything after the least recently added
                {
                    // if the current node is pointing to the last thing in the node
                    if (cur->historyNext == nullptr)
                        cur->historyPrev->historyNext = nullptr; // make the value before cur point its next to null (skip cur)
                    else
                    {
                        // if the node in question is somewhere "landlocked" between other nodes, connect the ones before & after it
                        cur->historyNext->historyPrev = cur->historyPrev;
                        cur->historyPrev->historyNext = cur->historyNext;
                    }
                    // make this values previous value point to the most recently added
                    cur->historyNext = m_mostRecentlyAdded;
                    m_mostRecentlyAdded->historyPrev = cur; // the previously most recently added's should have its prev now point to rear
                    m_mostRecentlyAdded = cur; // now reset the most recently added
                    cur->historyPrev = nullptr;
                    cur->historyPrev = m_leastRecentlyAdded;
                }
            }
            return true;    // Don't let the rest of the code execute
        }
        cur = cur->next;
    }
    if (cur->m_key == key)  // Because now cur is pointing to the last Node, check it because we didn't in the while loop.
    {
        cur->m_value = value;
        if (!cur->m_permanent)
        {
            if (cur == m_leastRecentlyAdded) // If the current node we're updating is the most recently added
            {
                m_leastRecentlyAdded = cur->historyPrev; // the cur is being placed directly in front of the least recently added
                m_leastRecentlyAdded->historyNext = nullptr; // because we are resetting, the most least recently added
                m_leastRecentlyAdded->historyPrev = cur->historyPrev;
                cur->historyNext = m_mostRecentlyAdded; // its now the newest thing in history, so it will have nothing newer after it
                m_mostRecentlyAdded->historyPrev = cur; // the previously most recently added thing will have this before it in history
                m_mostRecentlyAdded = cur; // the most recently added thing in history is now this!
                m_mostRecentlyAdded->historyPrev = nullptr; // there is nothing before our new most recently added thing in history
            }
            else
            {
                // if the current node is pointing to the last thing in the node
                if (cur->historyNext == nullptr && cur->historyPrev != nullptr)
                    cur->historyPrev->historyNext = nullptr; // make the value before cur point its next to null (skip cur)
                
                cur->historyNext = m_mostRecentlyAdded;
                (m_mostRecentlyAdded)->historyPrev = cur;
                m_mostRecentlyAdded = cur;
                cur->historyPrev = nullptr;
            }
        }
        return true;
    }
    
    if (isFull())
        return false;
    
    //cerr << "Added new value in new Bucket -- B" << endl;
    Node* newNode = new Node(key, value);   // Create a new Node and pass in the proper parameters
    newNode->m_permanent = permanent;   // Set the permanence
    newNode->prev = cur;    // Set the new Node's previous equal to the cur.
    newNode->next = nullptr;
    if (cur != nullptr){
        cur->next = newNode;    // Put the cur's (pointer to last Node now) next equal to the new Node just created
    }
    if (!newNode->m_permanent)
    {
        newNode->historyPrev = nullptr; // due to this node new location it will not be preceded by anything
        newNode->historyNext = m_mostRecentlyAdded; // the value after it will be what was the most recently added before the change
        if (m_mostRecentlyAdded != nullptr){
            m_mostRecentlyAdded->historyPrev = newNode;
        }
        m_mostRecentlyAdded = newNode; // now reset the most recently added
    }
    m_assoc++;
    return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
    unsigned int tempIndex = getBucketForKey(key);
    Node* cur = m_head[tempIndex];
    
    // After setting cur equal to the pointer to the specific linked list
    while (cur !=  nullptr)
    {
        if (cur->m_key == key)
        {
            value = cur->m_value;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard( KeyType& key, ValueType& value)
{
    Node* cur = m_leastRecentlyAdded;
    if (m_leastRecentlyAdded == nullptr) // if the linked list of nodes for this bucket is empty
        return false;
    key = cur->m_key;
    value = cur->m_value;
    
    if (cur == m_mostRecentlyAdded && cur == m_leastRecentlyAdded) // if its the only value in the linked list (most recently & least recently added)
    {
        m_mostRecentlyAdded = nullptr;
        m_leastRecentlyAdded = nullptr;
        return true;
    }
    
    if (cur->prev == nullptr) // If we're trying to delete the first value in the linked list 
    {
        m_head[getBucketForKey(cur->m_key)] = cur->next;
        if (m_head[getBucketForKey(cur->m_key)] != nullptr)
            m_head[getBucketForKey(cur->m_key)]->prev = nullptr;
    }
    else if (cur->next != nullptr) // If we're trying to delete something in the middle of the linked list (not either edge)
    {
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
    }
    else // If it's the last value in the linked list
        cur->prev->next = nullptr;
    
    if (cur->historyPrev != nullptr) // Put inside a nullptr check
        cur->historyPrev->historyNext = nullptr;
    
    m_leastRecentlyAdded = cur->historyPrev;
    delete cur;
    m_assoc--;
    return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
    unsigned int tempIndex = getBucketForKey(key);
    Node* cur = m_head[tempIndex];
    
    while (cur != nullptr) // While the current Node being pointed to is valid
    {
        if (!(cur->m_permanent) && cur->m_key == key) // And the permanence is false and the key matches up
        {
            if (cur->historyNext == nullptr) // If this is currently the last Node in the recency list (most recently added)
            {
                if (cur->historyPrev != nullptr) // Make sure its also not the first
                {
                    cur->historyPrev->historyNext = nullptr;
                    m_leastRecentlyAdded = cur->historyPrev;
                }
            }
            else
            {
                if (cur->historyPrev != nullptr) // If this is just a Node somewhere in the list
                {
                    cur->historyNext->historyPrev = cur->historyPrev;
                    cur->historyPrev->historyNext = cur->historyNext;
                }
            }
            cur->historyNext = m_mostRecentlyAdded;
            m_mostRecentlyAdded->historyPrev = cur;
            m_mostRecentlyAdded = cur; // Set the most recently added to be this node after pointer rearranging
            cur->historyPrev = nullptr;
            return true;
        }
        cur = cur->next; // keep iterating forward
    }
    return false;
}

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
    // Same type of destructor logic as Multiset
    for (int i = 0; i < m_numBuckets; i++)
    {
        Node* p = m_head[i];
        while (p != nullptr)
        {
            Node* n = p->next;  // Use temp variable to delete things so you can save iterator location
            delete p;
            p = n;
        }
    }
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isFull() const
{
    return (m_assoc == m_capacity);
}




#endif
