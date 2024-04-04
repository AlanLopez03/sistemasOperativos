#include <stdio.h>
#include <unistd.h>

int main() {
    // El nombre del programa a ejecutar
    char *programa = "ls";
    
    // Lista de argumentos para el programa
    // El primer argumento debe ser el nombre del programa
    // La lista debe terminar con NULL
    char *argumentos[] = { "-l", "/home", NULL};
    
    // Ejecuta el comando "ls -l /home"
    execvp(programa, argumentos);
    
    // Si execvp retorna, hubo un error
    perror("execvp");
    return 1;
}