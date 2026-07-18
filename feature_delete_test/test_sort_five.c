#include "push_swap.h"

void print_arr(int* num)
{
	for(int i = 0;i < 5;i++)
		printf("%d",num[i]);
	puts("");
}

/*
start=0:
  i=0: swap(arr[0],arr[0]) → {1,2,3}（変化なし）
       再帰 permute(arr,1,2)
         start=1:
           i=1: swap(arr[1],arr[1]) → {1,2,3}
                再帰 permute(arr,2,2) → start==end → 完成！{1,2,3} を出力
                swapを戻す（変化なし）

*/
int is_sorted(t_stack *a)
{
	int i = 0;
	while(i < a->size - 1)
	{
		if(a->data[(a->head + i )% a->capacity] > a->data[(a->head + i + 1) %
			a->capacity])
			return 0;
		i++;
	}
		return 1;

}

/*
1. int copy[5]; のような配列を用意し、arr[0]〜arr[4] の値を1つずつコピーする（ループで良いです）
2. a: capacity=5, size=5, head=0, data = copy（コピーの方）
3. b: capacity=5, size=0, head=0, data は別途用意した空の配列
4. sort_five(&a, &b) を呼ぶ
5. is_sorted(&a) で判定 → 失敗していたら、元の arr（どんな並びだったか）をprintfして知らせる
*/
void on_permutation(int *arr) 
{
	int copy[5];
	for(int i = 0;i<5;i++)
		copy[i] = arr[i];
	int null[5];
	
	t_stack a;
	t_stack b;
	a.capacity = 5;
	a.size = 5;
	a.head = 0;
	a.data = copy;
	b.capacity = 5;
	b.size = 0;
	b.head = 0;
	b.data = null;
	
	sort_five(&a,&b);
	if(!is_sorted(&a))
		print_arr(arr);

}

void swap(int *a,int *b)
{
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void permute(int *arr, int start, int end, void (*on_permutation)(int *))
{
	int i = start;
	
	if(start == end)
		return on_permutation(arr);
	
	if(start != end){
		while(i <= end)
		{
			swap(&arr[start],&arr[i]);
				permute(arr,start + 1,end,on_permutation);
			swap(&arr[start],&arr[i]);
			i++;
		}
	}
}

int main()
{
	int arr[5] = {1, 2, 3, 4, 5};
	permute(arr, 0, 4, on_permutation);
	//on_permutation(arr);

	return 0;
}

// void	op_visualize(t_stack *a, t_stack *b, char*label)
//{
//	int	a_top;
//	int	b_top;
//	puts("WWWWWWWWWWWWWWWWWWWWWWWWW");
//	printf("#a->size[%d], #b->size[%d]\n", a->size, b->size);
//	for (int i = 0; i < a->capacity; i++)
//	{
//		a_top = (a->head + i) % a->capacity;
//		printf("%s: a->head[%d], a->data[(%d)] = %d\n",label, a->head, a_top,
//			a->data[a_top]);
//	}
//	puts("------------------------");
//	for (int i = 0; i < b->capacity; i++)
//	{
//		b_top = (b->head + i) % b->capacity;
//		printf("%s: b->head[%d], b->data[(%d)] = %d\n",label, b->head, b_top,
//			b->data[b_top]);
//	}
//}

//int main()
//{
//	t_stack a;
//	t_stack b;
//	int i = 0;
//	int num = 1;
//	int s_c = 5;
//	a.capacity = s_c;
//	b.capacity = s_c;
//	a.size = s_c;
//	b.size = 0;
//	a.head = 0;
//	b.head = 0;
//
//	int bf[5];
//	a.data = bf;
//	int bf_b[5];
//	b.data = bf_b;
//
//	puts("------before--------\n");
//	for(int i = 0;i < 5;i++)
//		printf("[%d]\n",a.data[i]);
//
//	sort_five(&a,&b);
//
//	puts("------after--------\n");
//	for(int i = 0;i < 5;i++)
//		printf("[%d]\n",a.data[i]);
//	
//	return 0;
//}


//t_stack a, b をローカル変数として用意
//
//a.data = {3, 1, 4, 5, 2} のような配列
//a.capacity = a.size = 5, a.head = 0
//b.data = 空の配列(要素数5ぶん確保), b.capacity = 5, b.size = 0, b.head = 0
//sort_five(&a, &b) を呼ぶ
//呼んだ後、a.dataを (a.head + i) % a.capacity の順で全部printfして、昇順になっているか目視確認
