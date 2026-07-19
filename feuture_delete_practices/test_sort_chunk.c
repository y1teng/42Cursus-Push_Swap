#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
**sort inside chunk
**range is start to len
*/


void insection_sort(int *arr, int start, int len)
{
	int i;
	int j;
	int key;

	i = start + 1;
	while(i < start + len)
	{
		key = arr[i];
		j = i - 1;
		while(j >= start && arr[j] > key)
		{
			arr[j + 1] = arr[j];
			j--;
		}
		arr[j + 1] = key;
		i++;
	}
}

/*
** done sort matomeru(k-way merge)
** pos[i] = chunk i "now dokomade"
** end[i] = chunk i no end(execlusive)
** all chunk compare ,and take small one
*/
void merge_chunks(int *arr,int n, int chunk_size)
{
	int n_chunks;
	int *pos;
	int *end;
	int *result;
	int i;
	int k;
	int min_chunk;
	int min_val;

	n_chunks = (n + chunk_size -1) / chunk_size;
	pos = malloc(sizeof(int) * n_chunks);
	end = malloc(sizeof(int) * n_chunks);
	result = malloc(sizeof(int) * n);
	i = 0;
	while(i < n_chunks)
	{
		pos[i] = i * chunk_size;
		end[i] = pos[i] +chunk_size;
		if(end[i] > n)
			end[i] = n;
		i++;
	}
	k = 0;
	while(k < n)
	{
		min_chunk = -1;
		i = 0;
		while(i < n_chunks)
		{
			if(pos[i] < end[i] && (min_chunk == -1 || arr[pos[i]] < min_val))
			{
				min_val = arr[pos[i]];
				min_chunk = i;
			}
			i++;
		}
		result[k] = arr[pos[min_chunk]];
		pos[min_chunk]++;
		k++;
	}
	i = 0;
	while(i < n)
	{
		arr[i] = result[i];
		i++;
	}
	free(pos);
	free(end);
	free(result);
}

/*	real chunk
**	split root n ,each insertion sort
**	all chunk intgrate merge_chunks()
*/
void chunk_sort(int *arr, int n)
{
	int chunk_size;
	int i;
	int len;

	chunk_size = (int)sqrt((double)n);
	if(chunk_size < 1)
		chunk_size = 1;
	i = 0;
	while(i < n)
	{
		len = chunk_size;
		if(i + len > n)
			len = n -1;
		insection_sort(arr, i, len);
		i+=chunk_size;
	}
	merge_chunks(arr, n, chunk_size);
}

int main()
{
	int arr[9];
	int i;

	arr[0] = 9;
	arr[1] = 1;
	arr[2] = 4;
	arr[3] = 7;
	arr[4] = 2;
	arr[5] = 8;
	arr[6] = 3;
	arr[7] = 6;
	arr[8] = 5;

	printf("before: ");
	i = 0;
	while(i < 9)
	{
		printf("%d ", arr[i]);
		i++;
	}
	printf("\n");

	chunk_sort(arr, 9);

	printf("after: ");
	i = 0;
	while(i < 9)
	{
		printf("%d ", arr[i]);
		i++;
	}
	printf("\n");

	return 0;
}
