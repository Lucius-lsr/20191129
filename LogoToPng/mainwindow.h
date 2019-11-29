#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "filereader.h"
#include "linkmaker.h"

struct Function;
struct Constant // 定义的常量
{
    std::string name; // 常量的名字
    int value; // 值
    int value2; // 可能用到的第二个值
    int value3; // 可能用到的第三个值
};
struct ActionNode;

struct Function // 函数
{
    std::string name;
    Constant con[100];//函数形参的结构数组
    ActionNode* firstAction=NULL;
};

struct Turtle
{
    int angle;
    QPoint position;
    bool cloak;
    QColor color;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void generatePicture();
    void paintPicture(ActionNode*);
    void savePicture();
    ActionNode* creat(int begin, int end,int god);
    int search(int i,int god);
    void TURN(int&);
    ~MainWindow();

public:
    FileReader reader;
    QImage* image;
    ActionNode* link;
    Turtle turtle;
    Constant cons[100]; // 全局变量的结构数组
    Function fun[100]; // 最多可以存100个函数
    int fc;//函数参数数列的下标
    int f;//下标
    int c;//全局变量数组的下标

private:
    Ui::MainWindow *ui;

};

struct Action // 动作对象
{
    std::string type; // 哪种类型
    int value; // 值
    int value2; // 可能用到的第二个值
    int value3; // 可能用到的第三个值
    Function* function; // 如果是调用函数，则保存函数的名称
    Constant* constant; // 如果是改动常量，则保存常量的名称
    ActionNode* firstAction; // 如果是循环，则保存循环链表的头指针
    int loopTimes; // 循环次数
    bool cloak;
};

struct ActionNode // 链表中的结点
{
    Action action;
    ActionNode* next;
};





#endif // MAINWINDOW_H
