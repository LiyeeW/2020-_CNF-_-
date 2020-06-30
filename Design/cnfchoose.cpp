#include "cnfchoose.h"

#include <QMenuBar>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <random>
#include <cstdlib>
#include <ctime>
#include <QLineEdit>
#include <QFileDialog>
#include <QByteArray>

CNFchoose::CNFchoose(QWidget *parent) : QMainWindow(parent)
{
    //设置主菜单
    QMenuBar *menu = new QMenuBar(this);
    setMenuBar(menu);
    QMenu *pageMenu = menu->addMenu("界面");
    QMenu *infoMenu = menu->addMenu("信息");

    //设置菜单一：主页面
    QAction *actionHome = pageMenu->addAction("主页面");
    actionHome->setParent(this);
    connect(actionHome,&QAction::triggered,this,[=](){
        this->hide();
        parent->show();
        deleteLater();
    });

    //设置菜单二：退出
    QAction *actionQuit = pageMenu->addAction("退出");
    connect(actionQuit,&QAction::triggered,this,[=](){
        close();
    });
    actionQuit->setParent(this);

    //设置菜单三：个人信息
    QAction *actionInfo = infoMenu->addAction("个人信息");
    actionInfo->setParent(this);/*
    QDialog *dialogInfo = new QDialog(this);
    dialogInfo->
    dialogInfo->setWindowTitle("Student Information");
    */
    QMessageBox* infoMsgBox = new QMessageBox(this);
    infoMsgBox->setWindowTitle("个人信息");
    infoMsgBox->setText("班级\tCS1805\n学号\tU201814643\n姓名\t王俪晔");
    connect(actionInfo,&QAction::triggered,infoMsgBox,[=](){
        infoMsgBox->exec();
        infoMsgBox->setAttribute(Qt::WA_DeleteOnClose);
    });

    //设置标题尺寸
    setWindowTitle("数据结构课程设计");
    setFixedSize(600,400);

    //设置文件选择
    const QFont myFont = QFont("Adobe 仿宋 Std R");

    QLineEdit* fileChoose=new QLineEdit(this);
    fileChoose->setFont(myFont);
    fileChoose->setFixedSize(240,20);
    fileChoose->move(175,100+40*0);
    fileChoose->setDisabled(true);

    QPushButton *fileBtn=new QPushButton("选择CNF文件",this);
    fileBtn->setFont(myFont);
    fileBtn->setFixedSize(100,19);
    fileBtn->move(430,100+40*0);
    connect(fileBtn,&QPushButton::clicked,this,[=](){
       filename=QFileDialog::getOpenFileName(this,"选择CNF文件","C:/","*.cnf *.CNF");
       fileChoose->setText(filename);
    });

    QRadioButton *parserBtn;
    const QString levelText[2] = {"执行后显示解析内容","执行后不显示解析内容"};
    for(int i=0;i<2;i++){
        parserBtn = new QRadioButton(levelText[i],this);
        if(i==0)
            parserBtn->setChecked(true);
        parserBtn->setFont(myFont);
        parserBtn->setFixedSize(240,19);
        parserBtn->move(175,140+40*i);
        connect(parserBtn,&QPushButton::clicked,this,[=](){
            parserNeed = (i==0)?true:false;
        });
    }
    QPushButton *yesBtn = new QPushButton("确认",this);
    yesBtn->setFont(myFont);
    yesBtn->setFixedSize(234,19);
    yesBtn->move(186,250);

    QPushButton *backBtn = new QPushButton("返回",this);
    backBtn->setFont(myFont);
    backBtn->setFixedSize(234,19);
    backBtn->move(186,290);
    connect(backBtn,&QPushButton::clicked,this,[=](){
        this->hide();
        parent->show();
        deleteLater();
    });

    QMessageBox* errorMsgBox = new QMessageBox(this);
    errorMsgBox->setWindowTitle("有误");
    errorMsgBox->setText("文件有误或含中文路径，请重试");

    QMessageBox* okMsgBox = new QMessageBox(this);
    okMsgBox->setWindowTitle("已完成");
    okMsgBox->setText("res文件已保存在cnf文件目录中");




    //执行solver
    connect(yesBtn,&QPushButton::clicked,this,[=](){
        if(filename==NULL){
            errorMsgBox->exec();
            this->hide();
            parent->show();
            deleteLater();
        }
        else{
            solverEvent* sE=new solverEvent;
            sE->filename=filename;
            sE->shouldBe=nullptr;
            sE->parserShow=nullptr;
            if(parserNeed){
                parserShow=(int**)calloc(1,sizeof(int*));
                sE->parserShow=parserShow;
            }
            sE->varValue=nullptr;
            sE->result=nullptr;
            if(solver(sE)==ERROR){
                errorMsgBox->exec();
                this->hide();
                parent->show();
                deleteLater();
            }
            else if(parserNeed){
                MyDialog* parserText=new MyDialog(this,NULL);
                char* string=parserString(sE);
                parserText->setText(string);
                parserText->exec();
                free(string);
                this->hide();
                parent->show();
                deleteLater();
            }
            else{
                okMsgBox->exec();
                this->hide();
                parent->show();
                deleteLater();
            }
            free(sE);
            free(parserShow);
            parserShow=nullptr;
        }
    });
}

char* CNFchoose::parserString(solverEvent* sE){
    int* c=*(sE->parserShow);
    int clauNum=c[1];
    int length=c[0];
    int selfLenth=0;
    char* string=(char*)calloc(6*length,sizeof(char));
    if(!string)
        exit(-1);
    for(int i=2;i<=length;i++){
        if(c[i]){
            sprintf(string, "%s%d ", string, c[i]);
            if(abs(c[i])>=1000) selfLenth+=5;
            else if(abs(c[i])>=100) selfLenth+=4;
            else if(abs(c[i])>=10) selfLenth+=3;
            else if(abs(c[i])>=1) selfLenth+=2;
            if(c[i]<0) selfLenth+=1;
        }
        else{
            sprintf(string, "%s0\n", string);
            selfLenth+=2;
            i++;
            clauNum--;
            if(!clauNum){
                string[selfLenth]=0;
                realloc(string,(selfLenth+1)*sizeof (char));
                if(!string)
                    exit(-1);
                return string;
            }
        }
    }
    return string;
}
