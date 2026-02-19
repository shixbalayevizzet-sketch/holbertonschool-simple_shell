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

	/* If command is already a full path, check if it exists */
	if (command[0] == '/' || command[0] == '.')
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));
		return (NULL);
	}

	/* Find PATH in environment variables */
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
		/* Construct full path: directory + / + command */
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
		/* Print prompt if in interactive mode */
		if (isatty(STDIN_FILENO))
			printf("$ ");

		/* Read input */
		if (getline(&line, &len, stdin) == -1)
			break;

		/* Tokenize input */
		i = 0;
		argv[i] = strtok(line, " \t\r\n\a");
		while (argv[i])
			argv[++i] = strtok(NULL, " \t\r\n\a");

		if (argv[0] == NULL)
			continue;

		/* Find
