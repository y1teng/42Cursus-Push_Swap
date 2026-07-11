#include <stdio.h>
#include <stdlib.h>

void bubble_sort(int *arr, int size){
	int tmp;
	int i = 0;
	int j = 0;

	while(j < size -1)
	{
		i = 0;
		while(i < size -1)
		{	
			if(arr[i] < arr[i + 1])
			{
				tmp = arr[i];
				arr[i] = arr[i + 1];
				arr[i + 1] = tmp;
			}
			i++;
		}
		j++;
	}
	for(i = 0; i < size; i++)
		printf("%d",arr[i]);
}

int main(){
	int arr[5] = {5, 2, 4, 1, 3};
	int size = 5;
	bubble_sort(arr,size);

	return 0;
}
