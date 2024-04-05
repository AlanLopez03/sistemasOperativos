#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int n = 3;  // Número de comandos a ejecutar
    char *comandos[] = {"ls", "grep 'txt'", "wc -l"};
    int tuberias[n - 1][2];  // Matriz para almacenar las tuberías
    int i, pid;

    // Crear todas las tuberías en el bucle
    for (i = 0; i < n - 1; i++) {
        if (pipe(tuberias[i]) == -1) {
            perror("Error al crear la tubería");
            exit(EXIT_FAILURE);
        }
    }

    // Crear los procesos hijos y conectar las tuberías en serie
    for (i = 0; i < n; i++) {
        pid = fork();

        if (pid == -1) {
            perror("Error al crear el proceso hijo");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Código del proceso hijo

            // Si no es el primer comando, conectar la tubería de entrada a STDIN
            if (i > 0) {
                dup2(tuberias[i - 1][0], STDIN_FILENO);
            }

            // Si no es el último comando, conectar la tubería de salida a STDOUT
            if (i < n - 1) {
                dup2(tuberias[i][1], STDOUT_FILENO);
            }

            // Cerrar todos los descriptores de archivo de las tuberías
            for (int j = 0; j < n - 1; j++) {
                close(tuberias[j][0]);
                close(tuberias[j][1]);
            }

            // Ejecutar el comando correspondiente
            execlp(comandos[i], comandos[i], NULL);
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        } else {
            // Código del proceso padre
            // Cerrar la tubería de entrada del proceso hijo actual
            if (i > 0) {
                close(tuberias[i - 1][0]);
            }

            // Cerrar la tubería de salida del proceso hijo actual
            if (i < n - 1) {
                close(tuberias[i][1]);
            }

            // Esperar a que el proceso hijo termine
            wait(NULL);
        }
    }

    return 0;
}