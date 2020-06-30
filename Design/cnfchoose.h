#ifndef CNFCHOOSE_H
#define CNFCHOOSE_H

#include <QMainWindow>
#include "solver.h"
#include "mydialog.h"

class CNFchoose : public QMainWindow
{
    Q_OBJECT
public:
    explicit CNFchoose(QWidget *parent = nullptr);
    QString filename=NULL;
    QByteArray qByteArray;
    bool parserNeed=true;
    int** parserShow=NULL;
    char* parserString(solverEvent* sE);
signals:

};

#endif // CNFCHOOSE_H
