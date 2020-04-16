#include <iostream>
#include <fstream>
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;//подключаем функции стандартных библиотек

char* OpenURL(const char* url){
    WSADATA lpWSAData;SOCKET s;
    // Проверим на правильность введенный адрес.
    // Он должен начинаться с "http://"
    if (memcmp(url, "HTTPS://", 7) != 0 && memcmp(url, "https://", 7) != 0) return(NULL);
    url += 8;
    // Инициализация библиотеки Ws2_32.dll.
    if (WSAStartup(MAKEWORD(1, 1), &lpWSAData) != 0) return(NULL);
    // Получим имя хоста, номер порта и путь ----------------------------
    char* http_host = _strdup(url); // Имя хоста ( HTTP_HOST)
    int port_num = 80;             // Номер порта по умолчанию ( HTTP_PORT)
    char* http_path = NULL;        // Путь (REQUEST_URI)

    char* pch = strchr(http_host, ':');
    if (!pch) {
        pch = strchr(http_host, '/');
        if (pch) {
            *pch = 0;
            http_path = _strdup(pch + 1);
        }
        else http_path = _strdup("");
    }
    else {
        *pch = 0; pch++;
        char* pch1 = strchr(pch, '/');
        if (pch1) {
            *pch1 = 0;
            http_path = _strdup(pch1 + 1);
        }
        else http_path = _strdup("");
        port_num = atoi(pch);
        if (port_num == 0) port_num = 80;
    }

    // Поучаем IP адрес по имени хоста
    struct hostent* hp;
    if (!(hp = gethostbyname(http_host))) {
        free(http_host); free(http_path);
        return(NULL);
    }

    // Открываем сокет
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        free(http_host); free(http_path);
        return(NULL);
    }
    // Заполняем структуру sockaddr_in
    struct sockaddr_in ssin;
    memset((char*)&ssin, 0, sizeof(ssin));
    ssin.sin_family = AF_INET;
    ssin.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr[0];
    ssin.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr[1];
    ssin.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr[2];
    ssin.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr[3];
    ssin.sin_port = htons(port_num);
    // Выводим IP адрес хоста, с которым будем соединятся
    printf("Conecting to %d.%d.%d.%d...", (unsigned char)hp->h_addr[0], (unsigned char)hp->h_addr[1], (unsigned char)hp->h_addr[2], (unsigned char)hp->h_addr[3]);

    //ssin.sin_addr.s_addr = inet_addr(IP); /* assign the address */

    // Соединяемся с хостом
    if (connect(s, (sockaddr*)&ssin, sizeof(ssin))) {
        free(http_host); free(http_path);
        printf("Error %s\n", GetLastError());
        printf("Not connect\n");        
    }
    else printf("Ok\n");
    // Формируем HTTP запрос
    char query[100]; 
    strcpy_s(query, sizeof query, "GET / HTTP/1.1\r\nHost: www."); strcat_s(query, sizeof query, http_host);
    strcat_s(query, sizeof query, "\r\nConnection: Close\r\n\r\n");
    // Отправляем запрос серверу
    bool cnt = send(s, query, strlen(query), 0);
    // Освобождаем память
    //free(http_host);  free(http_path);  free(query);
    // Проверяем, не произошло ли ошибки при отправке запроса на сервер
    if (cnt == SOCKET_ERROR) std::cout << "\nЗапрос отправлен с проблемами\n";
    cnt = 1;
    char buffer[69999];
    recv(s, buffer, sizeof(buffer), 0);
    int k = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '2' and buffer[i + 1] == '0' and buffer[i + 2] == '0' and buffer[i + 4] == 'O' and buffer[i + 5] == 'K') { std::cout << "\nОтвет успешно получен\n" << buffer; k = 1; }
        if (buffer[i] == '3' and buffer[i + 1] == '0' and buffer[i + 2] == '2') { std::cout << "\nКод перенаправления. Данная ссылка перенаправляет на сайт: " ; k = 2; break; }
        if (buffer[i] == '3' and buffer[i + 1] == '0' and buffer[i + 2] == '1') { std::cout << "\nКод перенаправления. Данная ссылка перенаправляет на сайт: "; k = 2; break; }
    }
    if (k == 0) { std::cout << "\nОтвет получен с проблемами\n" << buffer; }
    if (k == 2) {
        for (int i = 0; i < strlen(buffer); i++) {
            if (buffer[i] == 'L' and buffer[i + 1] == 'o' and buffer[i + 2] == 'c' and buffer[i + 3] == 'a' and buffer[i + 4] == 't' and buffer[i + 5] == 'i' and buffer[i + 6] == 'o' and buffer[i + 7] == 'n') {
                for (int j = i+10; j < strlen(buffer); j++) {
                    std::cout << buffer[j];
                    if (buffer[j] == '/' and(( buffer[j-4] == '.')or (buffer[j-3] == '.'))) { break; }
                }
            }
        }
        std::cout << "\n" << buffer;
    }
    closesocket(s);

    // Деинициализация библиотеки Ws2_32.dll
    WSACleanup();
    //  return(result);
}



void main() {
    setlocale(LC_ALL, "rus"); cout.imbue(locale("rus_rus.1251"));//выводит русские буквы
    char ddomen[50];
    char prishlo[20] = "ebalka";   int kk = strlen(prishlo);
    char tt[13] = "https://"; char tt2[7] = ".info/";
    int k = strlen(tt);  int k2 = strlen(tt2);
    for (int i = 0; i < k; i++) {
        ddomen[i] = tt[i];
        for (int j = 0; j < kk; j++) {
            ddomen[k + j] = prishlo[j];
            for (int g = 0; g < k2; g++) {
                ddomen[k + kk + g] = tt2[g];
            }
        }
    }
    ddomen[k + kk + k2] = 0;
    const char* t = ddomen;
    char* result = OpenURL(t);
    //if (result) {  printf("%s", result); free(result);  }
    //else { printf("Error # WSABASEERR+%d\n", WSAGetLastError() - WSABASEERR); }
    std::cout << "Конец\n";
}

