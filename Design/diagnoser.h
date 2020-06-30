#ifndef DIAGNOSER_H
#define DIAGNOSER_H

#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include "solverdata.h"
#include "decider.h"


int diagnoser(solverData* D);
int backer(solverData* D);
int recaller(solverData* D, decision* P);
void watch_back(solverData* D, int bottom);
void borrow_U(solverData* D, watch* wp);


#endif // DIAGNOSER_H
