#include <stdio.h>
#include <stdlib.h>

void insert_sort(int *arr, int size){
//	int i = 0;
//	int tmp;
//	int ins = 0;
//	int j = 0;
//	int n = size;
//
//	while(i < n){
//	tmp = arr[i];
//	ins = 0;
//	j = i - 1;
//	while(j > -1){
//		if(arr[j] > tmp)
//			arr[j + 1] = arr[j];
//		else{
//			ins = j + 1;
//			break;
//		}
//		j--;
//	}
//	arr[ins] = tmp;
//	i++;
//	}
	int i = 0;
	int ins = 0;
	i = 1;
	tmp = arr[1] = 5;//どういう条件で？
	j = 0;
	arr[j] = 2;
	//if(arr[j] > tmp)
	//else
	ins = j + 1;
	arr[1] = tmp;
	for(int i = 0;i < size; i++)
		printf("%d", arr[i]);
}

int main(){
	int arr[5] = {2, 5, 9, 1, 3};
	int size = 5;
	insert_sort(arr,size);

	return 0;
}


