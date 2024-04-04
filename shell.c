#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **separarComandos(char *linea)//Recibe una cadena y devuelve un arreglo de cadenas con los comandos separados 
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
    {
        i++;
    }
    return i;
}

char *quitarSalto(char *linea)//Quita el salto de linea cuando lees el comando
{
    linea[strcspn(linea, "\n")] = '\0';
    return linea;
}


int main(){
    int numComandos;//Numero de comandos que se ingresaron
    char linea[255];//Bufer de entrada
    char **comandos;//Arreglo de comandos
    char **argumentos;//Arreglo de argumentos
    while (1){
        printf("\n$>");
        fflush(stdin);
        fgets(linea, 255, stdin);
        quitarSalto(linea);
        comandos=separarComandos(linea);//Obtenemos los comandos separados por |><
        numComandos=contarElementos(comandos);//Contamos cuantos comandos hay
        for(int i=0;i<numComandos ;i++){
            //tuberia[0] es de lectura
            //tuberia[1] es de escritura

                int tuberia[2];                 
                pipe(tuberia);
                int hijo=fork();

                 if(hijo==0){
                    //close(tuberia[0]);
                    //dup2(tuberia[1],1);
                    //close(tuberia[1]);
                    char **argumentos=separarArgumentos(comandos[i]);//Obtenemos los argumentos separados por espacio
                    execvp(argumentos[0],argumentos);
                    printf("Error al ejecutar el comando\n"); 
               }
               else{
                    wait(NULL);//Esperamos a que el hijo termine para que imprima el prompt
                }
            if (strcmp(comandos[i], "exit") == 0){
                printf("Saliendo del programa\n");
                exit(0);
            }  

        }
    }
}
