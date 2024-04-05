#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

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
    char linea[255],linea2[255];//Bufer de entrada
    char **comandos;//Arreglo de comandos
    char **argumentos;//Arreglo de argumentos
    int tam;

    while (1){
        printf("\n$>");
        fflush(stdin);
        fgets(linea, 255, stdin);
        strcpy(linea2,linea);
        quitarSalto(linea);
        comandos=separarComandos(linea);//Obtenemos los comandos separados por |><
        numComandos=contarElementos(comandos);//Contamos cuantos comandos hay
        //printf("Numero de comandos: %d\n",numComandos);
        if(numComandos == 2){
            if(linea2[strcspn(linea2, ">")] == '>'){
                //tuberia[0] es de lectura
                //tuberia[1] es de escritura
                int tuberia[2];                 
                pipe(tuberia);
                int hijo=fork();

                //STDIN_FILENO = 0 (Entrada estandar)
                //STDOUT_FILENO = 1 (Salida estandar)
                if(hijo==0){
                    char **argumentos=separarArgumentos(comandos[0]);//Obtenemos los argumentos separados por espacio
                    printf("comandos[0]=%s\n",comandos[0]);
                    printf("argumentos[0]=%s\n",argumentos[0]);
                    close(tuberia[STDIN_FILENO]); //Liberamos el descriptor de archivo correspondiente
                    dup2(tuberia[STDOUT_FILENO],STDOUT_FILENO);
                    close(tuberia[STDOUT_FILENO]);
                    
                    //char **argumentos=separarArgumentos(comandos[0]);//Obtenemos los argumentos separados por espacio
                    execvp(argumentos[0],argumentos);
                    printf("Error al ejecutar el comando\n"); 
                }
                close( tuberia[ STDOUT_FILENO ] );

                int hijo2=fork();
                if(hijo2==0){
                    dup2(tuberia[STDIN_FILENO],STDIN_FILENO);
                    close(tuberia[STDIN_FILENO]);
                    
                    char **argumentos=separarArgumentos(comandos[1]);
                    execvp(argumentos[0],argumentos);
                    printf("Error al ejecutar el comando\n");
                }
                close( tuberia[ STDIN_FILENO ] );

                wait(NULL);//Esperamos a que el hijo termine para que imprima el prompt
                wait(NULL);//Esperamos a que el hijo termine para que imprima el prompt

                exit(1);
                // if(strcmp(linea[strcspn(linea, "<")], "<")==0){
            
            }
        }
        if (strcmp(comandos[0], "exit") == 0){
            printf("Saliendo del programa\n");
            exit(0);
        }  
    }
}
