#include"Linklist.h"
void changeList(Linklist &list){
   if(!list || !list->next || list->next==list) return;
   Linklist odd = nullptr,even = nullptr;
   Linklist oddTail = nullptr, evenTail = nullptr;
   Linklist current = list->next;
   int index = 1;
   while(current !=list){
    if(index % 2==1){
        if(!odd){
            odd=current;
            oddTail = current;
        }
        else{
            oddTail->next=current;
            current->prior=oddTail;
            oddTail=current;
        }
    }
    else{
        if(!even){
            even=current;
            evenTail=current;
        }
        else{
            evenTail->next=current;
            current->prior=evenTail;
            evenTail=current;
        }
        
    }
    current=current->next;
    index++;
   }
   if(oddTail){
    oddTail->next=list;
    list->prior=oddTail;

   }
   if(evenTail){
    evenTail->next=nullptr;
   }
   Linklist prev=nullptr, nextNode = nullptr;
   current=even;
   while(current){
    nextNode=current->next;
    current->next=prev;
    current->prior=nextNode;
    prev=current;
    current=nextNode;
   }
   Linklist reversedEven=prev;
   if(oddTail && reversedEven){
    oddTail->next=reversedEven;
    reversedEven->prior=oddTail;
    while(reversedEven->next)
    {
        reversedEven=reversedEven->next;
    }
    reversedEven->next=list;
    list->prior=reversedEven;
   }
   }