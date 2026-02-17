#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/**
 * get_path - Əmrin tam yolunu tapır (PATH daxilində)
 * @command: İstifadəçinin yazdığı əmr (məs: ls)
 * Return: Tam yol (məs: /bin/ls) və ya NULL
 */
char *get_path(char *command)
{
    char *path = NULL, *path_copy = NULL, *token = NULL, *file_path = NULL;
    int i;

    /* Əgər əmr artıq tam yoldursa (/bin/ls), onu yoxla və qaytar */
    if (access(command, X_OK) == 0)
        return (strdup(command));

    /* Environment-dən PATH-i tap */
    for (i = 0; environ[i]; i++)
    {
        if (strncmp(environ[i], "PATH=", 5) == 0)
        {
            path = environ[i] + 5;
            break;
        }
    }
    if (!path) return (NULL);

    path_copy = strdup(path);
    token = strtok(path_copy, ":");

    while (token != NULL)
    {
        /* Qovluq yolu + / + əmr üçün yaddaş ayır */
        file_path = malloc(strlen(token) + strlen(command) + 2);
        sprintf(file_path, "%s/%s", token, command);

        if (access(file_path, X_OK) == 0) /* Əmr tapıldısa */
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

int main(void)
{
    char *line = NULL, *full_path = NULL;
    size_t len = 0;
    char *argv[1024];
    int i, status;
    pid_t child_pid;

    while (1)
    {
        if (isatty(STDIN_FILENO)) printf("$ ");
        if (getline(&line, &len, stdin) == -1) break;

        i = 0;
        argv[i] = strtok(line, " \t\r\n\a");
        while (argv[i]) argv[++i] = strtok(NULL, " \t\r\n\a");

        if (argv[0] == NULL) continue;

        /* PATH-dən tam yolu tapırıq */
        full_path = get_path(argv[0]);

        if (full_path == NULL) /* Əmr tapılmadısa FORK ETMİRİK */
        {
            fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
            continue;
        }

        child_pid = fork();
        if (child_pid == 0)
        {
            if (execve(full_path, argv, environ) == -1)
            {
                perror("Error");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            wait(&status);
            free(full_path);
        }
    }
    free(line);
    return (0);
}
