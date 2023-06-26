#include <iostream>
#include <vector>

//sort string
void sort(std::vector<std::vector<std::string>> &v)
{
    for (int i = 0; i < v.size() - 1; i++)
    {
        for (int j = 0; j < v.size() - i - 1; j++)
        {
            if (std::stoi(v[j][1]) > std::stoi(v[j + 1][1]))
            {
                std::swap(v[j], v[j + 1]);
            }
        }
    }
}

//split string into array of string
void split(const std::string &originalString, char delimiter, std::vector<std::string> &arr)
{
    std::string::size_type i = 0;
    std::string::size_type j = originalString.find(delimiter);

    while (j != std::string::npos)
    {
        arr.push_back(originalString.substr(i, j - i));
        i = ++j;
        j = originalString.find(delimiter, j);
        if (j == std::string::npos)
        {
            arr.push_back(originalString.substr(i, originalString.length()));
        }
    }
}
