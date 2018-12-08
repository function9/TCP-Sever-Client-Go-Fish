/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"
void func(int sockfd) 
{ 
    char buff[200]; 
    int n, count; 
    while (1) { 
	count = 0;
	while(1){
		//reading from server
		if(count > 10){
			gameOver();
		}
		if(buff[0] == '~') gameOver();
        	bzero(buff, sizeof(buff)); 
		read(sockfd, buff, sizeof(buff)); 
		if(buff[0] == '*') break;
        	printf(buff);
		printf("\n");
		count++;
		
		
	}
	//reading in user input
	bzero(buff, sizeof(buff));  
        printf("\nEnter Rank : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n'){
	}
        write(sockfd, buff, sizeof(buff));
	/*
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        }
	*/ 
    } 
} 
void gameOver(int ip){
	printf("Game Over\n");
	//char buff[200]; 
	//read(ip, buff, sizeof(buff)); 
	//printf(buff);
	exit(0);
}
int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
	fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
	exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
	func(clientfd);
	gameOver(clientfd);	
/*
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
	Rio_writen(clientfd, buf, strlen(buf));
	Rio_readlineb(&rio, buf, MAXLINE);
	read(&rio, buf, sizeof(buf));
	Fputs(buf, stdout);
    }
*/
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
