#include "mygrid.h"


MyGrid::MyGrid(QWidget *parent,int level,int id,bool given,int shouldBe) : QLabel(parent)
{
    setFixedSize(30,30);
    move((600-(level+1)*30)/2+(id%level)*30,(400-(level+1)*30)/2+(id/level)*30);
    setAlignment(Qt::AlignCenter);

    if(given){
        setStyleSheet("border:6px solid rgb(0, 200, 0);");
        setNum(shouldBe);
        emit isRignt(true);
    }
    else{
        setStyleSheet("border:2px solid rgb(0, 200, 0);");
        connect(this,&MyGrid::changed,this,[=](){
            bool old=status==shouldBe;
            status=(status+1)%3;
            if(status==2){
                clear();
                setStyleSheet("border:2px solid rgb(0, 200, 0);");
                if(old)
                    emit isRignt(false);
            }
            else {
                if(status!=shouldBe){
                    setStyleSheet("border:6px solid rgb(200, 0, 0);");
                    if(old)
                        emit isRignt(false);
                }
                else{
                    setStyleSheet("border:2px solid rgb(0, 200, 0);");
                    emit isRignt(true);
                }
                setNum(status);
            }
        });
    }

}


void MyGrid::mousePressEvent(QMouseEvent *event){
    emit MyGrid::changed();
}

