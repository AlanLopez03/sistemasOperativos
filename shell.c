#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


int main()
{
    int numComandos;
    char linea[255];
    char **comandos;
    char **argumentos;
    char *separadores;
    while (1)
    {
        int p = 0;
        printf("\n$>");
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        separadores = getSeparadores(linea);
        comandos = separarComandos(linea);       // Obtenemos los comandos separados por |><
        numComandos = contarElementos(comandos); // Contamos cuantos comandos hay
        if (numComandos == 1)
        {
            if (strcmp(comandos[0], "exit") == 0)
            {
                printf("Saliendo del programa\n");
                exit(0);
            }

            int hijo = fork();
            if (hijo == 0)
            {
                argumentos = separarArgumentos(comandos[0]); // Obtenemos los argumentos de cada comando
                execvp(argumentos[0], argumentos);
            }

            wait(NULL);
        }

        for (int i = 0; i < numComandos - 1; i++)
        {
            argumentos = separarArgumentos(comandos[i]);     // Obtenemos los argumentos de cada comando
            int numArgumentos = contarElementos(argumentos); // Contamos cuantos argumentos hay
            if (strcmp(comandos[i], "exit") == 0)
            {
                exit(0);
            }
            else if (separadores[p] == '>')
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
                    char *name = comandos[i + 1];
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
                p++;
            }

            else if (separadores[p] == "<") // falta que jale
            {
                int tubo[2];
                int hijo1, hijo2;
                pipe(tubo);
                hijo1 = fork();
                if (hijo1 == 0)
                {
                    char *name = comandos[1];
                    ponerFinCadena(name);
                    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);

                    dup2(STDIN_FILENO, fd);
                    close(fd);

                    dup2(tubo[STDOUT_FILENO], STDOUT_FILENO);
                    close(tubo[STDOUT_FILENO]);
                    execlp("cat", "cat", NULL); // leer el archivo
                }

                close(tubo[STDIN_FILENO]);
                hijo2 = fork();
                if (hijo2 == 0)
                {
                    char *texto=STDOUT_FILENO;

                    // comandos[i] trae el nombre del comando
                    dup2(tubo[STDIN_FILENO], STDIN_FILENO);
                    close(tubo[STDIN_FILENO]);
                    execvp(argumentos[0], argumentos);
                    printf("Error al ejecutar el comando\n");
                }
                wait(NULL);
                wait(NULL);
                p++;
            }
            else if(separadores[p] == '|')
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
                    dup2(tubo[STDIN_FILENO], STDIN_FILENO);
                    close(tubo[STDIN_FILENO]);
                    argumentos = separarArgumentos(comandos[i+1]);
                    execvp(argumentos[0], argumentos);
                }
                wait(NULL);
                wait(NULL);

            }
        }
    }
return 0;
}


