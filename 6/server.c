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
	int maxDs;
	int iAccept;
	int iaClient[MAXCLIENT];
	fd_set fsStatus;
	int iCNum = 0;
	int iCounter;
	int iAddSize;
	int iRet;
	unsigned char ucBuf[256];
	struct sockaddr_in stAddr;
	struct sockaddr_in stAccept;
	iAddSize = sizeof(struct sockaddr_in);
	bzero(&stAddr, iAddSize);
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
		printf("inet_pton() failed", "invalid address string");close(iDs);
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
	if(0>bind(iDs, (struct sockaddr *)&stAddr, iAddSize))
	{
		perror("bind() failed");
		close(iDs);
		return -100;
	}
	if(0>listen(iDs, 5))
	{
		perror("listen() failed");
		close(iDs);
		return -100;
	}
	maxDs = iDs+1;
	while(1)
	{
		printf("Connection Numbers : %d\n", iCNum);
		FD_ZERO(&fsStatus);
		FD_SET(iDs, &fsStatus);
		FD_SET(1, &fsStatus);
		for(iCounter=0;iCNum>iCounter;++iCounter)
		{
			FD_SET(iaClient[iCounter], &fsStatus);
		}
		if(0 > select(maxDs, &fsStatus, NULL, NULL, NULL))
		{
			perror("select() error... ");
			close(iDs);
			return -100;
		}
		printf("Select End\n");
		if(1 == FD_ISSET(0, &fsStatus))
		{
			iRet = read(0, ucBuf, sizeof(ucBuf));
			ucBuf[iRet] = 0;
			printf("%s\n", ucBuf);
			for(iCounter=0;iCNum>iCounter;++iCounter)
			{
				write(iaClient[iCounter], ucBuf, iRet);
			}
			continue;
		}
		
		{
			iAccept = accept(iDs, (struct sockaddr *)&stAccept, &iAddSize);
			if(0>iAccept)
			{
				perror("accept() error");
				close(iDs);
				for(iCounter=0;iCNum>iCounter;++iCounter)
				{
					close(iaClient[iCounter]);
				}
				return -100;
			}
			if(MAXCLIENT <= iCNum)
			{
				write(iAccept, "Server is full connection\n", sizeof("Server is full connection\n"));
				close(iAccept);
				continue ;
			}
			if(iAccept == maxDs)
			{
				maxDs = iAccept + 1;
				iaClient[iCNum] = iAccept;
				++iCNum;
			}
			printf("Client IP : [%s]\n", inet_ntoa(stAccept.sin_addr));
			continue ;
		}
		printf("Communimation socket input\n");
		for(iCounter=0;iCNum>iCounter;++iCounter)
		{
			if(1==FD_ISSET(iaClient[iCounter], &fsStatus))
			{
				iRet = read(iaClient[iCounter], ucBuf, sizeof(ucBuf));
				ucBuf[iRet] = 0;
				printf("%s\n", ucBuf);
				for(iCounter=0;iCNum>iCounter;++iCounter)
				{
					write(iaClient[iCounter], ucBuf, iRet);
				}
			}
		}
	}
	write(iAccept, "pong", sizeof("pong"));
	close(iDs);
	close(iAccept);
	return 0;
}
