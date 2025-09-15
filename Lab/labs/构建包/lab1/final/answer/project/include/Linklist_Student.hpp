#include"Linklist.hpp"

void changeList(Linklist &list){
    Linklist second, tail, tail_list, p=list->next;
    second=list->next->next;
    tail_list=(Linklist)malloc(sizeof(struct node));
    tail_list->prior = tail_list->next = NULL;
    if(list->next->next==list){
        return;
    }
    while(second!=list&&second!=list->next){
        tail=(Linklist)malloc(sizeof(struct node));
        tail->num=second->num;
        tail->prior=tail_list;
        tail_list->next=tail;
        tail_list=tail;
        second=second->next->next;
    }
    while(p->next->next!=list&&p->next->next!=list->next){
        p->next=p->next->next;
        p->next->next->prior=p;
        p=p->next;
    }
    while(tail->prior!=NULL){
    	Linklist temp = tail->prior;
        p->next=tail;
        tail->prior = p;
        p=tail;
        tail=temp;
    }
    p->next=list;
    list->prior = p;
}
