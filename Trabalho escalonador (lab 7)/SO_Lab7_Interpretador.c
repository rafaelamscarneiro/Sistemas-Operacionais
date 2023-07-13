#include <stdio.h>
#include <string.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SIZE 15
#define SMH_KEY 0x1968

int main(int argc, char* args[])
{
    FILE *arq_entrada;
    char exec[5], nomeProc[3], str_prio[11], str_ini[22], str_tot[21], numPrio[3], tempoIni[3], tempoTot[3]; //Caracteres a serem lidos do arquivo
    
    char fila[SIZE][SIZE];				// Matriz que pegará os processos a serem executados
    char conteudoMemComp[SIZE*SIZE];	// Array que terá o conteudo do arquivo em apenas um string
    int i = 0;
    
    int shmid = shmget(SMH_KEY,sizeof(char),0666|IPC_CREAT);	// Aloca a memória compartilhada
    char *lista = shmat(shmid, 0, 0);								// Associa a memória ao processo

    arq_entrada = fopen(args[1], "r");	// Abre o arquivo passado na linha de comando em modo de leitura
    
    while(fscanf(arq_entrada, "%s %[^,], %[^=]=%[^,], %[^=]=%[^,], %[^=]=%s", exec, nomeProc, str_prio, numPrio, str_ini, tempoIni, str_tot, tempoTot) != EOF) // Le linha a linha do arquivo
    {       
	strcpy(fila[i], nomeProc);	// Copia o nome do Processo para fila[i]
        strcat(fila[i], " ");		// Concatena " " a fila[i] para separar do elemento anterior
        strcat(fila[i], numPrio);	// Concatena a Prioriade do Processo a fila[i]
        strcat(fila[i], " ");		// Concatena " " a fila[i] para separar do elemento anterior
        strcat(fila[i], tempoIni);	// Concatena o Tempo de Inicio do Processo a fila[i]
        strcat(fila[i], " ");		// Concatena " " a fila[i] para separar do elemento anterior
        strcat(fila[i], tempoTot);	// Concatena o Tempo de Execução do Processo a fila[i]
        printf("Exibindo p[%d]: %s\n", i+1, fila[i]);	// Exibe os dados formatados
        if(i == 0)	// Se for o primeiro a ser inserido em conteudoMemComp
		{
			strcpy(conteudoMemComp, fila[i]);	// Copia a string para conteudoMemComp
		}
		else	// Se já tiver caractéres em conteudoMemComp
		{
			strcat(conteudoMemComp, " ");		// Concatena " " a fila[i] para separar do elemento anterior
			strcat(conteudoMemComp, fila[i]);	// Concatena a nova linha ao array conteudoMemComp
		}
        i++;	// Aumenta valor do contador
    }
    strcpy(lista, conteudoMemComp);	// Copia array conteudoMemComp para Memória Compartilhada
    printf("Conteudo Memoria Compartilhada: %s\n", lista);	// Exibe o que está na Memória Compartilhada
    fclose(arq_entrada);	// Fecha arquivo de entrada
    
    shmdt(lista);	// Desacopla a Memória Compartilhada do Processo 
    //shmctl (shmid, IPC_RMID, 0);	// Aqui eu apaguei só por boas práticas, mas tem que ser no SO_Lab7_Escalonador.c
    
    return 0;
}
