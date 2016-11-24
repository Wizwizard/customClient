#include <stdio.h>
#include <stdlib.h>

void ItoB(int, char*);
int BtoI(char *);

int main()
{
	char sendline[100];

	while(fgets(sendline, 100, stdin) != NULL)
	{
		fputs(sendline, stdout);
	}
	printf("haha exit!\n");
}

void ItoB(int i, char *arry)
{
//	if (arry == null) 
	//	return 0;
	arry[0] = (char)(i & 0xFF);
	arry[1] = (char)((i & 0xFF00) >> 8);
	arry[2] = (char)((i & 0xff0000) >> 16);
	arry[3] = (char)((i >> 24) & 0xff);
}

int BtoI(char *arry)
{
	return (arry[0] | arry[1]<<8 | arry[2]<<16 | arry[3]<<24);
}
