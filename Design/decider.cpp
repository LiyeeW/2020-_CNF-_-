#include "decider.h"



/*********决策与赋值主函数**********/
int decider(solverData* D) {
    picker(D);					//选择并赋值决策结点
    int result = bcp(D, 'D');	//有OK和lit两种返回结果
    if (result == OK)			//若无冲突，则直接调整好R[0]
        result = R_find(D);		//有OK和SATISFIED两种返回结果
    return result;
}



/*********BCP**********/
int bcp(solverData* D, const char type) {//type:1)B\back  2)D\decide
    decision* dp;
    watch* w;
    int result, i, var;
    D->U[1] = D->U[0] + 1;				//为当前层指定栈底
    var = (type == 'D') ? abs(D->N->value) : abs(D->N->last->value); //分D还是B
    w = &(D->V[var].wp);
    while (w->next) {				//如果有被该变元监控的子句
        result = DoWatch(w, D, type);
        if (result != OK) {			//不满足
            DoDuty(D, result);
            watch_away(D,w);
            return result;
        }
    }
    for (i = D->U[1]; i <= D->U[0]; i++) {//对U的内容逐一DoWatch
        var = abs(D->C[D->U[i]].lit);
        dp = D->V[var].dp;				//完善赋值，与未被dowatch的U区分开
        dp->level = 0;
        dp->sw.next = NULL;
        dp->last = D->N->last;
        D->N->last = dp;
        w = &(D->V[var].wp);
        while (w->next) {				//如果有被该变元监控的子句
            result = DoWatch(w, D, type);
            if (result != OK) {			//不满足
                DoDuty(D, result);
                watch_away(D,w);
                return result;
            }
        }
    }
    return OK;
}


/********使pal结点脱离变元链********/
void pal_away(solverData* D, watch* wp) {
    watch* w1 = D->C[wp->pal].wp;
    watch* w2 = &(D->V[abs(D->C[w1->lit].lit)].wp);
    while (w2->next) {
        if (w2->next == w1) {
            w2->next = w2->next->next;
            break;
        }
        w2 = w2->next;
    }
    w1->lit = 0;
}



/*********普适性：处理被赋值的变元的单个watch结点**********/
int DoWatch(watch* w0, solverData* D, const char type) {
    watch* w1 = w0->next;				//w1是处理对象，为id结点寻找新的lit
    int result;
    int lit1, lit2;
    int old = w1->lit;					//暂时保留旧值
    int another = D->C[w1->pal].wp->lit;//another是pal结点的lit，不可被重复指向，其是否为自由文字并不确定，因为赋值与DoWatch为异步
    int start = w1->id;					//暂时取id序号
    int end = w1->pal;					//暂时取pal序号
    start = (end > start) ? start + 1 : end + 1;//使start为较小者的下一个序号，end为较大者
    end = (start == end + 1) ? w1->id - 1 : end - 1;//使end为较大者的上一个序号
    while (start <= end) {
        if (start == another) {			//跳过another，避免重复
            start++;
            continue;
        }
        lit1 = D->C[start].lit;			//取出文字
        lit2 = D->V[abs(lit1)].dp->value;//取出对应变元的赋值
        if (lit2 == lit1) {				//子句被满足
            w1->lit = 0;
            w0->next = w1->next;
            w1->next = D->N->sw.next;
            D->N->sw.next = w1;			//满足子句的id结点放入D->N的sw链中
            pal_away(D, w1);			//销零并脱离pal结点
            return OK;
        }
        if (!lit2) {					//如果找到一个最近的自由文字
            w1->lit = start;			//对应新自由文字的序号
            w0->next = w1->next;
            w1->next = D->V[abs(lit1)].wp.next;
            D->V[abs(lit1)].wp.next = w1;//将watch*转移到新的变元上
            return OK;
        }
        start++;						//如果文字不满足则继续
    }//如果一直到end都既没有满足也没有自由文字，则将another放入U，以备单子句处理
    result = into_U(D, another, old, w1, type);
    if (result == another)
        return result;					//如果有冲突
    w0->next = w1->next;
    w1->lit = 0;						//将id结点的lit销零
    pal_away(D, w1);					//销零并脱离pal结点
    if (result == LITSATISFIED) {		//如果被满足
        w1->next = D->N->sw.next;
        D->N->sw.next = w1;				//满足子句的id结点放入D->N的sw链中
    }
    return OK;
}


