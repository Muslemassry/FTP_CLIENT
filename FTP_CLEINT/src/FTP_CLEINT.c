/*
 ============================================================================
 Name        : FTP_CLEINT.c
 Author      : Amaragy
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT_NUMBER 65496
#define SERVER_NUMBER "127.0.0.1"

void print_fatal_msg(char *msg);
void show(char* filename);

int main(void) {
	int server_socket;
	struct sockaddr_in server_address;
	char *msg;
	char buffer[1024];
	struct stat file_state;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		print_fatal_msg("server tcp socket creation has failed");
	}

	server_address.sin_addr.s_addr = 0;
	server_address.sin_port = htons(PORT_NUMBER);
	server_address.sin_family = AF_INET;
	memset(&(server_address.sin_zero), '\0', 8);

	if (connect(server_socket,(struct sockaddr*)&server_address, sizeof(server_address))) {
		print_fatal_msg("Could not open connection to server");
	}

	int recv_length = recv(server_socket, &buffer, 1024, 0);
//    printf("recv_length RECEIVED: %d\n", recv_length);
	printf("%s\n", buffer);
	int file_handler;
	char command[100], file_name[20];
	unsigned int file_size;
	int entered_choice;
	while(1) {
		memset(&command, '\0', 100);
		memset(&file_name, '\0', 20);
		printf("Enter a choice:1- get --- 2- put --- 3- pwd --- 4- ls --- 5- cd UP --- 6- cd FILE--- 7- quit\n");
		scanf("%d", &entered_choice);
		switch (entered_choice) {
			case 1:
			{
				printf("Enter the file name to get\n");
				scanf("%s", &file_name);
				strcpy(command, "get ");
				strcat(command, file_name);
				printf("To %s\n", command);
				send(server_socket, command, sizeof(command), 0);
				recv(server_socket, &file_size, sizeof(int), 0);
				if (file_size == -1) {
					printf("No file with the entered name\n");
					break;
				}

				char *file_bytes = (char*)malloc(file_size);
				recv(server_socket, file_bytes, file_size, 0);
				printf("%s", file_bytes);
			}
			break;
			case 2:
			{
				printf("Enter the file name to put\n");
				scanf("%s", &file_name);
				strcpy(command, "put ");
				strcat(command, file_name);
				printf("To %s\n", command);
				file_handler = open(file_name, O_RDONLY);
				if (file_handler == -1) {
					printf("Could not find the file: %s\n", file_name);
				} else {
					stat(file_name,&file_state);
					file_size = file_state.st_size;
					printf("the file size to send is %d\n", file_size);
					write(server_socket, command, strlen(command));
					send(server_socket, &file_size, sizeof(file_size), 0);
					ssize_t sent_file = sendfile(server_socket,file_handler,NULL,file_size);
					printf("the SENT file size is %d\n", sent_file);
					close(file_handler);
					printf("the file %s was put successfully\n", file_name);
				}
			}
			break;
			case 3:
			{
				strcpy(command, "pwd");
				printf("To %s\n", command);
				write(server_socket, command, strlen(command));
				recv(server_socket, &file_size, sizeof(int), 0);
				if (file_size == 0) {
					printf("No information sent by the server for the command pwd\n");
					break;
				}


				char *file_bytes = (char*)malloc(file_size);
				recv(server_socket, file_bytes, file_size, 0);
				printf("%s", file_bytes);
			}
			break;
			case 4:
			{
				strcpy(command, "ls");
				printf("To %s\n", command);
				write(server_socket, command, strlen(command));
				recv(server_socket, &file_size, sizeof(int), 0);
				if (file_size == 0) {
					printf("No information sent by the server for the command ls\n");
					break;
				}

				char *file_bytes = (char*)malloc(file_size);
				recv(server_socket, file_bytes, file_size, 0);
				printf("%s", file_bytes);
			}
			break;
			/*case 5:
			{
				strcpy(command, "cd ..");
				printf("To %s\n", command);
				send(server_socket, command, sizeof(command), 0);
				recv(server_socket, &file_size, sizeof(int), 0);
				char *new_pwd = (char*)malloc(file_size);
				recv(server_socket, new_pwd, file_size, 0);
				printf("Changed directory to %s", new_pwd);
			}
			break;
			case 6:
			{
				printf("Enter the directory to cd in \n");
				scanf("%s", &file_name);
				strcpy(command, "cd ");
				strcat(command, file_name);
				printf("To %s\n", command);
				send(server_socket, command, sizeof(command), 0);
				recv(server_socket, &file_size, sizeof(int), 0);
				if(file_size == -1) {
					printf("Not a directory");
				} else {
					char *new_pwd = (char*)malloc(file_size);
					recv(server_socket, &new_pwd, file_size, 0);
					printf("Changed directory to %s", new_pwd);
				}
			}
			break;*/
			case 7:
			{
				printf("Disconnecting from server\n");
				strcpy(command, "bye");
				printf("To %s\n", command);
				send(server_socket, command, sizeof(command), 0);
				close(server_socket);
				exit(0);
			}
		}
	}


	return 0;
}

void print_fatal_msg(char *msg) {
    printf("FATAL ERROR: %s\n", msg);
    exit(1);
}

void show(char* filename) {
    FILE *fp;
    char str[1000];
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Could not open file %s",filename);
        return;
    }
    while (fgets(str, 1000, fp) != NULL)
        printf("%s", str);
    fclose(fp);
}
