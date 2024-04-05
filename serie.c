#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
char **separarComandos(char *linea)
{
    char delimitadores[] = "|><";
    char *token;
    char **comandos = (char **)malloc(255 * sizeof(char *));
    int i = 0;
    token = strtok(linea, delimitadores);
    while (token != NULL)
    {
        comandos[i] = token;
        i++;
        token = strtok(NULL, delimitadores);
    }
    return comandos;
}
char **separarArgumentos(char *comando)
{
    char delimitadores[] = " ";
    char *token;
    char **argumentos = (char **)malloc(255 * sizeof(char *));
    int i = 0;
    token = strtok(comando, delimitadores);
    while (token != NULL)
    {
        argumentos[i] = token;
        i++;
        token = strtok(NULL, delimitadores);
    }
    return argumentos;
}
int contarElementos(char **argumentos)
{
    int i = 0;
    while (argumentos[i] != NULL)
        i++;

    return i;
}

void ponerFinCadena(char *linea)
{
    // printf("dd=%s\n", linea);
    linea[strcspn(linea, " ")] = '\0';
}

char *quitarSalto(char *linea)
{
    linea[strcspn(linea, "\n")] = '\0';
    return linea;
}
char *getSeparadores(char *linea)
{
    char *separadores = (char *)malloc(255 * sizeof(char));
    int j = 0;
    for (int i = 0; i < strlen(linea); i++)
    {
        if (linea[i] == '|' || linea[i] == '<' || linea[i] == '>')
        {
            separadores[j] = linea[i];
            j++;
        }
    }

    separadores[j] = '\0';
    return separadores;
}




int main(){

    int tuberias[3][2];
    char *comandos[] = {"ls", "wc","cat"};
    int n = 3;

    for (int i = 0; i < n - 1; i++)
    {
        if (pipe(tuberias[i]) == -1)
        {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0;i<n;i++){
        int pid = fork();
         if(pid == 0){
            if(i > 0){
                dup2(tuberias[i - 1][0], STDIN_FILENO);
            }
            if(i < n - 1){
                dup2(tuberias[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < n - 1; j++)
            {
                close(tuberias[j][0]);
                close(tuberias[j][1]);
            }
            execlp(comandos[i], comandos[i], NULL);
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }else{
            if(i > 0){
                close(tuberias[i - 1][0]);
            }
            if(i < n - 1){
                close(tuberias[i][1]);
            }
            wait(NULL);
        }

    }

}