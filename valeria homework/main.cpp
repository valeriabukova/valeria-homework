//
//  main.cpp
//  valeria homework
//
//  Created by Valeria  Bukova on 22.02.2025.
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

#define DEFAULT_BUFLEN 512   // Розмір буфера для прийому даних
#define DEFAULT_PORT "27015" // Порт сервера
#define PAUSE 1000           // Затримка в мілісекундах

int main(int argc, char** argv) {
    // Ініціалізація налаштувань
    setlocale(0, "Ukrainian");
    system("title CLIENT SIDE"); // Встановлення заголовку вікна (тільки для Windows)
    cout << "процес клiєнта запущено!\n";
    usleep(PAUSE * 1000); // Затримка для покращення читабельності

    // 1. Отримання адреси сервера та порту
    const char* ip = "localhost"; // Адреса сервера (локальний хост)
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(hints)); // Обнулення структури
    hints.ai_family = AF_UNSPEC; // Підтримка IPv4 та IPv6
    hints.ai_socktype = SOCK_STREAM; // Потоковий сокет (TCP)
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP
    
    int iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo не вдалося з помилкою: " << iResult << "\n";
        return 12;
    }
    cout << "отримання адреси та порту клiєнта пройшло успiшно!\n";
    usleep(PAUSE * 1000);

    // 2. Створення сокета
    int ConnectSocket = -1;
    for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == -1) {
            cout << "socket не вдалося створити з помилкою: " << errno << "\n";
            return 13;
        }
        
        // 3. Підключення до сервера
        iResult = connect(ConnectSocket, ptr->ai_addr, ptr->ai_addrlen);
        if (iResult == -1) {
            close(ConnectSocket);
            ConnectSocket = -1;
            continue;
        }
        cout << "створення сокета на клiєнтi пройшло успiшно!\n";
        usleep(PAUSE * 1000);
        break;
    }
    freeaddrinfo(result);
    
    if (ConnectSocket == -1) {
        cout << "неможливо пiдключитися до сервера. перевiрте, чи запущено процес сервера!\n";
        return 14;
    }
    cout << "пiдключення до сервера пройшло успiшно!\n";
    usleep(PAUSE * 1000);

    // 4. Відправка даних серверу
    const char* message = "hello from client!";
    iResult = send(ConnectSocket, message, strlen(message), 0);
    if (iResult == -1) {
        cout << "send не вдалося з помилкою: " << errno << "\n";
        close(ConnectSocket);
        return 15;
    }
    cout << "данi успiшно вiдправлено на сервер: " << message << "\n";
    cout << "байтiв вiдправлено з клiєнта: " << iResult << "\n";
    usleep(PAUSE * 1000);

    // 5. Закриття відправки даних
    iResult = shutdown(ConnectSocket, SHUT_WR);
    if (iResult == -1) {
        cout << "shutdown не вдалося з помилкою: " << errno << "\n";
        close(ConnectSocket);
        return 16;
    }
    cout << "процес клiєнта iнiцiює закриття з'єднання з сервером.\n";

    // 6. Отримання відповіді від сервера
    char answer[DEFAULT_BUFLEN];
    do {
        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            answer[iResult] = '\0'; // Додаємо термінуючий нуль для коректного відображення
            cout << "процес сервера надiслав вiдповiдь: " << answer << "\n";
            cout << "байтiв отримано: " << iResult << "\n";
        } else if (iResult == 0) {
            cout << "з'єднання з сервером закрито.\n";
        } else {
            cout << "recv не вдалося з помилкою: " << errno << "\n";
        }
    } while (iResult > 0);
    
    // 7. Закриття сокета
    close(ConnectSocket);
    cout << "процес клiєнта завершує свою роботу!\n";
    return 0;
}
