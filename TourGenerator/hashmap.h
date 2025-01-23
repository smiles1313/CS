//
//  HashMap.h
//  cs32Project4
//
//  Created by Sophia Miles on 3/9/24.
//

#ifndef HashMap_h
#define HashMap_h

#include <functional>
#include <string>
#include <iostream>
#include <list>

template <typename T>
class HashMap
{
    
private:
    //----Node Struct-----
    struct Node  // use for values in each bucket
    {
        Node(std::string s, T val)    // Node constructor
            : key(s), value(val)
        {}
        
        std::string key;
        T value;
    };
    
    //-----Private Member Variables-----
    std::vector<std::list<Node>> m_hashMap;
    double m_maxLoad;
    unsigned int m_numBuckets;
    unsigned int m_numItems;
    
    //-----Private Member Functions-----
    unsigned int hashString(const std::string &hashMe) const // hasher
    {
        unsigned int bucketNum = std::hash<std::string>()(hashMe) % m_numBuckets;  // uses std hasher
        return bucketNum;  // the bucket that the key goes into
    }
    
    HashMap(const HashMap&);             // do nothing - per spec
    HashMap& operator=(const HashMap&);  // do nothing - per spec
    
    void rehash(int newCapacity) // called by the insert or [] func
    {
        std::vector<std::list<Node>> newMap(newCapacity);   // will be deleted upon leaving function scope
        m_numBuckets = newCapacity;  // update the number of buckets in hashMap - so rehash can work

        for (auto& oldBucket : m_hashMap)
        {
            for (auto& node : oldBucket)
            {
                unsigned int newBucket = hashString(node.key);
                newMap[newBucket].push_back(Node(node.key, node.value));
            }
        }
        m_hashMap = newMap;
    }
    
    
public:
 HashMap(double max_load = 0.75) // constructor
    : m_maxLoad(max_load)
    {
        if (max_load <= 0.0)    // checks if a non-positive max load was passed in
            m_maxLoad = 0.75;   // if so, make it default (0.75)
                        
        m_hashMap.resize(10);  // initializes vector to have 10 buckets
        m_numBuckets = 10;
        m_numItems = 0;  // number of nodes - aka key/value pairs in hashMap
    }
    
 ~HashMap() // destructor; deletes all of the items in the hashmap
    {}  // STL's list and vector destructors will be called
    
 int size() const // return the number of associations in the hashmap
    {
        return m_numItems;  // aka number of nodes in map
    }
    
    
 // The insert method associates one item (key) with another (value).
 // If no association currently exists with that key, this method inserts
 // a new association into the hashmap with that key/value pair. If there is
 // already an association with that key in the hashmap, then the item
 // associated with that key is replaced by the second parameter (value).
 // Thus, the hashmap must contain no duplicate keys.
 void insert(const std::string& key, const T& value)
    {
         unsigned int index = hashString(key);
         std::list<Node>& bucket = m_hashMap[index];
         
         // if key is already in map, change its value to the one passed in and return from function
         for (auto& node : bucket)
         {
             if (node.key == key)
             {
                 node.value = value;
                 return;
             }
         }

        // if key does not exist, add the key and value
         bucket.push_back(Node(key, value));
         m_numItems++;
     
         // Check for load factor and resize if necessary
         if ((static_cast<double>(m_numItems) / m_numBuckets) > m_maxLoad)  // static cast so can get double load factor, not int
             rehash(m_numBuckets * 2);
    }

    
 // Defines the bracket operator for HashMap, so you can use your map like this:
 // your_map["david"] = 2.99;
 // If the key does not exist in the hashmap, this will create a new entry in
 // the hashmap and map it to the default value of type T (0 for builtin types).
 // It returns a reference to the newly created value in the map.
T& operator[](const std::string& key)
    {
        unsigned int index = hashString(key);
        std::list<Node>& bucket = m_hashMap[index];

        // if key already exists, return that (is returned as a reference var)
        for (auto& node : bucket)
        {
            if (node.key == key)
                return node.value;
        }

        // If the key does not exist, create a new Node and insert it into the bucket
        bucket.push_back(Node(key, T()));
        m_numItems++;

        // Check for load factor and resize if necessary
        if (static_cast<double>(m_numItems) / m_numBuckets > m_maxLoad)
            rehash(m_numBuckets * 2);

        // Return a reference to the value of the newly inserted Node
        return *(find(key));  // works even if rehashed bc find calls hashString - not dependent on bucket from this

    }
    
 // If no association exists with the given key, return nullptr; otherwise,
 // return a pointer to the value associated with that key. This pointer can be
 // used to examine that value within the map.
 const T* find(const std::string& key) const
    {
        unsigned int hashBucket = hashString(key);
        const std::list<Node>& bucket = m_hashMap[hashBucket];
    
        for (const auto& node: bucket)
        {
            if (node.key == key)
               return &node.value;
        }
        
        return nullptr;
    }
    
    
 // If no association exists with the given key, return nullptr; otherwise,
 // return a pointer to the value associated with that key. This pointer can be
 // used to examine that value or modify it directly within the map.
 T* find(const std::string& key)
     {
         const auto& hm = *this;
         return const_cast<T*>(hm.find(key));
     }
     
};

#endif /* HashMap_h */