/*********将一个数送入Unit队列**********/
int into_U(solverData* D, int i, int old, watch* id, const char type) {
    decision* dp;
    int lit1 = D->C[i].lit;				//取文字
    int lit2 = D->V[abs(lit1)].dp->value;//取变元赋值
    if (lit2 == lit1) 					//已有赋值且满足
        return LITSATISFIED;
    if (lit2)							//已有赋值且不满足
        return i;
    D->U[++(D->U[0])] = i;				//将i放入U
    dp = D->V[abs(lit1)].dp;			//对BCP结点赋值
    dp->value = lit1;
    dp->level = old;					//暂时记录相应的idw的旧的lit
    dp->sw.next = id;					//暂时记录相应的idw
    dp->duty = D->U[0];					//存放直接造成该赋值的文字的C序号的U序号
    dp->parent = D->N;
    if (type == 'B')
        dp->belong = 1;					//标注对同层已被取反变量的依赖关系
    return OK;
}



/*********选择并赋值决策变元**********/
int picker(solverData* D) {
    int var = D->R[0].rp->next->var;//取R[0]所指的变元为决策变元
    D->V[var].dp->value = (D->V[var].lp.choice == 0) ? -var : var;	//取与max相等的文字为决策文字
    D->V[var].dp->level = D->N->level + 1;	//计算该决策结点的层数，决策结点的最小层数为1
    D->V[var].dp->parent = D->N;
    D->N = D->V[var].dp;
    return OK;
}


void watch_set(solverData* D, watch* wp, int litc) {
    wp->lit = litc;
    int var = abs(D->C[litc].lit);
    wp->next = D->V[var].wp.next;
    D->V[var].wp.next = wp;
}


/*********将bcp报错后多余的watch*的pal移出原变元**********/
void watch_away(solverData* D, watch* w0) {
    decision* dp;
    watch* w1;
    int litc;
    if (D->U[0] > 1) {						//若=1则触底，说明该层没有进栈过
        litc = D->U[D->U[0]--];
        dp = D->V[abs(D->C[litc].lit)].dp;
        while (dp->level) {					//level不为0，说明尚未被dowatch
            watch_set(D, dp->sw.next, dp->level);
            watch_set(D, D->C[dp->sw.next->pal].wp, litc);
            dp->level = 0;
            dp->value = 0;
            dp->belong = 0;
            dp->sw.next = NULL;
            dp->duty = 0;
            dp->parent = NULL;
            litc = D->U[D->U[0]--];
            if (D->U[0] == 0) {
                litc = 0;
                break;
            }
            dp = D->V[abs(D->C[litc].lit)].dp;
        }
        D->U[0]++;
        w1 = w0;
        while (w1->next) {					//清理dowatch到一半的U
            pal_away(D, w1->next);
            w1->next->lit = 0;
            w1 = w1->next;
        }
        w1->next = D->N->sw.next;
        D->N->sw.next = w0->next;
        w0->next = NULL;
    }
}

/*********调整文字统计的记录**********/
void lmax_set(literal* lp) {
    if (lp->neg == lp->pos)
        lp->choice = random(2);//0;1;
    else lp->choice = (lp->neg > lp->pos) ? 0 : 1;
    lp->rp->max = (lp->choice == 0) ? lp->neg : lp->pos;
}


