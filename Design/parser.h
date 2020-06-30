#ifndef PARSER_H
#define PARSER_H
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include "solverdata.h"
#include "decider.h"
#include <QString>
#include <QFile>

int parser(QString fileName, solverData** Dp);
QFile* opener(QString fileName);
watch* watch_init(int index, int pal, int lit);
variable* varriable_init(int n);
solverData* clause_fix(QFile* fp);
int into_U_M(solverData* D, int i);
int DoWatch_M(watch* w0, solverData* D);
int bcp_M(solverData* D);
void ranker(solverData* D);
void heap_fix(solverData* D, int i, int n);
void heapify(solverData* D);
void R_transfer(solverData* D);
int un_rep(clause* C, int head, int self);
solverData* clause_copy(QString fileName);







#endif // PARSER_H
