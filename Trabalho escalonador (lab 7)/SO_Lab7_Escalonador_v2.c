#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

#define SIZE 15
#define SMH_KEY 0x1968
#define UT 1

typedef struct processo
{
	char nome[SIZE];
	int prioridade;
	int inicio;
	int total;
	int pid;
	int executado;
	struct processo *prox;
}Processo;

Processo *listaProc = NULL;
Processo *listaExec = NULL;
Processo *listaPronto = NULL;

int contUT = 0;

int criaProc()
{
	int pid;
	if (( pid = fork () ) < 0) 
	{
		printf (" Erro ao criar filho\n");
		exit (1);
	}
	if (pid==0)
	{
		pid = getppid();
		while(1);
	}
	return pid;
}

Processo* insereProc(char *nome, int prio, int ini, int total)
{
	Processo* novo = (Processo*) malloc(sizeof(Processo));
	Processo* atual = listaProc;
	Processo* anterior = NULL;
	if(novo == NULL)
	{
		printf("Erro ao criar novo Processo\n");
		exit(1);
	}
	strcpy(novo->nome, nome);
	novo->prioridade = prio;
	novo->inicio = ini;
	novo->total = total;
	novo->executado = 0;
	novo->prox = NULL;
	
	if(listaProc == NULL)
	{
		printf("Processo %s foi inserido na lista\n", novo->nome);
		return novo;
	}
	while(atual != NULL && novo->inicio > atual->inicio)
	{
		anterior = atual;
		atual = atual->prox;
	}
	novo->prox = atual;
	anterior->prox = novo;
	printf("Processo %s foi inserido na lista\n", novo->nome);
	return listaProc;
}

Processo* inserePronto(Processo *lProc) // Tem que ter argumento pois pode ser chamado por listaPocessos como listaExec
{
	Processo* novo = (Processo*) malloc(sizeof(Processo));
	Processo* atual = listaPronto;
	Processo* anterior = NULL;
	if(novo == NULL)
	{
		printf("Erro ao criar novo Processo\n");
		exit(1);
	}
	strcpy(novo->nome, lProc->nome);
	novo->prioridade = lProc->prioridade;
	novo->inicio = lProc->inicio;
	novo->total = lProc->total;
	
	if(lProc->pid == 0)
	{
		novo->pid = criaProc();
		kill(novo->pid, SIGSTOP);
	}
	else
	{
		novo->pid = lProc->pid;
		kill(novo->pid, SIGSTOP);
	}
	
	novo->executado = lProc->executado;
	novo->prox = NULL;
	if(listaPronto == NULL)
	{
		printf("Processo %s está Pronto\n", novo->nome);
		return novo;
	}
	while(atual != NULL && novo->prioridade > atual->prioridade)
	{
		anterior = atual;
		atual = atual->prox;
	}
	novo->prox = atual;
	if(anterior == NULL)		// PRIORIDADE DO NOVO < PRIORIDADE DO LISTAPRONTOS 
	{
		return novo;	
	}
	else
	{
		anterior->prox = novo;
	}
	return listaPronto;
}

Processo* insereExec()
{
	Processo* novo = (Processo*) malloc(sizeof(Processo));
	Processo* atual = listaExec;
	Processo* anterior = NULL;
	Processo* aux;
	if(novo == NULL)
	{
		printf("Erro ao criar novo Processo\n");
		exit(1);
	}
	strcpy(novo->nome, listaPronto->nome);
	novo->prioridade = listaPronto->prioridade;
	novo->inicio = listaPronto->inicio;
	novo->total = listaPronto->total;
	novo->pid = listaPronto->pid;
	novo->executado = listaPronto->executado;
	novo->prox = NULL;
	if(listaExec == NULL)
	{
		aux = listaPronto;
		listaPronto = listaPronto->prox;
		free(aux);
		printf("Processo %s será Executado\n", novo->nome);
		return novo;
	}
	while(atual != NULL && novo->prioridade == atual->prioridade)
	{
		aux = listaPronto;
		listaPronto = listaPronto->prox;
		free(aux);
		while(atual != NULL && (atual->inicio < novo->inicio))
		{
			anterior = atual;
			atual = atual->prox;
		}
		novo->prox = atual;
		anterior->prox = novo;
		printf("Processo %s será Executado\n", novo->nome);
	}
	if(atual != NULL && novo->prioridade < atual->prioridade)
	{
		aux = listaPronto;
		listaPronto = listaPronto->prox;
		free(aux);
		while(atual != NULL)
		{
			listaPronto = inserePronto(atual);
			aux = atual;
			atual = atual->prox;
			free(aux);
		}
		listaExec = novo;
		printf("Processo %s será Executado\n", novo->nome);
	}
	return listaExec;
}

