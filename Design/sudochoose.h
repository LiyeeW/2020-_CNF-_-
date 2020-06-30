#ifndef SUDOCHOOSE_H
#define SUDOCHOOSE_H

#include <QMainWindow>

#include "sudoplay.h"
#include "solver.h"

class SudoChoose : public QMainWindow
{
    Q_OBJECT
public:
    explicit SudoChoose(QWidget *parent = nullptr);
    QString fileName;
    int chooseLevel=4;
    bool *given;
    int *finish;
    SudoPlay* pageSudoPlay;
    bool sudo2CNF(int level);
    bool* CNF2Sudo();
    int* sudoRES();
    int* res;
    void resTransfer(int* finishCopy);
signals:

};

#endif // SUDOCHOOSE_H
