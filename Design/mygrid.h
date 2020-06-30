#ifndef MYGRID_H
#define MYGRID_H

#include <QLabel>
#include <QMouseEvent>


class MyGrid : public QLabel
{
    Q_OBJECT
public:
    explicit MyGrid(QWidget *parent = nullptr,int level=0,int id=-1,bool given=true,int shouldBe=-1);
    int status=2;   //2表示当前不显示，0表示显示0，1表示显示1
    void mousePressEvent(QMouseEvent *event);
signals:
    void changed();
    void isRignt(bool b);
};



#endif // MYGRID_H
