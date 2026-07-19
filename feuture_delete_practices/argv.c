#include <stdio.h>

int main(int ac, char **av){
	for(int i = 0; i < ac;i++)
	{
		printf("[%d]",ac);
		printf("%s\n",av[i]);
	}
	return 0;
}
