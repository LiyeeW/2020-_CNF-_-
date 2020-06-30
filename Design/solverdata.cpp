#include "solverdata.h"

int solver_destroy(solverData* D) {
    int i,j;
    if (!D) return OK;
    if (D->C) {
        i = D->C[0].lit;
        if(D->C[1].wp)
            for (j = 0; j < i; j++)
                if (D->C[j].wp)
                    free(D->C[j].wp);
        free(D->C);
    }
    if (D->null)
        free(D->null);
    if (D->R)
        free(D->R);
    if (D->U)
        free(D->U);
    if (D->V) {
        i = D->V[0].wp.id;
        for (j = 1; j <= i; j++)
            free(D->V[j].dp);
        free(D->V);
    }
    if (D->dutyC)
        free(D->dutyC);
    if (D->testV)
        free(D->testV);
    free(D);
    return OK;
}
