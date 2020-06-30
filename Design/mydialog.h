#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

class MyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MyDialog(QWidget *parent = nullptr,char* string=NULL);
    void setText(char* newString=NULL);
    QTextEdit *textShow;
    QPushButton *yesBtn;
signals:

};

#endif // MYDIALOG_H
