#include "push_swap.h"

int	main(void)
{
	t_stack	a;
	t_stack	b;
	int		buffer[3] = {10, 20, 30};
	int		buffer_02[3];

	a.data = buffer;
	a.capacity = 3;
	a.size = 3;
	a.head = 0;
	b.data = buffer_02;
	b.capacity = 3;
	b.size = 0;
	b.head = 0;
	pb(&b, &a);
	pb(&b, &a);
	printf("b.head=%d\n", b.head);
	printf("b top = %d\n", b.data[b.head]);
	printf("b second (b.head+1) = %d\n", b.data[(b.head + 1) % b.capacity]);
	return (0);
}
//構造体に名前をつけられる
//配列のトークンに大きな数字が入らないのでは？
