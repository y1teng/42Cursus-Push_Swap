#include <stdio.h>

typedef struct s_stack
{
	int *data;
	int capacity;
	int size;
	int head;
} t_stack;



void op_swap(t_stack *n)
{
    int first  = n->head;
    int second = (n->head + 1) % n->capacity;
    int tmp;
	tmp = n->data[first];
	n->data[first] = n->data[second];
	n->data[second] = tmp;
}

void sa(t_stack *a){
	op_swap(a);
}

void sb(t_stack *b){
	op_swap(b);
}

void ss(t_stack *a,t_stack *b){
	op_swap(a);
	op_swap(b);
}

void op_reverse_rotate(t_stack* n)
{
	n->head = (n->head - 1 + n->capacity) % n->capacity;
}

void rra(t_stack *a){
	op_reverse_rotate(a);
}

void rrb(t_stack *b){
	op_reverse_rotate(b);
}

void rrr(t_stack *a, t_stack *b){
	op_reverse_rotate(a);
	op_reverse_rotate(b);
}

void op_rotate(t_stack* n)
{
	n->head = (n->head +1) % n->capacity;
}

void ra(t_stack *a){
	op_rotate(a);
}

void rb(t_stack *b){
	op_rotate(b);
}

void rr(t_stack *a, t_stack *b){
	op_rotate(a);
	op_rotate(b);
}

void op_push(t_stack *dst,t_stack *src){
	if(!src->size)
		return;
	int tmp;

	tmp = src->data[src->head];
	dst->head = (dst->head - 1 + dst->capacity) % dst->capacity;
	src->head = (src->head + 1) % src->capacity;
	dst->data[dst->head] = tmp;
	dst->size++;
	src->size--;
}

void pa(t_stack *dst,t_stack *src){
	op_push(dst,src);
}

void pb(t_stack *dst,t_stack *src){
	op_push(dst,src);
}

int main(){
	t_stack a;
	t_stack b;
	int buffer[3] = {10,20,30};
	int buffer_02[3];
	a.data= buffer;
	a.capacity = 3;
	a.size = 3;
	a.head = 0;
	b.data = buffer_02;
	b.capacity = 3;
	b.size = 0;
	b.head = 0;

	pb(&b, &a);
	pb(&b, &a);
	
	printf("b.head=%d\n",b.head);
	printf("b top = %d\n", b.data[b.head]);
	printf("b second (b.head+1) = %d\n", b.data[(b.head+1) % b.capacity]);
	return 0;
}
//構造体に名前をつけられる
//配列のトークンに大きな数字が入らないのでは？
