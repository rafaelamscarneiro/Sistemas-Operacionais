/******************************************************************************

Eduardo Fontoura (1710220)
Rafaela Carneiro (2011483)

*******************************************************************************/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

void exibeArq(char *nomeArq, int tamBuffer) 
{
	int arq_entrada;
	char buffer[tamBuffer];
	
	arq_entrada = open(nomeArq, O_RDONLY, 0777);		
	read(arq_entrada, buffer, tamBuffer*sizeof(char));	// Lendo conteúdo do arquivo para o buffer para imprimir no terminal
	printf("%s\n", buffer);	
	close(arq_entrada);
}

void criaDir(char *nomeDir)
{
	int status = mkdir(nomeDir, 0777);	// Cria diretório
	
	if (status == 0)
	{
		printf("Diretorio %s criado com sucesso.\n", nomeDir);
	}
	else
	{
		printf("Erro na criacao do diretorio %s.\n", nomeDir);
		exit(1);
	}
}

void criaArq(char *nomeArq, char *conteudo, int tamConteudo)
{
	int arq_entrada;
	arq_entrada = open(nomeArq, O_CREAT | O_WRONLY, 0777);	// Abre arquivo para escrita
	
	write(arq_entrada, conteudo, tamConteudo*sizeof(char));	// Escreve conteúdo no arquivo
	
	close(arq_entrada);
}

void exibePropriedades(char *arq_nome, struct stat propriedades)
{
	printf("Nome do Arquivo: %s\n", arq_nome);
	printf("Permissoes: %o\n", propriedades.st_mode);
    printf("Tamanho: %d bytes\n", (int)propriedades.st_size); 
    printf("Acessado em: %s", ctime(&propriedades.st_atime));
    printf("Modificado em: %s", ctime(&propriedades.st_mtime));
}

void buscaArq(char *nomeArq, int tamBuffer)
{
	DIR * dir;
	struct dirent *dirPointer;	

	char dirArq = nomeArq[3];	
	dir = opendir("so");

	if (dir != NULL)		
	{
		while ((dirPointer = readdir(dir)) != NULL)	// Enquanto tiver diretório para buscar
		{
			if ((dirPointer->d_name)[0] == dirArq) // Encontrou diretório com nome passado para a funçãp
			{
				printf("Lendo o arquivo %s:\n",nomeArq);
				exibeArq(nomeArq, tamBuffer);		// Exibe conteúdo do arquivo 
			}
		}
	}
	else	
	{
		printf("Erro na leitura do diretorio 'so'.\n");
	}

	closedir(dir);
}

void modArq(char *nomeArq, char *conteudo, int tamConteudo, int deslocamento)
{
	int arq_entrada;
	arq_entrada = open(nomeArq, O_RDWR, 0777);	// Abre o arquive para leitura e escrita
	
	lseek(arq_entrada, deslocamento, SEEK_SET);	// Reposiciona o offset do do arquivo para modificar o conteúdo a partir de um deslocamento
	write(arq_entrada, conteudo, tamConteudo*sizeof(char));	// Escreve o novo conteúdo
	
	close(arq_entrada);
}

void exibeDir(struct dirent *dirPointer, DIR *dir)
{
	if(dirPointer == NULL)		// Caso base
	{
		return;
	}
	if(dirPointer->d_type == DT_DIR)	// Se for do tipo diretório, imprime o nome
	{
		printf("Sub-diretorio: %s\n", dirPointer->d_name);
	}
	dirPointer = readdir(dir);		
	exibeDir(dirPointer, dir);	// Entra recursivamente em todos os diretórios
}

int main(void)
{
	char * parentDir = "so";
	char * dirA = "so/a";
	char * dirB = "so/b";
	char * dirC = "so/c";
	char * arqA = "so/a/arqa.txt";
	char * textoA = "Hello There!";
	char * arqB = "so/b/arqb.txt";
	char * textoB = "General Kenobi!";
	char * arqC = "so/c/arqc.txt";
	char * textoC = "P2 - SO";
	struct stat propriedades;
	DIR *dir;
	struct dirent *dirPointer;
	
	//Letra (a) - Cria diretórios A, B e C e escreve os textos definidos em um arquivo texto dentro de cada diretório
	printf("-----------\nLetra (a)\n-----------\n");
	criaDir(parentDir);
	criaDir(dirA);
	criaDir(dirB);
	criaDir(dirC);
	criaArq(arqA, textoA, 12);
	criaArq(arqB, textoB, 15);
	criaArq(arqC, textoC, 7);

	//Letra (b) - Exibe as propriedades do arquivo (nome, permissões, tamanho, último acesso e última modificação)
	printf("\n-----------\nLetra (b)\n-----------\n");
	if (stat(arqA, &propriedades) == 0)
	{
		exibePropriedades(arqA, propriedades);
	}
	else
	{
		printf("Nao foi possivel exibir os dados do arquivo.\n");
	}
	
	//Letra (c) - Busca arquivo a partir do diretório e imprime o conteúdo dele
	printf("\n-----------\nLetra (c)\n-----------\n");
	buscaArq(arqA, 12);
	
	//Letra (d) - Modifica o conteúdo do arquivo
	printf("\n-----------\nLetra (d)\n-----------\n");
	modArq(arqC, textoA, 12, 5);
	exibeArq(arqC, 19);

	//Letra (e) - Modifica a permissão (para apenas usuário/proprietário podem ler, escrever e executar)
	printf("\n-----------\nLetra (e)\n-----------\n");
	chmod(arqA, 0700);
	if (stat(arqA, &propriedades) == 0)
	{
		exibePropriedades(arqA, propriedades);
	}
	else
	{
		printf("Nao foi possivel exibir os dados do arquivo.\n");
	}
	
	//Letra (f) - Exibe nome todos os diretórios e subdiretórios a partir do diretório corrente
	printf("\n-----------\nLetra (f)\n-----------\n");
	dir = opendir(get_current_dir_name());
	dirPointer = readdir(dir);
	exibeDir(dirPointer, dir);
	closedir(dir);
	
	return 0;
}
