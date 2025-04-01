#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define HTTP_PORT 8080
#define SSH_PORT 2222
#define DB_PORT 3306

void log_attempt(const char *message) {
    FILE *logfile = fopen("honeypot.log", "a");
    if (logfile == NULL) {
        perror("Error opening log file");
        return;
    }
    fprintf(logfile, "%s\n", message);
    fclose(logfile);
}

void alert_siemplify(const char *event_type, const char *details) {
    printf("ALERT: %s - %s\n", event_type, details);
    log_attempt(details);
}

void *start_fake_http_server(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        pthread_exit(NULL);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(HTTP_PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        pthread_exit(NULL);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        pthread_exit(NULL);
    }
    
    printf("Fake HTTP API listening on port %d...\n", HTTP_PORT);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        read(new_socket, buffer, 1024);
        alert_siemplify("Unauthorized HTTP Access", inet_ntoa(address.sin_addr));
        char *response = "HTTP/1.1 403 Forbidden\r\nContent-Type: application/json\r\n\r\n{\"error\": \"Invalid Request\"}";
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }
}

void *start_fake_ssh_server(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        pthread_exit(NULL);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SSH_PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        pthread_exit(NULL);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        pthread_exit(NULL);
    }
    
    printf("Fake SSH server listening on port %d...\n", SSH_PORT);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        read(new_socket, buffer, 1024);
        alert_siemplify("Unauthorized SSH Login Attempt", inet_ntoa(address.sin_addr));
        char *response = "Fake SSH server: Access denied\n";
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }
}

void *start_fake_db_server(void *arg) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        pthread_exit(NULL);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DB_PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        pthread_exit(NULL);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        pthread_exit(NULL);
    }
    
    printf("Fake Database server listening on port %d...\n", DB_PORT);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        read(new_socket, buffer, 1024);
        alert_siemplify("Unauthorized Database Access", inet_ntoa(address.sin_addr));
        
        if (strstr(buffer, "SELECT") != NULL) {
            char *response = "Fake MySQL server: Query returned 0 results\n";
            send(new_socket, response, strlen(response), 0);
        } else if (strstr(buffer, "INSERT") != NULL) {
            char *response = "Fake MySQL server: Data inserted successfully\n";
            send(new_socket, response, strlen(response), 0);
        } else if (strstr(buffer, "DELETE") != NULL) {
            char *response = "Fake MySQL server: Rows deleted: 1\n";
            send(new_socket, response, strlen(response), 0);
        } else {
            char *response = "Fake MySQL server: Syntax error\n";
            send(new_socket, response, strlen(response), 0);
        }
        
        close(new_socket);
    }
}

int main() {
    pthread_t http_thread, ssh_thread, db_thread;
    pthread_create(&http_thread, NULL, start_fake_http_server, NULL);
    pthread_create(&ssh_thread, NULL, start_fake_ssh_server, NULL);
    pthread_create(&db_thread, NULL, start_fake_db_server, NULL);
    
    pthread_join(http_thread, NULL);
    pthread_join(ssh_thread, NULL);
    pthread_join(db_thread, NULL);
    
    return 0;
}
