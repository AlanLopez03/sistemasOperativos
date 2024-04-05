#include <unistd.h>
#include <stdlib.h>

int main() {
    pid_t pid = fork();
if (pid == 0) {
        // Proceso hijo termina inmediatamente.
        exit(0);
    }
    else {
        // Proceso padre duerme por un tiempo y termina sin esperar al hijo.
        sleep(20);
    } 

    return 0;
}