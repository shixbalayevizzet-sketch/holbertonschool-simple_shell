#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * main - A simple shell that handles command lines with arguments.
 * * Return: Always 0 on success.
 */
int main(void)
{
    char *line = NULL;      /* Buffer for the input string */
    size_t len = 0;         /* Size of the buffer */
    ssize_t nread;          /* Number of characters read */
    char *argv[1024];       /* Array to store command and arguments */
    int i;
    pid_t child_pid;
    int status;

    while (1)
    {
        /* Display prompt only in interactive mode */
        if (isatty(STDIN_FILENO))
            printf("$ ");

        /* Read input from user */
        nread = getline(&line, &len, stdin);

        /* Handle End of File (Ctrl+D) */
        if (nread == -1)
        {
            if (isatty(STDIN_FILENO))
                printf("\n");
            break;
        }

        /* Tokenize the input string into arguments */
        i = 0;
        argv[i] = strtok(line, " \t\r\n\a");

        while (argv[i] != NULL)
        {
            i++;
            argv[i] = strtok(NULL, " \t\r\n\a");
        }

        /* If no command was entered, skip execution */
        if (argv[0] == NULL)
            continue;

        /* Create a child process */
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("Fork failed");
            continue;
        }

        if (child_pid == 0)
        {
            /* Child process: Execute the command */
            if (execve(argv[0], argv, NULL) == -1)
            {
                perror("./shell"); /* Print error if command fails */
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent process: Wait for the child to finish */
            wait(&status);
        }
    }

    free(line); /* Free allocated memory before exiting */
    return (0);
}
