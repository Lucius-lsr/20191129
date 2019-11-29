#ifndef FILEREADER_H
#define FILEREADER_H
#include <iostream>

class FileReader
{
public:
    std::string norm[1000];
public:
    void readFile();
    int length;
};

#endif // FILEREADER_H
