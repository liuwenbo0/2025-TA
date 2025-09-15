#include"SqList.hpp"

void insert(SqList& list, int num){
	int length = list.size();
	list.push_back(num);
	for(int i = 0; i < length; i++){
		if(list[i] > num){
			for(int j = length - 1; j > i - 1; j--){
				list[j + 1] = list[j];
			}
			list[i] = num;
			break;
		}
	}
}