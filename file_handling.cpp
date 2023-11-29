#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <list>
#include <vector>

#define TABLE_SIZE 10

std::list<std::pair<std::string, std::string>> read_lines_from_file(std::string filename)
{
    std::list<std::pair<std::string, std::string>> list{};
    std::ifstream inFile{filename};
    while (inFile)
    {
        std::string key{}, value{};
        {
            getline(inFile, key, ',') && getline(inFile, value);
            if (inFile)
            {
                list.push_back({key, value});
            }
        }
    }
    return list;
}

// Hash function for demonstration purposes
size_t djb2Hash(const std::string &key, size_t tableSize)
{
    size_t hashValue = 5381; // Initial hash value
    for (char ch : key)
    {
        hashValue = (hashValue << 5) + hashValue + ch; // hash * 33 + ch
    }
    return hashValue % tableSize;
}

// Entry structure to hold key-value pairs
template <typename K, typename V>
struct HashEntry
{
    K key;
    V value;
};

// HashTable class using linked lists for handling collisions
template <typename K, typename V>
class HashTable
{
private:
    size_t tableSize;
    // std::list is implemented with a doubly-linked list
    std::vector<std::list<HashEntry<K, V>>> table;

public:
    HashTable(size_t size) : tableSize(size), table(size) {}

    // Insert a key-value pair into the hash table
    void insert(const K &key, const V &value)
    {
        size_t index = djb2Hash(key, tableSize);

        // If the key is not found, insert a new entry
        table[index].push_back({key, value});
    }

    // Retrieve the value associated with a given key
    V get(const K &key)
    {
        size_t index = djb2Hash(key, tableSize);
        for (const auto &entry : table[index])
        {
            if (entry.key == key)
            {
                return entry.value;
            }
        }

        // Return a default value if the key is not found
        return V();
    }

    // Delete the entry associated with a given key
    void remove(const K &key)
    {
        size_t index = djb2Hash(key, tableSize);
        auto &bucket = table[index];

        // Use iterator to find and remove the entry
        auto it = std::find_if(bucket.begin(), bucket.end(),
                               [key](const HashEntry<K, V> &entry)
                               { return entry.key == key; });

        if (it != bucket.end())
        {
            bucket.erase(it);
        }
    }

    // Display the contents of the hash table
    void display()
    {
        for (size_t i = 0; i < tableSize; ++i)
        {
            std::cout << "Bucket " << i << ": ";
            for (const auto &entry : table[i])
            {
                std::cout << "\n    - ";
                std::cout << "(" << entry.key << ", " << entry.value << ") ";
            }
            std::cout << std::endl;
        }
    }

    std::pair<std::vector<std::list<HashEntry<std::string, std::string>>>::iterator, std::vector<std::list<HashEntry<std::string, std::string>>>::iterator> iterator()
    {
        return std::make_pair(table.begin(), table.end());
    }

    // Reduce each element by counting number of entries
    // and add this new node to end of element
    void reduce()
    {
        for (size_t i = 0; i < tableSize; ++i)
        {
            std::list<HashEntry<K, V>> &element = table[i];
            // Only reduce elements with key-value pairs
            if (element.size())
            {
                V count = std::to_string(element.size());
                K key = element.front().key;
                element.push_back({key, count});
            }
        }
    }
};

template <typename K, typename V>
std::list<HashTable<K, V>> create_mapping(const std::list<std::string> files)
{
    std::list<HashTable<K, V>> mapping_list{};
    for (const auto &file_name : files)
    {
        // Create line list from file
        std::list<std::pair<std::string, std::string>> list = read_lines_from_file(file_name);

        // Create a hash table with a size of TABLE_SIZE
        HashTable<K, V> hashTable(TABLE_SIZE);

        // Insert key-value pairs
        for (const auto &elem : list)
        {
            hashTable.insert(elem.first, elem.second);
        }

        // Add hash table to mapping list
        mapping_list.push_back(hashTable);
    }

    return mapping_list;
}

template <typename K, typename V>
void merge_mappings(HashTable<K, V> &table1, HashTable<K, V> table2)
{
    auto iterator = table2.iterator();
    // Iterate through linked lists and merge
    for (; iterator.first != iterator.second; iterator.first++)
    {
        // Add each node into table1
        for (const auto &node : *iterator.first)
        {
            table1.insert(node.key, node.value);
        }
    }
}

int main(int argc, char **argv)
{
    const std::list<std::string> files = {
        "/Users/bengoel/Documents/A5/db1.txt",
        "/Users/bengoel/Documents/A5/db2.txt",
        "/Users/bengoel/Documents/A5/db3.txt",
        "/Users/bengoel/Documents/A5/db4.txt",
    };

    // Create mapping for each file
    auto mapping_list = create_mapping<std::string, std::string>(files);

    // Merge all mappings into one
    HashTable<std::string, std::string> majorTable = mapping_list.front();
    mapping_list.pop_front();
    for (; !mapping_list.empty(); mapping_list.pop_front())
    {
        // Perform merge
        merge_mappings(majorTable, mapping_list.front());
    }

    // Reduce map
    majorTable.reduce();
    majorTable.display();
}
