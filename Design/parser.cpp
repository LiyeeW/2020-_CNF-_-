#include "parser.h"
#include <QDebug>


/*********初始化主函数**********/
int parser(QString fileName, solverData** Dp) {
    QFile* fp = opener(fileName);
    if (!fp)
        return ERROR;				//文件打开失败则返回ERROR
    *Dp = clause_fix(fp);				//建立完整的子句数组
    if (!*Dp) {
        *Dp = clause_copy(fileName);	//完善解析显示功能
        return UNSATISFIED;
    }
    if (bcp_M(*Dp) == UNSATISFIED)//如果cnf本身的单子句有冲突，则无解
        return UNSATISFIED;
    ranker(*Dp);
    R_find(*Dp);
    if ((*Dp)->V[(*Dp)->R[0].rp->next->var].dp->value) //如果排序结果最优先者也是已赋值，说明所有变元均被赋值
        return SATISFIED;
    return OK;
}



/*********打开用户输入的文件**********/
QFile* opener(QString fileName) {
    QFile *fp=new QFile(fileName);
    if (!fp||!fp->exists())
        return NULL; //如果文件不存在
    fp->open(QIODevice::ReadOnly | QIODevice::Text);
    if(fp->handle()==-1)
        return NULL; //如果打开失败
    char c;
    fp->getChar(&c);
    while(c=='c'){ //忽略批注
        fp->readLine();
        fp->getChar(&c);
    }
    for (int i = 0; i < 4; i++){ //忽略“ cnf” 四个字符
        fp->getChar(&c);
    }
    return fp;
}


/*********建立初始化的变元数组**********/
variable* varriable_init(int n) {
    int i;
    variable* V = (variable*)calloc(n + 1, sizeof(variable));
    if (!V)
        exit(-1);
    V[0].wp.id = n;
    for (i = 1; i <= n; i++) {
        V[i].dp = (decision*)calloc(1, sizeof(decision));
        if (!V[i].dp)
            exit(-1);
        V[i].lp.rp = (rank*)calloc(1, sizeof(rank));
        if (!V[i].lp.rp)
            exit(-1);
        V[i].lp.rp->var = i;
    }
    return V;
}


/*********初始化一个watch结点**********/
watch* watch_init(int index, int pal, int lit) {
    watch* w = (watch*)malloc(sizeof(watch));
    w->id = index;
    w->pal = pal;
    w->lit = lit;
    return w;
}


/*********查重并去重同一子句内的文字**********/
int un_rep(clause* C, int head, int self) {
    for (head = head + 1; head < self; head++) {
        if (C[head].lit == C[self].lit)
            return -1;
        if (C[head].lit == -C[self].lit)
            return 1;
    }
    return 0;
}


