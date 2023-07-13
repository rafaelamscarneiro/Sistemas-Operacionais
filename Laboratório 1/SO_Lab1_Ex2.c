/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int* cria_vetor(int n);

int main(void)
{
    int n = 10000; //Tamanho do vetor a ser criado 
    int* v = cria_vetor(n); //Função que gera o vetor
    int pid; //Variável que recebe o ID do processo gerado após a execução da fork()
    
    for(int i = 1; i <= 3; i++)
    {
        pid = fork(); //Por causa do for, gerará três filhos
        if(pid != 0) //Execução do Processo Pai
        {
            printf("Processo Pai ID %d gerou o Filho %d\n", pid, i);
        }
        else //Execução do Filho 
        {
            for(int j = 0; j < n; j++) //Somará o índice ao elemento do vetor
            {
                //printf("Processo Filho %d Executando\n", i);
                v[j] += i;
            }
            
            printf("Verificando valores do vetor do Filho %d\n", i);
            for (int j = 0; j < n; j++)//Verifica se os valores são os esperados
            {
                if(v[j] != j+i)
                {
                    printf("Erro na verificação de Filho %d\n", i);
                    exit(1);
                }
            }
            printf("Filho %d verificado com sucesso\n", i);
            printf("Processo Filho %d vai finalizar\n", i);
            exit(0); //Finaliza o Processo Filho
        }
    }
    
    for(int i = 0; i < 3; i++) //Repetirá três vezes por causa do número de filhos
    {
        wait(NULL); //Esperar pelo fim do Processo Filho
        printf("Processo Pai ID %d vai finalizar\n", pid);
    }
    exit(0); //Finaliza o Processo Pai
    
    return 0;
}

//Função que aloca um vetor dinamicamente e o preenche com o valor de seus índices
int* cria_vetor(int n)
{
    int* vetor = (int*) malloc(n*sizeof(int));
    for (int i = 0; i < n; i++)
    {
        vetor[i] = i;
    }
    
    return vetor;
}
