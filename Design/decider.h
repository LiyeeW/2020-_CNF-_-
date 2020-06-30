#ifndef DECIDER_H
#define DECIDER_H

#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include "solverdata.h"


int into_U(solverData* D, int i, int old, watch* id, const char type);
int DoWatch(watch* w0, solverData* D, const char type);
int bcp(solverData* D, const char type);
int decider(solverData* D);
int picker(solverData* D);
int R_find(solverData* D);
void R_fix(solverData* D);
void watch_away(solverData* D, watch* w0);
void pal_away(solverData* D, watch* wp);
void watch_set(solverData* D, watch* wp, int litc);
void lmax_set(literal* lp);
void DoDuty(solverData* D, int result);

#endif // DECIDER_H
