#include<QFile>
#include<QBuffer>
#include <QPainter>
#include <cmath>
#include<cstring>
#include<iostream>
#include<sstream>
#include <string>
#include <fstream>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*------------------初始化---------------*/
    fc = 0;//函数参数数列的下标
    f = 0;//下标
    c = 1;//全局变量数组的下标
    cons[0].name = "angle";
    cons[0].value = 90;//y轴正向
    reader.length = 0;
    turtle.color=QColor(0,0,0);
    turtle.cloak=true;
    /*------------------读取logo文件---------------*/
    reader.readFile();
    qDebug()<<reader.length;
    /*----------------初始化----------------*/
    generatePicture();
    /*----------------构造链表----------------*/
    ActionNode* mainHead = creat(0, reader.length,-1);
    ActionNode* current = mainHead;
    while (current)
    {
        qDebug() << QString::fromStdString(current->action.type) << current->action.value << " ";
        if(current->action.type=="LOOP")
        {
            ActionNode* loopcurrent = current->action.firstAction;
            while (loopcurrent)
            {
                qDebug() << QString::fromStdString(loopcurrent->action.type) << loopcurrent->action.value << " ";
                loopcurrent = loopcurrent->next;
            }
        }
        current = current->next;
    }

    /*---------------绘图-----------------*/
    paintPicture(mainHead);
    ui->label->setPixmap(QPixmap::fromImage(*image));
    /*------------------保存------------------*/
    savePicture();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int trans( string y) {
    stringstream ss;
    int x;
    ss << y;
    ss >> x;
    return x;
}

int MainWindow::search(int i,int god) {
    for (int m = 0; m < c; m++) {
        if (reader.norm[i + 1] == cons[m].name) {
            return cons[m].value;
        }
        for (int m = 0; m < 100; m++) {
            if (reader.norm[i + 1] == fun[god].con[m].name) {
                return fun[god].con[m].value;

            }
        }
    }
}

ActionNode* MainWindow::creat(int begin, int end,int god) {//god为-1表示为主函数调用，不然表示为调用fun[god]，这个量方便搜索对应的形参数组
    ActionNode* head = new ActionNode;
    head->action.type = "NULL";
    ActionNode* tail = head;
    for (int i = begin; i <= end; i++) {
        if (reader.norm[i] == "MOVE") // move
        {
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "MOVE";
            //判断是变量还是数字
            if (reader.norm[i + 1][0] > '0' && reader.norm[i + 1][0] <= '9') {
                newNode->action.value = trans(reader.norm[i + 1]);
            }
            else {
                newNode->action.value  =search(i,god);
            }
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i++;
        }
        else if (reader.norm[i] == "DEF")
        {
            if (god == -1) {
                cons[c].name = reader.norm[i + 1];
                cons[c].value = trans(reader.norm[i + 2]);
                c++;
                i = i + 2;
            }//如果是在调用函数里面定义参数
            else {
                fun[god].con[fc].name= reader.norm[i + 1];
                fun[god].con[fc].value = trans(reader.norm[i + 2]);
            }
        }
        else if (reader.norm[i] == "ADD") {
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "ADD";
            newNode->action.value = trans(reader.norm[i + 2]);
            bool get=false;//在给定数组下找到某变量
            for (int m = 0; m < c; m++) {
                if (reader.norm[i + 1] == cons[m].name) {
                    newNode->action.constant = &cons[m];
                    get = true;
                    break;
                }
            }
            if (!get) {
                for (int m=0; m < 100; m++) {
                    if (reader.norm[i + 1] == fun[god].con[m].name) {
                        newNode->action.constant = &fun[god].con[m];//指向某个被调用函数的某个参数项
                        break;
                    }
                }
            }
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i = i + 2;
        }
        else if (reader.norm[i] == "TURN") {
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "TURN";
            if (reader.norm[i + 1][0] > '0' && reader.norm[i + 1][0] <= '9') {
                newNode->action.value = trans(reader.norm[i + 1]);
            }
            else {
                newNode->action.value = search(i, god);
            }
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i = i + 1;
        }
        else if (reader.norm[i] == "COLOR") {//color
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "COLOR";
            if (reader.norm[i + 1][0] > '0' && reader.norm[i + 1][0] <= '9') {
                newNode->action.value = trans(reader.norm[i + 1]);
            }
            else {
                newNode->action.value = search(i, god);
            }
            if (reader.norm[i + 2][0] > '0' && reader.norm[i + 2][0] <= '9') {
                newNode->action.value2 = trans(reader.norm[i + 2]);
            }
            else {
                newNode->action.value2 = search(i+1, god);
            }
            if (reader.norm[i + 3][0] > '0' && reader.norm[i + 3][0] <= '9') {
                newNode->action.value3 = trans(reader.norm[i + 3]);
            }
            else {
                newNode->action.value3 = search(i+2, god);
            }
            newNode->action.cloak = true;
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i = i + 3;
        }
        else if (reader.norm[i] == "CLOAK") {//cloak
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "CLOAK";
            newNode->action.cloak = false;
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
        }
        else if (reader.norm[i]=="FUNC") {
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "FUNC";
            int m,mm=100;//函数名及变量名的提取
            for (m = 0; m <reader.norm[i + 1].length(); m++) {
                if (reader.norm[i + 1][m] == '(') {
                    fun[f].name = reader.norm[i+1].substr(0, m-1);
                    mm = m;
                }if (m > mm && (reader.norm[i+1][m] == ' ' || reader.norm[i+1][m] == ')')) {
                    fun[f].con[fc].name = reader.norm[i+1].substr(mm+1, m-1);
                    fc++;
                    mm = m;
                }
            }
            //函数体链表的生成：
            int mark = 1;
            int t = i + 2;
            while (mark != 0)
            {
                if (reader.norm[t] == "FUNC" && reader.norm[t - 1] != "END")
                {
                    mark++;
                }
                else if (reader.norm[t] == "END" && reader.norm[t + 1] == "FUNC")
                {
                    mark--;
                }
                t++;
            }
            newNode->action.firstAction = creat(i + 2, t - 2,f);
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i = t;
            f++;
        }
        else if (reader.norm[i] == "CALL") {
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "CALL";
            int m, mm = 100, t, b = 0;//函数名的提取与对应变量的赋值
            string functionName;
            for (m = 0; m <reader.norm[i + 1].length(); m++) {
                if (reader.norm[i + 1][m] == '(') {
                    functionName = reader.norm[i + 1].substr(0, m-1);
                    mm = m;
                    for (int l = 0; l < f; l++) {
                        if (functionName == fun[l].name) {
                            newNode->action.function = &fun[l];
                            t = l;
                            break;

                        }
                    }
                }if (m > mm && (reader.norm[i + 1][m] == ' ' || reader.norm[i + 1][m] == ')')) {//对应参数的赋值
                    fun[t].con[b].value = trans(reader.norm[i+1].substr(mm+1, m-1));
                    b++;
                    mm = m;
                }


            }
        }
        else if (reader.norm[i] == "LOOP") {//loop
            ActionNode* newNode = new ActionNode;
            newNode->action.type = "LOOP";
            if (reader.norm[i + 1][0] > '0' && reader.norm[i + 1][0] <= '9') {
                newNode->action.loopTimes = trans(reader.norm[i + 1]);
            }
            else {
                newNode->action.loopTimes = search(i, god);
            }
            int mark = 1;
            int t = i + 2;
            while (mark != 0)
            {
                if (reader.norm[t] == "LOOP" && reader.norm[t - 1] != "END")
                {
                    mark++;
                }
                else if (reader.norm[t] == "END" && reader.norm[t + 1] == "LOOP")
                {
                    mark--;
                }
                t++;
            }
            newNode->action.firstAction = creat(i + 2, t - 2,-1);
            ActionNode* cur=newNode->action.firstAction;
            while(cur)
            {
                cur=cur->next;
            }
            tail->next = newNode;
            tail = tail->next;
            tail->next = NULL;
            i = t;
        }
    }
    return head;
}

