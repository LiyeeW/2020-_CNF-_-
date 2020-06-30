#include "sudochoose.h"
#include <string.h>

#include <QMenuBar>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QRadioButton>
#include <random>
#include <cstdlib>
#include <ctime>

SudoChoose::SudoChoose(QWidget *parent) : QMainWindow(parent)
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

    //设置选项
    const QFont myFont = QFont("Adobe 仿宋 Std R");
    QRadioButton *level_nBtn;
    int8_t n;
    const QString levelText[3] = {"四阶","六阶","八阶"};
    for(int i=0;i<3;i++){
        n = 4+i*2;
        level_nBtn = new QRadioButton(levelText[i],this);
        level_nBtn->setFont(myFont);
        level_nBtn->setFixedSize(234,19);
        level_nBtn->move(186,100+40*i);
        if(i==0)
            level_nBtn->setChecked(true);
        connect(level_nBtn,&QPushButton::clicked,this,[=](){
            chooseLevel = n;
        });
    }
    QPushButton *yesBtn = new QPushButton("确认",this);
    yesBtn->setFont(myFont);
    yesBtn->setFixedSize(234,19);
    yesBtn->move(186,250);

//    //测试数据
//    given= new bool[16];
//    const bool G[16]={true,true,true,false,
//                      true,true,false,true,
//                      true,false,true,true,
//                      false,true,true,true};
//    for(int i=0;i<16;i++)
//        given[i]=G[i];
//    finish= new int[16];
//    const int F[16]={1,1,0,1,
//                    1,0,0,1,
//                    0,1,0,1,
//                    1,1,1,1};
//    for(int i=0;i<16;i++)
//        finish[i]=F[i];

    //设置跳转
    connect(yesBtn,&QPushButton::clicked,this,[=](){
        if(sudo2CNF(chooseLevel)==false)
            exit(-1);
        finish=sudoRES();
        res=(int*)calloc(chooseLevel*chooseLevel+1,sizeof(int));
        given=CNF2Sudo();
        hide();
        pageSudoPlay = new SudoPlay(parent,chooseLevel,given,finish);
        pageSudoPlay->show();
        deleteLater();
    });
}


