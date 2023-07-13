/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/types.h>
#include<stdio.h>
#include <unistd.h>

#define LIM 1		// Para limitar o tamanho da fila
#define ELEM 32	// Quantidade de mensagens a serem trocadas

typedef struct msgbuf 
{
         long    mtype;
         int    mNum;
} message_buf;

void produtor(int msgID);
void consumidor(int msgID);

int main(void)
{
	int msgID, id;
	msgID = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
	struct msqid_ds aux;			// Cria estrutura auxiliar para controlar a fila de mensagens
	msgctl(msgID, IPC_STAT, &aux);	// Coloca as definições da estrutura associada à fila de mensagens na estrutura auxiliar
	aux.msg_qbytes = sizeof(int)*LIM;	// Altera o limite de bytes permitidos na fila de mensagens definido na estrutura auxiliar
	msgctl(msgID, IPC_SET, &aux);		// Seta os valores de alguns campos da estrutura associada à fila de mensagens de acordo com os respectivos valores na estrutura auxiliar
						// Como a estrutura auxiliar tem as mesmas definições da estrutura associada à fila exceto pelo campo msg_qbytes, apenas esse será alterado
	if(msgID == -1)
	{
		printf("Erro na criação da Fila de Mensagens\n");
	}
	id = fork();
	if(id == 0)	// Caso processo filho (produtor)
	{
		produtor(msgID);
	}
	else		// Caso processo pai (consumidor)
	{
		consumidor(msgID);
	}
	
}

void produtor(int msgID)
{
	message_buf buffer;		// Cria buffer de mensagem
	int i, res_rcv, res_snd;
	for(i = 0; i < ELEM; i++)		// Produz ELEM mensagens (ELEM = 32)
	{
		res_rcv = msgrcv(msgID, &buffer, 0, 2, 0);	// Recebe mensagem do tipo 2 para indicar que o produtor pode produzir  
		if(res_rcv == -1) 
		{ 
			perror("Erro ao receber mensagem"); 	
		}
		buffer.mtype = 1;	// Seta o tipo da mensagem
		buffer.mNum = i;	// Coloca inteiro no buffer
		res_snd = msgsnd(msgID, &buffer, sizeof(buffer.mNum), 0);	// Envia para a fila
		if(res_snd == -1) 
		{ 
			perror("Erro ao enviar mensagem"); 
		}
		printf("Enviando %d\n", buffer.mNum);
	}
}
void consumidor(int msgID)
{
	message_buf buffer;		// Cria buffer de mensagem
	int res_rcv, res_snd, i;
	buffer.mtype = 2;				// Seta o tipo da mensagem para enviar mensagem vazia
	res_snd = msgsnd(msgID, &buffer, 0, 0);	// Envia mensagem vazia para o produtor
	if(res_snd == -1) 
	{ 
		perror("Erro ao enviar mensagem"); 
	}
	for(i = 0; i < ELEM; i++)	// Consome ELEM mensagens (ELEM = 32)
	{
		res_rcv = msgrcv(msgID, &buffer, sizeof(buffer.mNum), 1, 0);		// Lê mensagem do tipo 1 da fila de mensagens
		if(res_rcv == -1) 	
		{ 
			perror("Erro ao receber mensagem"); 
		}
		printf("Recebendo %d\n", buffer.mNum);
		buffer.mtype = 2;				// Seta tipo da mensagem para enviar mensagem vazia
		res_snd = msgsnd(msgID, &buffer, 0, 0);	// Envia mensagem vazia como resposta
		if(res_snd == -1) 
		{ 
			perror("Erro ao enviar mensagem"); 
		}
	}
}
