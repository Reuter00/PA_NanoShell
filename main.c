/**
* @file main.c
* @date 2020
* @author José Oliveira, Ricardo Reuter

NOT WORKING: 
- BYE
- COMMAND WITH NO ARGUMENTS, only works with space after


In progress: Signals
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "debug.h"
#include "memory.h"

/* ---------- Signal Treatment ------------- */

void trata_sinal(int signal);

void trata_sinal(int signal)
{
	int aux;
	/* Copia da variavel global errno */
	aux = errno;

	/* Signal SIGINT */
	if (signal == SIGINT)
	{
		printf(" \n ----------------------------------------- \n");
		printf("  Recieved signal SIGINT from PID (%d) \n", getpid());
		printf(" ----------------------------------------- \n");
		exit(0);
		/* Restaura valor da variavel global errno */
		errno = aux;
	}

	/* Signal SIGUSR1 */
	if (signal == SIGUSR1)
	{
		printf(" \n ----------------------------------------- \n");
		printf("  Recebi o sinal SIGUSR1 (%d)\n", signal);
		printf(" ----------------------------------------- \n");

		pid_t pid1 = fork();
		if (pid1 == 0)
		{ /* Processo filho que manda sinal 5 em 5 sec */
			printf("Filho 1\n");
			sleep(6);
		}
		else if (pid1 > 0)
		{ /* Processo pai */
			printf("Pai criou o filho 1\n");
		}
		exit(0);
		/* Restaura valor da variavel global errno */
		errno = aux;
	}
}
/* ---------- Signal Treatment ------------- */

int main(int argc, char *argv[])
{

	/* Disable warnings */
	(void)argc;
	(void)argv;

	char Exitcode[4] = "bye";
	int con = 0, i;
	char input_shell[20];
	char *argvNULL;
	struct sigaction act;

	/* Definir a rotina de resposta a sinais */
	act.sa_handler = trata_sinal;

	/* mascara sem sinais -- nao bloqueia os sinais */
	sigemptyset(&act.sa_mask);

	act.sa_flags = SA_RESTART; /*recupera chamadas bloqueantes*/

	/* Captura do sinal SIGUSR1 */
	if (sigaction(SIGUSR1, &act, NULL) < 0)
	{
		ERROR(1, "sigaction - SIGUSR1");
	}

	/* Captura do sinal SIGINT */
	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		ERROR(1, "sigaction - SIGINT");
	}

	// Get all arguments from terminal
	for (int i = 0; i < argc; ++i)
		argv[i];

	if (argc == 1)
	{

		while (con = 1)
		{

			printf("nanoShell$: ");

			fgets(input_shell, sizeof(input_shell), stdin);

			if (strcmp(input_shell, Exitcode) == 0)
			{
				/* BYE NOT WOTKING because of size of char */
				printf("[INFO] bye command detected. Terminating nanoshell \n");
				return 0;
			}
			else
			{

				char **res = NULL;
				char *p = strtok(input_shell, " ");
				int n_spaces = 0, i;

				/* split string and append tokens to 'res' */

				while (p)
				{
					res = realloc(res, sizeof(char *) * ++n_spaces);

					if (res == NULL)
						exit(-1); /* memory allocation failed */

					res[n_spaces - 1] = p;

					p = strtok(NULL, " ");
				}

				/* realloc one extra element for the last NULL */

				res = realloc(res, sizeof(char *) * (n_spaces + 1));
				res[n_spaces] = 0;

				/* Assign arguments to res, making res an array with arguments only*/

				for (i = 1; i < (n_spaces + 1); ++i)
				{
					res[i - 1] = res[i];
				}

				//This will run the first command with the arguments from res
				execvp(input_shell, res);

				/* free the memory allocated */
				free(res);
			}
		}
	}
}