/*********建立完整的子句数组**********/
solverData* clause_fix(QFile* fp) {
    watch* w;
    variable* v;
    clause* c;
    QByteArray data;
    int debug2=0;
    int result;
    int BASE = 30;						//对文字优先度的评估因子
    int i = 2, j, base, lit, head = 1;	//head指子句的分割头结点
    solverData* D = (solverData*)calloc(1, sizeof(solverData));
    if (!D)
        exit(-1);
    int var_num = 0, clau_num = 0;			//变元数  子句数
    data=fp->readLine();
    QTextStream out(&data);
    out>>var_num;
    out>>clau_num;
    data=fp->readAll();
    QTextStream Out(&data);
    D->U = (int*)calloc(2 * (clau_num + 1), sizeof(int));
    if (!D->U)
        exit(-1);
    D->U[0] = 1;						//栈顶，先加后进栈
    D->U[1] = 2;						//当前栈底
    D->null = (decision*)calloc(1, sizeof(decision));
    if (!D->null)
        exit(-1);
    D->N = D->null;
    int lit_num = 0;
    int add_volume = clau_num;
    int clau_volume = 1 + add_volume;	//子句数组的当前容量
    D->C = (clause*)calloc(clau_volume, sizeof(clause));
    if (!D->C)
        exit(-1);
    D->V = varriable_init(var_num);
    if (!D->V)
        exit(-1);
    D->V[0].wp.lit=clau_num;
    debug2=0;
    while (1) {							//文件尾时的return是循环的唯一出口
        for (; i < clau_volume - 2; i++) {
            Out>>(D->C[i].lit);
            result = un_rep(D->C, head, i);
            if (result == -1) {
                i--;
                continue;				//如果与同一子句的文字重复，则不计该文字
            }
            else if (result == 1) {
                i = 1;
                while (i) {
                    Out>>i;
                }
                clau_num--;				//剩余子句数减一
                i = head;
                continue;				//如果同一子句的文字相反，则不计该子句
            }
            if (!D->C[i].lit) {			//读到了子句尾
                clau_num--;				//剩余子句数减一
                if (lit_num == 1) {		//如果该子句为单子句
                    result = into_U_M(D, D->C[i - 1].lit);  //目标文字入U栈
                    if (result == UNSATISFIED) {
                        solver_destroy(D);
                        return NULL;
                    }
                    i -= 2;				//清除对该子句的记录
                    head = i;
                    lit_num = 0;
                    continue;
                }
                //若不是单子句，则加入监视处理，i即为tail
                w = watch_init(head, i, head + 1);
                if (!w)
                    exit(-1);
                D->C[head].wp = w;
                lit = abs(D->C[head + 1].lit);
                w->next = D->V[lit].wp.next;
                D->V[lit].wp.next = w;
                w = watch_init(i, head, i - 1);
                if (!w)
                    exit(-1);
                D->C[i].wp = w;
                lit = abs(D->C[i - 1].lit);
                w->next = D->V[lit].wp.next;
                D->V[lit].wp.next = w;
                //若不是单子句，再加入对文字的统计和变元对子句的记录
                base = BASE / lit_num;	//该子句的评估因子
                for (j = head + 1; j < i; j++) {
                    lit = D->C[j].lit;
                    v = &(D->V[abs(lit)]);
                    if (lit > 0) v->lp.pos += base;
                    else v->lp.neg += base;
                }
                if (!clau_num) {		//读到文件尾时，返回D
                    D->C[0].lit = i + 1;	//缩减多余的C的空间，只留下当前的i+1个
                    c = (clause*)realloc(D->C, (i + 1) * sizeof(clause));
                    if (!c)
                        exit(-1);
                    D->C = c;
                    D->dutyC = (int*)calloc(i, sizeof(int));
                    if (!D->dutyC)
                        exit(-1);
                    D->testV = (int*)calloc(var_num+1, sizeof(int));
                    if (!D->testV)
                        exit(-1);
                    for (i = 1; i <= var_num; i++)
                        lmax_set(&(D->V[i].lp));
                    fp->close();
                    return D;
                }
                //若没有读到文件尾，则准备下一个子句的读取
                D->C[++i].lit = 0;
                head = i;
                lit_num = 0;
            }
            else {
                lit_num++;
                D->C[i].wp = NULL;
            }
        }
        clau_volume += add_volume;		//扩容
        c = (clause*)realloc(D->C, clau_volume * sizeof(clause));
        if (!c)
            exit(-1);
        D->C = c;
    }
}


/*********M型BCP**********/
int bcp_M(solverData* D) {
    watch* w;
    int result, i;
    for (i = 2; i <= D->U[0]; i++) {							//对U的内容逐一DoWatch
        w = &(D->V[abs(D->U[i])].wp);
        while (w->next) {				//如果有被该变元监控的子句
            result = DoWatch_M(w, D);
            if (result == UNSATISFIED)	//来自mini型的不满足
                return UNSATISFIED;
        }
    }
    D->U[0] = 1;						//M型需要清栈
    return OK;
}


