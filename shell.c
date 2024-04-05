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
    printf("dd=%s\n", linea);
    linea[strcspn(linea, " ")] = '\0';
}

char *quitarSalto(char *linea)
{
    linea[strcspn(linea, "\n")] = '\0';
    return linea;
}
int main()
{
    int numComandos;
    char linea[255];
    char **comandos;
    char **argumentos;
    while (1)
    {
        printf("$>");
        fflush(stdin);
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        comandos = separarComandos(linea);       // Obtenemos los comandos separados por |><
        numComandos = contarElementos(comandos); // Contamos cuantos comandos hay
        for (int i = 0; i < numComandos-1; i++)
        {
            argumentos = separarArgumentos(comandos[i]);     // Obtenemos los argumentos de cada comando
            int numArgumentos = contarElementos(argumentos); // Contamos cuantos argumentos hay

            if (strcmp(comandos[i], "exit") == 0)
            {
                exit(0);
            }
            else
            {
                int tubo[2];
                int hijo1, hijo2;
                pipe(tubo);
                hijo1 = fork();

                if (hijo1 == 0)
                {
                    close(tubo[STDIN_FILENO]);
                    dup2(tubo[STDOUT_FILENO], STDOUT_FILENO);
                    close(tubo[STDOUT_FILENO]);
                    execvp(argumentos[0], argumentos);
                }
                close(tubo[STDOUT_FILENO]);
                 hijo2 = fork();
                if (hijo2 == 0)
                {
                    char *name=comandos[1];
                    ponerFinCadena(name);
                    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    dup2(tubo[STDIN_FILENO], STDIN_FILENO);
                    close(tubo[STDIN_FILENO]);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    execlp("cat", "cat", NULL);

                }
                wait(NULL);
                wait(NULL);
            }
        }
    }
    return 0;
}