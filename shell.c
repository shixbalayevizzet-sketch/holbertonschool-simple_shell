#include "shell.h"

/**
 * main - entry point for simple shell
 * @ac: argument count
 * @av: argument vector
 * @env: environment
 *
 * Return: 0 on success
 */
int main(int ac, char **av, char **env)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t child_pid;
	char *argv[2];
	int status, i;

	(void)ac;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			free(line);
			exit(EXIT_SUCCESS);
		}

		i = read - 1;
		while (i >= 0 && (line[i] == '\n' || line[i] == ' ' ||
				 line[i] == '\t' || line[i] == '\r'))
		{
			line[i] = '\0';
			i--;
		}

		if (line[0] == '\0')
			continue;

		argv[0] = line;
		argv[1] = NULL;
		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Error");
			continue;
		}
		if (child_pid == 0)
		{
			if (execve(argv[0], argv, env) == -1)
			{
				fprintf(stderr, "%s: 1: %s: not found\n", av[0], line);
				free(line);
				exit(127);
			}
		}
		else
			wait(&status);
	}
	free(line);
	return (0);
}
