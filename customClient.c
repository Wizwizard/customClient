#include "unp.h"
#include <pthread.h>

void doit(int);
void send2server(int);
void ItoB(int, char*);
int BtoI(char*);

const char *delim = " ";
int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	char* ip = "120.27.112.252";
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, ip, &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	
	doit(sockfd);

	exit(0);
}

void doit(int sockfd)
{
	/*
	pthread_t id_send, id_recv;
	int i, ret;
	ret = pthread_create(&id, NULL, (void *)send2server , sockfd);
	if (ret != 0)
	{
		unix_error("create pthread");
	}
	*/
	send2server(sockfd);
			
}
void send2server(int sockfd)
{
	char getline[MAXLINE], sendline[MAXLINE], recvline[MAXLINE];
	char *p;
	char byteArry[4];
	int i;
	int sendlength, recvlength;
	int result;	

	while ( Fgets(getline, MAXLINE, stdin) != NULL)
	{
		memset(sendline, 0, MAXLINE);
		memset(recvline, 0, MAXLINE);

		p = strtok(getline, delim);
	

	for(i = 0; ;)
	{
		printf("%s ", p);
		
		if(isOperator(p))
		{
			memcpy(&sendline[i], p, 1);
			i++;
		} else
		{
			ItoB(atoi(p), byteArry);
			memcpy(&sendline[i], &byteArry, 4);
			i += 4;	
		}
		if(!(p = strtok(NULL, delim)))
			break;
	}
	
	sendlength = i;
	for(i = 0; i < sendlength; i ++)
		printf("%d ", sendline[i]);
	printf("\n");
	Writen(sockfd, sendline, sendlength);	
	
	recvlength = read(sockfd, recvline, MAXLINE);
	if(recvlength <= 0)
	{
		printf("server is close!\n");
		exit(0);
	}
	printf("recvlength = %d\n", recvlength);
	printf("result = %d\n", BtoI(recvline));	
	}
}

int isOperator(char *str)
{
	if (strncasecmp(str,"+", 1) == 0)
		return 1;
	else if (strncasecmp(str, "-", 1) == 0)
		return 1;
	else if (strncasecmp(str, "*", 1) == 0)
		return 1;		
	else if (strncasecmp(str, "/", 1) == 0)
		return 1;
	
	return 0;
}

void ItoB(int i, char *arry)
{
	arry[0] = (char) (i & 0xff);
	arry[1] = (char) ((i & 0xff00) >> 8);
	arry[2] = (char) ((i & 0xff0000) >> 16);
	arry[3] = (char) ((i >> 24) & 0xff);
}

int BtoI(char *arry)
{
	return (arry[0] | arry[1]<<8 | arry[2]<<16 |arry[3]<<24);
}
