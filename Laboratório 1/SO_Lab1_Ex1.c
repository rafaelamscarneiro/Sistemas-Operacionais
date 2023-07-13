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
    pid_t pid, pid2; //Variável que recebe o ID do processo gerado após a execução da fork()
    
    pid = fork();
    if(pid != 0) //Execução do Processo Pai
    {
        printf("Pai foi criado\n");
        for(int i = 0; i < n; i++) //Somar 1 aos elementos do vetor
        {
            //printf("Executando PAI\n");
            v[i]++;
        }
        
        wait(NULL); //Espera o Processo Filho acabar
        printf("Verificando valores do vetor\n");
        for (int i = 0; i < n; i++) //Verifica se os valores são os esperados
        {
            if(v[i] != i+1)
            {
                printf("Erro na verificação de Pai\n");
                exit(1);
            }
        }
        printf("Pai verificado com sucesso\n");
        printf("Processo Pai vai finalizar\n");
    }
    else 
    {
        pid2 = fork(); //Gera o Processo Neto
        if(pid2!=0) //Execução do Processo Filho
        {
            printf("Filho foi criado\n");
            for(int i = 0; i < n; i++) //Soma 2 aos elementos do vetor
            {
                //printf("Executando FILHO\n");
                v[i]+=2;
            }
            
            wait(NULL); //Espera o Processo Neto acabar
            printf("Verificando valores do vetor\n");
            for (int i = 0; i < n; i++) //Verifica se os valores são os esperados
            {
                if(v[i] != i+2)
                {
                    printf("Erro na verificação de Filho");
                    exit(1);
                }
            }
            printf("Filho verificado com sucesso\n");
            printf("Processo Filho vai finalizar\n");
        }
        else //Execução do Processo Neto
        {
            printf("Neto foi criado\n");
            for(int i = 0; i < n; i++)
            {
                //printf("Executando NETO\n");
                v[i]+=3;
            }
            printf("Verificando valores do vetor\n");
            for (int i = 0; i < n; i++) //Verifica se os valores são os esperados
            {
                if(v[i] != i+3)
                {
                    printf("Erro na verificação de Neto");
                    exit(1);
                }
            }
            printf("Neto verificado com sucesso\n");
            printf("Processo Neto vai finalizar\n");
            exit(0); //Finaliza o Processo Neto
        }
        exit(0); //Finaliza o Processo Filho
    }
    exit(0); //Finaliza o Processo Pai
    free(v);
    
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
