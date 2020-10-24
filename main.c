/**
* @file main.c
* @date 2020
* @author José Oliveira, Ricardo Reuter

NOT WORKING:


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
		//int status = system("ps -aux | grep %d | cut -d ' ' -f27", getpid());
		printf(" ----------------------------------------- \n");

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

	char *exitcode = "bye";
	int con = 0, i;
	char input_shell[20];
	char *argvNULL;
	struct sigaction act;
	int tt = 0;

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

	// // Get all arguments from terminal
	// for (int i = 0; i < argc; ++i)
	// 	argv[i];

	if (argc == 1)
	{

		while (con == 0)
		{

			printf("nanoShell$: ");
			fgets(input_shell, sizeof(input_shell), stdin);

			//Cleans enter from input
			input_shell[strlen(input_shell) - 1] = 0;

			if (strcmp(input_shell, exitcode) == 0)
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

				// ************** Fork to execute input from shell ************************
				pid_t pid = fork();
				if (pid == 0)
				{
					// you are in the child process
					//This will run the first command with the arguments from res
					execvp(input_shell, res);
					return 0;
				}
				else if (pid > 0)
				{
					// you are in the parent process
				}
				// ************** END of Fork to execute input from shell ************************
			}
		}
	}
}
