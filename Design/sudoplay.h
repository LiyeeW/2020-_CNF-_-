#ifndef SUDOPLAY_H
#define SUDOPLAY_H

#include <QMainWindow>

#include "mygrid.h"

class SudoPlay : public QMainWindow
{
    Q_OBJECT
public:
    explicit SudoPlay(QWidget *parent = nullptr, int level=0, bool* givenGrid=nullptr, int* finishedGrid=nullptr);
    bool IsRight();
    MyGrid *grid;
    int residue=-1;
    void countRight(bool b);
signals:
    void finished();
};

#endif // SUDOPLAY_H
