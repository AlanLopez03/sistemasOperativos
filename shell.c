
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
    comandos[i] = NULL;
    for(int j=0; j<i; j++)
    {
        printf("comandos[%d]=%s\n",j,comandos[j]);
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
    argumentos[i] = NULL;

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
    int numSeparadores;
    char linea[255];
    char **comandos;
    char **argumentos;
    char *separadores;
    

    while (1)
    {

        printf("\n$>");
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        separadores = getSeparadores(linea);
        comandos = separarComandos(linea);       // Obtenemos los comandos separados por |><
        numComandos = contarElementos(comandos); // Contamos cuantos comandos hay
        numSeparadores = contarElementos(separadores);
        int tuberias[numSeparadores][2];        // Matriz para almacenar las tuberías
        printf("separdores: %d\n", numSeparadores);
        // Crear todas las tuberías en el bucle
        for (int i = 0; i < numSeparadores ; i++)
            pipe(tuberias[i]);
        

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
                QuitarEspacios(argumentos[0]);
                execvp(argumentos[0], argumentos);
            }

            wait(NULL);
        }

        for (int i=0;i<numSeparadores;i++)
        {
            int j=0;
            argumentos = separarArgumentos(comandos[j]);     // Obtenemos los argumentos de cada comando
            int numArgumentos = contarElementos(argumentos); // Contamos cuantos argumentos hay

            if (strcmp(comandos[j], "exit") == 0)
                exit(0);
            
             else if (separadores[i] == '>' && i==0)//caso donde es comando>archivo
             {
                 int hijo1, hijo2;
                 hijo1 = fork();
                 if (hijo1 == 0)
                 {
                     close(tuberias[0][STDIN_FILENO]);
                     dup2(tuberias[0][STDOUT_FILENO], STDOUT_FILENO);
                     close(tuberias[0][STDOUT_FILENO]);
                     execvp(argumentos[0], argumentos);
                 }
                 close(tuberias[0][STDOUT_FILENO]);
                 hijo2 = fork();
                 if (hijo2 == 0)
                 {
                     char *name = comandos[i + 1];
                     QuitarEspacios(name);
                     int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                     dup2(tuberias[0][STDIN_FILENO], STDIN_FILENO);//leer el contenido de la tuberia (salida del comando anterior)
                     close(tuberias[0][STDIN_FILENO]);
                     dup2(fd, STDOUT_FILENO);//escribe en el archivo
                     close(fd);
                     execlp("cat", "cat", NULL);
                 }
                 close(tuberias[0][STDIN_FILENO]);
                 wait(NULL);
                 wait(NULL);

             }
            //else if (separadores[i] == '>')//caso donde > puede estar en cualquier parte
            // {
            //     int hijo1, hijo2;
                
            //     hijo1 = fork();

            //     if (hijo1 == 0)
            //     {

            //         dup2(tuberias[i-1][STDIN_FILENO], STDIN_FILENO);
            //         close(tuberias[i-1][STDIN_FILENO]);

            //         close(tuberias[i][STDIN_FILENO]);
            //         dup2(tuberias[i][STDOUT_FILENO], STDOUT_FILENO);
            //         close(tuberias[i][STDOUT_FILENO]);
            //         execvp(argumentos[i], argumentos);
            //     }
            //     close(tuberias[i][STDOUT_FILENO]);
            //     hijo2 = fork();
            //     if (hijo2 == 0)
            //     {
            //         char *name = comandos[i + 1];
            //         QuitarEspacios(name);

            //         int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
            //         dup2(tuberias[p][STDIN_FILENO], STDIN_FILENO);//leer el contenido de la tuberia (salida del comando anterior)
            //         close(tuberias[p][STDIN_FILENO]);
            //         dup2(fd, STDOUT_FILENO);//escribe en el archivo
            //         close(fd);
            //         execlp("cat", "cat", NULL);

            //         //dup2(tuberias[i][STDOUT_FILENO],STDOUT_FILENO);
            //     }
            //     close(tuberias[p][STDIN_FILENO]);
            //     wait(NULL);
            //     wait(NULL);
            //     flagSalida = 1;
            //     p++;
            // }

            else if (separadores[i] == '<') // falta que jale
            {
                //int tubo[2];
                int hijo1, hijo2;
                //pipe(tubo);
                hijo1 = fork();
                if (hijo1 == 0)
                {
                    char *name = comandos[i + 1];

                    QuitarEspacios(name);
                    int fd = open(name, O_RDONLY);//abrir el archivo

                    close(tuberias[0][STDIN_FILENO]);
                    dup2(fd, STDIN_FILENO);//leer los datos desde el archivo
                    close(fd);

                    dup2(tuberias[0][STDOUT_FILENO], STDOUT_FILENO);//escribir en la tuberia
                    close(tuberias[0][STDOUT_FILENO]);
                    execlp("cat", "cat", name, NULL); // leer el archivo
                }

                close(tuberias[0][STDOUT_FILENO]);
                hijo2 = fork();
                if (hijo2 == 0)
                {
                    dup2(tuberias[0][STDIN_FILENO], STDIN_FILENO);
                    close(tuberias[0][STDIN_FILENO]);
                    execvp(argumentos[0], argumentos);
                    printf("Error al ejecutar el comando en <\n");
                }
                close(tuberias[0][STDIN_FILENO]);
                wait(NULL);
                wait(NULL);

                //flagEntrada = 1;
            }
            else if(separadores[i] == '|')
            {
                printf("Caso de tuberia\n");
                int hijo;
                hijo=fork();
                if(hijo==0)
                {
                    if(j>0)//si no es el primer comando
                        dup2(tuberias[i-1][STDIN_FILENO],STDIN_FILENO);//leer el contenido de la tuberia (salida del comando anterior)
                    
                    if(j<numComandos-1)//si no es el ultimo comando
                    {
                        dup2(tuberias[i][STDOUT_FILENO],STDOUT_FILENO);//escribir en la tuberia
                    }
                for (int j = 0; j < numComandos - 1; j++)
                {
                    close(tuberias[j][0]);
                    close(tuberias[j][1]);
                }   
                    execvp(argumentos[0],argumentos);
                    printf("Error al ejecutar el comando en |\n");
                }
                
                if(j>0)
                    close(tuberias[i-1][0]);
                if(j<numComandos-1)
                    close(tuberias[i][1]);
                wait(NULL);


                j++;



            }
            
        }
    }
return 0;
}


