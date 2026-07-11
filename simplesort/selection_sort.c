#include <stdio.h>
#include <stdlib.h>

void selection_sort(int *arr, int size){
	int i = 0;
	int min = arr[0];
	int min_idx;
	int tmp;
	int j = 0;

	while(j < size)
	{
		i = j;
		min = arr[j];
		while(i < size)
		{
			if(min > arr[i])
			{
				min = arr[i];
				min_idx = i;
				tmp = arr[j];
				arr[j] = arr[min_idx];
				arr[min_idx] = tmp;
			}
			i++;
		}
		j++;
	}
	printf("%d\n", j);
	printf("%d,%d\n",min,min_idx);
	for(int i = 0;i < size;i++)
		printf("%d", arr[i]);
}

int main(){
	int arr[5] = {3, 1, 5, 0, 4};
	int size = 5;
	selection_sort(arr,size);

	return 0;
}

