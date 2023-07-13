#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define N 32		// Quantidade de mensagens
#define TAM_BUFFER 8	

typedef struct {	// Lista circular para representar o buffer
	int vet[TAM_BUFFER];
	int ini;
	int fim;
}Lista;
	
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
void escreve(int msg, Lista* buffer);	// Escreve mensagem no buffer
int le(Lista* buffer);			// Le mensagem do buffer

int main(int argc, char *argv[])
{
	int segmento, id, pid, status, i, cheio, vazio, mutex, msg;
	Lista *buffer;
	
	segmento = shmget (IPC_PRIVATE, sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR); // Aloca a memória compartilhada
	if(segmento < 0)
	{
		perror("Erro ao alocar a memória compartilhada");
		exit(0);
	}
	buffer = (Lista *) shmat (segmento, 0, 0); 		// Associa a memória compartilhada ao processo
	if (buffer == -1) 
	{
		perror("Erro ao atrelar a memória compartilhada");
		exit(0);
	}
	buffer->ini = buffer->fim = 0; 
	cheio = semget (8752, 1, 0666 | IPC_CREAT);		// Cria semáforo para controlar se o buffer está cheio
	vazio = semget (8753, 1, 0666 | IPC_CREAT);		// Cria semáforo para controlar se o buffer está vazio
	mutex = semget (8754, 1, 0666 | IPC_CREAT);		// Cria semáforo para controlar a entrada na região crítica
	if (cheio < 0 || vazio < 0 || mutex < 0)
	{
		perror("Erro ao criar semáforo");
		exit(0);
	}
	/* Inicializando os semáforos */
	setSemValue(cheio, 0); 	
	setSemValue(vazio, TAM_BUFFER);  
	setSemValue(mutex, 1);
	
	if ((id = fork()) < 0) // Verifica se o processo foi criado com sucesso
	{
		puts ("Erro na criação do novo processo");
		exit (-2);
	}
	
	if (id == 0) 	// Caso filho (leitor)
	{ 
		for(i = 0; i < N; i++)
		{
			semaforoP(cheio);	// Verifica se tem mensagem no buffer
			semaforoP(mutex);	// Entra na região crítica		
			msg = le(buffer);	// Realiza a leitura
			printf("Mensagem lida: %d\n", msg);
			semaforoV(mutex);	// Sai da região crítica
			semaforoV(vazio);	// Libera espaço no buffer	
		}
	
	}
	else		// Caso pai (escritor) 
	{
		for (i = 0; i < N; i++) 
		{
			semaforoP(vazio);	// Verifica se tem espaço no buffer
			semaforoP(mutex);	// Entra na região crítica
			escreve(i, buffer);	
			printf("Escreveu %d\n", i);
			semaforoV(mutex); 	// Sai da região crítica		
			semaforoV(cheio); 	// Incrementa o preenchimento do buffer
		}
		pid = wait (&status); // Espera Processo Filho finalizar
		/* Após finalizar os processos, remove os semáforos */
		delSemValue(mutex);   
		delSemValue(vazio);
		delSemValue(cheio);
	}
	shmdt (buffer); 				// Libera a memória compartilhada do processo
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
void escreve(int msg, Lista *buffer)
{
	buffer->vet[buffer->fim] = msg;		// Escreve a mensagem no final da lista
	buffer->fim = (buffer->fim + 1) % TAM_BUFFER;	// Ajusta o final da lista
}
int le(Lista *buffer)
{
	int msg = buffer->vet[buffer->ini];		// Guarda a mensagem lida
	buffer->ini = (buffer->ini + 1) % TAM_BUFFER; // Ajusta o início da lista
	return msg;
}
	
	
	
	
