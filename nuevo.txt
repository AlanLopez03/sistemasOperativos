
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

    while (token != NULL)
    {
        strcat(limpio, token);     // Concatena el token actual a la cadena limpia
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

void crearFlujoSalida(int tuberia[][2], char **args, char *nombreArchivo, int indice)
{
    int hijo1, hijo2;
    hijo1 = fork();
    if (hijo1 == 0)
    {
        close(tuberia[indice][STDIN_FILENO]);
        dup2(tuberia[indice][STDOUT_FILENO], STDOUT_FILENO);
        close(tuberia[indice][STDOUT_FILENO]);
        execvp(args[0], args);
    }
    close(tuberia[indice][1]);
    hijo2 = fork();
    if (hijo2 == 0)
    {
        char *name = nombreArchivo;
        QuitarEspacios(name);
        int fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        dup2(tuberia[indice][0], 0);
        close(tuberia[indice][0]);
        dup2(fd, 1);
        close(fd);
        execlp("cat", "cat", NULL);
    }
    close(tuberia[indice][0]);
    wait(NULL);
    wait(NULL);
}
void crearFlujoEntrada(int tuberias[][2], char *archivo, char **argumentos, int i)
{
    int hijo1, hijo2;
    hijo1 = fork();
    if (hijo1 == 0)
    {
        char *name = archivo;

        QuitarEspacios(name);
        int fd = open(name, O_RDONLY); // abrir el archivo

        close(tuberias[i][STDIN_FILENO]);
        dup2(fd, STDIN_FILENO); // leer los datos desde el archivo
        close(fd);

        dup2(tuberias[i][STDOUT_FILENO], STDOUT_FILENO); // escribir en la tuberia
        close(tuberias[i][STDOUT_FILENO]);
        execlp("cat", "cat", name, NULL); // leer el archivo
    }

    close(tuberias[i][STDOUT_FILENO]);
    hijo2 = fork();
    if (hijo2 == 0)
    {

        dup2(tuberias[i][STDIN_FILENO], STDIN_FILENO);
        close(tuberias[i][STDIN_FILENO]);
        execvp(argumentos[i], argumentos);
    }
    close(tuberias[i][STDIN_FILENO]);

    wait(NULL);
    wait(NULL);
}
void crearTuberia(int tuberia[][2],char *comando,char **args,int i,int numComandos)
{
    int hijo = fork();
    if (hijo == 0)
    {
        if (i > 0)                                             // si no es el primer comando
            dup2(tuberia[i - 1][STDIN_FILENO], STDIN_FILENO); // leer el contenido de la tuberia (salida del comando anterior)

        if (i < numComandos - 1) // si no es el ultimo comando
        {
            dup2(tuberia[i][STDOUT_FILENO], STDOUT_FILENO); // escribir en la tuberia
        }
        for (int j = 0; j < numComandos - 1; j++)
        {
            close(tuberia[j][0]);
            close(tuberia[j][1]);
        }
        execvp(args[0], args);
        printf("Error al ejecutar el comando  |\n");
    }
    if (i > 0)
        close(tuberia[i - 1][0]);
    if (i < numComandos - 1)
        close(tuberia[i][1]);
    wait(NULL);
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
        int flagSalida = 0, flagEntrada = 0;
        printf("\n$>");
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        separadores = getSeparadores(linea);
        comandos = separarComandos(linea);       // Obtenemos los comandos separados por |><
        numComandos = contarElementos(comandos); // Contamos cuantos comandos hay
        int tuberias[numComandos - 1][2];        // Matriz para almacenar las tuberías

        // Crear todas las tuberías en el bucle
        for (int i = 0; i < numComandos - 1; i++)
        {
            pipe(tuberias[i]);
        }

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

        for (int i = 0; separadores[p] != '\0'; i++)
        {
            argumentos = separarArgumentos(comandos[i]); // Obtenemos los argumentos de cada comando
            int numArgumentos = contarElementos(argumentos); // Contamos cuantos argumentos hay
            if (strcmp(comandos[i], "exit") == 0)
                exit(0);
            else {
                if (separadores[p] == '>' && i == 0)
                crearFlujoSalida(tuberias, argumentos, comandos[i + 1], 0);
                
            else if (separadores[p] == '>') // caso donde > puede estar en cualquier parte
                crearFlujoSalida(tuberias, argumentos, comandos[i + 1], i);
            else if (separadores[p] == '<' && i == 0)
                crearFlujoEntrada(tuberias, comandos[i + 1], argumentos, 0);
            else if (separadores[p] == '<') // falta que jale      
                crearFlujoEntrada(tuberias, comandos[i + 1], argumentos, i);
            else if (separadores[p] == '|')
                crearTuberia(tuberias,comandos[i],argumentos,i,numComandos);
        }
            if(i%2==0)
                p++;
        }
    }
    return 0;
}
