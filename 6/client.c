#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXCLIENT 3

int main()
{
	int iDs;
	int iaClient[MAXCLIENT];
	fd_set fsStatus;
	int iCounter;
	int iDSize;
	int iRet;unsigned char ucBuf[256];
	struct sockaddr_in stAddr;
	iDSize = sizeof(struct sockaddr_in);
	bzero(&stAddr, iDSize);
	iDs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(iDs<0)
	{
		perror("socket() failed");
		close(iDs);
		return -10;
	}
	stAddr.sin_family = AF_INET;
	iRet = inet_pton(AF_INET, "127.0.0.1", &stAddr.sin_addr.s_addr);
	if(iRet == 0)
	{
		printf("inet_pton() failed", "invalid address string");
		close(iDs);
		return -100;
	}
	else if(iRet < 0)
	{
		perror("inet_pton() failded");
		close(iDs);
		return -100;
	}
	printf("IP : %s\n", inet_ntoa(stAddr.sin_addr));
	stAddr.sin_port = htons(30000);
	if(connect(iDs, (struct sockaddr *)&stAddr, iDSize))
	{
		perror("connect() failed");
		close(iDs);
		return -10;
	}
	while(1)
	{
		FD_ZERO(&fsStatus);
		FD_SET(iDs, &fsStatus);
		FD_SET(0, &fsStatus);
		if(0 > select(iDs+1, &fsStatus, NULL, NULL, NULL))
		{
			perror("select() error... ");
			close(iDs);
			return -100;
		}
		if(0 != FD_ISSET(0, &fsStatus))
		{
			iRet = read(0, ucBuf, sizeof(ucBuf));
			ucBuf[iRet] = 0;
			printf("%s\n", ucBuf);write(iDs, ucBuf, iRet);
			continue;
		}
		if(0!=FD_ISSET(iDs, &fsStatus))
		{
			iRet = read(iDs, ucBuf, sizeof(ucBuf));
			ucBuf[iRet] = 0;
			printf("%s\n", ucBuf);
		}
	}
	close(iDs);
	return 0;
}

