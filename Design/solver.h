#ifndef SOLVER_H
#define SOLVER_H
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include "solverdata.h"
#include "parser.h"
#include "decider.h"
#include "diagnoser.h"
#include <QString>
#include <QFile>

struct solverEvent {
    QString filename;         //CNF文件名
    int** parserShow;        //非空时，solver传给它C数组
    int* varValue;          //非空时，它传给solver给定的文字值
    bool* result;           //非空时，solver传给它结果
    int** shouldBe;         //非空时，solver给它传解
    int level;              //若为sudo调用，此为阶数
};


int solver(solverEvent* sE);
int* clauseCpy(clause* C,int clauNum);
int varVal(solverData* D,int* val);
int* sudoVar(solverData* D,solverEvent* sE);
int writeRES(solverData* D,QString filename,clock_t time,int result);

#endif // SOLVER_H
