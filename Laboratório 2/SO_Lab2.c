/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    int pid;
    int i;
    
    for(i = 0; i <= 3; i++) //Repetirá por 4 vezes para criação de 4 Processos Filhos
    {
        pid = fork();
        if(pid != 0)
        {
            printf("Processo Pai ID %d gerou Filho %d\n", pid, i);
        }
        else
        {
            switch(i)
            {
                case 0:
                {
                    char *args[4] = {"./meucp", "origem.txt", "destino_meucp.txt", NULL}; //Inicializa o vetor com os argumentos necessários para a função execv()
                    execv(args[0], args); //Executará o código meucp.c
                    printf("Erro no meucp\n"); //Só chegará nessa linha caso a execv dê erro
                    break;
                }
                case 1:
                {
                    char *args[4] = {"/bin/cp", "origem.txt", "destino_cp.txt", NULL}; //Inicializa o vetor com os argumentos necessários para a função execv()
                    execv(args[0], args); //Executará a função de sistema cp
                    printf("Erro no cp\n"); //Só chegará nessa linha caso a execv dê erro
                    break;
                }
                case 2:
                {
                    char *args[4] = {"./meuecho", "Hello", "World", NULL}; //Inicializa o vetor com os argumentos necessários para a função execv()
                    execv(args[0], args); //Executará o código meuecho.c
                    printf("Erro no meuecho\n"); //Só chegará nessa linha caso a execv dê erro
                    exit(1);
                    break;
                }
                case 3:
                {
                    char *args[4] = {"/bin/echo", "Hello", "World", NULL}; //Inicializa o vetor com os argumentos necessários para a função execv()
                    execv(args[0], args); //Executará a função de sistema echo
                    printf("Erro no echo\n"); //Só chegará nessa linha caso a execv dê erro
                    break;
                }
                default:
                    printf("Erro no Switch Case\n"); //Só chegará nessa linha caso algo inesperado aconteça
                    break;
                
            }
            exit(1); //Sai do programa com retorno 1
        }
    }
    
    for(i = 0; i <= 3; i++) //Repetirá por 4 vezes para aguardar a finalização de 4 Processos Filhos
    {
        wait(NULL);
        printf("%d - Processo Filho finalizou\n", i);
    }
    printf("Processo Pai vai finalizar\n"); //Encerramento do Processo Pai
    exit(0);

    return 0;
}
