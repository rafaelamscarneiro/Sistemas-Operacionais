#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define N 1000		// Quantidade de mensagens

union semun
{
	int val;
	struct semid_ds *buf;
	ushort *array;
};

int setSemValue(int semId, int val);	// Inicializa o valor do semáforo
void delSemValue(int semId);		// Remove o semáforo
int semaforoP(int semId);		// Up
int semaforoV(int semId);		// Down

int main(int argc, char *argv[])
{
	int segmento, *p, id, pid, status, i, escrita, leitura;
	
	segmento = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR); // Aloca a memória compartilhada
	if(segmento < 0)
	{
		perror("Erro ao alocar a memória compartilhada");
		exit(0);
	}
	p = (int *) shmat (segmento, 0, 0); 	// Associa a memória compartilhada ao processo
	if(*p == -1) 	// Compara o retorno com -1 para verificar sucesso
	{
		printf("Erro ao atrelar a memória compartilhada");
	}
	escrita = semget(8752, 1, 0666 | IPC_CREAT);		// Cria semáforo para controlar a escrita
	if(escrita < 0) 
	{
		perror("Erro na criação do semáforo");
		exit(0);
	}
	leitura = semget (8753, 1, 0666 | IPC_CREAT);		// Cria semáforo para controlar a leitura
	if (leitura < 0) 
	{
		perror("Erro na criação do semáforo");
		exit(0);
	}
	setSemValue(escrita, 1); // Inicializando semáforo da escrita liberado
	setSemValue(leitura, 0); // Inicializando semáforo da leitura bloqueado
	
	if ((id = fork()) < 0) // Verifica se o processo foi criado com sucesso
	{
		puts ("Erro na criação do novo processo");
		exit (-2);
	}
	
	if (id == 0) 		// Caso filho (leitor)
	{ 		
		for(i = 0; i < N; i++)
		{
			semaforoP(leitura);	// Confere se pode ler			
			printf("Leu %d\n", *p);
			semaforoV(escrita);	// Permite a escrita	
		}
	
	}
	else			// Caso pai (escritor) 
	{ 
		for (i = 0; i < N; i++) 
		{
			semaforoP(escrita);	// Confere se pode escrever
			*p = i;
			printf("Escreveu %d\n", i);		
			semaforoV(leitura); 	// Permite a leitura
		}
		pid = wait (&status); // Espera Processo Filho finalizar
		/* Após finalizar os processos, remove os semáforos */
		delSemValue(escrita);
		delSemValue(leitura);
	}
	shmdt (p); 					// Libera a memória compartilhada do processo
	shmctl (segmento, IPC_RMID, 0); 		// Libera a memória compartilhada
	
	return 0;
}

int setSemValue(int semId, int val)
{
	union semun semUnion;
	semUnion.val = val;
	return semctl(semId, 0, SETVAL, semUnion);
}
void delSemValue(int semId)
{
	union semun semUnion;
	semctl(semId, 0, IPC_RMID, semUnion);
}
int semaforoP(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = -1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
int semaforoV(int semId)
{
	struct sembuf semB;
	semB.sem_num = 0;
	semB.sem_op = 1;
	semB.sem_flg = SEM_UNDO;
	semop(semId, &semB, 1);
	return 0;
}
