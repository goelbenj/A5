#include <iostream>
#include <map>
#include <fstream>
#include <string>

void read_lines_from_file()
{
   std::map<std::string, std::string> myMap{};
   std::ifstream inFile {"/Users/bengoel/Documents/A5/db1.txt"};
   while (inFile)
   {
       std::string key{}, value{};
       {
            getline(inFile, key, ',') && getline(inFile, value);
            if(inFile)
            {
                    myMap[key] = value;
            }
        }
   }
   for (const auto& elem : myMap)
   {
       std::cout << elem.first << " | " << elem.second << "\n";
   }
}


int main(int argc, char **argv)
{
    read_lines_from_file();
}
