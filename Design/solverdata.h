#ifndef SOLVERDATA_H
#define SOLVERDATA_H

#include<stdlib.h>


#define mySwap(a,b)  temp=(a);(a)=(b);(b)=temp
#define random(x) (rand()%(x))
#define OK 0
#define ERROR -1
#define LITSATISFIED -2
#define SATISFIED -3
#define UNSATISFIED -4


struct watch {              //变元对子句的“监视”
    int id;                 //子句首间隔点或尾间隔点的序号
    int pal;                //另一个的序号
    int lit;                //记录被监视的文字在子句中的序号
    watch* next;
};

struct decision {           //取值结点，包含决策结点和bcp结点两类
    int value;              //取值
    int level;              //决策结点显示决策层数，bcp结点显示0
    decision* parent;       //决策结点指向上一个决策结点，bcp结点指向同层的决策结点
    decision* last;         //决策结点指向该层最后一个bcp结点，bcp结点指向上一个赋值结点
    int duty;               //存放使该结点赋值的子句结点的U序号
    watch sw;               //存放该层满足的子句的一个结点，仅决策结点有sw->next
    int belong;             //标注是否依赖于同层的已被取反变量
};

struct rank {               //记录排名
    int var;                //记录变元序号
    int max;                //记录该变元当前分数
    rank* next;             //链表的下一个(更低分数)
    rank* last;             //链表的上一个（更高分数）
};

struct literal {            //统计各文字的得分
    int neg;                //正文字的得分
    int pos;                //负文字的得分
    int choice;             //较高者的序号，0为neg，1为pos
    rank* rp;
};

struct variable {           //变元信息表
    decision* dp;           //一直放在变量表中，solver模块结束时才释放
    watch wp;               //监视链表的头结点
    literal lp;             //决策排序时的指标
};

struct clause {             //记录字句和文字关系的结构
    int lit;                //子句结点的lit为某个文字结点的C序号，文字结点的lit为文字值
    watch* wp;              //子句结点的wp一直挂载着，记录信息，文字结点的wp为NULL
};

union R {
    int var;
    rank* rp;
};

struct solverData {			//在solver模块使用的数据库
    clause* C;              //子句的结构数组
    variable* V;            //变元的结构数组
    R* R;					//指rank，以literal.max为标准对变元序号降序排列
    int* U;					//指Unit，对minibcp是目标文字，对bcp是目标单子句的分割结点序号
    decision* N;			//指Now，存放当前层的决策结点的地址
    decision* null;			//作为一个NULL的代替，便于第一层的取反
    decision* BD;			//从bcp产生，由backer使用的目标取反决策变量
    int* dutyC;				//用于分析回溯
    int* testV;				//用于分析回溯
};

int solver_destroy(solverData* D);




#endif // SOLVERDATA_H
