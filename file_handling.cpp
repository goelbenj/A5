#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <list>
#include <vector>


std::list<std::pair<std::string, std::string>> read_lines_from_file(std::string filename)
{
   std::list<std::pair<std::string, std::string>> list{};
   std::ifstream inFile {filename};
   while (inFile)
   {
       std::string key{}, value{};
       {
            getline(inFile, key, ',') && getline(inFile, value);
            if(inFile)
            {
                    list.push_back({key, value});
            }
        }
   }
   return list;
}

// Simple hash function for demonstration purposes
size_t basicHash(const std::string &key, size_t tableSize) {
    size_t hashValue = 0;
    for (char ch : key) {
        hashValue = (hashValue * 31) + ch;
    }
    return hashValue % tableSize;
}

// Entry structure to hold key-value pairs
template <typename K, typename V>
struct HashEntry {
    K key;
    V value;
};

// HashTable class using linked lists for handling collisions
template <typename K, typename V>
class HashTable {
private:
    size_t tableSize;
    // std::list is implemented with a doubly-linked list
    std::vector<std::list<HashEntry<K, V>>> table;

public:
    HashTable(size_t size) : tableSize(size), table(size) {}

    // Insert a key-value pair into the hash table
    void insert(const K &key, const V &value) {
        size_t index = basicHash(key, tableSize);
        // for (auto &entry : table[index]) {
        //     if (entry.key == key) {
        //         // Update the value if the key already exists
        //         entry.value = value;
        //         return;
        //     }
        // }

        // If the key is not found, insert a new entry
        table[index].push_back({key, value});
    }

    // Retrieve the value associated with a given key
    V get(const K &key) {
        size_t index = basicHash(key, tableSize);
        for (const auto &entry : table[index]) {
            if (entry.key == key) {
                return entry.value;
            }
        }

        // Return a default value if the key is not found
        return V();
    }

    // Delete the entry associated with a given key
    void remove(const K &key) {
        size_t index = basicHash(key, tableSize);
        auto &bucket = table[index];

        // Use iterator to find and remove the entry
        auto it = std::find_if(bucket.begin(), bucket.end(),
                               [key](const HashEntry<K, V> &entry) { return entry.key == key; });

        if (it != bucket.end()) {
            bucket.erase(it);
        }
    }

    // Display the contents of the hash table
    void display() {
        for (size_t i = 0; i < tableSize; ++i) {
            std::cout << "Bucket " << i << ": ";
            for (const auto &entry : table[i]) {
                std::cout << "(" << entry.key << ", " << entry.value << ") ";
            }
            std::cout << std::endl;
        }
    }
};

int main(int argc, char **argv)
{
    // Create line list from file
    std::list<std::pair<std::string, std::string>> list = read_lines_from_file("/Users/bengoel/Documents/A5/db1.txt");

    // Create a hash table with a size of 10
    HashTable<std::string, std::string> hashTable(10);

    // Insert key-value pairs
    for (const auto& elem : list)
    {
        std::cout << elem.second << std::endl;
        hashTable.insert(elem.first, elem.second);
    }

    // Display the initial contents of the hash table
    hashTable.display();
}
