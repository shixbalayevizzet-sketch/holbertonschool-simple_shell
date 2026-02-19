#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/**
 * get_path - Finds the full path of a command using the PATH variable.
 * @command: The command to find (e.g., "ls").
 * Return: Full path string if found, otherwise NULL.
 */
char *get_path(char *command)
{
	char *path, *path_copy, *token, *file_path;
	int i;

	if (command[0] == '/' || command[0] == '.')
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}

	path = NULL;
	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
		{
			path = environ[i] + 5;
			break;
		}
	}

	if (!path || *path == '\0')
		return (NULL);

	path_copy = strdup(path);
	token = strtok(path_copy, ":");
	while (token)
	{
		file_path = malloc(strlen(token) + strlen(command) + 2);
		if (!file_path)
			break;
		sprintf(file_path, "%s/%s", token, command);

		if (access(file_path, X_OK) == 0)
		{
			free(path_copy);
			return (file_path);
		}
		free(file_path);
		token = strtok(NULL, ":");
	}
	free(path_copy);
	return (NULL);
}

/**
 * main - Entry point for the simple shell.
 * Return: The exit status of the last executed command.
 */
int main(void)
{
	char *line = NULL, *full_path = NULL;
	size_t len = 0;
	char *argv[1024];
	int i, status, exit_status = 0;
	pid_t child_pid;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf("$ ");

		if (getline(&line, &len, stdin) == -1)
			break;

		i = 0;
		argv[i] = strtok(line, " \t\r\n\a");
		while (argv[i])
			argv[++i] = strtok(NULL, " \t\r\n\a");

		if (argv[0] == NULL)
			continue;

		full_path = get_path(argv[0]);

		if (full_path == NULL)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
			exit_status = 127;
			continue;
		}

		child_pid = fork();
		if (child_pid == 0)
		{
			if (execve(full_path, argv, environ) == -1)
			{
				perror("Error");
				exit(2);
			}
		}
		else
		{
			wait(&status);
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			free(full_path);
		}
	}
	free(line);
	return (exit_status);
}
