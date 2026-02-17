#include "shell.h"

/**
 * main - simple shell that executes commands
 * @ac: argument count
 * @av: argument vector
 * @env: environment variables
 *
 * Return: 0 on success
 */
int main(int ac, char **av, char **env)
{
	char *line = NULL, *command;
	size_t len = 0;
	ssize_t read;
	pid_t child_pid;
	char *args[2];
	int status;

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

		/* strtok cleans leading/trailing whitespace and newlines */
		command = strtok(line, " \n\t");

		if (command == NULL) /* Handle empty lines */
			continue;

		args[0] = command;
		args[1] = NULL;

		child_pid = fork();
		if (child_pid == -1)
		{
			perror("Error");
			continue;
		}
		if (child_pid == 0)
		{
			if (execve(args[0], args, env) == -1)
			{
				fprintf(stderr, "%s: 1: %s: not found\n", av[0], command);
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
