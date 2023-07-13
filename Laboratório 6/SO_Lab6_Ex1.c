/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

#define LIM 64		// Para limitar o tamanho da fila 
#define ELEM 256	// Quantidade de mensagens a serem trocadas

typedef struct msgbuf 
{
         long    mtype;
         int    mNum;
} message_buf;

void produtor(int msgID);	
void consumidor(int msgID, int i);

int main(void)
{
	int msgID, id;
	msgID = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);	// Cria fila de mensagens e guarda o ID da fila em msgID
	if(msgID == -1)
	{
		printf("Erro na criação da Fila de Mensagens\n");
	}
	struct msqid_ds aux;			// Cria estrutura auxiliar para controlar a fila de mensagens
	msgctl(msgID, IPC_STAT, &aux);	// Coloca as definições da estrutura associada à fila de mensagens na estrutura auxiliar
	aux.msg_qbytes = sizeof(int)*LIM;	// Altera o limite de bytes permitidos na fila de mensagens definido na estrutura auxiliar
	msgctl(msgID, IPC_SET, &aux);		// Seta os valores de alguns campos da estrutura associada à fila de mensagens de acordo com os respectivos valores na estrutura auxiliar
	/* Como a estrutura auxiliar tem as mesmas definições da estrutura associada à fila exceto pelo campo msg_qbytes, apenas esse será alterado */
	id = fork();
	if(id == 0)		// Caso filho (produtor)
	{
		produtor(msgID);
	}
	else			// Caso pai 
	{
		id = fork();
		if(id == 0)	// Caso segundo filho (consumidor 1);
		{
			consumidor(msgID, 1);
		}
		else		// Caso pai (consumidor 2);
		{
			consumidor(msgID, 2);
		}
	}
}

void produtor(int msgID)
{
	message_buf buffer;		// Cria buffer para enviar a mensagem
	int i, res;
	for(i = 0; i < ELEM; i++)	// Produzindo ELEM mensagens (ELEM = 256)
	{
		buffer.mtype = 1;	// Seta o tipo da mensagem no buffer de mensagens
		buffer.mNum = i;	// Coloca um inteiro no buffer de mensagens
		res = msgsnd(msgID, &buffer, sizeof(buffer.mNum), 0);	// Coloca mensagem na fila. Caso não tenha espaço na fila, bloqueia até ter
		if(res == -1) 
		{ 
			perror("Erro ao enviar mensagem"); 
		}
		printf("Produtor enviando %d para a fila de mensagens\n", buffer.mNum);
		//sleep(1);		// Envia a cada 1 segundo
	}
	/* Após ter produzido os 256 elementos, consumidor envia mensagem para os consumidores indicando que acabou */
	buffer.mtype = 2;
	res = msgsnd(msgID, &buffer, 0, 0);
}
void consumidor(int msgID, int i)
{
	message_buf buffer;		// Cria buffer para receber a mensagem
	int res;
	while(1)			// Os consumidores estão sempre esperando para consumir algo
	{
		res = msgrcv(msgID, &buffer, sizeof(buffer.mNum), 0, 0);	// Tira mensagem da fila e coloca no buffer 
		/* Lê a primeira mensagem (de qualquer tipo) da fila. Bloqueia caso não tenha mensagem na fila */
		if(res == -1) 
		{ 
			perror("Erro ao receber mensagem"); 
		}
		if(buffer.mtype == 2)	// Se a mensagem for do tipo 2, é porque o produtor parou de produzir
		{
			printf("Acabou\n");
			break;
		}
		printf("Consumidor %d recebendo %d da fila de mensagens\n", i, buffer.mNum);
		sleep(i);			// Consumidor i lê a cada i segundo(s)
	}
}
