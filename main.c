/**
* @file main.c
* @date 2020
* @author José Oliveira, Ricardo Reuter




In progress:  
-Signals -> José

Not Working:

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
#include <time.h>
#include <sys/time.h>

/* -------------- Signal Treatment ------------- */

void trata_sinal(int signal);
struct timeval tv;
time_t t;
struct tm *info;
char buffer[64];

void trata_sinal(int signal)
{
	int aux;
	/* Copia da variavel global errno */
	aux = errno;

	/* Signal SIGINT */
	if (signal == SIGINT)
	{
		printf(" \n *************************************** \n");
		printf("  Recieved signal SIGINT from PID (%d) \n", getpid());
		printf(" *************************************** \n");

		exit(0);

		/* Restaura valor da variavel global errno */
		errno = aux;
	}

	/* Signal SIGUSR1 */
	if (signal == SIGUSR1)
	{
		printf(" \n *************************************** \n");
		printf("Recebi o sinal SIGUSR1 (%d)\n", signal);

		/* Data */
		strftime(buffer, sizeof buffer, "%Y-%m-%dT%X%z.\n", info);
		printf("%s", buffer);

		printf(" *************************************** \n");

		exit(0);
		/* Restaura valor da variavel global errno */
		errno = aux;
	}

	/* Signal SIGUSR2 */
	if (signal == SIGUSR2)
	{
		printf(" \n *************************************** \n");
		printf("Recebi o sinal SIGUSR2 (%d)\n", signal);

		/* Data */
		strftime(buffer, sizeof buffer, "%Y-%m-%dT%X%z.\n", info);
		printf("%s", buffer);

		printf(" *************************************** \n");

		exit(0);
		/* Restaura valor da variavel global errno */
		errno = aux;
	}
}
/* ---------- End Signal Treatment ------------- */

/* ---------- Function Wrong Request ------------- */

char WrongRequestMessage(char *wrongcommand)
{
	printf("[ERROR] Wrong Request '%s' \n", wrongcommand);
	return 0;
}

/* ---------- End Function Wrong Request ------------- */

int main(int argc, char *argv[])
{

	/* Disable warnings */
	(void)argc;
	(void)argv;

	char *exitcode = "bye";
	int con = 0;
	char input_shell[80];
	struct sigaction act;
	char wrongrequestplaceholder[80];

	/* Definir a rotina de resposta a sinais */
	act.sa_handler = trata_sinal;

	/* mascara sem sinais -- nao bloqueia os sinais */
	sigemptyset(&act.sa_mask);

	act.sa_flags = SA_RESTART; /*recupera chamadas bloqueantes*/

	/* Captura do sinal SIGUSR2 */
	if (sigaction(SIGUSR2, &act, NULL) < 0)
	{
		ERROR(1, "sigaction - SIGUSR1");
	}

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

	//Check all arguments from terminal
	for (int i = 0; i < argc; ++i)
	{
		printf("ARGS[%d] de input: %s \n", i, argv[i]);
	}

	if (argc == 1)
	{
		/* Data */
		gettimeofday(&tv, NULL);
		t = tv.tv_sec;
		info = localtime(&t);

		while (con == 0)
		{
			printf("nanoShell$: ");
			fgets(input_shell, sizeof(input_shell), stdin);

			//Cleans enter from input
			input_shell[strlen(input_shell) - 1] = 0;

			//Coppy input to wrongrequesteplaceholder to pass later full comand in function
			strcpy(wrongrequestplaceholder, input_shell);

			/* BYE Function */
			if (strcmp(input_shell, exitcode) == 0)
			{

				printf("[INFO] bye command detected. Terminating nanoshell \n");
				return 0;
			}
			else
			{

				int i = 0;
				char *p = strtok(input_shell, " ");
				char *arguments[80];
				int count = 0;

				//Split input to array
				while (p != NULL)
				{
					arguments[i++] = p;
					p = strtok(NULL, " ");
					//Add counter to know size of for loop in next step
					count++;
				}

				for (i = 0; i < count; i++)
				{
					if (strstr(arguments[i], "'") || strstr(arguments[i], "\"") || strstr(arguments[i], "|") != NULL)
					{
						WrongRequestMessage(wrongrequestplaceholder);
					}
				}

				//Set array to pass only arguments (ex: ls la lb | arrey [0] = la arrey[1] = lb)
				for (i = 0; i < count; i++)
				{
					if (i == count - 1)
					{
						if (i == 0)
							arguments[1] = NULL;
						else
						{
							arguments[count] = NULL;
						}
					}
				}

				// //Troubleshooting variables and arreys
				// for (i = 0; i < count; i++)
				// {

				// 	printf("arguments [%d] é : %s \n", i, arguments[i]);
				// 	printf("input : %d \n", count);
				// }

				// ************** Fork to execute input from shell ************************
				pid_t pid = fork();
				if (pid == 0)
				{
					// you are in the child process
					//This will run the first command with the arguments from arrey
					execvp(input_shell, arguments);
					return 0;
				}
				else if (pid > 0)
				{
					int status;
					// you are in the parent process
					// Wait for pid to finish
					waitpid(pid, &status, WNOHANG);
					// ends zombie process
					waitpid(pid, &status, 0);
				}
				// ************** END of Fork to execute input from shell ************************
			}
		}
	}
	else
	{
		for (int i = 0; i < argc; i++)
		{
			if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
			{
				printf(" \n *************************************** \n");
				printf("Usage: ./Execomando [OPTION]... [FILE]... \n\n");
				printf(" -f. --file			Execute commands found in file \n");
				printf(" -h. --help			Displays options and more info \n");
				printf(" -m. --max			Specify how many commands can be executed by the nanoShell  \n");
				printf(" -s. --signalfile		Program will creat a txt file with SIGNAL commands named 'signal.txt' in local directory \n");
				printf(" \n ------------------------------------------------------ \n");
				printf(" |Author:   José Oliveira      Student Number: 2191738| \n");
				printf(" |Author:   Ricardo Reuter     Student Number: 2191739| \n");
				printf(" ------------------------------------------------------ \n");
				printf(" *************************************** \n");

				exit(0);
			}
		}
	}
}