/*********M型dowatch**********/
int DoWatch_M(watch* w0, solverData* D) {
    watch* w1 = w0->next;				//w1是处理对象，为id结点寻找新的lit
    int lit1, lit2;
    int another = D->C[w1->pal].wp->lit;//another是pal结点的lit，不可被重复指向，其是否为自由文字并不确定，因为赋值与DoWatch为异步
    int start = w1->id;					//暂时取id序号
    int end = w1->pal;					//暂时取pal序号
    start = (end > start) ? start + 1 : end + 1;//使start为较小者的下一个序号
    end = (start == end + 1) ? w1->id - 1 : end - 1;//使end为较大者的上一个序号
    while (start <= end) {
        if (start == another) {			//跳过another，避免重复
            start++;
            continue;
        }
        lit1 = D->C[start].lit;			//取出文字
        lit2 = D->V[abs(lit1)].dp->value;//取出对应变元的赋值
        if (lit2 == lit1) {				//如果被满足
            w1->lit = 0;
            w0->next = w1->next;		//M型直接脱离
            pal_away(D, w1);			//销零并脱离pal结点
            return OK;
        }
        if (!lit2) {					//找到一个最近的自由文字
            w1->lit = start;			//对应新自由文字的序号
            w0->next = w1->next;
            w1->next = D->V[abs(lit1)].wp.next;
            D->V[abs(lit1)].wp.next = w1;//将watch*转移到新的变元上
            return OK;
        }
        start++;						//如果文字不满足则继续
    }//如果一直到end都既没有满足也没有自由文字，则将another放入U，以备单子句处理
    w1->lit = 0;						//将id结点的lit销零
    w0->next = w1->next;
    pal_away(D, w1);					//销零并脱离pal结点
    if (into_U_M(D, D->C[another].lit) == UNSATISFIED)
        return  UNSATISFIED;			//mini型如果有冲突
    return OK;
}

/*********M型into_U**********/
int into_U_M(solverData* D, int i) {
    int var;
    D->U[++(D->U[0])] = i;				//将i放入U
    var = D->V[abs(i)].dp->value;
    if (var == i)					//已有赋值且满足
        return OK;
    if (var)						//已有赋值且不满足，则无解
        return UNSATISFIED;
    D->V[abs(i)].dp->value = i;		//赋值
    D->V[abs(i)].lp.rp->max = 0;		//定值不参与排序
    return OK;
}


/*********在初始化不满足后对显示解析结果的补充**********/
solverData* clause_copy(QString fileName) {
    QFile* fp = opener(fileName);
    clause* c;
    int i = 2, head = 1;	//head指子句的分割头结点
    solverData* D = (solverData*)calloc(1, sizeof(solverData));
    if (!D)
        exit(-1);
    int var_num = 0, clau_num = 0;			//变元数  子句数
    QByteArray data;
    data=fp->readLine();
    QTextStream out(&data);
    out>>var_num;
    out>>clau_num;
    data=fp->readAll();
    QTextStream Out(&data);
    int add_volume = clau_num;
    int clau_volume = 1 + add_volume;	//子句数组的当前容量
    D->C = (clause*)calloc(clau_volume, sizeof(clause));
    if (!D->C)
        exit(-1);
    D->C[1].wp = NULL;
    while (1) {							//文件尾时的return是循环的唯一出口
        for (; i < clau_volume - 2; i++) {
            Out>>D->C[i].lit;
            D->C[i].wp = NULL;
            if (!D->C[i].lit) {			//读到了子句尾
                clau_num--;				//剩余子句数减一
                if (!clau_num) {		//读到文件尾时，返回D
                    D->C[0].lit = i + 1;	//缩减多余的C的空间，只留下当前的i+1个
                    c = (clause*)realloc(D->C, (i + 1) * sizeof(clause));
                    if (!c)
                        exit(-1);
                    D->C = c;
                    return D;
                }
                //若没有读到文件尾，则准备下一个子句的读取
                D->C[++i].lit = 0;
                head = i;
            }
            else
                D->C[i].wp = NULL;
        }
        clau_volume += add_volume;		//扩容
        c = (clause*)realloc(D->C, clau_volume * sizeof(clause));
        if (!c)
            exit(-1);
        D->C = c;
    }
}


