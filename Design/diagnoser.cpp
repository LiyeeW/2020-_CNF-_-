#include "diagnoser.h"



/*********诊断与回溯主函数**********/
int diagnoser(solverData* D) {
    int Result;
    do {
        if (D->BD == D->null)					//如果指定取反层=0，则不可回溯
            return UNSATISFIED;
        Result = backer(D);						//回溯取反BD
    } while (Result != OK);						//回溯到直至BCP返回OK
    Result = R_find(D);							//时序性回溯的文字分值为静态，故只需要调整R[0]
    return Result;
}


/*********回撤指定层的BCP结点的赋值**********/
int recaller(solverData* D, decision* P) {
    decision* Last = P->last;
    decision* temp;
    int bottom = D->U[0]+1;				//防止该层没有用栈
    P->last = NULL;
    while (Last) {
        temp = Last->last;
        if (Last->duty)
            bottom = Last->duty;		//更新记录返回取反层的U栈底
        Last->duty = Last->value = Last->belong = 0;
        Last->last = Last->parent = NULL;
        Last = temp;
    }
    return bottom;
}


/*********复原监视结点**********/
void watch_back(solverData* D, int bottom) {
    int another;					//存放从U栈出来的another
    int w1, w2;						//存放与两个子句结点相关的C序号
    watch* W1, * W2;				//存放两个子句结点
    bottom--;						//指向上一层的栈顶
    while (D->U[0] != bottom) {
        w1 = w2 = another = D->U[D->U[0]--];
        while (D->C[--w1].lit);		//找到子句头结点
        W1 = D->C[w1].wp;
        while (D->C[++w2].lit);		//找到子句尾结点
        W2 = D->C[w2].wp;
        while (D->V[abs(D->C[++w1].lit)].dp->value);
        W1->lit = w1;
        w1 = abs(D->C[w1].lit);
        W1->next = D->V[w1].wp.next;
        D->V[w1].wp.next = W1;
        while (D->V[abs(D->C[--w2].lit)].dp->value);
        W2->lit = w2;
        w2 = abs(D->C[w2].lit);
        W2->next = D->V[w2].wp.next;
        D->V[w2].wp.next = W2;
    }
}



/*********回溯到指定位置**********/
int backer(solverData* D) {
    decision* dp;						//暂放被撤值的决策结点的parent
    int reverse;						//暂放指定决策结点被取反后的值
    int result;
    int bottom;							//最后的有效值是指定取反层的U栈底
    while(1) {
        bottom = recaller(D, D->N);		//回撤当前层的BCP结点赋值
        if (D->N->sw.next)
            borrow_U(D, &(D->N->sw));
        if (D->N == D->BD) {			//若当前层为指定取反层
            if (!D->BD->parent)
                D->N = D->null;			//当前层回退到指定层(=1)的上一层
            else
                D->N = D->BD->parent;	//当前层回退到指定层(>1)的上一层
            reverse = -D->BD->value;	//取反
            D->BD->value = 0;			//在watch_back之前，保持其为0，防止干扰
            D->BD->last = D->N->last;
            D->N->last = D->BD;			//插入last链表
            D->BD->level = 0;				//取反后的决策结点与BCP结点类似，层数为0，但BCP的duty不为0
            watch_back(D, bottom);				//监视结点复原
            break;						//处理完指定层则循环结束
        }
        else {							//若不是取反层则回撤该决策结点赋值
            dp = D->N->parent;
            D->N->value = D->N->level = 0;
            D->N->last = D->N->parent = NULL;
            D->N = dp;					//当前层回退到上一层
            watch_back(D, bottom);				//监视结点复原
        }

    }
    D->N->last->value = reverse;		//取反赋值
    result = bcp(D, 'B');				//取反后的BCP
    return result;						//无论BCP有无冲突都直接返回，交给diagnoser分析
}


/*********转移需要回溯的剩余wp和满足子句的wp**********/
void borrow_U(solverData* D, watch* wp) {
    while (wp->next) {//将需要回撤的满足子句伪装成需要回撤的单子句
        D->U[++(D->U[0])] = (wp->next->id < wp->next->pal) ? wp->next->id + 1 : wp->next->pal + 1;
        wp->next = wp->next->next;
    }
}
