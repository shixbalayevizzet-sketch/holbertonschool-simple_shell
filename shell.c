#include "shell.h"

/**
 * main - simple shell that executes one-word commands
 * @ac: argument count (unused)
 * @av: argument vector (unused)
 * @env: environment variables
 *
 * Return: 0 on success
 */
int main(int ac, char **av, char **env)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t child_pid;
	char *argv[] = {NULL, NULL};
	int status;

	(void)ac; (void)av;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		read = getline(&line, &len, stdin);
		if (read == -1) /* Handle Ctrl+D (EOF) */
		{
			free(line);
			exit(EXIT_SUCCESS);
		}

		if (line[read - 1] == '\n') /* Remove newline character */
			line[read - 1] = '\0';

		argv[0] = line;
		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Error");
			continue;
		}
		if (child_pid == 0) /* Child process */
		{
			if (execve(argv[0], argv, env) == -1)
			{
				perror(av[0]);
				free(line);
				exit(EXIT_FAILURE);
			}
		}
		else /* Parent process */
			wait(&status);
	}
	free(line);
	return (0);
}
