#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>
#define BUF_SIZE 0x100
#include <string.h>

void WriteLine (int sock);

int main( int argc, char *argv[] ) 
{
   int sockfd;
   int newsockfd;
   int port;
   int clilen;
   int one;
   char buffer[256];
   struct sockaddr_in serv_addr; 
   struct sockaddr_in cli_addr;
   int n, pid;
   
   // создаем сокет
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd == -1) 
   {
      printf("listener create error \n");
      exit(1);
   }
   
   // инициализируем структуру сокета
   bzero((char *) &serv_addr, sizeof(serv_addr));
   port = 50000;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(port);
   
   // вызываем bind для создания сокета
   one = bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

   if (one == -1) 
   {
      printf("bind() error \n");
      exit(1);
   }
   
   // слушаем клиентов в пассивном режиме и ждем подключения 
   one = listen(sockfd,5);
   if (one ==  -1) 
   {
      printf("listen() error \n");
      exit(1);
   }

   clilen = sizeof(cli_addr);
   
   while (1) 
   {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
      if (newsockfd < 0) 
      {
         perror("accept() error \n");
         exit(1);
      }
      
      // порождаем потомка
      pid = fork();
		
      if (pid == -1) 
      {
         printf("fork() error \n");
         exit(1);
      }
      
      if (pid == 0) 
      {
         // процесс клиента
         close(sockfd);
         WriteLine(newsockfd);
         exit(1);
      }
      else 
      {
         close(newsockfd);
      }
   }
}

void WriteLine (int sock) 
{
   int n;
   char buffer[1024];
   bzero(buffer,1024);
   n = read(sock,buffer,1023);
   
   if (n < 0) 
   {
      printf("ERROR reading from socket \n");
      exit(1);
   }

   write(sock, "HTTP/1.1 200 OK", 15);
   write(sock, "\n", 1);
   write(sock, "Content-Type: text/html;charset=utf-8", 38);
   write(sock, "\n", 1);
   write(sock, "Connection: Closed", 18);
   write(sock, "\n\n", 2);

   char buf[BUF_SIZE];
   int len, i;
   FILE * f;

   f = fopen("/home/bondaryuk/1.html", "r");///путь к файлу указывать здесь
   while((len = fread(buf, 1, BUF_SIZE-1, f)) > 0)
   {
	for (i = 0; i < len; i++) if (buf[i]==0) buf[i] = 10;
        buf[len] = 0;
        n = write(sock, buf, len);
   }

   fclose(f);

   //printf("hello: %s\n",buffer);
   //n = write(sock,"Hello world", 11);

   if (n < 0) 
   {
      printf("ERROR writing to socket \n");
      exit(1);

   }
	
}
