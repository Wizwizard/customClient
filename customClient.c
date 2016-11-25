#include "unp.h"
#include <pthread.h>

void init();
void doit(int);
void send2server(int);
void recvFserver(int);
void printPcontent(char *, int);
int isOperator(char *);
void ItoB(int, char*);
int BtoI(char*);

const char *delim = " ";
const int heartlength = 6;
char heartPackage[6];
const int messageWrapperLength = 6;

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
	
	init();

	doit(sockfd);

	exit(0);
}

void init()
{
	char lengthByte[4];
	int length = 1;

	memset(heartPackage, 0, heartlength);
	heartPackage[0] = 'p';
	ItoB(length, lengthByte);
	memcpy(&heartPackage[1], lengthByte, 4);
	heartPackage[5] = 'h';		

}

void doit(int sockfd)
{
	pthread_t id_send, id_recv;
	int ret;

	ret = pthread_create(&id_send, NULL, (void *)send2server , (void *)sockfd);
	if (ret != 0)
		unix_error("create pthread");
	ret = pthread_create(&id_recv, NULL, (void *)recvFserver, (void *)sockfd);
	if (ret != 0)
		unix_error("create pthread");
	
	pthread_join(id_send, NULL);
			
}
void send2server(int sockfd)
{
	char getline[MAXLINE], sendline[MAXLINE];
	char *p;
	char byteArry[4];
	int i;
	int sendlength;
	int result;	

	while ( Fgets(getline, MAXLINE, stdin) != NULL)
	{
		memset(sendline, 0, MAXLINE);

		p = strtok(getline, delim);
		sendline[0] = 'p';
		sendline[5] = 'm';
		for (sendlength = messageWrapperLength; ;)
		{
			printf("%s ", p);
			if (isOperator(p))
			{
				memcpy(&sendline[sendlength], p, 1);
				sendlength++;
			} else
			{
				ItoB(atoi(p), byteArry);
				memcpy(&sendline[sendlength], &byteArry, 4);
				sendlength += 4;
			}
			if (!(p = strtok(NULL, delim)))
				break;
		}
		sendline[sendlength] = '\0';
		ItoB(sendlength - messageWrapperLength, byteArry);
		memcpy(&sendline[1], &byteArry, 4);
		printf("send: ");
		printPcontent(sendline, sendlength);
		Writen(sockfd, sendline, sendlength);
	}
	printf("client exit...\n");	
}

void recvFserver(int sockfd)
{
	char recvline[MAXLINE];
	char phead;
	char ptype;
	int recvlength;
	char lengthByte[4];
	int rc;

	while( (readn(sockfd, &phead, 1)) == 1)
	{
		//printf("phead:%d\n",phead);
		if(phead != 'p')
		{
			printf("wrong package!\n");
			break;
		}

		rc = readn(sockfd, lengthByte, 4);
		if (rc != 4)
		{
			printf("wrong lengthByte!\n");
			break;
		}
		recvlength = BtoI(lengthByte);
		
		rc = readn(sockfd, &ptype, 1);
		if (rc != 1)
		{
			printf("wrong ptype!\n");
			break;
		}
		if (ptype == 'h')
		{
			Writen(sockfd, heartPackage, heartlength);
		} else if (ptype == 'm')
		{
			rc = readn(sockfd, recvline, recvlength);
			if( rc != recvlength)
			{
				printf("package loss content!\n");
				break;
			}
			printf("recv: ");
			printPcontent(recvline, recvlength);
			printf("calc_result = %d\n", BtoI(recvline));
		}	else
		{
			printf("wrong package!\n");
		}
	}	
	printf("server is close!\n");
	shutdown(sockfd, 2);	
	exit(0);
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

void printPcontent(char *pcontent, int length)
{
	int i ;
	for(i = 0; i < length; i ++)
	{
		printf("%d ", pcontent[i]);
	}
	printf("\n");
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


