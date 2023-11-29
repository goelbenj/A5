#include <iostream>
#include <map>
#include <mutex>
#include <fstream>
#include <string>
#include <list>
#include <thread>
#include <vector>

#define TABLE_SIZE 10
#define NUM_FILES 4

// mapping list lock
std::mutex m;

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

    // Retrieve the element associated with a given key
    std::list<HashEntry<K, V>> get(const K &key)
    {
        size_t index = djb2Hash(key, tableSize);
        return table[index];
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
    HashTable<K, V> reduce(const K word)
    {
        // Index element with word
        auto element = get(word);
        // Create new hash table to contain reduced key-value pairs
        HashTable<K, V> newTable(TABLE_SIZE);
        // Only reduce elements with key-value pairs
        if (element.size())
        {
            V count = std::to_string(element.size());
            K key = element.front().key;
            newTable.insert(key, count);
        }

        return newTable;
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
void parse_hash(std::string file_name, std::list<HashTable<K, V>> mapping_vector)
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

    // Add hash table to mapping vector
    m.lock();
    mapping_vector.push_back(hashTable);
    m.unlock();
}

template <typename K, typename V>
std::list<HashTable<K, V>> create_mapping_threaded(const std::list<std::string> files)
{
    std::list<HashTable<K, V>> mapping_list{};

    // perform simultaneous insertions using global lock
    std::vector<std::thread> threads{};
    int i = 0;
    for (const auto& file_name : files) {
        threads[i] = std::thread(parse_hash<K, V>, file_name, mapping_list);
        i++;
    }

    return mapping_list;
}

template <typename K, typename V>
void merge_mappings(HashTable<K, V> &table1, HashTable<K, V> table2, K word)
{
    // Get element corresponding to word
    auto element = table2.get(word);

    // Add each node into table1
    for (const auto &node : element)
    {
        table1.insert(node.key, node.value);
    }
}

void unoptimized_main(std::list<std::string> files, std::string word)
{
    // Create mapping for each file
    auto mapping_list = create_mapping<std::string, std::string>(files);

    // Merge all mappings into one
    HashTable<std::string, std::string> majorTable = mapping_list.front();
    mapping_list.pop_front();
    for (; !mapping_list.empty(); mapping_list.pop_front())
    {
        // Perform merge
        merge_mappings(majorTable, mapping_list.front(), word);
    }

    // Reduce map
    auto newTable = majorTable.reduce(word);
    newTable.display();
}

void multi_threaded_main(std::list<std::string> files, std::string word)
{
    // Create mapping for each file
    auto mapping_list = create_mapping_threaded<std::string, std::string>(files);

    // Merge all mappings into one
    HashTable<std::string, std::string> majorTable = mapping_list.front();
    mapping_list.pop_front();
    for (; !mapping_list.empty(); mapping_list.pop_front())
    {
        // Perform merge
        merge_mappings(majorTable, mapping_list.front(), word);
    }

    // Reduce map
    auto newTable = majorTable.reduce(word);
    newTable.display();
}

int main(int argc, char **argv)
{
    std::string word;
    if (argc > 1){
        word = argv[1];
        std::cout << "Performing Map Reduction for " << word << std::endl;
    }
    else {
        std::cout << "No word specified, performing map reduction for all words" << std::endl;
    }
    
    const std::list<std::string> files = {
        "db1.txt",
        "db2.txt",
        "db3.txt",
        "db4.txt",
    };

    // Run unoptimized reduced-map
    auto start = std::chrono::high_resolution_clock::now();
    // unoptimized_main(files, word);
    multi_threaded_main(files, word);
    auto stop = std::chrono::high_resolution_clock::now();
    double diff = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
    diff *= 1e-9;
    std::cout << "Total duration: " << std::setw(9) << diff << " seconds\n";
}
