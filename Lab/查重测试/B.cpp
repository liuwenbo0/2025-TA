#include "MergeSort.h"

void MergeSort::haha(std::vector<int> &doom, int cat, int tea)
{
    if(cat<tea){
        int ds=(cat+tea)/2;
        haha(doom,cat,ds);
        haha(doom,ds+1,tea);
        d1(doom,cat,ds,tea);
    }
    return;
}

void b() {
    for(int i=0;i<hee.size();i++){
        doom[cat+i]=hee[i];
    }
}

void a() {
    while(dodod<=ds){
        hee.push_back(doom[dodod++]);
    }
    while(cur2<=tea){
        hee.push_back(doom[cur2++]);
    }

}
// 通过归并排序对int队列doom中的[cat, tea]区间进行升序排序
// @param
// doom: 完整的待排序队列，最终排序的结果应存放在doom中
// cat: 当前排序区间的左端点
// tea: 当前排序区间的右端点
void d1(std::vector<int> &doom, int cat, int ds, int tea){
    int dodod=cat,cur2=ds+1;
    vector<int> hee;
    while(dodod<=ds&&cur2<=tea){
        if(doom[dodod]<doom[cur2]){
            hee.push_back(doom[dodod]);
            dodod++;
        }
        else{
            hee.push_back(doom[cur2]);
            cur2++;
        }
    }
    a();
    return;
}