bool SudoChoose::sudo2CNF(int level){

    fileName.setNum(level);
    fileName.append(".cnf");
    fileName.insert(0,"./sudogenerate");
    QFile file(fileName);
    QTextStream in(&file);

    //如果已存在，则直接返回true
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        file.close();
        return true;
    }

    //如果不存在，则生成
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)==false){
        return false;
    }

    int c2 = (level==4)?4:(level==6)?15:56;//组合数C(level/2+1,level),对应约束二
    int c3 = level*(level-1)/2;//组合数C(2，n),对应约束三
    int varNum=level*level+2*c3*(3*level+1);
    int clauNum=2*c2*2*level+(10*level+1)*c3*2;
    if(level!=4)
        clauNum+=2*(level-2)*2*level;

    //文件头
    in <<"c\np cnf "<<varNum<<" "<<clauNum<<endl;


    //约束一，level=4时约束二包含约束一，故跳过
    if(level!=4)
        for(int i=0;i+2<level;i++){
            for(int n=0;n<level;n++){
                in <<(i*level+n+1)<<" "<<((i+1)*level+n+1)<<" "<<((i+2)*level+n+1)<<" "<<0<<endl;
                in <<-(i*level+n+1)<<" "<<-((i+1)*level+n+1)<<" "<<-((i+2)*level+n+1)<<" "<<0<<endl;
                in <<(n*level+i+1)<<" "<<(n*level+i+2)<<" "<<(n*level+i+3)<<" "<<0<<endl;
                in <<-(n*level+i+1)<<" "<<-(n*level+i+2)<<" "<<-(n*level+i+3)<<" "<<0<<endl;
            }
        }

    //约束二
    for(int a=0;a<level;a++){
        for(int b=a+1;b<level;b++){
            for(int c=b+1;c<level;c++){
                if(level==4){
                    for(int n=0;n<level;n++){
                        in <<(a*level+n+1)<<" "<<(b*level+n+1)<<" "<<(c*level+n+1)<<" "<<0<<endl;
                        in <<-(a*level+n+1)<<" "<<-(b*level+n+1)<<" "<<-(c*level+n+1)<<" "<<0<<endl;
                        in <<(n*level+a+1)<<" "<<(n*level+b+1)<<" "<<(n*level+c+1)<<" "<<0<<endl;
                        in <<-(n*level+a+1)<<" "<<-(n*level+b+1)<<" "<<-(n*level+c+1)<<" "<<0<<endl;
                    }
                }
                else{
                    for(int d=c+1;d<level;d++){
                        if(level==6){
                            for(int n=0;n<level;n++){
                                in <<(a*level+n+1)<<" "<<(b*level+n+1)<<" "<<(c*level+n+1)<<" "<<(d*level+n+1)<<" "<<0<<endl;
                                in <<-(a*level+n+1)<<" "<<-(b*level+n+1)<<" "<<-(c*level+n+1)<<" "<<-(d*level+n+1)<<" "<<0<<endl;
                                in <<(n*level+a+1)<<" "<<(n*level+b+1)<<" "<<(n*level+c+1)<<" "<<(n*level+d+1)<<" "<<0<<endl;
                                in <<-(n*level+a+1)<<" "<<-(n*level+b+1)<<" "<<-(n*level+c+1)<<" "<<-(n*level+d+1)<<" "<<0<<endl;
                            }
                        }
                        else{
                            for(int e=d+1;e<level;e++){
                                for(int n=0;n<level;n++){
                                    in <<(a*level+n+1)<<" "<<(b*level+n+1)<<" "<<(c*level+n+1)<<" "<<(d*level+n+1)<<" "<<(e*level+n+1)<<" "<<0<<endl;
                                    in <<-(a*level+n+1)<<" "<<-(b*level+n+1)<<" "<<-(c*level+n+1)<<" "<<-(d*level+n+1)<<" "<<-(e*level+n+1)<<" "<<0<<endl;
                                    in <<(n*level+a+1)<<" "<<(n*level+b+1)<<" "<<(n*level+c+1)<<" "<<(n*level+d+1)<<" "<<(n*level+e+1)<<" "<<0<<endl;
                                    in <<-(n*level+a+1)<<" "<<-(n*level+b+1)<<" "<<-(n*level+c+1)<<" "<<-(n*level+d+1)<<" "<<-(n*level+e+1)<<" "<<0<<endl;
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    //约束三
    int curVar=level*level; //构造变元从level*level+1开始
    int litArrRow[level];
    int litArrCol[level];
    for(int i=0;i<level;i++){
        for(int j=i+1;j<level;j++){
            for(int n=0;n<level;n++){
                //行
                curVar++;
                in <<(i*level+n+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<(j*level+n+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<-(i*level+n+1)<<" "<<-(j*level+n+1)<<" "<<curVar<<" "<<0<<endl;
                curVar++;
                in <<-(i*level+n+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<-(j*level+n+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<(i*level+n+1)<<" "<<(j*level+n+1)<<" "<<curVar<<" "<<0<<endl;
                litArrRow[n]=++curVar;
                in <<-(curVar-2)<<" "<<curVar<<" "<<0<<endl;
                in <<-(curVar-1)<<" "<<curVar<<" "<<0<<endl;
                in <<(curVar-1)<<" "<<(curVar-2)<<" "<<-curVar<<" "<<0<<endl;

                //列
                curVar++;
                in <<(n*level+i+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<(n*level+j+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<-(n*level+i+1)<<" "<<-(n*level+j+1)<<" "<<curVar<<" "<<0<<endl;
                curVar++;
                in <<-(n*level+i+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<-(n*level+j+1)<<" "<<-curVar<<" "<<0<<endl;
                in <<(n*level+i+1)<<" "<<(n*level+j+1)<<" "<<curVar<<" "<<0<<endl;
                litArrCol[n]=++curVar;
                in <<-(curVar-2)<<" "<<curVar<<" "<<0<<endl;
                in <<-(curVar-1)<<" "<<curVar<<" "<<0<<endl;
                in <<(curVar-1)<<" "<<(curVar-2)<<" "<<-curVar<<" "<<0<<endl;
            }
            //（¬157∨¬1571∨¬1572∨…∨¬1578）∧（1571∨157）∧（1572∨157）…（1578∨157）
            curVar++;
            for(int n=0;n<level;n++){
                in <<litArrRow[n]<<" "<<curVar<<" "<<0<<endl;
                in <<litArrCol[n]<<" "<<(curVar+1)<<" "<<0<<endl;
            }
            for(int n=0;n<level;n++){
                in <<-litArrRow[n]<<" ";
            }
            in <<-(curVar)<<" "<<0<<endl;
            curVar++;
            for(int n=0;n<level;n++){
                in <<-litArrRow[n]<<" ";
            }
            in <<-(curVar)<<" "<<0;
            if(i!=level-2)          //文件末不输出换行符
                in <<endl;
        }
    }

    file.close();//完成没有初始赋值的cnf文件创建；
    return true;
}

bool* SudoChoose::CNF2Sudo(){
    //记录test结果
    solverEvent* sE;
    int *finishCopy=new int[chooseLevel*chooseLevel];
    memcpy(finishCopy,finish,chooseLevel*chooseLevel*sizeof(int));

    //记录每次test返回值
    bool mustGive=false;

    //记录每一格是否被test过
    bool *isTestedGrid=new bool[chooseLevel*chooseLevel];
    for(int i=0;i<chooseLevel*chooseLevel;i++)
        isTestedGrid[i]=false;

    int *waitTest=new int[chooseLevel*chooseLevel];
    for(int i=0;i<chooseLevel*chooseLevel;i++)
        waitTest[i]=i;
    int waitNum=chooseLevel*chooseLevel;

    //记录当前行被随机抽中的格
    int randGrid;

    while(waitNum>0){
        srand(time(nullptr));
        randGrid=rand()%waitNum;
        isTestedGrid[randGrid]=true;
        waitNum--;
        if(randGrid!=waitNum)
            waitTest[randGrid]=waitTest[waitNum];

        randGrid=waitTest[randGrid];

        finishCopy[randGrid]=-finish[randGrid];

        sE=new solverEvent;
        sE->filename=fileName;
        sE->level=chooseLevel;
        sE->result=&mustGive;
        sE->shouldBe=NULL;
        resTransfer(finishCopy);
        sE->varValue=res;
        sE->parserShow=NULL;
        solver(sE);
        delete(sE);

        if(!mustGive)
            finishCopy[randGrid]=-1;
        else
            finishCopy[randGrid]=finish[randGrid];
    }
    //根据最终的test结果生成given
    bool *Given=new bool[chooseLevel*chooseLevel];
    for(int i=0;i<chooseLevel;i++){
        if(finishCopy[i]==-1)
            Given[i]=false;
        else
            Given[i]=true;
    }
    return Given;
}



int* SudoChoose::sudoRES(){
    //设置随机种子
    srand(time(nullptr));
    int* value=(int*)calloc(3,sizeof(int));
    int** Res=(int**)calloc(1,sizeof(int*));
    int result;
    solverEvent* sE;
    QFile* myFile=new QFile(fileName);
    myFile->open(QIODevice::ReadOnly | QIODevice::Text);
    value[0]=2;
    for(int i=1;i<=value[0];i++){
        srand(time(nullptr));
        value[i]=rand()%(chooseLevel*chooseLevel*2)-chooseLevel*chooseLevel;
    }
    do{
        sE=new solverEvent;
        sE->filename=fileName;
        sE->level=chooseLevel;
        sE->result=NULL;
        sE->shouldBe=Res;
        sE->varValue=value;
        sE->parserShow=NULL;
        result=solver(sE);
        delete(sE);
    }while(result==ERROR);
    free(value);
    return *Res;

}


void SudoChoose::resTransfer(int* finishCopy){
    res[0]=0;
    for(int i=0;i<chooseLevel*chooseLevel;i++){
        if(finishCopy[i]==0)
            res[++res[0]]=-(i+1);
        else if(finishCopy[i]==1)
            res[++res[0]]=i+1;
    }
}