void MainWindow::generatePicture()
{
    int pointer=0;
    int sizeX=100,sizeY=100;
    int bgRed=255,bgGreen=255,bgBlue=255;
    int posX=50,posY=50;
    for(pointer=0;pointer<10;pointer++)
    {
        if(reader.norm[pointer]=="@SIZE")
        {
            sizeX=trans(reader.norm[pointer+1]);
            sizeY=trans(reader.norm[pointer+2]);
        }
        else if(reader.norm[pointer]=="@BACKGROUND")
        {
            bgRed=trans(reader.norm[pointer+1]);
            bgGreen=trans(reader.norm[pointer+2]);
            bgBlue=trans(reader.norm[pointer+3]);
        }
        else if(reader.norm[pointer]=="@POSITION")
        {
            posX=trans(reader.norm[pointer+1]);
            posY=trans(reader.norm[pointer+2]);
        }

    }
    image=new QImage(sizeX,sizeY,QImage::Format_RGB32);
    image->scaled(sizeX,sizeY);
    image->fill(QColor(bgRed,bgGreen,bgBlue));

    turtle.angle=0;
    turtle.position=QPoint(posX,posY);
    turtle.cloak=true;
}

void MainWindow::paintPicture(ActionNode* link)
{
    ActionNode* current=link;
    while(current)
    {
        if(current->action.type=="MOVE")
        {            
            QPoint newPosition(turtle.position.x()+current->action.value*cos(turtle.angle*3.1415926/180),
                               turtle.position.y()+current->action.value*sin(turtle.angle*3.1415926/180));
            if(turtle.cloak) // 非隐身状态绘图
            {
                QPainter painter(image);
                QPen pen(QColor(0,0,0), 3);
                painter.setPen(pen);
                QLine line(turtle.position,newPosition);
                painter.drawLine(line);
            }
            turtle.position=newPosition;
        }
        else if(current->action.type=="TURN")
        {
            turtle.angle+=current->action.value;
        }
        else if(current->action.type=="LOOP")
        {
            for(int i=0;i<current->action.loopTimes;i++)
            {
                paintPicture(current->action.firstAction);
            }
        }
        else if(current->action.type=="NULL")
        {
            ;
        }
        else if(current->action.type=="ADD")
        {
            current->action.constant->value+=current->action.value;
        }
        else if(current->action.type=="COLOR")
        {
            turtle.color.setRgb(current->action.value,current->action.value2,current->action.value3);
            turtle.cloak=true;
        }
        else if(current->action.type=="CLOAK")
        {
            turtle.cloak=false;
        }
        else if(current->action.type=="CALL")
        {
            paintPicture(current->action.function->firstAction);
        }
        current=current->next;
    }
}

void MainWindow::savePicture()
{
    QFile file("imageoutput.png");//创建一个文件对象，存储用户选择的文件
    if (!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    //----------------------------以流方式写入文件-------------------------
    QByteArray ba;
    QBuffer buffer(&ba);
    image->save(&buffer, "PNG");//把图片以流方式写入文件缓存流中
    buffer.open(QIODevice::WriteOnly);
    file.write(ba);//将流中的图片写入文件对象当中
}