void trataUT(int sinal)
{
	/*
	printf("***LISTA DE PRONTOS***\n");
	Processo *auxr = listaPronto;
	while(auxr) { printf("%s ", auxr->nome); auxr = auxr->prox; } printf("\n");
	*/
	contUT++;
	
	// Inicio Tratamento do Atualmente em Execução
	if(listaExec != NULL)
	{
		if(listaExec->prox == NULL)
		{
			printf("UT %d-%d: Executou %s\n", contUT-1, contUT, listaExec->nome);
			listaExec->executado++;
			if(listaExec->executado == listaExec->total)
			{
				printf("UT %d: %s acabou\n", contUT, listaExec->nome);
				kill(listaExec->pid, SIGKILL);
				listaExec = NULL;				
			}
		}
		
		else
		{				
			Processo *atual, *anterior, *aux1;			
			atual = listaExec;
			aux1 = atual;
			if((atual->executado)%3 == 0 && atual->executado > 0)  // se ja tiver executado por 3 uts, vai p final da lista
			{
				listaExec = listaExec->prox;   // inicio da lista passa a ser o proximo
				while(aux1->prox != NULL)	// anda com aux ate chegar no final da lista
				{
					anterior = aux1;
					aux1 = aux1->prox;
				}
				aux1->prox = atual;		// coloca o atual no final da lista
				atual->prox = NULL;
			}
			printf("UT %d-%d: Executou %s\n", contUT-1, contUT, atual->nome);
			atual->executado++;
			if(atual->executado == atual->total)	
	// se chegou aqui tem dois cenarios possiveis: o atual acabou de executar antes de fazer as 3 uts e esta no comeco da lista, ou ele acabou de terminar as 3 uts dele e foi colocado no final
			{
				printf("UT %d: %s acabou\n", contUT, atual->nome);
				kill(atual->pid, SIGKILL);
				if(atual->prox == NULL) // caso atual esta no final da lista
				{
					aux1->prox = NULL;
					free(atual);
				}
				else	// caso atual esta no inicio da lista
				{
					listaExec = listaExec->prox;
					free(atual);
				}
			}
			/*
			while(atual->prox != NULL && atual->executado == (atual->prox->executado)+3)
			{
				printf("Dentro\n");
				anterior = atual;
				atual = atual->prox;
			}
			printf("UT %d-%d: Executou %s\n", contUT-1, contUT, atual->nome);
			atual->executado++;
			if(atual->executado == atual->total)
			{
				printf("UT %d: %s acabou\n", contUT, atual->nome);
				kill(atual->pid, SIGKILL);
				anterior->prox = atual->prox;
				free(atual);
			}
			*/	
		}
	}
	// Final Tratamento do Atualmente em Execução
	
	// Inicio Atualização listaPronto e listaExec
	if(listaProc){
	while(listaProc->inicio == contUT)
	{
		Processo *aux;
		listaPronto = inserePronto(listaProc);
		aux = listaProc;
		listaProc = listaProc->prox;
		free(aux);
		if(listaProc == NULL) break;
	}
	}
	if(listaPronto != NULL)
	{
		listaExec = insereExec();
		while(listaPronto != NULL && (listaExec->prioridade == listaPronto->prioridade))
		{
			listaExec = insereExec();
		}
		kill(listaExec->pid, SIGCONT);
	}
	// Final Atualização listaPronto e listaExec
	//printf("logo antes da alarm\n");
	alarm(UT);
}

void execProc()
{
	while(listaProc->inicio == contUT)
	{
		Processo *aux;
		listaPronto = inserePronto(listaProc);
		aux = listaProc;
		listaProc = listaProc->prox;
		free(aux);
	}
	if(listaPronto != NULL)
	{
		listaExec = insereExec();
		while(listaPronto != NULL && (listaExec->prioridade == listaPronto->prioridade))
		{	
			listaExec = insereExec();
		}
		kill(listaExec->pid, SIGCONT);
		
	}
	alarm(UT);
	while(1)
	{
		if(listaProc == NULL && listaPronto == NULL && listaExec == NULL)
		{
			break;
		}
	}
}

int main()
{
	char strProc[SIZE*SIZE];
	char *leitorAux, esp[2] = " ";
	char nome[SIZE];
	int prio, ini, total;
	
	int shmid =shmget(SMH_KEY,sizeof(char),0666|IPC_CREAT);
	char *dados = shmat(shmid,0,0);

	signal(SIGALRM, trataUT);
	
	strcpy(strProc, dados);
	shmdt(dados);
	shmctl(shmid, IPC_RMID, NULL);
	
	printf("%s\n", strProc);
	leitorAux = strtok(strProc, esp);
	while(leitorAux != NULL)
	{
		strcpy(nome, leitorAux);
		
		leitorAux = strtok(NULL, esp);
		prio = atoi(leitorAux);
		
		leitorAux = strtok(NULL, esp);
		ini = atoi(leitorAux);
		
		leitorAux = strtok(NULL, esp);
		total = atoi(leitorAux);
		
		printf("Teste: %s %d %d %d\n", nome, prio, ini, total);
		listaProc = insereProc(nome, prio, ini, total);
		
		leitorAux = strtok(NULL, esp);
	}
	
	execProc();
	
	return 0;
}
