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

void QuitarEspacios(char *cadena)
{
    char *token;
    char limpio[256] = ""; // Aquí almacenaremos la cadena sin espacios

    token = strtok(cadena, " "); // Divide la cadena en tokens usando los espacios como delimitador

    while (token != NULL) {
        strcat(limpio, token); // Concatena el token actual a la cadena limpia
        token = strtok(NULL, " "); // Siguiente token
    }

    strcpy(cadena, limpio); // Copia la cadena limpia de vuelta a la cadena original
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

int main()
{
    int numComandos;
    char linea[255];
    char **comandos;
    char **argumentos;
    char *separadores;
    int p;
    while (1)
    {
        p = 0;
        printf("\n$>");
        // fflush(stdin);
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        separadores = getSeparadores(linea);
        // printf("separadores=%s\n",separadores);
        // printf("linea=%s\n",linea);

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
                //for(int pp=0; numComandos; pp++)
                //    printf("argumentos[%d]=%s\n",pp,argumentos[pp]);
                execvp(argumentos[0], argumentos);
                // printf("Error al ejecutar el comando\n");
            }

            wait(NULL);
        }

        // Arregla lo quitar el espacio al nombre del archivo  jala si tail shell.c >hola.txt

        for (int i = 0; i < numComandos - 1; i++)
        {
            //printf("comandos[%d]=%s", i, comandos[i]);
            argumentos = separarArgumentos(comandos[i]);     // Obtenemos los argumentos de cada comando
            int numArgumentos = contarElementos(argumentos); // Contamos cuantos argumentos hay
            //printf("numArgumentos=%d\n", numArgumentos);

            if (strcmp(comandos[i], "exit") == 0)
                exit(0);
            
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
                    QuitarEspacios(name);

                    int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    dup2(tubo[STDIN_FILENO], STDIN_FILENO);//leer el contenido de la tuberia (salida del comando anterior)
                    close(tubo[STDIN_FILENO]);
                    dup2(fd, STDOUT_FILENO);//escribe en el archivo
                    close(fd);
                    execlp("cat", "cat", NULL);
                }
                close(tubo[STDIN_FILENO]);
                wait(NULL);
                wait(NULL);
                p++;
            }

            else if (separadores[p] == '<') // falta que jale
            {
                int tubo[2];
                int hijo1, hijo2;
                pipe(tubo);
                hijo1 = fork();
                if (hijo1 == 0)
                {
                    char *name = comandos[i + 1];

                    QuitarEspacios(name);
                    int fd = open(name, O_RDONLY);//abrir el archivo

                    close(tubo[STDIN_FILENO]);
                    dup2(fd, STDIN_FILENO);
                    close(fd);

                    dup2(tubo[STDOUT_FILENO], STDOUT_FILENO);
                    close(tubo[STDOUT_FILENO]);
                    execlp("cat", "cat", name, NULL); // leer el archivo
                }

                close(tubo[STDOUT_FILENO]);
                hijo2 = fork();
                if (hijo2 == 0)
                {
                    dup2(tubo[STDIN_FILENO], STDIN_FILENO);
                    close(tubo[STDIN_FILENO]);
                    execvp(argumentos[0], argumentos);
                    printf("Error al ejecutar el comando\n");
                }
                close(tubo[STDIN_FILENO]);
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
                    argumentos = separarArgumentos(comandos[1]);
                    execvp(argumentos[0], argumentos);
                }
                wait(NULL);
                wait(NULL);
            }
        }
    }


return 0;
}