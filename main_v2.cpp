#include<iostream>
#include<stdlib.h>
#include<cstdio>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<thread>
#define BUF_SIZE 1024
#define _STR(x) _VAL(x)
#define _VAL(x) #x
void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n',stderr);
	exit(1);
}

void run_client(){
	int client = socket(PF_INET, SOCK_STREAM, 0);
	if(client == -1){
		error_handling(strcat(_STR(__LINE__),"client socket() error"));
		return;
	}
	struct sockaddr_in clnt_adr;
	memset(&clnt_adr, 0, sizeof(clnt_adr));
	
	clnt_adr.sin_family = AF_INET;
	clnt_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	clnt_adr.sin_port = htons(9777);
	
	int ret = connect(client, (struct sockaddr*)&clnt_adr, sizeof(clnt_adr));
	if(ret == -1){
		
		error_handling(strcat(_STR(__LINE__),"client connect() error"));
		close(client);
		return;
	}
	
	while(ret == 0){
		char buffer[BUF_SIZE] = "";
		fputs("Input message(Q to quit)", stdout);
		fgets(buffer, sizeof(buffer), stdin);
		if(strcmp(buffer, "q\n") == 0 || strcmp(buffer, "Q\n") == 0) break;
		
		write(client, buffer, strlen(buffer));
		memset(buffer, 0, sizeof(buffer));
		
		read(client, buffer, sizeof(buffer));
		std::cout << "from server: " << buffer;
	}
	close(client);
	std::cout << "client done!" << std::endl;
}

void run_server(){

	int server = socket(PF_INET, SOCK_STREAM, 0);
	if(server == -1){
		error_handling(strcat(_STR(__LINE__),"socket error"));
		return;
	}
	struct sockaddr_in serv_adr;
	memset(&serv_adr, 0, sizeof(serv_adr));
	
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_adr.sin_port = htons(9777);

	if(bind(server, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){	
		error_handling(strcat(_STR(__LINE__),"bind() error"));
		close(server);
		return;
	}	
	
	if(listen(server, 5) == -1){
		error_handling(strcat(_STR(__LINE__)," listen() error"));
		close(server);
		return;
	}
	
	int client;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_size = sizeof(clnt_adr);
	client = accept(server, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
	if(client == -1) 
	{	
		error_handling(strcat(_STR(__LINE__),"accept() error"));
		close(server);
		return;
	}
	
	char buffer[BUF_SIZE] = "";
	memset(buffer, 0, sizeof(buffer));
	int str_len;
	while((str_len = read(client, buffer, sizeof(buffer))) != 0){
		write(client, buffer, str_len);
		if(str_len != strlen(buffer)){	
			error_handling(strcat(_STR(__LINE__),"client write() error"));
			close(server);
			return ;
		}
		memset(buffer, 0, BUF_SIZE);
	}
	close(client);
	close(server);
	std::cout << "server done!" << std::endl;
}

int main(){

	pid_t t = fork();
	if(t == 0){
		run_server();
	}
	else if(t > 0){
		sleep(1);
		run_client();
		int status = 0;
		wait(&status);
	}else{
		error_handling(strcat(_STR(__LINE__),"fork() error"));
	}
	return 0;
}
