#include <stdio.h>
#include <stdlib.h>

FILE* abre_arq(char *caminho, char *modo);
void copia(FILE* arq_entrada, FILE* arq_saida);

int main(int argc, char* argv[])
{
	FILE* origem = abre_arq(argv[1], "r");	//Abrindo arquivo no modo de leitura
	FILE* destino = abre_arq(argv[2], "w");	//Abrindo arquivo no modo de escrita
	copia(origem, destino);			//Copia conteúdo do arquivo de origem para o arquivo de destino
	
}

FILE* abre_arq(char *caminho, char *modo) //Abre arquivo para modo de leitura ou escrita
{
  FILE *p_arq = fopen(caminho, modo);
  if (!p_arq)
  {
  	printf("Erro ao abrir o arquivo.");
  	exit(1);
  }
  return p_arq;
}

void copia(FILE* arq_entrada, FILE* arq_saida)
{
	int res = 1;    //Usada para continuar lendo o arquivo enquanto a fread retornar algo
	char c;	//Usada para armazenar os bytes do arquivo de origem	
	while(res) 
	{
		res = fread(&c, 1, 1, arq_entrada); //Lendo byte a byte
		fwrite(&c, 1, 1, arq_saida);	     //Escrevendo cada byte no arquivo de destino
		
	}
}

