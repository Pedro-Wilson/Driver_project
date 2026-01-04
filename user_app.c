#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/pico_usb0"

int main() {
    int fd;
    char buffer[256];

    printf("--- Executando Aplicação de Usuário ---\n");
    printf("Enviando dados: Mensagem do Pedro para o Pico\n");

    // Loop de 10 leituras (de 0 a 10)
    for(int i = 0; i <= 10; i++) {
        fd = open(DEVICE, O_RDWR);
        if(fd < 0) {
            printf("\nErro ao abrir o dispositivo! Verifique se o driver está carregado.\n");
            return -1;
        }

        // Escreve para o Driver
        write(fd, "Mensagem do Pedro", 17);

        // Limpa e lê do driver
        memset(buffer, 0, sizeof(buffer));
        read(fd, buffer, sizeof(buffer));

        // Remove o '\n' da resposta para manter o printf na mesma linha adequadamente
        buffer[strcspn(buffer, "\n")] = 0;

        // Imprime o contador atualizado sobrescrevendo a linha (\r)
        printf("\rLendo resposta do Pico... Resposta: %s", buffer);
        fflush(stdout);

        close(fd);

        /* * Ajuste Crucial: Esperamos 1.05 segundos. 
         * Isso dá tempo para o Timer do Kernel (que roda a cada 1s) 
         * incrementar o valor antes da nossa próxima leitura.
         */
        usleep(1050000); 
    }

    printf("\n--- Ciclo de leitura da aplicação finalizado ---\n");
    return 0;
}