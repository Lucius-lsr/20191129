#include<iostream>
#include<sstream>
#include <string>
#include <fstream>
#include <QDebug>

#include "filereader.h"

void FileReader::readFile() {
    length = 0;//p用于统计所有的字符数目
    std::ifstream  in;
    in.open("test1.txt");//双引号内文件名
    if(!in)
    {
        qDebug()<<"wrong";
    }
    for (int i = 0; i < 1000; i++) {
        in >> norm[i];
        //qDebug()<<QString::fromStdString(norm[i])<<endl;
        length++;
        if (norm[i] == "EXIT")//遇到EXIT便结束读写操作
            break;
    }
    in.close();
}
