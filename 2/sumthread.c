#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *join_thread(void *arg)
{
	pthread_exit(arg); /* return arg; */
}

int main(int argc, char *argv[])
{
	pthread_t tid;
	int arg, status;
	int sum = 0;
	void *result;
	if (argc < 2)
	{
		fprintf(stderr, "Usage: jointhread <number>\n");
		exit(1);
	}
	arg = atoi(argv[1]);
	/* 쓰레드 생성 */
	for(int i=0; i<arg; i++) {
		status = pthread_create(&tid, NULL,join_thread, (void *)i);
		if (status != 0)
		{
			fprintf(stderr, "Create thread: %d", status);
			exit(1);
		}
		status = pthread_join(tid, &result);
		sum += (int)result;
		if (status != 0)
		{
			fprintf(stderr, "Join thread: %d", status);
			exit(1);
		}
	}
	printf("Thread Sum : %d \n",sum);
	return sum;
}
