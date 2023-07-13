/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
int main (int argc, char *argv[])
{
	int segmento, *p, id, pid, status, i;
	
	segmento = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR); // Aloca a memória compartilhada
	p = (int *) shmat (segmento, 0, 0); // Associa a memória compartilhada ao processo
	
	if(*p == -1) // Compara o retorno com -1 para verificar sucesso
	{
		printf("Erro ao atrelar a memória compartilhada");
	}
	
	*p = 0; // Inicializa variável a ser utilizada com 0
	
	if ((id = fork()) < 0) // Verifica se o processo foi criado com sucesso
	{
		puts ("Erro na criação do novo processo");
		exit (-2);
	}
	else if (id == 0) // Caso Filho
	{
		for(i = 0; i < 10000; i++)
		{
			*p += 2; // Soma dois à variável compartilhada na memória
		}
		printf("%d - Filho val: %d\n", i, *p); // Exibe valor final após execução do Processo Filho
	}
	else // Caso Pai
	{
		for(i = 0; i < 10000; i++)
		{
			*p += 3; // Soma três à variável compartilhada na memória
		}
		printf("%d - Pai val: %d\n", i, *p); // Exibe valor final após execução do Processo Pai
		pid = wait (&status); // Espera Processo Filho finalizar
	}
	
	shmdt (p); // Libera a memória compartilhada do processo
	shmctl (segmento, IPC_RMID, 0); // Libera a memória compartilhada
	return 0;
}