#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <list>
#include <vector>


std::map<std::string, std::string> read_lines_from_file(std::string filename)
{
   std::map<std::string, std::string> map{};
   std::ifstream inFile {filename};
   while (inFile)
   {
       std::string key{}, value{};
       {
            getline(inFile, key, ',') && getline(inFile, value);
            if(inFile)
            {
                    map[key] = value;
            }
        }
   }
   return map;
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
        for (auto &entry : table[index]) {
            if (entry.key == key) {
                // Update the value if the key already exists
                entry.value = value;
                return;
            }
        }

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

// int main() {
//     // Create a hash table with a size of 10
//     HashTable<std::string, int> hashTable(10);

//     // Insert some key-value pairs
//     hashTable.insert("one", 1);
//     hashTable.insert("two", 2);
//     hashTable.insert("three", 3);

//     // Display the initial contents of the hash table
//     std::cout << "Initial Hash Table:" << std::endl;
//     hashTable.display();

//     // Retrieve and display values associated with keys
//     std::cout << "Value for key 'two': " << hashTable.get("two") << std::endl;

//     // Update the value for an existing key
//     hashTable.insert("two", 22);

//     // Display the updated contents of the hash table
//     std::cout << "Updated Hash Table:" << std::endl;
//     hashTable.display();

//     // Remove an entry from the hash table
//     hashTable.remove("one");

//     // Display the final contents of the hash table
//     std::cout << "Final Hash Table:" << std::endl;
//     hashTable.display();

//     return 0;
// }

int main(int argc, char **argv)
{
    // Create line map from file
    std::map<std::string, std::string> map = read_lines_from_file("/Users/bengoel/Documents/A5/db1.txt");

    // Create a hash table with a size of 10
    HashTable<std::string, std::string> hashTable(10);

    // Insert key-value pairs
    for (const auto& elem : map)
    {
        hashTable.insert(elem.first, elem.second);
    }

    // Display the initial contents of the hash table
    hashTable.display();
}
