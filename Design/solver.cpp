#include "solver.h"

#include <QDebug>


/*********解CNF主函数**********/
int solver(solverEvent* sE) {
    solverData* D;
    clock_t time = clock();
    int result = parser(sE->filename, &D);//初始化并解现存单子句
    if (result == ERROR)
        return result;
    if(sE->parserShow)          //如果要求显示解析结果
        *(sE->parserShow)=clauseCpy(D->C, D->V[0].wp.lit);
    if(sE->varValue)            //如果有被传入的预定值
        result=varVal(D,sE->varValue);
    while (1) {
        if(result==SATISFIED||result==UNSATISFIED)
            break;
        if(result==OK)              //正常则继续decide
            result=decider(D);
        else                        //冲突则diagnose
            result = diagnoser(D);
    }
    time = clock() - time;
    if(sE->result)                  //表示只需要输出有解还是无解
        *(sE->result)=(result==SATISFIED)?true:false;
    else if(sE->varValue){          //表示只需要输出shouldBe
        if(result==SATISFIED)
            *(sE->shouldBe)=sudoVar(D,sE);
        else{
            solver_destroy(D);
            return ERROR;
        }
    }
    else
        if(writeRES(D,sE->filename,time,result)!=OK){
            solver_destroy(D);
            return ERROR;
        }
    solver_destroy(D);
    return OK;
}



int* clauseCpy(clause* C,int clauNum){
    int* c=(int*)calloc(C[0].lit,sizeof (int));
    if(!c)
        exit(-1);
    for(int i=1;i<=C[0].lit;i++)
        c[i]=C[i].lit;
    c[0]=C[0].lit;
    c[1]=clauNum;
    return c;
}


int varVal(solverData* D,int* val){
    for(int i=1;i<=val[0];i++){
        D->U[++D->U[0]]=val[i];
        D->V[abs(val[i])].dp->value=val[i];
    }
    return bcp_M(D);
}


int* sudoVar(solverData* D,solverEvent* sE){
    int* s=(int*)calloc(sE->level*sE->level+1,sizeof(int));
    for(int i=0;i<sE->level*sE->level;i++)
        s[i]=(D->V[i+1].dp->value>0)?1:0;
    return s;
}

QString Res_filename(QString CNF) {
    QString res=QString(CNF);
    int i;
    for (i = 0;; i++) {
        res[i] = CNF[i];
        if (CNF[i] == '.' &&(( CNF[i + 1] == 'c' && CNF[i + 2] == 'n' && CNF[i + 3] == 'f')|| (CNF[i + 1] == 'C' && CNF[i + 2] == 'N' && CNF[i + 3] == 'F'))) {
            res[i + 1]='r';
            res[i + 2]='e';
            res[i + 3]='s';
            break;
        }
    }
    return res;
}


int writeRES(solverData* D,QString filename,clock_t time,int result){
    QFile* fp1=new QFile(filename);
    filename=Res_filename(filename);
    QFile* fp=new QFile(filename);
    if(fp1==fp)
        exit(62);
    QTextStream in(fp);
    fp->open(QIODevice::WriteOnly | QIODevice::Text);
    if(fp->handle()==-1)
        return ERROR;
    int s=(result==SATISFIED)?1:0;
    in<<"s "<<s<<"\nv";
    if(s)
        for (int i = 1; i <= D->V[0].wp.id; i++)
            in<<" "<<D->V[i].dp->value;
    in<<"\nt "<<time;
    fp->close();
    return OK;
}
