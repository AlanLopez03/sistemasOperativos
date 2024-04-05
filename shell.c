#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **separarComandos(char *linea)//Recibe una cadena y devuelve un arreglo de cadenas con los comandos separados 
{
    char delimitadores[] = "|><";
    char *token;
    char **comandos = (char **)malloc(255 * sizeof(char *));
    int i = 0;
    token = strtok(linea, delimitadores);//strtok busca el primer delimitador y lo reemplaza por un null y empieza con linea
    while (token != NULL)
    {
        comandos[i] = token;
        i++;
        token = strtok(NULL, delimitadores);//NULL para que siga buscando en la misma cadena
    }
    return comandos;
}

char **separarArgumentos(char *comando)//Recibe una cadena y devuelve un arreglo de cadenas con los argumentos separados(incluyendo el nombre del comando en la primera posicion)
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

int contarElementos(char **argumentos)//Devuelve el numero de elementos de un arreglo de strings
{
    int i = 0;
    while (argumentos[i] != NULL)
        i++;
    
    return i;
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
        strcpy(linea2,linea);
        quitarSalto(linea);
        comandos=separarComandos(linea);//Obtenemos los comandos separados por |><
        numComandos=contarElementos(comandos);//Contamos cuantos comandos hay
        for(int i=0;i<numComandos ;i++)
        {
            printf("Comando %d: %s\n", i+1, comandos[i]);
            //Tal vez ya no sea necesario separar los argumentos de cada comando
            argumentos=separarArgumentos(comandos[i]);//Obtenemos los argumentos de cada comando
            int numArgumentos=contarElementos(argumentos);//Contamos cuantos argumentos hay
            printf("Numero de argumentos: %d\n", numArgumentos);
            for(int j=1;j<numArgumentos;j++)
            {
                printf("Argumento %d: %s\n", j+1, argumentos[j]);
            }

            if (strcmp(comandos[i], "exit") == 0){
                printf("Saliendo del programa\n");
                exit(0);
            }  
        }

    }

    return 0;
}