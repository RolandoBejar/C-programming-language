#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>

int main() {
    printf("=== Process Monitor ===\n");
    printf("Parent PID: %d\n\n", getpid());

    // Comandos a ejecutar (puedes cambiar o añadir)
    char *commands[][3] = {
        {"ls", "-l", NULL},
        {"date", NULL, NULL},
        {"whoami", NULL, NULL},
    };

    int num_commands = 3;
    pid_t child_pids[num_commands];
    struct timeval start, end;

    // Tiempo inicial total
    gettimeofday(&start, NULL);

    printf("Creating child processes...\n");
    for (int i = 0; i < num_commands; i++) {
        pid_t rc = fork();

        if (rc < 0) {
            // Error en fork
            fprintf(stderr, "Fork failed for command %d\n", i);
            exit(1);
        } else if (rc == 0) {
            // Proceso hijo
            printf("Child %d (PID: %d) executing: %s\n",
                   i, getpid(), commands[i][0]);

            execvp(commands[i][0], commands[i]);

            // Si llega aquí, exec falló
            fprintf(stderr, "Exec failed for command: %s\n", commands[i][0]);
            exit(1);
        } else {
            // Proceso padre
            child_pids[i] = rc;
            printf("Parent created child %d with PID: %d\n", i, rc);
        }
    }

    printf("\nChild processes executing...\n\n");

    // Esperar a todos los hijos
    for (int i = 0; i < num_commands; i++) {
        int status;
        pid_t wc = wait(&status);

        if (wc == -1) {
            fprintf(stderr, "Wait failed\n");
            continue;
        }

        // Identificar cuál hijo terminó
        int child_index = -1;
        for (int j = 0; j < num_commands; j++) {
            if (child_pids[j] == wc) {
                child_index = j;
                break;
            }
        }

        // Mostrar resultados
        if (WIFEXITED(status)) {
            printf("Child %d (PID: %d) completed with exit code: %d\n",
                   child_index, wc, WEXITSTATUS(status));
        } else {
            printf("Child %d (PID: %d) terminated abnormally\n",
                   child_index, wc);
        }
    }

    // Tiempo final total
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\nTotal execution time: %.3f seconds\n", elapsed);

    return 0;
}