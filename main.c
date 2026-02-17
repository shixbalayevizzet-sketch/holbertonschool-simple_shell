#include "shell.h"

/**
 * main - Entry point for the simple shell
 * @ac: Argument count
 * @av: Argument vector
 *
 * Return: 0 on success
 */
int main(int ac, char **av)
{
	(void)ac;
	(void)av;

	write(STDOUT_FILENO, "($) ", 4);
	return (0);
}
