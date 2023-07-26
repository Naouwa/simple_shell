#include "main.h"
/**
 * prompt - displays the prompt
 *
 * Return: Always 0 (success)
 */
int prompt(void)
{
	ssize_t counter = 0;
	char *sign = "$ ";

	if (isatty(STDIN_FILENO) == 1)
	{
		counter = write(STDOUT_FILENO, sign, 2);
		if (counter == -1)
			exit(0);
	}

	return (0);
}

/**
 * reader - reads input
 *
 * Return: pointer to read content
 */
char *reader(void)
{
	size_t n = 0;
	char *buff = NULL;
	int i;
	ssize_t counter = 0;

	counter = getline(&buff, &n, stdin);
	if (counter == -1)
	{
		free(buff);
		if (isatty(STDIN_FILENO) != 0)
			write(STDOUT_FILENO, "\n", 1);
		exit(0);
	}

	counter = counter - 1;
	if (buff[counter] == '\t' || buff[counter] == '\n')
		buff[counter] = '\0';

	for (i = 0; buff[i]; i++)
	{
		if (buff[i] == '#' && buff[i - 1] == ' ')
		{
			buff[i] = '\0';
			break;
		}
	}
	return (buff);
}

/**
 * path - gets the full path to a command
 * @av: pointer to the command and options
 * @PATH: pointer to PATH
 * @cpy: pointer to copy of PATH
 *
 * Return: pointer to full path to the commands exe file
 */
char *path(char **av, char *PATH, char *cpy)
{
	struct stat s;
	char *fullpath = NULL, *buff = NULL, *token;
	static char temp[256];
	int i = 0, tokenlen = 0, counter = 0, flag = 0;

	cpy = NULL;
	cpy = _strdup(PATH);
	counter = splitter(cpy);
	token = strtok(cpy, ": =");

	while (token != NULL)
	{
		buff = completeconcat(temp, av, token);
		if (stat(buff, &s) == 0)
		{
			fullpath = (buff);
			flag = 1;
			break;
		}
		if (i < counter - 2)
		{
			tokenlen = _strlen(token);
			if (token[tokenlen + 1] == ':')
			{
				if (stat(av[0], &s) == 0)
				{
					fullpath = strdup(av[0]);
					flag = 1;
					break;
				}
			}
		}
		i++;
		token = strtok(NULL, ":");
	}
	if (flag == 0)
		fullpath = _strdup(av[0]);

	free(cpy);
	return (fullpath);
}

/**
 * _forku - executes command line
 * @av: pointer to arguments
 * @buff: pointer to buffer
 * @path: pointer to user input
 *
 * Return: Always 0 (success)
 */
int _forku(char **av, char *buff, char *path)
{
	pid_t pid;
	int i, res, status;
	int exitstatus = 0;

	pid = fork();
	if (pid == -1)
	{
		perror("Error");
		exit(1);
	}
	if (pid == 0)
	{
		res = execve(path, av, environ);
		if (res == -1)
		{
			perror(av[0]);
			for (i = 0; av[i]; i++)
				free(av[i]);
			free(av);
			free(buff);
			exit(127);
		}
	}
	wait(&status);
	if (WIFEXITED(status))
		exitstatus = WEXITSTATUS(status);

	for (i = 0; av[i]; i++)
		free(av[i]);
	free(av);
	free(buff);

	return (exitstatus);
}
