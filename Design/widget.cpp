#include "widget.h"
#include "ui_widget.h"
#include <QMenuBar>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置主菜单
    QMenuBar *menu = new QMenuBar(this);
    setMenuBar(menu);
    QMenu *pageMenu = menu->addMenu("界面");
    QMenu *infoMenu = menu->addMenu("信息");

    //设置菜单一：主页面
    QAction *actionHome = pageMenu->addAction("主页面");
    actionHome->setParent(this);
    actionHome->setDisabled(true);

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

    //设置数独选择页面


    //设置选择按钮
    ui->sudoBtn->setChecked(true);
    connect(ui->sudoBtn,&QPushButton::clicked,this,[=](){
        homeChoose=true;
    });
    connect(ui->cnfBtn,&QPushButton::clicked,this,[=](){
        homeChoose=false;
    });
    connect(ui->yesBtn,&QPushButton::clicked,this,[=](){
        pageSudoChoose = new SudoChoose(this);
        if(homeChoose){
            this->hide();
            pageSudoChoose->show();
        }
        else{
            pageCnfChoose=new CNFchoose(this);
            this->hide();
            pageCnfChoose->show();
        }

    });


}

Widget::~Widget()
{
    delete ui;
}