/*********建立小顶堆**********/
void heapify(solverData* D) {
    int n = D->R[0].var;			//n为非确定的且出现在子句中的变元数
    int i, k, j, temp;
    for (i = n / 2; i > 0; i--) {
        j = i;
        while (1) {
            if (2 * j > n)
                break;
            if (2 * j + 1 <= n && (D->V[D->R[2 * j + 1].var].lp.rp->max < D->V[D->R[2 * j].var].lp.rp->max))	//两子树的较小值
                k = 2 * j + 1;
            else
                k = 2 * j;
            if (D->V[D->R[k].var].lp.rp->max < D->V[D->R[j].var].lp.rp->max) {	//若两子树较小值小于父结点
                mySwap((D->R[k].var), (D->R[j].var));
                    j = k;											//继续向下调整
            }
            else break;
        }
    }
}


/*********普适性：调整小顶堆**********/
void heap_fix(solverData* D, int i, int n) {
    int k, temp = 0;
    while (i / 2 >= 1 && (D->V[D->R[i].var].lp.rp->max < D->V[D->R[i / 2].var].lp.rp->max)) {	//如果i对应值比起父结点小
        mySwap((D->R[i].var), (D->R[i / 2].var));
            i = i / 2;
    }
    if (!temp)
        while (2 * i <= n) {		//如果i没有与其父结点交换过，并且i在n的范围内还有子结点
            if (2 * i > n)
                break;
            if (2 * i + 1 <= n && (D->V[D->R[2 * i + 1].var].lp.rp->max < D->V[D->R[2 * i].var].lp.rp->max))	//两子树的较小值
                k = 2 * i + 1;
            else
                k = 2 * i;
            if (D->V[D->R[k].var].lp.rp->max < D->V[D->R[i].var].lp.rp->max) {	//若两子树较小值小于父结点
                mySwap((D->R[k].var), (D->R[i].var));
                    i = k;	//继续向下调整
            }
            else break;
        }
}


/*********对变元的分数排序**********/
void ranker(solverData* D) {
    int var_num = D->V[0].wp.id;
    int i,temp;
    D->R = (R*)calloc(var_num + 1, sizeof(R));
    if (!D->R)
        exit(-1);
    D->R[0].var = var_num;
    for (i = var_num; i >= 1; i--) {
        D->R[i].var = i;
        if (!D->V[D->R[i].var].lp.rp->max) {		//如果是赋值确定的变量或不在任何自由子句中的变量,则放到尾部
            mySwap((D->R[i].var), (D->R[D->R[0].var].var));
            D->R[0].var--;
        }
    }
    heapify(D);
    for (i = D->R[0].var; i > 1; i--) {
        mySwap((D->R[i].var), (D->R[1].var));
        heap_fix(D, 1, i-1);
    }
    R_transfer(D);					//排序完毕后，令R[0]的last指向第一个自由变元
}


/*将R的var形式转换为rp形式*/
void R_transfer(solverData* D) {
    int i;
    rank* last = NULL;
    rank* rp;
    for (i = 1; i <= D->R[0].var; i++) {
        rp = D->V[D->R[i].var].lp.rp;
        if (last)
            last->next = rp;
        rp->last = last;
        last = rp;
    }
    if (last)
        last->next = NULL;
    D->R[0].rp = (rank*)calloc(1, sizeof(rank));
    D->R[0].rp->last = D->R[0].rp->next = D->V[D->R[1].var].lp.rp;
    D->R[0].rp->max = 0;
}