/*********调整D->R[0]**********/
int R_find(solverData* D) {
    R_fix(D);
    rank* rp = D->R[0].rp->last;
    if (D->R[0].rp->var == 0)
        rp = D->R[0].rp->next;
    else
        D->R[0].rp->var = 0;
    while (rp) {
        if (D->V[rp->var].dp->value == 0)
            break;
        rp = rp->next;
    }
    if (!rp)
        return SATISFIED;				//所有变元已赋值，返回SATIFIED
    D->R[0].rp->next = rp;
    return OK;				//找到最近的未赋值的R序号，返回OK
}


/*********调整文字统计的排名**********/
void R_fix(solverData* D) {
    int i, flag;
    rank* rp0, * rp1 = NULL;
    for (i = D->R[0].rp->var; i > 0; i--) {
        rp0 = D->R[i].rp;
        if(rp0->last)						//仅对非队首的rp加，因此能限制极速增长带来的溢出
            rp0->max += 10;					//对于回溯加10
        rp1 = NULL;							//若之后还是NULL，说明rp0就是稳定的头一个或稳定的尾一个
        if (rp0->next&&(rp0->max < rp0->next->max)) {
            rp1 = rp0->next;
            while (rp1->next) {
                if (rp0->max >= rp1->next->max)
                    break;
                rp1 = rp1->next;
            }
        }
        else if (rp0->last && (rp0->max > rp0->last->max)) {
            rp1 = rp0->last;
            flag = 0;
            while (rp1->last) {
                rp1 = rp1->last;
                if (rp0->max <= rp1->max) {
                    flag = 1;
                    break;
                }
            }
            if (!flag) {//循环结束意味着rp0是目前分数最高的
                rp0->last->next = rp0->next;
                if (rp0->next)
                    rp0->next->last = rp0->last;
                rp0->next = rp1;
                rp0->last = NULL;
                rp1->last = rp0;
                D->R[0].rp->last = rp0;
                rp1 = NULL;
            }
        }
        if (!rp1)
            continue;
        if(rp0->last)
            rp0->last->next = rp0->next;
        if(rp0->next)
            rp0->next->last = rp0->last;
        rp0->next = rp1->next;
        rp0->last = rp1;
        if (rp1->next)
            rp1->next->last = rp0;
        rp1->next = rp0;
    }
}

void DoDuty(solverData* D, int result) {
    int litc,w;
    decision* dp;
    D->R[0].rp->max++;					//回溯次数加一
    D->BD = D->null;
    D->dutyC[0] = 1;
    D->dutyC[1] = 1;
    w = litc = result;					//冲突文字
    while (D->C[--w].lit);
    while (D->C[++w].lit){
        if (D->testV[abs(D->C[w].lit)]!= D->R[0].rp->max) {
            D->testV[abs(D->C[w].lit)] = D->R[0].rp->max;
            D->dutyC[++D->dutyC[0]] = abs(D->C[w].lit);
        }
    };
    do {
        dp = D->V[D->dutyC[++D->dutyC[1]]].dp;
        if ((dp->duty)&& (dp->parent->level > D->BD->level)) {	//两类BCP变元
            if (!dp->belong) {
                D->BD = dp->parent;
                if (D->BD == D->N)	//如果等于当前层，则直接结束
                    break;
            }
            w = D->U[dp->duty];
            while (D->C[--w].lit);
            while (D->C[++w].lit) {
                if (D->testV[abs(D->C[w].lit)]!= D->R[0].rp->max) {
                    D->testV[abs(D->C[w].lit)] = D->R[0].rp->max;
                    D->dutyC[++D->dutyC[0]] = abs(D->C[w].lit);
                }
            }
        }
        else if (!(dp->duty)&&(dp->value) && (dp->level > D->BD->level)) {	//决策变元
            D->BD = dp;
            if (D->BD == D->N)	//如果等于当前层，则直接结束
                break;
        }

    } while (D->dutyC[1]< D->dutyC[0]);
    if (D->BD == D->null)		//若全与已被取反变量有关，则取当前层
        D->BD = D->N;
    D->R[++(D->R[0].rp->var)].rp = D->V[abs(D->BD->value)].lp.rp;
}
