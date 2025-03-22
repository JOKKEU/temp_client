#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Создание UDP сокета
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка структуры адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Принимаем сообщения на всех интерфейсах
    server_addr.sin_port = htons(PORT); // Порт

    // Привязка сокета к адресу
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Ожидание сообщений на порту %d...\n", PORT);

    // Основной цикл для получения сообщений
    while (1) {
        ssize_t received_len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        
        if (received_len < 0) {
            perror("Ошибка при получении сообщения");
            continue;  // Продолжаем ожидание, если произошла ошибка
        }

        // Добавляем нуль-терминатор для корректного отображения строки
        buffer[received_len] = '\0';

        printf("Получено сообщение от %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    }

    // Закрытие сокета (данная часть кода вряд ли будет достигнута из-за бесконечного цикла)
    close(sock);
    return 0;
}
