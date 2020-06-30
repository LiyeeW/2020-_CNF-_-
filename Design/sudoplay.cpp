#include "sudoplay.h"

#include <QPainter>
#include <QMenuBar>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QRadioButton>
#include <QLabel>
#include <QPalette>
#include <QTimer>


SudoPlay::SudoPlay(QWidget *parent, int level, bool *givenGrid, int *finishedGrid) : QMainWindow(parent)
{

    if(!level)
        return;

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
    actionInfo->setParent(this);
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

    //布置棋盘
    residue=level*level;
    for(int i=0;i<level*level;i++){
        grid = new MyGrid(this,level,i,givenGrid[i],finishedGrid[i]);
        if(givenGrid[i])
            residue--;
        connect(grid,&MyGrid::isRignt,this,&SudoPlay::countRight);
    }

    //判断结束
    QMessageBox* finishedMsgBox = new QMessageBox(this);
    finishedMsgBox->setWindowTitle("胜利");
    finishedMsgBox->setText("游戏结束，点击确认返回主页面");
    connect(this,&SudoPlay::finished,this,[=](){
        QTimer *timer = new QTimer(this);
        timer->start(2000);
        finishedMsgBox->exec();
        finishedMsgBox->setAttribute(Qt::WA_DeleteOnClose);
        hide();
        parent->show();
        deleteLater();
    });
}


void SudoPlay::countRight(bool b){
    if(b)
        residue--;
    else
        residue++;
    if(!residue)
        emit finished();
    qDebug() <<residue;
};
