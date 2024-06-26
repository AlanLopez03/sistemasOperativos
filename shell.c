#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char **separarComandos(char *linea) // Recibe una cadena y devuelve un arreglo de cadenas con los comandos separados
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
char **separarArgumentos(char *comando) // Recibe una cadena y devuelve un arreglo de cadenas con los argumentos separados(incluyendo el nombre del comando en la primera posicion)
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

int contarElementos(char **argumentos) // Devuelve el numero de elementos de un arreglo de strings
{
    int i = 0;
    while (argumentos[i] != NULL)
    {
        i++;
    }
    return i;
}

char *quitarSalto(char *linea) // Quita el salto de linea cuando lees el comando
{
    linea[strcspn(linea, "\n")] = '\0';
    return linea;
}

char *ObtenerDelimitadores(char *linea)
{
    char d[255];
    char *token;
    int i = 0;
    for (int j = 0; j < strlen(linea); j++)
    {
        if (linea[j] == '|' || linea[j] == '<' || linea[j] == '>')
        {
            d[i] = linea[j];
            i++;
        }
    }
    d[i] = '\0';

    printf("Delimitadores=%s", d);
    return d;
}

int main()
{
    int numComandos;              // Numero de comandos que se ingresaron
    char linea[255], linea2[255]; // Bufer de entrada
    char **comandos;              // Arreglo de comandos
    char **argumentos;            // Arreglo de argumentos
    char *delimitadores;
 
    while (1)
    {
        printf("\n$>");
        fflush(stdin);
        fgets(linea, 255, stdin);
        strcpy(linea2, linea);
        quitarSalto(linea);
        comandos = separarComandos(linea);            // Obtenemos los comandos separados por |><
        numComandos = contarElementos(comandos);      // Contamos cuantos comandos hay
        delimitadores = ObtenerDelimitadores(linea2); // Obtenemos los delimitadores
        
        printf("numComandos=%d\n", numComandos);
        for (int i = 0; i < numComandos; i++)
        {
               int p = 0;
            // tuberia[0] es de lectura
            // tuberia[1] es de escritura
            if (strcmp(comandos[i], "exit") == 0)
            {
                printf("Saliendo del programa\n");
                printf("Linea=%s\n", linea);
                exit(0);
            }

              if (comandos[1] == NULL && strcmp(comandos[0], "exit") != 0)
             {
                 int hijo = fork();
                 if (hijo == 0)
                 {
                     char **argumentos = separarArgumentos(comandos[0]); // Obtenemos los argumentos separados por espacio
                     execvp(argumentos[0], argumentos);
                     printf("Error al ejecutar el comando\n");
                 }
                 wait(NULL);
             }
            else
            {
                int tuberia[2];
                pipe(tuberia);
                //printf("Delimitadores=%c\n", delimitadores[0]);
                getchar();
                printf("%c",delimitadores[p]);
                getchar();
                if (delimitadores[p] == '>'){
                    int hijo = fork();
                    if (hijo == 0){
                        getchar();
                        close(tuberia[0]);
                        dup2(tuberia[1], 1);
                        close(tuberia[1]);
                        char **argumentos = separarArgumentos(comandos[i]); // Obtenemos los argumentos separados por espacio
                        //printf("Hola\n", tuberia[0]);
                        execvp(argumentos[0], argumentos);
                        printf("Error al ejecutar el comando\n");
                    }
                    close(tuberia[1]);
                    int hijo2 = fork();
                    if (hijo2 == 0)
                    {
                        printf("Hola\n", tuberia[0]);
                        int fd = open("arch.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
                        dup2(fd, 1);
                        close(fd);
                        write(STDOUT_FILENO, tuberia[0], strlen(tuberia[0]));
                        // Puede haber violacion de segmento por el comando siguiente
                    }
                    close(tuberia[0]);
                    wait(NULL);
                    wait(NULL);
                    p++;
                }

                //int hijo = fork();
                //if (hijo == 0){
                //    // close(tuberia[0]);
                //    // dup2(tuberia[1],1);
                //    // close(tuberia[1]);
                //    char **argumentos = separarArgumentos(comandos[i]); // Obtenemos los argumentos separados por espacio
                //    execvp(argumentos[0], argumentos);
                //    printf("Error al ejecutar el comando\n");
                //}
                //wait(NULL); // Esperamos a que el hijo termine para que imprima el prompt
            }
        }
    }
}
