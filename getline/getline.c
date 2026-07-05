
//ssize_t	getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char *argv[])
{
	FILE	*stream;
	char	*line;
	size_t	len;
	ssize_t	nread;

	line = NULL;
	len = 0;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	stream = fopen(argv[1], "r");
	if (stream == NULL)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	while ((nread = getline(&line, &len, stream)) != -1)
	{
		printf("Retrieved line of length %zd:\n", nread);
		fwrite(line, nread, 1, stdout);
	}
	free(line);
	fclose(stream);
	exit(EXIT_SUCCESS);
}

