#include <stdio.h>

typedef struct s_queue
{
	char *data;
	int size;
	int head;
	int tail;
} t_queue;

int	is_full(t_queue *q)
{
	return(((q->tail +1) % q->size == q->head));
}

char pop(t_queue *q){
	printf("head=%d tail=%d\n", q->head, q->tail);
	char value;
	
	value = q->data[q->head];
	q->head = (q->head + 1) % q->size;
	return value;
//	return (q->data[q->head++]);
}

void	push(t_queue *q, char value){
	if(is_full(q))
		return;
	q->data[q->tail] = value;
	q->tail = (q->tail + 1) % q->size;
	
}

void init_queue(t_queue *q, char *buffer, int size)
{
	q->data = buffer;
	q->size = size;
	q->head = 0;
	q->tail = 0;
}

int main(){
	int size = 5;
	char buffer[size];
	t_queue q;
	init_queue(&q,buffer,size);

	for(int i = 0;i < 4; i++)
		push(&q,'a'+1);
	for(int i = 0; i < 6; i++)
		printf("pop = %c\n", pop(&q));
//	for(int i = 0;i < 8;i++){
//		push(&q, 'a'+i);
//	}
//	for(int i = 0;i < size;i++){
//		printf("%c,head[%d],tail[%d]\n",q.data[i],q.head,q.tail);
//	}
//	printf("%c,head[%d],tail[%d]\n", pop(&q),q.head,q.tail);
//	printf("%c,head[%d],tail[%d]\n", pop(&q),q.head,q.tail);
//	for (int i = 0; i < size; i++)
//	{
//		printf("%c,head[%d],tail[%d]\n", q.data[i],q.head,q.tail);
//	}
//	for (int i = 0; i < 10; i++)
//		printf("%c,head[%d],tail[%d]\n", pop(&q),q.head,q.tail);
	return 0;
}
