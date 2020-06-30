#ifndef WIDGET_H
#define WIDGET_H

#include "sudochoose.h"
#include "cnfchoose.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    SudoChoose* pageSudoChoose;
    CNFchoose* pageCnfChoose;/*
    QMainWindow* pageCnfSolve;*/
    bool homeChoose=true;

private:
    Ui::Widget *ui;
    Ui::Widget *ui2;
};
#endif // WIDGET_H
