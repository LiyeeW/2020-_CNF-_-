#include "mydialog.h"
#include <QTextEdit>




MyDialog::MyDialog(QWidget *parent,char* string) : QDialog(parent)
{
    const QFont myFont = QFont("Adobe 仿宋 Std R");
    textShow=new QTextEdit(string,this);
    textShow->setFont(myFont);
    textShow->setFixedSize(390,260);
    setFixedSize(400,300);
    setWindowTitle("已完成:解析内容");
    yesBtn=new QPushButton("确认",this);
    yesBtn->setFont(myFont);
    yesBtn->setFixedSize(100,19);
    yesBtn->move(130,270);
    connect(yesBtn,&QPushButton::clicked,this,[=](){
        close();
    });
}

void MyDialog::setText(char* newString){
    textShow->setText(newString);
}
