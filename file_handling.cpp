#include <iostream>
#include <map>
#include <fstream>
#include <string>

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


int main(int argc, char **argv)
{
    std::map<std::string, std::string> map = read_lines_from_file("/Users/bengoel/Documents/A5/db1.txt");
    for (const auto& elem : map)
    {
        std::cout << elem.first << " | " << elem.second << "\n";
    }
}
